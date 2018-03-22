/*

This code has released under license CC BY- SA
last release created 22/03/2018
by Giuseppe Tamanini

MEGA                           LCD
22.............................LCD_D0
23.............................LCD_D1
24.............................LCD_D2
25.............................LCD_D3
26.............................LCD_D4
27.............................LCD_D5
28.............................LCD_D6
29.............................LCD_D7
A0.............................LCD_RD
A1.............................LCD_WR
A2.............................LCD_RS
A3.............................LCD_CS
A4.............................LCD_RST
50.............................SD_D0
51.............................SD_D1
52.............................SD_SCK
53.............................SD_SS

30.............................WS2812 DIN 
*/

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SHT31.h>
#include <WS2812.h>
RTC_DS1307 RTC;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// *** SPFD5408 change -- End
#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 23   // can be a digital pin
#define XP 22    // can be a digital pin

// Original values
//#define TS_MINX 150
//#define TS_MINY 120
//#define TS_MAXX 920
//#define TS_MAXY 940

// Calibrate values
#define TS_MINX 180
#define TS_MINY 120
#define TS_MAXX 860
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define SD_CS 53     // Set the chip select line to whatever you use (10 doesnt conflict with the library)

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 60
#define PENRADIUS 3
int holdsel;
int currentsel;
char buffer[20];
byte holdminute=99;
byte holdsecond=99;
byte holdumid=0;
int umid;
byte holdtemp=0;
int temp;
String StringS[]={"Dom","Lun","Mar","Mer","Gio","Ven","Sab"};
String StringM[]={"Gen","Feb","Mar","Apr","Mag","Giu","Lug","Ago","Set","Ott","Nov","Dic"};
int gm[]={31,28,31,30,31,30,31,31,30,31,30,31};
int capomese[]={11,12,1,2,3,4,5,6,7,8,9,10};
int patta[]={24,5,16,27,8,19,0,11,22,3,14,25,6,17,28,9,20,1,12,23,4,15,26,7,18,29,10,21,2,13,24,5,16,27,8,19,0,11,22,3,14};
int faselunare;
#define outputPin 30  // Digital output pin command WS2812
#define LEDCount 60   // Number of LEDs WS2812
WS2812 LED(LEDCount);
cRGB value;
boolean mh=true;
String DataOra;
int hh=0;
int hho=0;
int mm=0;;
int ss=0;
int r[60];
int g[60];
int b[60];
int yyyy;
int yyyp;
int mmmm;
int mmmp;
int dddd;
int dddp;
int hhhh;
int nnnn;
int ssss;
int spdt=0; //spostadata
String inData1; //corrente1
String inData2; //corrente2
String inData3; //corrente1
String inData4; //corrente2
String inChar;
int nPas=0;
long sPas=0;
int Pas;
int holdPas=0;
int nPpr=0;
long sPpr=0;
int Ppr;
int holdPpr=0;
float Te;
float holdTe=0;
int Pe;
int aslm=725;
int holdPe=0;
int holdPae=0;
int holdNPuo=0;
int nPuo=3;
int nmp=1;
String Puo[3]={"In diminuzione","Stazionaria","In aumento"};
File myFile;
String nomefile;
boolean savetrue=false;
int holdminsave=-1;
String riga;
String o;
String m;
String c;
int on;
int mn;
int cn;
int cg[24];
long cst;
int cs[7];
long cmt;
int cm[30];
float scala;
int ngraph=0;
String gus[30];
int sgs;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  Wire.begin();
  RTC.begin();
  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  LED.setOutput(outputPin); // Digital Pin 23
  inputString.reserve(20);
  // following line sets the RTC to the date & time this sketch was compiled
  //RTC.adjust(DateTime(2016,2,7,4,59,0));

  tft.reset();

  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }
  
  tft.begin(identifier); // SDFP5408

  tft.setRotation(1); // Need for the Mega, please changed for your choice or rotation initial

  tft.fillScreen(BLACK);
  currentsel=1;
  holdsel=1;

  pinMode(13, OUTPUT);
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("failed!"));
    return;
  }
  bmpDraw("ICO01.BMP", 0, 0);
  bmpDraw("ICO02_A.BMP", 0, 60);
  bmpDraw("ICO03_A.BMP", 0, 120);
  bmpDraw("ICO04_A.BMP", 0, 180);
  Intro();
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
  TSPoint p = ts.getPoint();
  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    Serial.print(p.x);
    Serial.print(" ");
    Serial.println(p.y);
    
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
    // *** SPFD5408 change -- End
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.width(), 0);
    Serial.print(p.x);
    Serial.print(" ");
    Serial.println(p.y);
    if (p.y < BOXSIZE) {
       if (p.x< BOXSIZE) { 
         bmpDraw("ICO01.BMP", 0, 0);
         switch (holdsel) {
           case 2:
             bmpDraw("ICO02_A.BMP", 0, 60);
             break;
           case 3:
             bmpDraw("ICO03_A.BMP", 0, 120);
             break;
           case 4:
             bmpDraw("ICO04_A.BMP", 0, 180);
             break;
         }
         currentsel=1;
         holdminute=99;
         holdsecond=99;
         holdtemp=-99;
         holdumid=-1;
         holdPas=-1;
         holdPpr=-1;
         holdTe=-99;
         holdPe=0;
         Intro();
         ngraph=0;
         spdt=0;
       } else if (p.x < BOXSIZE*2) {
         bmpDraw("ICO02.BMP", 0, 60);
         switch (holdsel) {
           case 1:
             bmpDraw("ICO01_A.BMP", 0, 0);
             break;
           case 3:
             bmpDraw("ICO03_A.BMP", 0, 120);
             break;
         }
         switch (ngraph) {
           case 0:
             GraficoCorrenteGiorno();
             break;
           case 1:
             GraficoCorrenteSettimana();
             break;
           case 2:
             GraficoCorrenteMese();
             break;
         }
         ngraph=ngraph+1;
         if (ngraph>3) ngraph=3;
         currentsel=2;
       } else if (p.x < BOXSIZE*3) {
         bmpDraw("ICO03.BMP", 0, 120);
         switch (holdsel) {
           case 1:
             bmpDraw("ICO01_A.BMP", 0, 0);
             break;
           case 2:
             bmpDraw("ICO02_A.BMP", 0, 60);
             break;
           case 4:
             bmpDraw("ICO04_A.BMP", 0, 180);
             break;
         }
         currentsel=3;
       } else if (p.x < BOXSIZE*4 && holdsel==1) {
         bmpDraw("ICO04.BMP", 0, 180);
         switch (holdsel) {
           case 1:
             bmpDraw("ICO01_A.BMP", 0, 0);
             break;
           case 2:
             bmpDraw("ICO02_A.BMP", 0, 60);
             break;
           case 3:
             bmpDraw("ICO03_A.BMP", 0, 120);
             break;
         }
         currentsel=4;
         ModModifica();
       }
       holdsel=currentsel;
    }
    //Serial.println(currentsel);
    if (currentsel==4) {
    if (p.x > 120 && p.x < 180) {
      if (p.y > 70 && p.y < 130) {
        SalvaData();
      }
      else if (p.y > 130 && p.y < 190) {
        DiminuisciData();
      }
      else if (p.y > 190 && p.y < 250) {
        AumentaData();
      }
      else if (p.y > 250 && p.y < 310) {
        if (spdt<5) spdt=spdt+1;
        ModificaData();
      }
    }
    }
    delay(250);
  }
    DateTime now = RTC.now();
    ss=now.second(); 
    if (holdsecond!=ss) {
      mm=now.minute();
      hh=now.hour();
      hho=hh;
      if (hh>11) {
        hho=hh-12;
      }
      hho=hho*5+mm/12;
      holdsecond=ss;
      AccendiLed();
    }
    if (ss==30) ScriviDati();
    if (currentsel==1 && holdminute!=mm) {
      
      nmp=nmp+1;
      sgs=now.dayOfWeek();
      dddd=now.day();
      mmmm=now.month();
      yyyy=now.year();
      faselunare=dddd+capomese[mmmm-1]+patta[yyyy-2000];
      if (mmmm<3) faselunare=dddd+capomese[mmmm-1]+patta[yyyy-2000-1];
      if (faselunare>=30) faselunare=faselunare-30;
      if (faselunare>=30) faselunare=faselunare-30;
      tft.setTextSize(2);
      tft.setTextColor(RED);
      tft.fillRect(60,200,200,20,BLACK);
      if (faselunare>15) {
        tft.setCursor(110,200);
        tft.print("Luna calante");
      } else if (faselunare<15 && faselunare!=0) {
        tft.setCursor(84,200);
        tft.print("Luna crescente");
      } else if (faselunare==15) {
        tft.setCursor(132,200);
        tft.print("Luna piena");
      } else {
        tft.setCursor(132,200);
        tft.print("Luna nuova");
      }
      sprintf(buffer,"LUNA%02d.BMP",faselunare);
      bmpDraw(buffer,260,180);
      holdminute=mm;
      float mmfloat=mm;
      if (mmfloat/2==int(mmfloat/2) && holdminsave!=mm) {
        sprintf(buffer,"A%02d%02d%02d.CSV",yyyy-2000,mmmm,dddd);
        nomefile=buffer;
        myFile = SD.open(nomefile,FILE_WRITE);
        int mPas=sPas/nPas;
        sprintf(buffer,"%02d%02d%04d",hh,mm,mPas);
        myFile.println(buffer);
        sPas=0;
        nPas=0;
        myFile.close();
        sprintf(buffer,"P%02d%02d%02d.CSV",yyyy-2000,mmmm,dddd);
        nomefile=buffer;
        myFile = SD.open(nomefile,FILE_WRITE);
        int mPpr=sPpr/nPpr;
        sprintf(buffer,"%02d%02d%04d",hh,mm,mPpr);
        myFile.println(buffer);
        sPpr=0;
        nPpr=0;
        savetrue=false;
        holdminsave=mm;
        myFile.close();
      }      
      ScriviOra();
    }
}

void Intro() {
    tft.fillRect(60,0,260,240,BLACK);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(60,20);
    tft.print("Umid.interna:    %");
    tft.setCursor(60,40);
    tft.print("Temp.interna:     C");
    tft.setCursor(60,60);
    tft.print("Temp.esterna:       C");
    tft.setCursor(60,80);
    tft.print("Press.atm.:       hPa");
    tft.setCursor(60,140);
    tft.print("Pot. Ass.:      W");
    tft.setCursor(60,160);
    tft.print("Pot. Pro.:      W");
    bmpDraw("ANELLO.BMP",263,40);
    bmpDraw("ANELLO.BMP",287,60);
    tft.setCursor(60,100);
    tft.setTextColor(YELLOW);
    tft.fillRect(60,100,200,20,BLACK);
    tft.print(Puo[nPuo]);
}

void GraficoCorrenteGiorno() {
  dddp=dddd-1;
  mmmp=mmmm;
  yyyp=yyyy;
  if (dddp==0) {
    dddp=gm[mmmm-2];
    mmmp=mmmm-1;
    if (yyyy%4==0 && mmmp==2) dddp=dddp+1;
    if (mmmp==0) {
      mmmp=12;
      yyyp=yyyy-1;
    }
  }
  sprintf(buffer,"A%02d%02d%02d.CSV",yyyp-2000,mmmp,dddp);
  nomefile=buffer;
  Serial.println(nomefile);
  myFile = SD.open(nomefile);
  if (myFile) {
    tft.fillRect(60,0,260,240,BLACK);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(60,0);
    tft.print("POTENZA ASSORBITA");
    tft.setCursor(60,20);
    sprintf(buffer,"%02d",dddp);
    tft.print(buffer);
    tft.print(" ");
    tft.print(StringM[mmmp-1]);
    tft.print(" ");
    sprintf(buffer,"%04d",yyyp);
    tft.print(buffer);
    for (int i=0; i < 24; i++) {
      cg[i]=0;
    }
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      riga = myFile.readStringUntil('\n');
      o=riga.substring(0,2);
      on=o.toInt();
      m=riga.substring(2,4);
      mn=m.toInt();
      c=riga.substring(4,8);
      cn=c.toInt();
      cg[on]=cg[on]+cn;
    }
    for (int i=0; i < 24; i++) {
      cg[i]=cg[i]/30;
    }
    int cmax=0;
    int cmin=cg[0];
    for (int i=0; i < 24; i++) {
      if (cg[i]>cmax) cmax=cg[i];
      if (cg[i]<cmin) cmin=cg[i];
    }
    float fcmax=cmax;
    scala=fcmax/160;
    //Serial.println(scala);
    for (int i=0; i < 24; i++) {
      tft.fillRect(73+i*10,220-cg[i]/scala,9,cg[i]/scala,RED);
      tft.setTextSize(1);
      tft.setTextColor(WHITE);
      float ifloat=i;
      if (ifloat/2==int(ifloat/2)) {
        tft.setCursor(72+i*10,222);
      } else {
        tft.setCursor(72+i*10,230);
      }
      tft.print(i+1);
      //Serial.print(cg[i]);
      //Serial.print(',');
    }
    tft.drawLine(65,220-cmax/scala,65,220,WHITE);
    tft.drawLine(65,220,320,220,WHITE);
    tft.drawLine(319,220-cmax/scala,319,220,WHITE);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.drawLine(65,220-cmax/scala,320,220-cmax/scala,WHITE);
    tft.setCursor(120,205-cmax/scala);
    tft.print("Max:");
    tft.print(cmax);
    tft.print(" W");
    tft.drawLine(65,220-cmin/scala,320,220-cmin/scala,WHITE);
    if (cmin>cmax/2) {
      tft.setCursor(120,222-cmin/scala);
    } else {
      tft.setCursor(120,205-cmin/scala);
    }
    tft.print("Min:");
    tft.print(cmin);
    tft.print(" W");
    //Serial.println();
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening file: ");
    Serial.println(nomefile);

  }
}

void GraficoCorrenteSettimana() {

  tft.fillRect(60,0,260,240,BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(65,0);
  tft.print("  ATTENDI UN ATTIMO");
  for (int i=0; i < 7; i++) {
    cs[i]=0;
  }
  for (int gs=0; gs<7; gs++) {
    dddp=dddd-(gs+1);
    mmmp=mmmm;
    yyyp=yyyy;
    if (dddp<=0) {
      dddp=gm[mmmm-2]-gs+dddd-1;
      mmmp=mmmm-1;
      if (yyyy%4==0 && mmmp==2) dddp=dddp+1;
      if (mmmp==0) {
        mmmp=12;
        yyyp=yyyy-1;
      }
    }
    sprintf(buffer,"%02d/%02d",dddp,mmmp);
    gus[gs]=buffer;
    sprintf(buffer,"A%02d%02d%02d.CSV",yyyp-2000,mmmp,dddp);
    nomefile=buffer;
    myFile = SD.open(nomefile);
    cst=0;
    if (myFile) {
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        riga = myFile.readStringUntil('\n');
        o=riga.substring(0,2);
        on=o.toInt();
        m=riga.substring(2,4);
        mn=m.toInt();
        c=riga.substring(4,8);
        cn=c.toInt();
        cst=cst+cn;
      }
      Serial.println(nomefile);
      Serial.println(cst/30);
      cs[gs]=cst/30;
      myFile.close();
      } else {
        // if the file didn't open, print an error:
        Serial.print("error opening file: ");
        Serial.println(nomefile);
      }
    }
    int cmax=0;
    int cmin=cs[0];
    for (int i=0; i < 7; i++) {
      if (cs[i]>cmax) cmax=cs[i];
      if (cs[i]<cmin) cmin=cs[i];
    }
      float lcmax=cmax;
      scala=lcmax/160;
      tft.fillRect(60,0,260,240,BLACK);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.setCursor(65,0);
      tft.print("POTENZA ASSORBITA");
      tft.setTextSize(2);
      tft.setCursor(65,20);
      tft.print("dal ");
      tft.print(gus[6]);
      tft.print(" al ");
      tft.print(gus[0]);

    for (int i=0; i<7; i++) {
      tft.drawLine(65,220-cmax/scala,65,220,WHITE);
      tft.drawLine(65,220,320,220,WHITE);
      tft.drawLine(319,220-cmax/scala,319,220,WHITE);
      tft.fillRect(75+i*34,220-cs[6-i]/scala,30,cs[6-i]/scala,RED);
      tft.setTextSize(1);
      tft.setTextColor(WHITE);
      float ifloat=i;
      if (ifloat/2==int(ifloat/2)) {
        tft.setCursor(74+i*34,222);
      } else {
        tft.setCursor(74+i*34,230);
      }
      tft.print(gus[6-i]);
      //Serial.print(cs[6-i]);
      //Serial.print(',');
    }
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.drawLine(65,220-cmax/scala,320,220-cmax/scala,WHITE);
    tft.setCursor(120,205-cmax/scala);
    tft.print("Max:");
    tft.print(cmax);
    tft.print(" W");
    tft.drawLine(65,220-cmin/scala,320,220-cmin/scala,WHITE);
    if (cmin>cmax/2) {
      tft.setCursor(120,222-cmin/scala);
    } else {
      tft.setCursor(120,205-cmin/scala);
    }
    tft.print("Min:");
    tft.print(cmin);
    tft.print(" W");
    //Serial.println();
    // close the file:*/

}

void GraficoCorrenteMese() {

  tft.fillRect(60,0,260,240,BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(65,0);
  tft.print("  ATTENDI UN MINUTO");
  for (int i=0; i<30; i++) {
    cm[i]=0;
  }
  for (int gs=0; gs<30; gs++) {
    dddp=dddd-(gs+1);
    mmmp=mmmm;
    yyyp=yyyy;
    if (dddp<=0) {
      dddp=gm[mmmm-2]-gs+dddd-1;
      mmmp=mmmm-1;
      if (yyyy%4==0 && mmmp==2) dddp=dddp+1;
      if (mmmp==0) {
        mmmp=12;
        yyyp=yyyy-1;
      }
    }
    sprintf(buffer,"%02d/%02d",dddp,mmmp);
    gus[gs]=buffer;
    sprintf(buffer,"A%02d%02d%02d.CSV",yyyp-2000,mmmp,dddp);
    nomefile=buffer;
    cmt=0;
    myFile = SD.open(nomefile);
    if (myFile) {
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        riga = myFile.readStringUntil('\n');
        o=riga.substring(0,2);
        on=o.toInt();
        m=riga.substring(2,4);
        mn=m.toInt();
        c=riga.substring(4,8);
        cn=c.toInt();
        cmt=cmt+cn;
      }
      Serial.println(nomefile);
      Serial.println(cmt/30);
      cm[gs]=cmt/30;
      myFile.close();
      } else {
        // if the file didn't open, print an error:
        Serial.print("error opening file: ");
        Serial.println(nomefile);
      }
    }
    int cmax=0;
    int cmin=cm[0];

    for (int i=0; i<30; i++) {
      if (cm[i]>cmax) cmax=cm[i];
      if (cm[i]<cmin) cmin=cm[i];
    }
    float lcmax=cmax;
    scala=lcmax/160;

      tft.fillRect(60,0,260,240,BLACK);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.setCursor(65,0);
      tft.print("POTENZA ASSORBITA");
      tft.setTextSize(2);
      tft.setCursor(65,20);
      tft.print("dal ");
      tft.print(gus[29]);
      tft.print(" al ");
      tft.print(gus[0]);

    for (int i=0; i<30; i++) {
      tft.drawLine(65,220-cmax/scala,65,220,WHITE);
      tft.drawLine(65,220,320,220,WHITE);
      tft.drawLine(319,220-cmax/scala,319,220,WHITE);
      tft.fillRect(73+i*8,220-cm[29-i]/scala,7,cm[29-i]/scala,RED);
      tft.setTextSize(1);
      tft.setTextColor(WHITE);
      float ifloat=i;
      if (ifloat/2==int(ifloat/2)) {
        tft.setCursor(71+i*8,222);
      } else {
        tft.setCursor(71+i*8,230);
      }
      String d;
      d=gus[29-i].substring(0,2);
      tft.print(d);
    }
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.drawLine(65,220-cmax/scala,320,220-cmax/scala,WHITE);
    tft.setCursor(120,205-cmax/scala);
    tft.print("Max:");
    tft.print(cmax);
    tft.print(" W");
    tft.drawLine(65,220-cmin/scala,320,220-cmin/scala,WHITE);
    if (cmin>cmax/2) {
      tft.setCursor(120,222-cmin/scala);
    } else {
      tft.setCursor(120,205-cmin/scala);
    }
    tft.print("Min:");
    tft.print(cmin);
    tft.print(" W");
    //Serial.println();
    // close the file:*/

}

void SalvaData() {
  RTC.adjust(DateTime(yyyy,mmmm,dddd,hhhh,nnnn,ssss));
  holdsel=1;
  currentsel=1;
  spdt=0;
  bmpDraw("ICO01.BMP", 0, 0);
  bmpDraw("ICO04_A.BMP", 0, 180);
  holdminute=99;
  holdsecond=99;
  holdtemp=99;
  holdumid=99;
  Intro();
}

void ModModifica() {
  tft.fillRect(60,0,260,240,BLACK);
  bmpDraw("ICOS.BMP", 70, 120);
  bmpDraw("ICOM.BMP", 130, 120);
  bmpDraw("ICOP.BMP", 190, 120);
  bmpDraw("ICOA.BMP", 250, 120);
  DateTime now = RTC.now();
  yyyy=now.year();
  mmmm=now.month();
  dddd=now.day();
  hhhh=now.hour();
  nnnn=now.minute();
  ssss=now.second();
  ModificaData();
}

void AccendiLed() {
  for (int i=0; i < 60; i++) {
    r[i]=0;
    g[i]=15;
    b[i]=15;
  }
  r[mm]=0;
  g[mm]=255;
  b[mm]=0;
  r[hho]=255;
  g[hho]=0;
  b[hho]=0;
  if (mm==hho && mh==true) {
    r[mm]=0;
    g[mm]=255;
    b[mm]=0;
    mh=false;
  }
  else if (mm==hho && mh==false) {
    mh=true;
  }
  r[ss]=0;
  g[ss]=0;
  b[ss]=255;
  if (ss==hho) {
    r[ss]=127;
    g[ss]=0;
    b[ss]=127;
  }
  if (ss==mm) {
    r[ss]=0;
    g[ss]=127;
    b[ss]=127;
  }
  for (int i=0; i<60; i++) {
    value.b = b[59-i]; value.g = g[59-i]; value.r = r[59-i]; // RGB Value -> Blue
    LED.set_crgb_at(i, value); // Set value at LED found at index 0
    LED.sync(); // Sends the value to the LED
    
  }
}

void ScriviOra() {
      DataOra="";
      DataOra=StringS[sgs];
      DataOra=DataOra+" ";
      sprintf(buffer,  "%02d", dddd);
      DataOra=DataOra+buffer;
      DataOra=DataOra+" ";
      DataOra=DataOra+StringM[mmmm-1];
      DataOra=DataOra+" ";
      sprintf(buffer,  "%d", yyyy);
      DataOra=DataOra+buffer;
      DataOra=DataOra+" ";
      sprintf(buffer,  "%02d:%02d", hh, mm);
      DataOra=DataOra+buffer;
      tft.fillRect(60,0,260,20,BLACK);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(60,0);
      tft.print(DataOra);
      Serial.println(DataOra);
}
      
void ScriviDati() {
  temp = sht31.readTemperature();
  umid = sht31.readHumidity();
  if (! isnan(temp)) {
    if (holdumid!=umid) {
      tft.fillRect(230,20,32,20,BLACK);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(230,20);
      tft.print(umid);
      holdumid=umid;
    }
  }
  if (! isnan(umid)) {
    if (holdtemp!=temp) {
      tft.fillRect(230,40,32,20,BLACK);
      tft.setTextSize(2);
      tft.setCursor(230,40);
      tft.print(temp);
      holdtemp=temp;
    }
  }
}

void ScriviDatiRicevuti() {

   if (stringComplete) {
      Serial.print("Dati ricevuti: ");
      Serial.println(inputString);
      Serial.print("1: ");
      Serial.println(inputString. substring(0, 1));
      if (inputString.substring(0,1)=="A") {
        inData1=inputString.substring(1,3);
        Pas=inData1.toInt()*60;
        Serial.print("Pas=");
        Serial.println(Pas);
        sPas=sPas+Pas;
        nPas=nPas+1;
        inData2=inputString.substring(4,9);
        Te=inData2.toFloat();
        Serial.print("Te=");
        Serial.println(Te);
        inData3=inputString.substring(10,14);
        Pe=inData3.toInt();
        Pe=Pe+aslm/29.27*Pe/(Te+273.15);
        Serial.print("Pe=");
        Serial.println(Pe);
        if (holdPae==0) holdPae=Pe;
     if (currentsel==1) {
        tft.setTextSize(2);
        tft.setCursor(185,140);
        if(Pas!=holdPas) {
          tft.fillRect(187,140,60,20,BLACK);
          sprintf(buffer,"%5d", Pas);
          tft.print(buffer);
          holdPas=Pas;
        }
        tft.setTextColor(WHITE);
        tft.setCursor(185,160);
        if(Ppr!=holdPpr) {
          tft.fillRect(185,160,60,20,BLACK);
          sprintf(buffer,"%5d", Ppr);
          tft.print(buffer);
          holdPpr=Ppr;
        }
        tft.setCursor(230,60);
        if (Te!=holdTe) {
          tft.fillRect(230,60,50,20,BLACK);
          dtostrf(Te,2,1,buffer);
          tft.print(buffer);
          holdTe=Te;
        }
        tft.setCursor(220,80);
        if (Pe!=holdPe) {
          tft.fillRect(220,80,50,20,BLACK);
          sprintf(buffer,"%4d",Pe);
          tft.print(buffer);
          holdPe=Pe;
        }
      }

      if (nmp>180) {
        int vpu3ore=Pe-holdPae;
        if (vpu3ore>1) {
          nPuo=2;
        }
        else if (vpu3ore<-1) {
          nPuo=0;
        }
        else if (vpu3ore<=1 && vpu3ore>=0) { 
          nPuo=1;
        }
        else if (vpu3ore>=-1 && vpu3ore<=0) { 
          nPuo=1;
        }
        nmp=1;
        holdPae=Pe;
        if (currentsel==1) {
          if (nPuo!=holdNPuo) {
            tft.setCursor(60,100);
            tft.setTextColor(YELLOW);
            tft.fillRect(60,100,260,20,BLACK);
            tft.print(Puo[nPuo]);
            holdNPuo=nPuo;
          }
        }
      }
    }
    inputString = "";
    stringComplete = false;
    if (holdPae==0) holdPae=Pe;
   }
}

void ModificaData() {
  tft.fillRect(70,100,240,24,BLACK);
  tft.setTextSize(3);
  switch (spdt) {
    case 0:
      tft.setCursor(70,100);
      tft.print("Anno: ");
      tft.setCursor(220,100);
      tft.print(yyyy);
      break;
    case 1:
      tft.setCursor(70,100);
      tft.print("Mese: ");
      tft.setCursor(220,100);
      tft.print(mmmm);
      break;
    case 2:
      tft.setCursor(70,100);
      tft.print("Giorno: ");
      tft.setCursor(220,100);
      tft.print(dddd);
      break;
    case 3:
      tft.setCursor(70,100);
      tft.print("Ore: ");
      tft.setCursor(220,100);
      tft.print(hhhh);
      break;
    case 4:
      tft.setCursor(70,100);
      tft.print("Minuti: ");
      tft.setCursor(220,100);
      tft.print(nnnn);
      break;
    case 5:
      tft.setCursor(70,100);
      tft.print("Secondi: ");
      tft.setCursor(220,100);
      tft.print(ssss);
  }    
}

void AumentaData() {
  tft.fillRect(220,100,80,24,BLACK);
  tft.setTextSize(3);
  tft.setCursor(220,100);
  switch (spdt) {
    case 0:
      yyyy=yyyy+1;
      tft.print(yyyy);
      break;
    case 1:
      if (mmmm<12) mmmm=mmmm+1;
      tft.print(mmmm);
      break;
    case 2:
      {
        int vdddd=dddd;
        int gmc=gm[mmmm-1];
        if (dddd<gmc) dddd=dddd+1;
        if (yyyy%4==0 && mmmm==2 && vdddd==28) dddd=dddd+1;
        tft.print(dddd);
      }
      break;
    case 3:
      if (hhhh<23) hhhh=hhhh+1;
      tft.print(hhhh);
      break;
    case 4:
      if (nnnn<59) nnnn=nnnn+1;
      tft.print(nnnn);
      break;
    case 5:
      if (ssss<59) ssss=ssss+1;
      tft.print(ssss);
   }
}

void DiminuisciData() {
  tft.fillRect(220,100,80,24,BLACK);
  tft.setTextSize(3);
  tft.setCursor(220,100);
  switch (spdt) {
    case 0:
      if (yyyy>2016) yyyy=yyyy-1;
      tft.print(yyyy);
      break;
    case 1:
      if (mmmm>1) mmmm=mmmm-1;
      tft.print(mmmm);
      break;
    case 2:
      if (dddd>1) dddd=dddd-1;
      tft.print(dddd);
      break;
    case 3:
      if (hhhh>0) hhhh=hhhh-1;
      tft.print(hhhh);
      break;
    case 4:
      if (nnnn>0) nnnn=nnnn-1;
      tft.print(nnnn);
      break;
    case 5:
      if (ssss>0) ssss=ssss-1;
      tft.print(ssss);
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      ScriviDatiRicevuti();
    }
  }
}

void drawBorde() {

  // Draw a border

  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
  
}

#define BUFFPIXEL 20

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
