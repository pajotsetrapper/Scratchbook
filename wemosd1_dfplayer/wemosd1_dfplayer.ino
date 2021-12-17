
/* */

#define DFPLAY_RX D1
#define DFPLAY_TX D2

#define MY_MAC_ADDRESS 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02


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
  Serial.begin(115200);

  byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
  };
   
  dfplayer.begin();  
  uint16_t volume = dfplayer.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfplayer.setVolume(26);  
  dfplayer.playMp3FolderTrack(1);
}


void loop()  {
  dfplayer.loop();
}
