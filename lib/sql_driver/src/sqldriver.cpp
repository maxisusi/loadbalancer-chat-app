#include <cassert>
#include <cstring>
#include <format>
#include <sqldriver.h>
#include <sqlite3.h>
#include <string>

using namespace std;

void SqlDriver::init() {

  sqlite3_open(name, &instance);
  if (instance == NULL)
    throw SqlDriverError(sqlite3_errmsg(instance));
}
sqlite3_stmt *SqlDriver::stage(const char *statement) {

  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(instance, statement, -1, &stmt, nullptr);

  if (stmt == NULL) {
    // Clean up the stmt
    sqlite3_finalize(stmt);
    throw SqlDriverError(format("The staging statement provided is incorrect, "
                                "the previous statement was probably not "
                                "flushed or the syntax is incorrect\nGot={}",
                                statement)
                             .c_str());
  }

  return stmt;
}

void SqlDriver::run_query(sqlite3_stmt *statement) {
  // Logger log;

  while (int step = sqlite3_step(statement) != SQLITE_DONE) {

    if (sqlite3_errcode(instance) < 0) {
      throw SqlDriverError(format("An error occured while stepping %c\nGot: ",
                                  sqlite3_errmsg(instance))
                               .c_str());
    };

    int nums_col = sqlite3_column_count(statement);
    for (int i = 0; i < nums_col; ++i) {
      switch (sqlite3_column_type(statement, i)) {
      case SQLITE_TEXT: {
        std::string column_text = std::string(
            reinterpret_cast<const char *>(sqlite3_column_text(statement, i)));
        break;
      }
      case SQLITE_INTEGER: {
        auto column = std::to_string(sqlite3_column_int(statement, i));
        break;
      }

      case SQLITE_FLOAT: {
        auto column = sqlite3_column_double(statement, i);
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
        format("An error occured while calling finalize %c\nGot: ",
               sqlite3_errmsg(instance))
            .c_str());
  };
}
void SqlDriver::close() {
  if (sqlite3_close(instance)) {
    throw SqlDriverError(
        format("An error occured while calling finalize %c\nGot: ",
               sqlite3_errmsg(instance))
            .c_str());
  }
}
