//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPackList.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Pack Data List Class
//---------------------------------------------------------------------------
#include "Precompile.h"
#include "XPackList.h"
#include "FileType.h"
#include "SmartPtr.h"

//---------------------------------------------------------------------------
KPackList g_EnginePackList;

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KPackList::KPackList()
{
	m_PakNumber = 0;
}

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KPackList::~KPackList()
{
	Close();
}

//---------------------------------------------------------------------------
// ����:	�ر������ļ�
//---------------------------------------------------------------------------
void KPackList::Close()
{
	for (int i = 0; i < m_PakNumber; i++)
		delete m_PakFilePtrList[i];
	m_PakNumber = 0;

	XPackFile::Terminate();
}

//---------------------------------------------------------------------------
// ����:	�����а���ɨ��ָ���ļ�
// ����:	uId			�ļ���ID
//			ElemRef		���ڴ�ţ��������ļ���Ϣ
// ����:	�Ƿ�ɹ��ҵ�
//---------------------------------------------------------------------------
int KPackList::FindElemFile(unsigned int uId, XPackFile::XPackElemFileRef& ElemRef)
{
	int nFounded = false;
	for (int i = 0; i < m_PakNumber; i++)
	{
		if (m_PakFilePtrList[i]->FindElemFile(uId, ElemRef))
		{
			nFounded = true;
			break;
		}
	}
	return nFounded;
}

//---------------------------------------------------------------------------
// ����:	�����а���ɨ��ָ���ļ�
// ����:	pszFileName	�ļ���
//			ElemRef	���ڴ�ţ��������ļ���Ϣ
// ����:	�Ƿ�ɹ��ҵ�
//---------------------------------------------------------------------------
int KPackList::FindElemFile(const char* pszFileName, XPackFile::XPackElemFileRef& ElemRef)
{
	int nFounded = false;
	if (pszFileName && pszFileName[0])
	{
		char szPackName[256];
		#ifdef WIN32
			szPackName[0] = '\\';
		#else
			szPackName[0] = '/';
		#endif
		g_GetPackPath(szPackName + 1, pszFileName);
		unsigned int uId = g_FileNameHash(szPackName);
		nFounded = FindElemFile(uId, ElemRef);
	}
	return nFounded;
}

//--------------------------------------------------------------------
// ����:	Open package ini file
// ����:	char* filename
//--------------------------------------------------------------------
int KPackList::Open(const char* pPakListFile, const char* pSection)
{
	Close();

	if (!XPackFile::Initialize())
		return false;

	KSmartIniFile pIni;
	pIni = g_OpenIniFile(pPakListFile, true, false);
	if (pIni == NULL)
		return false;

	char	szBuffer[64], szKey[16], szFile[MAX_PATH];

	if (pIni->GetString(pSection, "Path", "", szBuffer, sizeof(szBuffer)))
		//g_GetFullPath(szFile, szBuffer);
		strcpy(szFile, szBuffer);
	else
		g_GetRootPath(szFile);
	int nNameStartPos = (int)strlen(szFile);

	if (szFile[nNameStartPos - 1] != '\\' || szFile[nNameStartPos - 1] != '/')
	{
		#ifdef WIN32
			szFile[nNameStartPos++] = '\\';
		#else
			szFile[nNameStartPos++] = '/';
		#endif
		szFile[nNameStartPos] = 0;
	}

	for (int i = 0; i < PAK_LIST_MAX_PAK; i++)
	{
		#ifdef WIN32
			itoa(i, szKey, 10);
		#else
			sprintf(szKey, "%d", i);
		#endif
		if (!pIni->GetString(pSection, szKey, "", szBuffer, sizeof(szBuffer)))
			break;
		if (szBuffer[0] == 0)
			break;
		strcpy(szFile + nNameStartPos, szBuffer);
		m_PakFilePtrList[m_PakNumber] = new XPackFile;
		if (m_PakFilePtrList[m_PakNumber])
		{
			if (m_PakFilePtrList[m_PakNumber]->Open(szFile, m_PakNumber))
			{
				m_PakNumber++;
			}
			else
			{
				delete (m_PakFilePtrList[m_PakNumber]);
			}
		}
	}

	return true;
}

//��ȡ���ڵ����ļ�
unsigned long KPackList::ElemFileRead(XPackFile::XPackElemFileRef& ElemRef,
					void* pBuffer, unsigned long uSize)
{
	if (ElemRef.PakFileIndex >= 0 && ElemRef.PakFileIndex < m_PakNumber)
		return m_PakFilePtrList[ElemRef.PakFileIndex]->ElemFileRead(ElemRef, pBuffer, uSize);
	return 0;
}


//---------------------------------------------------------------------------
// ����:	�ֿ��ȡ�ļ�
// ����:	ElemRef			�ڰ��ж�λҪ��ȡ�ļ�������
//			Buffer			������ָ��
//			ReadBytes		Ҫ��ȡ�ĳ���
//			Offset			Number of bytes from origin. 
//			CompressSize	�ļ�ѹ���Ĵ�С��0��ʾû��ѹ������ȡ��ʽΪֱ�Ӷ�ȡ�������ı�ʾΪ��ѹ��ȡ
// ����:	�������ֽڳ���
//---------------------------------------------------------------------------
unsigned long KPackList::ElemReadFragment(XPackFile::XPackElemFileRef& ElemRef, int nFragmentIndex, void*& Buffer)
{
	return m_PakFilePtrList[ElemRef.PakFileIndex]->XElemReadFragment(ElemRef.ElemFileIndex, nFragmentIndex, Buffer);
}


//---------------------------------------------------------------------------
// ����:	�ж��Ƿ�ֿ�ѹ���ļ�

// ����:	��0ֵΪ�ֿ�ѹ���ļ�
//---------------------------------------------------------------------------
int KPackList::ElemIsPackedByFragment(XPackFile::XPackElemFileRef& ElemRef)
{
	return m_PakFilePtrList[ElemRef.PakFileIndex]->XElemIsPackedByFragment(ElemRef.ElemFileIndex);
}

//��ȡ�ļ��ֿ����Ŀ
int	KPackList::ElemGetFragmentCount(XPackFile::XPackElemFileRef& ElemRef)
{
	return m_PakFilePtrList[ElemRef.PakFileIndex]->XElemGetFragmentCount(ElemRef.ElemFileIndex);
}

//��ȡĳ�����ļ�ĳ���ֿ�Ĵ�С
unsigned int KPackList::ElemGetFragmentSize(XPackFile::XPackElemFileRef& ElemRef, int nFragmentIndex)
{
	return m_PakFilePtrList[ElemRef.PakFileIndex]->ElemGetFragmentSize(ElemRef.ElemFileIndex, nFragmentIndex);
}

//��ð����������ļ��������ݵĻ�����
void* KPackList::GetElemFileBuffer(XPackFile::XPackElemFileRef& ElemRef)
{
	if (ElemRef.PakFileIndex >= 0 && ElemRef.PakFileIndex < m_PakNumber)
		return m_PakFilePtrList[ElemRef.PakFileIndex]->GetElemFileBuffer(ElemRef);
	return NULL;
}
