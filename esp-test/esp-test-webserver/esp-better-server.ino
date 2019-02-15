/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   ToDo:
        - Add data from motor and PIXEL to update on server
        - Test

*/
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define PIXPIN 5
#define NUM_PIX 12
#define RELAY 4
#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif


const char *ssid = STASSID;
const char *password = STAPSK;


ESP8266WebServer server(80);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIX, PIXPIN, NEO_GRB + NEO_KHZ800);


void handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  // make sure relay is off
  digitalWrite(RELAY, LOW);

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <button><a href=\"/PixOn\">Pixel On</a></button>\
    <button><a href=\"/MotorOn\">Start Motor</a></button>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>", hr, min % 60, sec % 60 );
  server.send(200, "text/html", temp);
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  // setup relay
  pinMode(RELAY, OUTPUT);
  digtialWrite(RELAY, LOW);

  // start with PIXEL off
  pixels.begin()
  pixels.show()

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println("Not Connected");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/PixOn", flashLED);
  server.on("/MotorOn", moveMotor);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}


void loop(void) {
  server.handleClient();
  MDNS.update();
}


void flashLED() {
    char temp[400];
    int cycles = 0;
    String color; // wire and display what color is on

    // make sure motor is off
    digitalWrite(RELAY, LOW);

    // setup html first
    snprintf(temp, 400,
             "<html>\
             <head>\
                <meta http-equiv='refresh' content='5'/>\
                <title>ESP8266 Demo</title>\
                <style>\
                    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
                </style>\
                </head>\
             <body>\
                <h1>Hello from ESP8266!</h1>\
                <button><a href=\"/PixOn\">Pixel On</a></button>\
                <button><a href=\"/MotorOn\">Start Motor</a></button>\
                <p>PIXEL CYCLES: %d</p>\
                <p>CURRENT PIXEL COLOR: %s</p>\
                <img src=\"/test.svg\" />\
             </body>\
             </html>", hr, min % 60, sec % 60 );

    server.send(200, "text/html", temp);
    // endless cycles when href is active
    for (int j = 0;; ++j) {
        // PIX On
        for (int i = 0; i < NUM_PIX; ++i) {
            pixels.setPixelColor(i, pixels.Color(255,155,155));
            pixels.show();
            color = "Blue";
        }
        delay(500);
        // PIX Off
        for (int x = 0; x < NUM_PIX; ++x) {
            pixels.setPixelColor(x, pixels.Color(0,0,0));
            pixels.show();
            color = "Off";
        }
        delay(500);  // flash every half second
        cycles++;   // increase cycle value after every off/on
    }
}


void drawGraph() {
   String out = "";
   char temp[100];

   // Make sure pixel is off
    for (int i = 0; i < NUM_PIX; ++i) {
        pixels.setPixelColor(x, pixels.Color(0,0,0));
        pixels.show();
   }

  // make sure motor is off
  digitalWrite(RELAY, HIGH);

  // delay(600); delay just in case the off sequence doesnt work
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  // get values for graph
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}


/* move the motor and update sever */
void moveMotor() {
    char temp[400];
    int cycles = 0;
    String motor_state; // wire and display what color is on
    // setup html first
    snprintf(temp, 400,
             "<html>\
             <head>\
                <meta http-equiv='refresh' content='5'/>\
                <title>ESP8266 Demo</title>\
                <style>\
                    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
                </style>\
                </head>\
             <body>\
                <h1>Hello from ESP8266!</h1>\
                <button><a href=\"/PixOn\">Pixel On</a></button>\
                <button><a href=\"/MotorOn\">Start Motor</a></button>\
                <p>MOTOR STATE: %s</p>\
                <img src=\"/test.svg\" />\
             </body>\
             </html>", motor_state );

    server.send(200, "text/html", temp);
    digitalWrite(RELAY, HIGH);  // turn on motor
}
