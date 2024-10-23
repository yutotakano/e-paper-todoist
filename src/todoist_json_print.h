#ifndef TODOIST_JSON_PRINT_H
#define TODOIST_JSON_PRINT_H

#include "Print.h"
#include <lwjson/lwjson.h>
#include "time.h"

extern char task1_title[100];
extern char task1_due_string[16];
extern time_t task1_due;

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
