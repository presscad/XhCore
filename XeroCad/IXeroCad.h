// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 XEROCAD_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// XEROCAD_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define XEROCAD_API	//内嵌时XEROCAD_API什么都不代表
#else
#ifdef XEROCAD_EXPORTS
#define XEROCAD_API __declspec(dllexport)
#else
#define XEROCAD_API __declspec(dllimport)
#endif
#endif
#pragma once

#include "f_ent.h"
#include "XhCharString.h"
#include "HiberarchyId.h"
#include "list.h"
// 此类是从 XeroCad.dll 导出的
//返回当前版本号，类似Ip地址一样格式如0x01000001表示V1.0.0.1，最末尾版本修订号不影响兼容性，其余会影响兼容性 wjh-2017.4.26
XEROCAD_API DWORD XeroCadDllGetVersion();
/////////////////////////////////////////////////////////////////
//附加数据对象，用以满足在不派生新类基础上，扩充用户自定义数据
struct XEROCAD_API ILifeDataObject{
	static const BYTE INTEGER;	// = 1;
public:
	virtual int GetType()=0;
	virtual void DestroySelf()=0;	//自动销毁
};
struct XEROCAD_API IAttachIntegerData : public ILifeDataObject{
	virtual void SetIntPara(long para)=0;
	virtual long GetIntPara()=0;
};
/////////////////////////////////////////////////////////////////
//基础数据图元对象
class IEntityDatabase;
typedef struct XEROCAD_API IDbEntity{
	static const BYTE DbPoint;	// = 1;
	static const BYTE DbLine;	// = 2;
	static const BYTE DbArcline;	// = 3;
	static const BYTE DbRect;		// = 4;
	static const BYTE DbArrow;		// = 5;
	static const BYTE DbMark;			// = 6;
	static const BYTE DbPolyline;	// = 7;
	static const BYTE DbCircle;		// = 8;
	static const BYTE DbSpline;		// = 9;
	static const BYTE DbText;		// =10;
	static const BYTE DbMText;		// =11;
	static const BYTE DbDiametricDimension;		// =12;
	static const BYTE DbRadialDimension;		// =13;
	static const BYTE DbAngularDimension;		// =14;
	static const BYTE DbAlignedDimension;		// =15;
	static const BYTE DbRotatedDimension;		// =16;
	//State Flag
	static const BYTE NORMAL		= 0x00;
	static const BYTE SELECTED		= 0x01;
	static const BYTE HIDED			= 0x02;
	static const BYTE SNAPDISABLED	= 0x04;
protected:
	virtual IDbEntity* BaseClassObject()=0;
public:
	virtual long GetId();
	virtual HIBERID GetHiberId();	//该图元对应的外部对象标识句柄及层级标识值
	virtual BYTE SetStateFlag(BYTE cStateFlag);
	virtual BYTE GetStateFlag();
	virtual bool SetSelectable(bool bEnable=true);
	virtual bool IsCanSelect();
	virtual BYTE SetSelectLevel(BYTE ciCanSelectLevel=1);
	virtual BYTE CanSelectLevel();
	virtual IEntityDatabase* BelongDatabase();
	virtual bool CloneFrom(IDbEntity* pSrcEnt){return false;}
	virtual long GetDbEntType();
	virtual bool IsDimension(){return false;}
	virtual ILifeDataObject* AppendLifeDataObject(int idDataObjType);
	virtual ILifeDataObject* GetLifeDataObject();
	virtual CXhChar16 Layer();
	virtual HIBERID SetHiberId(HIBERID hiberid);	//该图元对应的外部对象标识句柄及层级标识值
	virtual void SetLayer(char* layer);
	virtual PEN_STRU SetPen(PEN_STRU pen);
	virtual PEN_STRU GetPen();
	virtual void SetUpperDirection(double* upperDirect);
	virtual GEPOINT UpperDirection(bool bCoordGL=true);
	virtual void Offset(double* offset){;}
	virtual void ZoomScale(double zoomCoef){;}	//按比例缩放圆弧线>1为放大<1为缩小
	virtual BOOL CoordTrans(const GECS& cs,bool bTransFromCS=true){return TRUE;}//cs 的各坐标轴方向必须是单位化向量
}*IDbEntityPtr;

struct XEROCAD_API IDbPoint : public IDbEntity{
	//设置属性类接口函数
	virtual GEPOINT SetPosition(GEPOINT pos)=0;
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	//获取属性类接口函数
	virtual GEPOINT Position()=0;
};
struct XEROCAD_API IDbLine : public IDbEntity{
	//设置属性类接口函数
	virtual GEPOINT SetStartPosition(GEPOINT start)=0;
	virtual GEPOINT SetEndPosition(GEPOINT end)=0;
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	//获取属性类接口函数
	virtual GEPOINT StartPosition()=0;
	virtual GEPOINT EndPosition()=0;
};
struct XEROCAD_API IDbCircle : public IDbEntity{
	//设置属性类接口函数
	virtual void SetCenter(double* center)=0;
	virtual void SetNormal(double* norm)=0;
	virtual void SetRadius(double radius)=0;
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	//获取属性类接口函数
	virtual GEPOINT Center()=0;
	virtual GEPOINT Normal()=0;
	virtual double Radius()=0;
};
struct XEROCAD_API IDbArcline : public IDbEntity{
	//根据圆心，圆弧起始点及工作面法线及圆弧扇形角生成圆弧
	virtual BOOL CreateMethod1(double* c,double* startPt,double* work_norm,double angle)=0;
	//根据圆弧起始点，终止点及工作面法线及圆弧扇形角生成圆弧
	virtual BOOL CreateMethod2(double* startPt,double* endPt,double* work_norm,double angle)=0;
	//根据圆弧起始点，终止点、工作面法线及半径生成圆弧,center_pick用于标识圆心是位于弦的左侧还是右侧
	virtual BOOL CreateMethod3(double* startPt,double* endPt,double* work_norm,double r,double* center_pick)=0;
	//根据椭圆的圆心，起始点，终止点，圆柱面法线，工作面法线及圆柱面半径
	virtual BOOL CreateEllipse( double* c,double* startPt,double* endPt,
						double* columnnorm,double* worknorm,double radius)=0;
	virtual GEPOINT StartPosition() const = 0;
	virtual GEPOINT EndPosition() const =0;
	virtual GEPOINT ColumnNorm() =0;
	virtual GEPOINT WorkNorm() =0;
	virtual GEPOINT Center() =0;
	virtual double Radius() =0;
	//弧度制单位
	virtual double SectorAngle() =0;
	//圆弧长度
	virtual double Length() =0;
	virtual f3dArcLine GetArcLine()=0;
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual BOOL SetStart(double x,double y,double z) =0;
	virtual BOOL SetEnd(double x,double y,double z) =0;
	virtual BOOL SetStartPosition(double* startPt)=0;
	virtual BOOL SetEndPosition(double* endPt)=0;
	virtual BOOL SetWorkNorm(double* norm)=0;
	virtual BOOL SetColumnNorm(double* columnnorm)=0;
	virtual BOOL SetCenter(double* center)=0;
	virtual double SetRadius(double R)=0;
	//弧度制单位
	virtual double SetSectorAngle(double angle)=0;	//弧度制单位
	virtual GEPOINT PositionInAngle(double posAngle)=0;
	virtual GEPOINT TangentVecInAngle(double posAngle)=0;	//逆时针旋转为正切向方向
};
struct XEROCAD_API IDbRect : public IDbEntity{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetTopLeft(const double* topLeft)=0;
	virtual void SetBottomRight(const double* btmRight)=0;
	virtual void SetDirectAxisX(const double* directAxisX)=0;
	//获取属性类接口函数
	virtual GEPOINT TopLeft()=0;
	virtual GEPOINT BottomRight()=0;
	virtual GEPOINT DirectAxisX()=0;
};
struct XEROCAD_API IDbArrow : public IDbEntity{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual double SetArrowLength(double arrowLength)=0;
	virtual double SetArrowHeaderLength(double arrowHeaderLength)=0;
	virtual void SetArrowOrigin(const double* origin)=0;
	virtual void SetArrowDirect(const double* direct)=0;
	//获取属性类接口函数
	virtual double ArrowLength()=0;
	virtual double ArrowHeaderLength()=0;
	virtual GEPOINT ArrowOrigin()=0;
	virtual GEPOINT ArrowDirect()=0;
};
struct XEROCAD_API IDbPolyline : public IDbEntity{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual BOOL AddVertexAt(int index,const GEPOINT& vertice,double startWidth=-1,double endWidth=-1)=0;
	virtual BOOL GetVertexAt(int index,GEPOINT& vertice)=0;
	virtual BOOL ModifyVertexAt(int index,GEPOINT& vertice)=0;
	virtual BOOL RemoveVertexAt(int index)=0;
	virtual BOOL SetWidthsAt(int index,double startWidth,double endWidth)=0;
	virtual BOOL GetWidthsAt(int index,double& startWidth,double& endWidth)=0;
	virtual int  numVerts()=0;
	virtual void CloseLoop(bool closing=true)=0;
	virtual bool IsClosedLoop()=0;
	//根据圆弧起始点，终止点及工作面法线及圆弧扇形角生成圆弧
	virtual BOOL CreateArclineByAngle(int index,double* work_norm,double angle)=0;
	//根据圆弧起始点，终止点、工作面法线及半径生成圆弧,center_pick用于标识圆心是位于弦的左侧还是右侧
	virtual BOOL CreateArclineByRadius(int index,double* work_norm,double r,double* center_pick)=0;
	//根据椭圆的圆心，起始点，终止点，圆柱面法线，工作面法线及圆柱面半径
	virtual BOOL CreateEllipse(int index,double* c, double* columnnorm,double* worknorm,double radius)=0;
	virtual BOOL GetArclineAt(int index,f3dArcLine& arcline)=0;
};
struct XEROCAD_API IDbSpline : public IDbEntity{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual BOOL SetFitData(GEPOINT* fitPoints,int num)=0;
	virtual BOOL SetFitData(GEPOINT* fitPoints,int num,const GEPOINT& startTangent,const GEPOINT& endTangent)=0;
	virtual void SetFitTangent(const GEPOINT& startTangent,const GEPOINT& endTangent)=0;
	virtual BOOL SetFitPointAt(int index,const GEPOINT& point)=0;
	virtual BOOL InsertFitPointAt(int index,const GEPOINT& point)=0;
	virtual BOOL RemoveFitPointAt(int index,const GEPOINT& point)=0;
	virtual void ClearFitPointAt()=0;
	virtual int GetNumFitPoints()=0;
	virtual BOOL GetFitPointAt(int index,GEPOINT& point)=0;
	virtual void GetFitTangent(int index,GEPOINT& startTangent,GEPOINT& endTangent)=0;
};
struct XEROCAD_API IDbText : public IDbEntity{
	static const int AlignDefault;		//= 1;
	static const int AlignTopLeft;		//= 2;
	static const int AlignTopCenter;	//= 3;
	static const int AlignTopRight;		//= 4;
	static const int AlignMiddleLeft;	//= 5;
	static const int AlignMiddleCenter;	//= 6;
	static const int AlignMiddleRight;	//= 7;
	static const int AlignBottomLeft;	//= 8;
	static const int AlignBottomCenter;	//= 9;
	static const int AlignBottomRight;	//= 10
protected:
	virtual IDbText* BaseTextObject();
public:
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetPosition(const GEPOINT& pos);
	virtual void SetTextStyleId(UINT_PTR idTextStyle);
	virtual void SetTextString(const char* contents);
	virtual void SetAlignment(int aligment);
	virtual void SetHeight(double height);
	virtual double SetRotation(double rotation);
	virtual bool SetNormal(const double* normal);
	virtual GEPOINT GetNormal();
	virtual bool ParseTextShape(short flowDirection=1);//flowDirection=IDbMText::kLtoR
	virtual UINT TextShapeLineCount();
	virtual bool EnumFirstLine(double* start,double* end);
	virtual bool EnumNextLine(double* start,double* end);
	//获取属性类接口函数
	virtual GEPOINT GetPosition();
	virtual UINT_PTR GetTextStyleId();
	virtual char* GetTextString();	//返回临时指针, 不需要用户释放内存
	virtual int GetAlignment();
	virtual double GetHeight();
	virtual double GetRotation();
};
struct XEROCAD_API IDbMText : public IDbText{
	static const BYTE kLtoR;		// = 1;left to right
	static const BYTE kTtoB;		// = 3;top to bottom
	static const BYTE kByStyle;		// = 5;inherited from the text style at run-time
public:
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetWidth(double width)=0;
	virtual double GetWidth()=0;
	virtual void SetFlowDirection(short flowDirection)=0;
	virtual short GetFlowDirection()=0;
};
struct XEROCAD_API IDbDimension : public IDbEntity{
	virtual bool IsDimension(){return true;}
	virtual void SetDimText(const char* dimText)=0;
	virtual char* GetDimText()=0;	//返回临时指针, 不需要用户释放内存
	virtual UINT_PTR SetDimStyleId(UINT_PTR idDimStyle)=0;
	virtual UINT_PTR GetDimStyleId()=0;
	virtual double SetTextHeight(double fHeight=0)=0;	//=0：表示默认标注样式中指定的字高
	virtual double GetTextHeight()=0;			//表示实际执行的标注文本字高
	virtual double GetUniqueTextHeight()=0;	//表示手动指定的标注文本字高
	virtual double SetArrowSize(double fArrowSize=0)=0;	//=0：表示默认标注样式中指定的简头大小
	virtual double GetArrowSize()=0;	//表示手动指定的标注文本字高
	virtual double GetUniqueArrowSize()=0;	//表示手动指定的标注文本字高
	virtual double SetUpwardOffset(double fUpOffset)=0;
	virtual double UpwardOffset(){return 0;}
	virtual bool UpwardOffsetMode(){return false;}
	virtual void SetNormal(double* normal)=0;
	virtual GEPOINT WorkNormal(bool bCoordGL=true)=0;
	virtual double SetNormalOffset(double fNormalOffset)=0;
	virtual double NormalOffset(){return 0;}
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	virtual bool GetDimRect(GEPOINT rgnVertArr[4],double zoomFontCoef=1.0) const{return false;}
};
//直径标注
struct XEROCAD_API IDbDiametricDimension : public IDbDimension{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetChordPoint(const double* chordPoint)=0;
	virtual void SetFarChordPoint(const double* farChordPoint)=0;
	virtual void SetLeaderLength(const double leaderLength)=0;
	//获取属性类接口函数
	virtual void GetChordPoint(double* chordPoint,bool bCoordsInDCS=true)=0;
	virtual void GetFarChordPoint(double* farChordPoint,bool bCoordsInDCS=true)=0;
	virtual double GetLeaderLength()=0;
};
//半径标注(对应ARX中AcDbRadialDimension)
struct XEROCAD_API IDbRadialDimension : public IDbDimension{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetChordPoint(const double* chordPoint)=0;
	virtual void SetCenterPoint(const double* centerPoint)=0;
	virtual void SetLeaderLength(const double leaderLength)=0;
	//获取属性类接口函数
	virtual void GetChordPoint(double* chordPoint,bool bCoordsInDCS=true)=0;
	virtual void GetCenterPoint(double* farChordPoint,bool bCoordsInDCS=true)=0;
	virtual double GetLeaderLength()=0;
};
//角度标注（对应ARX中AcDb2LineAngularDimension）
struct XEROCAD_API IDbAngularDimension : public IDbDimension{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetArcPoint(const double* arcPoint)=0;
	virtual void SetLine1Start(const double* line1Start)=0;
	virtual void SetLine1End(const double* line1End)=0;
	virtual void SetLine2Start(const double* line2Start)=0;
	virtual void SetLine2End(const double* line2End)=0;
	//获取属性类接口函数
	virtual void GetArcPoint(double* arcPoint)=0;
	virtual void GetLine1Start(double* line1Start)=0;
	virtual void GetLine1End(double* line1End)=0;
	virtual void GetLine2Start(double* line2Start)=0;
	virtual void GetLine2End(double* line2End)=0;
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	virtual bool GetDimRect(GEPOINT rgnVertArr[4],double zoomFontCoef=1.0) const{return false;}
};
//旋转投影尺寸线标注（对应ARX中AcDbAlignedDimension）
struct XEROCAD_API IDbAlignedDimension : public IDbDimension{
	//设置属性类接口函数
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual void SetDimLinePoint(const double* dimLinePoint)=0;
	virtual void SetOblique(const double oblique)=0;
	virtual void SetLine1Point(const double* line1Point)=0;
	virtual void SetLine2Point(const double* line2Point)=0;
	//获取属性类接口函数
	virtual void GetDimLinePoint(double* dimLinePoint,bool bCoordsInDCS=true)=0;
	virtual double GetOblique()=0;
	virtual void GetLine1Point(double* line1Point,bool bCoordsInDCS=true)=0;
	virtual void GetLine2Point(double* line2Point,bool bCoordsInDCS=true)=0;
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	virtual bool GetDimRect(GEPOINT rgnVertArr[4],double zoomFontCoef=1.0) const{return false;}
};
//旋转投影尺寸线标注（对应ARX中AcDbRotatedDimension）
struct XEROCAD_API IDbRotatedDimension : public IDbDimension{
	//设置属性类接口函数
	virtual void SetDimLinePoint(const double* dimLinePoint)=0;
	virtual bool CloneFrom(IDbEntity* pSrcEnt)=0;
	virtual GEPOINT SetOcsAxisX(const double* vector){return GEPOINT(vector);}
	virtual void SetRotationRadian(const double rotation)=0;
	virtual void SetOblique(const double oblique)=0;
	virtual void SetLine1Point(const double* line1Point)=0;
	virtual void SetLine2Point(const double* line2Point)=0;
	//获取属性类接口函数
	virtual void GetDimLinePoint(double* dimLinePoint,bool bCoordsInDCS=true)=0;
	virtual double GetOblique()=0;
	virtual GEPOINT GetOcsAxisX(){return GEPOINT(1,0,0);}
	virtual double GetRotationRadian()=0;
	virtual void GetLine1Point(double* line1Point,bool bCoordsInDCS=true)=0;
	virtual void GetLine2Point(double* line2Point,bool bCoordsInDCS=true)=0;
	_declspec(property (put=SetOcsAxisX,get=GetOcsAxisX)) GEPOINT PrjAxisX;
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	virtual bool GetDimRect(GEPOINT rgnVertArr[4],double zoomFontCoef=1.0) const{return false;}
};
struct DbEntityType{
	IDbEntity* (*CreateNewAtom)(int CLSTYPE_ID,int key);
	BOOL (*DeleteAtom)(IDbEntity *pAtom);
};
struct LifeDataObjectType{
	ILifeDataObject* (*CreateNewAtom)(int idObjClsType,int key);
	BOOL (*DeleteAtom)(ILifeDataObject *pAtom);
};
struct XEROCAD_API ITextStyle{
	virtual UINT GetId()=0;
	virtual bool InitAsciiFontFile(char* file_name)=0;
	virtual bool InitBigFontFile(char* file_name)=0;
	virtual double SetWtoH(double wtoh=0.7)=0;
	virtual double WtoH()=0;
	virtual double SetHeight(double height=3)=0;
	virtual double Height()=0;
	virtual double SetClearance(double clearance=0)=0;
	virtual double Clearance()=0;
	virtual void ParseTextShape(CXhSimpleList<GELINE> *lpLineList,char* text,double height,double WtoH=0.7,WORD wAlignStyle=6,short flowDirection=IDbMText::kLtoR,double clearance=0)=0;
	virtual void CalTextDrawSize(char* text,double height,double* size_w,double* size_h,double WtoH=0.7,double clearance=0)=0;
};
struct XERO_DIMSTYLEVAR
{
	static const BYTE DIMBLK_ARROWLINE30	= 0;
	static const BYTE DIMBLK_ARROWLINE90	= 1;
	//static const BYTE DIMBLK_ARROWSOLID	= 2;
	static const BYTE DIMBLK_ARROWHOLLOW	= 3;
	//static const BYTE DIMBLK_POINTSOLID	= 4;
	static const BYTE DIMBLK_POINTHOLLOW	= 5;
	//static const BYTE DIMBLK_SQUARESOLID	= 6;
	static const BYTE DIMBLK_SQUAREHOLLOW	= 7;
	static const BYTE DIMBLK_SLASH			= 8;
	char m_ciDimBlk;		//标注及箭头的箭头标记类型Id
	char m_ciVerticalPosType;//相当于ACAD的setDimtad(...),0.居中 1.文字位于标注线上方 2.外部 3.JIS 4.下方
	long m_idTextStyle;		//文本样式Id
	double m_fTextHeight;	//标注文本高度,为0时根据指定的文本样式绘制高度
	double m_fDimExtend;	//尺寸界线在标注线上方的延伸量
	double m_fDimGap;		//标注线与文字间距应为
	double m_fArrowSize;	//箭头长度
	XERO_DIMSTYLEVAR(){
		m_ciDimBlk=DIMBLK_ARROWLINE30;
		m_ciVerticalPosType=1;
		m_idTextStyle=0;
		m_fTextHeight=2.5;
		m_fDimExtend=5;
		m_fDimGap=2;
		m_fArrowSize=3;
	}
};
struct XEROCAD_API IDimStyle{
	virtual long SetDimStyleVar(XERO_DIMSTYLEVAR dimvar)=0;
	virtual bool GetDimStyleVar(XERO_DIMSTYLEVAR *pdimvar)=0;
};

class XEROCAD_API IEntityDatabase{
public:
	virtual long GetId()=0;
	virtual bool SetOCS(GECS& ocs)=0;	//设定在原始模型坐标系中进行描述的绘图坐标系
	virtual void GetOCS(GECS& ocs)=0;
	virtual IDbEntity* AppendDbEntity(int idDbEntType)=0;
	virtual BOOL DeleteDbEntity(long id)=0;
	virtual BOOL DeleteCursorDbEntity(BOOL bClean=FALSE)=0;
	virtual void CleanDeletedEntities()=0;
	virtual BOOL DeleteDbEntityByHiberMasterId(long hiberMasterId)=0;
	virtual void EmptyDbEntities()=0;
	virtual IDbEntity* GetDbEntity(long id)=0;
	virtual IDbEntity* EnumFirstDbEntity()=0;
	virtual IDbEntity* EnumNextDbEntity()=0;
	virtual IDbEntity* EnumCursorDbEntity()=0;
	virtual long GetEntityCount()=0;
	virtual void RegisterDbEntityType(int idDbEntType,DbEntityType regType)=0;
	virtual void UnRegisterAllEntityType()=0;
	virtual void RegisterLifeDataObjType(int idDataObjClsType,LifeDataObjectType regType)=0;
	virtual void UnRegisterAllLifeDataObjType()=0;
	virtual ITextStyle* AddTextStyle(char* name,char* asciiFontFile,char* bigFontFile)=0;
	virtual ITextStyle* GetTextStyle(DWORD idTextStyle)=0;
	virtual long AddDimStyle(char* label,XERO_DIMSTYLEVAR dimvar)=0;
	virtual long SetDimStyle(char* label,XERO_DIMSTYLEVAR dimvar)=0;
	virtual long SetDimStyle(long idDimStyle,XERO_DIMSTYLEVAR dimvar)=0;
	virtual IDimStyle* GetDimStyle(char* label,XERO_DIMSTYLEVAR *pdimvar=NULL)=0;
	virtual IDimStyle* GetDimStyle(long idDimStyle,XERO_DIMSTYLEVAR *pdimvar=NULL)=0;

	virtual ILifeDataObject* AppendLifeDataObject(long idEntity,int idDbEntType)=0;
	virtual BOOL DeleteLifeDataObject(long id)=0;
	virtual void EmptyLifeDataObjects()=0;
	virtual ILifeDataObject* GetLifeDataObject(int idEntity)=0;
	virtual ILifeDataObject* EnumFirstLifeDataObject()=0;
	virtual ILifeDataObject* EnumNextLifeDataObject()=0;
	virtual long GetLifeDataObjectCount()=0;
public:
	static IEntityDatabase* CreateEntityDatabase();
	static IEntityDatabase* EntityDatabaseFromSerial(long serial);
	static BOOL DestroyEntityDatabase(long serial);
};

struct IDrawing;
struct IDrawingComponent;
struct IDrawingCommand;
struct DrawingCommandType{
	IDrawingCommand* (*CreateNewAtom)(int CLSTYPE_ID,int key,void* pContext);
	BOOL (*DeleteAtom)(IDrawingCommand *pAtom);
};
struct DrawingComponentType{
	IDrawingComponent* (*CreateNewAtom)(int CLSTYPE_ID,int key,void* pContext);
	BOOL (*DeleteAtom)(IDrawingComponent *pAtom);
};
struct DrawingType{
	IDrawing* (*CreateNewAtom)(int key,void* pContext);
	BOOL (*DeleteAtom)(IDrawing *pAtom);
};
struct XEROCAD_API IDrawingCommand{
	virtual int GetDrawCmdType()=0;
	virtual void* SetContext(void* pContext)=0;	//设定上下文指针
	virtual void* GetContext()=0;				//获取上下文指针
	virtual IDrawing* BelongDrawing()=0;
	virtual IDrawingComponent* BelongDrawingComponent()=0;
	virtual IDbEntity* EnumFirstDbEntity()=0;
	virtual IDbEntity* EnumNextDbEntity()=0;
	virtual IDbEntity* AppendDbEntity(int idDbEntType);
	virtual IDbEntity* AppendDbEntityRef(int idDbEnt,IDbEntity* pDbEnt)=0;
	virtual void EmptyDbEntitySet()=0;
	virtual void Redraw();
public:
	static void RegisterDrawCmdType(int idDrawCmdType,DrawingCommandType regType);
	static void UnRegisterAllDrawCmdType();
	static DrawingCommandType* EnumFirstRegType();
	static DrawingCommandType* EnumNextRegType();
	static long GetCursorRegType();
	static void EmptyRegTypes();
};
struct XEROCAD_API IDrawingComponent{
	virtual int GetComponentType()=0;
	virtual void* SetContext(void* pContext)=0;	//设定上下文指针
	virtual void* GetContext()=0;				//获取上下文指针
	virtual IDrawing* BelongDrawing()=0;
	virtual IDrawingCommand* EnumFirstDrawingCommand()=0;
	virtual IDrawingCommand* EnumNextDrawingCommand()=0;
	virtual IDbEntity* EnumFirstDbEntity()=0;
	virtual IDbEntity* EnumNextDbEntity()=0;
	virtual IDbEntity* AppendDbEntity(int idDbEntType);
	virtual IDbEntity* AppendDbEntityRef(int idDbEnt,IDbEntity* pDbEnt)=0;
	virtual IDrawingCommand *AppendDrawingCommand(int cmd_type)=0;
	virtual void EmptyDbEntitySet()=0;
	virtual void Redraw();
public:
	static void RegisterDrawComponentType(int idDrawComponentType,DrawingComponentType regType);
	static void UnRegisterAllDrawComponentType();
	static DrawingComponentType* EnumFirstRegType();
	static DrawingComponentType* EnumNextRegType();
	static long GetCursorRegType();
	static void EmptyRegTypes();
};
struct XEROCAD_API IDrawing{
	virtual long GetId()=0;
	virtual void* SetContext(void* pContext)=0;	//设定上下文指针
	virtual void* GetContext()=0;				//获取上下文指针
	virtual bool SetOCS(GECS& ocs)=0;	//设定在原始模型坐标系中进行描述的绘图坐标系
	virtual void GetOCS(GECS& ocs)=0;
	virtual int SetName(char* name)=0;
	virtual int GetDrawType()=0;
	virtual IDrawingComponent* EnumFirstDrawingComponent()=0;
	virtual IDrawingComponent* EnumNextDrawingComponent()=0;
	virtual IDrawingCommand* EnumFirstDrawingCommand()=0;
	virtual IDrawingCommand* EnumNextDrawingCommand()=0;
	virtual IDbEntity* EnumFirstDbEntity()=0;
	virtual IDbEntity* EnumNextDbEntity()=0;
	virtual IDbEntity* EnumCursorDbEntity()=0;
	virtual IDrawingCommand   *AppendDrawingCommand(int cmd_type)=0;
	virtual IDrawingComponent *AppendDrawingComponent(int component_type)=0;
	virtual BOOL DeleteDrawingComponent(long serial)=0;
	virtual BOOL DeleteDrawingCommand(long serial)=0;
	virtual IDbEntity* AppendDbEntity(int idDbEntType)=0;
	virtual IDbEntity* GetDbEntity(long id)=0;
	virtual BOOL DeleteDbEntity(long id)=0;
	virtual BOOL DeleteCursorDbEntity(BOOL bClean=FALSE)=0;
	virtual void CleanDeletedEntities()=0;
	virtual BOOL DeleteDbEntityByHiberMasterId(long hiberMasterId)=0;
	virtual void EmptyDrawingCommands()=0;
	virtual void EmptyDrawingComponents()=0;
	virtual void EmptyDbEntities()=0;
	virtual BYTE SetRenderedMode(BYTE cbRenderMode=0)=0;
	virtual BYTE RenderMode()=0;	//0.不需要重新渲染;1.部分图元需要重新渲染;2.全部图元需要重新渲染
	virtual IEntityDatabase* Database()=0;
	virtual void Redraw();
public:
	static void RegisterDrawingType(int idDrawingType,DrawingType regType);
	static void UnRegisterAllDrawingType();
};
struct XEROCAD_API IDrawingAssembly{
	virtual long GetSerialId()=0;
	virtual bool SetOCS(GECS& ocs)=0;	//设定在原始模型坐标系中进行描述的绘图坐标系
	virtual void GetOCS(GECS& ocs)=0;
	virtual void* SetContext(void* pContext)=0;	//设定上下文指针
	virtual void* GetContext()=0;
	virtual IDrawing* EnumFirstDrawing()=0;
	virtual IDrawing* EnumNextDrawing()=0;
	virtual IDrawing* GetDrawing(long idDrawing)=0;
	virtual void EmptyDrawings()=0;
	virtual IDrawing* AppendDrawing(int type,char* name)=0;
	virtual BOOL DeleteDrawing(long serial)=0;
};

class XEROCAD_API CDrawingAssemblyTable{
public:
	static IDrawingAssembly* CreateDrawingAssemblyInstance();
	static IDrawingAssembly* DrawingAssemblyFromSerial(long serial);
	static BOOL Destroy(long serial);
};
/////////////////////////////////////////////////////////////////
//原始模型数据对象，即生成图纸集的依据
struct XEROCAD_API IDrawingDataObject{
	virtual int RetrieveDrawings(IDrawingAssembly* pDrawingAssembly);
	virtual int RetrieveDrawingComponents(IDrawing* pDrawing);
	virtual int RetrieveDrawingCommands(IDrawingComponent* pComponent);
};
