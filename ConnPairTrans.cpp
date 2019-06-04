//============================================================================
// Name        : ConnPairTrans.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#include "ConnPairTrans.h"

#include "libevwork/EVWork.h"
#include "BurstLimit.h"
#include "ConnectLimit.h"

using namespace evwork;

// 参数：目标端的IP和端口
CConnPairTrans::CConnPairTrans(const std::string &strDstIp, uint16_t uDstPort)
	: m_strDstIp(strDstIp), m_uDstPort(uDstPort)
{
}
CConnPairTrans::~CConnPairTrans()
{
}

// 凡是有连接接入和断开均触发的事件

void CConnPairTrans::onConnected(IConn *pConn)
{
	// 是否新连接
	// 表示从源端的新接入
	if (__isNewConn(pConn))
	{
		std::string strSrcIp = "";
		uint16_t uSrcPort = 0;
		pConn->getPeerInfo(strSrcIp, uSrcPort);

		// 检查该IP对应的连接接入速率限制
		if (!CBurstLimitMaps::getInstance()->getBurstLimit(strSrcIp)->decBurst())
		{
			LOG(Warn, "[CConnPairTrans::onConnected] src:[%s:%d] burst limit!", strSrcIp.c_str(), uSrcPort);
			delete pConn;
			return;
		}

		// 检查该IP对应的连接总数限制
		if (!CConnectLimitMaps::getInstance()->getConnectLimit(strSrcIp)->incConnect())
		{
			LOG(Warn, "[CConnPairTrans::onConnected] src:[%s:%d] connect limit!", strSrcIp.c_str(), uSrcPort);
			delete pConn;
			return;
		}

		LOG(Info, "[CConnPairTrans::onConnected] src:[%s:%d] connect...", strSrcIp.c_str(), uSrcPort);

		// 合法连接，创建连接对后返回
		__createConnPair(pConn);
		return;
	}

	// 是否目标连接
	// 表示目标端连接成功
	if (__isDstConn(pConn))
	{
		SConnPair *pPair = __getConnPairByDstConn(pConn);

		std::string strSrcIp = "", strDstIp = "";
		uint16_t uSrcPort = 0, uDstPort = 0;
		pPair->pConnSrc->getPeerInfo(strSrcIp, uSrcPort);
		pPair->pConnDst->getPeerInfo(strDstIp, uDstPort);

		LOG(Info, "[CConnPairTrans::onConnected] src:[%s:%d] <-> dst:[%s:%d] connected",
			strSrcIp.c_str(), uSrcPort, strDstIp.c_str(), uDstPort);

		// 恢复源端连接收包
		((CClientConn *)pPair->pConnSrc)->startRead();
	}
}

void CConnPairTrans::onClose(IConn *pConn)
{
	// 任意一方断开连接，需要立即销毁整个连接对
	// 并且要避免事件源连接被重复释放
	if (__isSrcConn(pConn))
	{
		__destroyConnPair(__getConnPairBySrcConn(pConn), true);
	}
	else if (__isDstConn(pConn))
	{
		__destroyConnPair(__getConnPairByDstConn(pConn), false);
	}
}

// 凡是有收到任意连接数据的事件

int CConnPairTrans::onData(IConn *pConn, const char *pData, size_t uSize)
{
	// 任意一方收到数据，立即向对端转发

	if (__isSrcConn(pConn))
	{
		__getConnPairBySrcConn(pConn)->pConnDst->sendBin(pData, uSize);
	}
	else if (__isDstConn(pConn))
	{
		__getConnPairByDstConn(pConn)->pConnSrc->sendBin(pData, uSize);
	}

	return uSize;
}

// 是否新连接
bool CConnPairTrans::__isNewConn(IConn *pConn)
{
	if (m_mapPConnSrc2PairPtr.find(pConn) != m_mapPConnSrc2PairPtr.end())
		return false;

	if (m_mapPConnDst2PairPtr.find(pConn) != m_mapPConnDst2PairPtr.end())
		return false;

	return true;
}

// 是否源端连接
bool CConnPairTrans::__isSrcConn(IConn *pConn)
{
	if (m_mapPConnSrc2PairPtr.find(pConn) != m_mapPConnSrc2PairPtr.end())
		return true;

	return false;
}

// 是否目标端连接
bool CConnPairTrans::__isDstConn(IConn *pConn)
{
	if (m_mapPConnDst2PairPtr.find(pConn) != m_mapPConnDst2PairPtr.end())
		return true;

	return false;
}

// 创建连接对
void CConnPairTrans::__createConnPair(IConn *pConnSrc)
{
	SConnPair *pPair = new SConnPair();
	pPair->pConnSrc = pConnSrc;
	// 立即向目标端发起连接
	pPair->pConnDst = CEnv::getThreadEnv()->getConnManager()->getConnByIpPort(m_strDstIp, m_uDstPort);

	// 建立映射表
	m_mapPConnSrc2PairPtr.insert(std::make_pair(pPair->pConnSrc, pPair));
	m_mapPConnDst2PairPtr.insert(std::make_pair(pPair->pConnDst, pPair));

	// 并且暂停源端连接收包
	((CClientConn *)pPair->pConnSrc)->stopRead();

	std::string strSrcIp = "", strDstIp = "";
	uint16_t uSrcPort = 0, uDstPort = 0;
	pPair->pConnSrc->getPeerInfo(strSrcIp, uSrcPort);
	pPair->pConnDst->getPeerInfo(strDstIp, uDstPort);
	LOG(Info, "[CConnPairTrans::__createConnPair] src:[%s:%d] <-> dst:[%s:%d] create",
		strSrcIp.c_str(), uSrcPort, strDstIp.c_str(), uDstPort);
}

// 销毁连接对
void CConnPairTrans::__destroyConnPair(SConnPair *pPair, bool bFromSrc)
{
	std::string strSrcIp = "", strDstIp = "";
	uint16_t uSrcPort = 0, uDstPort = 0;
	pPair->pConnSrc->getPeerInfo(strSrcIp, uSrcPort);
	pPair->pConnDst->getPeerInfo(strDstIp, uDstPort);
	LOG(Info, "[CConnPairTrans::__destroyConnPair] src:[%s:%d] <-> dst:[%s:%d] destroy",
		strSrcIp.c_str(), uSrcPort, strDstIp.c_str(), uDstPort);

	// 更新源端IP的当前连接计数
	CConnectLimitMaps::getInstance()->getConnectLimit(strSrcIp)->decConnect();

	// 清除对应映射表
	m_mapPConnSrc2PairPtr.erase(pPair->pConnSrc);
	m_mapPConnDst2PairPtr.erase(pPair->pConnDst);

	// 销毁对端连接
	delete (bFromSrc ? pPair->pConnDst : pPair->pConnSrc);
	// 销毁连接对对象
	delete pPair;
}

// 根据源端连接取连接对
SConnPair *CConnPairTrans::__getConnPairBySrcConn(IConn *pConn)
{
	MAP_PCONN_PAIRPTR_t::iterator iter = m_mapPConnSrc2PairPtr.find(pConn);
	if (iter != m_mapPConnSrc2PairPtr.end())
		return iter->second;

	return NULL;
}

// 根据目标端连接取连接对
SConnPair *CConnPairTrans::__getConnPairByDstConn(IConn *pConn)
{
	MAP_PCONN_PAIRPTR_t::iterator iter = m_mapPConnDst2PairPtr.find(pConn);
	if (iter != m_mapPConnDst2PairPtr.end())
		return iter->second;

	return NULL;
}
