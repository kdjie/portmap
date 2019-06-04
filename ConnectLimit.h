//============================================================================
// Name        : ConnectLimit.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <stdint.h>
#include <string>
#include <libevwork/TimerHandler.h>

class CConnectLimitMaps;

class CConnectLimit
{
public:
    CConnectLimit(CConnectLimitMaps *pParent, const std::string &strIp, int32_t nConnectMax);
    virtual ~CConnectLimit();

    bool incConnect(int32_t nVal = 1);
    void decConnect(int32_t nVal = 1);

private:
    bool __handlerTimer();
    TimerHandler<CConnectLimit, &CConnectLimit::__handlerTimer> m_timer;

private:
    CConnectLimitMaps *m_pParent;
    std::string m_strIp;
    int32_t m_nConnectMax;
    int32_t m_nConnectCur;
    uint32_t m_uLastDecTime;
};

class CConnectLimitMaps
{
public:
    static void setInstance(CConnectLimitMaps *p);
    static CConnectLimitMaps* getInstance();

    CConnectLimitMaps(int32_t nConnectMax);
    virtual ~CConnectLimitMaps();

    CConnectLimit* getConnectLimit(const std::string &strIp);
    void freeConnectLimit(const std::string &strIp, CConnectLimit *pLimit);

private:
    static CConnectLimitMaps *g_pConnectLimitMaps;

    typedef std::map<std::string, CConnectLimit*> MAP_IP_LIMITPTR_t;
    MAP_IP_LIMITPTR_t m_mapIpLimitPtr;

    int32_t m_nConnectMax;
};
