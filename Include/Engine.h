//---------------------------------------------------------------------------
//  Engineͷ�ļ�,����Engine������ģ�������ͷ�ʼ������Engine���ֽӿ����ݵĶ���
//	Copyright : Kingsoft Season 2004
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2004-3-21
//---------------------------------------------------------------------------
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "KGStdAfx.h"
#include <string.h>		// �����°汾GCC�ı������
#include "./CommonDefine.h"
#include "./Engine/EngineBase.h"

#include "./Engine/Debug.h"
#include "./Engine/EDOneTimePad.h"		//���������
#include "./Engine/KSG_MD5_String.h"	//md5���������
#include "./Engine/KSG_StringProcess.h"	//�ַ�������
#include "./Engine/Text.h"				//���ִ���
#include "./Engine/File.h"				//�ļ�����
#include "./Engine/FileType.h"			//ini�ļ�����
#include "./Engine/KFileCache.h"		//�ļ�����
#include "./Engine/Random.h"
#include "./Engine/KCodePoints.h"
#include "./Engine/KList.h"
#include "./Engine/LinkStructEx.h"
#include "./Engine/KBinsTree.h"
#include "./Engine/SmartPtr.h"
#include "./Engine/KPolygon.h"
#include "./Engine/KLinkArray.h"		//�ɹ��ܴ��룬�´�����Ӧ��KOccupyList
#include "./Engine/KOccupyList.h"		//ռ�ñ�
#include "./Engine/KLogFile.h"			//��־��¼����ģ��(Jizheng 2004-9-2)
#include "./Engine/KLuaWrap.h"

#include "./Engine/CRC32.h"

//----���½ӿڵĶ����漰��ƽ̨���----
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <pthread.h>
	#include <assert.h>
#endif

#include "./Engine/Mutex.h"
#include "./Engine/RecycleBin.h"
#include "./Engine/Timer.h"
#include "./Engine/KThread.h"
#include "./Engine/ObjCache.h"

#ifdef WIN32
	#include "./Engine/KWin32App.h"
	#include "./Engine/KUrl.h"
	#include "./Engine/Kime.h"
#endif

#endif //ifndef _ENGINE_H_
