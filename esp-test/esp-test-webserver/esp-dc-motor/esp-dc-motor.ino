#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
const char* ssid = "";
const char* password = "";
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
const int LEDPin = LED_BUILTIN;
// motor not working, mayabe digitalWrite() issue
const int motor_pin = 4;


void setup(void){
  //the HTML of the web page
  page = "<h1>Whasup Mane!!!!</h1><br><h2>Motor Control!!</h2><p><a href=\"LEDOn\"><button>ON</button></a>&nbsp;<a href=\"LEDOff\"><button>OFF</button></a></p>";
  //make the LED pin output and initially turned off
  pinMode(LEDPin, OUTPUT);
  pinMode(motor_pin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  digitalWrite(motor_pin, LOW);
   
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
  });
  // throw in some serial.print()'s
  server.on("/LEDOn", [](){
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, LOW);
    digitalWrite(motor_pin, HIGH);
    delay(1000);
  });
  server.on("/LEDOff", [](){
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, HIGH);
    digitalWrite(motor_pin, LOW);
    delay(1000);
  });
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(void){
  // eskedit
  server.handleClient();
}
