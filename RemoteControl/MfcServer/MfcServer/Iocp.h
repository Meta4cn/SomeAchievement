#pragma once

// IOCPģ��

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


	//����IOCP���̳߳�
	bool Create(CMfcServerDlg* pDlg);

	//��IOCP��SOCKET ��
	bool Bind(SOCKET s, ULONG_PTR CompletionKey = 0);

	//����IOCP���̳߳�
	bool Destroy();

	//��������
	bool Send(TagClientContext* pContext,
		unsigned char nType,
		unsigned char* pData = NULL,
		int nDataLength = 0);

	bool Send(SOCKET s,
		unsigned char nType,
		unsigned char* pData = NULL,
		int nDataLength = 0);

	//Ͷ��һ����������
	bool PostSend(TagClientContext* pContext);

	//Ͷ��һ����������
	bool PostRecv(TagClientContext* pContext);

	static DWORD __stdcall ThreadFunc(LPVOID lpParam);



private:
	CMfcServerDlg* m_pDlg;
	HANDLE m_hIOCP;
	HANDLE m_hThreadAry[THREAD_COUNT];
};

