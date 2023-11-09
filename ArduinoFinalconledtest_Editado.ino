#include <DFRobot_RGBLCD1602.h>
#include <Ultrasonic.h>

// Pines de Sensores y configuracion inicial LCD
Ultrasonic SensorUltrasonico1(2, 3);   
Ultrasonic SensorUltrasonico2(11,10); 
DFRobot_RGBLCD1602 lcd(0x27, 16, 2);
// Definiciones de los parametros y Constantes
#define BUZZER               9
#define FREQUENCIA          800
#define CONFIGURAR_BUZZER    pinMode(BUZZER, OUTPUT)
#define PRENDER_BUZZER       tone(BUZZER, FREQUENCIA)
#define APAGAR_BUZZER        noTone(BUZZER)
//Definicion de leds
#define LED1                 4
#define CONFIGURAR_LED1      pinMode(LED1, OUTPUT)

#define LED2                 12
#define CONFIGURAR_LED2      pinMode(LED2, OUTPUT)
//Definicion lecturas de distancia
#define DISTANCIA1           SensorUltrasonico1.read()
#define DISTANCIA2           SensorUltrasonico2.read()
//Constante Distancia Limite
#define DISTANCIA_LIMITE    20

//-- definicion LedTest
#define MS_INTERVALO_LED_TEST   250 //Function LedTest()
#define LED_TEST                LED_BUILTIN
#define CONFIG_LED_TEST         pinMode(LED_TEST, OUTPUT)
#define ACTUALIZAR_LED_TEST(x)  digitalWrite(LED_TEST, x)



//Definiciones auxiliares
bool pantallaLimpia = false;
bool mostrarAlarma = false;
bool mostrarDistancia = true;
bool alarmaActiva = true;

unsigned long tiempoActualizacion = 0;
unsigned long intervalo = 250;


void setup() {
    CONFIG_LED_TEST;
  ACTUALIZAR_LED_TEST(0);
  lcd.init();
  
  CONFIGURAR_LED1;
  CONFIGURAR_LED2;
  CONFIGURAR_BUZZER;
  APAGAR_BUZZER;
  lcd.setCursor(0, 0);
  lcd.print("Distancia1:  ");
  lcd.setCursor(0, 1);
  lcd.print("Distancia2:  ");
  Serial.begin(9600);
}




void loop(){
  LedTest();
  unsigned long currentTime = millis();

  if (currentTime - tiempoActualizacion >= intervalo) {
    medirDistanciaSensor1();
    medirDistanciaSensor2();
    verificarDistanciaSensores();
    tiempoActualizacion = currentTime;
  } 
}
//---Ledtest
void LedTest() {
  static int ledState = 0;
  static unsigned long antMillis = 0;

  if (millis() - antMillis < MS_INTERVALO_LED_TEST) {
    return;
  }

  antMillis = millis();
  ledState = !ledState;
  ACTUALIZAR_LED_TEST(ledState);
}
//----------------------------- MEDIMOS LA DISTANCIA DE LOS SENSORES 1 Y 2 -------------------------
//Medimos la distancia entre el sensor utilizando la funcion de la libreria, diferenciando ambos sensores y establecemos 
//la velocidad de parpadeo de los leds y en el caso de cercania el activar o desactivar el buzzer
void medirDistanciaSensor1(){
  
  int frequenciaBuzzer = map(DISTANCIA1, 0, DISTANCIA_LIMITE, 800, 1000);
  int valorPWM = map(DISTANCIA1, 0, DISTANCIA_LIMITE, 255, 0); 
  int mapearIntervalo = map(DISTANCIA1, 0, DISTANCIA_LIMITE, 255, 100);
  parpadeoLEDs(LED1, valorPWM, mapearIntervalo, DISTANCIA1);
  
  lcd.setCursor(12, 0);
  lcd.print("   ");
  
  if(mostrarDistancia){
    lcd.setCursor(12, 0);
    lcd.print(DISTANCIA1);
  }
   if (DISTANCIA1 == 0) {
    PRENDER_BUZZER; 
  } else {
    APAGAR_BUZZER;
  }
}


void medirDistanciaSensor2(){
  
  int frequenciaBuzzer = map(DISTANCIA2, 0, DISTANCIA_LIMITE, 800, 1000);
  int valorPWM = map(DISTANCIA2, 0, DISTANCIA_LIMITE, 255, 0); 
  int mapearIntervalo = map(DISTANCIA2, 0, DISTANCIA_LIMITE, 255, 100);
  parpadeoLEDs2(LED2, valorPWM, mapearIntervalo, DISTANCIA2);
  
  lcd.setCursor(12, 0);
  lcd.print("   ");
  
  if(mostrarDistancia){
    lcd.setCursor(12, 0);
    lcd.print(DISTANCIA2);
  }

   if (DISTANCIA2 == 0) {
    PRENDER_BUZZER; 
  } else {
    APAGAR_BUZZER;
  }
}
//-------------------------------------------------------------------------------------------------------



//------------------------------ VERIFICAMOS LA DISTANCIA DE LOS SENSORES -------------------------------
//Establece que si la distancia de uno u otro sensor es menor o igual a la distancia limite establecida , que se active la alarma
//que es que vacie la pantalla y muestre un mensaje de alerta, aparte de al activarla empieza a sonar el buzzer
void verificarDistanciaSensores(){
  
  if (DISTANCIA1 <= DISTANCIA_LIMITE || DISTANCIA2 <= DISTANCIA_LIMITE) {
    alarmaActiva = true;
  } else {
    alarmaActiva = false;
  }

  if (!pantallaLimpia) {
    lcd.clear();
    pantallaLimpia = true; 
  }

  // Mostrar texto en el LCD
  lcd.setCursor(0, 0);
  lcd.print("Distancia1:    ");
  lcd.setCursor(0, 1);
  lcd.print("Distancia2:    ");

  if (!alarmaActiva) {
    
    lcd.setCursor(12, 0);
    lcd.print(DISTANCIA1);
    lcd.setCursor(12, 1);
    lcd.print(DISTANCIA2);
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
// Funcion que controla el intervalo y el brillo de los leds, en base a la distancia que entrega el sensor
void parpadeoLEDs(int led1, int brillo, int intervalo, int distancia) {
  
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalo) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }

  if (distancia == 0) {
    digitalWrite(led1, HIGH);
  } else {
    analogWrite(led1, brillo * ledState);
  }
}

void parpadeoLEDs2(int led2, int brillo, int intervalo, int distancia) {
  
  static unsigned long previousMillis = 0;
  static bool ledState = LOW;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalo) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }

  if (distancia == 0) {
    digitalWrite(led2, HIGH);
  } else {
    analogWrite(led2, brillo * ledState);
  }
}

