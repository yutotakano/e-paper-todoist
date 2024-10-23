#include "Print.h"
#include "HardwareSerial.h"
#include "todoist_json_print.h"

static lwjson_stream_parser_t stream_parser;
char task1_title[100];
char task1_due_string[16];
time_t task1_due;

char current_parsing_title[100];
char current_parsing_due_string[16];
time_t current_parsing_due;

static void
prv_example_callback_func(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type)
{
  if (jsp->stack_pos == 3 && lwjson_stack_seq_3(jsp, 0, ARRAY, OBJECT, KEY) && strcmp(jsp->stack[2].meta.name, "content") == 0)
  {
    // Title
    Serial.printf("\"content\": \"%s\", ", jsp->data.str.buff);
    strncpy(current_parsing_title, jsp->data.str.buff, sizeof(current_parsing_title));
  }
  else if (jsp->stack_pos == 5 && lwjson_stack_seq_5(jsp, 0, ARRAY, OBJECT, KEY, OBJECT, KEY) && strcmp(jsp->stack[2].meta.name, "due") == 0)
  {
    if (strcmp(jsp->stack[4].meta.name, "string") == 0)
    {
      Serial.printf("\"due.string\": \"%s\", ", jsp->data.str.buff);
      strncpy(current_parsing_due_string, jsp->data.str.buff, sizeof(current_parsing_due_string));
    }
    else if (strcmp(jsp->stack[4].meta.name, "date") == 0)
    {
      Serial.printf("\"due.date\": \"%s\", ", jsp->data.str.buff);
      // If only due time is not set yet, set to unix time from date
      if (current_parsing_due == 0)
      {
        struct tm tm = {0};
        strptime(jsp->data.str.buff, "%Y-%m-%d", &tm);
        current_parsing_due = mktime(&tm);
      }
    }
    else if (strcmp(jsp->stack[4].meta.name, "datetime") == 0)
    {
      Serial.printf("\"due.datetime\": \"%s\", ", jsp->data.str.buff);
      struct tm tm = {0};
      strptime(jsp->data.str.buff, "%Y-%m-%dT%H:%M:%S", &tm);
      current_parsing_due = mktime(&tm);
    }
  }
  else if (jsp->stack_pos == 1 && lwjson_stack_seq_1(jsp, 0, ARRAY) && type == LWJSON_STREAM_TYPE_OBJECT_END)
  {
    Serial.printf("}");
    // Just finished parsing a task
    // Check if this is due earlier than the previous one
    if (current_parsing_due != 0 && (task1_due == 0 || current_parsing_due < task1_due))
    {
      strncpy(task1_title, current_parsing_title, sizeof(task1_title));
      strncpy(task1_due_string, current_parsing_due_string, sizeof(task1_due_string));
      task1_due = current_parsing_due;
      memset(current_parsing_title, 0, sizeof(current_parsing_title));
      memset(current_parsing_due_string, 0, sizeof(current_parsing_due_string));
      current_parsing_due = 0;
    }
  }
}

void TodoistJsonPrint::init()
{
  lwjson_stream_init(&stream_parser, prv_example_callback_func);
}

inline size_t TodoistJsonPrint::write(uint8_t t)
{
  res = lwjson_stream_parse(&stream_parser, (char)t);
  if (res == lwjsonSTREAMINPROG)
  {
  }
  else if (res == lwjsonSTREAMWAITFIRSTCHAR)
  {
    Serial.printf("Waiting first character\r\n");
  }
  else if (res == lwjsonSTREAMDONE)
  {
    Serial.printf("Done\r\n");
  }
  else
  {
    Serial.printf("Error\r\n");
  }

  return 1;
}

inline size_t TodoistJsonPrint::write(const uint8_t *buffer, size_t size)
{
  size_t i;
  for (i = 0; i < size; i++)
  {
    if (buffer[i] == 0)
    {
      break;
    }
    write(buffer[i]);
  }
  return i;
}

inline int TodoistJsonPrint::availableForWrite()
{
  return 512;
}
