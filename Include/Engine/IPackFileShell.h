/*****************************************************************************************
//	���ļ������ɡ�չ�����޸ġ��ϲ��Ȳ����Ľӿ�
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-3-23
------------------------------------------------------------------------------------------
*****************************************************************************************/

#ifndef   __IPACKILESHELL_H_HEADER__
#define   __IPACKILESHELL_H_HEADER__

struct IPackFileShell
{
public:
	enum IPACK_FILE_SHELL_PARAM
	{
		PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM = 24,			//���ͬʱ֧�ֵĴ���ļ�����Ŀ
		PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM = 200000,	//һ������ļ��������԰������ļ��ĸ���
		IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_MIN = 100,
		IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_MAX = 1000,
		IPACK_FILE_SIGNATURE_FLAG = 0x4b434150,				//'PACK'������ļ�����
		IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_DEF = 131072,		//128K��Ĭ��Ϊspr�ļ�����128K���֡ѹ��
								//ͨ��SetOption(IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE..)�������趨
		
	};
	enum IPACK_FILE_SHELL_OPTION
	{
		IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE = 0,
		IPACK_FILE_SHELL_OPTION_RETRENCH = 1,					//��������ļ�,ȥ������Ҫ�Ŀռ䡣�ڴ���ļ�֮��ɾ�����ļ����ϲ��Ȳ���֮��
																//nValue == 0 ��ʾ������������ nValue == ��0 ��ʾҪ����������
	};
	typedef int		(*fnAddFolderToPakCallback)(const char* pFileName);
	struct ELEM_FILE_INFO
	{
		int				nPakIndex;		//���ĸ�����
		int				nElemIndex;		//�ڰ��еĵڼ������ļ�
		unsigned int	uId;			//���ļ�ID
		unsigned int	uSize;			//���ļ�ԭʼ��С
		unsigned int	uStoreSize;		//���ļ��ڰ��еĴ洢��С
		unsigned int	uCompressFlag;	//���ļ���ѹ����� = (XPackIndexInfo::uCompressSizeFlag & 0xff000000)
		unsigned int	uTime;			//���ļ���ʱ��
		unsigned int	uCRC;			//У���
		char			szFileName[128];//���ļ����ļ������������Ϸ��Ŀ¼��
	};
	//ɨ����ļ��Ļص����������������ʾɨ�赽�����ļ�
	//����ֵ���Ϊ��0�������ɨ�裬����ֵΪ0����ֹɨ�衣
	//�ο�IPackFileShell::ScanAllPack
	typedef int		(*fnScanPackCallback)(ELEM_FILE_INFO& ElemInfo, void* pParam);

public:
	//���ٽӿڶ���
	virtual void	Release() = 0;
	//�������ļ��ĸ�Ŀ¼
	virtual void	SetElemFileRootPath(const char* pPath) = 0;
	//����/�򿪵�һ������ļ������ش���ļ����������ظ�ֵ��ʾ����ʧ�ܡ�
	virtual int		CreatePack(const char* pszFile, int bOpenExist, int bExcludeOfCheckId) = 0;
	//�رմ���ļ�
	virtual void	ClosePack(int nPakIndex) = 0;
	//������ļ������һ��Ŀ¼
	virtual bool	AddFolderToPak(int nPakIndex, const char* pFolder, fnAddFolderToPakCallback pCallback) = 0;
	//������ļ������һ�����ļ�
	virtual bool	AddElemToPak(int nPakIndex, const char* pElemFile) = 0;
	//ɾ������ļ��е�һ�����ļ�
	virtual int		DeleteElemInPak(int nPakIndex, const char* pElemFile) = 0;
	//��ȡ���ļ���Ϣ
	virtual bool	GetElemInfo(const char* pElemName, ELEM_FILE_INFO& info) = 0;
	//��ȡ���ļ���Ϣ
	virtual bool	GetElemInfo(unsigned int uElemId, ELEM_FILE_INFO& info) = 0;
	//�Ӱ��н��ĳ���ļ�
	virtual bool	UnpackElem(int nPakIndex, const char* pElemName, const char* pDestName) = 0;
	//�Ӱ��н��ĳ���ļ�
	virtual bool	UnpackElemByID(int nPakIndex, unsigned int uElemId, const char* pDestName) = 0;
	//�Ӱ��н��ĳ���ļ�
	virtual bool	UnpackElemByIndex(int nPakIndex, unsigned int uElemIndex, const char* pDestName) = 0;
	//�Ӱ��н��ȫ���ļ�
	//����nUnpackCount���ڴ���������˶��ٸ��ļ�
	//����pFileNamePrefix��ʾ���ļ�����ǰ׺������˸������ַ�����ȫһ�£������ִ�Сд���������ָ���ʾ��ǰ׺����
	virtual bool	UnpackAll(int nPakIndex, int& nUnpackCount, const char* pDestPath, const char* pFileNamePrefix = 0) = 0;
	//�õ��������ļ�����Ŀ
	virtual int		GetElemCountInPak(int nPakIndex) = 0;
	//���ýӿڲ�������
	virtual void	SetOption(IPACK_FILE_SHELL_OPTION eOption, int nValue) = 0;
	//ɨ���������ļ����ϣ�����ÿ�����ļ���������һ�λص���������
	//����pParam Ϊ�����ص�����pCallback�Ĳ���
	//����nCount ���ڴ�����ɨ�赽���ģ�ƥ��Ҫ�����ļ�����Ŀ
	//����pFileNamePrefix��ʾ�����������ļ�����ǰ׺������˸������ַ�����ȫһ�£������ִ�Сд����ǰ׺���������ļ������Թ��������ָ���ʾ��ǰ׺���ơ�
	//����ֵ��ʾɨ������Ƿ�ִ�����δ�����
	virtual bool	ScanAllPack(fnScanPackCallback pCallback, void* pCallbackParam, int& nCount, const char* pFileNamePrefix = NULL) = 0;
	//��ð���ĳ�����ļ��Ĵ洢����
	virtual unsigned int	GetElemStoreDataInPak(int nPakIndex, int nElemIndex, void* pBuffer, unsigned int uBufferSize) = 0;
	//������ļ������һ�����ļ����Ѿ�ѹ���õģ�
	//�������ElemInfo::nPakIndex��ʾҪ�����ĸ�Pak�ļ�
	//�������ElemInfo::nElemIndex�����屻����
	virtual bool	AddElemToPak(ELEM_FILE_INFO& ElemInfo, void* pBuffer) = 0;
};

//------ģ��Ľӿں�����ԭ�͵Ķ���------
typedef IPackFileShell*					(*fnCreatePackFileShell)();

//------�ӿں����ĺ������ַ���----------
#define	FN_CREATE_PACKFILE_SHELL			"CreatePackFileShell"


C_ENGINE_API IPackFileShell* CreatePackFileShell();

#endif //__IPACKILESHELL_H_HEADER__