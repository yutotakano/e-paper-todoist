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
#include "lvgl.h"

// const char* ssid = "Waveshare";
// const char* password = "password";
// const char* ssid = "TheHome";
// const char* password = "qq330447168";
const char *ssid = WIFI_SSID;     //"your ssid";
const char *password = WIFI_PASS; //"your password";
ESP8266WebServer server(80);
IPAddress myIP; // IP address in your local wifi net

int32_t last_counter = 0;
lv_display_t *lvgl_display_black;
lv_display_t *lvgl_display_red;
static uint8_t lvgl_draw_buffer[400 * 300 / 8 + 8];

void lvgl_flush_callback(lv_display_t *display, const lv_area_t *area, unsigned char *px_map);
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
  Serial.println("HTTP server started");

  lv_init();
  lvgl_display_black = lv_display_create(400, 300);
  lv_display_set_buffers(lvgl_display_black, lvgl_draw_buffer, NULL, sizeof(lvgl_draw_buffer), LV_DISPLAY_RENDER_MODE_DIRECT);
  lv_display_set_flush_cb(lvgl_display_black, lvgl_flush_callback);
  lv_display_set_default(lvgl_display_black);

  lvgl_display_red = lv_display_create(400, 300);
  lv_display_set_buffers(lvgl_display_red, lvgl_draw_buffer, NULL, sizeof(lvgl_draw_buffer), LV_DISPLAY_RENDER_MODE_DIRECT);
  lv_display_set_flush_cb(lvgl_display_red, lvgl_flush_callback);

  lv_obj_t *text_label = lv_label_create(lv_screen_active());
  lv_label_set_long_mode(text_label, LV_LABEL_LONG_WRAP); // Breaks the long lines
  lv_label_set_text(text_label, "Hello, world!");
  lv_obj_set_width(text_label, 150); // Set smaller width to make the lines wrap
  lv_obj_set_style_text_align(text_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(text_label, LV_ALIGN_CENTER, -30, -80);

  lv_obj_t *text_labela = lv_label_create(lv_display_get_screen_active(lvgl_display_red));
  lv_label_set_long_mode(text_labela, LV_LABEL_LONG_WRAP); // Breaks the long lines
  lv_label_set_text(text_labela, "Hello, world!");
  lv_obj_set_width(text_labela, 150); // Set smaller width to make the lines wrap
  lv_obj_set_style_text_align(text_labela, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(text_labela, LV_ALIGN_CENTER, -20, 90);
}

void lvgl_flush_callback(lv_display_t *display, const lv_area_t *area, unsigned char *px_map)
{
  Serial.printf("Running flush...\n");
  EPD_dispIndex = 1;
  EPD_Init_4in2_V2();

  if (display != lvgl_display_black)
  {
    EPD_SendCommand(0x26); // DATA_START_TRANSMISSION_1
    for (int i = 0; i < 15000; i++)
      EPD_SendData((byte)px_map[i]); // Red channel
  }
  else
  {
    EPD_SendCommand(0x24); // DATA_START_TRANSMISSION_1
    for (int i = 0; i < 15000; i++)
      EPD_SendData((byte)px_map[i]); // Black channel
  }
  EPD_4IN2_V2_Show();

  // Inform the graphics library that the flush is done
  lv_disp_flush_ready(display);
  Serial.printf("Flush done\n");
}

void loop(void)
{
  // int32_t counter;
  // asm volatile("rsr %0, ccount" : "=r"(counter));
  // if (last_counter > counter)
  // {
  //   Serial.println("Counter overflow");
  //   lv_tick_inc((0xFFFFFFFF - last_counter + counter) / 80000);
  // }
  // else
  // {
  //   lv_tick_inc((counter - last_counter) / 80000);
  // }
  // last_counter = counter;
  lv_task_handler();
  lv_timer_handler();
  lv_tick_inc(5);
  delay(5);
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
