/*
 * Title:       Smart Room 8 sided die controller
 * Description: Using an accelerometer to control lights based on orientation.
 * Author:      Liv Bouligny
 * Date:        14 July 2022
 */

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneButton.h>
#include "colors.h"
#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <Ethernet.h>
#include <mac.h>
#include <hue.h>

const int SCREENWIDTH = 128;
const int SCREENHEIGHT = 64;
const int OLEDRESET = -1;
const int SCREENADDRESS = 0x3C;
const int PIXELPIN = 8;
const int PIXELCOUNT = 16;
float angle;
float rad;
byte count, i;
int px = 0;
int currentTime;
int lastTime = 1000;
bool buttonState;
bool last = LOW;
bool onState = LOW;

OneButton button1(20,false);
Adafruit_MPU6050 mpu;
Adafruit_NeoPixel pixel(PIXELCOUNT, PIXELPIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREENWIDTH, SCREENHEIGHT, &Wire, OLEDRESET);

void setup() {  
  Serial.begin(9600);
  button1.attachClick(click1);
  button1.attachDoubleClick(doubleClick1);
  button1.attachLongPressStart(longPressStart1);
  button1.attachLongPressStop(longPressStop1);
  button1.attachDuringLongPress(longPress1);
  button1.setClickTicks(250);
  button1.setPressTicks(2000);
  while(!Serial);
  Serial.printf("Serial Monitor Initialized\n");
  Wire.begin(); 
  Serial.printf("Adafruit MPU6050 test!\n");
  
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  Ethernet.begin(mac);
  delay(200);          //ensure Serial Monitor is up and running           
  printIP();
  Serial.printf("LinkStatus: %i  \n",Ethernet.linkStatus());
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.printf("MPU6050 Found!\n");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.printf("Accelerometer range set to: 8G\n");
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREENADDRESS))  {
    Serial.printf("SSD1306 allocation failed");
    for (;;);
  }
  
  display.display();
  delay(2000);
  display.clearDisplay();  

  display.setCursor(1,1);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();  

  pixel.begin();
  pixel.show();
  
}

void loop() {
  pixel.setBrightness(125);

//  sensors_event_t a, g, temp;
//  mpu.getEvent(&a, &g, &temp);
//
//  rad = atan2(a.acceleration.x,a.acceleration.y);
//  angle = ((360 * rad) / (2 * M_PI) + 180);
//  Serial.printf("Acceleration Y: %0.03f X:%0.03f\n:",a.acceleration.y,a.acceleration.x);
//  Serial.printf("Radians: %0.03f\n",rad);
//  Serial.printf("Angle: %0.03f\n",angle);
//  
//  delay(500);

  button1.tick();
  if (buttonState) {
    Serial.printf("Button Pressed\n");
    pixel.fill(blue, 0, 15);
    pixel.show();
  }
  else  {
    pixel.fill(red, 0, 15);
    pixel.show();
  }

}

void printIP() {
  Serial.printf("My IP address: ");
  for (byte thisByte = 0; thisByte < 3; thisByte++) {
    Serial.printf("%i.",Ethernet.localIP()[thisByte]);
  }
  Serial.printf("%i\n",Ethernet.localIP()[3]);
}

void click1()  {
  buttonState = !buttonState;
}

void doubleClick1() {
  Serial.printf("Double Clicked\n");
}

void longPressStart1()  {  
}

void longPress1() {
  i=0;
  pixel.setPixelColor(i, rainbow[i]);
  pixel.show();  
  delay(1000);
  i++;
  pixel.setPixelColor(i, rainbow[i]);
  pixel.show(); 
  delay(1000);
  i++;
  pixel.setPixelColor(i, rainbow[i]);
  pixel.show();
  delay(1000);
}

void longPressStop1() {
  i=15;
  pixel.setPixelColor(i, rainbow[i]);
  pixel.show();
  delay(1000);
  i--;
  pixel.setPixelColor(i, rainbow[i]);
  pixel.show();
  delay(1000);
  i--;
  pixel.setPixelColor(i, rainbow[i]);
  pixel.show();
  delay(1000);
}
