#include <DFRobot_RGBLCD1602.h>
#include "Ultrasonic.h"

// Pines de Sensores y configuracion inicial de la pantalla LCD
Ultrasonic SENSOR_ULTRASONICO_DELANTERO(2, 3);
Ultrasonic SENSOR_ULTRASONICO_TRASERO(11, 10);
DFRobot_RGBLCD1602 lcd(0x27, 16, 2);

// Definiciones de los parametros del buzzer
#define BUZZER               9
#define FREQUENCIA            800
#define CONFIGURAR_BUZZER     pinMode(BUZZER, OUTPUT)
#define PRENDER_BUZZER        tone(BUZZER, FREQUENCIA)
#define APAGAR_BUZZER         noTone(BUZZER)

// Definiciones de los Led´s delanteros y traseros
#define LED_DELANTERO               4
#define CONFIGURAR_LED_DELANTERO    pinMode(LED_DELANTERO, OUTPUT)

#define LED_TRASERO                 12
#define CONFIGURAR_LED_TRASERO      pinMode(LED_TRASERO, OUTPUT)

// Definiciones de la lectura de distancia de los sensores
#define DISTANCIA_DELANTERA           SENSOR_ULTRASONICO_DELANTERO.read()
#define DISTANCIA_TRASERA             SENSOR_ULTRASONICO_TRASERO.read()

// Definicion sobre la distancia limite de los sensores
#define DISTANCIA_LIMITE    20

// Definciones del LedTest
#define MS_INTERVALO_LED_TEST   250 //Function LedTest()
#define LED_TEST                LED_BUILTIN
#define CONFIG_LED_TEST         pinMode(LED_TEST, OUTPUT)
#define ACTUALIZAR_LED_TEST(x)  digitalWrite(LED_TEST, x)

// Variables globales utilizadas en el codigo
bool pantallaLimpia = false;
bool mostrarAlarma = false;
bool mostrarDistancia = true;
bool alarmaActiva = true;

unsigned long tiempoActualizacion = 0;
unsigned long intervalo = 250;

//-----------------------------------------------------

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
}

void loop() {
  LedTest();
  medirDistanciaSensoresUltrasonicos();
}

//-----------------------------------------------------

void LedTest() {
  static int ledState = 0;
  static unsigned long antMillis = 0;

  if (millis() - antMillis < MS_INTERVALO_LED_TEST) return;

  antMillis = millis();
  ledState = !ledState;
  ACTUALIZAR_LED_TEST(ledState);
}

//-----------------------------------------------------

void medirDistanciaSensoresUltrasonicos() {
  unsigned long currentTime = millis();

  if (currentTime - tiempoActualizacion >= intervalo) {
    medirDistanciaSensor1();
    medirDistanciaSensor2();
    verificarDistanciaSensores();
    tiempoActualizacion = currentTime;
  }
}

//-----------------------------------------------------

//----------------------------- MEDIMOS LA DISTANCIA DE LOS SENSORES 1 Y 2 -------------------------

/* 
 *  En estas funciones medimos la distancia de ambos sensores utilizando la función de la librería, y ajustamos la velocidad de parpadeo de los LEDs,
 *  donde en el caso de que los sensores detecten una proximidad, activamos o desactivamos el buzzer.
*/

void medirDistanciaSensor1() {
  
  int frequenciaBuzzer = map(DISTANCIA_DELANTERA, 0, DISTANCIA_LIMITE, 800, 1000);
  int valorPWM = map(DISTANCIA_DELANTERA, 0, DISTANCIA_LIMITE, 255, 0);
  int mapearIntervalo = map(DISTANCIA_DELANTERA, 0, DISTANCIA_LIMITE, 255, 100);
  parpadeoLedDelanteros(LED_DELANTERO, valorPWM, mapearIntervalo, DISTANCIA_DELANTERA);

  lcd.setCursor(12, 0);
  lcd.print("   ");

  if (mostrarDistancia) {
    lcd.setCursor(12, 0);
    lcd.print(DISTANCIA_DELANTERA);
  }
  if (DISTANCIA_DELANTERA == 0) PRENDER_BUZZER;
  else APAGAR_BUZZER;

}

void medirDistanciaSensor2() {
  
  int frequenciaBuzzer = map(DISTANCIA_TRASERA, 0, DISTANCIA_LIMITE, 800, 1000);
  int valorPWM = map(DISTANCIA_TRASERA, 0, DISTANCIA_LIMITE, 255, 0);
  int mapearIntervalo = map(DISTANCIA_TRASERA, 0, DISTANCIA_LIMITE, 255, 100);
  parpadeoLedTraseros(LED_TRASERO, valorPWM, mapearIntervalo, DISTANCIA_TRASERA);

  lcd.setCursor(12, 0);
  lcd.print("   ");

  if (mostrarDistancia) {
    lcd.setCursor(12, 0);
    lcd.print(DISTANCIA_TRASERA);
  }

  if (DISTANCIA_TRASERA == 0) PRENDER_BUZZER;
  else APAGAR_BUZZER;
  
}
//-------------------------------------------------------------------------------------------------------

//------------------------------ VERIFICAMOS LA DISTANCIA DE LOS SENSORES -------------------------------

/* 
 *  Esta funcion se va encargar de activar la alarma si la distancia de uno de los sensores es menor que la distancia límite,
 *  donde si se sobre pasa la distancia limite la alarma va vacíar la pantalla y muestra un mensaje de alerta, además de iniciar el sonido del buzzer.
*/

void verificarDistanciaSensores() {
  
  if (DISTANCIA_DELANTERA <= DISTANCIA_LIMITE || DISTANCIA_TRASERA <= DISTANCIA_LIMITE) 
  alarmaActiva = true;
  else alarmaActiva = false;

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
    PRENDER_BUZZER;
  }
  
}

//-------------------------------------------------------------------------------------------------------

//------------------- CONTROLAMOS LOS PARPADEOS DE LOS LED DE ATRAS Y ADELANTE  -------------------------

// En esta funcion controlamos el intervalo de parpadeo de los LEDs según la distancia que esta siendo medida por el sensor.

void parpadeoLedDelanteros(int ledDelantero, int brilloLed, int intervalo, int distanciaDelantera) {
  
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalo) {
    previousMillis = currentMillis;
    if (ledState == LOW) ledState = HIGH;
    else ledState = LOW;
  }

  if (distanciaDelantera == 0) digitalWrite(ledDelantero, HIGH);
  else analogWrite(ledDelantero, brilloLed * ledState);
  
}

void parpadeoLedTraseros(int ledTrasero, int brilloLed, int intervalo, int distanciaTrasera) {
  
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalo) {
    previousMillis = currentMillis;
    if (ledState == LOW) ledState = HIGH;
    else ledState = LOW;
  }

  if (distanciaTrasera == 0) digitalWrite(ledTrasero, HIGH);
  else analogWrite(ledTrasero, brilloLed * ledState);
}
