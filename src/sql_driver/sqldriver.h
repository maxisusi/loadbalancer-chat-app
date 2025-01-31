#include <sqlite3.h>
#include <string>

class SqlDriver {

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
