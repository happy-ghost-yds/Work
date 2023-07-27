#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "common/ConfigUtil.h"
#include "md/CBaseUDPMulticastListener.h"

#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "i2305"

CBaseUDPMulticastListener::CBaseUDPMulticastListener(const std::string& multicast_address, const int multicast_port) :
        multicast_address_(multicast_address), multicast_port_(multicast_port) {}

CBaseUDPMulticastListener::~CBaseUDPMulticastListener() {}

void CBaseUDPMulticastListener::setDataManager(CMyDataManager* dataManager)
{
    this->dataManager = dataManager;
}

void CBaseUDPMulticastListener::setDataTaskManager(CDataTaskManager* dataTaskManager)
{
    this->dataTaskManager = dataTaskManager;
}

void CBaseUDPMulticastListener::setConfig(ConfigUtil* config)
{
    this->config = config;
}


void CBaseUDPMulticastListener::exitOnKeyboard()
{
    printf("[LL]press q to exit ...\n");

    char ch = 0;
    while ('q' != ch && 'Q' != ch)
    {
        ch = getchar();
    }

    exitFlag = true;
}

void CBaseUDPMulticastListener::processData(FILE* mFile)
{
    while (!exitFlag)
    {
        // this shall be delayed for performance at trading time
        // as it locks the dataManager object
        vector<Mydata>* datalist = dataManager->GetDatas();
        if (nullptr != datalist)
        {
            for (auto iter = datalist->begin(); iter != datalist->end(); iter++)
            {
                Mydata data = *iter;
                fwrite(&data.time, sizeof(timespec),
                    1, mFile);
                
                fwrite(&data.len, sizeof(int),
                    1, mFile);
                
                fwrite(&data.datas, data.len,
                    1, mFile);
            }  
            
            delete datalist;
            fflush(mFile);
        }
        usleep(config->m_Config.sleeptime);
    }
}

void CBaseUDPMulticastListener::Start() {
   
    // Step 1: init with config info
    time_t now_time = time(NULL);
    tm* t_tm = localtime(&now_time);
    FILE* gFile = NULL;

    // bind CPU
    if (false == bind_cpu(config->m_Config.cpuid, pthread_self()))
    {

        pexit("Error: LL bind cpu error.\n", 0);
    }

    std::cout << "Start listening to multicast data..." << std::endl;

    // create UDP socket
    if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation error!" << std::endl;
        return;
    }

    // allow multiple sockets to use the same PORT number
    u_int yes = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        std::cerr << "Setsockopt reuseaddr error!" << std::endl;
        close(sockfd_);
        return;
    }

    // set up receiving address
    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(multicast_port_);

    // bind to receiving address
    if (bind(sockfd_, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0) {
        std::cerr << "Bind error!" << std::endl;
        close(sockfd_);
        return;
    }

    // set up the multicast group address
    struct ip_mreq mc_group;
    memset(&mc_group, 0, sizeof(mc_group));
    mc_group.imr_multiaddr.s_addr = inet_addr(multicast_address_.c_str());
    mc_group.imr_interface.s_addr = htonl(INADDR_ANY);

    // join the multicast group
    if (setsockopt(sockfd_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mc_group, sizeof(mc_group)) < 0) {
        std::cerr << "Join group error!" << std::endl;
        close(sockfd_);
        return;
    }

    std::cout << "Join multicast group: " << multicast_address_ << ":" << multicast_port_ << std::endl;



    // exit on keypress
    exitFlag = false;
    aboutToCloseFlag = false;

    Mydata mydata = { 0 };
    unsigned int seq = 0;
    timespec ts;
    timespec prg_start_ts;
    timespec last_md_ts;
    tm tm_datetime;

    struct sockaddr_in send_addr;
    int len = sizeof(send_addr);

    // start listening for data
    while (!exitFlag) {
        
        int n = recvfrom(sockfd_, mydata.datas, MAXLINE, 0,
                         (struct sockaddr *) &send_addr, reinterpret_cast<socklen_t *>(&len));
        
        if (n < 0) {
            if(aboutToCloseFlag)
            {
                clock_gettime(CLOCK_REALTIME, &ts);
                 if ((long long)(ts.tv_sec - last_md_ts.tv_sec) > 10)
                    {
                        exitFlag = true;
                        printf("Afternoon trading session is over. Shutting down LL\n");
                        dataTaskManager->shutdown();
                    }
            }
            if (seq == 0)
            {
                    clock_gettime(CLOCK_REALTIME, &ts);
                    if ((long long)(ts.tv_sec - prg_start_ts.tv_sec) > 60*60)
                    {
                        exitFlag = true;
                        printf("Not receving anything for 1hour. Not a trading day? Shutting down CL\n");
                    }
            }
                
            continue;
            
        } 
        if (0 != strcmp(inet_ntoa(send_addr.sin_addr), const_cast<char*>(config->m_Config.source_addr.c_str()))) {
        {
                std::cout << "local listener address not match " << inet_ntoa(send_addr.sin_addr) << "\n";
                continue;
        }
        memcpy(&mydata.datas[n], (char*)&seq, sizeof(unsigned int));
        mydata.len = n;
        if ((getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT1) && (getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT2))
        {
                // std::cout << "symbol not match " << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << "\n";
                continue;
        } 
        clock_gettime(CLOCK_REALTIME, &mydata.time);
       
    
        std::cout << "[LL" << seq <<"]"<< getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << " " << getTextField(mydata, DATA_FIELD_TIME) << ":" << getField(mydata, DATA_FIELD_MS) << std::endl;
            // logDataID(&mydata, "LL");
            // dataManager->SaveData(mydata);
        dataTaskManager->push(mydata);   
        ++seq;      
            
        }
    }

    // close the socket when finished
    close(sockfd_);
}
