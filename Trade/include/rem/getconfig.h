#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include "EESQuoteDemo.h"
#include "json/json.hpp"
using nlohmann::json;


bool Loadjson(EqsTcpInfo& tcpInfo, EqsLoginParam& loginParam, EesEqsIntrumentType& instrType, std::vector<std::string>& pSymbol);
