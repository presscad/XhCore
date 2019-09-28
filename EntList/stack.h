#pragma once 

#include "math.h"

template <class TYPE> struct ISimpleStack
{
public:
	virtual bool IsEmpty()=0;
	virtual void Empty()=0;
	virtual int GetRemnantSize()=0;
	virtual int GetPushSize()=0;
	virtual TYPE* TopAtom()=0;		//访问栈顶元素
	virtual int push(const TYPE& val)=0;
	virtual bool pop(TYPE& res)=0;
};
typedef void* XhPtr;
typedef ISimpleStack<XhPtr> ISimplePtrStack;
/////////////////////////////////////////////////////////
//普通堆栈类
template <class TYPE> class CStack : public ISimpleStack<TYPE>
{
	char *top;
	char *bottom;
	char *cursor;
	char *work;
	size_t bytes;
	size_t blockSize;
public:
	CStack(size_t nCount=2)
	{
		blockSize=2;
		bytes = sizeof(TYPE);
		top = new char[nCount*bytes];
		bottom = cursor = top + nCount*bytes;
		work = NULL;
	}
	~CStack(){ delete []top; }
	void SetBlockSize(size_t nCount)
	{
		blockSize=nCount;
	}
	//重载基类虚函数
	virtual bool IsEmpty()
	{
		if(bottom<=cursor)
			return true;
		else
			return false;
	}
	virtual void Empty()
	{
		cursor = bottom;
	}
	virtual int GetRemnantSize()
	{
		return (int)(cursor-top)/(int)bytes;
	}
	virtual int GetPushSize()
	{
		return (int)(bottom-cursor)/(int)bytes;
	}
	virtual TYPE* TopAtom()		//访问栈顶元素
	{
		return (TYPE*)cursor;
	}
	virtual int push(const TYPE& val)
	{
		if(top > cursor - bytes)
		{
			size_t d=2;
			int nPos=(int)(bottom-cursor);
			int nLen=(int)(bottom-top);
			char *tm=new char[nLen+blockSize*bytes];
			memcpy(tm+blockSize*bytes,top,nLen);
			delete []top;
			top=tm;
			bottom=top+nLen+blockSize*bytes;
			cursor=bottom-nPos-bytes;
			memcpy(cursor,&val, bytes);
		}
		else
		{
			cursor -= bytes;
			memcpy(cursor, &val, bytes);
		}
		return (int)(bottom-cursor)/(int)bytes;
	}
	virtual bool pop(TYPE& res)
	{
		if(bottom < (cursor + bytes))
			return false;
		else
		{
			memcpy(&res, cursor, bytes);
			cursor += bytes;
			return true;
		}
	}
};
