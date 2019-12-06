#include "stdafx.h"
#include "Iocp.h"
#include "MfcServerDlg.h"

CIocp::CIocp()
{
	m_hIOCP = NULL;
}


CIocp::~CIocp()
{
}

// 线程回调函数
DWORD __stdcall CIocp::ThreadFunc(LPVOID lpParam)
{
	CMfcServerDlg* pDlg = (CMfcServerDlg*)lpParam;
	CIocp* pIocp = &pDlg->m_IOCP;
	ULONG_PTR dwCompleteKey = 0;
	int dwTransferedBytes = 0;
	OVERLAPPEDPLUS* pOverlap = NULL;
	SOCKET sClient;

	while (true)
	{
		BOOL bRet = GetQueuedCompletionStatus(pIocp->m_hIOCP,
			(DWORD*)&dwTransferedBytes,
			&dwCompleteKey,
			(LPOVERLAPPED*)&pOverlap,
			INFINITE);

		if (!bRet)
		{
			break;
		}

		sClient = (SOCKET)dwCompleteKey;
		pDlg->m_cs.Lock();
		TagClientContext* pContext = pDlg->m_ClientMap[sClient];
		pDlg->m_cs.Unlock();

		switch (pOverlap->m_ioType)
		{
		case RECV_TYPE:
		{
			//表示已经成功的接收到了dwTransferedBytes字节的数据
			bool bRet = false;
			do
			{
				unsigned char nType(0);
				int nBufSize = 0;

				bRet = ServerRecvPacket(
					pContext->m_sClient,
					(PBYTE)pContext->m_TmpBuf,
					nType,
					pContext->m_recvBuf,
					nBufSize, //传出参数,表示一个完整的包的长度
					dwTransferedBytes);

				if (bRet)
				{
					pDlg->OnRecvData(pContext, nType, pContext->m_recvBuf, nBufSize);
				}
			} while (false);
			//表示已经成功的处理了数据包,再往任务队列中
			//投递下一个任务
			pIocp->PostRecv(pContext);
		}
			break;

		case SEND_TYPE:
		{
			//表示已经成功的发送了dwTransferedBytes字节的数据
			pContext->m_cs.Lock();
			pContext->m_sendBuf.Delete(dwTransferedBytes);
			pContext->m_cs.Unlock();

			//表示数据没有发完整，则需要再次投递一个发送请求
			UINT uBufLen = pContext->m_sendBuf.GetBufferLen();
			if (uBufLen > 0)
			{
				//表示已经成功的处理了数据包,再往任务队列中
				//再次投递一个请求发送包
				pIocp->PostSend(pContext);
			}
		}
			break;
		default:
			break;
		}

		delete pOverlap;

	}


	return 0;
}

//初始化
bool CIocp::Create(CMfcServerDlg* pDlg)
{
	m_pDlg = pDlg;
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
		NULL,
		0,
		THREAD_COUNT);

	if (m_hIOCP == NULL)
	{
		return false;
	}

	//创建线程池
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		m_hThreadAry[i] = (HANDLE)_beginthreadex(NULL,
			0,
			(unsigned int(__stdcall *)(void *))ThreadFunc,
			pDlg,
			0,
			NULL);
	}

	return 0;
}

//销毁
bool CIocp::Destroy()
{
	CloseHandle(m_hIOCP);

	WaitForMultipleObjects(THREAD_COUNT,
		m_hThreadAry,
		TRUE,
		INFINITE);

	return true;
}

//绑定
bool CIocp::Bind(SOCKET s, ULONG_PTR CompletionKey)
{
	HANDLE nRet = CreateIoCompletionPort((HANDLE)s,
		m_hIOCP,
		CompletionKey,
		THREAD_COUNT);

	if (nRet == NULL)
	{
		return false;
	}

	return true;
}

bool CIocp::Send(SOCKET s,
	unsigned char nType,
	unsigned char* pData,
	int nDataLength)
{
	m_pDlg->m_cs.Lock();
	TagClientContext* pContext = m_pDlg->m_ClientMap[s];
	m_pDlg->m_cs.Unlock();

	return Send(pContext, nType, pData, nDataLength);
}

//将要发送的数据写在发送缓冲区中
bool CIocp::Send(TagClientContext* pContext,
	unsigned char nType,
	unsigned char* pData,
	int nDataLength)
{
	int nSendLength = sizeof(TagPacket)+nDataLength;
	TagPacket PktHead;
	PktHead.nType = nType;
	PktHead.nLength = nDataLength;

	pContext->m_cs.Lock();
	pContext->m_sendBuf.Write((BYTE*)&PktHead, sizeof(TagPacket));
	if (!(pData == NULL || nDataLength == 0))
	{
		pContext->m_sendBuf.Write(pData, nDataLength);
	}
	pContext->m_cs.Unlock();


	//需要保证I/O队列中，一个socket最多只有一个的请求
	if (pContext->m_sendBuf.GetBufferLen() == nSendLength)
	{
		PostSend(pContext);
	}

	return true;
}

//外部发送接口
//投递一个发送任务
bool CIocp::PostSend(TagClientContext* pContext)
{
	WSABUF buf;
	buf.len = pContext->m_sendBuf.GetBufferLen();
	buf.buf = (char*)pContext->m_sendBuf.GetBuffer();
	DWORD dwSendedBytes = 0;

	OVERLAPPEDPLUS* pOverLaped = new OVERLAPPEDPLUS(SEND_TYPE);

	int nRet = WSASend(pContext->m_sClient,
		&buf,
		1,
		&dwSendedBytes,
		0,
		(LPWSAOVERLAPPED)pOverLaped,
		NULL);

	if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		return false;
	}


	return true;
}

//投递一个接受任务
bool CIocp::PostRecv(TagClientContext* pContext)
{
	WSABUF buf;
	buf.len = BUF_SIZE;
	buf.buf = pContext->m_TmpBuf;
	DWORD dwRecvedBytes = 0;
	DWORD Flags = 0;
	OVERLAPPEDPLUS* pOverLaped = new OVERLAPPEDPLUS(RECV_TYPE);

	int nRet = WSARecv(pContext->m_sClient,
		&buf,
		1,
		&dwRecvedBytes,
		&Flags,
		(LPWSAOVERLAPPED)pOverLaped,
		NULL);

	if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		return false;
	}

	return true;
}

