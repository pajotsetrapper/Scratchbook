/**
The Magic SoundMachine

- curl http://soundmachine.lan/playSound?params=3
- curl http://soundmachine.lan/changeVolume?params=7 (0-32)
**/

#define DFPLAY_RX D1
#define DFPLAY_TX D2
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino
#include <SoftwareSerial.h>
#include <DFMiniMp3.h> // https://github.com/Makuna/DFMiniMp3/wiki
#include <aREST.h> // https://www.arduino.cc/reference/en/libraries/arest/

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

int playSound(String command);
int changeVolume(String command);
SoftwareSerial dfPlaySerial(DFPLAY_TX, DFPLAY_RX);
DFMiniMp3<SoftwareSerial, Mp3Notify> dfplayer(dfPlaySerial);
aREST rest = aREST();
WiFiServer server = WiFiServer(80); //Webserver listening on TCP port 80

int playSound(String command) {  
  int tracknumber = 0;
  tracknumber = command.toInt();
  Serial.println(command);
  Serial.print("Request to play track "); Serial.println(tracknumber);
  dfplayer.stop();
  dfplayer.playMp3FolderTrack(tracknumber);
  return 0;
}

int changeVolume(String command) {
  int volume = 0;
  volume = command.toInt();
  Serial.println(command);
  Serial.print("Request to change volume to "); Serial.println(volume);
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

  // REST interface: Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("111000");
  rest.set_name((char*)"soundMachine");  
  rest.function((char*)"playSound", playSound); // register function to REST interface
  rest.function((char*)"changeVolume", changeVolume); // register function to REST interface
  server.begin();
  dfplayer.begin();  
  dfplayer.setVolume(26);
  dfplayer.playMp3FolderTrack(1);
}

void loop() {  
  dfplayer.loop();
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
}
