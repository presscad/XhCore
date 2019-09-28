#pragma once

#include "gl\glu.h"
#include "f_ent.h"
#include "f_ent_list.h"
//#include "f_alg_fun.h"
#include "UserDefOper.h"
#include "SolidBody.h"
#include "HiberarchyId.h"
/////////////////////////////////////////////////////////////////////////////
enum DISPLAY_TYPE{DISP_LINE,DISP_BOUNDARY,DISP_SOLID,DISP_DRAWING};

#ifndef __CLS_CSOLIDPART_DEFINED_
#define __CLS_CSOLIDPART_DEFINED_
class CSolidPart{
public:
	HIBERID hiberid;
	long get_m_hPart() const{return (long)hiberid.masterId;}
	long set_m_hPart(long hPartId){
		hiberid.masterId=(DWORD)hPartId;
		return hPartId;
	}
	// 构件的句柄
	__declspec(property(put=set_m_hPart,get=get_m_hPart)) long m_hPart;
	int  m_idClassType;	// 构件的类型
	GECS acs;			// 构件的装配坐标系
	BOOL m_bDisplaySingle;	//线型构件是否需要在单线图中显示
	BOOL m_bSelected;		//构件是否当前被选中(此属性只由程序控制)
	int m_iLineType;		//0:构件为实体类型；1:构件为直线型构件；2:构件为弧线型构件; 3:箭头
	f3dLine line;		// 单线状态下的简化线及	//箭头长度
	f3dArcLine arc;		// 单线状态下的弧线简化线
	double dfSphereRadius;
	CSolidBody *pBody;		// 显示构件的实体显示数据
public:
	CSolidPart(){
		m_iLineType=1;
		m_bDisplaySingle=TRUE;
		m_bSelected=FALSE;pBody=NULL;
		m_idClassType=0;
		dfSphereRadius=0;
	}
};
#endif
struct SKETCH_PLANE_DISPLAY{
	bool bArrowAlongNorm;
	UINT uArrowScrLen,nVertexCount;
	double fUnderPlaneModelLength;	//平面法线箭头示意图,自平面基点向平面下延伸的实际长度(非屏幕长度)
	COLORREF crPlane,crArrowLine;
	GEPOINT vNormal,*pVertexArr;
};
struct OBJECT_HANDLER{
	HIBERID hiberid;
	int idClassType;
	OBJECT_HANDLER(){hiberid.masterId=idClassType=0;}
	OBJECT_HANDLER(HIBERID _hiberid,int _idClassType)
	{
		hiberid=_hiberid;
		idClassType=_idClassType;
	}
};
struct PRINTPAGE{
	static const int AlignDefault		= 1;//
	static const int AlignTopLeft		= 2;//左上对齐;
	static const int AlignTopCenter 	= 3;//上中央对齐;
	static const int AlignTopRight		= 4;//左中对齐;
	static const int AlignMiddleLeft	= 5;//
	static const int AlignMiddleCenter	= 6;//中央对齐;
	static const int AlignMiddleRight	= 7;//
	static const int AlignBottomLeft	= 8;//左下对齐;
	static const int AlignBottomCenter	= 9;//下中央对齐;
	static const int AlignBottomRight	=10;//
	char ciAlignMode;	//打印时图形相对于纸张的对齐模式
	BYTE ciPenWidthPerPixel;//每原始单位线宽（一般为屏幕显示像素）代表的打印机线宽逻辑单位数
	char ciLayout;			//0.自适应,1.纵向打印;2.横向打印LyingMode
	union MARGIN{
		struct{BYTE ciTop,ciBottom,ciLeft,ciRight;};
		DWORD dwMargin;	//毫米为单位
	}margin;	//边距
	int iValidOffsetX;	//有效要印区域左上角与纸张左上角间的偏移,单位:DC的逻辑坐标单位
	int iValidOffsetY;
	WORD wWidth,wHeight;			//打印机DC的实际打印区域的宽(X向)与高(Y向),单位:DC的逻辑坐标单位
	WORD wActualWidth,wActualHeight;//根据用户设定边距执行的实际打印时的有效打印区域的宽与高,单位:DC的逻辑坐标单位
	double scaleDevicetoModel;		//设备尺寸/模型尺寸的比例,0表示最大比例全显
	POINT printStartPoint;			//实际打印时起始原(零)点,相对于打印机DC的实际打印区域,单位:DC的逻辑坐标单位
	SIZE sizePaper;					//当前打印机使用的纸张大小,单位:DC的逻辑坐标单位
	PRINTPAGE(){
		ciLayout=0;
		ciPenWidthPerPixel=1;
		ciAlignMode=AlignMiddleCenter;
		margin.dwMargin=0;
		scaleDevicetoModel=1;
		iValidOffsetX=iValidOffsetY=0;
		wActualWidth=wActualHeight=0;
	}
};
struct ISolidDraw
{
	//文件对齐类型，用于NewText()
	static const WORD TEXT_ALIGN_TOPLEFT	= 0;//0.左上角
	static const WORD TEXT_ALIGN_TOPCENTER	= 1;//1.中上
	static const WORD TEXT_ALIGN_TOPRIGHT	= 2;//2.右上
	static const WORD TEXT_ALIGN_MIDLEFT	= 3;//3.左中
	static const WORD TEXT_ALIGN_MIDCENTER	= 4;//4.中中
	static const WORD TEXT_ALIGN_MIDRIGHT	= 5;//5.右中
	static const WORD TEXT_ALIGN_BTMLEFT	= 6;//6.左下
	static const WORD TEXT_ALIGN_BTMCENTER	= 7;//7.中下
	static const WORD TEXT_ALIGN_BTMRIGHT	= 8;//8.右下
	//坐标系标识，用于AddCS
	static const WORD MODEL_CS = 1;		//当前操作空间的模型坐标系
	static const WORD WORK_CS  = 2;		//工作坐标系，即当前输入坐标系
	static const WORD BLK_LOCATE_CS = 3;//部件自身定位坐标系，与装配坐标系一起定义部件装配方位与装配坐标系一起定义部件装配方位
	virtual void InitialUpdate()=0;
	virtual bool ModifyDbPoint(long idPoint,const double* point_coords)=0;
	virtual void NewPoint(double x0, double y0, double z0,long ID=0,HIBERARCHY hiber=0)=0;
	virtual void NewPoint(const f3dPoint& pt,HIBERARCHY hiber=0)=0;
	virtual void NewLine(double x0, double y0, double z0, double x1, double y1, double z1,long ID=0,HIBERARCHY hiber=0)=0;
	virtual void NewLine(const f3dPoint& pt1, const f3dPoint& pt2,long ID=0,HIBERARCHY hiber=0)=0;
	virtual void NewLine(const f3dLine& line,HIBERARCHY hiber=0)=0;
	virtual void NewSolidPart(const CSolidPart& solid_part)=0;
	virtual bool GetSolidObject(DWORD idSolidPart,OBJECT_HANDLER* pObjHandler)=0;	//根据系统分配的Id查找对应的构件原始标识信息 wjh-2019.4.6
	virtual UINT QueryBuffSolidObject(const HIBERID& hiberid)=0;//根据当前实体显示对象的复合分级Id查找显示缓存中对应显示对象的32位Id wjh-2019.4.9
	virtual void NewArcLine(const f3dArcLine& arc,HIBERARCHY hiber=0)=0;
	virtual void NewCircle(const f3dCircle& circle,HIBERARCHY hiber=0)=0;
	virtual void NewRect(f3dRect &rect,HIBERARCHY hiber=0)=0;
	virtual void NewWorkPlaneSketch(UINT idWorkPlane,SKETCH_PLANE_DISPLAY* pSketchPlane,const char* prompt)=0;
	virtual void NewWorkPlaneSketch(UINT idWorkPlane,COLORREF crMaterial,GEPOINT* pVertexArr,UINT count, const double* normal,const char* prompt,UINT uArrowScrLen=150,bool bAlongNorm=true,double fUnderPlaneModelLen=0)=0;
	virtual BOOL DelWorkPlaneSketch(UINT idWorkPlane)=0;
	virtual void NewComplexFace(f3dPolyFace *pLoopFace)=0;
	//以下三个文本绘制函数尚未实现
	virtual void CalTextDrawSize(char* text,double height,double* size_w,
		double* size_h=NULL,double WtoH=0.7,double clearance=0)=0;
	virtual void NewText(char* text,f3dPoint pos,double height,double clearance=0)=0;
	virtual void NewText(char* text,f3dPoint pos,double height,
		f3dPoint& align_vec,f3dPoint& norm,WORD wAlignStyle,double margin=0,double clearance=0)=0;
	virtual void AddCS(long ID,UCS_STRU cs,char* symbol_x=NULL,char* symbol_y=NULL,
		char* symbol_z=NULL,char* symbol_o=NULL,long axis_len=100)=0;
	virtual void DelCS(long ID)=0;
	virtual void BatchClearCS(ULONG minCsId=0)=0;

	virtual void EmptyDisplayBuffer()=0;
	virtual void BuildDisplayList(void* pContext=NULL)=0;
	virtual void Draw()=0;	//向屏幕窗口绘制图形主函数
	virtual void Print(CDC* pDC,PRINTPAGE& pageinfo)=0;	//向屏幕窗口绘制图形主函数

	virtual void NewArrow(const f3dPoint& pt,const f3dPoint& vec,const double arrow_len)=0;
	virtual void EmptyArrowSet()=0;
	virtual void NewDragSetPoint(const f3dPoint& pt)=0;
	virtual void NewDragSetLine(const f3dPoint& pt1, const f3dPoint& pt2)=0;
	virtual void EmptyDragSet()=0;

	virtual void NewMark(f3dPoint pos,int scope=5, AtomType::Mark nMarkType=AtomType::mkX)=0;
	virtual BOOL DestroyMark(int ID)=0;
	virtual int  DestroyAllMark()=0;
	virtual BOOL DelEnt(int ID)=0;
	virtual void ReleaseSnapStatus()=0;
	virtual BOOL SetEntSnapStatus(long ID, bool bStatus=true)=0;
	virtual BOOL SetEntSnapStatus(long* id_arr, DWORD size,bool bStatus=true)=0;
	virtual BOOL SetAllSnapStatus(bool bStatus=true)=0;
	virtual BOOL ExportBmpFile(char *file_name,int left,int bottom,int right,int top,int width,int height)=0;
	virtual int CalArcResolution(double radius,double sector_angle)=0;
};
#include "IXeroCad.h"
struct I2dDrawing
{
	virtual IDrawingAssembly* GetDrawingAssembly()=0;
	virtual IDrawingAssembly* InitDrawingAssembly(IDrawingAssembly* pAssembly=NULL)=0;
	virtual IDrawing* GetActiveDrawing()=0;
	virtual IDrawing* SetActiveDrawing(long idDrawing)=0;
	virtual void RenderDrawing()=0;
};
struct LIVE_DISPLAY_CONTEXT{HWND hWnd;HDC hDC;};
struct ISolidSet
{
	virtual bool Init(HWND hWnd)=0;   // 初始化OpenGL
	virtual bool Init(HWND hWnd,HWND hParentWnd)=0;   // 初始化OpenGL
	virtual void Activate()=0;		//多个引擎交叉使用时，只能同时启用一个OpenGL引擎 wjh-2015.11.18
	virtual void SetClassTypeIdTable(int arrClsTypeIdTable[32])=0;
	virtual IDrawing* SetSolidBuddyDrawing(long idBuddyDrawing)=0;
	virtual IDrawing* GetSolidBuddyDrawing()=0;
	virtual void SetActiveDisplayBuffer(DWORD serial)=0;
	virtual DWORD GetActiveDisplayBuffer()=0;
	virtual BOOL RemoveActiveDisplayBuffer(DWORD serial)=0;
	virtual PEN_STRU SetPen(COLORREF crColor, int style, int width)=0;
	virtual PEN_STRU SetPen(PEN_STRU pen)=0;
	virtual void SetPointSize(WORD pointsize)=0;
	virtual WORD GetPointSize()=0;
	virtual void SetPointStyle(BYTE ciPointStyle)=0;
	virtual BYTE GetPointStyle()=0;
	virtual void SetCreateEnt(BOOL bStatus=TRUE)=0;
	virtual void SetOperType(OPER_TYPE oper_type)=0;
	virtual OPER_TYPE GetOperType()=0;
	virtual void SetSolidAndLineStatus(BOOL bSolidAndLine)=0;
	virtual BOOL GetSolidAndLineStatus()=0;
	virtual BOOL SetObjectUcs(const UCS_STRU &ucs)=0;
	virtual void GetObjectUcs(UCS_STRU &ucs)=0;
	virtual void SetDispPlaneNorm(const double* normPlane,const double* normOrg,const double* axisX=NULL)=0;
	virtual void SetEnvTypeId(long env_id)=0;
	virtual long GetEnvTypeId()=0;
	virtual void SetDispObjectUcs(BOOL bDisplay=TRUE)=0;
	virtual DISPLAY_TYPE SetDisplayType(DISPLAY_TYPE disp=DISP_LINE)=0;
	virtual void GetDisplayType(DISPLAY_TYPE *disp)=0;
	DISPLAY_TYPE get_eDisplayMode() {
		DISPLAY_TYPE mode;
		GetDisplayType(&mode);
		return mode;
	}
	__declspec(property(get = get_eDisplayMode)) DISPLAY_TYPE eDisplayMode;
	virtual void SetDisplayLinePoints(BOOL bDisplay=TRUE)=0;	// 设置重显函数
	virtual void SetDisplayLineVecMark(BOOL bDisplay=TRUE)=0;	// 设置重显函数
	virtual void SetBkSceneMode(int mode)=0;	//0.单色背景;1.渐变背景
	virtual void SetBkColor(const COLORREF &crBkColor)=0;
	virtual void SetSelectEntsColor(COLORREF crColor)=0;
	virtual COLORREF GetSelectEntsColor()=0;
	virtual void SetSelectEntsPenStyle(short penStyle)=0;
	virtual short GetSelectEntsPenStyle()=0;
	virtual void GetBkColor(COLORREF *crBkColor)=0;
	virtual void SetRotOrg(f3dPoint org)=0;
	virtual void GetRotOrg(f3dPoint *org)=0;
	virtual void SetDragSetBasePos(f3dPoint pos)=0;
	virtual void SetRotRadius(const double& radius)=0;
	virtual void GetRotRadius(double *radius)=0;
	virtual void SetRotRadAngle(const double& angle)=0;
	virtual double GetRotRadAngle()=0;
	virtual void SetZoomStyle(ZOOM_CENTER_STYLE style)=0;
	virtual void GetZoomStyle(ZOOM_CENTER_STYLE *style)=0;
	virtual bool LineModelDepthTestState()=0;
	virtual void EnableLineModelDepthTest()=0;
	virtual void DisableLineModelDepthTest()=0;

	virtual void  SetDrawType(DRAW_TYPE draw_type)=0;
	virtual DRAW_TYPE GetDrawType()=0;
	virtual void SetDisplayFunc(void (*fun)(void* pContext))=0;		// 设置预置重显函数
	virtual void SetLiveDisplayFunc(void (*fun)(LIVE_DISPLAY_CONTEXT* pContext))=0;	// 设置实时绘制函数
	virtual void SetDelObjectFunc(int (*pFunc)(int ID,int ent_type))=0;
	virtual void SetSolidBodyCreateFunc(CSolidBody* (*FireSolidPartBody)(long hPart,long idClassType))=0;
	virtual void SetBmpScopeFunc(void (*FireBmpScope)(long left,long bottom,long right,long top))=0;
	virtual void SetEntPickedFunc(long (*FireEntPicked)(long ID))=0;
	virtual void SetEntSnapPosFunc(bool (*pFunc)(long ID,const double* front_pos,const double* back_pos,double* nearest_pos))=0;
	virtual bool SetShxFontFile(char* file_name)=0;
	virtual bool SetBigFontFile(char* file_name)=0;
	virtual BOOL EnableActive(BOOL bActive=TRUE)=0;
	virtual UINT GetArcSamplingLength()=0;
	virtual void SetArcSamplingLength(UINT uLength)=0;
	virtual UINT GetSmoothness()=0;
	virtual void SetSmoothness(UINT slices)=0;
	virtual void ExitComponent()=0;
	/*	 
	STDMETHOD_(glPEN, SetPen)(COLORREF crColor, int style=0,float width=2)=0;
	STDMETHOD_(glPEN, SetPen)(glPEN pen)=0;
	// 绘图函数
	STDMETHOD_(void,DrawCoordinate)(void)=0;// 画坐标系*/
};
struct ISnapTypeVerify{
	virtual bool IsValidObjType(char ciProvider,int iObjClsType)=0;
	virtual bool IsValidEntityObjType(int iObjClsType)=0;
	virtual bool IsValidSolidObjType(int iObjClsType)=0;
	virtual bool IsValidGeObjType(int iObjClsType)=0;
	virtual bool IsEnableSnapSpace(int providerSpace)=0;	//1.DRAWINGSPACE;2.SOLIDSPACE;3.LINESPACE
	virtual DWORD GetSnapTypeFlag(char ciProvider){return 0xffffffff;}
	virtual BYTE GetEntsSelectLevel()=0;
};
struct ISolidSnap
{
	struct SNAPMSGLOOP_CONTEXT{
		//调用SnapObject的客户端上下文环境
		void* pExterContext;
		//窗口消息
		HWND hMsgSrcWnd;
		UINT        message;
		WPARAM      wParam;
		LPARAM      lParam;
	};
	virtual double SetOSnapScope(UINT scope)=0;
	virtual void SetSnapType(int snap_type)=0;
	virtual DWORD SetSelectPartsType(DWORD dwSelectTypeFlag)=0;
	virtual int SnapObject(DWORD* phObj,DWORD* pdwExportFlag,ISnapTypeVerify* pSnapTypeVerify=NULL,
		int (*BreakSnapAsReturnDetectFunc)(SNAPMSGLOOP_CONTEXT* pContext)=NULL,void* pExternalContext=NULL)=0;
	virtual int SnapPoint(f3dPoint *&point,BOOL bSnapAllStatus=FALSE)=0;
	virtual int SnapLine(f3dLine &line,BOOL bSnapAllStatus=FALSE)=0;
	virtual int SnapCircle(f3dCircle *&circle,BOOL bSnapAllStatus=FALSE)=0;
	virtual int SnapNearPoint(f3dPoint &point,const f3dLine &line3d)=0;
	virtual int GetLastSelectEnts(long *&id_arr,HIBERARCHY** ppHiberarchy=NULL)=0;	//不用用户管理(释放)内存
	virtual int InverseSelectEntsStatus()=0;	//反转对象的选中状态
	virtual fAtom* GetCurSnapedObject()=0;
};
struct ISolidOper
{
	virtual void ReSize()=0;
	//scr_org为屏幕上对应的绘图原点,drawPlaneCS为通过绘图坐标系(以X和Y轴作为绘图平面，Z轴无意义)来描述的绘图平面
	virtual bool SpecView(POINT scr_org,UCS_STRU drawPlaneCS,double user2scr_scale)=0;
	virtual void ReSetView()=0;
	virtual void ZoomAll(double scale)=0;          // 屏幕全显       
	// 屏幕开窗
	virtual void ZoomWnd(CPoint topLeft,CPoint bottomRight)=0;
	//scope为聚焦范围,maxScaleScr2User为最大放大倍数
	virtual void FocusTo(SCOPE_STRU&scope ,double maxScaleScr2User,long idHighLightObj=0)=0;
	virtual void Scale(double zoom_scale = 1.0)=0;  		 // 屏幕缩放    
	virtual void RefreshScope()=0;
	virtual void DrawShape(CPoint TopLeft, CPoint BottomRight)=0;
	virtual BOOL IsHasHighlightPoint(f3dPoint *point)=0;
	// 图形操作函数
                            			
	virtual int KeyDown(UINT nChar)=0;// 按键操作
	virtual void MouseMove(CPoint point,LPARAM lParam=0)=0;// 鼠标移动函数
	virtual void LMouseDown(CPoint point)=0; // 按鼠标函数
	virtual void LMouseUp(CPoint point)=0; // 鼠标松开函数
	virtual void LMouseDoubleClick(CPoint point)=0; // 按鼠标函数
	virtual void RMouseDown(CPoint point)=0; // 按鼠标函数
	virtual void RMouseUp(CPoint point)=0; // 鼠标松开函数
	virtual void PushEnv()=0; 
	virtual bool PopEnv()=0; 
	//env_blob为预留存储显示环境的内存块指针(可以为NULL)，返回值为所需内存大小
	virtual UINT GetCurrEnvionment(char* env_blob)=0;
	virtual bool SetCurrEnvionment(char* env_blob)=0;	//env_blob为存储当前要指定显示环境的内存块指针（固定格式不需要大小）

	virtual double GetScaleUserToScreen()=0;
	virtual double UserToScreen(double user_x, double user_y, double user_z, double *port_x, double* port_y)=0;
	virtual double UserToScreen(const f3dPoint& user_pt, f2dPoint *port_pt)=0;
	virtual double UserToScreen(const f3dPoint& user_pt, f3dPoint *port_pt)=0;
	virtual double UserToScreen(const f3dLine& line_3d, f2dLine *line_2d)=0;
	virtual double UserToScreen(const f3dCircle& circle_3d, f2dCircle *circle_2d)=0;
	virtual double UserToScreen(const f3dRect& rect_3d, f2dRect *rect_2d)=0;

	virtual void  ScreenToUser(double *user_x, double *user_y, double *user_z, double port_x, double port_y, double port_z = 0.5)=0;
	virtual void  ScreenToUser(f3dPoint *user_pt,const f2dPoint& port_pt)=0;
	virtual void  ScreenToUser(f3dPoint *user_pt,const f3dPoint& port_pt)=0;
	virtual void  ScreenToUser(f3dLine *line_3d,const f2dLine& line_2d)=0;
	virtual void  ScreenToUser(f3dCircle *circle_3d,const f2dCircle& circle_2d)=0;
	virtual void  ScreenToUser(f3dRect *rect_3d,const f2dRect& rect_2d)=0;
	//用户模型坐标与绘图引擎内部的坐标系转换函数
	virtual GEPOINT TransPFromGL(const double* point)=0;
	virtual GEPOINT TransVFromGL(const double* vector)=0;
	virtual GEPOINT TransPToGL(const double* point)=0;
	virtual GEPOINT TransVToGL(const double* vector)=0;
	//用户模型坐标与屏幕坐标系转换函数
	virtual GEPOINT TransPFromScr(const double* point)=0;
	virtual GEPOINT TransVFromScr(const double* vector)=0;
	virtual GEPOINT TransPToScr(const double* point)=0;
	virtual GEPOINT TransVToScr(const double* vector)=0;
};
//实体绘制引擎接口
struct IDrawSolid
{
public:	//接口
	virtual long GetSerial()=0;
	virtual ISolidDraw* SolidDraw()=0;	//获取引擎绘图子接口
	virtual ISolidSet * SolidSet()=0;	//获取引擎设置子接口
	virtual ISolidSnap* SolidSnap()=0;	//获取引擎捕捉子接口
	virtual ISolidOper* SolidOper()=0; //获取引擎操作子接口
	virtual I2dDrawing* Drawing2d()=0; //获取引擎二维图子接口
};
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
struct CDrawSolidFactory{
#else
struct AFX_EXT_CLASS CDrawSolidFactory{
#endif
public:	//绘制引擎生成函数
	static IDrawSolid* CreateDrawEngine();
	static IDrawSolid* DrawEngineFromSerial(long serial);
	static BOOL Destroy(long serial);
	static DWORD DrawSolidDllGetVersion();
	static bool ValidateVersionCompatible(char* errmsg=NULL,int nMsgStrBuffLen=50);
};
