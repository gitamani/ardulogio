//Creato da Giuseppe Tamanini
20-02-2018
//con licenza CC BY-SA

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Inizializza la connessione Wifi al tuo router
const char* ssid = "retewifi"; // Nome della tua rete Wifi
const char* password = "password"; // Password della tua rete Wifi

IPAddress ip(192, 168, 1, 79); // indirizzo IP assegnato all'ESP
IPAddress gateway(192, 168, 1, 1); // gateway del tuo router
#define USE_SERIAL Serial

int LightOn = 5; // pin a cui è collegato il pin di uscita dell'UA741 che è alto (5V) quando la fotoresistenza è illuminata
int Led = 12; // pin del led che si accende quando il pin 12 è alto
boolean LightState; // stato del pin 5
int cont = 0; // contataore dei lampeggi
int oldcont = 0; // vecchio valore di cont
char Stringacont[3];

unsigned long ttime;

boolean stato = true;

void setup() {
  Serial.begin(9600);
  delay(10);
 
  pinMode(LightOn, INPUT); // imposta il pin 5 in Ingresso
  pinMode(Led, OUTPUT); // imposta il pin 12 in Uscita
 
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
 
  // Connette alla tua rete WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connessione a ");
  Serial.println(ssid);
  IPAddress subnet(255, 255, 255, 0); // imposta la subnet mask della tua rete
  WiFi.config(ip, gateway, subnet); 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connessa");
 
  // Start the server
 
  // Stampa l'indirizzo IPs
  Serial.print("Stai usando questo URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}



void loop() {
  if (stato == true) {
    ttime=millis();
    stato = false;
  }

  // legge lo stato del pin 5 e lo memorizza in LighState
  LightState = digitalRead(LightOn);
  if (LightState) { // se è Alto
    delay(50); // aspetta 50 ms
    LightState = digitalRead(LightOn); // rilegge lo stato del pin 5
    if (LightState==true & cont==oldcont) { // se il pin 5 è alto e precedentemente non lo era
      cont = cont + 1; // incrementa cont
      Serial.println(cont); // visualizza il valore sulla seriale
      Serial.println(millis() - ttime);
      digitalWrite(Led, HIGH); // accende il led collegato al pin 12
    }
  } else { // se il pin 5 è basso
    digitalWrite(Led, LOW); // spegne il led
    oldcont = cont; // memorizza cont in oldcont
  }
  delay(50); // aspetta 50 ms
  if (millis() - ttime > 120000) {
    char Stringacont[3];
    sprintf(Stringacont, "%03d", cont); 
    Serial.println(Stringacont);
    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin("192.168.1.69", 80, Stringacont); //HTTP
    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    USE_SERIAL.print("httpCode");
    USE_SERIAL.print(httpCode);
    cont = 0;
    stato = true;
  }


}

void inviacont() {

}

