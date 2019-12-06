#pragma once

// IOCP模块

struct TagClientContext;
class CMfcServerDlg;
#define THREAD_COUNT 4

enum IOTYPE
{
	RECV_TYPE = 1,
	SEND_TYPE 
};

#pragma pack(1)
struct OVERLAPPEDPLUS
{
	OVERLAPPED            m_ol;
	IOTYPE                m_ioType;

	OVERLAPPEDPLUS(IOTYPE ioType) {
		ZeroMemory(this, sizeof(OVERLAPPEDPLUS));
		m_ioType = ioType;
	}
};


class CIocp
{
public:
	CIocp();
	virtual ~CIocp();


	//创建IOCP及线程池
	bool Create(CMfcServerDlg* pDlg);

	//将IOCP与SOCKET 绑定
	bool Bind(SOCKET s, ULONG_PTR CompletionKey = 0);

	//销毁IOCP与线程池
	bool Destroy();

	//发送数据
	bool Send(TagClientContext* pContext,
		unsigned char nType,
		unsigned char* pData = NULL,
		int nDataLength = 0);

	bool Send(SOCKET s,
		unsigned char nType,
		unsigned char* pData = NULL,
		int nDataLength = 0);

	//投递一个发送任务
	bool PostSend(TagClientContext* pContext);

	//投递一个接受任务
	bool PostRecv(TagClientContext* pContext);

	static DWORD __stdcall ThreadFunc(LPVOID lpParam);



private:
	CMfcServerDlg* m_pDlg;
	HANDLE m_hIOCP;
	HANDLE m_hThreadAry[THREAD_COUNT];
};

