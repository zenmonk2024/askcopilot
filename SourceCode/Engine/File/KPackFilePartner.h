/*****************************************************************************************
//	���ļ��İ����ļ�
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-3-23
------------------------------------------------------------------------------------------
�����¼����ļ��и����ļ����ļ���֮��������Ϣ
*****************************************************************************************/
#ifndef _ENGINE_PACK_FILE_PARTNER_H_
#define _ENGINE_PACK_FILE_PARTNER_H_

#include "IPackFileShell.h"

class KPackFilePartner
{
public:
	struct PACKPARTNER_ELEM_INFO
	{
		int				nElemIndex;					//�ڰ��еĵڼ������ļ�
		unsigned int	uId;						//���ļ�ID
		unsigned int	uSize;						//���ļ�ԭʼ��С
		unsigned int	uStoreSizeAndCompressFlag;	//���ļ��ڰ��еĴ洢��С�����ļ���ѹ�����,��XPackIndexInfo::uCompressSizeFlag��ͬ
		unsigned int	uTime;						//���ļ���ʱ��
		unsigned int	uCRC;						//У���
		char			szFileName[128];			//���ļ����ļ������������Ϸ��Ŀ¼��
	};
public:
	KPackFilePartner();
	~KPackFilePartner();
	//��ʼ��
	bool	Init();
	//�������
	void	Clear();
	//���ش���ļ���Ϣ
	bool	Load(const char* pFileName);
	//�������ļ���Ϣ
	bool	Save(const char* pFileName, unsigned int uPackTime, unsigned int uPackCRC);
	//�������Ϣ�ļ��Ƿ�ƥ���ض���Ϣ
	bool	IsPartnerMatch(int nElemCount, unsigned int uPackTime, unsigned int uCRC);
	//������ļ������һ�����ļ�
	bool	AddElem(PACKPARTNER_ELEM_INFO& ElemInfo);
	//ɾ������ļ��е�һ�����ļ�
	bool	DeleteElemInPak(unsigned int uElemID);
	//��ȡ���ļ���Ϣ
	bool	GetElemInfo(unsigned int uElemId, PACKPARTNER_ELEM_INFO& info);
	//ɨ���������ļ����ϣ�����ÿ�����ļ���������һ�λص���������
	//����pFileNamePrefix��ʾ�����������ļ�����ǰ׺������˸������ַ�����ȫһ�£������ִ�Сд����ǰ׺���������ļ������Թ��������ָ���ʾ��ǰ׺���ơ�
	//����ֵ��ʾ���ٸ��������������ļ���ɨ�赽
	//int		ScanAllPack(IPackFileShell::fnScanPackCallback pCallback, const char* pFileNamePrefix = NULL);

private:
	//�������ļ���
    bool    FindElem(unsigned int uElemId, unsigned int& uIndex);
private:
	PACKPARTNER_ELEM_INFO*		m_pElemInfoList;

	int							m_nElemCount;		//���ļ���Ŀ
	unsigned int				m_uPackTime;		//����ļ�����ʱ��ʱ�䣬��Ϊ��λtime()
	unsigned int				m_uCRC;				//У���

	IPackFileShell::fnAddFolderToPakCallback	m_pScanCallback;
};

#endif //_ENGINE_PACK_FILE_PARTNER_H_