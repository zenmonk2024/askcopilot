//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Random Number Generate Functions
//---------------------------------------------------------------------------
#include "Precompile.h"

#define IM 139968
#define IA 3877
#define IC 29573

//---------------------------------------------------------------------------
static unsigned int s_nRandomSeed = 42;
//---------------------------------------------------------------------------
// ����:	RandomnSeed
// ����:	�������������
// ����:	s_nRandomSeed	:	���������
// ����:	void
//---------------------------------------------------------------------------
C_ENGINE_API void g_RandomSeed(unsigned int nSeed)
{
	s_nRandomSeed = nSeed;
}
//---------------------------------------------------------------------------
// ����:	Random
// ����:	����һ��С��nMax���������
// ����:	nMax	:	���ֵ
// ����:	һ��С��nMax�������
//---------------------------------------------------------------------------
C_ENGINE_API unsigned int g_Random(unsigned int nMax)
{
	if (nMax)
	{
		s_nRandomSeed = s_nRandomSeed * IA + IC;
		return s_nRandomSeed % nMax;
	}
	else
	{
		return 0;
	}
}

//---------------------------------------------------------------------------
// ����:	GetRandomSeed
// ����:	ȡ�õ�ʱ��α�������
// ����:	���ص�ǰ��α�������
//---------------------------------------------------------------------------
C_ENGINE_API unsigned int g_GetRandomSeed()
{
	return s_nRandomSeed;
}

