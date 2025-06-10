//---------------------------------------------------------------------------
//	Ԥ����ͷ�ʼ�,Engine2�Ļ�����������ͷ�ʼ�
//	Copyright : Kingsoft Season 2004
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2004-3-21
//---------------------------------------------------------------------------
#ifndef _COMMON_DEF_H_
#define _COMMON_DEF_H_

#include "Ksdef.h"

//-----------------------------------------------------------
//	�������Ͷ���
//-----------------------------------------------------------
/*#ifndef BYTE
	#define BYTE	unsigned char
#endif
#ifndef	WORD
	#define WORD	unsigned short
#endif
#ifndef DWORD
	#define DWORD	unsigned int
#endif
#ifndef	BOOL
	#define BOOL	int
#endif
#ifndef PSTR
	#define	PSTR	char*
#endif
#ifndef PCSTR
	#define	PCSTR	const char*
#endif
#ifndef PBYTE
	#define PBYTE	unsigned char*
#endif
#ifndef PVOID
	#define PVOID	void*
#endif
#ifndef LPVOID
	#define LPVOID	void*
#endif

#ifndef LONG
	#define	LONG	long
#endif

struct	KRECT
{
	int	left; 
	int	top; 
	int	right; 
	int	bottom;
};

//-----------------------------------------------------------
//	������������
//-----------------------------------------------------------
#ifndef NULL
	#define	NULL	0
#endif
#ifndef VOID
	#define	VOID	void
#endif
#ifndef TRUE
	#define TRUE	1
#endif
#ifndef FALSE
	#define FALSE	0
#endif
*/
#ifndef MAX_PATH
	#define MAX_PATH	260
#endif

//-----------------------------------------------------------
//	�����ͷŶ���/�������궨��
//-----------------------------------------------------------
#ifndef SAFE_FREE
	#define SAFE_FREE(a)	if (a) {free(a); (a)=NULL;}
#endif
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(a)	if (a) {(a)->Release(); (a)=NULL;}
#endif
#ifndef SAFE_DELETE
	#define SAFE_DELETE(a)	if (a) {delete (a); (a)=NULL;}
#endif
#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(a)	if (a) {delete[] (a); (a)=NULL;}
#endif

#define COMBO_2SHORT(a, b)	((long)(((short)((a) & 0xffff)) | ((long)((short)((b) & 0xffff))) << 16))
#define LOSHORT(l)			((short)((l) & 0xffff))
#define HISHORT(l)			((short)((l) >> 16))

#define g_StrCpyLen(dest, src, size)	\
		{								\
		strncpy(dest, src, size);		\
		dest[size - 1] = '\0';			\
		}

#define g_StrLower(str)									\
		{												\
			char c;										\
			for (int i = 0; 0 != (c = str[i]); i++)		\
			{											\
				if (c >= 'A' && c <= 'Z')				\
					str[i] = c + ('a' - 'A');			\
			}											\
		}

#endif //_COMMON_DEF_H_
