#include "Arduino.h"
#include "ULP.h"
#include <dht.h>
#include <Wire.h>
#include <MS5611.h>
#include <SD.h>
#define DHT22_PIN 2 //온습도는 A2로!

File myFile; //SD 함수정의
dht DHT; //온습도 함수정의
MS5611 ms5611; //기압 함수정의

const int C2 = A0; //Vgas, 센서 1번
const int T2 = A3; //Vtemp, 센서 3번
const float Sf2 = 4.05; // nA/ppm, 감도
O3 sensor2(C2, T2, Sf2); //오존 센서 변수지정
float temp1, TZero, Vzero1; //오존 센서 변수지정
double referencePressure; //기준기압
int uv_ain = A2; //UV, A2핀
int ad_value, uv_val; //UV센서 변수지정

void setup(){
  Serial.flush();
  Serial.begin(9600);
  pinMode(9, OUTPUT); //MS5611 기압센서 전원공급
  digitalWrite(9, HIGH); //MS5611 기압센서 전원공급
  pinMode(10, OUTPUT); // 온습도 전원공급
  digitalWrite(10, HIGH); // 온습도 전원공급
  pinMode(uv_ain, INPUT); //UV 핀지정
  while(!ms5611.begin())
  {
    Serial.println("Could not find a valid MS5611 sensor, check wiring!");
    delay(500); //기압 안되면 메시지 띄우기
  }
  referencePressure = ms5611.readPressure(); //기준기압 설정

  if(!SD.begin(53)){
    Serial.println("SD init failed!");
    return;
    }
    Serial.println("SD init done!");
  }

void loop(){
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
  Serial.print("[OZONE]: ");
  Serial.print(sensor2.getVgas(1));
  Serial.print("\n");
  myFile.println("[OZONE]: ");
  myFile.print(sensor2.getVgas(1));
  myFile.println("\n");
  delay(4000);
  
  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();
  Serial.print("[TEMP]: ");
  Serial.print(DHT.temperature, 1);
  Serial.print("\n");
  Serial.print("[HUMIDITY] :");
  Serial.print(DHT.humidity, 1);
  Serial.print("\n");
  myFile.println("[TEMP]: ");
  myFile.print(DHT.temperature, 1);
  myFile.println("\n");
  myFile.println("[HUMIDITY] :");
  myFile.print(DHT.humidity, 1);
  myFile.println("\n");
  delay(4000);
  
  
  long realPressure = ms5611.readPressure();
  long pressure = realPressure / 100;
  float relativeAltitude = ms5611.getAltitude(realPressure, referencePressure); 
  
  Serial.print("[Pressure] :");
  Serial.print(pressure);
  Serial.print("\n");
  Serial.print("[Altitude] :");
  Serial.print(relativeAltitude);
  Serial.print("\n");
  myFile.println("[Pressure] :");
  myFile.print(pressure);
  myFile.println("\n");
  myFile.println("[Altitude] :");
  myFile.print(relativeAltitude);
  Serial.println("\n");
  delay(4000);

  ad_value = analogRead(uv_ain);
  uv_val = ad_value * 307; //Irradiance, mW/m^2, 일정 면적당 받는 전자기력?
  Serial.print("[UV/Irradiance]: ");
  Serial.print(uv_val);
  Serial.print("\n");
  myFile.println("[UV/Irradiance]: ");
  myFile.print(uv_val);
  myFile.println("\n");
  delay(4000);
  
  myFile.close();
  }
  
}
