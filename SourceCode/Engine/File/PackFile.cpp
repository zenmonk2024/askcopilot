//---------------------------------------------------------------------------
//  Engine�ļ�ģ��-Դ�ڴ���ļ����ļ�
//	Copyright : Kingsoft Season 2004
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2004-5-18
//---------------------------------------------------------------------------
#include "Precompile.h"
#include "PackFile.h"
#include "XPackList.h"

KRecycleBin<KPackFile, 4, RBAFM_NEW_DELETE>	KPackFile::ms_Recycle;

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KPackFile::KPackFile()
{
	m_Core.NameId = 0;
	m_Core.CacheIndex = 0;
	m_Core.ElemFileIndex = 0;
	m_Core.PakFileIndex = -1;
	m_Core.Offset = 0;
	m_Core.Size = 0;
	m_pContentBuffer = NULL;
}

//---------------------------------------------------------------------------
// ����:	���캯��
//---------------------------------------------------------------------------
KPackFile::~KPackFile()
{
	Close();
}

//---------------------------------------------------------------------------
// ����:	�жϴ��ļ��Ƿ�Ӱ��д򿪵�
//---------------------------------------------------------------------------
// �жϴ򿪵��Ƿ��ǰ��е��ļ�
int	KPackFile::IsFileInPak()
{
	return true;
}

//---------------------------------------------------------------------------
// ����:	��һ���ļ�, ��Ѱ�ҵ�ǰĿ¼���Ƿ���ͬ���ĵ����ļ�,
// ����:	FileName	�ļ���
// ����:	TRUE		�ɹ�
//			FALSE		ʧ��
//---------------------------------------------------------------------------
int KPackFile::Open(const char* FileName)
{
	//�����ĺϷ��Լ���ڸ���һ����
	Close();
	if (g_EnginePackList.FindElemFile(FileName, m_Core))
		return true;
	Close();
	return false;
}

//---------------------------------------------------------------------------
// ����:	���ļ��ж�ȡ����
// ����:	pBuffer		������ָ��
//			dwSize		Ҫ��ȡ�ĳ���
// ����:	�������ֽڳ���
//---------------------------------------------------------------------------
unsigned long KPackFile::Read(void* Buffer, unsigned long ReadBytes)
{
	if (m_pContentBuffer)
	{
		if (m_Core.Offset < 0)
			m_Core.Offset = 0;
		if ((unsigned long)m_Core.Offset < m_Core.Size)
		{
			if ((unsigned long)(m_Core.Offset + ReadBytes) > m_Core.Size)
				ReadBytes = m_Core.Size - m_Core.Offset;
			memcpy(Buffer, (char*)m_pContentBuffer + m_Core.Offset, ReadBytes);
			m_Core.Offset += ReadBytes;
		}
		else
		{
			m_Core.Offset = m_Core.Size;
		}
		return ReadBytes;
	}

	if (m_Core.NameId)
		return g_EnginePackList.ElemFileRead(m_Core, Buffer, ReadBytes);
	return 0;
}

// д���ļ�����
unsigned long KPackFile::Write(const void* Buffer, unsigned long WriteBytes)
{
	return 0;
}

// ����ļ�����Buffer
void* KPackFile::GetBuffer()
{
	if (m_Core.NameId && m_pContentBuffer == NULL)
		m_pContentBuffer = g_EnginePackList.GetElemFileBuffer(m_Core);
	return m_pContentBuffer;
}

//---------------------------------------------------------------------------
// ����:	�ֿ��ȡ�ļ�(��ȡһ����)
// ����:	pBuffer			Ŀ�껺����ָ��
//							�������Ļ�����ָ��Ϊ�գ����ڲ�������µĻ�������������ָ�룬�ⲿ��Ҫ�������١�
// ����:	�������ֽڳ���
//		�������Ĵ�С��Ҫ�������·ֿ�����ݣ�����ͨ��GetFragmentCount֪��ÿ���ֿ�����ݴ�С����������������Ҫ����ô��
//		����ֵ��ʾʵ�ʶ�ȡ����������ֿ�����ݴ�С
//---------------------------------------------------------------------------
unsigned long KPackFile::ReadFragment(int nFragmentIndex, void*& pBuffer)
{
	if (m_Core.NameId && nFragmentIndex >= 0)
		return g_EnginePackList.ElemReadFragment(m_Core, nFragmentIndex, pBuffer);
	return 0;
}

//---------------------------------------------------------------------------
// ����:	�ж��Ƿ�ֿ�ѹ���ļ�

// ����:	��0ֵΪ�ֿ�ѹ���ļ�
//---------------------------------------------------------------------------
int	KPackFile::IsPackedByFragment()
{
	if (m_Core.NameId)
		return g_EnginePackList.ElemIsPackedByFragment(m_Core);
	return 0;
}

//��ȡ�ļ��ֿ����Ŀ
int	KPackFile::GetFragmentCount()
{
	if (m_Core.NameId)
		return g_EnginePackList.ElemGetFragmentCount(m_Core);
	return 0;
}

//��ȡ�ֿ�Ĵ�С
unsigned int KPackFile::GetFragmentSize(int nFragmentIndex)
{
	if (m_Core.NameId && nFragmentIndex >= 0)
		return g_EnginePackList.ElemGetFragmentSize(m_Core, nFragmentIndex);
	return 0;
}

//---------------------------------------------------------------------------
// ����:	�ļ���ָ�붨λ
// ����:	Offset		Number of bytes from origin. 
//			Origin		Initial position: SEEK_CUR SEEK_END SEEK_SET
// ����:	�ļ���ָ��
//---------------------------------------------------------------------------
long KPackFile::Seek(long Offset, int Origin)
{
	if (m_Core.NameId)
	{
		if (Origin == SEEK_CUR)
			m_Core.Offset += Offset;
		else if (Origin == SEEK_SET)
			m_Core.Offset = Offset;
		else if (Origin == SEEK_END)
			m_Core.Offset = m_Core.Size + Offset;
		if (m_Core.Offset < 0)
			m_Core.Offset = 0;
		else if ((unsigned long)m_Core.Offset > m_Core.Size)
			m_Core.Offset =  m_Core.Size;
	}
	return m_Core.Offset;
}

//---------------------------------------------------------------------------
// ����:	�����ļ���ָ��
// ����:	�ļ���ָ��
//---------------------------------------------------------------------------
long KPackFile::Tell()
{
	return m_Core.Offset;
}

//---------------------------------------------------------------------------
// ����:	�����ļ���С
// ����:	�ļ��Ĵ�С in bytes
//---------------------------------------------------------------------------
unsigned long KPackFile::Size()
{
	return m_Core.Size;
}

//---------------------------------------------------------------------------
// ����:	�ر�һ���ļ�
//---------------------------------------------------------------------------
void KPackFile::Close()
{
	SAFE_FREE(m_pContentBuffer);
	m_Core.NameId = 0;
	m_Core.PakFileIndex = -1;
	m_Core.Offset = 0;
	m_Core.Size = 0;
}

// �ӿڶ�������
void KPackFile::Release()
{
	Close();
	ms_Recycle.Delete(this);
}

//��������һ�����󣬰������Լ���ȫ������
KPackFile*	KPackFile::Deprive()
{
	KPackFile* pClone = ms_Recycle.New();
	if (pClone)
	{
		*pClone = *this;
		m_Core.NameId = 0;
		m_Core.CacheIndex = 0;
		m_Core.ElemFileIndex = 0;
		m_Core.PakFileIndex = -1;
		m_Core.Offset = 0;
		m_Core.Size = 0;
		m_pContentBuffer = NULL;
	}
	return pClone;
}

//����һ���յ�KPackFile����
KPackFile*	KPackFile::New()
{
	return ms_Recycle.New();
}
