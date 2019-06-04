//============================================================================
// Name        : ConnPairTrans.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include "libevwork/EVComm.h"
#include <map>

// 连接对
struct SConnPair
{
	SConnPair() : pConnSrc(NULL), pConnDst(NULL) {}

	evwork::IConn *pConnSrc; // 源端连接
	evwork::IConn *pConnDst; // 目标连接
};

// 连接对传输管理
class CConnPairTrans
	: public evwork::ILinkEvent,
	  public evwork::IDataEvent
{
public:
	// 参数：目标端的IP和端口
	CConnPairTrans(const std::string& strDstIp, uint16_t uDstPort);
	virtual ~CConnPairTrans();

	// 凡是有连接接入和断开均触发的事件
	virtual void onConnected(evwork::IConn *pConn);
	virtual void onClose(evwork::IConn *pConn);

	// 凡是有收到任意连接数据的事件
	virtual int onData(evwork::IConn *pConn, const char *pData, size_t uSize);

private:
	// 是否新连接
	bool __isNewConn(evwork::IConn *pConn);
	// 是否源端连接
	bool __isSrcConn(evwork::IConn *pConn);
	// 是否目标端连接
	bool __isDstConn(evwork::IConn *pConn);
	// 创建连接对
	void __createConnPair(evwork::IConn *pConnSrc);
	// 销毁连接对
	void __destroyConnPair(SConnPair *pPair, bool bFromSrc);
	// 根据源端连接取连接对
	SConnPair* __getConnPairBySrcConn(evwork::IConn *pConn);
	// 根据目标端连接取连接对
	SConnPair* __getConnPairByDstConn(evwork::IConn *pConn);

private:
	typedef std::map<evwork::IConn*, SConnPair*> MAP_PCONN_PAIRPTR_t;

	// 源端连接 -> 连接对 映射表
	MAP_PCONN_PAIRPTR_t m_mapPConnSrc2PairPtr;
	// 目标连接 -> 连接对 映射表
	MAP_PCONN_PAIRPTR_t m_mapPConnDst2PairPtr;

	// 目标端的IP和端口
	std::string m_strDstIp;
	uint16_t m_uDstPort;
};
