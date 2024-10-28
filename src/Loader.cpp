#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "epd.h"     // e-Paper driver
#include "secrets.h" // Wifi credentials, ignored from Git.
#include "lvgl/lvgl.h"
#include "todoist_json_print.h"
#include "time.h"

// Declare font locally (see lv_conf.h for globally declared fonts)
LV_FONT_DECLARE(neuton_50_digits);

const char *ssid PROGMEM = WIFI_SSID;     //"your ssid";
const char *password PROGMEM = WIFI_PASS; //"your password";
IPAddress myIP;                           // IP address in your local wifi net

BearSSL::WiFiClientSecure client;
HTTPClient https;

lv_display_t *lvgl_display_black;
lv_display_t *lvgl_display_red;
#define DRAW_BUFFER_SIZE 400
static uint8_t lvgl_draw_buffer[DRAW_BUFFER_SIZE + 8];

typedef struct
{
  lv_obj_t *container;
  lv_obj_t *content_text;
  lv_obj_t *due_text;
} task_obj_t;

// Layout
lv_obj_t *current_time_text;
lv_point_precise_t header_line_points[] = {{140, 30}, {400, 30}};
lv_obj_t *header_line;
lv_obj_t *list_container;
task_obj_t task_objs[3] = {0};

void update_tasks(lv_timer_t *timer);
void update_time(lv_timer_t *timer);
lv_timer_t *task_update_timer;
lv_timer_t *time_update_timer;

TodoistJsonPrint todoist_json_parser;

void lvgl_flush_callback(lv_display_t *display, const lv_area_t *area, unsigned char *px_map);

uint32_t last_millis = 0;

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
  client.setInsecure();
  client.setBufferSizes(512, 512);

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

  current_time_text = lv_label_create(lv_display_get_screen_active(lvgl_display_red));
  lv_obj_set_style_text_font(current_time_text, &neuton_50_digits, 0);
  lv_label_set_text(current_time_text, "00:00");
  lv_obj_set_style_text_align(current_time_text, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(current_time_text, LV_ALIGN_TOP_LEFT, 15, 15);

  header_line = lv_line_create(lv_display_get_screen_active(lvgl_display_red));
  lv_line_set_points(header_line, header_line_points, 2);
  lv_obj_set_style_line_width(header_line, 4, 0);

  list_container = lv_obj_create(lv_display_get_screen_active(lvgl_display_black));
  lv_obj_set_size(list_container, 400, 250);
  lv_obj_align(list_container, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_hor(list_container, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_row(list_container, 20, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(list_container, LV_OPA_0, LV_PART_MAIN);
  lv_obj_set_style_border_opa(list_container, LV_OPA_0, LV_PART_MAIN);

  for (size_t i = 0; i < sizeof(task_objs) / sizeof(task_obj_t); i++)
  {
    task_objs[i].container = lv_obj_create(list_container);
    lv_obj_set_size(task_objs[i].container, 380, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(task_objs[i].container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(task_objs[i].container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(task_objs[i].container, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(task_objs[i].container, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_opa(task_objs[i].container, LV_OPA_0, LV_PART_MAIN);

    task_objs[i].content_text = lv_label_create(task_objs[i].container);
    lv_label_set_text_fmt(task_objs[i].content_text, "Placeholder Task %d", i);
    lv_label_set_long_mode(task_objs[i].content_text, LV_LABEL_LONG_WRAP); // Breaks the long lines
    lv_obj_set_width(task_objs[i].content_text, 380);                      // Set smaller width to make the lines wrap

    task_objs[i].due_text = lv_label_create(task_objs[i].container);
    lv_label_set_text(task_objs[i].due_text, "Due: 2024-01-01");
    lv_obj_set_style_pad_left(task_objs[i].due_text, 24, LV_PART_MAIN);
  }

  // Set up timers
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

    Serial.printf("Entering deep sleep\n");
    EPD_SendCommand(0x10); // DEEP_SLEEP
    EPD_SendData(0x01);
    first = true;
  }
  // Serial.printf("Flush done, informing ready\n");
  // Inform the graphics library that the flush is done
  lv_display_flush_ready(display);
}

void set_labels_from_tasks()
{
  // Get current time (local time) as estimated from last NTP sync
  time_t now = time(nullptr);
  struct tm local_now;
  localtime_r(&now, &local_now);
  char relative_due_string[20];

  for (size_t i = 0; i < sizeof(todoist_tasks) / sizeof(todoist_task_t); i++)
  {
    tm *local_task_time = localtime(&todoist_tasks[i].due);

    // Replace tick mark if overdue
    if (todoist_tasks[i].due < now)
    {
      memcpy(todoist_tasks[i].content, "\xEE\xAE\xA0", 3);
    }

    // Set titles easy peasy
    lv_label_set_text(task_objs[i].content_text, todoist_tasks[i].content);

    // Set due dates. Todoist gives us both a timestamp and a string, but the
    // string isn't relative (it's always MMM DD format), which isn't very
    // informative when quickly glacing at the display. So we calculate a relative
    // date string, and attach the time to it.
    if (local_task_time->tm_mday == local_now.tm_mday - 1)
    {
      strcpy(relative_due_string, "Yesterday ");
      if (todoist_tasks[i].has_time)
      {
        strftime(relative_due_string + 10, sizeof(relative_due_string) - 10, "%H:%M", local_task_time);
      }
      lv_label_set_text(task_objs[i].due_text, relative_due_string);
    }
    else if (local_task_time->tm_mday == local_now.tm_mday)
    {
      strcpy(relative_due_string, "Today ");
      if (todoist_tasks[i].has_time)
      {
        strftime(relative_due_string + 6, sizeof(relative_due_string) - 6, "%H:%M", local_task_time);
      }
      lv_label_set_text(task_objs[i].due_text, relative_due_string);
    }
    else if (local_task_time->tm_mday == local_now.tm_mday + 1)
    {
      strcpy(relative_due_string, "Tomorrow ");
      if (todoist_tasks[i].has_time)
      {
        strftime(relative_due_string + 9, sizeof(relative_due_string) - 9, "%H:%M", local_task_time);
      }
      lv_label_set_text(task_objs[i].due_text, relative_due_string);
    }
    else
    {
      if (todoist_tasks[i].has_time)
      {
        strftime(relative_due_string, sizeof(relative_due_string), "%Y-%m-%d %H:%M", local_task_time);
        lv_label_set_text(task_objs[i].due_text, relative_due_string);
      }
      else
      {
        strftime(relative_due_string, sizeof(relative_due_string), "%Y-%m-%d", local_task_time);
      }
      lv_label_set_text(task_objs[i].due_text, relative_due_string);
    }
  }
}

void tick()
{
  uint32_t now = millis();
  lv_tick_inc(now - last_millis);
  last_millis = now;
}

void update_tasks(lv_timer_t *timer)
{
  todoist_json_parser.init();
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print(F("[HTTPS] begin...\n"));
    if (https.begin(client, F(TODOIST_ENDPOINT)))
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
          https.writeToPrint(&todoist_json_parser);
          set_labels_from_tasks();
        }
        else
        {
          lv_label_set_text(task_objs[0].content_text, https.errorToString(httpCode).c_str());
        }
      }
      else
      {
        char text[256];
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        // get last ssl error
        client.getLastSSLError(text, sizeof(text));
        lv_label_set_text(task_objs[0].content_text, text);
      }

      https.end();
    }
    else
    {
      lv_label_set_text(task_objs[0].content_text, "[HTTPS] Unable to connect");
    }
  }
}

void update_time(lv_timer_t *timer)
{
  time_t now = time(nullptr);
  // 15 seconds forward to take into account the display update time and slow
  // processing of the ESP8266
  now += 15;
  tm *timeinfo = localtime(&now);
  char time_str[6] = {0};
  sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
  lv_label_set_text(current_time_text, time_str);

  if (timeinfo->tm_min == 0)
  {
    // Flash black, then red, to both indicate the hour, and to fully refresh
    // the display and avoid ghost pixels or discolouration
    lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_black), lv_color_black(), 0);
    lv_obj_set_style_text_color(current_time_text, lv_color_white(), 0);
    lv_refr_now(lvgl_display_black);
    lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_black), lv_color_white(), 0);
    lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_red), lv_color_black(), 0);
    lv_refr_now(NULL);
    lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_red), lv_color_white(), 0);
    lv_obj_set_style_text_color(current_time_text, lv_color_black(), 0);
  }
  lv_obj_set_style_bg_color(lv_display_get_screen_active(lvgl_display_red), lv_color_white(), 0);
}

void loop(void)
{
  tick();
  lv_timer_handler();
  delay(5000);
}
