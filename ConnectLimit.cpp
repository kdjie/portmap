//============================================================================
// Name        : ConnectLimit.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#include "ConnectLimit.h"
#include "libevwork/EVWork.h"

using namespace evwork;

CConnectLimit::CConnectLimit(CConnectLimitMaps *pParent, const std::string &strIp, int32_t nConnectMax)
    : m_pParent(pParent), m_strIp(strIp), m_nConnectMax(nConnectMax), m_nConnectCur(0)
{
    m_uLastDecTime = (uint32_t)time(NULL);
    m_timer.init(this);
    m_timer.start(1000);
}
CConnectLimit::~CConnectLimit()
{
    m_timer.stop();
}

bool CConnectLimit::incConnect(int32_t nVal)
{
    LOG(Debug, "[CConnectLimit::incConnect] ip:[%s] before inc cur:[%d]", m_strIp.c_str(), m_nConnectCur);

    if (m_nConnectCur >= m_nConnectMax)
        return false;

    m_nConnectCur += nVal;

    LOG(Debug, "[CConnectLimit::incConnect] ip:[%s] after inc cur:[%d]", m_strIp.c_str(), m_nConnectCur);
    return true;
}
void CConnectLimit::decConnect(int32_t nVal)
{
    m_uLastDecTime = (uint32_t)time(NULL);

    LOG(Debug, "[CConnectLimit::decConnect] ip:[%s] before dec cur:[%d]", m_strIp.c_str(), m_nConnectCur);

    if (m_nConnectCur >= nVal)
    {
        m_nConnectCur -= nVal;

        LOG(Debug, "[CConnectLimit::decConnect] ip:[%s] after dec cur:[%d]", m_strIp.c_str(), m_nConnectCur);
    }
}

bool CConnectLimit::__handlerTimer()
{
    if (m_nConnectCur == 0 && (uint32_t)time(NULL) - m_uLastDecTime > 300)
    {
        m_pParent->freeConnectLimit(m_strIp, this);
        return false;
    }

    return true;
}

CConnectLimitMaps* CConnectLimitMaps::g_pConnectLimitMaps = NULL;

void CConnectLimitMaps::setInstance(CConnectLimitMaps *p)
{
    g_pConnectLimitMaps = p;
}
CConnectLimitMaps* CConnectLimitMaps::getInstance()
{
    return g_pConnectLimitMaps;
}

CConnectLimitMaps::CConnectLimitMaps(int32_t nConnectMax)
    : m_nConnectMax(nConnectMax)
{
}
CConnectLimitMaps::~CConnectLimitMaps()
{
    for (MAP_IP_LIMITPTR_t::iterator iter = m_mapIpLimitPtr.begin(); iter != m_mapIpLimitPtr.end();)
    {
        delete iter->second;
        m_mapIpLimitPtr.erase(iter++);
    }
}

CConnectLimit* CConnectLimitMaps::getConnectLimit(const std::string &strIp)
{
    MAP_IP_LIMITPTR_t::iterator iter = m_mapIpLimitPtr.find(strIp);
    if (iter != m_mapIpLimitPtr.end())
        return iter->second;

    CConnectLimit *pLimit = new CConnectLimit(this, strIp, m_nConnectMax);
    m_mapIpLimitPtr.insert(std::make_pair(strIp, pLimit));
    return pLimit;
}
void CConnectLimitMaps::freeConnectLimit(const std::string &strIp, CConnectLimit *pLimit)
{
    m_mapIpLimitPtr.erase(strIp);
    delete pLimit;
}
