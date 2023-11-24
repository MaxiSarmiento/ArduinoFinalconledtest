#include <DFRobot_RGBLCD1602.h>
#include "Ultrasonic.h"


Ultrasonic SENSOR_ULTRASONICO_DELANTERO(2, 3);
Ultrasonic SENSOR_ULTRASONICO_TRASERO(11, 10);
DFRobot_RGBLCD1602 lcd(0x27, 16, 2);

#define BUZZER_PIN        9
#define FREQUENCIA         800
#define CONFIGURAR_BUZZER  pinMode(BUZZER_PIN, OUTPUT)
#define PRENDER_BUZZER     tone(BUZZER_PIN, FREQUENCIA)
#define APAGAR_BUZZER      noTone(BUZZER_PIN)

#define LED_DELANTERO_PIN        4
#define CONFIGURAR_LED_DELANTERO pinMode(LED_DELANTERO_PIN, OUTPUT)
#define PRENDER_LED_DELANTERO    digitalWrite(LED_DELANTERO_PIN, HIGH)

#define DISTANCIA_DELANTERA     SENSOR_ULTRASONICO_DELANTERO.read()
#define DISTANCIA_TRASERA       SENSOR_ULTRASONICO_TRASERO.read()

#define LED_TRASERO_PIN          12
#define CONFIGURAR_LED_TRASERO   pinMode(LED_TRASERO_PIN, OUTPUT)
#define PRENDER_LED_TRASERO      digitalWrite(LED_TRASERO_PIN, HIGH)


#define DISTANCIA_LIMITE 20

#define MS_INTERVALO_LED_TEST   250
#define LED_TEST                LED_BUILTIN
#define CONFIG_LED_TEST         pinMode(LED_TEST, OUTPUT)
#define ACTUALIZAR_LED_TEST(x)  digitalWrite(LED_TEST, x)

bool pantallaLimpia = false;
bool mostrarAlarma = false;
bool mostrarDistancia = true;
bool alarmaActiva = true;

unsigned long tiempoActualizacion = 0;



void setup() {
  CONFIG_LED_TEST;
  ACTUALIZAR_LED_TEST(0);
  CONFIGURAR_LED_DELANTERO;
  CONFIGURAR_LED_TRASERO;
  CONFIGURAR_BUZZER;
  APAGAR_BUZZER;
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("Distancia1:  ");
  lcd.setCursor(0, 1);
  lcd.print("Distancia2:  ");
  Serial.begin(9600);
}

void loop() {
  LedTest();
  cambiarDistanciaLimite();
  medirDistanciaSensoresUltrasonicos();
  controlarBuzzers();
  interfaceLCD();
  actualizarPantalla();
  valoresLeds();

}

void LedTest() {
  static int ledState = 0;
  static unsigned long antMillis = 0;

  if (millis() - antMillis < MS_INTERVALO_LED_TEST) return;

  antMillis = millis();
  ledState = !ledState;
  ACTUALIZAR_LED_TEST(ledState);
}

void medirDistanciaSensoresUltrasonicos() {
  unsigned long currentTime = millis();
  unsigned long intervalo = 250;

  if (currentTime - tiempoActualizacion >= intervalo) return;

  medirDistanciaSensores(DISTANCIA_DELANTERA, LED_DELANTERO_PIN, BUZZER_PIN);
  medirDistanciaSensores(DISTANCIA_TRASERA, LED_TRASERO_PIN, BUZZER_PIN);
  verificarDistanciaSensores();
  tiempoActualizacion = currentTime;
}

void valoresLeds() {
  int ledDelantero = LED_DELANTERO_PIN;
  int brilloLedDelantero = 255;
  int intervaloDelantero = 500;
  int distanciaDelantera = 1;

  int ledTrasero = LED_TRASERO_PIN;
  int brilloLedTrasero = 255;
  int intervaloTrasero = 500;
  int distanciaTrasera = 1;

  parpadeoLedDelanteros(ledDelantero, brilloLedDelantero, intervaloDelantero, distanciaDelantera);
  parpadeoLedTraseros(ledTrasero, brilloLedTrasero, intervaloTrasero, distanciaTrasera);
}

void actualizarPantalla() {
  actualizarLCD(DISTANCIA_DELANTERA);
  actualizarLCD(DISTANCIA_TRASERA);
}

void interfaceLCD() {
  if (!pantallaLimpia) {
    lcd.clear();
    pantallaLimpia = true;
  }
  lcd.setCursor(0, 0);
  lcd.print("Distancia1:    ");
  lcd.setCursor(0, 1);
  lcd.print("Distancia2:    ");

  if (!alarmaActiva) {
    lcd.setCursor(12, 0);
    lcd.print(DISTANCIA_DELANTERA);
    lcd.setCursor(12, 1);
    lcd.print(DISTANCIA_TRASERA);
    APAGAR_BUZZER;

    if (mostrarAlarma) {
      lcd.setCursor(1, 0);
      lcd.print("                   ");
      lcd.setCursor(2, 1);
      lcd.print("                   ");
      mostrarAlarma = false;
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("  DETENGA SU      ");
    lcd.setCursor(0, 1);
    lcd.print("  VEHICULO       ");

    mostrarAlarma = true;
    mostrarDistancia = false;
  }
}

void actualizarLCD(int valorDistancia) {
  lcd.setCursor(12, 0);
  lcd.print("   ");

  if (mostrarDistancia) {
    lcd.setCursor(12, 0);
    lcd.print(valorDistancia);
  }
}

void controlarBuzzers() {
  unsigned long currentTime = millis();
  unsigned long intervalo = 250;

  if (currentTime - tiempoActualizacion >= intervalo) return;
  controlarBuzzerDelantero(DISTANCIA_DELANTERA, BUZZER_PIN);
  controlarBuzzerTrasero(DISTANCIA_TRASERA, BUZZER_PIN);
  tiempoActualizacion = currentTime;
}

void controlarBuzzerDelantero(int distancia, int buzzerPin) {
  if (!distancia) PRENDER_BUZZER;
  else APAGAR_BUZZER;
}

void controlarBuzzerTrasero(int distancia, int buzzerPin) {
  if (!distancia) PRENDER_BUZZER;
  else APAGAR_BUZZER;
}

void medirDistanciaSensores(int distancia, int ledPin, int buzzerPin) {
  int frecuenciaBuzzer = map(distancia, 0, DISTANCIA_LIMITE, 800, 1000);
  int valorPWM = map(distancia, 0, DISTANCIA_LIMITE, 255, 0);
  int mapearIntervalo = map(distancia, 0, DISTANCIA_LIMITE, 255, 100);

  parpadeoLed(ledPin, valorPWM, mapearIntervalo, distancia);
  actualizarLCD(distancia);
}

void verificarDistanciaSensores() {
  alarmaActiva = (DISTANCIA_DELANTERA <= DISTANCIA_LIMITE || DISTANCIA_TRASERA <= DISTANCIA_LIMITE) ? true : false;
}

void parpadeoLed(int led, int brillo, int intervalo, int distancia) {
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < intervalo) return;
  previousMillis = currentMillis;
  ledState = !ledState;

  distancia == 0 ? (PRENDER_LED_DELANTERO, PRENDER_LED_TRASERO) : analogWrite(led, brillo * ledState);
}

void parpadeoLedDelanteros(int ledDelantero, int brilloLed, int intervalo, int distanciaDelantera) {
  parpadeoLed(ledDelantero, brilloLed, intervalo, distanciaDelantera);
}

void parpadeoLedTraseros(int ledTrasero, int brilloLed, int intervalo, int distanciaTrasera) {
  parpadeoLed(ledTrasero, brilloLed, intervalo, distanciaTrasera);
}

void cambiarDistanciaLimite() {
   char cmd;
   int distanciaLimite = DISTANCIA_LIMITE;

    if (!Serial.available() )return; 
    switch (cmd = Serial.read()) {
    case 'a':
      distanciaLimite = 10;
      Serial.println("Distancia Limite = 10");
      break;
    case 's':
      distanciaLimite = 20;
      Serial.println("Distancia Limite = 20");
      break;
    case 'd':
      distanciaLimite = 30;
      Serial.println("Distancia Limite = 30");
      break;
    case 'f':
      distanciaLimite = 40;
      Serial.println("Distancia Limite = 40");
      break;
    case 'g':
      distanciaLimite = 50;
      Serial.println("Distancia Limite = 50");
      break;
  }
}

