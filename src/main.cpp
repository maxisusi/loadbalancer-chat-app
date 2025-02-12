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

  SqlDriver sqlc("db_test.db");

  sqlc.dispatch("CREATE TABLE IF NOT EXISTS Person (name varchar(255))");
  sqlc.dispatch("INSERT INTO Person (name) VALUES ('Max')");
  sqlc.dispatch("SELECT * FROM Person");

  sqlc.close();

  return 0;
}
