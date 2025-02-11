#include "logger.hpp"
#include <cstring>
#include <sqldriver.h>
#include <sqlite3.h>
#include <string>

void SqlDriver::init() {

  sqlite3_open(name, &instance);
  if (instance == NULL)
    throw SqlDriverBadAllocation(sqlite3_errmsg(instance));
}
sqlite3_stmt *SqlDriver::stage(const char *statement) {

  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(instance, statement, -1, &stmt, nullptr);

  if (stmt == NULL)
    throw SqlDriverBadStatment();

  return stmt;
}

void SqlDriver::run_query(sqlite3_stmt *statement) {
  // Logger log;

  while (int step = sqlite3_step(statement) != SQLITE_DONE) {

    if (sqlite3_errcode(instance) == SQLITE_ERROR) {
      // log.log(LogLevel::INFO, sqlite3_errmsg(instance));
      // return SQLD_RUN_ERROR;
    }

    int nums_col = sqlite3_column_count(statement);

    for (int i = 0; i < nums_col; ++i) {
      switch (sqlite3_column_type(statement, i)) {
      case SQLITE_TEXT: {
        std::string column_text = std::string(
            reinterpret_cast<const char *>(sqlite3_column_text(statement, i)));
        // log.log(LogLevel::INFO, column_text);
        break;
      }
      case SQLITE_INTEGER: {
        auto column = std::to_string(sqlite3_column_int(statement, i));
        // log.log(LogLevel::INFO, column);
        break;
      }

      case SQLITE_FLOAT: {
        auto column = sqlite3_column_double(statement, i);
        // log.log(LogLevel::INFO, std::to_string(column));
        break;
      }
      default: {
        // log.log(LogLevel::WARNING, "Unhandled table type");
        // return SQLD_ERROR;
      }
      }
    }
  }

  sqlite3_finalize(statement); // TODO: Handle error

  // return 0;
}
void SqlDriver::close() {
  sqlite3_close(instance); // TODO: Handle error
}
