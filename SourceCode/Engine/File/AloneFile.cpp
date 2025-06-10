//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2005 by Kingsoft
// File:	KAloneFile.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis), Wooy
// Desc:	Win32 File Operation Class
//---------------------------------------------------------------------------
#include "Precompile.h"
#include "AloneFile.h"

KRecycleBin<KAloneFile, 4, RBAFM_NEW_DELETE>	KAloneFile::ms_Recycle;
#define CONTENT_BUFFER_NOT_SUPPORT	(m_nContentBufferSize = (unsigned long)(-1))     // �����Ǹ�ֵ��䣿������һ�������ô�����ǱȽ�==;�����棬����û��IS
#define	IS_CONTENT_BUFFER_SUPPORT	(m_nContentBufferSize != (unsigned long)(-1))

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KAloneFile::KAloneFile()
{
	m_hFile	= NULL;
	m_pContentBuffer = NULL;
	m_nContentBufferSize = 0;
}

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KAloneFile::~KAloneFile()
{
	Close();
}

//---------------------------------------------------------------------------
// ����:	��һ���ļ���׼����ȡд
// ����:	FileName	�ļ���
// ����:	�ɹ�����TRUE��ʧ�ܷ���FALSE��
//---------------------------------------------------------------------------
int	KAloneFile::Open(const char* FileName, int WriteSupport /*= false*/)
{
	//��ڲ����Ϸ����ж���engine�Ķ���ӿں����������ﲻ�ٴ���

	Close();

	char PathName[MAX_PATH];
	g_GetFullPath(PathName, FileName);

	#ifdef __linux
	{
		char *ptr = PathName;
        while(*ptr)
		{
			if (*ptr == '\\')
				*ptr = '/';
			ptr++;
        }
	}
	#endif	// #ifdef __linux

	const char*	pMode = "rb";
	if (WriteSupport)
	{
		if (g_IsFileExist(PathName))
			pMode = "r+b";
		else
			pMode = "a+b";
	}
	m_hFile = fopen(PathName, pMode);

	#ifdef __linux
	{
		if (m_hFile == NULL)
		{
			g_StrLower(PathName);
			m_hFile = fopen(PathName, pMode);
		}
	}
	#endif	// #ifdef __linux

	if (WriteSupport && m_hFile)
		CONTENT_BUFFER_NOT_SUPPORT;

	return (m_hFile != NULL);
}
//---------------------------------------------------------------------------
// ����:	����һ���ļ���׼��д�롣
// ����:	FileName	�ļ���
// ����:	�ɹ�����TRUE��ʧ�ܷ���FALSE��
//---------------------------------------------------------------------------
int	KAloneFile::Create(const char*  FileName)
{
	//��ڲ����Ϸ����ж���engine�Ķ���ӿں����������ﲻ�ٴ���

	Close();

	char PathName[MAX_PATH];
	g_GetFullPath(PathName, FileName);
	m_hFile = fopen(PathName, "wb+");

	return (m_hFile != NULL);
}

//---------------------------------------------------------------------------
// ����:	�رմ򿪵��ļ�
//---------------------------------------------------------------------------
void KAloneFile::Close()
{
	m_nContentBufferSize = 0;
	SAFE_FREE(m_pContentBuffer);
	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile	= NULL;
	}
}

//---------------------------------------------------------------------------
// ����:	��ȡ�ļ�����
// ����:	Buffer	��ȡ���ݴ�ŵ��ڴ�����
//			ReadBytes	��ȡ���ݵ��ֽ���
// ����:	�ɹ����ض�ȡ���ֽ�����ʧ�ܷ���0��
//---------------------------------------------------------------------------
unsigned long KAloneFile::Read(void* Buffer, unsigned long ReadBytes)
{
	long BytesReaded;
	if (m_hFile)
		BytesReaded = (unsigned long)fread(Buffer, 1, ReadBytes, m_hFile);
	else
		BytesReaded = 0;
	return BytesReaded;
}

//---------------------------------------------------------------------------
// ����:	д���ļ�����
// ����:	Buffer		д�����ݴ�ŵ��ڴ�����
//			WriteBytes	д�����ݵ��ֽ���
// ����:	�ɹ�����д����ֽ�����ʧ�ܷ���0��
//---------------------------------------------------------------------------
unsigned long KAloneFile::Write(const void* Buffer, unsigned long WriteBytes)
{
	unsigned long BytesWrited;
	if (m_hFile)
		BytesWrited = (unsigned long)fwrite(Buffer, 1, WriteBytes, m_hFile);
	else
		BytesWrited = 0;

	return BytesWrited;
}

// ����ļ�����Buffer
void* KAloneFile::GetBuffer()
{
	if (m_pContentBuffer == NULL && m_hFile && IS_CONTENT_BUFFER_SUPPORT == true)
	{
		m_nContentBufferSize = Size();
		if (m_nContentBufferSize)
		{
			m_pContentBuffer = malloc(m_nContentBufferSize);
			if (m_pContentBuffer)
			{
				Seek(0, SEEK_SET);
				if (Read(m_pContentBuffer, m_nContentBufferSize) != m_nContentBufferSize)
				{
					free(m_pContentBuffer);
					m_pContentBuffer = NULL;
				}
			}
		}
	}
	return m_pContentBuffer;
}

//---------------------------------------------------------------------------
// ����:	�ƶ��ļ�ָ��λ��
// ����:	Offset		Number of bytes from origin. 
//			Origin		Initial position: SEEK_CUR SEEK_END SEEK_SET
// ����:	�ɹ�����ָ��λ�ã�ʧ�ܷ���-1��
//---------------------------------------------------------------------------
long KAloneFile::Seek(long Offset, int Origin)
{
	if (m_hFile)
	{
		fseek(m_hFile, Offset, Origin);
		Offset = ftell(m_hFile);
	}
	else
	{
		Offset = -1;
	}

	return Offset;
}

//---------------------------------------------------------------------------
// ����:	ȡ���ļ�ָ��λ��
// ����:	�ɹ�����ָ��λ�ã�ʧ�ܷ���-1��
//---------------------------------------------------------------------------
long KAloneFile::Tell()
{
	long Offset;
	if (m_hFile)
		Offset = ftell(m_hFile);
	else
		Offset = -1;
	return Offset;
}

//---------------------------------------------------------------------------
// ����:	ȡ���ļ�����
// ����:	�ɹ������ļ����ȣ�ʧ�ܷ���0��
//---------------------------------------------------------------------------
unsigned long KAloneFile::Size()
{
	unsigned long Size;
	if (m_pContentBuffer)
	{
		Size = m_nContentBufferSize;
	}
	else if (m_hFile)
	{
		long Offset = ftell(m_hFile);
		fseek(m_hFile, 0, SEEK_END);
		Size = ftell(m_hFile);
		fseek(m_hFile, Offset, SEEK_SET);
//		if (Size == (unsigned long)(-1))
//			Size = 0;
	}
	else
	{
		Size = 0;
	}
	return Size;
}

// �жϴ򿪵��Ƿ��ǰ��е��ļ�
int	KAloneFile::IsFileInPak()
{
	return false;
}

void KAloneFile::Release()
{
	Close();
	ms_Recycle.Delete(this);
}

//��������һ�����󣬰������Լ���ȫ������
KAloneFile* KAloneFile::Deprive()
{
	KAloneFile* pClone = ms_Recycle.New();
	if (pClone)
	{
		*pClone = *this;
		m_hFile	= NULL;
	}
	return pClone;
}

//����һ���յ�KAloneFile����
KAloneFile*	KAloneFile::New()
{
	return ms_Recycle.New();
}

//---------------------------------------------------------------------------
// ����:	�ж��Ƿ�ֿ�ѹ���ļ�

// ����:	��0ֵΪ�ֿ�ѹ���ļ�
//---------------------------------------------------------------------------
int	KAloneFile::IsPackedByFragment()
{
	return 0;
}

//��ȡ�ļ��ֿ����Ŀ
int	KAloneFile::GetFragmentCount()
{
	return 0;
}

//��ȡ�ֿ�Ĵ�С
unsigned int KAloneFile::GetFragmentSize(int nFragmentIndex)
{
	return 0;
}

//��ȡһ���ļ��ֿ�
unsigned long	KAloneFile::ReadFragment(int nFragmentIndex, void*& pBuffer)
{
	return 0;
}
