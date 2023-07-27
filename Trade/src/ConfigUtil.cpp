#include <string>
#include <fstream>
#include <iostream>
#include "common/ConfigUtil.h"

// const char* configFilename = "program.cfg";
// const std::string title = "dcel2_5672";

ConfigUtil::ConfigUtil(std::string title)
    : title(title)
{
    
}

void ConfigUtil::loadConfig()
{
    Env& env = Env::getInstance();
    m_Config = {};
    m_Config.pidfile = env[title]["pidfile"];
    m_Config.type = env[title]["type"];
    m_Config.port = env[title]["port"];
    m_Config.multicast_addr = env[title]["multicast_addr"];
    m_Config.source_addr = env[title]["source_addr"];
    m_Config.local_addr = env[title]["local_addr"];
    m_Config.forward_addr = env[title]["forward_addr"];
    m_Config.cpuid = env[title]["cpuid"];
    m_Config.sleeptime = env[title]["sleeptime"];
    m_Config.datdir = env[title]["datdir"];
}

void ConfigUtil::printConfig()
{
    std::cout << m_Config << std::endl;
}