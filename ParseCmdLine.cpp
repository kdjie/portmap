//============================================================================
// Name        : ParseCmdLine.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#include "ParseCmdLine.h"

#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

CParseCmdLine::CParseCmdLine(int argc, char *argv[])
    : m_strBindIp(""), m_uListenPort(0), m_strTargetIp(""), m_uTargetPort(0), m_uTimeout(300), m_fLimitBurst(10000.0), m_fLimitSpeed(10000.0), m_uLimitConnect(100000), m_bLogTerminal(false)
{
    int opt;
    int option_index = 0;
    const char *optstring = "vs:d:";
    struct option long_options[] = {
        {"timeout", required_argument, NULL, 1001},
        {"limit_burst", required_argument, NULL, 1002},
        {"limit_speed", required_argument, NULL, 1003},
        {"limit_connect", required_argument, NULL, 1004},
        {"logterm", no_argument, NULL, 1005},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc,
                              argv,
                              optstring,
                              long_options,
                              &option_index)) != -1)
    {
        switch (opt)
        {
        case 'v':
        {
            __printUsage();
            exit(0);
        }
        break;
        case 's':
        {
            __parseIpPort(optarg, m_strBindIp, m_uListenPort);
        }
        break;
        case 'd':
        {
            __parseIpPort(optarg, m_strTargetIp, m_uTargetPort);
        }
        break;
        case 1001:
        {
            m_uTimeout = atoi(optarg);
        }
        break;
        case 1002:
        {
            m_fLimitBurst = atof(optarg);
        }
        break;
        case 1003:
        {
            __parseLimitSpeed(optarg, m_fLimitSpeed);
        }
        break;
        case 1004:
        {
            m_uLimitConnect = atoi(optarg);
        }
        break;
        case 1005:
        {
            m_bLogTerminal = true;
        }
        break;
        }
    }
}
CParseCmdLine::~CParseCmdLine()
{
}

// 解析 "ip:port"
void CParseCmdLine::__parseIpPort(const char *p, std::string &strIp, uint16_t &uPort)
{
    const char *pSplit = strstr(p, ":");
    if (pSplit)
    {
        strIp = std::string(p, pSplit - p);
        uPort = atoi(pSplit + 1);
    }
}

// 解析 "1/s" "10/m" "100/h"
void CParseCmdLine::__parseLimitSpeed(const char *p, float &fLimitSpeed)
{
    const char *pSplit = strstr(p, "/");
    if (pSplit)
    {
        std::string v1 = std::string(p, pSplit - p);
        std::string v2 = pSplit + 1;

        if (v2 == "s")
        {
            m_fLimitSpeed = atof(v1.c_str());
        }
        else if (v2 == "m")
        {
            m_fLimitSpeed = atof(v1.c_str()) / 60;
        }
        else if (v2 == "h")
        {
            m_fLimitSpeed = atof(v1.c_str()) / 3600;
        }
    }
}

// 打印用法
void CParseCmdLine::__printUsage()
{
    printf("Usage: \n \
Samples: \n \
    ./portmap -s 127.0.0.1:1982 -d 127.0.0.1:1983 --logterm --timeout 60 --limit_burst 10 --limit_speed 1/s --limit_connect 10 \n \
Options: \n \
    -s bindip:port \n \
    -d targetip:port \n \
    --logterm \n \
      open terminal log print, default close \n \
    --timeout seconds \n \
      set idle connection timeout, default 300s \n \
    --limit_burst 5 \n \
      set max burst number for per source ip, default 10000 \n \
    --limit_speed 1/m \n \
      set speed for per source ip, format: 1/s,10/m,100/h, default 10000/s \n \
    --limit_connect 1 \n \
      set max connection for per source ip, default 100000 \n");
}
