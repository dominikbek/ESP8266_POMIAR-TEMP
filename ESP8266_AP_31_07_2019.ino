#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#ifndef APSSID
#define APSSID "POMIAR_TEMP_1"
#define APPSK  "hasło"
#define Ni 10
#define LM35 A0
#define w 0.81
#define RED D7 
#define GREEN D6
#define BLUE D5
#endif

const char *ssid = APSSID;
const char *password = APPSK;
//String strona = "<h1>ESP 8266:</h1>";
//String strona1 = "<h1>-------</h1>";
ESP8266WebServer server(80);
int minuty = 0;
int sekundy = 0;
int godziny = 0;
int dni = 0;
unsigned long czaspracy;
float temp_max = -100;
float temp_min = 100;
int led1 = 0;
int led2 = 0;
int led3 =0;

class Pomiar {
  public:
  float srednia;
  float temp;
  float nap;
  int probki [Ni];
  int odchyl [Ni];
  float sredniaodchylst;
  float tempodchylst;
  float napodchylstand;
     float oblsr(){
        for(int i = 0; i < Ni; i++){
        probki[i]=analogRead(A0);
        delay(10);}
        for(int i = 0; i < Ni; i++){
        srednia+=probki[i];
        delay(10);
  }
  srednia /= Ni;
  nap = (srednia*(3.3/1023.0))*100*w;
  
  }
  float odchylst(){
      for(int i = 0; i < Ni; i++){
          probki[i]=analogRead(A0);
          delay(10);}
      for(int i = 0; i < Ni; i++){
         odchyl[i]=(probki[i]-srednia)*(probki[i]-srednia);
          delay(10);
    }
     for(int i = 0; i < Ni; i++){
        sredniaodchylst+=odchyl[i];
        delay(10);
    }
  sredniaodchylst /= Ni;
  sredniaodchylst= sqrt(sredniaodchylst);
  napodchylstand = (sredniaodchylst*(3.3/1023.0))*100*w;
  }};

class Przelaczanie {
  public:
      int stan = LOW;
      void przelacznik() { //metdoda - funkcja w klasie.
      if ( stan == LOW) {
        stan = HIGH;
      }
      else {
        stan = LOW;
      }
    }
};

int czas() {
  sekundy++;
  delay(1000);
  if (sekundy >= 59) {
    sekundy = 0;
    minuty++;
  }
  if (minuty >= 59) {
    minuty = 0;
    godziny++;
  }
  if (godziny >= 24) {
    godziny = 0;
    dni++;
  }
}

int dioda(){
  Pomiar led;
  led.oblsr();
    if(led.nap>20){
    digitalWrite(RED,HIGH);
  }
  else{
    digitalWrite(RED, LOW);
    }
    if(led.nap<20){
      digitalWrite(BLUE, HIGH);
    }
    else{
      digitalWrite(BLUE, LOW);
    }
    if(led.nap>18){
      digitalWrite(GREEN, HIGH);
    }
    if(led.nap<18){
      digitalWrite(GREEN, LOW);
    }
    if(led.nap>22){
      digitalWrite(GREEN, LOW);
    }
}
void handleRoot() {
  float nap;
  float napodchylstand;
  int moc;
  Pomiar temp;
  temp.oblsr();
  nap = temp.nap;
  if(nap>=temp_max){
    temp_max=nap;
  }
  if(nap<=temp_min){
    temp_min=nap;
  }
  server.send(200, "text/html", "<tr><th>temp.:</th><th>float(nap)</th></tr>");
  //"temp. max.: " + float(temp_max) + " oC " "temp. min.: " + float(temp_min) + " oC " + " czas pracy: " + int(dni) + " d " + int(godziny) + " g " + int(minuty) + " m  " + int(sekundy) + " s");
  }

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress localIp(192,168,0,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(localIp, gateway, subnet);
  Serial.print("AP IP address: ");
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  pinMode(LM35, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  }

void loop() {
    czaspracy = millis()/1000;
    if(czaspracy>=0){
    Przelaczanie glowne;
    glowne.przelacznik();
    if(glowne.stan==HIGH){
      czas();
      server.handleClient();
      dioda();
     }
    if(glowne.stan==LOW){
       
       }
  }}
