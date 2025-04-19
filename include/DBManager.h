#pragma once
#include <string>
#include <sqlite3.h>

class DBManager {
public:
    explicit DBManager(const std::string& dbPath);
    ~DBManager();

    bool init();

    bool addViolation(const std::string& user,
        const std::string& time,
        const std::string& place,
        const std::string& plate,
        int points);

private:
    sqlite3* db_;
    std::string dbPath_;
};