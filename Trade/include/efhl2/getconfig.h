#pragma once
#include "json/json.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include "efhl2/i_sl_dce_api.h"
using namespace std;
using json = nlohmann::json;

bool parseJson(dce_api_param_t& param);
