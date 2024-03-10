#ifndef PROG_CONFIG
#define PROG_CONFIG

#include <WiFi.h>

#include <Server.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "WebSocketsServer.h"

#include <ArduinoJson.h>

#define MOSFET_GATE_PIN 25
#define MOSFET_GATE_FEEDBACK 36
#define MOSFET_SOURCE_FEEDBACK 39
#define LED_INDICATOR 2
#define DAC_PIN 25

unsigned long now;
unsigned long hardware_ADC_Millies = 0;  
unsigned long Web_event_init_Millies = 0;
uint16_t ADC_interval = 500;
uint16_t WEB_interval = 500;

static const String ssid = "HUAWEI nova 3i KM_Y";
static const String password = "1@#_s-27R93";

extern uint8_t DAC_Value;
extern bool led_indicaror_state;

extern volatile float source_feedback_voltage;
extern volatile float gate_feedback_voltage;

void com_check(void);

float MOSFET_SOURCE_FEEDBACK_voltage(void);

float MOSFET_GATE_FEEDBACK_voltage(void);

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);

#endif