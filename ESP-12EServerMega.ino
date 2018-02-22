//Creato da Giuseppe Tamanini
//20-02-2018
//con licenza CC BY-SA

#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "Nomeretewifi";               // SSID della wifi del tuo router
char pass[] = "passwordretewifi";          // password del wifi del tuo router
WiFiServer server(80);                    

IPAddress ip(192, 168, 1, 80);            // indirizzo IP da assegnareal Server
IPAddress gateway(192,168,1,1);           // Gateway del router
IPAddress subnet(255,255,255,0);          // Subnet Mask del tuo router
void setup() {
  
  Serial.begin(115200);                   // solo per il debug
  WiFi.config(ip, gateway, subnet);       // configura l'IP statico
  WiFi.begin(ssid, pass);                 // connette la wifi al router
  while (WiFi.status() != WL_CONNECTED) { // attende che la connesione sia avvenuta
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // Avvia il Server
  /*Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // Stampa i parametri della connessione
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());*/
  
}

void loop () {
  WiFiClient client = server.available(); // attende un client
  if (client) {
    if (client.connected()) { // se un client è connesso
      //Serial.println(".");
      String request = client.readStringUntil('\r');    // Riceve il messaggio dal client
      //Serial.print("Dal Client: ");
      Serial.println(request); // manda sulla seriale (dell'Arduino) il messaggio ricevuto
      client.flush(); // attende finché non sono stati inviati tutti i caratteri in uscita
      client.print(request);
      client.println("\r"); // rimanda il messaggio al client
    }
    client.stop();  // termina la connessione con il client
  }
}
