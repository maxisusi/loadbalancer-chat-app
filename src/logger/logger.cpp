#include <ctime>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>

enum LogLevel { INFO, WARNING, ERROR, CRITICAL };

class Logger {
  bool disable = false;

public:
  Logger() {};

public:
  void log(LogLevel loglvl, std::string message) {

    auto timestamp = time(NULL);
    struct tm date_time = *localtime(&timestamp);
    char date[50];
    strftime(date, 50, "[%I:%M:%S]", &date_time);

    switch (loglvl) {
    case LogLevel::CRITICAL:
      std::cout << "[CRITICAL]" << date << " " << message << std::endl;
      break;

    case LogLevel::ERROR:
      std::cout << "[ERROR]" << date << " " << message << std::endl;
      break;

    case LogLevel::INFO:
      std::cout << "[INFO]" << date << " " << message << std::endl;
      break;

    case LogLevel::WARNING:
      std::cout << "[WARNING]" << date << " " << message << std::endl;
      break;
    };
  }
};
