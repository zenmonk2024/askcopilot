/********************************************************************
File        : LinkStructEx.h
Creator     : Fyt(Fan Zhanpeng)
create data : 01-17-2003(mm-dd-yyyy)
Description : һά�����ģ���������
*********************************************************************/
//////////////////////////////////////////////////////////////////////
//By Fyt(Fan Zhanpeng)
//*****************************************************************************
//Function List:
//Count() Add()   Get()	  Current() Insert() Remove() Next()
//Prev()  Reset() Begin() End()     Clear()  LeaveAlone()
//*****************************************************************************
//Description  :

#ifndef _ENGINE_LINKSTRUCTEX_H_
#define _ENGINE_LINKSTRUCTEX_H_

#include "KCodePoints.h"

//#define NULL 0

template <class T> class KLinkStructEx
{
public:
	struct SLINK_STRUCT
	{
		T* pThis;
		SLINK_STRUCT* pNext;
		SLINK_STRUCT* pPrevious;
	};

public:
	KLinkStructEx()
	{
		m_nCount = 0;
		m_pEndItem = NULL;
		m_pBeginItem = NULL;
		m_pCurrentItem = NULL;
	}


	virtual ~KLinkStructEx()
	{
		Clear();
	}

	int	Count()
	{
		return m_nCount;
	}

    //������ĩβ���һ��Ԫ��
	T*	Add(T *pIt)
	{
		if(!m_pBeginItem)
		{
			m_pBeginItem = new SLINK_STRUCT;
			ADD_CODEPOINT(m_pBeginItem);
			if(m_pBeginItem)
			{
				m_pBeginItem->pThis = pIt;
				m_pBeginItem->pNext = NULL;
				m_pBeginItem->pPrevious = NULL;

				m_nCount = 1;
				m_pEndItem = m_pBeginItem;
				m_pCurrentItem = m_pBeginItem;
			}
		}
		else if(m_pBeginItem == m_pEndItem)
		{
			m_pEndItem = new SLINK_STRUCT;
			ADD_CODEPOINT(m_pEndItem);
			if(m_pEndItem)
			{
				m_pEndItem->pThis = pIt;
				m_pBeginItem->pNext = m_pEndItem;
				m_pEndItem->pPrevious = m_pBeginItem;
				
				m_nCount ++;
				m_pEndItem->pNext = NULL;
				m_pCurrentItem = m_pEndItem;
			}
			else
			{
				m_pEndItem = m_pBeginItem;
			}
		}
		else
		{
			m_pCurrentItem = m_pEndItem;
			m_pEndItem = new SLINK_STRUCT;
			ADD_CODEPOINT(m_pEndItem);
			if(m_pEndItem)
			{
				m_pEndItem->pThis = pIt;
				m_pCurrentItem->pNext = m_pEndItem;
				m_pEndItem->pPrevious = m_pCurrentItem;
				
				m_nCount++;
				m_pEndItem->pNext = NULL;
				m_pCurrentItem = m_pEndItem;
			}
			else
			{
				m_pEndItem = m_pCurrentItem;
			}
		}
		return m_pEndItem->pThis;
	}

	//��ȡ��nWhich(0��,�±�)��Ԫ��
	T*	Get(int nWhich)
	{
		if(!m_nCount)
		{
			return NULL;
		}
		else if(nWhich >= m_nCount)
		{
			m_pCurrentItem = m_pEndItem;
		}
		else if(nWhich < 0)
		{
			m_pCurrentItem = m_pBeginItem;
		}
		else
		{
			Reset();
			for(int i = 0;i < nWhich;i++)
			{
				Next();
			}
		}
		return m_pCurrentItem->pThis;
	}

	//���ص�ǰ�����Ԫ��
	T*	Current()
	{
		if(m_pCurrentItem)
		{
			return m_pCurrentItem->pThis;
		}
		else
		{
			return NULL;
		}
	}

        //��nWhereλ�ò���һ��pItԪ��
	T*	Insert(int nWhere, T* pIt)
	{
		Get(nWhere);
		return Insert(pIt);
	}

	//�ڵ�ǰ����Ԫ��ǰ����һ��pItԪ��
	T*	Insert(T* pIt)
	{
		if(!m_pBeginItem)
		{
			Add(pIt);
		}
		else if(m_pCurrentItem == m_pBeginItem)
		{
			m_pBeginItem = new SLINK_STRUCT;
			ADD_CODEPOINT(m_pBeginItem);
			if(m_pBeginItem)
			{
				m_pBeginItem->pThis = pIt;
				m_pBeginItem->pPrevious = NULL;
				m_pBeginItem->pNext = m_pCurrentItem;
				m_pCurrentItem->pPrevious = m_pBeginItem;
				m_pCurrentItem = m_pBeginItem;
			}
			else
			{
				m_pBeginItem = m_pCurrentItem;
			}
		}
		else
		{
			SLINK_STRUCT *pTemp;

			pTemp = new SLINK_STRUCT;
			ADD_CODEPOINT(pTemp);
			if(pTemp)
			{
				pTemp->pThis = pIt;

				pTemp->pPrevious = m_pCurrentItem->pPrevious;
				pTemp->pNext = m_pCurrentItem;
				
				m_pCurrentItem->pPrevious->pNext = pTemp;
				m_pCurrentItem->pPrevious = pTemp;
				m_pCurrentItem = pTemp;
			}
		}
		return m_pCurrentItem->pThis;
	}

	//ɾ����nWhich��Ԫ��
	int Remove(int nWhich)
	{
		Get(nWhich);
		return Remove();
	}

	//ɾ����ǰ����Ԫ��
	int Remove()
	{
		T *pRemove;
		if(pRemove = LeaveAlong())
		{
			DEL_CODEPOINT(pRemove);
			delete pRemove;
		}
		return m_nCount;
	}

	T* LeaveAlong()
	{
		T *pTemp;
		SLINK_STRUCT *pDelete;
		if(!m_pCurrentItem)
		{
			return 0;
		}
		if(m_pCurrentItem->pPrevious)
		{
			m_pCurrentItem->pPrevious->pNext = m_pCurrentItem->pNext;
		}
		else
		{
			m_pBeginItem = m_pCurrentItem->pNext;
		}
		if(m_pCurrentItem->pNext)
		{
			m_pCurrentItem->pNext->pPrevious = m_pCurrentItem->pPrevious;
		}
		else
		{
			m_pEndItem = m_pCurrentItem->pPrevious;
		}
		pTemp = m_pCurrentItem->pThis;
		pDelete = m_pCurrentItem;
		m_pCurrentItem = m_pCurrentItem->pNext;
		DEL_CODEPOINT(pDelete);
		delete pDelete;
		m_nCount--;
		return pTemp;
	}

    //�ѽ����Ƶ���һ��Ԫ��
	T*  Next()
	{
		if(!m_pCurrentItem)
		{
			return NULL;
		}
		m_pCurrentItem = m_pCurrentItem->pNext;
		if(m_pCurrentItem)
		{
			return m_pCurrentItem->pThis;
		}
		else
		{
			return NULL;
		}
	}

	//�ѽ����Ƶ���һ��Ԫ��
	T*  Prev()
	{
		if(!m_pCurrentItem)
		{
			return NULL;
		}
		m_pCurrentItem = m_pCurrentItem->pPrevious;
		if(m_pCurrentItem)
		{
			return m_pCurrentItem->pThis;
		}
		else
		{
			return NULL;
		}
	}

	//�ѽ����Ƶ���һ��Ԫ��
	T*  Reset()
	{
		m_pCurrentItem = m_pBeginItem;
		if(m_pCurrentItem)
		{
			return m_pCurrentItem->pThis;
		}
		else
		{
			return NULL;
		}
	}

	//�ѽ����Ƶ���һ��Ԫ��
	T*  Begin()
	{
		return Reset();
	}

	//�ѽ����Ƶ����һ��Ԫ��
	T*  End()
	{
		m_pCurrentItem = m_pEndItem;
		if(m_pCurrentItem)
		{
			return m_pCurrentItem->pThis;
		}
		else
		{
			return NULL;
		}
	}

	//�����������
    void Clear()
    {
        Reset();
        while(Remove() != NULL);
    }

	//�ѵ�ǰԪ������һ��λ��
	void MoveUp()
	{
		if(m_pCurrentItem != NULL)
		{
			if(m_pCurrentItem != m_pBeginItem)
			{
				SLINK_STRUCT* pLast = m_pCurrentItem->pPrevious;
				SLINK_STRUCT* pLastLast = pLast->pPrevious;
				SLINK_STRUCT* pNext = m_pCurrentItem->pNext;

				if(m_pEndItem == m_pCurrentItem)
				{
					m_pEndItem = pLast;
				}
				if(m_pBeginItem == pLast)
				{
					m_pBeginItem = m_pCurrentItem;
				}

				m_pCurrentItem->pNext = pLast;
				m_pCurrentItem->pPrevious = pLastLast;

				pLast->pPrevious = m_pCurrentItem;
				pLast->pNext = pNext;

				if(pLastLast)
				{
					pLastLast->pNext = m_pCurrentItem;
				}
				if(pNext)
				{
					pNext->pPrevious = pLast;
				}
			}
		}
	}

	//�ѵ�ǰԪ������һ��λ��
	void MoveDown()
	{
		if(m_pCurrentItem != NULL)
		{
			if(m_pCurrentItem != m_pEndItem)
			{
				SLINK_STRUCT* pNext = m_pCurrentItem->pNext;
				SLINK_STRUCT* pNextNext = pNext->pNext;
				SLINK_STRUCT* pLast = m_pCurrentItem->pPrevious;

				if(m_pEndItem == pNext)
				{
					m_pEndItem = m_pCurrentItem;
				}
				if(m_pBeginItem == m_pCurrentItem)
				{
					m_pBeginItem = pNext;
				}

				m_pCurrentItem->pNext = pNextNext;
				m_pCurrentItem->pPrevious = pNext;

				pNext->pPrevious = pLast;
				pNext->pNext = m_pCurrentItem;

				if(pNextNext)
				{
					pNextNext->pPrevious = m_pCurrentItem;
				}
				if(pLast)
				{
					pLast->pNext = pNext;
				}
			}
		}
	}

public: //����������
	T* operator++()
	{
		return Next();
	}

	T* operator--()
	{
		return Prev();
	}

private:
    int  m_nCount;
    SLINK_STRUCT *m_pBeginItem;
    SLINK_STRUCT *m_pEndItem;
    SLINK_STRUCT *m_pCurrentItem;
};

#endif // !defined _ENGINE_LINKSTRUCTEX_H_
