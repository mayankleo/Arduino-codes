#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

AsyncWebServer server(80);
WebSocketsServer websockets(81);

const char *ssid = "SONOFF_HOME";
const char *password = "123456789";

int relayState = 0;

const int button = 0;
const int relay = 12;
const int led = 13;

int cbs;
int lbs;

int tstat = 0;
int tloop = 0;

unsigned long previousTime = 0;

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
            margin: 110px auto;
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
            cursor: pointer;
            letter-spacing: 4px;
            text-shadow: 0 0 3px #f00;
            box-shadow: 0 0 10px #f00, 0 0 40px #f00;}
        .timerbt {
            color: aquamarine;
            background: rgb(80, 80, 80);
            border: 3px rgb(255, 255, 0) solid;
            border-radius: 10px;
            height: 45px;
            width: 120px;
            font-size: 20px;
            padding: 5px;
            cursor: pointer;}
        #h,#m,#s {
            height: 30px;
            width: 90px;
            border-radius: 30px;
            border: 2px yellow solid;
            background: rgb(80, 80, 80);
            color: aquamarine;
            outline: none;
            cursor: pointer;}
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
        <option value=1>01 Hours</option>
        <option value=2>02 Hours</option>
        <option value=3>03 Hours</option>
        <option value=4>04 Hours</option>
        <option value=5>05 Hours</option>
        <option value=6>06 Hours</option>
        <option value=7>07 Hours</option>
        <option value=8>08 Hours</option>
        <option value=9>09 Hours</option>
        <option value=10>10 Hours</option>
        <option value=11>11 Hours</option>
    </select><label>HH</label>
    <select id="m">
        <option value=0>00</option>
        <option value=0.09>05 Seconds</option>
        <option value=5 >05 Minutes</option>
        <option value=10>10 Minutes</option>
        <option value=15>15 Minutes</option>
        <option value=20>20 Minutes</option>
        <option value=25>25 Minutes</option>
        <option value=30>30 Minutes</option>
        <option value=35>35 Minutes</option>
        <option value=40>40 Minutes</option>
        <option value=45>45 Minutes</option>
        <option value=50>50 Minutes</option>
        <option value=55>55 Minutes</option>
        <option value=60>60 Minutes</option>
    </select><label>MM</label>
    <select id="s">
        <option value=1>ON</option>
        <option value=0>OFF</option>
    </select>
    <div id="timesetbt"><button onclick="setime()" id="setbt" class="timerbt">SET</button>
        <button onclick="timedata(0,0)" id="stopbt" class="timerbt">STOP</button>
    </div>
    <h3 id="stime"><label class="ltime" id="timed">00:00:00</label> Time Left To <label class="ltime"
            id="times">OFF</label> The Device</h3>
    <script>
        var connection = new WebSocket('ws://' + location.hostname + ':81/');
        var buts = 0;
        var buttonid = document.getElementById("button");
        var btstyle = buttonid.style
        var full_data;
        var timeloop;
        var stat;
        var ttotal;
        var h = document.getElementById("h");
        var m = document.getElementById("m");
        var s = document.getElementById("s");
        var timed = document.getElementById("timed");
        var times = document.getElementById("times");
        var stime = document.getElementById("stime") .style
        var stopbt = document.getElementById("stopbt").style
        var setbt = document.getElementById("setbt") .style
        function btaction() {
            if (buttonid.innerText == "OFF"){buts = 1;} 
            else{buts = 0;}
            data = '{"type":1,"b1":' + buts + '}';
            connection.send(data);
        }
        connection.onmessage = function (event) {
            var stats_data = event.data;
            var data = JSON.parse(stats_data);
            if (data.type == 1) {
                if (data.b1 == 1) {
                    buttonid.innerText = "ON";
                    btstyle.color = "#0f0";
                    btstyle.borderColor = "#0f0";
                    btstyle.boxShadow = "0 0 10px #0f0, 0 0 40px #0f0";
                    btstyle.textShadow = "0 0 3px #0f0";
                } else {
                    buttonid.innerText = "OFF";
                    btstyle.color = "#f00";
                    btstyle.borderColor = "#f00";
                    btstyle.boxShadow = "0 0 10px #f00, 0 0 40px #f00";
                    btstyle.textShadow = "0 0 3px #f00";
                }
            } else if (data.type == 2) {
                if (data.tloop > 0) {
                    clearInterval(timeloop);
                    ttotal = data.tloop;
                    stat = data.tstat;
                    containts("visible","inline-block","none");
                    if (stat == 1) {times.innerText = "ON";} 
                    else {times.innerText = "OFF";}
                    timeloop = setInterval(function () { startime(); }, 1000);
                }else{
                    clearInterval(timeloop);
                    containts("hidden","none","inline-block");
                }
            }
        }
        function setime() {
            ttotal = Math.round(Number(h.value) * 60 * 60 + Number(m.value) * 60);
            stat = Number(s.value);
            timedata(ttotal,stat);
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
                containts("hidden","none","inline-block");
                ttotal = 0;
                clearInterval(timeloop);
            }
        ttotal--;}

        function timedata(sec,stat){
            jsondata = '{"type":2,"sec":' + sec + ',"stat":' + stat + '}';
            connection.send(jsondata);}

        function containts(a,b,c){
            stime.visibility = a;
            stopbt.display = b;
            setbt.display = c;} 
            
    </script>
</body>
</html>
)=====";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
        case WStype_CONNECTED:
        {
            relayaction(relayState);
            timerset(tloop,tstat);
        }
        case WStype_TEXT:
        {
            String message = String((char *)(payload));
            DynamicJsonDocument doc(200);
            deserializeJson(doc, message);
            if (doc["type"] == 1)
            {
                relayaction(doc["b1"]);
            }
            else if (doc["type"] == 2)
            {
                tloop = doc["sec"];
                tstat = doc["stat"];
                timerset(tloop,tstat);
            }
        }
    }
}

void setup()
{   
    pinMode(button, INPUT_PULLUP);
    pinMode(relay, OUTPUT);
    pinMode(led, OUTPUT);
    digitalWrite(relay, relayState);
    digitalWrite(led, relayState);
    cbs = digitalRead(button);
    
    EEPROM.begin(1);

    WiFi.softAP(ssid, password);
    server.on("/", [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", MAIN_page); });
    server.begin();
    websockets.begin();
    websockets.onEvent(webSocketEvent);

    int value = EEPROM.read(0);
    if (value==1){
          relayState = 1;
          digitalWrite(relay, 1);
          digitalWrite(led, 1);
      }         
}

void loop()
{
    unsigned long currentTime = millis();
    websockets.loop();
    lbs = cbs;
    cbs = digitalRead(button);
    delay(50);
    if (lbs == HIGH && cbs == LOW){
      relayaction(!relayState);
    }

  if (currentTime - previousTime >= 1000){
      if (tloop > 0)
    {
        tloop--;
        if (tloop == 0)
        {
            delay(1500);
            relayaction(tstat);
        }
    }
    previousTime = currentTime;
  }
    
}

void relayaction(int input_state)
{
    relayState = input_state;
    digitalWrite(relay, input_state);
    digitalWrite(led, input_state);
    EEPROM.write(0, input_state);
    EEPROM.commit();
    stats = "{\"type\":1,\"b1\":" + String(input_state) + "}";
    websockets.broadcastTXT(stats);
}

void timerset(int loops,int stat)
{
    staring = "{\"type\":2,\"tloop\":" + String(loops) + ",\"tstat\":" + String(stat) + "}";
    websockets.broadcastTXT(staring);
}
