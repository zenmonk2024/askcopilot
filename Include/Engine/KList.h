//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// File:	KList.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef	_ENGINE_KLLIST_H_
#define	_ENGINE_KLLIST_H_

//---------------------------------------------------------------------------
#include "KNode.h"
//---------------------------------------------------------------------------

class ENGINE_API KList
{
public:
	KList();
	KNode*	GetHead()	{ return m_pListHead; }
	KNode*	GetTail()	{ return m_pListTail; }
	void	AddHead(KNode *pNode); // ����ǰ������һ���ڵ�
	void	AddTail(KNode *pNode); // �����������һ���ڵ�
	KNode*	RemoveHead();	// ɾ����һ���ڵ�
	KNode*	RemoveTail();	// ɾ�����һ���ڵ�
	int		IsEmpty();		// �Ƿ��Ǹ��յ�����
	long	GetNodeCount();
private:
	friend class KNode;

	KNode*	m_pListHead;	// ͷ�ڵ�
	KNode*	m_pListTail;
	int		m_nodeCount;
};

#endif	//_ENGINE_KLLIST_H_
