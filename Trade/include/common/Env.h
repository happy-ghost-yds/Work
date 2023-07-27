#pragma once
#include <iostream>
#include <fstream>
#include "json/json.hpp"

using json = nlohmann::json;

class Env : public json {
public:
    static Env& getInstance(){
        static Env instance;
        return instance;
    };
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        file >> *this;
    }

private:
    Env() = default;
    ~Env() = default;
    Env(const Env&) = delete;
    Env& operator=(const Env&) = delete;
};