//Creato da Giuseppe Tamanini 18-03-2018
//con licenza CC BY-SA

#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "NomeWiFi";           // SSID wifi del tuo router
char pass[] = "PasswordWiFi";      // password del wifi del tuo router

int LightOn = 16; // pin a cui è collegato il pin di uscita dell'UA741 che è alto (5V) quando la fotoresistenza è illuminata
int Led = 12; // pin del led che si accende quando il pin 12 è alto
boolean LightState; // stato del pin 5
int cont = 0; // contataore dei lampeggi
int oldcont = 0; // vecchio valore di cont
char Stringacont[3]; // stringa inviata al server del conteggio
unsigned long ttime; // tempo do controllo

Adafruit_BMP085 bmp; // sensore pressione atmosferica
int temp; // temperatura esterna
int pressa; // pressione atmosferica
char Stringatemp[4]; // stringa inviata al server della temperatura
char Stringapressa[5]; // stringa inviata al server della temperatura

IPAddress server(192,168,1,80);  // indirizzo Ip del Server
WiFiClient client;

void setup() {
  Serial.begin(115200);               // Solo per il debug

  pinMode(LightOn, INPUT); // imposta il pin 5 in Ingresso
  pinMode(Led, OUTPUT); // imposta il pin 12 in Uscita

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

  if (!bmp.begin()) { // controlla il collegamento del sensore
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  
  ttime=millis(); // pone ttime uguale a millis()
    
}

void loop () {
  
  // legge lo stato del pin 12 e lo memorizza in LighState
  LightState = digitalRead(LightOn);
  if (LightState) { // se è Alto cioè il led del contatatore è acceso
    delay(50); // aspetta 50 ms
    LightState = digitalRead(LightOn); // rilegge lo stato del pin 12
    if (LightState==true & cont==oldcont) { // se il pin 12 è ancora alto
      cont = cont + 1; // incrementa il conteggio cont
      Serial.println(cont); // visualizza il valore sulla seriale
      Serial.println(millis() - ttime);
      digitalWrite(Led, HIGH); // accende il led collegato al pin 12
    }
  } else { // se il pin 5 è basso
    digitalWrite(Led, LOW); // spegne il led
    oldcont = cont; // memorizza cont in oldcont
  }
  
  if (millis()-ttime > 60000) { // Se è passato un minuto
    ttime=millis(); // pone ttime uguale a millis() facendo ripartire il conteggio del tempo 
    client.connect(server, 80);   // si connette al Server
    Serial.println(".");
    sprintf(Stringacont, "A%02d", cont); // formatta il numero di conteggi con su due cifre antecedendoci una A (Es. A12)
    client.print(Stringacont);
    // Serial.println(Stringacont);
    temp = bmp.readTemperature();   // legge
    sprintf(Stringatemp, "T %02d", temp); // formatta il valore con su due cifre antecedendoci una T e un segno negativo o uno spazio (Es. T 18, T-04 se negativa)
    client.print(Stringatemp); // legge la temperatura dal modulo BMP e la memorizza in temp
   // Serial.println(Stringatemp);
    pressa =  bmp.readPressure(); // legge la pressione atmosferica el amemorizza in pressa
    sprintf(Stringapressa, "P%04d", pressa); // formatta il valore con su quattro cifre antecedendoci una P (Es. P1014)
    client.print(Stringapressa);
    client.println("\r");  // invia il messaggio al Server
    Serial.println(Stringapressa);
    String answer = client.readStringUntil('\r');   // Riceve il messaggio dal Server
    // Serial.println("Dal Server: " + answer);
    client.flush();
    delay(100); // attendi 100 millisecondi
    cont = 0; // azzera il conteggio dei lampeggi
  }
}
