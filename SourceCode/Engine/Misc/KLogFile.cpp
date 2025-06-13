//////////////////////////////////////////////////////////////////////
//
// Filename :	KLogFile.cpp 
// Creater  :	Tony (Jizheng)
// Date     :	2004-8-31
// Comment  :	Implementation for the KLogFile class.
//				�ṩ��¼��־�ļ��Ĺ���ģ��
//
//////////////////////////////////////////////////////////////////////

#include "Precompile.h"
#include "File.h"
#include "KLogFile.h"
#include "Debug.h"
#include "KGCRT.h"
#include <stdarg.h>

#ifdef __linux
	#define	CH_SEPARATOR '/'
#else
	#define CH_SEPARATOR '\\'
#endif

const char		m_strFileSuffix[] = ".log";

/**
*@brief Construction
*/
KLogFile::KLogFile()
{
	m_pFile = NULL;
	m_nCurrentState = 0;
	m_nCurrentVersion = 0;
	m_nCurrentTime.Hour = 0;
	m_nCurrentTime.Minute = 0;
	m_nCurrentTime.Second = 0;
	m_nMaxSize = 0;
	m_szFullname[0] = 0;
	m_strRoot[0] = 0;
	m_strFilePrefix[0] = 0;
	memset(&m_lastLogFileTime,0 ,sizeof(tm));
}

/**
*@brief Destruction
*/
KLogFile::~KLogFile()
{
	FinishLogFile();
}

/**
*@brief ָ��LOG�ļ��Ĵ��·�����Լ�ÿ��LOG�ļ��Ĵ�С
*/
BOOL KLogFile::InitialLogFile(const char* strRootPath, const char* strFilePrefix, size_t nSize)
{
	if (nSize == 0)
	{
		return FALSE;
	}

	int nLen = strlen(strRootPath);
	if (nLen > sizeof(m_strRoot) - 4 || strchr(strFilePrefix, '%'))
		return FALSE;

	memcpy(m_strRoot, strRootPath, nLen + 1);
	if (nLen)
	{
		if (m_strRoot[nLen - 1] != CH_SEPARATOR)
		{
			m_strRoot[nLen] = CH_SEPARATOR;
			m_strRoot[nLen + 1] = '\0';
		}	
		g_CreatePath(m_strRoot);
	}

	strcpy(m_strFilePrefix, strFilePrefix);
	m_nMaxSize = nSize;

	m_nCurrentState = 0;
	return TRUE;
}

/**
*@brief ������¼LOG�ļ�
*/
int KLogFile::FinishLogFile()
{
	CloseLogFile();
	return m_nCurrentState;
}

/**
*@brief ��ǰ�򿪵�LOG�ļ���д��һ����Ϣ
*/
//�������nInfoLen��ʾszInfo�ַ����ĳ��ȣ���������β���������븺����ʾ�ַ�����'\0'�ַ���β��
void KLogFile::LogRecord(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfo, int nInfoLen/* = -1*/)
{
	assert(szInfo);

 	time_t tNow = time(NULL);
	struct tm tmNow = {0};
	localtime_r(&tNow, &tmNow);

	if (m_pFile == NULL ||
		m_pFile->Size() > m_nMaxSize ||
                ((m_lastLogFileTime.tm_year != tmNow.tm_year)|| 
                (m_lastLogFileTime.tm_mon != tmNow.tm_mon)||
                (m_lastLogFileTime.tm_mday != tmNow.tm_mday))
           )
	{
		CloseLogFile();
		if (!PrepareLogFile())
			return;
                m_lastLogFileTime = tmNow;
	}

	char szBuffer[MAX_LINE_LEN];
	const char* pszReminder = "INFO";

	///���ݴ���ļ�¼�ȼ���������LOG�ļ��еı��
	switch (nRemindLevel) 
	{
	case LOG_ATTENTION:
		pszReminder = "ATTEN";
		m_nCurrentState |= 0x2;
		break;
	case LOG_WARNING:
		pszReminder = "WARN";
		m_bCurrentWarning = TRUE;
		m_nCurrentState |= 0x4;
		break;
	case LOG_ERROR:
		pszReminder = "ERR!";
		m_bCurrentError = TRUE;
		m_nCurrentState |= 0x8;
		break;
	}

	if (nInfoLen < 0)
		nInfoLen = strlen(szInfo);
	int nLen = sprintf(szBuffer, "[%04d-%02d-%02d %02d:%02d:%02d]\t%s\t",
		tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday,
		tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec,
		pszReminder);

	if (nInfoLen >= MAX_LINE_LEN - 4 - nLen)
		nInfoLen = MAX_LINE_LEN - 4 - nLen;

	memcpy(szBuffer + nLen, szInfo, nInfoLen);
	nLen += nInfoLen;
	strcpy(szBuffer + nLen, "\r\n");

	m_pFile->Write(szBuffer, nLen + 2/*"\r\n"*/);
}

//��¼�Զ��ı���ʽ���ַ���
void KLogFile::LogRecordVar(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfoFmt, ...)
{
	char szBuffer[MAX_LINE_LEN - 24];
	va_list va;
	va_start(va, szInfoFmt);
	int nInfoLen = vsprintf(szBuffer, szInfoFmt, va);
	va_end(va);
	LogRecord(nRemindLevel, szBuffer, nInfoLen);
}

//���԰汾�������LOG
void KLogFile::DebugLogRecord(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfo, int nInfoLen/* = -1*/)
{
#ifdef _DEBUG
	LogRecord(nRemindLevel, szInfo, nInfoLen);
#endif
}

//��¼�Զ��ı���ʽ���ַ���
void KLogFile::DebugLogRecordVar(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfoFmt, ...)
{
#ifdef _DEBUG
	char szBuffer[MAX_LINE_LEN - 24];
	va_list va;
	va_start(va, szInfoFmt);
	int nInfoLen = vsprintf(szBuffer, szInfoFmt, va);
	va_end(va);
	LogRecord(nRemindLevel, szBuffer, nInfoLen);
#endif
}

/**
*@brief ��ѯ��ǰLOGģ���Ƿ��д�����
*@return ״̬ʾ�⣺
*        0001 - INFO, 0010 - ATTEN, 0100 - WARN, 1000 - ERROR
*        0011 - INFO and ATTEN
*        ��������
*/
int	KLogFile::QueryState()
{
	return m_nCurrentState;
}

///private access
/**
*@brief �½�һ��LOG�ļ�
*/
BOOL KLogFile::PrepareLogFile()
{
	m_bCurrentError = 0;
	m_bCurrentWarning = 0;

	time_t tNow = time(NULL);
	struct tm tmNow = {0};
	localtime_r(&tNow, &tmNow);

	///���������LOG�ļ���
	char	path[MAX_PATH];

	sprintf(path, "%s%04d_%02d_%02d", m_strRoot, tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday);

	g_CreatePath(path);

	char szTime[128];
	char szFullname[MAX_PATH];

	///���ͬһ����ֵļ�¼��Ҫ�ֶ��LOG�ļ����棬��ʹ�����ֺ�׺���ְ汾
	if (m_nCurrentTime.Hour == tmNow.tm_hour && 
		m_nCurrentTime.Minute == tmNow.tm_min && 
		m_nCurrentTime.Second == tmNow.tm_sec)
	{
		m_nCurrentVersion++;
		sprintf(szTime, "_%02d_%02d_%02d-%02d", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, m_nCurrentVersion);
	}
	else
	{
		m_nCurrentVersion = 0;
		m_nCurrentTime.Hour = tmNow.tm_hour;
		m_nCurrentTime.Minute = tmNow.tm_min;
		m_nCurrentTime.Second = tmNow.tm_sec;
		sprintf(szTime, "_%02d_%02d_%02d", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
	}

	//m_szFullname���治��.log��׺�����ļ���
	sprintf(m_szFullname, "%s%c%s%s", path, CH_SEPARATOR, m_strFilePrefix, szTime);

	//�½��򿪴�.log��׺�����ļ�
	sprintf(szFullname, "%s%s", m_szFullname, m_strFileSuffix);
	m_pFile = g_OpenFile(szFullname, TRUE, TRUE);

	return (m_pFile != NULL);
}

/**
*@brief �رյ�ǰ���ڼ�¼��LOG�ļ�
*/
BOOL KLogFile::CloseLogFile()
{
	if (m_pFile == NULL)
	{
		return FALSE;
	}

	int bEmpty = !m_pFile->Size();
	m_pFile->Release();
	m_pFile = NULL;

	char szFullname[MAX_PATH];
	g_GetFullPath(szFullname, m_szFullname);
	strcat(szFullname, m_strFileSuffix);

	int bOk = TRUE;
	if (bEmpty)		///���LOG�ļ�Ϊ�գ�ɾ������LOG�ļ�
	{
		remove(szFullname);
	}		
	else			///�ж�LOG�ļ����Ƿ��д�����߾��棬���������ļ������б�ʾ
	{
		char szFinalLog[MAX_PATH];
		g_GetFullPath(szFinalLog, m_szFullname);

		if (m_bCurrentError && m_bCurrentWarning)
		{
			strcat(szFinalLog, "=EW");
		}
		else if (m_bCurrentError)
		{
			strcat(szFinalLog, "=E");
		}
		else if (m_bCurrentWarning)
		{
			strcat(szFinalLog, "=W");
		}
		strcat(szFinalLog, m_strFileSuffix);

		bOk = (rename(szFullname, szFinalLog) != -1);
	}

	return bOk;
}

//�жϵ�ǰlogģ������д��������������ʾ������Ϣ
void KLogFile::StateNotifyUi()
{
	if (m_nCurrentState)
	{
		char	info[1024];
		int		nLen = sprintf(info, "------------! ATTENTION !-------------\n\n");
		if	(m_nCurrentState & LOG_ERROR)
			nLen += sprintf(info + nLen, "There's ERROR in <%s> LOG!\n", m_strFilePrefix);
		if (m_nCurrentState & LOG_WARNING)
			nLen += sprintf(info + nLen, "There's WARNING in <%s> LOG!\n", m_strFilePrefix);
		if (m_nCurrentState & LOG_ATTENTION)
			sprintf(info + nLen, "There's ATTENTION in <%s> LOG!\n", m_strFilePrefix);
		strcat(info, "--------------------------------------\n");
		g_MessageBox(info);
	}
}
