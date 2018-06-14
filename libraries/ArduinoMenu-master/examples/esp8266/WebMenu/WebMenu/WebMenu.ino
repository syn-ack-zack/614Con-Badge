/********************
Arduino generic menu system
XmlServer menu example
based on WebServer:
  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
  https://github.com/Links2004/arduinoWebSockets

Dec. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu on web browser served by esp8266 device (experimental)
output: ESP8266WebServer -> Web browser
input: ESP8266WebSocket <- Web browser
format: xml

this requires the data folder to be stored on esp8266 spiff
for development purposes some files are left external,
therefor requiring an external webserver to provide them (just for dev purposes)
i'm using nodejs http-server (https://www.npmjs.com/package/http-server)
to static serve content from the data folder. This allows me to quick change
the files without having to upload them to SPIFFS
also gateway ssid and password are stored on this code (bellow),
so don't forget to change it.
'dynamic' list of detected wi-fi and password request would require the new
added textField (also experimental).

*/
#include <menu.h>
#include <menuIO/esp8266Out.h>
#include <menuIO/xmlFmt.h>//to write a menu has html page
#include <menuIO/serialIn.h>
// #include <Streaming.h>
#include <streamFlow.h>
//#include <menuIO/jsFmt.h>//to send javascript thru web socket (live update)
#include <FS.h>
#include <Hash.h>
extern "C" {
  #include "user_interface.h"
}

using namespace Menu;

#define ANALOG_PIN 4

#ifndef MENU_SSID
  #error "need to define WiFi SSID here"
  #define MENU_SSID "r-site.net"
#endif
#ifndef MENU_PASS
  #error "need to define WiFi password here"
  #define MENU_PASS ""
#endif

const char* ssid = MENU_SSID;
const char* password = MENU_PASS;
const char* serverName="192.168.1.79";
#ifdef DEBUG
  // on debug mode I put aux files on external server to allow changes without SPIFF update
  // on this mode the browser MUST be instructed to accept cross domain files
  #define xslt "http://neurux:8080/menu.xslt"
#else
  #define xslt "/menu.xslt"
  #define endl "\r\n"
  menuOut& operator<<(menuOut&o, long unsigned int i) {
    o.print(i);
    return o;
  }
#endif

#define HTTP_PORT 80
#define WS_PORT 81
#define USE_SERIAL Serial
// ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

#define MAX_DEPTH 2
idx_t tops[MAX_DEPTH];
PANELS(webPanels,{0,0,30,20});
xmlFmt<esp8266_WebServerOut> serverOut(server,tops,webPanels);

//menu action functions
result action1(eventMask event, navNode& nav, prompt &item) {
  Serial.println("action A called!");
  serverOut<<"This is action <b>A</b> web report "<<(millis()%1000)<<"<br/>";
  return proceed;
}
result action2(eventMask event, navNode& nav, prompt &item) {
  Serial.println("action B called!");
  serverOut<<"This is action <b>B</b> web report "<<(millis()%1000)<<"<br/>";
  return proceed;
}

int ledCtrl=LOW;
#define LEDPIN LED_BUILTIN
void updLed() {
  digitalWrite(LEDPIN,!ledCtrl);
}

TOGGLE(ledCtrl,setLed,"Led: ",updLed,enterEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

int timeOn=50;
void updAnalog() {
  analogWrite(ANALOG_PIN,map(timeOn,0,100,0,255/*PWMRANGE*/));
}

//the menu
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(setLed)
  ,OP("Action A",action1,enterEvent)
  ,OP("Action B",action2,enterEvent)
  ,FIELD(timeOn,"On","ms",0,100,10,1, updAnalog, anyEvent, noStyle)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
);

result idle(menuOut& o,idleEvent e) {
  //if (e==idling)
  Serial.println("suspended");
  o<<"suspended..."<<endl<<"press [select]"<<endl<<"to continue"<<endl<<(millis()%1000);
  return quit;
}

MENU_OUTLIST(out,&serverOut);
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

//config myOptions('*','-',defaultNavCodes,false);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      //USE_SERIAL.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        //USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "console.log('ArduinoMenu Connected')");
      }
      break;
    case WStype_TEXT:
      //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      nav.async((const char*)payload);//this is slow!!!!!!!!
      break;
    case WStype_BIN: {
        USE_SERIAL<<"[WSc] get binary length:"<<length<<"[";
        for(int c=0;c<length;c++) {
          USE_SERIAL.print(*(char*)(payload+c),HEX);
          USE_SERIAL.write(',');
        }
        USE_SERIAL<<"]"<<endl;
        uint16_t id=*(uint16_t*) payload++;
        idx_t len=*((idx_t*)++payload);
        idx_t* pathBin=(idx_t*)++payload;
        const char* inp=(const char*)(payload+len);
        //Serial<<"id:"<<id<<endl;
        if (id==nav.active().hash()) {
          //Serial<<"id ok."<<endl;Serial.flush();
          //Serial<<"input:"<<inp<<endl;
          //StringStream inStr(inp);
          //while(inStr.available())
          nav.doInput(inp);
          webSocket.sendTXT(num, "binBusy=false;");//send javascript to unlock the state
        } //else Serial<<"id not ok!"<<endl;
        //Serial<<endl;
      }
      break;
  }
}

void pageStart() {
  server.sendHeader("Cache-Control","no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma","no-cache");
  server.sendHeader("Expires","0");
  serverOut
    <<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\r\n"
      "<?xml-stylesheet type=\"text/xsl\" href=\""
      xslt
      "\"?>\r\n<menuLib>\r\n"
      "<sourceURL>"
    <<serverName
    <<"</sourceURL>";
}

void pageEnd() {
  serverOut<<"</menuLib>";
  server.send(200, "text/xml", serverOut.response);
}

void handleNotFound(){
  //digitalWrite(led, 1);
  Serial.println("404 not found");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  USE_SERIAL.println(message);
  //digitalWrite(led, 0);
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".xsl")) return "text/xml";
  else if(filename.endsWith(".xslt")) return "text/xsl";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  // Serial<<"file not found "<<path<<endl;
  return false;
}

inline void notfound() {
  server.send(404, "text/plain", "FileNotFound"+server.uri());
}

bool handleMenu(){
  if (server.hasArg("at"))
    return nav.async(server.arg("at").c_str());
  else return true;
}

void setup(){
  pinMode(LEDPIN,OUTPUT);
  updLed();
  analogWriteRange(1023);
  pinMode(ANALOG_PIN,OUTPUT);
  updAnalog();
  //options=&myOptions;//menu options
  Serial.begin(115200);
  while(!Serial)
  //USE_SERIAL.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

  // Serial.setDebugOutput(1);
  // Serial.setDebugOutput(0);
  // while(!Serial);
  // delay(10);
  // wifi_station_set_hostname((char*)serverName);
  Serial.println("");
  Serial.println("Arduino menu webserver example");

  SPIFFS.begin();

  //WiFi.hostname(serverName);//not good

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // WiFiMulti.addAP(ssid, password);
  // while(WiFiMulti.run() != WL_CONNECTED) delay(100);
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  webSocket.begin();
  Serial.println("");
  webSocket.onEvent(webSocketEvent);
  Serial.println("Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();

  // if (MDNS.begin("esp8266")) {
  //   Serial.println("MDNS responder started");
  // }

  nav.idleTask=idle;//point a function to be used when menu is suspended

  //we have none, so do not ask again! use a standard...
  server.on("/favicon.ico",handleNotFound);

  server.on("/", HTTP_GET, []() {
    handleMenu();//no output will be done on this mode
    if(!handleFileRead("/index.html")) notfound();}//static page
  );

  //menu xml server
  server.on("/menu", HTTP_GET, []() {
    handleMenu();
    String r(serverOut.response);
    serverOut.response.remove(0);
    pageStart();
    nav.doOutput();
    serverOut<<"<output>";
    serverOut<<r;
    serverOut<<"</output>";
    pageEnd();
    delay(1);
  });

  //file server, if MDNS handler not found.
  //so, do not put sensitive info in the data files
  server.onNotFound([](){if (!handleFileRead(server.uri())) notfound();});

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Serving ArduinoMenu example.");
}

void loop(void){
  serverOut.response.remove(0);
  webSocket.loop();
  server.handleClient();
  delay(1);
}
