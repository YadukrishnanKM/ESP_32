#include <WiFi.h>

#include <Server.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "WebSocketsServer.h"

#include "driver/dac.h"
#include <ArduinoJson.h>  
  
#include "prog_config.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WebServer server(80);                                 // the server uses port 80 (standard port for websites)
WebSocketsServer webSocket(81);                       // the webSocket uses port 81 (standard port for updates)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String webpage = "<!DOCTYPE html><html lang=\"en\"><head><title>MoS Volt Ctrl</title></head><style>h1{ text-align:center; } p { text-align:center; } #p1 { color: #ff8400;text-align: center;      font-weight: bold;      font-style: oblique;    }    #BTN_SEND_BACK {      border: 50%%;      border-radius: 15px;      background-color: #85bef7;      color: rgb(0, 0, 0);      padding: 15px 32px;      text-align: center;      text-decoration: none;      font-weight: bolder;      display: inline-block;      font-size: 16px;      margin: 4px 2px;      cursor: pointer;    }    .slider {      -webkit-appearance: none;      width: 70%%;      height: 15px;      border-radius: 25px;      background: #8ee3fd;      outline: none;      opacity: 0.7;      -webkit-transition: 0.2s;      transition: opacity 0.2s;      margin: 240px;    }    .slider::-webkit-slider-thumb {      -webkit-appearance: none;      appearance: none;      width: 15px;      height: 25px;      border-radius: 15%%;      background: #ff8400;      cursor: pointer;    }    .slider::-moz-range-thumb {      width: 25px;      height: 25px;      border-radius: 50%;      background: #046daa;      cursor: pointer;    }  </style>  <body style=\"background-color: #150e0e\">    <span style=\"color: #85bef7\">      <h1>MOSFET Voltage Control (ESP-32)</h1>  <p>The current voltage is : <span id=\"volt1\">-</span></p> <div class=\"slidecontainer\">  <input  type=\"range\"  min=\"0\" max=\"255\"   value=\"0\" class=\"slider\" id=\"voltage_Range\" /> <p id=\"p1\">The slider voltage is : <span id=\"volt2\">-</span></p> <p id=\"p2\"><button type=\"button\" id=\"BTN_SEND_BACK\">send</button></p> </div> </span>  </body>  <script>    var Socket;    document      .getElementById(\"BTN_SEND_BACK\")      .addEventListener(\"click\", button_send_back);    var slider = document.getElementById(\"voltage_Range\");    var output = document.getElementById(\"volt2\");    var isShow = true;    output.innerHTML = slider.value;    update = () => {      output.innerHTML = (slider.value / 255) * 3.3;      console.log(slider.value);    };    slider.addEventListener(\"input\", update);    function init() {      Socket = new WebSocket(\"ws://\" + window.location.hostname + \":81/\");      Socket.onmessage = function (event) {        processCommand(event);      };    }    function button_send_back() {      var msg = {        type: \"Digi\",        value: slider.value,      };      Socket.send(JSON.stringify(msg));    }    function processCommand(event) {      var obj = JSON.parse(event.data);      document.getElementById(\"volt1\").innerHTML = obj.volt;      console.log(ob.type, \" , \", obj.volt);    }    window.onload = function (event) {      init();    };  </script></html>";
unsigned long hardware_ADC_Millies = 0;  
unsigned long Web_event_init_Millies = 0;

float ADC_val;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:

  /////////////////////////////////////////////////////////////////////////////////

  pinMode(MoSFET_PIN, OUTPUT); 
  pinMode(analog_OUT_FEEDBACK, INPUT);   //set the GPIO-26 as output 
  analogSetWidth(4095);
  Serial.begin(115200); 

  WiFi.begin(ssid, password);                         
  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));

  com_check();                   
  
  server.on("/", []() 
    {
      server.send(200, "text/html", webpage);
    }
  );

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  /////////////////////////////////////////////////////////////////////////////////


}

void loop() {
  // put your main code here, to run repeatedly:

  server.handleClient();                              // Needed for the webserver to handle all clients
  webSocket.loop(); 

  unsigned long now = millis();

  /////////////////////////////////////////////////////////////////////////////////////

  if ((unsigned long)(now - hardware_ADC_Millies) > ADC_interval){
    ADC_val = ADC_read_Val();
    hardware_ADC_Millies = now;
  }

  /////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////

  if ((unsigned long)(now - Web_event_init_Millies) > WEB_interval){

    Send_info_Json(ADC_val);

    Serial.print("DAC Val is :");
    Serial.println(DAC_vaue);
    Serial.print("AdC Val is :");
    Serial.println(mosfet_voltage);


    Web_event_init_Millies = now;
  }

  /////////////////////////////////////////////////////////////////////////////////////

}

void Send_info_Json(float ADC_val){

  String jsonString = "";
  StaticJsonDocument<200> doc;                      // create a JSON container
  JsonObject object = doc.to<JsonObject>();

  object["type"] = "Analog";                   
  object["volt"] = mosfet_voltage;
  serializeJson(doc, jsonString);                   // convert JSON object to string
  Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
  webSocket.broadcastTXT(jsonString);
}
