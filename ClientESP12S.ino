//Creato da Giuseppe Tamanini
23-02-2018
//con licenza CC BY-SA

#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "nomeretewifi";           // SSID wifi del tuo router
char pass[] = "passwordretewifi";      // password del wifi del tuo router

int LightOn = 2; // pin a cui è collegato il pin di uscita dell'UA741 che è alto (5V) quando la fotoresistenza è illuminata
int Led = 12; // pin del led che si accende quando il pin 12 è alto
boolean LightState; // stato del pin 5
int cont = 0; // contataore dei lampeggi
int oldcont = 0; // vecchio valore di cont
char Stringacont[3]; // serve per formattare il valore da inviare al server 
unsigned long ttime; // memorizza il tempo che passa
boolean stato = true; // serve per far ripartire il conteggio
float ftime;

IPAddress server(192,168,1,80);       // indirizzo Ip del Server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // Solo per il debug

  pinMode(LightOn, INPUT); // imposta il pin in Ingresso
  pinMode(Led, OUTPUT); // imposta il pin in Uscita

  WiFi.begin(ssid, pass);             // Si connette con il router in wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  /*Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());    // Visualizza i parametri della rete
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());*/
  
  pinMode(Led, OUTPUT);
}

void loop () {
  
  if (stato == true) {  // se stato è vera fa ripartire il tempo
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
  
  if (millis()-ttime > 60000) { // se sono passati 60 secondi
    client.connect(server, 80);   // si connette al Server
    Serial.println("."); // invia un carattere . sulla seriale
    sprintf(Stringacont, "C%02d", cont); // formatta il valore su due cifre aggiungendo una C davanti (Es. C09)
    client.print(Stringacont);
    client.println("\r");  // Invia il valore al Server
    // Serial.println(Stringacont);
    String answer = client.readStringUntil('\r');   // riceve il valore di ritorno dal Server
    Serial.println("Dal Server: " + answer); // lo manda sulla seriale
    client.flush(); // attende finché non sono stati inviati tutti i caratteri in uscita
    delay(250); // attendi 250 millisecondi
    cont = 0; // azzera cont fa ripartire il conteggio
    stato = true; // pone vera stato per azzerrare il tempo
  }
}
