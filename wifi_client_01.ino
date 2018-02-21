
#include <SPI.h>
#include <ESP8266WiFi.h>

byte ledPin = 4; // pin a cui è collegato il led
char ssid[] = "TP-LINK_GT";           // SSID wifi del tuo router
char pass[] = "toninat20261956";      // password del wifi del tuo router

unsigned long askTimer = 0;

IPAddress server(192,168,1,80);       // indirizzo Ip del Server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // Solo per il debug
  WiFi.begin(ssid, pass);             // Si connette con il router in wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());    // Visualizza i parametri della rete
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  
  pinMode(ledPin, OUTPUT);
}

void loop () {
  client.connect(server, 80);   // Si connette al Server
  digitalWrite(ledPin, LOW);    // Spegne il led solo per vedere se parte la connessione
  Serial.println(".");
  client.println("Ciao Server! Stai dormendo?\r");  // Invia il messaggio al Server
  String answer = client.readStringUntil('\r');   // Riceve il messaggio dal Server
  Serial.println("Dal Server: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH); // Accende il led
  delay(2000); // attendi 2 secondi
}
