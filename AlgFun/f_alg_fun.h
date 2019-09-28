
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
//////////////////////-------全局函数声明区-------//////////////////////////////
#include "f_ent.h"
long F_ALG_FUN_API ftol(double fVal);
int F_ALG_FUN_API ftoi(double fVal);
double F_ALG_FUN_API fto_halfi(double fVal);
bool F_ALG_FUN_API ToFraction(double f,int* up,int* down,short maxDetectNumber=10,double tolerance=0.001);
double F_ALG_FUN_API TruncateDouble(double f,int precision);
f3dPoint F_ALG_FUN_API cross_prod(f3dPoint v1, f3dPoint v2);
double F_ALG_FUN_API dot_prod(f3dPoint v1, f3dPoint v2);
//避免大量点积为1，但由于浮点运算稍大于1导致acos出错的情况
double F_ALG_FUN_API ACOS(f3dPoint v1,f3dPoint v2);
double F_ALG_FUN_API ACOS(double cosa);
//避免由于浮点运算导致部分极小负数开放异常问题出错
double F_ALG_FUN_API SQRT(double v);
// 一、坐标系
	// 1.判断用户ucs坐标系是否正确合理，即检查三轴是否正交，及右手法则。
	bool F_ALG_FUN_API is_ucs_valid(const UCS_2dSTRU&);
	bool F_ALG_FUN_API is_ucs_valid(const UCS_STRU&);
	int  F_ALG_FUN_API is_right_hand_ucs_valid(UCS_STRU ucs);
	void F_ALG_FUN_API NormalizeUcs(UCS_STRU *ucs, int axis=0);
	void F_ALG_FUN_API NormalizeUcs(UCS_2dSTRU *ucs, int axis=0);
	//modeY0orZ1=0或1计算出的两矢量都与指定axis_x垂直，且互相垂直
	GEPOINT F_ALG_FUN_API CalAxisYZByX(const double* axis_x_coords,char modeY0orZ1=1);
	/*
		nFlag为判断坐标轴标志,0代表X轴; 1代表Y轴; 2代表Z轴
		pnt和原坐标系的原点共同决定新坐标系中nFlag所代表的坐标轴方向,
		其余两轴跟随旋转,坐标原点不变
	*/
	void F_ALG_FUN_API rotate_ucs_axis(UCS_STRU &ucs, f3dPoint pnt, int nFlag);

	// 2.坐标系转换 TRUE：UCS-->WCS; FALSE：WCS-->UCS
	/*
		WCS 定义:
			origin: 0,0,0;
			axis_x: 1,0,0;
			axis_y: 0,1,0;
			axis_z: 0,0,1;
	*/
	//矢量方向的坐标系转换(与坐标原点无关)
	//TRUE：UCS-->WCS; FALSE：WCS-->UCS;bRapidMode:True，要求各坐标轴方向分量必须为单位化,且互相正交
	BOOL F_ALG_FUN_API vector_trans( double* ptcoord,
								UCS_STRU ucs, BOOL fromUcs, BOOL bRapidMode=FALSE);
	BOOL F_ALG_FUN_API vector_trans( f2dPoint& pt,
								UCS_2dSTRU ucs, BOOL fromUcs);
	BOOL F_ALG_FUN_API coord_trans( double* ptcoord,
								UCS_STRU ucs, BOOL fromUcs, BOOL bRapidMode=FALSE);
	BOOL F_ALG_FUN_API coord_trans( f2dPoint& pt,
								UCS_2dSTRU ucs, BOOL fromUcs);
	//vec为要投影的矢量,face_norm为投影面的法线方向
	BOOL F_ALG_FUN_API project_vector(double* vec, const double* face_norm);
	//pt为待投影的点,
	//base为投影面上的一个基点,face_norm为投影面的法线方向
	BOOL F_ALG_FUN_API project_point(COORD3D& pt, COORD3D base, COORD3D face_norm);
	bool F_ALG_FUN_API ProjectPoint(double* xPointCoord, const double* xBaseCoord, const double* vPlaneNormalCoord);
			// 建议换成以下两个函数: WW 00.7.4.
			//     UCS_to_WCS( f3dPoint& pt, UCS_STRU ucs );
			//     WCS_to_UCS( f3dPoint& pt, UCS_STRU ucs );

// 二、点
	// 1.判断三点是否共线 共线:TRUE 不共线:FALSE Wjh 
	int F_ALG_FUN_API is_points_are_collinear(f3dPoint p1,f3dPoint p2,
										 f3dPoint p3,double epsilon=EPS);

// 三、矢量
	// 3.向量单位化
	BOOL F_ALG_FUN_API normalize(GEPOINT&);
	BOOL F_ALG_FUN_API normalize(f3dPoint&);
	BOOL F_ALG_FUN_API normalize(f2dPoint&);

	// 4.两矢量间的夹角 0=<angle<=π
	double F_ALG_FUN_API cal_angle_of_2vec(f3dPoint v1, f3dPoint v2);

	// 5.是否为零矢量
	BOOL F_ALG_FUN_API is_zero_vector(f3dPoint vec);

// 四、点与线
	/* 1.计算二维点到直线间距离。返回值{ 
			正数：点在线左侧
			负数：点在线右侧
			零  ：直线定义有误或点线重合 }*/
	double F_ALG_FUN_API 
	DistOf2dPtRay( double polarX,double polarY,double cosa,double sina,double pickX,double pickY);
	//二维平面下点与线间的关系判断，>0:点在线左侧;<0:点在线右侧;0:点在线上
	short F_ALG_FUN_API SideOfPtLine(const double* pick,const double* lineStart,const double* lineEnd,double tolerance=EPS);
	double F_ALG_FUN_API 
	DistOf2dPtLine( const double* pt_coord2d,const double* start_coord2d,const double* end_coord2d);
	double F_ALG_FUN_API DistOf2dPtLine(double orgx,double orgy,
		double startx,double starty,double endx,double endy,double* scaleInLineS2E=NULL);

	// 3.计算三维点到直线间的空间距离(恒正)。
	double F_ALG_FUN_API 
	DistOf3dPtLine( const f3dPoint& pt,
					const f3dPoint& start,
					const f3dPoint& end,double epsilon=EPS);
	// 4.计算三维直线间的最短距离(用于处理异面直线间求公垂线长)
	double F_ALG_FUN_API 
	DistOf3dll( const f3dLine& L1, const f3dLine& L2);
	double F_ALG_FUN_API 
	DistOf3dll( const f3dPoint &start0, const f3dPoint &end0, 
				const f3dPoint &start1, const f3dPoint &end1);
	// 5.点pp绕线p1--p2逆时针旋转angle角度
	BOOL F_ALG_FUN_API rotate_point_around_axis( 
		double* pp_coord,double rot_ang,f3dPoint p1,f3dPoint p2);
	BOOL F_ALG_FUN_API rotate_point_around_axis(
		double* pp_coord, double rot_ang, f3dLine axis);
	BOOL F_ALG_FUN_API RotateVectorAroundVector(double* dest_vec, double angle, const double* axis);
	BOOL F_ALG_FUN_API RotateVectorAroundVector(double* dest_vec, double sina, double cosa, const double* axis);
	//|axis|为旋转角,弧度制
	BOOL F_ALG_FUN_API RotateVectorAroundVector(double* dest_vec, const double* axis);
	double F_ALG_FUN_API CalRotateAngle(f3dPoint rot_axis,f3dPoint srcRotVec,f3dPoint destRotVec);
	//获得一次指定旋转的变换矩阵
	BOOL F_ALG_FUN_API GetLTransMatrixOfRotate(double rot_ang,f3dPoint axis_vec,double *matrix);


// 五、线与线
	// 1.计算直线(段)的倾角(相对水平X轴)//返回值为0=<angle<2π
	double F_ALG_FUN_API 
	Cal2dLineAng( double start_x, double start_y,
				  double end_x,   double end_y);
	double F_ALG_FUN_API
	Cal2dLineAng(f2dPoint start, f2dPoint end);
	double F_ALG_FUN_API 
	Cal2dLineAng(f2dLine line);

	/*直线与圆求交点
	return value:
		-1:直线或圆定义有误
		 0:直线与圆无交点
		 1:找到一个有效交点
		 2:找到两个有效交点
	*/
	F_ALG_FUN_API int Int2dlc(f2dLine L, f2dCircle Cir,
					   double&P1X,double&P1Y,double&P2X,double&P2Y);
	/*直线与圆求交点
	return value:
		 0:直线与圆无交点
		 1:两交点重合为一个切点
		 2:找到两个有效交点
		-1:直线或圆弧定义有误
	*/
	F_ALG_FUN_API int Int2dplc(f2dLine L, f2dCircle Cir,
						   double&P1X,double&P1Y,double&P2X,double&P2Y);
	/*直线与圆弧求交点
	return value:
		-1:直线或圆弧定义有误
		 0:直线与圆弧无交点
		 1:找到一个有效交点
		 2:找到两个有效交点
	*/
	F_ALG_FUN_API int Int2dla(f2dLine L,f2dArc Arc,
						   double&P1X,double&P1Y,double&P2X,double&P2Y);
	/*直线与椭圆求交点
	long_axis表示椭圆长轴方向，mod(long_axis)＝长轴半径,long_axis=NULL时退化为标准圆弧
	return value:
		-1;	//输入参数定义有误
		 0;	//直线与椭圆弧无交点
		 1;	//直线与椭圆弧有1个交点
		 2;	//直线与椭圆弧有2个交点
	*/
	F_ALG_FUN_API int Int2dle(double* inters2d1,double* inters2d2,double* line_start,double* line_end,
		double* ellipse_center,double* arc_start,double short_radius,double arc_angle,double* long_axis);
	/*圆与圆求交点
	return value:
		-1;	//输入参数定义有误
		 0;	//两圆相离或为同心圆，无交点
		 1;	//两圆相切，有1个交点
		 2;	//两圆相交，有2个交点
	*/
	F_ALG_FUN_API int Int2dcc(double* inters2d1,double* inters2d2,
		double* arc_center1,double radius1,double* arc_center2,double radius2);
	/*圆与椭圆求交点
	return value:
		-1;	//输入参数定义有误
		 0;	//两圆相离或为同心圆，无交点
		 1;	//两圆相切，有1个交点
		 2;	//两圆相交，有2个交点
	*/
	F_ALG_FUN_API int Int2dce(double* inters2d1,double* inters2d2,double* inters2d3,double* inters2d4,
		double* arc_center1,double radius1,double* ellipse_center,double ellipse_radius,double* long_axis);
	////////////////////////////////////////////////////////////////////
	//-----------------00.03.21Wjh,下面是求切函数---------------------//

	/*点与圆求切点
	return value:
		0:合法切点不存在
		1:找到一合法切点
	*/
	F_ALG_FUN_API int Tang2dpc(f2dPoint basePt, 
							f2dCircle cir, f2dPoint pickPt, f2dPoint& retPt);
	/*点与圆弧求切点
	return value:
		0:合法切点不存在
		1:找到一合法切点
	*/
	F_ALG_FUN_API int Tang2dpa(f2dPoint basePt, 
							f2dArc arc,f2dPoint pickPt, f2dPoint& retPt);
	/*圆弧与圆弧求公切线
	pick1及pick2为切线拾取点，
	pick1应与切线在圆1上端点位于圆1与圆2圆心连线同一侧
	pick2应与切线在圆2上端点位于圆1与圆2圆心连线同一侧
	return value:
	   -1:圆定义有误
		0:合法切线不存在,无公切线，两圆相套或相交
		1:找到一合法切线
	*/
	F_ALG_FUN_API int TangLine2dcc(f2dCircle cir1,f2dPoint pick1, 
							f2dCircle arc,f2dPoint pick2, f2dLine& tan_line);
	/* 2.二维直线段与直线段求交点
	返回值：
		-2：//交点无效（直线外点）
		-1：//直线端点重合，定义有误
		 0：//L1与L2平行或重合
		 1：//交点为两直线段的内点
		 2：//交点为直线段端点
	*/
	int F_ALG_FUN_API Int2dll(f2dLine L1, f2dLine L2,double &PX,double &PY);
	/*纯直线(intersection of 2d pure line)
		-1：//直线端点重合，定义有误
		 0：//L1与L2平行或重合
		 1：//交点为两直线段的内点
	求交不判断交点是否有效(即在线段内)*/
	int F_ALG_FUN_API Int2dpl(f2dLine L1, f2dLine L2,double &PX,double &PY);
	/* 2.二维直线段与射线求交点
	返回值：
		-2：//交点无效（直线外点）
		-1：//直线端点重合，定义有误
		 0：//Line与Ray平行或重合
		 1：//交点为直线段的内点
		 2：//交点为直线段端点
		 3：//交点为射线起始点
	*/
	int F_ALG_FUN_API Int2dlr(f2dLine line, f2dRay ray,double &PX,double &PY);

	/*3.三维直线段与直线段求交点
	返回值：
		-2：//交点无效（直线外点）
		-1：//直线端点重合，定义有误
		 0：//L1与L2平行、重合或异面
		 1：//交点为两直线段的内点
		 2：//交点为直线段端点
	
	*/
	int F_ALG_FUN_API Int3dll( const f3dPoint start0, const f3dPoint end0,
							const f3dPoint start1, const f3dPoint end1, COORD3D& pt,double TOLERANCE=EPS);
	int F_ALG_FUN_API Int3dll(const f3dLine& L1, const f3dLine& L2,COORD3D& pt,double TOLERANCE=EPS);
	int F_ALG_FUN_API Int3dll(const f3dAtomLine& L1, const f3dAtomLine& L2,COORD3D& pt,double TOLERANCE=EPS);
	//异面直线求交，求交结果保证交点在第一条直线上，cPrjAxis=0:投影求交面为两直线叉积面; cPrjAxis='X'、'Y'、'Z'
	//返回值，0:L1与L2平行或重合或直线定义有误; 1:获取有效交点，且交点在L1之内点；2:有效交点为L1之端点；3:交点为L1或L2投影线外点
	int F_ALG_FUN_API Int3dDifferPlaneLL( const f3dPoint start0, const f3dPoint end0,
							const f3dPoint start1, const f3dPoint end1, COORD3D& pt,char cPrjAxis=0);
	/*纯直线(intersection of 3d pure line)
		-1：//直线端点重合，定义有误
		 0：//L1与L2平行、重合或异面
		 1：//交点为两直线段的内点
	//求交不判断交点是否有效(即在线段内)*/
	//int F_ALG_FUN_API Int3dpl(const f3dLine& L0, const f3dLine& L1,f3dPoint &pt);
	int F_ALG_FUN_API Int3dpl(f3dLine L0, f3dLine L1,COORD3D &pt,double TOLERANCE=EPS);
	/* 三维直线段与射线求交点
	返回值：
		-2：//交点无效（直线外点）
		-1：//直线端点重合，定义有误
		 0：//Line与Ray平行、重合或异面
		 1：//交点为直线段的内点
		 2：//交点为直线段端点
		 3：//交点为射线起始点
	*/
	int F_ALG_FUN_API Int3dlr(f3dLine line, f3dRay ray,COORD3D &pt,UCS_STRU *pUCS=NULL);
	
	//求直线L1与由面上的一点pick及面法线norm确定的面之间的交点
	//返回值 -1:直线或面定义有误0:线与面平行或重合 1:得到有效交点
	int F_ALG_FUN_API Int3dlf(double *inters,const double *line_pick,const double *line_vec,
		const double *face_pick,const double *face_norm);
	int F_ALG_FUN_API Int3dlf(double *inters,f3dLine line, f3dPoint face_pick,f3dPoint face_norm);

	//求直线L1与圆柱(锥)面之间的交点
	//返回值 -1:直线或圆柱(锥)面定义有误0:线与圆柱(锥)面平行或无交点 1:得到1个有效交点2:得到2个有效交点
	int F_ALG_FUN_API Int3dlc(COORD3D *inters1,COORD3D *inters2,
		f3dPoint line_pick,f3dPoint line_vec,
		f3dPoint axis_pick_start,f3dPoint axis_pick_end,double radius_start,double radius_end);
	//返回值:
	//-1: 面定义有误 0:面平行或重合 1:求得一有效交点
	int F_ALG_FUN_API Int3d3f(f3dPoint *inters,
		f3dPoint face1_pick,f3dPoint face1_norm,
		f3dPoint face2_pick,f3dPoint face2_norm,
		f3dPoint face3_pick,f3dPoint face3_norm);
	//返回值:
	//-1: 面定义有误 0:面平行或重合 1:求得一有效交线
	//inters_pick:交线上的任一拾取点
	//inters_line_vec:(单位化交线矢量方向)
	int F_ALG_FUN_API Int3d2f(f3dPoint *inters_pick,f3dPoint *inters_line_vec,
		f3dPoint face1_pick,f3dPoint face1_norm,f3dPoint face2_pick,f3dPoint face2_norm);
	int F_ALG_FUN_API _Int3d2f(double *inters_pick,double *inters_line_vec,
		const double* vPlanePick1,const double* vPlaneNorm1,const double* vPlanePick2,const double* vPlaneNorm2);
	//判断给定直线是否与当前直线垂直(包括异面情况), perp为返回的给定直线上的垂足点
	BOOL F_ALG_FUN_API IsPerpToLine(f3dLine line,f3dLine oth_line, f3dPoint &perp,double epsilon=eps);

// 六、面
	/*捕捉点函数
	ptx,pty					为捕捉拾取点
	originx,originy			分为待捕捉点
	scope					为捕捉距离范围,
	dist					为拾取点与待捕捉点间的距离
	*/
	F_ALG_FUN_API BOOL 
	Snap2dPnt ( f2dPoint pt, f2dPoint origin,
				double scope ,double &dist);
	F_ALG_FUN_API BOOL 
	Snap2dPnt ( double ptx,	  double pty,
				double originx,double originy,
				double scope ,double &dist);
	/*捕捉直线函数
	ptx,pty					为捕捉拾取点
	startx,starty;endx,endy	分为直线的起始点和终止点
	scope					为捕捉距离范围,
	dist					为拾取点与直线间的垂直距离
	*/
	F_ALG_FUN_API BOOL 
	Snap2dLine( f2dPoint pt, f2dLine line,
				double scope ,double &dist);
	F_ALG_FUN_API BOOL 
	Snap2dLine( double ptx,	  double pty,
				double startx,double starty,
				double endx  ,double endy,
				double scope ,double &dist);
	//将角度规范到0～2*Pi范围内
	F_ALG_FUN_API double normalizeRadAngle(double radAngle);
	/*捕捉圆弧函数
	pt						为捕捉拾取点
	arc						待捕捉圆
	scope					为捕捉距离范围,
	dist					为拾取点与圆弧间的垂直距离
	*/
	F_ALG_FUN_API BOOL 
	Snap2dArcLine( f2dPoint pt, f2dArc arc,
				double scope ,double &dist);
	/*捕捉圆函数
	pt						为捕捉拾取点
	circle					待捕捉圆
	scope					为捕捉距离范围,
	dist					为拾取点与圆弧间的垂直距离
	*/
	F_ALG_FUN_API BOOL 
	Snap2dCircle( f2dPoint pt, f2dCircle circle,
				double scope ,double &dist);
	/*捕捉椭圆弧函数
	默认椭圆弧的中心在原点长轴在X轴方向
	pt						为捕捉拾取点
	majorRadius				长轴半径
	minorRadius				短轴半径
	scope					为捕捉距离范围,
	dist					为拾取点与圆弧间的垂直距离
	startAngle				椭圆弧的起始角
	endAngle				椭圆弧的终止角
	*/
	F_ALG_FUN_API BOOL 
	Snap2dEllipseArc( f2dPoint pt,double majorRadius,
				double minorRadius,double scope ,double *dist=NULL,double startAngle=0,double endAngle=2*Pi);
	/*捕捉矩形函数
	pt						为捕捉拾取点
	rect					待捕捉矩形
	scope					为捕捉距离范围,
	dist					为拾取点与矩形间的垂直距离
	*/
	F_ALG_FUN_API BOOL 
	Snap2dRect( f2dPoint pt, f2dRect rect,
				double scope ,double &dist);

	//返回一个与矢量vec方向不同的新矢量
	F_ALG_FUN_API f3dPoint inters_vec(const double* pxInitialVec);
	// 根据两点确定用户坐标系		--王金合
	// 输入；origin(原点), axis_pt(坐标轴方向确定点)
	// AXIS_X0Y1Z2表示输入的axis_pt参数表示用户坐标系的坐标轴标识0:X轴1:Y轴2:Z轴
	// 输出：正确返回 TRUE 错误返回 FALSE
	//		 ucs  程序所确定的用户坐标系
	F_ALG_FUN_API BOOL CalCSBy2Pt(f3dPoint origin, UCS_STRU &ucs, f3dPoint axis_pt,int AXIS_X0Y1Z2=0);
	//返回一个在直线start-->end上由捡取点pick确定的唯一垂足点
	F_ALG_FUN_API BOOL SnapPerp(COORD3D* perp, f3dLine line,const double* pick, double* pDist=NULL);
	//用参数COORD3D&，而未用const double* 是为了避免与后面的二维点重载冲突 wjh-2019.8.26
	F_ALG_FUN_API BOOL SnapPerp(COORD3D* perp, const COORD3D& start, const COORD3D& end,
						const COORD3D& pick, double* pDist=NULL);
	F_ALG_FUN_API BOOL SnapPerp(f2dPoint* perp, f2dPoint start, f2dPoint end,
						f2dPoint pick, double* pDist=NULL);
	F_ALG_FUN_API BOOL SnapPerp(f2dPoint* perp, f2dLine line,
						f2dPoint pick, double* pDist=NULL);
	//返回如下的点:
	//首先自点origin沿vec方向移动offset的距离,然后再沿direct的方向移动dist的距离所得到的点
	F_ALG_FUN_API f3dPoint get_pt_by_vec( f3dPoint vec, f3dPoint direct, 
				f3dPoint origin, double offset, double dist);
	//angle为直线在X-Y平面上的投影线与X轴正向的夹角，
	//故此操作也只是在此X-Y平面上进行的
	F_ALG_FUN_API f3dPoint GetPtInPolar(f3dPoint base, double angle,double dist);
	//角度转换 rad-->degree
	F_ALG_FUN_API double RadToDegree(double rad);
	//角度转换 degree-->rad
	F_ALG_FUN_API double DegreeToRad(double deg);
	F_ALG_FUN_API void LoadDefaultUCS(UCS_STRU *ucs);
	F_ALG_FUN_API UCS_STRU LoadDefaultUCS(const f3dPoint& origin);
	F_ALG_FUN_API void ReverseUCS(UCS_STRU *ucs);
	F_ALG_FUN_API void ReverseUCS(GECS *ucs);
	//以一点为基准求其沿某两方向的等距点偏移点
	F_ALG_FUN_API f3dPoint OffsetBy2Vec(f3dPoint offset_origin,f3dPoint vec1,f3dPoint vec2,double offset_dist1=0.0,double offset_dist2=0.0);
	//根据平面上三点来求三角形面积（根据海伦公式area=sqrt(s*(s-a)*(s-b)*(s-c));s=(a+b+c)/2).
	F_ALG_FUN_API double CalTriArea(double p1x,double p1y,double p2x,double p2y,double p3x,double p3y);
	struct PLANAR_PLATE{
		GECS cs;
		double thick;
		int vertex_count;
		GEPOINT2D* vertex_arr;
	};
	//检测两块凸多边形平板之间是否发生碰撞或干涉
	F_ALG_FUN_API bool IsIntersPlates(PLANAR_PLATE& plate1, PLANAR_PLATE& plate2,double intervene_space);
	//检测凸多边形平板与圆柱体（可模拟螺栓）之间是否发生碰撞或干涉
	F_ALG_FUN_API bool IsIntersPlateAndColumn(PLANAR_PLATE& plate,
		f3dPoint axis_pick_start,f3dPoint axis_pick_end,double radius,double intervene_space);

#endif