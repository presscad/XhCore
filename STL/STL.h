#pragma once

// CSTLDataApp
// �йش���ʵ�ֵ���Ϣ������� STLData.cpp
//
#include <vector>
#include <string>
#include "ArrayList.h"
#include "f_ent.h"
#include "SolidBody.h"
#include "gl/GL.h"
#include "gl/glu.h"
#include "ISTL.h"
using namespace std;

class CGLFace  
{
public:
	struct GLFACEINFO{
		//��һλ��0.��ʾ����ɫ����Ƭ��CGLFaceGroup��ͬ��1.��ʾ����ָ��ֵ
		//�ڶ�λ��0.��ʾ��Ƭ��������Ƭ��CGLFaceGroup��ͬ��1.��ʾ����ָ��ֵ
		char clr_norm;
		char mode;		//������������glBegin()����Ҫ�Ļ���ģʽ
		WORD uVertexNum;//ÿһ��Ƭ�Ķ�����
	};
public:
	GLfloat red,green,blue,alpha;	//��ɫ
	GLdouble nx,ny,nz;				//�淨��
	GLFACEINFO header;
	GLdouble *m_pVertexCoordArr;
	CGLFace(){memset(this,0,sizeof(CGLFace));header.mode=GL_TRIANGLES;}
	~CGLFace()
	{
		if(m_pVertexCoordArr)	//header.uVertexNum>0&&
			delete []m_pVertexCoordArr;
		m_pVertexCoordArr=NULL;
		header.uVertexNum=0;
	}
};
class CSTLPoint
{
public:
	double x,y,z;
	CSTLPoint(){;}
	CSTLPoint(GEPOINT pos){SetParam(pos.x,pos.y,pos.z);}
	CSTLPoint(double posx,double posy,double posz){SetParam(posx,posy,posz);}
	void SetParam(double posx,double posy,double posz){x=posx;y=posy;z=posz;}
};
class CSTLFacet
{
public:
	CSTLPoint m_PointList[3];//��������
	CSTLPoint m_Normal;//������
	CSTLFacet(){;}
};
class CSTLData : public IStlData
{
private:
	int m_iNo;
	ARRAY_LIST<CSTLFacet> m_FacetList;
public:
	static const int STL_LABEL_SIZE = 80;
private:
	bool IsSTLBinary(const char *filename);
	bool SaveStlBinary(const char *file_path);
	bool SaveStlASCII(const char *file_path);
public:
	CSTLData(int iNo=1);
	~CSTLData(void);
	//
	virtual int GetSerial(){return m_iNo;}
	virtual void AddSolidBody(char* solidbuf=NULL,DWORD size=0);
	virtual void SaveFile(const char* file_path,int nType=TYPE_SAVE_BINARY);
};
