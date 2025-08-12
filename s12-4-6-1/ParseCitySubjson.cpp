// Дана функция ParseCitySubjson, обрабатывающая JSON-объект со списком городов конкретной страны:
void ParseCitySubjson(vector<City>& cities, const Json& country_json, const vector<Language>& languages) {
    for (const auto& city_json : country_json.AsList()) {
        const auto& city_obj = city_json.AsObject();
        cities.push_back({ country_json["name"s].AsString(), city_obj["iso_code"s].AsString()
            , country_json.AsObject().country_obj["phone_code"s].AsString() + city_obj["phone_code"s].AsString()
            , country_json.AsObject().country_obj["name"s].AsString() 
            , country_json.AsObject().country_obj["iso_code"s].AsString()
            , country_json.AsObject().country_obj["time_zone"s].AsString()
            , languages });
    }
}

// ParseCitySubjson вызывается только из функции ParseCountryJson следующим образом:
void ParseCountryJson(vector<Country>& countries, vector<City>& cities, const Json& json) {
    for (const auto& country_json : json.AsList()) {
        const auto& country_obj = country_json.AsObject();
        countries.push_back({
            country_obj["name"s].AsString(),
            country_obj["iso_code"s].AsString(),
            country_obj["phone_code"s].AsString(),
            country_obj["time_zone"s].AsString(),
            });
        Country& country = countries.back();
        for (const auto& lang_obj : country_obj["languages"s].AsList()) {
            country.languages.push_back(FromString<Language>(lang_obj.AsString()));
        }
        ParseCitySubjson(cities, country_obj["cities"s], country.languages);
    }
}