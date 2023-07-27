#include "nhmd/NhMdListener.h"

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

#define PORT 10170
#define INSTRUMENT1 "rb2305"
#define INSTRUMENT2 "i2305"

// bool bind_cpu(int cpu_id, pthread_t thd_id)
// {
//     int         cpu = (int)sysconf(_SC_NPROCESSORS_ONLN); // number of CPUs
//     cpu_set_t   cpu_info;

//     if (cpu < cpu_id)
//     {
//         return false;
//     }

//     CPU_ZERO(&cpu_info);
//     CPU_SET(cpu_id, &cpu_info);

//     if (pthread_setaffinity_np(thd_id, sizeof(cpu_set_t), &cpu_info) != 0)
//     {
//         return false;
//     }

//     return true;
// }

// void pexit(const char* msg, int sockfd)
// {
//     if (msg)
//         printf("%s\n", msg);

//     if (sockfd)
//         close(sockfd);

//     exit(-1);
// }

void NhMdListener::setConfig(ConfigUtil* config)
{
    this->config = config;
}

void NhMdListener::exitOnKeyboard()
{
    printf("[LL]press q to exit ...\n");

    char ch = 0;
    while ('q' != ch && 'Q' != ch)
    {
        ch = getchar();
    }

    exitFlag = true;
}

void NhMdListener::processData(FILE* mFile)
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

void NhMdListener::start()
{
    // dataManager and config must exist
    bool isTestForwarding = false; //false; // true for forwarding test, false for normal script
    bool isForwardingMode = ((config->m_Config.forward_addr) != "0.0.0.0");

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

        pexit("Error: LL bind cpu error.\n", 0);
    }

    // create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Local listener sock %d\n", sockfd);

    // create TCP socket for forward
    int tcpsockfd;
    int tcp_client_fd;

    // init server address
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // receive from all network adapter locally
    servaddr.sin_port = htons(config->m_Config.port); // recieving port

    // 设定SO_REUSEADDR，允许多个应用绑定同一个本地端口接收数据包
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
        pexit("Error: set SO_REUSEADDR error", sockfd);

    // 绑卡
    // struct ifreq freq;
    // strcpy(freq.ifr_name, "ens1f1");
    // if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (char*)&freq, sizeof(freq)) < 0)
    //     pexit("Error: set SO_BINDTODEVICE error", sockfd);

    // 将服务器地址和服务器套接字绑定
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        pexit("Error: bind socket error", sockfd);

    struct in_addr addr = { 0 };
    addr.s_addr = inet_addr(const_cast<char*>(config->m_Config.local_addr.c_str()));
    if (-1 == setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr)))
        pexit("Error: set error IP_MULTICAST_IF", sockfd);

    // 设置回环许可
    int loop = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
        pexit("Error: setsockopt():IP_MULTICAST_LOOP", sockfd);

    // 设置要加入组播的地址
    struct ip_mreq mreq;
    bzero(&mreq, sizeof(struct ip_mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(const_cast<char*>(config->m_Config.multicast_addr.c_str()));
    mreq.imr_interface.s_addr = addr.s_addr;

    // 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1)
        pexit("Error: setsockopt():IP_ADD_MEMBERSHIP", sockfd);

    // 设置非阻塞recv
    // printf("about to setup nonblock\n");
    int flag;
    if (flag = fcntl(sockfd, F_GETFL, 0) < 0)
        pexit("Error: get flag error", sockfd);
    flag |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flag) < 0)
        pexit("Error: set flag error", sockfd);

    // resender setup
    // int sockfd_resend = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr_resend;
    if (isForwardingMode)
    {
        memset(&servaddr_resend, 0, sizeof(servaddr_resend));
        servaddr_resend.sin_family = AF_INET;
        servaddr_resend.sin_port = htons(PORT);
        servaddr_resend.sin_addr.s_addr = inet_addr(const_cast<char*>(config->m_Config.forward_addr.c_str()));
    }

    bool tcp_connected = false;
    if (isForwardingMode)
    {
        while (!tcp_connected)
        {
            tcpsockfd = socket(AF_INET, SOCK_STREAM, 0);
            if ((tcp_client_fd = connect(tcpsockfd, (struct sockaddr*) &servaddr_resend, sizeof(servaddr_resend)))<0)
            {
            std::cout << "Error: connect error" << std::endl;
            close(tcpsockfd);
            usleep(1000*1000);
            } else {
                tcp_connected = true;
                int flag;
                if (flag = fcntl(tcpsockfd, F_GETFL, 0) < 0)
                    pexit("Error: get flag error", sockfd);
                flag |= O_NONBLOCK;
                if (fcntl(tcpsockfd, F_SETFL, flag) < 0)
                    pexit("Error: set flag error", sockfd);
                std::cout << "connect success!" << std::endl;
            }
        }
    }

    // exit on keypress
    exitFlag = false;
    aboutToCloseFlag = false;
    // std::thread t1(&NhMdListener::exitOnKeyboard, this);

    // data processing
    // std::thread t2(&NhMdListener::processData, this, gFile);

    // client address and length
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    Mydata mydata = { 0 };
    char test_str[MAXLINE];
    timespec ts;
    timespec prg_start_ts;
    timespec last_md_ts;
    tm tm_datetime;
    unsigned int seq = 0;
    unsigned int failedSinceMarketClose = 0;
    double exchangeTime;

    clock_gettime(CLOCK_REALTIME, &prg_start_ts);
    if (isTestForwarding)
    {
        std::cout << "test forward sender" << std::endl;
        int n_message = 10;
        while (n_message > 0)
        {
            clock_gettime(CLOCK_REALTIME, &ts);
            sprintf(test_str, "hello data %lld.%ld", (long long)ts.tv_sec, ts.tv_nsec);
            // int n = sendto(sockfd, test_str, 256, 0, (struct sockaddr*) &servaddr_resend, sizeof(servaddr_resend));
            // std::cout << "b4 send" << std::endl;
            int n = send(tcpsockfd, test_str, MAXLINE, 0);
            // std::cout << "aft send" << std::endl;
            if (n > 0)
            {
                std::cout << "[FWD] Sent a message (size=" << n << ") " << test_str << " to " << inet_ntoa(servaddr_resend.sin_addr) << std::endl;
            } else {
                std::cout << "[FWD] Sent failed with errno=" << strerror(errno) << std::endl;
            }
            usleep(250*1000); //config->m_Config.sleeptime
            n_message--;
        }
    } else 
    {
        printf("start recv data!\n");
    while (!exitFlag)
        {
            int n = recvfrom(sockfd, mydata.datas, MAXLINE, 0, (struct sockaddr*)&cliaddr, &len);
            if (n < 0){
                if (aboutToCloseFlag)
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
            // printf("recv %d\n", n);
            if (0 != strcmp(inet_ntoa(cliaddr.sin_addr), const_cast<char*>(config->m_Config.source_addr.c_str())))
            {
                std::cout << "local listener address not match " << inet_ntoa(cliaddr.sin_addr) << "\n";
                continue;
            }
            if ((n != _ftdc_struct_mdl2Size) && (n != _efh3_2_fut_lev2Size))
                continue;
            
            memcpy(&mydata.datas[n], (char*)&seq, sizeof(unsigned int)); 
            mydata.len = n;
            if ((getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT1) && (getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) != INSTRUMENT2))
            {
                // std::cout << "symbol not match " << getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << "\n";
                continue;
            }
            
            // printf("%s\n", getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME).c_str());
            // send data to another location
            if (tcp_connected && isForwardingMode)
            {
                for (int rt=0;rt<1;rt++)
                {
                    int n2 = send(tcpsockfd, mydata.datas, n+sizeof(unsigned int), 0);
                    // int n2 = sendto(sockfd, mydata.datas, n+sizeof(unsigned int), 0, (struct sockaddr*) &servaddr_resend, sizeof(servaddr_resend));
                    if (n2 < 0)
                    {
                        std::cout << "[FWD] Forward failed with " << strerror(errno) << "\n";
                    }
                }
            }

            clock_gettime(CLOCK_REALTIME, &mydata.time);
            std::cout << "[LL" << seq <<"]"<< getTextField(mydata, DATA_FIELD_INSTRUMENT_NAME) << " " << getTextField(mydata, DATA_FIELD_TIME) << ":" << getField(mydata, DATA_FIELD_MS) << std::endl;
            // logDataID(&mydata, "LL");
            // dataManager->SaveData(mydata);
            dataTaskManager->push(mydata);

            // prepare to close program
            exchangeTime = getField(mydata, DATA_FIELD_TIME_H)*10000+getField(mydata, DATA_FIELD_TIME_M)*100+getField(mydata, DATA_FIELD_TIME_S);
            aboutToCloseFlag = ((exchangeTime >= MARKET_CLOSE_ST) && (exchangeTime <= MARKET_CLOSE_ET));
            aboutToCloseFlag |= ((exchangeTime >= MARKET_CLOSE_OVN_ST) && (exchangeTime <= MARKET_CLOSE_OVN_ET));
            if (aboutToCloseFlag)
            {
                printf("exchange is about to close: %f\n", exchangeTime);
                clock_gettime(CLOCK_REALTIME, &last_md_ts);
            }

            ++seq;
        }
    }
    close(sockfd);
    // t1.join();
    // t2.join();
    // fclose(gFile);
    std::cout << "Gracefully shutdown local listener!" << "...\n";
}