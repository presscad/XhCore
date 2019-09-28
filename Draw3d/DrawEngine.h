// DrawEngine.h: interface for the CDrawEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWENGINE_H__41754DB4_F5AA_4418_A6B7_C8AFED5A226D__INCLUDED_)
#define AFX_DRAWENGINE_H__41754DB4_F5AA_4418_A6B7_C8AFED5A226D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f_ent.h"
#include "UserDefOper.h"
struct AFX_EXT_CLASS CDrawEngine  
{
public:
	enum TEXT_ALIGN_STYLE{TopLeft,TopCenter,TopRight,MiddleLeft,MiddleCenter,MiddleRight,BottomLeft,BottomCenter,BottomRight};
	DWORD dwParam;	//用于记录绘图内容相关的附属信息
	static CDrawEngine* CreateEngine();
	static CDrawEngine* FromHandle(long h);
	static BOOL Destroy(long h);
public:
	virtual long GetEngineHandle()=0;
	//1.绘图类函数
	virtual bool Init(HWND hWnd,LPRECT lpViewportRect=NULL)=0;   // 初始化OpenGL
	virtual bool UpdateViewport(LPRECT lpViewportRect=NULL)=0;   // 初始化OpenGL
	virtual void InitialUpdate()=0;
	virtual void BuildDisplayBuffer()=0;
	virtual void ReGenerate()=0;	//重新绘制屏幕上的图形数据
	virtual void Draw(CDC *pDC)=0;
	virtual void ClearGraphicDatabase()=0;
	virtual void NewPoint(f3dPoint pt)=0;
	virtual void NewPoint(double x0, double y0, double z0,long ID=0)=0;
	virtual void NewLine(f3dLine line)=0;
	virtual void NewLine(const f3dPoint& pt1, const f3dPoint& pt2,long ID=0)=0;
	virtual void NewArcLine(f3dArcLine arc)=0;
	virtual void NewCircle(f3dCircle circle)=0;
	virtual void NewRect(f3dRect rect)=0;
	virtual void CalTextDrawSize(char* text,double height,double* size_w,
		double* size_h=NULL,double WtoH=0.7,double clearance=2)=0;
	virtual void NewText(char* text,f3dPoint pos,double height)=0;
	virtual void NewText(char* text,f3dPoint pos,double height,
		f3dPoint& align_vec,f3dPoint& norm,WORD wAlignStyle)=0;
	virtual void NewCS(UCS_STRU cs,char* symbol_x=NULL,char* symbol_y=NULL,
		char* symbol_z=NULL,char* symbol_o=NULL,long ID=0,long axis_len=50)=0;
	virtual void DelCS(long ID)=0;
	virtual void ClearAllCS()=0;
	virtual BOOL DelEnt(int ID)=0;
	/*
	virtual void ReleaseSnapStatus()=0;
	virtual BOOL SetEntSnapStatus(int ID=NULL, bool bStatus=true)=0;
	virtual BOOL SetAllSnapStatus(bool bStatus=true)=0;*/
	//2.设置类函数
	virtual bool SetViewportOrg(int x,int y)=0;
	virtual bool SetDrawArea(LPRECT lpRect)=0;
	virtual bool SetBkColor(const COLORREF &crBkColor)=0;
	virtual bool GetBkColor(COLORREF *crBkColor)=0;
	virtual PEN_STRU SetPen(COLORREF crColor, short style, int width)=0;
	virtual void SetPen(PEN_STRU pen)=0;
	virtual PEN_STRU GetPen()=0;
	virtual BOOL SetObjectUcs(const UCS_STRU &ucs)=0;
	virtual void GetObjectUcs(UCS_STRU *pCS)=0;
	virtual void SetDrawFunc(void (*fun)(CDrawEngine* pDraw))=0;	// 设置绘图函数
	virtual bool SetShxFontFile(char* file_name)=0;
	virtual bool SetBigFontFile(char* file_name)=0;
	virtual void SetOperType(OPER_TYPE oper_type)=0;
	virtual OPER_TYPE GetOperType()=0;
	virtual HCURSOR GetCursor()=0;
	/*virtual BOOL SetWorkUcs(const UCS_STRU &work_ucs)=0;
	virtual void GetWorkUcs(UCS_STRU &work_ucs)=0;
	virtual void SetDispPlaneNorm(const f3dPoint normPlane,const f3dPoint normOrg,const f3dPoint axisX=f3dPoint(0,0,0))=0;
	virtual void SetDispWorkUcs(BOOL bDisplay=TRUE)=0;
	virtual void SetDispObjectUcs(BOOL bDisplay=TRUE)=0;
	virtual void SetDisplayLineVecMark(BOOL bDisplay=TRUE)=0;	// 设置重显函数
	virtual void SetRotOrg(f3dPoint org)=0;
	virtual void GetRotOrg(f3dPoint *org)=0;
	virtual void SetDragSetBasePos(f3dPoint pos)=0;
	virtual void SetRotRadius(const double& radius)=0;
	virtual void GetRotRadius(double *radius)=0;
	virtual void SetRotRadAngle(const double& angle)=0;
	virtual double GetRotRadAngle()=0;*/
	virtual void SetZoomStyle(ZOOM_CENTER_STYLE style)=0;
	virtual ZOOM_CENTER_STYLE GetZoomStyle()=0;
	/*virtual void SetDrawType(DRAW_TYPE draw_type)=0;
	virtual DRAW_TYPE GetDrawType()=0;
	virtual void SetDelObjectFunc(int (*pFunc)(int ID,int ent_type))=0;
	virtual BOOL EnableActive(BOOL bActive=TRUE)=0;
	virtual UINT GetArcSamplingLength()=0;
	virtual void SetArcSamplingLength(UINT uLength)=0;
	virtual UINT GetSmoothness()=0;
	virtual void SetSmoothness(UINT slices)=0;
	virtual void ExitComponent()=0;
	//3.捕捉类函数
	virtual double SetOSnapScope(UINT scope)=0;
	virtual void SetSnapType(int snap_type)=0;
	virtual DWORD SetSelectPartsType(DWORD dwSelectTypeFlag)=0;
	virtual int SnapPoint(f3dPoint *&point,BOOL bSnapAllStatus=FALSE)=0;
	virtual int SnapLine(f3dLine &line,BOOL bSnapAllStatus=FALSE)=0;
	virtual int SnapCircle(f3dCircle *&circle,BOOL bSnapAllStatus=FALSE)=0;
	virtual int SnapNearPoint(f3dPoint &point,const f3dLine &line3d)=0;
	virtual int GetLastSelectEnts(long *&id_arr)=0;	//不用用户管理(释放)内存
	virtual fAtom* GetCurSnapedObject()=0;*/
	//4.操作类函数
	//scr_org为屏幕上对应的绘图原点,drawPlaneCS为通过绘图坐标系(以X和Y轴作为绘图平面，Z轴无意义)来描述的绘图平面
	virtual bool FixView(POINT scr_org,UCS_STRU drawPlaneCS,double user2scr_scale)=0;
	virtual void ReSetView()=0;
	virtual void ZoomAll(double scale)=0;						// 屏幕全显       
	virtual void ZoomWnd(CPoint topLeft,CPoint bottomRight)=0;	// 屏幕开窗
	virtual void Scale(double zoom_scale = 1.0)=0;  			// 屏幕缩放    
	/*virtual void RefreshScope()=0;
	virtual void DrawShape(CPoint TopLeft, CPoint BottomRight)=0;
	virtual BOOL IsHasHighlightPoint(f3dPoint *point)=0;*/
	// 交互操作事件响应函数
	virtual int KeyDown(UINT nChar)=0;// 按键操作
	virtual void MouseMove(CPoint point,LPARAM lParam=0)=0;// 鼠标移动函数
	virtual void LMouseDown(CPoint point)=0; // 按鼠标函数
	virtual void LMouseUp(CPoint point)=0; // 鼠标松开函数
	virtual void LMouseDoubleClick(CPoint point)=0; // 按鼠标函数
	virtual void RMouseDown(CPoint point)=0; // 按鼠标函数
	virtual void RMouseUp(CPoint point)=0; // 鼠标松开函数
	virtual double GetScaleUserToScreen()=0;
	/*virtual void PushEnv()=0; 
	virtual void PopEnv()=0; 

	virtual double UserToScreen(double user_x, double user_y, double user_z, double *port_x, double* port_y)=0;
	virtual double UserToScreen(const f3dPoint& user_pt, f2dPoint *port_pt)=0;
	virtual double UserToScreen(const f3dPoint& user_pt, f3dPoint *port_pt)=0;
	virtual double UserToScreen(const f3dLine& line_3d, f2dLine *line_2d)=0;
	virtual double UserToScreen(const f3dCircle& circle_3d, f2dCircle *circle_2d)=0;
	virtual double UserToScreen(const f3dRect& rect_3d, f2dRect *rect_2d)=0;
	virtual void ScreenToUser(double *user_x, double *user_y, double *user_z,double port_x, double port_y, double port_z = 0.5)=0;
	virtual void ScreenToUser(f3dPoint *user_pt,const f2dPoint& port_pt)=0;
	virtual void ScreenToUser(f3dPoint *user_pt,const f3dPoint& port_pt)=0;
	virtual void ScreenToUser(f3dLine *line_3d,const f2dLine& line_2d)=0;
	virtual void ScreenToUser(f3dCircle *circle_3d,const f2dCircle& circle_2d)=0;
	virtual void ScreenToUser(f3dRect *rect_3d,const f2dRect& rect_2d)=0;*/
};

#endif // !defined(AFX_DRAWENGINE_H__41754DB4_F5AA_4418_A6B7_C8AFED5A226D__INCLUDED_)
