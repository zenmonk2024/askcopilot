//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2004 by Kingsoft
// Date:	2002.01.10
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef __ENGINE_POPYGON_H__
#define __ENGINE_POPYGON_H__

#define	defMaxVertex	8
typedef struct
{
	int		nVertex;
	int		nXPos[defMaxVertex];
	int		nYPos[defMaxVertex];
}TPolygon, *LPPolygon;

class ENGINE_API KPolygon  
{
private:
	TPolygon	m_Polygon;
public:
	KPolygon();
	int			IsPointInPolygon(int x, int y);					//	��鵱ǰ���Ƿ��ڶ������
	int			AddPointToVertex(int x, int y);					//	���Ӷ���
	int			GetNearVertex(int XPos, int YPos);				//	ȡ���뵱ǰ������Ķ�������
	int			GetVertexNumber() {return m_Polygon.nVertex;};	//	ȡ�ö���εĶ�����Ŀ
	int			GetIndexVertex(int i, int* pXPos, int* pYPos);	//	ȡ�ö���εĵ�I����������
	int			RemoveIndexVertex(int i);						//	�Ӷ������ȥ����һ������
	void		LoopVertex(int i);								//	�Ѷ���ζ���˳�����I��ѭ���仯
	void		Clear();										//	��ն������Ϣ
	void		GetCenterPos(int* pXPos, int* pYPos);			//	ȡ������λ��
	LPPolygon	GetPolygonPtr() {return &m_Polygon;};			//	ȡ�ö����ָ��
	int			ShiftVertex(int nDir, int nDistance);			//	�����ⶥ����ʲô�����ƶ�
	int			GetLeftVertex();								//	ȡ�����󶥵������
	int			GetRightVertex();								//	ȡ�����Ҷ��������
};

#endif	//__ENGINE_POPYGON_H__

