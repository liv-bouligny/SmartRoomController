/*
 * Title:       Smart Room 8 sided die controller
 * Description: Using an accelerometer to control lights based on orientation.
 * Author:      Liv Bouligny
 * Date:        14 July 2022
 */
#include <wemo.h>
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
#include <mac.h>
#include <hue.h>

const int SCREENWIDTH = 128;
const int SCREENHEIGHT = 64;
const int OLEDRESET = -1;
const int SCREENADDRESS = 0x3C;
const int PIXELPIN = 8;
const int PIXELCOUNT = 32;
float rad;
float angle;
byte count, i;
int px = 0;
int currentTime;
int lastTime = 1000;
bool buttonState;
bool rollMode = LOW;
bool moveState = LOW;
bool last = LOW;
bool onState = LOW;
int previousSide;
int currentSide;
int deviceArray[8] = {1,2,3,4,5,6,0,2};

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
  button1.setClickTicks(500);
  button1.setPressTicks(2000);
  while(!Serial);
  Serial.printf("Serial Monitor Initialized\n");
  Wire.begin(); 
  Serial.printf("Adafruit MPU6050 test!\n");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.printf("MPU6050 Found!\n");
  
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  Ethernet.begin(mac);
  delay(200);          //ensure Serial Monitor is up and running           
  printIP();
  Serial.printf("LinkStatus: %i  \n",Ethernet.linkStatus());
  
  
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
  display.display();

  pixel.begin();
  pixel.setBrightness(125);
  pixel.show();  
  i=0;
}

void loop() {
//  Serial.printf("Acceleration Y: %0.03f X:%0.03f\n:",a.acceleration.y,a.acceleration.x);
//  Serial.printf("Radians: %0.03f\n",rad);
//  Serial.printf("Angle: %0.03f\n",angle);
//  
//  delay(500);  
//  currentAngle = ((360*rad) / (2*M_PI) + 180);  
  
  button1.tick();
  while (!rollMode) {     
    button1.tick();
    if (buttonState != last) {  
      if (buttonState) {    
        Serial.printf("ButtonState: %i\n",buttonState);        
        if (i < 6)  {
          Serial.printf("Hue light %i on!\n",deviceArray[i]);
          setHue(deviceArray[i],true,HueRainbow[(i)],200,255);
          pixel.fill(green, 0, 32);
          pixel.show(); 
        }
        else  {
          Serial.printf("Wemo %i on!\n",deviceArray[i]);
          switchON(deviceArray[i]);
          pixel.fill(green, 0, 32);
          pixel.show(); 
        }
      }
      else  {
        Serial.printf("ButtonState: %i\n",buttonState);
        if (i < 6)  {
          Serial.printf("Hue light %i off!\n",deviceArray[i]);
          pixel.fill(red, 0, 32);
          pixel.show();
          setHue(deviceArray[i],false,0,0,0);  
        }
        else {
          Serial.printf("Wemo %i off!\n",deviceArray[i]);
          switchOFF(deviceArray[i]);  
          pixel.fill(red, 0, 32);
          pixel.show();    
        }
      }
      last = buttonState;      
    } 
  }
//  readAccel();
//  currentSide = checkSide(angle);
//  previousSide = currentSide;
//  while (rollMode) {
//    button1.tick();
//    readAccel();
//    currentSide = checkSide(angle);    
//    if (currentSide != previousSide)  {             
//      delay(5000);          
//      readAccel();
//      currentSide = checkSide(angle);
//      Serial.printf("Current side up: %i\n",currentSide);
//      Serial.printf("%f\n",angle);             
//      previousSide = currentSide;     
//    }
//  }
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
  rollMode = !rollMode;
  if (rollMode == true)  {
    Serial.printf("rollMode: %i  Automatic Roll mode initiated!\n",rollMode);
  }
  else  {
    Serial.printf("rollMode: %i  Manual mode initiated!\n",rollMode);    
  }
}

void longPressStart1()  {  
}

void longPress1() {  
}

void longPressStop1() {  
  i++;
  if(i > 7) {
    i = 0;
  }
  Serial.printf("i = %i\n", i);
}

void readAccel() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);  
  rad = atan2(a.acceleration.x,a.acceleration.y);
  angle = ((360 * rad) / (2 * M_PI) + 180);
}

int checkSide(float inputAngle) {
  int answer;
  if ((inputAngle > 75)&&(inputAngle < 105))  {
    answer = 8;
    return answer;
  }
  if ((inputAngle > 30)&&(inputAngle < 60))  {
    answer = 2;
    return answer;
  }
  if (((inputAngle > 345)&&(inputAngle <= 360)) || ((inputAngle >=0)&&(inputAngle<15)))  {
    answer = 5;
    return answer;
  }
  if ((inputAngle > 300)&&(inputAngle < 330))  {
    answer = 6;
    return answer;
  }
  if ((inputAngle > 255)&&(inputAngle < 285))  {
    answer = 1;
    return answer;
  }
  if ((inputAngle > 210)&&(inputAngle < 240))  {
    answer = 4;
    return answer;
  }
  if ((inputAngle > 165)&&(inputAngle < 195))  {
    answer = 7;
    return answer;
  }
  if ((inputAngle > 120)&&(inputAngle < 150))  {
    answer = 3;
    return answer;
  }
  else  {
    answer = 0;
    return answer;
  }
}
