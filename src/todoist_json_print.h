#ifndef TODOIST_JSON_PRINT_H
#define TODOIST_JSON_PRINT_H

#include "Print.h"
#include <lwjson/lwjson.h>
#include "time.h"

typedef struct
{
  char content[100] = {0};
  time_t due = 0;
  bool has_time = false;
} todoist_task_t;

extern todoist_task_t todoist_tasks[3];

class TodoistJsonPrint : public Print
{
  virtual size_t write(uint8_t t) override;
  virtual size_t write(const uint8_t *buffer, size_t size) override;
  virtual int availableForWrite() override;

public:
  void init();

private:
  lwjsonr_t res;
};

#endif
