
/*  Prints "Hello from ESP8266" on the webserver
 *  simply enter your Wifi Settings and proceed
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// input wifi details here
const char *ssid = "*****wifiname to connect to*****";
const char *password = "****wifi passwrod *****";

//what to call the wemos on your wifi network
const char *wifiHostName = "D1Wemos210"; 

//time to reboot the wemos, currently midnight
int resetHour=00;
int resetMin=00;

//set static ip address on wifi-make sure not used already-maybe make fixed in router
IPAddress staticIP(192,168,0,210);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
// get mac from serial print and input here-sometimes used in routers to fix ip address-else delete
byte mac[] = {0xEC, 0xFA, 0xBC, 0xA7, 0xAF, 0xEC};

//light led as 2 
const int led = 2;

//set webserver and time server 
ESP8266WebServer server(80);
WiFiUDP ntpUDP;

//set up A0 as input pin and converting calc
  // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  //float voltage = sensorValue * (3.3 / 1023.0) * r1/r2;
    float voltage = sensorValue / 29.92;
 
//setup time server locale here
// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP, "ntp.is.co.za", 7200);
// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


//reboot function
void(* resetFunc)  (void) = 0;   // declare reset function at address 0


//webpage reset used to manually reset wemos
void resetComb() {
  String messageReset = "Resetting the Wifi...\n";
  messageReset += "Please check with 192.168.0.210/ in a few seconds.";
  server.send(200, "text/plain", messageReset);
  WiFi.disconnect();
      for (int j = 0; j<25; j++) {
        digitalWrite(led, LOW);
        delay(50);
        digitalWrite(led, HIGH);
        delay(50);
      } 
resetFunc();
}

//formats time for human reading on webpage and check for midnight-then reboots wemos-useful if ISP resets network
void timeCheck() {
  String stringHour=timeClient.getFormattedTime().substring(0,2);
  String stringMin=timeClient.getFormattedTime().substring(3,5);
  String stringSec=timeClient.getFormattedTime().substring(6,8);
  int intHour=stringHour.toInt();
  int intMin=stringMin.toInt();
  int intSec=stringSec.toInt();
    if (intHour == resetHour && intMin == resetMin) {
      delay(61000);
      resetComb();   
    }
}

//root of webpage triggered or refreshe-handle-text send-time send-volts reading send
void handleRoot() {
  String messageRoot = "Hello from D1Wemos210!\n";
  messageRoot += "D1Wemos & Wifi will reset at midnight.\n";
  messageRoot += "Use 192.168.0.210/reset to reset D1Wemos & WiFi manually.\n\n";
  messageRoot += "My time is: ";
  messageRoot += timeClient.getFormattedTime()+ "\n\n";
  messageRoot += "The battery Volt reading is: ";
  messageRoot += voltage;
  server.send(200, "text/plain", messageRoot);
    for (int i = 0; i<5; i++) {
      digitalWrite(led, LOW);
      delay(200);
      digitalWrite(led, HIGH);
      delay(200);
    }
}

//if by change any other unknown webpage used
void handleNotFound(){
  String messageDead = "Nothing to see here.\n";
  messageDead += "Please check with 192.168.0.210/";
  server.send(404, "text/plain", messageDead);
}

//starts webserver-timeserver-serialprint info
void setup(void){
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  
  WiFi.config(staticIP, gateway, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  Serial.printf("Connected, mac address: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  WiFi.hostname(wifiHostName);
  Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());
  
  server.on("/", handleRoot);
  server.on("/reset", resetComb);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
      
  timeClient.begin();
  Serial.println("TIME server started");

}

//checks time for midnight and catches web refreshes-every 2 seconds
void loop(void){
  timeClient.update();
  
  server.handleClient();
  
  sensorValue = analogRead(A0);
  voltage = sensorValue / 29.92;
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(2000);

  //Serial.println(timeClient.getFormattedTime());
    Serial.println(sensorValue);
  timeCheck();
}
