/*
 * Title:       Smart Room 8 sided die controller
 * Description: Using an accelerometer to control lights based on orientation.
 * Author:      Liv Bouligny
 * Date:        14 July 2022
 */

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

float angle;
float rad;
byte count, i;

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(9600);
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

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.printf("Accelerometer range set to: 8G\n");
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
}

void loop() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  rad = atan2(a.acceleration.x,a.acceleration.y);
  angle = ((360 * rad) / (2 * M_PI) + 180);
  Serial.printf("Acceleration Y: %0.03f X:%0.03f\n:",a.acceleration.y,a.acceleration.x);
  Serial.printf("Radians: %0.03f\n",rad);
  Serial.printf("Angle: %0.03f\n",angle);
  
  delay(500);

}
