#include "Precompile.h"
#include "KNode.h"
#include "KList.h"
//---------------------------------------------------------------------------
// ����:	Knode
// ����:	����
//---------------------------------------------------------------------------
KNode::KNode()
{
	m_pNext = NULL;
	m_pPrev = NULL;
	m_pList = NULL;
}

//----------------------------------------------------------------------------
// ����:	InsertBefore
// ����:	��ǰ�����һ�����
// ����:	KNode*
//---------------------------------------------------------------------------
void KNode::InsertBefore(KNode *pNode)
{
	KASSERT(pNode && !pNode->IsLinked());
	pNode->m_pList = m_pList;
	pNode->m_pPrev = m_pPrev;
	pNode->m_pNext = this;
	if (m_pPrev)
	{
		m_pPrev->m_pNext = pNode;
	}
	if (m_pList)
	{
		if (m_pList->m_pListHead == this)
			m_pList->m_pListHead = pNode;
		++m_pList->m_nodeCount;
	}
	m_pPrev = pNode;
}

//---------------------------------------------------------------------------
// ����:	InsertAfter
// ����:	�ں������һ�����
// ����:	KNode*
//---------------------------------------------------------------------------
void KNode::InsertAfter(KNode *pNode)
{
	KASSERT(pNode && !pNode->IsLinked());
	pNode->m_pList = m_pList;
	pNode->m_pPrev = this;
	pNode->m_pNext = m_pNext;
	if (m_pNext)
	{
		m_pNext->m_pPrev = pNode;
	}
	if (m_pList)
	{
		if (m_pList->m_pListTail == this)
			m_pList->m_pListTail = pNode;
		++m_pList->m_nodeCount;
	}
	m_pNext = pNode;
}

//---------------------------------------------------------------------------
// ����:	Remove the node
// ����:	ɾ��������
// ����:	KNode*
//---------------------------------------------------------------------------
void KNode::Remove()
{
	if (m_pList && m_pList->m_pListHead == this)
	{
		m_pList->RemoveHead();
	}
	else if (m_pList && m_pList->m_pListTail == this)
	{
		m_pList->RemoveTail();
	}
	else
	{
		if (m_pList) --m_pList->m_nodeCount;
		if (m_pPrev) m_pPrev->m_pNext = m_pNext;
		if (m_pNext) m_pNext->m_pPrev = m_pPrev;
	}
	m_pPrev = NULL;
	m_pNext = NULL;
	m_pList = NULL;
}

//---------------------------------------------------------------------------
// ����:	IsLinked
// ����:	����ڵ��Ƿ�����
// ����:	bool
//---------------------------------------------------------------------------
int KNode::IsLinked()
{
	return (m_pList || m_pNext || m_pPrev);
}
//--------------------------------------------------------------------------------
