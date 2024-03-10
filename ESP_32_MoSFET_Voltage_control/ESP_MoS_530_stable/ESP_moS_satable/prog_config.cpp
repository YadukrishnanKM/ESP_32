#include <cstdint>
#include "esp32-hal-adc.h"
#include "esp32-hal.h"
#include "esp32-hal-gpio.h"

#include "Server.h"
#include <WiFi.h>
#include "WebSocketsServer.h"

#include <ArduinoJson.h> 

#define MOSFET_GATE_PIN 25
#define MOSFET_GATE_FEEDBACK 36
#define MOSFET_SOURCE_FEEDBACK 39
#define LED_INDICATOR 2
#define DAC_PIN 25

#define ADC_16_bit_CONVERSION(adc_val) adc_val*(3.3/4045)

uint8_t DAC_Value;;
bool led_indicaror_state;

volatile float source_feedback_voltage;
volatile float gate_feedback_voltage;


////////////////////////////////////////////////////////////////////////////////////////////////////

void com_check(void){
  /*check the wifi connection */
  while (WiFi.status() != WL_CONNECTED) {            
    delay(1000);
    Serial.print(".");
    led_indicaror_state = digitalRead(LED_INDICATOR);
    digitalWrite(LED_INDICATOR,(1^led_indicaror_state));

  }
  
  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP()); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float MOSFET_SOURCE_FEEDBACK_voltage(void){
  /* calculathe the Mosfet source voltage*/

  source_feedback_voltage = analogRead(MOSFET_SOURCE_FEEDBACK);
  source_feedback_voltage = ADC_16_bit_CONVERSION(source_feedback_voltage);
  return source_feedback_voltage;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float MOSFET_GATE_FEEDBACK_voltage(void){
  /* calculathe the Mosfet Gate voltage*/

  gate_feedback_voltage = analogRead(MOSFET_SOURCE_FEEDBACK);
  gate_feedback_voltage = ADC_16_bit_CONVERSION(gate_feedback_voltage);
  return gate_feedback_voltage;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length){

  switch (type) {                                     // switch on the type of information sent
    case WStype_DISCONNECTED:                         // if a client is disconnected, then type == WStype_DISCONNECTED
      Serial.println("Client " + String(num) + " disconnected");
      break;

    case WStype_CONNECTED:                            // if a client is connected, then type == WStype_CONNECTED
      Serial.println("Client " + String(num) + " connected");
      // optionally you can add code here what to do when connected
      break;

    case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
      // try to decipher the JSON string received
      StaticJsonDocument<200> doc;                    // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      else {
        // JSON string was received correctly, so information can be retrieved:
        const char* in_type = doc["type"];
        DAC_Value = doc["value"];
        dacWrite(MOSFET_GATE_PIN,DAC_Value);
        dacWrite(MOSFET_GATE_PIN,DAC_Value);      // conformation in case of VHS Switching event
        delay(100);                               //100 microsecond dely
  
      }

      Serial.println("");
      break;
  }

}