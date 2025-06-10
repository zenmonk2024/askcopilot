// -------------------------------------------------------------------------
//	�ļ�����	��	�ļ����棨������Ini��TabFile����ʵ����Load(szFilePath)���ļ��ࣩ
//	������		��	Fanghao_Wu
//	����ʱ��	��	2005-04-12
// -------------------------------------------------------------------------

#ifndef KFILECACHE_H
#define KFILECACHE_H

#include <string>
#include <map>

template < class T >
class KFileCache
{
protected:
	struct KFileNode
	{
		T*			m_pFile;		// �ļ�ָ��
		int			m_nRef;			// �ļ����ü���
	};

	typedef std::map< std::string, KFileNode > MAPFILESET;
	MAPFILESET m_mpFileSet;			// �����ļ���

public:
	~KFileCache()
	{
		UnloadAll();
	}

public:
	T* Load(char* szFileName);		// ����ָ���ļ���ָ���ļ����ü�����1
	bool Unload(char* szFileName);	// ж��ָ���ļ���ָ���ļ����ü�����1������Ϊ0ʱ���ļ������г������
	bool UnloadAll();				// �������ü�����ȫ�����ļ������г������
	T* Open(char* szFileName);		// ��ʹ��ָ���ļ�
};

// ����ָ���ļ���ָ���ļ����ü�����1
template < class T >
T* KFileCache< T >::Load(char* szFileName)
{
	if (!szFileName || !szFileName[0])
		return NULL;

	typename MAPFILESET::iterator it = m_mpFileSet.find(szFileName);
	if (it != m_mpFileSet.end())
	{
		// �ļ��Ѵ���
		it->second.m_nRef++;
		return it->second.m_pFile;
	}

	
	T* pFile = NULL;
	g_OpenFile(&pFile, szFileName);
	if (!pFile)
		return false;

	KFileNode fileNode;
	fileNode.m_pFile = pFile;
	fileNode.m_nRef = 1;

	m_mpFileSet[szFileName] = fileNode;

	return pFile;
}

// ж��ָ���ļ���ָ���ļ����ü�����1������Ϊ0ʱ���ļ������г������
template < class T >
bool KFileCache< T >::Unload(char* szFileName)
{
	typename MAPFILESET::iterator it = m_mpFileSet.find(szFileName);

	if (it == m_mpFileSet.end())
		return false;

	it->second.m_nRef--;

	if (it->second.m_nRef == 0)
	{
		SAFE_RELEASE(it->second.m_pFile);
		m_mpFileSet.erase(it);
	}

	return true;
}

// �������ü�����ȫ�����ļ������г������
template < class T >
bool KFileCache< T >::UnloadAll()
{
	typename MAPFILESET::iterator it;
	for (it = m_mpFileSet.begin(); it != m_mpFileSet.end(); it++)
	{
		SAFE_RELEASE(it->second.m_pFile);
	}
	m_mpFileSet.clear();
	return true;
}

// ��ʹ��ָ���ļ�
template < class T >
T* KFileCache< T >::Open(char* szFileName)
{
	typename MAPFILESET::iterator it = m_mpFileSet.find(szFileName);

	if (it != m_mpFileSet.end())
		return it->second.m_pFile;

	return NULL;
}

#endif	// #ifndef KFILECACHE_H
