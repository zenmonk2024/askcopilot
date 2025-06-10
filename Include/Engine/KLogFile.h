//////////////////////////////////////////////////////////////////////
//
// Filename :	KLogFile.h 
// Creater  :	Tony (Jizheng)
// Date     :	2004-8-31
// Comment  :	Implementation for the KLogFile class.
//				�ṩ��¼��־�ļ��Ĺ���ģ��
//
//////////////////////////////////////////////////////////////////////

#ifndef __ENGINE_KLOGFILE_H__
#define __ENGINE_KLOGFILE_H__
#include <time.h>
enum LOG_RECORD_REMIND_LEVEL
{
	LOG_INFO		= 0x1,
	LOG_ATTENTION	= 0x2,
	LOG_WARNING		= 0x4,
	LOG_ERROR		= 0x8,
};

struct KSystemTime 
{
	int	Hour;
	int	Minute;
	int	Second;
};

class ENGINE_API KLogFile
{
public:
	enum KLogFileParam
	{
		MAX_LINE_LEN = 2048,
	};
	///�����ʹ�õĽӿ�
public:
	KLogFile();
	virtual ~KLogFile();

	///brief ָ��LOG�ļ��Ĵ��·�����Լ�ÿ��LOG�ļ��Ĵ�С
	int		InitialLogFile(const char* strRootPath, const char* strFilePrefix, size_t nSize = 0x100000);

	///������¼LOG�ļ�,����LOG_ATTENTION��LOG_WARNING��LOG_ERROR�������������������ʾ�Ƿ��¼��������log��¼
	int		FinishLogFile();

	//��ǰ�򿪵�LOG�ļ���д��һ����Ϣ
	//�������nInfoLen��ʾszInfo�ַ����ĳ��ȣ���������β���������븺����ʾ�ַ�����'\0'�ַ���β��
	void	LogRecord(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfo, int nInfoLen = -1);
	//��¼�Զ��ı���ʽ���ַ���
	void	LogRecordVar(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfoFmt, ...);

	//���԰汾�������LOG
	void	DebugLogRecord(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfo, int nInfoLen = -1);
	//��¼�Զ��ı���ʽ���ַ���
	void	DebugLogRecordVar(LOG_RECORD_REMIND_LEVEL nRemindLevel, const char* szInfoFmt, ...);

	///��ѯ��ǰLOGģ���Ƿ��д�����������LOG_ATTENTION��LOG_WARNING��LOG_ERROR�������������������ʾ�Ƿ��¼��������log��¼
	int		QueryState();

	//�жϵ�ǰlogģ������д��������������ʾ������Ϣ
	void	StateNotifyUi();

	///�ڲ�ʵ��
private:
	///�½�һ��LOG�ļ�
	int		PrepareLogFile();

	///�رյ�ǰ���ڼ�¼��LOG�ļ�
	int		CloseLogFile();

	///Attribs
private:
	IFile*			m_pFile;
	char			m_szFullname[MAX_PATH];
	char			m_strRoot[64];
	char			m_strFilePrefix[64];

	size_t			m_nMaxSize;
	int				m_bCurrentError;
	int				m_bCurrentWarning;

	int				m_nCurrentVersion;
	KSystemTime		m_nCurrentTime;
	int				m_nCurrentState;
	struct tm				m_lastLogFileTime;
};

#endif // !defined __ENGINE_KLOGFILE_H__
