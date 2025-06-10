 /*****************************************************************************************
//  �ļ���/�ַ���������Դ����cache��ʽ����
//	Copyright : Kingsoft 2002-2004
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-11-11
//  Comment   : Դ�� Represent2\ImageStore2
*****************************************************************************************/
#ifndef _ENGINE_OBJCACHE_H_
#define _ENGINE_OBJCACHE_H_

//## ÿ���ļ���/�ַ�����Ӧһ����Դ�����ļ���/�ַ������ڵ�id��ֵ��ӳ�䣬
//## ��ͬid����Դ������cache����Ϊ��ͬһ������ֻ����һ����

//KObjCache�ļ��ض���ص�����
//������pszObjeName --> ������Դ�ļ����� ppObject ���ڻ�ȡ���غ�����ָ��Ļ�����
//����: ��ʾ�ɹ����Ĳ���ֵ
typedef int (*fnObjCacheLoadObjCallback)(const char* pszObjName, void** ppObject);
//KObjCache���Ѽ��ض�������Ļص�����
typedef int	(*fnObjCacheOperOnObjCallback)(void* pObject, void* pParam);

class ENGINE_API KObjCache
{
public:
    KObjCache();
    ~KObjCache();

	//## ��ʼ��KObjCache����,���ü���/ж��/ֹͣ����Ļص�����
    void Init(fnObjCacheLoadObjCallback fnLoad,
			fnObjCacheOperOnObjCallback fnFree,
			fnObjCacheOperOnObjCallback fnIsInsUsing);
    //## ����ͷ�ȫ���Ķ���
    void FreeAllObj();
	//## �ṩ��ȫ���Ķ�����в����Ľӿ�
	void ActionForAllObj(fnObjCacheOperOnObjCallback fnActionForAll, void* pParam);
    //## ���ö�̬����ƽ�������
    void SetBalanceParam(
        //## ���ض������Ŀ��ƽ��ֵ��
        int nNumObj, 
        //## ÿ���ٴ����ö������һ��ƽ���顣
		unsigned int uCheckPoint = 10000);

    //## ��ȡ��Դ����
    void* GetObj(
        //## ��Դ�ļ�����
        const char* pszObj,
        //## id
        unsigned int& uObj,
        //## Objectԭ����Cache�ڵ�λ�á�
        int& nObjPosition);

	//## ָ���ͷ���Դ����
    void FreeObj(
        //## �ļ�����
        const char* pszObj);

private:

    //## �洢����id�Լ�����ָ��Ľṹ
    struct KObj
    {
        //## id
        unsigned int uId;
		//�ڱ�������ѭ�����Ƿ����õ�
		int nRef;
        //## ͼ�����ݶ���ָ�롣
        void* pObject;
    };

private:

	//## ��չm_pObjectList�Ŀռ䣬ʹ�ô洢����ͼ��Դ���ñ�������
    bool ExpandSpace();

	int	 CanBeFree(void *pObject)
		{ return	(!m_fnObjIsInUsing(pObject, 0)); }

	//## ���ͼ��Դ�洢����ƽ��״����
    void CheckBalance();

    //## ��ø���id�ڼ�¼���е�λ�á�
    //## ����ֵ���壺
    //##  ����ֵ >= 0 --> ������id�ڼ�¼���е�λ�ã���0��ʼ������ֵ��
    //##  ����ֵ < 0  --> ����û�и�����id�������Ҫ�����id�Ļ�������λ��Ӧ��Ϊ(-����ֵ-1)
    int FindObj(
        //##Documentation
        //## ͼ�ε�id
        unsigned int uObj, 
        //##Documentation
        //## ͼ�μ�¼��m_pObjectList�п��ܵ�λ�á�
        int nPossiblePosition);

private:

    enum IS_BALANCE_PARAM
	{
        //## ��չm_pObjectList��ÿ�β������ӵ�Ԫ�ص���Ŀ
        ISBP_EXPAND_SPACE_STEP = 64,
        //## ÿ���ٴ����ö������һ��ƽ�����Ĭ��ֵ��
        ISBP_CHECK_POINT_DEF = 512,
        //## ������Ŀƽ���Ĭ��ֵ
		ISBP_BALANCE_NUM_DEF = 256,
        //## ������Χ���Ȳ��ҵķ�Χ��С��
        ISBP_TRY_RANGE_DEF = 8,
	};

private:
    //## ����ͼ�ζ���ṹ�����Ա���������ͼ�ε�id�����������С�
	KObj* m_pObjectList;

    //## m_pObjectList�����а���KObjԪ�ص���Ŀ���ߵ�ǰ��������ɴ洢ͼ����Ϣ����Ŀ��m_pObjectList�ռ��С�ı�ʱͬʱ������ֵ��
    int m_nNumReserved;

    //## �������Ŀ����Щ�������Ϣ�����Ŵ洢��m_pObjectList��ͷ��ʼ��λ�ã�����ͼ�ε�id�������С�
    int m_nNumObjs;

    //## �Զ�������ܴ������ۼӼ�¼ֵ��
    unsigned int m_uObjAccessCounter;

    //## ����ƽ���ʱ�����á�
    unsigned int m_uCheckPoint;

    //## �������ڴ��ж�����Ŀ��ƽ��ֵ
	int m_nBalanceNum;

	fnObjCacheLoadObjCallback		m_fnLoadObj;
	fnObjCacheOperOnObjCallback		m_fnFreeObj;
	fnObjCacheOperOnObjCallback		m_fnObjIsInUsing;
};


#endif //ifndef _ENGINE_OBJCACHE_H_
