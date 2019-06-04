//============================================================================
// Name        : BurstLimit.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <stdint.h>
#include <string>
#include <libevwork/TimerHandler.h>

class CBurstLimitMaps;

class CBurstLimit
{
public:
    CBurstLimit(CBurstLimitMaps *pParent, const std::string &strIp, float fBurstMax, float fAddPerSec);
    virtual ~CBurstLimit();

    bool decBurst(float fVal = 1.0);

private:
    bool __handlerTimer();
    TimerHandler<CBurstLimit, &CBurstLimit::__handlerTimer> m_timer;

private:
    CBurstLimitMaps *m_pParent;
    std::string m_strIp;
    float m_fBurstMax;
    float m_fBurstCur;
    float m_fAddPerSec;
    uint32_t m_uLastDecTime;
};

class CBurstLimitMaps
{
public:
    static void setInstance(CBurstLimitMaps *p);
    static CBurstLimitMaps* getInstance();

    CBurstLimitMaps(float fBurstMax, float fAddPerSec);
    virtual ~CBurstLimitMaps();

    CBurstLimit* getBurstLimit(const std::string &strIp);
    void freeBurstLimit(const std::string &strIp, CBurstLimit *pLimit);

private:
    static CBurstLimitMaps *g_pBurstLimitMaps;

    typedef std::map<std::string, CBurstLimit*> MAP_IP_LIMITPTR_t;
    MAP_IP_LIMITPTR_t m_mapIpLimitPtr;

    float m_fBurstMax;
    float m_fAddPerSec;
};
