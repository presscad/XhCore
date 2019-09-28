#pragma once
#include "Hashtable.h"
#include "..\IXeroCad.h"

/*class CDbEntityAttachData{
public:
	long m_idBelongEntity;	//所附属图元的内部标识CDbEntity::id
	ILifeDataObject* m_pDataObject;
	CDbEntityAttachData(){m_idBelongEntity=0;m_pDataObject=NULL;}
	~CDbEntityAttachData(){m_pDataObject->DestroySelf();m_pDataObject=NULL;}
};*/
struct XERODRAW_TYPE{
	int idType; 
	char name[51];
};
class CDrawingType{
public:
	int m_nDrawingType;
	int m_nDrawComponentType;
	int m_nDrawCommandType;
	CHashList<XERODRAW_TYPE>hashDrawTypes,hashComponentTypes,hashDrawCmdTypes;
};
struct DrawCmdType{
	int idType;
	IDrawingCommand* (*CreateNewAtom)(int CLSTYPE_ID);
	BOOL (*DeleteAtom)(IDrawingCommand *pAtom);
};
struct DrawingCommandContext{
	IDrawing* pParentDrawing;
	IDrawingComponent* pParentComponent;
};
class CXeroDrawingCommand : public IDrawingCommand{
protected:
	int m_id;
	void *m_pDataContext;	//CTowerModel*
	IDrawing* m_pParentDrawing;
	IDrawingComponent* m_pParentComponent;
	CHashSet<IDbEntity*>listDbEntSet;
	ILifeDataObject* m_pDataObject;
	int m_iDrawCmdType;
public:
public:
	static CHashList<DrawCmdType>hashDrawCmdTypes;
public:
	CXeroDrawingCommand(int id=0,DrawingCommandContext* pContext=NULL);
	virtual ~CXeroDrawingCommand(){EmptyDbEntitySet();}
	bool RegisterCmdType(int type,	CXeroDrawingCommand* (*CreateNewAtom)(int CLSTYPE_ID),	BOOL (*DeleteAtom)(CXeroDrawingCommand *pAtom));
	//void ExecuteCmd(int type,CHashSet<CDbEntity*>& hashSetEnts);
public:
	virtual int GetDrawCmdType(){return m_iDrawCmdType;}
	virtual void* SetContext(void* pContext);	//设定上下文指针
	virtual void* GetContext(){return m_pDataContext;}				//获取上下文指针
	virtual IDrawing* BelongDrawing(){return m_pParentDrawing;}
	virtual IDrawingComponent* BelongDrawingComponent(){return m_pParentComponent;}
	virtual IDbEntity* EnumFirstDbEntity(){return listDbEntSet.GetFirst();}
	virtual IDbEntity* EnumNextDbEntity(){return listDbEntSet.GetNext();}
	virtual IDbEntity* AppendDbEntityRef(int idDbEnt,IDbEntity* pDbEnt);
	virtual void EmptyDbEntitySet(){return listDbEntSet.Empty();}
	//virtual void Redraw();
};
struct DrawingComponentContext{
	void *pDataContext;	//CTowerModel*
	IDrawing* pParentDrawing;
};
class CXeroDrawingComponent : public IDrawingComponent
{
protected:
	int m_id;
	DrawingCommandContext context;
	void *m_pDataContext;	//CTowerModel*
	IDrawing* m_pParentDrawing;
	ILifeDataObject* m_pDataObject;
	friend class CXeroDrawing;
public:
	int m_iComponentType;	//图纸类型
	UCS_STRU dcs;		//坐标系
	CSuperHashList<IDrawingCommand> listCmds;	//图元列表
	CHashSet<IDbEntity*>listDbEntSet;
public:
	CXeroDrawingComponent(int id=0,IDrawing* pDrawing=NULL);
	virtual ~CXeroDrawingComponent(){listCmds.Empty();EmptyDbEntitySet();}
	virtual int GetComponentType(){return m_iComponentType;}
	virtual void* SetContext(void* pContext);	//设定上下文指针
	virtual void* GetContext(){return m_pDataContext;}				//获取上下文指针
	virtual IDrawing* BelongDrawing(){return m_pParentDrawing;}
	virtual IDrawingCommand* EnumFirstDrawingCommand(){return listCmds.GetFirst();}
	virtual IDrawingCommand* EnumNextDrawingCommand(){return listCmds.GetNext();}
	virtual IDbEntity* EnumFirstDbEntity(){return listDbEntSet.GetFirst();}
	virtual IDbEntity* EnumNextDbEntity(){return listDbEntSet.GetNext();}
	virtual IDbEntity* AppendDbEntityRef(int idDbEnt,IDbEntity* pDbEnt){return listDbEntSet.SetValue(idDbEnt,pDbEnt);}
	virtual IDrawingCommand *AppendDrawingCommand(int cmd_type);
	virtual void EmptyDbEntitySet(){return listDbEntSet.Empty();}
	//virtual void Redraw();
};

class CXeroDrawing : public IDrawing
{
	long m_id;
	void *m_pContext;	//CTowerModel*
	ILifeDataObject* m_pDataObject;
protected:
	BYTE m_cbRenderMode;
	DrawingCommandContext contextCmd;
	DrawingComponentContext contextCmpt;
public:
	int m_iDrawType;	//图纸类型
	char m_sDrawingName[51];
	IEntityDatabase* m_pEntityDatabase;
	CSuperHashList<IDrawingComponent> listComponents;	//图元列表
	CSuperHashList<IDrawingCommand> listCmds;	//图元列表
	static const BYTE ENTITY_STATE_INSERT = 0x01;
	static const BYTE ENTITY_STATE_MODIFY = 0x02;
	static const BYTE ENTITY_STATE_DELETE = 0x03;
	CHashList<BYTE>hashEntityState;
	CXeroDrawing(int id=0);
	virtual ~CXeroDrawing();
public:
	virtual long GetId(){return m_id;}
	virtual void* SetContext(void* pContext);	//设定上下文指针
	virtual void* GetContext(){return m_pContext;}				//获取上下文指针
	virtual bool SetOCS(GECS& ocs){return m_pEntityDatabase->SetOCS(ocs);}
	virtual void GetOCS(GECS& ocs){m_pEntityDatabase->GetOCS(ocs);}
	virtual int SetName(char* name);
	virtual int GetDrawType(){return m_iDrawType;}
	virtual IDrawingComponent* EnumFirstDrawingComponent(){return listComponents.GetFirst();}
	virtual IDrawingComponent* EnumNextDrawingComponent(){return listComponents.GetNext();}
	virtual IDrawingCommand* EnumFirstDrawingCommand(){return listCmds.GetFirst();}
	virtual IDrawingCommand* EnumNextDrawingCommand(){return listCmds.GetNext();}
	virtual IDrawingCommand   *AppendDrawingCommand(int cmd_type);
	virtual IDrawingComponent *AppendDrawingComponent(int component_type);
	virtual IDbEntity* EnumFirstDbEntity(){return m_pEntityDatabase->EnumFirstDbEntity();}
	virtual IDbEntity* EnumNextDbEntity(){return m_pEntityDatabase->EnumNextDbEntity();}
	virtual IDbEntity* EnumCursorDbEntity(){return m_pEntityDatabase->EnumCursorDbEntity();}
	virtual BOOL DeleteDrawingComponent(long serial){return listComponents.DeleteNode(serial);}
	virtual BOOL DeleteDrawingCommand(long serial){return listCmds.DeleteNode(serial);}
	virtual void EmptyDrawingCommands(){listCmds.Empty();}
	virtual void EmptyDrawingComponents(){listComponents.Empty();}
	virtual IDbEntity* AppendDbEntity(int idDbEntType);
	virtual IDbEntity* GetDbEntity(long id){return m_pEntityDatabase->GetDbEntity(id);}
	virtual BOOL DeleteDbEntity(long id);
	virtual BOOL DeleteCursorDbEntity(BOOL bClean=FALSE);
	virtual void CleanDeletedEntities(){m_pEntityDatabase->CleanDeletedEntities();}
	virtual BOOL DeleteDbEntityByHiberMasterId(long hiberMasterId);
	virtual void EmptyDbEntities();
	virtual BYTE GetEntityState(long id);
	virtual BYTE SetRenderedMode(BYTE cbRenderMode=0);
	virtual BYTE RenderMode();	//0.不需要重新渲染;1.部分图元需要重新渲染;2.全部图元需要重新渲染
	virtual IEntityDatabase* Database(){return m_pEntityDatabase;}
	//virtual void Redraw();
};
