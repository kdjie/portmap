//============================================================================
// Name        : BurstLimit.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#include "BurstLimit.h"
#include "libevwork/EVWork.h"

using namespace evwork;

CBurstLimit::CBurstLimit(CBurstLimitMaps *pParent, const std::string &strIp, float fBurstMax, float fAddPerSec)
    : m_pParent(pParent), m_strIp(strIp), m_fBurstMax(fBurstMax), m_fBurstCur(fBurstMax), m_fAddPerSec(fAddPerSec)
{
    m_uLastDecTime = (uint32_t)time(NULL);
    m_timer.init(this);
    m_timer.start(1000);
}
CBurstLimit::~CBurstLimit()
{
    m_timer.stop();
}

bool CBurstLimit::decBurst(float fVal)
{
    m_uLastDecTime = (uint32_t)time(NULL);

    LOG(Debug, "[CBurstLimit::decBurst] ip:[%s] before dec cur:[%.02f]", m_strIp.c_str(), m_fBurstCur);

    if (m_fBurstCur < fVal)
        return false;

    m_fBurstCur -= fVal;

    LOG(Debug, "[CBurstLimit::decBurst] ip:[%s] after dec cur:[%.02f]", m_strIp.c_str(), m_fBurstCur);
    return true;
}

bool CBurstLimit::__handlerTimer()
{
    if ((uint32_t)time(NULL) - m_uLastDecTime > 300)
    {
        m_pParent->freeBurstLimit(m_strIp, this);
        return false;
    }

    m_fBurstCur += m_fAddPerSec;
    if (m_fBurstCur > m_fBurstMax)
        m_fBurstCur = m_fBurstMax;

    return true;
}

CBurstLimitMaps* CBurstLimitMaps::g_pBurstLimitMaps = NULL;

void CBurstLimitMaps::setInstance(CBurstLimitMaps *p)
{
    g_pBurstLimitMaps = p;
}
CBurstLimitMaps* CBurstLimitMaps::getInstance()
{
    return g_pBurstLimitMaps;
}

CBurstLimitMaps::CBurstLimitMaps(float fBurstMax, float fAddPerSec)
    : m_fBurstMax(fBurstMax), m_fAddPerSec(fAddPerSec)
{
}
CBurstLimitMaps::~CBurstLimitMaps()
{
    for (MAP_IP_LIMITPTR_t::iterator iter = m_mapIpLimitPtr.begin(); iter != m_mapIpLimitPtr.end();)
    {
        delete iter->second;
        m_mapIpLimitPtr.erase(iter++);
    }
}

CBurstLimit *CBurstLimitMaps::getBurstLimit(const std::string &strIp)
{
    MAP_IP_LIMITPTR_t::iterator iter = m_mapIpLimitPtr.find(strIp);
    if (iter != m_mapIpLimitPtr.end())
        return iter->second;

    CBurstLimit *pLimit = new CBurstLimit(this, strIp, m_fBurstMax, m_fAddPerSec);
    m_mapIpLimitPtr.insert(std::make_pair(strIp, pLimit));
    return pLimit;
}

void CBurstLimitMaps::freeBurstLimit(const std::string &strIp, CBurstLimit *pLimit)
{
    m_mapIpLimitPtr.erase(strIp);
    delete pLimit;
}
