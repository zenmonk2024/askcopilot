//---------------------------------------------------------------------------
//	���ִ�����
//	Copyright : Kingsoft 2002-2007
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-31
//---------------------------------------------------------------------------
#ifndef _ENGINE_TEXT_H_
#define _ENGINE_TEXT_H_

enum LOCALIZATION_CHARACTER_SET
{
	LOCALIZATION_CHARACTER_SET_GBK		= 0,	//GBK����
	LOCALIZATION_CHARACTER_SET_BIG5		= 1,	//BIG5����
	LOCALIZATION_CHARACTER_SET_ENGLISH	= 2,	//Ӣ�ı���
	LOCALIZATION_CHARACTER_SET_VIETNAM	= 3,	//Խ���ı���
};

//�ַ�����������ڲ�����
enum KTEXT_CTRL_CODE
{
	KTC_INVALID			=	0,
	KTC_ENTER			=	0x0a,
	KTC_COLOR			=	0x02,	//�����������BYTE���ݷֱ�ΪRGB��ɫ����
	KTC_COLOR_RESTORE	=	0x03,	//�ظ���ԭ�����õ���ɫ
	KTC_BORDER_COLOR	=	0x04,	//���ñ�Եɫ�������������BYTE���ݷֱ�ΪRGB��ɫ����
	KTC_BORDER_RESTORE	=	0x05,	//���ñ�Եɫ��ԭ
	KTC_INLINE_PIC		=	0x06,	//Ƕ��ʽͼƬ[wxb 2003-6-19]
	KTC_TAB				=	0x09,	//tab
	KTC_SPACE			=	0x20,	//�ո�
	KTC_TAIL			=	0xFF,	//�ַ�������
};

struct KTP_CTRL
{
	unsigned char	cCtrl;	//�˳�ԱȡֵΪText.h��÷��KTEXT_CTRL_CODE�е�һ��ֵ
	union
	{
	    struct
	    {
   			unsigned char cParam0;	//�˼�����������Ա��ȡֵ�뺬������cCtrl��ȡֵ����
		    unsigned char cParam1;
		    unsigned char cParam2;
	    };
	    unsigned short wParam;
   };
};

struct IInlinePicEngineSink
{
	enum INLINE_PIC_ENGINE_SINK_PARAM
	{
		MAX_SYSTEM_INLINE_PICTURES	= 4096,	//ϵͳԤ����Ƕ��ʽͼƬ����
	};

	//��ȡָ��Ƕ��ͼƬ�Ĵ�С,���ز���ֵ
	virtual int GetPicSize(unsigned short wIndex, int& cx, int& cy) = 0;
	//����ָ��ͼƬ,���ز���ֵ
	virtual int DrawPic(unsigned short wIndex, int x, int y, int nColor, int nStretchPercent = 100) = 0;
	//��̬����ͼƬ,��ȡһ��WORD,��ͼƬ������,���ز���ֵ
	virtual int AddCustomInlinePic(unsigned short& wIndex, const char* szSprPathName) = 0;
	//��̬ж��ͼƬ,���ز���ֵ
	virtual int RemoveCustomInlinePic(unsigned short wIndex) = 0;
	//�ͷŽӿڶ���
	virtual void Release() = 0;
};

#ifndef ENGINE_EXPORTS

//�ӿ� IInlinePicEngineSink ��Ӧ�ò�ʵ�ֲ��ҽӽ� Engine ģ�� [wxb 2003-6-19]
//��عҽӺ���:
// AdviseEngine(IInlinePicEngineSink*);
// UnAdviseEngine(IInlinePicEngineSink*);
extern "C"
{
	//���ñ����ַ���
	void LOC_SetLocalCharacterSet(LOCALIZATION_CHARACTER_SET LocCharacterSet);
	//ȡ�ñ����ַ���
	LOCALIZATION_CHARACTER_SET  LOC_GetLocalCharacterSet();
	//��ȡ���е��¸���ʾ�ַ�
	const char* TGetSecondVisibleCharacterThisLine(const char* pCharacter, int nPos, int nLen);
	//���ĳ���ַ��Ƿ�Ϊ����������׵��ַ������������ַ��򷵻�0�����򷵻��ַ�ռ���ӽ���
	int TIsCharacterNotAlowAtLineHead(const char* pCharacter);
	//���ԭ�ַ������ȣ�������β���������޶��ĳ��ȣ���ض���������..��׺
	const char* TGetLimitLenString(const char* pOrigString, int nOrigLen, char* pLimitLenString, int nLimitLen);
	//���ԭ(�������Ʒ�)�ַ������ȣ�������β���������޶��ĳ��ȣ���ض���������..��׺
	const char* TGetLimitLenEncodedString(const char* pOrigString, int nOrigLen, int nFontSize,
		int nWrapCharaNum, char* pLimitLenString, int& nShortLen, int nLineLimit, int bPicPackInSingleLine = false);
	//Ѱ�ҷָ��ַ����ĺ���λ��
	int	TSplitString(const char* pString, int nDesirePos, int bLess);
	//�ڱ����ִ�Ѱ�ҷָ��ַ����ĺ���λ��
	int	TSplitEncodedString(const char* pString, int nCount, int nDesirePos, int bLess);
	//���ָ���еĿ�ʼλ��
	int TGetEncodeStringLineHeadPos(const char* pBuffer, int nCount, int nLine, int nWrapCharaNum, int nFontSize, int bPicPackInSingleLine = false);
	//���ı����еĿ��Ʊ�ǽ���ת����ȥ����Ч�ַ��������ı����洢����
	int	TEncodeText(char* pBuffer, int nCount);
	//���ı����еĿ��Ʊ�ǽ���ת����ȥ����Ч�ַ��������ı����洢����
	int TFilterEncodedText(char* pBuffer, int nCount);
	//ȥ�������ı��еĿ��Ʒ���
	int	TRemoveCtrlInEncodedText(char* pBuffer, int nCount);
	//��ȡ�����ı�������������п�
	int	TGetEncodedTextLineCount(const char* pBuffer, int nCount, int nWrapCharaNum, int& nMaxLineLen, int nFontSize, int nSkipLine = 0, int nLineLimit = 0, int bPicSingleLine = false);
	//���Ѿ�������ı�����ָ��λ�ÿ�ʼ����ָ���Ŀ��Ʒ��ŵ�λ�ã�����-1��ʾδ�ҵ�
	int	TFindSpecialCtrlInEncodedText(const char* pBuffer, int nCount, int nStartPos, char cControl);
	//���Ѿ�������ı���ȥ��ָ�����͵Ŀ��Ʒ�
	int	TClearSpecialCtrlInEncodedText(char* pBuffer, int nCount, char cControl);
	//���Ѿ�������ı���ָ��������ȵ��ڻ�������λ��
	int TGetEncodedTextOutputLenPos(const char* pBuffer, int nCount, int& nLen, int bLess, int nFontSize);
	//���Ѿ�������ı���ָ����ǰ�λ������п��Ʒ����Ժ�����������Ч��Ӱ��
	int TGetEncodedTextEffectCtrls(const char* pBuffer, int nSkipCount, KTP_CTRL& Ctrl0, KTP_CTRL& Ctrl1);
	//���ز���ֵ
	int	TAdviseEngine(IInlinePicEngineSink*);
	//���ز���ֵ
	int	TUnAdviseEngine(IInlinePicEngineSink*);
}

#endif	//ENGINE2_EXPORTS

#endif  //ifndef _ENGINE_TEXT_H_
