#include "common/customListener.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <iomanip>
#include <time.h>
#include <sched.h>  
#include <map>
#include <fstream>
#include <iostream>
#include <vector>
#include <errno.h>

#define PORT 10170

bool bind_cpu(int cpu_id, pthread_t thd_id)
{
    int         cpu = (int)sysconf(_SC_NPROCESSORS_ONLN); // number of CPUs
    cpu_set_t   cpu_info;

    if (cpu < cpu_id)
    {
        return false;
    }

    CPU_ZERO(&cpu_info);
    CPU_SET(cpu_id, &cpu_info);

    if (pthread_setaffinity_np(thd_id, sizeof(cpu_set_t), &cpu_info) != 0)
    {
        return false;
    }

    return true;
}

void pexit(const char* msg, int sockfd)
{
    if (msg)
        printf("%s\n", msg);

    if (sockfd)
        close(sockfd);

    exit(-1);
}

void CustomListener::setDataManager(CMyDataManager* dataManager)
{
    this->dataManager = dataManager;
}

void CustomListener::setDataTaskManager(CDataTaskManager* dataTaskManager)
{
    this->dataTaskManager = dataTaskManager;
}

void CustomListener::setConfig(ConfigUtil* config)
{
    this->config = config;
}

void CustomListener::exitOnKeyboard()
{
    printf("[CL]press q to exit ...\n");

    char ch = 0;
    while ('q' != ch && 'Q' != ch)
    {
        ch = getchar();
    }

    exitFlag = true;
}

void CustomListener::processData(FILE* mFile)
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

void CustomListener::start()
{
    bool isTestForwarding = false;
    bool isForwardingMode = ((config->m_Config.source_addr) != "0.0.0.0");

    if (!isForwardingMode) return;
    // dataManager and config must exist
    // Step 1: init with config info
    time_t now_time = time(NULL);
    tm* t_tm = localtime(&now_time);
    FILE* gFile = NULL;

    // save data to file
    // char gDataFile[1024];
    // sprintf(gDataFile, "%s/%04d%02d%02d_%02d%02d%02d.dat", const_cast<char*>(config->m_Config.datdir.c_str()), 
    //         1900+t_tm->tm_year, t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
    // gFile = fopen(gDataFile, "a+");
    // if (NULL == gFile)
    // {
    //     std::cout << "Error: Failed to open file: " << gDataFile << "...\n";
    //     exit(-1);
    // }

    // bind CPU
    if (false == bind_cpu(config->m_Config.cpuid, pthread_self()))
    {
        pexit("Error: CL bind cpu error.\n", 0);
    }

    // create UDP socket
    // int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Custom listener sock %d\n", sockfd);

    // init server address
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; // receive from all network adapter locally
    servaddr.sin_port = htons(config->m_Config.port); // recieving port

    // 设定SO_REUSEADDR，允许多个应用绑定同一个本地端口接收数据包
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
        pexit("Error: set SO_REUSEADDR error", sockfd);

    // 将服务器地址和服务器套接字绑定
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        pexit("Error: bind socket error", sockfd);

    // struct in_addr addr = { 0 };
    // addr.s_addr = inet_addr(const_cast<char*>(config->m_Config.local_addr.c_str()));
    // if (-1 == setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr)))
    //     pexit("Error: set error IP_MULTICAST_IF", sockfd);

    // 设置回环许可
    // int loop = 1;
    // if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
    //     pexit("Error: setsockopt():IP_MULTICAST_LOOP", sockfd);

    // // 设置要加入组播的地址
    // struct ip_mreq mreq;
    // bzero(&mreq, sizeof(struct ip_mreq));
    // mreq.imr_multiaddr.s_addr = inet_addr(const_cast<char*>(config->m_Config.multicast_addr.c_str()));
    // mreq.imr_interface.s_addr = addr.s_addr;

    // // 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息
    // if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1)
    //     pexit("Error: setsockopt():IP_ADD_MEMBERSHIP", sockfd);

    // TCP only
    if (listen(sockfd, 3) < 0)
        pexit("Error: listen error", sockfd);
    int new_socket;
    int addrlen = sizeof(servaddr);
    if ((new_socket = accept(sockfd, (struct sockaddr*)&servaddr, (socklen_t*)&addrlen)) < 0){
        pexit("Error: accept error", sockfd);
    }
    else {
        std::cout << "accept success!" << std::endl;
    }

    // 设置非阻塞recv
    int flag;
    if (flag = fcntl(sockfd, F_GETFL, 0) < 0)
        pexit("Error: get flag error", sockfd);
    flag |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flag) < 0)
        pexit("Error: set flag error", sockfd);
    fcntl(new_socket, F_SETFL, O_NONBLOCK);

    // exit on keypress
    exitFlag = false;
    aboutToCloseFlag = false;
    // std::thread t1(&CustomListener::exitOnKeyboard, this);

    // data processing
    // std::thread t2(&NhMdListener::processData, this, gFile);

    // client address and length
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    bzero(&cliaddr, sizeof(cliaddr));

    Mydata mydata = { 0 };
    timespec ts;
    timespec prg_start_ts;
    timespec last_md_ts;
    unsigned int seq = 0;
    unsigned int failedSinceMarketClose = 0;
    double exchangeTime;

    clock_gettime(CLOCK_REALTIME, &prg_start_ts);
    if (isTestForwarding)
    {
        // char test_str[MAXLINE];
        std::cout << "test forward recv" << std::endl;
        while (!exitFlag)
        {
            // UDP
            // int n = recvfrom(sockfd, mydata.datas, MAXLINE, 0, (struct sockaddr*)&cliaddr, &len);
            // TCP
            // std::cout << "b4 read" << std::endl;
            int n = recv(new_socket, mydata.datas, MAXLINE, 0);
            // std::cout << "aft read" << std::endl;
            if (n > 0)
            {
                clock_gettime(CLOCK_REALTIME, &ts);
                std::cout << "[" << (long long)ts.tv_sec << "." << ts.tv_nsec << "] " << n << " [SH]message " << mydata.datas << ", from address " << inet_ntoa(cliaddr.sin_addr) << std::endl;
            } else {
                // std::cout << n << " [SH]message failed with errno=" << strerror(errno) << std::endl;
                continue;
            }
            mydata.len = n;
            clock_gettime(CLOCK_REALTIME, &mydata.time);
            dataTaskManager->push(mydata);
        }
    } else
    {
        while (!exitFlag)
        {
            // clock_gettime(CLOCK_REALTIME, &ts);
            // std::cout << "[" << (long long)ts.tv_sec << "." << ts.tv_nsec << "] CL start loop\n";

            // int n = recvfrom(sockfd, mydata.datas, MAXLINE, 0, (struct sockaddr*)&cliaddr, &len);
            int n = read(new_socket, mydata.datas, MAXLINE);

            // std::cout << "custom listener receive size=" << n << "<-" << inet_ntoa(cliaddr.sin_addr) << "\n";
            if (n < 0)
            {
                if (aboutToCloseFlag)
                {
                    clock_gettime(CLOCK_REALTIME, &ts);
                    
                    if ((long long)(ts.tv_sec - last_md_ts.tv_sec) > 10)
                    {
                        exitFlag = true;
                        printf("Afternoon trading session is over. Shutting down CL\n");
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
                // failedAll++;
                // if (failedAll > 10000)
                // {
                //     exitFlag = true;
                //     printf("Not getting anything from sender. Shutting down CL\n");
                // }
                // std::cout << "[FWD] Receive forward failed with " << strerror(errno) << ", message from " << inet_ntoa(cliaddr.sin_addr) << "\n";
                continue;
            }
            // dont need this for TCP connection
            // if (0 != strcmp(inet_ntoa(cliaddr.sin_addr), const_cast<char*>(config->m_Config.source_addr.c_str())))
            // {
            //     std::cout << "custom listener address not match " << inet_ntoa(cliaddr.sin_addr) << "\n";
            //     continue;
            // }
            
            if ((n != (_ftdc_struct_mdl2Size + sizeof(unsigned int))) && (n != (_efh3_2_fut_lev2Size + sizeof(unsigned int))))
                continue;

            mydata.len = n - sizeof(unsigned int);
            clock_gettime(CLOCK_REALTIME, &mydata.time);
            std::cout << "[CL" << *((unsigned int*)(mydata.datas + mydata.len)) << "]" << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << " " << getTextField(mydata, DATA_FIELD_TIME) << ":" << getField(mydata, DATA_FIELD_MS) << std::endl;
            // logDataID(&mydata, "CL");
            // dataManager->SaveData(mydata);
            dataTaskManager->push(mydata);

            exchangeTime = getField(mydata, DATA_FIELD_TIME_H)*10000+getField(mydata, DATA_FIELD_TIME_M)*100+getField(mydata, DATA_FIELD_TIME_S);
            aboutToCloseFlag = ((exchangeTime >= MARKET_CLOSE_ST) && (exchangeTime <= MARKET_CLOSE_ET));
            aboutToCloseFlag |= ((exchangeTime >= MARKET_CLOSE_OVN_ST) && (exchangeTime <= MARKET_CLOSE_OVN_ET));
            if (aboutToCloseFlag)
            {
                printf("exchange is about to close: %f\n", exchangeTime);
                clock_gettime(CLOCK_REALTIME, &last_md_ts);
            }// clock_gettime(CLOCK_REALTIME, &ts);
            // std::cout << "[" << (long long)ts.tv_sec << "." << ts.tv_nsec << "] CL done loop\n";

            seq++;
        }
    }
    // UDP
    // close(sockfd);

    // TCP
    close(new_socket);
    shutdown(sockfd, SHUT_RDWR);
    // t1.join();
    // t2.join();
    // fclose(gFile);
    std::cout << "Gracefully shutdown remote listener!" << "...\n";
}