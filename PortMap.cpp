//============================================================================
// Name        : PortMap.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2019
// Description : 14166097@qq.com
//============================================================================

#include "libevwork/EVWork.h"

#include "ConnPairTrans.h"
#include "BurstLimit.h"
#include "ConnectLimit.h"
#include "ParseCmdLine.h"

using namespace evwork;

int main(int argc, char *argv[])
{
	//-------------------------------------------------------------------------
	// libevwork初使化

	signal(SIGPIPE, SIG_IGN);

	CSyslogReport LG;
	CEVLoop LP;
	CConnManager CM;
	CWriter WR;

	CEnv::getThreadEnv()->setLogger(&LG);
	CEnv::getThreadEnv()->setEVLoop(&LP);
	CEnv::getThreadEnv()->setLinkEvent(&CM);
	CEnv::getThreadEnv()->setConnManager(&CM);
	CEnv::getThreadEnv()->setWriter(&WR);

	LP.init();

	//-------------------------------------------------------------------------
	// 应用程序初使化

	// 解析命令行参数
	CParseCmdLine __CMD(argc, argv);

	// 设置连接空闲超时（单位S）
	// 设置合理的取值，可以有效防御空连接
	CEnv::getThreadEnv()->getEVParam().uConnTimeout = __CMD.m_uTimeout;

	// 设置日志输出级别（默认为Debug）
	LG.setLevelUp(Debug);
	// 设置日志向终端输出（默认为false）
	LG.setTerminal(__CMD.m_bLogTerminal);

	// 服务器监听端口和地址
	CListenConn listenConn(__CMD.m_uListenPort, __CMD.m_strBindIp);
	LOG(Info, "Listen %s:%u ...", __CMD.m_strBindIp.c_str(), __CMD.m_uListenPort);
	LOG(Info, "Target %s:%u", __CMD.m_strTargetIp.c_str(), __CMD.m_uTargetPort);

	// 初使化代理转发对象
	CConnPairTrans __Pair(__CMD.m_strTargetIp, __CMD.m_uTargetPort);
	CEnv::getThreadEnv()->setDataEvent(&__Pair);
	CEnv::getThreadEnv()->getLinkEvent()->addLE(&__Pair);

	LOG(Info, "Limit Burst:%u Speed:%.02f/s Connect:%u", (int)__CMD.m_fLimitBurst, __CMD.m_fLimitSpeed, __CMD.m_uLimitConnect);
	LOG(Info, "Connect Timeout:%us", __CMD.m_uTimeout);

	// 初使化连接接入速率限制对象（防CC）
	CBurstLimitMaps __BurstLimitMaps(__CMD.m_fLimitBurst, __CMD.m_fLimitSpeed);
	CBurstLimitMaps::setInstance(&__BurstLimitMaps);
	// 初使化连接总数限制对象（防CC和空连接）
	CConnectLimitMaps __ConnectLimitMaps(__CMD.m_uLimitConnect);
	CConnectLimitMaps::setInstance(&__ConnectLimitMaps);

	//-------------------------------------------------------------------------
	// 启动事件循环

	LP.runLoop();

	return 0;
}
