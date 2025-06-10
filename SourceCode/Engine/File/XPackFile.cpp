#pragma warning (disable: 4018)
/*****************************************************************************************
//	��ȡ����ļ�
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-9-16
*****************************************************************************************/
#include "Precompile.h"
#include "XPackFile.h"
#include "../Ucl/ucl.h"

XPackFile::XPackElemFileCache	XPackFile::ms_ElemFileCache[MAX_XPACKFILE_CACHE];
int								XPackFile::ms_nNumElemFileCache = 0;
int								XPackFile::ms_HasInitialize = false;
MUTEX_TYPEDEF					XPackFile::ms_ReadCritical;

int	XPackFile::Initialize()
{
	if (ms_HasInitialize == false)
	{
		MUTEX_INIT(ms_ReadCritical);
		ms_HasInitialize = true;
	}
	return ms_HasInitialize;
}

void	XPackFile::Terminate()
{
	if (ms_HasInitialize)
	{
		MUTEX_LOCK(ms_ReadCritical);
		for (int i = 0; i < ms_nNumElemFileCache; i++)
			FreeElemCache(i);
		ms_nNumElemFileCache = 0;
		MUTEX_UNLOCK(ms_ReadCritical);
		MUTEX_DELETE(ms_ReadCritical);
		ms_HasInitialize = false;
	}
}

XPackFile::XPackFile()
{
	m_uFileSize = 0;
	m_pIndexList = NULL;
	m_uElemFileCount = 0;
	m_nSelfIndex = -1;
}

XPackFile::~XPackFile()
{
	Close();
}

//-------------------------------------------------
//���ܣ��򿪰��ļ�
//���أ��ɹ����
//-------------------------------------------------
int XPackFile::Open(const char* pszPackFileName, int nSelfIndex)
{
	if (ms_HasInitialize == false)
		return false;

	bool bResult = false;
	Close();
	MUTEX_LOCK(ms_ReadCritical);
	m_nSelfIndex = nSelfIndex;
	while (m_File.Open(pszPackFileName, false))
	{
		m_uFileSize = m_File.Size();
		if (m_uFileSize <= sizeof(XPackFileHeader))
			break;

		XPackFileHeader	Header;
		unsigned long uListSize;
		//--��ȡ���ļ�ͷ--
		if (m_File.Read(&Header, sizeof(Header)) != sizeof(Header))
			break;
		//--���ļ���������ݵĺϷ����ж�--
		if (*(int*)(&Header.cSignature) != XPACKFILE_SIGNATURE_FLAG ||
			Header.uCount == 0 ||
			Header.uIndexTableOffset < sizeof(XPackFileHeader) ||
			Header.uIndexTableOffset >= m_uFileSize ||
			Header.uDataOffset < sizeof(XPackFileHeader) ||
			Header.uDataOffset >= m_uFileSize)
		{
			break;
		}

		//--��ȡ������Ϣ��--
		uListSize = sizeof(XPackIndexInfo) * Header.uCount;
		m_pIndexList = (XPackIndexInfo*)malloc(uListSize);
		if (m_pIndexList == NULL ||
			m_File.Seek(Header.uIndexTableOffset, SEEK_SET) != (long)Header.uIndexTableOffset)
		{
			break;
		}
		if (m_File.Read(m_pIndexList, uListSize) != uListSize)
			break;
		m_uElemFileCount = Header.uCount;
		bResult = true;
		break;
	};
	MUTEX_UNLOCK(ms_ReadCritical);
	if (bResult == false)
		Close();
	return bResult;
}

//-------------------------------------------------
//���ܣ��رհ��ļ�
//-------------------------------------------------
void XPackFile::Close()
{
	if (ms_HasInitialize == false)
		return;

	MUTEX_LOCK(ms_ReadCritical);

	if (m_pIndexList)
	{
		//----���cache�л��浽�ģ����ܣ��Ǵ˰��е����ļ�----
		for (int i = ms_nNumElemFileCache - 1; i >=0; i--)
		{
			if (ms_ElemFileCache[i].nPackIndex == m_nSelfIndex)
			{
				FreeElemCache(i);
				ms_nNumElemFileCache --;
				if (i < ms_nNumElemFileCache)
					ms_ElemFileCache[i] = ms_ElemFileCache[ms_nNumElemFileCache];
			}
		}
		free (m_pIndexList);
		m_pIndexList = NULL;
	}
	m_uElemFileCount = 0;
	m_File.Close();
	m_uFileSize = 0;
	m_nSelfIndex = -1;
	MUTEX_UNLOCK(ms_ReadCritical);
}

//-------------------------------------------------
//���ܣ��ͷ�һ��cache��������
//���أ��ɹ����
//-------------------------------------------------
void XPackFile::FreeElemCache(int nCacheIndex)
{
	assert(nCacheIndex >= 0 && nCacheIndex < ms_nNumElemFileCache);
	SAFE_FREE(ms_ElemFileCache[nCacheIndex].pBuffer);
	SAFE_FREE(ms_ElemFileCache[nCacheIndex].pFragmentInfo);
	ms_ElemFileCache[nCacheIndex].uId = 0;
	ms_ElemFileCache[nCacheIndex].lSize = 0;
	ms_ElemFileCache[nCacheIndex].uRefFlag = 0;
	ms_ElemFileCache[nCacheIndex].nPackIndex = -1;
	ms_ElemFileCache[nCacheIndex].nNumFragment = 0;
}

//-------------------------------------------------
//���ܣ�ֱ�Ӷ�ȡ���ļ������е����ݵ�������
//���أ��ɹ����
//-------------------------------------------------
int	XPackFile::DirectRead(void* pBuffer, unsigned int uOffset, unsigned int uSize)
{
	int bResult = false;
	assert(pBuffer);
	if (uOffset + uSize <= m_uFileSize &&
		m_File.Seek(uOffset, SEEK_SET) == (long)uOffset)
	{
		bResult =  (m_File.Read(pBuffer, uSize) == uSize);
	}
	return bResult;
}

//-------------------------------------------------
//���ܣ�����ѹ�ض�ȡ���ļ���������
//������pBuffer --> ������ָ��
//		uExtractSize  --> ���ݣ���������ѹ��Ĵ�С��pBuffer�������Ĵ�С��С�ڴ���
//		lCompressType --> ֱ�ӴӰ��ж�ȡ��ԭʼ��/ѹ������С
//		uOffset  --> �Ӱ��еĴ�ƫ��λ�ÿ�ʼ��ȡ
//		uSize    --> �Ӱ���ֱ�Ӷ�ȡ�ã�ѹ�������ݵĴ�С
//���أ��ɹ����
//-------------------------------------------------
int	XPackFile::ExtractRead(void* pBuffer, unsigned int uExtractSize,
						long lCompressType, unsigned int uOffset, unsigned int uSize)
{
	assert(pBuffer);
	int bResult = false;
	if (lCompressType == XPACK_METHOD_NONE)
	{
		if (uExtractSize == uSize)
			bResult = DirectRead(pBuffer, uOffset, uSize);
	}
	else
	{
		void*	pReadBuffer = malloc(uSize);
		if (pReadBuffer)
		{
		    //if (lCompressType == XPACK_METHOD_UCL && DirectRead(pReadBuffer, uOffset, uSize))
			if ((lCompressType == XPACK_METHOD_UCL || lCompressType == 0x20000000) && DirectRead(pReadBuffer, uOffset, uSize))
			{
				unsigned int uDestLength;// = uExtractSize;
				//ucl_nrv2b_decompress_8((unsigned char*)pReadBuffer, uSize, (unsigned char*)pBuffer, &uDestLength, NULL);
				ucl_nrv2b_decompress_8((BYTE*)pReadBuffer, uSize, (BYTE*)pBuffer, &uDestLength, NULL);
				bResult =  (uDestLength == uExtractSize);
			}			
			free (pReadBuffer);
		}
	}
	return bResult;
}


//---------------------------------------------------------------------------
// ����:	�ֿ��ȡ�ļ�
// ����:	nElemIndex		�ڰ��ж�λҪ��ȡ�ļ�������
//			Buffer			������ָ��
//			ReadBytes		Ҫ��ȡ�ĳ���
//			Offset			����ȥ��ֵ������ڿ��ļ��е�ƫ������֮��Ҫ���Ͽ��ļ�����ڰ��ļ�ͷ��ƫ���� 
//			CompressSize	�ļ�ѹ���Ĵ�С��0��ʾû��ѹ������ȡ��ʽΪֱ�Ӷ�ȡ�������ı�ʾΪ��ѹ��ȡ
// ����:	�������ֽڳ���
//---------------------------------------------------------------------------
unsigned long XPackFile::XElemReadFragment(int nElemIndex, int nFragmentIndex, void*& Buffer)
{
	MUTEX_LOCK(ms_ReadCritical);
	XPackFileFragmentElemHeader	header;
	if (!(m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_FLAG_FRAGMENT) ||		//���Ƿֿ�ѹ���ģ�������XElemReadFragment����ȡ
		!DirectRead(&header, m_pIndexList[nElemIndex].uOffset, sizeof(header)) ||	//��ȡͷ�ṹ
		nFragmentIndex >= header.nNumFragment)	
	{
		MUTEX_UNLOCK(ms_ReadCritical);
		return 0;
	}

	//��ȡָ�����ݿ�ķֿ���Ϣ
	XPackFileFragmentInfo	fragment;
	unsigned int			uOffset = m_pIndexList[nElemIndex].uOffset + header.nFragmentInfoOffest + sizeof(XPackFileFragmentInfo) * nFragmentIndex;
	if (!DirectRead(&fragment,  uOffset, sizeof(XPackFileFragmentInfo)))
	{
		MUTEX_UNLOCK(ms_ReadCritical);
		return 0;
	}
	uOffset = m_pIndexList[nElemIndex].uOffset + fragment.uOffset;

	//�������bufferΪ�գ������buffer
	if (Buffer == NULL)
	{
		Buffer = malloc(fragment.uSize);
		if (Buffer == NULL)
		{
			MUTEX_UNLOCK(ms_ReadCritical);
			return 0;
		}
	}

	int bOk;
	if ((fragment.uCompressSizeFlag & XPACK_METHOD_FILTER) != XPACK_METHOD_NONE)
	{
		bOk = ExtractRead(Buffer, fragment.uSize, (fragment.uCompressSizeFlag & XPACK_METHOD_FILTER),
					uOffset, (fragment.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER));
	}
	else
	{
		bOk = DirectRead(Buffer, uOffset, fragment.uSize);
	}

	MUTEX_UNLOCK(ms_ReadCritical);
	return (bOk ? fragment.uSize : 0);
}

//---------------------------------------------------------------------------
// ����:	�ж��Ƿ�ֿ�ѹ���ļ�

// ����:	��0ֵΪ�ֿ�ѹ���ļ�
//---------------------------------------------------------------------------
int	XPackFile::XElemIsPackedByFragment(int nElemIndex)
{
	return ((m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_FLAG_FRAGMENT) != 0);
}

//��ȡ�ļ��ֿ����Ŀ
int	XPackFile::XElemGetFragmentCount(int nElemIndex)
{
	if ((m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_FLAG_FRAGMENT) != 0)
	{
		XPackFileFragmentElemHeader	header;
		if (DirectRead(&header, m_pIndexList[nElemIndex].uOffset, sizeof(header)))
		{
			return header.nNumFragment;
		}
	}
	return 0;
}

//��ȡĳ�����ļ�ĳ���ֿ�Ĵ�С
unsigned int XPackFile::ElemGetFragmentSize(int nElemIndex, int nFragmentIndex)
{
	if ((m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_FLAG_FRAGMENT) != 0)
	{
		MUTEX_LOCK(ms_ReadCritical);
		XPackFileFragmentElemHeader	header;
		XPackFileFragmentInfo		fragment;
		if (DirectRead(&header, m_pIndexList[nElemIndex].uOffset, sizeof(header)) && nFragmentIndex < header.nNumFragment)
		{
			unsigned int uOffset = m_pIndexList[nElemIndex].uOffset + header.nFragmentInfoOffest + sizeof(XPackFileFragmentInfo) * nFragmentIndex;
			//��ȡָ�����ݿ�ķֿ���Ϣ
			if (DirectRead(&fragment,  uOffset, sizeof(XPackFileFragmentInfo)))
			{
				MUTEX_UNLOCK(ms_ReadCritical);
				return fragment.uSize;
			}
		}
		MUTEX_UNLOCK(ms_ReadCritical);
	}
	return 0;
}

//-------------------------------------------------
//���ܣ����������в������ļ���(���ַ���)
//���أ����ҵ��������������е�λ��(>=0)����δ�ҵ�����-1
//-------------------------------------------------
int XPackFile::FindElemFile(unsigned long ulId) const
{
	int nBegin, nEnd, nMid;
	nBegin = 0;
	nEnd = m_uElemFileCount - 1;
	while (nBegin <= nEnd)
	{
		nMid = (nBegin + nEnd) / 2;
		if (ulId < m_pIndexList[nMid].uId)
			nEnd = nMid - 1;
		else if (ulId > m_pIndexList[nMid].uId)
			nBegin = nMid + 1;
		else
			break;
	}
	return ((nBegin <= nEnd) ? nMid : -1);
}

//-------------------------------------------------
//���ܣ����Ұ��ڵ����ļ�
//������uId --> ���ļ���id
//		ElemRef -->����ҵ����ڴ˽ṹ���������ļ��������Ϣ
//���أ��Ƿ��ҵ�
//-------------------------------------------------
int XPackFile::FindElemFile(unsigned int uId, XPackElemFileRef& ElemRef)
{
	int nFound = false;
	if (uId)
	{
		MUTEX_LOCK(ms_ReadCritical);
		ElemRef.CacheIndex = FindElemFileInCache(uId, -1);
		if (ElemRef.CacheIndex >= 0)
		{
			ElemRef.NameId = uId;
			ElemRef.PakFileIndex = ms_ElemFileCache[ElemRef.CacheIndex].nPackIndex;
			ElemRef.ElemFileIndex = ms_ElemFileCache[ElemRef.CacheIndex].nElemIndex;
			ElemRef.Size = ms_ElemFileCache[ElemRef.CacheIndex].lSize;
			ElemRef.Offset = 0;
			nFound = true;
		}
		else
		{
			int nIndex = FindElemFile(uId);
			if (nIndex >= 0)
			{
				nFound = true;
				ElemRef.ElemFileIndex = nIndex;
				ElemRef.NameId = uId;
				ElemRef.PakFileIndex = m_nSelfIndex;
				ElemRef.Size = m_pIndexList[ElemRef.ElemFileIndex].uSize;
				ElemRef.Offset = 0;
			}
		}
		MUTEX_UNLOCK(ms_ReadCritical);
	}
	return nFound;
}

//-------------------------------------------------
//���ܣ����仺�������������ڵ����ļ������ݵ����У����ڷֿ�洢�ļ������̶�ȡ�ļ�����
//���������ļ��ڰ��ڵ�����
//���أ��ɹ��򷵻ػ�������ָ�룬���򷵻ؿ�ָ��
//-------------------------------------------------
void* XPackFile::ReadElemFile(int nElemIndex)
{
	assert(nElemIndex >= 0 && (unsigned long)nElemIndex < m_uElemFileCount);
	XPackIndexInfo&	info = m_pIndexList[nElemIndex];
	void*	pDataBuffer = malloc(info.uSize);
	if (pDataBuffer)
	{
		if ((info.uCompressSizeFlag >> 24 & XPACK_FLAG_FRAGMENT) == 0)
		{
			if (ExtractRead(pDataBuffer,
				m_pIndexList[nElemIndex].uSize,
				(m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_METHOD_FILTER),
				m_pIndexList[nElemIndex].uOffset,
				(m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER)) == false)
			{
				free (pDataBuffer);
				pDataBuffer = NULL;
			}
		}
	}
	return pDataBuffer;
}

//-------------------------------------------------
//���ܣ���cache��������ļ�
//������uId --> ���ļ�id
//		nDesireIndex --> ��cache�еĿ���λ��
//���أ��ɹ��򷵻�cache�ڵ�����(>=0),ʧ���򷵻�-1
//-------------------------------------------------
int XPackFile::FindElemFileInCache(unsigned int uId, int nDesireIndex)
{
	if (nDesireIndex >= 0 && nDesireIndex < ms_nNumElemFileCache &&
		uId == ms_ElemFileCache[nDesireIndex].uId)
	{
		ms_ElemFileCache[nDesireIndex].uRefFlag = 0xffffffff;
		return nDesireIndex;
	}

	nDesireIndex = -1;
	for (int i = 0; i < ms_nNumElemFileCache; i++)
	{
		if (uId == ms_ElemFileCache[i].uId)
		{
			ms_ElemFileCache[i].uRefFlag = 0xffffffff;
			nDesireIndex = i;
			break;
		}
	}
	return nDesireIndex;
}

//-------------------------------------------------
//���ܣ������ļ�������ӵ�cache
//������pBuffer --> �������ļ����ݵĻ�����
//		nElemIndex --> ���ļ��ڰ��е�����
//���أ���ӵ�cache������λ��
//-------------------------------------------------
int XPackFile::AddElemFileToCache(void* pBuffer, int nElemIndex)
{
	assert(pBuffer && nElemIndex >= 0 && (unsigned long)nElemIndex < m_uElemFileCount);
	int nCacheIndex;
	if (ms_nNumElemFileCache < MAX_XPACKFILE_CACHE)
	{	//�ҵ�һ����λ��
		nCacheIndex = ms_nNumElemFileCache++;
	}
	else
	{	//�ͷ�һ���ɵ�cache�ڵ�
		nCacheIndex = 0;
		if (ms_ElemFileCache[0].uRefFlag)
			ms_ElemFileCache[0].uRefFlag --;
		for (int i = 1; i < MAX_XPACKFILE_CACHE; i++)
		{
			if (ms_ElemFileCache[i].uRefFlag)
				ms_ElemFileCache[i].uRefFlag --;
			if (ms_ElemFileCache[i].uRefFlag < ms_ElemFileCache[nCacheIndex].uRefFlag)
				nCacheIndex = i;

		}
		FreeElemCache(nCacheIndex);
	}

	ms_ElemFileCache[nCacheIndex].pBuffer = pBuffer;
	ms_ElemFileCache[nCacheIndex].uId = m_pIndexList[nElemIndex].uId;
	ms_ElemFileCache[nCacheIndex].lSize = m_pIndexList[nElemIndex].uSize;
	ms_ElemFileCache[nCacheIndex].nPackIndex = m_nSelfIndex;
	ms_ElemFileCache[nCacheIndex].nElemIndex = nElemIndex;
	ms_ElemFileCache[nCacheIndex].uRefFlag = 0xffffffff;
	ms_ElemFileCache[nCacheIndex].nNumFragment = 0;
	ms_ElemFileCache[nCacheIndex].pFragmentInfo = NULL;
	if ((m_pIndexList[nElemIndex].uCompressSizeFlag & XPACK_FLAG_FRAGMENT) == 0)
	{
		return nCacheIndex;
	}

	XPackFileFragmentElemHeader	header;
	if (DirectRead(&header, m_pIndexList[nElemIndex].uOffset, sizeof(header)))
	{
		int nInfoSize = sizeof(XPackFileFragmentInfo) * header.nNumFragment;
		ms_ElemFileCache[nCacheIndex].pFragmentInfo = (XPackFileFragmentInfo*)malloc(nInfoSize);
		if (ms_ElemFileCache[nCacheIndex].pFragmentInfo)
		{
			//��ȡָ�����ݿ�ķֿ���Ϣ
			if (DirectRead(ms_ElemFileCache[nCacheIndex].pFragmentInfo,  m_pIndexList[nElemIndex].uOffset + header.nFragmentInfoOffest, nInfoSize))
			{
				ms_ElemFileCache[nCacheIndex].nNumFragment = header.nNumFragment;
				return nCacheIndex;
			}
		}
	}

	ms_nNumElemFileCache --;
	if (nCacheIndex < ms_nNumElemFileCache)
		ms_ElemFileCache[nCacheIndex] = ms_ElemFileCache[ms_nNumElemFileCache];
	return -1;
}

//-------------------------------------------------
//���ܣ���ȡ���ļ�һ�����ȵ����ݵ�������
//������pBuffer --> ������ȡ���ݵĻ�����
//		uSize --> Ҫ��ȡ�����ݵĳ���
//���أ��ɹ���ȡ���ֽ���
//-------------------------------------------------
unsigned long XPackFile::ElemFileRead(XPackElemFileRef& ElemRef, void* pBuffer, unsigned long ReadBytes)
{
	unsigned int nResult = 0;
	if (pBuffer && ElemRef.NameId)
	{
		MUTEX_LOCK(ms_ReadCritical);

		//--�ȿ��Ƿ��Ѿ���cache����---
		ElemRef.CacheIndex = FindElemFileInCache(ElemRef.NameId, ElemRef.CacheIndex);

		if (ElemRef.CacheIndex < 0 &&								//��cache��δ�ҵ�
			(unsigned int)ElemRef.ElemFileIndex < m_uElemFileCount &&
			m_pIndexList[ElemRef.ElemFileIndex].uId == ElemRef.NameId)
		{
			void*	pDataBuffer = ReadElemFile(ElemRef.ElemFileIndex);
			if (pDataBuffer)
				ElemRef.CacheIndex = AddElemFileToCache(pDataBuffer, ElemRef.ElemFileIndex);
		}

		if (ElemRef.CacheIndex >= 0 &&
			//����������Ӧ��չ����飬��ֹ��ģ���ⲿ�ı䣬�������
			//ΪЧ�ʿɿ���ʡ�ԣ������ⲿ���չ��򲻿����ı�ElemRef�����ݡ�
			ElemRef.PakFileIndex == ms_ElemFileCache[ElemRef.CacheIndex].nPackIndex &&
			ElemRef.ElemFileIndex == ms_ElemFileCache[ElemRef.CacheIndex].nElemIndex &&
			ElemRef.Size == ms_ElemFileCache[ElemRef.CacheIndex].lSize
			)
		{
			if (ElemRef.Offset < 0)
				ElemRef.Offset = 0;
			if ((unsigned long)ElemRef.Offset < ElemRef.Size)
			{
				if ((unsigned long)(ElemRef.Offset + ReadBytes) <= ElemRef.Size)
					nResult = ReadBytes;
				else
					nResult = ElemRef.Size - ElemRef.Offset;

				if (ms_ElemFileCache[ElemRef.CacheIndex].nNumFragment == 0)
				{
					memcpy(pBuffer, (char*)ms_ElemFileCache[ElemRef.CacheIndex].pBuffer + ElemRef.Offset, nResult);
					ElemRef.Offset += nResult;
				}
				else if (EnsureElemFlieContent(ElemRef.CacheIndex, ElemRef.Offset, ElemRef.Offset + nResult))
				{
					memcpy(pBuffer, (char*)ms_ElemFileCache[ElemRef.CacheIndex].pBuffer + ElemRef.Offset, nResult);
					ElemRef.Offset += nResult;
				}
				else
				{
					nResult = 0;
				}
			}
			else
			{
				ElemRef.Offset = ElemRef.Size;
			}
		}
		MUTEX_UNLOCK(ms_ReadCritical);
	}
	return nResult;
}

//��ð����������ļ��������ݵĻ�����
void* XPackFile::GetElemFileBuffer(XPackElemFileRef& ElemRef)
{
	void* pBuffer = NULL;
	if (ElemRef.NameId)
	{
		MUTEX_LOCK(ms_ReadCritical);

		//--�ȿ��Ƿ��Ѿ���cache����---
		ElemRef.CacheIndex = FindElemFileInCache(ElemRef.NameId, ElemRef.CacheIndex);

		if (ElemRef.CacheIndex < 0 &&								//��cache��δ�ҵ�
			(unsigned int)ElemRef.ElemFileIndex < m_uElemFileCount &&
			m_pIndexList[ElemRef.ElemFileIndex].uId == ElemRef.NameId)
		{
			void*	pDataBuffer = ReadElemFile(ElemRef.ElemFileIndex);
			if (pDataBuffer)
				ElemRef.CacheIndex = AddElemFileToCache(pDataBuffer, ElemRef.ElemFileIndex);
		}

		if (ElemRef.CacheIndex >= 0 &&
			//����������Ӧ��չ����飬��ֹ��ģ���ⲿ�ı䣬�������
			//ΪЧ�ʿɿ���ʡ�ԣ������ⲿ���չ������ı�ElemRef�����ݡ�
			ElemRef.PakFileIndex == ms_ElemFileCache[ElemRef.CacheIndex].nPackIndex &&
			ElemRef.ElemFileIndex == ms_ElemFileCache[ElemRef.CacheIndex].nElemIndex &&
			ElemRef.Size == ms_ElemFileCache[ElemRef.CacheIndex].lSize
			)
		{
			if (ms_ElemFileCache[ElemRef.CacheIndex].nNumFragment == 0)
			{
				pBuffer = ms_ElemFileCache[ElemRef.CacheIndex].pBuffer;
				ms_ElemFileCache[ElemRef.CacheIndex].pBuffer = NULL;
			}
			else if (EnsureElemFlieContent(ElemRef.CacheIndex, 0, ElemRef.Size))
			{
				pBuffer = ms_ElemFileCache[ElemRef.CacheIndex].pBuffer;
				ms_ElemFileCache[ElemRef.CacheIndex].pBuffer = NULL;
			}
			if (ms_ElemFileCache[ElemRef.CacheIndex].pBuffer == NULL)
			{
				FreeElemCache(ElemRef.CacheIndex);
				ms_nNumElemFileCache --;
				if (ElemRef.CacheIndex < ms_nNumElemFileCache)
					ms_ElemFileCache[ElemRef.CacheIndex] = ms_ElemFileCache[ms_nNumElemFileCache];
			}
		}
		MUTEX_UNLOCK(ms_ReadCritical);
	}
	return pBuffer;
}

//���طֿ����ļ���ȫ������,�ɹ����ط�0ֵ��ʧ�ܷ���0ֵ
int	XPackFile::EnsureElemFlieContent(int nCacheIndex, int nStartOffset, int nEndOffset)
{
	assert(nCacheIndex >= 0 && nCacheIndex < MAX_XPACKFILE_CACHE);
	assert(nStartOffset <= nEndOffset);
	XPackElemFileCache&	cache = ms_ElemFileCache[nCacheIndex];
	assert(cache.pBuffer && cache.nNumFragment && cache.pFragmentInfo);
	int	nSize = 0;
	int nResult = true;
	bool bAllFragmentLoaded = true;
	for (int nFragment = 0; nFragment < cache.nNumFragment; nFragment++)
	{
		XPackFileFragmentInfo& frag = cache.pFragmentInfo[nFragment];
		if (frag.uCompressSizeFlag)
		{
			if (nEndOffset > nSize && nStartOffset < (int)(nSize + frag.uSize))
			{
				if (!ExtractRead(((char*)cache.pBuffer) + nSize, frag.uSize,
					(frag.uCompressSizeFlag & XPACK_METHOD_FILTER),
					m_pIndexList[cache.nElemIndex].uOffset + frag.uOffset,
					(frag.uCompressSizeFlag & XPACK_COMPRESS_SIZE_FILTER)))
				{
					nResult = false;
					break;
				}
				cache.pFragmentInfo[nFragment].uCompressSizeFlag = 0;
			}
			else
			{
				bAllFragmentLoaded = false;
				if (nEndOffset <= nSize)
					break;
			}
		}
		nSize += frag.uSize;
	}
	if (nResult && bAllFragmentLoaded)
	{
		SAFE_FREE(cache.pFragmentInfo);
		cache.nNumFragment = 0;
	}
	return nResult;
}

