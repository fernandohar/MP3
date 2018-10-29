#include <Arduino.h>
#include "SoftwareSerial.h"
#include "MP3TF16P.h"


///////////////////////SSD1306 OLED BEGIN//////////////////////////
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C // 0X3C+SA0 - 0x3C or 0x3D
#define RST_PIN -1 // Define proper RST_PIN if required.
SSD1306AsciiWire oled; //Global Variable
///////////////////////SSD1306 OLED END//////////////////////////



///////////////////////HC-SR04 OLED BEGIN//////////////////////////
#define TRIG_PIN 7
#define ECHO_PIN 8
#define ULTRASOUND_CHECK_FREQ 1500
unsigned long ultrasoundLastCheck = 0;
long duration;
int distance;
///////////////////////HC-SR04 OLED END//////////////////////////

///////////////////////MP3 END//////////////////////////
#define MAXFILE 2
SoftwareSerial ss(10,11);
MP3TF16P mp3(&ss, &Serial);
int mp3FileName2Play = 0;
#define MP3_CHECK_FREQ 1500
unsigned long mp3LastCheck = 0;
unsigned long songStartTime;
///////////////////////mp3 END//////////////////////////

void initOLED(){
 Wire.begin();
 Wire.setClock(400000L);
 oled.begin(&Adafruit128x64, I2C_ADDRESS);
 oled.setFont(SystemFont5x7);
 
 oled.set2X();
 oled.println("  OLED  ");
 oled.println("DEBUGGER");
 delay(5000);
 oled.clear();
 oled.set1X();
 oled.setScroll(true);
}
void initHCSR04(){
  pinMode(TRIG_PIN, OUTPUT); //Set trigPin as Output
  pinMode(ECHO_PIN, INPUT); //Set echoPin as Input
  
}
void initMP3TF16P(){
    mp3.setDebug(false);
    mp3.begin();
    mp3.stop();
    delay(10);
    mp3.setAmplification(true, 3);
    mp3.setVol(20);
    delay(10);
}

void playSong(){
   uint8_t playStatus = mp3.getPlayStatus();
  if(playStatus == 8){
    oled.println("[OFFLINE]");
    return;
  }else if (playStatus == 1){
    unsigned long elapse = (millis() - songStartTime) / 1000;
    oled.setCursor(0, 2); //col, row
    oled.print ("Elapse time:");
    oled.println(elapse);
    return;
  }else if (playStatus == 2){
    mp3.play();
    oled.println("[RESUME]");
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
void stopSong(){
  mp3.pause();
  oled.clear();
  oled.println("PAUSED");
}
void loopSong(unsigned long currentMillis){
  if ((currentMillis - mp3LastCheck) < MP3_CHECK_FREQ){
    return;
  }
  
  mp3LastCheck = currentMillis;
 
  uint8_t playStatus = mp3.getPlayStatus();
  //playStatus 1 Playing
    //playStatus 2 Paused
    //playStatus 0 Stopped
    //playStatus 8 no device online / sleeping
  if(playStatus == 8){
    oled.clear();
    oled.println("[OFFLINE]");
    return;
  }else if (playStatus == 1){
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

int ultraSoundDetect(){
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);  

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  return distance;
}
void setup() {
    Serial.begin(115200);

    //Hardware initialization
    initHCSR04();    
    initOLED();
    initMP3TF16P();

    uint8_t mp3_Vol = mp3.getVol();
    oled.print("Volume:"); oled.println(mp3_Vol);

    //mp3.setDebug(true);
}

void loop() {
    uint8_t playStatus = mp3.getPlayStatus();
    unsigned long current = millis();
    if ((current - ultrasoundLastCheck) >= ULTRASOUND_CHECK_FREQ){
      ultrasoundLastCheck = current;
      if( ultraSoundDetect() < 50){
        playSong();
      }else{
        stopSong();
      }
    }
    delay(1000);
}
