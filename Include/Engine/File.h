//---------------------------------------------------------------------------
// Sword Engine (c) 1999-2004 by Kingsoft
// Date:	2000.08.08
// Code:	wooy
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef _ENGINE_FILE_H_
#define _ENGINE_FILE_H_

class IFile
{
public:
	// ��ȡ�ļ�����
	virtual unsigned long	Read(void* Buffer, unsigned long ReadBytes) = 0;
	// д���ļ�����
	virtual unsigned long	Write(const void* Buffer, unsigned long WriteBytes) = 0;
	// ����ļ�����Buffer,��iFile�ӿڸ����ͷ�������������ⲿӦ�ò��������ͷ�����
	virtual void*			GetBuffer() = 0;
	// �ƶ��ļ�ָ��λ�ã�Origin	-> Initial position: SEEK_CUR SEEK_END SEEK_SET
	virtual long	Seek(long Offset, int Origin) = 0;
	// ȡ���ļ�ָ��λ��,ʧ�ܷ���-1��
	virtual long	Tell() = 0;
	// ȡ���ļ�����,ʧ�ܷ���0��
	virtual unsigned long	Size() = 0;
	// �жϴ򿪵��Ƿ��ǰ��е��ļ�,���ز���ֵ
	virtual int		IsFileInPak() = 0;

	//�ж��ļ��Ƿ�ֿ�ѹ����
	virtual int		IsPackedByFragment() = 0;
	//��ȡ�ļ��ֿ����Ŀ
	virtual int		GetFragmentCount() = 0;
	//��ȡ�ֿ�Ĵ�С
	virtual unsigned int	GetFragmentSize(int nFragmentIndex) = 0;
	//��ȡһ���ļ��ֿ飬�����pBufferΪĿ�껺�������������Ļ�����ָ��Ϊ�գ����ڲ�������µĻ�������������ָ�룬�ⲿ��Ҫ�������١�
	//�������Ĵ�С��Ҫ�������·ֿ�����ݣ�����ͨ��GetFragmentCount֪��ÿ���ֿ�����ݴ�С����������������Ҫ����ô��
	//����ֵ��ʾʵ�ʶ�ȡ����������ֿ�����ݴ�С,�������ʧ��(�����ֿ鲻����,���ļ����ǰ��ֿ鷽ʽ�洢�ȵ�)����0��
	virtual unsigned long	ReadFragment(int nFragmentIndex, void*& pBuffer) = 0;

	// �رմ򿪵��ļ�
	virtual void	Close() = 0;
	// �ӿڶ�������
	virtual void	Release() = 0;

	virtual ~IFile() {};
};


extern "C"
{
ENGINE_API	IFile*	g_OpenFile(const char* FileName, int ForceUnpakFile = false, int ForWrite = false);
ENGINE_API	IFile*	g_CreateFile(const char* FileName);

//���ò����ļ������ȴ���(�Ƿ��Ȳ��Ҵ���ļ�)
ENGINE_API	void	g_SetFindFileMode(int FindPakFirst);
//���ش���ļ������ز���ֵ
ENGINE_API	int		g_LoadPackageFiles(const char* pListFile, const char* pSection);
//ж�ش���ļ�
ENGINE_API	void	g_ClearPackageFiles();
//�����ļ�������Ŀ¼
ENGINE_API	void	g_SetRootPath(const char* lpPathName = 0);
//��ȡ�ļ�������Ŀ¼
ENGINE_API	void	g_GetRootPath(char* lpPathName);

//���̷߳���engine���ļ�ģ��ʱ����ʹ��������������������ʱ����ʹ�������rootĿ¼������·��
ENGINE_API	void	g_SetFilePath(const char* lpPathName);
ENGINE_API	void	g_GetFilePath(char* lpPathName);

ENGINE_API	void	g_GetFullPath(char* lpPathName, const char* lpFileName);
ENGINE_API	void	g_GetHalfPath(char* lpPathName, const char* lpFileName);
ENGINE_API	void	g_GetPackPath(char* lpPathName, const char* lpFileName);
ENGINE_API	BOOL	g_GetFilePathFromFullPath(char* lpPathName,
											  const char* lpFullFilePath);
//ENGINE_API	void	g_GetDiskPath(char* lpPathName, const char* lpFileName);
ENGINE_API	int		g_CreatePath(const char* lpPathName);
ENGINE_API	int		g_IsFileExist(const char* lpPathName);
//��һ���ַ���תΪhash��ֵ
ENGINE_API	unsigned int	g_StringHash(const char* pString);
//��һ���ַ���Сд������תΪhash��ֵ(���ڷ������ַ��������ַ��������ת��,������ײ���ʣ��������Ա���ƽ̨�޹�)
ENGINE_API	unsigned int	g_StringLowerHash(const char* pString);
//��һ���ļ����ַ���Сд������תΪhash��ֵ
//a. ���� / �ַ�����תΪ \
//b. �Զ��ڷ������ַ��������ַ��������ת��,������ײ���ʣ��������Ա���ƽ̨�޹�
ENGINE_API	unsigned int	g_FileNameHash(const char* pString);

// һ��·����һ���ļ������ϲ���lpGet���γ�һ��������·���ļ���
ENGINE_API	void	g_UnitePathAndName(const char *lpPath, const char *lpFile, char *lpGet);
ENGINE_API	void	g_ChangeFileExt(char* lpFileName, const char* lpNewExt);
ENGINE_API	void	g_ExtractFileName(char* lpFileName, const char* lpFilePath);
ENGINE_API	void	g_ExtractFilePath(char* lpPathName, const char* lpFilePath);
}

//====UCLѹ���㷨����====
extern "C"
{
	ENGINE_API int CD_LCU_I();
	ENGINE_API int CD_LCU_C(const unsigned char* pSrcBuffer, unsigned int nSrcLen,
						unsigned char* pDestBuffer, unsigned int* pDestLen, int nCompressLevel);
	ENGINE_API int CD_LCU_D(const unsigned char* pSrcBuffer, unsigned nSrcLen,
						unsigned char* pDestBuffer, unsigned int uExtractSize);
}

#endif //#ifndef _ENGINE_FILE_H_
