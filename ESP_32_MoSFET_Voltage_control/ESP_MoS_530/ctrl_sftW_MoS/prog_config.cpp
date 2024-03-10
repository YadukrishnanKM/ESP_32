#include "esp32-hal-adc.h"
#include "esp32-hal.h"
//config file
#include "esp32-hal-gpio.h"
#include "Server.h"
#include <WiFi.h>
#include "WebSocketsServer.h"
#include <ArduinoJson.h> 

#define MoSFET_PIN 25            //DAC0
#define analog_OUT_FEEDBACK 32    //ADC 0
#define LED_indicator 2           // Wifi connection status LED (on board)

#define ADC_interval 1000            //for millies 1sec <-::-> It can also used for std-interval between data collection
#define WEB_interval 2000

uint8_t DAC_vaue;

float mosfet_voltage;

volatile int myADC;


static const String ssid = "HUAWEI nova 3i KM_Y";
static const String password = "1@#_s-27R93";


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void com_check(void){
  pinMode(LED_indicator, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {            
    delay(1000);
    Serial.print(".");
    bool led_indicaror_state = digitalRead(LED_indicator);
    digitalWrite(LED_indicator,(1^led_indicaror_state));

  }
  
  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP()); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float ADC_read_Val(void){
  volatile uint16_t feedback_val;
  volatile uint16_t constrain_feedback_val = (feedback_val/4095)*3.3; //75% of 4095 (12bit adc)<-::-> 75% of 3.3 aprox => 2.5

  return ((feedback_val/4095)*3.3);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length){

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  switch (type) {                                     // switch on the type of information sent
    case WStype_DISCONNECTED:                         // if a client is disconnected, then type == WStype_DISCONNECTED
      Serial.println("Client " + String(num) + " disconnected");
      break;

    case WStype_CONNECTED:                            // if a client is connected, then type == WStype_CONNECTED
      Serial.println("Client " + String(num) + " connected");
      // optionally you can add code here what to do when connected
      break;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
      // try to decipher the JSON string received
      StaticJsonDocument<200> doc;                    // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      else {
        // JSON string was received correctly, so information can be retrieved:
        const char* in_type = doc["type"];
        DAC_vaue = doc["value"];
        dacWrite(26,DAC_vaue);
        dacWrite(26,DAC_vaue);
        myADC = analogRead(32);
        dacWrite(26,DAC_vaue);
        mosfet_voltage = (myADC*(3.3/4045));
        //Serial.println((analogRead(32))*(3.3/4095));
        delay(3000);
  
      }

      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      Serial.println("");
      break;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}