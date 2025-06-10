//---------------------------------------------------------------------------
//	���ִ�����
//	Copyright : Kingsoft 2007
//	CreateTime:	2007-8
//---------------------------------------------------------------------------
//���ִ������и��ݲ�ͬ�ı���������в�ͬʵ�ֵĴ�����/���룬�㼯�ڴ�Դ�ļ��С�
//---------------------------------------------------------------------------


//=============================================================================================================
//>>  ���� LOCALIZATION_CHARACTER_SET_ENGLISH ��ش��� ��ʼ
//=============================================================================================================
//���ĳ���ַ��Ƿ�Ϊ����������׵��ַ�
#define	ENGLISh_NUM_CHARACTER_IN_00	7
unsigned char	s_ENGLISH_NotAllowAtLineHead00Characters[ENGLISh_NUM_CHARACTER_IN_00] =
{
	//!),.:;>?
	0x21, 0x29, 0x2c, /*0x2e,*/ 0x3a, 0x3b, 0x3e, 0x3f
};

//���ĳ���ַ��Ƿ�Ϊ����������׵��ַ������������ַ��򷵻�0�����򷵻��ַ�ռ���ӽ���
int ENGLISH_IsCharacterNotAlowAtLineHead(const char *pCharacter)
{
	for (int i = 0; i < ENGLISh_NUM_CHARACTER_IN_00; ++i)
		if ((unsigned char)(*pCharacter) == s_ENGLISH_NotAllowAtLineHead00Characters[i])
			return 1;
	return false;
}
//=============================================================================================================
//  ���� LOCALIZATION_CHARACTER_SET_ENGLISH ��ش��� ����
//=============================================================================================================



//=============================================================================================================
//>> �������� LOCALIZATION_CHARACTER_SET_GBK ��ش��� ��ʼ
//=============================================================================================================
//���ĳ���ַ��Ƿ�Ϊ����������׵��ַ�
#define	GBK_NUM_CHARACTER_IN_A1	11
unsigned char	s_GBK_NotAllowAtLineHeadA1Characters[GBK_NUM_CHARACTER_IN_A1] = 
{
	//��������������������
	0xa2, 0xa3, 0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb9, 0xbb, 0xbd, 0xbf
};
#define	GBK_NUM_CHARACTER_IN_A3	10
unsigned char	s_GBK_NotAllowAtLineHeadA3Characters[GBK_NUM_CHARACTER_IN_A3] = 
{
	//��   ��    ��   ��     ��    ��   ��    ��    ��    ��
	0xa1, 0xa9, 0xac, 0xae, 0xba, 0xbb, 0xbe, 0xbf, 0xdd, 0xfd
};
// �������ĵĲ���������׵��ַ�

//���GBK�����ĳ���ַ��Ƿ�Ϊ����������׵��ַ������������ַ��򷵻�0�����򷵻��ַ�ռ���ӽ���
int GBK_IsCharacterNotAlowAtLineHead(const char *pCharacter)
{
	int				i;
	unsigned char	cChar;
	cChar = (unsigned char)(*pCharacter);

	if (cChar == 0xa3)
	{
		cChar = (unsigned char)pCharacter[1];
		if (cChar >= 0xa1 && cChar <= 0xfd)
		{
			for (i = 0; i < GBK_NUM_CHARACTER_IN_A3; i++)
				if (s_GBK_NotAllowAtLineHeadA3Characters[i] == cChar)
					return 2;
		}
	}
	else if (cChar == 0xa1)
	{
		cChar = (unsigned char)pCharacter[1];
		if (cChar >= 0xa2 && cChar <= 0xbf)
		{
			for (i = 0; i < GBK_NUM_CHARACTER_IN_A1; i++)
				if (s_GBK_NotAllowAtLineHeadA1Characters[i] == cChar)
					return 2;
		}
	}
	else 
		return ENGLISH_IsCharacterNotAlowAtLineHead(pCharacter);

	return false;
}
//=============================================================================================================
//  �������� LOCALIZATION_CHARACTER_SET_GBK ��ش��� ����
//=============================================================================================================



//=============================================================================================================
//>> �������� LOCALIZATION_CHARACTER_SET_BIG5 ��ش��� ��ʼ
//=============================================================================================================
//���ĳ���ַ��Ƿ�Ϊ����������׵��ַ�
#define BIG5_NUM_CHARACTER_IN_A1 36
unsigned char s_BIG5_NotAllowAtLineHeadA3Characters[BIG5_NUM_CHARACTER_IN_A1] = 
{
	//��   ��    ��    ��    ?    ��    ��    ��    ��    �U    ��    �E    �o    �p    �q
	0x41, 0x42, 0x43, 0x44 ,0x45 ,0x46 ,0x47 ,0x48 ,0x49 ,0x4A ,0x4B, 0x4C, 0x4D, 0x4E ,0x4F ,
	//��   �r   �s    �t    �u     ��
	0x50 ,0x51 ,0x52 ,0x53 ,0x54 ,0x5E ,
	//��   ��    ��    ��
	0x62 ,0x66 ,0x6A ,0x6E ,
	//��   ��    ��    �w
	0x72 ,0x76 ,0x7A ,0x7E ,
	//�y   �{    ��    ��     ��    ��
	0xA2 ,0xA4 ,0xA6 ,0xA8 ,0xAA ,0xAC ,
	//��
	0xE1 ,
};
//�����ĵĹ�����ͬ

//���BIG5�����ĳ���ַ��Ƿ�Ϊ����������׵��ַ������������ַ��򷵻�0�����򷵻��ַ�ռ���ӽ���
int BIG5_IsCharacterNotAlowAtLineHead(const char *pCharacter)
{
	if ((unsigned char)(*pCharacter) == 0xa1)
	{
		for (int i = 0; i < BIG5_NUM_CHARACTER_IN_A1; ++i)
			if ((unsigned char)(pCharacter[1]) == s_BIG5_NotAllowAtLineHeadA3Characters[i])
				return 2;
	}
	else 
		return ENGLISH_IsCharacterNotAlowAtLineHead(pCharacter);

	return false;
}
//=============================================================================================================
//  �������� LOCALIZATION_CHARACTER_SET_BIG5 ��ش��� ����
//=============================================================================================================



//=============================================================================================================
//>> Խ���� LOCALIZATION_CHARACTER_SET_VIETNAM ��ش��� ��ʼ
//=============================================================================================================
//���ĳ���ַ��Ƿ�Ϊ����������׵��ַ�
//Խ���ĵĹ�������ĵ���ͬ

//=============================================================================================================
//  Խ�� LOCALIZATION_CHARACTER_SET_VIETNAM ��ش��� ����
//=============================================================================================================

