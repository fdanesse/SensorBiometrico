/* 
Conexiones del I2C al Arduino:
I2C ---->  Arduino
SCL        A5      
SDA        A4
VCC        5V
GND        GND  
Capitulo 35 de Arduino desde cero en Español - LCD 1602A - LCD I2C - Adaptador basado en circuito integrado PCF8574 y libreria LiquidCrystal_I2C descargada desde:  https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/
*/

/*
https://github.com/oxullo/Arduino-MAX30100/blob/master/examples/MAX30100_Debug/MAX30100_Debug.ino
beat detection reporting
heart rate calculation
SpO2 (oxidation level) calculation
*/
 
#include <Wire.h>

#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7


#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS 1000

int MH = 0; //maximo ritmo
int MO = 0; //maximo oximetria

PulseOximeter pox;
uint32_t tsLastReport = 0;


// https://github.com/adafruit/Adafruit_Sensor
// https://github.com/adafruit/DHT-sensor-library
#include <DHT.h>

#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


#define buz 7


void setup(){
    pinMode(buz, OUTPUT);
    
    Wire.begin();
  
    lcd.setBacklightPin(3, POSITIVE); lcd.setBacklight(LOW); lcd.begin(16, 2);

    if (!pox.begin(PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT)) {
        lcd.clear(); lcd.setCursor(0, 0); lcd.print("ERROR...");
        for(;;);
    }

    pox.setOnBeatDetectedCallback(onBeatDetected);

    //lcd.clear(); lcd.setCursor(0, 0); lcd.print("INICIO CORRECTO");
    //delay(5000);

    dht.begin();
}

void loop(){
    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        int h = pox.getHeartRate();
        int o = pox.getSpO2();

        if (h > MH) {MH = h;}
        if (o > MO) {MO = o;}
        
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("H: ");
        lcd.setCursor(3, 0); lcd.print(h);
        lcd.setCursor(7, 0); lcd.print(MH);
        
        lcd.setCursor(0, 1); lcd.print("O: ");
        lcd.setCursor(3, 1); lcd.print(o);
        lcd.setCursor(7, 1); lcd.print(MO);
        
        tsLastReport = millis();
    }

    lcd.setCursor(11, 0); lcd.print(dht.readHumidity());
    lcd.setCursor(11, 1); lcd.print(dht.readTemperature());
    //dht.computeHeatIndex(t, h, false);

}


void onBeatDetected(){
    tone(buz, 350, 50);
}
