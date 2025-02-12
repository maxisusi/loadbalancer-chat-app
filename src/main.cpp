#include "logger.hpp"
#include "protocat.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <sched.h>
#include <sqldriver.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define DB_NAME "db_test.db"

int main() {

  SqlDriver sql_instance("db_test.db");
  sql_instance.init();
  sqlite3_stmt *create_stmt = sql_instance.stage(
      "CREATE TABLE IF NOT EXISTS Person (name varchar(255))");
  sql_instance.run_query(create_stmt);

  auto insert_stmt =
      sql_instance.stage("INSERT INTO Person (name) VALUES ('Max')");

  sql_instance.run_query(insert_stmt);
  auto select_stmt = sql_instance.stage("SELECT * FROM Person");

  sql_instance.run_query(select_stmt);

  sql_instance.close();

  return 0;
}
