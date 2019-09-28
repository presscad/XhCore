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
// f_ent.H  [��������Զ���ϵͳ���] �����ź��콡����Ƽ���չ���޹�˾
// =========== �������ͼ��Ԫ������ṹ 2002.09.12. �����(Wjh) ============
// ���ܣ������Ͷ���CAD�еĲ������ؼ�ͼ���㷨

#include "math.h"

/****************************************************************************/
/* ���峣��: DEFINES                                                                 */
/****************************************************************************/
#define      TRUE             1
#define      FALSE            0
#define      Pi               3.14159265358979323846
#define      RADTODEG_COEF    0.01745329251994329577
#define      DEGTORAD_COEF    57.2957795130823208768
#define      DEGRAD           (Pi/180.0)

// ����ֵ: ��
#define      EPS_COS          0.999999999
#define      eps_cos          0.999999999
#define      EPS              1e-9
#define      eps              1e-9
// ���̼���ֵ: ��2 (2%�����)
#define      EPS_COS2         0.97
#define      eps_cos2         0.97
#define      EPS2             0.03
#define      eps2             0.03

// ƽ��: x*x
#define      SQR(x)           ((x)*(x))

// �ռ������ĳ���:  sqrt(x*x+y*y+z*z)
#define      LENGTH(v)        sqrt(SQR((v).x)+SQR((v).y)+SQR((v).z))

// �ռ�3D�����ľ���:
#define      DISTANCE(a,b)    sqrt(SQR((a).x-(b).x)+SQR((a).y-(b).y)\
                   +SQR((a).z-(b).z))

// 3D�㸳ֵ: a=b
#define      Cpy_Pnt(a,b)     ((a).x=(b).x,(a).y=(b).y,(a).z=(b).z)

// 3D�����: a=b-c
#define      Sub_Pnt(a,b,c)   ((a).x=(b).x-(c).x,(a).y=(b).y-(c).y,\
                   (a).z=(b).z-(c).z)

// 3D�����: a=b+c
#define      Add_Pnt(a,b,c)   ((a).x=(b).x+(c).x,(a).y=(b).y+(c).y,\
                   (a).z=(b).z+(c).z)

// 2D�㸳ֵ: a=b
#define      Cpy_2dPnt(a,b)     ((a).x=(b).x,(a).y=(b).y)

// 2D�����: a=b-c
#define      Sub_2dPnt(a,b,c)   ((a).x=(b).x-(c).x,(a).y=(b).y-(c).y)

// 2D�����: a=b+c
#define      Add_2dPnt(a,b,c)   ((a).x=(b).x+(c).x,(a).y=(b).y+(c).y)

//////////////////////-------����ṹ������-------//////////////////////////////

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
				mkRect	= 1,		//ƽ����ο���
				mkX		= 2,		//���ͱ��
				mkRectX	= 3,		//���ο�ӡ��ͱ��
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
//COORD3D��GEPOINT����Ҫ���������޹��캯���������ڶ���union�ӳ�Ա���� wjh-2015.9.1
struct F_ENT_API COORD3D{
	double x,y,z;
	void Set(double x=0,double y=0,double z=0){this->x=x;this->y=y;this->z=z;};
	void Fillet();	//�����������(����ֵС��EPS�ķ���)Բ��Ϊ��
	bool IsZero() const;
	operator double*(){return &x;}
	bool IsEqual(double x,double y,double z,double epsilong=eps) const;
	bool IsEqual(const double* coords,double epsilong=eps) const;
	bool IsEqual(const COORD3D& pt,double epsilong=eps) const;
	double mod2() const{return (x*x+y*y+z*z);};		//!< get the squared length of the vector
	double mod () const{return sqrt(x*x+y*y+z*z);}	//!< get the length of the vector
	COORD3D normalized() const;			//���ص�λʸ����һ������
	COORD3D Perpendicular(bool standardize=false)const;//����һ����ʸ��vec����ͬ����ʸ��
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
	bool is_highlight;	//�Ƿ������ʾ
	//BOOL is_visible;	//�Ƿ�����ʾ״̬
	int feature;		//�洢�û������һЩ��Ϣ
	NAMEID ID;
	PEN_STRU pen;
    fAtom();
    ~fAtom(){;}
};

inline fAtom::fAtom(){	// ���캯��
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
	operator double*(){return &x;};			//����һ��˫����ָ��
};

//#---------------------------------------------------------------------------
class F_ENT_API f2dLine : public fAtom{			// ��ά�߽ṹ
public:
	double DistToPt(double pt_x, double pt_y);
	double GetRadAng();// ����ֱ��(��)�����(���ˮƽX��),����ֵΪ0=<angle<2��
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

class F_ENT_API f2dRay : public fAtom{			// ��ά�߽ṹ
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

class F_ENT_API f2dCircle : public fAtom{	// ��άԲ�ṹ
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
};  //��Բ���ṹ
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
//-------------------------- ��ά�����ṹ���� ------------------------------
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
	}// ����
	operator f2dPoint(void);
};
//------------------------ �û�����ϵ�ṹUCS_STRU���� ----------------------------
struct F_ENT_API GECS{
    GEPOINT axis_x;	// ������ X
    GEPOINT axis_y;	// ������ Y
    GEPOINT axis_z;	// ������ Z
    GEPOINT origin;	// ����ԭ��
public:
	GECS(){;}
	GECS(const GEPOINT& org,const GEPOINT& axis_x,const GEPOINT& axis_y,const GEPOINT& axis_z);
	GEPOINT TransPToCS(const double* coord3d);	//const
	GEPOINT TransPFromCS(const double* coord3d);	//const
	GEPOINT TransVToCS(const double* vector3d);	//const
	GEPOINT TransVFromCS(const double* vector3d);	//const
	GEPOINT TransformP(const double* coord3d);	//const���һ�������任,�任����ϵ��ͬ�ڶԱ任ǰԭʼ����ϵ���б任�Ľ��
	GEPOINT TransformV(const double* vector3d);	//const���һ�������任,�任����ϵ��ͬ�ڶԱ任ǰԭʼ����ϵ���б任�Ľ��
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
    f2dPoint axis_x;	// ������ X
    f2dPoint axis_y;	// ������ Y
    f2dPoint origin;	// ����ԭ��
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
class F_ENT_API f3dLine : public fAtom	// ��ά�߽ṹ
{
public:
	f3dLine();
	f3dLine(f3dPoint start, f3dPoint end);
	//-2:��ֱ����,��λ���߶������� 
	//-1:��ֱ����,��λ���߶��յ����
	//0:����ֱ���� 1:���߶ζ˵��� 2:���߶���
	int PtInLine(f3dPoint,double epsilon=eps)const;
	f3dPoint SnapPerp(f3dPoint pick, double *dist=NULL);	//���ش����ʹ���
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
	double distance;	//������ߵ��������ݲ�
	double tangent;		//�������������ֱ�߼�н�����ֵ����ݲ�
	COLINEAR_TOLERANCE(double distTolerance=5.0,double tangentTolerance=0.001)
	{
		distance=distTolerance;
		tangent =tangentTolerance;
	}
};
//���ټ����Ƿ���ֱ���ϵĹ�����
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
	//<-2.0ֱ�߶�������;<0.0||>1.0:�㲻�����ϻ�ֱ����;0.0:����ֱ��ʼ���غ�;<1.0����ֱ����;1.0����ֱ���ն��غ�
	double CheckPoint(const double* pick,double tolerance=EPS,double tolerance_tangent=1);
	//pbOnLine=trueʱ������ֵ=(pick-start)/line_vec
	double CheckPointEx(const double* pick,bool* pbOnLine=NULL,double tolerance=EPS,double tolerance_tangent=1);
	double Length(){return len;}
	f3dPoint LineVec(){return lineVec;}
	f3dPoint LineStdVec(){return lineStdVec;}
};

class F_ENT_API f3dRay : public fAtom{			// ��ά�߽ṹ
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

class F_ENT_API f3dCircle : public fAtom	// ��άԲ�ṹ
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
};  //Բ���ṹ

class F_ENT_API f3dArcLine : public fAtom
{
	GEPOINT posStart,posEnd;
	GEPOINT center;
	GEPOINT column_norm;
	GEPOINT work_norm;
	double radius;
	//�����Ƶ�λ
	double sector_angle;
	//����Բ����ʼ�㣬��ֹ����������Բ��
	BOOL CreateMethodPrivate(f3dPoint &arc_pick);
public:
	//��ʾԲ��ʱ��Ƭ����
	unsigned int m_uDisplaySlices;
	f3dArcLine();
	f3dArcLine(f3dLine line);
	int ToByteArr(char* buf);	//ֱ��48Bytes,Բ��112Bytes,��Բ��136Bytes;ע��ֻ���������Ϣ����
	void FromByteArr(char* buf,DWORD buf_size);//�ӻ����л�ԭ����ֱ��
	void ZoomScale(double zoomCoef);	//����������Բ����>1Ϊ�Ŵ�<1Ϊ��С
	BOOL CoordTrans(UCS_STRU& ucs,bool fromUcs);
	void Offset(f3dPoint offset);
	//����Բ�ģ�Բ����ʼ�㼰�����淨�߼�Բ�����ν�����Բ��
	BOOL CreateMethod1(f3dPoint c,f3dPoint startPt,f3dPoint work_norm,double angle);
	//����Բ����ʼ�㣬��ֹ�㼰�����淨�߼�Բ�����ν�����Բ��
	BOOL CreateMethod2(f3dPoint startPt,f3dPoint endPt,f3dPoint work_norm,double angle);
	//����Բ����ʼ�㣬��ֹ�㡢�����淨�߼��뾶����Բ��,center_pick���ڱ�ʶԲ����λ���ҵ���໹���Ҳ�
	BOOL CreateMethod3(f3dPoint startPt,f3dPoint endPt,f3dPoint work_norm,double r,f3dPoint center_pick);
	//������Բ��Բ�ģ���ʼ�㣬��ֹ�㣬Բ���淨�ߣ������淨�߼�Բ����뾶
	BOOL CreateEllipse( f3dPoint c,f3dPoint startPt,f3dPoint endPt,
						f3dPoint columnnorm,f3dPoint worknorm,double radius);
	GEPOINT Start() const{return posStart;}
	GEPOINT End() const{return posEnd;}
	GEPOINT ColumnNorm(){return column_norm;}	//const
	GEPOINT WorkNorm(){return work_norm;}	//const
	GEPOINT Center(){return center;}	//const
	double Radius(){return radius;}	//const
	//�����Ƶ�λ
	double SectorAngle(){return sector_angle;}
	//Բ������
	double Length();
	BOOL SetStart(double x,double y,double z);
	BOOL SetEnd(double x,double y,double z);
	BOOL SetStart(f3dPoint startPt);
	BOOL SetEnd(f3dPoint endPt);
	BOOL SetWorkNorm(f3dPoint norm);
	BOOL SetColumnNorm(f3dPoint columnnorm);
	BOOL SetCenter(f3dPoint center);
	double SetRadius(double R);
	//�����Ƶ�λ
	double SetSectorAngle(double angle);	//�����Ƶ�λ
	GEPOINT PositionInAngle(double posAngle);
	GEPOINT TangentVecInAngle(double posAngle);	//��ʱ����תΪ��������
	//vertex_arr=NULLʱ��ֱ�ӷ�������������Ŀ
	UINT SamplePoints(GEPOINT* vertex_arr,UINT maxArrayCount=0,double sample_length=5,int smoothness=36);
};  //Բ���ṹ

//������ʾ��ǡ�
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
	BOOL is_visible;	//�Ƿ�����ʾ״̬
	void CreateMark(int scope, f2dPoint scr_pos);
};

class F_ENT_API f3dAtomLine
{	// ��ά�߽ṹ
	GEPOINT center;
	GEPOINT column_norm;	//Բ����ķ���
	GEPOINT work_norm;		//��ǰԲ����ķ��ߣ������
	double radius;
	//�����Ƶ�λ
	double sector_angle;

    f3dAtomLine* next;    //ָ����һ�߶�
    f3dAtomLine* prev;    //ָ��ǰһ�߶�
	friend class fLineList;
public:
    f3dPoint* pStart;
    f3dPoint* pEnd;
	//��ʾԲ��ʱ��Ƭ����
	unsigned int m_uDisplaySlices;
	ULONG id;
	int feature;
    bool is_positive_line;	// TRUE:ʸ�������ָ���յ�
public:
	f3dAtomLine();
	operator f3dLine();		//����άֱ�ߵ�����ת��
	long GetId(){return id;}
	BOOL CoordTrans(UCS_STRU& ucs,bool fromUcs,bool bConvertVertex=false);
	//����Բ����ʼ�㣬��ֹ�㼰�����淨�߼�Բ�����ν�����Բ��
	BOOL CreateArcLine(f3dPoint work_norm,double angle);
	//����Բ����ʼ�㣬��ֹ�㡢�����淨�߼��뾶����Բ��,center_pick���ڱ�ʶԲ����λ���ҵ���໹���Ҳ�
	BOOL CreateArcLine2(f3dPoint work_norm,double r,f3dPoint center_pick);
	//������Բ��Բ�ģ���ʼ�㣬��ֹ�㣬Բ���淨�ߣ������淨�߼�Բ����뾶
	BOOL CreateEllipseArcLine(f3dPoint c,f3dPoint columnnorm,f3dPoint worknorm,double r);
	GEPOINT WorkNorm(){return work_norm;}
	GEPOINT ColumnNorm(){return column_norm;}
	GEPOINT Center(){return center;}
	double Radius(){return radius;}
	//�����Ƶ�λ
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

class F_ENT_API f3dTriFace : public fAtom{	// ������ṹ
public:
	f3dPoint CalNorm()const;	//����������ķ��߷���
    f3dPoint vertex[3];
	short int feature;	// �����ڼ�¼�����������Ϣ(����ʵ�)
	f3dTriFace(){atom_type=AtomType::prTriFace;}
};

#endif


