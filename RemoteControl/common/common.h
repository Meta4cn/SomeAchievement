#pragma once

#include <assert.h>
#include <afx.h>
#include "winsock2.h"
#include "Buffer.h"

#define STRING_END '\0'

#include "zlib.h"
#include "zconf.h"
#pragma comment(lib, "zlib.lib")

#define Z_PREFIX

static Bytef g_uncomBuff[0xFF0000];//MAXBYTE
static Bytef g_comBuff[0xFF0000];
static BYTE  g_aryPacket[0xFF0000];

// nRetCode: compress����uncompress�ķ���ֵ 
// ����ֵ:    ��DebugView�д�ӡ����
static void check_error(int nRetCode)
{
	switch (nRetCode)
	{
	case Z_OK:
		OutputDebugStringA("�ɹ���");
		break;

	case Z_DATA_ERROR:
		OutputDebugStringA("ԭʼ�����ѱ��𻵡�");
		break;

	case Z_MEM_ERROR:
		OutputDebugStringA("zlibѹ���ڴ治�㡣");
		break;

	case Z_BUF_ERROR:
		OutputDebugStringA("����Ļ��������㡣");
		break;

	case Z_VERSION_ERROR:
		OutputDebugStringA("�汾����");
		break;

	default:
		break;
	}
}


#pragma pack(push)
#pragma pack(1)
struct TagPacket
{
	int nLength;
	unsigned char nType;
	//char pData[1];
};
#pragma pack(pop)

enum PACKET_TYPE
{
	//������
	CMD_REQ = 1, //��ʾ����CMD
	CMD_RLY,     //��ʾCMD�Ļظ�

	CMD_DATA, //��ʾcmd��������
	CMD_DATA_RLY, //��ʾcmd���ݻظ�

	SCREEN_REQ,  //��ʾ����SCREEN
	SCREEN_RLY,  //��ʾSCREEN�Ļظ�

	FILE_REQ, // ��ʾ���������ļ���Ϣ����
	FILE_RLY, // ��ʾ�ļ���Ϣ�Ļظ�����

	FILE_LOGICDRIVE_DATA_REQ, // ��ʾ���������Ϣ����
	FILE_LOGICDRIVE_DATA_RLY, // ��ʾ���������Ϣ�Ļظ�

	FILE_LIST_DATA_REQ, // ��ʾ�����ļ���Ϣ����
	FILE_LIST_DATA_RLY, // ��ʾ�����ļ���Ϣ�Ļظ�

	FILE_DOWNLOAD_REQ, // ��ʾ�����ļ�����
	FILE_DOWNLOAD_RLY, // ��ʾ�ظ��ļ�����

	FILE_DOWNLOAD_DATA_REQ, // ��ʾ����ʼ�ļ�����
	FILE_DOWNLOAD_DATA_RLY, // ��ʾ�ظ���ʼ�ļ�����

	FILE_UPLOAD_REQ, // ��ʾ�����ļ��ϴ�
	FILE_UPLOAD_RLY, // ��ʾ�ظ��ļ��ϴ�

	FILE_UPLOAD_DATA_REQ, // ��ʾ����ʼ�ļ��ϴ�
	FILE_UPLOAD_DATA_RLY, // ��ʾ�ظ���ʼ�ļ��ϴ�

	PROCESS_REQ,   //��ʾ������̴���
	PROCESS_RLY,   //��ʾ������̴���Ļظ�

	PROCESS_DATA_REQ, //��ʾ���������Ϣ
	PROCESS_DATA_RLY, //��ʾ���������Ϣ�Ļظ�

	PROCESS_CLS,   //��ʾ����رս���
	PROCESS_CLS_RLY_FAIL,   //��ʾ�رս���ʧ�ܵĻظ�
	PROCESS_CLS_RLY_SCS,    //��ʾ�رս��̳ɹ��Ļظ�

	HEART_REQ,   //��ʾ������
	HEART_RLY,   //��ʾ�������Ļظ�
};

inline bool SendPacket(SOCKET s,
	char nType,
	char* pData = NULL,
	int nDataLength = 0)
{
	CStringA str;
	int nPacketSize = sizeof(TagPacket)+nDataLength;
	TagPacket* pPktHead = reinterpret_cast<TagPacket*>(str.GetBuffer(nPacketSize));

	pPktHead->nType = nType;
	pPktHead->nLength = nDataLength;

	if (!(pData == NULL || nDataLength == 0))
	{
		memcpy(pPktHead + 1, pData, nDataLength);
	}

	send(s, (char*)pPktHead, nPacketSize, 0);

	str.ReleaseBuffer();

	return true;
}

//��ȡָ�����ȵ����ݰ�
inline bool RecvData(SOCKET s,
	char* pData,
	int nDataLength)
{
	int nTotalSize = nDataLength;
	int nCurBytes = 0;
	while (nCurBytes < nTotalSize)
	{
		int nRecvedBytes = recv(s,
			(char*)pData + nCurBytes,
			nDataLength - nCurBytes,
			0);

		if (nRecvedBytes == 0)
		{
			//��ʾ���Źر�
			closesocket(s);
			return false;
		}
		else if (nRecvedBytes < 0)
		{
			//��ʾ���ͳ���
			//������
			closesocket(s);
			return false;
		}
		nCurBytes += nRecvedBytes;
	}

	return true;
}

//��֤����һ����������
//inline bool RecvPacket(TagClientContext* pContext,
inline bool ServerRecvPacket(SOCKET sClient, // (in)
	BYTE* pRecvBuf, // �Ѿ���ȡ�����ݰ� (in)
	unsigned char& nType, // ���ص����ݰ������� (out)
	CBuffer& strData, // ���ص��������������������� (out)
	int& nBufSize, // ��������,��ʾһ�������İ��ĳ���(out)
	int& nRecvedBytes  // �Ѿ���ȡ�����ݴ�С(in)
	)
{
	SOCKET s = sClient;
	TagPacket* pPktHead = (TagPacket*)pRecvBuf;

	int nRecvedDataLen = nRecvedBytes - sizeof(TagPacket);

	if (nRecvedDataLen == 0)
	{
		//��ʾ�������յ�����ظ��������ݰ��Ѿ�����
		nType = pPktHead->nType;
		nBufSize = sizeof(TagPacket);
		return true;
	}
	else if (nRecvedDataLen < 0)
	{
		// ���󣬷���
		return false;
	}
	else
	{
		//��ʾ�ɹ����յ������ݰ�,
		nType = pPktHead->nType;

		// ��IOCP������£��������ڽ��ն��֧�����������Ƭʱ���ƺ�һ���յ���
		// ����������ǰ����һ���յ�
		// ʹ�ý�������Ĺ�����RecvData����ת�Ƶ������� 
		PBYTE pTmpRecvBuf = (PBYTE)g_aryPacket;
		ZeroMemory(pTmpRecvBuf, 0xFF0000);
		memcpy(pTmpRecvBuf, pRecvBuf, nRecvedBytes);

		TagPacket* pTmpPktHead;
		DWORD pShadow = (DWORD)pTmpRecvBuf;

		do
		{
			// ������ݷ�Ƭ������һ�𣬽���
			pTmpPktHead = (TagPacket*)pTmpRecvBuf;

			if (pTmpPktHead->nLength <= 0)
				break;

			pTmpRecvBuf += sizeof(TagPacket);  // ָ���ƶ�������ͷ�������������
			strData.Write((PBYTE)pTmpRecvBuf, pTmpPktHead->nLength); // ��������
			pTmpRecvBuf += pTmpPktHead->nLength;  // ָ���ƶ�����һ������ͷ
			//nDataLen += pTmpPktHead->nLength;

		} while (true);

		nBufSize = strData.GetBufferLen();
	}

	return true;
}

//��֤����һ����������
//inline bool RecvPacket(TagClientContext* pContext,
inline bool ClientRecvPacket(SOCKET sClient,
	//BYTE* pTmpBuf,
	unsigned char& nType,
	CBuffer& strData,
	int& nBufSize,
	int& nRecvedBytes)
{
	unsigned int nLength = 0;
	SOCKET s = sClient;
	TagPacket pktHead = { 0 };

	bool bRet = RecvData(s, (PCHAR)&pktHead, sizeof(TagPacket));
	if (!bRet)
	{
		return false;
	}

	//��ȡ����
	if (pktHead.nLength == 0)
	{
		// ������û�����ݣ�˵���������������
		nType = pktHead.nType;
		nRecvedBytes += sizeof(TagPacket);

		return true;
	}
	else if (pktHead.nLength < 0)
	{
		// ����
		return false;
	}
	else
	{
		// pktHead.nLength > 0 �յ��˾�������
		//��ʾ�ɹ����յ������ݰ�,
		//���յ���������ӵ�client�Ľ��ջ�������

		TagPacket* pPktHead = (TagPacket*)&pktHead;
		nRecvedBytes += sizeof(TagPacket);

		nLength = pPktHead->nLength;
		nType = pPktHead->nType;

		// ����������,��Ҫ������ȡδ�յ�����
		PBYTE pByteBuf = new BYTE[nLength];
		memset(pByteBuf, 0, nLength);
		bRet = RecvData(sClient, (PCHAR)pByteBuf, nLength);
		if (!bRet)
		{
			delete pByteBuf;
			pByteBuf = NULL;
			return false;
		}
		// �յ��˶���������pByteBuf��
		strData.Write(pByteBuf, nLength);

		delete pByteBuf;
		pByteBuf = NULL;

		// Write�����õ���Copy������ʱ�߼���Ӧ����Ƴɲ���Ӱ�츱���Ŷԡ�����
		nRecvedBytes += strData.GetBufferLen();
		if (strData.GetBufferLen() >= nLength)
		{
			nType = pPktHead->nType;
			nBufSize = nLength;
			//˵��һ�����Ѿ���������,���Կ�ʼ������
			//strData.Delete(sizeof(TagPacket));

			return true;
		}

		return false;
	}


	return false;
}
