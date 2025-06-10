//---------------------------------------------------------------------------
//  Engine�ļ�ģ��-Դ�ڴ���ļ����ļ�
//	Copyright : Kingsoft Season 2004
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2004-5-18
//---------------------------------------------------------------------------
#ifndef _ENGINE_PAKFILE_H_
#define _ENGINE_PAKFILE_H_

#include "File.h"
#include "./XPackFile.h"


class KPackFile : public IFile
{
public:
	KPackFile();
	virtual ~KPackFile();

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

	// �жϴ򿪵��Ƿ��ǰ��е��ļ�,���ز���ֵ
	int		IsFileInPak();
	//�ж��ļ��Ƿ�ֿ�ѹ����
	int		IsPackedByFragment();
	//��ȡ�ļ��ֿ����Ŀ
	int		GetFragmentCount();
	//��ȡ�ֿ�Ĵ�С
	unsigned int	GetFragmentSize(int nFragmentIndex);
	//��ȡһ���ļ��ֿ飬�����pBufferΪĿ�껺�������������Ļ�����ָ��Ϊ�գ����ڲ�������µĻ�������������ָ�룬�ⲿ��Ҫ�������١�
	//�������Ĵ�С��Ҫ�������·ֿ�����ݣ�����ͨ��GetFragmentCount֪��ÿ���ֿ�����ݴ�С����������������Ҫ����ô��
	//����ֵ��ʾʵ�ʶ�ȡ����������ֿ�����ݴ�С
	unsigned long	ReadFragment(int nFragmentIndex, void*& pBuffer);
	// �رմ򿪵��ļ�
	void	Close();
	// �ӿڶ�������
	void	Release();
//----IFile�ֿں���������----

public:
	// ��һ���ļ���׼����ȡд,���ز���ֵ
	int		Open(const char* FileName);
	//��������һ�����󣬰������Լ���ȫ������
	KPackFile*	Deprive();
	//����һ���յ�KPackFile����
	static KPackFile*	New();

private:
	XPackFile::XPackElemFileRef	m_Core;
	void*						m_pContentBuffer;		// ���ݻ�����
	static KRecycleBin<KPackFile, 4, RBAFM_NEW_DELETE>	ms_Recycle;
};

#endif //ifndef _ENGINE_PAKFILE_H_
