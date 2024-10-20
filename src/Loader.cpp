#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "scripts.h" // JavaScript code
#include "css.h"     // Cascading Style Sheets
#include "html.h"    // HTML page of the tool
#include "epd.h"     // e-Paper driver
#include "secrets.h" // Wifi credentials, ignored from Git.

// const char* ssid = "Waveshare";
// const char* password = "password";
// const char* ssid = "TheHome";
// const char* password = "qq330447168";
const char *ssid = WIFI_SSID;     //"your ssid";
const char *password = WIFI_PASS; //"your password";
ESP8266WebServer server(80);
IPAddress myIP; // IP address in your local wifi net

void EPD_Init();
void EPD_Load();
void EPD_Next();
void EPD_Show();
void handleNotFound();

void setup(void)
{

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Static IP setting---by Lin
  wifi_station_dhcpc_stop();
  struct ip_info info;
  IP4_ADDR(&info.ip, 192, 168, 1, 189);
  IP4_ADDR(&info.gw, 192, 168, 1, 1);
  IP4_ADDR(&info.netmask, 255, 255, 255, 0);
  wifi_set_ip_info(STATION_IF, &info);

  // Connect to WiFi network
  Serial.println("");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  // SPI initialization
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_DIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(DC_PIN, OUTPUT);
  pinMode(BUSY_PIN, INPUT);
  //   SPI.begin();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\r\nIP address: ");
  Serial.println(myIP = WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/styles.css", sendCSS);
  server.on("/processingA.js", sendJS_A);
  server.on("/processingB.js", sendJS_B);
  server.on("/processingC.js", sendJS_C);
  server.on("/processingD.js", sendJS_D);
  server.on("/LOAD", EPD_Load);
  server.on("/EPD", EPD_Init);
  server.on("/NEXT", EPD_Next);
  server.on("/SHOW", EPD_Show);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
}

void EPD_Init()
{
  EPD_dispIndex = ((int)server.arg(0)[0] - 'a') + (((int)server.arg(0)[1] - 'a') << 4);
  // Print log message: initialization of e-Paper (e-Paper's type)
  Serial.printf("EPD %s\r\n", EPD_dispMass[EPD_dispIndex].title);

  // Initialization
  EPD_dispInit();
  Serial.printf("Init done");
  server.send(200, "text/plain", "Init ok\r\n");
}

void EPD_Load()
{
  // server.arg(0) = data+data.length+'LOAD'
  String p = server.arg(0);
  if (p.endsWith("LOAD"))
  {
    Serial.println("LOAD");
    int index = p.length() - 8;
    int L = ((int)p[index] - 'a') + (((int)p[index + 1] - 'a') << 4) + (((int)p[index + 2] - 'a') << 8) + (((int)p[index + 3] - 'a') << 12);
    if (L == (p.length() - 8))
    {
      Serial.println("LOAD");
      // if there is loading function for current channel (black or red)
      // Load data into the e-Paper
      if (EPD_dispLoad != 0)
        EPD_dispLoad();
    }
  }
  Serial.println("LOAD ok");
  server.send(200, "text/plain", "Load ok\r\n");
}

void EPD_Next()
{
  Serial.println("NEXT");

  // Instruction code for for writting data into
  // e-Paper's memory
  int code = EPD_dispMass[EPD_dispIndex].next;
  if (EPD_dispIndex == 34)
  {
    if (flag == 0)
      code = 0x26;
    else
      code = 0x13;
  }

  // If the instruction code isn't '-1', then...
  if (code != -1)
  {
    // Do the selection of the next data channel
    EPD_SendCommand(code);
    delay(2);
  }
  // Setup the function for loading choosen channel's data
  EPD_dispLoad = EPD_dispMass[EPD_dispIndex].chRd;

  server.send(200, "text/plain", "Next ok\r\n");
}

void EPD_Show()
{
  Serial.println("\r\nSHOW\r\n");
  // Show results and Sleep
  EPD_dispMass[EPD_dispIndex].show();
  server.send(200, "text/plain", "Show ok\r\n");
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(200, "text/plain", message);
  Serial.print("Unknown URI: ");
  Serial.println(server.uri());
}
