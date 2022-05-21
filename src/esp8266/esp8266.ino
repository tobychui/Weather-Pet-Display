#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

//Library required for HDSv2
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266WebServer.h>   // Include the WebServer library

//HMI Related
SoftwareSerial HMISerial(D1, D2); // RX, TX
bool debugMode = false;         //Enable debug mode

/*
 * HMI Element Definations 
 * 
 * time -> Time Display (00:00)
 * ampm -> am / pm text
 * date -> Current date, YYYY/MM/DD format 
 * dow -> Date of week, Monday to Sunday
 * 
 */
 
//WiFi Related
WiFiManager wifiManager;

//Properties of HDSv2 Devices
const String DeviceName = "ArOZ DSSP";     //The name of this IoT device
const int ListeningPort = 12110;           //The port where this IoT device listen
ESP8266WebServer server(ListeningPort);    //Create an Web Server on the listening port

//Clock Related
WiFiUDP UDPconn;
NTPClient ntpClient(UDPconn, "pool.ntp.org");
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

//Weather related
//We are getting weather info from https://opendata.cwb.gov.tw/api/v1/rest/datastore/O-A0001-001?Authorization=rdec-key-123-45678-011121314&limit=40&format=JSON&elementName=TEMP,HUMD&stationId=C0X100
const char* host = "opendata.cwb.gov.tw";
const char* stationId = "C0X100";
long cyclyCounter = 0;
DynamicJsonDocument doc(2048);
StaticJsonDocument<1024> filter;
String currentTemp = "N/A";
String currentHumd = "N/A";

//Background, messageBox & Clothing
/*
 *  Picture ID / Time
 *  30 (Early Morning): 5  - 8am
 *  31 (Morning)      : 9  - 12am
 *  32 (Afternoon)    : 13 - 15pm
 *  33 (Evening)      : 16 - 18pm
 *  34 (Night)        : 19 - 23pm
 *  35 (Late Night)   : 24 - 4am
 */

String currentBackgroundId = "0";


void setup() {
  // Start debug Serial and HMI Serial
  Serial.begin(115200);
  HMISerial.begin(9600);

  // Connect to Wi-Fi
  wifiManager.setClass("invert");
  wifiManager.autoConnect("ArOZ DSSP", "arozdssp");
  if (!debugMode){
    wifiManager.setDebugOutput(false);
  }

  //Start MDNS broadcast
  MDNS.setHostProbeResultCallback(hostProbeResult);
  if (!MDNS.begin(DeviceName) && debugMode) {
    Serial.println("Error setting up MDNS responder!");
  }

  //Advertise the port that you are using
  MDNS.addService("http", "tcp", ListeningPort);
  if (debugMode){
    Serial.println("mDNS responder started");
  }
  
  //Start WebServer
  delay(100);
  server.on("/", handle_index);
  server.on("/status", handle_status);
  server.on("/eps", handle_endpoints);
  server.on("/msg", handle_msg);
  server.on("/face", handle_face);
  
  server.begin();
  
  //Start NTP Client
  ntpClient.begin();
  ntpClient.setTimeOffset(28800);

  //Init UI Elements
  hideMessageBox();

  //Apply JSON filter
  filter["records"]["location"][0] = true;

  delay(1000);
  //Get weather information
  UpdateWeatherInfo();
}

//Dim background brightness, range 0 - 100 in string
void DimBackgroundBrightness(String brightness){
  sendCommand("dim=" + brightness);
}

//Update background by time (Current Hour)
void updateBackgroundByTime(int ch){
  String backgroundId = "35";
  if (ch >= 5 && ch <= 8){
    backgroundId = "30";
    SetClothesSet(0);
  }else if (ch >= 9 && ch <= 12){
    backgroundId = "31";
    SetClothesSet(1);
  }else if (ch >= 13 && ch <= 15){
    backgroundId = "32";
    SetClothesSet(2);
  }else if (ch >= 16 && ch <= 18){
    backgroundId = "33";
    SetClothesSet(3);
  }else if (ch >= 19 && ch <= 23){
    backgroundId = "34";
    SetClothesSet(4);
  }else if (ch >= 24 || ch <= 4){
    backgroundId = "35";
    SetClothesSet(5);
  }

  if (!currentBackgroundId.equals(backgroundId)){
       //Change of section of day, adjust brightness
       if (backgroundId.equals("30")){
          DimBackgroundBrightness("10");
       }else if (backgroundId.equals("31")){
          DimBackgroundBrightness("30");
       }else if (backgroundId.equals("32")){
          DimBackgroundBrightness("50");
       }else if (backgroundId.equals("33")){
          DimBackgroundBrightness("50");
       }else if (backgroundId.equals("34")){
          DimBackgroundBrightness("30");
       }else if (backgroundId.equals("35")){
          DimBackgroundBrightness("3");
       }
  }

  //Background need to change
  sendCommand("p3.pic=" + backgroundId);
  currentBackgroundId = backgroundId;
  
  
}

//Update the on screen display datetime
void updateTimeInfo(){
   ntpClient.update();
   time_t epochTime = ntpClient.getEpochTime();

   //Update time display
   String currentTime = timeZeroPad(ntpClient.getHours()) + ":" + timeZeroPad(ntpClient.getMinutes());
   sendCommand("time.txt=\"" + currentTime + "\"");

  //Update am/pm indicator
   String currentAfterNoon = "am";
   if (ntpClient.getHours() >=12){
      currentAfterNoon = "pm";
   }
   sendCommand("ampm.txt=\"" + currentAfterNoon + "\"");

   //Update background
   updateBackgroundByTime(ntpClient.getHours());
  
   //Updat the date display
   struct tm *ptm = gmtime ((time_t *)&epochTime); 
   String currentDate = String(ptm->tm_year+1900) + "/" + String(ptm->tm_mon+1) + "/" + String(ptm->tm_mday);
   
   //English
   //sendCommand("date.txt=\"" + currentDate + "\"");
   //Chinese
   SendDateTimeChinese(String(ptm->tm_year+1900), String(ptm->tm_mon+1), String(ptm->tm_mday));
  
  
   //Update day of week
   //English
   //String weekDay = weekDays[ntpClient.getDay()];
   //sendCommand("dow.txt=\"" + weekDay + "\"");

   //Chinese (big-5)
   SendWeekDayChinese(ntpClient.getDay());
}

void UpdateWeatherInfo(){
  WiFiClientSecure httpsClient;
  const int httpPort = 443;
  httpsClient.setInsecure();
  if (!httpsClient.connect(host, httpPort)) {
    //Fail to conenct
    if (debugMode){
      Serial.println("connection failed");
    }
    return;
  }

    //Create URI request
    String url = "/api/v1/rest/datastore/O-A0001-001?Authorization=rdec-key-123-45678-011121314&limit=40&format=JSON&elementName=TEMP,HUMD&stationId=";
    url += stationId;
    httpsClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    String line;
    String reply = "";
    while(httpsClient.available()){        
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      reply += line;
    }

    //Trim the first unwannted byte and the last
    int startTrimPos = reply.indexOf("{");
    int lastTrimPos = reply.lastIndexOf("}");
    reply = reply.substring(startTrimPos, lastTrimPos);

    //Serial.println(reply);
    deserializeJson(doc, reply);
    String temp = doc["records"]["location"][0]["weatherElement"][0]["elementValue"];
    String humd = doc["records"]["location"][0]["weatherElement"][1]["elementValue"];
    double humdPercentage = humd.toDouble() * 100;

    //Update the on-screen weather information
    sendCommand("temp.txt=\"" + temp + "\"");
    printFloatCommand("humi.txt=", humdPercentage, "");

    //Update global cache variable, force copying
    currentTemp = "" + temp;
    currentHumd = "" + humd;

}

void loop() {
  //Update Clock Display
  updateTimeInfo();
  
  //Update Messagebox display status
  updateMessageboxStatus();

  //Handle web requests
  server.handleClient();
  MDNS.update();

  //Update weather information
  if (cyclyCounter%1800 == 0){
    //Update weather info every 30 min +- 10min
    UpdateWeatherInfo();
    cyclyCounter = 0; //prevent overflow
  }

  cyclyCounter++;
  delay(100);
}
