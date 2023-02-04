#ifndef _PERSISTENT_LOGS_
#define _PERSISTENT_LOGS

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <stdarg.h>
#include <stdio.h>
#include "../nvs/nvs.h"
#include <stdlib.h>

#define ZEPHYR_LOG_LEVEL_INFO 0
#define ZEPHYR_LOG_LEVEL_DEBUG 1
#define ZEPHYR_LOG_LEVEL_WARN 2
#define ZEPHYR_LOG_LEVEL_ERROR 3
#define ZEPHYR_LOG_LEVEL_VERBOS 4

#define MAX_PERSISTENT_LOGS 10
#define LOG_SIZE 50
LOG_MODULE_REGISTER(main, 4);
int LOG_KEY = 0;
char buffer[LOG_SIZE];
int buf[20];
int write_to_nvs(int logLevel, char *data, size_t len)
{
  int ret = 0;
  LOG_INF("Entered into write to nvs\n");

  if (LOG_KEY >= MAX_PERSISTENT_LOGS)
  {
    LOG_ERR("Maximum persistent logs limit reached! Dumping all the logs\n");

    for (int i = 0; i < MAX_PERSISTENT_LOGS; i++)
    {
      nvs_read(&fs, i, buf, 20);
      LOG_ERR("%s", buf);
    }
    LOG_KEY = 0;
  }

  ret = nvs_write(&fs, LOG_KEY, data, len);
  LOG_INF("Data was written at %dth index\n", LOG_KEY);
  LOG_KEY++;

  if (ret)
  {
    LOG_INF("Wrote to NVS successfully\n");
  }

  return ret;
}

int write_p_log(int logLevel, char *format, ...)
{

  va_list list;
  va_start(list, format);

  /*
   * buffer here is used to create the string log message as well as
   * store the encoded protobuf message
   */
  //char buffer[100];
  memset(buffer, 0, LOG_SIZE);
  vsprintf((char *)buffer, format, list);
  // vprintf(format, list);
  va_end(list);

  write_to_nvs(logLevel, buffer, LOG_SIZE);
}

#define P_LOG_INF(_format, ...)    \
  LOG_INF(_format, ##__VA_ARGS__); \
  write_p_log(ZEPHYR_LOG_LEVEL_INFO, _format, ##__VA_ARGS__);\

#define P_LOG_DBG(_format, ...)    \
  LOG_DBG(_format, ##__VA_ARGS__); \
  write_p_log(ZEPHYR_LOG_LEVEL_DEBUG, _format, ##__VA_ARGS__);\

#define P_LOG_ERR(_format, ...)    \
  LOG_ERR(_format, ##__VA_ARGS__); \
  write_p_log(ZEPHYR_LOG_LEVEL_ERROR, _format, ##__VA_ARGS__);\


#endif