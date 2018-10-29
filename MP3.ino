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
#define ULTRASOUND_CHECK_FREQ 300
unsigned long ultrasoundLastCheck = 0;
///////////////////////HC-SR04 OLED END//////////////////////////

///////////////////////MP3 END//////////////////////////
#define MAXFILE 2
SoftwareSerial ss(10,11);
MP3TF16P mp3(&ss, &Serial);
int mp3FileNamePlay = -1;
#define MP3_CHECK_FREQ 1500
unsigned long mp3LastCheck = 0;
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
    mp3.setDebug(true);
    mp3.begin();
    mp3.stop();
    delay(10);
    mp3.setAmplification(false, 0);
    mp3.setVol(1);
    delay(10);
}

void playSong(unsigned long currentMicros){
  if ((currentMicros - ultrasoundLastCheck) < MP3_CHECK_FREQ){
    return;
  }else{
    ultrasoundLastCheck = currentMicros;
  }  
  uint8_t playStatus = mp3.getPlayStatus();
  //playStatus 1 Playing
    //playStatus 2 Paused
    //playStatus 0 Stopped
    //playStatus 8 no device online / sleeping
  if(playStatus == 8){
    oled.println("[OFFLINE]");
    return;
  }else if (playStatus == 1){
    oled.println("[PLAYING]");
    return;
  }else if (playStatus == 2){
    mp3.play();
    oled.println("[PAUSED] mp3.play()");
  }else if (playStatus == 0){
    oled.println("[STOPPED] playMp3File");
    ++mp3FileNamePlay;
    mp3FileNamePlay = mp3FileNamePlay % MAXFILE;
    mp3.playMp3File(mp3FileNamePlay);
  }else{
    oled.print("[ERROR]");
    oled.println(playStatus);
  }
}

void ultraSoundDetect(){
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
    oled.println("test");
    uint8_t playStatus = mp3.getPlayStatus();
    playSong();
    ultraSoundDetect();
    oled.println("Keep going!");
    delay(1000);
}
