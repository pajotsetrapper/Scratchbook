
/* */

#define DFPLAY_RX D1
#define DFPLAY_TX D2

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>                //DFPlayer mini https://github.com/Makuna/DFMiniMp3/wiki

//Callbacks for DFPlayer events
class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      if (source & DfMp3_PlaySources_Sd) 
      {
          Serial.print("SD Card, ");
      }
      if (source & DfMp3_PlaySources_Usb) 
      {
          Serial.print("USB Disk, ");
      }
      if (source & DfMp3_PlaySources_Flash) 
      {
          Serial.print("Flash, ");
      }
      Serial.println(action);
    }
    static void OnError(uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);
    }
    static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
    {
      Serial.print("Play finished for #");
      Serial.println(track);
    }
    static void OnPlaySourceOnline(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "online");
    }
    static void OnPlaySourceInserted(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "inserted");
    }
    static void OnPlaySourceRemoved(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "removed");
    }
};

SoftwareSerial dfPlaySerial(DFPLAY_TX, DFPLAY_RX);
DFMiniMp3<SoftwareSerial, Mp3Notify> dfplayer(dfPlaySerial);


void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();
  
  while ((millis() - start) < msWait)
  {
    // calling mp3.loop() periodically allows for notifications 
    // to be handled without interrupts
    dfplayer.loop(); 
    delay(1);
  }
}

int playSound(String command) {  
  int tracknumber = 0;
  tracknumber = command.toInt();
  dfplayer.playMp3FolderTrack(tracknumber);
  return 0;
}

int changeVolume(String command) {  
  int volume = 0;
  volume = command.toInt();
  if ((volume >=0) and (volume <=32)){
    dfplayer.setVolume(volume);
  }
  return 0;
}

void setup(){

  bool res;
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  //wm.resetSettings(); reset settings - wipe stored credentials for testing (stored by esp library)
  res = wm.autoConnect(); // auto generated AP name from chipid, no pwd - connect to stored Wifi SSID
                          // Start config portal if it cannot connect
  if(!res) {
      Serial.println("Failed to connect to preconfigured Wifi");
      ESP.restart();
  }
  else {
      //if you get here you have connected to the WiFi
      Serial.println(WiFi.localIP());
  }
   
  dfplayer.begin();  
  uint16_t volume = dfplayer.getVolume();
  Serial.print("volume "); Serial.println(volume);
  dfplayer.setVolume(26);  
  dfplayer.playMp3FolderTrack(1);
}


void loop()  {
  dfplayer.loop();
}
