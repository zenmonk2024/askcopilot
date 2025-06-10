//---------------------------------------------------------------------------
// (c) 2005 by Kingsoft
// Date:	2005.2.28
// Code:	wooy
//---------------------------------------------------------------------------
#ifndef __ENGINE_PAK_DEF_H_
#define __ENGINE_PAK_DEF_H_

//------------------------------------------------------------------
//----				����ļ��洢�ṹ����					--------
//------------------------------------------------------------------
#define	XPACKFILE_SIGNATURE_FLAG		0x4b434150	//'PACK'

//һ��Pack�ļ����е�ͷ�ṹ:
struct XPackFileHeader
{
	unsigned char	cSignature[4];		//�ĸ��ֽڵ��ļ���ͷ��־���̶�Ϊ�ַ���'PACK'
	unsigned int	uCount;				//���ݵ���Ŀ��
	unsigned int	uIndexTableOffset;	//������ƫ����
	unsigned int	uDataOffset;		//���ݵ�ƫ����
	unsigned int	uCrc32;				//У���(���������������������)
	unsigned int	uPakTime;			//����ļ�����ʱ��ʱ�䣬��Ϊ��λtime()
	unsigned char	cReserved[8];		//�������ֽ�
};

//Pack�ж�Ӧÿ�����ļ���������Ϣ��
struct XPackIndexInfo
{
	unsigned int	uId;				//���ļ�id
	unsigned int	uOffset;			//���ļ��ڰ��е�ƫ��λ��
	unsigned int	uSize;				//���ļ���ԭʼ��С
	unsigned int	uCompressSizeFlag;	//���ļ�ѹ����Ĵ�С��ѹ������
	//����ֽڱ�ʾѹ����������XPACK_METHOD��XPAK_FLAG
	//�͵������ֽڱ�ʾ���ļ�ѹ����Ĵ�С�����ڷֿ�ѹ�����ļ����������ļ�ȫ���ֿ����ݣ�ͷ��Ϣ���ݣ��ֿ���Ϣ��ȼ�������ȫ����С��
};

//�ֿ�ѹ�����ļ����ļ����ݴ洢���洢��������˳��һ��Ϊ��
// [XPackFileFragmentElemHeader] + [��һ������] + [�ڶ�������] + .. + [��n������] + [n��XPackFileFragmentInfo������]

//�ֿ�ѹ�����ļ��ڸ��ļ����ݴ洢����ʼλ�õ�ͷ���ݽṹ
struct XPackFileFragmentElemHeader
{
	int				nNumFragment;		//�ֿ����Ŀ
	int				nFragmentInfoOffest;//�ֿ���Ϣ���ƫ��λ��,�����XPackFileFragmentElemHeader��ʼλ�õ�ƫ��
};

//�ֿ�ѹ���ļ��ķֿ���Ϣ
struct XPackFileFragmentInfo
{
	unsigned int	uOffset;			//�˷ֿ����ݵ���ʼ�������ļ��������е�ƫ��λ��,�����XPackFileFragmentElemHeader��ʼλ�õ�ƫ��
	unsigned int	uSize;				//�˷ֿ��ԭʼ��С
	unsigned int	uCompressSizeFlag;	//�˷ֿ�ѹ����Ĵ�С��ѹ��������������XPackIndexInfo�ṹ�е�uCompressSizeFlag
};


//���ļ���ѹ����ʽ
enum XPACK_METHOD_AND_FLAG
{
	XPACK_METHOD_NONE				= 0x00000000,	//û��ѹ��
//	XPACK_METHOD_UCL_OLD			= 0x01000000,	//UCLѹ��
//	XPACK_METHOD_FILTER_OLD			= 0x0f000000,	//���˱��
//	XPACK_COMPRESS_SIZE_FILTER_OLD	= 0x00ffffff,	//ѹ�����С���˱��

	XPACK_METHOD_UCL				= 0x20000000,	//UCLѹ��
//	XPACK_METHOD_FILTER				= 0xf0000000,	//���˱��
	XPACK_METHOD_FILTER				= 0x0f000000,

	XPACK_FLAG_FRAGMENT				= 0x10000000,	//���ļ��ֿ�ѹ��
	XPACK_COMPRESS_SIZE_FILTER		= 0x07ffffff,	//ѹ�����С���˱��,���֧��128M
	XPACK_COMPRESS_SIZE_BIT			= 27,
};

/*
enum XPACK_METHOD_AND_FLAG
{
	XPACK_METHOD_NONE	           = 0x00000000,			
	XPACK_METHOD_UCL	           = 0x20000000,			
	//XPACK_METHOD_BZIP2	           = 0x20000000,	        
	XPACK_METHOD_FRAGMENT          = 0x30000000,           
	XPACK_METHOD_FRAGMENTA         = 0x40000000,           
	XPACK_METHOD_FRAME	           = 0x10000000,			
	XPACK_METHOD_METHOD_FILTER     = 0xf0000000,	        
	XPACK_METHOD_FILTER            = 0xf0000000,			
	XPACK_COMPRESS_SIZE_FILTER     = 0x07ffffff,        
	XPACK_METHOD_UCL_OLD	       = 0x01000000,			
	XPACK_METHOD_BZIP2_OLD	       = 0x02000000,	       
	XPACK_METHOD_METHOD_FILTER_OLD = 0x0f000000,            
	XPACK_METHOD_FILTER_OLD        = 0xff000000,			
	XPACK_METHOD_FRAGMENT_OLD      = 0x03000000,            
	XPACK_METHOD_FRAGMENTA_OLD     = 0x04000000,            
	XPACK_METHOD_FILTER_SIZE_OLD   = 0x00ffffff,   
	XPACK_FLAG_FRAGMENT				= 0x10000000,
	XPACK_COMPRESS_SIZE_BIT	= 27,
};*/


//------------------------------------------------------------------
//----			�����Ϣ�ļ��洢�ṹ����					--------
//	�����Ϣ�ļ���¼����ÿһ������ļ�һһ��Ӧ����¼�˰���ÿ�����ļ�
//�������Ϣ������ļ���ϴ����Ϣ�ļ����Ϳ������ɵĽ�����е�ȫ����
//�ļ��� �����ļ��ļ���Ϊimage.pak���Ӧ�Ĵ����Ϣ�ļ��ļ���Ϊ
//image.pak.txt��
//------------------------------------------------------------------
//  �����Ϣ�ļ�Ϊ�ı���ʽÿ���и���������\t�ָ���
//  ��һ�и�ʽʾ����
//		TotalFile:220	PakTime:2005-3-20 18:00:00	PakTimeSave:4239E52B	CRC:02FE0701
//		��һ�������֡�220��Ϊ�������ļ�����Ŀ
//		�ڶ������֡�2005-3-20 18:00:00��Ϊ�����Ķ��İ��ļ�������ʱ�䣬��XPackFileHeader::uPakTimeΪ��Ӧһ�µ�ʱ�䡣
//		���������֡�4239E52B��(16����)��ʾ���ļ�������ʱ�䣬��XPackFileHeader::uPakTimeΪ��Ӧһ�µ�ʱ�䡣
//		���������֡�02FE0701��(16����)Ϊ���ļ���CRCУ������
//	�ڶ���Ϊ��ʾ�����п�ʼ�����ݶ�Ӧ�ĸ�ʽ�������̶����£�
//		Index	ID	Time	FileName	Size	InPakSize	ComprFlag	CRC	
//		�ֱ��ʾ�������£�
//			Index		���ļ��ڰ��е�����
//			ID			���ļ���ID
//			Time		���ļ���ʱ�䣨����ȡ�Կ��е��ļ�ΪCheckin����ʱ�䣬���ڸ����ڹ���Ŀ¼���ļ�Ϊ�ļ�������޸�ʱ�䣩
//			FileName	���ļ����ļ������������Ϸ��Ŀ¼��
//			Size		���ļ���ԭʼ��С����λ���ֽڣ�
//			InPakSize	���ļ��ڰ��еĴ洢��С����λ���ֽڣ�
//			ComprFlag	���ļ���ѹ�����(16������)����ֵΪ(XPackIndexInfo::uCompressSizeFlag >> 24)
//			CRC			���ļ���CRCУ�������������ļ�ѹ��ǰ��������������á�
//  �����ÿһ�б�ʾһ���������ļ�����Ϣ������Index����˳�����У�ʾ�����£�
//		0	769629AC	2005-3-19 18:50:00	\settings\serverlist.ini	741		741	0	04657F80
//		1	6FCAA162	2005-3-18 19:00:00	\ui\setting.ini	1519	624	1	2D332871

#endif //#ifndef __ENGINE_PAK_DEF_H_
