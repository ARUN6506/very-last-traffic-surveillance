#include "DBManager.h"
#include <iostream>

DBManager::DBManager(const std::string& dbPath)
    : db_(nullptr), dbPath_(dbPath) {}

DBManager::~DBManager() {
    if (db_) sqlite3_close(db_);
}

bool DBManager::init() {
    int rc = sqlite3_open(dbPath_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS violations(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user TEXT,
            time TEXT,
            place TEXT,
            plate TEXT,
            points INTEGER
        );
    )";
    char* errMsg = nullptr;
    rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DBManager::addViolation(const std::string& user,
    const std::string& time,
    const std::string& place,
    const std::string& plate,
    int points) {
    const char* sql = "INSERT INTO violations (user, time, place, plate, points) VALUES (?, ?, ?, ?, ?);"; // Corrected SQL
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, time.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, place.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, plate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, points);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt); // Added finalize call for success case.
    return true;
}