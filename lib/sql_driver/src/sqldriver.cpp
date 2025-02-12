#include <cassert>
#include <cstring>
#include <format>
#include <sqldriver.h>
#include <sqlite3.h>
#include <string>

using namespace std;

void SqlDriver::init() {
  sqlite3_open(_name, &_instance);

  if (_instance == NULL)
    throw SqlDriverError(sqlite3_errmsg(_instance));
}

sqlite3_stmt *SqlDriver::stage(const char *statement) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(_instance, statement, -1, &stmt, nullptr);

  if (stmt == NULL) {
    sqlite3_finalize(stmt); /** Clean up bad stmt */
    throw SqlDriverError(format("The staging statement provided is incorrect, "
                                "the previous statement was probably not "
                                "flushed or the syntax is incorrect\nGot={}",
                                statement)
                             .c_str());
  }

  return stmt;
}

void SqlDriver::run_query(sqlite3_stmt *statement) {
  while (int step = sqlite3_step(statement) != SQLITE_DONE) {

    if (sqlite3_errcode(_instance) < 0) {
      throw SqlDriverError(format("An error occured while stepping \nGot={}",
                                  sqlite3_errmsg(_instance))
                               .c_str());
    };

    for (int i = 0; i < sqlite3_column_count(statement); ++i) {
      switch (sqlite3_column_type(statement, i)) {

      case SQLITE_TEXT: {
        sqlite3_column_text(statement, i);
        break;
      }
      case SQLITE_INTEGER: {
        sqlite3_column_int(statement, i);
        break;
      }
      case SQLITE_FLOAT: {
        sqlite3_column_double(statement, i);
        break;
      }
      default: {
        throw SqlDriverError("Row type not implemented yet");
      }
      }
    }
  }

  if (sqlite3_finalize(statement) > 0) {
    throw SqlDriverError(
        format("An error occured while calling finalize \nGot={}",
               sqlite3_errmsg(_instance))
            .c_str());
  };
}

void SqlDriver::dispatch(const char *query) {
  auto stmt = stage(query);
  run_query(stmt);
}

void SqlDriver::close() {
  if (sqlite3_close(_instance)) {
    throw SqlDriverError(
        format("An error occured while calling finalize \nGot={}",
               sqlite3_errmsg(_instance))
            .c_str());
  }
}
