#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "epd.h"     // e-Paper driver
#include "secrets.h" // Wifi credentials, ignored from Git.
#include "lvgl/lvgl.h"
#include "todoist_json_print.h"
#include "time.h"
#include "neuton_50_digits.c"

LV_FONT_DECLARE(neuton_50_digits);

// const char* ssid = "Waveshare";
// const char* password = "password";
// const char* ssid = "TheHome";
// const char* password = "qq330447168";
const char *ssid PROGMEM = WIFI_SSID;     //"your ssid";
const char *password PROGMEM = WIFI_PASS; //"your password";
IPAddress myIP;                           // IP address in your local wifi net

std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
HTTPClient https;

lv_display_t *lvgl_display_black;
lv_display_t *lvgl_display_red;
#define DRAW_BUFFER_SIZE 200
static uint8_t lvgl_draw_buffer[DRAW_BUFFER_SIZE + 8];

// Layout
lv_obj_t *list_container;
lv_obj_t *first_task_content;
lv_obj_t *first_task_due;
lv_obj_t *second_task_content;
lv_obj_t *second_task_due;
lv_obj_t *third_task_content;
lv_obj_t *third_task_due;
lv_obj_t *current_time_text;

void update_tasks(lv_timer_t *timer);
void update_time(lv_timer_t *timer);
lv_timer_t *task_update_timer;
lv_timer_t *time_update_timer;

TodoistJsonPrint todoistJsonPrint;

void lvgl_flush_callback(lv_display_t *display, const lv_area_t *area, unsigned char *px_map);

void setup(void)
{
  Serial.begin(115200);

  // Set wifi up
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Static IP setting---by Lin
  // wifi_station_dhcpc_stop();
  // struct ip_info info;
  // IP4_ADDR(&info.ip, 192, 168, 1, 189);
  // IP4_ADDR(&info.gw, 192, 168, 1, 1);
  // IP4_ADDR(&info.netmask, 255, 255, 255, 0);
  // wifi_set_ip_info(STATION_IF, &info);

  // Connect to WiFi network
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.print(F("Connected to "));
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
  client->setInsecure();
  client->setBufferSizes(512, 512);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Set time up
  configTime(TIMEZONE, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 1577836800) // arbitrary old but recent time: 2020-01-01T00:00:00Z
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print(F("\nIP address: "));
  Serial.println(myIP = WiFi.localIP());

  // Set lvgl up
  lv_init();
  lvgl_display_black = lv_display_create(400, 300);
  lv_display_set_color_format(lvgl_display_black, LV_COLOR_FORMAT_I1);
  lv_display_set_buffers(lvgl_display_black, lvgl_draw_buffer, NULL, sizeof(lvgl_draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(lvgl_display_black, lvgl_flush_callback);
  lv_display_set_default(lvgl_display_black);

  lvgl_display_red = lv_display_create(400, 300);
  lv_display_set_color_format(lvgl_display_red, LV_COLOR_FORMAT_I1);
  lv_display_set_buffers(lvgl_display_red, lvgl_draw_buffer, NULL, sizeof(lvgl_draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(lvgl_display_red, lvgl_flush_callback);

  list_container = lv_obj_create(lv_screen_active());
  lv_obj_set_size(list_container, 400, 300);
  lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);

  first_task_content = lv_label_create(list_container);
  lv_label_set_long_mode(first_task_content, LV_LABEL_LONG_WRAP); // Breaks the long lines
  lv_label_set_text(first_task_content, "Placeholder Task 1");
  lv_obj_set_width(first_task_content, 300); // Set smaller width to make the lines wrap
  lv_obj_set_style_text_align(first_task_content, LV_TEXT_ALIGN_LEFT, 0);

  first_task_due = lv_label_create(list_container);
  lv_label_set_text(first_task_due, "Due: 2021-01-01");

  second_task_content = lv_label_create(list_container);
  lv_label_set_long_mode(second_task_content, LV_LABEL_LONG_WRAP); // Breaks the long lines
  lv_label_set_text(second_task_content, "Placeholder Task 2");
  lv_obj_set_width(second_task_content, 300); // Set smaller width to make the lines wrap
  lv_obj_set_style_text_align(second_task_content, LV_TEXT_ALIGN_LEFT, 0);

  second_task_due = lv_label_create(list_container);
  lv_label_set_text(second_task_due, "Due: 2021-01-01");

  third_task_content = lv_label_create(list_container);
  lv_label_set_long_mode(third_task_content, LV_LABEL_LONG_WRAP); // Breaks the long lines
  lv_label_set_text(third_task_content, "Placeholder Task 3");
  lv_obj_set_width(third_task_content, 300); // Set smaller width to make the lines wrap
  lv_obj_set_style_text_align(third_task_content, LV_TEXT_ALIGN_LEFT, 0);

  third_task_due = lv_label_create(list_container);
  lv_label_set_text(third_task_due, "Due: 2021-01-01");

  current_time_text = lv_label_create(lv_display_get_screen_active(lvgl_display_red));
  lv_obj_set_style_text_font(current_time_text, &neuton_50_digits, 0);
  lv_label_set_text(current_time_text, "00:00");
  lv_obj_set_style_text_align(current_time_text, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(current_time_text, LV_ALIGN_CENTER, 0, 0);

  time_update_timer = lv_timer_create(update_time, 30000, NULL);
  task_update_timer = lv_timer_create(update_tasks, 300000, NULL);

  // Call immediately in next loop
  lv_timer_ready(task_update_timer);
  lv_timer_ready(time_update_timer);
}

bool first = true;

void lvgl_flush_callback(lv_display_t *display, const lv_area_t *area, unsigned char *px_map)
{
  px_map += 8; // Skip the first 8 bytes since it is LVGL metadata

  // Serial.printf("Running flush for %s (%d->%d, %d->%d)...\n", display == lvgl_display_black ? "black" : "red", area->x1, area->x2, area->y1, area->y2);
  if (first)
  {
    EPD_dispIndex = 1;
    EPD_Init_4in2_V2();
    // Serial.printf("Init done\n");
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
  for (int32_t i = 0; i < DRAW_BUFFER_SIZE; i++)
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
  // Serial.printf("Sending data done\n");

  if (lv_display_flush_is_last(display))
  {
    EPD_SendCommand(0x22);
    EPD_SendData(0xF7);
    EPD_SendCommand(0x20);
    EPD_WaitUntilIdle_high();
    // Serial.printf("Flush done\n");

    // Serial.printf("Entering deep sleep\n");
    EPD_SendCommand(0x10); // DEEP_SLEEP
    EPD_SendData(0x01);
    first = true;
  }
  // Serial.printf("Flush done, informing ready\n");
  // Inform the graphics library that the flush is done
  lv_display_flush_ready(display);
}

void update_tasks(lv_timer_t *timer)
{
  todoistJsonPrint.init();
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print(F("[HTTPS] begin...\n"));
    if (https.begin(*client, F(TODOIST_ENDPOINT)))
    {
      Serial.print(F("[HTTPS] GET...\n"));
      https.addHeader("Authorization", F(TODOIST_BEARER));
      https.setReuse(false);

      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          https.writeToPrint(&todoistJsonPrint);
          lv_label_set_text(first_task_content, task1_title);
          lv_label_set_text(first_task_due, task1_due_string);
          lv_label_set_text(second_task_content, task2_title);
          lv_label_set_text(second_task_due, task2_due_string);
          lv_label_set_text(third_task_content, task3_title);
          lv_label_set_text(third_task_due, task3_due_string);
        }
        else
        {
          lv_label_set_text(first_task_content, https.errorToString(httpCode).c_str());
        }
      }
      else
      {
        char text[100];
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        // get last ssl error
        strcpy(text, "[HTTPS] \n");
        client->getLastSSLError(text + 9, sizeof(text) - 9);
        lv_label_set_text(first_task_content, text);
      }

      https.end();
    }
    else
    {
      lv_label_set_text(first_task_content, "[HTTPS] Unable to connect");
    }
  }
}

void update_time(lv_timer_t *timer)
{
  time_t now = time(nullptr);
  tm *timeinfo = localtime(&now);
  char time_str[6];
  // 15 seconds forward to take into account update time
  sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min + (timeinfo->tm_sec / 45));
  lv_label_set_text(current_time_text, time_str);

  if (timeinfo->tm_min == 0)
  {
    // Make whole screen red
    lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_red), lv_color_make(0, 0, 0), 0);
  }
  else
  {
    // Make whole screen white
    lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_red), lv_color_make(255, 255, 255), 0);
  }
}

void loop(void)
{
  lv_timer_handler();
  lv_tick_inc(1000);
  delay(1000);
}
