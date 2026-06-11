#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal.h>

const int PINO_SENSOR = 2;
const int PINO_DHT = A1;
#define TIPO_DHT DHT11

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

DHT dht(PINO_DHT, TIPO_DHT);

volatile bool mudouEstado = false;
unsigned long ultimoTempo = 0;
const unsigned long debounce = 200;

unsigned long ultimaLeituraDHT = 0;
const unsigned long intervaloDHT = 2000;

unsigned long ultimoEnvio = 0;
const unsigned long intervaloEnvio = 2000;

float ultimaTemp = 0;
float ultimaUmid = 0;
bool liquidoPresente = false;

void detectarMudanca() {
    unsigned long tempoAtual = millis();
    if (tempoAtual - ultimoTempo > debounce) {
        mudouEstado = true;
        ultimoTempo = tempoAtual;
    }
}

void atualizaLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(ultimaTemp, 1);
    lcd.print((char)223); // grau
    lcd.print("C H:");
    lcd.print((int)ultimaUmid);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Nivel: ");
    lcd.print(liquidoPresente ? "PRESENTE" : "AUSENTE");
}

void enviarJSON() {
    Serial.print(F("{\"temp\":"));
    Serial.print(ultimaTemp, 1);
    Serial.print(F(",\"umid\":"));
    Serial.print(ultimaUmid, 1);
    Serial.print(F(",\"nivel\":"));
    Serial.print(liquidoPresente ? 1 : 0);
    Serial.println(F("}"));
}

void setup() {
    Serial.begin(9600);

    pinMode(PINO_SENSOR, INPUT_PULLUP);
    attachInterrupt(
        digitalPinToInterrupt(PINO_SENSOR),
        detectarMudanca,
        CHANGE
    );

    dht.begin();

    lcd.begin(16, 2);
    lcd.print("Iniciando...");

    liquidoPresente = (digitalRead(PINO_SENSOR) == LOW);
    delay(2000);

    ultimaTemp = dht.readTemperature();
    ultimaUmid = dht.readHumidity();
    if (isnan(ultimaTemp)) ultimaTemp = 0;
    if (isnan(ultimaUmid)) ultimaUmid = 0;

    atualizaLCD();
    enviarJSON();
    ultimoEnvio = millis();
}

void loop() {
    if (mudouEstado) {
        mudouEstado = false;
        int estado = digitalRead(PINO_SENSOR);
        liquidoPresente = (estado == LOW);
        atualizaLCD();
        enviarJSON();
        ultimoEnvio = millis();
    }

    unsigned long agora = millis();
    if (agora - ultimaLeituraDHT >= intervaloDHT) {
        ultimaLeituraDHT = agora;

        float umidade = dht.readHumidity();
        float temp = dht.readTemperature();

        if (!isnan(umidade) && !isnan(temp)) {
            ultimaTemp = temp;
            ultimaUmid = umidade;
            atualizaLCD();
        }
    }

    if (agora - ultimoEnvio >= intervaloEnvio) {
        ultimoEnvio = agora;
        enviarJSON();
    }
}
