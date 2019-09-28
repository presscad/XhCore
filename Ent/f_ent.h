#ifndef __F_ENT_H_
#define __F_ENT_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the F_ENT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// F_ENT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define F_ENT_API
#else
#ifdef F_ENT_EXPORTS
#define F_ENT_API __declspec(dllexport)
#else
#define F_ENT_API __declspec(dllimport)
#endif
#endif
// f_ent.H  [铁塔设计自动化系统软件] 北京信狐天健软件科技发展有限公司
// =========== 定义基本图形元互的类结构 2002.09.12. 王金合(Wjh) ============
// 功能：声明和定义CAD中的部分体素及图形算法

#include "math.h"

/****************************************************************************/
/* 定义常量: DEFINES                                                                 */
/****************************************************************************/
#define      TRUE             1
#define      FALSE            0
#define      Pi               3.14159265358979323846
#define      RADTODEG_COEF    0.01745329251994329577
#define      DEGTORAD_COEF    57.2957795130823208768
#define      DEGRAD           (Pi/180.0)

// 极限值: ε
#define      EPS_COS          0.999999999
#define      eps_cos          0.999999999
#define      EPS              1e-9
#define      eps              1e-9
// 工程极限值: ε2 (2%的误差)
#define      EPS_COS2         0.97
#define      eps_cos2         0.97
#define      EPS2             0.03
#define      eps2             0.03

// 平方: x*x
#define      SQR(x)           ((x)*(x))

// 空间向量的长度:  sqrt(x*x+y*y+z*z)
#define      LENGTH(v)        sqrt(SQR((v).x)+SQR((v).y)+SQR((v).z))

// 空间3D两点间的距离:
#define      DISTANCE(a,b)    sqrt(SQR((a).x-(b).x)+SQR((a).y-(b).y)\
                   +SQR((a).z-(b).z))

// 3D点赋值: a=b
#define      Cpy_Pnt(a,b)     ((a).x=(b).x,(a).y=(b).y,(a).z=(b).z)

// 3D点相减: a=b-c
#define      Sub_Pnt(a,b,c)   ((a).x=(b).x-(c).x,(a).y=(b).y-(c).y,\
                   (a).z=(b).z-(c).z)

// 3D点相加: a=b+c
#define      Add_Pnt(a,b,c)   ((a).x=(b).x+(c).x,(a).y=(b).y+(c).y,\
                   (a).z=(b).z+(c).z)

// 2D点赋值: a=b
#define      Cpy_2dPnt(a,b)     ((a).x=(b).x,(a).y=(b).y)

// 2D点相减: a=b-c
#define      Sub_2dPnt(a,b,c)   ((a).x=(b).x-(c).x,(a).y=(b).y-(c).y)

// 2D点相加: a=b+c
#define      Add_2dPnt(a,b,c)   ((a).x=(b).x+(c).x,(a).y=(b).y+(c).y)

//////////////////////-------类与结构声明区-------//////////////////////////////

struct AtomType{
	enum TA{ taJg	= 0,
			 taNode	= 1,
			 taLs	= 2,
			 taJdb	= 3,
			 taPlank = 4,
			 taJgJoint= 5,
			 taWrapJg = 6,
			 taWrapPlate=7,
			 taShortJg = 8,
			 taGuaXianBan= 9,
			 taExplodeJg = 10,
			 taSonJg = 11,
			 taHuoQuSonJg = 12,
			 taHuoQuJg = 13,
			 taVirtualNode = 14,
			 taGroupJg	= 15,
			 taTianBan	= 16,
			 taFoot		= 16,
			 taOther = 100 };
	enum Primitive{ prPoint 	= 0,
					prLine		= 1,
					prRay		= 2,
					prCircle	= 3,
					prPolyFace	= 4,
					pr4PolyFace = 5,
					prTriFace	= 6,
					prRect  	= 7,
					prArc   	= 8,
					prEllipse  	= 9,
					prArrow   	= 10,
					prOther		= 100 };
	enum Mark{  mkCir 	= 0,
				mkRect	= 1,		//平面矩形框标记
				mkX		= 2,		//×型标记
				mkRectX	= 3,		//矩形框加×型标记
				mkOther	= 100 };
};
typedef long NAMEID;
typedef long THANDLE;

struct F_ENT_API PEN_STRU{
	DWORD crColor;
	unsigned char width;
	short style;
};
struct F_ENT_API GEPOINT2D{
	double x,y;
	GEPOINT2D(double x=0,double y=0){this->x=x;this->y=y;};
};
//COORD3D与GEPOINT的主要区别在于无构造函数，可用于定义union子成员对象 wjh-2015.9.1
struct F_ENT_API COORD3D{
	double x,y,z;
	void Set(double x=0,double y=0,double z=0){this->x=x;this->y=y;this->z=z;};
	void Fillet();	//将近似零分量(绝对值小于EPS的分量)圆整为零
	bool IsZero() const;
	operator double*(){return &x;}
	bool IsEqual(double x,double y,double z,double epsilong=eps) const;
	bool IsEqual(const double* coords,double epsilong=eps) const;
	bool IsEqual(const COORD3D& pt,double epsilong=eps) const;
	double mod2() const{return (x*x+y*y+z*z);};		//!< get the squared length of the vector
	double mod () const{return sqrt(x*x+y*y+z*z);}	//!< get the length of the vector
	COORD3D normalized() const;			//返回单位矢量的一个拷贝
	COORD3D Perpendicular(bool standardize=false)const;//返回一个与矢量vec方向不同的新矢量
// operand override functions
	double* operator =(double* coord);
    bool operator==(const COORD3D& pt);
    bool operator!=(const COORD3D& pt);
	COORD3D operator * (short  c) const;
    COORD3D operator * (int    c) const;
    COORD3D operator * (double c) const;
	COORD3D& operator+=(const COORD3D& other);//!< the sum & assign
	COORD3D& operator-=(const COORD3D& other);//!< the subtract & assign
	COORD3D& operator*=(const double& fact);//!< the short multiply by a scalar factor & assign
	COORD3D& operator/=(const double& fact);//!< the short divide by a scalar factor & assign
// friend functions	
	friend F_ENT_API COORD3D operator+(const COORD3D& v1,const COORD3D& v2);//!< the sum
	friend F_ENT_API COORD3D operator-(const COORD3D& v1,const COORD3D& v2);//!< the difference
	friend F_ENT_API COORD3D operator-(const COORD3D& v1);//!< the negation
	friend F_ENT_API double  operator*(const COORD3D& v1,const COORD3D& v2);//!< the vector dot product
	friend F_ENT_API COORD3D operator^(const COORD3D& v1,const COORD3D& v2);//!< the vector cross product
    friend F_ENT_API COORD3D operator*(short, const COORD3D& pt);
	friend F_ENT_API COORD3D operator*(int, const COORD3D& pt);
	friend F_ENT_API COORD3D operator*(double fact,const COORD3D& v);//!< the multiply a scalar factor by a vector
	friend F_ENT_API COORD3D operator/(const COORD3D& v,const double& fact);//!< the divide a vector by a scalar factor
	friend F_ENT_API COORD3D Bisect(COORD3D v0,COORD3D v1);//!< returns the unit vector which halves the arc between v0 and v1
};
struct F_ENT_API GEPOINT : public COORD3D{
	GEPOINT(const COORD3D& coord3d) { x=coord3d.x;y=coord3d.y;z=coord3d.z; }
	GEPOINT(double x=0,double y=0,double z=0){this->x=x;this->y=y;this->z=z;};
	GEPOINT(const double* coord3d)
	{
		if(coord3d==NULL)
			x=y=z=0.0;
		else
		{
			x=coord3d[0];
			y=coord3d[1];
			z=coord3d[2];
		}
	}
};
struct F_ENT_API GELINE{
	GEPOINT start,end;
	GELINE(){;}
	GELINE(GEPOINT _start,GEPOINT _end){
		start=_start;
		end=_end;
	}
};
class F_ENT_API fAtom{
public:
	AtomType::Primitive atom_type;
	bool is_highlight;	//是否高亮显示
	//BOOL is_visible;	//是否处于显示状态
	int feature;		//存储用户定义的一些信息
	NAMEID ID;
	PEN_STRU pen;
    fAtom();
    ~fAtom(){;}
};

inline fAtom::fAtom(){	// 构造函数
	atom_type = AtomType::prOther;
	is_highlight = false;
	//is_visible = TRUE;
	//memset(layer,0,sizeof(layer));
	pen.crColor = 0;
	pen.style = 0;
	pen.width = 1;
	ID = 0;
	feature = 0;
}
//#---------------------------------------------------------------------------
class f3dPoint;
class F_ENT_API f2dPoint : public fAtom{
public:
    double x,y;
	f2dPoint(){ID = 0;x = y = 0;atom_type=AtomType::prPoint;}
    f2dPoint(const double* coord2d){
		ID=0;
		x=coord2d[0];
		y=coord2d[1];
		atom_type=AtomType::prPoint;
	}
	f2dPoint(double X,double Y){
		ID = 0;
		x = X;
		y = Y;
		atom_type=AtomType::prPoint;
	}
	void Set(double X, double Y){x=X;y=Y;}
	f2dPoint operator =(f3dPoint);
	double operator*(f2dPoint pt){return x*pt.x+y*pt.y;}
	operator double*(){return &x;};			//返回一个双精度指针
};

//#---------------------------------------------------------------------------
class F_ENT_API f2dLine : public fAtom{			// 二维线结构
public:
	double DistToPt(double pt_x, double pt_y);
	double GetRadAng();// 计算直线(段)的倾角(相对水平X轴),返回值为0=<angle<2π
    f2dPoint startPt;
    f2dPoint endPt;
	f2dLine();
	f2dLine(const double* start2d,const double* end2d){
		startPt.Set(start2d[0],start2d[1]);
		endPt.Set(end2d[0],end2d[1]);
	}
	int IntWith2dLine(f2dLine oth_line,double &PX,double &PY);
	double Length();
};

class F_ENT_API f2dRay : public fAtom{			// 二维线结构
public:
    f2dPoint basePt;
    f2dPoint vec;
	f2dRay(){atom_type = AtomType::prRay;}
	f2dRay(const double* polar,const double* ray_vec){
		basePt.Set(polar[0],polar[1]);
		vec.Set(ray_vec[0],ray_vec[1]);
		atom_type = AtomType::prRay;
	}
};

class F_ENT_API f2dCircle : public fAtom{	// 三维圆结构
public:
	f2dPoint centre;
	double   radius;
	f2dCircle();
	f2dCircle(double R, double CX, double CY);
	f2dCircle(double R, f2dPoint CirPt);
};
class F_ENT_API f2dArc : public fAtom{
public:
	f2dPoint startPt;
	f2dPoint centre;
	double angle;
	f2dArc();
	f2dPoint endPt();
	double Radius();
};  //椭圆弧结构
class F_ENT_API f2dEllipse : public fAtom{
public:
	f2dPoint topLeft;
	f2dPoint bottomRight;
	f2dEllipse();
};
inline f2dEllipse::f2dEllipse()
{
	memset(this,0,sizeof(f2dEllipse));
	atom_type = AtomType::prEllipse;
}

class F_ENT_API f2dRect : public fAtom{
public:
	f2dRect();
	f2dPoint topLeft;
	f2dPoint bottomRight;
	double Width();
	double Height();
	void NormalizeRect();
	void SetRect(f2dPoint top_left, f2dPoint bottom_right);
	BOOL PtInRect(f2dPoint pt);
	BOOL IntersWithLine(f2dLine line2d);
};
inline f2dRect::f2dRect()
{
	atom_type = AtomType::prRect;
}
class F_ENT_API f2dPoly : public fAtom
{
	int number;
public:
	f2dPoly();
	f2dPoint vertex[16];
	f2dLine  outer_edge[16];
	BOOL CreatePoly(f2dPoint *pnt_arr, int nCount);
	BOOL CreateEdge();
	BOOL PtInPoly(f2dPoint pt);
	~f2dPoly();
};
inline f2dPoly::f2dPoly()
{
	atom_type = AtomType::prPolyFace;
	number = 0;
}
inline f2dPoly::~f2dPoly()
{
}
//-------------------------- 三维坐标点结构定义 ------------------------------
class F_ENT_API f3dPoint : public COORD3D, public fAtom{
public:
	f3dPoint(const COORD3D& coord3d) { x=coord3d.x;y=coord3d.y;z=coord3d.z; atom_type=AtomType::prPoint;}
	f3dPoint(double x=0,double y=0,double z=0){
		this->x=x;this->y=y;this->z=z;
		atom_type=AtomType::prPoint;
	};
    f3dPoint(const double* coord3d){
		if(coord3d==NULL)
			x=y=z=0.0;
		else
		{
			x=coord3d[0];
			y=coord3d[1];
			z=coord3d[2];
		}
		atom_type=AtomType::prPoint;
	}// 构造
	operator f2dPoint(void);
};
//------------------------ 用户坐标系结构UCS_STRU定义 ----------------------------
struct F_ENT_API GECS{
    GEPOINT axis_x;	// 坐标轴 X
    GEPOINT axis_y;	// 坐标轴 Y
    GEPOINT axis_z;	// 坐标轴 Z
    GEPOINT origin;	// 坐标原点
public:
	GECS(){;}
	GECS(const GEPOINT& org,const GEPOINT& axis_x,const GEPOINT& axis_y,const GEPOINT& axis_z);
	GEPOINT TransPToCS(const double* coord3d);	//const
	GEPOINT TransPFromCS(const double* coord3d);	//const
	GEPOINT TransVToCS(const double* vector3d);	//const
	GEPOINT TransVFromCS(const double* vector3d);	//const
	GEPOINT TransformP(const double* coord3d);	//const完成一个动作变换,变换坐标系等同于对变换前原始坐标系进行变换的结果
	GEPOINT TransformV(const double* vector3d);	//const完成一个动作变换,变换坐标系等同于对变换前原始坐标系进行变换的结果
	bool Standardize(char cOrthoDatumAxis='Z');
	void InitStdCS();
	void InitFromConstTrans(BYTE trans_type,double rot_angle=0);
	BYTE TestConstTransType();
public:
	static const BYTE TRANS_MOVE;// = 1;
	static const BYTE TRANS_MIRX;// = 2;
	static const BYTE TRANS_MIRY;// = 3;
	static const BYTE TRANS_MIRZ;// = 4;
	static const BYTE TRANS_ROTX;// = 5;
	static const BYTE TRANS_ROTY;// = 6;
	static const BYTE TRANS_ROTZ;// = 7;
};
struct F_ENT_API UCS_STRU : public GECS{
	UCS_STRU(){;}
	operator GECS() const;
	UCS_STRU(const GECS& gecs);
};
struct F_ENT_API UCS_2dSTRU{
    f2dPoint axis_x;	// 坐标轴 X
    f2dPoint axis_y;	// 坐标轴 Y
    f2dPoint origin;	// 坐标原点
};
class f3dArcLine;
class F_ENT_API SCOPE_STRU{
public:
	double fMinX, fMaxX;
	double fMinY, fMaxY;
	double fMinZ, fMaxZ;
public:
	SCOPE_STRU();
	double wide(){return max(0,fMaxX-fMinX);}
	double high(){return max(0,fMaxY-fMinY);}
	double depth(){return max(0,fMaxZ-fMinZ);}
	void centre(double *cx,double *cy,double *cz);
	void ClearScope();
	void SetCubeScope(double size=1.0){fMinX=fMinY=fMinZ=-size/2.0;fMaxX=fMaxY=fMaxZ=size/2.0;}
	bool IsIntersWith(SCOPE_STRU &scope);
	bool IsIntersWithLine(const double* start,const double* end);
	bool IsIncludePoint(const double* pt,double tolerance=EPS);
	void VerifyVertex(const double* pt);
	void VerityArcLine(f3dArcLine& arcline);	//const 
};
class F_ENT_API TAPER_SCOPE{
	double fTopMinX,fTopMaxX, fBtmMinX,fBtmMaxX;
	double fTopMinY,fTopMaxY, fBtmMinY,fBtmMaxY;
	double fTopZ,fBtmZ;
	void ClearTopLevel();
	void ClearBtmLevel();
public:
	TAPER_SCOPE();
	double TopMinX()const{return fTopMinX;}
	double TopMaxX()const{return fTopMaxX;}
	double BtmMinX()const{return fBtmMinX;}
	double BtmMaxX()const{return fBtmMaxX;}
	double TopMinY()const{return fTopMinY;}
	double TopMaxY()const{return fTopMaxY;}
	double BtmMinY()const{return fBtmMinY;}
	double BtmMaxY()const{return fBtmMaxY;}
	double TopZ()const{return fTopZ;}
	double BtmZ()const{return fBtmZ;}
	void ClearScope();
	bool IsIncludePoint(const double* ptcoords,double tolerance=EPS);
	bool IsIncludePoint(double px,double py,double pz,double tolerance=EPS);
	void VerifyVertex(const double* ptcoords);
	void VerifyVertex(double px,double py,double pz);
};
class F_ENT_API f3dLine : public fAtom	// 三维线结构
{
public:
	f3dLine();
	f3dLine(f3dPoint start, f3dPoint end);
	//-2:在直线上,但位于线段起点外侧 
	//-1:在直线上,但位于线段终点外侧
	//0:不在直线上 1:在线段端点上 2:在线段内
	int PtInLine(f3dPoint,double epsilon=eps)const;
	f3dPoint SnapPerp(f3dPoint pick, double *dist=NULL);	//返回垂足点和垂距
    f3dPoint startPt;
    f3dPoint endPt;
	char layer[24];
};
inline f3dLine::f3dLine()
{
	atom_type = AtomType::prLine;
	memset(layer,0,24);
}
inline f3dLine::f3dLine(f3dPoint start, f3dPoint end)
{
	startPt=start;
	endPt=end;
	atom_type = AtomType::prLine;
	memset(layer,0,24);
}
struct F_ENT_API COLINEAR_TOLERANCE{
	double distance;	//点距离线的最大距离容差
	double tangent;		//点与基点连线与直线间夹角正切值最大容差
	COLINEAR_TOLERANCE(double distTolerance=5.0,double tangentTolerance=0.001)
	{
		distance=distTolerance;
		tangent =tangentTolerance;
	}
};
//快速检测点是否在直线上的工具类
class F_ENT_API CPtInLineCheck
{
protected:
	double len;
	f3dPoint lineStart,lineEnd,lineVec,lineStdVec;
public:
	CPtInLineCheck(){;}
	CPtInLineCheck(f3dPoint start,f3dPoint end){SetLine(start,end);}
	bool SetLine(f3dPoint start,f3dPoint end);
	bool ReviseLenStdVec(const double* lenstdvec,double* pfDistOfEnd=NULL);
	//<-2.0直线定义有误;<0.0||>1.0:点不在线上或直线内;0.0:点与直线始端重合;<1.0点在直线内;1.0点与直线终端重合
	double CheckPoint(const double* pick,double tolerance=EPS,double tolerance_tangent=1);
	//pbOnLine=true时，返回值=(pick-start)/line_vec
	double CheckPointEx(const double* pick,bool* pbOnLine=NULL,double tolerance=EPS,double tolerance_tangent=1);
	double Length(){return len;}
	f3dPoint LineVec(){return lineVec;}
	f3dPoint LineStdVec(){return lineStdVec;}
};

class F_ENT_API f3dRay : public fAtom{			// 二维线结构
public:
    f3dPoint basePt;
    f3dPoint vec;
	f3dRay(){atom_type = AtomType::prRay;}
};

class F_ENT_API f3dRect : public fAtom{
public:
	f3dRect(){atom_type = AtomType::prRect;}
	f3dPoint topLeft;
	f3dPoint bottomRight;
	f3dPoint norm;
	double Width()const;
	double Height()const;
	void NormalizeRect();
	void SetRect(f3dPoint top_left, f3dPoint bottom_right);
	//BOOL PtInRect(f2dPoint pt);
};

class F_ENT_API f3dCircle : public fAtom	// 三维圆结构
{
public:
	f3dPoint centre;
	double   radius;
	f3dPoint norm;
	BOOL GetRect(f3dRect* rect)const;
	f3dCircle(){atom_type=AtomType::prCircle; radius = 0;}
};
class F_ENT_API f3dEllipse : public fAtom
{
public:
	f3dPoint topLeft;
	f3dPoint bottomRight;
	f3dPoint norm;
	BOOL GetRect(f3dRect *rect)const;
	f3dEllipse(){atom_type=AtomType::prEllipse;}
};  //圆弧结构

class F_ENT_API f3dArcLine : public fAtom
{
	GEPOINT posStart,posEnd;
	GEPOINT center;
	GEPOINT column_norm;
	GEPOINT work_norm;
	double radius;
	//弧度制单位
	double sector_angle;
	//根据圆弧起始点，终止点重新生成圆弧
	BOOL CreateMethodPrivate(f3dPoint &arc_pick);
public:
	//显示圆弧时的片段数
	unsigned int m_uDisplaySlices;
	f3dArcLine();
	f3dArcLine(f3dLine line);
	int ToByteArr(char* buf);	//直线48Bytes,圆弧112Bytes,椭圆弧136Bytes;注意只输出几何信息部分
	void FromByteArr(char* buf,DWORD buf_size);//从缓存中还原弧或直线
	void ZoomScale(double zoomCoef);	//按比例缩放圆弧线>1为放大<1为缩小
	BOOL CoordTrans(UCS_STRU& ucs,bool fromUcs);
	void Offset(f3dPoint offset);
	//根据圆心，圆弧起始点及工作面法线及圆弧扇形角生成圆弧
	BOOL CreateMethod1(f3dPoint c,f3dPoint startPt,f3dPoint work_norm,double angle);
	//根据圆弧起始点，终止点及工作面法线及圆弧扇形角生成圆弧
	BOOL CreateMethod2(f3dPoint startPt,f3dPoint endPt,f3dPoint work_norm,double angle);
	//根据圆弧起始点，终止点、工作面法线及半径生成圆弧,center_pick用于标识圆心是位于弦的左侧还是右侧
	BOOL CreateMethod3(f3dPoint startPt,f3dPoint endPt,f3dPoint work_norm,double r,f3dPoint center_pick);
	//根据椭圆的圆心，起始点，终止点，圆柱面法线，工作面法线及圆柱面半径
	BOOL CreateEllipse( f3dPoint c,f3dPoint startPt,f3dPoint endPt,
						f3dPoint columnnorm,f3dPoint worknorm,double radius);
	GEPOINT Start() const{return posStart;}
	GEPOINT End() const{return posEnd;}
	GEPOINT ColumnNorm(){return column_norm;}	//const
	GEPOINT WorkNorm(){return work_norm;}	//const
	GEPOINT Center(){return center;}	//const
	double Radius(){return radius;}	//const
	//弧度制单位
	double SectorAngle(){return sector_angle;}
	//圆弧长度
	double Length();
	BOOL SetStart(double x,double y,double z);
	BOOL SetEnd(double x,double y,double z);
	BOOL SetStart(f3dPoint startPt);
	BOOL SetEnd(f3dPoint endPt);
	BOOL SetWorkNorm(f3dPoint norm);
	BOOL SetColumnNorm(f3dPoint columnnorm);
	BOOL SetCenter(f3dPoint center);
	double SetRadius(double R);
	//弧度制单位
	double SetSectorAngle(double angle);	//弧度制单位
	GEPOINT PositionInAngle(double posAngle);
	GEPOINT TangentVecInAngle(double posAngle);	//逆时针旋转为正切向方向
	//vertex_arr=NULL时，直接返回理想样点数目
	UINT SamplePoints(GEPOINT* vertex_arr,UINT maxArrayCount=0,double sample_length=5,int smoothness=36);
};  //圆弧结构

//用于显示标记　
class F_ENT_API CMark{
	f2dLine line_arr[16];
	int		line_n;
public:
	int scope;
	NAMEID ID;
    CMark();
    ~CMark();
	CMark& operator = (CMark mark);
	int GetLineN();
	void GetLine(f2dLine *line, int i);
	f3dPoint pos;
	AtomType::Mark nMarkType;
	BOOL is_visible;	//是否处于显示状态
	void CreateMark(int scope, f2dPoint scr_pos);
};

class F_ENT_API f3dAtomLine
{	// 三维线结构
	GEPOINT center;
	GEPOINT column_norm;	//圆柱面的法线
	GEPOINT work_norm;		//当前圆弧面的法线，如果是
	double radius;
	//弧度制单位
	double sector_angle;

    f3dAtomLine* next;    //指向下一线段
    f3dAtomLine* prev;    //指向前一线段
	friend class fLineList;
public:
    f3dPoint* pStart;
    f3dPoint* pEnd;
	//显示圆弧时的片段数
	unsigned int m_uDisplaySlices;
	ULONG id;
	int feature;
    bool is_positive_line;	// TRUE:矢量从起点指向终点
public:
	f3dAtomLine();
	operator f3dLine();		//向三维直线的类型转换
	long GetId(){return id;}
	BOOL CoordTrans(UCS_STRU& ucs,bool fromUcs,bool bConvertVertex=false);
	//根据圆弧起始点，终止点及工作面法线及圆弧扇形角生成圆弧
	BOOL CreateArcLine(f3dPoint work_norm,double angle);
	//根据圆弧起始点，终止点、工作面法线及半径生成圆弧,center_pick用于标识圆心是位于弦的左侧还是右侧
	BOOL CreateArcLine2(f3dPoint work_norm,double r,f3dPoint center_pick);
	//根据椭圆的圆心，起始点，终止点，圆柱面法线，工作面法线及圆柱面半径
	BOOL CreateEllipseArcLine(f3dPoint c,f3dPoint columnnorm,f3dPoint worknorm,double r);
	GEPOINT WorkNorm(){return work_norm;}
	GEPOINT ColumnNorm(){return column_norm;}
	GEPOINT Center(){return center;}
	double Radius(){return radius;}
	//弧度制单位
	double SectorAngle(){return sector_angle;}
	GEPOINT PositionInAngle(double posAngle);
};

class F_ENT_API f3dCurve : public fAtom
{
public:
	f3dCurve(){atom_type = AtomType::prLine;}
	struct F_ENT_API DATA{
		f3dCircle cir;
		f3dArcLine arc;
		f3dAtomLine line;
	}data;
};

class F_ENT_API f3dTriFace : public fAtom{	// 三角面结构
public:
	f3dPoint CalNorm()const;	//计算三角面的法线方向
    f3dPoint vertex[3];
	short int feature;	// 可用于记录此面的特征信息(如材质等)
	f3dTriFace(){atom_type=AtomType::prTriFace;}
};

#endif


