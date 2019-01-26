#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
// Replace with your network credentials
const char* ssid = "";
const char* password = "";
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
int LEDPin = LED_BUILTIN;
int BTNpin = 4;
int BTNstate = 0;

void setup(void){
  //the HTML of the web page
  page = "<h1>Whasup Mane!!!!</h1><p><a href=\"LEDOn\"><button>ON</button></a>&nbsp;<a href=\"LEDOff\"><button>OFF</button></a></p>";
  //make the LED pin output and initially turned off
  pinMode(LEDPin, OUTPUT);
  pinMode(BTNpin, INPUT_PULLUP);
  digitalWrite(LEDPin, HIGH);
   
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", [](){
    server.send(200, "text/html", page);
    delay(1000);
  });
  server.on("/LEDOn", [](){
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, LOW);
    delay(1000);
  });
  server.on("/LEDOff", [](){
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, HIGH);
    delay(1000); 
  });
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(void){
  // eskedit
  server.handleClient();
  if (digitalRead(BTNpin) == LOW) {
    Serial.println("Nothing Happened...");
    digitalWrite(LEDPin, LOW);
  } else {
    Serial.println("Button Pressed");
    digitalWrite(LEDPin, HIGH);
  }
  
}
