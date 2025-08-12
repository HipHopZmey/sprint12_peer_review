#include <algorithm>
#include <cassert>
#include <iostream>
#include <string_view>
#include <optional>
#include <vector>

using namespace std;

class Person;
class DBLogLevel;
class DBConnector;
class DBHandler;
class DBQuery;

struct LoadingParams {
    bool CheckParams() const {
        return db_allow_exceptions.has_value()   && db_name.has_value()
            && db_connection_timeout.has_value() && db_log_level.has_value()
            && age_rage.has_value() && name_filter.has_value();
    }
    LoadingParams& AllowExceptions(bool allowed) {
        db_allow_exceptions = allowed;
    }
    LoadingParams& SetDBName(string_view db_name) {
        this->db_name = db_name;
    }
    LoadingParams& SetDBConnectionTimeout(int db_connection_timeout) {
        this->db_connection_timeout = db_connection_timeout;
    }
    LoadingParams& SetDBLogLevel(DBLogLevel db_log_level) {
        this->db_log_level = db_log_level;
    }
    LoadingParams& SetAgeRage(int min_age, int max_age) {
        this->age_rage = { min_age, max_age };
    }
    LoadingParams& SetNameFilter(string_view name_filter) {
        this->name_filter = name_filter;
    }
    optional<bool> db_allow_exceptions;
    optional<string_view> db_name;
    optional<int> db_connection_timeout;
    optional<DBLogLevel> db_log_level;
    optional<pair<int, int>> age_rage;
    optional< string_view> name_filter;
};

vector<Person> LoadPersons(const LoadingParams& params) {
    assert(params.CheckParams());

    DBConnector connector(params.db_allow_exceptions.value(), params.db_log_level.value());
    DBHandler db;

    if (params.db_name.value().starts_with("tmp."s)) {
        db = connector.ConnectTmp(params.db_name.value(), params.db_connection_timeout.value());
    }
    else {
        db = connector.Connect(params.db_name.value(), params.db_connection_timeout.value());
    }
    if (!params.db_allow_exceptions && !db.IsOK()) {
        return {};
    }

    ostringstream query_str;
    query_str << "from Persons "s
        << "select Name, Age "s
        << "where Age between "s << params.age_rage.value().first 
        << " and "s 
        << params.age_rage.value().second << " "s
        << "and Name like '%"s << db.Quote(params.name_filter.value()) << "%'"s;
    DBQuery query(query_str.str());

    vector<Person> persons;
    for (auto [name, age] : db.LoadRows<string, int>(query)) {
        persons.push_back({ move(name), age });
    }
    return persons;
}