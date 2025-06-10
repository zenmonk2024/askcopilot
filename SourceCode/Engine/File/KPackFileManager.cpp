/*****************************************************************************************
//	���ļ������ɡ�չ�����޸ġ��ϲ��Ȳ���
//	Copyright : Kingsoft 2005
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2005-3-23
*****************************************************************************************/
#include "Precompile.h"
#include "CRC32.h"
#include "File.h"
#include "SmartPtr.h"
#include "KPackFileManager.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __linux
	#include <sys/time.h>
	#include <sys/types.h>
	#include <utime.h>
#else
	#include <time.h>
	#include <sys/utime.h>
#endif

//---------------------------------------------------------------------------
//	Sprͼ�ļ��ṹ����
//---------------------------------------------------------------------------
#define	SPR_COMMENT_FLAG				0x525053	//'SPR'
struct SPRHEAD	//Spr�ļ�ͷ�ṹ
{
	BYTE	Comment[4];	// ע������(SPR\0)
	WORD	Width;		// ͼƬ���
	WORD	Height;		// ͼƬ�߶�
	WORD	CenterX;	// ���ĵ�ˮƽλ��
	WORD	CenterY;	// ���ĵĴ�ֱλ��
	WORD	Frames;		// ��֡��
	WORD	Colors;		// ��ɫ��
	WORD	Directions;	// ������
	WORD	Interval;	// ÿ֡���������Ϸ֡Ϊ��λ��
	WORD	Reserved[6];// �����ֶΣ����Ժ�ʹ�ã�
};
struct SPROFFS	//Spr�ļ�֡ƫ����Ϣ�ṹ
{
	DWORD	Offset;		// ÿһ֡��ƫ��
	DWORD	Length;		// ÿһ֡�ĳ���
};
//---------------------------------------------------------------------------


C_ENGINE_API
IPackFileShell* CreatePackFileShell()
{
    KPackFileManager* pShell = new KPackFileManager;
    if (!pShell->Initialize())
    {
        pShell->Release();
        pShell = NULL;
    }
    return ((IPackFileShell*)pShell);
}

KPackFileManager::KPackFileManager()
{
    memset(&m_PackItemList, 0, sizeof(m_PackItemList));
    m_ElemFileRootPath[0] = 0;
    m_nElemFileRootPathLen = m_nElemFileRootPathNotEnderLen = 0;
    m_FullFolderFileName[0] = 0;
    m_nFullFolderFileNameLen = 0;
    m_pCallback = NULL;
    m_uPackFileShellOptionSprSplitFrameBalance = IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_DEF;
}

//�������ļ��ĸ�Ŀ¼
void KPackFileManager::SetElemFileRootPath(const char* pPath)
{
    if (pPath && pPath[0])
    {
        m_nElemFileRootPathLen = (int)strlen(pPath);
        if (m_nElemFileRootPathLen < MAX_PATH - 128)
        {          
            memcpy(m_ElemFileRootPath, pPath, m_nElemFileRootPathLen + 1);
            if (m_ElemFileRootPath[m_nElemFileRootPathLen - 1] != '\\' &&
                m_ElemFileRootPath[m_nElemFileRootPathLen - 1] != '/')
            {
                m_ElemFileRootPath[m_nElemFileRootPathLen++] = '\\';
                m_ElemFileRootPath[m_nElemFileRootPathLen] = 0;
            }
            m_nElemFileRootPathNotEnderLen = m_nElemFileRootPathLen - 1;
            return;
        }
    }
    m_ElemFileRootPath[0] = 0;
    m_nElemFileRootPathLen = m_nElemFileRootPathNotEnderLen = 0;
}

int	KPackFileManager::Initialize()
{
    if (!CD_LCU_I())
        return false;
    return true;
}

//���ٽӿڶ���
void KPackFileManager::Release()
{
    for (int i = 0; i < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; i++)
    {
        ClosePack(i);
    }
    delete this;
}

//����/�򿪵�һ������ļ������ش���ļ�����������0ֵ��ʾ����ʧ�ܡ�
int KPackFileManager::CreatePack(const char* pszFile, int bOpenExist, int bExcludeOfCheckId)
{
    int nPakIndex;
    for (nPakIndex = 0; nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; nPakIndex++)
    {
        if (m_PackItemList[nPakIndex].pIOFile == NULL)
            break;
    }
    if (nPakIndex == PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM)
        return -1;

    PACK_ITEM& item = m_PackItemList[nPakIndex];
    bool bOk = false;
	g_GetFullPath(item.PackFileName, pszFile);
    while(true)
    {
        item.pIndexList = (XPackIndexInfo*)malloc(sizeof(XPackIndexInfo) * PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM);
        if (item.pIndexList == NULL)
            break;
        memset(item.pIndexList, 0, sizeof(XPackIndexInfo) * PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM);
        if (bOpenExist)
        {
			if (!g_IsFileExist(pszFile))	//FilePath.cpp:426 --> pszFile = ·�������ļ���
				break;
			item.pIOFile = g_OpenFile(pszFile, true, true);
			if (item.pIOFile == NULL)
				break;
			if (item.pIOFile->Read(&item.Header, sizeof(item.Header)) != sizeof(item.Header) ||
				(*(int*)(&(item.Header.cSignature)) != IPACK_FILE_SIGNATURE_FLAG))
			{
				break;
			}
			item.pIOFile->Seek(item.Header.uIndexTableOffset, SEEK_SET);
			if (item.pIOFile->Read(item.pIndexList, sizeof(XPackIndexInfo) * item.Header.uCount) != sizeof(XPackIndexInfo) * item.Header.uCount)//�����ļ�β���������µ��ļ�����֮���ٲ����
				break;
			item.nDataEndOffset = item.Header.uIndexTableOffset;
			item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);
			LoadPackPartner(nPakIndex);
        }
        else
        {
            item.pIOFile = g_CreateFile(pszFile);
            if (item.pIOFile == NULL)
                break;
            memset(&item.Header, 0, sizeof(item.Header));
            *(int*)(&(item.Header.cSignature)) = IPACK_FILE_SIGNATURE_FLAG;		/* ǰ������Ϊ��Little-Endian */
            if (item.pIOFile->Write(&item.Header, sizeof(item.Header)) != sizeof(item.Header))
                break;
            item.Header.uDataOffset = sizeof(item.Header);
            item.nDataEndOffset = sizeof(item.Header);
            item.bModified = true;
			CreatePackPartner(nPakIndex);
        }
        item.bExcludeOfCheckId = (bExcludeOfCheckId != false);
        bOk = true;
        break;
    }

    if (!bOk)
    {
		SAFE_FREE(item.pIndexList);
		SAFE_RELEASE(item.pIOFile);
		item.PackFileName[0] = 0;
        return -1;
    }

    return nPakIndex;
}

//�رմ���ļ�
void KPackFileManager::ClosePack(int nPakIndex)
{
    if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM)
        return;
    PACK_ITEM& item = m_PackItemList[nPakIndex];
    if (item.pIOFile)
    {
        if (item.bModified && item.pIndexList)
        {
			int nLen = sizeof(XPackIndexInfo) * item.Header.uCount;
			item.Header.uCrc32 = Misc_CRC32(0, item.pIndexList, nLen);
			item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);
            item.pIOFile->Write(item.pIndexList, nLen);
            item.pIOFile->Seek(0, SEEK_SET);
            item.Header.uIndexTableOffset = item.nDataEndOffset;
			time_t	t;
			item.Header.uPakTime = (unsigned int)time(&t);
            item.pIOFile->Write(&item.Header, sizeof(item.Header));
			GetPackPartnerFileName(nPakIndex, m_FullFolderFileName);
			m_PackPartnerList[nPakIndex].Save(m_FullFolderFileName, item.Header.uPakTime, item.Header.uCrc32);
        }
        item.pIOFile->Release();
        item.pIOFile = NULL;
    }
    SAFE_FREE(item.pIndexList);
    memset(&item, 0, sizeof(item));
	m_PackPartnerList[nPakIndex].Clear();
}

//������ļ������һ��Ŀ¼
bool KPackFileManager::AddFolderToPak(int nPakIndex, const char* pFolder, fnAddFolderToPakCallback pCallback)
{
    if (pFolder == NULL || pFolder[0] == 0)
        return false;
    m_nCurrentPakIndex = nPakIndex;
    m_pCallback = pCallback;

	int nFullFolderFileNameLen = (int)strlen(pFolder);
	if (strcmp(pFolder, ".") == 0)
	{	//====ѹ����Ŀ¼��ȫ��Ŀ¼====
		strcpy(m_FullFolderFileName, m_ElemFileRootPath);
		nFullFolderFileNameLen = m_nElemFileRootPathLen;
	}
	else
	{
	    memcpy(m_FullFolderFileName, m_ElemFileRootPath, m_nElemFileRootPathLen);
		memcpy(m_FullFolderFileName + m_nElemFileRootPathLen, pFolder, nFullFolderFileNameLen);
		nFullFolderFileNameLen += m_nElemFileRootPathLen;
	}

    if (m_FullFolderFileName[nFullFolderFileNameLen - 1] != '\\' &&
        m_FullFolderFileName[nFullFolderFileNameLen - 1] != '/')
    {
        m_FullFolderFileName[nFullFolderFileNameLen++] = '\\';
        m_FullFolderFileName[nFullFolderFileNameLen] = 0;
    }

    bool nRet = AddFolderToPak(nFullFolderFileNameLen);
    m_pCallback = NULL;
    return nRet;
}

//������ļ������һ��Ŀ¼
bool KPackFileManager::AddFolderToPak(int nFullFolderFileNameLen)
{
    strcpy(m_FullFolderFileName + nFullFolderFileNameLen, "*.*");
    HANDLE			hFindHandle;	//win32�����ļ����
    WIN32_FIND_DATA	FindData;
    hFindHandle = ::FindFirstFile(m_FullFolderFileName, &FindData);
    if (hFindHandle == INVALID_HANDLE_VALUE)
        return false;

    bool bOk = true;
    do
    {
        if (FindData.cFileName[0] == '.')
            continue;
        int nNewLen = (int)strlen(FindData.cFileName);
        memcpy(m_FullFolderFileName + nFullFolderFileNameLen, FindData.cFileName, nNewLen);
        nNewLen += nFullFolderFileNameLen;
		if (nNewLen - m_nElemFileRootPathNotEnderLen >= 128)
		{
			printf("Error: The length of [%S] is out of limit!\n", m_FullFolderFileName + m_nElemFileRootPathNotEnderLen);
		}
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            m_FullFolderFileName[nNewLen++] = '\\';
            m_FullFolderFileName[nNewLen] = 0;
            if (!AddFolderToPak(nNewLen))
            {
                bOk = false;
                break;
            }
        }
        else
        {
            m_FullFolderFileName[nNewLen] = 0;
            if (m_pCallback && !m_pCallback(m_FullFolderFileName))
                continue;
            if (!AddElemToPak())
            {
                bOk = false;
                break;
            }
        }
    }while(::FindNextFile(hFindHandle, &FindData));

    ::FindClose(hFindHandle);
    hFindHandle = INVALID_HANDLE_VALUE ;
    return bOk;
}

//������ļ������һ�����ļ�
bool KPackFileManager::AddElemToPak(int nPakIndex, const char* pElemFile)
{
    if (pElemFile == NULL || pElemFile[0] == 0)
        return false;
    m_nCurrentPakIndex = nPakIndex;
    memcpy(m_FullFolderFileName, m_ElemFileRootPath, m_nElemFileRootPathLen);
    strcpy(m_FullFolderFileName + m_nElemFileRootPathLen, pElemFile);
    return AddElemToPak();
}

bool KPackFileManager::GenerateElemIndexAndHashId(unsigned int & uElemIndex, unsigned int & uHashId)
{
    PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];

	if (m_PackItemList[m_nCurrentPakIndex].Header.uCount == PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM)
	{
		printf("Error: The elem file count in a single pak has reach %d!\n", PACK_FILE_SHELL_MAX_SUPPORT_ELEM_FILE_NUM);
		return false;
	}

	uHashId = g_FileNameHash(m_FullFolderFileName + m_nElemFileRootPathNotEnderLen);

	if (item.bExcludeOfCheckId == false)
	{	//==����Ƿ��Ѿ�������������ͬ��id==
		for (unsigned int i = 0; i < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; i++)
		{
			if (m_PackItemList[i].pIOFile == NULL)
				continue;
			if (i != m_nCurrentPakIndex && m_PackItemList[i].bExcludeOfCheckId)
				continue;
			unsigned int uIndex;
			if (FindElem(uHashId, i, uIndex))
			{
				printf("Error: %s has the same id %X!\n", m_FullFolderFileName + m_nElemFileRootPathNotEnderLen, uHashId);
				return false;
			}
			if (i == m_nCurrentPakIndex)
				uElemIndex = uIndex;
		}
	}
    return true;
}

bool KPackFileManager::AddBufferToFile(unsigned char * pSrcBuffer, int nSrcSize, unsigned int uCompressType, 
                             unsigned int & uDestSize, unsigned int & uDestCompressType)
{
    PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];

	void*	pWriteBuffer = pSrcBuffer;
	uDestSize = nSrcSize;
	uDestCompressType = XPACK_METHOD_NONE;
	if (uCompressType == XPACK_METHOD_UCL)
    {
        if (CD_LCU_C(pSrcBuffer, nSrcSize, m_CompressBuffer, &uDestSize, 5))
        {
			if (nSrcSize > (int)uDestSize)
			{	// Compress
				if (uDestSize <= COMPRESS_BUFFER_SIZE)
				{
					pWriteBuffer = m_CompressBuffer;
					uDestCompressType = XPACK_METHOD_UCL;
				}
				else
				{
					printf("Warning : compressbuffer overflow!");
				}
			}
        }
		if (pWriteBuffer == pSrcBuffer)
			uDestSize = nSrcSize;
	}

	if (item.pIOFile->Write(pWriteBuffer, uDestSize) == uDestSize)
	{
		return true;
	}

	printf("Error: Cannot write XPackFileFragment\n");
	return false;
}

//������ļ��зֿ鷽ʽ������ļ�
bool KPackFileManager::AddElemToPakFragment(unsigned char* pSrcBuffer, int nNumFragment, int* pFragmentSizeList, unsigned int& uCompressSize)
{
	PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];
	//ʹ��UCLѹ��
	uCompressSize = 0;

	unsigned int nBufferCompressType;
	unsigned int uBufferCompressSize = 0;
	int			 nFragmentInfoListSize = sizeof(XPackFileFragmentInfo) * nNumFragment;
	XPackFileFragmentInfo	FragmentInfoList[100];
	XPackFileFragmentInfo*	pFragmentInfoList = NULL;
	if (nNumFragment <= 100)
		pFragmentInfoList = FragmentInfoList;
	else
		pFragmentInfoList = (XPackFileFragmentInfo*)malloc(nFragmentInfoListSize);
	if (!pFragmentInfoList)
		return false;

	// [XPackFileFragmentElemHeader] + [��һ������] + [�ڶ�������] + .. + [��n������] + [n��XPackFileFragmentInfo������]
	long lItemFileBegin = item.pIOFile->Tell();

	XPackFileFragmentElemHeader fragmentHeader = { 0, 0 };

	// Step 1: [XPackFileFragmentElemHeader] ����λ��
	{
		if (item.pIOFile->Write(&fragmentHeader, sizeof(XPackFileFragmentElemHeader)) 
			!= sizeof(XPackFileFragmentElemHeader))
		{
			printf("Error: Cannot write XPackFileFragmentElemHeader [%s]\n", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
		fragmentHeader.nFragmentInfoOffest += sizeof(XPackFileFragmentElemHeader);
	}

	// Step 2: �洢��������
	int nSrcOffset = 0;
	for (int nFragment = 0; nFragment < nNumFragment; nFragment++)
	{
		if (!AddBufferToFile(pSrcBuffer + nSrcOffset, pFragmentSizeList[nFragment], XPACK_METHOD_UCL, uBufferCompressSize, nBufferCompressType))
		{
			printf("Error: Cannot add fragment of [%s]", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
		nSrcOffset += pFragmentSizeList[nFragment];
		pFragmentInfoList[nFragment].uSize		= pFragmentSizeList[nFragment];
		pFragmentInfoList[nFragment].uOffset	= fragmentHeader.nFragmentInfoOffest;
		pFragmentInfoList[nFragment].uCompressSizeFlag = uBufferCompressSize | nBufferCompressType;
		fragmentHeader.nFragmentInfoOffest += uBufferCompressSize;
		if (fragmentHeader.nFragmentInfoOffest + nFragmentInfoListSize > MAX_SUPPORTABLE_STORE_SIZE)
		{
			printf("Warning: file store size exceed limit [%s].", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
	}

	// Step 3: [nFragment��XPackFileFragmentInfo������]
	{
		if (item.pIOFile->Write(pFragmentInfoList, nFragmentInfoListSize) != nFragmentInfoListSize)
		{
			printf("Error: Cannot write XPackFileFragmentInfo [%s]\n", m_FullFolderFileName);
			goto ERROR_EXIT;
		}
	}

	// Step 4: [XPackFileFragmentElemHeader] ����д��
	item.pIOFile->Seek(lItemFileBegin, SEEK_SET);
	fragmentHeader.nNumFragment = nNumFragment;
	if (item.pIOFile->Write(&fragmentHeader, sizeof(XPackFileFragmentElemHeader)) 
		!= sizeof(XPackFileFragmentElemHeader))
	{
		printf("Error: Cannot write XPackFileFragmentElemHeader [%s]\n", m_FullFolderFileName);
		goto ERROR_EXIT;
	}
	uCompressSize = fragmentHeader.nFragmentInfoOffest + nFragmentInfoListSize;
	item.pIOFile->Seek(lItemFileBegin + uCompressSize, SEEK_SET);

	if (nNumFragment > 100)
		SAFE_FREE(pFragmentInfoList);
	return true;

ERROR_EXIT:
	if (nNumFragment > 100)
		SAFE_FREE(pFragmentInfoList);
	return false;
}

bool KPackFileManager::AddElemToPakCommon(unsigned char* pSrcBuffer, int nSrcSize, unsigned int& uCompressType, unsigned int& uCompressSize)
{
	if (nSrcSize <= COMMON_FILE_SPLIT_SIZE)
		return AddBufferToFile(pSrcBuffer, nSrcSize, uCompressType, uCompressSize, uCompressType);
	int nNumFragment = (nSrcSize + COMMON_FILE_SPLIT_SIZE - 1) / COMMON_FILE_SPLIT_SIZE;
	int	nFragmentSizeList[20];
	int*	pFragmentSizeList = NULL;
	if (nNumFragment <= 20)
		pFragmentSizeList = nFragmentSizeList;
	else
		pFragmentSizeList = (int*)malloc(sizeof(int) * nNumFragment);
	if (!pFragmentSizeList)
		return false;
	for (int i = 0; i < nNumFragment; i++)
		pFragmentSizeList[i] = COMMON_FILE_SPLIT_SIZE;
	if (nSrcSize % COMMON_FILE_SPLIT_SIZE)
		pFragmentSizeList[nNumFragment - 1] = nSrcSize % COMMON_FILE_SPLIT_SIZE;
	uCompressType = XPACK_FLAG_FRAGMENT;
	bool bResult = AddElemToPakFragment(pSrcBuffer, nNumFragment, pFragmentSizeList, uCompressSize);
	if (nNumFragment > 20)
		SAFE_FREE(pFragmentSizeList);
	return bResult;
}

bool KPackFileManager::AddElemToPakFragmentSPR(unsigned char* pSrcBuffer, int nSrcSize, unsigned int& uCompressSize)
{
	// ����SPR�ļ����ֿ鷽ʽΪ��
	//[SPRHEAD + pallette] + [offsettable] + [SPRFRAME0] + .. + [SPRFRAMEn]

    //ʹ��UCLѹ��
	uCompressSize = 0;

	SPRHEAD* head = (SPRHEAD*)pSrcBuffer;
	unsigned int const uSprHeadSize = sizeof(SPRHEAD) + head->Colors * 3;
	unsigned int const uOffsetTableSize = sizeof(SPROFFS) * head->Frames;
	SPROFFS*    const pSprOffsTable     = (SPROFFS *)(pSrcBuffer + uSprHeadSize);

	int nNumFragment = head->Frames + 2;
	int	nFragmentSizeList[100];
	int*	pFragmentSizeList = NULL;
	if (nNumFragment <= 100)
		pFragmentSizeList = nFragmentSizeList;
	else
		pFragmentSizeList = (int*)malloc(sizeof(int) * nNumFragment);
	if (!pFragmentSizeList)
		return false;

	pFragmentSizeList[0] = uSprHeadSize;
	pFragmentSizeList[1] = uOffsetTableSize;
	for (int i = 0; i < head->Frames; i++)
	{
		SPROFFS * pSprOffs = pSprOffsTable + i;
		pFragmentSizeList[i + 2] = pSprOffs->Length;
	}

	bool bResult = AddElemToPakFragment(pSrcBuffer, nNumFragment, pFragmentSizeList, uCompressSize);
	if (nNumFragment > 100)
		SAFE_FREE(pFragmentSizeList);
	return bResult;
}

//������ļ������һ�����ļ�
bool KPackFileManager::AddElemToPak()
{
	unsigned int uElemIndex = 0;
	unsigned int uHashId    = 0;
	unsigned int uCRC = 0;
	if (!GenerateElemIndexAndHashId(uElemIndex, uHashId))
		return false;

	KSmartFile SrcFile;
	SrcFile = g_OpenFile(m_FullFolderFileName, true, false);
	if (!SrcFile)
		return false;

	int	nSrcSize = SrcFile->Size();
	if (nSrcSize == 0)
		return true;

	unsigned int uCompressType = XPACK_METHOD_UCL;
	unsigned int uCompressSize;

	unsigned char * pSrcBuffer = (unsigned char*)SrcFile->GetBuffer();	/* ֱ�Ӷ�ȡ���ݣ�û��ѹ�� */
	if (pSrcBuffer == NULL)
		return false;

	const char* pExt = strrchr(m_FullFolderFileName, '.');
	if (pExt && !stricmp(pExt + 1, "spr"))    // �ж��Ƿ�ΪSPR�ļ�
	{
		if ((unsigned int)nSrcSize >= m_uPackFileShellOptionSprSplitFrameBalance)	//spr������С��ֿ�ѹ��
		{
			SPRHEAD* pSpr = (SPRHEAD*)pSrcBuffer;
			if (*(int*)(&(pSpr->Comment)) == SPR_COMMENT_FLAG && pSpr->Frames > 1)
				uCompressType = XPACK_FLAG_FRAGMENT;
		}
	}

	PACK_ITEM & item = m_PackItemList[m_nCurrentPakIndex];
	item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);

	bool bOk = false;
	if (uCompressType == XPACK_FLAG_FRAGMENT)	//�ֿ���spr
		bOk = AddElemToPakFragmentSPR(pSrcBuffer, nSrcSize, uCompressSize);
	else
		bOk = AddElemToPakCommon(pSrcBuffer, nSrcSize, uCompressType, uCompressSize);

	uCRC = Misc_CRC32(0, pSrcBuffer, nSrcSize);   /// ԭʼ���ݣ���������AddElemToPakXxx����û�ж�pSrcBuffer���в���
	SrcFile.Release();
	if (bOk)
	{
		for (unsigned int i = item.Header.uCount; i > uElemIndex; i--)
			item.pIndexList[i] = item.pIndexList[i - 1];
		item.Header.uCount++;
		item.pIndexList[uElemIndex].uCompressSizeFlag = uCompressSize | uCompressType;
		item.pIndexList[uElemIndex].uSize = nSrcSize;
		item.pIndexList[uElemIndex].uId = uHashId;
		item.pIndexList[uElemIndex].uOffset = item.nDataEndOffset;
		item.nDataEndOffset += uCompressSize;
		item.bModified = true;

		KPackFilePartner::PACKPARTNER_ELEM_INFO	info;
		info.nElemIndex = uElemIndex;
		strcpy(info.szFileName, m_FullFolderFileName + m_nElemFileRootPathNotEnderLen);
		info.uCRC = uCRC;
		info.uId = uHashId;
		info.uSize = nSrcSize;
		info.uStoreSizeAndCompressFlag = uCompressSize | uCompressType;
		time_t	t;
		struct _stat	s;
		if (!_stat(m_FullFolderFileName, &s))
			info.uTime = (unsigned int)s.st_mtime;
		else
			info.uTime = (unsigned int)time(&t);
		bOk = m_PackPartnerList[m_nCurrentPakIndex].AddElem(info);
		assert(bOk);
	}
	return bOk;
}

//�������ļ��ڰ��е�λ��,����ҵ�����true,uIndexΪ�����ҵ���λ�ã�δ�ҵ��򷵻�false,uIndex����Ӧ�������λ��
/**
 * @brief �������ļ��ڰ��е�λ��,����ҵ�����true,uIndexΪ�����ҵ���λ�ã�δ�ҵ��򷵻�false,uIndex����Ӧ�������λ��
 * @param uElemId �ļ���ID��hash֮��õ���
 * @param nPakIndex ��ʹ�õ�pak����
 * @param uIndex Ϊ�����ҵ���λ�ã�����
 * @return δ�ҵ��򷵻�false,uIndex����Ӧ�������λ��
 */
bool KPackFileManager::FindElem(unsigned int uElemId, int nPakIndex, unsigned int& uIndex)
{
    PACK_ITEM&		item = m_PackItemList[nPakIndex];

    int nBegin, nEnd, nMid;
    nBegin = 0;
    nEnd = item.Header.uCount - 1;
    while (nBegin <= nEnd)
    {
        nMid = (nBegin + nEnd) / 2;
        if (uElemId < item.pIndexList[nMid].uId)	/* ǰ�������ǣ�IDֵ�ǰ���С����˳��洢�� */
        {
            nEnd = nMid - 1;
        }
        else if (uElemId > item.pIndexList[nMid].uId)
        {
            nBegin = nMid + 1;
        }
        else
        {
            uIndex = nMid;
            return true;
        }
    }

    if (nBegin == nEnd)
        uIndex = (uElemId < item.pIndexList[nMid].uId) ? nMid : (nMid + 1);
    else
        uIndex = nBegin;
    return false;
}

//Jackie
bool KPackFileManager::FindElementInPak(unsigned int uElemId, int nPakIndex, unsigned int& uIndex)
{
    PACK_ITEM&		item_ip = m_PackItemList[nPakIndex];

    int nBegin_ip, nEnd_ip, nMid_ip;
    nBegin_ip = 0;
    nEnd_ip = item_ip.Header.uCount - 1;
    while (nBegin_ip <= nEnd_ip)
    {
        nMid_ip = (nBegin_ip + nEnd_ip) / 2;
        if (uElemId < item_ip.pIndexList[nMid_ip].uId)	/* ǰ�������ǣ�IDֵ�ǰ���С����˳��洢�� */
        {
            nEnd_ip = nMid_ip - 1;
        }
        else if (uElemId > item_ip.pIndexList[nMid_ip].uId)
        {
            nBegin_ip = nMid_ip + 1;
        }
        else
        {
            uIndex = nMid_ip;			
            return true;
        }
    }

    if (nBegin_ip == nEnd_ip)
	{
        uIndex = (uElemId < item_ip.pIndexList[nMid_ip].uId) ? nMid_ip : (nMid_ip + 1);		
	}
    else
	{
        uIndex = nBegin_ip;
	}	
    return false;
}


//Jackie

/* chua tim ra cach de su dung code, khong the tra ve char * vi trong ipackfileshell no = 0*/
unsigned int KPackFileManager::GetIdOffset(int nPakIndex, unsigned int uElemIndex)
{
	ELEM_FILE_INFO	info;
	info.nPakIndex = nPakIndex;
	PACK_ITEM& item = m_PackItemList[nPakIndex];

	for (info.nElemIndex = 0; info.nElemIndex < (int)item.Header.uCount; info.nElemIndex++)
	{
		XPackIndexInfo& IndexInfo = item.pIndexList[info.nElemIndex];
		info.uId = IndexInfo.uId;		
		sprintf(info.szFileName, "\\_-ID-_%08x", info.uId);		///���û��������ID��Ϊ���֣�		
		//sprintf(uIdOffset, "\\%08x", info.uId);
	}
	return info.uId;
}

//ɾ������ļ��е�һ�����ļ�
int	KPackFileManager::DeleteElemInPak(int nPakIndex, const char* pElemFile)
{
    return false;
}

//���ýӿڲ�������
void KPackFileManager::SetOption(IPACK_FILE_SHELL_OPTION eOption, int nValue)
{
    switch (eOption)
    {
    case IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE:
		if (nValue > 0)
			m_uPackFileShellOptionSprSplitFrameBalance = nValue;
		else
			m_uPackFileShellOptionSprSplitFrameBalance = IPACK_FILE_SHELL_OPTION_SPR_SPLIT_FRAME_BALANCE_DEF;
        break;
	case IPACK_FILE_SHELL_OPTION_RETRENCH:
		break;
    }
}

//�õ��������ļ�����Ŀ
int KPackFileManager::GetElemCountInPak(int nPakIndex)
{
    int nRet = 0;
    if (nPakIndex >= 0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM)
    {
        nRet = m_PackItemList[nPakIndex].Header.uCount;
    }
    return nRet;
}

//��ȡ���ļ���Ϣ
bool KPackFileManager::GetElemInfo(const char* pElemName, ELEM_FILE_INFO& info)
{
	assert(pElemName);
	char	szName[128];
	strcpy(szName, pElemName);
	if (GetElemInfo(g_FileNameHash(szName), info))
	{
		if (!info.szFileName[0])
			strcpy(info.szFileName, szName);
		return true;
	}
	return false;
}

//��ȡ���ļ���Ϣ
bool KPackFileManager::GetElemInfo(unsigned int uElemId, ELEM_FILE_INFO& info)
{
	memset(&info, 0, sizeof(info));
	info.uId = uElemId;
	for (info.nPakIndex = 0; info.nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; info.nPakIndex++)
	{
		unsigned int uElemIndex;
		if (!FindElem(info.uId, info.nPakIndex, uElemIndex))
			continue;
		KPackFilePartner::PACKPARTNER_ELEM_INFO	PartnerInfo;
		info.nElemIndex = uElemIndex;

		XPackIndexInfo&		IndexInfo = (m_PackItemList[info.nPakIndex].pIndexList[uElemIndex]);
		info.uCompressFlag	= (IndexInfo.uCompressSizeFlag & (~XPACK_COMPRESS_SIZE_FILTER));
		info.uStoreSize		= (IndexInfo.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);
		info.uSize			= IndexInfo.uSize;
		if (m_PackPartnerList[info.nPakIndex].GetElemInfo(info.uId, PartnerInfo))
		{
			assert(info.nElemIndex == PartnerInfo.nElemIndex);
			info.uCRC = PartnerInfo.uCRC;
			info.uTime = PartnerInfo.uTime;
			strcpy(info.szFileName, PartnerInfo.szFileName);
		}
		return true;
	}
	info.nPakIndex = -1;
	return false;
}

//�Ӱ��н��ĳ���ļ�
bool KPackFileManager::UnpackElem(int nPakIndex, const char* pElemName, const char* pDestName)
{
	if (pElemName && pElemName[0])
	{
		strcpy(m_FullFolderFileName, pElemName);
		unsigned int uElemId = g_FileNameHash(m_FullFolderFileName);
		return UnpackElemByID(nPakIndex, uElemId, pDestName);
	}
	return false;
}

//���仺�������������ļ��⿪��ȡ���û��������ɹ����ػ�����ָ�룬ʧ�ܷ��ؿ�ָ�룬���ô�Ҫ�����ͷŸô����صĻ�����
/**
 * @brief ���仺�������������ļ��⿪��ȡ���û�����
 * @param nPakIndex ��ǰʹ�õ�pack����
 * @param uElemIndex �ҵ�����Ҫ��ѹ�������ļ�����
 * @param uElemSize ���ظ��ļ��Ĵ�С��ԭʼ��С��
 * @return
 */
unsigned char* KPackFileManager::AllocBufferAndReadElemFile(int nPakIndex, unsigned int uElemIndex, unsigned int& uElemSize)
{
	uElemSize = 0;
	//----�����Ϸ����ж�-----
	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile)
		return NULL;
	PACK_ITEM&		item = m_PackItemList[nPakIndex];
	if (uElemIndex >= item.Header.uCount)
		return NULL;

	XPackIndexInfo& elem = item.pIndexList[uElemIndex];
	unsigned uSize = elem.uSize;	
	unsigned char* pBuffer = (unsigned char*)malloc(uSize);
	if (pBuffer == NULL)
		return NULL;

	bool bOk = true;
	while(true)
	{
		if ((elem.uCompressSizeFlag & XPACK_FLAG_FRAGMENT) == 0)
		{
			bOk = ReadElemBufferFromPak(nPakIndex, elem.uOffset, (elem.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER),
				(elem.uCompressSizeFlag & XPACK_METHOD_FILTER), pBuffer, uSize);
			break;
		}
		XPackFileFragmentElemHeader header;
		if (!ReadElemBufferFromPak(nPakIndex, elem.uOffset, sizeof(header), XPACK_METHOD_NONE, &header, sizeof(header)))
		{
			bOk = false;
			break;
		}
		uSize = 0;
		for (int i = 0; i < header.nNumFragment; i++)
		{
			XPackFileFragmentInfo	fragment;
			if (!ReadElemBufferFromPak(nPakIndex, elem.uOffset + header.nFragmentInfoOffest + sizeof(fragment) * i,
				sizeof(fragment), XPACK_METHOD_NONE, &fragment, sizeof(fragment)))
			{
				bOk = false;
				break;
			}
			if (!ReadElemBufferFromPak(nPakIndex, elem.uOffset + fragment.uOffset, (fragment.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER),
				(fragment.uCompressSizeFlag & XPACK_METHOD_FILTER), pBuffer + uSize, fragment.uSize))
			{
				bOk = false;
				break;
			}
			uSize += fragment.uSize;
		}
		break;
	};
	item.pIOFile->Seek(item.nDataEndOffset, SEEK_SET);	///�ָ��ļ�ָ��λ��?

	if (bOk)
	{
		uElemSize = uSize;
	}
	else
	{
		SAFE_FREE(pBuffer);
	}
	return pBuffer;
}

//�Ӱ��н��ĳ���ļ�
bool KPackFileManager::UnpackElemByID(int nPakIndex, unsigned int uElemId, const char* pDestName)
{
	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile)
		return false;
	unsigned int	uElemIndex;
	if (FindElem(uElemId, nPakIndex, uElemIndex))
		return UnpackElemByIndex(nPakIndex, uElemIndex, pDestName);
	return false;
}

//�Ӱ��н��ĳ���ļ�
bool KPackFileManager::UnpackElemByIndex(int nPakIndex, unsigned int uElemIndex, const char* pDestName)
{
	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile)
		return false;
	KSmartFile		dest;
	dest = g_CreateFile(pDestName);
	if (dest)
	{
		unsigned uElemSize = 0;
		unsigned char* pBuffer = AllocBufferAndReadElemFile(nPakIndex, uElemIndex, uElemSize);
		if (pBuffer)
		{
			dest->Write(pBuffer, uElemSize);
			SAFE_FREE(pBuffer);
			return true;
		}
	}
	return false;
}

//�Ӱ��н��ȫ���ļ�
bool KPackFileManager::UnpackAll(int nPakIndex, int& nUnpackCount, const char* pDestPath, const char* pFileNamePrefix)
{
	nUnpackCount = 0;
	if (pFileNamePrefix == NULL)
		pFileNamePrefix = "";

	if (nPakIndex < 0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || !m_PackItemList[nPakIndex].pIOFile ||
		pDestPath == NULL || pDestPath[0] == 0)
	{
		return false;
	}
	int		nFileNameStartOffset = strlen(pDestPath);
	if (nFileNameStartOffset + 128 > MAX_PATH)
		return false;

	g_CreatePath(pDestPath);
	char	szFullName[MAX_PATH] = "";
	char*	pElemName = NULL;
	strcpy(szFullName, pDestPath);
	if (szFullName[nFileNameStartOffset - 1] == '\\')		/// �ļ������Ǵ���'\\'�ģ�eg��\image\effect\sfx\�ؿ�\��������.3e
	{
		pElemName = szFullName + nFileNameStartOffset - 1;
	}
	else
	{
		pElemName = szFullName + nFileNameStartOffset;
	}

	ELEM_FILE_INFO	info;
	info.nPakIndex = nPakIndex;
	PACK_ITEM& item = m_PackItemList[nPakIndex];

	for (info.nElemIndex = 0; info.nElemIndex < (int)item.Header.uCount; info.nElemIndex++)
	{
		XPackIndexInfo& IndexInfo = item.pIndexList[info.nElemIndex];
		info.uId = IndexInfo.uId;
	
		KPackFilePartner::PACKPARTNER_ELEM_INFO	PartnerInfo;
		if (m_PackPartnerList[info.nPakIndex].GetElemInfo(info.uId, PartnerInfo))
		{
			if (pFileNamePrefix[0])
			{
				if (strstr(PartnerInfo.szFileName, pFileNamePrefix) != PartnerInfo.szFileName)
					continue;	//�ų�ǰ׺�����ϵ��ļ�
			}
			strcpy(info.szFileName, PartnerInfo.szFileName);
			info.uCRC = PartnerInfo.uCRC;
			info.uTime = PartnerInfo.uTime;
		}
		else
		{
			if (pFileNamePrefix[0])
				continue;
			sprintf(info.szFileName, "\\_-ID-_%08x", info.uId);		///���û��������ID��Ϊ���֣�
			info.uCRC = 0;
			info.uTime = 0;
		}
//		info.uCompressFlag = (IndexInfo.uCompressSizeFlag & (~XPACK_COMPRESS_SIZE_FILTER));
//		info.uSize = IndexInfo.uSize;
//		info.uStoreSize = (IndexInfo.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);

		strcpy(pElemName, info.szFileName);				/// ·�� + �ļ���
		char* pLastSplit = strrchr(pElemName, '\\');	/// �����ַ���ָ���ַ����дӺ��濪ʼ�ĵ�һ�γ��ֵ�λ��  <---> strchr
		if (pLastSplit && pLastSplit != pElemName)		/// ���ļ��еĴ������ļ��������Ǵ���Ŀ¼����ʽ��eg: \image\dc\B2_M7_1.Mtl
		{
			*pLastSplit = 0;
			g_CreatePath(szFullName);			
			*pLastSplit = '\\';
		}
		//----�⿪����----
		UnpackElemByIndex(nPakIndex, info.nElemIndex, szFullName);

		//----�޸��ļ�ʱ��----
		if (info.uTime)
		{
			_utimbuf time;
			time.actime = time.modtime = info.uTime;
			_utime(szFullName, &time);
		}

		nUnpackCount++;
	}
	return true;
}

//�Ӱ��ж�ȡһ������
/**
 * @breif �Ӱ��ж�ȡһ������
 * @param nPakIndex ����ʹ�õ�Pak����
 * @param uOffset ƫ��������Ҫ��ȡ���ݵĿ�ʼλ�ã����item.pIOFile��ʼλ��
 * @param uStorSize �洢�Ĵ�С�����ѹ���ˣ�����ѹ����Ĵ�С
 * @param uPakMethod pack��ʽ��none��UCL��Filter
 * @param pBuffer ��ȡ���ݵĴ洢�ռ�
 * @param uSize
 * @return 
 */
bool KPackFileManager::ReadElemBufferFromPak(int nPakIndex, unsigned int uOffset, unsigned int uStoreSize,
				unsigned int uPakMethod, void* pBuffer, unsigned int uSize)
{
	assert(nPakIndex >= 0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM && m_PackItemList[nPakIndex].pIOFile);
	PACK_ITEM&		item = m_PackItemList[nPakIndex];
	item.pIOFile->Seek(uOffset, SEEK_SET);

	if (uPakMethod == XPACK_METHOD_NONE)
	{
		assert(uStoreSize == uSize);
		if (item.pIOFile->Read(pBuffer, uSize) == uSize)
			return true;
	}
	else if (uStoreSize <= COMPRESS_BUFFER_SIZE && uPakMethod == XPACK_METHOD_UCL)
	{
		if (item.pIOFile->Read(m_CompressBuffer, uStoreSize) == uStoreSize &&
			CD_LCU_D(m_CompressBuffer, uStoreSize, (unsigned char*)pBuffer, uSize))
		{
			return true;
		}
	}
	return false;
}

//��ô����Ϣ�ļ����ļ���
bool KPackFileManager::GetPackPartnerFileName(int nPakIndex, char* pFileName)
{
	if (nPakIndex >=  0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM && m_PackItemList[nPakIndex].PackFileName[0])
	{
		assert(pFileName);
		sprintf(pFileName, "%s.txt", m_PackItemList[nPakIndex].PackFileName);
		return true;
	}
	return false;
}

//ɨ���������ļ����ϣ�����ÿ�����ļ���������һ�λص���������
//����pFileNamePrefix��ʾ�����������ļ�����ǰ׺������˸������ַ�����ȫһ�£������ִ�Сд����ǰ׺���������ļ������Թ��������ָ���ʾ��ǰ׺���ơ�
bool	KPackFileManager::ScanAllPack(fnScanPackCallback pCallback, void* pCallbackParam, int& nCount, const char* pFileNamePrefix)
{
	nCount = 0;
	if (pFileNamePrefix == NULL)
		pFileNamePrefix = "";
	ELEM_FILE_INFO	info;
	for (info.nPakIndex = 0; info.nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM; info.nPakIndex++)
	{
		PACK_ITEM& item = m_PackItemList[info.nPakIndex];
		for (info.nElemIndex = 0; info.nElemIndex < (int)item.Header.uCount; info.nElemIndex++)
		{
			XPackIndexInfo& IndexInfo = item.pIndexList[info.nElemIndex];
			info.uId = IndexInfo.uId;
			//�ų��Ѿ����ֹ�ͬID���ļ�
			{
				int nPrePak = 0;
				for (nPrePak = 0; nPrePak < info.nPakIndex; nPrePak++)
				{
					unsigned int uPreElemIndex;                                        
					if (FindElem(info.uId, nPrePak, uPreElemIndex))
						break;
				}
				if (nPrePak < info.nPakIndex)
					continue;
			}

			KPackFilePartner::PACKPARTNER_ELEM_INFO	PartnerInfo;
			if (m_PackPartnerList[info.nPakIndex].GetElemInfo(info.uId, PartnerInfo))
			{
				if (pFileNamePrefix[0])
				{
					if (strstr(PartnerInfo.szFileName, pFileNamePrefix) != PartnerInfo.szFileName)
						continue;	//�ų�ǰ׺�����ϵ��ļ�
				}
				strcpy(info.szFileName, PartnerInfo.szFileName);
				info.uCRC = PartnerInfo.uCRC;
				info.uTime = PartnerInfo.uTime;
			}
			else
			{///��һ����Ϊ��ʲô��û����
				if (pFileNamePrefix[0])
					continue;
				info.szFileName[0] = 0;
				info.uCRC = 0;
				info.uTime = 0;
			}
			info.uCompressFlag = (IndexInfo.uCompressSizeFlag & (~XPACK_COMPRESS_SIZE_FILTER));
			info.uSize = IndexInfo.uSize;
			info.uStoreSize = (IndexInfo.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);
			nCount++;
			if (pCallback)
			{
				if (!pCallback(info, pCallbackParam))	//����ֵΪ0����ֹɨ��
				{
					return false;
				}
			}
		}
	}
	return true;
}

//��ð���ĳ�����ļ��Ĵ洢����
unsigned int KPackFileManager::GetElemStoreDataInPak(int nPakIndex, int nElemIndex, void* pBuffer, unsigned int uBufferSize)
{
	if (pBuffer == NULL || nPakIndex <  0 || nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM || nElemIndex < 0)
		return 0;
	PACK_ITEM& item = m_PackItemList[nPakIndex];
	if (nElemIndex >= (int)item.Header.uCount || uBufferSize < (item.pIndexList[nElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER))
		return 0;
	unsigned int uStoreSize = (item.pIndexList[nElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER);
	if (ReadElemBufferFromPak(nPakIndex, item.pIndexList[nElemIndex].uOffset, uStoreSize, XPACK_METHOD_NONE, pBuffer, uStoreSize))
		return uStoreSize;
	return 0;
}

//������ļ������һ�����ļ����Ѿ�ѹ���õģ�
//�������ElemInfo::nPakIndex��ʾҪ�����ĸ�Pak�ļ�
//�������ElemInfo::nElemIndex�����屻����
bool KPackFileManager::AddElemToPak(ELEM_FILE_INFO& ElemInfo, void* pBuffer)
{
	if (pBuffer == NULL || ElemInfo.nPakIndex < 0 || ElemInfo.nPakIndex >= PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM ||
		ElemInfo.uId == 0 || ElemInfo.uSize == 0 || ElemInfo.uStoreSize == 0)
	{
		return false;
	}

	PACK_ITEM& item = m_PackItemList[ElemInfo.nPakIndex];
	unsigned int uElemIndex;
	bool bExist = FindElem(ElemInfo.uId, ElemInfo.nPakIndex, uElemIndex);

	//���ԭ������ͬid�ļ���ԭ�洢��С���ڵ����´洢��С���������ݴ���ԭ���ļ��Ĵ洢λ�á�
	//ԭ�洢��СС���Ĵ洢��С���������ݴ��ڴ���ļ�ĩβ��
	long	lOffset = item.nDataEndOffset;
	bool	bAppend = true;
	if (bExist && (item.pIndexList[uElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER) >= ElemInfo.uStoreSize)
	{
		lOffset = item.pIndexList[uElemIndex].uOffset;
		bAppend = false;
	}

	unsigned int uType;
	item.pIOFile->Seek(lOffset, SEEK_SET);
	m_nCurrentPakIndex = ElemInfo.nPakIndex;
	if (AddBufferToFile((unsigned char*)pBuffer, ElemInfo.uStoreSize, XPACK_METHOD_NONE, ElemInfo.uStoreSize, uType))
	{
		KPackFilePartner::PACKPARTNER_ELEM_INFO	info;
		if (!bExist)
		{
			for (unsigned int i = item.Header.uCount; i > uElemIndex; i--)
				item.pIndexList[i] = item.pIndexList[i - 1];
			item.Header.uCount++;
		}
		else
		{
			m_PackPartnerList[m_nCurrentPakIndex].GetElemInfo(ElemInfo.uId, info);
			if (stricmp(ElemInfo.szFileName, info.szFileName))
			{
				m_PackPartnerList[m_nCurrentPakIndex].GetElemInfo(ElemInfo.uId, info);
				printf("WARNING : Elem file [%s]of ID [%x] has been replace with [%s].\n",
					ElemInfo.szFileName, ElemInfo.uId, info.szFileName);
			}
		}

		item.pIndexList[uElemIndex].uCompressSizeFlag = ElemInfo.uStoreSize | ElemInfo.uCompressFlag;
		item.pIndexList[uElemIndex].uSize = ElemInfo.uSize;
		item.pIndexList[uElemIndex].uId = ElemInfo.uId;
		if (bAppend)
		{
			item.pIndexList[uElemIndex].uOffset = item.nDataEndOffset;
			item.nDataEndOffset += ElemInfo.uStoreSize;
		}
		item.bModified = true;

		info.nElemIndex = uElemIndex;
		strcpy(info.szFileName, ElemInfo.szFileName);
		info.uCRC = ElemInfo.uCRC;
		info.uId = ElemInfo.uId;
		info.uSize = ElemInfo.uSize;
		info.uStoreSizeAndCompressFlag = ElemInfo.uStoreSize | ElemInfo.uCompressFlag;
		info.uTime = ElemInfo.uTime;
		m_PackPartnerList[m_nCurrentPakIndex].AddElem(info);
	}
	return true;
}

//��������Ϣ
bool KPackFileManager::LoadPackPartner(int nPakIndex)
{
	char	szFileName[MAX_PATH];
	if (GetPackPartnerFileName(nPakIndex, szFileName))
	{
		PACK_ITEM& item = m_PackItemList[nPakIndex];	// ����
		m_PackPartnerList[nPakIndex].Load(szFileName);	// m_PackPartnerList
		if (m_PackPartnerList[nPakIndex].IsPartnerMatch(
			item.Header.uCount,	item.Header.uPakTime, item.Header.uCrc32))
		{
			return true;
		}
		printf("ERROR : elem list is not match to the pak file [%s] .\n", item.PackFileName);
	}
	return false;
}

//�½������Ϣ�ļ�
bool KPackFileManager::CreatePackPartner(int nPakIndex)
{
	assert(nPakIndex >= 0 && nPakIndex < PACK_FILE_SHELL_MAX_SUPPORT_PAK_NUM);
	return m_PackPartnerList[nPakIndex].Init();
}
