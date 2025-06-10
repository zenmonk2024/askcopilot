/*****************************************************************************************
//	���ļ���ȡ
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-9-16
------------------------------------------------------------------------------------------
	XPackFile֧�ֶ��̵߳�ͬʱ���ʡ�
	XPackFile�ⲿͨ��XPackElemFileRef���������õĶ԰������ļ������á�
	�ⲿ����������Ķ��������ṹ�ڵ����ݣ��Ի�������İ����ʹ��ܡ�
*****************************************************************************************/
#ifndef _ENGINE_XPACKFILE_H_
#define _ENGINE_XPACKFILE_H_
#include "Mutex.h"
#include "PackDef.h"
#include "./AloneFile.h"

class XPackFile
{
public:
	//--------------------------------------------------
	//ʹ��XPackFile�����ļ����з��ʲ������õ��ĸ����ṹ
	//--------------------------------------------------
	typedef struct 
	{
		unsigned int	NameId;

		//----��Щ���ݶԴ���ļ��е�С�ļ���Ч----
		short			CacheIndex;		//��������
		short			PakFileIndex;	//�����Ĵ���ļ�������
		int				ElemFileIndex;	//�ڴ���ļ��е�����(�Ǵ���ļ��ĵڼ���С�ļ�)
		long			Offset;			//��ǰ���ļ�����ƫ��λ��
		unsigned long	Size;			//���ļ���С
		//----endof: ��Щ���ݶԴ���ļ��е�С�ļ���Ч----
	} XPackElemFileRef;

public:
	XPackFile();
	~XPackFile();
	//��ʼ�����ļ�ģ��,���ز���ֵ
	static int	Initialize();
	//�رհ��ļ�ģ��
	static void	Terminate();
	//�򿪰��ļ�,���ز���ֵ
	int			Open(const char* pszPackFileName, int nSelfIndex);
	//�رհ��ļ�
	void		Close();
	//���Ұ��ڵ����ļ�,���ز���ֵ
	int			FindElemFile(unsigned int uId, XPackElemFileRef& ElemRef);
	//��ȡ���ڵ����ļ�
	unsigned long	ElemFileRead(XPackElemFileRef& ElemRef, void* pBuffer, unsigned long ReadBytes);
	//��ȡ�ֿ��ļ����ڵ����ݣ�CompressSize = 0��ʾ����ѹֱ�Ӷ�ȡ
	unsigned long	XElemReadFragment(int nElemIndex, int nFragmentIndex, void*& Buffer);
	//�ж��ļ��Ƿ�ֿ�ѹ����
	int				XElemIsPackedByFragment(int nElemIndex);
	//��ȡ�ļ��ֿ����Ŀ
	int				XElemGetFragmentCount(int nElemIndex);
	//��ȡĳ�����ļ�ĳ���ֿ�Ĵ�С
	unsigned int	ElemGetFragmentSize(int nElemIndex, int nFragmentIndex);
	//��ð����������ļ��������ݵĻ�����
	void*			GetElemFileBuffer(XPackElemFileRef& ElemRef);

private:

	//ֱ�Ӷ�ȡ���ļ������е����ݵ�������,���ز���ֵ
	int			DirectRead(void* pBuffer, unsigned int uOffset, unsigned int uSize);
	//����ѹ�ض�ȡ���ļ���������,���ز���ֵ
	int			ExtractRead(void* pBuffer, unsigned int uExtractSize,
						long lCompressType, unsigned int uOffset, unsigned int uSize);
	//���������в������ļ���
	int			FindElemFile(unsigned long ulId) const;

	//��cache��������ļ�
	int			FindElemFileInCache(unsigned int uId, int nDesireIndex);
	//�����ļ�������ӵ�cache
	int			AddElemFileToCache(void* pBuffer, int nElemIndex);
	//����һ��������������ָ�������ļ����ݶ�������
	void*		ReadElemFile(int nElemIndex);
	//�ͷ�һ��cache��������
	static void	FreeElemCache(int nCacheIndex);
	//���طֿ����ļ���ȫ������,�ɹ����ط�0ֵ��ʧ�ܷ���0ֵ
	int			EnsureElemFlieContent(int nCacheIndex, int nStartOffset, int nEndOffset);

private:
	KAloneFile				m_File;				//���ļ�
	unsigned long			m_uFileSize;		//���ļ���С
	unsigned long			m_uElemFileCount;	//���ļ��ĸ���
	int						m_nSelfIndex;		//���ļ��Լ��ڰ������е�����
	struct XPackIndexInfo*	m_pIndexList;		//���ļ������б�

	//----���ļ�����cache----
	struct XPackElemFileCache
	{
		void*			pBuffer;			//�������ļ����ݵĻ�����
		unsigned long	uId;				//���ļ�id
		unsigned long	lSize;				//���ļ���С
		int				nPackIndex;			//�����ĸ����ļ�
		int				nElemIndex;			//���ļ��������б��е�λ��
		unsigned int	uRefFlag;			//�������ñ��
		int				nNumFragment;		//�ֿ���Ŀ��Ϊ0��ʾ���ֿ�
		XPackFileFragmentInfo*	pFragmentInfo;	//�ֿ���Ϣ
	};

	static int			ms_HasInitialize;	//�Ƿ��Ѿ���ʼ������
	static MUTEX_DECLARE(ms_ReadCritical);	//�������ļ�ʱ���ٽ�������

	#define	MAX_XPACKFILE_CACHE			10
	//���ļ���cache����
	static	XPackElemFileCache	ms_ElemFileCache[MAX_XPACKFILE_CACHE];
	//���ļ���cache����Ŀ
	static	int					ms_nNumElemFileCache;
};


#endif //#ifndef _ENGINE_XPACKFILE_H_
