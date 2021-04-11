#include <ESP8266WiFi.h>

const char* ssid     = "Google Home";
const char* password = "333666999";
WiFiServer server(80);
String header;
int relayState = LOW;
const int button = 0;
const int relay = 12;
const int led = 13;
    
int lastButtonState;   
int currentButtonState;

void setup() {
  pinMode(button,INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(relay, LOW);
  digitalWrite(led, LOW);
  currentButtonState = digitalRead(button);
  WiFi.softAP(ssid, password);
  server.begin();
}

void loop(){
  WiFiClient client = server.available();  
  if (client) {                            
    String currentLine = "";               
    while (client.connected()) {          
      if (client.available()) {             
        char c = client.read();             
        header += c;
        if (c == '\n') {        
          if (currentLine.length() == 0) { 
            if (header.indexOf("GET /ON") >= 0) 
            {
              relayState = HIGH;
              digitalWrite(relay, HIGH);
              digitalWrite(led, HIGH);
            } 
            else if (header.indexOf("GET /OFF") >= 0) 
            {
              relayState = LOW;
              digitalWrite(relay, LOW);
              digitalWrite(led, LOW);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html{font-family: Helvetica;display: inline-block;margin: 100px auto; text-align: center; cursor: crosshair;}");
            client.println(".st{color: tomato;font-size: 30px;}");
            client.println(".button {background-color:#4CAF50;border: none;outline: none;color: white; padding: 30px 60px;border-radius: 50px;text-decoration: none;font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            client.println("<body><h1>HOME AUTOMATION SWITCH</h1>");
              
            if (relayState==LOW) {client.println("<p class=\"st\">Appliance is : OFF</p>");
              client.println("<p><a href=\"/ON\"><button class=\"button\">ON</button></a></p>");}
            else                   {
              client.println("<p class=\"st\">Appliance is : ON</p>");
              client.println("<p><a href=\"/OFF\"><button class=\"button button2\">OFF</button></a></p>");} 
            
            client.println("</body></html>");
            client.println();
          
            break;} 
           else {currentLine = "";}}
           else if (c != '\r') 
    {currentLine += c; }}}
    header = "";
    client.stop();}

      lastButtonState    = currentButtonState;      
  currentButtonState = digitalRead(button); 
  delay(50);
  if(lastButtonState == HIGH && currentButtonState == LOW) {
    relayState = !relayState;
    digitalWrite(relay, relayState);
    digitalWrite(led, relayState);
    
  }
}




    
