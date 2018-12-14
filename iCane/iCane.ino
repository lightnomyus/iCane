#include "Adafruit_FONA.h"
#include <TinyGPS++.h>
#include <Servo.h>
#define FONA_RST 4
#define sendButton 5
#define wheelButton 6
#define servoPin1 9

//Create Object
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
Servo wheelServo;

//variabel
char replybuffer[255];
String sendtoNumber = "08174115932";//nomor tujuan SMS
String gps_latitude = "";
String gps_longitude = "";
int sudutAwalServo1 = 0;//sudut awal servo dalam satuan derajat (0 - 180), kondisi awal = roda turun
int sudutAkhirServo1 = 150;//sudut akhir servo dalam satuan derajat (0 - 180), kondisi awal = roda turun
bool isRetracted = false;

void setup() {
  //check GSM
  while (!Serial);
  Serial.begin(115200);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  Serial1.begin(4800);//Serial1 = for Fonas GSM, baud rate = 4800 
  if (! Serial1.begin()) {            
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));

  //check GPS
  Serial2.begin(9600);//Serial2 = for Fonas GPS NEO 6M, baud rate = 9600

  //input mode
  pinMode(sendButton,INPUT_PULLUP);//push button to send location
  pinMode(wheelButton, INPUT_PULLUP);//push button to retract/ reject wheel

  //servo
  wheelServo.attach(servoPin1);//

}

void loop() {
  //update GPS lat and long
  while (Serial2.available()){
    gps.encode(Serial2.read());
  }
  cekGPS();//check data from GPS
  
  //send Message by a button
  if( digitalRead(sendButton)==LOW ){
    sendLocation();
  }

  if( digitalRead(wheelButton)==LOW ){
    if( !isRetracted ){
      writeServo(sudutAwalServo1, sudutAkhirServo1);
      isRetracted=true;//roda sudah dlm posisi retracted 
    } else{
      writeServo(sudutAkhirServo1, sudutAwalServo1);
      isRetracted=false;//roda dlm posisi turun / not retracted
    }
  }
}

void sendLocation(){
  //to send coordinate via sms use this command:
  //http://maps.google.com/?q=<lat>,<lng>

  String message = "http://maps.google.com/?q=" + gps_latitude + gps_longitude;
  if (!fona.sendSMS(sendtoNumber, message)) {
    Serial.println(F("Failed"));//fail to send GPS location
  } else {
    Serial.println(F("Sent!"));//GPS location sent!
  }
}

void cekGPS(){
  if(gps.location.isValid()){
    gps_latitude = "<" + String(gps.location.lat()) + ">,";
    gps_longitude = "<" + String(gps.location.lng()) + ">";
  }
}

void writeServo(int awal, int akhir){
  int servoPosisition;
  if(awal<akhir){
    for( servoPosisition=awal; servoPosisition<=akhir; servoPosisition=servoPosisition+1 ){
      wheelServo.write(servoPosisition);
      delay(10);
    }
  } else{
    for( servoPosisition=awal; servoPosisition<=akhir; servoPosisition=servoPosisition-1 ){
      wheelServo.write(servoPosisition);
      delay(10);
    }
  }
}