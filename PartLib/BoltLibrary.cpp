#include "stdafx.h"
#include "PartLib.h"
#include "stdlib.h"
#include "HashTable.h"
#include "BoltLibrary.h"

/////////////////////////////////////////////////////////
CHashList<CLsFamily> globalBoltLib;
CHashList<CLsSubPartFamily> globalLsSubPartLib;
CXhSimpleList<LOCAL_BOLT_CODESERIES> globalLocalLsSeries;
//////////////////////////////////////////////////////////////////////////
//CLsSubPartFamily
//////////////////////////////////////////////////////////////////////////
double LS_SUB_PART::set_dfShapeDe(double dfDe)
{	//零件外形参数D2(六角螺栓默认为螺栓对角顶点宽度)
	this->dfShapeDs=dfDe*0.86602540378443864676372317075294;
	return this->_dfShapeDe=dfDe;
}
double LS_SUB_PART::get_dfShapeDe()
{	//零件外形参数D2(六角螺栓默认为螺栓对角顶点宽度)
	double dfDe=dfShapeDs*1.1547005383792515290182975610039;
	return dfDe;
}

CLsSubPartFamily::CLsSubPartFamily()
{
	m_nSize=0;
	m_pLsSubPartSet=NULL;
	m_widType=0;
	strcpy(name,"");
}
CLsSubPartFamily::~CLsSubPartFamily()
{
	if(m_pLsSubPartSet)
		delete []m_pLsSubPartSet;
}
bool CLsSubPartFamily::CreateLsSubPartFamily(int num)
{
	if(num<=0)
		return false;
	if(m_pLsSubPartSet)
		delete []m_pLsSubPartSet;
	m_pLsSubPartSet=new LS_SUB_PART[num];
	m_nSize=num;
	return true;
}
void CLsSubPartFamily::CleanSubParts()
{
	m_nSize=0;
	if(m_pLsSubPartSet)
		delete []m_pLsSubPartSet;
}
LS_SUB_PART *CLsSubPartFamily::RecordAt(int i)
{
	if(i>=0&&i<m_nSize)
		return &m_pLsSubPartSet[i];
	else
		return NULL;
}
LS_SUB_PART *CLsSubPartFamily::FindLsSubPart(int d)
{
	LS_SUB_PART* pSunPart=NULL;
	for(int i=0;i<m_nSize;i++)
	{
		if(m_pLsSubPartSet[i].wiLsD==d)
		{
			pSunPart=&m_pLsSubPartSet[i];
			break;
		}
	}
	return pSunPart;
}
/////////////////////////////////////////////////////////
//CLsFamily 类定义
/////////////////////////////////////////////////////////
CLsFamily::CLsFamily()
{
	m_hFamily=m_hMappingAntiTheftFamily=m_hMappingAntiLooseFamily=0;
	m_nFamilySize=0;
	m_uiCodeSerial=1;
	m_pFamily=NULL;
	m_bDualCap=false;	//单帽螺栓
	m_bBurglarproof=FALSE;	//默认非防盗普通螺栓
	m_bAntiLoose=false;		//默认无显示垫圈
  	m_bFootNail=FALSE;		//默认为普通非脚钉螺栓
	strcpy(name,"");
	strcpy(descr,"");
	for(int i=0;i<8;i++)
	{
		ls_sub_part[i].idSubPart=0;
		ls_sub_part[i].nCount=0;
	}
}
CLsFamily::~CLsFamily()
{
	if(m_pFamily)
		delete []m_pFamily;
}
bool CLsFamily::CreateFamily(int num)
{
	if(num<=0)
		return false;
	if(m_pFamily)
		delete []m_pFamily;
	m_pFamily=new LS_XING_HAO[num];
	m_nFamilySize=num;
	return true;
}
LS_XING_HAO* CLsFamily::RecordAt(int i)
{
	if(i>=0&&i<m_nFamilySize)
		return &m_pFamily[i];
	else
		return NULL;
}
LS_XING_HAO* CLsFamily::FindLsGuiGe(int d, double L0)
{
	LS_XING_HAO *pDownLimXingHao=NULL,*pUpLimXingHao=NULL;
	for(int i=0;i<m_nFamilySize;i++)
	{
		/*以往脚钉不受通厚限制，但日前发现同直径有两种以上长度脚钉的出现，
		因此改为脚钉也受通厚限制，但通厚范围是0~L。 wjh 2011.11.28
		if(m_pFamily[i].up_lim==0)
		{
			if(m_pFamily[i].d==d)
				return &m_pFamily[i];
		}
		else */
		if(m_pFamily[i].d==d)
		{
			if(L0<=m_pFamily[i].up_lim&&L0>=m_pFamily[i].down_lim)
				return &m_pFamily[i];
			if(pDownLimXingHao==NULL)
				pDownLimXingHao=&m_pFamily[i];
			else if(m_pFamily[i].down_lim<pDownLimXingHao->down_lim)
				pDownLimXingHao=&m_pFamily[i];
			if(pUpLimXingHao==NULL)
				pUpLimXingHao=&m_pFamily[i];
			else if(m_pFamily[i].up_lim<pUpLimXingHao->up_lim)
				pUpLimXingHao=&m_pFamily[i];
		}
	}
	//无合适螺栓规格时，查找相对较佳螺栓规格，不合理情况交由用户进行螺栓合法性校核
	if(pDownLimXingHao&&L0<=pDownLimXingHao->down_lim)
		return pDownLimXingHao;
	else if(pUpLimXingHao&&L0>=pUpLimXingHao->up_lim)
		return pUpLimXingHao;
	return NULL;
}
LS_XING_HAO* CLsFamily::FindLsGuiGeByValidLen(long d, double valid_len)
{
	for(int i=0;i<m_nFamilySize;i++)
	{
		if(m_pFamily[i].d==d&&valid_len==m_pFamily[i].valid_len)
			return &m_pFamily[i];
	}
	return NULL;
}
int CLsFamily::GetComponentCount()
{
	int nCount=0;
	for(int i=0;i<8;i++)
	{
		if(ls_sub_part[i].idSubPart>0&&ls_sub_part->nCount>0)
			nCount++;
	}
	return nCount;
}
int CLsFamily::GetLsSubPartNum(int iSubPartType)
{
	for(int i=0;i<8;i++)
	{
		if(ls_sub_part[i].idSubPart==iSubPartType)
			return ls_sub_part[i].nCount;
	}
	return 0;
}
void CLsFamily::SetLsSubPartNum(int iSubPartType,int nNum)
{
	BOOL bFind=FALSE;
	for(int i=0;i<8;i++)
	{
		if(ls_sub_part[i].idSubPart==iSubPartType)
		{
			ls_sub_part[i].nCount=nNum;
			if(nNum==0)
				ls_sub_part[i].idSubPart=0;
			bFind=TRUE;
			break;
		}
	}
	if(!bFind)
	{	//没有该记录，在空的位置添加记录
		for(int i=0;i<8;i++)
		{
			if(ls_sub_part[i].idSubPart==0)
			{
				ls_sub_part[i].idSubPart=iSubPartType;
				ls_sub_part[i].nCount=nNum;
				break;
			}
		}
	}
}
void CLsFamily::CopyLsFamily(CLsFamily* pSrcFamily)
{
	if(pSrcFamily==NULL)
		return ;
	this->m_uiCodeSerial=pSrcFamily->m_uiCodeSerial;
	strcpy(this->name,pSrcFamily->name);
	strcpy(this->descr,pSrcFamily->descr);
	this->m_bAntiLoose=pSrcFamily->m_bAntiLoose;
	this->m_bDualCap=pSrcFamily->m_bDualCap;
	this->m_bFootNail=pSrcFamily->m_bFootNail;
	this->m_bBurglarproof=pSrcFamily->m_bBurglarproof;
	this->m_hMappingAntiTheftFamily=pSrcFamily->m_hMappingAntiTheftFamily;
	this->m_hMappingAntiLooseFamily=pSrcFamily->m_hMappingAntiLooseFamily;
	for(int i=0;i<8;i++)
	{
		this->ls_sub_part[i].idSubPart=pSrcFamily->ls_sub_part[i].idSubPart;
		this->ls_sub_part[i].nCount=pSrcFamily->ls_sub_part[i].nCount;
	}
	this->CreateFamily(pSrcFamily->GetCount());
	for(int j=0;j<pSrcFamily->GetCount();j++)
	{
		LS_XING_HAO *pSrcXingHao=pSrcFamily->RecordAt(j);
		LS_XING_HAO *pNewXingHao=this->RecordAt(j);
		pNewXingHao->grade=pSrcXingHao->grade;			//螺栓级别
		pNewXingHao->d=pSrcXingHao->d;					//螺栓直径
		strcpy(pNewXingHao->guige,pSrcXingHao->guige);	//螺栓规格
		pNewXingHao->valid_len=pSrcXingHao->valid_len;	//有效长
		pNewXingHao->no_thread_len=pSrcXingHao->no_thread_len;	//无扣长
		pNewXingHao->down_lim=pSrcXingHao->down_lim;	//螺栓通厚下限
		pNewXingHao->up_lim=pSrcXingHao->up_lim;		//螺栓通厚上限
		pNewXingHao->washer_thick=pSrcXingHao->washer_thick;	//垫圈厚度pXingHao->pad_thick
		pNewXingHao->weight=pSrcXingHao->weight;		//螺栓理论重量
	}
}
/////////////////////////////////////////////////////////
//CLsLibrary 类定义
/////////////////////////////////////////////////////////
long CLsLibrary::m_hDefaultFamily=0;
static CLsLibrary defaultLsLibary;
CLsLibrary::CLsLibrary()
{
	//初始化螺栓系列增长空间
	globalBoltLib.SetHashTableGrowSize(3);
	if(globalBoltLib.GetNodeNum()==0)	//对象初始化时，如无螺栓规格库，则初始化默认库
	{
		CLsFamily *pCommLib=AppendFamily();
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pCommLib->name,"Standard Bolt");
		CLsFamily *pTwoCapLib=AppendFamily();
		strcpy(pTwoCapLib->name,"Double-nut Bolt");
		strcpy(pTwoCapLib->descr,"Double-nut");
#else 
		strcpy(pCommLib->name,"标准螺栓");
		CLsFamily *pTwoCapLib=AppendFamily();
		strcpy(pTwoCapLib->name,"双帽螺栓");
		strcpy(pTwoCapLib->descr,"双帽");
#endif
		pTwoCapLib->m_bDualCap=true;
		CLsFamily *pNailLib=AppendFamily();
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pNailLib->name,"Climbing Bolt");
		strcpy(pNailLib->descr,"Climbing bolt");
#else 
		strcpy(pNailLib->name,"脚钉");
		strcpy(pNailLib->descr,"脚钉");
#endif
		pNailLib->m_bFootNail=true;
		CLsFamily *pFlLib=AppendFamily();
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pFlLib->name,"Flange Bolt");
		strcpy(pFlLib->descr,"Double-nut Double Washer");
#else 
		strcpy(pFlLib->name,"法兰螺栓");
		strcpy(pFlLib->descr,"双帽双垫");
#endif
		pFlLib->m_bDualCap=true;//m_nLsCap=2;	//TODO:未定是为设为true适宜 wjh-2017.11.24
	}
	//初始化螺栓子零件系列
	if(globalLsSubPartLib.GetNodeNum()==0)	//对象初始化时，如无螺栓规格库，则初始化默认库
	{
		CLsSubPartFamily *pLsSubPartFamily=NULL;
		//普通螺母
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"普通螺母");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_COMMON_NUT;
		//普通螺母
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"扣紧螺母");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_FASTEN_NUT;
		//防盗螺母
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"防盗螺母");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_ANTITHEFT_NUT;
		//薄螺母
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"薄螺母");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_THIN_NUT;
		//平垫圈
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"平垫圈");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_COMMON_GASKET;
		//弹簧垫圈
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"弹簧垫圈");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_SPRING_GASKET;
		//防松扣
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"防松扣");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_LOCK_CLASP;
	}
}
//螺栓规格库操作函数
CLsFamily* CLsLibrary::DefaultFamily()
{
	CLsFamily* pDefault=NULL;
	if(m_hDefaultFamily!=0)
		pDefault=globalBoltLib.GetValue(m_hDefaultFamily);
	if(pDefault==NULL)
		pDefault=globalBoltLib.GetFirst();
	return pDefault;
}
CLsFamily* CLsLibrary::GetFirstFamily()
{
	return globalBoltLib.GetFirst();
}
CLsFamily* CLsLibrary::GetNextFamily()
{
	return globalBoltLib.GetNext();
}

CLsFamily* CLsLibrary::FromHandle(long h)
{
	return globalBoltLib.GetValue(h);
}
int CLsLibrary::GetFamilyNum()
{
	return globalBoltLib.GetNodeNum();
}
void CLsLibrary::ClearLibrary()
{
	globalBoltLib.Empty();
}
CLsFamily* CLsLibrary::AppendFamily()
{
	for(long i=1;i<500;i++)
	{
		if(globalBoltLib.GetValue(i))
			continue;
		else
		{
			CLsFamily* pFamily=globalBoltLib.Add(i);
			pFamily->m_hFamily=i;
			return pFamily;
		}
	}
	return NULL;
}
CLsFamily* CLsLibrary::AppendFamily(long hFamiy)
{
	CLsFamily* pFamily=globalBoltLib.GetValue(hFamiy);
	if(pFamily==NULL)
	{
		pFamily=globalBoltLib.Add(hFamiy);
		pFamily->m_hFamily=hFamiy;
	}
	return pFamily;
}
BOOL CLsLibrary::RemoveFamily(long h)
{
	return globalBoltLib.DeleteNode(h);
}
static long _local_prefer_footnail_family=0;
long CLsLibrary::SetFootNailFamily(long hDefaultNailFamily)
{
	CLsFamily* pFamily=globalBoltLib.GetValue(hDefaultNailFamily);
	if(pFamily->m_bFootNail&&!pFamily->m_bBurglarproof)	//只有合法的普通脚钉系列才可以设为默认脚钉系列
		_local_prefer_footnail_family=hDefaultNailFamily;
	return _local_prefer_footnail_family;
}
long CLsLibrary::GetFootNailFamily()
{
	if(_local_prefer_footnail_family==0)
	{
		for(CLsFamily *pFamily=globalBoltLib.GetFirst();pFamily;pFamily=globalBoltLib.GetNext())
		{
			if(pFamily->m_bFootNail&&!pFamily->m_bBurglarproof)
			{
				SetFootNailFamily(pFamily->GetFamilyHandle());
				return pFamily->GetFamilyHandle();
			}
		}
	}
	return _local_prefer_footnail_family;
}
//螺栓规格查询函数
LS_XING_HAO* CLsLibrary::FindLsGuiGe(int d, double L0,long hFamily/*=0*/)
{
	CLsFamily* pFamily=NULL;
	if(hFamily==0)
		pFamily=DefaultFamily();
	else
		pFamily=FromHandle(hFamily);
	if(pFamily)
		return pFamily->FindLsGuiGe(d,L0);
	else
		return NULL;
}
LS_XING_HAO* CLsLibrary::FindLsGuiGeByValidLen(long d, double valid_len,long hFamily/*=0*/)
{
	CLsFamily* pFamily=NULL;
	if(hFamily==0)
		pFamily=DefaultFamily();
	else
		pFamily=FromHandle(hFamily);
	if(pFamily)
		return pFamily->FindLsGuiGeByValidLen(d,valid_len);
	else
		return NULL;
}
//
CLsSubPartFamily* CLsLibrary::AppendLsSubPartFamily(WORD widType/*=0*/)
{
	if(widType>0)
	{
		CLsSubPartFamily* pLsSubPartFamily=globalLsSubPartLib.Add(widType);
		pLsSubPartFamily->m_widType=widType;
		return pLsSubPartFamily;
	}
	for(WORD i=1;i<500;i++)
	{
		if(globalLsSubPartLib.GetValue(i))
			continue;
		else
		{
			CLsSubPartFamily* pLsSubPartFamily=globalLsSubPartLib.Add(i);
			pLsSubPartFamily->m_widType=i;
			return pLsSubPartFamily;
		}
	}
	return NULL;
}
CLsSubPartFamily* CLsLibrary::FromLsSubPartHandle(long h)
{
	return globalLsSubPartLib.GetValue(h);
}
CLsSubPartFamily* CLsLibrary::GetFirstLsSubPartFamily()
{
	return globalLsSubPartLib.GetFirst();
}
CLsSubPartFamily* CLsLibrary::GetNextLsSubPartFamily()
{
	return globalLsSubPartLib.GetNext();
}
BOOL CLsLibrary::RemoveLsSubPartFamily(long h)
{
	return globalLsSubPartLib.DeleteNode(h);
}
void CLsLibrary::ClearLsSubPartLibrary()
{
	globalLsSubPartLib.Empty();
}
int CLsLibrary::GetLsSubPartFamilyNum()
{
	return globalLsSubPartLib.GetNodeNum();
}
/////////////////////////////////////////////////////////
//CBoltLibrary 类定义
/////////////////////////////////////////////////////////
void IBoltLibrary::SetFamilyId(CLsFamily *pFamily,long hFamily)
{
	pFamily->m_hFamily=hFamily;
}
CBoltLibrary::CBoltLibrary()
{	//初始化螺栓系列增长空间
	boltLib.SetHashTableGrowSize(3);
	serialId=0;
}
//螺栓规格库操作函数
CLsFamily* CBoltLibrary::GetFirstFamily()
{
	return boltLib.GetFirst();
}
CLsFamily* CBoltLibrary::GetNextFamily()
{
	return boltLib.GetNext();
}

CLsFamily* CBoltLibrary::FromHandle(long h)
{
	return boltLib.GetValue(h);
}
int CBoltLibrary::GetFamilyNum()
{
	return boltLib.GetNodeNum();
}
void CBoltLibrary::ClearLibrary()
{
	boltLib.Empty();
}
CLsFamily* CBoltLibrary::AppendFamily()
{
	for(long i=1;i<500;i++)
	{
		if(boltLib.GetValue(i))
			continue;
		else
		{
			CLsFamily* pFamily=boltLib.Add(i);
			//pFamily->m_hFamily=i;
			SetFamilyId(pFamily,i);
			return pFamily;
		}
	}
	return NULL;
}
CLsFamily* CBoltLibrary::AppendFamily(long hFamily)
{
	CLsFamily* pFamily=boltLib.GetValue(hFamily);
	if(pFamily==NULL)
	{
		pFamily=boltLib.Add(hFamily);
		SetFamilyId(pFamily,hFamily);
	}
	return pFamily;
}
BOOL CBoltLibrary::RemoveFamily(long h)
{
	return boltLib.DeleteNode(h);
}
//螺栓规格查询函数
LS_XING_HAO* CBoltLibrary::FindLsGuiGe(int d, double L0,long hFamily/*=0*/)
{
	CLsFamily* pFamily=NULL;
	if(hFamily==0)
	{
		for(pFamily=boltLib.GetFirst();pFamily;pFamily=boltLib.GetNext())
		{
			LS_XING_HAO *pLsXingHao=pFamily->FindLsGuiGe(d,L0);
			if(pLsXingHao)
				return pLsXingHao;
		}
	}
	else
	{
		pFamily=FromHandle(hFamily);
		if(pFamily)
			return pFamily->FindLsGuiGe(d,L0);
	}
	return NULL;
}
LS_XING_HAO* CBoltLibrary::FindLsGuiGeByValidLen(long d, double valid_len,long hFamily/*=0*/)
{
	CLsFamily* pFamily=NULL;
	if(hFamily==0)
	{
		for(pFamily=boltLib.GetFirst();pFamily;pFamily=boltLib.GetNext())
		{
			LS_XING_HAO *pLsXingHao=pFamily->FindLsGuiGeByValidLen(d,valid_len);
			if(pLsXingHao)
				return pLsXingHao;
		}
	}
	else
	{
		pFamily=FromHandle(hFamily);
		if(pFamily)
			return pFamily->FindLsGuiGeByValidLen(d,valid_len);
	}
	return NULL;
}

void CBoltLibrary::Copy(IBoltLibrary *pSrcBoltLib)
{
	ClearLibrary();
	for(CLsFamily *pSrcFamily=pSrcBoltLib->GetFirstFamily();pSrcFamily;pSrcFamily=pSrcBoltLib->GetNextFamily())
	{
		CLsFamily *pNewFamily=boltLib.Add(pSrcFamily->GetFamilyHandle());
		SetFamilyId(pNewFamily,pSrcFamily->GetFamilyHandle());
		CopyLsFamily(pNewFamily,pSrcFamily);
	}
}
void CBoltLibrary::CopyLsFamily(CLsFamily* pNewFamily,CLsFamily* pSrcFamily)
{
	if(pNewFamily==NULL || pSrcFamily==NULL)
		return ;
	strcpy(pNewFamily->name,pSrcFamily->name);
	strcpy(pNewFamily->descr,pSrcFamily->descr);
	pNewFamily->m_uiCodeSerial=pSrcFamily->m_uiCodeSerial;
	pNewFamily->m_bAntiLoose=pSrcFamily->m_bAntiLoose;
	pNewFamily->m_bDualCap=pSrcFamily->m_bDualCap;
	pNewFamily->m_bFootNail=pSrcFamily->m_bFootNail;
	pNewFamily->m_bBurglarproof=pSrcFamily->m_bBurglarproof;
	pNewFamily->m_hMappingAntiTheftFamily=pSrcFamily->m_hMappingAntiTheftFamily;
	pNewFamily->m_hMappingAntiLooseFamily=pSrcFamily->m_hMappingAntiLooseFamily;
	for(int i=0;i<8;i++)
	{
		pNewFamily->ls_sub_part[i].idSubPart=pSrcFamily->ls_sub_part[i].idSubPart;
		pNewFamily->ls_sub_part[i].nCount=pSrcFamily->ls_sub_part[i].nCount;
	}
	pNewFamily->CreateFamily(pSrcFamily->GetCount());
	for(int j=0;j<pSrcFamily->GetCount();j++)
	{
		LS_XING_HAO *pSrcXingHao=pSrcFamily->RecordAt(j);
		LS_XING_HAO *pNewXingHao=pNewFamily->RecordAt(j);
		pNewXingHao->grade=pSrcXingHao->grade;			//螺栓级别
		pNewXingHao->d=pSrcXingHao->d;					//螺栓直径
		strcpy(pNewXingHao->guige,pSrcXingHao->guige);	//螺栓规格
		pNewXingHao->valid_len=pSrcXingHao->valid_len;	//有效长
		pNewXingHao->no_thread_len=pSrcXingHao->no_thread_len;	//无扣长
		pNewXingHao->down_lim=pSrcXingHao->down_lim;	//螺栓通厚下限
		pNewXingHao->up_lim=pSrcXingHao->up_lim;		//螺栓通厚上限
		pNewXingHao->washer_thick=pSrcXingHao->washer_thick;	//垫圈厚度pXingHao->pad_thick
		pNewXingHao->weight=pSrcXingHao->weight;		//螺栓理论重量
	}
}
//////////////////////////////////////////////////////////////////////////
// 螺栓库列表
CHashList<CBoltLibrary> globalBoltLibSet;
CHashList<LSSPACEEX> globalBoltSpace;
IBoltLibrary *IBoltLibrary::CreateBoltLibrary()
{
	int iNo=1;
	do{
		if(globalBoltLibSet.GetValue(iNo)!=NULL)
			iNo++;
		else	//找到一个空号
			break;
	}while(true);
	CBoltLibrary* pBoltLib = globalBoltLibSet.Add(iNo);
	pBoltLib->SetSerialId(iNo);
	return pBoltLib;
}
BOOL IBoltLibrary::DestroyBoltLibrary(long serialId)
{
	return globalBoltLibSet.DeleteNode(serialId);
}

//螺栓直径，单排间距，双排间距，端距，连板边距，轧制边距，切角边距，法兰内边距，法兰外边距
int lsspace_N = 4;
LSSPACE_STRU LSSPACE_TABLE[15] = {
	{12,13.5,40, 60,20,20,17,18,30,30},//13.50},
	{16,17.5,50, 80,25,25,21,23,25,20},//17.50},
	{20,21.5,60,100,30,30,26,28,30,25},//21.50},
	{24,25.5,80,120,40,40,31,33,40,30}};//,25.50}};
LSSPACEEX::LSSPACEEX()
{
	M_Diameter=16;		//螺栓直径
	Aperture=17.5;		//构件孔径
	SingleRowSpace=50;	//单排间距
	doubleRowSpace=80;	//双排间距
	EndSpace=25;		//端距
	PlateEdgeSpace=25;	//连板边距
	RollingEdgeSpace=21;//轧制边距
	CutAngleSpace=23;	//切角边距
	InnerFLSpace=25;	//螺栓到法兰内侧钢管壁最小边距
	OutterFLSpace=20;	//螺栓到法兰外边绷最小边距
	WasherEdgeSpace=25;	//垫板边距EndSpace;
}
LSSPACEEX::LSSPACEEX(LSSPACE_STRU& space)
{
	*this=space;
}
LSSPACEEX LSSPACEEX::operator =(const LSSPACE_STRU& space)
{
	M_Diameter		=space.M_Diameter;		//螺栓直径
	Aperture		=space.Aperture;		//构件孔径
	SingleRowSpace	=space.SingleRowSpace;	//单排间距
	doubleRowSpace	=space.doubleRowSpace;	//双排间距
	EndSpace		=space.EndSpace;		//端距
	PlateEdgeSpace	=space.PlateEdgeSpace;	//连板边距
	RollingEdgeSpace=space.RollingEdgeSpace;	//轧制边距
	CutAngleSpace	=space.CutAngleSpace;		//切角边距
	InnerFLSpace	=space.InnerFLSpace;		//螺栓到法兰内侧钢管壁最小边距
	OutterFLSpace	=space.OutterFLSpace;		//螺栓到法兰外边绷最小边距
	WasherEdgeSpace	=space.PlateEdgeSpace;
	return *this;
}
LSSPACEEX::operator LSSPACE_STRU()
{
	LSSPACE_STRU space;
	space.M_Diameter		=M_Diameter;		//螺栓直径
	space.Aperture			=Aperture;		//构件孔径
	space.SingleRowSpace	=SingleRowSpace;	//单排间距
	space.doubleRowSpace	=doubleRowSpace;	//双排间距
	space.EndSpace			=EndSpace;			//端距
	space.PlateEdgeSpace	=PlateEdgeSpace;	//连板边距
	space.RollingEdgeSpace	=RollingEdgeSpace;	//轧制边距
	space.CutAngleSpace		=CutAngleSpace;		//切角边距
	space.InnerFLSpace		=InnerFLSpace;		//螺栓到法兰内侧钢管壁最小边距
	space.OutterFLSpace		=OutterFLSpace;		//螺栓到法兰外边绷最小边距
	return space;
}
BOOL IBoltLibrary::GetLsSpaceEx(long diameter,LSSPACEEX* pSpace)
{
	//垫板边距
	LSSPACEEX* pLsSpace=globalBoltSpace.GetValue(diameter);
	if(pLsSpace)
	{
		*pSpace=*pLsSpace;
		return TRUE;
	}
	else
	{
		LSSPACE_STRU space;
		if(!GetLsSpace(space,diameter))
			return FALSE;
		*pSpace=space;
		return TRUE;
	}
}
BOOL IBoltLibrary::SetLsSpaceEx(long diameter,LSSPACEEX& space)
{
	for(int i=0;i<lsspace_N;i++)
	{
		if(LSSPACE_TABLE[i].M_Diameter==diameter)
		{
			LSSPACE_TABLE[i]=(LSSPACE_STRU)space;
			break;
		}
	}
	return globalBoltSpace.SetValue(diameter,space)!=NULL;
}
