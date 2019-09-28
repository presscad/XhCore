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
{	//������β���D2(������˨Ĭ��Ϊ��˨�ԽǶ�����)
	this->dfShapeDs=dfDe*0.86602540378443864676372317075294;
	return this->_dfShapeDe=dfDe;
}
double LS_SUB_PART::get_dfShapeDe()
{	//������β���D2(������˨Ĭ��Ϊ��˨�ԽǶ�����)
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
//CLsFamily �ඨ��
/////////////////////////////////////////////////////////
CLsFamily::CLsFamily()
{
	m_hFamily=m_hMappingAntiTheftFamily=m_hMappingAntiLooseFamily=0;
	m_nFamilySize=0;
	m_uiCodeSerial=1;
	m_pFamily=NULL;
	m_bDualCap=false;	//��ñ��˨
	m_bBurglarproof=FALSE;	//Ĭ�ϷǷ�����ͨ��˨
	m_bAntiLoose=false;		//Ĭ������ʾ��Ȧ
  	m_bFootNail=FALSE;		//Ĭ��Ϊ��ͨ�ǽŶ���˨
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
		/*�����Ŷ�����ͨ�����ƣ�����ǰ����ֱͬ�����������ϳ��ȽŶ��ĳ��֣�
		��˸�Ϊ�Ŷ�Ҳ��ͨ�����ƣ���ͨ��Χ��0~L�� wjh 2011.11.28
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
	//�޺�����˨���ʱ��������Խϼ���˨��񣬲�������������û�������˨�Ϸ���У��
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
	{	//û�иü�¼���ڿյ�λ����Ӽ�¼
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
		pNewXingHao->grade=pSrcXingHao->grade;			//��˨����
		pNewXingHao->d=pSrcXingHao->d;					//��˨ֱ��
		strcpy(pNewXingHao->guige,pSrcXingHao->guige);	//��˨���
		pNewXingHao->valid_len=pSrcXingHao->valid_len;	//��Ч��
		pNewXingHao->no_thread_len=pSrcXingHao->no_thread_len;	//�޿۳�
		pNewXingHao->down_lim=pSrcXingHao->down_lim;	//��˨ͨ������
		pNewXingHao->up_lim=pSrcXingHao->up_lim;		//��˨ͨ������
		pNewXingHao->washer_thick=pSrcXingHao->washer_thick;	//��Ȧ���pXingHao->pad_thick
		pNewXingHao->weight=pSrcXingHao->weight;		//��˨��������
	}
}
/////////////////////////////////////////////////////////
//CLsLibrary �ඨ��
/////////////////////////////////////////////////////////
long CLsLibrary::m_hDefaultFamily=0;
static CLsLibrary defaultLsLibary;
CLsLibrary::CLsLibrary()
{
	//��ʼ����˨ϵ�������ռ�
	globalBoltLib.SetHashTableGrowSize(3);
	if(globalBoltLib.GetNodeNum()==0)	//�����ʼ��ʱ��������˨���⣬���ʼ��Ĭ�Ͽ�
	{
		CLsFamily *pCommLib=AppendFamily();
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pCommLib->name,"Standard Bolt");
		CLsFamily *pTwoCapLib=AppendFamily();
		strcpy(pTwoCapLib->name,"Double-nut Bolt");
		strcpy(pTwoCapLib->descr,"Double-nut");
#else 
		strcpy(pCommLib->name,"��׼��˨");
		CLsFamily *pTwoCapLib=AppendFamily();
		strcpy(pTwoCapLib->name,"˫ñ��˨");
		strcpy(pTwoCapLib->descr,"˫ñ");
#endif
		pTwoCapLib->m_bDualCap=true;
		CLsFamily *pNailLib=AppendFamily();
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pNailLib->name,"Climbing Bolt");
		strcpy(pNailLib->descr,"Climbing bolt");
#else 
		strcpy(pNailLib->name,"�Ŷ�");
		strcpy(pNailLib->descr,"�Ŷ�");
#endif
		pNailLib->m_bFootNail=true;
		CLsFamily *pFlLib=AppendFamily();
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pFlLib->name,"Flange Bolt");
		strcpy(pFlLib->descr,"Double-nut Double Washer");
#else 
		strcpy(pFlLib->name,"������˨");
		strcpy(pFlLib->descr,"˫ñ˫��");
#endif
		pFlLib->m_bDualCap=true;//m_nLsCap=2;	//TODO:δ����Ϊ��Ϊtrue���� wjh-2017.11.24
	}
	//��ʼ����˨�����ϵ��
	if(globalLsSubPartLib.GetNodeNum()==0)	//�����ʼ��ʱ��������˨���⣬���ʼ��Ĭ�Ͽ�
	{
		CLsSubPartFamily *pLsSubPartFamily=NULL;
		//��ͨ��ĸ
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"��ͨ��ĸ");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_COMMON_NUT;
		//��ͨ��ĸ
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"�۽���ĸ");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_FASTEN_NUT;
		//������ĸ
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"������ĸ");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_ANTITHEFT_NUT;
		//����ĸ
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"����ĸ");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_THIN_NUT;
		//ƽ��Ȧ
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"ƽ��Ȧ");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_COMMON_GASKET;
		//���ɵ�Ȧ
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"���ɵ�Ȧ");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_SPRING_GASKET;
		//���ɿ�
		pLsSubPartFamily=AppendLsSubPartFamily();
		strcpy(pLsSubPartFamily->name,"���ɿ�");
		pLsSubPartFamily->m_widType=CLsSubPartFamily::TYPE_LOCK_CLASP;
	}
}
//��˨�����������
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
	if(pFamily->m_bFootNail&&!pFamily->m_bBurglarproof)	//ֻ�кϷ�����ͨ�Ŷ�ϵ�вſ�����ΪĬ�ϽŶ�ϵ��
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
//��˨����ѯ����
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
//CBoltLibrary �ඨ��
/////////////////////////////////////////////////////////
void IBoltLibrary::SetFamilyId(CLsFamily *pFamily,long hFamily)
{
	pFamily->m_hFamily=hFamily;
}
CBoltLibrary::CBoltLibrary()
{	//��ʼ����˨ϵ�������ռ�
	boltLib.SetHashTableGrowSize(3);
	serialId=0;
}
//��˨�����������
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
//��˨����ѯ����
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
		pNewXingHao->grade=pSrcXingHao->grade;			//��˨����
		pNewXingHao->d=pSrcXingHao->d;					//��˨ֱ��
		strcpy(pNewXingHao->guige,pSrcXingHao->guige);	//��˨���
		pNewXingHao->valid_len=pSrcXingHao->valid_len;	//��Ч��
		pNewXingHao->no_thread_len=pSrcXingHao->no_thread_len;	//�޿۳�
		pNewXingHao->down_lim=pSrcXingHao->down_lim;	//��˨ͨ������
		pNewXingHao->up_lim=pSrcXingHao->up_lim;		//��˨ͨ������
		pNewXingHao->washer_thick=pSrcXingHao->washer_thick;	//��Ȧ���pXingHao->pad_thick
		pNewXingHao->weight=pSrcXingHao->weight;		//��˨��������
	}
}
//////////////////////////////////////////////////////////////////////////
// ��˨���б�
CHashList<CBoltLibrary> globalBoltLibSet;
CHashList<LSSPACEEX> globalBoltSpace;
IBoltLibrary *IBoltLibrary::CreateBoltLibrary()
{
	int iNo=1;
	do{
		if(globalBoltLibSet.GetValue(iNo)!=NULL)
			iNo++;
		else	//�ҵ�һ���պ�
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

//��˨ֱ�������ż�࣬˫�ż�࣬�˾࣬����߾࣬���Ʊ߾࣬�нǱ߾࣬�����ڱ߾࣬������߾�
int lsspace_N = 4;
LSSPACE_STRU LSSPACE_TABLE[15] = {
	{12,13.5,40, 60,20,20,17,18,30,30},//13.50},
	{16,17.5,50, 80,25,25,21,23,25,20},//17.50},
	{20,21.5,60,100,30,30,26,28,30,25},//21.50},
	{24,25.5,80,120,40,40,31,33,40,30}};//,25.50}};
LSSPACEEX::LSSPACEEX()
{
	M_Diameter=16;		//��˨ֱ��
	Aperture=17.5;		//�����׾�
	SingleRowSpace=50;	//���ż��
	doubleRowSpace=80;	//˫�ż��
	EndSpace=25;		//�˾�
	PlateEdgeSpace=25;	//����߾�
	RollingEdgeSpace=21;//���Ʊ߾�
	CutAngleSpace=23;	//�нǱ߾�
	InnerFLSpace=25;	//��˨�������ڲ�ֹܱ���С�߾�
	OutterFLSpace=20;	//��˨��������߱���С�߾�
	WasherEdgeSpace=25;	//���߾�EndSpace;
}
LSSPACEEX::LSSPACEEX(LSSPACE_STRU& space)
{
	*this=space;
}
LSSPACEEX LSSPACEEX::operator =(const LSSPACE_STRU& space)
{
	M_Diameter		=space.M_Diameter;		//��˨ֱ��
	Aperture		=space.Aperture;		//�����׾�
	SingleRowSpace	=space.SingleRowSpace;	//���ż��
	doubleRowSpace	=space.doubleRowSpace;	//˫�ż��
	EndSpace		=space.EndSpace;		//�˾�
	PlateEdgeSpace	=space.PlateEdgeSpace;	//����߾�
	RollingEdgeSpace=space.RollingEdgeSpace;	//���Ʊ߾�
	CutAngleSpace	=space.CutAngleSpace;		//�нǱ߾�
	InnerFLSpace	=space.InnerFLSpace;		//��˨�������ڲ�ֹܱ���С�߾�
	OutterFLSpace	=space.OutterFLSpace;		//��˨��������߱���С�߾�
	WasherEdgeSpace	=space.PlateEdgeSpace;
	return *this;
}
LSSPACEEX::operator LSSPACE_STRU()
{
	LSSPACE_STRU space;
	space.M_Diameter		=M_Diameter;		//��˨ֱ��
	space.Aperture			=Aperture;		//�����׾�
	space.SingleRowSpace	=SingleRowSpace;	//���ż��
	space.doubleRowSpace	=doubleRowSpace;	//˫�ż��
	space.EndSpace			=EndSpace;			//�˾�
	space.PlateEdgeSpace	=PlateEdgeSpace;	//����߾�
	space.RollingEdgeSpace	=RollingEdgeSpace;	//���Ʊ߾�
	space.CutAngleSpace		=CutAngleSpace;		//�нǱ߾�
	space.InnerFLSpace		=InnerFLSpace;		//��˨�������ڲ�ֹܱ���С�߾�
	space.OutterFLSpace		=OutterFLSpace;		//��˨��������߱���С�߾�
	return space;
}
BOOL IBoltLibrary::GetLsSpaceEx(long diameter,LSSPACEEX* pSpace)
{
	//���߾�
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
