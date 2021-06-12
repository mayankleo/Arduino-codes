#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h> 
#include <FS.h>

AsyncWebServer server(80);
WebSocketsServer websockets(81); 

Ticker timer;

const char *ssid = "SONOFF_TEST";
const char *password = "123456789";

int relayState = 0;

const int button = 5;
const int relay = 4;
const int led = 2;

int cbs;
int lbs;
unsigned long previousTime = 0;
int tstat=0;
int tloop=0;

String stats; 
String staring;

char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HOME</title>
    <style>
        html {
            font-family: Helvetica;
            margin: 90px auto;
            text-align: center;
            cursor: crosshair;
            background: #242424;}
        h1 { color: white;}
        #button {
            background-color: rgb(80, 80, 80);
            border: 6px #f00 solid;
            outline: none;
            color: #f00;
            padding: 30px 60px;
            border-radius: 90px;
            font-size: 30px;
            font-weight: bold;
            margin: 2px;
            margin-bottom: 50px;
            cursor: pointer;}
        .timerbt {
            color: aquamarine;
            background: rgb(80, 80, 80);
            border: 3px rgb(255, 255, 0) solid;
            border-radius: 10px;
            height: 45px;
            width: 120px;
            font-size: 20px;
            padding: 5px;}
        #h,#m,#s {
            height: 30px;
            width: 60px;
            border-radius: 30px;
            border: 2px yellow solid;
            background: rgb(80, 80, 80);
            color: aquamarine;
            outline: none;}
        #stime {color: aquamarine;visibility: hidden;}
        .ltime {color: yellow;}
        label {color: #fff;}
        #timesetbt {margin-top: 10px;}
        #stopbt {display: none;}
        @media screen and (min-width: 601px) {#button {width: 200px;}}
        @media screen and (max-width: 1000px) {#button {width: 250px;}}
    </style>
</head>
<body>
    <h1>SONOFF WIFI SWITCH</h1>
    <button onclick="btaction()" id="button">OFF</button>
    <br>
    <select id="h">
        <option value=0>00</option>
        <option value=1>01</option>
        <option value=2>02</option>
        <option value=3>03</option>
        <option value=4>04</option>
        <option value=5>05</option>
        <option value=6>06</option>
        <option value=7>07</option>
        <option value=8>08</option>
        <option value=9>09</option>
        <option value=10>10</option>
        <option value=11>11</option>
    </select><label>HH</label>
    <select id="m">
        <option value=0>00</option>
        <option value=0.09>5 sec</option>
        <option value=5>05</option>
        <option value=10>10</option>
        <option value=15>15</option>
        <option value=20>20</option>
        <option value=25>25</option>
        <option value=30>30</option>
        <option value=35>35</option>
        <option value=40>40</option>
        <option value=45>45</option>
        <option value=50>50</option>
        <option value=55>55</option>
        <option value=60>60</option>
    </select><label>MM</label>
    <select id="s">
        <option value=1>ON</option>
        <option value=0>OFF</option>
    </select>
    <div id="timesetbt"><button onclick="setime()" id="setbt" class="timerbt">SET</button>
        <button onclick="stoptime()" id="stopbt" class="timerbt">STOP</button>
    </div>
    <h3 id="stime"><label class="ltime" id="timed">00:00:00</label> Time Left To <label class="ltime"
            id="times">OFF</label> The Device</h3>
    <script>
        var connection = new WebSocket('ws://' + location.hostname + ':81/');
        var buts = 0;
        var buttonid = document.getElementById("button");
        var full_data;
        var timeloop;
        var stat;
        var ttotal;
        var h = document.getElementById("h");
        var m = document.getElementById("m");
        var s = document.getElementById("s");
        var timed = document.getElementById("timed");
        var times = document.getElementById("times");
        function btaction() {
            if (buttonid.innerText == "OFF") {
                buts = 1;
                data = '{"type":1,"b1":' + buts + '}';
                connection.send(data);
            } else if (buttonid.innerText == "ON") {
                buts = 0;
                data = '{"type":1,"b1":' + buts + '}';
                connection.send(data);
            }
        }
        connection.onmessage = function (event) {
            var stats_data = event.data;
            var data = JSON.parse(stats_data);
            if (data.type == 1) {
                if (data.b1 == 1) {
                    buttonid.innerText = "ON";
                    buttonid.style.color = "#0f0";
                    buttonid.style.border = "6px #0f0 solid";
                } else {
                    buttonid.innerText = "OFF";
                    buttonid.style.color = "#f00";
                    buttonid.style.border = "6px #f00 solid";
                }
            } else if (data.type == 2) {
                if (data.tloop > 0) {
                    clearInterval(timeloop);
                    ttotal = data.tloop;
                    stat = data.tstat;
                    document.getElementById("stime").style.visibility = "visible";
                    document.getElementById("setbt").style.display = "none";
                    document.getElementById("stopbt").style.display = "inline-block";
                    if (stat == 1) {times.innerText = "ON";} 
                    else {times.innerText = "OFF";}
                    timeloop = setInterval(function () { startime(); }, 1000);
                }else{
                    clearInterval(timeloop);
                    document.getElementById("stime").style.visibility = "hidden";
                    document.getElementById("stopbt").style.display = "none";
                    document.getElementById("setbt").style.display = "inline-block";
                }
            }
        }
        function setime() {
            document.getElementById("stime").style.visibility = "visible";
            document.getElementById("setbt").style.display = "none";
            document.getElementById("stopbt").style.display = "inline-block";
            ttotal = Math.round(Number(h.value) * 60 * 60 + Number(m.value) * 60);
            stat = Number(s.value);
            jsondata = '{"type":2,"sec":' + ttotal + ',"stat":' + stat + '}';
            connection.send(jsondata);
        }
        function startime() {
            hoh = ttotal / (60 * 60);
            hhh = String(Math.floor(hoh));
            mom = (hoh - hhh) * 60;
            mmm = String(Math.floor(mom));
            sss = String(Math.round((mom - mmm) * 60));
            if (hhh.length == 1){hhh = "0" + hhh;}
            if (mmm.length == 1){mmm = "0" + mmm;}
            if (sss.length == 1){sss = "0" + sss;}
            timed.innerText = hhh + ":" + mmm + ":" + sss;
            if (ttotal <= 0) {
                document.getElementById("stime").style.visibility = "hidden";
                document.getElementById("stopbt").style.display = "none";
                document.getElementById("setbt").style.display = "inline-block";
                ttotal = 0;
                clearInterval(timeloop);
            }
        ttotal--;}
        function stoptime() {
            jsondata = '{"type":2,"sec":0,"stat":0}';
            connection.send(jsondata);
        }
    </script>
</body>
</html>
)=====";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) 
  {
    case WStype_CONNECTED:{
      stats = "{\"type\":1,\"b1\":"+String(relayState)+"}"; 
      websockets.broadcastTXT(stats);
      staring = "{\"type\":2,\"tloop\":"+String(tloop)+",\"tstat\":"+String(tstat)+"}";
      websockets.broadcastTXT(staring);}
    case WStype_TEXT:{
      String message = String((char*)( payload));
      DynamicJsonDocument doc(200);
      deserializeJson(doc, message);
      if (doc["type"]==1){
        relayState = doc["b1"];
        digitalWrite(relay,relayState);
        digitalWrite(led,relayState);
        stats = "{\"type\":1,\"b1\":"+String(relayState)+"}"; 
        websockets.broadcastTXT(stats);
      }else if (doc["type"]==2){
        tloop = doc["sec"];
        tstat = doc["stat"];
        staring = "{\"type\":2,\"tloop\":"+String(tloop)+",\"tstat\":"+String(tstat)+"}";
        websockets.broadcastTXT(staring);
      }}
  }
}


void NotFound(AsyncWebServerRequest *request){request->send(404, "text/plain", "PAGE NOT FOUND!!!");}

void setup()
{
  pinMode(button,INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(relay, relayState);
  digitalWrite(led, relayState);
  cbs = digitalRead(button);
  SPIFFS.begin();
  WiFi.softAP(ssid, password);

  server.on("/", [](AsyncWebServerRequest * request){request->send_P(200, "text/html", MAIN_page);});
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/favicon.png", "image/png");});
  server.onNotFound(NotFound);
  server.begin();
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  timer.attach(1,timecounter);
}

void loop(void)
{
    websockets.loop();
    lbs  = cbs;      
    cbs = digitalRead(button); 
    delay(50);
    if(lbs == HIGH && cbs == LOW) {
        if (relayState==1){
            relayState=0;
            digitalWrite(relay, relayState);
            digitalWrite(led, relayState);
            stats = "{\"type\":1,\"b1\":"+String(relayState)+"}";
            websockets.broadcastTXT(stats);
        }else if (relayState==0)
        {
            relayState=1;
            digitalWrite(relay, relayState);
            digitalWrite(led, relayState);
            stats = "{\"type\":1,\"b1\":"+String(relayState)+"}";
            websockets.broadcastTXT(stats);
        }
    }
}
void timecounter(){
  if (tloop>0){
     tloop--;
     if (tloop==0){
        delay(1300);
        relayState=tstat;
        digitalWrite(relay, relayState);
        digitalWrite(led, relayState);
        stats = "{\"type\":1,\"b1\":"+String(relayState)+"}"; 
        websockets.broadcastTXT(stats); 
     }
  }
}
