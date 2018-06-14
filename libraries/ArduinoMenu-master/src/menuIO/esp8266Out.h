/* -*- C++ -*- */

#ifndef RSITE_ARDUINO_MENU_ESP8266OUT
  #define RSITE_ARDUINO_MENU_ESP8266OUT
  #include "../menu.h"
  #include <ESP8266WiFi.h>
  // #include <ESP8266WiFiMulti.h>
  //#include <WiFiClient.h>
  #include <WebSocketsServer.h>
  #include <ESP8266WebServer.h>
  // #include <ESP8266mDNS.h>
  //#include <Hash.h>
  //#include <FS.h>
  #include <vector>

  using namespace std;

  namespace Menu {

    class esp8266Out:public webOut {
      public:
        esp8266Out(
          idx_t* t,
          panelsList& p,
          menuOut::styles styles=(menuOut::styles)(redraw|expandEnums)
        ):webOut(t,p,styles) {}
        menuOut& fill(
          int x1, int y1, int x2, int y2,char ch=' ',
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false) override {return *this;}
        void setCursor(idx_t x,idx_t y,idx_t panelNr=0) override {}
        void clear() override {panels.reset();}
        void clear(idx_t panelNr) override {
          setCursor(0,0,panelNr);
          panels.nodes[panelNr]=NULL;
        }
        void clearLine(
          idx_t ln,
          idx_t panelNr=0,
          colorDefs color=bgColor,
          bool selected=false,
          status stat=enabledStatus,
          bool edit=false
        ) override {}
        void setColor(colorDefs c,bool selected=false,status s=enabledStatus,bool e=false) override {};
        //template<typename T> esp8266Out& operator<<(T t)=0;
    };

    menuOut& operator<<(menuOut&o,classes c);
    template<typename T> inline String& operator<<(String& o,T t) {return o.operator+=(t);}

    class esp8266_WebServerOut:public esp8266Out {
      public:
        String response;
        ESP8266WebServer &server;
        //using esp8266Out::esp8266Out;
        esp8266_WebServerOut(
          ESP8266WebServer &srv,
          /*const colorDef<esp8266Out::webColor> (&c)[nColors],*/
          idx_t* t,
          panelsList& p
        ):esp8266Out(t,p),server(srv) {}
        size_t write(uint8_t ch) override {response<<(char)ch;return 1;}
        template<typename T> inline esp8266_WebServerOut& operator<<(T t) {response<<t;return *this;}
    };

    class esp8266_WebSocketOut:public esp8266Out {
      public:
        String response;
        WebSocketsServer &webSocket;
        esp8266_WebSocketOut(WebSocketsServer &socket,idx_t* tops,panelsList& panels)
          :webSocket(socket),esp8266Out(tops,panels,(menuOut::styles)(minimalRedraw|expandEnums)) {}
        size_t write(uint8_t ch) override {response<<(char)ch;return 1;}
    };

    //template<typename T> inline esp8266_WebServerOut& operator<<(esp8266_WebServerOut&o , T t) {return o.operator<<(t);}
    //template<typename T> inline esp8266_WiFiClientOut& operator<<(esp8266_WiFiClientOut&o , T t) {return o.operator<<(t);}
    //template<typename T> inline esp8266Out& operator<<(esp8266Out&o , T t) {return o.operator<<(t);}

  }//namespace Menu

#endif
