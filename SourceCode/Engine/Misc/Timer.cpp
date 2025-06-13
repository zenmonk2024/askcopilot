//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Timer Class
//---------------------------------------------------------------------------
#include "Precompile.h"
#include "Timer.h"

#ifdef __linux
	#include <sys/time.h>
#endif

KTimer::KTimer()
{
	#ifdef WIN32
		m_nFrequency.QuadPart = 200 * 1024 * 1024;
		m_nTimeStart.QuadPart = 0;
		m_nTimeStop.QuadPart = 0;
		m_nFPS = 0;
		QueryPerformanceFrequency(&m_nFrequency);
	#else
		//m_nFrequency = CLOCKS_PER_SEC;
	#endif
}
//---------------------------------------------------------------------------
// ����:	Start
// ����:	��ʼ��ʱ
//---------------------------------------------------------------------------
void KTimer::Start()
{
#ifdef WIN32
	QueryPerformanceCounter(&m_nTimeStart);
#else
    gettimeofday(&m_nTimeStart, NULL);
#endif
}
//---------------------------------------------------------------------------
// ����:	Stop
// ����:	ֹͣ��ʱ
//---------------------------------------------------------------------------
void KTimer::Stop()
{
#ifdef WIN32
	QueryPerformanceCounter(&m_nTimeStop);
#else
	gettimeofday(&m_nTimeStop, NULL);
#endif
}
//---------------------------------------------------------------------------
// ����:	GetElapse
// ����:	����ӿ�ʼ��ʱ�������Ѿ�����ʱ��
// ����:	unsigned int in ms
//---------------------------------------------------------------------------
unsigned int KTimer::GetElapse()
{
	#ifdef WIN32
		LARGE_INTEGER nTime;
		QueryPerformanceCounter(&nTime);
		return (unsigned int)((nTime.QuadPart - m_nTimeStart.QuadPart) 
			* 1000 / m_nFrequency.QuadPart);
	#else
		timeval tv;
		gettimeofday(&tv, NULL);
		return (tv.tv_sec - m_nTimeStart.tv_sec) * 1000 + tv.tv_usec / 1000;
	#endif
}

//---------------------------------------------------------------------------
// ����:	GetElapseFrequency
// ����:	����ӿ�ʼ��ʱ�������Ѿ�����ʱ��
// ����:	unsigned int in frequency
//---------------------------------------------------------------------------
unsigned int KTimer::GetElapseFrequency()
{
	#ifdef WIN32
		LARGE_INTEGER nTime;
		QueryPerformanceCounter(&nTime);
		return (unsigned int)(nTime.QuadPart - m_nTimeStart.QuadPart);
	#endif
		return 0;
}
//---------------------------------------------------------------------------
// ����:	GetElapseFrequency
// ����:	����ӿ�ʼ��ʱ�������Ѿ�����ʱ��
// ����:	unsigned int in frequency
//---------------------------------------------------------------------------
unsigned int KTimer::GetElapseMicrosecond()
{
#ifdef WIN32
	LARGE_INTEGER nTime;
	QueryPerformanceCounter(&nTime);
	return (unsigned int)((nTime.QuadPart - m_nTimeStart.QuadPart) 
		* 1000000 / m_nFrequency.QuadPart);
#else
	timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec - m_nTimeStart.tv_sec) * 1000000 + tv.tv_usec;
#endif
}
//---------------------------------------------------------------------------
// ����:	GetInterval
// ����:	ȡ�ôӿ�ʼ��ֹ֮ͣ���ʱ�������Ժ���Ϊ��λ
// ����:	����ֵ
//---------------------------------------------------------------------------
unsigned int KTimer::GetInterval()
{
	#ifdef WIN32
		return (unsigned int)((m_nTimeStop.QuadPart - m_nTimeStart.QuadPart) 
			* 1000 / m_nFrequency.QuadPart);
	#endif
		return 0;
}

//---------------------------------------------------------------------------
// ����:	Passed
// ����:	���Ƿ����nTime����
// ����:	nTime	����
// ����:	true	�Ѿ�����
//			false	��û�й�
//---------------------------------------------------------------------------
int KTimer::Passed(int nTime)
{

	if (GetElapse() >= (unsigned int)nTime)
	{
		Start();
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
// ����:	GetFPS
// ����:	ȡ��֡����
// ����:	nFPS	֡����
// ����:	true	�ɹ�
//			false	ʧ��
//---------------------------------------------------------------------------
int KTimer::GetFPS(int *nFPS)
{
	if (GetElapse() >= 1000)
	{
		*nFPS = m_nFPS;
		m_nFPS = 0;
		Start();
		return true;
	}
	m_nFPS++;
	return false;
}
