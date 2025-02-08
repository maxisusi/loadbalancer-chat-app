#ifndef SQL_DRIVER_H
#define SQL_DRIVER_H

#include <sqlite3.h>
#include <string>

using namespace std;

/**
 * Define Result Code
 *
 * Future Error code may be added
 **/
#define SQLD_ERROR 1     /* Generic Error */
#define SQLD_SET_ERROR 2 /* Error generated while stepping */
#define SQLD_RUN_ERROR 3 /* Runtime Error when calling `run` */

class SqlDriver {

  std::string db_name;
  sqlite3 *db_instance;

public:
  SqlDriver(string db_n);

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
