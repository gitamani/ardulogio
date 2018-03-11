#include <SPI.h>
#include <ESP8266WiFi.h>

byte ledPin = 5;
char ssid[] = "NomeWiFi";               // SSID della wifi del tuo router
char pass[] = "PasswordWiFi";          // password del wifi del tuo router
WiFiServer server(80);                    

IPAddress ip(192, 168, 1, 80);            // Indirizzo IP da assegnareal Server
IPAddress gateway(192,168,1,1);           // Gateway del router
IPAddress subnet(255,255,255,0);          // Subnet Mask del tuo router
void setup() {
  Serial.begin(115200);                   // Solo per il debug
  WiFi.config(ip, gateway, subnet);       // Configura l'IP statico
  WiFi.begin(ssid, pass);                 // Connette la wifi al router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // Avvia il Server
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // Stampa i parametri della connessione
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());
  pinMode(ledPin, OUTPUT);
}

void loop () {
  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      digitalWrite(ledPin, LOW);  // Spegne il led solo per vedere se parte la connessione
      Serial.println(".");
      String request = client.readStringUntil('\r');    // Riceve il messaggio dal Client
      Serial.print("Dal Client: ");
      Serial.println(request);
      client.flush();
      client.println("No Client! Sono sveglio!\r"); // Invia la risposta al Client
      digitalWrite(ledPin, HIGH);
    }
    client.stop();  // Termina la connessione con il Client
  }
}
