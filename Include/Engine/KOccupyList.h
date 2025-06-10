//---------------------------------------------------------------------------
// Sword3 Engine (c) 2004 by Kingsoft
// File:	KOccupyList.h
// Date:	2004.08.29
// Code:	Wuyue(wooy)
// Desc:	ռ�������
//---------------------------------------------------------------------------
#ifndef __ENGINE_OCCUPYLIST_H__
#define	__ENGINE_OCCUPYLIST_H__

//���ļ������ʺ��壺
//�  ռ�ñ�Ҫ����Ķ��󼯺ϵ�һ������
//�ڵ㣺ռ�ñ���ڲ��ṹ
//һ���ڵ���һ����һһ��Ӧ����KOccupyListNode::nItemIndexָ��ǰ�ߵ����߶�Ӧ��ϵ��
//��KOccupyListNode::nItemInListIndexָ�����ߵ�ǰ�ߵĶ�Ӧ��ϵ��

//���������⺯�����봫������������������������
//��0����ڵ㣩������ʹ�ò��ɱ�ռ��
//GetNext��GetPre���ڱ���ռ����
//KOccupyList::GetNext(0)��������ȡ�õ�һ��ռ����������
class ENGINE_API KOccupyList
{
public:
	bool			Init(int nSize);
	void			UnInit();

	void			Occupy(int nIdx);	//ռ��ĳ��
	void			Free(int nIdx);		//�ͷ�ĳ��
	void			OccupyAll();		//ȫ����ռ��
	void			FreeAll();			//�ͷ�ȫ����

	int				IsOccupy(int nIdx);				//�ж�ĳ���Ƿ�ռ��
	int				GetFirstFree(int bOccupyIt);	//�õ���һ��δ��ռ�õ��������,���������ʾ�Ƿ�Ҫͬʱռ����
	int				GetOccupyCount() const { return m_nOccupyCount; }	//���ر�ռ�õ������Ŀ

	inline int		GetNext(int nIdx) const			//��ȡ��һ��ռ���������
	{ 
		if (nIdx >= 0 && nIdx < m_nTotalCount)
		{
			int nIndexInList = m_pNodeList[nIdx].nItemInListIndex;
			if (nIndexInList < m_nOccupyCount)
				return m_pNodeList[nIndexInList + 1].nItemIndex;
		}
		return 0;
	}

	inline int		GetPrev(int nIdx) const			//��ȡǰһ��ռ���������
	{
		if (nIdx >= 0 && nIdx < m_nTotalCount)
		{
			int nIndexInList = m_pNodeList[nIdx].nItemInListIndex;
			if (nIndexInList >= 1 && nIndexInList <= m_nOccupyCount)
				return m_pNodeList[nIndexInList - 1].nItemIndex;
		}
		return 0;
	}

	KOccupyList()		{ m_pNodeList = NULL; m_nTotalCount = 0; m_nOccupyCount = 0;}
	~KOccupyList()		{ UnInit(); }

private:
	struct		KOccupyListNode
	{
		int		nItemIndex;				//�ýڵ��Ӧ���������
		int		nItemInListIndex;		//��ýڵ���ͬ���������ڽڵ���е�λ��
	};
	KOccupyListNode*	m_pNodeList;	//�ڵ�����
	int					m_nTotalCount;	//�ڵ�����Ŀ
	int					m_nOccupyCount;	//ռ����Ŀ
};

#endif //__ENGINE_OCCUPYLIST_H__

