#include <HardwareSerial.h>
#include <data.h>

// Configuración del puerto serie para el SIM800L
HardwareSerial SIM800l(2); // Usamos el puerto serial 2
const int pinButton2 = 27;
int stateButton2 = 0;

void setup() {
  // Inicialización del puerto serial para la comunicación con el monitor serial
  Serial.begin(115200);
  pinMode (pinButton2,INPUT_PULLDOWN);
  // Inicialización del puerto serial para la comunicación con el SIM800L
  SIM800l.begin(9600, SERIAL_8N1, 16, 17); // Baudrate, parity, RX, TX

  Serial.println("ESP32 con SIM800L listo. Ingresa comandos AT:");
  delay(7000);
}

void loop() {
 stateButton2 = digitalRead(pinButton2);
  if(stateButton2 == HIGH){
    Serial.println("Button2 is pressed!"); 
    SIM800l.println("AT");
    delay(1000);

    if(SIM800l.available()){
      char c = SIM800l.read();
      Serial.print(c);
      Serial.println();
    }
    SIM800l.println("AT+CLVL=100"); // Adjust the volume level as needed
    delay(1000);
    SIM800l.println("ATD" + TEST_NUMBER + ";");
    Serial.println("ATD" + TEST_NUMBER + ";");
    Serial.println("Llamando...");
    if(SIM800l.available()){
      char c = SIM800l.read();
      Serial.print(c);
      Serial.println();
    }
    delay(5000);
    SIM800l.println("AT+CMIC=0, 6");
    delay(1000);
  }else{
    Serial.println("Nothing happen");
  }
}