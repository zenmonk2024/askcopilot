//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// File:	KPath.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	File Path Utility
//---------------------------------------------------------------------------
#include "Precompile.h"
#ifdef WIN32
	#include <direct.h>
	#include <mbstring.h>
#else
	#include <unistd.h>
	#include <sys/stat.h>
#endif
#include "XPackList.h"

#ifdef LINUX
void strlwr(char* str)
{
	char c;
	for (int i = 0; 0 != (c = str[i]); i++)
	{
		if (c >= 'A' && c <= 'Z')
			str[i] = c + ('a' - 'A');
	}
}

#endif //#ifdef LINUX
//---------------------------------------------------------------------------
#ifdef WIN32
	static char s_szRootPath[MAX_PATH] = "C:";		// ����·��
	static char s_szCurrPath[MAX_PATH] = "\\";		// ��ǰ·��
	#define		PATH_SPLITTER		'\\'
	#define		PATH_SPLITTER_STR	"\\"
#else
	static char s_szRootPath[MAX_PATH] = "/";		// ����·��
	static char s_szCurrPath[MAX_PATH] = "/";		// ��ǰ·��
	#define		PATH_SPLITTER		'/'
	#define		PATH_SPLITTER_STR	"/"
#endif

int RemoveTwoPointPath(char* szPath, int nLength)
{
	int nRemove = 0;
	assert(szPath);

#ifdef WIN32
	const char* lpszOld = "\\..\\";
#else
	const char* lpszOld = "/../";
#endif

	char* lpszTarget = strstr(szPath, lpszOld);

	if (lpszTarget)
	{
		const char* lpszAfter = lpszTarget + 3;
		while(lpszTarget > szPath)
		{
			lpszTarget--;
			if ((*lpszTarget) == '\\' ||(*lpszTarget) == '/')
				break;
		}
		memmove(lpszTarget, lpszAfter, (nLength - (lpszAfter - szPath) + 1) * sizeof(char));
		nRemove = (int)(lpszAfter - lpszTarget);
		return RemoveTwoPointPath(szPath, nLength - nRemove);
	}

	return nLength - nRemove;
}

int RemoveOnePointPath(char* szPath, int nLength)
{
	int nRemove = 0;
	assert(szPath);
#ifdef WIN32
	const char* lpszOld = "\\.\\";
#else
	const char* lpszOld = "/./";
#endif
	char* lpszTarget = strstr(szPath, lpszOld);
	if (lpszTarget)
	{
		char* lpszAfter = lpszTarget + 2;
		memmove(lpszTarget, lpszAfter, (nLength - (lpszAfter - szPath) + 1) * sizeof(char));
		nRemove = (int)(lpszAfter - lpszTarget);
		return RemoveOnePointPath(szPath, nLength - nRemove);
	}

	return nLength - nRemove;
}

int RemoveAllPointPath(char* szPath, int nLength)
{
	return RemoveOnePointPath(szPath, RemoveTwoPointPath(szPath, nLength));
}

//---------------------------------------------------------------------------
// ����:	���ó���ĸ�·��
// ����:	lpPathName	·����
//---------------------------------------------------------------------------
C_ENGINE_API
void g_SetRootPath(const char* lpPathName)
{
	if (lpPathName)
		strcpy(s_szRootPath, lpPathName);
	else
		getcwd(s_szRootPath, MAX_PATH);

	// ȥ��·��ĩβ�� '\'
	int len = (int)strlen(s_szRootPath);
	if (s_szRootPath[len - 1] == '\\' || s_szRootPath[len - 1] == '/')
		s_szRootPath[len - 1] = 0;
}

//---------------------------------------------------------------------------
// ����:	GetRootPath
// ����:	ȡ�ó���ĸ�·��
// ����:	lpPathName	·����
//---------------------------------------------------------------------------
C_ENGINE_API
void g_GetRootPath(char* lpPathName)
{
	strcpy(lpPathName, s_szRootPath);
}
//---------------------------------------------------------------------------
// ����:	SetFilePath
// ����:	���õ�ǰ�ļ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_SetFilePath(const char* lpPathName)
{
	// ȥ��ǰ��� "\\"
	if (lpPathName[0] == '\\' ||lpPathName[0] == '/')
	{
		strcpy(s_szCurrPath, lpPathName + 1);
	}
	else
	{
		strcpy(s_szCurrPath, lpPathName);
	}

	// ĩβ���� "\\"
	int len = (int)strlen(s_szCurrPath);
	if (len > 0 && s_szCurrPath[len - 1] != '\\' && s_szCurrPath[len - 1] != '/')
	{
		s_szCurrPath[len] = PATH_SPLITTER;
		s_szCurrPath[len + 1] = 0;
	}
	RemoveAllPointPath(s_szCurrPath, len + 1);
#ifndef WIN32
	//'\\' -> '/' [wxb 2003-7-29]
	for (len = 0; s_szCurrPath[len]; len++)
	{
		if (s_szCurrPath[len] == '\\')
			s_szCurrPath[len] = '/';
	}
#endif
}

//---------------------------------------------------------------------------
// ����:	GetFilePath
// ����:	ȡ�õ�ǰ�ļ�·��
// ����:	lpPathName	·����
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_GetFilePath(char* lpPathName)
{
	strcpy(lpPathName, s_szCurrPath);
}

//---------------------------------------------------------------------------
// ����:	GetFullPath
// ����:	ȡ���ļ���ȫ·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_GetFullPath(char* lpPathName, const char* lpFileName)
{
#ifdef WIN32
	if (lpFileName[1] == ':' ||	// �ļ�����ȫ·��
		(lpFileName[0] == '\\' && lpFileName[1] == '\\'))// ������'\\'���־�����·��
	{
		strcpy(lpPathName, lpFileName);
		return;
	}
#endif

	// �ļ����в���·��
	if (lpFileName[0] == '\\' || lpFileName[0] == '/')
	{
		strcpy(lpPathName, s_szRootPath);
		strcat(lpPathName, lpFileName);
		return;
	}

	// ��ǰ·��Ϊȫ·��
#ifdef WIN32
	if (s_szCurrPath[1] == ':')
	{
		strcpy(lpPathName, s_szCurrPath);
		strcat(lpPathName, lpFileName);
		return;
	}
#endif
	// ��ǰ·��Ϊ����·��
	strcpy(lpPathName, s_szRootPath);
	if(s_szCurrPath[0] != '\\' && s_szCurrPath[0] != '/')
	{
		strcat(lpPathName, PATH_SPLITTER_STR);
	}
	strcat(lpPathName, s_szCurrPath);

	if (lpFileName[0] == '.' && (lpFileName[1] == '\\'||lpFileName[1] == '/') )
		strcat(lpPathName, lpFileName + 2);
	else
		strcat(lpPathName, lpFileName);
}
//---------------------------------------------------------------------------
// ����:	GetHalfPath
// ����:	ȡ���ļ��İ�·������������·��
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_GetHalfPath(char* lpPathName, const char* lpFileName)
{
	// �ļ����в���·��
	if (lpFileName[0] == '\\' || lpFileName[0] == '/')
	{
		strcpy(lpPathName, lpFileName);
	}
	else
	{
		strcpy(lpPathName, PATH_SPLITTER_STR);
		strcat(lpPathName, s_szCurrPath);
		strcat(lpPathName, lpFileName);
	}
}

//---------------------------------------------------------------------------
// ����:	GetPackPath
// ����:	ȡ���ļ���ѹ�����е�·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_GetPackPath(char* lpPathName, const char* lpFileName)
{
	// �ļ����в���·��
	if (lpFileName[0] == '\\' || lpFileName[0] == '/')
	{
		strcpy(lpPathName, lpFileName + 1);
	}
	else
	{
		strcpy(lpPathName, s_szCurrPath);
		strcat(lpPathName, lpFileName);
	}
	int len = (int)strlen(lpPathName);
	RemoveAllPointPath(lpPathName, len + 1);
	// ȫ��ת��ΪСд��ĸ
	strlwr(lpPathName);
}

//---------------------------------------------------------------------------
// ����:	GetDiskPath
// ����:	ȡ��CDROM��Ӧ���ļ�·����
// ����:	lpPathName	·����
//			lpFileName	�ļ���
// ����:	void
//---------------------------------------------------------------------------
/*ENGINE_API void g_GetDiskPath(char* lpPathName, char* lpFileName)
{
	strcpy(lpPathName, "C:");
	for (int i = 0; i < 24; lpPathName[0]++, i++)
	{
//		if (GetDriveType(lpPathName) == DRIVE_CDROM)
//			break;
	}
	if (lpFileName[0] == '\\' || lpPathName[0] == '/')
	{
		strcat(lpPathName, lpFileName);
	}
	else
	{
#ifdef WIN32
		strcat(lpPathName, "\\");
#else
		strcat(lpPathName, "/");
#endif
		strcat(lpPathName, s_szCurrPath);
		strcat(lpPathName, lpFileName);
	}
}
*/
//---------------------------------------------------------------------------
// ����:	CreatePath
// ����:	����Ϸ��Ŀ¼�½���һ��·��
// ����:	lpPathName	·����
//---------------------------------------------------------------------------
C_ENGINE_API
int	g_CreatePath(const char* lpPathName)
{
	if (!lpPathName || !lpPathName[0])
		return false;

	char szTempPathName[MAX_PATH];
	memset(szTempPathName, 0, sizeof(szTempPathName));
	strncpy(szTempPathName, lpPathName, sizeof(szTempPathName));
	szTempPathName[sizeof(szTempPathName) - 1] = 0;

	char	szFullPath[MAX_PATH];
	int		i;
	if (szTempPathName[0] == '\\' || szTempPathName[0] == '/')    /// ����Ǵ���ʲô�ģ�
	{
		szTempPathName[0] = PATH_SPLITTER;
		if (szTempPathName[1] != '\\')
		{
			strcpy(szFullPath, s_szRootPath);
			strcat(szFullPath, szTempPathName);
			i = (int)strlen(s_szRootPath) + 1;
			
		}
		else
		{
			strcpy(szFullPath, szTempPathName);
			i = 2;
		}
	}
#ifndef LINUX
	else if (szTempPathName[1] == ':')// �ļ�����ȫ·��
	{
		if (strlen(szTempPathName) < 4)
			return false;
		strcpy(szFullPath, szTempPathName);
		i = 4;
	}
#endif
	else
	{
		strcpy(szFullPath, s_szRootPath);
		strcat(szFullPath, PATH_SPLITTER_STR);
		strcat(szFullPath, szTempPathName);
		i = (int)strlen(s_szRootPath) + 1;
	}

	int nResult = true;
	for (; nResult && i < (int)strlen(szFullPath); i++)  /// ѭ���������е�Ŀ¼
	{
		if (szFullPath[i] == '\\' || szFullPath[i] == '/')
		{
			szFullPath[i] = 0;
			#ifdef WIN32
				CreateDirectory(szFullPath, NULL);
			#else
				mkdir(szFullPath, 0777);
			#endif
			szFullPath[i] = PATH_SPLITTER;
		}
	}
	#ifdef WIN32
		CreateDirectory(szFullPath, NULL);
		DWORD dwAtt = GetFileAttributes(szFullPath);
		nResult = ((dwAtt != 0xFFFFFFFF)  && (dwAtt & FILE_ATTRIBUTE_DIRECTORY));
	#else
		mkdir(szFullPath, 0777);
	#endif

	return nResult;
}

//---------------------------------------------------------------------------
// ����:	g_UnitePathAndName
// ����:	һ��·����һ���ļ������ϲ���lpGet���γ�һ��������·���ļ���
// ����:	pcszPath ����·���� pcszFile �����ļ��� pszFullName ��õ����������ļ���
// ����:	void
// ע�⣺   ����û�п����ַ����ĳ��ȣ�ʹ�õ�ʱ��Ҫ��֤�ַ����ĳ����㹻
//---------------------------------------------------------------------------
C_ENGINE_API
void	g_UnitePathAndName(const char *pcszPath, const char *pcszFile, char *pszRetFullName)
{
    if (pszRetFullName)
        pszRetFullName[0] = '\0';
  /**
   *  �����жϾ�û�в��� (pcszPath && pcszPath[0])����ʽ
   */  
	if (
        (!pcszPath) || 
        (!pcszFile) || 
        (!pszRetFullName)
    )
		return;

	strcpy(pszRetFullName, pcszPath);
	int	nSize = (int)strlen(pszRetFullName);
    if (nSize > 0)
    {
	    if (pszRetFullName[nSize - 1] != '\\')
	    {
		    pszRetFullName[nSize] = '\\';
            nSize++;
		    pszRetFullName[nSize] = '\0';
	    }
    }

	if (pcszFile[0] != '\\')
	{
		strcat(pszRetFullName + nSize, pcszFile);
	}
	else
	{
		strcat(pszRetFullName + nSize, &pcszFile[1]);
	}
}

//---------------------------------------------------------------------------
// ����:	find if file exists in pak or in hard disk
// ����:	����ָ�����ļ��Ƿ����
// ����:	lpPathName	·�������ļ���
// ����:	TRUE���ɹ���false��ʧ�ܡ�
//---------------------------------------------------------------------------
C_ENGINE_API
int g_IsFileExist(const char* FileName)
{
	int	bExist = false;
	char	szFullName[MAX_PATH];
	if (FileName && FileName[0])	//�����жϷ�ʽ��������
	{
		//�Ȳ����Ƿ��ڴ���ļ���
		XPackFile::XPackElemFileRef	PackRef;
		bExist = g_EnginePackList.FindElemFile(FileName, PackRef);
		//�ڼ���Ƿ񵥶������ļ�ϵͳ��
		if (bExist == false)
		{
			g_GetFullPath(szFullName, FileName);
			#ifdef	WIN32
				bExist = !(GetFileAttributes(szFullName) & FILE_ATTRIBUTE_DIRECTORY);// || dword == INVALID_FILE_ATTRIBUTES) ����������ļ��У�
			#else
//				bExist = _sopen(szFullName, _O_BINARY, _SH_DENYNO, 0);
//				if (bExist != -1)
//				{
//					_close(bExist);
//					bExist = true;
//				}
//				else
//				{
//					bExist = false;
//				}
			#endif
		}
	}
	return bExist;
}

//---------------------------------------------------------------------------
// ����:	String to 32bit Id
// ����:	�ļ���ת���� Hash 32bit ID
// ����:	lpFileName	�ļ���
// ����:	FileName Hash 32bit ID
// ע��:	��Ϸ����������ؽ����������õĹ�ϣ��������Ҳ����
//			������������������޸��������ʱҲ��Ӧ�޸�������
//			�����Ӧ���Ǹ��������������������Common.lib���̵�Utils.h
//			�У���������Ϊ unsigned int HashStr2ID( const char * const pStr );
//---------------------------------------------------------------------------
C_ENGINE_API
unsigned int g_StringHash(const char* pString)
{
	unsigned int Id = 0;
	char c = 0;
	for (int i = 0; pString[i]; i++)
	{
		c = pString[i];
		Id = (Id + (i + 1) * c) % 0x8000000b * 0xffffffef;
	}
	return (Id ^ 0x12345678);
}

//��һ���ַ���Сд������תΪhash��ֵ
C_ENGINE_API	unsigned int	g_StringLowerHash(const char* pString)
{
	unsigned int Id = 0;
	char c = 0;
	for (int i = 0; pString[i]; i++)
	{
		c = pString[i];
		if (c >= 'A' && c <= 'Z')
			c += 0x20;	//�����ַ��ĺ��ֽ�����ܱ�ת������Ϊ�ؼӴ�������ĸ��ʡ�
		Id = (Id + (i + 1) * c) % 0x8000000b * 0xffffffef;
	}
	return (Id ^ 0x12345678);
}

//��һ���ַ���Сд������תΪhash��ֵ
C_ENGINE_API	unsigned int	g_FileNameHash(const char* pString)
{
	unsigned int Id = 0;
	char c = 0;
	for (int i = 0; pString[i]; i++)
	{
		c = pString[i];
		if (c >= 'A' && c <= 'Z')
			c += 0x20;	//�����ַ��ĺ��ֽ�����ܱ�ת������Ϊ�ؼӴ�������ĸ��ʡ�
		else if (c == '/')
			c = '\\';
		Id = (Id + (i + 1) * c) % 0x8000000b * 0xffffffef;
	}
	return (Id ^ 0x12345678);
}

//---------------------------------------------------------------------------
// ����:	change file extention
// ����:	�ı��ļ�����չ��
// ����:	lpFileName	�ļ���
//			lpNewExt	����չ����������'.'
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_ChangeFileExt(char* lpFileName, const char* lpNewExt)
{
	char* pDot = strrchr(lpFileName, '.');
	char* pPathSplit = strrchr(lpFileName, PATH_SPLITTER);

	if (pDot && pPathSplit < pDot)
	{
		strcpy(pDot + 1, lpNewExt);
	}
	else
	{          /// ��������� xxx./yyy???
		strcat(lpFileName, ".");
		strcat(lpFileName, lpNewExt);
	}
}

//---------------------------------------------------------------------------
// ����:	Extract File Name from path name
// ����:	ȡ���ļ�����������·����
// ����:	lpFileName	�ļ�����������·����
//			lpFilePath	�ļ���������·����
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_ExtractFileName(char* lpFileName, const char* lpFilePath)
{
	int nPos = (int)strlen(lpFilePath);
//	if (nPos < 5)
//		return;
	while ((--nPos) >= 0)
	{
		if (lpFilePath[nPos] == '\\' || lpFilePath[nPos] == '/')
			break;
	}
	strcpy(lpFileName, &lpFilePath[nPos + 1]);
}

//---------------------------------------------------------------------------
// ����:	Extract File Path from path name
// ����:	ȡ��·����
// ����:	lpFileName	·����
//			lpFilePath	·�������ļ���
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
void g_ExtractFilePath(char* lpPathName, const char* lpFilePath)
{
	int nPos = (int)strlen(lpFilePath);
//	if (nLen < 5)
//		return;
//	int nPos = nLen;
	while ((--nPos) > 0)
	{
		if (lpFilePath[nPos] == '\\' ||lpFilePath[nPos] == '/')
			break;
	}
	if (nPos > 0)
	{
		memcpy(lpPathName, lpFilePath, nPos);
		lpPathName[nPos] = 0;
	}
	else
	{
		lpPathName[0] = 0;
	}
}
//---------------------------------------------------------------------------
// ����:	Get File Path from full path name
// ����:	��ȫ·����ȡ�����·����
// ����:	lpPathName		���·����
//			lpFullFilePath	ȫ·����
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API
BOOL g_GetFilePathFromFullPath(char* lpPathName, const char* lpFullFilePath)
{
	if (!lpPathName || !lpFullFilePath)
		return FALSE;

	int nRootSize = strlen(s_szRootPath);
	int nFullSize = strlen(lpFullFilePath);

	if (nFullSize <= nRootSize)
		return FALSE;

	memcpy(lpPathName, lpFullFilePath, nRootSize);
	lpPathName[nRootSize] = 0;

	if (stricmp(lpPathName, s_szRootPath))
		return FALSE;

	//������·�����"\\"
	memcpy(lpPathName, &lpFullFilePath[nRootSize + 1], nFullSize - nRootSize - 1);
	lpPathName[nFullSize - nRootSize - 1] = 0;

	return TRUE;
}

