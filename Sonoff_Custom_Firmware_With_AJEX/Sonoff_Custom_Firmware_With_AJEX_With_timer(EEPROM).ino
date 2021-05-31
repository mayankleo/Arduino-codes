#include <ESP8266WiFi.h>           
#include <ESP8266WebServer.h>   
#include <EEPROM.h>
   
ESP8266WebServer server(80); 

//change ssid and password according to you and upload 
//after that connect you phone/pc from this wifi
//and search "home.local" and you can set on/off time also ,
// IN THIS PROGRAM YOU WILL GET EEPROM SUPPORT WHICH SAVE YOUR APPLINCE STATUS SO THAT YOU WILL NOT LOSE YOUR APPLINCE STATE AFTER POWER CUTOFF. 

const char *ssid = "SONOFF_HOME";
const char *password = "123456789";

const char MAIN_page[] PROGMEM = R"=====(
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
            background: #242424;
        }

        h1 {
            color: white;
        }

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
            cursor: pointer;
        }

        #em {
            color: rgb(255, 0, 0);
            visibility: hidden;

        }

        .intime {
            color: aquamarine;
            background: rgb(80, 80, 80);
            border: 3px rgb(255, 255, 0) solid;
            border-radius: 10px;
            height: 45px;
            width: 120px;
            font-size: 20px;
            padding: 5px;
        }

        #stopbt {
            display: none;
        }

        #h,
        #m,
        #s {
            height: 30px;
            width: 60px;
            border-radius: 30px;
            border: 2px yellow solid;
            background: rgb(80, 80, 80);
            color: aquamarine;
            outline: none;
        }

        #ts {
            color: aquamarine;
            visibility: hidden;
        }

        .tss {
            color: yellow;
        }

        label {
            color: #fff;
        }

        #timesetbt {
            margin-top: 10px;
        }

        @media screen and (min-width: 601px) {
            #button {
                width: 200px;
            }
        }

        @media screen and (max-width: 1000px) {
            #button {
                width: 250px;
            }
        }
    </style>
</head>

<body>
    <h1>HOME AUTOMATION SWITCH</h1>
    <button onclick="isend()" id="button">OFF</button>
    <h4 id="em">Massage</h4>
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
    <div id="timesetbt"><button onclick="setime()" id="setbt" class="intime">SET</button>
        <button onclick="stoptime()" id="stopbt" class="intime">STOP</button>
    </div>
    <h3 id="ts"><label class="tss" id="timed">00:00:00</label> Time Left To <label class="tss" id="times">OFF</label> The Device</h3>
    <script>
        var buts = "OFF";
        var buttonid = document.getElementById("button");
        var eid = document.getElementById("em");
        var intm;
        var stat;
        var timel;
        var h = document.getElementById("h");
        var m = document.getElementById("m");
        var s = document.getElementById("s");
        var timed = document.getElementById("timed");
        var times = document.getElementById("times");
        gettime()
        function setime() {
            document.getElementById("ts").style.visibility = "visible";
            document.getElementById("setbt").style.display = "none";
            document.getElementById("stopbt").style.display = "inline-block";
            timel = Number(h.value) * 60 * 60 + Number(m.value) * 60;
            stat = Number(s.value);
            serversendvalue = "timset?timevalue=" + String(stat) + ":" + String(timel);
            var xhttp = new XMLHttpRequest();
            xhttp.timeout = 1000;
            xhttp.ontimeout = function () {
                eid.style.visibility = "visible";
                eid.innerText = "";
                eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
            }
            xhttp.open("GET", serversendvalue, true);
            xhttp.send();

            if (stat == 1) {
                times.innerText = "ON";
            } else {
                times.innerText = "OFF";
            }

            intm = setInterval(function () { startime(); }, 1000);
        }
        function startime() {
            hoh = timel / (60 * 60);
            hhh = String(Math.floor(hoh));
            mom = (hoh - hhh) * 60;
            mmm = String(Math.floor(mom));
            sss = String(Math.round((mom - mmm) * 60));
            if (hhh.length == 1) {
                hhh = "0" + hhh;
            }
            if (mmm.length == 1) {
                mmm = "0" + mmm;
            }
            if (sss.length == 1) {
                sss = "0" + sss;
            }
            timed.innerText = hhh + ":" + mmm + ":" + sss;
            if (timel <= 0) {
                document.getElementById("ts").style.visibility = "hidden";
                document.getElementById("stopbt").style.display = "none";
                document.getElementById("setbt").style.display = "inline-block";
                if (stat == 0) {
                    var xhttp = new XMLHttpRequest();
                    xhttp.timeout = 1000;
                    xhttp.ontimeout = function () {
                        eid.style.visibility = "visible";
                        eid.innerText = "";
                        eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
                    }
                    xhttp.open("GET", "input?ls=OFF", true);
                    xhttp.send();
                } else if (stat == 1) {
                    var xhttp = new XMLHttpRequest();
                    xhttp.timeout = 1000;
                    xhttp.ontimeout = function () {
                        eid.style.visibility = "visible";
                        eid.innerText = "";
                        eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
                    }
                    xhttp.open("GET", "input?ls=ON", true);
                    xhttp.send();
                }
                clearInterval(intm);
            }
            timel--;
            if (timel < 0) {
                timel = 0;
            }
        }
        function stoptime() {
            clearInterval(intm);
            document.getElementById("ts").style.visibility = "hidden";
            document.getElementById("stopbt").style.display = "none";
            document.getElementById("setbt").style.display = "inline-block";
            serversendvalue = "timset?timevalue=0:0";
            var xhttp = new XMLHttpRequest();
            xhttp.timeout = 1000;
            xhttp.ontimeout = function () {
                eid.style.visibility = "visible";
                eid.innerText = "";
                eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
            }
            xhttp.open("GET", serversendvalue, true);
            xhttp.send();
        }
        function isend() {
            if (buttonid.innerText == "OFF") {
                var xhttp = new XMLHttpRequest();
                xhttp.timeout = 1000;
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        buttonid.innerText = "ON";
                        buttonid.style.color = "#0f0";
                        buttonid.style.border = "6px #0f0 solid";
                        eid.style.visibility = "hidden";
                    }
                }
                xhttp.ontimeout = function () {
                    eid.style.visibility = "visible";
                    eid.innerText = "";
                    eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
                }
                xhttp.open("GET", "input?ls=ON", true);
                xhttp.send();
            } else if (buttonid.innerText == "ON") {
                var xhttp = new XMLHttpRequest();
                xhttp.timeout = 1000;
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        buttonid.innerText = "OFF";
                        buttonid.style.color = "#f00";
                        buttonid.style.border = "6px #f00 solid";
                        eid.style.visibility = "hidden";
                    }
                }
                xhttp.ontimeout = function () {
                    eid.style.visibility = "visible";
                    eid.innerText = "";
                    eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
                }
                xhttp.open("GET", "input?ls=OFF", true);
                xhttp.send();
            }
        }
        setInterval(function () {
            getData();
        }, 500);
        function getData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var dt = this.responseText;
                    if (dt == "ON") {
                        buttonid.innerText = "ON";
                        buttonid.style.color = "#0f0";
                        buttonid.style.border = "6px #0f0 solid";
                        eid.style.visibility = "hidden";
                    } else if (dt == "OFF") {
                        buttonid.innerText = "OFF";
                        buttonid.style.color = "#f00";
                        buttonid.style.border = "6px #f00 solid";
                        eid.style.visibility = "hidden";
                    }
                }
            }
            xhttp.open("GET", "info", true);
            xhttp.send();
        }
        function gettime() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var dt = JSON.parse(this.response);
                    if (dt.time > 0) {
                        timel = dt.time;
                        stat = dt.status;
                        document.getElementById("ts").style.visibility = "visible";
                        document.getElementById("setbt").style.display = "none";
                        document.getElementById("stopbt").style.display = "inline-block";
                        if (stat == 1) {
                            times.innerText = "ON";
                        } else {
                            times.innerText = "OFF";
                        }
                        intm = setInterval(function () { startime(); }, 1000);
                    }
                }
            }
            xhttp.open("GET", "timeinfo", true);
            xhttp.send();
        }
        setInterval(function () {
            getre();
        }, 30000);
        function getre() {
            location.reload();
        }
    </script>
</body>

</html>
)=====";


String relayState = "OFF";

const int button = 0;
const int relay = 12;
const int led = 2;

int cbs;
int lbs;
unsigned long previousTime = 0;
int timestat=0;
int timestatus=0;

void handle_OnConnect(){
    String s = MAIN_page;
    server.send(200, "text/html", s);
}

void handle_info(){
    server.send(200, "text/plane", relayState);
}
void handle_timeinfo(){
    String jsondata = "{\"time\":"+String(timestat)+",\"status\":"+String(timestatus)+"}";
    server.send(200, "text/json", jsondata);
}

void handle_input(){
    String ls = "OFF";
    relayState = server.arg("ls"); 
    if (relayState == "ON")
    {
        digitalWrite(relay, HIGH);
        digitalWrite(led, HIGH);
        EEPROM.write(0, 1);
        EEPROM.commit();

    }else if(relayState == "OFF")
    {
        digitalWrite(relay, LOW);
        digitalWrite(led, LOW);
        EEPROM.write(0, 0);
        EEPROM.commit();

    }
    server.send(200); 
}

void handle_timset(){
    String timevalue = "0:0";
    String timevaluearg = server.arg("timevalue");
    timestat =  (timevaluearg.substring(2,-1)).toInt();
    timestatus = (timevaluearg.substring(0,1)).toInt();
    server.send(200);
}

void handle_NotFound(){
    server.send(404, "text/plain", "Not found");
}

void setup()
{
    pinMode(button,INPUT_PULLUP);
    pinMode(relay, OUTPUT);
    pinMode(led, OUTPUT);
    digitalWrite(relay, LOW);
    digitalWrite(led, LOW);
    cbs = digitalRead(button);

    EEPROM.begin(1);
    WiFi.softAP(ssid, password);
    delay(100);  

    server.on("/", handle_OnConnect);
    server.on("/input", handle_input);
    server.on("/info", handle_info);
    server.on("/timset", handle_timset);
    server.on("/timeinfo", handle_timeinfo);
    server.onNotFound(handle_NotFound);
    server.begin();

    int value = EEPROM.read(0);
    if (value==1){
          relayState="ON";
          digitalWrite(relay, HIGH);
          digitalWrite(led, HIGH);
      }
  
}

void loop(void)
{
    server.handleClient();
    unsigned long currentTime = millis();

    lbs  = cbs;      
    cbs = digitalRead(button); 
    delay(50);
    if(lbs == HIGH && cbs == LOW) {
        if (relayState=="ON"){
            relayState="OFF";
            digitalWrite(relay, LOW);
            digitalWrite(led, LOW);
            EEPROM.write(0, 0);
            EEPROM.commit();


        }else if (relayState=="OFF")
        {
            relayState="ON";
            digitalWrite(relay, HIGH);
            digitalWrite(led, HIGH);
            EEPROM.write(0, 1);
            EEPROM.commit();

   
        }
    }
    if ( currentTime - previousTime >= 1000) {
      if (timestat>0){
        timestat--;
        previousTime = currentTime;
        if (timestat==0){
          delay(900);
          if (timestatus==1){
            relayState="ON";
            digitalWrite(relay, HIGH);
            digitalWrite(led, HIGH);
            EEPROM.write(0, 1);
            EEPROM.commit();
          }else if (timestatus==0){
            relayState="OFF";
            digitalWrite(relay, LOW);
            digitalWrite(led, LOW);
            EEPROM.write(0, 0);
            EEPROM.commit();
          }

        }
      }
  }
}
