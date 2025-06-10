//---------------------------------------------------------------------------
// Sword Engine (c) 2004 by Kingsoft
// Date:	2004.08.17
// Code:	wooy
//---------------------------------------------------------------------------
#ifndef _ENGINE_AUTOFREEPTR_H_
#define _ENGINE_AUTOFREEPTR_H_

//�����Զ��ͷŵ�bufferָ��
class KSmartFreePtr
{
public:
	KSmartFreePtr()
	{
		m_pPtr = 0;
	}
	~KSmartFreePtr()
	{
		Free();
	}
	void Free()
	{
		if (m_pPtr)
		{
			free(m_pPtr);
			m_pPtr = NULL;
		}
	}
	void* operator = (void* pPtr)
	{
		Free();
		return (m_pPtr = pPtr);
	}
	void* operator () ()
	{
		return m_pPtr;
	}
	operator void*()
	{
		return (m_pPtr);
	}
private:
	void*	m_pPtr;
};


//�����Զ��ͷŵ�IIniFileָ��
template <class PtrType> class KSmartReleasePtr
{
public:
	KSmartReleasePtr()
	{
		m_pPtr = 0;
	}
	~KSmartReleasePtr()
	{
		Release();
	}
	void Release()
	{
		if (m_pPtr)
		{
			m_pPtr->Release();
			m_pPtr = NULL;
		}
	}
	PtrType* operator = (PtrType* pPtr)
	{
		Release();
		return (m_pPtr = pPtr);
	}
	PtrType* operator -> ()	// �������أ�
	{
		return m_pPtr;
	}
	PtrType* operator & ()
	{
		return m_pPtr;
	}
	operator bool()
	{
		return (m_pPtr != 0);
	}
private:
	PtrType*	m_pPtr;
};

struct IFile;
struct IIniFile;
struct ITabFile;
typedef KSmartReleasePtr<IFile>	KSmartFile;
typedef KSmartReleasePtr<IIniFile>	KSmartIniFile;
typedef KSmartReleasePtr<ITabFile>	KSmartTabFile;

#endif //#ifndef _ENGINE_AUTOFREEPTR_H_

