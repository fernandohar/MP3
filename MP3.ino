#include <Arduino.h>
#include "SoftwareSerial.h"
#include "MP3TF16P.h"


//SSD1306 OLED BEGIN
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C // 0X3C+SA0 - 0x3C or 0x3D
#define RST_PIN -1 // Define proper RST_PIN if required.

SSD1306AsciiWire oled; //Global Variable
//SSD1306 OLED END


//POT
#define POT_PIN  7
int potReading = 0;
int setVolume = 0;
int currentVolume = 0;
//PIR
#define PIR_PIN  2              // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

//mp3
#define MAXFILE 2
SoftwareSerial ss(10,11);
MP3TF16P mp3(&ss, &Serial);
int mp3FileName2Play = 0;
unsigned long songStartTime;


void initOLED(){
 Wire.begin();
 Wire.setClock(400000L);
 oled.begin(&Adafruit128x64, I2C_ADDRESS);
 oled.setFont(SystemFont5x7);
 
 oled.set2X();
 oled.println("  OLED  ");
 oled.println("DEBUGGER");
 //delay(5000);
 oled.clear();
 oled.set1X();
 oled.setScroll(true);
}

void initMP3TF16P(){
    mp3.setDebug(true);
    mp3.begin();
    mp3.stop();
    delay(100);
    mp3.setAmplification(false, 0);
    mp3.setVol(10);
    delay(100);
    mp3.setDebug(false);
}

void checkPot(){
 potReading = analogRead(POT_PIN);
 //Serial.print("potReading:"); Serial.print (potReading);
 potReading = min(800, analogRead(POT_PIN)); 
  
 setVolume = max(0,min(30, (800 - potReading)));
 if(abs(currentVolume - setVolume) > 2){
  mp3.setVol( setVolume );
  currentVolume = setVolume;
  uint8_t mp3_Vol = mp3.getVol();
  delay(10);
  Serial.print("Volume:"); Serial.println(mp3_Vol);
 }
 // mp3.setVol( volume );
 
}

void checkPIR(){
  val = digitalRead(PIR_PIN);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      playSong();
      pirState = HIGH;
    }
  } else {
   
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      
      pirState = LOW;
    }
  }
}
void playSong(){
  uint8_t playStatus = mp3.getPlayStatus();
  //playStatus 1 Playing
  //playStatus 2 Paused
  //playStatus 0 Stopped
  //playStatus 8 no device online / sleeping
  if(playStatus == 8){
    oled.println("[OFFLINE]");
    return;
  }else if (playStatus == 1){
    //oled.println("[PLAYING]");
    unsigned long elapse = (millis() - songStartTime) / 1000;
    oled.setCursor(0, 2); //col, row
    oled.print ("Elapse time:");
    oled.println(elapse);
    return;
  }else if (playStatus == 2){
    mp3.play();
    oled.println("[PAUSED] mp3.play()");
  }else if (playStatus == 0){
    if(++mp3FileName2Play > MAXFILE){
      mp3FileName2Play = 1;
    }
    mp3.playMp3File(mp3FileName2Play);
    songStartTime = millis();
    oled.clear();
    oled.print("playMp3File #");
    oled.println(mp3FileName2Play);
  }else{
    oled.clear();
    oled.print("[ERROR]");
    oled.println(playStatus);
  }
}

void setup() {
    Serial.begin(115200);   
        
    initOLED();
    initMP3TF16P();
  
    uint8_t mp3_Vol = mp3.getVol();
    oled.print("Volume:"); oled.println(mp3_Vol);

    //mp3.setDebug(true);
}

void loop() {
    //uint8_t playStatus = mp3.getPlayStatus();
    //playSong();
    checkPot();
    checkPIR();
    //delay(1000);
}
