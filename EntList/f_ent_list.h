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

//����ģ����
template <class TYPE> class ATOM_LIST
{
public:
    ATOM_LIST()
	{// ���캯��
		NodeNum=0;
		index = 0;
		cursor = head = tail = NULL;
	}
    ~ATOM_LIST(){Empty();}

//1.˽�г�Ա��������
protected:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		TYPE atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		BOOL bDeleted;
		tagDATA_TYPE(){bDeleted=FALSE;}
	}DATA_TYPE;
	CStack<DATA_TYPE*> POS_STACK;
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
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
    TYPE* append()//�ڽڵ������ĩβ��ӽڵ�
	{
		if(NodeNum<=0)//������
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
    TYPE* append(const TYPE &atom)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pAtom = append();
		*pAtom = atom;
		return pAtom;
	}
	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    TYPE* insert(const TYPE &atom, long ii=-1)
	{
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;
		ptr->atom = atom;

		if(NodeNum<=0)//������
		{
    		cursor = head = tail = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum++;
			index = NodeNum-1;//������ڵ���Ϊ��ǰ�ڵ�
			return &ptr->atom;
		}

		if(ii == -1)//�Ե�ǰ�ڵ�λ��Ϊ�½ڵ����λ��
		{
			//index = GetCurrentIndex();
			//cursor = &GetByIndex( index );
        		//��������ڵ���������ԭ�������ӹ�ϵ
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//ԭ��ǰ�ڵ㴦��
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else	//cursorԭΪ�׽ڵ�
				head = ptr;
			cursor->prev = ptr;

			index = ii;//������ڵ���Ϊ��ǰ�ڵ�
		}
		else if(ii>=0&&ii<NodeNum)//ָ�������λ��
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
        		//��������ڵ���������ԭ�������ӹ�ϵ
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//����λ�ô�ԭ�ڵ㴦��
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else if(cursor->prev==NULL)//ָ�������ԭΪ�����׽ڵ�
				head = ptr;
			cursor->prev = ptr;

			index = ii;//������ڵ���Ϊ��ǰ�ڵ�
		}
		else if(ii<0)
		{
  			//GetFirst();//cursor is equal to tail now.   --00.05.25
			ptr->prev = NULL;
			ptr->next = head;
			head->prev = ptr;
			cursor = head =ptr;

			index = 0;//������ڵ���Ϊ��ǰ�ڵ�
		}
		else //(ii>=NodeNum)
		{
  			//GetTail();//cursor is equal to tail now.   --00.05.25
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;

			index = NodeNum;//������ڵ���Ϊ��ǰ�ڵ�
		}
		NodeNum++;
		return &ptr->atom;
	}
	long GetCurrentIndex()const//��ȡ��ǰ�ڵ��������
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
    TYPE& operator[](long ii)//���������Ż�ȡ�߶�ʵ��
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
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
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    BOOL DeleteAt(long ii)	// ɾ��һ���ڵ�
	{
		int i=0;
		for(TYPE* pType=GetFirst();pType;pType=GetNext(),i++)
		{
			if(i==ii)
				return DeleteCursor();
		}
		return FALSE;
	}
    void Destroy()		// ����������ǰ��ʵ��
	{
		~ATOM_LIST();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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

//------------------------------ ������: fPtList������ -----------------------------------
class F_ENT_LIST_API fPtList
{
public:
    fPtList()
	{// ���캯��
		NodeNum=0;
		index = 0;
		cursor = head = tail = NULL;
		m_pAttachStack=NULL;
		m_lpTmpPosition=NULL;
	}
    ~fPtList();

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		f3dPoint atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
	}DATA_TYPE;
	//CStack<DATA_TYPE*> POS_STACK;
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
	ISimplePtrStack* m_pAttachStack;
	DATA_TYPE* m_lpTmpPosition;//��¼һ����ʱ�α�λ�ã��Ա���ڻָ��α�λ��

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
	//��ջ��������
	ISimplePtrStack* AttachPosStack(ISimplePtrStack *pStack){return m_pAttachStack=pStack;}
	void DetachPosStack(){m_pAttachStack=NULL;}
	int PushStack();
	bool PopStack(int pos=-1);
	int GetStackRemnantSize();
	
    f3dPoint* append();//�ڽڵ������ĩβ��ӽڵ�
    f3dPoint* append(const f3dPoint &pt,BOOL bJudgeNearest=FALSE);//�ڽڵ������ĩβ��ӽڵ�
	f3dPoint* append(double x,double y,double z,BOOL bJudgeNearest=FALSE);//�ڽڵ������ĩβ��ӽڵ�
	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    f3dPoint* insert(const f3dPoint &pt, long ii=-1);
	long GetCurrentIndex()const{return index;}//��ȡ��ǰ�ڵ��������
    f3dPoint* GetByIndex(long ii)const;
    f3dPoint& operator[](long ii)const{return *GetByIndex(ii);}//���������Ż�ȡ�߶�ʵ��
    f3dPoint* GetNext();
    f3dPoint* GetPrev();
    f3dPoint* GetFirst();
    f3dPoint* GetTail();
    f3dPoint* GetCursor();
	long GetNodeNum()const{return NodeNum;}
    BOOL DeleteCursor();	// ɾ��һ���ڵ�
    BOOL DeleteAt(long ii);	// ɾ��һ���ڵ�
    void Destroy();			// ����������ǰ��ʵ��
    void Empty();			// ��յ�ǰ��ʵ��������Ԫ��
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

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    f3dAtomLine* cursor;    // ��ǰ�߶�ָ��
    f3dAtomLine* tail;		// �߶�����ĩβָ��
    f3dAtomLine* head;		// �߶�������ʼָ��

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
    void Destroy();		// ����������ǰ��ʵ��
    void Empty();		// ��յ�ǰ��ʵ��������Ԫ��

    f3dAtomLine* append(const f3dAtomLine&);//�ڽڵ������ĩβ��ӽڵ�
    f3dAtomLine* append(f3dPoint* start,f3dPoint* end);//�ڽڵ������ĩβ��ӽڵ�

	//  void appendList(f3dLine& headLine);//�ڽڵ������ĩβ��ӽڵ�����
    
	static const long INSERT_BEFORE = -1;
	static const long INSERT_AFTER  = -2;

	//index==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    f3dAtomLine* insert(const f3dAtomLine&, long liAtIndex=INSERT_BEFORE);
    
	long GetCurrentIndex()const;//��ȡ��ǰ�ڵ��������

    f3dAtomLine* GetByIndex(long ii)const;
    f3dAtomLine& operator[](long)const;//���������Ż�ȡ�߶�ʵ��
    f3dAtomLine* GetNext();
    f3dAtomLine* GetPrev();
    f3dAtomLine* GetFirst();
    f3dAtomLine* GetTail();
    
	long GetNodeNum()const{return NodeNum;}
    BOOL DeleteAt(long index);	// ɾ��һ���ڵ�
	BOOL DeleteById(long id);	// ����idɾ��ֱ�߽ڵ�
};

inline fLineList::fLineList(){	// ���캯��
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
    void Empty();		// ��յ�ǰ��ʵ��������Ԫ��
	//�ڽڵ������ĩβ��ӽڵ�
    f3dAtomLine* append(const f3dAtomLine& atom_line){return loop->append(atom_line);}
    f3dAtomLine* append(f3dPoint* pStart,f3dPoint* pEnd){return loop->append(pStart,pEnd);}

	//index==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    void insert(const f3dAtomLine& atom_line, long index=-1){loop->insert(atom_line,index);}
    
	long GetCurrentIndex()const{return loop->GetCurrentIndex();}//��ȡ��ǰ�ڵ��������

    f3dAtomLine* GetByIndex(long ii)const{return loop->GetByIndex(ii);}
    f3dAtomLine& operator[](long ii)const{return *loop->GetByIndex(ii);}//���������Ż�ȡ�߶�ʵ��
    f3dAtomLine* GetNext(){return loop->GetNext();}
    f3dAtomLine* GetPrev(){return loop->GetPrev();}
    f3dAtomLine* GetFirst(){return loop->GetFirst();}
    f3dAtomLine* GetTail(){return loop->GetTail();}
    
	long GetNodeNum()const{return loop->GetNodeNum();}
    BOOL DeleteAt(long index){return loop->DeleteAt(index);}	// ɾ��һ���ڵ�
};
//�������������ߣ�
class F_ENT_LIST_API fCurveList{
public:
    fCurveList();
    ~fCurveList();

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		f3dCurve curve;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
	}DATA_TYPE;
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��

//2.���г�Ա��������
public:
	operator fLoop();
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
    void Destroy();		// ����������ǰ��ʵ��
    void Empty();		// ��յ�ǰ��ʵ��������Ԫ��

    f3dCurve* append(const f3dCurve&);//�ڽڵ������ĩβ��ӽڵ�
    f3dCurve* append(f3dPoint* start,f3dPoint* end);//�ڽڵ������ĩβ��ӽڵ�

	//  void appendList(f3dLine& headLine);//�ڽڵ������ĩβ��ӽڵ�����
    
	//index==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    f3dCurve* insert(const f3dCurve&, long index=-1);
    
	long GetCurrentIndex()const;//��ȡ��ǰ�ڵ��������

    f3dCurve* GetByIndex(long ii)const;
    f3dCurve& operator[](long)const;//���������Ż�ȡ�߶�ʵ��
    f3dCurve* GetNext();
    f3dCurve* GetPrev();
    f3dCurve* GetFirst();
    f3dCurve* GetTail();
    
	long GetNodeNum()const{return NodeNum;}
    int DeleteNode(long index);	// ɾ��һ���ڵ�
};

//------------------------------ ����������: fTriFaceList������ ----------------------------------
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
//1.˽�г�Ա��������
private:
    long index;		// �׽ڵ�������Ϊ1
    long NodeNum;	// ����
    DATA_TYPE* cursor;		//����������ǰָ��
    DATA_TYPE* tail;		//����������ĩβָ��
    DATA_TYPE* head;       //������������ʼָ��

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
    void Destroy();
    void Empty();
    f3dTriFace* append();//�ڽڵ������ĩβ��ӽڵ�
    f3dTriFace* append(const f3dTriFace&);//�ڽڵ������ĩβ��ӽڵ�
    f3dTriFace* append(const f3dPoint& p1,
                const f3dPoint& p2,
                const f3dPoint& p3);//�ڽڵ������ĩβ��ӽڵ�
	//  void appendList(fTriFace& headFace);//�ڽڵ������ĩβ��ӽڵ�����

    //index==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    f3dTriFace* insert(long index=-1);
    f3dTriFace* insert(const f3dTriFace&,long index=-1);
    f3dTriFace* insert(const f3dPoint& p0,
                const f3dPoint& p1,
                const f3dPoint& p2,long index=-1);

    long GetCurrentIndex()const;//��ȡ��ǰ�ڵ��������
    f3dTriFace* GetByIndex(long index);//���������Ż�ȡ������ʵ��
    f3dTriFace& operator [](long index);
    f3dTriFace* GetNext();
    f3dTriFace* GetPrev();
    f3dTriFace* GetFirst();
    f3dTriFace* GetTail();
    long GetNodeNum()const{return NodeNum;}
    int DeleteNode(long index);
};
inline fTriFaceList::fTriFaceList(){	// ���캯��
    NodeNum=0;
    index = 0;
    cursor = head = tail = NULL;
}

//------------------------------ �������: fPolyFace������ ---------------------------------

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
//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
    long index;			// �׽ڵ�������(�±�)Ϊ1
    long NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��

//2.���г�Ա��������
public:

//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
    void Destroy();		// ����������ǰ��ʵ��
    void Empty();		// ��յ�ǰ��ʵ��������Ԫ��

    fLoop* append();//�ڽڵ������ĩβ��ӽڵ�

	//index==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    fLoop* insert(fLoop loop, long index=-1);
    
	long GetCurrentIndex()const;//��ȡ��ǰ�ڵ��������
    fLoop* GetByIndex(long index)const;
    fLoop& operator[](long)const;//���������Ż�ȡ�߶�ʵ��
    fLoop* GetNext();
    fLoop* GetPrev();
    fLoop* GetFirst();
    fLoop* GetTail();
    
	long GetNodeNum()const{return NodeNum;}
    int DeleteNode(long index);	// ɾ��һ���ڵ�
};
inline fLoopList::fLoopList(){	// ���캯��
    NodeNum=0;
    index = 0;
    cursor = head = tail = NULL;
};

//class F_ENT_LIST_API fAtomList<fLineList*>;
class F_ENT_LIST_API f3dPolyFace{
	friend class fPolyFaceList;
	long id;	//���ڱ�ʶ����������е�ĳһ�ض���
public:
	f3dPoint poly_norm;			//�������������߷���
    fLineList outer_edge;		// �⻷��������
	fLoopList inner_loop;		// ����ε�������(�ڻ�)������
	COLORREF material;		// �����ڼ�¼�����������Ϣ(����ʵ�)
	//���ڿ�����ʾ����ʱ�����ȷֺ������棬��ʾʱ�Ͳ����ٷֽ���������
	fTriFaceList* m_pTriList;

public:
	BOOL IsValidFace();
	long GetId(){return id;}
	/*	vertex_list���������������������ʾ����ʵ�����������ָ��Ķ����б�
		axis_start,axis_end,radius�ֱ��ʾԲ�������ʼ����ֹ��Բ�ĵ㼰Բ����뾶
		cut_line_list������������ɵ��и���б�
	*/
	//void CutByCylinder(fPtList *vertex_list,f3dPoint axis_start,f3dPoint axis_end,double radius,fLineList *cut_line_list=NULL);
	/*	vertex_list���������������������ʾ����ʵ�����������ָ��Ķ����б�
		pick,norm�ֱ��ʾ�и���Ļ�׼�㼰�и��淨��
		cut_line_list������������ɵ��и���б�
	*/
	void CutByPlane(fPtList *vertex_list,f3dPoint pick,f3dPoint norm,fLineList *cut_line_list=NULL);
    f3dPolyFace(){id=0;m_pTriList=NULL;material = 0;}	// ��ʼ���͹��캯��.
    f3dPolyFace(const f3dPolyFace& face);		// �����͹��캯��.

    BOOL GetPolyUCS(UCS_STRU& ucs)const;	// �õ����������������ϵ
							// ������ϵ�� WJH ��������Զ����ɵ�

    ~f3dPolyFace();
	/*������ż�㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�㲻�ڶ����������
				1��ʾ���ڶ���������ڣ������߽����ڣ�
				2��ʾ���ڶ���������ϵ����ڶ����������
	*/
	int PtInFace(f3dPoint pt);
    /*  �ж�һ��������Ƿ�Ϊ͹������档
         1��ʾ͹;
         0��ʾ��;
        -1��ʾ����ζ�������(���б߽����غ�)
        -2��ʾ����ζ�������(�߽��߼以�಻����)
        -3��ʾ����ζ�������(����β����) 
		-4��ʾ����ζ�������(����α߽��ཻ��)*/
    int is_fPolyFace_protrusive()const;
	// ��һ�������ֽ�Ϊһ������������(��һϵ�в��ཻ����������ɵ�)
	//		�ں����ڲ��Զ������ڴ棬����ʹ����Ϻ���Ҫ����
	//		Destory()�������ٸ÷��ص�ָ�롣
	void ExplodeToTriFace();
};

//------------------------------ �����������: fPolyFaceList������ ---------------------------------
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
//1.˽�г�Ա��������
private:
    long index;				//�׽ڵ�������Ϊ1
    long NodeNum;
    DATA_TYPE* cursor;    //�����������ǰָ��
    DATA_TYPE* tail;		//�����������ĩβָ��
    DATA_TYPE* head;		//�������������ʼָ��

	ISimplePtrStack* m_pAttachStack;
	DATA_TYPE* m_lpTmpPosition;//��¼һ����ʱ�α�λ�ã��Ա���ڻָ��α�λ��
//2.���г�Ա��������
public:
//3.˽�г�Ա��������
private:
//4.1��ջ��������
public:
	ISimplePtrStack* AttachPosStack(ISimplePtrStack *pStack){return m_pAttachStack=pStack;}
	void DetachPosStack(){m_pAttachStack=NULL;}
	int PushStack();
	bool PopStack(int pos=-1);
	int GetStackRemnantSize();
	
//4.2���г�Ա��������
public:
    void Destroy();
    void Empty();
    f3dPolyFace* append();	//�ڽڵ������ĩβ��ӽڵ�

	//  void appendList(fPolyFace& headFace);//�ڽڵ������ĩβ��ӽڵ�����
    
	//index==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    f3dPolyFace* insert(long index=-1);
    
	long GetCurrentIndex()const;//��ȡ��ǰ�ڵ��������
    f3dPolyFace* GetByIndex(long index);
    f3dPolyFace& operator [](long index);
    f3dPolyFace* GetNext();
    f3dPolyFace* GetPrev();
    f3dPolyFace* GetFirst();
    f3dPolyFace* GetTail();
    long GetNodeNum()const{return NodeNum;}
    int DeleteAt(long index);	//��������ֵɾ����
	int DeleteById(long id);	//���ݶ�������ʶɾ����
};
inline fPolyFaceList::fPolyFaceList(){	// ���캯��
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
	BOOL GetNorm(f3dPoint& norm)const;	//��ö���εķ��߷���
	BOOL CreatePolygonRgn(f3dPoint* lp3dPoints, int nCount);
	BOOL is_fPolygonRgn_a_face()const;
	/*���ݽ����㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�㲻�ڶ����������
				1��ʾ���ڶ���������ڣ������߽����ڣ�
				2��ʾ���ڶ���������ϵ����ڶ����������
	*/
	int PtInRgn(f3dPoint pt)const;
	/*���ݽ����㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�߲��ڶ����������
				1��ʾ���ڶ���������ڣ������߽����ڣ�
				2��ʾ���ڶ���������ϵ���ȫ�ڶ����������
				3��ʾ���ڶ���������ϵ�ȫ���ڶ����������
	*/
	int LineInRgn(f3dLine line)const;
	void Empty();
};
//POLYGON���ڴ���fPolygonRgn���ټ����Ƿ��ڶ���������ڵ��㷨 wjh-2015.4.12
class F_ENT_LIST_API POLYGON{
protected:
	bool m_bRectangleXY;	//���ڴ����ƽ����εļ���
	int count;
	GEPOINT zAxis, *vertexes;
public:
	GEPOINT GetAxisZ(){return zAxis;}
	BOOL CreatePolygonRgn(f3dPoint* lp3dPoints, int nCount);
	int GetVertexCount(){return count;}
	GEPOINT GetVertexAt(int i);
	/*���ݽ����㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�㲻�ڶ����������
				1��ʾ���ڶ���������ڣ������߽����ڣ�
				2��ʾ���ڶ���������ϵ����ڶ����������
	*/
	int PtInRgn(const double* poscoord)const;
	/*���ݽ����㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�߲��ڶ����������
				1��ʾ���ڶ���������ڣ������߽����ڣ�
				2��ʾ���ڶ���������ϵ���ȫ�ڶ����������
				3��ʾ���ڶ���������ϵ�ȫ���ڶ����������
	*/
	int LineInRgn(const double* posStart,const double* posEnd)const;
	/*���ݽ����㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�㲻�ڶ����������
				1��ʾ���ڶ��������߽���
				2��ʾ���ڶ��������߽���
				3��ʾ���ڶ��������߽���
	*/
	int PtInRgn2(const double* poscoord);
	/*���ݽ����㷨�ж�һ���Ƿ��ڵ�ǰ�ռ���������
		����ֵ��0��ʾ�߲��ڶ����������
				1��ʾ���ڶ���������ڣ������߽����ڣ�
				2��ʾ���ڶ���������ϵ���ȫ�ڶ����������
				3��ʾ���ڶ���������ϵ�ȫ���ڶ����������
	*/
	int LineInRgn2(const double* posStart,const double* posEnd);
	double CalArea(double* center=NULL);	//center Ϊ���صļ������ĵ�����
	POLYGON(){count=0;vertexes=NULL;m_bRectangleXY=false;}
	~POLYGON();
};

//------------------------ ʵ����: fBody���� -------------------------------------
class F_ENT_LIST_API fBody{
	int start_vertex_i;
	void CutFaceByBandPlaneList(f3dPolyFace *pFace,fPtList &band_plane_vertex_list,
			f3dPoint extend_vec,double extend_dist,fLineList *cut_line_list,COLORREF material);
public:
	fBody(){start_vertex_i=0;m_bModified=TRUE;}
	//�������������Χ����ͨ��blAddSectFace�����Ƿ���������� wjh-2019.4.17
	int CutByPlane(f3dPoint pick, f3dPoint norm,COLORREF material=NULL,bool blAddSectFace=true);//������ӵ�������
	//Ҫ��extend_vec���϶���η���и����������������ͬʱҪ���и����λ��ͱ��и��涼Ϊ͹
	void CutByBandPlaneList(fPtList &band_plane_vertex_list,f3dPoint extend_vec,double extend_dist,COLORREF material=NULL);
	BOOL IsIntWith(fBody *pBody);//�Ƿ�������ʵ����ײ
		// �������㡱������ı�ʱ, 
		//	 ʵ��ĸ������桱��������ָ����Ǹá����㡱��
		//	 ��ˡ��桱�ܹ��õ��Զ��ĸ��¡�
	void Empty();
	f3dAtomLine* NewEdgeLine(f3dPolyFace *pFace, int e_vertex_i, int s_vertex_i=-1,BOOL bVisible=TRUE);
public:
	BOOL m_bModified;
    fPolyFaceList faceList;		// ʵ���ɡ��桱�������.
    fPtList vertex;				// ʵ��ġ����㡱����
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
	GEPOINT FaceNorm();			//�������������߷���
	COLORREF MatColor();		// �����ڼ�¼�����������Ϣ(����ʵ�)
	long FaceId();	//���ڱ�ʶ����������е�ĳһ�ض���
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
