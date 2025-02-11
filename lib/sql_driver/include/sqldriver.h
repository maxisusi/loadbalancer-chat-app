#ifndef SQL_DRIVER_H
#define SQL_DRIVER_H

#include <sqlite3.h>
#include <stdexcept>

/**
 * Define Result Code
 *
 * Future Error code may be added
 **/
#define SQLD_ERROR 1     /* Generic Error */
#define SQLD_SET_ERROR 2 /* Error generated while stepping */
#define SQLD_RUN_ERROR 3 /* Runtime Error when calling `run` */

using namespace std;

class SqlDriver {

private:
  const char *name;
  sqlite3 *instance;

public:
  SqlDriver(const char *db_n) : name(db_n) {};

  void init();
  void run_query(sqlite3_stmt *statement);
  void close();

  sqlite3_stmt *stage(const char *statement);
};

class SqlDriverError : public exception {
private:
  std::string message;

public:
  SqlDriverError();
  SqlDriverError(const char *msg) { message += msg; };

  virtual const char *what() const noexcept {
    if (message.length() > 1) {
      return message.c_str();
    }
    return "An unexpected SqlDriver error happened";
  };
};

class SqlDriverBadStatment : public exception {
public:
  virtual const char *what() const noexcept {
    return "The staging statement provided is incorrect";
  };
};

class SqlDriverBadAllocation : public exception {
private:
  string message = "SqlDriverBadAllocation: ";

public:
  SqlDriverBadAllocation();
  SqlDriverBadAllocation(const char *msg) { message += msg; };

  virtual const char *what() const noexcept {
    if (message.length() > 1) {
      return message.c_str();
    }
    return "The database couldn't be allocated in memory";
  };
};

class SqlDriverStatmentEval : public exception {
  string message;
  int error_code;

public:
  SqlDriverStatmentEval();
  SqlDriverStatmentEval(const char *msg) { message += msg; };

  virtual const char *what() const noexcept {
    if (message.length() > 1) {
      return message.c_str();
    }
    return "An unexpected SqlDriver error happened";
  };
};
#endif
