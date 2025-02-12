#ifndef SQL_DRIVER_H
#define SQL_DRIVER_H

#include <exception>
#include <sqlite3.h>
#include <string>

using namespace std;

class SqlDriver {

private:
  /**  Db name */
  const char *_name = nullptr;
  sqlite3 *_db_instance = nullptr;

private:
  void init();
  sqlite3_stmt *stage(const char *statement);
  void dispatch_query(sqlite3_stmt *statement);

public:
  void run(const char *query);
  void close();
  SqlDriver(const char *db_n) : _name(db_n) { init(); };
};

class SqlDriverError : public exception {
private:
  string _message;

public:
  SqlDriverError();
  SqlDriverError(const char *message) { _message = message; };

  virtual const char *what() const noexcept {
    if (_message.length() > 1) {
      return _message.c_str();
    }
    return "An unexpected SqlDriver error happened";
  };
};

#endif
