#include <Arduino.h>

//DS18B20 probe libraries
#include <OneWire.h>
#include <DallasTemperature.h>

//TFT screen libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#include <WiFi.h>

//Webserver libraries
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

float targetMashTemp;
float currentMashTemp;
float offsetMashTemp = 1;

// Pin donde se conecta el bus 1-Wire del DS18B20
const int pinDataDQ = 15;
 
// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObject(pinDataDQ);
DallasTemperature sensorDS18B20(&oneWireObject);

#define TFT_CS 14  //for D32 Pro
#define TFT_DC 27  //for D32 Pro
#define TFT_RST 33 //for D32 Pro
#define TS_CS  12 //for D32 Pro

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//XPT2046_Touchscreen ts(TS_CS);

// 'Outlined_brew-pot', 100x100px
const unsigned char brewPot100 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 
	0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xe0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x7e, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3f, 0xe6, 0x0e, 0x00, 
	0x00, 0x00, 0x1f, 0x86, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x06, 0x1f, 0x80, 0x00, 0x00, 0x3d, 
	0xc6, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x06, 0x3b, 0xc0, 0x00, 0x00, 0x30, 0xe6, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x70, 0xc0, 0x00, 0x00, 0x38, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x61, 0xc0, 0x00, 0x00, 0x3c, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc3, 
	0xc0, 0x00, 0x00, 0x0e, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 
	0x07, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0e, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x03, 
	0x0c, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x11, 0x00, 0x80, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x10, 0x80, 0x80, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x10, 0xe0, 0x80, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x10, 0x60, 0x80, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x10, 0x60, 0x80, 0x00, 0x06, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x80, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x00, 0x08, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x04, 0x02, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x03, 0x8c, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xff, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00
};

// 'icon_fireprevention', 100x100px
const unsigned char noFire100 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x7f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 
	0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x7f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 
	0x00, 0x03, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0xff, 
	0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x80, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1f, 0xfe, 0x00, 0x01, 0x80, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x80, 
	0x01, 0xc0, 0x00, 0x01, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x80, 0x01, 0xc0, 0x00, 
	0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x7f, 0xf0, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xff, 0x80, 0x03, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 
	0xc0, 0x01, 0xe0, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x01, 0xf0, 
	0x00, 0x00, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x7f, 0xf0, 0x01, 0xf0, 0x00, 0x00, 0x03, 
	0xfe, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x3f, 0xf9, 0xc1, 0xf8, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 
	0x00, 0x07, 0xfd, 0x9f, 0xfc, 0xf1, 0xfc, 0x80, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xf8, 
	0x0f, 0xfe, 0x79, 0xfc, 0xe0, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x07, 0xff, 0x3d, 
	0xfe, 0x70, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0x1f, 0xf0, 0x03, 0xff, 0x9f, 0xff, 0x70, 0x00, 
	0x00, 0x7f, 0x80, 0x00, 0x00, 0x1f, 0xe0, 0x01, 0xff, 0xcf, 0xff, 0xf8, 0x00, 0x00, 0x7f, 0x80, 
	0x00, 0x00, 0x1f, 0xe0, 0x00, 0xff, 0xe7, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x3f, 
	0xe0, 0x00, 0x7f, 0xf3, 0xff, 0xfc, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x3f, 
	0xf9, 0xff, 0xfc, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x1f, 0xfc, 0xff, 0xfc, 
	0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x0f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x1f, 
	0xe0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x07, 0xff, 0x3f, 0xfe, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 
	0x7f, 0x80, 0x00, 0x03, 0xff, 0x9f, 0xfe, 0x40, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x7f, 0x80, 0x00, 
	0x09, 0xff, 0xcf, 0xfe, 0x60, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x04, 0xff, 0xe7, 
	0xfe, 0x70, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x06, 0x7f, 0xf3, 0xfe, 0x70, 0x00, 
	0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x07, 0x3f, 0xf9, 0xff, 0x70, 0x00, 0x0f, 0xf0, 0x00, 
	0x00, 0x7f, 0x80, 0x00, 0x07, 0x9f, 0xfc, 0xff, 0xf8, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 
	0x00, 0x07, 0xcf, 0xfe, 0x7f, 0xf8, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x07, 0xe7, 
	0xff, 0x3f, 0xf8, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x07, 0xc7, 0xf3, 0xff, 0x9f, 0xf8, 
	0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x01, 0xf7, 0xf9, 0xff, 0xcf, 0xf0, 0x00, 0x0f, 0xf0, 
	0x00, 0x00, 0x7f, 0x80, 0x00, 0xff, 0xfc, 0xff, 0xe7, 0xf0, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 
	0x80, 0x00, 0x7f, 0xfe, 0x7f, 0xf3, 0xf0, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x7f, 
	0xff, 0x3f, 0xf9, 0xf4, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x3f, 0xff, 0x9f, 0xfc, 
	0xfe, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x3f, 0xff, 0xcf, 0xfe, 0x7e, 0x00, 0x1f, 
	0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x3f, 0xff, 0xe7, 0xff, 0x3e, 0x00, 0x1f, 0xe0, 0x00, 0x00, 
	0x3f, 0xc0, 0x00, 0x3f, 0xff, 0xf3, 0xff, 0x9e, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 
	0x7f, 0xff, 0xf9, 0xff, 0xce, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x7f, 0xff, 0xfc, 
	0xff, 0xe6, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x7f, 0xff, 0x70, 0x7f, 0xf2, 0x00, 
	0x3f, 0xc0, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x7f, 0xfe, 0x31, 0x3f, 0xf8, 0x00, 0x7f, 0xc0, 0x00, 
	0x00, 0x1f, 0xf0, 0x00, 0x7f, 0xde, 0x01, 0x1f, 0xfc, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x0f, 0xf0, 
	0x00, 0x7f, 0x8e, 0x00, 0x0f, 0xfe, 0x00, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x7f, 0x8e, 
	0x00, 0x27, 0xff, 0x00, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x7f, 0x86, 0x00, 0x03, 0xff, 
	0x81, 0xff, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x7f, 0x87, 0x00, 0x01, 0xff, 0xc1, 0xff, 0x00, 
	0x00, 0x00, 0x07, 0xfe, 0x00, 0x3f, 0x83, 0x00, 0x0c, 0xff, 0xe3, 0xfe, 0x00, 0x00, 0x00, 0x03, 
	0xfe, 0x00, 0x3f, 0xc0, 0x00, 0x1e, 0x7f, 0xf7, 0xfe, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x3f, 
	0xc0, 0x00, 0x1f, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x01, 0xff, 0x80, 0x1f, 0xe0, 0x00, 0x3f, 
	0x9f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x1f, 0xe0, 0x00, 0x7f, 0xcf, 0xff, 0xf8, 
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x0f, 0xf0, 0x01, 0xff, 0xe7, 0xff, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x7f, 0xf0, 0x07, 0xf8, 0x03, 0xff, 0x83, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 
	0x03, 0xfe, 0x1f, 0xfe, 0x09, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x01, 0xff, 0xff, 
	0xfc, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x7f, 0xff, 0xe0, 0x07, 0xff, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x07, 0xff, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0xff, 0xf8, 0x00, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 
	0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0x00, 0xff, 0xff, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
	0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00
};

// 'pngtree-vector-bone-fire-icon-png-image_3760510', 100x100px
const unsigned char fire100 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0xff, 0xc0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
	0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xc1, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc3, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xcf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 
	0xff, 0xdf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xdf, 0xc0, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 
	0xff, 0xfb, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0xff, 0xf3, 0xff, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0xff, 0xc3, 0xff, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xcf, 0xff, 0x83, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xcf, 0xff, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0xcf, 0xff, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe7, 0xfe, 0x03, 
	0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe7, 0xfe, 0x01, 0xff, 0xf0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf3, 0xfc, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0xfb, 0xfc, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0xff, 0x3c, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x1c, 
	0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x0c, 0x00, 0x3f, 0xf8, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x04, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 
	0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x0d, 
	0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x0d, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x04, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1e, 0x00, 0x00, 0x08, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 
	0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0xff, 0x80, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 
	0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x9f, 0xff, 0x9f, 0xff, 0xff, 0xe0, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x1f, 0xff, 0x8f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0xc3, 0xff, 0xfc, 0x1f, 0xff, 0x83, 0xff, 0xfc, 0x30, 0x00, 0x00, 0x00, 0x00, 0xc1, 0xff, 
	0xf8, 0x1f, 0xff, 0x81, 0xff, 0xf8, 0x30, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xf0, 0x1f, 0xff, 
	0xc0, 0x7f, 0xf0, 0x10, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x7f, 0xe0, 0x1f, 0xff, 0xc0, 0x3f, 0xe0, 
	0x30, 0x00, 0x00, 0x00, 0x00, 0x40, 0x7f, 0xc0, 0x1f, 0xff, 0xc0, 0x1f, 0xe0, 0x20, 0x00, 0x00, 
	0x00, 0x00, 0x40, 0x3f, 0x00, 0x1e, 0x01, 0xe0, 0x0f, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x20, 
	0x3e, 0x00, 0x1c, 0x00, 0xe0, 0x07, 0xc0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x30, 0x3c, 0x00, 0x1c, 
	0x00, 0x60, 0x03, 0xc0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x00, 0x18, 0x00, 0x60, 0x01, 
	0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x18, 0x70, 0x00, 0x0c, 0x00, 0x40, 0x00, 0xe1, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x06, 0x01, 0xc0, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0xc0, 0x00, 0x03, 0xff, 0x80, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00
};


#define relayPin 13
String relayStatus = "OFF";

// For WiFi connection
const char* ssid = "xxx";
const char* password = "yyy";

AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "targetTemp";
const char* PARAM_INPUT_2 = "offsetTemp";

// HTML web page to handle 2 input fields (targetTemp, offsetTemp)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    targetTemp: <input type="text" name="targetTemp">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    offsetTemp: <input type="text" name="offsetTemp">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void wifiConnect(){
	// Start: Connect to Wi-Fi network
	Serial.println("WiFi not connected.");
	Serial.println();
	Serial.print("Connecting to...");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	//Removing the while loop to avoid unnecesary waiting. If necessary WiFi will be reconnected in loop()
	//while (WiFi.status() != WL_CONNECTED) {
	//delay(500);
	//Serial.print(".");
	//}

	Serial.println("");
	Serial.println("Wi-Fi connected successfully");
	Serial.print("IP address: ");
	Serial.print(WiFi.localIP());
	// End: Connect to Wi-Fi network
}

void setup() {
	// put your setup code here, to run once:
	// Serial port communication
	Serial.begin(9600);

	// Iniciamos el bus 1-Wire
	sensorDS18B20.begin(); 

	//TFT initialize
	tft.begin();
	tft.setRotation(0);

	pinMode(relayPin, OUTPUT);

	wifiConnect();

	// Start: Connect to Wi-Fi network
	//Serial.println();
	//Serial.println();
	//Serial.print("Connecting to...");
	//Serial.println(ssid);
	//
	//WiFi.begin(ssid, password);
	//
	//while (WiFi.status() != WL_CONNECTED) {
	//delay(500);
	//Serial.print(".");
	//}
	//Serial.println("");
	//Serial.println("Wi-Fi connected successfully");
	//Serial.print("IP address: ");
	//Serial.print(WiFi.localIP());
	// End: Connect to Wi-Fi network

	// Send web page with input fields to client
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
	request->send_P(200, "text/html", index_html);
	});

	// Send a GET request to <ESP_IP>/get?targetTemp=<inputMessage>
	server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
	String inputMessage;
	String inputParam;
	// GET targetTemp value on <ESP_IP>/get?targetTemp=<inputMessage>
	if (request->hasParam(PARAM_INPUT_1)) {
		inputMessage = request->getParam(PARAM_INPUT_1)->value();
		targetMashTemp = inputMessage.toFloat();
		inputParam = PARAM_INPUT_1;
	}
	// GET offsetTemp value on <ESP_IP>/get?offsetTemp=<inputMessage>
	else if (request->hasParam(PARAM_INPUT_2)) {
		inputMessage = request->getParam(PARAM_INPUT_2)->value();
		offsetMashTemp = inputMessage.toFloat();
		inputParam = PARAM_INPUT_2;
	}    
	else {
		inputMessage = "No message sent";
		inputParam = "none";
	}
	Serial.println(inputMessage);
	Serial.println(inputParam);
	request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
										+ inputParam + ") with value: " + inputMessage +
										"<br><a href=\"/\">Return to Home Page</a>");
	});
	server.onNotFound(notFound);
	server.begin();


}

void loop() {
	if (WiFi.status() != WL_CONNECTED) {
		wifiConnect()
	}
	
	tft.fillScreen(ILI9341_BLACK);
	yield();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(2);

	// put your main code here, to run repeatedly:
	// Mandamos comandos para toma de temperatura a los sensores
	Serial.println("Sending request to sensor");
	sensorDS18B20.requestTemperatures();
	//tft.println("Sending request to sensor");


	// Leemos y mostramos los datos del sensor DS18B20
	currentMashTemp = sensorDS18B20.getTempCByIndex(0);
	Serial.print("DS18B20 sensor temperature: ");
	Serial.print(currentMashTemp);
	Serial.println(" C");

	tft.println("DS18B20 sensor temp: ");
	tft.setTextSize(3);
	tft.print(currentMashTemp);
	tft.print((char)247); //Display degree symbol
	tft.println("C");
	tft.setTextSize(2);
	tft.println("");
	tft.setTextColor(ILI9341_YELLOW);
	tft.println("Target mash temp: ");
	tft.println("");
	tft.setTextSize(3);
	tft.print(targetMashTemp);
	tft.print((char)247); //Display degree symbol
	tft.println("C");
	tft.println("");

	tft.setTextSize(2);
	tft.setTextColor(ILI9341_GREEN);
	tft.print("Temp offset: ");
	tft.print(offsetMashTemp);
	tft.print((char)247); //Display degree symbol
	tft.println("C");
	tft.setTextSize(1);
	tft.println("");

	tft.setTextSize(2);
	if ((currentMashTemp + offsetMashTemp) < targetMashTemp){
	tft.setTextColor(ILI9341_GREEN);
	digitalWrite(relayPin, HIGH);
	relayStatus = "ON";
	}
	if (currentMashTemp >= targetMashTemp){
	tft.setTextColor(ILI9341_RED);
	digitalWrite(relayPin, LOW);
	relayStatus = "OFF";
	}
	tft.print("Relay status: ");
	tft.println(relayStatus);
	tft.setTextSize(1);
	tft.println("");

	tft.setTextSize(2);
	tft.setTextColor(ILI9341_GREEN);
	tft.print("IP/URL:");
	tft.print(WiFi.localIP());

	if (relayStatus == "OFF"){
	tft.drawBitmap(15, 220, noFire100, 100, 100, ILI9341_WHITE); //  x1,y1,filename,width,height,color
	} else {
	tft.drawBitmap(15, 220, fire100, 100, 100, ILI9341_WHITE); //  x1,y1,filename,width,height,color
	}
	tft.drawBitmap(120, 220, brewPot100, 100, 100, ILI9341_WHITE); //  x1,y1,filename,width,height,color

	delay(5000); 
}