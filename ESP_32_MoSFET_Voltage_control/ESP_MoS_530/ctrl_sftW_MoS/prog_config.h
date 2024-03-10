//config file
#include "esp32-hal-gpio.h"
#include "Server.h"
#include <WiFi.h>
#include "WebSocketsServer.h"
#include <ArduinoJson.h> 

#define MoSFET_PIN 25           //DAC0
#define analog_OUT_FEEDBACK 32    //ADC 0
#define LED_indicator 2           // Wifi connection status LED (on board)

#define ADC_interval 1000            //for millies 1sec <-::-> It can also used for std-interval between data collection
#define WEB_interval 2000

extern uint8_t DAC_vaue;

extern float mosfet_voltage;



static const String ssid = "HUAWEI nova 3i KM_Y";
static const String password = "1@#_s-27R93";


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void com_check(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float ADC_read_Val(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);