#include "Precompile.h"
#include "KList.h"
//---------------------------------------------------------------------------
// ����:	KList
// ����:	����
KList::KList()
{
	m_nodeCount = 0;
	m_pListHead = NULL;
	m_pListTail = NULL;
}

//-------------------------------------------------------------------------
// ����:	IsEmpty
// ����:	�Ƿ�Ϊ��
// ����:	int
int KList::IsEmpty()
{
	return m_pListHead == NULL;
}

//-------------------------------------------------------------------------
// ����:	AddHead
// ����:	��ͷ������һ���ڵ�
// ����:	KNode*
// ����:	int
//---------------------------------------------------------------------------
void KList::AddHead(KNode *pNode)
{
	KASSERT(pNode && !pNode->IsLinked());
	if (m_pListHead)
	{
		m_pListHead->m_pPrev = pNode;
	}
	pNode->m_pList = this;
	pNode->m_pPrev = NULL;
	pNode->m_pNext = m_pListHead;
	m_pListHead = pNode;

	if (!m_pListTail)
	{
		m_pListTail = pNode;
	}
	++m_nodeCount;
}

//-------------------------------------------------------------------------
// ����:	AddTail
// ����:	��ĩβ����һ���ڵ�
// ����:	KNode*
//---------------------------------------------------------------------------
void KList::AddTail(KNode *pNode)
{
	KASSERT(pNode && !pNode->IsLinked());
	if (m_pListTail)
	{
		m_pListTail->m_pNext = pNode;
	}
	pNode->m_pList = this;
	pNode->m_pPrev = m_pListTail;
	pNode->m_pNext = NULL;
	m_pListTail = pNode;

	if (!m_pListHead)
	{
		m_pListHead = pNode;
	}
	++m_nodeCount;
}

//-------------------------------------------------------------------------
// ����:	RemoveHead
// ����:	ɾ����һ���ڵ�
// ����:	KNode*
//---------------------------------------------------------------------------
KNode* KList::RemoveHead()
{
	KNode* pRetNode = m_pListHead;
	if (m_pListHead && (m_pListHead == m_pListTail))
	{
		m_pListHead = m_pListTail = NULL;
		--m_nodeCount;
	}
	else if (m_pListHead)
	{
		m_pListHead = m_pListHead->GetNext();
		m_pListHead->m_pPrev = NULL;
		--m_nodeCount;
	}
	if (pRetNode)
	{
		pRetNode->m_pList = NULL;
		pRetNode->m_pNext = NULL;
		pRetNode->m_pPrev = NULL;
	}
	return pRetNode;
}

//-------------------------------------------------------------------------
// ����:	RemoveTail
// ����:	ɾ�����һ���ڵ�
// ����:	KNode*
//---------------------------------------------------------------------------
KNode* KList::RemoveTail()
{
	KNode* pRetNode = m_pListTail;
	if (m_pListTail && (m_pListHead == m_pListTail))
	{
		m_pListHead = m_pListTail = NULL;
		--m_nodeCount;
	}
	else if (m_pListTail)
	{
		m_pListTail = m_pListTail->GetPrev();
		m_pListTail->m_pNext = NULL;
		--m_nodeCount;
	}
	if (pRetNode)
	{
		pRetNode->m_pList = NULL;
		pRetNode->m_pNext = NULL;
		pRetNode->m_pPrev = NULL;
	}
	return pRetNode;
}

//-------------------------------------------------------------------------
// ����:	GetNodeCount
// ����:	ȡ�ýڵ����
// ����:	long
//---------------------------------------------------------------------------
long KList::GetNodeCount()
{
	return m_nodeCount;
}
//-------------------------------------------------------------------------
