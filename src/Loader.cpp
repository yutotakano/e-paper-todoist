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
  lv_display_set_color_format(lvgl_display_black, LV_COLOR_FORMAT_I1);
  lv_display_set_buffers(lvgl_display_black, lvgl_draw_buffer, NULL, sizeof(lvgl_draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(lvgl_display_black, lvgl_flush_callback);
  lv_display_set_default(lvgl_display_black);

  lvgl_display_red = lv_display_create(400, 300);
  lv_display_set_color_format(lvgl_display_red, LV_COLOR_FORMAT_I1);
  lv_display_set_buffers(lvgl_display_red, lvgl_draw_buffer2, NULL, sizeof(lvgl_draw_buffer2), LV_DISPLAY_RENDER_MODE_PARTIAL);
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

bool first = true;

void lvgl_flush_callback(lv_display_t *display, const lv_area_t *area, unsigned char *px_map)
{
  px_map += 8; // Skip the first 8 bytes since it is LVGL metadata

  Serial.printf("Running flush for %s (%d->%d, %d->%d)...\n", display == lvgl_display_black ? "black" : "red", area->x1, area->x2, area->y1, area->y2);
  if (first)
  {
    EPD_dispIndex = 1;
    EPD_Init_4in2_V2();
    Serial.printf("Init done\n");
    first = false;
  }

  EPD_Send_1(0x4E, area->x1 & 0x3F);
  EPD_Send_2(0x4F, area->y1 & 0xFF, (area->y1 >> 8) & 0xFF);

  if (display != lvgl_display_black)
  {
    EPD_SendCommand(0x26); // Start RED transmission
  }
  else
  {
    EPD_SendCommand(0x24); // Start BW transmission
  }
  for (int32_t i = 0; i < 1500; i++)
  {
    if (i % (area->x2 - area->x1 + 1) == 0 && (area->x2 != 399 || area->x1 != 0))
    {
      // New line of this area (and it's not the whole width), so send 0x4E to set x pos
      EPD_Send_1(0x4E, area->x1 & 0x3F);
      int32_t y = area->y1 + i / (area->x2 - area->x1 + 1);
      EPD_Send_2(0x4F, y & 0xFF, (y >> 8) & 0xFF);
    }
    EPD_SendData((byte)px_map[i]); // Red channel
  }
  Serial.printf("Sending data done\n");

  if (lv_display_flush_is_last(display))
  {
    EPD_SendCommand(0x22);
    EPD_SendData(0xF7);
    EPD_SendCommand(0x20);
    EPD_WaitUntilIdle_high();
    Serial.printf("Flush done\n");

    Serial.printf("Entering deep sleep\n");
    EPD_SendCommand(0x10); // DEEP_SLEEP
    EPD_SendData(0x01);
    first = true;
  }
  Serial.printf("Flush done, informing ready\n");
  // Inform the graphics library that the flush is done
  lv_display_flush_ready(display);
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
