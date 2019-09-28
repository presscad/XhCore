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
	// �����ľ��
	__declspec(property(put=set_m_hPart,get=get_m_hPart)) long m_hPart;
	int  m_idClassType;	// ����������
	GECS acs;			// ������װ������ϵ
	BOOL m_bDisplaySingle;	//���͹����Ƿ���Ҫ�ڵ���ͼ����ʾ
	BOOL m_bSelected;		//�����Ƿ�ǰ��ѡ��(������ֻ�ɳ������)
	int m_iLineType;		//0:����Ϊʵ�����ͣ�1:����Ϊֱ���͹�����2:����Ϊ�����͹���; 3:��ͷ
	f3dLine line;		// ����״̬�µļ��߼�	//��ͷ����
	f3dArcLine arc;		// ����״̬�µĻ��߼���
	double dfSphereRadius;
	CSolidBody *pBody;		// ��ʾ������ʵ����ʾ����
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
	double fUnderPlaneModelLength;	//ƽ�淨�߼�ͷʾ��ͼ,��ƽ�������ƽ���������ʵ�ʳ���(����Ļ����)
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
	static const int AlignTopLeft		= 2;//���϶���;
	static const int AlignTopCenter 	= 3;//���������;
	static const int AlignTopRight		= 4;//���ж���;
	static const int AlignMiddleLeft	= 5;//
	static const int AlignMiddleCenter	= 6;//�������;
	static const int AlignMiddleRight	= 7;//
	static const int AlignBottomLeft	= 8;//���¶���;
	static const int AlignBottomCenter	= 9;//���������;
	static const int AlignBottomRight	=10;//
	char ciAlignMode;	//��ӡʱͼ�������ֽ�ŵĶ���ģʽ
	BYTE ciPenWidthPerPixel;//ÿԭʼ��λ�߿�һ��Ϊ��Ļ��ʾ���أ�����Ĵ�ӡ���߿��߼���λ��
	char ciLayout;			//0.����Ӧ,1.�����ӡ;2.�����ӡLyingMode
	union MARGIN{
		struct{BYTE ciTop,ciBottom,ciLeft,ciRight;};
		DWORD dwMargin;	//����Ϊ��λ
	}margin;	//�߾�
	int iValidOffsetX;	//��ЧҪӡ�������Ͻ���ֽ�����ϽǼ��ƫ��,��λ:DC���߼����굥λ
	int iValidOffsetY;
	WORD wWidth,wHeight;			//��ӡ��DC��ʵ�ʴ�ӡ����Ŀ�(X��)���(Y��),��λ:DC���߼����굥λ
	WORD wActualWidth,wActualHeight;//�����û��趨�߾�ִ�е�ʵ�ʴ�ӡʱ����Ч��ӡ����Ŀ����,��λ:DC���߼����굥λ
	double scaleDevicetoModel;		//�豸�ߴ�/ģ�ͳߴ�ı���,0��ʾ������ȫ��
	POINT printStartPoint;			//ʵ�ʴ�ӡʱ��ʼԭ(��)��,����ڴ�ӡ��DC��ʵ�ʴ�ӡ����,��λ:DC���߼����굥λ
	SIZE sizePaper;					//��ǰ��ӡ��ʹ�õ�ֽ�Ŵ�С,��λ:DC���߼����굥λ
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
	//�ļ��������ͣ�����NewText()
	static const WORD TEXT_ALIGN_TOPLEFT	= 0;//0.���Ͻ�
	static const WORD TEXT_ALIGN_TOPCENTER	= 1;//1.����
	static const WORD TEXT_ALIGN_TOPRIGHT	= 2;//2.����
	static const WORD TEXT_ALIGN_MIDLEFT	= 3;//3.����
	static const WORD TEXT_ALIGN_MIDCENTER	= 4;//4.����
	static const WORD TEXT_ALIGN_MIDRIGHT	= 5;//5.����
	static const WORD TEXT_ALIGN_BTMLEFT	= 6;//6.����
	static const WORD TEXT_ALIGN_BTMCENTER	= 7;//7.����
	static const WORD TEXT_ALIGN_BTMRIGHT	= 8;//8.����
	//����ϵ��ʶ������AddCS
	static const WORD MODEL_CS = 1;		//��ǰ�����ռ��ģ������ϵ
	static const WORD WORK_CS  = 2;		//��������ϵ������ǰ��������ϵ
	static const WORD BLK_LOCATE_CS = 3;//��������λ����ϵ����װ������ϵһ���岿��װ�䷽λ��װ������ϵһ���岿��װ�䷽λ
	virtual void InitialUpdate()=0;
	virtual bool ModifyDbPoint(long idPoint,const double* point_coords)=0;
	virtual void NewPoint(double x0, double y0, double z0,long ID=0,HIBERARCHY hiber=0)=0;
	virtual void NewPoint(const f3dPoint& pt,HIBERARCHY hiber=0)=0;
	virtual void NewLine(double x0, double y0, double z0, double x1, double y1, double z1,long ID=0,HIBERARCHY hiber=0)=0;
	virtual void NewLine(const f3dPoint& pt1, const f3dPoint& pt2,long ID=0,HIBERARCHY hiber=0)=0;
	virtual void NewLine(const f3dLine& line,HIBERARCHY hiber=0)=0;
	virtual void NewSolidPart(const CSolidPart& solid_part)=0;
	virtual bool GetSolidObject(DWORD idSolidPart,OBJECT_HANDLER* pObjHandler)=0;	//����ϵͳ�����Id���Ҷ�Ӧ�Ĺ���ԭʼ��ʶ��Ϣ wjh-2019.4.6
	virtual UINT QueryBuffSolidObject(const HIBERID& hiberid)=0;//���ݵ�ǰʵ����ʾ����ĸ��Ϸּ�Id������ʾ�����ж�Ӧ��ʾ�����32λId wjh-2019.4.9
	virtual void NewArcLine(const f3dArcLine& arc,HIBERARCHY hiber=0)=0;
	virtual void NewCircle(const f3dCircle& circle,HIBERARCHY hiber=0)=0;
	virtual void NewRect(f3dRect &rect,HIBERARCHY hiber=0)=0;
	virtual void NewWorkPlaneSketch(UINT idWorkPlane,SKETCH_PLANE_DISPLAY* pSketchPlane,const char* prompt)=0;
	virtual void NewWorkPlaneSketch(UINT idWorkPlane,COLORREF crMaterial,GEPOINT* pVertexArr,UINT count, const double* normal,const char* prompt,UINT uArrowScrLen=150,bool bAlongNorm=true,double fUnderPlaneModelLen=0)=0;
	virtual BOOL DelWorkPlaneSketch(UINT idWorkPlane)=0;
	virtual void NewComplexFace(f3dPolyFace *pLoopFace)=0;
	//���������ı����ƺ�����δʵ��
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
	virtual void Draw()=0;	//����Ļ���ڻ���ͼ��������
	virtual void Print(CDC* pDC,PRINTPAGE& pageinfo)=0;	//����Ļ���ڻ���ͼ��������

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
	virtual bool Init(HWND hWnd)=0;   // ��ʼ��OpenGL
	virtual bool Init(HWND hWnd,HWND hParentWnd)=0;   // ��ʼ��OpenGL
	virtual void Activate()=0;		//������潻��ʹ��ʱ��ֻ��ͬʱ����һ��OpenGL���� wjh-2015.11.18
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
	virtual void SetDisplayLinePoints(BOOL bDisplay=TRUE)=0;	// �������Ժ���
	virtual void SetDisplayLineVecMark(BOOL bDisplay=TRUE)=0;	// �������Ժ���
	virtual void SetBkSceneMode(int mode)=0;	//0.��ɫ����;1.���䱳��
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
	virtual void SetDisplayFunc(void (*fun)(void* pContext))=0;		// ����Ԥ�����Ժ���
	virtual void SetLiveDisplayFunc(void (*fun)(LIVE_DISPLAY_CONTEXT* pContext))=0;	// ����ʵʱ���ƺ���
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
	// ��ͼ����
	STDMETHOD_(void,DrawCoordinate)(void)=0;// ������ϵ*/
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
		//����SnapObject�Ŀͻ��������Ļ���
		void* pExterContext;
		//������Ϣ
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
	virtual int GetLastSelectEnts(long *&id_arr,HIBERARCHY** ppHiberarchy=NULL)=0;	//�����û�����(�ͷ�)�ڴ�
	virtual int InverseSelectEntsStatus()=0;	//��ת�����ѡ��״̬
	virtual fAtom* GetCurSnapedObject()=0;
};
struct ISolidOper
{
	virtual void ReSize()=0;
	//scr_orgΪ��Ļ�϶�Ӧ�Ļ�ͼԭ��,drawPlaneCSΪͨ����ͼ����ϵ(��X��Y����Ϊ��ͼƽ�棬Z��������)�������Ļ�ͼƽ��
	virtual bool SpecView(POINT scr_org,UCS_STRU drawPlaneCS,double user2scr_scale)=0;
	virtual void ReSetView()=0;
	virtual void ZoomAll(double scale)=0;          // ��Ļȫ��       
	// ��Ļ����
	virtual void ZoomWnd(CPoint topLeft,CPoint bottomRight)=0;
	//scopeΪ�۽���Χ,maxScaleScr2UserΪ���Ŵ���
	virtual void FocusTo(SCOPE_STRU&scope ,double maxScaleScr2User,long idHighLightObj=0)=0;
	virtual void Scale(double zoom_scale = 1.0)=0;  		 // ��Ļ����    
	virtual void RefreshScope()=0;
	virtual void DrawShape(CPoint TopLeft, CPoint BottomRight)=0;
	virtual BOOL IsHasHighlightPoint(f3dPoint *point)=0;
	// ͼ�β�������
                            			
	virtual int KeyDown(UINT nChar)=0;// ��������
	virtual void MouseMove(CPoint point,LPARAM lParam=0)=0;// ����ƶ�����
	virtual void LMouseDown(CPoint point)=0; // ����꺯��
	virtual void LMouseUp(CPoint point)=0; // ����ɿ�����
	virtual void LMouseDoubleClick(CPoint point)=0; // ����꺯��
	virtual void RMouseDown(CPoint point)=0; // ����꺯��
	virtual void RMouseUp(CPoint point)=0; // ����ɿ�����
	virtual void PushEnv()=0; 
	virtual bool PopEnv()=0; 
	//env_blobΪԤ���洢��ʾ�������ڴ��ָ��(����ΪNULL)������ֵΪ�����ڴ��С
	virtual UINT GetCurrEnvionment(char* env_blob)=0;
	virtual bool SetCurrEnvionment(char* env_blob)=0;	//env_blobΪ�洢��ǰҪָ����ʾ�������ڴ��ָ�루�̶���ʽ����Ҫ��С��

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
	//�û�ģ���������ͼ�����ڲ�������ϵת������
	virtual GEPOINT TransPFromGL(const double* point)=0;
	virtual GEPOINT TransVFromGL(const double* vector)=0;
	virtual GEPOINT TransPToGL(const double* point)=0;
	virtual GEPOINT TransVToGL(const double* vector)=0;
	//�û�ģ����������Ļ����ϵת������
	virtual GEPOINT TransPFromScr(const double* point)=0;
	virtual GEPOINT TransVFromScr(const double* vector)=0;
	virtual GEPOINT TransPToScr(const double* point)=0;
	virtual GEPOINT TransVToScr(const double* vector)=0;
};
//ʵ���������ӿ�
struct IDrawSolid
{
public:	//�ӿ�
	virtual long GetSerial()=0;
	virtual ISolidDraw* SolidDraw()=0;	//��ȡ�����ͼ�ӽӿ�
	virtual ISolidSet * SolidSet()=0;	//��ȡ���������ӽӿ�
	virtual ISolidSnap* SolidSnap()=0;	//��ȡ���沶׽�ӽӿ�
	virtual ISolidOper* SolidOper()=0; //��ȡ��������ӽӿ�
	virtual I2dDrawing* Drawing2d()=0; //��ȡ�����άͼ�ӽӿ�
};
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
struct CDrawSolidFactory{
#else
struct AFX_EXT_CLASS CDrawSolidFactory{
#endif
public:	//�����������ɺ���
	static IDrawSolid* CreateDrawEngine();
	static IDrawSolid* DrawEngineFromSerial(long serial);
	static BOOL Destroy(long serial);
	static DWORD DrawSolidDllGetVersion();
	static bool ValidateVersionCompatible(char* errmsg=NULL,int nMsgStrBuffLen=50);
};
