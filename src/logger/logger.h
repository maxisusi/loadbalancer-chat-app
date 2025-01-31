#include <string>

enum LogLevel { INFO, WARNING, ERROR, CRITICAL };

class Logger {
  bool disable = false;

public:
  Logger() {};

public:
  void log(LogLevel loglvl, std::string message);
};
