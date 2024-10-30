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
#define DRAW_BUFFER_SIZE 1600
static uint16_t lvgl_draw_buffer[DRAW_BUFFER_SIZE];

typedef struct
{
  lv_obj_t *container;
  lv_obj_t *content_text;
  lv_obj_t *due_text;
} task_obj_t;

// Layout
lv_obj_t *current_time_text;
lv_point_precise_t header_time_line_points[] = {{140, 24}, {140, 24}};
lv_obj_t *header_time_line;
lv_point_precise_t header_line_points[] = {{140, 30}, {380, 30}};
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
  lv_display_set_default(lvgl_display_black);
  lv_display_set_color_format(lvgl_display_black, LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(lvgl_display_black, lvgl_draw_buffer, NULL, sizeof(lvgl_draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(lvgl_display_black, lvgl_flush_callback);

  // Use the simple theme, and disable default theme in conf.h to cut down binary size
  lv_theme_simple_init(lvgl_display_black);

  current_time_text = lv_label_create(lv_screen_active());
  lv_obj_set_style_text_font(current_time_text, &neuton_50_digits, 0);
  lv_label_set_text(current_time_text, "00:00");
  lv_obj_set_style_text_align(current_time_text, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(current_time_text, LV_ALIGN_TOP_LEFT, 15, 15);
  lv_obj_set_style_text_color(current_time_text, lv_color_make(255, 0, 0), LV_PART_MAIN);

  header_line = lv_line_create(lv_screen_active());
  lv_line_set_points(header_line, header_line_points, 2);
  lv_obj_set_style_line_width(header_line, 4, 0);
  lv_obj_set_style_line_color(header_line, lv_color_make(255, 0, 0), 0);

  header_time_line = lv_line_create(lv_screen_active());
  lv_line_set_points(header_time_line, header_time_line_points, 2);
  lv_obj_set_style_line_width(header_time_line, 4, 0);
  lv_obj_set_style_line_color(header_time_line, lv_color_make(0, 0, 0), 0);

  list_container = lv_obj_create(lv_screen_active());
  lv_obj_set_size(list_container, 400, 250);
  lv_obj_align(list_container, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_hor(list_container, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_row(list_container, 20, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(list_container, LV_OPA_0, LV_PART_MAIN);
  lv_obj_set_style_border_opa(list_container, LV_OPA_0, LV_PART_MAIN);
  lv_obj_set_style_shadow_opa(list_container, LV_OPA_0, LV_PART_MAIN);

  for (size_t i = 0; i < sizeof(task_objs) / sizeof(task_obj_t); i++)
  {
    task_objs[i].container = lv_obj_create(list_container);
    lv_obj_set_size(task_objs[i].container, 380, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(task_objs[i].container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(task_objs[i].container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(task_objs[i].container, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(task_objs[i].container, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_border_opa(task_objs[i].container, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(task_objs[i].container, LV_OPA_0, LV_PART_MAIN);

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
  uint16_t *px_map_16 = (uint16_t *)px_map;

  if (first)
  {
    EPD_Init_4in2_V2();
    Serial.printf("Init done\n");
    first = false;
  }

  for (int mode = 0; mode < 2; mode++)
  {
    EPD_Send_1(0x4E, (area->x1) >> 3);
    EPD_Send_2(0x4F, area->y1 & 0xFF, (area->y1 >> 8) & 0xFF);
    // Mode 0: RED, Mode 1: BW
    if (mode == 0)
    {
      EPD_SendCommand(0x26); // Start RED transmission
    }
    else
    {
      EPD_SendCommand(0x24); // Start BW transmission
    }

    for (int i = 0; i < ((area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1)); i += 8)
    {
      if (i + 7 >= ((area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1)))
      {
        Serial.printf("shouldn't happen\n");
        break;
      }
      byte final_color = 0x00;
      // for each of the 8 pixels going into the final single byte
      for (unsigned int pixel = 0; pixel < 8; pixel++)
      {
        uint16_t color = px_map_16[i + pixel];
        if (mode == 0)
        {
          // RED/White mode, so we want to set the bit if the pixel has a strong red component
          if ((color >> 11) > 16 && ((color & 0x1F) < 16 && ((color & 0x7E0) >> 5) < 32))
          {
            final_color |= 0x01 << (7 - pixel);
          }
        }
        else
        {
          // BW mode, so we want to set the bit only if the pixel is all bright
          if ((color >> 11) > 24 && ((color & 0x1F) > 24) && (((color & 0x7E0) >> 5) > 48))
          {
            final_color |= 0x01 << (7 - pixel);
          }
        }
      }

      if (i % (area->x2 - area->x1 + 1) == 0 && (area->x2 != 399 || area->x1 != 0))
      {
        Serial.printf("new line within subarea\n");
        // New line of this area (and it's not the whole width), so send 0x4E to set x pos
        EPD_Send_1(0x4E, area->x1 >> 3);
        int32_t y = area->y1 + i / (area->x2 - area->x1 + 1);
        EPD_Send_2(0x4F, y & 0xFF, (y >> 8) & 0xFF);
      }

      EPD_SendData(final_color);
    }
  }

  if (lv_display_flush_is_last(display))
  {
    Serial.printf("Finished sending data\n");
    EPD_SendCommand(0x22); // Set display option 2
    EPD_SendData(0xF7);    // Update the display 1 (partial)
    EPD_SendCommand(0x20); // Activate the option set above
    Serial.printf("Waiting for idle\n");
    EPD_WaitUntilIdle_high(); // busy pin is high during previous operation, wait until complete

    Serial.printf("Entering deep sleep\n");
    EPD_SendCommand(0x10); // DEEP_SLEEP
    EPD_SendData(0x01);
    Serial.printf("Entered deep sleep\n");
    first = true;
  }
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

  header_time_line_points[1].x = 140 + (timeinfo->tm_min * (header_line_points[1].x - header_line_points[0].x)) / 60;

  if (timeinfo->tm_min % 10 == 0)
  {
    // Flash black, then red, to both indicate the hour, and to fully refresh
    // the display and avoid ghost pixels or discolouration
    lv_obj_t *sys_layer_black = lv_display_get_layer_top(lvgl_display_black);
    lv_obj_set_style_bg_color(sys_layer_black, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(sys_layer_black, LV_OPA_100, 0);
    lv_refr_now(NULL);
    delay(1);
    lv_obj_set_style_bg_color(sys_layer_black, lv_color_make(255, 0, 0), 0);
    lv_refr_now(NULL);
    delay(1);
    lv_obj_set_style_bg_color(sys_layer_black, lv_color_black(), 0);
    lv_refr_now(NULL);
    delay(1);
    lv_obj_set_style_bg_color(sys_layer_black, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(sys_layer_black, LV_OPA_0, 0);
    lv_refr_now(NULL);
    delay(1);
  }
  else
  {
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
  }
}

void loop(void)
{
  tick();
  lv_timer_handler();
  delay(5000);
}
