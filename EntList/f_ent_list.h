#ifndef __F_ENTTITY_LIST_H_
#define __F_ENTTITY_LIST_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the F_ENT_LIST_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// F_ENT_LIST_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define F_ENT_LIST_API
#else
#ifdef F_ENT_LIST_EXPORTS
#define F_ENT_LIST_API __declspec(dllexport)
#else
#define F_ENT_LIST_API __declspec(dllimport)
#endif
#endif

#include "math.h"
#include "f_ent.h"
#include "stack.h"

//链表模板类
template <class TYPE> class ATOM_LIST
{
public:
    ATOM_LIST()
	{// 构造函数
		NodeNum=0;
		index = 0;
		cursor = head = tail = NULL;
	}
    ~ATOM_LIST(){Empty();}

//1.私有成员变量定义
protected:			// 注:“索引”即“下标”
	typedef struct tagDATA_TYPE
	{
		TYPE atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		BOOL bDeleted;
		tagDATA_TYPE(){bDeleted=FALSE;}
	}DATA_TYPE;
	CStack<DATA_TYPE*> POS_STACK;
    long index;			// 首节点索引号(下标)为1
    long NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针

//2.公有成员变量定义
public:
//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
	int push_stack()
	{
		if(cursor)
		{
			return POS_STACK.push(cursor);
			//return true;
		}
		else
			return 0;//false
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
		if(POS_STACK.pop(cursor))
			return true;
		else
		{
			cursor = NULL;
			return false;
		}
	}
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
    TYPE* append()//在节点链表的末尾添加节点
	{
		if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->bDeleted = FALSE;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			index = NodeNum-1;
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;
		//GetTail();//cursor is equal to tail now.   --00.05.25
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		index = NodeNum-1;
		
		return &ptr->atom;
	}
    TYPE* append(const TYPE &atom)//在节点链表的末尾添加节点
	{
		TYPE *pAtom = append();
		*pAtom = atom;
		return pAtom;
	}
	//ii==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    TYPE* insert(const TYPE &atom, long ii=-1)
	{
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;
		ptr->atom = atom;

		if(NodeNum<=0)//空链表
		{
    		cursor = head = tail = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum++;
			index = NodeNum-1;//将新添节点置为当前节点
			return &ptr->atom;
		}

		if(ii == -1)//以当前节点位置为新节点插入位置
		{
			//index = GetCurrentIndex();
			//cursor = &GetByIndex( index );
        		//建立新添节点与链表内原点间的连接关系
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//原当前节点处理
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else	//cursor原为首节点
				head = ptr;
			cursor->prev = ptr;

			index = ii;//将新添节点置为当前节点
		}
		else if(ii>=0&&ii<NodeNum)//指定插入点位置
		{
			int i=0;
			for(cursor=head;cursor;cursor=cursor->next,i++)
			{
				if(cursor->bDeleted)
				{
					i--;
					continue;
				}
				else if(i==ii)
					break;
			}
        		//建立新添节点与链表内原点间的连接关系
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//插入位置处原节点处理
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else if(cursor->prev==NULL)//指定插入点原为链表首节点
				head = ptr;
			cursor->prev = ptr;

			index = ii;//将新添节点置为当前节点
		}
		else if(ii<0)
		{
  			//GetFirst();//cursor is equal to tail now.   --00.05.25
			ptr->prev = NULL;
			ptr->next = head;
			head->prev = ptr;
			cursor = head =ptr;

			index = 0;//将新添节点置为当前节点
		}
		else //(ii>=NodeNum)
		{
  			//GetTail();//cursor is equal to tail now.   --00.05.25
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;

			index = NodeNum;//将新添节点置为当前节点
		}
		NodeNum++;
		return &ptr->atom;
	}
	long GetCurrentIndex()const//获取当前节点的索引号
	{
		return index;
	}

    TYPE* GetByIndex(long ii)
	{
		if(ii<0)
    		return NULL;
		long i=0;
		TYPE *pItem;
		for(pItem=GetFirst();pItem;pItem=GetNext(),i++)
		{
			if(i==ii)
				return pItem;
		}
		return NULL;
	}
    TYPE& operator[](long ii)//根据索引号获取线段实体
	{
		return *GetByIndex(ii);
	}
    TYPE* GetNext(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->next;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
		{
			index++;
			return &cursor->atom;
		}
		else
			return NULL;
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
	    		cursor = cursor->prev;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
		{
    		index--;
			return &cursor->atom;
		}
		else
			return NULL;
	}
    TYPE* GetFirst(BOOL bIterDelete=FALSE)
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			index = 0;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return NULL;
		}
	}
    TYPE* GetTail(BOOL bIterDelete=FALSE)
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			index = NodeNum-1;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
	BOOL IsCursorDeleted(){ return cursor->bDeleted;}
	void RestoreCursor(){ cursor->bDeleted=FALSE;}

	long GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteCursor(BOOL bClean=FALSE)
	{
		if(cursor==NULL||cursor->bDeleted)
			return FALSE;
		else
		{
			if(bClean)
			{
				DATA_TYPE *cursor2=NULL;
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
					cursor2=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
						cursor2=head;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
						cursor2=tail;
					}
				}
				delete cursor;
				cursor=cursor2;
			}
			else
				cursor->bDeleted=TRUE;
			NodeNum--;	//只是在对未删除节点进行删除时节点总数减一，对已重复删除节点总数不变
			if(NodeNum<0)
				NodeNum=0;
			return TRUE;
		}
	}
	void Clean()
	{
		for(cursor=head;cursor;)
		{
			DATA_TYPE *cursor2=cursor->next;
			if(cursor->bDeleted)
			{	//已被删除元素,移位后清除
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
					}
				}
				delete cursor;
			}
			cursor=cursor2;
		}
	}
    BOOL DeleteAt(long ii)	// 删除一个节点
	{
		int i=0;
		for(TYPE* pType=GetFirst();pType;pType=GetNext(),i++)
		{
			if(i==ii)
				return DeleteCursor();
		}
		return FALSE;
	}
    void Destroy()		// 销毁整个当前类实例
	{
		~ATOM_LIST();
	}
    void Empty()		// 清空当前类实例的所有元素
	{
		POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			delete tail;
			tail = cursor;
		}
		NodeNum=0;
		index = 0;
		head = tail = NULL;
		cursor = NULL;
	}
	void EmptyStack(){POS_STACK.Empty();}
};

//------------------------------ 点链表: fPtList类声明 -----------------------------------
class F_ENT_LIST_API fPtList
{
public:
    fPtList()
	{// 构造函数
		NodeNum=0;
		index = 0;
		cursor = head = tail = NULL;
		m_pAttachStack=NULL;
		m_lpTmpPosition=NULL;
	}
    ~fPtList();

//1.私有成员变量定义
private:			// 注:“索引”即“下标”
	typedef struct tagDATA_TYPE
	{
		f3dPoint atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
	}DATA_TYPE;
	//CStack<DATA_TYPE*> POS_STACK;
    long index;			// 首节点索引号(下标)为1
    long NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
	ISimplePtrStack* m_pAttachStack;
	DATA_TYPE* m_lpTmpPosition;//记录一个临时游标位置，以便后期恢复游标位置

//2.公有成员变量定义
public:
//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
	//堆栈操作函数
	ISimplePtrStack* AttachPosStack(ISimplePtrStack *pStack){return m_pAttachStack=pStack;}
	void DetachPosStack(){m_pAttachStack=NULL;}
	int PushStack();
	bool PopStack(int pos=-1);
	int GetStackRemnantSize();
	
    f3dPoint* append();//在节点链表的末尾添加节点
    f3dPoint* append(const f3dPoint &pt,BOOL bJudgeNearest=FALSE);//在节点链表的末尾添加节点
	f3dPoint* append(double x,double y,double z,BOOL bJudgeNearest=FALSE);//在节点链表的末尾添加节点
	//ii==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    f3dPoint* insert(const f3dPoint &pt, long ii=-1);
	long GetCurrentIndex()const{return index;}//获取当前节点的索引号
    f3dPoint* GetByIndex(long ii)const;
    f3dPoint& operator[](long ii)const{return *GetByIndex(ii);}//根据索引号获取线段实体
    f3dPoint* GetNext();
    f3dPoint* GetPrev();
    f3dPoint* GetFirst();
    f3dPoint* GetTail();
    f3dPoint* GetCursor();
	long GetNodeNum()const{return NodeNum;}
    BOOL DeleteCursor();	// 删除一个节点
    BOOL DeleteAt(long ii);	// 删除一个节点
    void Destroy();			// 销毁整个当前类实例
    void Empty();			// 清空当前类实例的所有元素
};
inline fPtList::~fPtList()
{
	Empty();
}
//#-------------------------------------------------------------------------------
class F_ENT_LIST_API fLineList{
	long base_id;
public:
    fLineList();
    ~fLineList();

//1.私有成员变量定义
private:			// 注:“索引”即“下标”
    long index;			// 首节点索引号(下标)为1
    long NodeNum;		// 总个数
    f3dAtomLine* cursor;    // 当前线段指针
    f3dAtomLine* tail;		// 线段链表末尾指针
    f3dAtomLine* head;		// 线段链表起始指针

//2.公有成员变量定义
public:
//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
    void Destroy();		// 销毁整个当前类实例
    void Empty();		// 清空当前类实例的所有元素

    f3dAtomLine* append(const f3dAtomLine&);//在节点链表的末尾添加节点
    f3dAtomLine* append(f3dPoint* start,f3dPoint* end);//在节点链表的末尾添加节点

	//  void appendList(f3dLine& headLine);//在节点链表的末尾添加节点链表
    
	static const long INSERT_BEFORE = -1;
	static const long INSERT_AFTER  = -2;

	//index==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    f3dAtomLine* insert(const f3dAtomLine&, long liAtIndex=INSERT_BEFORE);
    
	long GetCurrentIndex()const;//获取当前节点的索引号

    f3dAtomLine* GetByIndex(long ii)const;
    f3dAtomLine& operator[](long)const;//根据索引号获取线段实体
    f3dAtomLine* GetNext();
    f3dAtomLine* GetPrev();
    f3dAtomLine* GetFirst();
    f3dAtomLine* GetTail();
    
	long GetNodeNum()const{return NodeNum;}
    BOOL DeleteAt(long index);	// 删除一个节点
	BOOL DeleteById(long id);	// 根据id删除直线节点
};

inline fLineList::fLineList(){	// 构造函数
	base_id=0;
    NodeNum=0;
    index = 0;
    cursor = head = tail = NULL;
}
//#-------------------------------------------------------------------------------
class fCurveList;
class fLoopList;
class F_ENT_LIST_API fLoop{
	fLoop* prev;
	fLoop* next;
	friend class fLoopList;
public:
	fLineList *loop;
	fLoop(){prev = next = NULL;loop=NULL;}
	~fLoop(){if(loop) delete loop;}
	operator fLineList* (){return loop;}
    void Empty();		// 清空当前类实例的所有元素
	//在节点链表的末尾添加节点
    f3dAtomLine* append(const f3dAtomLine& atom_line){return loop->append(atom_line);}
    f3dAtomLine* append(f3dPoint* pStart,f3dPoint* pEnd){return loop->append(pStart,pEnd);}

	//index==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    void insert(const f3dAtomLine& atom_line, long index=-1){loop->insert(atom_line,index);}
    
	long GetCurrentIndex()const{return loop->GetCurrentIndex();}//获取当前节点的索引号

    f3dAtomLine* GetByIndex(long ii)const{return loop->GetByIndex(ii);}
    f3dAtomLine& operator[](long ii)const{return *loop->GetByIndex(ii);}//根据索引号获取线段实体
    f3dAtomLine* GetNext(){return loop->GetNext();}
    f3dAtomLine* GetPrev(){return loop->GetPrev();}
    f3dAtomLine* GetFirst(){return loop->GetFirst();}
    f3dAtomLine* GetTail(){return loop->GetTail();}
    
	long GetNodeNum()const{return loop->GetNodeNum();}
    BOOL DeleteAt(long index){return loop->DeleteAt(index);}	// 删除一个节点
};
//边链表（包括曲边）
class F_ENT_LIST_API fCurveList{
public:
    fCurveList();
    ~fCurveList();

//1.私有成员变量定义
private:			// 注:“索引”即“下标”
	typedef struct tagDATA_TYPE
	{
		f3dCurve curve;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
	}DATA_TYPE;
    long index;			// 首节点索引号(下标)为1
    long NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针

//2.公有成员变量定义
public:
	operator fLoop();
//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
    void Destroy();		// 销毁整个当前类实例
    void Empty();		// 清空当前类实例的所有元素

    f3dCurve* append(const f3dCurve&);//在节点链表的末尾添加节点
    f3dCurve* append(f3dPoint* start,f3dPoint* end);//在节点链表的末尾添加节点

	//  void appendList(f3dLine& headLine);//在节点链表的末尾添加节点链表
    
	//index==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    f3dCurve* insert(const f3dCurve&, long index=-1);
    
	long GetCurrentIndex()const;//获取当前节点的索引号

    f3dCurve* GetByIndex(long ii)const;
    f3dCurve& operator[](long)const;//根据索引号获取线段实体
    f3dCurve* GetNext();
    f3dCurve* GetPrev();
    f3dCurve* GetFirst();
    f3dCurve* GetTail();
    
	long GetNodeNum()const{return NodeNum;}
    int DeleteNode(long index);	// 删除一个节点
};

//------------------------------ 三角面链表: fTriFaceList类声明 ----------------------------------
class fTriFaceList;

class F_ENT_LIST_API  fTriFaceList
{
public:
    fTriFaceList();
    ~fTriFaceList();

	struct DATA_TYPE
	{
		f3dTriFace face;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		DATA_TYPE(){prev=next=NULL;}
	};
//1.私有成员变量定义
private:
    long index;		// 首节点索引号为1
    long NodeNum;	// 总数
    DATA_TYPE* cursor;		//三角面链表当前指针
    DATA_TYPE* tail;		//三角面链表末尾指针
    DATA_TYPE* head;       //三角面链表起始指针

//2.公有成员变量定义
public:
//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
    void Destroy();
    void Empty();
    f3dTriFace* append();//在节点链表的末尾添加节点
    f3dTriFace* append(const f3dTriFace&);//在节点链表的末尾添加节点
    f3dTriFace* append(const f3dPoint& p1,
                const f3dPoint& p2,
                const f3dPoint& p3);//在节点链表的末尾添加节点
	//  void appendList(fTriFace& headFace);//在节点链表的末尾添加节点链表

    //index==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    f3dTriFace* insert(long index=-1);
    f3dTriFace* insert(const f3dTriFace&,long index=-1);
    f3dTriFace* insert(const f3dPoint& p0,
                const f3dPoint& p1,
                const f3dPoint& p2,long index=-1);

    long GetCurrentIndex()const;//获取当前节点的索引号
    f3dTriFace* GetByIndex(long index);//根据索引号获取三角面实体
    f3dTriFace& operator [](long index);
    f3dTriFace* GetNext();
    f3dTriFace* GetPrev();
    f3dTriFace* GetFirst();
    f3dTriFace* GetTail();
    long GetNodeNum()const{return NodeNum;}
    int DeleteNode(long index);
};
inline fTriFaceList::fTriFaceList(){	// 构造函数
    NodeNum=0;
    index = 0;
    cursor = head = tail = NULL;
}

//------------------------------ 多边形面: fPolyFace类声明 ---------------------------------

/////////////////////////////////////////////////////////////////////////////
// fLoopList
class F_ENT_LIST_API fLoopList
{
public:
    fLoopList();
    ~fLoopList();

	struct DATA_TYPE
	{
		fLoop loop;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		DATA_TYPE(){prev=next=NULL;}
	};
//1.私有成员变量定义
private:			// 注:“索引”即“下标”
    long index;			// 首节点索引号(下标)为1
    long NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针

//2.公有成员变量定义
public:

//3.私有成员函数定义
private:

//4.公有成员函数定义
public:
    void Destroy();		// 销毁整个当前类实例
    void Empty();		// 清空当前类实例的所有元素

    fLoop* append();//在节点链表的末尾添加节点

	//index==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    fLoop* insert(fLoop loop, long index=-1);
    
	long GetCurrentIndex()const;//获取当前节点的索引号
    fLoop* GetByIndex(long index)const;
    fLoop& operator[](long)const;//根据索引号获取线段实体
    fLoop* GetNext();
    fLoop* GetPrev();
    fLoop* GetFirst();
    fLoop* GetTail();
    
	long GetNodeNum()const{return NodeNum;}
    int DeleteNode(long index);	// 删除一个节点
};
inline fLoopList::fLoopList(){	// 构造函数
    NodeNum=0;
    index = 0;
    cursor = head = tail = NULL;
};

//class F_ENT_LIST_API fAtomList<fLineList*>;
class F_ENT_LIST_API f3dPolyFace{
	friend class fPolyFaceList;
	long id;	//用于标识多边形面链中的某一特定面
public:
	f3dPoint poly_norm;			//多边形面的正法线方向
    fLineList outer_edge;		// 外环边线链表
	fLoopList inner_loop;		// 多边形的内轮廓(内环)的链表
	COLORREF material;		// 可用于记录此面的特征信息(如材质等)
	//用于快速显示此面时，事先分好三角面，显示时就不用再分解三角面了
	fTriFaceList* m_pTriList;

public:
	BOOL IsValidFace();
	long GetId(){return id;}
	/*	vertex_list即是输入又是输出参数表示的是实体面各顶点所指向的顶点列表
		axis_start,axis_end,radius分别表示圆柱面的起始和终止面圆心点及圆柱面半径
		cut_line_list用于输出新生成的切割边列表
	*/
	//void CutByCylinder(fPtList *vertex_list,f3dPoint axis_start,f3dPoint axis_end,double radius,fLineList *cut_line_list=NULL);
	/*	vertex_list即是输入又是输出参数表示的是实体面各顶点所指向的顶点列表
		pick,norm分别表示切割面的基准点及切割面法线
		cut_line_list用于输出新生成的切割边列表
	*/
	void CutByPlane(fPtList *vertex_list,f3dPoint pick,f3dPoint norm,fLineList *cut_line_list=NULL);
    f3dPolyFace(){id=0;m_pTriList=NULL;material = 0;}	// 初始化型构造函数.
    f3dPolyFace(const f3dPolyFace& face);		// 拷贝型构造函数.

    BOOL GetPolyUCS(UCS_STRU& ucs)const;	// 得到多边形面的相对坐标系
							// 该坐标系是 WJH 根据情况自动生成的

    ~f3dPolyFace();
	/*根据奇偶算法判断一点是否在当前空间多边形域内
		返回值：0表示点不在多边形域面上
				1表示点在多边形域面内（包括边界在内）
				2表示点在多边形域面上但不在多边形域面内
	*/
	int PtInFace(f3dPoint pt);
    /*  判断一多边形面是否为凸多边形面。
         1表示凸;
         0表示凹;
        -1表示多边形定义有误(所有边界线重合)
        -2表示多边形定义有误(边界线间互相不共面)
        -3表示多边形定义有误(多边形不封闭) 
		-4表示多边形定义有误(多边形边界相交叉)*/
    int is_fPolyFace_protrusive()const;
	// 将一多边形面分解为一个三角面链表(由一系列不相交的三角面组成的)
	//		在函数内部自动分配内存，所以使用完毕后需要调用
	//		Destory()函数销毁该返回的指针。
	void ExplodeToTriFace();
};

//------------------------------ 多边形面链表: fPolyFaceList类声明 ---------------------------------
class F_ENT_LIST_API fPolyFaceList{
	int base_id;
public:
    fPolyFaceList();
    ~fPolyFaceList();

	struct DATA_TYPE
	{
		f3dPolyFace face;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		DATA_TYPE(){prev=next=NULL;}
	};
//1.私有成员变量定义
private:
    long index;				//首节点索引号为1
    long NodeNum;
    DATA_TYPE* cursor;    //多边形面链表当前指针
    DATA_TYPE* tail;		//多边形面链表末尾指针
    DATA_TYPE* head;		//多边形面链表起始指针

	ISimplePtrStack* m_pAttachStack;
	DATA_TYPE* m_lpTmpPosition;//记录一个临时游标位置，以便后期恢复游标位置
//2.公有成员变量定义
public:
//3.私有成员函数定义
private:
//4.1堆栈操作函数
public:
	ISimplePtrStack* AttachPosStack(ISimplePtrStack *pStack){return m_pAttachStack=pStack;}
	void DetachPosStack(){m_pAttachStack=NULL;}
	int PushStack();
	bool PopStack(int pos=-1);
	int GetStackRemnantSize();
	
//4.2公有成员函数定义
public:
    void Destroy();
    void Empty();
    f3dPolyFace* append();	//在节点链表的末尾添加节点

	//  void appendList(fPolyFace& headFace);//在节点链表的末尾添加节点链表
    
	//index==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加
    f3dPolyFace* insert(long index=-1);
    
	long GetCurrentIndex()const;//获取当前节点的索引号
    f3dPolyFace* GetByIndex(long index);
    f3dPolyFace& operator [](long index);
    f3dPolyFace* GetNext();
    f3dPolyFace* GetPrev();
    f3dPolyFace* GetFirst();
    f3dPolyFace* GetTail();
    long GetNodeNum()const{return NodeNum;}
    int DeleteAt(long index);	//根据索引值删除面
	int DeleteById(long id);	//根据多边形面标识删除面
};
inline fPolyFaceList::fPolyFaceList(){	// 构造函数
    NodeNum=0;
    base_id = index = 0;
    cursor = head = tail = NULL;
	m_pAttachStack=NULL;
	m_lpTmpPosition=NULL;
}

class F_ENT_LIST_API fPolygonRgn : public fAtom{
	UCS_STRU ucs;
public:
	fPtList vertex;
	short feature;
public:
	fLineList edgeLine;
	fPolygonRgn(){feature = 0;atom_type = AtomType::prPolyFace;}
	BOOL GetPolyUcs(UCS_STRU &cs)const;
	BOOL GetNorm(f3dPoint& norm)const;	//获得多边形的法线方向
	BOOL CreatePolygonRgn(f3dPoint* lp3dPoints, int nCount);
	BOOL is_fPolygonRgn_a_face()const;
	/*根据进出算法判断一点是否在当前空间多边形域内
		返回值：0表示点不在多边形域面上
				1表示点在多边形域面内（包括边界在内）
				2表示点在多边形域面上但不在多边形域面内
	*/
	int PtInRgn(f3dPoint pt)const;
	/*根据进出算法判断一点是否在当前空间多边形域内
		返回值：0表示线不在多边形域面上
				1表示线在多边形域面内（包括边界在内）
				2表示线在多边形域面上但不全在多边形域面内
				3表示线在多边形域面上但全不在多边形域面内
	*/
	int LineInRgn(f3dLine line)const;
	void Empty();
};
//POLYGON用于代替fPolygonRgn快速检测点是否在多边形区域内的算法 wjh-2015.4.12
class F_ENT_LIST_API POLYGON{
protected:
	bool m_bRectangleXY;	//用于处理简单平面矩形的计算
	int count;
	GEPOINT zAxis, *vertexes;
public:
	GEPOINT GetAxisZ(){return zAxis;}
	BOOL CreatePolygonRgn(f3dPoint* lp3dPoints, int nCount);
	int GetVertexCount(){return count;}
	GEPOINT GetVertexAt(int i);
	/*根据进出算法判断一点是否在当前空间多边形域内
		返回值：0表示点不在多边形域面上
				1表示点在多边形域面内（包括边界在内）
				2表示点在多边形域面上但不在多边形域面内
	*/
	int PtInRgn(const double* poscoord)const;
	/*根据进出算法判断一点是否在当前空间多边形域内
		返回值：0表示线不在多边形域面上
				1表示线在多边形域面内（包括边界在内）
				2表示线在多边形域面上但不全在多边形域面内
				3表示线在多边形域面上但全不在多边形域面内
	*/
	int LineInRgn(const double* posStart,const double* posEnd)const;
	/*根据进出算法判断一点是否在当前空间多边形域内
		返回值：0表示点不在多边形域面上
				1表示点在多边形域面边界内
				2表示点在多边形域面边界上
				3表示点在多边形域面边界外
	*/
	int PtInRgn2(const double* poscoord);
	/*根据进出算法判断一点是否在当前空间多边形域内
		返回值：0表示线不在多边形域面上
				1表示线在多边形域面内（包括边界在内）
				2表示线在多边形域面上但不全在多边形域面内
				3表示线在多边形域面上但全不在多边形域面内
	*/
	int LineInRgn2(const double* posStart,const double* posEnd);
	double CalArea(double* center=NULL);	//center 为返回的几何形心点坐标
	POLYGON(){count=0;vertexes=NULL;m_bRectangleXY=false;}
	~POLYGON();
};

//------------------------ 实体类: fBody定义 -------------------------------------
class F_ENT_LIST_API fBody{
	int start_vertex_i;
	void CutFaceByBandPlaneList(f3dPolyFace *pFace,fPtList &band_plane_vertex_list,
			f3dPoint extend_vec,double extend_dist,fLineList *cut_line_list,COLORREF material);
public:
	fBody(){start_vertex_i=0;m_bModified=TRUE;}
	//个别情况允许外围程序通过blAddSectFace控制是否添加剖切面 wjh-2019.4.17
	int CutByPlane(f3dPoint pick, f3dPoint norm,COLORREF material=NULL,bool blAddSectFace=true);//返回添加的剖面数
	//要求extend_vec符合多边形封闭切割面的右手螺旋法则，同时要求切割多边形环和被切割面都为凸
	void CutByBandPlaneList(fPtList &band_plane_vertex_list,f3dPoint extend_vec,double extend_dist,COLORREF material=NULL);
	BOOL IsIntWith(fBody *pBody);//是否与其它实体碰撞
		// 当“顶点”的坐标改变时, 
		//	 实体的各个“面”由于最终指向的是该“顶点”，
		//	 因此“面”能够得到自动的更新。
	void Empty();
	f3dAtomLine* NewEdgeLine(f3dPolyFace *pFace, int e_vertex_i, int s_vertex_i=-1,BOOL bVisible=TRUE);
public:
	BOOL m_bModified;
    fPolyFaceList faceList;		// 实体由“面”链表组成.
    fPtList vertex;				// 实体的“顶点”链表
	fPtList listKeyVertex;
	fCurveList listKeyCurve;
};
/*class F_ENT_LIST_API CFaceLoop{
	BYTE* memoryAddr;
public:
	CFaceLoop(BYTE* address){memoryAddr=address;}
	int LoopEdgeLineNum();
	bool GetLoopEdgeLineAt(int i,f3dArcLine& line);
};
class F_ENT_LIST_API CSolidFace{
	BYTE* memoryAddr;
public:
	CSolidFace(BYTE* address){memoryAddr=address;}
	GEPOINT FaceNorm();			//多边形面的正法线方向
	COLORREF MatColor();		// 可用于记录此面的特征信息(如材质等)
	long FaceId();	//用于标识多边形面链中的某一特定面
	int InnerLoopNum();
	CFaceLoop GetInnerLoopAt(int i);
	CFaceLoop GetOutterLoop();
};
class F_ENT_LIST_API CSolidBody{
	DWORD m_dwBufSize;
	BYTE* data;
public:
	CSolidBody(BYTE* buf=NULL,DWORD size=0);
	~CSolidBody();
	DWORD SolidBufferLength(){return m_dwBufSize;}
	bool ReadFrom(BUFFER_IO* io,DWORD size);
	void WriteTo(BUFFER_IO* io);
	int KeyPointNum();
	GEPOINT GetKeyPointAt(int i);
	int KeyEdgeLineNum();
	bool GetKeyEdgeLineAt(int i,f3dArcLine& line);
	int PolyFaceNum();
	bool GetPolyFaceAt(int i,CSolidFace& face);
	int GLFaceNum();
	bool GetGLFaceAt(int i,CGLFace& face);
};
/*struct DB_REC_INDEX{
	DWORD iPageIndex;
	DWORD offset;
};
struct DB_PAGE_BUF{
	BYTE data[1024];
};
class CSolidDababase{
	CHashList<DB_REC_INDEX> hashRecById;
	DB_PAGE_BUF page_arr[1024];
public:
	bool LoadSolidBody(long id,CSolidBody& body);
	bool SaveSolidBody(long id,CSolidBody& body);
	bool DeleteSolidBody(long id);
	bool ReadFromDbFile(char* file);
	bool WriteDbFile(char* file);
};*/
typedef ATOM_LIST<fBody*> fBodyPtrList;
#endif
