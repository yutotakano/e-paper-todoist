#include "Print.h"
#include "HardwareSerial.h"
#include "todoist_json_print.h"
#include "secrets.h"

#define TICK_SEQUENCE "\xEF\x81\x94 "
#define TICK_SEQUENCE_LEN sizeof(TICK_SEQUENCE) - 1

static lwjson_stream_parser_t stream_parser;

todoist_task_t todoist_tasks[3];
todoist_task_t current_parsing_task;

static void
prv_example_callback_func(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type)
{
  if (jsp->stack_pos == 3 && lwjson_stack_seq_3(jsp, 0, ARRAY, OBJECT, KEY) && strcmp(jsp->stack[2].meta.name, "content") == 0)
  {
    // Title
    Serial.printf("\"content\": \"%s\", ", jsp->data.str.buff);
    strncpy(current_parsing_task.content, jsp->data.str.buff, sizeof(current_parsing_task.content));
  }
  else if (jsp->stack_pos == 5 && lwjson_stack_seq_5(jsp, 0, ARRAY, OBJECT, KEY, OBJECT, KEY) && strcmp(jsp->stack[2].meta.name, "due") == 0)
  {
    if (strcmp(jsp->stack[4].meta.name, "date") == 0)
    {
      Serial.printf("\"due.date\": \"%s\", ", jsp->data.str.buff);
      // If only due time is not set yet, set to unix time from date
      if (current_parsing_task.due == 0)
      {
        struct tm tm = {0};
        strptime(jsp->data.str.buff, "%Y-%m-%d", &tm);
        current_parsing_task.due = mktime(&tm);
      }
    }
    else if (strcmp(jsp->stack[4].meta.name, "datetime") == 0)
    {
      Serial.printf("\"due.datetime\": \"%s\", ", jsp->data.str.buff);
      struct tm tm = {0};
      strptime(jsp->data.str.buff, "%Y-%m-%dT%H:%M:%S", &tm);
      current_parsing_task.due = mktime(&tm);
      current_parsing_task.has_time = true;
    }
  }
  else if (jsp->stack_pos == 1 && lwjson_stack_seq_1(jsp, 0, ARRAY) && type == LWJSON_STREAM_TYPE_OBJECT_END)
  {
    Serial.printf("}");
    // Just finished parsing a task, now insert it somewhere in 1, 2, or 3
    // First begin by checking against the latemost one. If we start from the
    // earliest, then we may have to shift everything, which might lose data.
    if (current_parsing_task.due == 0)
      return;

    if (todoist_tasks[2].due == 0 || current_parsing_task.due < todoist_tasks[2].due)
    {
      // Insert into task3 (maybe might require moving it later)
      strcpy(todoist_tasks[2].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[2].content + TICK_SEQUENCE_LEN, current_parsing_task.content, sizeof(todoist_tasks[2].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[2].due = current_parsing_task.due;
    }
    if (todoist_tasks[1].due == 0 || current_parsing_task.due < todoist_tasks[1].due)
    {
      // Insert into task2, move whatever was in task2 to task3
      strcpy(todoist_tasks[2].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[2].content + TICK_SEQUENCE_LEN, todoist_tasks[1].content + TICK_SEQUENCE_LEN, sizeof(todoist_tasks[2].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[2].due = todoist_tasks[1].due;
      strcpy(todoist_tasks[1].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[1].content + TICK_SEQUENCE_LEN, current_parsing_task.content, sizeof(todoist_tasks[1].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[1].due = current_parsing_task.due;
    }
    if (todoist_tasks[0].due == 0 || current_parsing_task.due < todoist_tasks[0].due)
    {
      // Insert into task1, move whatever was in task1 to task2
      strcpy(todoist_tasks[1].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[1].content + TICK_SEQUENCE_LEN, todoist_tasks[0].content + TICK_SEQUENCE_LEN, sizeof(todoist_tasks[1].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[1].due = todoist_tasks[0].due;
      strcpy(todoist_tasks[0].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[0].content + TICK_SEQUENCE_LEN, current_parsing_task.content, sizeof(todoist_tasks[0].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[0].due = current_parsing_task.due;
    }

    current_parsing_task = {0};
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
