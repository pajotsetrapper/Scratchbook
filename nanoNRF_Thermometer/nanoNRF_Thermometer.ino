/*
Sketch intended for Nano RF Arduino (Arduino nano with built-in NRF24L01)
https://nl.aliexpress.com/item/4000457027535.html?spm=a2g0s.9042311.0.0.27424c4dQauFES
ATmega en NRF24L01 zijn verbonden door CSN = D8 CE = D7
*/

#define MY_RF24_CE_PIN 7
#define MY_RF24_CS_PIN 8
#define ONE_WIRE_BUS 2
#define PIN_CLK 4
#define PIN_DIO 3


// Enable debug prints to serial monitor
#define MY_DEBUG
// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_REPEATER_FEATURE //Enable repeater mode

//Configure channel (frequency), transmission power & speed. Lower speed => higher range.
//These values are the same as the ones used on the gateway (mandatory!)
#define MY_RF24_DATARATE (RF24_1MBPS) // RF24_1MBPS for 1Mbps / RF24_2MBPS for 2Mbps // @note nRF24L01, BK2401, BK2421, BK2491 and XN297 does not support RF24_250KBPS
#define MY_RF24_PA_LEVEL (RF24_PA_MAX) // RF24 PA level for sending msgs // RF24_PA_MIN = -18dBm; RF24_PA_LOW = -12dBm; RF24_PA_HIGH = -6dBm; RF24_PA_MAX = 0dBm
#define MY_RF24_CHANNEL (85) //Set to 2485Mhz, to be outside of common used Wifi 2.4Ghz Bands & outside of medical device range
// https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/g/n/ax)
// https://www.bipt.be/en/operators/radio/frequency-management/frequency-plan/table

#define CHILD_ID_TEMP 1
#define SEND_ATTEMPTS 100

#include <MySensors.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "SevenSegmentFun.h"
#include <OneWire.h>                  //Dallas OneWire protocol
#include <DallasTemperature.h>        //DS18b20

#include <avr/wdt.h>

SevenSegmentFun display(PIN_CLK, PIN_DIO);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
MyMessage temperature_msg(CHILD_ID_TEMP, V_TEMP);
unsigned long latest_update_timestamp = 0;
float temp=NAN;

float getTemperature(){
  float temp;
  ds18b20.requestTemperatures();
  //temp = round(ds18b20.getTempCByIndex(0));
  temp = ds18b20.getTempCByIndex(0);
  Serial.print("temperature: "); Serial.println(temp);
  return (temp);
}

void setup()
{
  wdt_enable(WDTO_8S);
  ds18b20.begin();
  display.begin();
  display.setBacklight(100);  
  display.clear();
  display.setCursor(0,0);
  display.print(getTemperature(),1);
}

void presentation()
{
  sendSketchInfo("NanoNRF24L01", "1.0");
  present(CHILD_ID_TEMP, S_TEMP);
}

bool sendMySensorsMsgWithRetries(MyMessage msg, int max_attempts){
  //return true if message was sent successfully, false otherwise.
  for (unsigned int cnt=1; cnt <= max_attempts; cnt++){
    Serial.print("Sending message, attempt "); Serial.println(String(cnt));
      if (send(msg)){
        return(true);
      }
      else{
        wait(50);
      }
    }
    return (false);
}

void loop()
{
  wdt_reset();
  if ((millis() - latest_update_timestamp) > 30000){
    latest_update_timestamp = millis();    
    temp = getTemperature();
    display.clear();
    display.setCursor(0,0);
    display.print(round(getTemperature()));
    sendMySensorsMsgWithRetries(temperature_msg.set(temp, 1), SEND_ATTEMPTS);
  }
}
