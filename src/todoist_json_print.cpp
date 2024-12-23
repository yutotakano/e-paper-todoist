#include "Print.h"
#include "HardwareSerial.h"
#include "todoist_json_print.h"
#include "secrets.h"

#define TICK_SEQUENCE "\xEE\xB4\xBB "
#define TICK_SEQUENCE_LEN sizeof(TICK_SEQUENCE) - 1

static lwjson_stream_parser_t stream_parser;

todoist_task_t todoist_tasks[3];
todoist_task_t current_parsing_task;

/**
 * @brief Callback function for the lwjson parser. This function will be called
 * whenever a new JSON token is parsed. We use this to extract the content and
 * due date of each task, and then insert them into the todoist_tasks array.
 *
 * @param jsp lwjson stream parser context
 * @param type type of the current token
 */
static void
prv_example_callback_func(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type)
{
  if (jsp->stack_pos == 3 && lwjson_stack_seq_3(jsp, 0, ARRAY, OBJECT, KEY) && strcmp(jsp->stack[2].meta.name, "content") == 0)
  {
    // [{"content": "..."}, ...]
    // Title
    Serial.printf("\"content\": \"%s\", ", jsp->data.str.buff);
    strncpy(current_parsing_task.content, jsp->data.str.buff, sizeof(current_parsing_task.content));
  }
  else if (jsp->stack_pos == 5 && lwjson_stack_seq_5(jsp, 0, ARRAY, OBJECT, KEY, OBJECT, KEY) && strcmp(jsp->stack[2].meta.name, "due") == 0)
  {
    // [{"due": {k: v}}, ...]
    if (strcmp(jsp->stack[4].meta.name, "date") == 0)
    {
      // [{"due": {"date": "..."}}, ...]
      Serial.printf("\"due.date\": \"%s\", ", jsp->data.str.buff);
      // If only due time is not set yet, set to unix time from date
      if (!current_parsing_task.has_time)
      {
        char tz[64];
        memcpy(tz, getenv("TZ"), sizeof(tz));
        setenv("TZ", TODOIST_TIMEZONE, 1);
        tzset();
        struct tm tm = {.tm_isdst = -1};
        strptime(jsp->data.str.buff, "%Y-%m-%d", &tm);
        // By default it's 00:00:00, but we want to show it below tasks with
        // time, so set it to 23:59:59
        tm.tm_hour = 23;
        tm.tm_min = 59;
        tm.tm_sec = 59;
        current_parsing_task.due = mktime(&tm);
        setenv("TZ", tz, 1);
        tzset();
      }
    }
    else if (strcmp(jsp->stack[4].meta.name, "datetime") == 0)
    {
      // [{"due": {"datetime": "..."}}, ...]
      Serial.printf("\"due.datetime\": \"%s\", ", jsp->data.str.buff);
      // Temporarily hold onto current TZ. Since getenv() returns a pointer to
      // the actual environment variable, we need to copy it to a new buffer.
      char tz[64];
      memcpy(tz, getenv("TZ"), sizeof(tz));
      setenv("TZ", TODOIST_TIMEZONE, 1);
      tzset();
      struct tm tm = {.tm_isdst = -1};
      strptime(jsp->data.str.buff, "%Y-%m-%dT%H:%M:%S", &tm);
      current_parsing_task.due = mktime(&tm);
      // Restore TZ
      setenv("TZ", tz, 1);
      tzset();
      current_parsing_task.has_time = true;
    }
  }
  else if (jsp->stack_pos == 1 && lwjson_stack_seq_1(jsp, 0, ARRAY) && type == LWJSON_STREAM_TYPE_OBJECT_END)
  {
    // [{}, ...]
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
      todoist_tasks[2].has_time = current_parsing_task.has_time;
    }
    if (todoist_tasks[1].due == 0 || current_parsing_task.due < todoist_tasks[1].due)
    {
      // Insert into task2, move whatever was in task2 to task3
      strcpy(todoist_tasks[2].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[2].content + TICK_SEQUENCE_LEN, todoist_tasks[1].content + TICK_SEQUENCE_LEN, sizeof(todoist_tasks[2].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[2].due = todoist_tasks[1].due;
      todoist_tasks[2].has_time = todoist_tasks[1].has_time;
      strcpy(todoist_tasks[1].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[1].content + TICK_SEQUENCE_LEN, current_parsing_task.content, sizeof(todoist_tasks[1].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[1].due = current_parsing_task.due;
      todoist_tasks[1].has_time = current_parsing_task.has_time;
    }
    if (todoist_tasks[0].due == 0 || current_parsing_task.due < todoist_tasks[0].due)
    {
      // Insert into task1, move whatever was in task1 to task2
      strcpy(todoist_tasks[1].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[1].content + TICK_SEQUENCE_LEN, todoist_tasks[0].content + TICK_SEQUENCE_LEN, sizeof(todoist_tasks[1].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[1].due = todoist_tasks[0].due;
      todoist_tasks[1].has_time = todoist_tasks[0].has_time;
      strcpy(todoist_tasks[0].content, TICK_SEQUENCE);
      strncpy(todoist_tasks[0].content + TICK_SEQUENCE_LEN, current_parsing_task.content, sizeof(todoist_tasks[0].content) - TICK_SEQUENCE_LEN);
      todoist_tasks[0].due = current_parsing_task.due;
      todoist_tasks[0].has_time = current_parsing_task.has_time;
    }

    // Reset current_parsing_task for the next task
    current_parsing_task = {0};
  }
}

/**
 * @brief Initialise our Print interface for parsing JSON from the Todoist API.
 * Should be called for every new JSON response to parse, as it resets the
 * tasks array and the internal state of the parser.
 */
void TodoistJsonPrint::init()
{
  lwjson_stream_init(&stream_parser, prv_example_callback_func);

  // Reset all tasks to 0, otherwise if any were completed, they'd still show up
  // as being earlier than the new tasks and will stay on screen
  for (int i = 0; i < 3; i++)
  {
    todoist_tasks[i] = {0};
  }
}

/**
 * @brief Write a byte into the stream parser. This function implements a part
 * of the Print interface required by ESP8266HTTPClient::writeToPrint().
 *
 * @param t byte content
 * @return size_t bytes written (always 1 unless error)
 */
inline size_t TodoistJsonPrint::write(uint8_t t)
{
  res = lwjson_stream_parse(&stream_parser, (char)t);
  if (res == lwjsonSTREAMINPROG)
  {
    // do nothing
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
    return 0;
  }

  return 1;
}

/**
 * @brief Write a sequence of bytes into the stream parser. This function
 * implements a part of the Print interface required by
 * ESP8266HTTPClient::writeToPrint().
 *
 * @param buffer pointer to the buffer
 * @param size size of the buffer
 * @return size_t bytes written
 */
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

/**
 * @brief Return the number of bytes available for writing. This function
 * implements a part of the Print interface required by
 * ESP8266HTTPClient::writeToPrint(). A useful implementation may return some
 * number of bytes to be written into a limited buffer, but we don't have such
 * limit so we just return a constant (i.e. always available to write).
 * 512 was chosen so we could write the whole TLS chunk in one go.
 *
 * @return int
 */
inline int TodoistJsonPrint::availableForWrite()
{
  return 512;
}
