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

// nRetCode: compress或者uncompress的返回值 
// 返回值:    在DebugView中打印错误
static void check_error(int nRetCode)
{
	switch (nRetCode)
	{
	case Z_OK:
		OutputDebugStringA("成功！");
		break;

	case Z_DATA_ERROR:
		OutputDebugStringA("原始数据已被损坏。");
		break;

	case Z_MEM_ERROR:
		OutputDebugStringA("zlib压缩内存不足。");
		break;

	case Z_BUF_ERROR:
		OutputDebugStringA("输出的缓冲区不足。");
		break;

	case Z_VERSION_ERROR:
		OutputDebugStringA("版本错误");
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
	//包类型
	CMD_REQ = 1, //表示请求CMD
	CMD_RLY,     //表示CMD的回复

	CMD_DATA, //表示cmd数据请求
	CMD_DATA_RLY, //表示cmd数据回复

	SCREEN_REQ,  //表示请求SCREEN
	SCREEN_RLY,  //表示SCREEN的回复

	FILE_REQ, // 表示请求启动文件信息交互
	FILE_RLY, // 表示文件信息的回复交互

	FILE_LOGICDRIVE_DATA_REQ, // 表示请求磁盘信息传送
	FILE_LOGICDRIVE_DATA_RLY, // 表示请求磁盘信息的回复

	FILE_LIST_DATA_REQ, // 表示请求文件信息传送
	FILE_LIST_DATA_RLY, // 表示请求文件信息的回复

	FILE_DOWNLOAD_REQ, // 表示请求文件下载
	FILE_DOWNLOAD_RLY, // 表示回复文件下载

	FILE_DOWNLOAD_DATA_REQ, // 表示请求开始文件下载
	FILE_DOWNLOAD_DATA_RLY, // 表示回复开始文件下载

	FILE_UPLOAD_REQ, // 表示请求文件上传
	FILE_UPLOAD_RLY, // 表示回复文件上传

	FILE_UPLOAD_DATA_REQ, // 表示请求开始文件上传
	FILE_UPLOAD_DATA_RLY, // 表示回复开始文件上传

	PROCESS_REQ,   //表示请求进程传输
	PROCESS_RLY,   //表示请求进程传输的回复

	PROCESS_DATA_REQ, //表示请求进程信息
	PROCESS_DATA_RLY, //表示请求进程信息的回复

	PROCESS_CLS,   //表示请求关闭进程
	PROCESS_CLS_RLY_FAIL,   //表示关闭进程失败的回复
	PROCESS_CLS_RLY_SCS,    //表示关闭进程成功的回复

	HEART_REQ,   //表示心跳包
	HEART_RLY,   //表示心跳包的回复
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

//收取指定长度的数据包
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
			//表示优雅关闭
			closesocket(s);
			return false;
		}
		else if (nRecvedBytes < 0)
		{
			//表示发送出错
			//错误处理
			closesocket(s);
			return false;
		}
		nCurBytes += nRecvedBytes;
	}

	return true;
}

//保证仅收一个包的数据
//inline bool RecvPacket(TagClientContext* pContext,
inline bool ServerRecvPacket(SOCKET sClient, // (in)
	BYTE* pRecvBuf, // 已经获取的数据包 (in)
	unsigned char& nType, // 返回的数据包的类型 (out)
	CBuffer& strData, // 返回的数据区，不包括请求区 (out)
	int& nBufSize, // 传出参数,表示一个完整的包的长度(out)
	int& nRecvedBytes  // 已经获取的数据大小(in)
	)
{
	SOCKET s = sClient;
	TagPacket* pPktHead = (TagPacket*)pRecvBuf;

	int nRecvedDataLen = nRecvedBytes - sizeof(TagPacket);

	if (nRecvedDataLen == 0)
	{
		//表示服务器收到请求回复包，数据包已经完整
		nType = pPktHead->nType;
		nBufSize = sizeof(TagPacket);
		return true;
	}
	else if (nRecvedDataLen < 0)
	{
		// 错误，返回
		return false;
	}
	else
	{
		//表示成功的收到了数据包,
		nType = pPktHead->nType;

		// 在IOCP的情况下，服务器在接收多个支离破碎的数据片时，似乎一起收到了
		// 而不是像以前那样一起收到
		// 使得解析请求的工作从RecvData函数转移到了这里 
		PBYTE pTmpRecvBuf = (PBYTE)g_aryPacket;
		ZeroMemory(pTmpRecvBuf, 0xFF0000);
		memcpy(pTmpRecvBuf, pRecvBuf, nRecvedBytes);

		TagPacket* pTmpPktHead;
		DWORD pShadow = (DWORD)pTmpRecvBuf;

		do
		{
			// 多个数据分片合在了一起，解析
			pTmpPktHead = (TagPacket*)pTmpRecvBuf;

			if (pTmpPktHead->nLength <= 0)
				break;

			pTmpRecvBuf += sizeof(TagPacket);  // 指针移动过请求头部后面的数据区
			strData.Write((PBYTE)pTmpRecvBuf, pTmpPktHead->nLength); // 拷贝数据
			pTmpRecvBuf += pTmpPktHead->nLength;  // 指针移动到下一个请求头
			//nDataLen += pTmpPktHead->nLength;

		} while (true);

		nBufSize = strData.GetBufferLen();
	}

	return true;
}

//保证仅收一个包的数据
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

	//收取数据
	if (pktHead.nLength == 0)
	{
		// 数据区没有内容，说明是请求包，返回
		nType = pktHead.nType;
		nRecvedBytes += sizeof(TagPacket);

		return true;
	}
	else if (pktHead.nLength < 0)
	{
		// 错误
		return false;
	}
	else
	{
		// pktHead.nLength > 0 收到了具体请求
		//表示成功的收到了数据包,
		//将收到的数据添加到client的接收缓冲区中

		TagPacket* pPktHead = (TagPacket*)&pktHead;
		nRecvedBytes += sizeof(TagPacket);

		nLength = pPktHead->nLength;
		nType = pPktHead->nType;

		// 包还不完整,需要继续收取未收的数据
		PBYTE pByteBuf = new BYTE[nLength];
		memset(pByteBuf, 0, nLength);
		bRet = RecvData(sClient, (PCHAR)pByteBuf, nLength);
		if (!bRet)
		{
			delete pByteBuf;
			pByteBuf = NULL;
			return false;
		}
		// 收到了定长数据在pByteBuf中
		strData.Write(pByteBuf, nLength);

		delete pByteBuf;
		pByteBuf = NULL;

		// Write方法用的是Copy，销毁时逻辑上应该设计成不会影响副本才对。。。
		nRecvedBytes += strData.GetBufferLen();
		if (strData.GetBufferLen() >= nLength)
		{
			nType = pPktHead->nType;
			nBufSize = nLength;
			//说明一个包已经收完整了,可以开始处理了
			//strData.Delete(sizeof(TagPacket));

			return true;
		}

		return false;
	}


	return false;
}
