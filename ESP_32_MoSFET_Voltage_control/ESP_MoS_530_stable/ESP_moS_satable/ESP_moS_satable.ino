/*Main program

date of creation 02-Feb-2004

Auther K.M.Yadukriahnan
@ R L Jalappa Institute of Technology

follow @
github YadukrishnanKM https://github.com/YadukrishnanKM
instagram  kmy_126_

*/


#include "prog_config.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WebServer server(80);                                 // the server uses port 80 (standard port for websites)
WebSocketsServer webSocket(81);                       // the webSocket uses port 81 (standard port for updates)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


String webpage = "<!DOCTYPE html><html lang=\"en\"><head><title>MoS Volt Ctrl</title></head><style>h1{ text-align:center; } p { text-align:center; } #p1 { color: #ff8400;text-align: center;      font-weight: bold;      font-style: oblique;    }    #BTN_SEND_BACK {      border: 50%%;      border-radius: 15px;      background-color: #85bef7;      color: rgb(0, 0, 0);      padding: 15px 32px;      text-align: center;      text-decoration: none;      font-weight: bolder;      display: inline-block;      font-size: 16px;      margin: 4px 2px;      cursor: pointer;    }    .slider {      -webkit-appearance: none;      width: 70%%;      height: 15px;      border-radius: 25px;      background: #8ee3fd;      outline: none;      opacity: 0.7;      -webkit-transition: 0.2s;      transition: opacity 0.2s;      margin: 240px;    }    .slider::-webkit-slider-thumb {      -webkit-appearance: none;      appearance: none;      width: 15px;      height: 25px;      border-radius: 15%%;      background: #ff8400;      cursor: pointer;    }    .slider::-moz-range-thumb {      width: 25px;      height: 25px;      border-radius: 50%;      background: #046daa;      cursor: pointer;    }  </style>  <body style=\"background-color: #150e0e\">    <span style=\"color: #85bef7\">      <h1>MOSFET Voltage Control (ESP-32)</h1>  <p>The current voltage is : <span id=\"volt1\">-</span></p> <div class=\"slidecontainer\">  <input  type=\"range\"  min=\"0\" max=\"194\"   value=\"0\" class=\"slider\" id=\"voltage_Range\" /> <p id=\"p1\">The slider voltage is : <span id=\"volt2\">-</span></p> <p id=\"p2\"><button type=\"button\" id=\"BTN_SEND_BACK\">send</button></p> </div> </span>  </body>  <script>    var Socket;    document      .getElementById(\"BTN_SEND_BACK\")      .addEventListener(\"click\", button_send_back);    var slider = document.getElementById(\"voltage_Range\");    var output = document.getElementById(\"volt2\");    var isShow = true;    output.innerHTML = slider.value;    update = () => {      output.innerHTML = (slider.value / 255) * 3.3;      console.log(slider.value);    };    slider.addEventListener(\"input\", update);    function init() {      Socket = new WebSocket(\"ws://\" + window.location.hostname + \":81/\");      Socket.onmessage = function (event) {        processCommand(event);      };    }    function button_send_back() {      var msg = {        type: \"Digi\",        value: slider.value,      };      Socket.send(JSON.stringify(msg));    }    function processCommand(event) {      var obj = JSON.parse(event.data);      document.getElementById(\"volt1\").innerHTML = obj.volt;      console.log(ob.type, \" , \", obj.volt);    }    window.onload = function (event) {      init();    };  </script></html>";


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_INDICATOR, OUTPUT);     //setup the indicatior pin
  analogSetWidth(4095);              // setup the analog bits (4095 for 12bit)
  Serial.begin(115200);              // initiate the serial comunication 

  WiFi.begin(ssid, password);                                                               //initiate Wifi
  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));

  com_check();                                                                              // check the connectoin
  
  server.on("/", []() 
    {
      server.send(200, "text/html", webpage);
    }
  );

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();                              // Needed for the webserver to handle all clients

  webSocket.loop(); 

  now = millis();

  if ((unsigned long)(now - hardware_ADC_Millies) > ADC_interval){
    MOSFET_SOURCE_FEEDBACK_voltage();
    delay(100);
    MOSFET_GATE_FEEDBACK_voltage();
    hardware_ADC_Millies = now;
  }

  if ((unsigned long)(now - Web_event_init_Millies) > WEB_interval){

    Send_info_Json(gate_feedback_voltage);

    Serial.print("DAC Val is :");
    Serial.println(DAC_Value);
    Serial.print("AdC Val is :");
    Serial.println(gate_feedback_voltage);


    Web_event_init_Millies = now;
  }
}

void Send_info_Json(float ADC_val){

  String jsonString = "";
  StaticJsonDocument<200> doc;                      // create a JSON container
  JsonObject object = doc.to<JsonObject>();

  object["type"] = "Analog";                   
  object["volt"] = ADC_val;
  serializeJson(doc, jsonString);                   // convert JSON object to string
  Serial.println(jsonString);                       // print JSON string to console for debug purposes (you can comment this out)
  webSocket.broadcastTXT(jsonString);
}
