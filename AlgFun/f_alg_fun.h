
#ifndef __F_ALG_FUNC_H_
#define __F_ALG_FUNC_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the F_ALG_FUN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// F_ALG_FUN_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define F_ALG_FUN_API
#else
#ifdef F_ALG_FUN_EXPORTS
#define F_ALG_FUN_API __declspec(dllexport)
#else
#define F_ALG_FUN_API __declspec(dllimport)
#endif
#endif
//////////////////////-------ȫ�ֺ���������-------//////////////////////////////
#include "f_ent.h"
long F_ALG_FUN_API ftol(double fVal);
int F_ALG_FUN_API ftoi(double fVal);
double F_ALG_FUN_API fto_halfi(double fVal);
bool F_ALG_FUN_API ToFraction(double f,int* up,int* down,short maxDetectNumber=10,double tolerance=0.001);
double F_ALG_FUN_API TruncateDouble(double f,int precision);
f3dPoint F_ALG_FUN_API cross_prod(f3dPoint v1, f3dPoint v2);
double F_ALG_FUN_API dot_prod(f3dPoint v1, f3dPoint v2);
//����������Ϊ1�������ڸ��������Դ���1����acos��������
double F_ALG_FUN_API ACOS(f3dPoint v1,f3dPoint v2);
double F_ALG_FUN_API ACOS(double cosa);
//�������ڸ������㵼�²��ּ�С���������쳣�������
double F_ALG_FUN_API SQRT(double v);
// һ������ϵ
	// 1.�ж��û�ucs����ϵ�Ƿ���ȷ��������������Ƿ������������ַ���
	bool F_ALG_FUN_API is_ucs_valid(const UCS_2dSTRU&);
	bool F_ALG_FUN_API is_ucs_valid(const UCS_STRU&);
	int  F_ALG_FUN_API is_right_hand_ucs_valid(UCS_STRU ucs);
	void F_ALG_FUN_API NormalizeUcs(UCS_STRU *ucs, int axis=0);
	void F_ALG_FUN_API NormalizeUcs(UCS_2dSTRU *ucs, int axis=0);
	//modeY0orZ1=0��1���������ʸ������ָ��axis_x��ֱ���һ��ഹֱ
	GEPOINT F_ALG_FUN_API CalAxisYZByX(const double* axis_x_coords,char modeY0orZ1=1);
	/*
		nFlagΪ�ж��������־,0����X��; 1����Y��; 2����Z��
		pnt��ԭ����ϵ��ԭ�㹲ͬ����������ϵ��nFlag������������᷽��,
		�������������ת,����ԭ�㲻��
	*/
	void F_ALG_FUN_API rotate_ucs_axis(UCS_STRU &ucs, f3dPoint pnt, int nFlag);

	// 2.����ϵת�� TRUE��UCS-->WCS; FALSE��WCS-->UCS
	/*
		WCS ����:
			origin: 0,0,0;
			axis_x: 1,0,0;
			axis_y: 0,1,0;
			axis_z: 0,0,1;
	*/
	//ʸ�����������ϵת��(������ԭ���޹�)
	//TRUE��UCS-->WCS; FALSE��WCS-->UCS;bRapidMode:True��Ҫ��������᷽���������Ϊ��λ��,�һ�������
	BOOL F_ALG_FUN_API vector_trans( double* ptcoord,
								UCS_STRU ucs, BOOL fromUcs, BOOL bRapidMode=FALSE);
	BOOL F_ALG_FUN_API vector_trans( f2dPoint& pt,
								UCS_2dSTRU ucs, BOOL fromUcs);
	BOOL F_ALG_FUN_API coord_trans( double* ptcoord,
								UCS_STRU ucs, BOOL fromUcs, BOOL bRapidMode=FALSE);
	BOOL F_ALG_FUN_API coord_trans( f2dPoint& pt,
								UCS_2dSTRU ucs, BOOL fromUcs);
	//vecΪҪͶӰ��ʸ��,face_normΪͶӰ��ķ��߷���
	BOOL F_ALG_FUN_API project_vector(double* vec, const double* face_norm);
	//ptΪ��ͶӰ�ĵ�,
	//baseΪͶӰ���ϵ�һ������,face_normΪͶӰ��ķ��߷���
	BOOL F_ALG_FUN_API project_point(COORD3D& pt, COORD3D base, COORD3D face_norm);
	bool F_ALG_FUN_API ProjectPoint(double* xPointCoord, const double* xBaseCoord, const double* vPlaneNormalCoord);
			// ���黻��������������: WW 00.7.4.
			//     UCS_to_WCS( f3dPoint& pt, UCS_STRU ucs );
			//     WCS_to_UCS( f3dPoint& pt, UCS_STRU ucs );

// ������
	// 1.�ж������Ƿ��� ����:TRUE ������:FALSE Wjh 
	int F_ALG_FUN_API is_points_are_collinear(f3dPoint p1,f3dPoint p2,
										 f3dPoint p3,double epsilon=EPS);

// ����ʸ��
	// 3.������λ��
	BOOL F_ALG_FUN_API normalize(GEPOINT&);
	BOOL F_ALG_FUN_API normalize(f3dPoint&);
	BOOL F_ALG_FUN_API normalize(f2dPoint&);

	// 4.��ʸ����ļн� 0=<angle<=��
	double F_ALG_FUN_API cal_angle_of_2vec(f3dPoint v1, f3dPoint v2);

	// 5.�Ƿ�Ϊ��ʸ��
	BOOL F_ALG_FUN_API is_zero_vector(f3dPoint vec);

// �ġ�������
	/* 1.�����ά�㵽ֱ�߼���롣����ֵ{ 
			���������������
			�������������Ҳ�
			��  ��ֱ�߶������������غ� }*/
	double F_ALG_FUN_API 
	DistOf2dPtRay( double polarX,double polarY,double cosa,double sina,double pickX,double pickY);
	//��άƽ���µ����߼�Ĺ�ϵ�жϣ�>0:���������;<0:�������Ҳ�;0:��������
	short F_ALG_FUN_API SideOfPtLine(const double* pick,const double* lineStart,const double* lineEnd,double tolerance=EPS);
	double F_ALG_FUN_API 
	DistOf2dPtLine( const double* pt_coord2d,const double* start_coord2d,const double* end_coord2d);
	double F_ALG_FUN_API DistOf2dPtLine(double orgx,double orgy,
		double startx,double starty,double endx,double endy,double* scaleInLineS2E=NULL);

	// 3.������ά�㵽ֱ�߼�Ŀռ����(����)��
	double F_ALG_FUN_API 
	DistOf3dPtLine( const f3dPoint& pt,
					const f3dPoint& start,
					const f3dPoint& end,double epsilon=EPS);
	// 4.������άֱ�߼����̾���(���ڴ�������ֱ�߼��󹫴��߳�)
	double F_ALG_FUN_API 
	DistOf3dll( const f3dLine& L1, const f3dLine& L2);
	double F_ALG_FUN_API 
	DistOf3dll( const f3dPoint &start0, const f3dPoint &end0, 
				const f3dPoint &start1, const f3dPoint &end1);
	// 5.��pp����p1--p2��ʱ����תangle�Ƕ�
	BOOL F_ALG_FUN_API rotate_point_around_axis( 
		double* pp_coord,double rot_ang,f3dPoint p1,f3dPoint p2);
	BOOL F_ALG_FUN_API rotate_point_around_axis(
		double* pp_coord, double rot_ang, f3dLine axis);
	BOOL F_ALG_FUN_API RotateVectorAroundVector(double* dest_vec, double angle, const double* axis);
	BOOL F_ALG_FUN_API RotateVectorAroundVector(double* dest_vec, double sina, double cosa, const double* axis);
	//|axis|Ϊ��ת��,������
	BOOL F_ALG_FUN_API RotateVectorAroundVector(double* dest_vec, const double* axis);
	double F_ALG_FUN_API CalRotateAngle(f3dPoint rot_axis,f3dPoint srcRotVec,f3dPoint destRotVec);
	//���һ��ָ����ת�ı任����
	BOOL F_ALG_FUN_API GetLTransMatrixOfRotate(double rot_ang,f3dPoint axis_vec,double *matrix);


// �塢������
	// 1.����ֱ��(��)�����(���ˮƽX��)//����ֵΪ0=<angle<2��
	double F_ALG_FUN_API 
	Cal2dLineAng( double start_x, double start_y,
				  double end_x,   double end_y);
	double F_ALG_FUN_API
	Cal2dLineAng(f2dPoint start, f2dPoint end);
	double F_ALG_FUN_API 
	Cal2dLineAng(f2dLine line);

	/*ֱ����Բ�󽻵�
	return value:
		-1:ֱ�߻�Բ��������
		 0:ֱ����Բ�޽���
		 1:�ҵ�һ����Ч����
		 2:�ҵ�������Ч����
	*/
	F_ALG_FUN_API int Int2dlc(f2dLine L, f2dCircle Cir,
					   double&P1X,double&P1Y,double&P2X,double&P2Y);
	/*ֱ����Բ�󽻵�
	return value:
		 0:ֱ����Բ�޽���
		 1:�������غ�Ϊһ���е�
		 2:�ҵ�������Ч����
		-1:ֱ�߻�Բ����������
	*/
	F_ALG_FUN_API int Int2dplc(f2dLine L, f2dCircle Cir,
						   double&P1X,double&P1Y,double&P2X,double&P2Y);
	/*ֱ����Բ���󽻵�
	return value:
		-1:ֱ�߻�Բ����������
		 0:ֱ����Բ���޽���
		 1:�ҵ�һ����Ч����
		 2:�ҵ�������Ч����
	*/
	F_ALG_FUN_API int Int2dla(f2dLine L,f2dArc Arc,
						   double&P1X,double&P1Y,double&P2X,double&P2Y);
	/*ֱ������Բ�󽻵�
	long_axis��ʾ��Բ���᷽��mod(long_axis)������뾶,long_axis=NULLʱ�˻�Ϊ��׼Բ��
	return value:
		-1;	//���������������
		 0;	//ֱ������Բ���޽���
		 1;	//ֱ������Բ����1������
		 2;	//ֱ������Բ����2������
	*/
	F_ALG_FUN_API int Int2dle(double* inters2d1,double* inters2d2,double* line_start,double* line_end,
		double* ellipse_center,double* arc_start,double short_radius,double arc_angle,double* long_axis);
	/*Բ��Բ�󽻵�
	return value:
		-1;	//���������������
		 0;	//��Բ�����Ϊͬ��Բ���޽���
		 1;	//��Բ���У���1������
		 2;	//��Բ�ཻ����2������
	*/
	F_ALG_FUN_API int Int2dcc(double* inters2d1,double* inters2d2,
		double* arc_center1,double radius1,double* arc_center2,double radius2);
	/*Բ����Բ�󽻵�
	return value:
		-1;	//���������������
		 0;	//��Բ�����Ϊͬ��Բ���޽���
		 1;	//��Բ���У���1������
		 2;	//��Բ�ཻ����2������
	*/
	F_ALG_FUN_API int Int2dce(double* inters2d1,double* inters2d2,double* inters2d3,double* inters2d4,
		double* arc_center1,double radius1,double* ellipse_center,double ellipse_radius,double* long_axis);
	////////////////////////////////////////////////////////////////////
	//-----------------00.03.21Wjh,���������к���---------------------//

	/*����Բ���е�
	return value:
		0:�Ϸ��е㲻����
		1:�ҵ�һ�Ϸ��е�
	*/
	F_ALG_FUN_API int Tang2dpc(f2dPoint basePt, 
							f2dCircle cir, f2dPoint pickPt, f2dPoint& retPt);
	/*����Բ�����е�
	return value:
		0:�Ϸ��е㲻����
		1:�ҵ�һ�Ϸ��е�
	*/
	F_ALG_FUN_API int Tang2dpa(f2dPoint basePt, 
							f2dArc arc,f2dPoint pickPt, f2dPoint& retPt);
	/*Բ����Բ��������
	pick1��pick2Ϊ����ʰȡ�㣬
	pick1Ӧ��������Բ1�϶˵�λ��Բ1��Բ2Բ������ͬһ��
	pick2Ӧ��������Բ2�϶˵�λ��Բ1��Բ2Բ������ͬһ��
	return value:
	   -1:Բ��������
		0:�Ϸ����߲�����,�޹����ߣ���Բ���׻��ཻ
		1:�ҵ�һ�Ϸ�����
	*/
	F_ALG_FUN_API int TangLine2dcc(f2dCircle cir1,f2dPoint pick1, 
							f2dCircle arc,f2dPoint pick2, f2dLine& tan_line);
	/* 2.��άֱ�߶���ֱ�߶��󽻵�
	����ֵ��
		-2��//������Ч��ֱ����㣩
		-1��//ֱ�߶˵��غϣ���������
		 0��//L1��L2ƽ�л��غ�
		 1��//����Ϊ��ֱ�߶ε��ڵ�
		 2��//����Ϊֱ�߶ζ˵�
	*/
	int F_ALG_FUN_API Int2dll(f2dLine L1, f2dLine L2,double &PX,double &PY);
	/*��ֱ��(intersection of 2d pure line)
		-1��//ֱ�߶˵��غϣ���������
		 0��//L1��L2ƽ�л��غ�
		 1��//����Ϊ��ֱ�߶ε��ڵ�
	�󽻲��жϽ����Ƿ���Ч(�����߶���)*/
	int F_ALG_FUN_API Int2dpl(f2dLine L1, f2dLine L2,double &PX,double &PY);
	/* 2.��άֱ�߶��������󽻵�
	����ֵ��
		-2��//������Ч��ֱ����㣩
		-1��//ֱ�߶˵��غϣ���������
		 0��//Line��Rayƽ�л��غ�
		 1��//����Ϊֱ�߶ε��ڵ�
		 2��//����Ϊֱ�߶ζ˵�
		 3��//����Ϊ������ʼ��
	*/
	int F_ALG_FUN_API Int2dlr(f2dLine line, f2dRay ray,double &PX,double &PY);

	/*3.��άֱ�߶���ֱ�߶��󽻵�
	����ֵ��
		-2��//������Ч��ֱ����㣩
		-1��//ֱ�߶˵��غϣ���������
		 0��//L1��L2ƽ�С��غϻ�����
		 1��//����Ϊ��ֱ�߶ε��ڵ�
		 2��//����Ϊֱ�߶ζ˵�
	
	*/
	int F_ALG_FUN_API Int3dll( const f3dPoint start0, const f3dPoint end0,
							const f3dPoint start1, const f3dPoint end1, COORD3D& pt,double TOLERANCE=EPS);
	int F_ALG_FUN_API Int3dll(const f3dLine& L1, const f3dLine& L2,COORD3D& pt,double TOLERANCE=EPS);
	int F_ALG_FUN_API Int3dll(const f3dAtomLine& L1, const f3dAtomLine& L2,COORD3D& pt,double TOLERANCE=EPS);
	//����ֱ���󽻣��󽻽����֤�����ڵ�һ��ֱ���ϣ�cPrjAxis=0:ͶӰ����Ϊ��ֱ�߲����; cPrjAxis='X'��'Y'��'Z'
	//����ֵ��0:L1��L2ƽ�л��غϻ�ֱ�߶�������; 1:��ȡ��Ч���㣬�ҽ�����L1֮�ڵ㣻2:��Ч����ΪL1֮�˵㣻3:����ΪL1��L2ͶӰ�����
	int F_ALG_FUN_API Int3dDifferPlaneLL( const f3dPoint start0, const f3dPoint end0,
							const f3dPoint start1, const f3dPoint end1, COORD3D& pt,char cPrjAxis=0);
	/*��ֱ��(intersection of 3d pure line)
		-1��//ֱ�߶˵��غϣ���������
		 0��//L1��L2ƽ�С��غϻ�����
		 1��//����Ϊ��ֱ�߶ε��ڵ�
	//�󽻲��жϽ����Ƿ���Ч(�����߶���)*/
	//int F_ALG_FUN_API Int3dpl(const f3dLine& L0, const f3dLine& L1,f3dPoint &pt);
	int F_ALG_FUN_API Int3dpl(f3dLine L0, f3dLine L1,COORD3D &pt,double TOLERANCE=EPS);
	/* ��άֱ�߶��������󽻵�
	����ֵ��
		-2��//������Ч��ֱ����㣩
		-1��//ֱ�߶˵��غϣ���������
		 0��//Line��Rayƽ�С��غϻ�����
		 1��//����Ϊֱ�߶ε��ڵ�
		 2��//����Ϊֱ�߶ζ˵�
		 3��//����Ϊ������ʼ��
	*/
	int F_ALG_FUN_API Int3dlr(f3dLine line, f3dRay ray,COORD3D &pt,UCS_STRU *pUCS=NULL);
	
	//��ֱ��L1�������ϵ�һ��pick���淨��normȷ������֮��Ľ���
	//����ֵ -1:ֱ�߻��涨������0:������ƽ�л��غ� 1:�õ���Ч����
	int F_ALG_FUN_API Int3dlf(double *inters,const double *line_pick,const double *line_vec,
		const double *face_pick,const double *face_norm);
	int F_ALG_FUN_API Int3dlf(double *inters,f3dLine line, f3dPoint face_pick,f3dPoint face_norm);

	//��ֱ��L1��Բ��(׶)��֮��Ľ���
	//����ֵ -1:ֱ�߻�Բ��(׶)�涨������0:����Բ��(׶)��ƽ�л��޽��� 1:�õ�1����Ч����2:�õ�2����Ч����
	int F_ALG_FUN_API Int3dlc(COORD3D *inters1,COORD3D *inters2,
		f3dPoint line_pick,f3dPoint line_vec,
		f3dPoint axis_pick_start,f3dPoint axis_pick_end,double radius_start,double radius_end);
	//����ֵ:
	//-1: �涨������ 0:��ƽ�л��غ� 1:���һ��Ч����
	int F_ALG_FUN_API Int3d3f(f3dPoint *inters,
		f3dPoint face1_pick,f3dPoint face1_norm,
		f3dPoint face2_pick,f3dPoint face2_norm,
		f3dPoint face3_pick,f3dPoint face3_norm);
	//����ֵ:
	//-1: �涨������ 0:��ƽ�л��غ� 1:���һ��Ч����
	//inters_pick:�����ϵ���һʰȡ��
	//inters_line_vec:(��λ������ʸ������)
	int F_ALG_FUN_API Int3d2f(f3dPoint *inters_pick,f3dPoint *inters_line_vec,
		f3dPoint face1_pick,f3dPoint face1_norm,f3dPoint face2_pick,f3dPoint face2_norm);
	int F_ALG_FUN_API _Int3d2f(double *inters_pick,double *inters_line_vec,
		const double* vPlanePick1,const double* vPlaneNorm1,const double* vPlanePick2,const double* vPlaneNorm2);
	//�жϸ���ֱ���Ƿ��뵱ǰֱ�ߴ�ֱ(�����������), perpΪ���صĸ���ֱ���ϵĴ����
	BOOL F_ALG_FUN_API IsPerpToLine(f3dLine line,f3dLine oth_line, f3dPoint &perp,double epsilon=eps);

// ������
	/*��׽�㺯��
	ptx,pty					Ϊ��׽ʰȡ��
	originx,originy			��Ϊ����׽��
	scope					Ϊ��׽���뷶Χ,
	dist					Ϊʰȡ�������׽���ľ���
	*/
	F_ALG_FUN_API BOOL 
	Snap2dPnt ( f2dPoint pt, f2dPoint origin,
				double scope ,double &dist);
	F_ALG_FUN_API BOOL 
	Snap2dPnt ( double ptx,	  double pty,
				double originx,double originy,
				double scope ,double &dist);
	/*��׽ֱ�ߺ���
	ptx,pty					Ϊ��׽ʰȡ��
	startx,starty;endx,endy	��Ϊֱ�ߵ���ʼ�����ֹ��
	scope					Ϊ��׽���뷶Χ,
	dist					Ϊʰȡ����ֱ�߼�Ĵ�ֱ����
	*/
	F_ALG_FUN_API BOOL 
	Snap2dLine( f2dPoint pt, f2dLine line,
				double scope ,double &dist);
	F_ALG_FUN_API BOOL 
	Snap2dLine( double ptx,	  double pty,
				double startx,double starty,
				double endx  ,double endy,
				double scope ,double &dist);
	//���Ƕȹ淶��0��2*Pi��Χ��
	F_ALG_FUN_API double normalizeRadAngle(double radAngle);
	/*��׽Բ������
	pt						Ϊ��׽ʰȡ��
	arc						����׽Բ
	scope					Ϊ��׽���뷶Χ,
	dist					Ϊʰȡ����Բ����Ĵ�ֱ����
	*/
	F_ALG_FUN_API BOOL 
	Snap2dArcLine( f2dPoint pt, f2dArc arc,
				double scope ,double &dist);
	/*��׽Բ����
	pt						Ϊ��׽ʰȡ��
	circle					����׽Բ
	scope					Ϊ��׽���뷶Χ,
	dist					Ϊʰȡ����Բ����Ĵ�ֱ����
	*/
	F_ALG_FUN_API BOOL 
	Snap2dCircle( f2dPoint pt, f2dCircle circle,
				double scope ,double &dist);
	/*��׽��Բ������
	Ĭ����Բ����������ԭ�㳤����X�᷽��
	pt						Ϊ��׽ʰȡ��
	majorRadius				����뾶
	minorRadius				����뾶
	scope					Ϊ��׽���뷶Χ,
	dist					Ϊʰȡ����Բ����Ĵ�ֱ����
	startAngle				��Բ������ʼ��
	endAngle				��Բ������ֹ��
	*/
	F_ALG_FUN_API BOOL 
	Snap2dEllipseArc( f2dPoint pt,double majorRadius,
				double minorRadius,double scope ,double *dist=NULL,double startAngle=0,double endAngle=2*Pi);
	/*��׽���κ���
	pt						Ϊ��׽ʰȡ��
	rect					����׽����
	scope					Ϊ��׽���뷶Χ,
	dist					Ϊʰȡ������μ�Ĵ�ֱ����
	*/
	F_ALG_FUN_API BOOL 
	Snap2dRect( f2dPoint pt, f2dRect rect,
				double scope ,double &dist);

	//����һ����ʸ��vec����ͬ����ʸ��
	F_ALG_FUN_API f3dPoint inters_vec(const double* pxInitialVec);
	// ��������ȷ���û�����ϵ		--�����
	// ���룻origin(ԭ��), axis_pt(�����᷽��ȷ����)
	// AXIS_X0Y1Z2��ʾ�����axis_pt������ʾ�û�����ϵ���������ʶ0:X��1:Y��2:Z��
	// �������ȷ���� TRUE ���󷵻� FALSE
	//		 ucs  ������ȷ�����û�����ϵ
	F_ALG_FUN_API BOOL CalCSBy2Pt(f3dPoint origin, UCS_STRU &ucs, f3dPoint axis_pt,int AXIS_X0Y1Z2=0);
	//����һ����ֱ��start-->end���ɼ�ȡ��pickȷ����Ψһ�����
	F_ALG_FUN_API BOOL SnapPerp(COORD3D* perp, f3dLine line,const double* pick, double* pDist=NULL);
	//�ò���COORD3D&����δ��const double* ��Ϊ�˱��������Ķ�ά�����س�ͻ wjh-2019.8.26
	F_ALG_FUN_API BOOL SnapPerp(COORD3D* perp, const COORD3D& start, const COORD3D& end,
						const COORD3D& pick, double* pDist=NULL);
	F_ALG_FUN_API BOOL SnapPerp(f2dPoint* perp, f2dPoint start, f2dPoint end,
						f2dPoint pick, double* pDist=NULL);
	F_ALG_FUN_API BOOL SnapPerp(f2dPoint* perp, f2dLine line,
						f2dPoint pick, double* pDist=NULL);
	//�������µĵ�:
	//�����Ե�origin��vec�����ƶ�offset�ľ���,Ȼ������direct�ķ����ƶ�dist�ľ������õ��ĵ�
	F_ALG_FUN_API f3dPoint get_pt_by_vec( f3dPoint vec, f3dPoint direct, 
				f3dPoint origin, double offset, double dist);
	//angleΪֱ����X-Yƽ���ϵ�ͶӰ����X������ļнǣ�
	//�ʴ˲���Ҳֻ���ڴ�X-Yƽ���Ͻ��е�
	F_ALG_FUN_API f3dPoint GetPtInPolar(f3dPoint base, double angle,double dist);
	//�Ƕ�ת�� rad-->degree
	F_ALG_FUN_API double RadToDegree(double rad);
	//�Ƕ�ת�� degree-->rad
	F_ALG_FUN_API double DegreeToRad(double deg);
	F_ALG_FUN_API void LoadDefaultUCS(UCS_STRU *ucs);
	F_ALG_FUN_API UCS_STRU LoadDefaultUCS(const f3dPoint& origin);
	F_ALG_FUN_API void ReverseUCS(UCS_STRU *ucs);
	F_ALG_FUN_API void ReverseUCS(GECS *ucs);
	//��һ��Ϊ��׼������ĳ������ĵȾ��ƫ�Ƶ�
	F_ALG_FUN_API f3dPoint OffsetBy2Vec(f3dPoint offset_origin,f3dPoint vec1,f3dPoint vec2,double offset_dist1=0.0,double offset_dist2=0.0);
	//����ƽ��������������������������ݺ��׹�ʽarea=sqrt(s*(s-a)*(s-b)*(s-c));s=(a+b+c)/2).
	F_ALG_FUN_API double CalTriArea(double p1x,double p1y,double p2x,double p2y,double p3x,double p3y);
	struct PLANAR_PLATE{
		GECS cs;
		double thick;
		int vertex_count;
		GEPOINT2D* vertex_arr;
	};
	//�������͹�����ƽ��֮���Ƿ�����ײ�����
	F_ALG_FUN_API bool IsIntersPlates(PLANAR_PLATE& plate1, PLANAR_PLATE& plate2,double intervene_space);
	//���͹�����ƽ����Բ���壨��ģ����˨��֮���Ƿ�����ײ�����
	F_ALG_FUN_API bool IsIntersPlateAndColumn(PLANAR_PLATE& plate,
		f3dPoint axis_pick_start,f3dPoint axis_pick_end,double radius,double intervene_space);

#endif