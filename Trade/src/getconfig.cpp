#include"rem/getconfig.h"
#include"efhl2/getconfig.h"

bool LoadConfig(const std::string& filePath, EqsTcpInfo& tcpInfo, EqsLoginParam& loginParam, EesEqsIntrumentType& instrumentType, std::string& pSymbol) {
    std::ifstream ifs(filePath, std::ios::in);
    if (!ifs.is_open()) {
        return false;
    }

    std::string line;
    while (getline(ifs, line)) {
        size_t pos = line.find_first_of("=");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1, line.size() - pos - 1);
            if (key == "eqsId") {
                strncpy(tcpInfo.m_eqsId, value.c_str(), EES_EQS_ID_LEN);
                tcpInfo.m_eqsId[EES_EQS_ID_LEN] = '\0';
            }
            else if (key == "eqsIp") {
                strncpy(tcpInfo.m_eqsIp, value.c_str(), EES_EQS_ID_LEN);
                tcpInfo.m_eqsIp[EES_EQS_ID_LEN] = '\0';
            }
            else if (key == "Port") {
                tcpInfo.m_eqsPort = static_cast<unsigned short>(std::stoi(value));
            }
            else if (key == "LoginId") {
                strncpy(loginParam.m_loginId, value.c_str(), EES_EQS_USER_ID_LEN);
                loginParam.m_loginId[EES_EQS_USER_ID_LEN] = '\0';
            }
            else if (key == "Password") {
                strncpy(loginParam.m_password, value.c_str(), EES_EQS_PASSWORD_LEN);
                loginParam.m_password[EES_EQS_PASSWORD_LEN] = '\0';
            }
            else if (key == "IntrumentType") {
                instrumentType = static_cast<EesEqsIntrumentType>(value[0]);
            }
            else if (key == "pSymbol") {
                pSymbol = value;
            }
        }
    }

    return true;
}

bool Loadjson(EqsTcpInfo& tcpInfo, EqsLoginParam& loginParam, EesEqsIntrumentType& intrType, std::vector<std::string>& pSymbol)
{
    Env& env = Env::getInstance();
    // 解析 eqsId
    if (env["mdEES"].find("eqsId") == env["mdEES"].end() || !env["mdEES"]["eqsId"].is_string())
    {
        return false;
    }
    strncpy(tcpInfo.m_eqsId, env["mdEES"]["eqsId"].get<std::string>().c_str(), EES_EQS_ID_LEN);

    // 解析 eqsIp
    if (env["mdEES"].find("eqsIp") == env["mdEES"].end() || !env["mdEES"]["eqsIp"].is_string())
    {
        return false;
    }
    strncpy(tcpInfo.m_eqsIp, env["mdEES"]["eqsIp"].get<std::string>().c_str(), EES_EQS_ID_LEN);

    // 解析 Port
    if (env["mdEES"].find("Port") == env["mdEES"].end() || !env["mdEES"]["Port"].is_number_unsigned())
    {
        return false;
    }
    tcpInfo.m_eqsPort = env["mdEES"]["Port"].get<unsigned short>();

    // 解析 LoginId
    if (env["mdEES"].find("LoginId") == env["mdEES"].end() || !env["mdEES"]["LoginId"].is_string())
    {   
        return false;
    }
    strncpy(loginParam.m_loginId, env["mdEES"]["LoginId"].get<std::string>().c_str(), EES_EQS_USER_ID_LEN);

    // 解析 Password
    if (env["mdEES"].find("Password") == env["mdEES"].end() || !env["mdEES"]["Password"].is_string())
    {
        return false;
    }
    strncpy(loginParam.m_password, env["mdEES"]["Password"].get<std::string>().c_str(),  EES_EQS_PASSWORD_LEN);

    // 解析 IntrumentType
    if (env["mdEES"].find("IntrumentType") == env["mdEES"].end()|| !env["mdEES"]["IntrumentType"].is_number() )
    {
        return false;
    }
    int instrTypeInt = env["mdEES"]["IntrumentType"].get<int>();
    intrType = static_cast<EesEqsIntrumentType>(instrTypeInt);

    // 解析 pSymbol
    if (env["mdEES"].find("pSymbol") != env["mdEES"].end() && env["mdEES"]["pSymbol"].is_array())
    {
        for (const auto& symbol : env["mdEES"]["pSymbol"])
        {
            
            if (!symbol.is_string())
            {
                return false;
            }
            pSymbol.push_back(symbol.get<std::string>());
        }
    }

    return true;
}

bool parseJson(dce_api_param_t& param)
{
    Env& env = Env::getInstance();
    param.session_nums = env["mdEFHL2"]["session_nums"];
    string  eth_name   = env["mdEFHL2"]["eth_name"];
    strcpy(param.eth_name, eth_name.c_str());

    for(int i=0; i<param.session_nums; ++i)
    {   string dst_ip = env["mdEFHL2"]["sessions"][i]["dst_ip"];
        strcpy(param.sessions[i].dst_ip, dst_ip.c_str());
        
        param.sessions[i].dst_port = env["mdEFHL2"]["sessions"][i]["dst_port"];
    }
    param.cpu_nums = env["mdEFHL2"]["cpu_nums"];
    for(int i=0; i<param.cpu_nums; ++i) 
    {
        param.cpu_arrays[i] = env["mdEFHL2"]["cpu_arrays"][i];
    }
    return true;
}