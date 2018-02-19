//Creato da Giuseppe Tamanini
//20-02-2018
//con licenza CC BY-SA
 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

unsigned long ttime;
boolean s_time = true;
const char* ssid = "retewifi";
const char* password = "password";
 
int ledPin = 5;
WiFiServer server(80);
IPAddress ip(192, 168, 1, 69); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network

void setup() {
  Serial.begin(9600);
  delay(10);
 
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  //Serial.print(F("Setting static ip to : "));
  //Serial.println(ip);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  if (s_time==true) {
    ttime=millis();
    s_time=false;
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  //Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  // Return the response
  if (millis()-ttime>=120000) {
    client.println("send");
    delay(1);
  }
}
