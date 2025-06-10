//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
//
// File:	KPackList.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis),Wuyue(Wooy)
// Desc:	Header File
// Modify:	Wooy(2003-9-17)
//---------------------------------------------------------------------------
#ifndef _ENGINE_XPAKLIST_H_
#define _ENGINE_XPAKLIST_H_

#include "XPackFile.h"
//---------------------------------------------------------------------------

class KPackList
{
public:
	KPackList();
	~KPackList();
	//���ش���ļ������ز���ֵ
	int			Open(const char* pPakListFile, const char* pSection);
	void		Close();                                       
	//���Ұ��ڵ����ļ�
	int			FindElemFile(unsigned int uId, XPackFile::XPackElemFileRef& ElemRef);
	//���Ұ��ڵ����ļ�
	int			FindElemFile(const char* pszFileName, XPackFile::XPackElemFileRef& ElemRef);
	//��ȡ���ڵ����ļ�
	unsigned long	ElemFileRead(XPackFile::XPackElemFileRef& ElemRef, void* pBuffer, unsigned long uSize);

	//��ȡ�ֿ��ļ����ڵ����ݣ�CompressSize = 0��ʾ����ѹֱ�Ӷ�ȡ
	unsigned long	ElemReadFragment(XPackFile::XPackElemFileRef& ElemRef, int nFragmentIndex, void*& Buffer);
	//�ж��ļ��Ƿ�ֿ�ѹ����
	int				ElemIsPackedByFragment(XPackFile::XPackElemFileRef& ElemRef);
	//��ȡ�ļ��ֿ����Ŀ
	int				ElemGetFragmentCount(XPackFile::XPackElemFileRef& ElemRef);
	//��ȡĳ�����ļ�ĳ���ֿ�Ĵ�С
	unsigned int	ElemGetFragmentSize(XPackFile::XPackElemFileRef& ElemRef, int nFragmentIndex);
	//��ð����������ļ��������ݵĻ�����
	void*			GetElemFileBuffer(XPackFile::XPackElemFileRef& ElemRef);

private:
	enum	PACK_LIST_PARAM
	{
		PAK_LIST_MAX_PAK = 32,
	};
	XPackFile*			m_PakFilePtrList[PAK_LIST_MAX_PAK];
	long				m_PakNumber;

};

extern KPackList g_EnginePackList;

#endif //ifndef _ENGINE_XPAKLIST_H_
