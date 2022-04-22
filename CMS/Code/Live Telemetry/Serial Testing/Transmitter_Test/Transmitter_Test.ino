#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
Adafruit_BMP280 bmp;
int x = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting..."));

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid sensor, check wiring!"));
    while (1);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,   
                  Adafruit_BMP280::FILTER_X16,     
                  Adafruit_BMP280::STANDBY_MS_500);
}

void loop() {
    Serial.print(x);
    Serial.print(F(" Altitude = "));
    Serial.print(bmp.readAltitude(1013.25));
    Serial.println(" m");

    Serial.println();
    x++;
}
