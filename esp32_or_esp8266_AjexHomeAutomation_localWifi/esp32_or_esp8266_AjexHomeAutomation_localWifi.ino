//#include <ESP8266WiFi.h>           //////////////////////////////////////////////////////////////////////////////////////////////
//#include <ESP8266WebServer.h>      //////////////////////////////////////////////////////////////////////////////////////////////

#include <WiFi.h>
#include <WebServer.h>

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
            margin: 2px;
            cursor: pointer;
        }

        #em {
            color: rgb(255, 0, 0);
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
        }, 100);

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

const char *ssid = "Home Control";
const char *password = "123456789";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//ESP8266WebServer server(80);        /////////////////////////////////////////////////////////////////////////////////////////////////////

WebServer server(80);

String relayState = "OFF";

//const int button = 0;
//const int relay = 12;
//const int led = 13;

const int button = 4;
const int relay = 2;
const int led = 5;

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
    }else if(relayState == "OFF")
    {
        digitalWrite(relay, LOW);
        digitalWrite(led, LOW);
    }
    server.send(200, "text/plane", relayState); 
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

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server.on("/", handle_OnConnect);
    server.on("/input", handle_input);
    server.on("/info", handle_info);
    server.onNotFound(handle_NotFound);
    server.begin();

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
        }else if (relayState=="OFF")
        {
            relayState="ON";
            digitalWrite(relay, HIGH);
            digitalWrite(led, HIGH);
        }
    }
}
