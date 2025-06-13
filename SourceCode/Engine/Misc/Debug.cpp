//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDebug.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Debug Helper Functions
//---------------------------------------------------------------------------
#include "Precompile.h"
#include <stdarg.h>

//---------------------------------------------------------------------------
// ����:	g_DebugLog
// ����:	����Դ������������Ϣ
// ����:	Fmt		��ʽ���ַ���
//			...		������ַ���
//---------------------------------------------------------------------------
C_ENGINE_API
void g_DebugLog(const char* Fmt, ...)
{
	#ifdef _DEBUG
		char buffer[1024];
		va_list va;
		va_start(va, Fmt);
		vsprintf(buffer, Fmt, va);
		va_end(va);
		strcat(buffer, "\n");
		#ifdef __linux
			printf("%s", buffer);
		#else
			OutputDebugString(buffer);
		#endif
	#endif
}

//---------------------------------------------------------------------------
// ����:	g_MessageBox
// ����:	Display a System Message Box
// ����:	char* lpMsg, ...
//---------------------------------------------------------------------------
C_ENGINE_API
void g_MessageBox(const char* lpMsg, ...)
{
	char szMsg[1024];
	va_list va;
	va_start(va, lpMsg);
	vsprintf(szMsg, lpMsg, va);
	va_end(va);
	g_DebugLog(szMsg);
		
	#ifndef __linux
		HWND	hWnd = NULL;
		GUITHREADINFO	info;
		memset(&info, 0, sizeof(info));
		info.cbSize = sizeof(info);
		if (GetGUIThreadInfo(NULL, &info))
			hWnd = info.hwndActive;
		MessageBox(hWnd, szMsg, 0, MB_OK | MB_ICONINFORMATION);
	#endif
}

//---------------------------------------------------------------------------
// ����:	g_AssertFailed
// ����:	����ʧ��
// ����:	FileName	���ĸ��ļ���ʧ��
//			LineNum		���ļ��еĵڼ���
//---------------------------------------------------------------------------
C_ENGINE_API
void g_AssertFailed(const char* pFileName, int nLineNum)
{
	g_MessageBox("Assert failed in %s, line = %i", pFileName, nLineNum);
	assert(0);
}

//---------------------------------------------------------------------------
/*KSG_LogFile::KSG_LogFile(const char cszFileName[])
{
    if (!cszFileName)
        return;

    m_fpFile = fopen(cszFileName, "ab+");
    if (!m_fpFile)
    {
        //printf("Open Log File %s error!\n", cszFileName);
        return;
    }
}

KSG_LogFile::KSG_LogFile()
{
    m_fpFile = NULL;
}



KSG_LogFile::~KSG_LogFile()
{
    if (m_fpFile)
    {
        fclose(m_fpFile);
        m_fpFile = NULL;
    }
}

int KSG_LogFile::Init(const char cszFileName[])
{
    if (!cszFileName)
        return false;

    if (m_fpFile)
    {
        fclose(m_fpFile);
        m_fpFile = NULL;
    }

    m_fpFile = fopen(cszFileName, "ab+");
    if (!m_fpFile)
    {
        return false;
    }

    return true;
}

*/

