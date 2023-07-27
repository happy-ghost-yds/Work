#ifndef _CONFIGUTIL_H_
#define _CONFIGUTIL_H_

#include <string>
#include <iostream>
#include "common/Env.h"

struct DCEDataConfig
{
    std::string pidfile; //暂时没有用
    int type;
    int port;
    std::string multicast_addr;
    std::string source_addr;
    std::string local_addr;
    std::string forward_addr;
    int cpuid;
    int sleeptime;
    std::string datdir;

    friend std::ostream& operator <<(std::ostream& os, DCEDataConfig const& config)
    {
        return os << "pidfile=" << config.pidfile << '\n'
                  << "type=" << config.type << '\n'
                  << "port=" << config.port << '\n'
                  << "multicast_addr=" << config.multicast_addr << '\n'
                  << "source_addr=" << config.source_addr << '\n'
                  << "local_addr=" << config.local_addr << '\n'
                  << "forward_addr=" << config.forward_addr << '\n'
                  << "cpuid=" << config.cpuid << '\n'
                  << "sleeptime=" << config.sleeptime << '\n'
                  << "datdir=" << config.datdir << '\n';
    }
};

class ConfigUtil
{
    public:
        ConfigUtil(std::string title);
        void loadConfig();
        void printConfig();

    public:
        DCEDataConfig m_Config;
        std::string title; // = "dcel2_5672";
};

#endif