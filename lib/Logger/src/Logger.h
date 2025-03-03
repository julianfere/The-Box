#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LogLevel
{
  DEBUG,
  INFO,
  WARN,
  ERROR
};

class Logger
{
public:
  Logger();

  void debug(const char *tag, const char *message);
  void debug(const char *tag, const String &message);
  void debug(const char *tag, int message);
  void debug(const char *tag, float message);
  void debug(const char *tag, double message);

  void info(const char *tag, const char *message);
  void info(const char *tag, const String &message);
  void info(const char *tag, int message);
  void info(const char *tag, float message);
  void info(const char *tag, double message);

  void warn(const char *tag, const char *message);
  void warn(const char *tag, const String &message);
  void warn(const char *tag, int message);
  void warn(const char *tag, float message);
  void warn(const char *tag, double message);

  void error(const char *tag, const char *message);
  void error(const char *tag, const String &message);
  void error(const char *tag, int message);
  void error(const char *tag, float message);
  void error(const char *tag, double message);

private:
  void log(LogLevel level, const char *tag, const char *message);
  void log(LogLevel level, const char *tag, int message);
  void log(LogLevel level, const char *tag, float message);
  void log(LogLevel level, const char *tag, double message);

  const char *getLevelString(LogLevel level);
};

#endif // LOGGER_H
