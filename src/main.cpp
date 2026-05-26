/*******************************************************************************
* RoboCore - Primeiros Passos com Sensor de Nivel
* Leitura do Sensor do Tipo Boia
*******************************************************************************/

//Declaracao do pino conectado ao sensor
#include <Arduino.h>

const int PINO_SENSOR = 2;

// Guarda o ultimo estado lido do sensor para detectar mudancas
int previousState = -1;

void setup() {

  //Inicializacao do monitor serial
  Serial.begin(9600);

  //Configuracao do pino como uma entrada
  pinMode(PINO_SENSOR, INPUT);
  
  // Inicializa previousState com o estado atual do pino
  previousState = digitalRead(PINO_SENSOR);
}

void loop() {
  //Realiza a leitura do sensor
  int leitura_sensor = digitalRead(PINO_SENSOR);

  //Somente reporta quando houver mudanca de estado
  if (leitura_sensor != previousState) {
    Serial.print("Leitura do sensor: ");
    if (leitura_sensor == LOW) {
      Serial.print("LOW");
      Serial.print("   |  ");
      Serial.println("Switch ABERTO");
    } else {
      Serial.print("HIGH");
      Serial.print("  |  ");
      Serial.println("Switch FECHADO");
    }
    previousState = leitura_sensor;
  }

  delay(100); //Aguarda 100 milissegundo para nova leitura

}