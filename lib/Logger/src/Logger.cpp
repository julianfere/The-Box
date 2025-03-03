#include "Logger.h"

Logger::Logger() {}

void Logger::debug(const char *tag, const char *message)
{
  log(DEBUG, tag, message);
}

void Logger::debug(const char *tag, const String &message)
{
  log(DEBUG, tag, message.c_str());
}

void Logger::debug(const char *tag, int message)
{
  log(DEBUG, tag, message);
}

void Logger::debug(const char *tag, float message)
{
  log(DEBUG, tag, message);
}

void Logger::debug(const char *tag, double message)
{
  log(DEBUG, tag, message);
}

void Logger::info(const char *tag, const char *message)
{
  log(INFO, tag, message);
}

void Logger::info(const char *tag, const String &message)
{
  log(INFO, tag, message.c_str());
}

void Logger::info(const char *tag, int message)
{
  log(INFO, tag, message);
}

void Logger::info(const char *tag, float message)
{
  log(INFO, tag, message);
}

void Logger::info(const char *tag, double message)
{
  log(INFO, tag, message);
}

void Logger::warn(const char *tag, const char *message)
{
  log(WARN, tag, message);
}

void Logger::warn(const char *tag, const String &message)
{
  log(WARN, tag, message.c_str());
}

void Logger::warn(const char *tag, int message)
{
  log(WARN, tag, message);
}

void Logger::warn(const char *tag, float message)
{
  log(WARN, tag, message);
}

void Logger::warn(const char *tag, double message)
{
  log(WARN, tag, message);
}

void Logger::error(const char *tag, const char *message)
{
  log(ERROR, tag, message);
}

void Logger::error(const char *tag, const String &message)
{
  log(ERROR, tag, message.c_str());
}

void Logger::error(const char *tag, int message)
{
  log(ERROR, tag, message);
}

void Logger::error(const char *tag, float message)
{
  log(ERROR, tag, message);
}

void Logger::error(const char *tag, double message)
{
  log(ERROR, tag, message);
}

void Logger::log(LogLevel level, const char *tag, const char *message)
{
  Serial.print(getLevelString(level));
  Serial.print(" [");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

void Logger::log(LogLevel level, const char *tag, int message)
{
  Serial.print(getLevelString(level));
  Serial.print(" [");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

void Logger::log(LogLevel level, const char *tag, float message)
{
  Serial.print(getLevelString(level));
  Serial.print(" [");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

void Logger::log(LogLevel level, const char *tag, double message)
{
  Serial.print(getLevelString(level));
  Serial.print(" [");
  Serial.print(tag);
  Serial.print("] ");
  Serial.println(message);
}

const char *Logger::getLevelString(LogLevel level)
{
  switch (level)
  {
  case DEBUG:
    return "[DEBUG]";
  case INFO:
    return "[INFO]";
  case WARN:
    return "[WARN]";
  case ERROR:
    return "[ERROR]";
  default:
    return "[UNKNOWN]";
  }
}
