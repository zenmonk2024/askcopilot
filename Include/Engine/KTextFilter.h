// ���ֹ��˵�ͨ����汾: Zhao chunfeng, yanrui
// 1. ���й��˲����ִ�Сд
// 2. ͨ���'?'��ʾƥ��0����1�������ַ�
// 3. ͨ���'*'��ʾƥ��0�����������ַ�
// 4. ���˹���,ÿ�����32�ֽ�(����β��'\0')
// 5. ���˴��������,һ�����1024�ֽ�(����β��'\0')
// 6. ���˹���ʽ,�����˵��ı�,������ΪAnsi����

#ifndef _KTEXTFILTER_H_
#define _KTEXTFILTER_H_

#include "EngineBase.h"
#include "KSUnknown.h"

struct ITextFilter : IUnknown
{
    virtual BOOL LoadFilterFile(const char cszFilterFile[]) = 0;

    virtual BOOL AddFilterText(const char cszFilterText[]) = 0;

    virtual void ClearAll() = 0;

    // Check ����ֵ:
    // true  - �ַ�����OK��
    // false - �ַ����޷�ʶ�𣬻���̫���޷��������߰����˷Ƿ��ʻ�
    virtual BOOL Check(char* pszTextAnsi) = 0;

    // Replace ����ֵ:
    // true  - �ַ����滻�ɹ������������滻
    // false - �ַ����޷�ʶ�𣬻���̫���޷�����
    virtual BOOL Replace(char* pszTextAnsi) = 0;
};

ENGINE_API ITextFilter* CreateTextFilterInterface();

#endif
