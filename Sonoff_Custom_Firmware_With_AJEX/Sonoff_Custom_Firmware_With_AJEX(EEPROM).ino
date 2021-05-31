#include <ESP8266WiFi.h>           
#include <ESP8266WebServer.h>   
#include <EEPROM.h>
   
ESP8266WebServer server(80); 

//change ssid and password according to you and upload 
//after that connect you phone/pc from this wifi
//and search "home.local"
// IN THIS PROGRAM YOU WILL GET EEPROM SUPPORT WHICH SAVE YOUR APPLINCE SO THAT YOU WILL NOT LOSE APPLINCE STATE AFTER POWER CUTOFF. 

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
            margin: 100px auto;
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
            margin: 10px;
            margin-top: 90px;
            cursor: pointer;
        }
        #em {
            color: rgb(255, 0, 0);
        }
        @media screen and (min-width: 601px) {
            #button {
                width: 250px;
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
    <h1>SONOFF WIFI SWITCH</h1>
    <button onclick="isend()" id="button">OFF</button>
    <h4 id="em"></h4>
    <script>
        var buts = "OFF";
        var buttonid = document.getElementById("button");
        var eid = document.getElementById("em");
        function isend() {
            if (buttonid.innerText == "OFF") {
                var xhttp = new XMLHttpRequest();
                xhttp.timeout = 1000;
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        buttonid.innerText = "ON";
                        buttonid.style.color = "#0f0";
                        buttonid.style.border = "6px #0f0 solid";
                        eid.innerText = "";
                    } 
                }
                xhttp.ontimeout = function() {
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
                        eid.innerText = "";
                    } 
                }
                xhttp.ontimeout = function() {
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
                        eid.innerText = "";
                    } else if (dt == "OFF") {
                        buttonid.innerText = "OFF";
                        buttonid.style.color = "#f00";
                        buttonid.style.border = "6px #f00 solid";
                        eid.innerText = "";
                    }
                }
            }
            xhttp.open("GET", "info", true);
            xhttp.send();
        }
    </script>
</body>
</html>
)=====";


String relayState = "OFF";

const int button = 0;
const int relay = 12;
const int led = 13;

int cbs;
int lbs;

void handle_OnConnect()
{
    String s = MAIN_page;
    server.send(200, "text/html", s);
}

void handle_info(){
    server.send(200, "text/plane", relayState);
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

void handle_NotFound()
{
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
}
