#ifndef LOGLEVEL_H
#define LOGLEVEL_H

#include <string>

enum LogLevel { INFO, WARNING, ERROR, CRITICAL };
class Logger {
public:
  Logger();

public:
  void log(LogLevel loglvl, std::string message);
};

#endif
