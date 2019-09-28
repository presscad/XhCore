#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "LocalDrawing.h"

IDrawingComponent* CreateDrawComponent(int idDrawComponentType,DWORD key,void* pContext)
{
	for(DrawingComponentType* pDrawComponentType=IDrawingComponent::EnumFirstRegType();pDrawComponentType;
		pDrawComponentType=IDrawingComponent::EnumNextRegType())
	{
		if(IDrawingCommand::GetCursorRegType()==idDrawComponentType)
			return pDrawComponentType->CreateNewAtom(idDrawComponentType,key,pContext);
	}
	return NULL;
}
BOOL DeleteDrawComponent(IDrawingComponent *pDrawComponent)				//必设回调函数
{
	for(DrawingComponentType* pDrawComponentType=IDrawingComponent::EnumFirstRegType();pDrawComponentType;
		pDrawComponentType=IDrawingComponent::EnumNextRegType())
	{
		if(IDrawingCommand::GetCursorRegType()==pDrawComponent->GetComponentType())
			return pDrawComponentType->DeleteAtom(pDrawComponent);
	}
	return FALSE;
}
IDrawingCommand* CreateDrawCommand(int idDrawCmdType,DWORD key,void* pContext)
{
	for(DrawingCommandType* pDrawCmdType=IDrawingCommand::EnumFirstRegType();pDrawCmdType;
		pDrawCmdType=IDrawingCommand::EnumNextRegType())
	{
		if(IDrawingCommand::GetCursorRegType()==idDrawCmdType)
			return pDrawCmdType->CreateNewAtom(idDrawCmdType,key,pContext);
	}
	return NULL;
}
BOOL DeleteDrawCommand(IDrawingCommand *pDrawCmd)				//必设回调函数
{
	for(DrawingCommandType* pDrawCmdType=IDrawingCommand::EnumFirstRegType();pDrawCmdType;
		pDrawCmdType=IDrawingCommand::EnumNextRegType())
	{
		if(IDrawingCommand::GetCursorRegType()==pDrawCmd->GetDrawCmdType())
			return pDrawCmdType->DeleteAtom(pDrawCmd);
	}
	return FALSE;
}
/////////////////////////////////////////////////////
// class CXeroDrawing
CXeroDrawing::CXeroDrawing(int id/*=0*/)
{
	m_id=id;
	m_iDrawType=1;
	memset(m_sDrawingName,0,51);
	m_pDataObject=NULL;
	m_pContext=NULL;
	contextCmpt.pDataContext=NULL;
	contextCmpt.pParentDrawing=contextCmd.pParentDrawing=this;
	contextCmd.pParentComponent=NULL;
	listComponents.m_pContext=&contextCmpt;
	listCmds.m_pContext=&contextCmd;
	m_pEntityDatabase=IEntityDatabase::CreateEntityDatabase();
	m_cbRenderMode=2;
	
	listComponents.CreateNewAtom=CreateDrawComponent;
	listComponents.DeleteAtom=DeleteDrawComponent;
	listCmds.CreateNewAtom=CreateDrawCommand;
	listCmds.DeleteAtom=DeleteDrawCommand;
}
CXeroDrawing::~CXeroDrawing()
{
	if(m_pEntityDatabase)
		IEntityDatabase::DestroyEntityDatabase(m_pEntityDatabase->GetId());
}
void* CXeroDrawing::SetContext(void* pContext)
{
	m_pContext=pContext;
	return m_pContext;
}
int CXeroDrawing::SetName(char* name)
{
	strncpy(m_sDrawingName,name,50);
	m_sDrawingName[50]=0;
	return strlen(m_sDrawingName);
}
IDrawingCommand *CXeroDrawing::AppendDrawingCommand(int cmd_type)
{
	return listCmds.Add(0,cmd_type);
}
IDrawingComponent *CXeroDrawing::AppendDrawingComponent(int component_type)
{
	return listComponents.Add(0,component_type);
}
IDbEntity* CXeroDrawing::AppendDbEntity(int idDbEntType)
{
	SetRenderedMode(1);
	IDbEntity* pEnt=m_pEntityDatabase->AppendDbEntity(idDbEntType);
	BYTE* pcbState=hashEntityState.GetValue(pEnt->GetId());
	if(pcbState)
		hashEntityState.SetValue(pEnt->GetId(),ENTITY_STATE_MODIFY);
	else
		hashEntityState.SetValue(pEnt->GetId(),ENTITY_STATE_INSERT);
	return pEnt;
}
BOOL CXeroDrawing::DeleteDbEntity(long id)
{
	SetRenderedMode(1);
	hashEntityState.SetValue(id,ENTITY_STATE_DELETE);
	return m_pEntityDatabase->DeleteDbEntity(id);
}
BOOL CXeroDrawing::DeleteCursorDbEntity(BOOL bClean/*=FALSE*/)
{
	SetRenderedMode(1);
	IDbEntity* pEnt=m_pEntityDatabase->EnumCursorDbEntity();
	if(pEnt==NULL)
		return FALSE;
	hashEntityState.SetValue(pEnt->GetId(),ENTITY_STATE_DELETE);
	return m_pEntityDatabase->DeleteCursorDbEntity(bClean);
}
BOOL CXeroDrawing::DeleteDbEntityByHiberMasterId(long hiberMasterId)
{
	SetRenderedMode(1);
	for(IDbEntity* pEnt=EnumFirstDbEntity();pEnt;pEnt=EnumNextDbEntity())
	{
		if(pEnt->GetHiberId().masterId==hiberMasterId)
			hashEntityState.SetValue(pEnt->GetId(),ENTITY_STATE_DELETE);
	}
	return m_pEntityDatabase->DeleteDbEntityByHiberMasterId(hiberMasterId);
}
void CXeroDrawing::EmptyDbEntities()
{
	for(IDrawingComponent* pComponent=EnumFirstDrawingComponent();pComponent;pComponent=EnumNextDrawingComponent())
		pComponent->EmptyDbEntitySet();
	for(IDrawingCommand* pDrawCmd=EnumFirstDrawingCommand();pDrawCmd;pDrawCmd=EnumNextDrawingCommand())
		pDrawCmd->EmptyDbEntitySet();
	if(m_pEntityDatabase)
		m_pEntityDatabase->EmptyDbEntities();
	SetRenderedMode(2);
	hashEntityState.Empty();
}
BYTE CXeroDrawing::GetEntityState(long id)
{
	BYTE* pcbState=hashEntityState.GetValue(id);
	return pcbState!=NULL ? *pcbState : 0;
}
BYTE CXeroDrawing::SetRenderedMode(BYTE cbRenderMode/*=0*/)
{
	if(m_cbRenderMode==2&&cbRenderMode==1)
		return m_cbRenderMode;	//只要之前设定了全部刷新,则全部刷新
	return m_cbRenderMode=cbRenderMode;
}
//0.不需要重新渲染;1.部分图元需要重新渲染;2.全部图元需要重新渲染
BYTE CXeroDrawing::RenderMode()
{
	return m_cbRenderMode;//!m_bRendered;
}

/////////////////////////////////////////////////////
// class CXeroDrawingComponent
CXeroDrawingComponent::CXeroDrawingComponent(int id/*=0*/,IDrawing* pDrawing/*=NULL*/)
{
	m_id=id;
	m_iComponentType=0;
	m_iComponentType=NULL;
	m_pDataObject=NULL;
	context.pParentComponent=this;
	context.pParentDrawing=m_pParentDrawing=pDrawing;
	listCmds.m_pContext=&context;
	listCmds.CreateNewAtom=CreateDrawCommand;
	listCmds.DeleteAtom=DeleteDrawCommand;
}
void* CXeroDrawingComponent::SetContext(void* pContext)
{
	m_pDataContext=pContext;
	return m_pDataContext;
}
/////////////////////////////////////////////////////
// class CXeroDrawingCommand
CXeroDrawingCommand::CXeroDrawingCommand(int id/*=0*/,DrawingCommandContext* pContext/*=NULL*/)
{
	m_id=id;
	if(pContext!=NULL)
	{
		m_pParentDrawing=pContext->pParentDrawing;
		m_pParentComponent=pContext->pParentComponent;
	}
	else
	{
		m_pParentDrawing=NULL;
		m_pParentComponent=NULL;
	}
	m_pDataContext=NULL;	//CTowerModel*
	m_pDataObject=NULL;
	m_iDrawCmdType=0;
}
void* CXeroDrawingCommand::SetContext(void* pContext)
{
	m_pDataContext=pContext;
	return m_pDataContext;
}
IDbEntity* CXeroDrawingCommand::AppendDbEntityRef(int idDbEnt,IDbEntity* pDbEnt)
{
	if(BelongDrawingComponent())
		BelongDrawingComponent()->AppendDbEntityRef(idDbEnt,pDbEnt);
	return listDbEntSet.SetValue(idDbEnt,pDbEnt);
}
IDrawingCommand *CXeroDrawingComponent::AppendDrawingCommand(int cmd_type)
{
	return listCmds.Add(0,cmd_type);
}
