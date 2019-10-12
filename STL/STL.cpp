// STLData.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "STL.h"
#include "glDrawTool.h"
#include "f_alg_fun.h"

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//


#include "stdafx.h"
#include "Stl.h"
#include "io.h"
#include <iostream>
#include <sstream>

#pragma region 
static bool Standize(double* vector3d)
{
	double mod_len=vector3d[0]*vector3d[0]+vector3d[1]*vector3d[1]+vector3d[2]*vector3d[2];
	mod_len=sqrt(max(0,mod_len));
	if(mod_len<EPS)
		return false;
	vector3d[0]/=mod_len;
	vector3d[1]/=mod_len;
	vector3d[2]/=mod_len;
	return true;
}
int FloatToInt(double f)
{
	int fi=(int)f;
	return f-fi>0.5?fi+1:fi;
}
int CalArcResolution(double radius,double sector_angle)
{
	double user2scr_scale=1.0;
	int sampling=5;		//Բ������ʱ�ĳ�������(������)
	UINT uMinSlices=36;	//��Բ����ʾ�����ƽ����Ƭ��
	double length=sector_angle*radius;
	int n = FloatToInt((length/user2scr_scale)/sampling);
	int min_n=FloatToInt(uMinSlices*sector_angle/Pi)/2;
	int max_n=FloatToInt(72*sector_angle/Pi);
	if(max_n==0)
		max_n=1;
	if(min_n==0)
		min_n=1;
	n=max(n,min_n);
	n=min(n,max_n);
	return n;
}
void GetArcSimuPolyVertex(f3dArcLine* pArcLine, GEPOINT vertex_arr[], int slices)
{
	int i,sign=1;
	if(pArcLine->ID&0x80000000)	//����
		sign=-1;
	double slice_angle = pArcLine->SectorAngle()/slices;
	if(pArcLine->WorkNorm()==pArcLine->ColumnNorm())	//Բ��
	{
		if(sign<0)
			slice_angle*=-1;
		double sina = sin(slice_angle);	//��Ƭ������
		double cosa = cos(slice_angle);	//��Ƭ������
		vertex_arr[0].Set(pArcLine->Radius());
		for(i=1;i<=slices;i++)
		{
			vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
			vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		}
		GEPOINT origin=pArcLine->Center();
		GEPOINT axis_x=pArcLine->Start()-origin;
		Standize(axis_x);
		GEPOINT axis_y=pArcLine->WorkNorm()^axis_x;
		for(i=0;i<=slices;i++)
		{
			vertex_arr[i].Set(	origin.x+vertex_arr[i].x*axis_x.x+vertex_arr[i].y*axis_y.x,
				origin.y+vertex_arr[i].x*axis_x.y+vertex_arr[i].y*axis_y.y,
				origin.z+vertex_arr[i].x*axis_x.z+vertex_arr[i].y*axis_y.z);
		}
	}
	else	//��Բ��
	{
		if(sign<0)
		{
			for(i=slices;i>=0;i--)
				vertex_arr[i] = pArcLine->PositionInAngle(i*slice_angle);
		}
		else
		{
			for(i=0;i<=slices;i++)
				vertex_arr[i] = pArcLine->PositionInAngle(i*slice_angle);
		}
	}
}
static f3dPoint GetPolyFaceWorkNorm(CSolidBody* pBody,CRawSolidFace& rawface)
{
	CFaceLoop loop=rawface.GetOutterLoop();
	f3dPoint vec1,vec2,norm;//��ʱ��Ϊ�м���άʸ��
	long n = loop.LoopEdgeLineNum();
	if(n<3)
		return norm;
	//--------------VVVVVVVVV---2.������������û�����ϵucs---------------------
	if(rawface.WorkNorm.IsZero())
	{	//��ָ���������
		f3dArcLine line1,line2;
		for(long start=0;start<n;start++)
		{
			loop.GetLoopEdgeLineAt(pBody,start,line1);
			if(line1.ID&0x80000000)
				Sub_Pnt( vec1, line1.Start(), line1.End());
			else
				Sub_Pnt( vec1, line1.End(), line1.Start());
			Standize(vec1);
			long i,j;
			for(i=start+1;i<n;i++)
			{
				loop.GetLoopEdgeLineAt(pBody,i,line2);
				if(line2.ID&0x80000000)	//����
					Sub_Pnt( vec2, line2.Start(), line2.End());
				else
					Sub_Pnt( vec2, line2.End(), line2.Start());
				Standize(vec2);
				norm = vec1^vec2;
				double dd=norm.mod();
				if(dd>EPS2)
				{
					norm /= dd;	//��λ��
					break;
				}
			}
			if( i==n)
				return norm;	//������涨������(���б߽����غ�)����������
			//ͨ��������ʱ����ϵ���жϷ�����������
			GECS cs;
			cs.origin=line1.Start();
			cs.axis_x=vec1;
			cs.axis_z=norm;
			cs.axis_y=cs.axis_z^cs.axis_x;
			for(j=0;j<n;j++)
			{
				if(j==0||j==i)
					continue;
				loop.GetLoopEdgeLineAt(pBody,start+j,line2);
				f3dPoint vertice=line2.End();
				vertice=cs.TransPToCS(vertice);
				if(vertice.y<-EPS)	//��������ֱ���ϻ򸽽�ʱ��Ӧ����������
					break;
			}
			if(j==n)	//ȫ�����㶼�ڵ�ǰ�ߵ���࣬����ʾ����ķ���Ϊ�����ߣ�����Ӧ����Ѱ��
				break;
		}
	}
	else
		norm=rawface.WorkNorm;
	return norm;
}
void WriteToSolidBuffer(CSolidBodyBuffer& solidbuf,int indexBasicFace,CXhSimpleList<CGLFace>& listFacets,COLORREF color,const double* poly_norm)
{
	//д��һ�������Ƭ���ݣ���Ӧһ����������ֳɵ����ɻ�����Ƭ��
	solidbuf.SeekPosition(solidbuf.BasicFaceIndexStartAddr+indexBasicFace*4);
	DWORD basicface_lenpos=solidbuf.GetLength();
	solidbuf.WriteDword(basicface_lenpos);	//��������λ��д��������
	solidbuf.SeekToEnd();
	solidbuf.WriteWord((WORD)0);	//CTidBasicFace�����ݼ�¼����
	solidbuf.Write(&color,4);
	WORD facets_n=0;
	LIST_NODE<CGLFace>* pTailNode=listFacets.EnumTail();
	if(pTailNode!=NULL)
		facets_n=(WORD)listFacets.EnumTail()->serial;
	BUFFERPOP stack(&solidbuf,facets_n);
	solidbuf.WriteWord(facets_n);
	for(CGLFace* pGLFace=listFacets.EnumObjectFirst();pGLFace;pGLFace=listFacets.EnumObjectNext())
	{
		WORD cluster_buf_n=1+24+2+pGLFace->header.uVertexNum*24;
		solidbuf.WriteWord(cluster_buf_n);
		solidbuf.WriteByte(pGLFace->header.mode);
		if(pGLFace->header.clr_norm&0x02)
		{	//ָ����Ƭ�ط���ֵ
			solidbuf.WriteDouble(pGLFace->nx);
			solidbuf.WriteDouble(pGLFace->ny);
			solidbuf.WriteDouble(pGLFace->nz);
		}
		else	//��Ƭ�ط��߼�Ϊԭʼ������ķ���
			solidbuf.WritePoint(GEPOINT(poly_norm));
		//д������������Ƭ����
		if(pGLFace->header.mode==GL_TRIANGLES)
			solidbuf.WriteWord((WORD)(pGLFace->header.uVertexNum/3));
		else
			solidbuf.WriteWord((WORD)(pGLFace->header.uVertexNum-2));
		solidbuf.Write(pGLFace->m_pVertexCoordArr,pGLFace->header.uVertexNum*24);
		stack.Increment();
	}
	stack.VerifyAndRestore(true,2);
	WORD wBasicfaceBufLen=(WORD)(solidbuf.GetCursorPosition()-basicface_lenpos-2);
	solidbuf.SeekPosition(basicface_lenpos);
	solidbuf.WriteWord(wBasicfaceBufLen);
	solidbuf.SeekToEnd();
}
bool SplitToBasicFacets(CSolidBody* pSrcBody,CSolidBody* pDstBody)
{
	if(pSrcBody==NULL)
		return false;
	//Ǩ��ԭʵ�嶨���ڴ�ͬʱ�ڳ�����������Ƭ�Ļ���ʵ����ʾ���������ռ�
	int face_n=pSrcBody->PolyFaceNum();
	int edge_n=pSrcBody->KeyEdgeLineNum();
	if(face_n==0||edge_n==0)
		return false;
	CSolidBodyBuffer solidbuf;
	solidbuf.Write(pSrcBody->BufferPtr(),33);
	solidbuf.BasicFaceNumber=face_n;	//д�������Ƭ��=ԭʼ���������
	DWORD dwIndexBufSize=(edge_n+face_n)*4;
	solidbuf.WriteAt(45,pSrcBody->BufferPtr()+45,dwIndexBufSize);
	solidbuf.BasicFaceIndexStartAddr=45+dwIndexBufSize;
	solidbuf.SeekToEnd();	//BasicFaceIndexStartAddr����ֵ��䵱ǰ�洢ָ��λ��
	solidbuf.Write(NULL,face_n*4);	//@45+dwIndexBufSize	ʵ�������Ƭ��������д����Ӧ�Ŀ��ֽ�ռλ
	DWORD dwDataBufSize=pSrcBody->BufferLength()-solidbuf.VertexDataStartAddr;
	if(pSrcBody->BasicGLFaceNum()>0)	//ֻ����ʵ��ԭʼ�����������򣬺���ԭ�еĻ�����Ƭ������
		dwDataBufSize=pSrcBody->BasicFaceDataStartAddr()-solidbuf.VertexDataStartAddr;
	long iNewVertexDataStartAddr=solidbuf.GetCursorPosition();
	long iOldVertexDataStartAddr=solidbuf.VertexDataStartAddr;
	solidbuf.VertexDataStartAddr=iNewVertexDataStartAddr;
	solidbuf.EdgeDataStartAddr=solidbuf.EdgeDataStartAddr+4*face_n;
	solidbuf.RawFaceDataStartAddr=solidbuf.RawFaceDataStartAddr+4*face_n;
	solidbuf.SeekToEnd();
	solidbuf.Write(pSrcBody->BufferPtr()+iOldVertexDataStartAddr,dwDataBufSize);	//д��ԭʵ�嶨����������ڴ�
	//���������ӻ�����Ƭ������¼�������ĺ�����ַλ��ֵ
	int addr_offset=(face_n-pSrcBody->BasicGLFaceNum())*4;
	if(addr_offset!=0)
	{	//������λ�������ͼԪ������ָ����ڴ��ַƫ��
		DWORD* RawFaceAddr=(DWORD*)(solidbuf.GetBufferPtr()+solidbuf.RawFaceIndexStartAddr);
		for(int i=0;i<face_n;i++)
			*(RawFaceAddr+i)+=addr_offset;
		DWORD* RawEdgeAddr=(DWORD*)(solidbuf.GetBufferPtr()+solidbuf.EdgeIndexStartAddr);
		for(int i=0;i<face_n;i++)
			*(RawEdgeAddr+i)+=addr_offset;
	}
	if(solidbuf.BasicFaceDataStartAddr==0)	//��ǰ������Ƭ��Ϊ��
		solidbuf.BasicFaceDataStartAddr=solidbuf.GetCursorPosition();
	else	//��дԭ������Ƭ������
		solidbuf.BasicFaceDataStartAddr=solidbuf.BasicFaceDataStartAddr+4*face_n;
	//д������������
	double alpha=0.6;	//���ǵ���ʾЧ���ľ���ϵ��
	int i=0,j=0,n=0;
	CRawSolidFace rawface;
	CXhSimpleList<CGLFace> listFacets;
	CXhSimpleList<GEPOINT> listVertices;
	for(int indexFace=0;indexFace<face_n;indexFace++)
	{
		listFacets.DeleteList();
		listVertices.DeleteList();
		if(!pSrcBody->GetPolyFaceAt(indexFace,rawface))
		{
			WriteToSolidBuffer(solidbuf,indexFace,listFacets,0,GEPOINT(0,0,0));
			continue;
		}
		/*����һ�������棬�϶���һ���⻷�����⻹�������������ڻ������ǵķ��߷�������ͬ��
		  ��Է�����˵���⻷Ӧ����ʱ�뷽�򣬶��ڻ���Ӧ����˳ʱ�뷽��
		*/
		f3dArcLine edgeLine[4];
		GEPOINT poly_norm=rawface.WorkNorm;
		CFaceLoop outerloop=rawface.GetOutterLoop();
		if(outerloop.LoopEdgeLineNum()==3)
		{
			outerloop.GetLoopEdgeLineAt(pSrcBody,0,edgeLine[0]);
			outerloop.GetLoopEdgeLineAt(pSrcBody,1,edgeLine[1]);
			outerloop.GetLoopEdgeLineAt(pSrcBody,2,edgeLine[2]);
			f3dPoint pt_arr[3];
			if(edgeLine[0].ID&0x80000000)	//����
				pt_arr[0] = edgeLine[0].End();
			else
				pt_arr[0] = edgeLine[0].Start();
			if(edgeLine[1].ID&0x80000000)	//����
				pt_arr[1] = edgeLine[1].End();
			else
				pt_arr[1] = edgeLine[1].Start();
			if(edgeLine[2].ID&0x80000000)	//����
				pt_arr[2] = edgeLine[2].End();
			else
				pt_arr[2] = edgeLine[2].Start();
			poly_norm=rawface.WorkNorm;
			if(poly_norm.IsZero())
			{
				f3dPoint vec1=pt_arr[1]-pt_arr[0];
				f3dPoint vec2=pt_arr[2]-pt_arr[1];
				poly_norm=vec1^vec2;
			}
			if(!Standize(poly_norm))
			{
				WriteToSolidBuffer(solidbuf,indexFace,listFacets,rawface.MatColor(),poly_norm);
				continue;//return false;
			}
			//���������淨�ߡ�������ɫ�������������Ϣ
			CGLFace *pGLFace=listFacets.AttachObject();
			pGLFace->nx=poly_norm.x;
			pGLFace->ny=poly_norm.y;
			pGLFace->nz=poly_norm.z;
			pGLFace->red   = GetRValue(rawface.MatColor())/255.0f;
			pGLFace->green = GetGValue(rawface.MatColor())/255.0f;
			pGLFace->blue  = GetBValue(rawface.MatColor())/255.0f;
			pGLFace->alpha = (GLfloat)alpha;
			pGLFace->header.uVertexNum=3;
			pGLFace->header.clr_norm=0x03;	//Ĭ�ϱ任��ɫ������
			pGLFace->m_pVertexCoordArr=new GLdouble[9];
			for(j=0;j<3;j++)
			{
				pGLFace->m_pVertexCoordArr[3*j]=pt_arr[j].x;
				pGLFace->m_pVertexCoordArr[3*j+1]=pt_arr[j].y;
				pGLFace->m_pVertexCoordArr[3*j+2]=pt_arr[j].z;
			}
			WriteToSolidBuffer(solidbuf,indexFace,listFacets,rawface.MatColor(),poly_norm);
			continue;
		}
		if(outerloop.LoopEdgeLineNum()==4)
		{
			outerloop.GetLoopEdgeLineAt(pSrcBody,0,edgeLine[0]);
			outerloop.GetLoopEdgeLineAt(pSrcBody,1,edgeLine[1]);
			outerloop.GetLoopEdgeLineAt(pSrcBody,2,edgeLine[2]);
			outerloop.GetLoopEdgeLineAt(pSrcBody,3,edgeLine[3]);
			if(rawface.WorkNorm.IsZero())
			{
				f3dPoint vec1=edgeLine[0].End()-edgeLine[0].Start();
				f3dPoint vec2=edgeLine[1].End()-edgeLine[1].Start();
				poly_norm=vec1^vec2;
				int sign1=1,sign2=1;
				if(edgeLine[0].ID&0x80000000)
					sign1=-1;
				if(edgeLine[1].ID&0x80000000)
					sign2=-1;
				if(sign1+sign2==0)	//��ű���
					poly_norm*=-1;
			}
			else
				poly_norm=rawface.WorkNorm;
			if(!Standize(poly_norm))
			{
				if(edgeLine[0].SectorAngle()>0)
				{
					poly_norm=edgeLine[0].WorkNorm();
					if(edgeLine[0].ID&0x80000000)
						poly_norm*=-1;
				}
				else if(edgeLine[1].SectorAngle()>0)
				{
					poly_norm=edgeLine[1].WorkNorm();
					if(edgeLine[1].ID&0x80000000)
						poly_norm*=-1;
				}
				//TODO: δ���ԭ�⣬�����ǵ��Ĺ��߱߳���
				//edgeLine[0]=NULL;
			}
			if(edgeLine[0].SectorAngle()>0&&edgeLine[1].SectorAngle()==0&&edgeLine[2].SectorAngle()>0&&edgeLine[3].SectorAngle()==0
				&&fabs(edgeLine[0].WorkNorm()*poly_norm)<EPS_COS)
			{
				n=max(edgeLine[0].m_uDisplaySlices,edgeLine[2].m_uDisplaySlices);
				if(n==0)
				{
					int n1=CalArcResolution(edgeLine[0].Radius(),edgeLine[0].SectorAngle());
					int n2=CalArcResolution(edgeLine[2].Radius(),edgeLine[2].SectorAngle());
					n=max(n1,n2);
				}
				n=min(n,200);
				GEPOINT vertex_arr1[200],vertex_arr2[200];
				GetArcSimuPolyVertex(&edgeLine[0],vertex_arr1,n);
				GetArcSimuPolyVertex(&edgeLine[2],vertex_arr2,n);
				// 			double part_angle1=edgeLine[0]->SectorAngle()/n;
				// 			double part_angle2=edgeLine[2]->SectorAngle()/n;
				// 			double posAngle;

				for(i=0;i<n;i++)
				{
					f3dPoint pt_arr[3];
					//����Բ���м��
					//posAngle=edgeLine[0]->SectorAngle()-i*part_angle1;
					pt_arr[0] = vertex_arr1[n-i];//edgeLine[0]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=i*part_angle2;
					pt_arr[1] = vertex_arr2[i];//edgeLine[2]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=(i+1)*part_angle2;
					pt_arr[2] = vertex_arr2[i+1];//edgeLine[2]->PositionInAngle(posAngle);
					f3dPoint axis_x=pt_arr[1]-pt_arr[0];
					f3dPoint axis_y=pt_arr[2]-pt_arr[0];
					poly_norm=axis_x^axis_y;
					Standize(poly_norm);
					//���������淨�ߡ�������ɫ�������������Ϣ
					CGLFace *pGLFace=listFacets.AttachObject();
					pGLFace->nx=poly_norm.x;
					pGLFace->ny=poly_norm.y;
					pGLFace->nz=poly_norm.z;
					pGLFace->red   = GetRValue(rawface.MatColor())/255.0f;
					pGLFace->green = GetGValue(rawface.MatColor())/255.0f;
					pGLFace->blue  = GetBValue(rawface.MatColor())/255.0f;
					pGLFace->alpha = (GLfloat)alpha;
					pGLFace->header.uVertexNum=3;
					pGLFace->m_pVertexCoordArr=new GLdouble[9];
					for(j=0;j<3;j++)
					{
						pGLFace->m_pVertexCoordArr[3*j]=pt_arr[j].x;
						pGLFace->m_pVertexCoordArr[3*j+1]=pt_arr[j].y;
						pGLFace->m_pVertexCoordArr[3*j+2]=pt_arr[j].z;
					}

					//����Բ���м��
					//posAngle=(i+1)*part_angle2;
					pt_arr[0] = vertex_arr2[i+1];//edgeLine[2]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=edgeLine[0]->SectorAngle()-(i+1)*part_angle1;
					pt_arr[1] = vertex_arr1[n-i-1];//edgeLine[0]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=edgeLine[0]->SectorAngle()-i*part_angle1;
					pt_arr[2] = vertex_arr1[n-i];//edgeLine[0]->PositionInAngle(posAngle);
					axis_x = pt_arr[1]-pt_arr[0];
					axis_y = pt_arr[2]-pt_arr[0];
					poly_norm=axis_x^axis_y;
					Standize(poly_norm);
					//���������淨�ߡ�������ɫ�������������Ϣ
					pGLFace=listFacets.AttachObject();
					pGLFace->nx=poly_norm.x;
					pGLFace->ny=poly_norm.y;
					pGLFace->nz=poly_norm.z;
					pGLFace->red   = GetRValue(rawface.MatColor())/255.0f;
					pGLFace->green = GetGValue(rawface.MatColor())/255.0f;
					pGLFace->blue  = GetBValue(rawface.MatColor())/255.0f;
					pGLFace->alpha = (GLfloat)alpha;
					pGLFace->header.uVertexNum=3;
					pGLFace->m_pVertexCoordArr=new GLdouble[9];
					for(j=0;j<3;j++)
					{
						pGLFace->m_pVertexCoordArr[3*j]=pt_arr[j].x;
						pGLFace->m_pVertexCoordArr[3*j+1]=pt_arr[j].y;
						pGLFace->m_pVertexCoordArr[3*j+2]=pt_arr[j].z;
					}
				}
				WriteToSolidBuffer(solidbuf,indexFace,listFacets,rawface.MatColor(),poly_norm);
				continue;
			}
			else if(edgeLine[0].SectorAngle()==0&&edgeLine[1].SectorAngle()>0&&edgeLine[2].SectorAngle()==0&&edgeLine[3].SectorAngle()>0
				&&fabs(edgeLine[1].WorkNorm()*poly_norm)<EPS_COS)
			{
				n=max(edgeLine[1].m_uDisplaySlices,edgeLine[3].m_uDisplaySlices);
				if(n==0)
				{
					int n1=CalArcResolution(edgeLine[1].Radius(),edgeLine[1].SectorAngle());
					int n2=CalArcResolution(edgeLine[3].Radius(),edgeLine[3].SectorAngle());
					n=max(n1,n2);
				}
				n=min(n,200);
				GEPOINT vertex_arr1[200],vertex_arr2[200];
				GetArcSimuPolyVertex(&edgeLine[1],vertex_arr1,n);
				GetArcSimuPolyVertex(&edgeLine[3],vertex_arr2,n);
				// 			double part_angle1=edgeLine[1]->SectorAngle()/n;
				// 			double part_angle2=edgeLine[3]->SectorAngle()/n;
				// 			double posAngle;
				glEnable(GL_NORMALIZE);
				glEnable(GL_AUTO_NORMAL);
				for(i=0;i<n;i++)
				{
					f3dPoint pt_arr[3];
					//����Բ���м��
					//posAngle=edgeLine[1]->SectorAngle()-i*part_angle1;
					pt_arr[0] = vertex_arr1[n-i];//edgeLine[1]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=i*part_angle2;
					pt_arr[1] = vertex_arr2[i];//edgeLine[3]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=(i+1)*part_angle2;
					pt_arr[2] = vertex_arr2[i+1];//edgeLine[3]->PositionInAngle(posAngle);
					f3dPoint axis_x=pt_arr[1]-pt_arr[0];
					f3dPoint axis_y=pt_arr[2]-pt_arr[0];
					poly_norm=axis_x^axis_y;
					Standize(poly_norm);
					//���������淨�ߡ�������ɫ�������������Ϣ
					CGLFace *pGLFace=listFacets.AttachObject();
					pGLFace->nx=poly_norm.x;
					pGLFace->ny=poly_norm.y;
					pGLFace->nz=poly_norm.z;
					pGLFace->red   = GetRValue(rawface.MatColor())/255.0f;
					pGLFace->green = GetGValue(rawface.MatColor())/255.0f;
					pGLFace->blue  = GetBValue(rawface.MatColor())/255.0f;
					pGLFace->alpha = (GLfloat)alpha;
					pGLFace->header.uVertexNum=3;
					pGLFace->m_pVertexCoordArr=new GLdouble[9];
					for(j=0;j<3;j++)
					{
						pGLFace->m_pVertexCoordArr[3*j]=pt_arr[j].x;
						pGLFace->m_pVertexCoordArr[3*j+1]=pt_arr[j].y;
						pGLFace->m_pVertexCoordArr[3*j+2]=pt_arr[j].z;
					}

					//����Բ���м��
					//posAngle=(i+1)*part_angle2;
					pt_arr[0] = vertex_arr2[i+1];//edgeLine[3]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=edgeLine[1]->SectorAngle()-(i+1)*part_angle1;
					pt_arr[1] = vertex_arr1[n-i-1];//edgeLine[1]->PositionInAngle(posAngle);
					//����Բ���м��
					//posAngle=edgeLine[1]->SectorAngle()-i*part_angle1;
					pt_arr[2] = vertex_arr1[n-i];//edgeLine[1]->PositionInAngle(posAngle);
					axis_x=pt_arr[1]-pt_arr[0];
					axis_y=pt_arr[2]-pt_arr[0];
					poly_norm=axis_x^axis_y;
					Standize(poly_norm);
					//���������淨�ߡ�������ɫ�������������Ϣ
					pGLFace=listFacets.AttachObject();
					pGLFace->nx=poly_norm.x;
					pGLFace->ny=poly_norm.y;
					pGLFace->nz=poly_norm.z;
					pGLFace->red   = GetRValue(rawface.MatColor())/255.0f;
					pGLFace->green = GetGValue(rawface.MatColor())/255.0f;
					pGLFace->blue  = GetBValue(rawface.MatColor())/255.0f;
					pGLFace->alpha = (GLfloat)alpha;
					pGLFace->header.uVertexNum=3;
					pGLFace->m_pVertexCoordArr=new GLdouble[9];
					for(j=0;j<3;j++)
					{
						pGLFace->m_pVertexCoordArr[3*j]=pt_arr[j].x;
						pGLFace->m_pVertexCoordArr[3*j+1]=pt_arr[j].y;
						pGLFace->m_pVertexCoordArr[3*j+2]=pt_arr[j].z;
					}
				}
				WriteToSolidBuffer(solidbuf,indexFace,listFacets,rawface.MatColor(),poly_norm);
				continue;
			}
		}
		CGLTesselator t;
		t.SetFilling(TRUE);
		t.SetWindingRule(GLU_TESS_WINDING_ODD);
		if(poly_norm.IsZero())
			poly_norm=GetPolyFaceWorkNorm(pSrcBody,rawface);
		t.StartDef();
		t.TessNormal(poly_norm.x,poly_norm.y,poly_norm.z);
		//��һ��Ϊ�⻷���μ�B-repģ�ͣ�
		int ei=0,edge_n=outerloop.LoopEdgeLineNum();
		//for(pLine=pFace->outer_edge.GetFirst();pLine!=NULL;pLine=pFace->outer_edge.GetNext())
		f3dArcLine line;
		f3dPoint vertice;
		for(ei=0;ei<edge_n;ei++)
		{
			outerloop.GetLoopEdgeLineAt(pSrcBody,ei,line);
			if(line.SectorAngle()==0)
			{
				if(line.Start()==line.End())
					continue;
				if(line.ID&0x80000000)
					vertice = line.End();
				else
					vertice = line.Start();
				listVertices.AttachObject(vertice);
			}
			else
			{
				if(line.m_uDisplaySlices>0)
					n=line.m_uDisplaySlices;
				else
					n=CalArcResolution(line.Radius(),line.SectorAngle());
				double piece_angle=line.SectorAngle()/n;
				for(i=0;i<n;i++)
				{
					if(line.ID&0x80000000)
					{
						if(i==0)
							vertice=line.End();
						else
							vertice = line.PositionInAngle((n-i-1)*piece_angle);
					}
					else
					{
						if(i==0)
							vertice=line.Start();
						else
							vertice = line.PositionInAngle(i*piece_angle);
					}
					listVertices.AttachObject(vertice);
				}
			}
		}
		for(GEPOINT *pp=listVertices.EnumObjectFirst();pp!=NULL;pp=listVertices.EnumObjectNext())
			t.AddVertex(*pp);
		//�ڶ���Ϊ�ڻ�
		//for(pLoop=pFace->inner_loop.GetFirst();pLoop!=NULL;pLoop=pFace->inner_loop.GetNext())
		for(int loopi=0;loopi<rawface.InnerLoopNum();loopi++)
		{
			CFaceLoop innerloop=rawface.GetInnerLoopAt(loopi);
			t.ContourSeparator();	//���߽�����
			edge_n=innerloop.LoopEdgeLineNum();
			//for(pLine=pLoop->loop->GetFirst();pLine!=NULL;pLine=pLoop->loop->GetNext())
			for(ei=0;ei<edge_n;ei++)
			{
				innerloop.GetLoopEdgeLineAt(pSrcBody,ei,line);
				if(line.SectorAngle()==0)
				{
					vertice = line.Start();
					GEPOINT *pp=listVertices.AttachObject(vertice);
					t.AddVertex(*pp);
				}
				else
				{
					if(line.m_uDisplaySlices>0)
						n=line.m_uDisplaySlices;
					else
						n=CalArcResolution(line.Radius(),line.SectorAngle());
					double piece_angle=line.SectorAngle()/n;
					for(j=0;j<n;j++)
					{
						if(j==0)
							vertice=line.Start();
						else
							vertice = line.PositionInAngle(j*piece_angle);
						GEPOINT *pp=listVertices.AttachObject(vertice);
						t.AddVertex(*pp);
					}
				}
			}
		}
		t.EndDef();
		trianglesBuffer.SeekPosition(0);
		while(trianglesBuffer.GetRemnantSize()>0)
		{
			//���������淨�ߡ�������ɫ�������������Ϣ
			CGLFace *pGLFace=listFacets.AttachObject();
			pGLFace->nx=poly_norm.x;
			pGLFace->ny=poly_norm.y;
			pGLFace->nz=poly_norm.z;
			pGLFace->red   = GetRValue(rawface.MatColor())/255.0f;
			pGLFace->green = GetGValue(rawface.MatColor())/255.0f;
			pGLFace->blue  = GetBValue(rawface.MatColor())/255.0f;
			pGLFace->alpha = (GLfloat)alpha;
			CGLFace *pPrevGLFace=listFacets.EnumObjectTail();
			pGLFace->header.clr_norm=0x03;	//Ĭ�ϱ任��ɫ������
			if(pPrevGLFace!=NULL)
			{
				if( pPrevGLFace->red==pGLFace->red&&pPrevGLFace->green==pGLFace->green&&
					pPrevGLFace->blue==pGLFace->blue&&pPrevGLFace->alpha==pGLFace->alpha)
					pGLFace->header.clr_norm &= 0x02;
				if( pPrevGLFace->nx==pGLFace->nx&&pPrevGLFace->ny==pGLFace->ny&&pPrevGLFace->nz==pGLFace->nz)
					pGLFace->header.clr_norm &= 0x01;
			}
			trianglesBuffer.ReadByte(&pGLFace->header.mode);
			trianglesBuffer.ReadWord(&pGLFace->header.uVertexNum);
			pGLFace->m_pVertexCoordArr=new GLdouble[pGLFace->header.uVertexNum*3];
			trianglesBuffer.Read(pGLFace->m_pVertexCoordArr,pGLFace->header.uVertexNum*24);
		}
		WriteToSolidBuffer(solidbuf,indexFace,listFacets,rawface.MatColor(),poly_norm);
	}
	if(pDstBody)
		pDstBody->CopyBuffer(solidbuf.GetBufferPtr(),solidbuf.GetLength());
	return true;
}

//////////////////////////////////////////////////////////////////////////
//CSTLData
CSTLData::CSTLData(int iNo/*=1*/)
{
	m_iNo=iNo;
	m_FacetList.Empty();
	m_FacetList.SetSize(0, 0x200000);	//0x200000=2M����Ƭ
}
CSTLData::~CSTLData(void)
{
}
bool CSTLData::IsSTLBinary(const char *filename)
{
	bool bBinary = false;//return value
	FILE *fp = nullptr;
	int errorCode = fopen_s(&fp,filename, "r");
	if (errorCode==0)//�ɹ����ļ�
	{
		//ȷ���ļ�ʵ�ʴ�С
		fseek(fp, 0, SEEK_END);//��fp�ƶ����ļ�β��
		int fileSize = ftell(fp);//�����ĵ��ײ���fpλ�ô�С��bytes��
		int facetNum;
		//�����׼stl�������ļ��Ĵ�С
		fseek(fp, STL_LABEL_SIZE, SEEK_SET);//�����ĵ���ͷ��ע��
		fread(&facetNum, sizeof(int), 1, fp);//��ȡfacet����Ŀ��������facetNum��
		int standardBinaryFileSize = 80 + sizeof(int)+facetNum * 50;
		//�ж��Ƿ��Ǳ�׼stl�ļ�
		if (fileSize==standardBinaryFileSize)
		{
			bBinary = true;        
		}
		//�ж��Ƿ��ǷǱ�׼stl�ļ�
		unsigned char tmpbuf[128];//����ļ����̣��������Bug
		fread(tmpbuf, sizeof(tmpbuf), 1, fp);//��ȡ128��char��С������
		for (unsigned int i = 0;i<sizeof(tmpbuf);i++)
		{
			//char����ȡֵ��Χ��-128~127,�����ASCII��ʽ��Ӧ��ȫ��char
			if (tmpbuf[i]>127)
			{
				bBinary = true;
				break;
			}
		}

		fclose(fp);
	}
	return bBinary;
}

bool CSTLData::SaveStlBinary(const char *file_path)
{
	FILE *fp = fopen(file_path,"wb");
	if(fp==NULL)
		return false;
	int nFaceName=m_FacetList.GetSize();
	char fname[MAX_PATH],fileInf[MAX_PATH];
	_splitpath(file_path,NULL,NULL,fname,NULL);
	sprintf(fileInf,"%s.stl", fname);
	//
	fwrite(fileInf,sizeof(char),80,fp);
	fwrite(&nFaceName,sizeof(int),1,fp);
	for(int i=0;i<nFaceName;i++)
	{
		CSTLFacet face=	m_FacetList[i];
		float v1x = (float)face.m_PointList[0].x;
		float v1y = (float)face.m_PointList[0].y;
		float v1z = (float)face.m_PointList[0].z;
		float v2x = (float)face.m_PointList[1].x;
		float v2y = (float)face.m_PointList[1].y;
		float v2z = (float)face.m_PointList[1].z;
		float v3x = (float)face.m_PointList[2].x;
		float v3y = (float)face.m_PointList[2].y;
		float v3z = (float)face.m_PointList[2].z;
		float nx = (v1y-v3y)*(v2z-v3z)-(v1z-v3z)*(v2y-v3y);
		float ny = (v1z-v3z)*(v2x-v3x)-(v2z-v3z)*(v1x-v3x);
		float nz = (v1x-v3x)*(v2y-v3y)-(v2x-v3x)*(v1y-v3y);
		float nxyz = sqrt(nx*nx+ny*ny+nz*nz);
		float normX = nx/nxyz;
		float normY = ny/nxyz;
		float normZ = nz/nxyz;
		//
		fwrite(&normX, sizeof(float), 1, fp);
		fwrite(&normY, sizeof(float), 1, fp);
		fwrite(&normZ, sizeof(float), 1, fp);
		fwrite(&v1x, sizeof(float), 1, fp);
		fwrite(&v1y, sizeof(float), 1, fp);
		fwrite(&v1z, sizeof(float), 1, fp);
		fwrite(&v2x, sizeof(float), 1, fp);
		fwrite(&v2y, sizeof(float), 1, fp);
		fwrite(&v2z, sizeof(float), 1, fp);
		fwrite(&v3x, sizeof(float), 1, fp);
		fwrite(&v3y, sizeof(float), 1, fp);
		fwrite(&v3z, sizeof(float), 1, fp);
		fwrite("wl",sizeof(char),2,fp);
	}
	fclose(fp);
	return true;
}
bool CSTLData::SaveStlASCII(const char *file_path)
{
	FILE *fp = fopen(file_path,"wt");
	if(fp==NULL)
		return false;
	char fname[MAX_PATH],fileInf[MAX_PATH];
	_splitpath(file_path,NULL,NULL,fname,NULL);
	sprintf(fileInf,"solid %s",fname);
	//д���ļ���Ϣ
	fprintf(fp,"%s\n",fileInf);
	//д����������Ϣ
	for(int i=0;i<m_FacetList.GetSize();i++)
	{
		CSTLFacet face=	m_FacetList[i];
		float v1x = (float)face.m_PointList[0].x;
		float v1y = (float)face.m_PointList[0].y;
		float v1z = (float)face.m_PointList[0].z;
		float v2x = (float)face.m_PointList[1].x;
		float v2y = (float)face.m_PointList[1].y;
		float v2z = (float)face.m_PointList[1].z;
		float v3x = (float)face.m_PointList[2].x;
		float v3y = (float)face.m_PointList[2].y;
		float v3z = (float)face.m_PointList[2].z;
		float nx = (v1y-v3y)*(v2z-v3z)-(v1z-v3z)*(v2y-v3y);
		float ny = (v1z-v3z)*(v2x-v3x)-(v2z-v3z)*(v1x-v3x);
		float nz = (v1x-v3x)*(v2y-v3y)-(v2x-v3x)*(v1y-v3y);
		float nxyz = sqrt(nx*nx+ny*ny+nz*nz);
		//
		fprintf(fp,"facet normal %f %f %f\n",nx/nxyz,ny/nxyz,nz/nxyz);
		fprintf(fp,"outer loop\n");
		fprintf(fp,"vertex %f %f %f\n",v1x,v1y,v1z);
		fprintf(fp,"vertex %f %f %f\n",v2x,v2y,v2z);
		fprintf(fp,"vertex %f %f %f\n",v3x,v3y,v3z);
		fprintf(fp,"endloop\n");
		fprintf(fp,"endfacet\n");
	}
	sprintf(fileInf,"endsolid %s.stl  by master",fname);
	fprintf(fp,"%s\n",fileInf);
	fclose(fp);
	return true;
}
void CSTLData::AddSolidBody(char* solidbuf/*=NULL*/,DWORD size/*=0*/)
{
	//��Solid����Ϊ����������
	CSolidBody xSolid,xSolidExter(solidbuf,size);
	if(!SplitToBasicFacets(&xSolidExter,&xSolid))
		return;
	//��¼��������Ϣ
	int face_n=xSolid.BasicGLFaceNum();
	GEPOINT xarrVertexPool[16];
	for(int i=0;i<face_n;i++)
	{
		CBasicGlFace xBasicFace;
		if(!xSolid.GetBasicGLFaceAt(i,xBasicFace))
			continue;
		int cluster_n=xBasicFace.FacetClusterNumber;
		for(int j=0;j<cluster_n;j++)
		{
			CFacetCluster xFacet;
			if(!xBasicFace.GetFacetClusterAt(j,xFacet))
				continue;

			DYN_ARRAY<GEPOINT> vptrs(xFacet.VertexNumber,false, xarrVertexPool,16);
			for(int jj=0;jj<xFacet.VertexNumber;jj++)
				vptrs[jj]=xFacet.VertexAt(jj);
			for(int k=0;k<xFacet.FacetNumber;k++)
			{
				CSTLFacet* pStlFace=m_FacetList.append();
				pStlFace->m_Normal.x=xFacet.Nx;
				pStlFace->m_Normal.y=xFacet.Ny;
				pStlFace->m_Normal.z=xFacet.Nz;
				if(xFacet.Mode==GL_TRIANGLES)
				{ 
					pStlFace->m_PointList[0]=vptrs[k*3];
					pStlFace->m_PointList[1]=vptrs[k*3+1];
					pStlFace->m_PointList[2]=vptrs[k*3+2];
				}
				else if(xFacet.Mode==GL_TRIANGLE_STRIP)
				{
					if(k%2==0)
					{
						pStlFace->m_PointList[0]=vptrs[k];
						pStlFace->m_PointList[1]=vptrs[k+1];
						pStlFace->m_PointList[2]=vptrs[k+2];
					}
					else
					{
						pStlFace->m_PointList[0]=vptrs[k];
						pStlFace->m_PointList[1]=vptrs[k+2];
						pStlFace->m_PointList[2]=vptrs[k+1];
					}
				}
				else if(xFacet.Mode==GL_TRIANGLE_FAN)
				{
					pStlFace->m_PointList[0]=vptrs[k];
					pStlFace->m_PointList[1]=vptrs[k+1];
					pStlFace->m_PointList[2]=vptrs[k+2];
				}
				else
					continue;
			}
		}
	}
}
void CSTLData::SaveFile(const char* path,int nType)
{
	if(nType==TYPE_SAVE_ASCII)
		SaveStlASCII(path);
	else if(nType==TYPE_SAVE_BINARY)
		SaveStlBinary(path);
}