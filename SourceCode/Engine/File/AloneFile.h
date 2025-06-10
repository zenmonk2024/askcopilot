//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// File:	KAloneFile.h
// Date:	2000.08.08
// Code:	wooy
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef _ENGINE_ALONE_FILE_H_
#define _ENGINE_ALONE_FILE_H_

#include "File.h"
#include "RecycleBin.h"

class KAloneFile : public IFile
{
public:
	KAloneFile();
	virtual ~KAloneFile();
//----IFile�ֿں�������ʼ----
	// ��ȡ�ļ�����
	unsigned long	Read(void* Buffer, unsigned long ReadBytes);
	// д���ļ�����
	unsigned long	Write(const void* Buffer, unsigned long WriteBytes);
	// ����ļ�����Buffer
	void*			GetBuffer();
	// �ƶ��ļ�ָ��λ�ã�Origin	-> Initial position: SEEK_CUR SEEK_END SEEK_SET
	long	Seek(long Offset, int Origin);
	// ȡ���ļ�ָ��λ��,ʧ�ܷ���-1��
	long	Tell();
	// ȡ���ļ�����,ʧ�ܷ���0��
	unsigned long	Size();

	// �жϴ򿪵��Ƿ��ǰ��е��ļ�
	int		IsFileInPak();
	//�ж��ļ��Ƿ�ֿ�ѹ����
	int		IsPackedByFragment();
	//��ȡ�ļ��ֿ����Ŀ
	int		GetFragmentCount();
	//��ȡ�ֿ�Ĵ�С
	unsigned int	GetFragmentSize(int nFragmentIndex);
	//��ȡһ���ļ��ֿ�
	unsigned long	ReadFragment(int nFragmentIndex, void*& pBuffer);

	// �رմ򿪵��ļ�
	void	Close();
	// �ӿڶ�������
	void	Release();
//----IFile�ֿں���������----

public:
	// ��һ���ļ���׼����ȡд
	int		Open(const char* FileName, int WriteSupport = false);
	// ����һ���ļ���׼��д�롣
	int		Create(const char*  FileName);
	//��������һ�����󣬰������Լ���ȫ������
	KAloneFile*	Deprive();
	//����һ���յ�KAloneFile����
	static KAloneFile*	New();

private:
	FILE*			m_hFile;				// �ļ�������
	void*			m_pContentBuffer;		// ���ݻ�����
	unsigned long	m_nContentBufferSize;	// ���ݻ�������С
	static KRecycleBin<KAloneFile, 4, RBAFM_NEW_DELETE>	ms_Recycle;
};


#endif //#ifndef _ENGINE_ALONE_FILE_H_

