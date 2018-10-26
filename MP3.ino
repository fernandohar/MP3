#include <Arduino.h>
#include "SoftwareSerial.h"
#include "MP3TF16P.h"

#define MAXFILE 2
SoftwareSerial ss(10,11);

MP3TF16P mp3(&ss, &Serial);
int playFile = 0;
void setup() {
    Serial.begin(115200);
    Serial.println("Let's go!");
    mp3.setDebug(true);
    mp3.begin();
    mp3.stop();
    delay(10);
    mp3.setAmplification(false, 0);
    mp3.setVol(1);
    delay(10);
    uint8_t mp3_Vol = mp3.getVol();
    Serial.print("\nVol: ");
    Serial.println(mp3_Vol);

    //mp3.playMp3File(++playFile);
    //mp3.playAllLoop();
    mp3.setDebug(false);
}

void loop() {
    
    uint8_t playStatus = mp3.getPlayStatus();
    //playStatus 1 Playing
    //playStatus 2 Paused
    //playStatus 0 Stopped
    //playStatus 8 no device online / sleeping
    if(playStatus == 2 ){
      mp3.play(); //Resume
      Serial.println("\nResume song");
    }else if (playStatus == 0){
      ++playFile;
      playFile = playFile % MAXFILE;
      mp3.playMp3File(playFile);
      Serial.println("");
      Serial.print("Playing File#");
      Serial.print(playFile);
    }else if(playStatus == 1){
      Serial.print(">");
    }else{
      Serial.println(playStatus);
    }
    //Serial.println("Keep going!");
    delay(1000);
}
