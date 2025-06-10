/*****************************************************************************************
//	���ļ������ɡ�չ�����޸ġ��ϲ��Ȳ���
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-3-23
------------------------------------------------------------------------------------------
   �Դ���ļ����в������࣬�������������ɡ�չ�����޸ġ��ϲ�...�ȵ�
*****************************************************************************************/
#ifndef _ENGINE_PACK_FILE_MANAGER_H_
#define _ENGINE_PACK_FILE_MANAGER_H_

#include "PackDef.h"
#include "KPackFilePartner.h"

class KPackFileManager : public IPackFileShell
{
public:
	KPackFileManager();
	//���ٽӿڶ���
	void	Release();
	//�������ļ��ĸ�Ŀ¼
	void	SetElemFileRootPath(const char* pPath);
	//����/�򿪵�һ������ļ������ش���ļ�����������0ֵ��ʾ����ʧ�ܡ�
	int		CreatePack(const char* pszFile, int bOpenExist, int bExcludeOfCheckId);
	//�رմ���ļ�
	void	ClosePack(int nPakIndex);
	//������ļ������һ��Ŀ¼
	bool	AddFolderToPak(int nPakIndex, const char* pFolder, fnAddFolderToPakCallback pCallback);
	//������ļ������һ�����ļ�
	bool	AddElemToPak(int nPakIndex, const char* pElemFile);
	//ɾ������ļ��е�һ�����ļ�
	int		DeleteElemInPak(int nPakIndex, const char* pElemFile);
	//��ȡ���ļ���Ϣ
	bool	GetElemInfo(const char* pElemName, ELEM_FILE_INFO& info);
	//��ȡ���ļ���Ϣ
	bool	GetElemInfo(unsigned int uElemId, ELEM_FILE_INFO& info);
	//�Ӱ��н��ĳ���ļ�
	bool	UnpackElem(int nPakIndex, const char* pElemName, const char* pDestName);
	//�Ӱ��н��ĳ���ļ�
	bool	UnpackElemByID(int nPakIndex, unsigned int uElemId, const char* pDestName);
	//�Ӱ��н��ĳ���ļ�
	bool	UnpackElemByIndex(int nPakIndex, unsigned int uElemIndex, const char* pDestName);
	//�Ӱ��н��ȫ���ļ�
	//����nUnpackCount���ڴ���������˶��ٸ��ļ�
	//����pFileNamePrefix��ʾ���ļ�����ǰ׺������˸������ַ�����ȫһ�£������ִ�Сд���������ָ���ʾ��ǰ׺����
	bool	UnpackAll(int nPakIndex, int& nUnpackCount, const char* pDestPath, const char* pFileNamePrefix = NULL);
	//�õ��������ļ�����Ŀ
	int		GetElemCountInPak(int nPakIndex);
	//���ýӿڲ�������
	void	SetOption(IPACK_FILE_SHELL_OPTION eOption, int nValue);
	//ɨ���������ļ����ϣ�����ÿ�����ļ���������һ�λص���������
	//����pFileNamePrefix��ʾ�����������ļ�����ǰ׺������˸������ַ�����ȫһ�£������ִ�Сд����ǰ׺���������ļ������Թ��������ָ���ʾ��ǰ׺���ơ�
	//����ֵ��ʾ���ٸ��������������ļ���ɨ�赽
	bool	ScanAllPack(fnScanPackCallback pCallback, void* pCallbackParam, int& nCount, const char* pFileNamePrefix = NULL);
	//��ð���ĳ�����ļ��Ĵ洢����
	unsigned int	GetElemStoreDataInPak(int nPakIndex, int nElemIndex, void* pBuffer, unsigned int uBufferSize);
	//������ļ������һ�����ļ����Ѿ�ѹ���õģ�
	//�������ElemInfo::nPakIndex��ʾҪ�����ĸ�Pak�ļ�
	//�������ElemInfo::nElemIndex�����屻����
	bool	AddElemToPak(ELEM_FILE_INFO& ElemInfo, void* pBuffer);
	bool	FindElementInPak(unsigned int uElemId, int nPakIndex, unsigned int& uIndex);
	unsigned int GetIdOffset(int nPakIndex, unsigned int uElemIndex);
	//��ʼ��
	int		Initialize();
private:
	//������ļ������һ��Ŀ¼
	bool	AddFolderToPak(int nFullFolderFileNameLen);
	//������ļ������һ�����ļ�
	bool	AddElemToPak();
	//�������ļ��ڰ��е�λ��,����ҵ�����true,uIndexΪ�����ҵ���λ�ã�δ�ҵ��򷵻�false,uIndex����Ӧ�������λ��
	bool	FindElem(unsigned int uElemId, int nPakIndex, unsigned int& uIndex);

    //�������ڴ���õ�ElemIndex�Լ�HashId
    bool	GenerateElemIndexAndHashId(unsigned int & uElemIndex, unsigned int & uHashId);

	//������ļ��зֿ鷽ʽ������ļ�
	bool	AddElemToPakFragment(unsigned char* pSrcBuffer, int nNumFragment, int* pFragmentSizeList, unsigned int& uCompressSize);
	//������ļ������һ���������ļ�
    bool	AddElemToPakCommon(unsigned char* pSrcBuffer, int nSrcSize, unsigned int& uCompressType, unsigned int& uCompressSize);
    //������ļ��зֿ鷽ʽ���һ��SPR���ļ�
    bool	AddElemToPakFragmentSPR(unsigned char* pSrcBuffer, int nSrcSize,unsigned int& uCompressSize);

    //��һ�黺�������ļ���
    bool	AddBufferToFile(unsigned char * pSrcBuffer, int nSrcSize, unsigned int uCompressType, 
        unsigned int & uDestSize, unsigned int & uDestCompressType);
	//�Ӱ��ж�ȡһ������
	bool	ReadElemBufferFromPak(int nPakIndex, unsigned int uOffset, unsigned int uStoreSize,
				unsigned int uPakMethod, void* pBuffer, unsigned int uSize);
	//��ô����Ϣ�ļ����ļ���
	bool	GetPackPartnerFileName(int nPakIndex, char* pFileName);
	//��������Ϣ
	bool	LoadPackPartner(int nPakIndex);
	//�½������Ϣ�ļ�
	bool	CreatePackPartner(int nPakIndex);
	//���仺�������������ļ��⿪��ȡ���û��������ɹ����ػ�����ָ�룬ʧ�ܷ��ؿ�ָ�룬���ô�Ҫ�����ͷŸô����صĻ�����
	unsigned char* AllocBufferAndReadElemFile(int nPakIndex, unsigned int uElemIndex, unsigned int& uElemSize);

private:
	struct PACK_ITEM
	{
		IFile*			pIOFile;			//��ȡд��������ļ�����
		XPackFileHeader	Header;				//����ļ���Ϣ
		XPackIndexInfo*	pIndexList;			//���ļ������б�����
		int				nDataEndOffset;	    //��ǰ����ļ����ݽ���λ�õ�ƫ��λ�ã�������ļ�ͷ��
		bool			bModified;			//�Ѿ����޸�
		bool			bExcludeOfCheckId;	//�Ƿ��ų��ڼ����ͬid֮��
		char			PackFileName[MAX_PATH];	//���ļ���
	}		m_PackItemList[PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM];
	KPackFilePartner	m_PackPartnerList[PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM];		//����ļ�����

	char	m_ElemFileRootPath[MAX_PATH];	//���ļ��ĸ�Ŀ¼
	int		m_nElemFileRootPathLen;			//���ļ��ĸ�Ŀ¼�ַ�������
	int		m_nElemFileRootPathNotEnderLen;	//���ļ��ĸ�Ŀ¼�ַ�������(����������Ǹ�'\')

	//--�����ⲿ�������ù����е��ڲ�����--
	int		m_nCurrentPakIndex;
	char	m_FullFolderFileName[MAX_PATH + MAX_PATH];	//���Ǹ���ʱ�����������ڷ��õ�ǰ�������ļ�/Ŀ¼�����ƣ���ʱ�������ļ�����ڸ�Ŀ¼��������ơ�
	int		m_nFullFolderFileNameLen;

	#define MAX_SUPPORTABLE_STORE_SIZE	XPACK_COMPRESS_SIZE_FILTER
 	#define COMPRESS_BUFFER_SIZE		4194304		//4M
	#define COMMON_FILE_SPLIT_SIZE		2097152		//2M
    unsigned char	m_CompressBuffer[COMPRESS_BUFFER_SIZE];
	fnAddFolderToPakCallback	m_pCallback;

    unsigned int  m_uPackFileShellOptionSprSplitFrameBalance; //spr�ļ���С��֡ѹ����ֵ�����ڴ�ֵ��ѹ����С�ڴ�ֵ�����ļ�д�룬
                                                              //spr�ļ�����m_bPackFileShellOptionSizeLimit����
};

#endif //_ENGINE_PACK_FILE_MANAGER_H_

