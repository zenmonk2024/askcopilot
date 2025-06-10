//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// Date:	2000.08.08
// Code:	WangWei(Daphnis), Wuyue(Wooy)
// Desc:	Ini File Operation Class

// ע��:   �������ݶ�ȡ,�еĽӿ��ϲ��ܴ���Ĭ��ֵ,��������������ݿɹ���ȡ,
//			�򱣳�ԭ����ռ��е�ֵ����.

//---------------------------------------------------------------------------
#include "Precompile.h"
#include "KGPublic.h"
#include "KGCRT.h"
#include "File.h"
#include "IniFile.h"
#include "KSG_StringProcess.h"

extern int kg_atoi (const char *str);

#define COPY_SECTION_AND_ADD_BOUND(d, s)	\
{											\
	if (s[0] != '[')						\
	{										\
		d[0] = '[';							\
		strcpy(&d[1], s);					\
		strcat(&d[1], "]");					\
	}										\
	else									\
	{										\
		strcpy(d, s);						\
	}										\
}

#define COPY_SECTION_AND_REMOVE_BOUND(d, s)	\
{											\
	int nSecLen = strlen(s);				\
	if (s[0] == '[' && s[nSecLen-1] == ']')	\
	{										\
		strncpy(d, &s[1], nSecLen-2);		\
		d[nSecLen-2] = 0;					\
	}										\
	else									\
	{										\
		strcpy(d, s);						\
	}										\
}

KRecycleBin<KIniFile, 4, RBAFM_NEW_DELETE>	KIniFile::ms_Recycle;     /// ��ģ���еľ�̬��Ա����

C_ENGINE_API IIniFile*
g_OpenIniFile(const char* FileName, int ForceUnpakFile/* = false*/, int ForWrite/* = false*/)
{
	KIniFile*	pIni = NULL;
	IFile*		pFile = g_OpenFile(FileName, ForceUnpakFile, ForWrite);

	if (pFile)
	{
		pIni = KIniFile::New();
		if (pIni)
		{
			if (!pIni->LoadData(pFile))
			{
				pIni->Release();
				pIni = NULL;
			}
		}
		pFile->Release();
		pFile = NULL;
	}

	if (pIni == NULL && ForWrite)
		pIni = KIniFile::New();
	return pIni;
}

ENGINE_API bool g_OpenFile(IIniFile** ppIniFile, const char* FileName, int ForceUnpakFile /* = false */, int ForWrite /* = false */)
{
	*ppIniFile = g_OpenIniFile(FileName, ForceUnpakFile, ForWrite);
	return (*ppIniFile != NULL);
}

C_ENGINE_API IIniFile* g_CreateIniFile()
{
	return KIniFile::New();
}

//---------------------------------------------------------------------------
// ����:	KIniFile
// ����:	���캯��
//---------------------------------------------------------------------------
KIniFile::KIniFile()
{
	ZeroMemory(&m_Header, sizeof(SECNODE));
	m_Offset = 0;
}

//---------------------------------------------------------------------------
// ����:	~KIniFile
// ����:	���캯��
//---------------------------------------------------------------------------
KIniFile::~KIniFile()
{
	Clear();
}

//����һ���յ�KIniFile����
KIniFile* KIniFile::New()
{
    return ms_Recycle.New();
}

//---------------------------------------------------------------------------
// ����:	����һ��INI�ļ�
// ����:	1 �ɹ�,  0	ʧ��
//---------------------------------------------------------------------------
int KIniFile::LoadData(IFile* pFile)
{
	assert(pFile);
	Clear();

	int				nResult = false;
	BYTE*			pBuffer = NULL;
	unsigned long	uSize = pFile->Size();

	pBuffer = new BYTE[uSize + 4];	//�ļ��п���Ҫ��0֮��ģ�������ļ�ԭ��С�п��ܻ�Խ��
	memset(pBuffer, 0, sizeof(uSize + 4));
	if (pBuffer)
	{
		if (pFile->Read(pBuffer, uSize) == uSize)
		{
			CreateIniLink(pBuffer, uSize);
			nResult = true;
		}
	}
	SAFE_DELETE_ARRAY(pBuffer);
	return nResult;
}

//---------------------------------------------------------------------------
// ����:	Save
// ����:	���浱ǰ��INI�ļ�
// ����:	1 �ɹ�, 0ʧ��
//---------------------------------------------------------------------------
int KIniFile::Save(const char* FileName)
{
	int			nResult         = false;
	IFile*		piFile          = NULL;
	SECNODE*	SecNode         = m_Header.pNextNode;
	KEYNODE*	KeyNode         = NULL;
    DWORD       dwStringLen     = 0;
    DWORD       dwWriteSize     = 0;
    const char  cszNewLine[3]   = "\r\n";

    piFile = g_CreateFile(FileName);
    KG_PROCESS_ERROR(piFile);

	while (SecNode != NULL)
	{
		dwStringLen = (DWORD)strlen(SecNode->pSection);
		dwWriteSize = piFile->Write(SecNode->pSection, dwStringLen);
		KG_PROCESS_ERROR(dwWriteSize == dwStringLen);
		dwWriteSize = piFile->Write(cszNewLine, 2);
		KG_PROCESS_ERROR(dwWriteSize == 2);

		KeyNode = SecNode->RootNode.pNextNode;
		while (KeyNode != NULL)
		{
			dwStringLen = (DWORD)strlen(KeyNode->pKey);
			dwWriteSize = piFile->Write(KeyNode->pKey, dwStringLen);
			KG_PROCESS_ERROR(dwWriteSize == dwStringLen);
			dwWriteSize = piFile->Write("=", 1);
			KG_PROCESS_ERROR(dwWriteSize == 1);
			dwStringLen = (DWORD)strlen(KeyNode->pValue);
			dwWriteSize = piFile->Write(KeyNode->pValue, dwStringLen);
			KG_PROCESS_ERROR(dwWriteSize == dwStringLen);
			dwWriteSize = piFile->Write(cszNewLine, 2);
			KG_PROCESS_ERROR(dwWriteSize == 2);
			KeyNode = KeyNode->pNextNode;
		}
		dwWriteSize = piFile->Write(cszNewLine, 2);
		KG_PROCESS_ERROR(dwWriteSize == 2);
		SecNode = SecNode->pNextNode;
	}
	nResult = true;
Exit0:
	KG_COM_RELEASE(piFile);
	return nResult;
}

//---------------------------------------------------------------------------
// ����:	Clear
// ����:	���INI�ļ�������
//---------------------------------------------------------------------------
void KIniFile::Clear()
{
	ReleaseIniLink();
}

//---------------------------------------------------------------------------
// ����:	ReadLine
// ����:	��ȡINI�ļ���һ��
// ����:	Buffer	����
//			Szie	����
// ����:	true		�ɹ�
//			false		ʧ��
//---------------------------------------------------------------------------
int KIniFile::ReadLine(char* Buffer,long Size)
{
	if (m_Offset >= Size)
	{
		return false;
	}
	while (Buffer[m_Offset] != 0x0D && Buffer[m_Offset] != 0x0A)
	{
		m_Offset++;
		if (m_Offset >= Size)
			break;
	}
	Buffer[m_Offset] = 0;
  /// ��֤Bufferָ����һ�еĿ�ʼ
	if (Buffer[m_Offset] == 0x0D && Buffer[m_Offset + 1] == 0x0A)
		m_Offset += 2;
	else
		m_Offset += 1;	//linux [wxb 2003-7-29]
	return true;
}

//---------------------------------------------------------------------------
// ����:	IsKeyChar
// ����:	�ж�һ���ַ��Ƿ�Ϊ��ĸ
// ����:	ch		Ҫ�жϵ��ַ�	
// ����:	true	����ĸ
//			false	������ĸ
// ע�ͣ�	��֧��������Key�������ǿ�����Section��
//---------------------------------------------------------------------------
int KIniFile::IsKeyChar(char ch)
{
	if (((ch >= 'A') && (ch <= 'Z')) ||
		((ch >= 'a') && (ch <= 'z')) ||
		((ch >= '0') && (ch <= '9')) ||
		(ch == '$') || (ch == '.') ||
		(ch == '_') || (ch == '-'))
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
// ����:	CreateIniLink
// ����:	����Ini����
// ����:	pBuffer		����
//			nBufLen		����
//---------------------------------------------------------------------------
void KIniFile::CreateIniLink(void* pBuffer, long nBufLen)
{
	char* lpBuffer = (char*)pBuffer;
	char* lpString = NULL;
	char* lpValue  = NULL;
	char  szSection[64] = "[MAIN]";

	m_Offset = 0;
	while (m_Offset < nBufLen)
	{
		lpString = &lpBuffer[m_Offset];
		if (!ReadLine(lpBuffer, nBufLen))
			break;
		if (IsKeyChar(*lpString))
		{
			lpValue = SplitKeyValue(lpString);
			if (lpValue)
				SetKeyValue(szSection, lpString, lpValue);
		}
		else if (*lpString == '[')
		{
			g_StrCpyLen(szSection, lpString, sizeof(szSection));
		}
	}
}

//---------------------------------------------------------------------------
// ����:	ReleaseIniLink()
// ����:	�ͷ�Ini����
//---------------------------------------------------------------------------
void KIniFile::ReleaseIniLink()
{
	SECNODE* pThisSec = &m_Header;
	SECNODE* pNextSec = pThisSec->pNextNode;
	KEYNODE* pThisKey = NULL;
	KEYNODE* pNextKey = NULL;

	while (pNextSec != NULL)
	{
		pThisSec = pNextSec->pNextNode;
		pThisKey = &pNextSec->RootNode;
		pNextKey = pThisKey->pNextNode;
		while (pNextKey != NULL)
		{
			pThisKey = pNextKey->pNextNode;
			SAFE_DELETE_ARRAY(pNextKey->pKey);
			SAFE_DELETE_ARRAY(pNextKey->pValue);
			SAFE_DELETE(pNextKey);
			pNextKey = pThisKey;
		}
		SAFE_DELETE_ARRAY(pNextSec->pSection);
		SAFE_DELETE_ARRAY(pNextSec);
		pNextSec = pThisSec;
	}
	m_Header.pNextNode = NULL;
}

//---------------------------------------------------------------------------
// ����:	SplitKeyValue
// ����:	�ָ�Key��Value
// ����:	pString		Key=Value
// ����:	ָ��Value
//---------------------------------------------------------------------------
char* KIniFile::SplitKeyValue(char* pString)
{
	char* pValue = pString;
	while (*pValue)
	{
		if (*pValue == '=')
		{
			*pValue = 0;
			return pValue + 1;    /// = ����û�пո�
		}
		pValue++;
	}
	return NULL;
}

//---------------------------------------------------------------------------
// ����:	String2Id
// ����:	�ַ���ת��32 bits ID
// ����:	pString		�ַ���
// ����:	32 bits ID
//---------------------------------------------------------------------------
unsigned int KIniFile::String2Id(const char* pString)
{
	unsigned int Id = 0;
	for (int i=0; pString[i]; i++)
	{
		Id = (Id + (i+1) * pString[i]) % 0x8000000b * 0xffffffef;
	}
	return Id ^ 0x12345678;
}

//---------------------------------------------------------------------------
// ����:	IsSectionExist
// ����:	Section�Ƿ����
// ����:	pSection	�ڵ�����
// ����:	int
//---------------------------------------------------------------------------
int KIniFile::IsSectionExist(const char* pSection)
{
	// setup section name
	char szSection[64];
	COPY_SECTION_AND_ADD_BOUND(szSection, pSection);
	
	// search for the matched section
	SECNODE* pSecNode = m_Header.pNextNode;
	unsigned int dwID = String2Id(szSection);
	while (pSecNode != NULL)
	{
		if (dwID == pSecNode->dwID)
			return true;
		pSecNode = pSecNode->pNextNode;
	}
	
	return false;
}

//---------------------------------------------------------------------------
// ����:	EraseSection
// ����:	���һ��Section������
// ����:	pSection	�ڵ�����
//---------------------------------------------------------------------------
void KIniFile::EraseSection(const char* pSection)
{
	// setup section name
	char szSection[64];
	COPY_SECTION_AND_ADD_BOUND(szSection, pSection);

	// search for the matched section
	unsigned int dwID = String2Id(szSection);
	SECNODE* pPreSec = &m_Header;
	SECNODE* pSecNode;
	while((pSecNode = pPreSec->pNextNode) != NULL)
	{
		if (dwID == pSecNode->dwID)
			break;
		pPreSec = pSecNode;
	}

	// if no such section found
	if (pSecNode == NULL)
		return;
	pPreSec->pNextNode = pSecNode->pNextNode;

	// erase all key in the section
	KEYNODE* pThisKey = &pSecNode->RootNode;
	KEYNODE* pNextKey = pThisKey->pNextNode;
	while (pNextKey != NULL)
	{
		pThisKey = pNextKey->pNextNode;
		SAFE_DELETE_ARRAY(pNextKey->pKey);
		SAFE_DELETE_ARRAY(pNextKey->pValue);
		SAFE_DELETE(pNextKey);
		pNextKey = pThisKey;
	}
	pSecNode->RootNode.pNextNode = NULL;
	SAFE_DELETE_ARRAY(pSecNode->pSection);
	SAFE_DELETE(pSecNode);
}

//---------------------------------------------------------------------------
// ����:	EraseKey
// ����:	���Section����һ��Key������
// ����:	pSection	�ڵ�����
//---------------------------------------------------------------------------
void	KIniFile::EraseKey(const char* lpSection, const char* lpKey)
{
	// setup section name
	char szSection[64];
	COPY_SECTION_AND_ADD_BOUND(szSection, lpSection);

	// search for the matched section
	SECNODE* pSecNode = m_Header.pNextNode;
	unsigned int dwID = String2Id(szSection);
	while (pSecNode != NULL)
	{
		if (dwID == pSecNode->dwID)
		{
			break;
		}
		pSecNode = pSecNode->pNextNode;
	}

	// if no such section found
	if (pSecNode == NULL)
	{
		return;
	}

	KEYNODE* pThisKey = &pSecNode->RootNode;
	KEYNODE* pNextKey = pThisKey->pNextNode;
	dwID = String2Id(lpKey);
	while (pNextKey != NULL)
	{
		if (pNextKey->dwID == dwID)
		{
			pThisKey->pNextNode = pNextKey->pNextNode;
			SAFE_DELETE_ARRAY(pNextKey->pKey);
			SAFE_DELETE_ARRAY(pNextKey->pValue);
			SAFE_DELETE(pNextKey);
			if (pSecNode->RootNode.pNextNode == NULL)
				EraseSection(szSection);
			return;
		}
		pThisKey = pNextKey;
		pNextKey = pNextKey->pNextNode;
	}
}

//---------------------------------------------------------------------------
// ����:	SetKeyValue
// ����:	����Key��Value
// ����:	pSection	����
//			pKey		����
//			pValue		��ֵ
// ����:	true���ɹ� false��ʧ��
//---------------------------------------------------------------------------
int KIniFile::SetKeyValue(
	const char*	pSection,
	const char*	pKey,
	const char*	pValue)
{
	int		nLen;
	unsigned int	dwID;

	// setup section name
	char szSection[64];
	COPY_SECTION_AND_ADD_BOUND(szSection, pSection);

	// search for the matched section
	SECNODE* pThisSecNode = &m_Header;
	SECNODE* pNextSecNode = pThisSecNode->pNextNode;
	dwID = String2Id(szSection);
	while (pNextSecNode != NULL)
	{
		if (dwID == pNextSecNode->dwID)
		{
			break;
		}
		pThisSecNode = pNextSecNode;
		pNextSecNode = pThisSecNode->pNextNode;
	}

	// if no such section found create a new section
	if (pNextSecNode == NULL)
	{
		nLen = strlen(szSection) + 1;
		pNextSecNode = new SECNODE;
		pNextSecNode->pSection = new char[nLen];
		memcpy(pNextSecNode->pSection, szSection, nLen);
		pNextSecNode->dwID = dwID;
		pNextSecNode->RootNode.pNextNode = NULL;
		pNextSecNode->pNextNode = NULL;
		pThisSecNode->pNextNode = pNextSecNode;
	}

	// search for the same key
	KEYNODE* pThisKeyNode = &pNextSecNode->RootNode;
	KEYNODE* pNextKeyNode = pThisKeyNode->pNextNode;
	dwID = String2Id(pKey);
	while (pNextKeyNode != NULL)
	{
		if (dwID == pNextKeyNode->dwID)
		{
			break;
		}
		pThisKeyNode = pNextKeyNode;
		pNextKeyNode = pThisKeyNode->pNextNode;
	}

	// if no such key found create a new key
	if (pNextKeyNode == NULL)
	{
		pNextKeyNode = new KEYNODE;

		nLen = strlen(pKey) + 1;
		pNextKeyNode->pKey = new char[nLen];
		memcpy(pNextKeyNode->pKey, (void*)pKey, nLen);

		nLen = strlen(pValue) + 1;
		pNextKeyNode->pValue = new char[nLen];
		memcpy(pNextKeyNode->pValue, (void*)pValue, nLen);

		pNextKeyNode->dwID = dwID;
		pNextKeyNode->pNextNode = NULL;
		pThisKeyNode->pNextNode = pNextKeyNode;
	}
	// replace the old value with new
	else
	{
		SAFE_DELETE_ARRAY(pNextKeyNode->pValue);
		nLen = strlen(pValue) + 1;
		pNextKeyNode->pValue = new char[nLen];
		memcpy(pNextKeyNode->pValue, (void*)pValue, nLen);
	}
	return true;
}

//---------------------------------------------------------------------------
// ����:	GetKeyValue
// ����:	ȡ��Key��Value
// ����:	pSection	����
//			pKey		����
//			pValue		��ֵ
// ����:	true���ɹ� false��ʧ��
//---------------------------------------------------------------------------
int	KIniFile::GetKeyValue(const char* pSection,const char* pKey,char* pValue,unsigned int dwSize)
{
	unsigned int	dwID;

	// setup section name
	char szSection[64];
	COPY_SECTION_AND_ADD_BOUND(szSection, pSection);

	// search for the matched section
	SECNODE* pSecNode = m_Header.pNextNode;
	dwID = String2Id(szSection);
	while (pSecNode != NULL)
	{
		if (dwID == pSecNode->dwID)
		{
			break;
		}
		pSecNode = pSecNode->pNextNode;
	}

	// if no such section founded
	if (pSecNode == NULL)
	{
		return false;
	}

	// search for the same key
	KEYNODE* pKeyNode = pSecNode->RootNode.pNextNode;
	dwID = String2Id(pKey);
	while (pKeyNode != NULL)
	{
		if (dwID == pKeyNode->dwID)
		{
			break;
		}
		pKeyNode = pKeyNode->pNextNode;
	}

	// if no such key found
	if (pKeyNode == NULL)
	{
		return false;
	}

	// copy the value of the key
	g_StrCpyLen(pValue, pKeyNode->pValue, dwSize);
	return true;
}

//---------------------------------------------------------------------------
// ����:	GetString
// ����:	��ȡһ���ַ���
// ����:	lpSection		����
//			lpKeyName		����
//			lpDefault		ȱʡֵ
//			lpRString		����ֵ
//			dwSize			�����ַ�������󳤶�
//---------------------------------------------------------------------------
int KIniFile::GetString(
	const char* lpSection,		// points to section name
	const char* lpKeyName,		// points to key name
	const char* lpDefault,		// points to default string
	char* lpRString,		// points to destination buffer
	unsigned int dwSize			// size of string buffer
	)
{
	if (GetKeyValue(lpSection, lpKeyName, lpRString, dwSize))
		return true;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return false;
}

//---------------------------------------------------------------------------
// ����:	GetInteger
// ����:	��ȡһ������
// ����:	lpSection		����
//			lpKeyName		����
//			nDefault		ȱʡֵ
//			pnValue			����ֵ
//---------------------------------------------------------------------------
int KIniFile::GetInteger(
	const char* lpSection,		// points to section name
	const char* lpKeyName,		// points to key name
	int   nDefault,			// default value
	int   *pnValue          // points to value
	)
{
	char Buffer[32];
	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		*pnValue = kg_atoi(Buffer);
		return true;
	}
	else
	{
		*pnValue = nDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// ����:	GetInteger2
// ����:	��ȡ2������������֮���ö��ŷָ
// ����:	lpSection		����
//			lpKeyName		����
//			pnValue1		����ֵ1
//			pnValue2		����ֵ2
// ����:	void
//---------------------------------------------------------------------------
int KIniFile::GetInteger2(
	const char* lpSection,		// points to section name
	const char* lpKeyName,		// points to key name
	int   *pnValue1,		// value 1
	int   *pnValue2			// value 2
	)
{
	char  Buffer[64];
	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		char* pSplit = strchr(Buffer, ',');
		if (pSplit)
		{
			*pSplit = '\0';
			*pnValue2 = kg_atoi(pSplit + 1);
		}
		*pnValue1 = kg_atoi(Buffer);
		return (pSplit != NULL);
	}
	return false;
}

//---------------------------------------------------------------------------
// ����:	GetFloat
// ����:	��ȡ1��������
// ����:	lpSection		����
//			lpKeyName		����
//			fDefault		ȱʡֵ
//			pfValue			����ֵ
// ����:	
//---------------------------------------------------------------------------
int KIniFile::GetFloat(
	const char*	lpSection,		// points to section name
	const char*	lpKeyName,		// points to key name
	float	fDefault,		// default value
	float	*pfValue        // return value
	)
{
	char Buffer[32];
	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return true;
	}
	else
	{
		*pfValue = fDefault;
		return false;
	}
}
//---------------------------------------------------------------------------
// ����:	GetFloat2
// ����:	��ȡ2����������֮���ö��ŷָ
// ����:	lpSection		����
//			lpKeyName		����
//			pfValue1		����ֵ1
//			pfValue2		����ֵ2
// ����:	
//---------------------------------------------------------------------------
int KIniFile::GetFloat2(
	const char* lpSection,		// points to section name
	const char* lpKeyName,		// points to key name
	float *pfValue1,		// value 1
	float *pfValue2			// value 2
	)
{
	char  Buffer[64];
	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		char* pSplit = strchr(Buffer, ',');
		if (pSplit)
		{
			*pSplit = '\0';
			*pfValue2 = (float)atof(pSplit + 1);
		}
		*pfValue1 = (float)atof(Buffer);
		return (pSplit != NULL);
	}
	return false;
}

//---------------------------------------------------------------------------
// ����:	GetStruct
// ����:	��ȡһ���ṹ
// ����:	lpSection		����
//			lpKeyName		����
//			lpStruct		����
//			dwSize			�����С
// ����:	void
//---------------------------------------------------------------------------
int KIniFile::GetStruct(
	const char*	lpSection,	// pointer to section name
	const char*	lpKeyName,	// pointer to key name
	void*	lpStruct,		// pointer to buffer that contains data to add
	unsigned int	dwSize	// size, in bytes, of the buffer
	)
{
	char    Buffer[2048 * 2];
	unsigned char*	lpByte;
	unsigned char	ah,al;

	if (!GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
		return false;
	lpByte = (unsigned char*)lpStruct;
	int len = strlen(Buffer);
	if (len != (int)dwSize * 2)
		return false;
	for (int i = 0; i < len; i += 2)
	{
		// get byte high
		ah = Buffer[i];
		if ((ah >= 48) && (ah <= 57))
			ah = (unsigned char)(ah - 48);
		else
			ah = (unsigned char)(ah - 65 + 10);
		// get byte low
		al = Buffer[i+1];
		if ((al >= 48) && (al <= 57))
			al = (unsigned char)(al - 48);
		else
			al = (unsigned char)(al - 65 + 10);
		// set struct bye
		*lpByte++ = (unsigned char)(ah * 16 + al);    /// ��0xFFFFFFFFת����ʮ���Ƶģ� eg����A302B1�� ->13 2 12
	}
	return true;
}

//---------------------------------------------------------------------------
// ����:	WriteString
// ����:	д��һ���ַ���
// ����:	lpSection		����
//			lpKeyName		����
//			lpString		�ַ���
//---------------------------------------------------------------------------
int KIniFile::WriteString(
	const char*	lpSection,		// pointer to section name
	const char*	lpKeyName,		// pointer to key name
	const char*	lpString		// pointer to string to add
	)
{
	return SetKeyValue(lpSection, lpKeyName, lpString);
}

//---------------------------------------------------------------------------
// ����:	WriteInteger
// ����:	д��һ������
// ����:	lpSection		����
//			lpKeyName		����
//			nValue			����
//---------------------------------------------------------------------------
int KIniFile::WriteInteger(
	const char*	lpSection,		// pointer to section name
	const char*	lpKeyName,		// pointer to key name
	int 	nValue			// Integer to write
	)
{
	char Buffer[32];
	sprintf(Buffer, "%d", nValue);
	return SetKeyValue(lpSection, lpKeyName, Buffer);
}
//---------------------------------------------------------------------------
// ����:	WriteInteger2
// ����:	д��2������
// ����:	lpSection		����
//			lpKeyName		����
//			nValue1			����1
//			nValue2			����2
//---------------------------------------------------------------------------
int KIniFile::WriteInteger2(
	const char*	lpSection,		// pointer to section name
	const char*	lpKeyName,		// pointer to key name
	int 	Value1,			// value 1 to write
	int		Value2			// value 2 to write
	)
{
	char Buffer[64];
	sprintf(Buffer, "%d,%d", Value1, Value2);
	return SetKeyValue(lpSection, lpKeyName, Buffer);
}

//---------------------------------------------------------------------------
// ����:	WriteFloat
// ����:	д��1��������
// ����:	lpSection		����
//			lpKeyName		����
//			fValue			������
//---------------------------------------------------------------------------
int KIniFile::WriteFloat(
	const char*	lpSection,		// pointer to section name
	const char*	lpKeyName,		// pointer to key name
	float	fValue			// Integer to write
	)
{
	char Buffer[32];
	sprintf(Buffer,"%f",fValue);
	return SetKeyValue(lpSection, lpKeyName, Buffer);
}
//---------------------------------------------------------------------------
// ����:	WriteFloat2
// ����:	д��2��������
// ����:	lpSection		����
//			lpKeyName		����
//			fValue1			������1
//			fValue2			������2
// ����:	void
//---------------------------------------------------------------------------
int KIniFile::WriteFloat2(
	const char*	lpSection,		// pointer to section name
	const char*	lpKeyName,		// pointer to key name
	float 	fValue1,		// value 1 to write
	float	fValue2			// value 2 to write
	)
{
	char Buffer[64];
	sprintf(Buffer, "%f,%f", fValue1, fValue2);
	return SetKeyValue(lpSection, lpKeyName, Buffer);
}

//---------------------------------------------------------------------------
// ����:	WriteFloat2
// ����:	д��һ���ṹ
// ����:	lpSection		����
//			lpKeyName		����
//			lpStruct		�ṹ
//			dwSize			�ṹ��С
//---------------------------------------------------------------------------
int KIniFile::WriteStruct(
	const char*	lpSection,		// pointer to section name
	const char*	lpKeyName,		// pointer to key name
	void*	lpStruct,		// pointer to buffer that contains data to add
	unsigned int 	dwSize			// size, in bytes, of the buffer
	)
{
	char    Buffer[2048 * 2];
	char*	lpBuff = Buffer;
	unsigned char*	lpByte;

	if (dwSize >= 2048)
		return false;

	lpByte = (unsigned char*) lpStruct;
	for (unsigned int i=0; i<dwSize; i++)
	{
		sprintf(lpBuff,"%02X",*lpByte);
		lpBuff++;
		lpBuff++;
		lpByte++;
	}
	return SetKeyValue(lpSection, lpKeyName, Buffer);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int KIniFile::GetNextSection(const char* pSection, char* pNextSection)
{
	
	if (!pSection[0])	//	�������Ϊ""ʱ����ȡ��һ��Section
	{
		if (!m_Header.pNextNode)
			return false;
		else
		{
			COPY_SECTION_AND_REMOVE_BOUND(pNextSection, m_Header.pNextNode->pSection);
			return true;
		}
	}
	else
	{
		char szSection[64];
		COPY_SECTION_AND_ADD_BOUND(szSection, pSection);

		//	���Ҳ���������Section
		SECNODE* pThisSecNode = &m_Header;
		SECNODE* pNextSecNode = pThisSecNode->pNextNode;
		unsigned int dwID = String2Id(szSection);
		while (pNextSecNode != NULL)
		{
			if (dwID == pNextSecNode->dwID)
			{
				break;
			}
			pThisSecNode = pNextSecNode;
			pNextSecNode = pThisSecNode->pNextNode;
		}
		//	û�����Section������ν��һ��
		if (!pNextSecNode)
		{
			return false;
		}
		else
		{
			//	����������Section�Ѿ������һ����
			if (!pNextSecNode->pNextNode)
			{
				return false;
			}
			else	
			{
				COPY_SECTION_AND_REMOVE_BOUND(pNextSection, pNextSecNode->pNextNode->pSection);
				return true;
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int KIniFile::GetNextKey(const char* pSection, const char* pKey, char* pNextKey)
{
	char szSection[64];
	COPY_SECTION_AND_ADD_BOUND(szSection, pSection);

	//	���Ҳ���������Section
	SECNODE* pThisSecNode = &m_Header;
	SECNODE* pNextSecNode = pThisSecNode->pNextNode;
	unsigned int dwID = String2Id(szSection);
	while (pNextSecNode != NULL)
	{
		if (dwID == pNextSecNode->dwID)
		{
			break;
		}
		pThisSecNode = pNextSecNode;
		pNextSecNode = pThisSecNode->pNextNode;
	}
	//	û�����Section������νȡKey
	if (!pNextSecNode)
	{
		return false;
	}

	KEYNODE* pThisKeyNode = &pNextSecNode->RootNode;
	KEYNODE* pNextKeyNode = pThisKeyNode->pNextNode;

	//	Key���Ĳ���Ϊ""�Ļ���ȡ��һ��Key
	if (!pKey[0])
	{
		//	û��Key������ʧ��
		if (!pNextKeyNode->pKey)
		{
			return false;
		}
		else
		{
			strcpy(pNextKey, pNextKeyNode->pKey);
			return true;
		}
	}

	dwID = String2Id(pKey);
	while(pNextKeyNode != NULL)
	{
		if (dwID == pNextKeyNode->dwID)
		{
			break;
		}
		pThisKeyNode = pNextKeyNode;
		pNextKeyNode = pThisKeyNode->pNextNode;
	}
	//	�Ҳ���������Key������ν��һ��
	if (!pNextKeyNode)
	{
		return false;
	}
	//	����������Key�Ѿ������һ����
	if (!pNextKeyNode->pNextNode)
	{
		return false;
	}
	else
	{
		strcpy(pNextKey, pNextKeyNode->pNextNode->pKey);
		return true;
	}
}

int	KIniFile::GetSectionCount()
{
	int			nCount = 0;
	SECNODE*	pNextSecNode = m_Header.pNextNode;
	while (pNextSecNode)
	{
		nCount++;
		pNextSecNode = pNextSecNode->pNextNode;		
	}
	return nCount;
}

//��ȡ���intֵ
int KIniFile::GetMultiInteger(const char*	lpSection,
					const char* lpKeyName, int *pValues, int nCount)
{
	int	nReadCount = 0;
	if (nCount > 0 && nCount <= INI_MAX_SUPPORT_MULTI_VALUE_COUNT)
	{
		char  Buffer[256];
		if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
		{
			const char *pcszTemp = Buffer;
			do
			{
				pValues[nReadCount++] = KSG_StringGetInt(&pcszTemp, 0);
			}while((nReadCount < nCount) && KSG_StringSkipSymbol(&pcszTemp, ','));
		}
	}
	return nReadCount;
}

//��ȡ���longֵ
int KIniFile::GetMultiLong(const char* lpSection,
					const char*	lpKeyName, long *pValues, int nCount)
{
	int	nReadCount = 0;
	if (nCount > 0 && nCount <= INI_MAX_SUPPORT_MULTI_VALUE_COUNT)
	{
		char  Buffer[384];
		if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
		{
			char* pcszTemp = Buffer;
			do
			{
				char* pSplit = strchr(pcszTemp, ',');
				if (pSplit)
					*pSplit = '\0';
				pValues[nReadCount++] = atol(pcszTemp);
				pcszTemp = pSplit ? (pSplit + 1) : NULL;
			}while((nReadCount < nCount) && pcszTemp);
		}
	}
	return nReadCount;
}

int	KIniFile::GetMultiFloat(const char*	lpSection,
					const char*	lpKeyName, float *pValues, int nCount)
{
	int	nReadCount = 0;
	if (nCount > 0 && nCount <= INI_MAX_SUPPORT_MULTI_VALUE_COUNT)
	{
		char  Buffer[384];
		if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
		{
			char* pcszTemp = Buffer;
			do
			{
				char* pSplit = strchr(pcszTemp, ',');
				if (pSplit)
					*pSplit = '\0';
				pValues[nReadCount++] = (float)atof(pcszTemp);
				pcszTemp = pSplit ? (pSplit + 1) : NULL;
			}while((nReadCount < nCount) && pcszTemp);
		}
	}
	return nReadCount;
}

int	KIniFile::WriteMultiInteger(const char* lpSection,
					const char* lpKeyName, int *pValues, int nCount)
{
	if (nCount > 0 && nCount <= INI_MAX_SUPPORT_MULTI_VALUE_COUNT)
	{
		char Buffer[256];
		int	 nPos = sprintf(Buffer, "%d", *pValues);
		while(--nCount)
		{
			pValues++;
			Buffer[nPos++] = ',';
			nPos += sprintf(Buffer + nPos, "%d", *pValues);
		}
		return SetKeyValue(lpSection, lpKeyName, Buffer);
	}
	return false;
}

int	KIniFile::WriteMultiLong(const char* lpSection,
					const char* lpKeyName, long *pValues, int nCount)
{
	if (nCount > 0 && nCount <= INI_MAX_SUPPORT_MULTI_VALUE_COUNT)
	{
		char	Buffer[384];
		const char*	pFormat = (sizeof(long) == 8) ? "%I64d" : "%d";
		int	 nPos = sprintf(Buffer, pFormat, *pValues);
		while(--nCount)
		{
			pValues++;
			Buffer[nPos++] = ',';
			nPos += sprintf(Buffer + nPos, pFormat, *pValues);
		}
		return SetKeyValue(lpSection, lpKeyName, Buffer);
	}
	return false;
}

int	KIniFile::WriteMultiFloat(const char* lpSection,
					const char* lpKeyName, float *pValues, int nCount)
{
	if (nCount > 0 && nCount <= INI_MAX_SUPPORT_MULTI_VALUE_COUNT)
	{
		char Buffer[384];
		int	 nPos = sprintf(Buffer, "%f", *pValues);
		while(--nCount)
		{
			pValues++;
			Buffer[nPos++] = ',';
			nPos += sprintf(Buffer + nPos, "%f", *pValues);
		}
		return SetKeyValue(lpSection, lpKeyName, Buffer);
	}
	return false;
}

int KIniFile::GetBool(const char* lpSection, const char* lpKeyName, int* pBool)
{
	char	Buffer[16];
	const char*	Value[]=
	{
		"true",	"false",
		"1",	"0",
		"yes",	"no"
	};

	int		nResult = false;
	if (GetKeyValue(lpSection, lpKeyName, Buffer, sizeof(Buffer)))
	{
		for (int i = 0; i < sizeof(Value) / sizeof(const char*); i++)
		{
#ifdef __linux
			if (!strcasecmp(Buffer, Value[i]))
#else
			if (!stricmp(Buffer, Value[i]))
#endif
			{
				*pBool = ((i % 2) == 0);
				nResult = true;
				break;
			}
		}
	}
	return nResult;
}

void KIniFile::Release()
{
	Clear();
	ms_Recycle.Delete(this);
}
