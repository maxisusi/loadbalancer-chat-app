#ifndef SQL_DRIVER_H
#define SQL_DRIVER_H

#include <sqlite3.h>
#include <string>

class SqlDriver {

  std::string db_name;
  sqlite3 *db_instance;

public:
  SqlDriver(std::string db_n);

public:
  int init();

public:
  sqlite3_stmt *stage(const char *statement);

public:
  int run_query(sqlite3_stmt *statement);

public:
  void close();
};

#endif
