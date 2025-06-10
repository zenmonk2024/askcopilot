//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
// File:	KTabFile.cpp
// Date:	2002.02.20
// Code:	Huyi(Spe), wooy(Wu yue)
// Desc:	Tab File Operation Class
//---------------------------------------------------------------------------
#include "Precompile.h"
#include <ctype.h>
#include "File.h"
#include "TabFile.h"
#include "TabFileWritable.h"
#include "KGLog.h"

KRecycleBin<KTabFile, 2, RBAFM_NEW_DELETE>	KTabFile::ms_Recycle;

int kg_atoi (const char *str)
{
	const char *ip = str;
	int ch, ival;
	int neg;

	/* Skip leading whitespace. */
	while (isspace (*ip))
		ip++;

	if (*ip == 0)
		return (0);

	/* Check for indefinite. */
//	if ((ch = *ip) == 'I')
//		if (strncmp (ip, "INDEF", 5) == 0)
//			if (! (isalnum (ch = *(ip+5)) || ch == '_'))
//				return (INDEFI);
//	���ǲ���Ҫ INDEFI -- Modified by spe
	
	ch = *ip;

	/* Check for leading + or - sign. */
	neg = 0;
	if (ch == '-') {
		neg++;
		ip++;
	} else if (ch == '+')
		ip++;

	/* Accumulate sequence of digits. */
	ival = 0;
	while (isdigit (ch = *ip++))
		ival = ival * 10 + (ch - '0');

	return (neg ? -ival : ival);
}

C_ENGINE_API ITabFile*
g_OpenTabFile(const char* FileName, int ForceUnpakFile/* = false*/, int ForWrite/* = false*/)
{
	ITabFile*	pTab = NULL;
	IFile*		pFile = g_OpenFile(FileName, ForceUnpakFile, ForWrite);
	int			nResult = false;

	if (pFile)
	{
		if (!ForWrite)
		{
			if ((pTab = KTabFile::New()) != NULL)
				nResult = ((KTabFile*)pTab)->LoadData(pFile);
		}
		else if ((pTab = KTabFileWritable::New()) != NULL)
		{
			nResult = ((KTabFileWritable*)pTab)->LoadData(pFile);
		}
	}
	if (!nResult)
		SAFE_RELEASE(pTab);
	SAFE_RELEASE(pFile);
	return pTab;
}

ENGINE_API bool g_OpenFile(ITabFile** ppTabFile, const char* FileName, int ForceUnpakFile /* = false */, int ForWrite /* = false */)
{
	*ppTabFile = g_OpenTabFile(FileName, ForceUnpakFile, ForWrite);
	return (*ppTabFile != NULL);
}

C_ENGINE_API ITabFile* g_CreateTabFile()
{
	return KTabFileWritable::New();
}

//---------------------------------------------------------------------------
// ����:	KTabFile
// ����:	���캯��
//---------------------------------------------------------------------------
KTabFile::KTabFile()
{
	m_Width		= 0;
	m_Height	= 0;
	m_pMemory	= NULL;
	m_uMemorySize = 0;
	m_pOffsetTable = NULL;
    m_bErrorLogEnable = true;
}

//---------------------------------------------------------------------------
// ����:	~KTabFile
// ����:	���캯��
//---------------------------------------------------------------------------
KTabFile::~KTabFile()
{
	Clear();
}

//����һ���յ�KTabFile����
KTabFile* KTabFile::New()
{
    KTabFile* pTabFile = ms_Recycle.New();
    if (pTabFile)
    {
        pTabFile->SetErrorLog(false);
    }
	return pTabFile;
}

// ����:	����һ��Tab�ļ�
int	KTabFile::LoadData(IFile* pFile)
{
	assert(pFile);
	Clear();

	int				nResult = false;
	unsigned int	dwSize = pFile->Size();
	if (dwSize == 0)
		return true;

	m_pMemory = (unsigned char *)malloc(dwSize + 1);
	m_pMemory[dwSize] = 0;	// ������һ���ֽڵĳ��ȣ��Ա��ں������Ի��з��ж�ʱ�����ڶ�ȡԽ���ڴ�
	if (m_pMemory)
	{
		m_uMemorySize = dwSize;
		if (pFile->Read(m_pMemory, dwSize) == dwSize)
		{
			if (CreateTabOffset())
				nResult = true;
		}
	}
	if (!nResult)
		Clear();
	return nResult;
}

int	KTabFile::GetWidth()
{
	return m_Width;
}

int KTabFile::GetHeight()
{
	return m_Height;
}

//---------------------------------------------------------------------------
// ����:	CreateTabOffset
// ����:	������������ָ��ļ���ƫ�Ʊ�
//---------------------------------------------------------------------------
int KTabFile::CreateTabOffset()
{
	int		nWidth, nHeight, nOffset, nSize;
	unsigned char	*Buffer;

	nWidth	= 1;
	nHeight	= 1;
	nOffset = 0;

	Buffer	= m_pMemory;
	nSize	= m_uMemorySize;
	
	if (!Buffer || !nSize)
		return true;

	// ����һ�о����ж�����
	while (nOffset < nSize && *Buffer != 0x0d && *Buffer != 0x0a)
	{
		if (*Buffer == 0x09)
		{
			nWidth++;
		}
		Buffer++;
		nOffset++;
	}

	if (nOffset < nSize)
	{
		if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
		{
			Buffer += 2;	// 0x0a����
			nOffset += 2;	// 0x0a����
		}
		else
		{
			Buffer += 1;	// 0x0a����		
			nOffset += 1;	// 0x0a����
		}
	}

	while(nOffset < nSize)
	{
		while (*Buffer != 0x0d && *Buffer != 0x0a)
		{
			Buffer++;
			nOffset++;
			if (nOffset >= nSize)
				goto out_of_range;
		}
		if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
		{
			Buffer += 2;	// 0x0a����
			nOffset += 2;	// 0x0a����
		}
		else
		{
			Buffer += 1;	// 0x0a����		
			nOffset += 1;	// 0x0a����
		}
out_of_range:
		nHeight++;
	}
	m_Width		= nWidth;
	m_Height	= nHeight;

	m_pOffsetTable = (TABOFFSET*)malloc(m_Width * m_Height * sizeof(TABOFFSET));
	if (m_pOffsetTable == NULL)
		return false;

	memset(m_pOffsetTable, 0, m_Width * m_Height * sizeof(TABOFFSET));
	TABOFFSET* TabBuffer = m_pOffsetTable;
	Buffer = m_pMemory;

	nOffset = 0;
	int nLength;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			TabBuffer->dwOffset = nOffset;
			nLength = 0;
			while(*Buffer != 0x09 && *Buffer != 0x0d && *Buffer != 0x0a && nOffset < nSize)
			{
				Buffer++;
				nOffset++;
				nLength++;
			}
			if (nOffset >= nSize)
				break;

			Buffer++;	// 0x09��0x0d��0x0a(linux)����
			nOffset++;
			TabBuffer->dwLength = nLength;
			TabBuffer++;
			if (*(Buffer - 1) == 0x0a || *(Buffer - 1) == 0x0d)	//	�����Ѿ������ˣ���Ȼ����û��nWidth
			{
				if (j + 1 < nWidth)
					TabBuffer += nWidth - j - 1;
				break;
			}
		}
		if (nOffset >= nSize)
			break;

		if (*(Buffer - 1) == 0x0d && *Buffer == 0x0a)
		{
			Buffer++;				// 0x0a����	
			nOffset++;				// 0x0a����	
		}
	}
	return true;
}

//---------------------------------------------------------------------------
// ����:	Str2Column
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	szColumn
// ����:	�ڼ���
//---------------------------------------------------------------------------
int KTabFile::Str2Col(const char* szColumn)
{
	char	szTemp[4];
	strncpy(szTemp, szColumn, 3);
	szTemp[2] = 0;
	g_StrLower(szTemp);

	int		nIndex;
	if (szTemp[0])
	{
		if (szTemp[1] == 0)
			nIndex = (szTemp[0] - 'a');
		else
			nIndex = ((szTemp[0] - 'a' + 1) * 26 + szTemp[1] - 'a') + 1;
	}
	else
	{
		nIndex = -1;
	}
	return nIndex;
}

//---------------------------------------------------------------------------
// ����:	GetString
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			nColomn			��
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetString(int nRow, const char* szColumn, const char* lpDefault,
						char* lpRString, unsigned int dwSize, int bColumnLab)
{
	int nColumn;
	int	nRet = 0;

	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);

	nRet = GetValue(nRow - 1, nColumn - 1, lpRString, dwSize);
	if (1 != nRet)
		g_StrCpyLen(lpRString, lpDefault, dwSize);

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetString(%d, %s) failed !\n", nRow, szColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetString
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	szRow			��	���ؼ��֣�
//			szColomn		��	���ؼ��֣�
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetString(const char* szRow, const char* szColumn,
					const char* lpDefault, char* lpRString, unsigned int dwSize)
{
	int nRow, nColumn;
	int nRet = 0;

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	nRet = GetValue(nRow - 1, nColumn - 1, lpRString, dwSize);
	if (1 != nRet)
		g_StrCpyLen(lpRString, lpDefault, dwSize);

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetString(%s, %s) failed !\n", szRow, szColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetString
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��		��1��ʼ
//			nColomn			��		��1��ʼ
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetString(int nRow, int nColumn, const char* lpDefault,
						char* lpRString, unsigned int dwSize)
{
	int nRet = 0;

	nRet = GetValue(nRow - 1, nColumn - 1,  lpRString, dwSize);

	if (1 != nRet)
		g_StrCpyLen(lpRString, lpDefault, dwSize);

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetString(%d, %d) failed !\n", nRow, nColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetInteger
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			szColomn		��
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetInteger(int nRow, const char* szColumn, int nDefault,
						int *pnValue, int bColumnLab)
{
	char	Buffer[32];
	int		nColumn;
	int		nRet = 0;

	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	nRet = GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer));

	if (1 == nRet)
	{
		*pnValue = kg_atoi(Buffer);
	}
	else
	{
		*pnValue = nDefault;
	}

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetInteger(%d, %s) failed !\n", nRow, szColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetInteger
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	szRow			��
//			szColomn		��
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetInteger(const char* szRow, const char* szColumn,
						int nDefault, int *pnValue)
{
	int		nRow, nColumn;
	char	Buffer[32];
	int		nRet = 0;

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);

	nRet = GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer));
	if (1 == nRet)
	{
		*pnValue = kg_atoi(Buffer);
	}
	else
	{
		*pnValue = nDefault;
	}

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetInteger(%s, %s) failed !\n", szRow, szColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetInteger
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��		��1��ʼ
//			nColomn			��		��1��ʼ
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int KTabFile::GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)
{
	char	Buffer[32];
	int		nRet = 0;

	nRet = GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer));
	if (1 == nRet)
	{
		*pnValue = kg_atoi(Buffer);
	}
	else
	{
		*pnValue = nDefault;
	}

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetInteger(%d, %d) failed !\n", nRow, nColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetFloat
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			szColomn		��
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetFloat(int nRow, const char* szColumn, float fDefault,
						float *pfValue, int bColumnLab)
{
	char	Buffer[32];
	int		nColumn;
	int		nRet = 0;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	nRet = GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer));
	if (1 == nRet)
	{
		*pfValue = (float)atof(Buffer);
	}
	else
	{
		*pfValue = fDefault;
	}

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetFloat(%d, %s) failed !\n", nRow, szColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetFloat
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	szRow			��
//			szColomn		��
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetFloat(const char* szRow, const char* szColumn,
						float fDefault, float *pfValue)
{
	int		nRow, nColumn;
	char	Buffer[32];
	int		nRet = 0;

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	nRet = GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer));
	if (1 == nRet)
	{
		*pfValue = (float)atof(Buffer);
	}
	else
	{
		*pfValue = fDefault;
	}

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetFloat(%s, %s) failed !\n", szRow, szColumn);
    }

	return nRet;
}

//---------------------------------------------------------------------------
// ����:	GetFloat
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��		��1��ʼ
//			nColomn			��		��1��ʼ
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int KTabFile::GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)
{
	char	Buffer[32];
	int nRet = 0;
	
	nRet = GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer));
	if (1 == nRet)
	{
		*pfValue = (float)atof(Buffer);
	}
	else
	{
		*pfValue = fDefault;
	}

    if (nRet == 0 && m_bErrorLogEnable)
    {
        KGLogPrintf(KGLOG_DEBUG, "GetFloat(%d, %d) failed !\n", nRow, nColumn);
    }

	return nRet;
}
//---------------------------------------------------------------------------
// ����:	GetValue
// ����:	ȡ��ĳ��ĳ���ַ�����ֵ
// ����:	nRow			��
//			nColomn			��
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
// ����:	1���ɹ�	0����񲻶�	-1��δ�ʹ��Ĭ��ֵ
//---------------------------------------------------------------------------
int	KTabFile::GetValue(int nRow, int nColumn, char* lpRString, unsigned int dwSize)
{
	if (nRow >= m_Height || nColumn >= m_Width || nRow < 0 || nColumn < 0)
    {
		return 0;
    }

	char*		Buffer = (char*)m_pMemory;
	TABOFFSET*	TempOffset = m_pOffsetTable;
	TempOffset += nRow * m_Width + nColumn;

	Buffer += TempOffset->dwOffset;
	if (TempOffset->dwLength == 0)
	{
		lpRString[0] = 0;
		return -1;
	}
	if (dwSize > TempOffset->dwLength)
	{
		memcpy(lpRString, Buffer, TempOffset->dwLength);
		lpRString[TempOffset->dwLength] = 0;
	}
	else
	{
		memcpy(lpRString, Buffer, dwSize);
		lpRString[dwSize - 1] = 0;
	}

	return 1;
}
//---------------------------------------------------------------------------
// ����:	Clear
// ����:	���TAB�ļ�������
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
void KTabFile::Clear()
{
	m_Width = 0;
	m_Height = 0;
	m_uMemorySize = 0;
	SAFE_FREE(m_pMemory);
	SAFE_FREE(m_pOffsetTable);
}

//---------------------------------------------------------------------------
// ����:	FindRow
// ����:	�����йؼ���
// ����:	szRow���йؼ��֣�
// ����:	int
//---------------------------------------------------------------------------
int KTabFile::FindRow(const char* szRow)
{
	char	szTemp[128];
	for (int i = 0; i < m_Height; i++)	// ��1��ʼ��������һ�е��ֶ���
	{
		if (1 == GetValue(i, 0, szTemp, sizeof(szTemp)))
		{ 
			if (!strcmp(szTemp, szRow))
				return i + 1; //�Ķ��˴�Ϊ��һ by Romandou,��������1Ϊ���ı��
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
// ����:	FindColumn
// ����:	�����йؼ���
// ����:	szColumn���йؼ��֣�
// ����:	int
//---------------------------------------------------------------------------
int KTabFile::FindColumn(const char* szColumn)
{
	char	szTemp[128];
	for (int i = 0; i < m_Width; i++)	// ��1��ʼ��������һ�е��ֶ���
	{
		if (1 == GetValue(0, i, szTemp, sizeof(szTemp)))
		{
			if (!strcmp(szTemp, szColumn))
				return i + 1;//�Ķ��˴�Ϊ��һ by Romandou,��������1Ϊ���ı��
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
// ����:	Col2Str
// ����:	������ת���ַ���
// ����:	szColumn
// ����:	�ڼ���
//---------------------------------------------------------------------------
void KTabFile::Col2Str(int nCol, char* szColumn)
{
	if (nCol < 26)
	{
		szColumn[0] = 'A' + nCol;
		szColumn[1]	= 0;
	}
	else
	{
		szColumn[0] = 'A' + (nCol / 26 - 1);
		szColumn[1] = 'A' + nCol % 26;
		szColumn[2] = 0;
	}
}

void KTabFile::Release()
{
	Clear();
	ms_Recycle.Delete(this);
}

void KTabFile::SetErrorLog(BOOL bEnable)
{
    m_bErrorLogEnable = bEnable;
}
