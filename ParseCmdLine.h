//============================================================================
// Name        : ParseCmdLine.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#pragma once 

#include <stdint.h>
#include <string>

class CParseCmdLine
{
public:
    CParseCmdLine(int argc, char *argv[]);
    virtual ~CParseCmdLine();

private:
    // 解析 "ip:port"
    void __parseIpPort(const char* p, std::string& strIp, uint16_t& uPort);
    // 解析 "1/s" "10/m" "100/h"
    void __parseLimitSpeed(const char* p, float& fLimitSpeed);

    // 打印用法
    void __printUsage();

public:
    std::string m_strBindIp;
    uint16_t m_uListenPort;
    std::string m_strTargetIp;
    uint16_t m_uTargetPort;

    uint32_t m_uTimeout;
    float m_fLimitBurst;
    float m_fLimitSpeed;
    uint32_t m_uLimitConnect;

    bool m_bLogTerminal;
};
