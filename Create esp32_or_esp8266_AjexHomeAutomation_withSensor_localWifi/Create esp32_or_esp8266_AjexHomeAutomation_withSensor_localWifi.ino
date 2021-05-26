// for esp8266 //
//#include <ESP8266WiFi.h>           //////////////////////////////////////////////////////////////////////////////////////////////
//#include <ESP8266WebServer.h>      //////////////////////////////////////////////////////////////////////////////////////////////
//ESP8266WebServer server(80);       //////////////////////////////////////////////////////////////////////////////////////////////

// for esp32 //
#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);

#include "DHT.h"

////////////////////////////////////////////////////////variables to change////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *ssid = "Home Control";
const char *password = "123456789";
// IP Address : http://192.168.1.1

#define DHTPIN 15 
const int button = 4;
const int relay = 2;
const int led = 5;
const int button2 = 18;
const int relay2 = 19;
const int led2 = 21;

//////////////////////////////////////////////////////////////////////////don't change if you know programming/////////////////////////////////////////////////////////////////////////////

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
            margin: 50px auto;
            text-align: center;
            cursor: crosshair;
            background: #242424;
        }

        h1 {
            color: white;
        }

        .buton {
            background-color: rgb(80, 80, 80);
            border: 6px #f00 solid;
            outline: none;
            color: #f00;
            padding: 30px 60px;
            border-radius: 90px;
            font-size: 30px;
            font-weight: bold;
            margin: 10px;
            cursor: pointer;
        }

        #em {
            color: rgb(255, 0, 0);
        }

        table{
            text-align: center;
            color: #ff0;
            width: 100%;
            margin-top: 60px;
        }

        @media screen and (min-width: 601px) {
            .buton {
                width: 250px;
            }
        }

        @media screen and (max-width: 1000px) {
            .buton {
                width: 250px;
            }
        }
    </style>
</head>
<body>

    <h1>HOME AUTOMATION SWITCH</h1>
    <button onclick="isend()" id="button" class="buton">OFF</button>
    <button onclick="isend2()" id="button2" class="buton">OFF</button><br>
    <h4 id="em"></h4>
    <table>
        <tr>
            <th>Temperatur</th>
            <th>Humidity</th>
        </tr>
        <tr>
            <th id="temp"><h2 name="t">00 <label for="t">&#8451;</label></h2></th>
            <th id="humi"><h2 name="h">00 <label for="h">%</label></h2></th>
        </tr>
    </table>

    <script>
        var buts = "OFF";
        var buttonid = document.getElementById("button");
        var buttonid2 = document.getElementById("button2");
        var eid = document.getElementById("em");
        var tempid = document.getElementById("temp")
        var humiid = document.getElementById("humi")

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

        function isend2() {
            if (buttonid2.innerText == "OFF") {
                var xhttp = new XMLHttpRequest();
                xhttp.timeout = 1000;
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        buttonid2.innerText = "ON";
                        buttonid2.style.color = "#0f0";
                        buttonid2.style.border = "6px #0f0 solid";
                        eid.innerText = "";
                    } 
                }
                xhttp.ontimeout = function() {
                  eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
                }
                xhttp.open("GET", "input2?ls2=ON", true);
                xhttp.send();
            } else if (buttonid2.innerText == "ON") {
                var xhttp = new XMLHttpRequest();
                xhttp.timeout = 1000;
                xhttp.onreadystatechange = function () {
                    if (this.readyState == 4 && this.status == 200) {
                        buttonid2.innerText = "OFF";
                        buttonid2.style.color = "#f00";
                        buttonid2.style.border = "6px #f00 solid";
                        eid.innerText = "";
                    } 
                }
                xhttp.ontimeout = function() {
                  eid.innerText = "Something Is Wrong ( status: " + String(this.status) + " )";
                }
                xhttp.open("GET", "input2?ls2=OFF", true);
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
                    var dt = JSON.parse(this.response);
                     tempid.innerHTML = '<h2 name="t">'+String(dt.tempos)+' '+'<label for="t">&#8451;</label></h2>'
                     humiid.innerHTML = '<h2 name="h">'+String(dt.humios)+' '+'<label for="h">%</label></h2>'
                    if (dt.sos == "ON") {
                        buttonid.innerText = "ON";
                        buttonid.style.color = "#0f0";
                        buttonid.style.border = "6px #0f0 solid";
                        eid.innerText = "";
                    } else if (dt.sos == "OFF") {
                        buttonid.innerText = "OFF";
                        buttonid.style.color = "#f00";
                        buttonid.style.border = "6px #f00 solid";
                        eid.innerText = "";
                    }
                    if (dt.sos2 == "ON") {
                        buttonid2.innerText = "ON";
                        buttonid2.style.color = "#0f0";
                        buttonid2.style.border = "6px #0f0 solid";
                        eid.innerText = "";
                    } else if (dt.sos2 == "OFF") {
                        buttonid2.innerText = "OFF";
                        buttonid2.style.color = "#f00";
                        buttonid2.style.border = "6px #f00 solid";
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


IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

String relayState = "OFF";
String relayState2 = "OFF";

int cbs;
int lbs;
int cbs2;
int lbs2;

float temp;
int humi;
 
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

void handle_OnConnect()
{
    String s = MAIN_page;
    server.send(200, "text/html", s);
}

void handle_info(){
    String jsondata = "{\"tempos\":"+String(temp)+",\"humios\":"+String(humi)+",\"sos\":\"" + relayState + "\",\"sos2\":\"" + relayState2 + "\"}";
    server.send(200, "text/json", jsondata);
}

void handle_input(){
    String ls = "OFF";
    relayState = server.arg("ls"); 
    if (relayState == "ON")
    {
        digitalWrite(relay, HIGH);
        digitalWrite(led, HIGH);
    }else if(relayState == "OFF")
    {
        digitalWrite(relay, LOW);
        digitalWrite(led, LOW);
    }
    server.send(200, "text/plane", relayState); 
}

void handle_input2(){
    String ls2 = "OFF";
    relayState2 = server.arg("ls2"); 
    if (relayState2 == "ON")
    {
        digitalWrite(relay2, HIGH);
        digitalWrite(led2, HIGH);
    }else if(relayState2 == "OFF")
    {
        digitalWrite(relay2, LOW);
        digitalWrite(led2, LOW);
    }
    server.send(200, "text/plane", relayState2); 
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
    pinMode(button2,INPUT_PULLUP);
    pinMode(relay2, OUTPUT);
    pinMode(led2, OUTPUT);
    digitalWrite(relay, LOW);
    digitalWrite(led, LOW);
    cbs = digitalRead(button);
    digitalWrite(relay2, LOW);
    digitalWrite(led2, LOW);
    cbs = digitalRead(button2);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", handle_OnConnect);
    server.on("/input", handle_input);
    server.on("/input2", handle_input2);
    server.on("/info", handle_info);
    server.onNotFound(handle_NotFound);
    server.begin();
    dht.begin();

    Serial.begin(9600);
    Serial.println();
    Serial.print("SSID: ");
    Serial.print(ssid);
    Serial.println();
    Serial.print("PASSWORD: ");
    Serial.print(password);
    Serial.println();
    Serial.print("IP Address: ");
    Serial.print(WiFi.softAPIP());
    Serial.println();
}

void loop(void)
{
    server.handleClient();
    
    temp = dht.readTemperature();
    humi = dht.readHumidity();
  
    lbs  = cbs;      
    cbs = digitalRead(button); 
    delay(50);
    if(lbs == HIGH && cbs == LOW) {
        if (relayState=="ON"){
            relayState="OFF";
            digitalWrite(relay, LOW);
            digitalWrite(led, LOW);
        }else if (relayState=="OFF")
        {
            relayState="ON";
            digitalWrite(relay, HIGH);
            digitalWrite(led, HIGH);
        }
    }

    lbs2  = cbs2;      
    cbs2 = digitalRead(button2); 
    delay(50);
    if(lbs2 == HIGH && cbs2 == LOW) {
        if (relayState2=="ON"){
            relayState2="OFF";
            digitalWrite(relay2, LOW);
            digitalWrite(led2, LOW);
        }else if (relayState2=="OFF")
        {
            relayState2="ON";
            digitalWrite(relay2, HIGH);
            digitalWrite(led2, HIGH);
        }
    }
}

