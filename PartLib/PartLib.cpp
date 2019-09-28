// PartLib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PartLib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "Buffer.h"
#include "HashTable.h"
#include "XhCharString.h"

#ifndef APP_EMBEDDED_MODULE_ENCRYPT
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif
extern CHashList<CLsFamily> globalBoltLib;
extern CHashList<CLsSubPartFamily> globalLsSubPartLib;
extern CXhSimpleList<LOCAL_BOLT_CODESERIES> globalLocalLsSeries;
//螺栓级别
int LsTypeCount=0;
LS_GRADE ls_table[5];
/*={
	"4.8",200,170,350,
	"5.8",240,210,440,
	"6.8",300,240,530,
	"8.8",400,300,710,
	"10.9",500,380,880,
};*/
//地脚螺栓库
static int g_nAnchorBoltCount=18;
static ANCHOR_BOLT anchorbolts[100]={
	{ 20, 40,16,2, 25,18.0f,0.35f, 32.50f, 30.0f,100,25},
	{ 24, 45,18,2, 30,21.5f,0.35f, 39.55f, 36.0f,115,25},
	{ 27, 50,20,2, 35,24.7f,0.35f, 45.20f, 41.0f,130,30},
	{ 30, 60,26,2, 40,25.6f,0.35f, 50.85f, 46.0f,140,30},
	{ 36, 75,30,2, 45,31.0f,0.35f, 60.79f, 55.0f,175,35},
	{ 42, 90,30,2, 55,34.0f,0.35f, 71.30f, 65.0f,185,35},
	{ 45,100,30,2, 55,36.0f,0.35f, 79.00f, 70.0f,200,40},
	{ 48,100,30,2, 60,38.0f,0.35f, 82.60f, 75.0f,205,45},
	{ 52,110,36,2, 65,42.0f,0.35f, 88.00f, 80.0f,235,55},
	{ 56,120,36,2, 70,45.0f,0.35f, 93.56f, 85.0f,245,55},
	{ 60,135,36,2, 75,48.0f,0.35f,100.00f, 90.0f,260,60},
	{ 64,140,40,2, 80,51.0f,0.35f,104.86f, 95.0f,275,60},
	{ 68,140,40,2, 85,54.4f,0.35f,116.20f,105.0f,285,65},
	{ 72,150,40,2, 90,57.0f,0.35f,116.66f,105.0f,290,65},
	{ 76,160,40,2, 95,60.8f,0.35f,121.80f,110.0f,305,65},
	{ 80,170,46,2,100,64.0f,0.35f,127.46f,115.0f,310,70},
	{ 90,180,46,2,115,72.0f,0.35f,144.07f,130.0f,325,70},
	{100,200,46,2,125,80.0f,0.35f,161.02f,145.0f,335,70},
};
int ANCHOR_BOLT::GetCount(){return g_nAnchorBoltCount;}
int ANCHOR_BOLT::SetCount(int count){return (g_nAnchorBoltCount=count);}
const ANCHOR_BOLT* ANCHOR_BOLT::RecordAt(int i)
{
	if(i<0||i>=g_nAnchorBoltCount)
		return NULL;
	return &anchorbolts[i];
}
const ANCHOR_BOLT* ANCHOR_BOLT::FromBoltD(WORD d)
{	//根据地脚螺栓名义直径查询
	for(int i=0;i<GetCount();i++)
	{
		if(anchorbolts[i].d==d)
			return &anchorbolts[i];
	}
	return NULL;
}
bool ANCHOR_BOLT::SetAnchorBolt(int i,ANCHOR_BOLT& anchorbolt)
{
	if(i<0||i>=18)
		return false;
	anchorbolts[i]=anchorbolt;
	return true;
}
//材质库
static int steelmat_N=0;
static char f_cQ235BriefMark,f_cQ345BriefMark,f_cQ390BriefMark,f_cQ420BriefMark,f_cQ460BriefMark;
static STEELMAT steelmat_tbl[20];
/*={
	{'S',"Q235",16,40,60,2.06e5,160,1.0,
		235,225,215,205,
		215,205,200,190,
		125,120,115,110,
		370,370,370,370},
	{'H',"Q345",16,35,50,2.06e5,200,1.0,
		345,325,295,275,
		310,295,265,250,
		180,170,155,145,
		510,490,470,450},
	{'G',"Q390",16,35,50,2.06e5,220,1.0,
		390,370,350,330,
		350,335,315,295,
		205,190,180,170,
		530,510,480,450},
	{'P',"Q420",16,35,50,2.06e5,220,1.0,	//新贯标中字符为'P'
		420,400,380,360,
		380,360,340,325,
		220,210,195,185,
		560,535,510,480},
	{'T',"Q460",16,35,50,2.06e5,220,1.0,	//新贯标中字符为'T'
		460,440,420,400,
		415,395,380,360,
		240,230,220,210,
		610,585,560,535},
};*/
CSteelMatLibrary::CSteelMatLibrary()
{
	f_cQ235BriefMark='S';
	f_cQ345BriefMark='H';
	f_cQ390BriefMark='G';
	f_cQ420BriefMark='P';
	f_cQ460BriefMark='T';
}
STEELMAT CSteelMatLibrary::RecordAt(int index)
{
	if(index<steelmat_N&&index>=0)
		return steelmat_tbl[index];
	else
		throw "材质库地址越界";
}
int CSteelMatLibrary::GetMaxCount(){return 20;}
int CSteelMatLibrary::GetCount(){return steelmat_N;}
int CSteelMatLibrary::SetCount(int count){return steelmat_N=min(count,GetMaxCount());}
bool CSteelMatLibrary::SetRecordAt(int index,STEELMAT mat)
{
	if(index<min(steelmat_N,GetMaxCount())&&index>=0)
	{
		steelmat_tbl[index]=mat;
		if(stricmp(mat.mark,"Q235")==0)
			f_cQ235BriefMark=mat.cBriefMark;
		else if(stricmp(mat.mark,"Q345")==0)
			f_cQ345BriefMark=mat.cBriefMark;
		else if(stricmp(mat.mark,"Q390")==0)
			f_cQ390BriefMark=mat.cBriefMark;
		else if(stricmp(mat.mark,"Q420")==0)
			f_cQ420BriefMark=mat.cBriefMark;
		else if(stricmp(mat.mark,"Q460")==0)
			f_cQ460BriefMark=mat.cBriefMark;
	}
	else
		return false;
	return true;
}
char CSteelMatLibrary::Q235BriefMark(){return f_cQ235BriefMark;}
char CSteelMatLibrary::Q345BriefMark(){return f_cQ345BriefMark;}
char CSteelMatLibrary::Q390BriefMark(){return f_cQ390BriefMark;}
char CSteelMatLibrary::Q420BriefMark(){return f_cQ420BriefMark;}
char CSteelMatLibrary::Q460BriefMark(){return f_cQ460BriefMark;}
STEELMAT* CSteelMatLibrary::Q235Material()
{
	return QuerySteelMatByBriefMark(f_cQ235BriefMark);
}
STEELMAT* CSteelMatLibrary::Q345Material()
{
	return QuerySteelMatByBriefMark(f_cQ345BriefMark);
}
STEELMAT* CSteelMatLibrary::Q390Material()
{
	return QuerySteelMatByBriefMark(f_cQ390BriefMark);
}
STEELMAT* CSteelMatLibrary::Q420Material()
{
	return QuerySteelMatByBriefMark(f_cQ420BriefMark);
}
STEELMAT* CSteelMatLibrary::Q460Material()
{
	return QuerySteelMatByBriefMark(f_cQ460BriefMark);
}
int CSteelMatLibrary::GetMatIndex(char cMat)
{
	int iMaterial=0;
	for(int i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		if(cMat==steelmat_tbl[i].cBriefMark)
		{
			iMaterial=i;
			break;
		}
	}
	return iMaterial;
}
bool InitPartLibrary(char* libfile,IBoltLibrary* pBoltLib/*=NULL*/)
{
	FILE* fp=fopen(libfile,"rb");
	if(fp==NULL)
		return false;
	long len=0;
	fread(&len,4,1,fp);
	CBuffer buffer;
	char* block=new char[len];
	fread(block,len,1,fp);
	buffer.Write(block,len);
	delete []block;
	int i,j,version,ls_series_n,record_n,nLsSubPart=0,nLsSubPartSeries=0,intVal=false;
	buffer.SeekToBegin();
	buffer.ReadInteger(&version);
	//螺栓系列库数据
	buffer.ReadInteger(&ls_series_n);
	if(pBoltLib)
	{	//在PLB文件中截取所需数据部分
		//读取螺栓系列库
		pBoltLib->ClearLibrary();
		for(i=0;i<ls_series_n;i++)
		{
			CLsFamily* pFamily=pBoltLib->AppendFamily(i+1);
			buffer.ReadString(pFamily->name,17);
			buffer.ReadInteger(&record_n);
			buffer.ReadString(pFamily->descr,51);
			buffer.ReadInteger(&intVal);pFamily->m_bAntiLoose=(intVal>0);
			buffer.ReadInteger(&intVal);pFamily->m_bDualCap=(intVal>=2);
			buffer.ReadInteger(&intVal);pFamily->m_bFootNail=(intVal==TRUE);
			buffer.ReadInteger(&intVal);pFamily->m_bBurglarproof=(intVal==TRUE);
			pFamily->CreateFamily(record_n);
			for(j=0;j<record_n;j++)
			{
				LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
				buffer.ReadDouble(&pXingHao->grade);			//螺栓级别
				buffer.ReadInteger(&pXingHao->d);			//螺栓直径
				buffer.ReadString(pXingHao->guige);			//螺栓规格
				buffer.ReadInteger(&pXingHao->valid_len);	//有效长
				buffer.ReadInteger(&pXingHao->no_thread_len);//无扣长
				buffer.ReadInteger(&pXingHao->down_lim);		//螺栓通厚下限
				buffer.ReadInteger(&pXingHao->up_lim);		//螺栓通厚上限
				buffer.ReadInteger(&pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
				buffer.ReadDouble(&pXingHao->weight);		//螺栓理论重量
			}
		}
		fclose(fp);
		return true;
	}
	CLsLibrary::ClearLibrary();
	for(i=0;i<ls_series_n;i++)
	{
		CLsFamily* pFamily=CLsLibrary::AppendFamily(i+1);
		buffer.ReadString(pFamily->name,17);
		buffer.ReadInteger(&record_n);
		buffer.ReadString(pFamily->descr,51);
		buffer.ReadInteger(&intVal);pFamily->m_bAntiLoose=(intVal>0);
		buffer.ReadInteger(&intVal);pFamily->m_bDualCap=(intVal>=2);
		buffer.ReadInteger(&intVal);pFamily->m_bFootNail=(intVal==TRUE);
		buffer.ReadInteger(&intVal);pFamily->m_bBurglarproof=(intVal==TRUE);
		pFamily->CreateFamily(record_n);
		for(j=0;j<record_n;j++)
		{
			LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
			buffer.ReadDouble(&pXingHao->grade);			//螺栓级别
			buffer.ReadInteger(&pXingHao->d);			//螺栓直径
			buffer.ReadString(pXingHao->guige);			//螺栓规格
			buffer.ReadInteger(&pXingHao->valid_len);	//有效长
			buffer.ReadInteger(&pXingHao->no_thread_len);//无扣长
			buffer.ReadInteger(&pXingHao->down_lim);		//螺栓通厚下限
			buffer.ReadInteger(&pXingHao->up_lim);		//螺栓通厚上限
			buffer.ReadInteger(&pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);		//螺栓理论重量
		}
	}
	if(version>=2)
	{
		//>=V2 写入钢材机械特性数据
		buffer.ReadInteger(&steelmat_N);
		for(i=0;i<steelmat_N;i++)
		{
			buffer.ReadString(steelmat_tbl[i].mark);
			buffer.ReadByte(&steelmat_tbl[i].cBriefMark);
			if(stricmp(steelmat_tbl[i].mark,"Q235")==0)
				f_cQ235BriefMark=steelmat_tbl[i].cBriefMark;
			else if(stricmp(steelmat_tbl[i].mark,"Q345")==0)
				f_cQ345BriefMark=steelmat_tbl[i].cBriefMark;
			else if(stricmp(steelmat_tbl[i].mark,"Q390")==0)
				f_cQ390BriefMark=steelmat_tbl[i].cBriefMark;
			else if(stricmp(steelmat_tbl[i].mark,"Q420")==0)
				f_cQ420BriefMark=steelmat_tbl[i].cBriefMark;
			else if(stricmp(steelmat_tbl[i].mark,"Q460")==0)
				f_cQ460BriefMark=steelmat_tbl[i].cBriefMark;
			steelmat_tbl[i].cPreferQualityLevel=0;	//默认材质质量等级为0表示材质中不显示质量等级
			buffer.ReadDouble(&steelmat_tbl[i].E);
			buffer.ReadDouble(&steelmat_tbl[i].price_coef);
			buffer.Read(steelmat_tbl[i].t_lim,sizeof(int)*3);
			buffer.Read(steelmat_tbl[i].intensity,sizeof(double)*16);
		}
		//>=V2 写入螺栓级别强度特性数据
		buffer.ReadInteger(&LsTypeCount);
		for(i=0;i<LsTypeCount;i++)
		{
			buffer.ReadString(ls_table[i].grade);
			buffer.ReadDouble(&ls_table[i].f);
			buffer.ReadDouble(&ls_table[i].fv);
			buffer.ReadDouble(&ls_table[i].fc);
		}
	}
	if(version>=3)
	{	//>=V3 写入接头规格匹配数据
		buffer.ReadInteger(&g_nJgJointRecordNum);
		for(i=0;i<g_nJgJointRecordNum;i++)
		{
			buffer.ReadDouble(&jg_joint_table[i].fBaseJgWidth);		//基准角钢宽度
			buffer.ReadDouble(&jg_joint_table[i].fBaseJgThick);		//基准角钢厚度
			buffer.ReadDouble(&jg_joint_table[i].fInnerJgWidth);	//内包角钢宽度
			buffer.ReadDouble(&jg_joint_table[i].fInnerJgThick);	//内包角钢厚度
			buffer.ReadDouble(&jg_joint_table[i].fOuterPlateWidth);	//外包钢板宽度
			buffer.ReadDouble(&jg_joint_table[i].fOuterPlateThick);	//外包钢板厚度
			buffer.ReadDouble(&jg_joint_table[i].fOuterJgWidth);	//外包角钢宽度
			buffer.ReadDouble(&jg_joint_table[i].fOuterJgThick);	//外包角钢厚度
		}
	}
	if(version>=4)
	{	//>=V4 写入螺栓零件规格数据,置于后的原因是为了旧版ＴＭＡ也可读取新版的螺栓库文件 wjh-2017.12.11
		int nMappingFamily=0;
		buffer.ReadInteger(&nMappingFamily);
		for(i=0;i<nMappingFamily;i++)
		{
			long hFamily=buffer.ReadInteger();
			CLsFamily* pCurrFamily=CLsLibrary::FromHandle(hFamily);
			buffer.ReadInteger(&pCurrFamily->m_hMappingAntiTheftFamily);	//映射的防盗螺栓系列
			buffer.ReadInteger(&pCurrFamily->m_hMappingAntiLooseFamily);	//映射的防盗螺栓系列
		}
		buffer.ReadInteger(&nLsSubPartSeries);
		CLsLibrary::ClearLsSubPartLibrary();
		for(i=0;i<nLsSubPartSeries;i++)
		{
			CLsSubPartFamily* pLsSubPartFamily=CLsLibrary::AppendLsSubPartFamily();
			buffer.ReadWord(&pLsSubPartFamily->m_widType);
			buffer.ReadString(pLsSubPartFamily->name,17);
			buffer.ReadInteger(&record_n);
			pLsSubPartFamily->CreateLsSubPartFamily(record_n);
			for(j=0;j<record_n;j++)
			{
				LS_SUB_PART* pLsSubPart=pLsSubPartFamily->RecordAt(j);
				buffer.ReadWord(&pLsSubPart->wiLsD);		//螺栓直径
				if(version==4)
				{
					WORD wiThick=0;
					buffer.ReadWord(&wiThick);
					pLsSubPart->thick=wiThick;
				}
				else
				{
					buffer.ReadDouble(&pLsSubPart->thick);		//零件厚度
					buffer.ReadDouble(&pLsSubPart->dfShapeDs);	//零件外形参数D(默认为螺栓对边宽度)
					buffer.ReadDouble(&pLsSubPart->_dfShapeDe);	//零件外形参数D2(六角螺栓默认为螺栓对角顶点宽度)
				}
				buffer.ReadDouble(&pLsSubPart->weight);		//零件重量
			}
		}
		for(i=0;i<ls_series_n;i++)
		{
			buffer.ReadInteger(&intVal);
			CLsFamily* pFamily=CLsLibrary::FromHandle(intVal);
			buffer.ReadInteger(&nLsSubPart);
			for(int j=0;j<nLsSubPart;j++)
			{
				buffer.ReadWord(&pFamily->ls_sub_part[j].idSubPart);	//零件标识号
				buffer.ReadWord(&pFamily->ls_sub_part[j].nCount);		//零件数
			}
		}
	}
	fclose(fp);
	return true;
}
//在指定路径下查找本地螺栓规格系列文件
static bool ReadBoltSeries(const char* sFileName,LOCAL_BOLT_CODESERIES* pLocalBoltSeries=NULL,CLsFamily* pLsFamily=NULL)
{
	FILE* fp=fopen(sFileName,"rt");
	if(fp==NULL)
		return false;
	if(pLocalBoltSeries)
		strcpy(pLocalBoltSeries->m_sFilePath,sFileName);
	int index=0;
	char line_txt[MAX_PATH];
	while(!feof(fp))
	{
		if(fgets(line_txt,MAX_PATH,fp)==NULL)
			break;
		if(strstr(line_txt,"XingHao"))
			break;
		if(strstr(line_txt,"Component")||strstr(line_txt,"Base")
			||strstr(line_txt,"<")||strstr(line_txt,"LsSubPart"))
			continue;
		CXhChar100 sLine=line_txt;
		sLine.Replace('=',' ');
		char *skey=strtok(line_txt,"=,;");
		if(stricmp(skey,"CodeSerial")==0)
		{
			long iCodeSerial=0;
			sscanf(sLine,"%s%d",skey,&iCodeSerial);
			if(pLocalBoltSeries)
				pLocalBoltSeries->m_uiCodeSerial=iCodeSerial;
			if(pLsFamily)
				pLsFamily->m_uiCodeSerial=iCodeSerial;
		}
		else if(stricmp(skey,"CodeName")==0)
		{	
			char sCodeName[100]="";
			sscanf(sLine,"%s%s",skey,sCodeName);
			if(pLocalBoltSeries)
				strcpy(pLocalBoltSeries->m_sCodeName,sCodeName);
		}
		else if(stricmp(skey,"id")==0)
		{
			int hFamily=0;
			sscanf(sLine,"%s%d",skey,&hFamily);
			if(pLocalBoltSeries)
				pLocalBoltSeries->m_hFamily=hFamily;
			if(pLsFamily)
				pLsFamily->SetFamilyHandle(hFamily);
		}
		else if(pLsFamily)
		{
			if(stricmp(skey,"name")==0)
				sscanf(sLine,"%s%s",skey,pLsFamily->name);
			else if(stricmp(skey,"notes")==0)
				sscanf(sLine,"%s%s",skey,pLsFamily->descr);
			else if(stricmp(skey,"bFootNail")==0)
				sscanf(sLine,"%s%d",skey,&pLsFamily->m_bFootNail);
			else
			{
				sscanf(sLine,"%d%d",&pLsFamily->ls_sub_part[index].idSubPart,&pLsFamily->ls_sub_part[index].nCount);
				index++;
			}
		}
	}
	if(pLsFamily)
	{
		//螺栓型号
		int record_n=0;
		fgets(line_txt,MAX_PATH,fp);
		sscanf(line_txt,"%d",&record_n);
		pLsFamily->CreateFamily(record_n);
		for(int iRow=0;iRow<record_n;iRow++)
		{
			LS_XING_HAO *pXingHao=pLsFamily->RecordAt(iRow);
			fgets(line_txt,100,fp);
			sscanf(line_txt,"%lf%d%d%d%d%d%d%lf",&pXingHao->grade,
				&pXingHao->d,&pXingHao->valid_len,
				&pXingHao->no_thread_len,&pXingHao->down_lim,
				&pXingHao->up_lim,&pXingHao->washer_thick,&pXingHao->weight);
			sprintf(pXingHao->guige,"%.1fM%dX%d",pXingHao->grade,pXingHao->d,pXingHao->valid_len);
		}
	}
	fclose(fp);
	return true;
}
	
bool InitLocalBoltSeries(const char* szBoltLibWorkFolder)
{
	globalLocalLsSeries.Empty();
	//
	char sFilePath[MAX_PATH];
	sprintf(sFilePath,"%s\\*.bfs",szBoltLibWorkFolder);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile=FindFirstFile(sFilePath,&FindFileData);
	if(hFindFile==INVALID_HANDLE_VALUE)
		return false;
	do{
		sprintf(sFilePath,"%s\\%s",szBoltLibWorkFolder,FindFileData.cFileName);
		LOCAL_BOLT_CODESERIES xlocalBoltSeries;
		if(ReadBoltSeries(sFilePath,&xlocalBoltSeries))
			globalLocalLsSeries.Append(xlocalBoltSeries);
	}while(FindNextFile(hFindFile,&FindFileData));
	FindClose(hFindFile);
	return true;
}
int GetLocalBoltSeries(long hFamily,LOCAL_BOLT_CODESERIES* series)
{
	LOCAL_BOLT_CODESERIES* pLocalLsSeries=NULL;
	int nCount=0;
	for(pLocalLsSeries=globalLocalLsSeries.EnumObjectFirst();pLocalLsSeries;pLocalLsSeries=globalLocalLsSeries.EnumObjectNext())
	{
		if(hFamily!=pLocalLsSeries->m_hFamily)
			continue;
		if(series)
			series[nCount]=*pLocalLsSeries;
		nCount++;
	}
	return nCount;
}
bool GetLocalLsFamily(long hFamily,char* sCodeName,CLsFamily* pLsFamily)
{
	LOCAL_BOLT_CODESERIES* pLocalLsSeries=NULL;
	for(pLocalLsSeries=globalLocalLsSeries.EnumObjectFirst();pLocalLsSeries;pLocalLsSeries=globalLocalLsSeries.EnumObjectNext())
	{
		if(pLocalLsSeries->m_hFamily==hFamily && stricmp(pLocalLsSeries->m_sCodeName,sCodeName)==0)
			break;
	}
	if(pLocalLsSeries)
		return ReadBoltSeries(pLocalLsSeries->m_sFilePath,NULL,pLsFamily);
	return false;
}
/////////////////////////////////////////////////////////////////////////////
//角钢规格	准距g	准距g1	准距g2	准距g3
WORD zhunju_N = 18;
//角钢准据新规范 wht 11-01-06
ANGLE_G_REC zhunju_table[100]=
{
	40, 20,	0,	0,	0,
	45, 23,	0,	0,	0,
	50, 25,	0,	0,	0,
	56, 28,	0,	0,	0,
	63,	30,	0,	0,	0,
	70,	35,	0,	0,	0,
	75,	38,	0,	0,	0,
	80,	40,	0,	0,	0,
	90,	45,	0,	0,	0,
	100,50,	0,	0,	0,
	110,55, 45, 75, 0,
	125,60,	50,	85,	0,
	140,70,	55,	90,	0,
	160,80,	60,	105,0,
	180,90,	65,	120,0,
	200,100,75,	135,0,
	220,110,85, 145,0,
	250,125,105,165,0,
};
/*角钢准据旧规范
WORD zhunju_table[100][5]=
{
	40, 20,	0,	0,	0,
	//45, 23,	0,	0,	0,		//新规范不合适	WJH--01.09.11
	45, 24,	0,	0,	0,			//改回旧规范
	//50, 25,	0,	0,	0,		//新规范不合适
	50, 28,	0,	0,	0,			//改回旧规范
	//56, 28,	0,	0,	0,		//新规范不合适
	56, 32,	0,	0,	0,			//改回旧规范
	//63,	30,	0,	0,	0,		//新规范不合适
	63,	36,	0,	0,	0,			//改回旧规范
	//70,	35,	0,	0,	0,		//新规范不合适
	70,	40,	0,	0,	0,			//改回旧规范
	75,	40,	0,	0,	0,
	80,	40,	0,	0,	0,
	//90,	45,	33,	64,	0,		//新规范不合适
	90,	50,	33,	64,	0,			//改回旧规范
	100,50,	37,	74,	0,
	110,60, 50, 90, 0,
	125,60,	50,	90,	0,
	140,70,	55,	80,	105,
	160,80,	75,	100,125,
	180,90,	75,	110,145,
	200,100,80,	120,160,
	220,110,85, 145,185,
	250,125,105,165,205,
};*/
//  构件孔径
//const double LS_APERTURE[4] = {13.5,17.5,21.5,25.5};

//肢宽b;肢厚d;内圆半径r;截面面积;理论重量;平行轴惯性矩;平行轴回转半径;平行轴抗弯截面模量;最小轴惯性矩;最小轴回转半径
int jgguige_N = 135;
JG_PARA_TYPE jgguige_table[150] =
{
	40,	3,	  5,	  2.359,   1.852, 3.59   ,1.23, 1.23  , 1.49   , 0.79 ,0.96  ,'L',
	40,	4,	  5,	  3.086,   2.422, 4.6    ,1.22, 1.6   , 1.91   , 0.79 ,1.19  ,'L',
	40,	5,	  5,	  3.791,   2.976, 5.53   ,1.21, 1.96  , 2.3    , 0.78 ,1.39  ,'L',
	45,	3,	  5,	  2.659,   2.088, 5.17   ,1.4 , 1.58  , 2.14   , 0.89 ,1.24  ,'L',
	45,	4,	  5,	  3.486,   2.736, 6.65   ,1.38, 2.05  , 2.75   , 0.89 ,1.54  ,'L',
	45,	5,	  5,	  4.292,   3.369, 8.04   ,1.37, 2.51  , 3.33   , 0.88 ,1.81  ,'L',
	45,	6,	  5,	  5.076,   3.985, 9.33   ,1.36, 2.95  , 3.89   , 0.88 ,2.06  ,'L',
	50,	3,	  5.5,    2.971,   2.332, 7.18   ,1.55, 1.96  , 2.98   , 1    ,1.57  ,'L',
	50,	4,	  5.5,    3.897,   3.059, 9.26   ,1.54, 2.56  , 3.82   , 0.99 ,1.96  ,'L',
	50,	5,	  5.5,    4.803,   3.77 , 11.21  ,1.53, 3.13  , 4.64   , 0.98 ,2.31  ,'L',
	50,	6,	  5.5,    5.688,   4.465, 13.05  ,1.52, 3.68  , 5.42   , 0.98 ,2.63  ,'L',
	56,	3,	  6  ,	  3.343,   2.624, 10.19  ,1.75, 2.48  , 4.24   , 1.13 ,2.02  ,'L',
	56,	4,	  6  ,	  4.39 ,   3.446, 13.18  ,1.73, 3.24  , 5.46   , 1.11 ,2.52  ,'L',
	56,	5,	  6  ,	  5.415,   4.251, 16.02  ,1.72, 3.97  , 6.61   , 1.1  ,2.98  ,'L',
	56,	8,	  6  ,	  8.367,   6.568, 23.63  ,1.68, 6.03  , 9.89   , 1.09 ,4.16  ,'L',
	63,	4,	  7  ,	  4.978,   3.907, 19.03  ,1.96, 4.13  , 7.89   , 1.26 ,3.29  ,'L',
	63,	5,	  7  ,	  6.143,   4.822, 23.17  ,1.94, 5.08  , 9.57   , 1.25 ,3.9   ,'L',
	63,	6,	  7  ,	  7.288,   5.721, 27.12  ,1.93, 6     , 11.2   , 1.24 ,4.46  ,'L',
	63,	8,	  7  ,	  9.515,   7.469, 34.46  ,1.9 , 7.75  , 14.33  , 1.23 ,5.47  ,'L',
	63,	10,	  7  ,	 11.657,   9.151, 41.09  ,1.88, 9.39  , 17.33  , 1.22 ,6.36  ,'L',
	70,	4,	  8  ,	  5.57 ,   4.372, 26.39  ,2.18, 5.14  , 10.99  , 1.4  ,4.17  ,'L',
	70,	5,	  8  ,	  6.875,   5.397, 32.21  ,2.16, 6.32  , 13.34  , 1.39 ,4.95  ,'L',
	70,	6,	  8  ,	  8.16 ,   6.406, 37.77  ,2.15, 7.48  , 15.61  , 1.38 ,5.67  ,'L',
	70,	7,	  8  ,	  9.424,   7.398, 43.09  ,2.14, 8.59  , 17.82  , 1.38 ,6.34  ,'L',
	70,	8,	  8  ,	 10.667,   8.373, 48.17  ,2.12, 9.68  , 19.98  , 1.37 ,6.98  ,'L',
	75,	5,	  9  ,	  7.367,   5.818, 39.97  ,2.33, 7.32  , 16.63  , 1.5  ,5.77  ,'L',
	75,	6,	  9  ,	  8.797,   6.905, 46.95  ,2.31, 8.64  , 19.51  , 1.49 ,6.67  ,'L',
	75,	7,	  9  ,	 10.16 ,   7.976, 53.57  ,2.3 , 9.93  , 22.18  , 1.48 ,7.44  ,'L',
	75,	8,	  9  ,	 11.503,   9.03 , 59.96  ,2.28, 11.2  , 24.86  , 1.47 ,8.19  ,'L',
	75,	10,	  9  ,	 14.126,  11.089, 71.98  ,2.26, 13.64 , 30.05  , 1.46 ,9.56  ,'L',
	80,	5,	  9  ,	  7.912,   6.211, 48.79  ,2.48, 8.34  , 20.25  , 1.60 ,6.66  ,'L',
	80,	6,	  9  ,	  9.397,   7.376, 57.35  ,2.47, 9.87  , 23.72  , 1.59 ,7.65  ,'L',
	80,	7,	  9  ,	 10.86 ,   8.525, 65.58  ,2.46, 11.37 , 27.09  , 1.58 ,8.58  ,'L',
	80,	8,	  9  ,   12.303,   9.658, 73.49  ,2.44, 12.83 , 30.39  , 1.57 ,9.46  ,'L',
	80,	10,	  9  ,	 15.126,  11.874, 88.43  ,2.42, 15.64 , 36.77  , 1.56 ,11.08 ,'L',
	90,	6,	  10,	 10.637,   8.35 , 82.77  ,2.79, 12.61 , 34.28  , 1.8  ,9.95  ,'L',
	90,	7,	  10,	 12.301,   9.656, 94.83  ,2.78, 14.54 , 39.18  , 1.78 ,11.19 ,'L',
	90,	8,	  10,	 13.944,  10.946, 106.47 ,2.76, 16.42 , 43.97  , 1.78 ,12.35 ,'L',
	90,	10,	  10,	 17.167,  13.476, 128.58 ,2.74, 20.07 , 53.26  , 1.76 ,14.52 ,'L',
	90,	12,	  10,	 20.306,  15.94 , 149.22 ,2.71, 23.57 , 62.22  , 1.75 ,16.49 ,'L',
	100,6,	  12,	 11.932,   9.366, 114.95 ,3.1 , 15.68 , 47.92  , 2    ,12.69 ,'L',
	100,7,	  12,	 13.796,  10.83 , 131.86 ,3.09, 18.1  , 54.74  , 1.99 ,14.26 ,'L',
	100,8,	  12,	 15.638,  12.276, 148.24 ,3.08, 20.47 , 61.41  , 1.98 ,15.75 ,'L',
	100,10,	  12,	 19.261,  15.12 , 179.51 ,3.05, 25.06 , 74.35  , 1.96 ,18.54 ,'L',
	100,12,	  12,	 22.8  ,  17.898, 208.9  ,3.03, 29.48 , 86.84  , 1.95 ,21.08 ,'L',
	100,14,	  12,	 26.256,  20.611, 236.53 ,3   , 33.73 , 99     , 1.94 ,23.44 ,'L',
	100,16,	  12,	 29.627,  23.257, 262.53 ,2.98, 37.82 , 110.89 , 1.94 ,25.63 ,'L',
	110,7,	  12,	 15.196,  11.928, 177.16 ,3.41, 22.05 , 73.38  , 2.2  ,17.51 ,'L',
	110,8,	  12,	 17.238,  13.532, 199.46 ,3.4 , 24.95 , 82.42  , 2.19 ,19.39 ,'L',
	110,10,	  12,	 21.261,  16.69 , 242.19 ,3.38, 30.6  , 99.98  , 2.17 ,22.91 ,'L',
	110,12,	  12,	 25.2  ,  19.782, 282.55 ,3.35, 36.05 , 116.93 , 2.15 ,26.15 ,'L',
	110,14,	  12,	 29.056,  22.809, 320.71 ,3.32, 41.31 , 133.4  , 2.14 ,29.14 ,'L',
	125,8,	  14,	 19.75 ,  15.504, 297.03 ,3.88, 32.52 , 123.16 , 2.5  ,25.86 ,'L',
	125,10,	  14,	 24.373,  19.133, 361.67 ,3.85, 39.97 , 149.46 , 2.48 ,30.62 ,'L',
	125,12,	  14,	 28.912,  22.696, 423.16 ,3.83, 41.17 , 174.88 , 2.46 ,35.03 ,'L',
	125,14,	  14,	 33.367,  26.193, 481.65 ,3.8 , 54.16 , 199.57 , 2.45 ,39.13 ,'L',
	140,10,	  14,	 27.373,  21.488, 514.65 ,4.34, 50.58 , 212.04 , 2.78 ,39.2  ,'L',
	140,12,	  14,	 32.512,  25.522, 603.68 ,4.31, 59.8  , 248.57 , 2.76 ,45.02 ,'L',
	140,14,	  14,	 37.567,  29.49 , 688.81 ,4.28, 68.75 , 284.06 , 2.75 ,50.45 ,'L',
	140,16,	  14,	 42.539,  33.393, 770.24 ,4.26, 77.46 , 318.67 , 2.74 ,55.55 ,'L',
	160,10,	  16,	 31.502,  24.729, 779.53 ,4.98, 66.7  , 321.76 , 3.2  ,52.76 ,'L',
	160,12,	  16,	 37.441,  29.391, 916.58 ,4.95, 78.98 , 377.49 , 3.18 ,60.74 ,'L',
	160,14,	  16,	 43.296,  33.987, 1048.36,4.92, 90.95 , 431.7  , 3.16 ,68.24 ,'L',
	160,16,	  16,	 49.067,  38.518, 1175.08,4.89, 102.63, 484.59 , 3.14 ,75.31 ,'L',
	180,12,	  16,	 42.242,  33.159, 1321.35,5.59, 100.82, 542.61 , 3.58 ,78.41 ,'L',
	180,14,	  16,	 48.896,  38.383, 1514.48,5.56, 116.25, 621.53 , 3.56 ,88.38 ,'L',
	180,16,	  16,	 55.467,  43.542, 1700.99,5.54, 131.13, 698.6  , 3.55 ,97.83 ,'L',
	180,18,	  16,	 61.955,  48.634, 1875.12,5.5 , 145.64, 762.01 , 3.51 ,105.14,'L',
	200,14,	  18,	 54.642,  42.894, 2103.55,6.2 , 144.7 , 863.83 , 3.98 ,111.82,'L',
	200,16,	  18,	 62.013,  48.68 , 2366.15,6.18, 163.65, 971.41 , 3.96 ,123.96,'L', 
	200,18,	  18,	 69.301,  54.401, 2620.64,6.15, 182.22, 1076.74, 3.94 ,135.52,'L',
	200,20,	  18,	 76.505,  60.056, 2867.3 ,6.12, 200.42, 1180.04, 3.93 ,146.55,'L',
	200,24,	  18,	 90.661,  71.168, 3338.25,6.07, 236.17, 1381.53, 3.9  ,166.65,'L',
	//新增肢宽220角钢6种，肢宽250角钢8种
	220,16,   21,	 68.664,  53.901, 3187.36,6.81, 199.55, 1310.99, 4.37, 153.81,'L',
	220,18,   21,	 76.752,  60.250, 3534.30,6.79, 222.37, 1453.27, 4.35, 168.29,'L', 
	220,20,   21,	 84.756,  66.533, 3871.49,6.76, 244.77, 1592.90, 4.34, 182.16,'L',
	220,22,   21,	 92.676,  72.751, 4199.23,6.73, 266.78, 1730.10, 4.32, 195.45,'L',
	220,24,   21,	100.512,  78.902, 4517.83,6.70, 288.39, 1865.11, 4.31, 208.21,'L',
	220,26,   21,	108.264,  84.987, 4827.58,6.68, 309.62, 1998.17, 4.30, 220.49,'L',
	250,18,   24,    87.842,  68.956, 5268.22,7.74, 290.12, 2167.41, 4.97, 224.03,'L',
	250,20,   24,    97.045,  76.180, 5779.34,7.72, 319.66, 2376.74, 4.95, 242.85,'L',
	250,22,   24,   106.166,  83.340, 5779.34,7.69, 319.66, 2376.74, 4.94, 242.85,'L',
	250,24,   24,   115.201,  90.433, 6763.93,7.66, 377.34, 2785.19, 4.92, 278.38,'L',
	250,26,   24,   124.154,  97.461, 7238.08,7.63, 406.50, 2984.84, 4.90, 295.19,'L',
	250,28,   24,   133.022, 104.422, 7700.60,7.61, 433.22, 3181.81, 4.89, 311.42,'L',
	250,30,   24,   141.807, 111.318, 8151.80,7.58, 460.51, 3376.34, 4.88, 327.12,'L',
	250,32,   24,   150.508, 118.149, 8592.01,7.56, 487.39, 3568.71, 4.87, 342.33,'L',
	250,35,   24,   163.402, 128.271, 9232.44,7.52, 526.97, 3853.72, 4.86, 364.30,'L',
	//新增肢宽280角钢8种，肢宽300角钢8种 肢宽320角钢7种 肢宽360角钢6种
    280,20,   26,   109.260, 85.769,  8229.11,  8.68, 404.57,3382.15, 5.56,312.22,'L',
	280,22,	  26,   119.580, 93.870,  8949.56,  8.65, 441.71,3677.13, 5.55,335.98,'L',
	280,24,	  26,   129.816, 101.906, 9654.05,  8.62, 478.32,3967.64, 5.53,358.92,'L',
	280,26,	  26,   139.968, 109.875, 10342.96, 8.60, 514.40,4254.00, 5.51,381.09,'L',
	280,28,	  26,   150.037, 117.779, 11016.67, 8.57, 549.97,4536.52, 5.50,402.55,'L',	
	280,30,	  26,   160.022, 125.617, 11675.57, 8.54, 585.05,4815.49, 5.49,423.33,'L',	
	280,32,	  26,   169.922, 133.389, 12320.02, 8.51, 619.64,5091.22, 5.47,443.49,'L',	
	280,35,	  26,   184.617, 144.924, 13260.42, 8.48, 670.65,5499.35, 5.46,472.64,'L',	
	300,20,   28,   117.492, 92.231 , 10199.99, 9.32, 466.79,4195.35, 5.98,364.06,'L',	
	300,22,   28,   128.612, 100.960, 11100.12, 9.29, 509.86,4562.18, 5.96,392.02,'L',	
	300,24,   28,   139.648, 109.624, 11981.60, 9.26, 552.35,4923.63, 5.94,419.06,'L',	
	300,26,   28,   150.600, 118.221, 12844.84, 9.24, 594.27,5280.02, 5.92,445.24,'L',	
	300,28,   28,   161.469, 126.753, 13690.26, 9.21, 635.63,5631.70, 5.91,470.60,'L',	
	300,30,   28,   172.253, 135.219, 14518.26, 9.18, 676.45,5978.99, 5.89,495.20,'L',	
	300,32,   28,   182.954, 143.619, 15329.25, 9.15, 716.73,6322.22, 5.88,519.08,'L',
	300,35,   28,   198.848, 156.096, 16514.72, 9.11, 776.19,6830.12, 5.86,553.64,'L',
	320,22,   28,   137.412, 107.868, 13563.78, 9.94, 582.84,5565.91, 6.36,450.92,'L',
	320,24,   28,   149.248, 117.160, 14650.10, 9.91, 631.67,6009.84, 6.35,482.51,'L',
	320,26,   28,   161.000, 126.385, 15715.30, 9.88, 679.87,6447.73, 6.33,513.14,'L',	
	320,28,   28,   172.669, 135.545, 16759.84, 9.85, 727.48,6879.93, 6.31,542.85,'L',	
	320,30,   28,   184.253, 144.639, 17784.15, 9.82, 774.49,7306.81, 6.30,571.70,'L',	
	320,32,   28,   195.754, 153.667, 18788.67, 9.80, 820.91,7728.71, 6.28,599.73,'L',	
	320,35,   28,   212.848, 167.086, 20259.29, 9.76, 889.50,8352.97, 6.26,640.34,'L',	
	360,24,   30,   168.697, 132.427, 21130.77, 11.19,806.42,8660.20, 7.16,625.08,'L',	
	360,26,   30,   182.049, 142.908, 22689.66, 11.16,868.52,9296.36, 7.15,665.63,'L',	
	360,28,   30,   195.318, 153.324, 24221.57, 11.14,929.93,9924.66, 7.13,705.07,'L',	
	360,30,   30,   208.502, 163.674, 25727.01, 11.11,990.65,10545.52,7.11,743.44,'L',	
	360,32,   30,   221.603, 173.958, 27206.47, 11.08,1050.69,11159.33,7.10,780.79,'L',	
    360,35,   30,   241.097, 189.261, 29378.04, 11.04,1139.52,12067.69,7.07, 835.04,'L',	

	//
	70,5,      8,	 13.750,  10.794,	64.15, 2.160,    0,   64.15, 2.160,     0,'D',
	75,6,      9,	 17.594,  13.810,	93.88, 2.310,    0,   93.88, 2.310,     0,'D',
	80,6,      9,    18.794,  14.752,  114.66, 2.470,    0,  114.66, 2.470,     0,'D',
	90,7,     10,	 24.602,  19.312,  190.13, 2.780,    0,  190.13, 2.780,     0,'D',
	90,8,     10,	 28.888,  21.892,  220.06, 2.760,    0,  220.06, 2.760,     0,'D',
	100,8,    12,	 31.276,  24.552,  296.70, 3.080,    0,  296.70, 3.080,     0,'D',
	110,8,    12,	 34.476,  27.064,  398.54, 3.400,    0,  398.54, 3.400,     0,'D',
	125,10,   14,	 48.746,  38.266,  722.54, 3.850,    0,  722.54, 3.850,     0,'D',
	140,14,   14,	 75.140,  58.980, 2191.08, 5.400,    0, 1376.44, 4.280,     0,'D',
	160,14,   16,	 86.600,  67.974, 3328.90, 6.200,    0, 2096.27, 4.920,     0,'D',
	160,16,   16,	 98.140,  77.036, 3736.08, 6.170,    0, 2346.73, 4.890,     0,'D',
	200,16,   18,	124.020,  97.360, 7526.04, 7.790,    0, 4736.62, 6.180,     0,'D',
	200,20,   18,	153.000, 120.112, 9118.56, 7.720,    0, 5730.52, 6.120,     0,'D',
	200,24,   18,	181.320, 142.336,10583.58, 7.640,    0, 6680.72, 6.070,     0,'D',
	160,16,   18,	196.280, 154.072,86724.43,21.020,    0,86724.43,21.020,     0,'X',
	200,16,   18,	248.040, 194.720,103325.6,20.410,    0,103325.6,20.410,     0,'X',
	200,20,   18,	306.000, 240.224,125479.1,20.250,    0,125479.1,20.250,     0,'X',
	200,24,   18,	362.640, 284.672,146466.5,20.097,    0,146466.5,20.097,     0,'X',  	
};

//规格guige; 高h; 宽b; 厚d; 肢厚t; 内圆半径r; 截面积area; 理论重量; Ix(cm4); Rx(cm); Wx(cm3); Iy(cm4); Ry(cm); Wy(cm3); Z0(cm)
int slotguige_N = 31;
SLOT_PARA_TYPE slotguige_table[100] =
{
	"5",	50,	 37,	4.5,	7.0,	7.0,	6.92,	5.44,	26.0,	1.94,	10.4,	8.3,	1.10,	3.5,	1.35,
	"5a",	50,	 50,	5.0,	0.0,	0.0,	7.00,	5.50,	26.0,	1.94,	10.4,	8.3,	1.10,	3.5,	1.35,	//脚钉板
	"6.3",	63,	 40,	4.8,	7.5,	7.5,	8.45,	6.63,	51.2,	2.46,	16.3,	11.9,	1.19,	4.6,	1.39,
	"8",	80,	 43,	5.0,	8.0,	8.0,	10.24,	8.04,	101.3,	3.14,	25.3,	16.6,	1.27,	5.8,	1.42,
	"10",	100, 48,	5.3,	8.5,	8.5,	12.74,	10.00,	198.3,	3.94,	39.7,	25.6,	1.42,	7.8,	1.52,
	"12.6",	126, 53,	5.5,	9.0,	9.0,	15.69,	12.31,	388.5,	4.98,	61.7,	38.0,	1.56,	10.3,	1.59,
	"14a",	140, 58,	6.0,	9.5,	9.5,	18.51,	14.53,	563.7,	5.52,	80.5,	53.2,	1.70,	13.0,	1.71,
	"14b",	140, 60,	8.0,	9.5,	9.5,	21.31,	16.73,	609.4,	5.35,	87.1,	61.2,	1.69,	14.1,	1.67,
	"16a",	160, 63,	6.5,	10.0,	10.0,	21.95,	17.23,	866.2,	6.28,	108.3,	73.4,	1.83,	16.3,	1.79,
	"16b",	160, 65,	8.5,	10.0,	10.0,	25.15,	19.75,	934.5,	6.10,	116.8,	83.4,	1.82,	17.6,	1.75,
	"18a",	180, 68,	7.0,	10.5,	10.5,	25.69,	20.17,	1272.7,	7.04,	141.4,	98.6,	1.96,	20.0,	1.88,
	"18b",	180, 70,	9.0,	10.5,	10.5,	29.29,	22.99,	1369.9,	6.84,	152.2,	111.0,	1.95,	21.5,	1.84,
	"20a",	200, 73,	7.0,	11.0,	11.0,	28.83,	22.63,	1780.4,	7.86,	178.0,	128.0,	2.11,	24.2,	2.01,
	"20b",	200, 75,	9.0,	11.0,	11.0,	32.83,	25.77,	1913.7,	7.64,	191.4,	143.6,	2.09,	25.9,	1.95,
	"22a",	220, 77,	7.0,	11.5,	11.5,	31.84,	24.99,	2393.9,	8.67,	217.6,	157.8,	2.23,	28.2,	2.10,	
	"22b",	220, 79,	9.0,	11.5,	11.5,	36.24,	28.45,	2571.3,	8.42,	233.8,	176.5,	2.21,	30.1,	2.03,
	"25a",	250, 78,	7.0,	12.0,	12.0,	34.91,	27.40,	3359.1,	9.81,	268.7,	175.9,	2.24,	30.7,	2.07,
	"25b",	250, 80,	9.0,	12.0,	12.0,	39.91,	31.33,	3619.5,	9.52,	289.6,	196.4,	2.22,	32.7,	1.99,
	"25c",	250, 82,	11.0,	12.0,	12.0,	44.91,	35.25,	3880.0,	9.30,	310.4,	215.9,	2.19,	34.6,	1.96,
	"28a",	280, 82,	7.5,	12.5,	12.5,	40.02,	31.42,	4752.5,	10.90,	339.5,	217.9,	2.33,	35.7,	2.09,
	"28b",	280, 84,	9.5,	12.5,	12.5,	45.62,	35.81,	5118.4,	10.59,	365.6,	241.5,	2.30,	37.9,	2.02,
	"28c",	280, 86,	11.5,	12.5,	12.5,	51.22,	40.21,	5484.3,	10.35,	391.7,	264.1,	2.27,	40.0,	1.99,
	"32a",	320, 88,	8.0,	14.0,	14.0,	48.50,	38.07,	7510.6,	12.44,	469.4,	304.7,	2.51,	46.4,	2.24,
	"32b",	320, 90,	10.0,	14.0,	14.0,	54.90,	43.10,	8056.8,	12.11,	503.5,	335.6,	2.47,	49.1,	2.16,
	"32c",	320, 92,	12.0,	14.0,	14.0,	61.30,	48.12,	8602.9,	11.85,	537.7,	365.0,	2.44,	51.6,	2.13,
	"36a",	360, 96,	9.0,	16.0,	16.0,	60.89,	47.80,	11874.1,13.96,	659.7,	455.0,	2.73,	63.6,	2.44,
	"36b",	360, 98,	11.0,	16.0,	16.0,	68.09,	53.45,	12651.7,13.63,	702.9,	496.7,	2.70,	66.9,	2.37,
	"36c",	360,100,	13.0,	16.0,	16.0,	75.29,	59.10,	13429.3,13.36,	746.1,	536.6,	2.67,	70.0,	2.34,
	"40a",	400,100,	10.5,	18.0,	18.0,	75.04,	58.91,	17577.7,15.30,	878.9,	592.0,	2.81,	78.8,	2.49,
	"40b",	400,102,	12.5,	18.0,	18.0,	83.04,	65.19,	18644.4,14.98,	932.2,	640.6,	2.78,	82.6,	2.44,
	"40c",	400,104,	14.5,	18.0,	18.0,	91.04,	71.47,	19711.0,14.71,	985.6,	687.8,	2.75,	86.2,	2.42,
};
//高h; 宽b; 腹板厚度t1; 翼缘厚度t2; 内圆半径r; 截面积area; 理论重量; Ix(cm4); Iy(cm4); Rx(cm); Ry(cm); Wx(cm3); Wy(cm3);
//宽翼缘H钢杆件
int hw_guige_N=21;
HSHAPE_PARA_TYPE hw_guige_table[200] = 
{
	100,	100,	6,		8,	10,	21.90,	17.2,	383,	134,	4.18,	2.47,	76.5,	26.7,
	125,	125,	6.5,	9,	10,	30.31,	23.8,	847,	294,	5.29,	3.11,	136,	47.0,
	150,	150,	7,		10,	13,	40.55,	31.9,	1660,	564,	6.39,	3.73,	221,	75.1,
	175,	175,	7.5,	11,	13,	51.43,	40.3,	2900,	984,	7.50,	4.37,	331,	112,
	200,	200,	8,		12,	16,	64.28,	50.5,	4770,	1600,	8.61,	4.99,	477,	160,
	200,	204,	12,		12,	16,	72.28,	56.7,	5030,	1700,	8.35,	4.85,	503,	167,
	250,	250,	9,		14,	16,	92.18,	72.4,	10800,	3650,	10.8,	6.29,	867,	292,
	250,	255,	14,		14,	16,	104.7,	82.2,	11500,	3880,	10.5,	6.09,	919,	304,
	294,	302,	12,		12,	20,	108.3,	85.0,	17000,	5520,	12.5,	7.14,	1160,	365,
	300,	300,	10,		15,	20,	120.4,	94.5,	20500,	6760,	13.1,	7.49,	1370,	450,
	300,	305,	15,		15,	20,	135.4,	106,	21600,	7100,	12.6,	7.24,	1440,	466,
	344,	348,	10,		16,	20,	146.0,	115,	33300,	11200,	15.1,	8.78,	1940,	646,
	350,	350,	12,		19,	20,	173.9,	137,	40300,	13600,	15.2,	8.84,	2300,	776,
	388,	402,	15,		15,	24,	179.2,	141,	49200,	16300,	16.6,	9.52,	2540,	809,
	394,	398,	11,		18,	24,	187.6,	147,	56400,	18900,	17.3,	10.0,	2860,	951,
	400,	400,	13,		21,	24,	219.5,	172,	66900,	22400,	17.5,	10.1,	3340,	1120,
	400,	408,	21,		21,	24,	251.5,	197,	71100,	23800,	16.8,	9.73,	3560,	1170,
	414,	405,	18,		28,	24,	296.2,	233,	93000,	31000,	17.7,	10.2,	4490,	1530,
	428,	407,	20,		35,	24,	361.4,	284,	119000,	39400,	18.2,	10.4,	5580,	1930,
	458,	417,	30,		50,	24,	529.3,	415,	187000,	60500,	18.8,	10.7,	8180,	2900,
	498,	432,	45,		70,	24,	770.8,	605,	298000,	94400,	19.7,	11.1,	12000,	4370,
};
//中翼缘H钢杆件
int hm_guige_N=12;	
HSHAPE_PARA_TYPE hm_guige_table[200] = 
{
	148,	100,	6,		9,	13,	27.25,	21.4,	1040,	151,	6.17,	2.35,	140,	30.2,
	194,	150,	6,		9,	16,	39.76,	31.2,	2740,	508,	8.30,	3.57,	283,	67.7,
	244,	175,	7,		11,	16,	56.24,	44.1,	6120,	985,	10.4,	4.18,	502,	113,
	294,	200,	8,		12,	20,	73.03,	57.3,	11400,	1600,	12.5,	4.69,	779,	160,
	340,	250,	9,		14,	20,	101.5,	79.7,	21700,	3650,	14.6,	6.00,	1280,	292,
	390,	300,	10,		16,	24,	136.7,	107,	38900,	7210,	16.9,	7.26,	2000,	481,
	440,	300,	11,		18,	24,	157.4,	124,	56100,	8110,	18.9,	7.18,	2550,	541,
	482,	300,	11,		15,	28,	146.4,	115,	60800,	6770,	20.4,	6.80,	2520,	451,
	488,	300,	11,		18,	28,	164.4,	129,	71400,	8120,	20.8,	7.03,	2930,	541,
	582,	300,	12,		17,	28,	174.5,	137,	103000,	7670,	24.3,	6.63,	3530,	511,
	588,	300,	12,		20,	28,	192.5,	151,	118000,	9020,	24.8,	6.85,	4020,	601,
	594,	302,	14,		23,	28,	222.4,	175,	137000,	10600,	24.9,	6.90,	4620,	701,
};
//窄翼缘H钢杆件
int hn_guige_N=32;	
HSHAPE_PARA_TYPE hn_guige_table[200] = 
{
	100,	50,		5,		7,	10,	12.16,	9.54,	192,	14.9,	3.98,	1.11,	38.5,	5.96,
	125,	60,		6,		8,	10,	17.01,	13.3,	417,	29.3,	4.95,	1.31,	66.8,	9.75,
	150,	75,		5,		7,	10,	18.16,	14.3,	679,	49.6,	6.12,	1.65,	90.6,	13.2,
	175,	90,		5,		8,	10,	23.21,	18.2,	1220,	97.6,	7.26,	2.05,	140,	21.7,
	198,	99,		4.5,	7,	13,	23.59,	18.5,	1610,	114,	8.27,	2.20,	163,	23.0,
	200,	100,	5.5,	8,	13,	27.57,	21.7,	1880,	134,	8.25,	2.21,	188,	26.8,
	248,	124,	5,		8,	13,	32.89,	25.8,	3560,	255,	10.4,	2.78,	287,	41.1,
	250,	125,	6,		9,	13,	37.87,	29.7,	4080,	294,	10.4,	2.79,	326,	47.0,
	298,	149,	5.5,	8,	16,	41.55,	32.6,	6460,	443,	12.4,	3.26,	433,	59.4,
	300,	150,	6.5,	9,	16,	47.53,	37.3,	7350,	508,	12.4,	3.27,	490,	67.7,
	346,	174,	6,		9,	16,	53.19,	41.8,	11200,	792,	14.5,	3.86,	649,	91.0,
	350,	175,	7,		11,	16,	63.66,	50.0,	13700,	985,	14.7,	3.93,	782,	113,
	400,	150,	8,		13,	16,	71.12,	55.8,	18800,	734,	16.3,	3.21,	942,	97.9,
	396,	199,	7,		11,	16,	72.16,	56.7,	20000,	1450,	16.7,	4.48,	1010,	145,
	400,	200,	8,		13,	16,	84.12,	66.0,	23700,	1740,	16.8,	4.54,	1190,	174,
	450,	150,	9,		14,	20,	83.41,	65.5,	27100,	793,	18.0,	3.08,	1200,	106,
	446,	199,	8,		12,	20,	84.95,	66.7,	29000,	1580,	18.5,	4.31,	1300,	159,
	450,	200,	9,		14,	20,	97.41,	76.5,	33700,	1870,	18.6,	4.38,	1500,	187,
	500,	150,	10,		16,	20,	98.23,	77.1,	38500,	907,	19.8,	3.04,	1540,	121,
	496,	199,	9,		14,	20,	101.3,	79.5,	41900,	1840,	20.3,	4.27,	1690,	185,
	500,	200,	10,		16,	20,	114.2,	89.6,	47800,	2140,	20.5,	4.33,	1910,	214,
	506,	201,	11,		19,	20,	131.3,	103,	56500,	2580,	20.8,	4.43,	2230,	257,
	596,	199,	10,		15,	24,	121.2,	95.1,	69300,	1980,	23.9,	4.04,	2330,	199,
	600,	200,	11,		17,	24,	135.2,	106,	78200,	2280,	24.1,	4.11,	2610,	228,
	606,	201,	12,		20,	24,	153.3,	120,	91000,	2720,	24.4,	4.21,	3000,	271,
	692,	300,	13,		20,	28,	211.5,	166,	172000,	9020,	28.6,	6.53,	4980,	602,
	700,	300,	13,		24,	28,	235.5,	185,	201000,	10800,	29.3,	6.78,	5760,	722,
	729,	300,	14,		22,	28,	243.4,	191,	254000,	9930,	32.3,	6.39,	6400,	662,
	800,	300,	14,		26,	28,	267.4,	210,	292000,	11700,	33.0,	6.62,	7290,	782,
	890,	299,	15,		23,	28,	270.9,	213,	345000,	10300,	35.7,	6.16,	7760,	688,
	900,	300,	16,		28,	28,	309.8,	243,	411000,	12600,	36.4,	6.39,	9140,	843,
	912,	302,	18,		34,	28,	364.0,	286,	498000,	15700,	37.0,	6.56,	10900,	1040,
};
//等翼缘H钢杆件
int hp_guige_N=19;	
HSHAPE_PARA_TYPE hp_guige_table[200] =
{
	200,	204,	12,		12,	16,	72.28,	56.7,	5030,	1700,	8.35,	4.85,	503,	167,
	244,	252,	11,		11,	16,	82.05,	64.4,	8790,	2940,	10.4,	5.98,	720,	233,
	250,	255,	14,		14,	16,	104.7,	82.2,	11500,	3880,	10.5,	6.09,	919,	304,
	294,	302,	12,		12,	20,	108.3,	85.0,	17000,	5520,	12.5,	7.13,	1150,	365,
	300,	300,	10,		15,	20,	120.4,	94.5,	20500,	6760,	13.1,	7.49,	1370,	450,
	300,	305,	15,		15,	20,	135.4,	106,	21600,	7110,	12.6,	7.24,	1440,	466,
	338,	351,	13,		13,	20,	135.3,	106,	28200,	9380,	14.4,	8.33,	1670,	535,
	344,	354,	16,		16,	20,	166.6,	131,	35300,	11800,	14.6,	8.43,	2050,	669,
	350,	350,	12,		19,	20,	173.9,	137,	40300,	13600,	15.2,	8.84,	2300,	776,
	350,	357,	19,		19,	20,	198.4,	156,	42800,	14400,	14.7,	8.53,	2450,	809,
	388,	402,	15,		15,	24,	179.2,	141,	49200,	16300,	16.6,	9.52,	2540,	809,
	394,	405,	18,		18,	24,	215.2,	169,	59900,	20000,	16.7,	9.63,	3040,	986,
	400,	400,	13,		21,	24,	219.5,	172,	66900,	22400,	17.5,	10.1,	3340,	1120,
	400,	408,	21,		21,	24,	251.5,	197,	71100,	23800,	16.8,	9.73,	3560,	1170,
	414,	405,	18,		28,	24,	296.2,	233,	93000,	31000,	17.7,	10.2,	4490,	1530,
	428,	407,	20,		35,	24,	361.4,	284,	119000,	39400,	18.2,	10.4,	5580,	1930,
	492,	465,	15,		20,	28,	260.5,	204,	118000,	33500,	21.3,	11.4,	4810,	1440,
	502,	465,	15,		25,	28,	307.0,	241,	147000,	41900,	21.9,	11.7,	5860,	1800,
	502,	470,	20,		25,	28,	332.1,	261,	152000,	43300,	21.4,	11.4,	6070,	1840,
};
//GD挂板规格数据
int gd_guige_N=10;
FITTING_GD gxarrGDLib[50]=
{	//size,b,S(预留挂线角钢通厚),M,Ф,H,H1,H2,L,L1
	{"GD-125",16,16,22,24,30.0,30,12,230,112},
	{"GD-215",20,16,24,26,40.0,35,12,250,112},
	{"GD-325",28,16,30,33,40.0,36,12,258,112},
	{"GD-425",32,16,36,39,45.0,36,12,278,130},
	{"GD-505",32,16,36,39,48.0,50,12,278,112},
	{"GD-12" ,16,16,22,24,30.0,35,12,250,112},
	{"GD-21" ,18,16,24,26,35.0,35,12,250,112},
	{"GD-30" ,30,16,30,33,46.5,35,12,264,112},
	{"GD-50" ,34,16,36,44,50.0,40,12,264,112},
	{"GD-60" ,42,16,42,50,50.0,46,12,290,112},
};
//EB挂板规格数据
int eb_guige_N=6;
FITTING_EB gxarrEBLib[50]=
{	//size,b,S(预留挂线角钢通厚),M,Ф,L,H,R
	{"EB-10/21-100",16,16,24,20, 80,100,24},
	{"EB-16/21-100",18,16,24,26, 80,100,32},
	{"EB-16/42-100",18,16,36,26,100,100,32},
	{"EB-21/42-100",20,16,36,26,100,100,32},
	{"EB-21/42-120",20,16,36,26,112,120,32},
	{"EB-32/42-100",28,16,36,33,100,100,40},
};
/*
int tubeguige_N=24;	//钢管类杆件
TUBE_PARA_TYPE tubeguige_table[200] =
{
	 38,  3.5,  3.79,  2.98,   5.70,  1.23,  3.00,
	 50,  6.0,  8.29,  6.51,  20.4 ,  1.57,  8.16,
	 60,  3.5,  6.21,  4.88,  24.88, 2.00,  8.29,
	 60,  8.0, 13.1 , 10.26,  45.2,  1.86,  15.1,
	 70, 10.0, 18.8 , 14.8 ,  87.2,  2.15,  24.9,
	 89,  4.0, 10.68,  8.38, 96.68,  3.01,  21.73,
	 95, 10.0, 26.7 , 20.96, 244.0,  3.02,  51.4,
	114,  4.0, 13.82, 10.85, 209.35, 3.89, 36.73,
	127, 10.0,  36.8, 28.85, 633.0,  4.15,  99.8,
	127, 12.0,  43.4, 34.03, 724.0,  4.09, 114.0,
	159,  6.0,  28.84,22.64, 845.19, 5.41, 106.31,
	159,  8.0,  37.95,29.79,1084.67, 5.35, 136.44,
	159, 10.0,  46.81,36.75,1304.88, 5.28, 164.14,
	140, 10.0,  40.8 ,32.06, 868.0,  4.61, 124.0,
	140, 14.0,  55.4 ,43.50,1114.0,  4.49, 159.0, 
	152, 16.0,  68.4 ,53.66,1608.0,  4.85, 212.0,
	180,  8.0,  43.23,33.93,1602.04, 6.09, 178,
	180, 10.0,  53.41,41.92,1936.01, 6.02, 215.11,
	180, 12.0,  63.33,49.72,2245.84, 5.95, 249.54,
	180, 14.0,  73.0 ,57.31,2533   , 5.89, 281,
	180, 18.0,  91.6 ,71.91,3043.0,  5.76, 338.0,
	219, 10.0,  65.66,51.54,3593.29, 7.4,  328.15,
	219, 12.0,  78.04,61.26,4193.81, 7.33, 383,
	219, 14.0,  90.16,70.78,4758.50, 7.26, 434.57,
};*/

/*
int tubeguige_N=70;	//钢管类杆件
TUBE_PARA_TYPE tubeguige_table[200] =
{
	89,		4.0,	10.681,		8.38,	96.68,		3.009,	21.73,
	114,	4.0,	13.823,		10.85,	209.35,		3.892,	36.73,
	140,	4.0,	17.090,		13.42,	395.47,		4.810,  56.50,
	159,	4.0,	19.478,		15.29,	585.33,		5.482,	73.63,
	168,	4.0,	20.609,		16.18,	693.28,		5.800,	82.53,
	159,	5.0,	24.190,		18.99,	717.88,		5.448,	90.30,
	168,	5.0,	25.190,		20.10,	851.14,		5.766,	101.33,
	194,	5.0,	29.688,		23.31,	1326.54,	6.684,	136.76,
	219,	5.0,	33.615,		26.39,	1925.34,	7.568,	175.83,
	219,	6.0,	40.150,		31.52,	2278.74,	7.534,	208.10,
	273,	6.0,	50.328,		39.51,	4487.08,	9.442,	328.72,
	325,	6.0,	60.130,		47.20,	7651.33,	11.280,	470.85,
	356,	6.0,	65.938,		51.79,	10105.15,	12.376,	567.71,
	325,	7.0,	69.932,		54.90,	8844.02,	11.246,	544.25,
	356,	7.0,	76.749,		60.25,	11689.85,	12.341,	656.73,
	377,	7.0,	81.367,		63.87,	13928.95,	13.084,	738.94,
	406,	7.0,	87.745,		68.88,	17466.68,	14.109,	860.43,
	377,	8.0,	92.740,		72.80,	15791.85,	13.049,	837.76,
	406,	8.0,	100.028,	78.52,	19814.11,	14.074,	976.06,
	426,	8.0,	105.055,	82.47,	22952.91,	14.781,	1077.60,
	457,	8.0,	112.846,	88.58,	28446.36,	15.877,	1244.92,
	480,	8.0,	118.627,	93.12,	33044.61,	16.690,	1376.86,
	426,	10.0,	130.690,	102.59,	28287.25,	14.712,	1328.04,
	457,	10.0,	140.429,	110.24,	35091.32,	15.808,	1535.73,
	480,	10.0,	147.655,	115.91,	40789.65,	16.621,	1699.57,
	508,	10.0,	156.451,	122.81,	48520.25,	17.611,	1910.25,
	529,	10.0,	163.049,	127.99,	54919.07,	18.353,	2076.34,
	559,	10.0,	172.473,	135.39,	65001.14,	19.413,	2325.62,
	480,	12.0,	176.432,	138.50,	48335.27,	16.552,	2013.97,
	508,	12.0,	186.988,	146.79,	57536.08,	17.541,	2265.20,
	529,	12.0,	194.904,	153.00,	65154.84,	18.284,	2463.32,
	559,	12.0,	206.214,	161.88,	77163.53,	19.344,	2760.77,
	610,	12.0,	225.441,	176.97,	100813.69,	21.147,	3305.37,
	630,	12.0,	232.981,	182.89,	111268.00,	21.854,	3532.32,
	660,	12.0,	244.290,	191.77,	128267.04,	22.914,	3886.88,
	610,	14.0,	262.134,	205.78,	116457.18,	21.078,	3818.27,
	630,	14.0,	270.931,	212.68,	128574.35,	21.785,	4081.73,
	660,	14.0,	284.126,	223.04,	148282.33,	22.845,	4493.40,
	711,	14.0,	306.557,	240.65,	186235.06,	24.648,	5238.68,
	762,	14.0,	328.988,	258.26,	230167.94,	26.450,	6041.15,
	711,	16.0,	349.345,	274.24,	211039.81,	24.578,	5936.42,
	762,	16.0,	374.980,	294.36,	260973.30,	26.381,	6849.69,
	813,	16.0,	400.616,	314.48,	318221.72,	28.184,	7828.33,
	864,	16.0,	426.251,	334.61,	383285.16,	29.987,	8872.34,
	914,	16.0,	451.384,	354.34,	455141.80,	31.754,	9959.34,
	813,	18.0,	449.562,	352.91,	355350.03,	28.115,	8741.70,
	864,	18.0,	478.402,	375.55,	428193.47,	29.917,	9911.89,
	914,	18.0,	506.676,	397.74,	508664.77,	31.685,	11130.52,
	965,	18.0,	535.516,	420.38,	600536.21,	33.488,	12446.35,	
	865,	20.0,	530.929,	416.78,	474136.33,	29.884,	10962.69,
	914,	20.0,	561.717,	440.95,	561461.19,	31.616,	12285.80,
	965,	20.0,	593.761,	466.10,	663101.16,	33.418,	13743.03,
	1016,	20.0,	625.805,	491.26,	776323.94,	35.221,	15281.97,
	1067,	20.0,	657.850,	516.41,	901754.61,	37.024,	16902.62,
	1016,	22.0,	687.003,	539.30,	848895.85,	35.152,	16710.55,
	1067,	22.0,	722.252,	566.97,	986333.72,	36.955,	18487.98,	
	1118,	22.0,	757.501,	594.64,	1137860.9,	38.757,	20335.29,
	1168,	22.0,	792.058,	621.77,	1300757.8,	40.525,	22273.25,
	1219,	22.0,	827.307,	649.44,	1482216.7,	42.327,	24318.57,
	1168,	24.0,	862.556,	677.11,	1411693.1,	40.455,	24172.83,
	1219,	24.0,	901.009,	707.29,	1608977.5,	42.258,	26398.32,
	1321,	24.0,	977.915,	767.66,	2057025.8,	45.864,	31143.46,	
	1422,	24.0,	1054.067,	827.44,	2575850.3,	49.434,	36228.56,
	1422,	26.0,	1140.272,	895.11,	2778690.1,	49.365,	39081.44,
	1524,	26.0,	1223.588,	960.52,	3433203.0,	52.970,	45055.16,
	1626,	26.0,	1306.903,	1025.92,4183192.5,	56.576,	51453.78,
	1626,	28.0,	1405.674,	1103.45,4488296.7,	56.507,	55206.60,
	1727,	28.0,	1494.518,	1173.20,5394062.7,	60.077,	62467.43,
	1727,	30.0,	1599.385,	1255.52,5759202.8,	60.007,	66696.04,
	1829,	30.0,	1695.518,	1330.98,6861125.8,	63.613,	75025.98,
};*/
	
int tubeguige_N=130;	//钢管类杆件
TUBE_PARA_TYPE tubeguige_table[200] =
{
	76,	3,	6.88,	5.401,	45.907,	2.583,	12.081,
	76,	4,	9.048,	7.103,	58.811,	2.55,	15.476,
	89,	3,	8.105,	6.363,	75.025,	3.042,	16.859,
	89,	4,	10.681,	8.385,	96.68,	3.009,	21.726,
	102,	3,	9.331,	7.324,	114.416,	3.502,	22.434,
	102,	4,	12.315,	9.667,	148.088,	3.468,	29.037,
	114,	4,	13.823,	10.851,	209.349,	3.892,	36.728,
	127,	4,	15.457,	12.133,	292.613,	4.351,	46.081,
	140,	4,	17.09,	13.416,	395.469,	4.81,	56.496,
	159,	4,	19.478,	15.29,	585.334,	5.482,	73.627,
	159,	5,	24.19,	18.989,	717.876,	5.448,	90.299,
	168,	4,	20.609,	16.178,	693.282,	5.8,	82.534,
	168,	5,	25.604,	20.099,	851.14,	5.766,	101.326,
	180,	4,	22.117,	17.362,	856.805,	6.224,	95.201,
	180,	5,	27.489,	21.579,	1053.17,	6.19,	117.019,
	180,	6,	32.798,	25.747,	1242.725,	6.155,	138.081,
	194,	5,	29.688,	23.305,	1326.536,	6.684,	136.756,
	194,	6,	35.437,	27.818,	1567.209,	6.65,	161.568,
	203,	5,	31.102,	24.415,	1525.114,	7.003,	150.258,
	203,	6,	37.134,	29.15,	1803.07,	6.968,	177.642,
	219,	5,	33.615,	26.388,	1925.344,	7.568,	175.83,
	219,	6,	40.15,	31.517,	2278.738,	7.534,	208.104,
	245,	5,	37.699,	29.594,	2715.514,	8.487,	221.675,
	245,	6,	45.05,	35.365,	3218.685,	8.453,	262.75,
	273,	5,	42.097,	33.046,	3780.815,	9.477,	276.983,
	273,	6,	50.328,	39.508,	4487.084,	9.442,	328.724,
	273,	7,	58.496,	45.92,	5177.302,	9.408,	379.29,
	299,	6,	55.229,	43.355,	5929.2,	10.361,	396.602,
	299,	7,	64.214,	50.408,	6847.878,	10.327,	458.052,
	325,	6,	60.13,	47.202,	7651.328,	11.28,	470.851,
	325,	7,	69.932,	54.897,	8844.019,	11.246,	544.247,
	325,	8,	79.671,	62.542,	10013.921,	11.211,	616.241,
	356,	6,	65.973,	51.789,	10105.153,	12.376,	567.705,
	356,	7,	76.749,	60.248,	11689.849,	12.341,	656.733,
	356,	8,	87.462,	68.658,	13246.985,	12.307,	744.213,
	377,	6,	69.932,	54.897,	12035.01,	13.119,	638.462,
	377,	7,	81.367,	63.873,	13928.954,	13.084,	738.937,
	377,	8,	92.74,	72.801,	15791.852,	13.049,	837.764,
	377,	10,	115.296,	90.508,	19425.867,	12.98,	1030.55,
	406,	6,	75.398,	59.188,	15083.038,	14.144,	743.007,
	406,	7,	87.745,	68.88,	17466.676,	14.109,	860.427,
	406,	8,	100.028,	78.522,	19814.108,	14.074,	976.064,
	406,	9,	112.249,	88.116,	22125.702,	14.04,	1089.936,
	406,	10,	124.407,	97.66,	24401.825,	14.005,	1202.06,
	426,	7,	92.143,	72.332,	20226.521,	14.816,	949.602,
	426,	8,	105.055,	82.468,	22952.911,	14.781,	1077.601,
	426,	9,	117.904,	92.555,	25639.693,	14.747,	1203.741,
	426,	10,	130.69,	102.592,	28287.252,	14.712,	1328.04,
	426,	12,	156.074,	122.518,	33466.237,	14.643,	1571.185,
	457,	7,	98.96,	77.684,	25055.354,	15.912,	1096.514,
	457,	8,	112.846,	88.584,	28446.363,	15.877,	1244.917,
	457,	9,	126.669,	99.435,	31791.548,	15.842,	1391.315,
	457,	10,	140.429,	110.237,	35091.324,	15.808,	1535.725,
	457,	12,	167.761,	131.692,	41556.299,	15.739,	1818.656,
	480,	7,	104.018,	81.654,	29096.212,	16.725,	1212.342,
	480,	8,	118.627,	93.122,	33044.609,	16.69,	1376.859,
	480,	9,	133.172,	104.54,	36942.277,	16.655,	1539.262,
	480,	10,	147.655,	115.909,	40789.654,	16.621,	1699.569,
	480,	12,	176.432,	138.499,	48335.268,	16.552,	2013.969,
	480,	14,	204.958,	160.892,	55684.904,	16.483,	2320.204,
	508,	10,	156.451,	122.814,	48520.246,	17.611,	1910.246,
	508,	11,	171.751,	134.824,	53055.991,	17.576,	2088.819,
	508,	12,	186.988,	146.785,	57536.083,	17.541,	2265.2,
	508,	14,	217.273,	170.559,	66331.136,	17.473,	2611.462,
	529,	10,	163.049,	127.993,	54919.068,	18.353,	2076.335,
	529,	11,	179.008,	140.521,	60067.236,	18.318,	2270.973,
	529,	12,	194.904,	153,	65154.838,	18.284,	2463.321,
	529,	14,	226.509,	177.809,	75150.25,	18.215,	2841.219,
	559,	10,	172.473,	135.392,	65001.142,	19.413,	2325.622,
	559,	11,	189.375,	148.66,	71116.308,	19.379,	2544.412,
	559,	12,	206.214,	161.878,	77163.527,	19.344,	2760.77,
	559,	14,	239.704,	188.167,	89056.15,	19.275,	3186.267,
	584,	11,	198.015,	155.441,	81297.363,	20.262,	2784.156,
	584,	12,	215.639,	169.277,	88230.82,	20.228,	3021.603,
	610,	11,	207,	162.495,	92870.861,	21.181,	3044.946,
	610,	12,	225.441,	176.971,	100813.694,	21.147,	3305.367,
	610,	14,	262.134,	205.776,	116457.18,	21.078,	3818.268,
	610,	16,	298.577,	234.383,	131781.423,	21.009,	4320.702,
	630,	12,	232.981,	182.89,	111267.997,	21.854,	3532.317,
	630,	13,	251.987,	197.81,	119964.151,	21.819,	3808.386,
	630,	14,	270.931,	212.681,	128574.346,	21.785,	4081.725,
	630,	16,	308.63,	242.275,	145539.135,	21.716,	4620.29,
	660,	12,	244.29,	191.768,	128267.036,	22.914,	3886.88,
	660,	13,	264.239,	207.428,	138322.037,	22.88,	4191.577,
	660,	14,	284.126,	223.039,	148282.33,	22.845,	4493.404,
	660,	16,	323.71,	254.112,	167921.173,	22.776,	5088.52,
	711,	13,	285.068,	223.778,	173668.129,	24.682,	4885.179,
	711,	14,	306.557,	240.647,	186235.057,	24.648,	5238.68,
	711,	16,	349.345,	274.236,	211039.813,	24.578,	5936.422,
	711,	18,	391.882,	307.628,	235410.046,	24.51,	6621.942,
	762,	14,	328.988,	258.255,	230167.938,	26.45,	6041.153,
	762,	15,	352.015,	276.332,	245633.746,	26.416,	6447.08,
	762,	16,	374.98,	294.36,	260973.303,	26.381,	6849.693,
	762,	18,	420.722,	330.267,	291276.42,	26.312,	7645.05,
	813,	15,	376.049,	295.198,	299442.362,	28.219,	7366.356,
	813,	16,	400.616,	314.483,	318221.725,	28.184,	7828.333,
	813,	18,	449.562,	352.906,	355350.029,	28.115,	8741.698,
	813,	20,	498.257,	391.131,	391909.33,	28.046,	9641.066,
	864,	16,	426.251,	334.607,	383285.161,	29.987,	8872.342,
	864,	17,	452.358,	355.101,	405820.474,	29.952,	9393.992,
	864,	18,	478.402,	375.545,	428193.468,	29.917,	9911.886,
	864,	20,	530.301,	416.286,	472455.624,	29.848,	10936.473,
	914,	16,	451.384,	354.336,	455141.805,	31.754,	9959.339,
	914,	17,	479.061,	376.063,	481994.518,	31.719,	10546.926,
	914,	18,	506.676,	397.741,	508664.767,	31.685,	11130.52,
	914,	20,	561.717,	440.948,	561461.185,	31.616,	12285.803,
	914,	22,	616.506,	483.957,	613537.665,	31.547,	13425.332,
	965,	18,	535.516,	420.38,	600536.214,	33.488,	12446.346,
	965,	19,	564.67,	443.266,	631919.927,	33.453,	13096.786,
	965,	20,	593.761,	466.102,	663101.165,	33.418,	13743.029,
	965,	21,	622.789,	488.89,	694080.8,	33.384,	14385.094,
	965,	22,	651.755,	511.628,	724859.705,	33.349,	15022.999,
	1016,	20,	625.805,	491.26,	776323.94,	35.221,	15281.97,
	1016,	22,	687.003,	539.3,	848895.85,	35.152,	16710.55,
	1067,	20,	657.85,	516.41,	901754.61,	37.024,	16902.62,
	1067,	22,	722.252,	566.97,	986333.72,	36.955,	18487.98,
	1118,	22,	757.501,	594.64,	1137860.9,	38.757,	20335.29,
	1168,	22,	792.058,	621.77,	1300757.8,	40.525,	22273.25,
	1168,	24,	862.556,	677.11,	1411693.1,	40.455,	24172.83,
	1219,	22,	827.307,	649.44,	1482216.7,	42.327,	24318.57,
	1219,	24,	901.009,	707.29,	1608977.5,	42.258,	26398.32,
	1321,	24,	977.915,	767.66,	2057025.8,	45.864,	31143.46,
	1422,	24,	1054.067,	827.44,	2575850.3,	49.434,	36228.56,
	1422,	26,	1140.272,	895.11,	2778690.1,	49.365,	39081.44,
	1524,	26,	1223.588,	960.52,	3433203,	52.97,	45055.16,
	1626,	26,	1306.903,	1025.92,	4183192.5,	56.576,	51453.78,
	1626,	28,	1405.674,	1103.45,	4488296.7,	56.507,	55206.6,
	1727,	28,	1494.518,	1173.2,	5394062.7,	60.077,	62467.43,
	1727,	30,	1599.385,	1255.52,	5759202.8,	60.007,	66696.04,
	1829,	30,	1695.518,	1330.98,	6861125.8,	63.613,	75025.98,
	/* 因更换法兰库注释掉(2011.10.22版本) wjh-2013.6.5
	89,		4.0,	10.681,		8.38,	96.68,		3.009,	21.73,
	114,	4.0,	13.823,		10.85,	209.35,		3.892,	36.73,
	140,	4.0,	17.090,		13.42,	395.47,		4.810,  56.50,
	159,	4.0,	19.478,		15.29,	585.33,		5.482,	73.63,
	159,	5.0,	24.190,		18.99,	717.88,		5.448,	90.30,
	168,	4.0,	20.609,		16.18,	693.28,		5.800,	82.53,
	168,	5.0,	25.604,		20.10,	851.14,		5.766,	101.33,
	194,	5.0,	29.688,		23.31,	1326.54,	6.684,	136.76,
	194,	6.0,	35.437,		27.82,	1567.21,	6.650,	161.57,	
	219,	5.0,	33.615,		26.39,	1925.34,	7.568,	175.83,
	219,	6.0,	40.150,		31.52,	2278.74,	7.534,	208.10,
	273,	6.0,	50.328,		39.51,	4487.08,	9.442,	328.72,
	273,	7.0,	58.496,		45.92,	5177.30,	9.408,	379.29,
	325,	6.0,	60.130,		47.20,	7651.33,	11.280,	470.85,
	325,	7.0,	69.932,		54.90,	8844.02,	11.246,	544.25,
	325,	8.0,	79.671,		62.54,	10013.92,	11.211,	616.24,
	356,	6.0,	65.938,		51.79,	10105.15,	12.376,	567.71,
	356,	7.0,	76.749,		60.25,	11689.85,	12.341,	656.73,
	356,	8.0,	87.462,		68.66,	13246.99,	12.307,	744.21,
	377,	7.0,	81.367,		63.87,	13928.95,	13.084,	738.94,
	377,	8.0,	92.740,		72.80,	15791.85,	13.049,	837.76,
	377,	10.0,	115.296,	90.51,	19425.87,	12.980,	1030.55,
	406,	7.0,	87.745,		68.88,	17466.68,	14.109,	860.43,
	406,	8.0,	100.028,	78.52,	19814.11,	14.074,	976.06,
	406,	10.0,	12.406,		97.66,	24401.82,	14.005,	1202.06,
	426,	8.0,	105.055,	82.47,	22952.91,	14.781,	1077.60,
	426,	10.0,	130.690,	102.59,	28287.25,	14.712,	1328.04,
	426,	12.0,	156.074,	12.552,	33466.24,	14.643,	1571.18,
	457,	8.0,	112.846,	88.58,	28446.36,	15.877,	1244.92,
	457,	10.0,	140.429,	110.24,	35091.32,	15.808,	1535.73,
	457,	12.0,	167.761,	131.69,	41556.30,	15.739,	1818.66,
	480,	8.0,	118.627,	93.12,	33044.61,	16.690,	1376.86,
	480,	10.0,	147.655,	115.91,	40789.65,	16.621,	1699.57,
	480,	12.0,	176.432,	138.50,	48335.27,	16.552,	2013.97,
	480,	14.0,	204.958,	160.89,	55684.90,	16.483,	2320.20,
	508,	10.0,	156.451,	122.81,	48520.25,	17.611,	1910.25,
	508,	12.0,	186.988,	146.79,	57536.08,	17.541,	2265.20,
	508,	14.0,	217.273,	170.56,	66331.14,	17.473,	2611.46,
	529,	10.0,	163.049,	127.99,	54919.07,	18.353,	2076.34,
	529,	12.0,	194.904,	153.00,	65154.84,	18.284,	2463.32,	
	529,	14.0,	226.509,	177.81,	75150.25,	18.215,	2841.22,
	559,	10.0,	172.473,	135.39,	65001.14,	19.413,	2325.62,
	559,	12.0,	206.214,	161.88,	77163.53,	19.344,	2760.77,
	559,	14.0,	239.704,	188.17,	89056.15,	19.275,	3186.27,
	610,	12.0,	225.441,	176.97,	100813.69,	21.147,	3305.37,
	610,	14.0,	262.134,	205.78,	116457.18,	21.078,	3818.27,
	610,	16.0,	298.577,	234.38,	131781.42,	21.009,	4320.70,
	630,	12.0,	232.981,	182.89,	111268.00,	21.854,	3532.32,
	630,	14.0,	270.931,	212.68,	128574.35,	21.785,	4081.73,
	630,	16.0,	308.630,	242.27,	145539.14,	27.716,	4620.29,
	660,	12.0,	244.290,	191.77,	128267.04,	22.914,	3886.88,
	660,	14.0,	284.126,	223.04,	148282.33,	22.845,	4493.40,
	660,	16.0,	323.710,	254.11,	167921.17,	22.776,	5088.52,
	711,	14.0,	306.557,	240.65,	186235.06,	24.648,	5238.68,
	711,	16.0,	349.345,	274.24,	211039.81,	24.578,	5936.42,
	711,	18.0,	391.882,	307.63,	235410.65,	24.510,	6621.94,
	762,	14.0,	328.988,	258.26,	230167.94,	26.450,	6041.15,
	762,	16.0,	374.980,	294.36,	260973.30,	26.381,	6849.69,
	762,	18.0,	420.772,	330.27,	291276.42,	26.312,	7654.05,
	813,	16.0,	400.616,	314.48,	318221.72,	28.184,	7828.33,
	813,	18.0,	449.562,	352.91,	355350.03,	28.115,	8741.70,
	813,	20.0,	498.257,	391.13,	391909.33,	28.046,	9641.07,
	864,	16.0,	426.251,	334.61,	383285.16,	29.987,	8872.34,
	864,	18.0,	478.402,	375.55,	428193.47,	29.917,	9911.89,	
	864,	20.0,	530.301,	416.29,	472455.62,	29.848,	10936.47,
	914,	16.0,	451.384,	354.34,	455141.80,	31.754,	9959.34,
	914,	18.0,	506.676,	397.74,	508664.77,	31.685,	11130.52,
	914,	20.0,	561.717,	440.95,	561461.19,	31.616,	12285.80,
	914,	22.0,	616.506,	483.96,	613537.67,	31.547,	13425.33,
	965,	18.0,	535.516,	420.38,	600536.21,	33.488,	12446.35,
	965,	20.0,	593.761,	466.10,	663101.16,	33.418,	13743.03,
	965,	22.0,	651.755,	511.63,	724859.71,	33.349,	15023.00,
	1016,	20.0,	625.805,	491.26,	776323.94,	35.221,	15281.97,
	1016,	22.0,	687.003,	539.30,	848895.85,	35.152,	16710.55,
	1067,	20.0,	657.850,	516.41,	901754.61,	37.024,	16902.62,
	1067,	22.0,	722.252,	566.97,	986333.72,	36.955,	18487.98,	
	1118,	22.0,	757.501,	594.64,	1137860.9,	38.757,	20335.29,
	1168,	22.0,	792.058,	621.77,	1300757.8,	40.525,	22273.25,
	1168,	24.0,	862.556,	677.11,	1411693.1,	40.455,	24172.83,
	1219,	22.0,	827.307,	649.44,	1482216.7,	42.327,	24318.57,
	1219,	24.0,	901.009,	707.29,	1608977.5,	42.258,	26398.32,
	1321,	24.0,	977.915,	767.66,	2057025.8,	45.864,	31143.46,	
	1422,	24.0,	1054.067,	827.44,	2575850.3,	49.434,	36228.56,
	1422,	26.0,	1140.272,	895.11,	2778690.1,	49.365,	39081.44,
	1524,	26.0,	1223.588,	960.52,	3433203.0,	52.970,	45055.16,
	1626,	26.0,	1306.903,	1025.92,4183192.5,	56.576,	51453.78,
	1626,	28.0,	1405.674,	1103.45,4488296.7,	56.507,	55206.60,
	1727,	28.0,	1494.518,	1173.20,5394062.7,	60.077,	62467.43,
	1727,	30.0,	1599.385,	1255.52,5759202.8,	60.007,	66696.04,
	1829,	30.0,	1695.518,	1330.98,6861125.8,	63.613,	75025.98,*/
};

int bianguige_N=10;		//扁钢类杆件
BIAN_PARA_TYPE bianguige_table[100] =
{
	40  , 3,  0.59,
	45  , 4,  1.41,
	50  , 4,  1.57,
	50  , 5,  1.96,
	70  , 5,  2.75,
	75  , 6,  3.53,
	75  , 7,  4.12,
	80  , 7,  4.40,
	90  , 7,  4.95,
	100 ,10,  7.85,
};

/*
//级别 直径 规格 有效长 无扣长 通厚下限 通厚上限 理论重量（不带弹簧垫，带弹簧垫，双帽螺栓）
int nLsXingHaoCount = 30+20+71;//当前版本螺栓类型记录数
//根据2010年01月【铁塔制图和构造规定】修正螺栓重量,并新增20种双帽螺栓对应的规格 wht 11-07-29
LS_XING_HAO lsguige_table[150]=
{	//16，20，24螺栓有效长度增加5,同时更新理论重量 wht 09-06-08
	{4.8,12,"4.8M12X30", 30,7,6,12,		0.0640,	0.0640,0.0723},	//此行数据的通厚下限原为8后改为6以适应需要
	{4.8,12,"4.8M12X35", 35,12,13,17,	0.0660,	0.0660,0.0768},
	{4.8,12,"4.8M12X40", 40,15,18,22,	0.0704,	0.0704,0.0813},
	{4.8,12,"4.8M12X45", 45,16,17,22,	0.0747,	0.0747,0.0858}, 
	{4.8,12,"4.8M12X50", 50,11,12,16,	0.0791,	0.0791,0.0903},
	{4.8,12,"4.8M12X55", 55,16,17,22,	0.0834,	0.0834,0.0948}, 	
	{4.8,16,"4.8M16X40", 40,7, 6,12,	0.1442,	0.1442,0.1711},		//此行数据的通厚下限原为8后改为6以适应需要
	{4.8,16,"4.8M16X50", 50,12,13,22,	0.1602,	0.1602,0.1875}, 
	{4.8,16,"4.8M16X60", 60,22,23,32,	0.1762,	0.1762,0.2039}, 
	{4.8,16,"4.8M16X70", 70,32,33,42,	0.1922,	0.1922,0.2203}, 
	{4.8,16,"4.8M16X80", 80,32,33,42,	0.2082,	0.2082,0.2369},
	{6.8,20,"6.8M20X45", 45, 9,8, 15,	0.2701,	0.2701,0.3216}, 
	{6.8,20,"6.8M20X55", 55,15,16,25,	0.2953,	0.2953,0.3475}, 
	{6.8,20,"6.8M20X65", 65,25,26,35,	0.3205,	0.3205,0.3734}, 
	{6.8,20,"6.8M20X75", 75,35,36,45,	0.3457,	0.3457,0.3993},
	{6.8,20,"6.8M20X85", 85,45,46,55,	0.3709,	0.3709,0.4252}, 
	{6.8,20,"6.8M20X95", 95,55,56,65,	0.3961,	0.3961,0.4511}, 
	{6.8,20,"6.8M20X105",105,65,66,75,	0.4213,	0.4213,0.477}, 
	{6.8,20,"6.8M20X60", 60,8, 9,15,	0.3079,	0.3079,0.3605}, 
	{6.8,20,"6.8M20X70", 70,15,16,25,	0.3331,	0.3331,0.3864}, 
	{6.8,20,"6.8M20X80", 80,25,26,35,	0.3583,	0.3583,0.4123}, 
	{6.8,20,"6.8M20X90", 90,35,36,45,	0.3835,	0.3835,0.4381}, 
	{6.8,20,"6.8M20X100",100,45,46,55,	0.4087,	0.4087,0.464}, 
	{6.8,20,"6.8M20X110",110,55,56,65,	0.4339,	0.4339,0.4899},
	{6.8,20,"6.8M20X120",120,65,66,75,	0.4591,	00.4591,0.5158}, 
	{6.8,24,"6.8M24X55",  55,15,16,20,	0.4631,	0.4631,0.5524}, 
	{6.8,24,"6.8M24X65",  65,20,21,30,	0.5000,	0.5000,0.5901}, 
	{6.8,24,"6.8M24X75",  75,30,31,40,	0.5368,	0.5368,0.6278},
	{6.8,24,"6.8M24X85",  85,40,41,50,	0.5737,	0.5737,0.6655},
	{6.8,24,"6.8M24X95",  95,50,51,60,	0.6105,	0.6105,0.7033},
	{6.8,24,"6.8M24X105",105,60,61,70,	0.6473,	0.6473,0.741},
	{6.8,24,"6.8M24X115",115,70,71,80,	0.6842,	0.6842,0.7787}, 
	{6.8,24,"6.8M24X130",130,80,81,95,	0.7375,	0.7375,0.8352}, 
	{6.8,24,"6.8M24X125",125,60,61,70,	0.7209,	0.7209,0.8165}, 
	{6.8,24,"6.8M24X135",135,70,71,80,	0.7578,	0.7578,0.8541}, 
	{6.8,24,"6.8M24X150",150,80,81,95,	0.8111,	0.8111,0.9074},
	{8.8,24,"8.8M24X55",  55,15,16,20,	0.4631,	0.4631,0.5524}, 
	{8.8,24,"8.8M24X65",  65,20,21,30,	0.5000,	0.5000,0.5901}, 
	{8.8,24,"8.8M24X75",  75,30,31,40,	0.5368,	0.5368,0.6278},
	{8.8,24,"8.8M24X85",  85,40,41,50,	0.5737,	0.5737,0.6655},
	{8.8,24,"8.8M24X95",  95,50,51,60,	0.6105,	0.6105,0.7033},
	{8.8,24,"8.8M24X105",105,60,61,70,	0.6473,	0.6473,0.741},
	{8.8,24,"8.8M24X115",115,70,71,80,	0.6842,	0.6842,0.7787}, 
	{8.8,24,"8.8M24X130",130,80,81,95,	0.7375,	0.7375,0.8352}, 
	{8.8,24,"8.8M24X125",125,60,61,70,	0.7209,	0.7209,0.8165}, 
	{8.8,24,"8.8M24X135",135,70,71,80,	0.7578,	0.7578,0.8541}, 
	{8.8,24,"8.8M24X150",150,80,81,95,	0.8111,	0.8111,0.9074},
	{4.8,16,"4.8M16X180",180,120,0, 0,	0.3799},		//脚钉
	{6.8,20,"6.8M20X200",200,120,0, 0,	0.6749},		//脚钉
	{8.8,24,"6.8M24X240",240,120,0, 0,	1.1803},		//脚钉
	{8.8,27,"8.8M27X80",   80, 20, 20, 50,0.5132,0.5394,0.7024},
	{8.8,27,"8.8M27X90",   90, 50, 50, 60,0.5488,0.5750,0.7380},
	{8.8,27,"8.8M27X100", 100, 60, 60, 70,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X110", 110, 70, 70, 80,0.5132,0.5394,0.7024},
	{8.8,27,"8.8M27X120", 120, 80, 80, 90,0.5488,0.5750,0.7380},
	{8.8,27,"8.8M27X130", 130, 90, 90,100,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X140", 140,100,100,110,0.5132,0.5394,0.7024},
	{8.8,27,"8.8M27X150", 150,110,110,120,0.5488,0.5750,0.7380},
	{8.8,27,"8.8M27X160", 160,120,120,130,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X170", 170,130,130,140,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X180", 180,140,141,150,0.5844,0.6106,0.7736},
	{8.8,30,"8.8M30X100", 100,20,20,67,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X110", 110,67,67,77,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X120", 120,77,77,87,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X130", 130,87,87,97,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X140", 140,97,97,107,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X150", 150,107,107,117,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X160", 160,117,117,127,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X170", 170,127,127,137,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X180", 180,137,137,147,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X190", 190,147,147,157,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X200", 200,157,157,167,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X210", 210,167,167,177,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X220", 220,177,177,187,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X110", 110,20,21,71,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X120", 120,71,71,81,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X130", 130,81,81,91,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X140", 140,91,91,101,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X150", 150,101,101,111,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X160", 160,111,111,121,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X170", 170,121,121,131,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X180", 180,131,131,141,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X190", 190,141,141,151,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X200", 200,151,151,161,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X210", 210,161,161,171,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X220", 220,171,171,181,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X230", 230,181,181,191,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X240", 240,191,191,201,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X130", 130,75,75,85,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X140", 140,85,85,95,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X150", 150,95,95,105,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X160", 160,105,105,115,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X170", 170,115,115,125,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X180", 180,125,125,135,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X190", 190,135,135,145,0.5132,0.5394,0.7024},
	{8.8,39,"8.8M39X200", 200,145,145,155,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X150", 150, 30, 31,105,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X160", 160,105,105,115,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X170", 170,115,115,125,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X180", 180,125,125,135,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X190", 190,135,135,145,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X200", 200,145,145,155,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X210", 210,155,155,165,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X220", 220,165,165,175,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X230", 230,175,175,185,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X240", 240,185,185,195,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X250", 250,195,195,205,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X150", 150, 40, 41, 97,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X160", 160, 97, 97,107,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X170", 170,107,107,117,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X180", 180,117,117,127,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X190", 190,127,127,137,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X200", 200,137,137,147,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X210", 210,147,147,157,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X220", 220,157,157,167,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X230", 230,167,167,177,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X240", 240,177,177,187,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X250", 250,187,187,197,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X260", 260,197,197,207,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X280", 280,217,217,227,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X300", 300,227,227,237,0.5132,0.5394,0.7024},
};

	旧规格表　wjh 2009-06-08
//级别 直径 规格 有效长 无扣长 通厚下限 通厚上限 螺帽直径 螺帽高 理论重量（不带弹簧垫，带弹簧垫，双帽螺栓）
LS_XING_HAO lsguige_table[100]=
{
	{4.8,12,"4.8M12X30",  30, 7, 6,12,21.9,8.0,0.0594,0.0640,0.0853},	//此行数据的通厚下限原为8后改为6以适应需要
	{4.8,12,"4.8M12X35",  35,12,13,17,21.9,8.0,0.0633,0.0679,0.0902},
	{4.8,12,"4.8M12X40",  40,15,18,22,21.9,8.0,0.0673,0.0719,0.0946},
	{4.8,16,"4.8M16X35",  35, 7, 6,12,27.7,10.0,0.1248,0.1326,0.1741},//此行数据的通厚下限原为8后改为6以适应需要
	{4.8,16,"4.8M16X45",  45,12,13,22,27.7,10.0,0.1381,0.1459,0.1896},
	{4.8,16,"4.8M16X55",  55,22,23,32,27.7,10.0,0.1536,0.1614,0.2054},
	{4.8,16,"4.8M16X65",  65,32,33,42,27.7,10.0,0.1694,0.1772,0.2212},
	{6.8,20,"6.8M20X40",  40, 9,10,15,34.6,13.0,0.2302,0.2454,0.3304},
	{6.8,20,"6.8M20X50",  50,15,16,25,34.6,13.0,0.2510,0.2662,0.3550},
	{6.8,20,"6.8M20X60",  60,25,26,35,34.6,13.0,0.2737,0.2889,0.3796},
	{6.8,20,"6.8M20X70",  70,35,36,45,34.6,13.0,0.2983,0.3135,0.4042},
	{6.8,20,"6.8M20X80",  80,45,46,55,34.6,13.0,0.3229,0.3381,0.4288},
	{6.8,20,"6.8M20X90",  90,55,56,65,34.6,13.0,0.3475,0.3627,0.4537},
	{6.8,20,"6.8M20X100",100,65,66,75,34.6,13.0,0.3721,0.3873,0.4785},
	{6.8,24,"6.8M24X50",  50,15,16,20,41.6,15.0,0.4148,0.4410,0.5956},
	{6.8,24,"6.8M24X60",  60,20,21,30,41.6,15.0,0.4448,0.4710,0.6312},
	{6.8,24,"6.8M24X70",  70,30,31,40,41.6,15.0,0.4776,0.5038,0.6668},
	{6.8,24,"6.8M24X80",  80,40,41,50,41.6,15.0,0.5132,0.5394,0.7024},
	{6.8,24,"6.8M24X90",  90,50,51,60,41.6,15.0,0.5488,0.5750,0.7380},
	{6.8,24,"6.8M24X100",100,60,61,70,41.6,15.0,0.5844,0.6106,0.7736},
	{8.8,24,"8.8M24X50",  50,15,16,20,41.6,15.0,0.4148,0.4410,0.5956},
	{8.8,24,"8.8M24X60",  60,20,21,30,41.6,15.0,0.4448,0.4710,0.6312},
	{8.8,24,"8.8M24X70",  70,30,31,40,41.6,15.0,0.4776,0.5038,0.6668},
	{8.8,24,"8.8M24X80",  80,40,41,50,41.6,15.0,0.5132,0.5394,0.7024},
	{8.8,24,"8.8M24X90",  90,50,51,60,41.6,15.0,0.5488,0.5750,0.7380},
	{8.8,24,"8.8M24X100",100,60,61,70,41.6,15.0,0.5844,0.6106,0.7736},
	{4.8,16,"4.8M16X180",180,120,0, 0,41.6,15.0,0.3799},		//脚钉
	{6.8,20,"6.8M20X200",200,120,0, 0,41.6,15.0,0.6749},		//脚钉
	{8.8,24,"6.8M24X240",240,120,0, 0,41.6,15.0,1.1803},		//脚钉
	{8.8,27,"8.8M27X80",  80,20,20,50,47.2,22,0.5132,0.5394,0.7024},
	{8.8,27,"8.8M27X90",  90,50,50,60,47.2,22,0.5488,0.5750,0.7380},
	{8.8,27,"8.8M27X100", 100,60,60,70,47.2,22,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X110", 110,70,70,80,47.2,22,0.5132,0.5394,0.7024},
	{8.8,27,"8.8M27X120", 120,80,80,90,47.2,22,0.5488,0.5750,0.7380},
	{8.8,27,"8.8M27X130", 130,90,90,100,47.2,22,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X140", 140,100,100,110,47.2,22,0.5132,0.5394,0.7024},
	{8.8,27,"8.8M27X150", 150,110,110,120,47.2,22,0.5488,0.5750,0.7380},
	{8.8,27,"8.8M27X160", 160,120,120,130,47.2,22,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X170", 170,130,130,140,47.2,22,0.5844,0.6106,0.7736},
	{8.8,27,"8.8M27X180", 180,140,141,150,47.2,22,0.5844,0.6106,0.7736},
	{8.8,30,"8.8M30X100", 100,20,20,67,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X110", 110,67,67,77,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X120", 120,77,77,87,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X130", 130,87,87,97,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X140", 140,97,97,107,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X150", 150,107,107,117,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X160", 160,117,117,127,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X170", 170,127,127,137,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X180", 180,137,137,147,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X190", 190,147,147,157,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X200", 200,157,157,167,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X210", 210,167,167,177,52.8,24,0.5132,0.5394,0.7024},
	{8.8,30,"8.8M30X220", 220,177,177,187,52.8,24,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X110", 110,20,21,71,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X120", 120,71,71,81,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X130", 130,81,81,91,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X140", 140,91,91,101,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X150", 150,101,101,111,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X160", 160,111,111,121,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X170", 170,121,121,131,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X180", 180,131,131,141,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X190", 190,141,141,151,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X200", 200,151,151,161,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X210", 210,161,161,171,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X220", 220,171,171,181,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X230", 230,181,181,191,62.7,28,0.5132,0.5394,0.7024},
	{8.8,36,"8.8M36X240", 240,191,191,201,62.7,28,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X150", 150,30,31,105,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X160", 160,105,105,115,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X170", 170,115,115,125,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X180", 180,125,125,135,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X190", 190,135,135,145,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X200", 200,145,145,155,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X210", 210,155,155,165,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X220", 220,165,165,175,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X230", 230,175,175,185,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X240", 240,185,185,195,74.0,32,0.5132,0.5394,0.7024},
	{8.8,42,"8.8M42X250", 250,195,195,205,74.0,32,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X150", 150,40,41,97,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X160", 160,97,97,107,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X170", 170,107,107,117,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X180", 180,117,117,127,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X190", 190,127,127,137,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X200", 200,137,137,147,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X210", 210,147,147,157,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X220", 220,157,157,167,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X230", 230,167,167,177,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X240", 240,177,177,187,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X250", 250,187,187,197,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X260", 260,197,197,207,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X280", 280,217,217,227,86.0,38,0.5132,0.5394,0.7024},
	{8.8,48,"8.8M48X300", 300,227,227,237,86.0,38,0.5132,0.5394,0.7024},
};*/
//材质库
double STEELMAT::fy(double t)
{	//屈服强度值(t为厚度或直径mm)
	for(int i=0;i<3;i++)
	{
		if(t<=t_lim[i])
			return intensity[i];
	}
	return intensity[3];
}
double STEELMAT::f(double t)
{	//抗拉、压、弯强度设计值(t为厚度或直径mm)
	for(int i=0;i<3;i++)
	{
		if(t<=t_lim[i])
			return intensity[4+i];
	}
	return intensity[7];
}
double STEELMAT::fv(double t)
{	//抗剪强度设计值(t为厚度或直径mm)
	for(int i=0;i<3;i++)
	{
		if(t<=t_lim[i])
			return intensity[8+i];
	}
	return intensity[11];
}
double STEELMAT::fc(double t)
{	//孔壁承压强度设计值(t为厚度或直径mm)
	for(int i=0;i<3;i++)
	{
		if(t<=t_lim[i])
			return intensity[12+i];
	}
	return intensity[15];
}

LS_GRADE *FindLsMatType(char *grade)
{
	for(int i=0;i<LsTypeCount;i++)
	{
		if(strcmp(ls_table[i].grade,grade)==0)
			return &ls_table[i];
	}
	return NULL;
}
//此外还有M22:16 M33:25 M39:25不常见螺栓规格对应的板厚为录入 来自青海院 刘福学
DIJIAOBAN_STRU diban_weld_table[12] = 
{
	{"M20",25, 75, 40,30,30,25,10,16,},
	{"M24",30, 75, 45,30,30,25,10,16,},
	{"M27",35, 90, 55,30,30,25,10,20,},
	{"M30",40, 90, 60,30,30,25,10,20,},
	{"M36",45,110, 70,30,30,25,10,25,},
	{"M42",55,120, 85,30,30,25,20,30,},
	{"M45",60,130, 90,30,30,25,20,32,},
	{"M48",60,130, 90,30,30,25,20,32,},
	{"M52",65,130,100,30,30,25,20,36,},
	{"M56",70,150,105,30,30,30,20,36,},
	{"M60",75,150,115,30,30,30,20,40,},
	{"M64",80,150,120,30,30,30,20,40,},
};
DIJIAOBAN_STRU diban_lslj_table[12] = 
{
	{"M20",25, 80, 40,30,30,25,10,16,},
	{"M24",30, 80, 45,30,30,25,10,16,},
	{"M27",35,100, 55,30,30,25,10,20,},
	{"M30",40,100, 60,30,30,25,10,20,},
	{"M36",45,120, 70,30,30,25,10,25,},
	{"M42",55,130, 85,30,30,25,20,30,},
	{"M45",60,140, 90,30,30,25,20,32,},
	{"M48",60,140, 90,30,30,25,20,32,},
	{"M52",65,140,100,30,30,25,20,36,},
	{"M56",70,160,105,30,30,30,20,36,},
	{"M60",75,160,115,30,30,30,20,40,},
	{"M64",80,170,120,30,30,30,20,40,},
};
//槽型插板类型库
int rollend_param_N=169;
INSERT_PLATE_PARAM rollend_param_table[200] =
{	//d t cTubeMat iLevel codeName plateMat ...
	//更新时间 wht 2015-12-08
	{76,3,'H',5,"C0703S5","","","",'S',140,80,140,40,25,25,6,1.45068,0,0,1,2,2,1,16,115,-30,0,60},
	{76,4,'H',5,"C0704S5","","","",'S',140,80,140,40,25,25,6,1.45068,0,0,1,2,2,1,16,115,-30,0,60},
	{89,3,'H',5,"C0803S5","","","",'S',140,80,150,40,25,25,6,1.52,0,0,1,2,2,1,16,115,-30,60,60},
	{89,4,'H',5,"C0804S5","","","",'S',160,90,150,40,30,30,6,1.73,0,0,1,2,2,1,20,130,-30,0,60},
	{102,3,'H',5,"C1003S5","","","",'S',170,100,160,40,30,30,6,1.92,0,0,1,2,2,1,20,140,-35,0,70},
	{102,4,'H',5,"C1004S5","","","",'S',190,120,160,40,30,30,8,2.86,0,0,1,2,2,1,20,160,-35,0,70},
	{114,4,'H',5,"C1104S5","","","",'S',180,110,170,40,30,30,8,2.83,0,0,1,2,2,1,20,150,-35,0,70},
	{127,4,'H',5,"C1204S5","","","",'S',240,120,190,40,30,30,8,4.07,0,0,2,2,2,1,20,160,-40,50,80,1,1,1,20,210,0,0,0},
	{140,4,'H',5,"C1404S5","","","",'S',250,130,215,50,30,30,8,4.95,0,0,2,2,2,1,20,170,-40,50,80,1,1,1,20,220,0,0,0},
	{159,4,'H',5,"C1504S5","","","",'S',260,140,240,60,30,30,8,5.88,0,0,2,2,2,1,20,180,-45,0,90,1,1,1,20,230,0,0,0},
	{159,5,'H',5,"C1505S5","","","",'S',270,140,250,60,30,30,8,6.27,0,0,2,4,2,2,20,180,-60,60,120,1,1,1,20,210,0,0,0},
	{168,4,'H',5,"C1604S5","","","",'S',270,150,250,60,30,30,8,6.27,0,0,2,2,2,1,20,190,-60,0,120,1,1,1,20,240,0,0,0},
	{168,5,'H',5,"C1605S5","","","",'S',280,150,260,60,30,30,8,6.68,0,0,2,4,2,2,20,190,-70,60,140,1,1,1,20,220,0,0,0},
	{180,4,'H',5,"C1804S5","","","",'S',280,150,280,60,30,30,8,7.03,0,0,2,4,2,2,20,190,-70,60,140,1,1,1,20,220,0,0,0},
	{180,5,'H',5,"C1805S5","","","",'S',280,150,280,60,30,30,8,7.03,0,0,2,4,2,2,20,190,-70,60,140,1,1,1,20,220,0,0,0},
	{194,5,'H',5,"C1905S5","","","",'S',300,170,290,70,30,30,8,8.1012,0,0,2,4,2,2,20,210,-80,60,160,1,1,1,20,240,0,0,0},
	{194,6,'H',5,"C1906S5","","","",'S',340,170,290,70,40,40,10,11.4767,0,0,2,4,2,2,24,220,-80,80,160,1,1,1,24,250,0,0,0},
	{203,5,'H',5,"C2005S5","","","",'S',310,180,310,70,30,30,8,8.7606,0,0,2,4,2,2,20,220,-80,60,160,1,1,1,20,250,0,0,0},
	{203,6,'H',5,"C2006S5","","","",'S',350,180,310,70,40,40,10,12.36375,0,0,2,4,2,2,24,230,-80,80,160,1,2,2,24,270,0,0,0},
	{219,5,'H',5,"C2105S5","","","",'S',360,190,330,80,40,40,8,11.07792,0,0,2,4,2,2,24,240,-90,80,180,1,1,1,24,280,0,0,0},
	{219,6,'H',5,"C2106S5","","","",'S',360,190,330,80,40,40,10,13.8474,0,0,2,4,2,2,24,240,-90,80,180,1,1,1,24,280,0,0,0},
	{245,5,'H',5,"C2405S5","","","",'S',380,210,370,80,40,40,8,12.64792,0,0,2,4,2,2,24,260,-90,80,180,1,1,1,24,300,0,0,0},
	{245,6,'H',5,"C2406S5","","","",'S',380,210,370,80,40,40,10,15.8099,0,0,2,4,2,2,24,260,-90,80,180,1,1,1,24,300,0,0,0},
	{273,5,'H',5,"C2705S5","","","",'S',430,220,400,100,40,40,8,16.2024,0,0,2,4,2,2,24,270,-90,120,180,1,1,1,24,330,0,0,0},
	{273,6,'H',5,"C2706S5","","","",'S',400,230,400,100,40,40,10,18.84,0,0,2,4,2,2,24,280,-90,80,180,1,1,1,24,320,0,0,0},
	{299,6,'H',5,"C2906S5","","","",'S',460,250,440,100,40,40,10,23.1104,0,0,2,4,2,2,24,300,-110,120,220,1,1,1,24,360,0,0,0},
	{299,7,'H',5,"C2907S5","","","",'S',420,250,440,100,40,40,10,21.1008,0,0,1,6,3,2,24,300,-110,80,110},
	{325,6,'H',5,"C3206S5","","","",'S',480,270,445,110,40,40,12,30.06864,0,0,2,4,2,2,24,320,-110,120,220,1,1,1,24,380,0,0,0},
	{325,7,'H',5,"C3207S5","","","",'S',430,260,470,110,40,40,12,27.94914,0,0,1,6,3,2,24,310,-140,80,140},
	{356,6,'H',5,"C3506S5","","","",'S',420,250,475,120,40,40,12,28.28826,0,0,1,6,3,2,24,300,-140,80,140},
	{356,7,'H',5,"C3507S5","","","",'S',540,290,510,120,40,40,12,38.151,0,0,2,4,2,2,24,340,-80,160,160,3,3,1,24,420,-160,0,160},
	{377,6,'H',5,"C3706S5","","","",'S',470,260,530,130,40,40,12,34.97646,0,0,2,4,2,2,24,310,-85,120,170,3,3,1,24,370,-170,0,170},
	{377,7,'H',5,"C3707S5","","","",'S',550,300,530,130,40,40,12,40.9299,0,0,2,4,2,2,24,350,-85,160,170,3,3,1,24,430,-170,0,170},
	{406,6,'H',5,"C4006S5","","","",'S',490,280,570,140,40,40,12,39.2343,0,0,2,4,2,2,24,330,-90,120,180,3,3,1,24,390,-180,0,180},
	{406,7,'H',5,"C4007S5","","","",'S',540,330,570,140,40,40,12,43.2378,0,0,2,4,2,2,24,380,-90,120,180,3,3,1,24,440,-180,0,180},
	{89,4,'H',7,"C0804S7","","","",'S',240,110,150,40,30,30,8,3.46656,0,0,2,2,2,1,20,150,-30,0,60,1,1,1,20,210,0,0,0},
	{102,4,'H',7,"C1004S7","","","",'S',270,140,160,40,30,30,8,4.06944,0,0,2,2,2,1,20,180,-30,0,60,1,1,1,20,240,0,0,0},
	{114,4,'H',7,"C1104S7","","","",'S',270,140,170,40,30,30,8,4.239,0,0,2,2,2,1,20,180,-35,0,70,1,1,1,20,240,0,0,0},
	{127,4,'H',7,"C1204S7","","","",'S',300,170,195,40,30,30,8,5.181,0,0,1,4,2,2,20,210,-40,60,80},
	{140,4,'H',7,"C1404S7","","","",'S',300,170,200,50,30,30,8,5.652,0,0,1,4,2,2,20,210,-40,60,80},
	{159,4,'H',7,"C1504S7","","","",'S',320,190,240,60,30,30,8,7.23456,0,0,1,4,2,2,20,230,-45,60,90},
	{159,5,'H',7,"C1505S7","","","",'S',320,190,245,60,30,30,8,7.33504,0,0,1,6,3,2,20,230,-60,60,60},
	{168,4,'H',7,"C1604S7","","","",'S',370,200,250,60,30,30,8,8.59732,0,0,2,4,2,2,20,240,-60,100,120,1,1,1,20,290,0,0,0},
	{168,5,'H',7,"C1605S7","","","",'S',330,200,260,60,30,30,8,7.87512,0,0,1,6,3,2,20,240,-70,60,70},
	{180,4,'H',7,"C1804S7","","","",'S',370,200,270,60,30,30,8,9.06204,0,0,2,4,2,2,20,240,-70,100,140,1,1,1,20,290,0,0,0},
	{180,5,'H',7,"C1805S7","","","",'S',330,200,270,60,30,30,8,8.08236,0,0,1,6,3,2,20,240,-70,60,70},
	{194,5,'H',7,"C1905S7","","","",'S',430,220,290,70,40,40,8,11.61172,0,0,2,4,2,2,24,270,-80,120,160,1,1,1,24,330,0,0,0},
	{194,6,'H',7,"C1906S7","","","",'S',390,220,290,70,40,40,10,13.16445,0,0,1,6,3,2,24,270,-80,80,80},
	{203,5,'H',7,"C2005S7","","","",'S',460,250,310,70,40,40,8,12.9996,0,0,2,4,2,2,24,300,-80,120,160,1,1,1,24,360,0,0,0},
	{203,6,'H',7,"C2006S7","","","",'S',420,250,310,70,40,40,10,14.8365,0,0,1,6,3,2,24,300,-80,80,80},
	{219,5,'H',7,"C2105S7","","","",'S',420,250,330,80,40,40,8,12.92424,0,0,1,6,3,2,24,300,-90,80,90},
	{219,6,'H',7,"C2106S7","","","",'S',420,250,335,80,40,40,10,16.32015,0,0,1,6,3,2,24,300,-90,80,90},
	{245,5,'H',7,"C2405S7","","","",'S',420,250,365,80,40,40,8,13.8474,0,0,1,6,3,2,24,300,-100,80,100},
	{245,6,'H',7,"C2406S7","","","",'S',460,250,365,80,40,40,10,18.95775,0,0,2,4,2,2,24,300,-50,120,100,3,3,1,24,360,-100,0,100},
	{273,5,'H',7,"C2705S7","","","",'S',510,300,405,100,40,40,8,19.37694,0,0,2,4,2,2,24,350,-60,120,120,3,3,1,24,410,-120,0,120},
	{273,6,'H',7,"C2706S7","","","",'S',520,310,400,100,40,40,10,24.492,0,0,2,4,2,2,24,360,-60,120,120,3,3,1,24,420,-120,0,120},
	{299,6,'H',7,"C2906S7","","","",'S',570,360,445,100,40,40,10,28.860525,0,0,2,4,2,2,24,410,-65,120,130,3,3,1,24,470,-130,0,130},
	{299,7,'H',7,"C2907S7","","","",'S',520,310,445,100,40,40,10,26.3289,0,0,2,6,3,2,24,360,-130,120,130,2,2,1,24,420,-65,0,130},
	{325,6,'H',7,"C3206S7","","","",'S',570,360,445,110,40,40,12,35.70651,0,0,2,4,2,2,24,410,-70,120,140,3,3,1,24,470,-140,0,140},
	{325,7,'H',7,"C3207S7","","","",'S',570,360,470,110,40,40,12,37.04886,0,0,2,6,3,2,24,410,-140,120,140,2,2,1,24,470,-70,0,140},
	{356,6,'H',7,"C3506S7","","","",'S',610,400,475,120,40,40,12,41.08533,0,0,2,6,3,2,24,450,-160,120,160,2,2,1,24,510,-80,0,160},
	{356,7,'H',7,"C3507S7","","","",'S',650,400,510,120,40,40,12,45.9225,0,0,1,9,3,3,24,450,-160,80,160},
	{377,6,'H',7,"C3706S7","","","",'S',610,400,540,130,40,40,12,45.9696,0,0,2,6,3,2,24,450,-160,120,160,2,2,1,24,510,-80,0,160},
	{377,7,'H',7,"C3707S7","","","",'S',630,420,530,130,40,40,12,46.88334,0,0,2,6,3,2,24,470,-120,120,120,4,4,1,24,530,-180,0,120},
	{406,6,'H',7,"C4006S7","","","",'S',630,420,570,140,40,40,12,50.4441,0,0,2,6,3,2,24,470,-120,120,120,4,4,1,24,530,-180,0,120},
	{406,7,'H',7,"C4007S7","","","",'S',660,450,570,140,40,40,12,52.8462,0,0,2,6,3,2,24,500,-120,120,120,4,4,1,24,560,-180,0,120},
	{89,4,'H',1,"C0804S1","","","",'S',280,150,150,40,30,30,8,4.04432,0,0,2,2,2,1,20,190,-35,0,70,1,1,1,20,250,0,0,0},
	{102,4,'H',1,"C1004S1","","","",'S',290,160,160,40,30,30,8,4.37088,0,0,1,4,2,2,20,200,-35,60,70},
	{114,4,'H',1,"C1104S1","","","",'S',320,190,170,40,30,30,8,5.024,0,0,1,4,2,2,20,230,-35,60,70},
	{127,4,'H',1,"C1204S1","","","",'S',370,200,200,40,30,30,8,6.50608,0,0,2,4,2,2,20,240,-50,100,100,1,1,1,20,290,0,0,0},
	{140,4,'H',1,"C1404S1","","","",'S',360,230,215,50,30,30,8,7.12152,0,0,2,3,3,1,20,270,-60,0,60,2,2,1,20,330,-30,0,60},
	{159,4,'H',1,"C1504S1","","","",'S',390,260,240,60,30,30,8,8.81712,0,0,1,6,3,2,20,300,-70,60,70},
	{159,5,'H',1,"C1505S1","","","",'S',430,260,250,60,30,30,10,12.48935,0,0,2,4,2,2,20,300,-35,100,70,3,3,1,20,350,-70,0,70},
	{168,4,'H',1,"C1604S1","","","",'S',400,270,250,60,30,30,8,9.2944,0,0,1,6,3,2,20,310,-80,60,80},
	{168,5,'H',1,"C1605S1","","","",'S',440,270,260,60,30,30,10,13.1252,0,0,2,6,3,2,20,310,-80,100,80,2,2,1,20,360,-40,0,80},
	{180,4,'H',1,"C1804S1","","","",'S',450,280,280,60,30,30,10,14.13,0,0,2,4,2,2,20,320,-40,100,80,3,3,1,20,370,-80,0,80},
	{180,5,'H',1,"C1805S1","","","",'S',450,280,280,60,30,30,12,16.956,0,0,2,6,3,2,20,320,-80,100,80,2,2,1,20,370,-40,0,80},
	{194,5,'H',1,"C1905S1","","","",'S',480,310,290,70,40,40,10,16.2024,0,0,1,6,3,2,24,360,-80,80,80},
	{194,6,'H',1,"C1906S1","","","",'S',500,330,290,70,40,40,12,20.253,0,0,1,6,3,2,24,380,-80,80,80},
	{203,5,'H',1,"C2005S1","","","",'S',500,330,310,70,40,40,10,17.6625,0,0,1,6,3,2,24,380,-90,80,90},
	{203,6,'H',1,"C2006S1","","","",'S',500,330,320,70,40,40,12,21.666,0,0,1,6,3,2,24,380,-90,80,90},
	{219,5,'H',1,"C2105S1","","","",'S',520,350,330,80,40,40,10,20.0018,0,0,1,6,3,2,24,400,-100,80,100},
	{219,6,'H',1,"C2106S1","","","",'S',560,350,330,80,40,40,12,25.84848,0,0,2,4,2,2,24,400,-50,120,100,3,3,1,24,460,-100,0,100},
	{245,5,'H',1,"C2405S1","","","",'S',570,360,370,80,40,40,10,23.71485,0,0,2,4,2,2,24,410,-55,120,110,3,3,1,24,470,-110,0,110},
	{245,6,'H',1,"C2406S1","","","",'S',580,370,370,80,40,40,12,28.95708,0,0,2,6,3,2,24,420,-110,120,110,2,2,1,24,480,-55,0,110},
	{273,5,'H',1,"C2705S1","","","",'S',610,400,410,100,40,40,12,35.05182,0,0,2,4,2,2,24,450,-60,120,120,3,3,1,24,510,-120,0,120},
	{273,6,'H',1,"C2706S1","","","",'S',640,430,410,100,40,40,12,36.77568,0,0,2,6,3,2,24,480,-120,120,120,2,2,1,24,540,-60,0,120},
	{89,4,'H',7,"C0804H7","","","",'H',260,130,155,40,30,30,8,3.83708,0,0,1,4,2,2,20,170,-30,60,60},
	{102,4,'H',7,"C1004H7","","","",'H',270,140,155,40,30,30,8,3.98466,0,0,1,4,2,2,20,180,-30,60,60},
	{114,4,'H',7,"C1104H7","","","",'H',330,160,180,40,30,30,8,5.38824,0,0,2,4,2,2,20,200,-40,100,80,1,1,1,20,250,0,0,0},
	{127,4,'H',7,"C1204H7","","","",'H',340,170,210,40,30,30,8,6.19208,0,0,2,4,2,2,20,210,-45,100,90,1,1,1,20,260,0,0,0},
	{140,4,'H',7,"C1404H7","","","",'H',320,190,210,50,30,30,8,6.22976,0,0,1,6,3,2,20,230,-50,60,50},
	{159,4,'H',7,"C1504H7","","","",'H',340,210,240,60,30,30,10,9.6084,0,0,1,6,3,2,20,250,-60,60,60},
	{159,5,'H',7,"C1505H7","","","",'H',420,210,240,60,30,30,10,11.8692,0,0,2,4,2,2,24,260,-60,120,120,1,1,1,24,320,0,0,0},
	{168,4,'H',7,"C1604H7","","","",'H',350,220,250,60,30,30,10,10.16575,0,0,1,6,3,2,20,260,-70,60,70},
	{168,5,'H',7,"C1605H7","","","",'H',430,220,250,60,40,40,10,12.48935,0,0,2,4,2,2,24,270,-70,120,140,1,1,1,24,330,0,0,0},
	{180,5,'H',7,"C1805H7","","","",'H',440,230,280,60,40,40,10,13.816,0,0,2,4,2,2,24,280,-70,120,140,1,1,1,24,340,0,0,0},
	{180,6,'H',7,"C1806H7","","","",'H',410,240,280,60,40,40,12,15.4488,0,0,1,6,3,2,24,290,-75,75,80},
	{194,5,'H',7,"C1905H7","","","",'H',420,250,280,70,40,40,10,13.8474,0,0,1,6,3,2,24,300,-80,80,80},
	{194,6,'H',7,"C1906H7","","","",'H',440,270,280,70,40,40,12,17.40816,0,0,1,6,3,2,24,320,-80,80,80},
	{203,5,'H',7,"C2005H7","","","",'H',430,260,305,70,40,40,10,15.020975,0,0,1,6,3,2,24,310,-80,80,80},
	{203,6,'H',7,"C2006H7","","","",'H',480,270,310,70,40,40,12,20.3472,0,0,2,4,2,2,24,320,-45,120,90,3,3,1,24,380,-90,0,90},
	{219,5,'H',7,"C2105H7","","","",'H',460,290,305,80,40,40,10,16.79115,0,0,1,6,3,2,24,340,-90,80,90},
	{219,6,'H',7,"C2106H7","","","",'H',500,290,310,80,40,40,12,22.137,0,0,2,4,2,2,24,340,-50,120,100,3,3,1,24,400,-100,0,100},
	{245,5,'H',7,"C2405H7","","","",'H',510,300,350,80,40,40,12,24.50142,0,0,2,4,2,2,24,350,-50,120,100,3,3,1,24,410,-100,0,100},
	{245,6,'H',7,"C2406H7","","","",'H',520,310,350,80,40,40,12,24.98184,0,0,2,6,3,2,24,360,-100,120,100,2,2,1,24,420,-50,0,100},
	{273,5,'H',7,"C2705H7","","","",'H',530,320,375,100,40,40,12,28.70745,0,0,2,4,2,2,24,370,-60,120,120,3,3,1,24,430,-120,0,120},
	{273,6,'H',7,"C2706H7","","","",'H',560,350,375,100,40,40,12,30.3324,0,0,2,6,3,2,24,400,-120,120,120,2,2,1,24,460,-60,0,120},
	{299,6,'H',7,"C2906H7","","","",'H',700,430,470,100,40,40,12,44.1798,0,0,2,6,3,2,24,480,-140,120,140,4,2,2,24,540,-70,120,140},
	{299,7,'H',7,"C2907H7","","","",'H',650,440,470,100,40,40,12,41.0241,0,0,2,8,4,2,24,490,-135,120,90,3,3,1,24,550,-90,0,90},
	{325,6,'H',7,"C3206H7","","","",'H',690,420,440,110,40,40,12,42.89868,0,0,2,6,3,2,24,470,-140,120,140,4,2,2,24,530,-70,120,140},
	{356,7,'H',7,"C3507H7","","","",'H',690,440,510,120,40,40,14,56.87325,0,0,1,12,4,3,24,490,-150,80,100},
	{356,8,'H',7,"C3508H7","","","",'H',710,440,510,120,40,40,16,66.882,0,0,2,8,4,2,24,490,-180,120,120,6,3,2,24,550,-120,120,120},
	{325,7,'H',7,"C3207H7","","","",'H',620,410,470,110,40,40,14,47.01522,0,0,2,8,4,2,24,460,-150,120,100,3,3,1,24,520,-100,0,100},
	{377,7,'H',7,"C3707H7","","","",'H',750,480,535,130,40,40,14,65.527875,0,0,2,8,4,2,24,530,-180,120,120,6,3,2,24,590,-120,120,120},
	{89,4,'H',1,"C0804H1","","","",'H',340,170,160,40,30,30,10,6.4056,0,0,2,4,2,2,20,210,-35,100,70,1,1,1,20,260,0,0,0},
	{102,4,'H',1,"C1004H1","","","",'H',370,180,170,40,30,30,10,7.26125,0,0,1,6,2,3,20,220,-35,60,70},
	{114,4,'H',1,"C1104H1","","","",'H',400,210,185,40,30,30,10,8.321,0,0,1,6,2,3,20,250,-35,60,70},
	{127,4,'H',1,"C1204H1","","","",'H',430,260,200,60,30,30,12,12.96192,0,0,2,4,2,2,20,300,-25,100,50,3,3,1,20,350,-50,0,50},
	{140,4,'H',1,"C1404H1","","","",'H',430,260,220,50,30,30,12,12.96192,0,0,2,6,3,2,20,300,-60,100,60,2,2,1,20,350,-30,0,60},
	{159,4,'H',1,"C1504H1","","","",'H',460,290,250,60,30,30,12,16.03284,0,0,2,6,3,2,20,330,-70,100,70,2,2,1,20,380,-35,0,70},
	{159,5,'H',1,"C1505H1","","","",'H',460,290,250,60,40,40,14,18.70498,0,0,1,6,3,2,24,340,-70,80,70},
	{168,4,'H',1,"C1604H1","","","",'H',460,290,260,60,30,30,12,16.46616,0,0,2,6,3,2,20,330,-80,100,80,2,2,1,20,380,-40,0,80},
	{168,5,'H',1,"C1605H1","","","",'H',480,310,260,60,40,40,14,20.04576,0,0,1,6,3,2,24,360,-80,80,80},
	{180,5,'H',1,"C1805H1","","","",'H',530,320,290,60,40,40,14,23.88127,0,0,2,4,2,2,24,370,-40,120,80,3,3,1,24,430,-80,0,80},
	{180,6,'H',1,"C1806H1","","","",'H',540,330,290,60,40,40,16,27.80784,0,0,2,6,3,2,24,380,-80,120,80,2,2,1,24,440,-40,0,80},
	{194,5,'H',1,"C1905H1","","","",'H',560,350,300,70,40,40,14,27.07936,0,0,2,4,2,2,24,400,-40,120,80,3,3,1,24,460,-80,0,80},
	{194,6,'H',1,"C1906H1","","","",'H',620,370,300,70,40,40,16,34.26368,0,0,1,9,3,3,24,420,-80,80,80},
	{203,5,'H',1,"C2005H1","","","",'H',580,370,320,70,40,40,16,33.51008,0,0,2,6,3,2,24,420,-90,120,90,2,2,1,24,480,-45,0,90},
	{203,6,'H',1,"C2006H1","","","",'H',590,340,320,70,40,40,16,34.08784,0,0,1,9,3,3,24,390,-90,80,90},
	{219,5,'H',1,"C2105H1","","","",'H',610,400,340,80,40,40,14,33.5195,0,0,2,6,3,2,24,450,-100,120,100,2,2,1,24,510,-50,0,100},
	{219,6,'H',1,"C2106H1","","","",'H',620,350,340,80,40,40,14,34.069,0,0,2,6,3,2,24,400,-100,120,100,4,2,2,24,460,-50,120,100},
	{245,5,'H',1,"C2405H1","","","",'H',690,420,380,70,40,40,14,39.43212,0,0,2,6,3,2,24,470,-110,120,110,2,2,1,24,530,-55,0,100},
	{273,5,'H',1,"C2705H1","","","",'H',670,400,420,100,40,40,14,45.65246,0,0,2,6,3,2,24,450,-120,120,120,4,2,2,24,510,-60,120,120},
	{273,6,'H',1,"C2706H1","","","",'H',760,430,420,100,40,40,14,51.78488,0,0,2,9,3,3,24,480,-120,120,120,4,2,2,24,540,-60,120,120},
	{245,6,'H',1,"C2406H1","","","",'H',750,420,380,80,40,40,16,50.868,0,0,2,9,3,3,24,470,-110,80,110,2,2,1,24,710,-55,0,110},
	{299,6,'H',1,"C2906H1","","","",'H',800,470,450,100,40,40,16,65.312,0,0,2,9,3,3,24,520,-140,120,140,4,2,2,24,580,-70,120,140},
	{299,7,'H',1,"C2907H1","","","",'H',860,470,460,100,40,40,16,71.29056,0,0,2,9,3,3,24,520,-140,120,140,6,2,3,24,580,-70,120,140},
	{89,4,'H',5,"C0804H5","","","",'H',200,70,150,40,30,30,6,2.1666,0,0,2,2,2,1,20,110,-30,0,60,1,1,1,20,170,0,0,0},
	{102,4,'H',5,"C1004H5","","","",'H',210,80,160,40,30,30,8,3.16512,0,0,2,2,2,1,20,120,-30,0,60,1,1,1,20,180,0,0,0},
	{114,4,'H',5,"C1104H5","","","",'H',220,90,170,40,30,30,8,3.454,0,0,2,2,2,1,20,130,-35,0,70,1,1,1,20,190,0,0,0},
	{127,4,'H',5,"C1204H5","","","",'H',230,100,190,50,30,30,8,4.18876,0,0,1,4,2,2,20,140,-40,60,80},
	{140,4,'H',5,"C1404H5","","","",'H',240,110,220,50,30,30,8,4.82304,0,0,1,4,2,2,20,150,-40,60,80},
	{159,4,'H',5,"C1504H5","","","",'H',290,120,240,60,30,30,8,6.55632,0,0,2,4,2,2,20,160,-60,100,120,1,1,1,20,210,0,0,0},
	{159,5,'H',5,"C1505H5","","","",'H',300,130,240,60,30,30,8,6.7824,0,0,2,4,2,2,20,170,-60,100,120,1,1,1,20,220,0,0,0},
	{168,4,'H',5,"C1604H5","","","",'H',300,130,260,60,30,30,8,7.1592,0,0,2,4,2,2,20,170,-60,100,120,1,1,1,20,220,0,0,0},
	{168,5,'H',5,"C1605H5","","","",'H',270,140,260,60,30,30,8,6.44328,0,0,1,6,3,2,20,180,-70,60,70},
	{180,4,'H',5,"C1804H5","","","",'H',310,140,280,70,30,30,8,8.17656,0,0,2,4,2,2,20,180,-70,100,140,1,1,1,20,230,0,0,70},
	{180,5,'H',5,"C1805H5","","","",'H',270,140,280,70,30,30,8,7.12152,0,0,1,6,3,2,20,180,-70,60,70},
	{194,5,'H',5,"C1905H5","","","",'H',320,150,300,70,30,30,8,8.84224,0,0,2,4,2,2,20,190,-45,100,90,3,3,1,20,240,-90,0,90},
	{194,6,'H',5,"C1906H5","","","",'H',330,160,300,70,30,30,10,11.3982,0,0,2,6,3,2,20,200,-90,100,90,2,2,1,20,250,-45,0,90},
	{203,5,'H',5,"C2005H5","","","",'H',330,160,310,80,30,30,8,9.74028,0,0,2,4,2,2,20,200,-50,100,100,3,3,1,20,250,-100,0,100},
	{203,6,'H',5,"C2006H5","","","",'H',380,170,310,80,40,40,10,14.0201,0,0,2,4,2,2,24,220,-80,120,160,1,1,1,24,280,0,0,0},
	{219,5,'H',5,"C2105H5","","","",'H',380,170,330,80,40,40,10,14.6167,0,0,2,4,2,2,24,220,-90,120,180,1,1,1,24,280,0,0,0},
	{219,6,'H',5,"C2106H5","","","",'H',350,180,330,80,40,40,10,13.46275,0,0,1,6,3,2,24,230,-90,80,90},
	{245,5,'H',5,"C2405H5","","","",'H',400,190,370,90,40,40,10,17.27,0,0,2,4,2,2,24,240,-100,120,200,1,1,1,24,300,0,0,0},
	{245,6,'H',5,"C2406H5","","","",'H',370,200,370,90,40,40,10,15.97475,0,0,1,6,3,2,24,250,-100,80,100},
	{273,5,'H',5,"C2705H5","","","",'H',380,210,410,100,40,40,10,18.1963,0,0,1,6,3,2,24,260,-120,80,120},
	{273,6,'H',5,"C2706H5","","","",'H',430,220,410,100,40,40,10,20.59055,0,0,2,4,2,2,24,270,-60,120,120,3,3,1,24,330,-120,0,120},
	{299,6,'H',5,"C2906H5","","","",'H',450,240,440,110,40,40,10,23.3145,0,0,2,4,2,2,24,290,-65,120,130,3,3,1,24,350,-130,0,130},
	{299,7,'H',5,"C2907H5","","","",'H',450,240,440,110,40,40,12,27.9774,0,0,2,6,3,2,24,290,-130,120,130,2,2,1,24,350,-65,0,130},
	{325,6,'H',5,"C3206H5","","","",'H',470,260,470,120,40,40,12,31.43454,0,0,2,4,2,2,24,310,-70,120,140,3,3,1,24,370,-140,0,140},
	{325,7,'H',5,"C3207H5","","","",'H',470,260,470,120,40,40,12,31.43454,0,0,2,6,3,2,24,310,-140,120,140,2,2,1,24,360,-70,0,140},
	{356,6,'H',5,"C3506H6","","","",'H',490,280,510,130,40,40,12,35.54166,0,0,2,6,3,2,24,330,-160,120,160,2,2,1,24,390,-80,0,160},
	{356,7,'H',5,"C3507H5","","","",'H',500,290,510,130,40,40,12,36.267,0,0,2,6,3,2,24,340,-120,120,120,4,4,1,24,400,-180,0,120},
	{377,6,'H',5,"C3706H5","","","",'H',510,330,540,140,40,40,12,39.39444,0,0,2,6,3,2,24,350,-160,120,160,2,2,1,24,410,-80,0,160},
	{377,7,'H',5,"C3707H5","","","",'H',510,300,540,140,40,40,12,39.39444,0,0,2,6,3,2,24,350,-120,120,120,4,4,1,24,410,-180,0,120},
	{406,6,'H',5,"C4006H5","","","",'H',530,320,580,150,40,40,12,43.93488,0,0,2,6,3,2,24,370,-120,120,120,4,4,1,24,430,-180,0,120},
	{406,7,'H',5,"C4007H5","","","",'H',530,320,580,150,40,40,12,43.93488,0,0,2,8,4,2,24,370,-180,120,120,3,3,1,24,430,-120,0,120},
	/* 因更换插板库注释掉(2013.6.5版本) wht 2015.12.08
	{89,4,'H',5,"C0804S5","","","",'S',160,90,150,40,30,30,6,1.73,0,0,1,2,2,1,20,130,-30,60,60},
	{168,5,'H',5,"C1605S5","","","",'S',280,150,260,60,30,30,8,6.68192,0,0,2,4,2,2,20,190,-70,60,140,1,1,1,20,220,0,0,0},
	{114,4,'H',5,"C1104S5","","","",'S',180,110,170,40,30,30,8,2.826,0,0,1,2,2,1,20,150,-35,50,70},
	{140,4,'H',5,"C1404S5","","","",'S',250,130,215,50,30,30,8,4.9455,0,0,2,1,1,1,20,220,0,50,40,2,2,1,20,170,-40,50,80},
	{159,4,'H',5,"C1504S5","","","",'S',260,140,240,60,30,30,8,5.87808,0,0,2,2,2,1,20,180,-45,80,90,1,1,1,20,230,0,80,90},
	{159,5,'H',5,"C1505S5","","","",'S',270,140,250,60,30,30,8,6.27372,0,0,2,4,2,2,20,180,-60,60,120,1,1,1,20,210,0,0,0},
	{168,4,'H',5,"C1604S5","","","",'S',270,150,250,60,30,30,8,6.27372,0,0,2,2,2,1,20,190,-60,0,120,1,1,1,20,240,0,0,0},
	{194,5,'H',5,"C1905S5","","","",'S',300,170,290,70,30,30,8,8.1012,0,0,2,4,2,2,20,210,-80,60,160,1,1,1,20,240,0,0,0},
	{219,5,'H',5,"C2105S5","","","",'S',360,190,330,80,40,40,8,11.07792,0,0,2,4,2,2,24,240,-90,80,180,1,1,1,20,280,0,0,0},
	{219,6,'H',5,"C2106S5","","","",'S',360,190,330,80,40,40,10,13.8474,0,0,2,4,2,2,24,240,-90,80,180,1,1,1,20,280,0,0,0},
	{273,6,'H',5,"C2706S5","","","",'S',400,230,400,100,40,40,10,18.84,0,0,2,4,2,2,24,280,-90,80,180,1,1,1,20,320,0,0,0},
	{325,6,'H',5,"C3206S5","","","",'S',480,270,445,110,40,40,12,30.06864,0,0,2,1,1,1,24,380,0,120,220,4,2,2,24,320,-110,120,220},
	{325,7,'H',5,"C3207S5","","","",'S',430,260,470,110,40,40,12,27.94914,0,0,1,6,3,2,24,310,-140,80,140},
	{356,6,'H',5,"C3506S5","","","",'S',420,250,475,120,40,40,12,28.28826,0,0,1,6,3,2,24,300,-140,80,140},
	{356,7,'H',5,"C3507S5","","","",'S',540,290,510,120,40,40,12,38.151,0,0,2,4,2,2,24,340,-80,160,160,3,3,1,24,420,-160,160,160},
	{377,7,'H',5,"C3707S5","","","",'S',550,300,530,130,40,40,12,40.9299,0,0,2,3,3,1,24,430,-170,160,170,4,2,2,24,350,-85,160,170},
	{406,7,'H',5,"C4007S5","","","",'S',540,330,570,140,40,40,12,43.2378,0,0,2,4,2,2,24,380,-90,120,180,3,3,1,24,440,-180,120,180},
	{377,8,'H',5,"C3708S5","","","",'S',550,340,540,110,40,40,12,39.38,0,0,2,6,3,2,24,390,-170,120,170,2,2,1,24,450,-85,120,170},
	{406,8,'H',5,"C4008S5","","","",'S',580,370,570,120,40,40,12,44.26,0,0,2,2,2,1,24,480,-90,120,180,6,3,2,24,420,-180,120,180},
	{89,4,'H',7,"C0804S7","","","",'S',240,110,150,40,30,30,8,3.47,0,0,2,2,2,1,20,150,-30,180,60,1,1,1,20,210,0,180,60},
	{114,4,'H',7,"C1104S7","","","",'S',270,140,170,40,30,30,8,4.239,0,0,2,1,1,1,20,240,0,180,60,2,2,1,20,180,-35,180,70},
	{140,4,'H',7,"C1404S7","","","",'S',300,170,200,50,30,30,8,5.652,0,0,1,4,2,2,20,210,-40,60,80},
	{159,4,'H',7,"C1504S7","","","",'S',310,190,240,60,30,30,8,7.00848,0,0,1,4,2,2,20,230,-45,50,90},
	{159,5,'H',7,"C1505S7","","","",'S',320,190,245,60,30,30,8,7.33504,0,0,1,6,3,2,20,230,-60,60,60},
	{168,4,'H',7,"C1604S7","","","",'S',370,200,250,60,30,30,8,8.59732,0,0,2,4,2,2,20,240,-60,100,120,1,1,1,20,290,0,120,160},
	{168,5,'H',7,"C1605S7","","","",'S',330,200,260,60,30,30,8,7.87512,0,0,1,6,3,2,20,240,-70,60,70},
	{194,5,'H',7,"C1905S7","","","",'S',430,220,290,70,40,40,8,11.61172,0,0,2,4,2,2,24,270,-80,120,160,1,1,1,24,330,0,120,160},
	{219,5,'H',7,"C2105S7","","","",'S',420,250,330,80,40,40,8,12.92424,0,0,1,6,3,2,24,300,-90,80,90},
	{219,6,'H',7,"C2106S7","","","",'S',420,250,335,80,40,40,10,16.32015,0,0,1,6,3,2,24,300,-90,80,90},
	{273,6,'H',7,"C2706S7","","","",'S',520,310,400,100,40,40,10,24.492,0,0,2,4,2,2,24,360,-60,120,120,3,3,1,24,420,-120,120,120},
	{325,6,'H',7,"C3206S7","","","",'S',570,360,445,110,40,40,12,35.70651,0,0,2,3,3,1,24,470,-140,120,140,4,2,2,24,410,-70,120,140},
	{325,7,'H',7,"C3207S7","","","",'S',570,360,470,110,40,40,12,37.04886,0,0,2,6,3,2,24,410,-140,120,140,2,2,1,24,470,-70,120,140},
	{356,6,'H',7,"C3506S7","","","",'S',610,400,475,120,40,40,12,41.08533,0,0,2,2,2,1,24,510,-80,120,160,6,3,2,24,450,-160,120,160},
	{356,7,'H',7,"C3507S7","","","",'S',650,400,510,120,40,40,12,45.9225,0,0,1,9,3,3,24,450,-160,80,160},
	{377,7,'H',7,"C3707S7","","","",'S',630,420,530,130,40,40,12,46.88334,0,0,2,4,4,1,24,530,-180,0,120,6,3,2,24,470,-120,120,120},
	{406,7,'H',7,"C4007S7","","","",'S',660,450,570,140,40,40,12,52.8462,0,0,2,4,4,1,24,560,-180,0,120,6,3,2,24,500,-120,120,120},
	{377,8,'H',7,"C3708S7","","","",'S',620,410,540,110,40,40,14,51.78,0,0,2,4,4,1,24,520,-165,120,110,6,3,2,24,460,-110,120,110},
	{406,8,'H',7,"C4008S7","","","",'S',660,450,570,120,40,40,14,58.75,0,0,2,8,4,2,24,500,-180,120,120,3,3,1,24,560,-120,120,120},
	{89,4,'H',7,"C0804H7","","","",'H',260,130,155,40,30,30,8,3.84,0,0,1,4,2,2,20,170,-30,60,60},
	{114,4,'H',7,"C1104H7","","","",'H',330,160,180,40,30,30,8,5.39,0,0,2,4,2,2,20,200,-40,100,80,1,1,1,20,250,0,100,80},
	{140,4,'H',7,"C1404H7","","","",'H',320,190,210,50,30,30,8,6.22976,0,0,1,6,3,2,20,230,-50,60,50},
	{159,4,'H',7,"C1504H7","","","",'H',340,210,240,60,30,30,10,9.6084,0,0,1,6,3,2,20,250,-60,60,60},
	{159,5,'H',7,"C1505H7","","","",'H',420,210,240,60,40,40,10,11.8692,0,0,2,4,2,2,24,260,-60,120,120,1,1,1,24,320,0,0,0},
	{168,4,'H',7,"C1604H7","","","",'H',350,220,250,60,30,30,10,10.16575,0,0,1,6,3,2,20,260,-70,60,70},
	{168,5,'H',7,"C1605H7","","","",'H',430,220,250,60,40,40,10,12.48935,0,0,2,4,2,2,24,270,-70,120,140,1,1,1,24,330,0,120,140},
	{194,5,'H',7,"C1905H7","","","",'H',420,250,280,70,40,40,10,13.8474,0,0,1,6,3,2,24,300,-80,80,80},
	{219,5,'H',7,"C2105H7","","","",'H',460,290,305,80,40,40,10,16.79115,0,0,1,6,3,2,24,340,-90,80,90},
	{219,6,'H',7,"C2106H7","","","",'H',500,290,310,80,40,40,12,22.137,0,0,2,4,2,2,24,340,-50,120,100,3,3,1,24,400,-100,120,100},
	{273,6,'H',7,"C2706H7","","","",'H',560,350,375,100,40,40,12,30.3324,0,0,2,6,3,2,24,400,-120,120,120,2,2,1,24,460,-60,120,120},
	{325,6,'H',7,"C3206H7","","","",'H',690,420,440,110,40,40,12,42.89868,0,0,2,6,3,2,24,470,-140,120,140,4,2,2,24,530,-70,120,140},
	{356,6,'H',7,"C3506H7","","","",'H',630,420,480,80,40,40,12,37.98144,0,0,2,8,4,2,24,470,-165,120,110,3,3,1,24,530,-110,120,110},
	{356,7,'H',7,"C3507H7","","","",'H',690,440,510,120,40,40,14,56.87325,0,0,1,12,4,3,24,490,-150,80,100},
	{325,7,'H',7,"C3207H7","","","",'H',620,410,470,110,40,40,14,47.01522,0,0,2,8,4,2,24,460,-150,120,100,3,3,1,24,520,-100,0,100},
	{377,7,'H',7,"C3707H7","","","",'H',750,480,535,130,40,40,14,65.527875,0,0,2,8,4,2,24,530,-180,120,120,6,3,2,24,590,-120,120,120},
	{406,7,'H',7,"C4007H7","","","",'H',750,480,575,120,40,40,12,57.58,0,0,2,6,3,2,24,590,-130,120,130,8,4,2,24,530,-195,120,130},
	{89,4,'H',1,"C0804S1","","","",'S',280,150,150,40,30,30,8,4.04432,0,0,2,2,2,1,20,190,-35,120,70,1,1,1,20,250,0,120,70},
	{114,4,'H',1,"C1104S1","","","",'S',320,190,170,40,30,30,8,5,0,0,1,4,2,2,20,230,-35,60,70},
	{140,4,'H',1,"C1404S1","","","",'S',360,230,215,50,30,30,8,7.1,0,0,2,3,3,1,20,270,-60,120,60,2,2,1,20,330,-30,120,60},
	{159,4,'H',1,"C1504S1","","","",'S',390,260,240,60,30,30,8,8.8,0,0,1,6,3,2,20,300,-70,60,70},
	{159,5,'H',1,"C1505S1","","","",'S',430,260,250,60,30,30,10,12.5,0,0,2,4,2,2,20,300,-35,100,70,3,3,1,20,350,-70,100,70},
	{168,4,'H',1,"C1604S1","","","",'S',400,270,250,60,30,30,8,9.3,0,0,1,6,3,2,20,310,-80,60,80},
	{168,5,'H',1,"C1605S1","","","",'S',440,270,260,60,30,30,10,13.1,0,0,2,6,3,2,20,310,-80,100,80,2,2,1,20,360,-40,100,80},
	{194,5,'H',1,"C1905S1","","","",'S',480,310,290,70,40,40,10,16.2,0,0,1,6,3,2,24,360,-80,80,80},
	{219,5,'H',1,"C2105S1","","","",'S',520,350,330,80,40,40,10,20.0018,0,0,1,6,3,2,24,400,-100,80,100},
	{219,6,'H',1,"C2106S1","","","",'S',560,350,330,80,40,40,12,25.8,0,0,2,4,2,2,24,400,-50,120,100,3,3,1,24,460,-100,120,100},
	{273,6,'H',1,"C2706S1","","","",'S',640,430,410,100,40,40,12,36.8,0,0,2,6,3,2,24,480,-120,120,120,2,2,1,24,540,-60,120,120},
	{89,4,'H',1,"C0804H1","","","",'H',340,170,160,40,30,30,10,6.4,0,0,2,4,2,2,20,210,-35,100,70,1,1,1,20,260,0,100,70},
	{114,4,'H',1,"C1104H1","","","",'H',400,210,185,40,30,30,10,8.3,0,0,1,6,2,3,20,250,-35,60,70},
	{140,4,'H',1,"C1404H1","","","",'H',430,260,220,50,30,30,12,12.96192,0,0,2,6,3,2,20,300,-60,100,60,2,2,1,20,350,-30,100,60},
	{159,4,'H',1,"C1504H1","","","",'H',460,290,250,60,30,30,12,16.03284,0,0,2,2,2,1,20,380,-35,100,70,6,3,2,20,330,-70,100,70},
	{159,5,'H',1,"C1505H1","","","",'H',460,290,250,60,40,40,14,18.7,0,0,1,6,3,2,24,340,-70,80,70},
	{168,4,'H',1,"C1604H1","","","",'H',460,290,260,60,30,30,12,16.46616,0,0,2,6,3,2,20,330,-80,100,80,2,2,1,20,380,-40,100,80},
	{168,5,'H',1,"C1605H1","","","",'H',480,310,260,60,40,40,14,20.04576,0,0,1,6,3,2,24,360,-80,80,80},
	{194,5,'H',1,"C1905H1","","","",'H',560,350,300,70,40,40,14,27.1,0,0,2,4,2,2,24,400,-40,120,80,3,3,1,24,460,-80,80,80},
	{219,5,'H',1,"C2105H1","","","",'H',610,400,340,80,40,40,14,33.5195,0,0,2,6,3,2,24,450,-100,120,100,2,2,1,24,510,-50,120,100},
	{219,6,'H',1,"C2106H1","","","",'H',620,350,340,80,40,40,14,34.069,0,0,2,4,2,2,24,460,-50,120,100,6,3,2,24,400,-100,120,100},
	{273,6,'H',1,"C2706H1","","","",'H',760,430,420,100,40,40,14,51.8,0,0,2,9,3,3,24,480,-120,120,120,4,2,2,24,540,-60,120,120},
	{194,6,'H',5,"C1906S5","","","",'S',340,170,290,70,40,40,10,11.4767,0,0,2,4,2,2,24,220,-80,80,160,1,1,1,24,260,0,0,0},
	{377,6,'H',5,"C3706S5","","","",'S',470,260,530,130,40,40,12,34.97646,0,0,2,3,3,1,24,370,-170,0,170,4,2,2,24,310,-85,120,170},
	{406,6,'H',5,"C4006S5","","","",'S',490,280,570,140,40,40,12,39.2343,0,0,2,3,3,1,24,390,-180,0,180,4,2,2,24,330,-90,120,180},
	{76,3,'H',5,"C0703S5","","","",'S',140,80,140,40,25,25,6,1.45068,0,0,1,2,2,1,16,115,-30,0,60},
	{76,4,'H',5,"C0704S5","","","",'S',140,80,140,40,25,25,6,1.45068,0,0,1,2,2,1,16,115,-30,0,60},
	{89,3,'H',5,"C0803S5","","","",'S',140,80,150,40,25,25,6,1.51662,0,0,1,2,2,1,16,115,-30,0,60},
	{102,3,'H',5,"C1003S5","","","",'S',170,100,160,40,30,30,6,1.92168,0,0,1,2,2,1,20,140,-35,0,70},
	{102,4,'H',5,"C1004S5","","","",'S',190,120,160,40,30,30,8,2.86368,0,0,1,2,2,1,20,160,-35,0,70},
	{127,4,'H',5,"C1204S5","","","",'S',240,120,190,40,30,30,8,4.06944,0,0,2,2,2,1,20,160,-40,0,80,1,1,1,20,210,0,0,0},
	{180,4,'H',5,"C1804S5","","","",'S',280,150,280,60,30,30,8,7.0336,0,0,2,4,2,2,20,190,-70,60,140,1,1,1,20,220,0,0,0},
	{180,5,'H',5,"C1805S5","","","",'S',280,150,280,60,30,30,8,7.0336,0,0,2,4,2,2,20,190,-70,60,140,1,1,1,20,220,0,0,0},
	{203,5,'H',5,"C2005S5","","","",'S',310,180,310,70,30,30,8,8.7606,0,0,2,4,2,2,20,220,-80,60,160,1,1,1,20,250,0,0,0},
	{203,6,'H',5,"C2006S5","","","",'S',350,180,310,70,40,40,10,12.36375,0,0,2,4,2,2,24,230,-80,80,160,1,1,1,24,270,0,0,0},
	{245,5,'H',5,"C2405S5","","","",'S',380,210,370,80,40,40,8,12.64792,0,0,2,4,2,2,24,260,-90,80,180,1,1,1,24,300,0,0,0},
	{245,6,'H',5,"C2406S5","","","",'S',380,210,370,80,40,40,10,15.8099,0,0,2,4,2,2,24,260,-90,80,180,1,1,1,24,300,0,0,0},
	{273,5,'H',5,"C2705S5","","","",'S',440,220,400,100,40,40,8,16.5792,0,0,2,4,2,2,24,270,-90,120,180,1,1,1,24,330,0,0,0},
	{299,6,'H',5,"C2906S5","","","",'S',460,250,440,100,40,40,10,23.1104,0,0,2,4,2,2,24,300,-110,120,220,1,1,1,24,360,0,0,0},
	{299,7,'H',5,"C2907S5","","","",'S',420,250,440,100,40,40,10,21.1008,0,0,1,6,3,2,24,300,-110,80,110},
	{194,6,'H',7,"C1906S7","","","",'S',390,220,290,70,40,40,10,13.16445,0,0,1,6,3,2,24,270,-80,80,80},
	{377,6,'H',7,"C3706S7","","","",'S',610,400,540,130,40,40,12,45.9696,0,0,2,6,3,2,24,450,-160,120,160,2,2,1,24,510,-80,0,160},
	{406,6,'H',7,"C4006S7","","","",'S',630,420,570,140,40,40,12,50.4441,0,0,2,4,4,1,24,530,-180,0,120,6,3,2,24,470,-120,120,120},
	{194,6,'H',7,"C1906H7","","","",'H',440,270,280,70,40,40,12,17.40816,0,0,1,6,3,2,24,320,-80,80,80},
	{273,5,'H',7,"C2705H7","","","",'H',530,320,375,100,40,40,12,28.70745,0,0,2,3,3,1,24,430,-120,0,120,4,2,2,24,370,-60,120,120},
	{356,8,'H',7,"C3508H7","","","",'H',710,440,510,120,40,40,16,66.882,0,0,2,8,4,2,24,490,-180,120,120,6,3,2,24,550,-120,120,120},
	{180,4,'H',7,"C1804S7","","","",'S',370,200,270,60,30,30,8,9.06204,0,0,2,4,2,2,20,240,-70,100,140,1,1,1,20,290,0,0,0},
	{102,4,'H',7,"C1004S7","","","",'S',270,140,160,40,30,30,8,4.06944,0,0,2,2,2,1,20,180,-30,0,60,1,1,1,20,240,0,0,0},
	{127,4,'H',7,"C1204S7","","","",'S',300,170,195,40,30,30,8,5.181,0,0,1,4,2,2,20,210,-40,60,80},
	{180,5,'H',7,"C1805S7","","","",'S',330,200,270,60,30,30,8,8.08236,0,0,1,6,3,2,20,240,-70,60,70},
	{203,5,'H',7,"C2005S7","","","",'S',460,250,310,70,40,40,8,12.9996,0,0,2,4,2,2,24,300,-80,120,160,1,1,1,24,360,0,0,0},
	{203,6,'H',7,"C2006S7","","","",'S',420,250,310,70,40,40,10,14.8365,0,0,1,6,3,2,24,300,-80,80,80},
	{245,5,'H',7,"C2405S7","","","",'S',420,250,365,80,40,40,8,13.8474,0,0,1,6,3,2,24,300,-100,80,100},
	{245,6,'H',7,"C2406S7","","","",'S',460,250,365,80,40,40,10,18.95775,0,0,2,3,3,1,24,360,-100,0,100,4,2,2,24,300,-50,120,100},
	{273,5,'H',7,"C2705S7","","","",'S',510,300,405,100,40,40,8,19.37694,0,0,2,3,3,1,24,410,-120,0,120,4,2,2,24,350,-60,120,120},
	{299,6,'H',7,"C2906S7","","","",'S',570,360,445,100,40,40,10,28.860525,0,0,2,3,3,1,24,470,-130,0,130,4,2,2,24,410,-65,120,130},
	{299,7,'H',7,"C2907S7","","","",'S',520,310,445,100,40,40,10,26.3289,0,0,2,6,3,2,24,360,-130,120,130,2,2,1,24,420,-65,0,130},
	{102,4,'H',7,"C1004H7","","","",'H',270,140,155,40,30,30,8,3.98466,0,0,1,4,2,2,20,180,-30,60,60},
	{127,4,'H',7,"C1204H7","","","",'H',340,170,210,40,30,30,8,6.19208,0,0,2,4,2,2,20,210,-45,100,90,1,1,1,20,260,0,0,0},
	{180,5,'H',7,"C1805H7","","","",'H',440,230,280,60,40,40,10,13.816,0,0,2,4,2,2,24,280,-70,120,140,1,1,1,24,340,0,0,0},
	{180,6,'H',7,"C1806H7","","","",'H',410,240,280,60,40,40,12,15.4488,0,0,1,6,3,2,24,290,-75,80,75},
	{203,5,'H',7,"C2005H7","","","",'H',430,260,305,70,40,40,10,15.020975,0,0,1,6,3,2,24,310,-80,80,80},
	{203,6,'H',7,"C2006H7","","","",'H',480,270,310,70,40,40,12,20.3472,0,0,2,3,3,1,24,380,-90,0,90,4,2,2,24,320,-45,120,90},
	{245,5,'H',7,"C2405H7","","","",'H',510,300,350,80,40,40,12,24.50142,0,0,2,3,3,1,24,410,-100,0,100,4,2,2,24,350,-50,120,100},
	{245,6,'H',7,"C2406H7","","","",'H',520,310,350,80,40,40,12,24.98184,0,0,2,6,3,2,24,360,-100,120,100,2,2,1,24,420,-50,0,100},
	{299,6,'H',7,"C2906H7","","","",'H',700,430,470,100,40,40,12,44.1798,0,0,2,6,3,2,24,480,-140,120,140,4,2,2,20,540,-70,120,140},
	{299,7,'H',7,"C2907H7","","","",'H',650,440,470,100,40,40,12,41.0241,0,0,2,8,4,2,24,490,-135,120,90,3,3,1,24,550,-90,0,90},
	{194,6,'H',1,"C1906S1","","","",'S',500,330,290,70,40,40,12,20.253,0,0,1,6,3,2,24,380,-80,80,80},
	{194,6,'H',1,"C1906H1","","","",'H',620,370,300,70,40,40,16,34.26368,0,0,1,9,3,3,24,420,-80,80,80},
	{273,5,'H',1,"C2705H1","","","",'H',670,400,420,100,40,40,14,45.65246,0,0,2,6,3,2,24,450,-120,120,120,4,2,2,24,510,-60,120,120},
	{102,4,'H',1,"C1004S1","","","",'S',290,160,160,40,30,30,8,4.37088,0,0,1,4,2,2,20,200,-35,60,70},
	{127,4,'H',1,"C1204S1","","","",'S',370,200,200,40,30,30,8,6.50608,0,0,2,4,2,2,20,240,-50,100,100,1,1,1,20,290,0,0,0},
	{180,4,'H',1,"C1804S1","","","",'S',450,280,280,60,30,30,10,14.13,0,0,2,3,3,1,20,370,-80,0,80,4,2,2,20,320,-40,100,80},
	{180,5,'H',1,"C1805S1","","","",'S',450,280,280,60,30,30,12,16.956,0,0,2,6,3,2,20,320,-80,100,80,2,2,1,20,370,-40,0,80},
	{203,5,'H',1,"C2005S1","","","",'S',500,330,310,70,40,40,10,17.6625,0,0,1,6,3,2,24,380,-90,80,90},
	{203,6,'H',1,"C2006S1","","","",'S',500,330,320,70,40,40,12,21.666,0,0,1,6,3,2,24,380,-90,80,90},
	{245,5,'H',1,"C2405S1","","","",'S',570,360,370,80,40,40,10,23.71485,0,0,2,3,3,1,24,470,-110,0,110,4,2,2,24,410,-55,120,110},
	{245,6,'H',1,"C2406S1","","","",'S',580,370,370,80,40,40,12,28.95708,0,0,2,6,3,2,24,420,-110,120,110,2,2,1,24,480,-55,0,110},
	{273,5,'H',1,"C2705S1","","","",'S',610,400,410,100,40,40,12,35.05182,0,0,2,3,3,1,24,510,-120,0,120,4,2,2,24,450,-60,120,120},
	{102,4,'H',1,"C1004H1","","","",'H',370,180,170,40,30,30,10,7.26125,0,0,1,6,2,3,20,220,-35,60,70},
	{127,4,'H',1,"C1204H1","","","",'H',430,260,200,60,30,30,12,12.96192,0,0,2,3,3,1,20,350,-50,0,50,4,2,2,20,300,-25,100,50},
	{180,5,'H',1,"C1805H1","","","",'H',530,320,290,60,40,40,14,23.88127,0,0,2,3,3,1,24,430,-80,0,80,4,2,2,24,370,-40,120,80},
	{180,6,'H',1,"C1806H1","","","",'H',540,330,290,60,40,40,16,27.80784,0,0,2,6,3,2,24,380,-80,120,80,2,2,1,24,440,-40,0,80},
	{203,5,'H',1,"C2005H1","","","",'H',580,370,320,70,40,40,16,33.51008,0,0,2,6,3,2,24,420,-90,120,90,2,2,1,24,480,-45,0,90},
	{203,6,'H',1,"C2006H1","","","",'H',590,340,320,70,40,40,16,34.08784,0,0,1,9,3,3,24,390,-90,80,90},
	{245,5,'H',1,"C2405H1","","","",'H',690,420,380,70,40,40,14,39.43212,0,0,2,6,3,2,24,470,-110,120,110,4,2,2,24,530,-55,120,110},
	{245,6,'H',1,"C2406H1","","","",'H',750,420,380,80,40,40,16,50.868,0,0,2,9,3,3,24,470,-110,80,110,2,2,1,24,710,-55,0,110},
	{299,6,'H',1,"C2906H1","","","",'H',800,470,450,100,40,40,16,65.312,0,0,2,9,3,3,24,520,-140,120,140,4,2,2,24,580,-70,120,140},
	{299,7,'H',1,"C2907H1","","","",'H',860,470,460,100,40,40,16,71.29056,0,0,2,9,3,3,24,520,-140,120,140,6,2,3,20,580,-70,120,140},*/
	/* 因更换插板库注释掉(2011.10.22版本) wjh-2013.6.5
	{89,4,'H',5,"C0804S5","","","",'S',160,90,150,40,30,30,6,1.73,0,0,1,2,2,1,20,130,-30,60,60},
	{168,5,'H',5,"C1605S5","","","",'S',280,150,260,50,30,30,8,6.33,0,0,1,4,2,2,20,190,-70,60,140},
	{114,4,'H',5,"C1104S5","","","",'S',180,110,175,40,30,30,8,2.88,0,0,1,2,2,1,20,150,-35,50,70},
	{140,4,'H',5,"C1404S5","","","",'S',250,130,200,40,30,30,8,4.40,0,0,2,1,1,1,20,220,0,50,40,2,2,1,20,170,-40,50,80},
	{159,4,'H',5,"C1504S5","","","",'S',260,140,220,40,30,30,8,4.90,0,0,2,2,2,1,20,180,-45,80,90,1,1,1,20,230,0,80,90},
	{159,5,'H',5,"C1505S5","","","",'S',270,140,245,50,30,30,8,5.85,0,0,1,4,2,2,20,180,-60,60,120},
	{168,4,'H',5,"C1604S5","","","",'S',270,150,240,50,30,30,8,5.77,0,0,2,2,2,1,20,190,-60,60,120,1,1,1,20,240,0,60,120},
	{194,5,'H',5,"C1905S5","","","",'S',320,170,300,50,40,40,8,8.04,0,0,1,4,2,2,24,220,-80,60,160},
	{219,5,'H',5,"C2105S5","","","",'S',340,190,330,55,40,40,8,9.39,0,0,1,4,2,2,24,240,-90,60,180},
	{219,6,'H',5,"C2106S5","","","",'S',360,190,335,55,40,40,10,12.58,0,0,1,4,2,2,24,240,-90,80,180},
	{273,6,'H',5,"C2706S5","","","",'S',400,230,400,65,40,40,12,19.97,0,0,1,4,2,2,24,280,-90,80,180},
	{325,6,'H',5,"C3206S5","","","",'S',480,270,445,75,40,40,12,26.90,0,0,2,1,1,1,24,380,0,120,220,4,2,2,24,320,-110,120,220},
	{325,7,'H',5,"C3207S5","","","",'S',430,260,470,90,40,40,12,26.33,0,0,1,6,3,2,24,310,-140,80,140},
	{356,6,'H',5,"C3506S5","","","",'S',420,250,475,80,40,40,12,25.12,0,0,1,6,3,2,24,300,-140,80,140},
	{356,7,'H',5,"C3507S5","","","",'S',540,290,510,100,40,40,12,36.12,0,0,2,4,2,2,24,340,-80,160,160,3,3,1,24,420,-160,160,160},
	{377,7,'H',5,"C3707S5","","","",'S',550,300,530,110,40,40,12,38.86,0,0,2,3,3,1,24,430,-170,160,170,4,2,2,24,350,-85,160,170},
	{406,7,'H',5,"C4007S5","","","",'S',540,330,570,120,40,40,12,41.20,0,0,2,4,2,2,24,380,-90,120,180,3,3,1,24,440,-180,120,180},
	{377,8,'H',5,"C3708S5","","","",'S',550,340,540,110,40,40,12,39.38,0,0,2,6,3,2,24,390,-170,120,170,2,2,1,24,450,-85,120,170},
	{406,8,'H',5,"C4008S5","","","",'S',580,370,570,120,40,40,12,44.26,0,0,2,2,2,1,24,480,-90,120,180,6,3,2,24,420,-180,120,180},
	{89,4,'H',7,"C0804S7","","","",'S',240,110,150,40,30,30,8,3.47,0,0,2,2,2,1,20,150,-30,180,60,1,1,1,20,210,0,180,60},
	{114,4,'H',7,"C1104S7","","","",'S',270,140,175,40,30,30,8,4.32,0,0,2,1,1,1,20,240,0,180,60,2,2,1,20,180,-35,180,70},
	{140,4,'H',7,"C1404S7","","","",'S',300,170,200,40,30,30,8,5.28,0,0,1,4,2,2,20,210,-40,60,80},
	{159,4,'H',7,"C1504S7","","","",'S',310,190,220,40,30,30,8,5.84,0,0,1,4,2,2,20,230,-45,50,90},
	{159,5,'H',7,"C1505S7","","","",'S',320,190,245,50,30,30,8,6.93,0,0,1,6,3,2,20,230,-60,60,60},
	{168,4,'H',7,"C1604S7","","","",'S',370,200,240,50,30,30,8,7.90,0,0,2,4,2,2,20,240,-60,100,120,1,1,1,20,290,0,120,160},
	{168,5,'H',7,"C1605S7","","","",'S',330,200,260,50,30,30,8,7.46,0,0,1,6,3,2,20,240,-70,60,70},
	{194,5,'H',7,"C1905S7","","","",'S',430,220,300,50,40,40,8,10.05,0,0,2,4,2,2,24,270,-80,120,160,1,1,1,24,330,0,120,160},
	{219,5,'H',7,"C2105S7","","","",'S',420,250,330,55,40,40,8,11.61,0,0,1,6,3,2,24,300,-90,80,90},
	{219,6,'H',7,"C2106S7","","","",'S',420,250,335,55,40,40,10,14.67,0,0,1,6,3,2,24,300,-90,80,90},
	{273,6,'H',7,"C2706S7","","","",'S',520,310,400,65,40,40,10,21.63,0,0,2,4,2,2,24,360,-60,120,120,3,3,1,24,420,-120,120,120},
	{325,6,'H',7,"C3206S7","","","",'S',570,360,445,75,40,40,12,31.95,0,0,2,3,3,1,24,470,-140,120,140,4,2,2,24,410,-70,120,140},
	{325,7,'H',7,"C3207S7","","","",'S',570,360,470,90,40,40,12,34.90,0,0,2,6,3,2,24,410,-140,120,140,2,2,1,24,470,-70,120,140},
	{356,6,'H',7,"C3506S7","","","",'S',610,400,475,80,40,40,12,36.49,0,0,2,2,2,1,24,510,-80,120,160,6,3,2,24,450,-160,120,160},
	{356,7,'H',7,"C3507S7","","","",'S',650,400,510,100,40,40,12,43.47,0,0,1,9,3,3,24,450,-160,80,160},
	{377,7,'H',7,"C3707S7","","","",'S',670,420,530,110,40,40,12,47.34,0,0,1,9,3,3,24,470,-175,80,175},
	{406,7,'H',7,"C4007S7","","","",'S',660,450,570,120,40,40,12,50.36,0,0,2,6,3,2,24,500,-120,120,120,4,4,1,24,560,-180,120,120},
	{377,8,'H',7,"C3708S7","","","",'S',620,410,540,110,40,40,14,51.78,0,0,2,4,4,1,24,520,-165,120,110,6,3,2,24,460,-110,120,110},
	{406,8,'H',7,"C4008S7","","","",'S',660,450,570,120,40,40,14,58.75,0,0,2,8,4,2,24,500,-180,120,120,3,3,1,24,560,-120,120,120},
	{89,4,'H',7,"C0804H7","","","",'H',260,130,155,40,30,30,8,3.84,0,0,1,4,2,2,20,170,-30,60,60},
	{114,4,'H',7,"C1104H7","","","",'H',330,160,180,40,30,30,8,5.39,0,0,2,4,2,2,20,200,-40,100,80,1,1,1,20,250,0,100,80},
	{140,4,'H',7,"C1404H7","","","",'H',320,190,210,40,30,30,8,5.83,0,0,1,6,3,2,20,230,-50,60,50},
	{159,4,'H',7,"C1504H7","","","",'H',340,210,240,50,30,30,10,9.07,0,0,1,6,3,2,20,250,-60,60,60},
	{159,5,'H',7,"C1505H7","","","",'H',420,210,240,50,40,40,10,11.01,0,0,1,6,3,2,24,300,-80,80,80},
	{168,4,'H',7,"C1604H7","","","",'H',350,220,250,50,30,30,10,9.62,0,0,1,6,3,2,20,260,-70,60,70},
	{168,5,'H',7,"C1605H7","","","",'H',430,220,250,50,40,40,10,11.81,0,0,2,4,2,2,24,270,-70,120,140,1,1,1,24,330,0,120,140},
	{194,5,'H',7,"C1905H7","","","",'H',420,250,280,50,40,40,10,12.53,0,0,1,6,3,2,24,300,-80,80,80},
	{219,5,'H',7,"C2105H7","","","",'H',460,290,305,55,40,40,10,14.99,0,0,1,6,3,2,24,340,-90,80,90},
	{219,6,'H',7,"C2106H7","","","",'H',500,290,310,55,40,40,12,19.78,0,0,2,4,2,2,24,340,-50,120,100,3,3,1,24,400,-100,120,100},
	{273,6,'H',7,"C2706H7","","","",'H',560,350,375,65,40,40,12,26.64,0,0,2,6,3,2,24,400,-120,120,120,2,2,1,24,460,-60,120,120},
	{325,6,'H',7,"C3206H7","","","",'H',690,420,440,75,40,40,12,38.35,0,0,2,6,3,2,24,470,-140,120,140,4,2,2,24,530,-70,120,140},
	{356,6,'H',7,"C3506H7","","","",'H',630,420,480,80,40,40,12,39.13,0,0,2,8,4,2,24,470,-165,120,110,3,3,1,24,530,-110,120,110},
	{356,7,'H',7,"C3507H7","","","",'H',690,440,510,105,40,40,12,37.98,0,0,1,12,4,3,24,490,-150,80,100},
	{325,7,'H',7,"C3207H7","","","",'H',620,410,470,100,40,40,12,46.80,0,0,2,8,4,2,24,460,-150,120,100,3,3,1,24,520,-100,120,100},
	{377,7,'H',7,"C3707H7","","","",'H',750,480,535,110,40,40,12,53.34,0,0,2,8,4,2,24,530,-180,120,120,6,3,2,24,590,-120,120,120},
	{406,7,'H',7,"C4007H7","","","",'H',750,480,575,120,40,40,12,57.58,0,0,2,6,3,2,24,590,-130,120,130,8,4,2,24,530,-195,120,130},
	{89,4,'H',1,"C0804S1","","","",'S',280,150,150,40,30,30,8,4.00,0,0,2,2,2,1,20,190,-35,120,70,1,1,1,20,250,0,120,70},
	{114,4,'H',1,"C1104S1","","","",'S',320,190,170,40,30,30,8,5.00,0,0,1,4,2,2,20,230,-35,60,70},
	{140,4,'H',1,"C1404S1","","","",'S',360,230,215,50,30,30,8,7.10,0,0,2,3,3,1,20,270,-60,120,60,2,2,1,20,330,-30,120,60},
	{159,4,'H',1,"C1504S1","","","",'S',390,260,240,60,30,30,8,8.80,0,0,1,6,3,2,20,300,-70,60,70},
	{159,5,'H',1,"C1505S1","","","",'S',430,260,250,60,30,30,10,12.50,0,0,2,4,2,2,20,300,-35,100,70,3,3,1,20,350,-70,100,70},
	{168,4,'H',1,"C1604S1","","","",'S',400,270,250,60,30,30,8,9.30,0,0,1,6,3,2,20,310,-80,60,80},
	{168,5,'H',1,"C1605S1","","","",'S',440,270,260,60,30,30,10,13.10,0,0,2,6,3,2,20,310,-80,100,80,2,2,1,20,360,-40,100,80},
	{194,5,'H',1,"C1905S1","","","",'S',480,310,290,70,40,40,10,16.20,0,0,1,6,3,2,24,360,-80,80,80},
	{219,5,'H',1,"C2105S1","","","",'S',520,350,310,80,40,40,10,19.20,0,0,1,6,3,2,24,400,-100,80,100},
	{219,6,'H',1,"C2106S1","","","",'S',560,350,330,80,40,40,12,25.80,0,0,2,4,2,2,24,400,-50,120,100,3,3,1,24,460,-100,120,100},
	{273,6,'H',1,"C2706S1","","","",'S',640,430,410,100,40,40,12,36.80,0,0,2,6,3,2,24,480,-120,120,120,2,2,1,24,540,-60,120,120},
	{89,4,'H',1,"C0804H1","","","",'H',340,170,160,40,30,30,10,6.40,0,0,2,4,2,2,20,210,-35,100,70,1,1,1,20,260,0,100,70},
	{114,4,'H',1,"C1104H1","","","",'H',400,210,185,40,30,30,10,8.30,0,0,1,6,2,3,20,240,-35,60,70},
	{140,4,'H',1,"C1404H1","","","",'H',430,260,220,50,30,30,12,13.00,0,0,2,6,3,2,20,300,-60,100,60,2,2,1,20,350,-30,100,60},
	{159,4,'H',1,"C1504H1","","","",'H',460,290,250,60,30,30,12,16.00,0,0,2,2,2,1,20,380,-35,100,70,6,3,2,20,330,-70,100,70},
	{159,5,'H',1,"C1505H1","","","",'H',460,290,250,60,40,40,14,18.70,0,0,1,6,3,2,24,340,-70,80,70},
	{168,4,'H',1,"C1604H1","","","",'H',460,290,260,60,30,30,12,16.50,0,0,2,6,3,2,20,330,-80,100,80,2,2,1,20,380,-40,100,80},
	{168,5,'H',1,"C1605H1","","","",'H',480,310,260,60,40,40,14,20.00,0,0,1,6,3,2,24,360,-80,80,80},
	{194,5,'H',1,"C1905H1","","","",'H',560,350,300,70,40,40,14,27.10,0,0,2,4,2,2,24,400,-40,120,80,3,3,1,24,460,-80,80,80},
	{219,5,'H',1,"C2105H1","","","",'H',610,400,340,80,40,40,14,33.30,0,0,2,6,3,2,24,450,-100,120,100,2,2,1,24,510,-50,120,100},
	{219,6,'H',1,"C2106H1","","","",'H',620,350,340,80,40,40,14,34.10,0,0,2,4,2,2,24,460,-50,120,100,6,3,2,24,400,-100,120,100},
	{273,6,'H',1,"C2706H1","","","",'H',760,430,420,100,40,40,14,51.80,0,0,2,9,3,3,24,480,-120,120,120,4,2,2,24,540,-60,120,120},
	/*{89,4,'S',1,"C89X4S",240,110,153,40,30,30,8,3.51,1,3,2,1,20,150,-30,60,60},
	{114,4,'S',1,"C114X4S",270,140,178,40,30,30,8,4.38,1,3,2,1,20,180,-35,60,70},
	{140,4,'S',1,"C140X4S",300,170,204,40,30,30,8,5.35,1,4,2,2,20,210,-40,60,80},
	{159,4,'S',1,"C159X4S",320,190,223,40,30,30,8,6.09,1,4,2,2,20,230,-45,60,90},
	{168,4,'S',1,"C168X4S",390,200,244,50,30,30,8,8.43,2,4,2,2,20,240,-90,120,180,1,1,1,20,300,0,0,0},
	{159,5,'S',1,"C159X5S",440,190,247,60,30,30,8,10.41,2,4,2,2,20,290,-100,120,200,2,1,2,20,230,0,120,0},
	{168,5,'S',1,"C168X5S",450,200,262,65,30,30,8,11.08,2,4,2,2,20,300,-100,120,200,2,1,2,20,240,0,120,0},
	{194,5,'S',1,"C194X5S",450,220,300,75,30,30,8,12.27,2,4,2,2,24,260,-120,160,240,1,1,1,24,340,0,120,0},
	{219,5,'S',1,"C219X5S",560,250,331,80,30,30,8,17.27,2,4,2,2,24,370,-130,160,260,2,1,2,24,290,0,160,0},
	{219,6,'S',1,"C219X6S",560,250,335,80,30,30,10,21.76,2,4,2,2,24,370,-130,160,260,2,1,2,24,290,0,160,0},
	{273,6,'S',1,"C273X6S",620,310,401,90,30,30,10,28.28,1,7,2,3,24,350,-150,80,300},
	{325,6,'S',1,"C325X6S",670,360,445,80,30,30,12,38.18,1,7,2,3,24,400,-170,80,340},
	{356,6,'S',1,"C356X6S",790,400,476,80,30,30,12,47.33,2,6,2,3,24,440,-190,160,380,2,1,2,24,520,0,160,0},
	{325,7,'S',1,"C325X7S",750,360,469,100,30,30,12,47.27,2,6,2,3,24,400,-180,160,360,2,1,2,24,480,0,160,0},
	{356,7,'S',1,"C356X7S",790,400,512,110,30,30,12,54.47,1,9,2,4,24,440,-200,80,400},
	{377,7,'S',1,"C377X7S",810,420,533,110,30,30,12,57.46,1,9,2,4,24,460,-220,80,440},
	{406,7,'S',1,"C406X7S",760,450,574,120,30,30,12,58.28,2,6,3,2,24,490,-250,160,250,4,2,2,24,570,-125,160,250},
	{377,8,'S',1,"C377X8S",720,410,537,110,30,30,14,59.90,2,6,3,2,24,450,-220,160,220,4,2,2,24,530,-110,160,220},
	{406,8,'S',1,"C406X8S",760,450,572,115,30,30,14,66.99,2,9,3,3,24,490,-220,80,220,2,2,1,24,730,-110,160,220},
	{426,8,'S',1,"C426X8S",780,470,598,120,30,30,14,71.84,1,12,3,4,24,510,-220,80,220},
	{426,10,'S',1,"C426X10S",850,460,658,170,30,30,14,93.23,2,12,3,4,24,500,-220,80,220,2,2,1,24,820,-110,160,220},
	{89,4,'H',1,"C89X4H",260,130,153,40,30,30,8,3.80,1,4,2,2,20,170,-30,60,60},
	{114,4,'H',1,"C114X4H",350,160,178,40,30,30,8,5.67,2,4,2,2,20,200,-70,120,140,1,1,1,20,260,0,0,0},
	{140,4,'H',1,"C140X4H",440,190,204,40,30,30,8,7.85,2,4,2,2,20,290,-80,120,160,2,1,2,20,230,0,120,160},
	{159,4,'H',1,"C159X4H",460,210,229,50,30,30,10,12.24,2,4,2,2,20,310,-90,120,180,2,1,2,20,250,0,120,0},
	{168,4,'H',1,"C168X4H",470,220,248,50,30,30,10,12.84,2,4,2,2,20,320,-100,120,200,2,1,2,20,260,0,120,0},
	{159,5,'H',1,"C159X5H",440,210,239,50,30,30,10,11.71,2,4,2,2,20,250,-90,160,180,1,1,1,20,330,0,0,0},
	{168,5,'H',1,"C168X5H",450,220,248,50,30,30,10,12.29,2,4,2,2,20,260,-100,160,200,1,1,1,20,340,0,0,0},
	{194,5,'H',1,"C194X5H",560,250,274,50,30,30,10,16.44,2,4,2,2,24,370,-120,160,240,2,1,2,24,290,0,160,0},
	{219,5,'H',1,"C219X5H",600,290,305,55,30,30,10,19.55,2,4,2,2,24,410,-130,160,260,2,1,2,24,320,0,160,0},
	{219,6,'H',1,"C219X6H",600,290,309,55,30,30,12,23.63,1,7,2,3,24,330,-120,80,240},
	{273,6,'H',1,"C273X6H",740,350,375,65,30,30,12,35.20,2,6,2,3,24,390,-150,160,300,1,1,2,24,470,0,160,0},
	{325,6,'H',1,"C325X6H",730,420,439,75,30,30,12,40.50,2,6,3,2,24,460,-180,160,180,4,2,2,24,540,-90,160,180},
	{356,6,'H',1,"C356X6H",760,450,476,80,30,30,12,45.53,2,9,3,3,24,490,-200,80,200,2,2,1,24,730,-100,80,200},
	{325,7,'H',1,"C325X7H",720,410,469,100,30,30,12,45.37,2,9,3,3,24,450,-180,80,180,2,2,1,24,690,-90,0,180},
	{356,7,'H',1,"C356X7H",750,440,506,105,30,30,12,50.59,1,12,3,4,24,480,-200,80,200},
	{377,7,'H',1,"C377X7H",870,480,533,110,30,30,12,61.71,3,6,3,2,24,520,-210,80,210,4,2,2,24,680,-115,160,210,3,3,1,24,760,-210,0,210},
	{406,7,'H',1,"C406X7H",900,510,574,120,30,30,12,69.01,2,12,3,4,24,550,-220,80,220,2,2,1,24,870,-110,0,220},
	{406,8,'H',1,"C406X8H",990,520,614,150,30,30,14,99.44,3,9,3,3,24,560,-240,80,240,4,2,2,24,800,-120,160,240,3,3,1,24,880,-240,0,240},
	{426,10,'H',1,"C426X10H",940,470,638,170,30,30,14,101.03,2,15,3,5,24,510,-250,80,250,2,2,1,24,910,-125,0,250},*/
};
//U型插板类型库
int uend_param_N=56;
INSERT_PLATE_PARAM uend_param_table[200] =
{
	//更新时间 wht 2015-12-08
	{102,4,'H',7,"U1004H7","","","",'H',250,140,140,14,25,25,8,4.69983,0,0,2,2,2,1,16,175,-30,50,60,1,1,1,16,225,0,0,0},
	{114,4,'H',7,"U1104H7","","","",'H',280,150,155,14,30,30,8,5.94,0,0,2,2,2,1,20,190,-30,0,60,1,1,1,20,240,0,0,0},
	{127,4,'H',7,"U1204H7","","","",'H',290,170,160,14,30,30,8,6.175074,0,0,2,2,2,1,20,210,-50,0,100,1,1,1,20,260,0,0,0},
	{140,4,'H',7,"U1404H7","","","",'H',310,190,180,14,30,30,8,7.57,0,0,2,2,2,1,20,230,-35,0,70,1,1,1,20,280,0,0,0},
	{159,4,'H',7,"U1504H7","","","",'H',330,210,200,16,30,30,8,8.92,0,0,2,2,2,1,20,250,-40,50,80,1,1,1,20,300,0,0,0},
	{159,5,'H',7,"U1505H7","","","",'H',340,210,200,16,30,30,10,11.317199,0,0,1,4,2,2,20,250,-40,60,80},
	{168,4,'H',7,"U1604H7","","","",'H',340,220,210,16,30,30,8,12.36,0,0,2,2,2,1,20,260,-45,0,90,1,1,1,20,310,0,0,0},
	{168,5,'H',7,"U1605H7","","","",'H',350,220,210,16,30,30,10,12.212759,0,0,1,4,2,2,20,260,-45,60,90},
	{180,5,'H',7,"U1805H7","","","",'H',400,230,220,14,30,30,10,14.68,0,0,2,4,2,2,20,270,-60,100,120,1,1,1,20,320,0,0,0},
	{180,6,'H',7,"U1806H7","","","",'H',410,240,230,16,30,30,10,15.71,0,0,2,4,2,2,20,280,-60,100,120,1,1,1,20,330,0,0,0},
	{194,5,'H',7,"U1905H7","","","",'H',420,250,235,14,30,30,10,16.191354,0,0,2,4,2,2,20,290,-50,100,100,1,1,1,20,340,0,0,0},
	{194,6,'H',7,"U1906H7","","","",'H',410,280,250,16,30,30,10,16.5,0,0,1,6,3,2,20,320,-85,60,85},
	{203,5,'H',7,"U2005H7","","","",'H',430,260,250,16,30,30,10,17.86,0,0,2,4,2,2,20,300,-60,100,120,1,1,1,20,350,0,0,0},
	{203,6,'H',7,"U2006H7","","","",'H',400,270,250,18,30,30,12,20.02,0,0,1,6,3,2,20,310,-70,60,70},
	{219,5,'H',7,"U2105H7","","","",'H',460,290,260,16,30,30,10,19.8,0,0,2,4,2,2,20,330,-60,100,120,1,1,1,20,380,0,0,0},
	{219,6,'H',7,"U2106H7","","","",'H',420,290,260,18,30,30,12,21.8,0,0,1,6,3,2,20,330,-70,60,70},
	{245,5,'H',7,"U2405H7","","","",'H',400,270,285,20,30,30,10,19.02,0,0,1,6,3,2,20,310,-90,60,90},
	{245,6,'H',7,"U2406H7","","","",'H',490,320,285,20,30,30,12,27.65,0,0,2,4,2,2,20,360,-50,100,100,3,3,1,20,410,-100,0,100},
	{273,5,'H',7,"U2705H7","","","",'H',490,320,315,16,30,30,12,25.47,0,0,2,4,2,2,20,360,-60,100,120,3,3,1,20,410,-120,0,120},
	{273,6,'H',7,"U2706H7","","","",'H',510,340,315,18,30,30,12,31.75,0,0,2,6,3,2,20,380,-100,100,100,2,2,1,20,430,-50,0,100},
	{299,6,'H',7,"U2906H7","","","",'H',550,360,350,16,30,30,12,37.92,0,0,1,9,3,3,20,400,-100,60,100},
	{299,7,'H',7,"U2907H7","","","",'H',590,370,360,16,30,30,12,37.92,0,0,2,6,3,2,20,410,-120,100,120,4,2,2,20,460,-60,100,120},
	{325,6,'H',7,"U3206H7","","","",'H',570,380,365,16,30,30,12,40.92,0,0,1,9,3,3,20,420,-120,60,120},
	{325,7,'H',7,"U3207H7","","","",'H',630,410,365,18,30,30,12,45.04,0,0,2,6,3,2,20,450,-120,100,120,4,2,2,20,500,-60,100,120},
	{356,6,'H',7,"U3506H7","","","",'H',670,450,400,16,30,30,12,52.37,0,0,2,6,3,2,20,490,-120,100,120,4,2,2,20,540,-60,100,120},
	{356,7,'H',7,"U3507H7","","","",'H',600,430,400,18,30,30,12,46.991628,0,0,2,8,4,2,20,470,-150,100,100,3,3,1,20,520,-100,0,100},
	{356,8,'H',7,"U3508H7","","","",'H',670,460,400,24,40,40,14,61.1,0,0,2,6,3,2,24,510,-130,120,130,2,2,1,24,570,-65,0,130},
	{377,7,'H',7,"U3707H7","","","",'H',660,450,420,24,40,40,12,54.2,0,0,2,4,2,2,24,500,-65,120,130,3,3,1,24,560,-130,0,130},
	{377,8,'H',7,"U3708H7","","","",'H',690,480,420,24,40,40,14,66,0,0,2,6,3,2,24,530,-130,120,130,2,2,1,24,590,-65,0,130},
	{406,7,'H',7,"U4007H7","","","",'H',720,510,450,24,40,40,12,63.16,0,0,2,4,2,2,24,560,-65,120,130,3,3,1,24,620,-130,0,130},
	{406,8,'H',7,"U4008H7","","","",'H',690,480,450,24,40,40,14,60.61,0,0,2,6,3,2,24,530,-140,120,140,2,2,1,24,590,-70,0,140},
	{406,9,'H',7,"U4009H7","","","",'H',790,520,450,24,40,40,14,81.090882,0,0,2,6,3,2,24,570,-140,120,140,4,2,2,24,630,-70,120,140},
	{426,8,'H',7,"U4208H7","","","",'H',770,520,470,24,40,40,14,82.13,0,0,1,9,3,3,24,570,-150,80,150},
	{426,9,'H',7,"U4209H7","","","",'H',810,540,470,24,40,40,14,86.26,0,0,2,6,3,2,24,590,-160,120,160,4,2,2,24,650,-80,120,160},
	{102,4,'H',1,"U1004H1","","","",'H',340,210,140,18,30,30,8,6.42,0,0,2,2,2,1,20,250,-40,0,80,1,1,1,20,310,0,0,0},
	{114,4,'H',1,"U1104H1","","","",'H',340,210,150,18,30,30,8,6.9,0,0,2,2,2,1,20,250,-40,0,80,1,1,1,20,310,0,0,0},
	{127,4,'H',1,"U1204H1","","","",'H',350,220,160,18,30,30,8,7.443967,0,0,1,4,2,2,20,260,-40,60,80},
	{140,4,'H',1,"U1404H1","","","",'H',350,220,180,18,30,30,8,8.374463,0,0,1,4,2,2,20,260,-45,60,90},
	{159,4,'H',1,"U1504H1","","","",'H',360,230,200,18,30,30,8,9.7,0,0,1,4,2,2,20,270,-50,60,100},
	{159,5,'H',1,"U1505H1","","","",'H',420,250,200,20,30,30,10,13.927845,0,0,2,4,2,2,20,290,-40,100,80,1,1,1,20,340,0,0,0},
	{168,4,'H',1,"U1604H1","","","",'H',410,240,200,18,30,30,8,10.812159,0,0,2,4,2,2,20,280,-40,100,80,1,1,1,20,330,0,0,0},
	{168,5,'H',1,"U1605H1","","","",'H',460,270,200,20,30,30,10,15.183845,0,0,1,6,2,3,20,310,-40,60,80},
	{180,5,'H',1,"U1805H1","","","",'H',420,290,230,20,30,30,10,16.017022,0,0,1,6,3,2,20,330,-75,60,75},
	{180,6,'H',1,"U1806H1","","","",'H',460,290,230,20,30,30,10,17.51,0,0,2,4,2,2,20,330,-40,100,80,3,3,3,20,380,-80,0,80},
	{194,5,'H',1,"U1905H1","","","",'H',480,310,240,20,30,30,10,18.974214,0,0,2,4,2,2,20,350,-45,100,90,3,3,1,20,400,-90,0,90},
	{194,6,'H',1,"U1906H1","","","",'H',500,330,240,20,30,30,12,23.744402,0,0,2,6,3,2,20,370,-75,100,75,2,2,1,20,420,-37.5,0,75},
	{203,5,'H',1,"U2005H1","","","",'H',510,340,250,20,30,30,10,20.942306,0,0,2,4,2,2,20,380,-50,100,100,3,3,1,20,430,-100,0,100},
	{203,6,'H',1,"U2006H1","","","",'H',610,360,260,20,40,40,10,25.861999,0,0,1,6,2,3,24,410,-70,80,140},
	{219,5,'H',1,"U2105H1","","","",'H',510,340,260,20,30,30,10,21.779999,0,0,2,4,2,2,20,380,-55,100,110,3,3,1,20,430,-100,0,100},
	{219,6,'H',1,"U2106H1","","","",'H',610,360,260,20,40,40,12,31.1,0,0,1,6,2,3,24,410,-80,80,160},
	{245,5,'H',1,"U2405H1","","","",'H',640,390,285,20,40,40,10,29.691079,0,0,1,6,2,3,24,440,-80,80,160},
	{245,6,'H',1,"U2406H1","","","",'H',570,400,310,22,40,40,12,34.849873,0,0,1,6,3,2,24,450,-105,80,105},
	{273,5,'H',1,"U2705H1","","","",'H',590,420,310,24,40,40,12,35.92,0,0,1,6,3,2,24,470,-90,80,90},
	{273,6,'H',1,"U2706H1","","","",'H',630,420,320,20,40,40,12,39.43,0,0,2,6,3,2,24,470,-110,120,110,2,2,1,24,530,-55,0,110},
	{299,6,'H',1,"U2906H1","","","",'H',630,420,350,24,40,40,12,43.19,0,0,2,6,3,2,24,470,-120,120,120,2,2,1,24,530,-60,0,120},
	{299,7,'H',1,"U2907H1","","","",'H',700,490,360,26,40,40,14,57.37,0,0,2,6,3,2,24,540,-120,120,120,2,2,1,24,600,-60,0,120},
	/* 因更换插板库注释掉(2013.6.5版本) wht 2015.12.08
	{114,4,'H',7,"U1104H7","","","",'H',280,150,155,14,30,30,8,5.787423,0,0,2,2,2,1,20,190,-30,120,60,1,1,1,20,250,0,120,60},
	{140,4,'H',7,"U1404H7","","","",'H',310,190,180,14,30,30,8,7.399118,0,0,2,1,1,1,20,280,0,120,60,2,2,1,20,230,-35,120,70},
	{159,4,'H',7,"U1504H7","","","",'H',330,210,200,16,30,30,8,8.763101,0,0,2,2,2,1,20,250,-40,120,80,1,1,1,20,300,0,120,80},
	{168,4,'H',7,"U1604H7","","","",'H',340,220,210,16,30,30,8,9.465016,0,0,2,1,1,1,20,310,0,120,80,2,2,1,20,260,-45,120,90},
	{159,5,'H',7,"U1505H7","","","",'H',340,210,200,16,30,30,10,11.317199,0,0,1,4,2,2,20,250,-40,60,80},
	{168,5,'H',7,"U1605H7","","","",'H',350,220,210,16,30,30,10,12.212759,0,0,1,4,2,2,20,260,-45,60,90},
	{194,5,'H',7,"U1905H7","","","",'H',420,250,235,14,30,30,10,16.191354,0,0,2,4,2,2,20,290,-50,100,100,1,1,1,20,340,0,100,100},
	{219,5,'H',7,"U2105H7","","","",'H',460,290,260,16,30,30,10,19.610759,0,0,2,1,1,1,20,380,0,100,100,4,2,2,20,330,-60,100,120},
	{219,6,'H',7,"U2106H7","","","",'H',420,290,260,18,30,30,12,21.727438,0,0,1,6,3,2,20,330,-70,60,70},
	{273,6,'H',7,"U2706H7","","","",'H',510,340,315,18,30,30,12,31.664767,0,0,2,6,3,2,20,380,-100,100,100,2,2,1,20,430,-50,100,100},
	{325,6,'H',7,"U3206H7","","","",'H',570,380,365,16,30,30,12,40.708863,0,0,1,9,3,3,20,420,-120,60,120},
	{325,7,'H',7,"U3207H7","","","",'H',630,410,365,18,30,30,12,44.942841,0,0,2,6,3,2,20,450,-120,100,120,4,2,2,20,500,-60,100,120},
	{356,6,'H',7,"U3506H7","","","",'H',670,450,400,16,30,30,12,52.148453,0,0,2,6,3,2,20,490,-120,100,120,4,2,2,20,540,-60,100,120},
	{356,7,'H',7,"U3507H7","","","",'H',600,430,400,18,30,30,12,46.991628,0,0,2,8,4,2,20,470,-150,100,100,3,3,1,20,520,-100,100,100},
	{377,7,'H',7,"U3707H7","","","",'H',660,450,420,24,40,40,12,54.461771,0,0,2,4,2,2,24,500,-65,120,130,3,3,1,24,560,-130,120,130},
	{377,8,'H',7,"U3708H7","","","",'H',690,480,420,24,40,40,14,66.453223,0,0,2,6,3,2,24,530,-130,120,130,2,2,1,24,590,-65,120,130},
	{406,7,'H',7,"U4007H7","","","",'H',720,510,450,24,40,40,12,63.438698,0,0,2,3,3,1,24,620,-130,0,130,4,2,2,24,560,-65,120,130},
	{406,8,'H',7,"U4008H7","","","",'H',690,480,450,24,40,40,14,71.199882,0,0,2,2,2,1,24,590,-70,120,140,6,3,2,24,530,-140,120,140},
	{426,8,'H',7,"U4208H7","","","",'H',770,520,470,24,40,40,14,82.628801,0,0,1,9,3,3,24,570,-150,80,150},
	{426,10,'H',7,"U4210H7","","","",'H',730,520,470,26,40,40,16,90.081264,0,0,2,8,4,2,24,570,-180,120,120,3,3,1,24,630,-120,120,120},
	{114,4,'H',1,"U1104H1","","","",'H',340,210,150,18,30,30,8,6.790319,0,0,2,2,2,1,20,250,-40,0,80,1,1,1,20,310,0,0,0},
	{159,4,'H',1,"U1504H1","","","",'H',360,230,200,18,30,30,8,9.556159,0,0,1,4,2,2,20,270,-50,60,100},
	{140,4,'H',1,"U1404H1","","","",'H',350,220,180,18,30,30,8,8.374463,0,0,1,4,2,2,20,260,-45,60,90},
	{159,5,'H',1,"U1505H1","","","",'H',420,250,200,20,30,30,10,13.927845,0,0,2,4,2,2,20,290,-40,100,80,1,1,1,20,340,0,100,80},
	{168,6,'H',1,"U1605H1","","","",'H',460,270,200,20,30,30,10,15.2,0,0,1,6,2,3,20,310,-40,60,80},
	{168,4,'H',1,"U1604H1","","","",'H',410,240,200,18,30,30,8,10.812159,0,0,2,4,2,2,20,280,-40,100,80,1,1,1,20,330,0,100,80},
	{194,5,'H',1,"U1905H1","","","",'H',480,310,240,20,30,30,10,19,0,0,2,3,3,1,20,400,-90,0,90,4,2,2,20,350,-45,100,90},
	{219,5,'H',1,"U2105H1","","","",'H',510,340,260,20,30,30,10,21.8,0,0,2,3,3,1,20,430,-100,0,100,4,2,2,20,380,-55,100,110},
	{219,6,'H',1,"U2106H1","","","",'H',610,360,260,20,40,40,12,31.1,0,0,1,6,2,3,24,410,-80,80,160},
	{273,6,'H',1,"U2706H1","","","",'H',630,420,320,20,40,40,12,39.496643,0,0,2,6,3,2,24,470,-120,120,120,2,2,1,24,530,-60,0,120},
	{194,6,'H',7,"U1906H7","","","",'H',410,280,235,16,30,30,10,15.880359,0,0,1,6,3,2,20,320,-85,60,85},
	{273,5,'H',7,"U2705H7","","","",'H',490,320,315,16,30,30,10,25.242839,0,0,2,3,3,1,20,410,-120,0,120,4,2,2,20,360,-60,100,120},
	{356,8,'H',7,"U3508H7","","","",'H',670,460,400,24,40,40,14,61.530384,0,0,2,6,3,2,24,510,-130,120,130,2,2,1,24,570,-65,0,130},
	{406,9,'H',7,"U4009H7","","","",'H',790,520,450,24,40,40,14,81.090882,0,0,2,6,3,2,24,570,-140,120,140,4,2,2,20,630,-70,120,140},
	{426,9,'H',7,"U4209H7","","","",'H',810,540,470,24,40,40,14,86.761041,0,0,2,6,3,2,24,590,-160,120,160,4,2,2,24,650,-80,120,160},
	{102,4,'H',7,"U1004H7","","","",'H',250,140,140,14,25,25,8,4.69983,0,0,2,2,2,1,16,175,-30,0,60,1,1,1,16,225,0,0,0},
	{127,4,'H',7,"U1204H7","","","",'H',290,170,160,14,30,30,8,6.175074,0,0,2,2,2,1,20,210,-50,0,100,1,1,1,20,260,0,0,0},
	{180,5,'H',7,"U1805H7","","","",'H',400,230,220,14,30,30,10,14.467064,0,0,2,4,2,2,20,270,-60,100,120,1,1,1,20,320,0,0,0},
	{180,6,'H',7,"U1806H7","","","",'H',410,240,230,16,30,30,10,15.542479,0,0,2,4,2,2,20,280,-60,100,120,1,1,1,20,330,0,0,0},
	{203,5,'H',7,"U2005H7","","","",'H',430,260,250,16,30,30,10,17.678999,0,0,2,4,2,2,20,300,-60,100,120,1,1,1,20,350,0,0,0},
	{203,6,'H',7,"U2006H7","","","",'H',400,270,250,18,30,30,12,19.949768,0,0,1,6,3,2,20,310,-70,60,70},
	{245,5,'H',7,"U2405H7","","","",'H',400,270,285,20,30,30,10,18.952279,0,0,1,6,3,2,20,310,-90,60,90},
	{299,6,'H',7,"U2906H7","","","",'H',550,360,350,16,30,30,12,37.717096,0,0,1,9,3,3,20,400,-100,60,100},
	{299,7,'H',7,"U2907H7","","","",'H',590,370,360,16,30,30,12,41.507688,0,0,2,6,3,2,20,410,-120,100,120,4,2,2,20,460,-60,100,120},
	{245,6,'H',7,"U2406H7","","","",'H',490,320,285,20,30,30,12,27.659537,0,0,2,3,3,1,20,410,-100,0,100,4,2,2,20,360,-50,100,100},
	{194,6,'H',1,"U1906H1","","","",'H',500,330,240,20,30,30,12,23.744402,0,0,2,6,3,2,20,370,-90,100,90,2,2,1,20,420,-45,0,90},
	{273,5,'H',1,"U2705H1","","","",'H',590,420,310,24,40,40,12,36.109694,0,0,1,6,3,2,24,470,-90,80,90},
	{102,4,'H',1,"U1004H1","","","",'H',340,210,140,18,30,30,8,6.337631,0,0,2,2,2,1,20,250,-40,0,80,1,1,1,20,310,0,0,0},
	{127,4,'H',1,"U1204H1","","","",'H',350,220,160,18,30,30,8,7.443967,0,0,1,4,2,2,20,260,-40,60,80},
	{180,5,'H',1,"U1805H1","","","",'H',420,290,220,20,30,30,10,15.32063,0,0,1,6,3,2,20,330,-75,60,75},
	{180,6,'H',1,"U1806H1","","","",'H',460,290,230,20,30,30,10,17.461422,0,0,2,3,3,1,20,380,-80,0,80,4,2,2,20,330,-40,100,80},
	{203,5,'H',1,"U2005H1","","","",'H',510,340,250,20,30,30,10,20.942306,0,0,2,3,3,1,20,430,-90,0,90,4,2,2,20,380,-50,100,100},
	{203,6,'H',1,"U2006H1","","","",'H',610,360,260,20,40,40,10,25.861999,0,0,1,6,2,3,24,410,-70,80,140},
	{245,5,'H',1,"U2405H1","","","",'H',640,390,285,20,40,40,10,29.691079,0,0,1,6,2,3,24,440,-80,80,160},
	{245,6,'H',1,"U2406H1","","","",'H',570,400,300,22,40,40,12,33.725684,0,0,1,6,3,2,24,450,-110,80,110},
	{299,6,'H',1,"U2906H1","","","",'H',630,420,350,24,40,40,12,43.40661,0,0,2,6,3,2,24,470,-120,120,120,2,2,1,24,530,-60,0,120},
	{299,7,'H',1,"U2907H1","","","",'H',700,490,360,26,40,40,14,57.875479,0,0,2,6,3,2,24,540,-120,120,120,2,2,1,24,600,-60,0,120},*/
	/* 因更换插板库注释掉(2011.10.22版本) wjh-2013.6.5
	{114,4,'H',7,"U1104H7","","","",'H',280,150,155,14,30,30,8,5.94,0,0,2,2,2,1,20,190,-30,120,60,1,1,1,20,250,0,120,60},
	{140,4,'H',7,"U1404H7","","","",'H',310,190,180,14,30,30,8,7.57,0,0,2,1,1,1,20,280,0,120,60,2,2,1,20,230,-35,120,70},
	{159,4,'H',7,"U1504H7","","","",'H',330,210,200,16,30,30,8,8.92,0,0,2,2,2,1,20,250,-40,120,80,1,1,1,20,300,0,120,80},
	{168,4,'H',7,"U1604H7","","","",'H',340,220,210,16,30,30,8,9.63,0,0,2,1,1,1,20,310,0,120,80,2,2,1,20,260,-45,120,90},
	{159,5,'H',7,"U1505H7","","","",'H',340,210,200,16,30,30,10,11.46,0,0,1,4,2,2,20,250,-40,60,80},
	{168,5,'H',7,"U1605H7","","","",'H',350,220,210,16,30,30,10,12.36,0,0,1,4,2,2,20,260,-45,60,90},
	{194,5,'H',7,"U1905H7","","","",'H',420,250,235,14,30,30,10,16.42,0,0,2,4,2,2,20,290,-50,100,100,1,1,1,20,340,0,100,100},
	{219,5,'H',7,"U2105H7","","","",'H',460,290,260,16,30,30,10,19.80,0,0,2,1,1,1,20,380,0,100,100,4,2,2,20,330,-60,100,120},
	{219,6,'H',7,"U2106H7","","","",'H',420,290,260,18,30,30,12,21.80,0,0,1,6,3,2,20,330,-70,60,70},
	{273,6,'H',7,"U2706H7","","","",'H',510,340,315,18,30,30,12,31.75,0,0,2,6,3,2,20,380,-100,100,100,2,2,1,20,430,-50,100,100},
	{325,6,'H',7,"U3206H7","","","",'H',570,380,365,16,30,30,12,40.92,0,0,1,9,3,3,20,420,-120,60,120},
	{325,7,'H',7,"U3207H7","","","",'H',630,410,365,18,30,30,12,45.04,0,0,2,6,3,2,20,450,-120,100,120,4,2,2,20,500,-60,100,120},
	{356,6,'H',7,"U3506H7","","","",'H',630,410,400,16,30,30,12,49.36,0,0,2,4,2,2,20,500,-65,100,130,6,3,2,20,450,-130,100,130},
	{356,7,'H',7,"U3507H7","","","",'H',600,430,400,18,30,30,12,54.64,0,0,2,8,4,2,20,470,-150,100,100,3,3,1,20,520,-100,100,100},
	{377,7,'H',7,"U3707H7","","","",'H',660,450,420,24,40,40,12,54.20,0,0,2,4,2,2,24,500,-65,120,130,3,3,1,24,560,-130,120,130},
	{377,8,'H',7,"U3708H7","","","",'H',690,480,420,24,40,40,14,66.00,0,0,2,6,3,2,24,530,-130,120,130,2,2,1,24,590,-65,120,130},
	{406,7,'H',7,"U4007H7","","","",'H',690,480,450,24,40,40,12,60.62,0,0,2,6,3,2,24,530,-140,120,140,2,2,1,24,590,-70,120,140},
	{406,8,'H',7,"U4008H7","","","",'H',690,480,450,24,40,40,14,70.72,0,0,2,2,2,1,24,590,-70,120,140,6,3,2,24,530,-140,120,140},
	{426,8,'H',7,"U4208H7","","","",'H',770,520,470,24,40,40,14,82.13,0,0,1,9,3,3,24,570,-150,80,150},
	{426,10,'H',7,"U4210H7","","","",'H',730,520,470,26,40,40,16,89.14,0,0,2,8,4,2,24,570,-180,120,120,3,3,1,24,630,-120,120,120},
	{114,4,'H',1,"U1104H1","","","",'H',340,210,150,18,30,30,8,6.90,0,0,2,2,2,1,20,250,-30,120,60,1,1,1,20,310,0,120,60},
	{159,4,'H',1,"U1504H1","","","",'H',360,230,200,18,30,30,8,9.70,0,0,1,4,2,2,20,270,-40,60,80},
	{140,4,'H',1,"U1404H1","","","",'H',350,220,180,18,30,30,8,8.50,0,0,1,4,2,2,20,260,-35,60,70},
	{159,5,'H',1,"U1505H1","","","",'H',420,250,200,20,30,30,10,14.00,0,0,2,4,2,2,20,290,-40,100,80,1,1,1,20,340,0,100,80},
	{168,6,'H',1,"U1605H1","","","",'H',460,270,200,20,30,30,10,15.2,0,0,1,6,2,3,20,310,-40,60,80},
	{168,4,'H',1,"U1604H1","","","",'H',410,240,200,18,30,30,8,10.90,0,0,2,4,2,2,20,280,-40,100,80,1,1,1,20,330,0,100,80},
	{194,5,'H',1,"U1905H1","","","",'H',480,310,240,20,30,30,10,19.00,0,0,2,4,2,2,20,350,-40,100,80,3,3,1,20,400,-80,100,80},
	{219,5,'H',1,"U2105H1","","","",'H',510,340,260,20,30,30,10,21.80,0,0,2,4,2,2,20,380,-40,100,80,3,3,1,20,430,-80,100,80},
	{219,6,'H',1,"U2106H1","","","",'H',610,360,260,20,40,40,12,31.1,0,0,1,6,2,3,24,410,-60,80,120},
	{273,6,'H',1,"U2706H1","","","",'H',630,420,310,20,40,40,12,38.20,0,0,2,6,3,2,24,470,-100,120,100,2,2,1,24,530,-50,120,100},
	/*{114,4,'H',1,"U114X4H",270,150,154,14,30,30,8,5.71,1,3,2,1,16,190,-30,50,60},
	{140,4,'H',1,"U140X4H",310,190,180,14,30,30,8,7.57,1,3,2,1,20,230,-30,50,60},
	{159,4,'H',1,"U159X4H",340,210,199,16,30,30,8,9.12,1,3,2,1,20,250,-35,60,70},
	{168,4,'H',1,"U168X4H",340,220,208,16,30,30,8,9.54,1,3,2,1,20,260,-35,50,70},
	{159,5,'H',1,"U159X5H",330,210,199,14,30,30,10,11.09,1,4,2,2,20,250,-35,50,70},
	{168,5,'H',1,"U168X5H",340,220,208,16,30,30,10,11.92,1,4,2,2,20,260,-40,50,80},
	{194,5,'H',1,"U194X5H",420,250,234,14,30,30,10,16.35,2,4,2,2,20,290,-40,100,80,1,1,1,20,340,0,0,0},
	{219,5,'H',1,"U219X5H",460,290,259,16,30,30,10,19.72,2,4,2,2,20,330,-45,100,90,1,1,1,20,380,0,0,0},
	{219,6,'H',1,"U219X6H",480,290,259,18,30,30,12,24.64,1,6,2,3,20,330,-45,60,90},
	{273,6,'H',1,"U273X6H",540,350,313,18,30,30,12,33.32,2,6,3,2,20,390,-90,60,90,1,2,1,20,510,-45,60,90},
	{325,6,'H',1,"U325X6H",610,420,365,16,30,30,12,43.67,1,9,3,3,20,460,-120,60,120},
	{356,6,'H',1,"U356X6H",700,450,396,16,30,30,12,54.09,2,6,3,2,20,490,-130,120,130,2,2,2,20,550,-65,120,130},
	{325,7,'H',1,"U325X7H",660,410,365,18,30,30,12,47.10,2,6,3,2,20,450,-120,120,120,2,2,2,20,510,-60,120,120},
	{356,7,'H',1,"U356X7H",700,450,396,18,30,30,12,54.09,2,9,3,3,20,490,-130,60,130,2,2,1,20,670,-65,60,130},
	{377,7,'H',1,"U377X7H",670,480,417,22,30,30,12,54.60,2,4,2,2,24,520,-65,120,130,3,3,1,24,580,-130,60,130},
	{406,7,'H',1,"U406X7H",700,510,446,24,30,30,12,60.92,2,4,2,2,24,550,-70,120,140,3,3,1,24,610,-140,0,140},
	{377,8,'H',1,"U377X8H",670,480,417,22,30,30,14,63.70,2,6,3,2,24,520,-130,60,130,2,2,1,24,640,-65,0,130},
	{406,8,'H',1,"U406X8H",710,520,446,24,30,30,14,72.05,2,6,3,2,24,560,-140,60,140,2,2,1,24,680,-70,0,140},
	{426,8,'H',1,"U426X8H",730,540,466,22,30,30,14,77.33,1,9,3,3,24,580,-150,60,250},
	{426,10,'H',1,"U426X10H",790,540,466,26,30,30,16,95.40,2,9,3,3,24,580,-150,60,150,2,2,1,24,760,-75,0,150},*/
};
//X型插板类型库
//十字型插板存储布置螺栓参数的时候  如果第一组布置螺栓参数的行数列数均为-1，
//则表示该十字插板上的螺栓为双排螺栓,且竖直面和水平面呈交错布置 wht 09-06-06
int xend_param_N=114;
INSERT_PLATE_PARAM xend_param_table[200] =
{	//d t cMaterial level guige L M H W N S Thick weight A R iBoltRect ls_param[6]
	//更新时间 wht 2015-12-08
	{114,4,'S',7,"X1104S7","XK1104S7","XZ1104S7","XL1104S7",'S',190,80,150,6,50,210,8,7.440544,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{127,4,'S',7,"X1204S7","XK1204S7","XZ1204S7","XL1204S7",'S',200,90,160,6,60,210,8,8.6664,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{140,4,'S',7,"X1404S7","XK1404S7","XZ1404S7","XL1404S7",'S',200,90,170,6,70,210,8,9.70888,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'S',7,"X1504S7","XK1504S7","XZ1504S7","XL1504S7",'S',210,100,190,6,80,210,8,11.236176,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'S',7,"X1505S7","XK1505S7","XZ1505S7","XL1505S7",'S',220,110,200,8,80,210,10,15.17562,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,4,'S',7,"X1604S7","XK1604S7","XZ1604S7","XL1604S7",'S',220,110,190,6,80,210,8,11.469792,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,5,'S',7,"X1605S7","XK1605S7","XZ1605S7","XL1605S7",'S',220,110,200,8,80,210,10,15.17562,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{180,4,'S',7,"X1804S7","XK1804S7","XZ1804S7","XL1804S7",'S',220,110,220,6,90,210,8,13.090032,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{180,5,'S',7,"X1805S7","XK1805S7","XZ1805S7","XL1805S7",'S',220,110,220,8,90,210,10,16.92146,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{194,5,'S',7,"X1905S7","XK1905S7","XZ1905S7","XL1905S7",'S',230,120,220,8,90,210,10,17.25901,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{194,6,'S',7,"X1906S7","XK1906S7","XZ1906S7","XL1906S7",'S',250,120,240,8,100,250,10,21.78375,10,20,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{203,5,'S',7,"X2005S7","XK2005S7","XZ2005S7","XL2005S7",'S',270,140,250,8,100,250,10,22.94555,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{203,6,'S',7,"X2006S7","XK2006S7","XZ2006S7","XL2006S7",'S',270,140,260,8,100,250,10,23.36945,10,30,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,5,'S',7,"X2105S7","XK2105S7","XZ2105S7","XL2105S7",'S',270,140,240,8,100,250,10,22.52165,10,20,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,6,'S',7,"X2106S7","XK2106S7","XZ2106S7","XL2106S7",'S',270,140,270,8,100,250,12,25.989152,10,35,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{245,5,'S',7,"X2405S7","XK2405S7","XZ2405S7","XL2405S7",'S',280,150,290,8,120,250,10,27.6006,10,25,2,2,1,2,20,30,60,60,80,2,1,2,20,160,60,60,80},
	{245,6,'S',7,"X2406S7","XK2406S7","XZ2406S7","XL2406S7",'S',280,150,300,8,120,250,12,30.581088,10,30,2,2,1,2,20,30,60,60,80,2,1,2,20,160,60,60,80},
	{273,5,'S',7,"X2705S7","XK2705S7","XZ2705S7","XL2705S7",'S',300,170,320,8,130,250,12,34.07528,10,30,2,2,1,2,20,30,65,60,80,2,1,2,20,160,65,60,80},
	{273,6,'S',7,"X2706S7","XK2706S7","XZ2706S7","XL2706S7",'S',340,170,310,8,130,330,12,41.025984,10,25,3,2,1,2,20,30,100,100,80,2,1,2,20,200,100,100,80,2,1,2,20,80,30,170,80},
	{299,6,'S',7,"X2906S7","XK2906S7","XZ2906S7","XL2906S7",'S',350,180,350,8,150,330,12,47.55216,10,25,3,2,1,2,20,30,120,100,80,2,1,2,20,200,120,100,80,2,1,2,20,80,30,170,80},
	{299,7,'S',7,"X2907S7","XK2907S7","XZ2907S7","XL2907S7",'S',360,190,360,8,150,330,12,48.878496,10,30,3,2,1,2,20,30,120,100,80,2,1,2,20,200,120,100,80,2,1,2,20,80,30,170,80},
	{325,6,'S',7,"X3206S7","XK3206S7","XZ3206S7","XL3206S7",'S',370,200,370,8,160,330,12,51.900432,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{325,7,'S',7,"X3207S7","XK3207S7","XZ3207S7","XL3207S7",'S',370,200,370,8,160,330,12,51.900432,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{356,6,'S',7,"X3506S7","XK3506S7","XZ3506S7","XL3506S7",'S',380,210,390,8,170,330,12,55.675968,10,25,3,2,1,2,20,30,140,100,80,2,1,2,20,200,140,100,80,2,1,2,20,80,30,170,80},
	{356,7,'S',7,"X3507S7","XK3507S7","XZ3507S7","XL3507S7",'S',380,210,390,8,170,330,14,60.174332,10,25,3,2,1,2,20,30,140,100,80,2,1,2,20,200,140,100,80,2,1,2,20,80,30,170,80},
	{377,6,'S',7,"X3706S7","XK3706S7","XZ3706S7","XL3706S7",'S',380,210,410,8,180,330,12,58.765728,10,25,3,2,1,2,20,30,150,100,80,2,1,2,20,200,150,100,80,2,1,2,20,80,30,170,80},
	{377,7,'S',7,"X3707S7","XK3707S7","XZ3707S7","XL3707S7",'S',390,220,410,8,180,330,14,64.388526,10,25,3,2,1,2,20,30,150,100,80,2,1,2,20,200,150,100,80,2,1,2,20,80,30,170,80},
	{406,6,'S',7,"X4006S7","XK4006S7","XZ4006S7","XL4006S7",'S',410,240,440,8,190,330,14,70.521574,10,30,3,2,1,2,20,30,160,100,80,2,1,2,20,200,160,100,80,2,1,2,20,80,30,170,80},
	{406,7,'S',7,"X4007S7","XK4007S7","XZ4007S7","XL4007S7",'S',410,240,440,8,190,330,16,75.993024,10,30,3,2,1,2,20,30,160,100,80,2,1,2,20,200,160,100,80,2,1,2,20,80,30,170,80},
	{426,7,'S',7,"X4207S7","XK4207S7","XZ4207S7","XL4207S7",'S',470,250,460,8,200,430,14,90.004018,10,30,4,2,1,2,20,30,170,100,80,2,1,2,20,300,170,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{426,8,'S',7,"X4208S7","XK4208S7","XZ4208S7","XL4208S7",'S',470,250,460,10,200,430,16,107.372928,10,30,4,2,1,2,20,30,170,100,80,2,1,2,20,300,170,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{114,4,'S',1,"X1104S1","XK1104S1","XZ1104S1","XL1104S1",'S',210,100,150,6,50,210,8,7.807296,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{127,4,'S',1,"X1204S1","XK1204S1","XZ1204S1","XL1204S1",'S',210,100,160,6,60,210,8,8.862336,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{140,4,'S',1,"X1404S1","XK1404S1","XZ1404S1","XL1404S1",'S',220,110,170,6,70,210,8,10.125872,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'S',1,"X1504S1","XK1504S1","XZ1504S1","XL1504S1",'S',230,120,180,8,70,210,10,13.70453,10,20,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,5,'S',1,"X1505S1","XK1505S1","XZ1505S1","XL1505S1",'S',270,140,180,8,70,250,10,16.21025,10,20,2,2,1,2,20,30,35,60,80,2,1,2,20,160,35,60,80},
	{168,4,'S',1,"X1604S1","XK1604S1","XZ1604S1","XL1604S1",'S',240,130,200,8,80,210,10,15.78792,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,5,'S',1,"X1605S1","XK1605S1","XZ1605S1","XL1605S1",'S',280,150,200,8,80,250,10,18.6202,10,20,2,2,1,2,20,30,40,60,80,2,1,2,20,160,40,60,80},
	{180,4,'S',1,"X1804S1","XK1804S1","XZ1804S1","XL1804S1",'S',280,150,220,8,90,250,10,20.7554,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{180,5,'S',1,"X1805S1","XK1805S1","XZ1805S1","XL1805S1",'S',280,150,220,8,90,250,10,20.7554,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{194,5,'S',1,"X1905S1","XK1905S1","XZ1905S1","XL1905S1",'S',290,160,230,8,90,250,12,23.542464,10,25,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{194,6,'S',1,"X1906S1","XK1906S1","XZ1906S1","XL1906S1",'S',330,160,240,8,100,330,12,31.127448,10,20,3,2,1,2,20,30,70,100,80,2,1,2,20,200,70,100,80,2,1,2,20,80,30,170,80},
	{203,5,'S',1,"X2005S1","XK2005S1","XZ2005S1","XL2005S1",'S',300,170,250,8,100,250,12,26.35088,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{203,6,'S',1,"X2006S1","XK2006S1","XZ2006S1","XL2006S1",'S',350,180,260,8,100,330,12,33.32796,10,30,3,2,1,2,20,30,70,100,80,2,1,2,20,200,70,100,80,2,1,2,20,80,30,170,80},
	{219,5,'S',1,"X2105S1","XK2105S1","XZ2105S1","XL2105S1",'S',300,170,250,8,100,250,12,26.35088,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,6,'S',1,"X2106S1","XK2106S1","XZ2106S1","XL2106S1",'S',350,180,270,8,110,330,12,35.64528,10,25,3,2,1,2,20,30,80,100,80,2,1,2,20,200,80,100,80,2,1,2,20,80,30,170,80},
	{245,5,'S',1,"X2405S1","XK2405S1","XZ2405S1","XL2405S1",'S',360,190,290,8,120,330,12,39.157056,10,25,3,2,1,2,20,30,90,100,80,2,1,2,20,200,90,100,80,2,1,2,20,80,30,170,80},
	{245,6,'S',1,"X2406S1","XK2406S1","XZ2406S1","XL2406S1",'S',360,190,300,8,120,330,12,39.835296,10,30,3,2,1,2,20,30,90,100,80,2,1,2,20,200,90,100,80,2,1,2,20,80,30,170,80},
	{273,5,'S',1,"X2705S1","XK2705S1","XZ2705S1","XL2705S1",'S',370,200,320,8,130,330,12,43.441272,10,30,3,2,1,2,20,30,100,100,80,2,1,2,20,200,100,100,80,2,1,2,20,80,30,170,80},
	{273,6,'S',1,"X2706S1","XK2706S1","XZ2706S1","XL2706S1",'S',370,200,330,8,140,330,12,45.796272,10,25,3,2,1,2,20,30,110,100,80,2,1,2,20,200,110,100,80,2,1,2,20,80,30,170,80},
	{299,6,'S',1,"X2906S1","XK2906S1","XZ2906S1","XL2906S1",'S',460,240,350,8,150,430,12,62.217216,10,25,4,2,1,2,16,30,120,100,80,2,1,2,16,300,120,100,80,2,1,2,16,80,30,100,80,2,1,2,16,250,30,100,80},
	{299,7,'S',1,"X2907S1","XK2907S1","XZ2907S1","XL2907S1",'S',460,240,360,8,150,430,12,63.083856,10,30,4,2,1,2,16,30,120,100,80,2,1,2,16,300,120,100,80,2,1,2,16,80,30,100,80,2,1,2,16,250,30,100,80},
	{114,4,'H',7,"X1104H7","XK1104H7","XZ1104H7","XL1104H7",'H',200,90,150,6,50,210,8,7.62392,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{127,4,'H',7,"X1204H7","XK1204H7","XZ1204H7","XL1204H7",'H',210,100,160,6,60,210,8,8.862336,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{140,4,'H',7,"X1404H7","XK1404H7","XZ1404H7","XL1404H7",'H',220,110,170,6,70,210,10,11.23806,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'H',7,"X1504H7","XK1504H7","XZ1504H7","XL1504H7",'H',230,120,190,6,80,210,10,13.01059,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'H',7,"X1505H7","XK1505H7","XZ1505H7","XL1505H7",'H',230,120,200,8,80,210,10,15.48177,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,4,'H',7,"X1604H7","XK1604H7","XZ1604H7","XL1604H7",'H',230,120,190,6,80,210,10,13.01059,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,5,'H',7,"X1605H7","XK1605H7","XZ1605H7","XL1605H7",'H',240,130,200,8,80,210,10,15.78792,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{180,5,'H',7,"X1805H7","XK1805H7","XZ1805H7","XL1805H7",'H',280,140,220,8,90,270,10,21.65972,10,20,3,2,1,2,16,25,65,80,80,2,1,2,16,165,65,80,80,2,1,2,16,65,25,140,80},
	{180,6,'H',7,"X1806H7","XK1806H7","XZ1806H7","XL1806H7",'H',280,140,230,8,90,270,10,22.09932,10,25,3,2,1,2,16,25,65,80,80,2,1,2,16,165,65,80,80,2,1,2,16,65,25,140,80},
	{194,5,'H',7,"X1905H7","XK1905H7","XZ1905H7","XL1905H7",'H',250,140,220,8,90,210,10,17.93411,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{194,6,'H',7,"X1906H7","XK1906H7","XZ1906H7","XL1906H7",'H',280,140,220,8,90,270,10,21.65972,10,20,3,2,1,2,16,25,65,80,80,2,1,2,16,165,65,80,80,2,1,2,16,65,25,140,80},
	{203,5,'H',7,"X2005H7","XK2005H7","XZ2005H7","XL2005H7",'H',280,140,250,8,100,270,10,24.335,10,25,3,2,1,2,16,25,75,80,80,2,1,2,16,165,75,80,80,2,1,2,16,65,25,140,80},
	{203,6,'H',7,"X2006H7","XK2006H7","XZ2006H7","XL2006H7",'H',290,150,260,8,100,270,10,25.17495,10,30,3,2,1,2,16,25,75,80,80,2,1,2,16,165,75,80,80,2,1,2,16,65,25,140,80},
	{219,5,'H',7,"X2105H7","XK2105H7","XZ2105H7","XL2105H7",'H',280,150,240,8,100,250,10,22.8906,10,20,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,6,'H',7,"X2106H7","XK2106H7","XZ2106H7","XL2106H7",'H',290,160,250,8,100,250,12,25.891184,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{245,5,'H',7,"X2405H7","XK2405H7","XZ2405H7","XL2405H7",'H',350,180,290,8,110,330,10,33.89787,10,35,3,2,1,2,20,30,80,100,80,2,1,2,20,200,80,100,80,2,1,2,20,80,30,170,80},
	{245,6,'H',7,"X2406H7","XK2406H7","XZ2406H7","XL2406H7",'H',350,180,300,8,110,330,12,37.62348,10,40,3,2,1,2,20,30,80,100,80,2,1,2,20,200,80,100,80,2,1,2,20,80,30,170,80},
	{273,5,'H',7,"X2705H7","XK2705H7","XZ2705H7","XL2705H7",'H',360,190,310,8,130,330,12,42.171456,10,25,3,2,1,2,20,30,100,100,80,2,1,2,20,200,100,100,80,2,1,2,20,80,30,170,80},
	{273,6,'H',7,"X2706H7","XK2706H7","XZ2706H7","XL2706H7",'H',360,190,310,8,130,330,12,42.171456,10,25,3,2,1,2,20,30,100,100,80,2,1,2,20,200,100,100,80,2,1,2,20,80,30,170,80},
	{299,6,'H',7,"X2906H7","XK2906H7","XZ2906H7","XL2906H7",'H',370,200,370,8,160,330,12,51.900432,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{299,7,'H',7,"X2907H7","XK2907H7","XZ2907H7","XL2907H7",'H',370,200,370,8,160,330,12,51.900432,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{325,6,'H',7,"X3206H7","XK3206H7","XZ3206H7","XL3206H7",'H',390,220,370,8,160,330,12,53.271984,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{325,7,'H',7,"X3207H7","XK3207H7","XZ3207H7","XL3207H7",'H',400,230,370,10,160,330,14,65.07336,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{356,7,'H',7,"X3507H7","XK3507H7","XZ3507H7","XL3507H7",'H',410,240,390,10,170,330,14,69.745994,10,25,3,2,1,2,20,30,130,100,80,2,1,2,20,200,130,100,80,2,1,2,20,80,30,170,80},
	{356,8,'H',7,"X3508H7","XK3508H7","XZ3508H7","XL3508H7",'H',460,240,400,10,170,430,14,85.642244,10,30,4,2,1,2,20,30,140,100,80,2,1,2,20,300,140,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{377,7,'H',7,"X3707H7","XK3707H7","XZ3707H7","XL3707H7",'H',480,260,410,10,180,430,14,91.125312,10,25,4,2,1,2,20,30,150,100,80,2,1,2,20,300,150,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{377,8,'H',7,"X3708H7","XK3708H7","XZ3708H7","XL3708H7",'H',480,260,410,10,180,430,14,91.125312,10,25,4,2,1,2,20,30,150,100,80,2,1,2,20,300,150,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{406,8,'H',7,"X4008H7","XK4008H7","XZ4008H7","XL4008H7",'H',500,280,440,10,190,430,14,98.8943,10,30,4,2,1,2,20,30,160,100,80,2,1,2,20,300,160,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{406,9,'H',7,"X4009H7","XK4009H7","XZ4009H7","XL4009H7",'H',540,270,470,10,210,530,14,124.850796,10,25,4,2,1,2,24,40,170,120,80,2,1,2,24,370,170,120,80,2,1,2,24,100,40,120,80,2,1,2,24,320,40,120,80},
	{426,8,'H',7,"X4208H7","XK4208H7","XZ4208H7","XL4208H7",'H',510,290,460,10,200,430,14,104.788394,10,30,4,2,1,2,20,30,170,100,80,2,1,2,20,300,170,100,80,2,1,2,20,80,30,100,80,2,1,2,20,250,30,100,80},
	{426,9,'H',7,"X4209H7","XK4209H7","XZ4209H7","XL4209H7",'H',560,290,470,10,210,530,16,134.886864,10,25,4,2,1,2,24,40,170,120,80,2,1,2,24,370,170,120,80,2,1,2,24,100,40,120,80,2,1,2,24,310,40,120,80},
	{114,4,'H',1,"X1104H1","XK1104H1","XZ1104H1","XL1104H1",'H',210,100,150,6,50,210,8,7.807296,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{127,4,'H',1,"X1204H1","XK1204H1","XZ1204H1","XL1204H1",'H',230,120,160,6,60,210,8,9.254208,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{140,4,'H',1,"X1404H1","XK1404H1","XZ1404H1","XL1404H1",'H',230,120,170,6,70,210,10,11.49711,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'H',1,"X1504H1","XK1504H1","XZ1504H1","XL1504H1",'H',240,130,200,8,80,210,10,15.78792,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'H',1,"X1505H1","XK1505H1","XZ1505H1","XL1505H1",'H',280,150,200,8,80,250,10,18.6202,10,20,2,2,1,2,20,30,40,60,80,2,1,2,20,160,40,60,80},
	{168,4,'H',1,"X1604H1","XK1604H1","XZ1604H1","XL1604H1",'H',250,140,220,8,90,210,10,17.93411,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{168,5,'H',1,"X1605H1","XK1605H1","XZ1605H1","XL1605H1",'H',290,160,220,8,90,250,10,21.09295,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{180,5,'H',1,"X1805H1","XK1805H1","XZ1805H1","XL1805H1",'H',310,170,220,8,90,270,10,22.67237,10,20,4,2,1,2,16,25,65,80,80,1,1,1,16,65,25,0,80,2,1,2,16,165,65,80,80,1,1,1,16,205,25,0,80},
	{180,6,'H',1,"X1806H1","XK1806H1","XZ1806H1","XL1806H1",'H',340,170,230,8,90,330,10,26.93178,10,25,4,2,1,2,20,30,60,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,60,100,80,1,1,1,20,250,30,0,80},
	{194,5,'H',1,"X1905H1","XK1905H1","XZ1905H1","XL1905H1",'H',310,170,240,8,100,270,10,25.00225,10,20,4,2,1,2,16,25,75,80,80,1,1,1,16,65,25,0,80,2,1,2,16,165,75,80,80,1,1,1,16,205,25,0,80},
	{194,6,'H',1,"X1906H1","XK1906H1","XZ1906H1","XL1906H1",'H',340,170,240,8,100,330,10,29.1235,10,20,4,2,1,2,20,30,70,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,70,100,80,1,1,1,20,250,30,80,80},
	{203,5,'H',1,"X2005H1","XK2005H1","XZ2005H1","XL20005H1",'H',350,180,250,8,100,330,10,30.04195,10,25,4,2,1,2,20,30,70,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,70,100,80,1,1,1,20,250,30,0,80},
	{203,6,'H',1,"X2006H1","XK2006H1","XZ2006H1","XL2006H1",'H',360,190,260,10,100,330,12,37.951296,10,30,4,2,1,2,20,30,70,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,70,100,80,1,1,1,20,250,30,0,80},
	{219,5,'H',1,"X2105H1","XK2105H1","XZ2105H1","XL2105H1",'H',350,180,260,8,110,330,10,32.24937,10,20,4,2,1,2,20,30,80,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,80,100,80,1,1,1,20,250,30,0,80},
	{219,6,'H',1,"X2106H1","XK2106H1","XZ2106H1","XL2106H1",'H',370,200,290,10,120,330,12,44.665872,10,25,4,2,1,2,20,30,90,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,90,100,80,1,1,1,20,250,30,0,80},
	{245,5,'H',1,"X2405H1","XK2405H1","XZ2405H1","XL2405H1",'H',380,210,310,8,130,330,10,39.74926,10,25,4,2,1,2,20,30,100,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,100,100,80,1,1,1,20,250,30,0,80},
	{245,6,'H',1,"X2406H1","XK2406H1","XZ2406H1","XL2406H1",'H',390,220,310,10,130,330,12,49.277904,10,25,4,2,1,2,20,30,100,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,100,100,80,1,1,1,20,250,30,0,80},
	{273,5,'H',1,"X2705H1","XK2705H1","XZ2705H1","XL2705H1",'H',400,230,330,10,140,330,12,53.43024,10,25,4,2,1,2,20,30,110,100,80,1,1,1,20,80,30,0,80,2,1,2,20,200,110,100,80,1,1,1,20,250,30,80,80},
	{273,6,'H',1,"X2706H1","XK2706H1","XZ2706H1","XL2706H1",'H',440,230,330,10,140,410,12,62.905504,10,25,4,2,1,2,24,40,100,120,80,1,1,1,24,100,40,0,80,2,1,2,24,250,100,120,80,1,1,1,24,310,40,0,80},
	{299,6,'H',1,"X2906H1","XK2906H1","XZ2906H1","XL2906H1",'H',480,270,380,10,160,410,12,75.018368,10,30,4,2,1,2,24,40,120,120,80,1,1,1,24,100,40,0,80,2,1,2,24,250,120,120,80,1,1,1,24,310,40,0,80},
	{299,7,'H',1,"X2907H1","XK2907H1","XZ2907H1","XL2907H1",'H',550,280,380,10,160,530,12,92.00828,10,30,4,2,1,2,24,40,120,120,80,2,1,2,24,100,40,120,80,2,1,2,20,370,120,120,80,2,1,2,20,310,40,120,80},
	{325,6,'H',1,"X3206H1","XK3206H1","XZ3206H1","XL3206H1",'H',550,280,370,10,160,530,12,90.97208,10,25,4,2,1,2,24,40,120,120,80,2,1,2,24,100,40,120,80,2,1,2,24,370,120,120,80,2,1,2,24,310,40,120,80},
	{325,7,'H',1,"X3207H1","XK3207H1","XZ3207H1","XL3207H1",'H',550,280,370,10,160,530,14,97.13747,10,25,4,2,1,2,24,40,120,120,80,2,1,2,24,100,40,120,80,2,1,2,20,370,120,120,80,2,1,2,20,310,40,120,80},
	{356,7,'H',1,"X3507H1","XK3507H1","XZ3507H1","XL3507H1",'H',580,310,410,10,180,530,14,111.287252,10,25,4,2,1,2,24,40,140,120,80,2,1,2,24,100,40,120,80,2,1,2,24,370,140,120,80,2,1,2,24,310,40,120,80},
	{356,8,'H',1,"X3508H1","XK3508H1","XZ3508H1","XL3508H1",'H',640,310,430,10,190,650,14,137.062256,10,25,4,3,1,3,24,40,140,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,140,120,80,2,1,2,24,430,50,120,80},
	{377,7,'H',1,"X3707H1","XK3707H1","XZ3707H1","XL3707H1",'H',650,320,430,10,190,650,14,137.99201,10,25,4,3,1,3,24,40,140,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,140,120,80,2,1,2,24,430,50,120,80},
	{377,8,'H',1,"X3708H1","XK3708H1","XZ3708H1","XL3708H1",'H',630,300,440,12,190,650,14,153.028842,10,30,4,3,1,3,24,40,140,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,140,120,80,2,1,2,24,430,50,120,80},
	{406,8,'H',1,"X4008H1","XK4008H1","XZ4008H1","XL4008H1",'H',650,320,480,12,210,650,14,170.44391,10,30,4,3,1,3,24,40,160,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,160,120,80,2,1,2,24,430,50,120,80},
	{406,9,'H',1,"X4009H1","XK4009H1","XZ4009H1","XL4009H1",'H',660,330,480,12,210,650,16,181.120224,10,30,4,3,1,3,24,40,160,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,160,120,80,2,1,2,24,430,50,120,80},
	{426,8,'H',1,"X4208H1","XK4208H1","XZ4208H1","XL4208H1",'H',670,340,500,12,220,650,14,180.366938,10,30,4,3,1,3,24,40,170,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,170,120,80,2,1,2,24,430,50,120,80},
	{426,9,'H',1,"X4209H1","XK4209H1","XZ4209H1","XL4209H1",'H',690,360,500,12,220,650,16,193.042176,10,30,4,3,1,3,24,40,170,120,80,2,1,2,24,100,50,120,80,3,1,3,24,370,170,120,80,2,1,2,24,430,50,120,80},
	/* 因更换插板库注释掉(2013.6.5版本) wht 2015.12.08
	{114,4,'S',7,"X1104S7","XK1104S7","XZ1104S7","XL1104S7",'S',190,80,150,6,50,210,8,7.440544,10,25,1,4,1,4,16,25,25,50,80},
	{140,4,'S',7,"X1404S7","XK1404S7","XZ1404S7","XL1404S7",'S',200,90,170,6,70,210,8,9.70888,10,15,1,4,1,4,16,25,35,50,80},
	{159,4,'S',7,"X1504S7","XK1504S7","XZ1504S7","XL1504S7",'S',210,100,190,6,80,210,8,11.236176,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'S',7,"X1505S7","XK1505S7","XZ1505S7","XL1505S7",'S',220,110,200,8,80,210,10,15.17562,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,4,'S',7,"X1604S7","XK1604S7","XZ1604S7","XL1604S7",'S',220,110,190,6,80,210,8,11.469792,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,5,'S',7,"X1605S7","XK1605S7","XZ1605S7","XL1605S7",'S',220,110,200,8,80,210,10,15.17562,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{194,5,'S',7,"X1905S7","XK1905S7","XZ1905S7","XL1905S7",'S',230,120,220,8,90,210,10,17.25901,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{219,5,'S',7,"X2105S7","XK2105S7","XZ2105S7","XL2105S7",'S',270,140,240,8,100,250,10,22.52165,10,20,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{194,6,'S',7,"X1906S7","XK1906S7","XZ1906S7","XL1906S7",'S',250,120,240,8,100,250,10,21.78375,10,20,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,6,'S',7,"X2106S7","XK2106S7","XZ2106S7","XL2106S7",'S',270,140,270,8,100,250,12,25.989152,10,35,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{273,6,'S',7,"X2706S7","XK2706S7","XZ2706S7","XL2706S7",'S',340,170,310,8,130,330,12,41.025984,10,25,4,2,-1,-1,20,30,100,100,80,2,-1,-1,20,200,100,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{325,6,'S',7,"X3206S7","XK3206S7","XZ3206S7","XL3206S7",'S',370,200,370,8,160,330,12,51.900432,10,25,4,2,-1,-1,20,30,130,100,80,2,-1,-1,20,200,130,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{325,7,'S',7,"X3207S7","XK3207S7","XZ3207S7","XL3207S7",'S',370,200,370,8,160,330,12,51.900432,10,25,4,2,-1,-1,20,30,130,100,80,2,-1,-1,20,200,130,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{356,6,'S',7,"X3506S7","XK3506S7","XZ3506S7","XL3506S7",'S',380,210,390,8,170,330,12,55.675968,10,25,4,2,-1,-1,20,30,140,100,80,2,-1,-1,20,200,140,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{356,7,'S',7,"X3507S7","XK3507S7","XZ3507S7","XL3507S7",'S',380,210,390,8,170,330,14,60.174332,10,25,4,2,-1,-1,20,30,140,100,80,2,-1,-1,20,200,140,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{377,7,'S',7,"X3707S7","XK3707S7","XZ3707S7","XL3707S7",'S',390,220,410,8,180,330,14,64.388526,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,150,100,80,2,-1,-1,20,200,150,100,80},
	{377,6,'S',7,"X3706S7","XK3706S7","XZ3706S7","XL3706S7",'S',380,210,410,8,180,330,12,58.765728,10,25,4,2,-1,-1,20,30,150,100,80,2,-1,-1,20,200,150,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{406,7,'S',7,"X4007S7","XK4007S7","XZ4007S7","XL4007S7",'S',410,240,440,8,190,330,16,75.993024,10,30,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,160,100,80,2,-1,-1,20,200,160,100,80},
	{406,6,'S',7,"X4006S7","XK4006S7","XZ4006S7","XL4006S7",'S',410,240,440,8,190,330,14,70.521574,10,30,4,2,-1,-1,20,30,160,100,80,2,-1,-1,20,200,160,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{426,8,'S',7,"X4208S7","XK4208S7","XZ4208S7","XL4208S7",'S',470,250,460,10,200,430,16,107.372928,10,30,4,2,-1,-1,20,30,170,100,80,2,-1,-1,20,300,170,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{426,7,'S',7,"X4207S7","XK4207S7","XZ4207S7","XL4207S7",'S',470,250,460,8,200,430,14,90.004018,10,30,4,2,-1,-1,20,30,170,100,80,2,-1,-1,20,300,170,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{114,4,'H',7,"X1104H7","XK1104H7","XZ1104H7","XL1104H7",'H',200,90,150,6,50,210,8,7.62392,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{140,4,'H',7,"X1404H7","XK1404H7","XZ1404H7","XL1404H7",'H',220,110,170,6,70,210,10,11.23806,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'H',7,"X1504H7","XK1504H7","XZ1504H7","XL1504H7",'H',230,120,190,6,80,210,10,13.01059,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'H',7,"X1505H7","XK1505H7","XZ1505H7","XL1505H7",'H',230,120,200,8,80,210,10,15.48177,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,4,'H',7,"X1604H7","XK1604H7","XZ1604H7","XL1604H7",'H',230,120,190,6,80,210,10,13.01059,10,15,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,5,'H',7,"X1605H7","XK1605H7","XZ1605H7","XL1605H7",'H',240,130,200,8,80,210,10,15.78792,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{194,5,'H',7,"X1905H7","XK1905H7","XZ1905H7","XL1905H7",'H',250,140,220,8,90,210,10,17.93411,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{219,5,'H',7,"X2105H7","XK2105H7","XZ2105H7","XL2105H7",'H',280,150,240,8,100,250,10,22.8906,10,20,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{194,6,'H',7,"X1906H7","XK1906H7","XZ1906H7","XL1906H7",'H',280,140,220,8,90,270,10,21.65972,10,20,4,2,-1,-1,16,25,65,80,80,2,-1,-1,16,165,65,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{273,5,'H',7,"X2705H7","XK2705H7","XZ2705H7","XL2705H7",'H',360,190,310,8,130,330,12,42.171456,10,25,4,2,-1,-1,20,30,100,100,80,2,-1,-1,20,200,100,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{273,6,'H',7,"X2706H7","XK2706H7","XZ2706H7","XL2706H7",'H',360,190,310,8,130,330,12,42.171456,10,25,4,2,-1,-1,20,30,100,100,80,2,-1,-1,20,200,100,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{325,6,'H',7,"X3206H7","XK3206H7","XZ3206H7","XL3206H7",'H',390,220,370,8,160,330,12,53.271984,10,25,4,2,-1,-1,20,30,130,100,80,2,-1,-1,20,200,130,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{325,7,'H',7,"X3207H7","XK3207H7","XZ3207H7","XL3207H7",'H',400,230,370,10,160,330,14,65.07336,10,25,4,2,-1,-1,20,30,130,100,80,2,-1,-1,20,200,130,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{356,8,'H',7,"X3508H7","XK3508H7","XZ3508H7","XL3508H7",'H',460,240,400,10,170,430,14,85.642244,10,30,4,2,-1,-1,20,30,140,100,80,2,-1,-1,20,300,140,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{356,7,'H',7,"X3507H7","XK3507H7","XZ3507H7","XL3507H7",'H',410,240,390,10,170,330,14,69.745994,10,25,4,2,-1,-1,20,30,140,100,80,2,-1,-1,20,200,140,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{377,7,'H',7,"X3707H7","XK3707H7","XZ3707H7","XL3707H7",'H',480,260,410,10,180,430,14,91.125312,10,25,4,2,-1,-1,20,30,150,100,80,2,-1,-1,20,300,150,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{377,8,'H',7,"X3708H7","XK3708H7","XZ3708H7","XL3708H7",'H',480,260,410,10,180,430,14,91.125312,10,25,4,2,-1,-1,20,30,150,100,80,2,-1,-1,20,300,150,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{406,9,'H',7,"X4009H7","XK4009H7","XZ4009H7","XL4009H7",'H',540,270,460,10,210,530,14,123.663876,10,20,4,2,-1,-1,24,40,170,120,80,2,-1,-1,24,370,170,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{406,8,'H',7,"X4008H7","XK4008H7","XZ4008H7","XL4008H7",'H',500,280,440,10,190,430,14,98.8943,10,30,4,2,-1,-1,20,30,160,100,80,2,-1,-1,20,300,160,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{426,8,'H',7,"X4208H7","XK4208H7","XZ4208H7","XL4208H7",'H',510,290,460,10,200,430,14,104.788394,10,30,4,2,-1,-1,20,30,170,100,80,2,-1,-1,20,300,170,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{426,9,'H',7,"X4209H7","XK4209H7","XZ4209H7","XL4209H7",'H',560,290,460,10,210,530,16,133.480144,10,20,4,2,-1,-1,24,40,170,120,80,2,-1,-1,24,370,170,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{127,4,'S',7,"X1204S7","XK1204S7","XZ1204S7","XL1204S7",'S',200,90,160,6,60,210,8,8.6664,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{203,5,'S',7,"X2005S7","XK2005S7","XZ2005S7","XL2005S7",'S',270,140,250,8,100,250,10,22.94555,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{245,6,'S',7,"X2406S7","XK2406S7","XZ2406S7","XL2406S7",'S',280,150,300,8,120,250,12,30.581088,10,30,2,2,1,2,20,30,60,60,80,2,1,2,20,160,60,60,80},
	{299,6,'S',7,"X2906S7","XK2906S7","XZ2906S7","XL2906S7",'S',350,180,350,8,150,330,12,47.55216,10,25,4,2,-1,-1,20,30,120,100,80,2,-1,-1,20,200,120,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{180,4,'S',7,"X1804S7","XK1804S7","XZ1804S7","XL1804S7",'S',220,110,220,6,90,210,8,13.090032,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{203,6,'S',7,"X2006S7","XK2006S7","XZ2006S7","XL2006S7",'S',270,140,260,8,100,250,10,23.36945,10,30,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{273,5,'S',7,"X2705S7","XK2705S7","XZ2705S7","XL2705S7",'S',300,170,320,8,130,250,12,34.07528,10,30,2,2,1,2,20,30,65,60,80,2,1,2,20,160,65,60,80},
	{299,7,'S',7,"X2907S7","XK2907S7","XZ2907S7","XL2907S7",'S',360,190,360,8,150,330,12,48.878496,10,30,4,2,-1,-1,20,30,120,100,80,2,-1,-1,20,200,120,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{180,5,'S',7,"X1805S7","XK1805S7","XZ1805S7","XL1805S7",'S',220,110,220,8,90,210,10,16.92146,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{245,5,'S',7,"X2405S7","XK2405S7","XZ2405S7","XL2405S7",'S',280,150,290,8,120,250,10,27.6006,10,25,2,2,1,2,20,30,60,60,80,2,1,2,20,160,60,60,80},
	{127,4,'H',7,"X1204H7","XK1204H7","XZ1204H7","XL1204H7",'H',210,100,160,6,60,210,8,8.862336,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{203,5,'H',7,"X2005H7","XK2005H7","XZ2005H7","XL2005H7",'H',280,140,250,8,100,270,10,24.335,10,25,4,2,-1,-1,16,25,75,80,80,2,-1,-1,16,165,75,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{245,6,'H',7,"X2406H7","XK2406H7","XZ2406H7","XL2406H7",'H',350,180,300,8,110,330,12,37.62348,10,40,4,2,-1,-1,20,30,80,100,80,2,-1,-1,20,200,80,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{180,5,'H',7,"X1805H7","XK1805H7","XZ1805H7","XL1805H7",'H',280,140,220,8,90,270,10,21.65972,10,20,4,2,-1,-1,16,25,65,80,80,2,-1,-1,16,165,65,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{203,6,'H',7,"X2006H7","XK2006H7","XZ2006H7","XL2006H7",'H',290,150,260,8,100,270,10,25.17495,10,30,4,2,-1,-1,16,25,75,80,80,2,-1,-1,16,165,75,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{299,6,'H',7,"X2906H7","XK2906H7","XZ2906H7","XL2906H7",'H',370,200,370,8,160,330,12,51.900432,10,25,4,2,-1,-1,20,30,130,100,80,2,-1,-1,20,200,130,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{180,6,'H',7,"X1806H7","XK1806H7","XZ1806H7","XL1806H7",'H',280,140,230,8,90,270,10,22.09932,10,25,4,2,-1,-1,16,25,65,80,80,2,-1,-1,16,165,65,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{245,5,'H',7,"X2405H7","XK2405H7","XZ2405H7","XL2405H7",'H',350,180,290,8,110,330,10,33.89787,10,35,4,2,-1,-1,20,30,80,100,80,2,-1,-1,20,200,80,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{299,7,'H',7,"X2907H7","XK2907H7","XZ2907H7","XL2907H7",'H',370,200,370,8,160,330,12,51.900432,10,25,4,2,-1,-1,20,30,130,100,80,2,-1,-1,20,200,130,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{114,4,'S',1,"X1104S1","XK1104S1","XZ1104S1","XL1104S1",'S',210,100,150,6,50,210,8,7.807296,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{140,4,'S',1,"X1404S1","XK1404S1","XZ1404S1","XL1404S1",'S',220,110,170,6,70,210,8,10.125872,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'S',1,"X1504S1","XK1504S1","XZ1504S1","XL1504S1",'S',230,120,180,8,70,210,10,13.70453,10,20,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,5,'S',1,"X1505S1","XK1505S1","XZ1505S1","XL1505S1",'S',270,140,180,8,70,250,10,16.21025,10,20,2,2,1,2,20,30,35,60,80,2,1,2,20,160,35,60,80},
	{168,4,'S',1,"X1604S1","XK1604S1","XZ1604S1","XL1604S1",'S',240,130,200,8,80,210,10,15.78792,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{168,5,'S',1,"X1605S1","XK1605S1","XZ1605S1","XL1605S1",'S',280,150,200,8,80,250,10,18.6202,10,20,2,2,1,2,20,30,40,60,80,2,1,2,20,160,40,60,80},
	{194,5,'S',1,"X1905S1","XK1905S1","XZ1905S1","XL1905S1",'S',290,160,230,8,90,250,12,23.542464,10,25,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{219,5,'S',1,"X2105S1","XK2105S1","XZ2105S1","XL2105S1",'S',300,170,250,8,100,250,12,26.35088,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,6,'S',1,"X2106S1","XK2106S1","XZ2106S1","XL2106S1",'S',350,180,270,8,110,330,12,35.64528,10,25,4,2,-1,-1,20,30,80,100,80,2,-1,-1,20,200,80,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{273,6,'S',1,"X2706S1","XK2706S1","XZ2706S1","XL2706S1",'S',370,200,330,8,140,330,12,45.796272,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,110,100,80,2,-1,-1,20,200,110,100,80},
	{194,6,'S',1,"X1906S1","XK1906S1","XZ1906S1","XL1906S1",'S',330,160,240,8,100,330,12,31.127448,10,20,4,2,-1,-1,20,30,70,100,80,2,-1,-1,20,200,70,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{114,4,'H',1,"X1104H1","XK1104H1","XZ1104H1","XL1104H1",'H',210,100,150,6,50,210,8,7.807296,10,25,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{140,4,'H',1,"X1404H1","XK1404H1","XZ1404H1","XL1404H1",'H',230,120,170,6,70,210,10,11.49711,10,15,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'H',1,"X1504H1","XK1504H1","XZ1504H1","XL1504H1",'H',240,130,200,8,80,210,10,15.78792,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'H',1,"X1505H1","XK1505H1","XZ1505H1","XL1505H1",'H',280,150,200,8,80,250,10,18.6202,10,20,2,2,1,2,20,30,40,60,80,2,1,2,20,160,40,60,80},
	{168,4,'H',1,"X1604H1","XK1604H1","XZ1604H1","XL1604H1",'H',250,140,220,8,90,210,10,17.93411,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{168,5,'H',1,"X1605H1","XK1605H1","XZ1605H1","XL1605H1",'H',290,160,220,8,90,250,10,21.09295,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{194,5,'H',1,"X1905H1","XK1905H1","XZ1905H1","XL1905H1",'H',310,170,240,8,100,270,10,25.00225,10,20,4,2,-1,-1,16,25,65,80,80,2,-1,-1,16,165,65,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{219,5,'H',1,"X2105H1","XK2105H1","XZ2105H1","XL2105H1",'H',350,180,260,8,110,330,10,32.24937,10,20,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,80,100,80,2,-1,-1,20,200,80,100,80},
	{194,6,'H',1,"X1906H1","XK1906H1","XZ1906H1","XL1906H1",'H',340,170,230,8,100,330,10,28.5897,10,15,4,2,-1,-1,20,30,70,100,80,2,-1,-1,20,200,70,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{219,6,'H',1,"X2106H1","XK2106H1","XZ2106H1","XL2106H1",'H',370,200,290,10,120,330,12,44.665872,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,90,100,80,2,-1,-1,20,200,90,100,80},
	{273,6,'H',1,"X2706H1","XK2706H1","XZ2706H1","XL2706H1",'H',440,230,330,10,140,410,12,62.905504,10,25,4,2,-1,-1,24,40,100,120,80,2,-1,-1,24,250,100,120,80,1,-1,-1,24,100,40,0,80,1,-1,-1,24,310,40,0,80},
	{273,5,'H',1,"X2705H1","XK2705H1","XZ2705H1","XL2705H1",'H',400,230,330,10,140,330,12,53.43024,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,110,100,80,2,-1,-1,20,200,110,100,80},
	{325,6,'H',1,"X3206H1","XK3206H1","XZ3206H1","XL3206H1",'H',550,280,370,10,160,530,12,90.97208,10,25,4,2,-1,-1,24,40,120,120,80,2,-1,-1,24,370,120,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{356,7,'H',1,"X3507H1","XK3507H1","XZ3507H1","XL3507H1",'H',580,310,410,10,180,530,14,111.287252,10,25,4,2,-1,-1,24,40,140,120,80,2,-1,-1,24,370,140,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{377,7,'H',1,"X3707H1","XK3707H1","XZ3707H1","XL3707H1",'H',650,320,430,10,190,650,14,137.99201,10,25,4,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80,3,-1,-1,24,40,140,120,80,3,-1,-1,24,370,140,120,80},
	{328,7,'H',1,"X3207H1","XK3207H1","XZ3207H1","XL3207H1",'H',550,280,370,10,160,530,14,97.13747,10,25,4,2,-1,-1,24,40,120,120,80,2,-1,-1,24,370,120,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{356,8,'H',1,"X3508H1","XK3508H1","XZ3508H1","XL3508H1",'H',640,310,430,10,190,650,14,137.062256,10,25,4,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80,3,-1,-1,24,40,140,120,80,3,-1,-1,24,370,140,120,80},
	{377,8,'H',1,"X3708H1","XK3708H1","XZ3708H1","XL3708H1",'H',630,300,440,12,190,650,14,153.028842,10,30,4,3,-1,-1,24,40,140,120,80,3,-1,-1,24,370,140,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{406,8,'H',1,"X4008H1","XK4008H1","XZ4008H1","XL4008H1",'H',650,320,480,12,210,650,14,170.44391,10,30,4,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80,3,-1,-1,24,40,160,120,80,3,-1,-1,24,370,160,120,80},
	{426,8,'H',1,"X4208H1","XK4208H1","XZ4208H1","XL4208H1",'H',670,340,500,12,220,650,14,180.366938,10,30,4,3,-1,-1,24,40,170,120,80,3,-1,-1,24,370,170,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{406,9,'H',1,"X4009H1","XK4009H1","XZ4009H1","XL4009H1",'H',660,330,480,12,210,650,14,171.483564,10,30,4,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80,3,-1,-1,24,40,160,120,80,3,-1,-1,24,370,160,120,80},
	{426,9,'H',1,"X4209H1","XK4209H1","XZ4209H1","XL4209H1",'H',690,360,500,12,220,650,16,193.042176,10,30,4,3,-1,-1,24,40,170,120,80,3,-1,-1,24,370,170,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{127,4,'S',1,"X1204S1","XK1204S1","XZ1204S1","XL1204S1",'S',210,100,160,6,60,210,8,8.862336,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{180,4,'S',1,"X1804S1","XK1804S1","XZ1804S1","XL1804S1",'S',280,150,220,8,90,250,10,20.7554,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{180,5,'S',1,"X1805S1","XK1805S1","XZ1805S1","XL1805S1",'S',280,150,220,8,90,250,10,20.7554,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{203,5,'S',1,"X2005S1","XK2005S1","XZ2005S1","XL2005S1",'S',300,170,250,8,100,250,12,26.35088,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{203,6,'S',1,"X2006S1","XK2006S1","XZ2006S1","XL2006S1",'S',350,180,260,8,100,330,12,33.32796,10,30,4,2,-1,-1,20,30,70,100,80,2,-1,-1,20,200,70,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{245,5,'S',1,"X2405S1","XK2405S1","XZ2405S1","XL2405S1",'S',360,190,290,8,120,330,12,39.157056,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,90,100,80,2,-1,-1,20,200,90,100,80},
	{245,6,'S',1,"X2406S1","XK2406S1","XZ2406S1","XL2406S1",'S',360,190,300,8,120,330,12,39.835296,10,30,4,2,-1,-1,20,30,90,100,80,2,-1,-1,20,200,90,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{299,6,'S',1,"X2906S1","XK2906S1","XZ2906S1","XL2906S1",'S',460,240,350,8,150,430,12,62.217216,10,25,4,2,-1,-1,20,30,120,100,80,2,-1,-1,20,300,120,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{273,5,'S',1,"X2705S1","XK2705S1","XZ2705S1","XL2705S1",'S',370,200,320,8,130,330,12,43.441272,10,30,4,2,-1,-1,20,30,100,100,80,2,-1,-1,20,200,100,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{299,7,'S',1,"X2907S1","XK2907S1","XZ2907S1","XL2907S1",'S',460,240,360,8,150,430,12,63.083856,10,30,4,2,-1,-1,20,30,120,100,80,2,-1,-1,20,300,120,100,80,2,-1,-1,20,80,30,100,80,2,-1,-1,20,250,30,100,80},
	{127,4,'H',1,"X1204H1","XK1204H1","XZ1204H1","XL1204H1",'H',230,120,160,6,60,210,8,9.254208,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{180,5,'H',1,"X1805H1","XK1805H1","XZ1805H1","XL1805H1",'H',310,170,220,8,90,270,10,22.67237,10,20,4,2,-1,-1,16,25,65,80,80,2,-1,-1,16,165,65,80,80,1,-1,-1,16,65,25,0,80,1,-1,-1,16,205,25,0,80},
	{180,6,'H',1,"X1806H1","XK1806H1","XZ1806H1","XL1806H1",'H',340,170,230,8,90,330,10,26.93178,10,25,4,2,-1,-1,20,30,60,100,80,2,-1,-1,20,200,60,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{203,5,'H',1,"X2005H1","XK2005H1","XZ2005H1","XL2005H1",'H',350,180,250,8,100,330,10,30.04195,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,70,100,80,2,-1,-1,20,200,70,100,80},
	{230,6,'H',1,"X2306H1","XK2306H1","XZ2306H1","XL2306H1",'H',360,190,260,10,100,330,12,37.951296,10,30,4,2,-1,-1,20,30,70,100,80,2,-1,-1,20,200,70,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},
	{245,5,'H',1,"X2405H1","XK2405H1","XZ2405H1","XL2405H1",'H',380,210,310,8,130,330,10,39.74926,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,100,100,80,2,-1,-1,20,200,100,100,80},
	{245,6,'H',1,"X2406H1","XK2406H1","XZ2406H1","XL2406H1",'H',390,220,310,10,130,330,12,49.277904,10,25,4,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80,2,-1,-1,20,30,100,100,80,2,-1,-1,20,200,100,100,80},
	{299,6,'H',1,"X2906H1","XK2906H1","XZ2906H1","XL2906H1",'H',480,270,380,10,160,410,12,75.018368,10,30,4,2,-1,-1,24,40,120,120,80,2,-1,-1,24,250,120,120,80,1,-1,-1,24,100,40,0,80,1,-1,-1,24,310,40,0,80},
	{299,7,'H',1,"X2907H1","XK2907H1","XZ2907H1","XL2907H1",'H',550,280,380,10,160,530,12,92.00828,10,30,4,2,-1,-1,24,40,120,120,80,2,-1,-1,24,370,120,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{219,6,'H',7,"X2106H7","XK2106H7","XZ2106H7","XL2106H7",'H',290,160,250,8,100,250,12,25.891184,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{203,6,'H',1,"X2006H1","XK2006H1","XZ2006H1","XL2006H1",'H',360,190,260,10,100,330,12,37.951296,10,30,4,2,-1,-1,20,30,70,100,80,2,-1,-1,20,200,70,100,80,1,-1,-1,20,80,30,0,80,1,-1,-1,20,250,30,0,80},*/
	/* 因更换插板库注释掉(2011.10.22版本) wjh-2013.6.5
	{114,4,'S',7,"X1104S7","XK1104S7","XZ1104S7","XL1104S7",'S',190,80,130,6,50,210,8,7.00,10,15,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{140,4,'S',7,"X1404S7","XK1404S7","XZ1404S7","XL1404S7",'S',200,90,170,6,70,210,8,9.70,10,15,2,2,1,2,16,25,35,50,100,2,1,2,16,135,35,50,100},
	{159,4,'S',7,"X1504S7","XK1504S7","XZ1504S7","XL1504S7",'S',210,100,190,6,80,210,8,11.20,10,15,2,2,1,2,16,25,40,50,110,2,1,2,16,135,40,50,110},
	{159,4,'S',7,"X1505S7","XK1505S7","XZ1505S7","XL1505S7",'S',220,110,200,8,80,210,10,15.20,10,15,2,2,1,2,16,25,40,50,120,2,1,2,16,135,40,50,120},
	{168,4,'S',7,"X1604S7","XK1604S7","XZ1604S7","XL1604S7",'S',220,110,190,6,80,210,8,11.50,10,15,2,2,1,2,16,135,40,50,110,2,1,2,16,25,40,50,110},
	{168,5,'S',7,"X1605S7","XK1605S7","XZ1605S7","XL1605S7",'S',220,110,200,8,80,210,10,15.20,10,20,2,2,1,2,16,25,40,50,120,2,1,2,16,135,40,50,120},
	{194,5,'S',7,"X1905S7","XK1905S7","XZ1905S7","XL1905S7",'S',230,120,220,8,90,210,10,17.30,10,20,2,2,1,2,16,25,45,50,130,2,1,2,16,135,45,50,130},
	{219,5,'S',7,"X2105S7","XK2105S7","XZ2105S7","XL2105S7",'S',270,140,240,8,100,250,10,22.50,10,20,2,2,1,2,20,30,50,60,140,2,1,2,20,160,50,60,140},
	{219,6,'S',7,"X2106S7","XK2106S7","XZ2106S7","XL2106S7",'S',270,140,240,8,100,250,12,25.00,10,25,2,2,1,2,20,30,50,60,150,2,1,2,20,160,50,60,150},
	{273,6,'S',7,"X2706S7","XK2706S7","XZ2706S7","XL2706S7",'S',340,170,310,8,130,330,12,41.00,10,25,3,2,-1,-1,20,30,100,100,250,2,-1,-1,20,200,100,100,250,2,-1,-1,20,80,30,170,110},
	{325,6,'S',7,"X3206S7","XK3206S7","XZ3206S7","XL3206S7",'S',370,200,370,8,160,330,12,51.90,10,25,3,2,-1,-1,20,80,30,170,110,2,-1,-1,20,30,130,100,310,2,-1,-1,20,200,130,100,310},
	{325,7,'S',7,"X3207S7","XK3207S7","XZ3207S7","XL3207S7",'S',370,200,370,8,160,330,12,51.90,10,25,3,2,-1,-1,20,80,30,170,310,2,-1,-1,20,30,130,100,310,2,-1,-1,20,200,130,100,310},
	{356,6,'S',7,"X3506S7","XK3506S7","XZ3506S7","XL3506S7",'S',380,210,390,8,170,330,12,55.70,10,25,3,2,-1,-1,20,80,30,170,110,2,-1,-1,20,30,140,100,330,2,-1,-1,20,200,140,100,330},
	{356,7,'S',7,"X3507S7","XK3507S7","XZ3507S7","XL3507S7",'S',380,210,390,8,170,330,14,60.20,10,25,3,2,-1,-1,20,80,30,170,110,2,-1,-1,20,30,140,100,330,2,-1,-1,20,200,140,100,330},
	{377,7,'S',7,"X3707S7","XK3707S7","XZ3707S7","XL3707S7",'S',390,220,410,8,180,330,14,64.40,10,25,3,2,-1,-1,20,80,30,170,110,2,-1,-1,20,30,150,100,350,2,-1,-1,20,200,150,100,350},
	{377,8,'S',7,"X3708S7","XK3708S7","XZ3708S7","XL3708S7",'S',400,230,420,10,180,330,16,78.70,10,30,3,2,-1,-1,20,80,30,170,120,2,-1,-1,20,30,150,100,360,2,-1,-1,20,200,150,100,360},
	{406,7,'S',7,"X4007S7","XK4007S7","XZ4007S7","XL4007S7",'S',410,240,440,8,190,330,16,76.00,10,30,3,2,-1,-1,20,30,160,100,360,2,-1,-1,20,200,160,100,360,2,-1,-1,20,80,30,170,120},
	{406,8,'S',7,"X4008S7","XK4008S7","XZ4008S7","XL4008S7",'S',460,240,440,10,190,430,16,101.20,10,30,4,2,-1,-1,20,80,30,100,120,2,-1,-1,20,250,30,100,120,2,-1,-1,20,30,160,100,400,2,-1,-1,20,300,160,100,400},
	{426,8,'S',7,"X4208S7","XK4208S7","XZ4208S7","XL4208S7",'S',470,250,460,10,200,430,16,107.40,10,30,4,2,-1,-1,20,80,30,100,120,2,-1,-1,20,250,30,100,120,2,-1,-1,20,30,170,100,400,2,-1,-1,20,300,170,100,400},
	{426,10,'H',7,"X4210S7","XK4210S7","XZ4210S7","XL4210S7",'S',470,250,460,10,200,430,16,107.40,10,30,4,2,-1,-1,20,80,30,100,120,2,-1,-1,20,250,30,100,120,2,-1,-1,20,30,170,100,400,2,-1,-1,20,300,170,100,400},
	{114,4,'H',7,"X1104H7","XK1104H7","XZ1104H7","XL1104H7",'H',200,90,130,8,50,210,8,7.10,10,15,2,2,1,2,16,25,25,50,80,2,1,2,16,135,25,50,80},
	{140,4,'H',7,"X1404H7","XK1404H7","XZ1404H7","XL1404H7",'H',220,110,170,6,70,210,8,10.90,10,15,2,2,1,2,16,25,35,50,100,2,1,2,16,135,35,50,100},
	{159,4,'H',7,"X1504H7","XK1504H7","XZ1504H7","XL1504H7",'H',230,120,190,6,80,210,8,11.70,10,15,2,2,1,2,16,25,40,50,110,2,1,2,16,135,40,50,110},
	{159,5,'H',7,"X1505H7","XK1505H7","XZ1505H7","XL1505H7",'H',230,120,200,8,80,210,10,15.50,10,15,2,2,1,2,16,25,40,50,120,2,1,2,16,135,40,50,120},
	{168,4,'H',7,"X1604H7","XK1604H7","XZ1604H7","XL1604H7",'H',230,120,190,6,80,210,8,11.70,10,15,2,2,1,2,16,25,40,50,110,2,1,2,16,135,40,50,110},
	{168,5,'H',7,"X1605H7","XK1605H7","XZ1605H7","XL1605H7",'H',240,130,200,8,80,210,10,15.80,10,20,2,2,1,2,16,25,40,50,120,2,1,2,16,135,40,50,120},
	{194,5,'H',7,"X1905H7","XK1905H7","XZ1905H7","XL1905H7",'H',250,140,220,8,90,210,10,17.90,10,20,2,2,1,2,16,25,45,50,130,2,1,2,16,135,45,50,130},
	{219,5,'H',7,"X2105H7","XK2105H7","XZ2105H7","XL2105H7",'H',280,150,240,8,100,250,10,22.90,10,20,2,2,1,2,20,30,60,50,140,2,1,2,20,170,60,50,140},
	{219,6,'H',7,"X2106H7","XK2106H7","XZ2106H7","XL2106H7",'H',290,160,250,8,100,250,12,25.90,10,25,2,2,1,2,20,30,60,50,60,2,1,2,20,170,60,50,60},
	{273,6,'H',7,"X2706H7","XK2706H7","XZ2706H7","XL2706H7",'H',360,190,310,8,130,330,12,42.20,10,25,3,2,-1,-1,20,80,30,170,110,2,-1,-1,20,30,100,100,250,2,-1,-1,20,200,100,100,250},
	{325,6,'H',7,"X3206H7","XK3206H7","XZ3206H7","XL3206H7",'H',390,220,370,8,160,330,12,52.50,10,25,3,2,-1,-1,20,30,130,100,100,2,-1,-1,20,200,130,100,100,2,-1,-1,20,80,30,170,170},
	{325,7,'H',7,"X3207H7","XK3207H7","XZ3207H7","XL3207H7",'H',400,230,370,10,160,330,14,65.10,10,25,3,2,-1,-1,20,30,130,100,100,2,-1,-1,20,200,130,100,100,2,-1,-1,20,80,30,170,170},
	{356,6,'H',7,"X3506H7","XK3506H7","XZ3506H7","XL3506H7",'H',410,240,390,8,170,330,12,57.10,10,25,3,2,-1,-1,20,80,30,170,170,2,-1,-1,20,30,140,100,100,2,-1,-1,20,200,140,100,100},
	{356,7,'H',7,"X3507H7","XK3507H7","XZ3507H7","XL3507H7",'H',410,240,390,10,170,330,14,69.70,10,25,3,2,-1,-1,20,30,140,100,100,2,-1,-1,20,200,140,100,100,2,-1,-1,20,80,30,170,170},
	{377,7,'H',7,"X3707H7","XK3707H7","XZ3707H7","XL3707H7",'H',480,260,410,10,180,430,14,91.10,10,25,4,2,-1,-1,20,30,150,100,100,2,-1,-1,20,300,150,100,100,2,-1,-1,20,80,30,100,100,2,-1,-1,20,250,30,100,100},
	{377,8,'H',7,"X3708H7","XK3708H7","XZ3708H7","XL3708H7",'H',480,260,410,10,180,430,14,93.80,10,25,4,2,-1,-1,20,30,150,100,100,2,-1,-1,20,300,150,100,100,2,-1,-1,20,80,30,100,100,2,-1,-1,20,250,30,100,100},
	{406,7,'H',7,"X4007H7","XK4007H7","XZ4007H7","XL4007H7",'H',490,270,430,10,190,430,14,94.20,10,25,4,2,-1,-1,20,30,160,100,100,2,-1,-1,20,300,160,100,100,2,-1,-1,20,80,30,100,100,2,-1,-1,20,250,30,100,100},
	{406,8,'H',7,"X4008H7","XK4008H7","XZ4008H7","XL4008H7",'H',500,280,440,10,190,430,14,98.90,10,30,4,2,-1,-1,20,30,160,100,100,2,-1,-1,20,300,160,100,100,2,-1,-1,20,80,30,100,100,2,-1,-1,20,250,30,100,100},
	{426,8,'H',7,"X4208H7","XK4208H7","XZ4208H7","XL4208H7",'H',510,290,460,10,200,430,14,104.80,10,30,4,2,-1,-1,20,30,170,100,100,2,-1,-1,20,300,170,100,100,2,-1,-1,20,80,30,100,100,2,-1,-1,20,250,30,100,100},
	{426,10,'H',7,"X4210H7","XK4210H7","XZ4210H7","XL4210H7",'H',560,290,460,10,200,530,16,130.20,10,30,4,2,-1,-1,24,40,160,120,120,2,-1,-1,24,370,160,120,120,2,-1,-1,24,100,40,120,120,2,-1,-1,24,310,40,120,120},
	{114,4,'S',1,"X1104S1","XK1104S1","XZ1104S1","XL1104S1",'S',210,100,130,6,50,210,8,7.30,10,15,2,2,1,2,16,25,25,50,50,2,1,2,16,135,25,50,50},
	{140,4,'S',1,"X1404S1","XK1404S1","XZ1404S1","XL1404S1",'S',220,110,170,6,70,210,8,10.20,10,15,2,2,1,2,16,25,35,50,50,2,1,2,16,135,35,50,50},
	{159,4,'S',1,"X1504S1","XK1504S1","XZ1504S1","XL1504S1",'S',230,120,180,6,70,210,10,13.40,10,20,2,2,1,2,16,135,35,50,50,2,1,2,16,25,35,50,50},
	{159,5,'S',1,"X1505S1","XK1505S1","XZ1505S1","XL1505S1",'S',270,140,180,8,70,250,10,16.20,10,15,2,2,1,2,20,30,35,60,110,2,1,2,20,160,35,60,110},
	{168,4,'S',1,"X1604S1","XK1604S1","XZ1604S1","XL1604S1",'S',240,130,200,8,80,210,10,16.20,10,20,2,2,1,2,16,30,40,50,110,2,1,2,16,135,40,50,110},
	{168,5,'S',1,"X1605S1","XK1605S1","XZ1605S1","XL1605S1",'S',280,150,200,8,80,250,10,19.80,10,20,2,2,1,2,20,30,40,60,110,2,1,2,20,160,40,60,110},
	{194,5,'S',1,"X1905S1","XK1905S1","XZ1905S1","XL1905S1",'S',290,160,230,8,90,250,12,22.40,10,25,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{219,5,'S',1,"X2105S1","XK2105S1","XZ2105S1","XL2105S1",'S',300,170,250,8,100,250,12,26.70,10,25,2,2,1,2,20,30,50,60,80,2,1,2,20,160,50,60,80},
	{219,6,'S',1,"X2106S1","XK2106S1","XZ2106S1","XL2106S1",'S',350,180,270,8,110,330,12,35.90,10,25,3,2,-1,-1,20,30,80,100,80,2,-1,-1,20,200,80,100,80,2,-1,-1,20,80,30,170,80},
	{273,6,'S',1,"X2706S1","XK2706S1","XZ2706S1","XL2706S1",'S',370,200,330,8,140,330,12,45.70,10,25,3,2,-1,-1,20,30,110,100,80,2,-1,-1,20,200,110,100,80,2,-1,-1,20,80,30,170,80},
	{114,4,'H',1,"X1104H1","XK1104H1","XZ1104H1","XL1104H1",'H',210,100,160,8,60,210,10,11.40,10,20,2,2,1,2,16,25,30,50,80,2,1,2,16,135,30,50,80},
	{140,4,'H',1,"X1404H1","XK1404H1","XZ1404H1","XL1404H1",'H',230,120,180,8,70,210,10,13.40,10,20,2,2,1,2,16,25,35,50,80,2,1,2,16,135,35,50,80},
	{159,4,'H',1,"X1504H1","XK1504H1","XZ1504H1","XL1504H1",'H',240,130,200,8,80,210,10,16.20,10,20,2,2,1,2,16,25,40,50,80,2,1,2,16,135,40,50,80},
	{159,5,'H',1,"X1505H1","XK1505H1","XZ1505H1","XL1505H1",'H',280,150,200,8,80,250,10,19.00,10,20,2,2,1,2,20,30,40,60,80,2,1,2,20,160,40,60,80},
	{168,4,'H',1,"X1604H1","XK1604H1","XZ1604H1","XL1604H1",'H',250,140,220,8,90,210,10,18.10,10,20,2,2,1,2,16,25,45,50,80,2,1,2,16,135,45,50,80},
	{168,5,'H',1,"X1605H1","XK1605H1","XZ1605H1","XL1605H1",'H',290,160,220,8,90,250,10,21.00,10,20,2,2,1,2,20,30,45,60,80,2,1,2,20,160,45,60,80},
	{194,5,'H',1,"X1905H1","XK1905H1","XZ1905H1","XL1905H1",'H',310,170,240,8,100,270,10,25.00,10,20,3,2,-1,-1,16,25,75,80,80,2,-1,-1,16,165,75,80,80,2,-1,-1,16,65,25,140,80},
	{219,5,'H',1,"X2105H1","XK2105H1","XZ2105H1","XL2105H1",'H',350,180,260,8,110,330,10,32.30,10,20,3,2,-1,-1,20,30,80,100,80,2,-1,-1,20,200,80,100,80,2,-1,-1,20,80,30,170,80},
	{219,6,'H',1,"X2106H1","XK2106H1","XZ2106H1","XL2106H1",'H',370,200,290,10,120,330,12,44.50,10,25,3,2,-1,-1,20,30,90,100,80,2,-1,-1,20,200,90,100,80,2,-1,-1,20,80,30,170,80},
	{273,6,'H',1,"X2706H1","XK2706H1","XZ2706H1","XL2706H1",'H',440,230,330,10,140,410,12,62.90,10,25,3,2,-1,-1,24,40,100,120,80,2,-1,-1,24,250,100,120,80,2,-1,-1,24,100,40,210,80},
	{325,6,'H',1,"X3206H1","XK3206H1","XZ3206H1","XL3206H1",'H',550,280,370,10,160,530,12,91.40,10,25,4,2,-1,-1,24,40,120,120,80,2,-1,-1,24,370,120,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{325,7,'H',1,"X3207H1","XK3207H1","XZ3207H1","XL3207H1",'H',550,280,370,10,160,530,12,91.40,10,25,4,2,-1,-1,24,40,120,120,80,2,-1,-1,24,370,120,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{356,6,'H',1,"X3506H1","XK3506H1","XZ3506H1","XL3506H1",'H',580,310,390,10,170,530,12,98.70,10,25,4,2,-1,-1,24,40,130,120,80,2,-1,-1,24,370,130,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{356,7,'H',1,"X3507H1","XK3507H1","XZ3507H1","XL3507H1",'H',580,310,410,10,180,530,12,104.20,10,25,4,2,-1,-1,24,40,140,120,80,2,-1,-1,24,370,140,120,80,2,-1,-1,24,100,40,120,80,2,-1,-1,24,310,40,120,80},
	{377,7,'H',1,"X3707H1","XK3707H1","XZ3707H1","XL3707H1",'H',650,320,430,10,190,650,12,129.50,10,25,5,2,-1,-1,24,40,140,120,80,2,-1,-1,24,280,140,90,80,2,-1,-1,24,490,140,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{377,8,'H',1,"X3708H1","XK3708H1","XZ3708H1","XL3708H1",'H',630,300,440,12,190,650,14,152.70,10,30,5,2,-1,-1,24,40,140,120,80,2,-1,-1,24,280,140,90,80,2,-1,-1,24,490,140,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{406,7,'H',1,"X4007H1","XK4007H1","XZ4007H1","XL4007H1",'H',660,330,450,10,200,650,12,136.80,10,25,5,2,-1,-1,24,40,150,120,80,2,-1,-1,24,280,150,90,80,2,-1,-1,24,490,150,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{406,8,'H',1,"X4008H1","XK4008H1","XZ4008H1","XL4008H1",'H',650,320,480,12,210,650,14,170.70,10,30,5,2,-1,-1,24,40,160,120,80,2,-1,-1,24,280,160,90,80,2,-1,-1,24,490,160,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{426,8,'H',1,"X4208H1","XK4208H1","XZ4208H1","XL4208H1",'H',670,340,500,12,220,650,14,180.60,10,30,5,2,-1,-1,24,40,170,120,80,2,-1,-1,24,280,170,90,80,2,-1,-1,24,490,170,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	{426,10,'H',1,"X4210H1","XK4210H1","XZ4210H1","XL4210H1",'H',690,360,500,12,220,650,16,193.30,10,30,5,2,-1,-1,24,40,170,120,80,2,-1,-1,24,280,170,90,80,2,-1,-1,24,490,170,120,80,2,-1,-1,24,100,50,120,80,2,-1,-1,24,430,50,120,80},
	/*{114,4,'H',1,"X114X4H",210,90,132,6,50,230,8,4.38,10,16,2,2,1,2,16,30,25,50,41,2,1,2,16,150,25,50,41,0,0,0,0,0,0,0,0},
	{140,4,'H',1,"X140X4H",230,110,172,6,70,230,8,5.35,10,16,2,2,1,2,16,30,35,50,0,2,1,2,16,150,35,50,0,0,0,0,0,0,0,0,0},
	{159,4,'H',1,"X159X4H",250,120,192,6,80,230,8,5.35,10,16,2,2,1,2,16,30,40,50,0,2,1,2,16,150,40,50,0,0,0,0,0,0,0,0,0},
	{168,4,'H',1,"X168X4H",240,120,192,6,80,230,8,5.35,10,16,2,2,1,2,16,30,40,50,0,2,1,2,16,150,40,50,0,0,0,0,0,0,0,0,0},
	{159,5,'H',1,"X159X5H",250,120,180,8,70,250,10,0.00,10,20,2,2,1,2,16,30,35,60,0,2,1,2,16,160,35,60,0,0,0,0,0,0,0,0,0},
	{168,5,'H',1,"X168X5H",260,130,200,8,80,250,10,0.00,10,20,2,2,1,2,16,30,40,60,0,2,1,2,16,160,40,60,0,0,0,0,0,0,0,0,0},
	{194,5,'H',1,"X194X5H",270,140,220,8,90,250,10,0.00,10,20,2,2,1,2,16,30,45,60,0,2,1,2,16,160,45,60,0,0,0,0,0,0,0,0,0},
	{219,5,'H',1,"X219X5H",290,160,240,8,100,250,10,0.00,10,20,2,2,1,2,20,30,50,60,0,2,1,2,20,160,50,60,0,0,0,0,0,0,0,0,0},
	{219,6,'H',1,"X219X6H",290,160,248,8,100,250,12,0.00,10,24,2,2,1,2,20,30,50,60,0,2,1,2,20,160,50,60,0,0,0,0,0,0,0,0,0},
	{273,6,'H',1,"X273X6H",380,190,308,8,130,370,12,0.00,10,24,2,3,1,3,20,30,65,60,0,3,1,3,20,220,65,60,0,0,0,0,0,0,0,0,0},
	{325,6,'H',1,"X325X6H",410,220,368,8,160,370,12,0.00,10,24,2,3,1,3,20,30,80,60,0,3,1,3,20,220,80,60,0,0,0,0,0,0,0,0,0},
	{356,6,'H',1,"X356X6H",430,240,388,8,170,370,12,0.00,10,24,2,3,1,3,20,30,85,60,0,3,1,3,20,220,85,60,0,0,0,0,0,0,0,0,0},
	{325,7,'H',1,"X325X7H",420,230,356,10,150,370,14,0.00,10,28,2,3,1,3,20,30,75,60,0,3,1,3,20,220,75,60,0,0,0,0,0,0,0,0,0},
	{356,7,'H',1,"X356X7H",430,240,396,10,170,370,14,0.00,10,28,2,3,1,3,20,30,85,60,0,3,1,3,20,220,85,60,0,0,0,0,0,0,0,0,0},
	{377,7,'H',1,"X377X7H",510,260,416,10,180,490,14,0.00,10,28,2,4,1,4,20,30,90,60,0,4,1,4,20,280,90,60,0,0,0,0,0,0,0,0,0},
	{406,7,'H',1,"X406X7H",520,270,436,10,190,490,14,0.00,10,28,2,4,1,4,20,30,95,60,0,4,1,4,20,280,95,60,0,0,0,0,0,0,0,0,0},
	{377,8,'H',1,"X377X8H",510,260,416,10,180,490,14,0.00,10,28,2,4,1,4,20,30,90,60,0,4,1,4,20,280,90,60,0,0,0,0,0,0,0,0,0},
	{406,8,'H',1,"X406X8H",530,280,456,10,200,490,14,0.00,10,28,2,4,1,4,20,30,100,60,0,4,1,4,20,280,100,60,0,0,0,0,0,0,0,0,0},
	{426,8,'H',1,"X426X8H",540,290,476,10,210,490,14,0.00,10,28,2,4,1,4,20,30,105,60,0,4,1,4,20,280,105,60,0,0,0,0,0,0,0,0,0},
	{426,10,'H',1,"X426X10H",540,290,484,10,210,490,16,0.00,10,32,2,4,1,4,24,30,105,60,0,4,1,4,24,280,105,60,0,0,0,0,0,0,0,0},
	{114,4,'S',1,"X114X4S",200,80,132,6,50,230,8,0.00,10,16,2,2,1,2,16,30,25,50,0,2,1,2,16,150,25,50,0,0,0,0,0,0,0,0,0},
	{140,4,'S',1,"X140X4S",210,90,172,6,70,230,8,0.00,10,16,2,2,1,2,16,30,35,50,0,2,1,2,16,150,35,50,0,0,0,0,0,0,0,0,0},
	{159,4,'S',1,"X159X4S",230,100,192,6,80,230,8,0.00,10,16,2,2,1,2,16,30,40,50,0,2,1,2,16,150,40,50,0,0,0,0,0,0,0,0,0},
	{168,4,'S',1,"X168X4S",230,110,192,6,80,230,8,0.00,10,16,2,2,1,2,16,30,40,50,0,2,1,2,16,150,40,50,0,0,0,0,0,0,0,0,0},
	{159,5,'S',1,"X159X5S",230,110,180,8,70,230,10,0.00,10,20,2,2,1,2,16,30,35,50,0,2,1,2,16,150,35,50,0,0,0,0,0,0,0,0,0},
	{168,5,'S',1,"X168X5S",230,110,200,8,80,230,10,0.00,10,20,2,2,1,2,16,30,40,50,0,2,1,2,16,150,40,50,0,0,0,0,0,0,0,0,0},
	{194,5,'S',1,"X194X5S",240,120,220,8,90,230,10,0.00,10,20,2,2,1,2,16,30,45,50,0,2,1,2,16,150,45,50,0,0,0,0,0,0,0,0,0},
	{219,5,'S',1,"X219X5S",270,140,240,8,100,250,10,0.00,10,20,2,2,1,2,20,30,50,60,0,2,1,2,20,160,50,60,0,0,0,0,0,0,0,0,0},
	{219,6,'S',1,"X219X6S",270,140,260,8,110,250,10,0.00,10,20,2,2,1,2,20,30,55,60,0,2,1,2,20,160,55,60,0,0,0,0,0,0,0,0,0},
	{273,6,'S',1,"X273X6S",360,170,308,8,130,370,12,0.00,10,24,2,3,1,3,20,30,65,60,0,3,1,3,20,220,65,60,0,0,0,0,0,0,0,0,0},
	{325,6,'S',1,"X325X6S",390,190,368,8,160,370,12,0.00,10,24,2,3,1,3,20,30,80,60,0,3,1,3,20,220,80,60,0,0,0,0,0,0,0,0,0},
	{356,6,'S',1,"X356X6S",400,210,388,8,170,370,12,0.00,10,24,2,3,1,3,20,30,85,60,0,3,1,3,20,220,85,60,0,0,0,0,0,0,0,0,0},
	{325,7,'S',1,"X325X7S",390,200,368,8,160,370,12,0.00,10,24,2,3,1,3,20,30,80,60,0,3,1,3,20,220,80,60,0,0,0,0,0,0,0,0,0},
	{356,7,'S',1,"X356X7S",400,210,396,8,170,370,14,0.00,10,28,2,3,1,3,20,30,85,60,0,3,1,3,20,220,85,60,0,0,0,0,0,0,0,0,0},
	{377,7,'S',1,"X377X7S",410,220,416,8,180,370,14,0.00,10,28,2,3,1,3,20,30,90,60,0,3,1,3,20,220,90,60,0,0,0,0,0,0,0,0,0},
	{406,7,'S',1,"X406X7S",430,240,444,8,190,370,16,0.00,10,32,2,3,1,3,20,30,95,60,0,3,1,3,20,220,95,60,0,0,0,0,0,0,0,0,0},
	{377,8,'S',1,"X377X8S",420,230,424,10,180,370,16,0.00,10,32,2,3,1,3,20,30,90,60,0,3,1,3,20,220,90,60,0,0,0,0,0,0,0,0,0},
	{406,8,'S',1,"X406X8S",490,240,444,10,190,490,16,0.00,10,32,2,4,1,4,20,30,95,60,0,4,1,4,20,280,95,60,0,0,0,0,0,0,0,0,0},
	{426,8,'S',1,"X426X8S",500,250,464,10,200,490,16,0.00,10,32,2,4,1,4,20,30,100,60,0,4,1,4,20,280,100,60,0,0,0,0,0,0,0,0,0},
	{426,10,'S',1,"X426X10S",560,250,474,10,210,550,16,0.00,10,27,2,5,1,5,20,30,105,60,0,4,1,4,20,340,105,60,0,0,0,0,0,0,0,0,0},*/
};
//根据当前主管壁厚cur_t返回相应分组的理论重量(平焊法兰返回第一分组理论重量),kg
double FL_PARAM::Weight()
{
	if(toupper(codeName[1])=='P')	//对焊法兰
		return group[0].weight;
	else
	{
		for(int i=0;i<5;i++)
		{
			if(group[i].t==t)
				return group[i].weight;
			else if(group[i].t<=0)
				return group[i].weight;	//找不到相应的分组数据信息(有可能是分组数据填写错误)
		}
	}
	return 0;
}
char FL_PARAM::ThickGroupSymbol(int thick/*=0*/)//返回当前壁厚或指定壁厚对应的分组字符 A,B,C,D,E
{
	if(thick==0)
		thick=(int)t;
	for(int i=0;i<5;i++)
	{
		if(group[i].t==0)
			break;
		if(group[i].t==thick)
			return 'A'+i;
	}
	return '?';
}
//对焊法兰代号中添加主管壁厚分组标识
void FL_PARAM::UpdateCodeName()
{
	CXhChar16 label(codeName);
	int len=label.GetLength();
	if(label[len-1]>='0'&& label[len-1]<='9')
	{	//对焊法兰代号中没有级别
		label.At(len)=ThickGroupSymbol();
		label.At(len+1)=0;
	}
	else
	{	//对焊法兰代号中含有级别
		for(int i=2;i<len;i++)
		{
			if(label[i]<'0'||label[i]>'9')
			{
				label.At(i)=ThickGroupSymbol();
				break;
			}
		}
	}
	strncpy(codeName,label,16);
}
//判断对焊法兰除主管壁厚分组标识字符外其余代号是否一致
bool FL_PARAM::IsSameCode(const char* code)	
{
	//处理标准法兰库中的法兰代号字符串(可能不含壁厚标识字符)
	char code1[16]="";
	strcpy(code1,codeName);
	int nLen=strlen(code1);
	if(code1[nLen-1]>='0'&& code1[nLen-1]<='9')
		strcat(code1,"A");	//填充壁厚标识符
	for(UINT i=2;i<strlen(code1);i++)
	{
		if(code1[i]<'0'||code1[i]>'9')
		{
			code1[i]='T';
			break;
		}
	}
	//处理选中对焊法兰的代号字符串(含壁厚标识字符)
	char code2[16]="";
	strcpy(code2,code);
	 nLen=strlen(code2);
	if(code2[nLen-1]>='0'&& code2[nLen-1]<='9')
		strcat(code2,"A");	//填充壁厚标识符
	for(UINT i=2;i<strlen(code2);i++)
	{
		if(code2[i]<'0'||code2[i]>'9')
		{
			code2[i]='T';
			break;
		}
	}
	//
	if(stricmp(code1,code2)==0)
		return true;
	else
		return false;
}
//对焊法兰类型库
int fld_param_N=110;
FL_PARAM fld_param_table[500] =
{
	{159,159,0,'A','H',"FD1515",161,147,270,219,22,10,20,20,62,173,2,20,6,7.09,4,7.09,5,0,0,0,0,0,0},
	{159,168,0,'B','H',"FD1615",161,147,284,228,22,10,20,20,62,174,2,20,6,8.08,4,8.08,5,0,0,0,0,0,0},
	{159,180,0,'C','H',"FD1815",161,147,288,241,22,14,20,22,66,174,2,20,6,8.88,4,8.88,5,0,0,0,0,0,0},
	{159,194,0,'D','H',"FD1915",161,147,310,255,22,14,20,22,66,174,2,20,6,10.67,4,10.67,5,0,0,0,0,0,0},
	{168,168,0,'A','H',"FD1616",170,168,284,228,22,10,20,20,62,182,2,20,6,7.79,4,7.79,5,0,0,0,0,0,0},
	{168,180,0,'B','H',"FD1816",170,168,288,241,22,14,20,22,66,183,2,20,6,8.60,4,8.60,5,0,0,0,0,0,0},
	{168,194,0,'C','H',"FD1916",170,168,310,255,22,14,20,22,66,183,2,20,6,10.38,4,10.38,5,0,0,0,0,0,0},
	{168,203,0,'D','H',"FD2016",170,168,310,265,22,16,20,24,70,184,2,20,6,11.18,4,11.18,5,0,0,0,0,0,0},
	{180,180,0,'A','H',"FD1818",182,180,288,241,22,14,20,22,66,195,2,20,6,8.46,5,8.46,6,0,0,0,0,0,0},
	{180,194,0,'B','H',"FD1918",182,180,310,255,22,14,20,22,66,200,2,20,6,10.39,5,10.39,6,0,0,0,0,0,0},
	{180,203,0,'C','H',"FD2018",182,180,310,265,22,16,20,24,70,206,2,20,6,11.33,5,11.33,6,0,0,0,0,0,0},
	{180,219,0,'D','H',"FD2118",182,180,336,283,22,16,20,26,75,214,2,20,6,15.21,5,15.21,6,0,0,0,0,0,0},
	{194,194,0,'A','H',"FD1919",196,194,310,255,22,14,20,22,66,209,2,20,6,9.73,5,9.73,6,0,0,0,0,0,0},
	{194,203,0,'B','H',"FD2019",196,194,310,265,22,16,20,24,70,216,2,20,6,10.66,5,10.66,6,0,0,0,0,0,0},
	{194,219,0,'C','H',"FD2119",196,194,336,283,22,16,20,26,75,224,2,20,6,14.49,5,14.49,6,0,0,0,0,0,0},
	{194,245,0,'D','H',"FD2419",196,194,362,309,22,18,20,26,75,234,2,20,6,17.63,5,17.63,6,0,0,0,0,0,0},
	{203,203,0,'A','H',"FD2020",205,203,310,265,22,16,20,24,70,219,2,20,6,10.07,5,10.07,6,0,0,0,0,0,0},
	{203,219,0,'B','H',"FD2120",205,203,336,283,22,16,20,26,75,230,2,20,6,13.97,5,13.97,6,0,0,0,0,0,0},
	{203,245,0,'C','H',"FD2420",205,203,362,309,22,18,20,26,75,240,2,20,6,17.13,5,17.13,6,0,0,0,0,0,0},
	{203,273,0,'D','H',"FD2720",205,203,392,338,22,20,20,28,79,252,2,20,6,22.67,5,22.67,6,0,0,0,0,0,0},
	{219,219,0,'A','H',"FD2121",221,219,336,283,22,16,20,26,75,236,2,20,6,12.79,5,12.79,6,0,0,0,0,0,0},
	{219,245,0,'B','H',"FD2421",221,219,362,309,22,18,20,26,75,250,2,20,6,16.00,5,16.00,6,0,0,0,0,0,0},
	{219,273,0,'C','H',"FD2721",221,219,392,338,22,20,20,28,79,264,2,20,6,21.70,5,21.70,6,0,0,0,0,0,0},
	{219,299,0,'D','H',"FD2921",221,219,434,373,26,18,24,32,88,260,2,20,6,30.85,5,30.85,6,0,0,0,0,0,0},
	{245,245,0,'A','H',"FD2424",247,245,362,309,22,18,20,26,75,262,2,20,6,14.05,5,14.05,6,0,0,0,0,0,0},
	{245,273,0,'B','H',"FD2724",247,245,392,338,22,20,20,28,79,278,2,20,6,19.59,5,19.59,6,0,0,0,0,0,0},
	{245,299,0,'C','H',"FD2924",247,245,434,373,26,18,24,32,88,272,2,20,6,28.29,5,28.29,6,0,0,0,0,0,0},
	{245,325,0,'D','H',"FD3224",247,245,460,401,26,20,24,34,92,282,2,20,6,35.23,5,35.23,6,0,0,0,0,0,0},
	{273,273,0,'A','H',"FD2727",275,273,392,338,22,20,20,28,79,292,2,20,6,16.87,5,16.87,6,0,0,0,0,0,0},
	{273,299,0,'B','H',"FD2927",275,273,434,373,26,18,24,32,88,294,2,20,6,25.68,5,25.68,6,0,0,0,0,0,0},
	{273,325,0,'C','H',"FD3227",275,273,460,401,26,20,24,34,92,300,2,20,6,32.21,5,32.21,6,0,0,0,0,0,0},
	{273,356,0,'D','H',"FD3527",275,273,500,435,26,24,24,36,97,300,2,20,6,41.98,5,41.98,6,0,0,0,0,0,0},
	{299,299,0,'A','H',"FD2929",301,299,434,373,26,18,24,32,88,320,2,20,6,23.90,6,23.90,7,0,0,0,0,0,0},
	{299,325,0,'B','H',"FD3229",301,299,460,401,26,20,24,34,92,336,2,20,6,31.11,6,31.11,7,0,0,0,0,0,0},
	{299,356,0,'C','H',"FD3529",301,299,500,435,26,24,24,36,97,336,2,20,6,40.80,6,40.80,7,0,0,0,0,0,0},
	{299,377,0,'D','H',"FD3729",301,299,522,458,26,26,24,38,101,346,2,20,6,48.84,6,48.84,7,0,0,0,0,0,0},
	{325,325,0,'A','H',"FD3232",327,325,460,401,26,20,24,34,92,348,2,20,6,27.25,6,27.25,7,0,0,0,0,0,0},
	{325,356,0,'B','H',"FD3532",327,325,500,435,26,24,24,36,97,350,2,20,6,36.92,6,36.92,7,0,0,0,0,0,0},
	{325,377,0,'C','H',"FD3732",327,325,522,458,26,26,24,38,101,362,2,20,6,44.97,6,44.97,7,0,0,0,0,0,0},
	{325,406,0,'D','H',"FD4032",327,325,566,495,29,24,27,40,106,362,2,20,6,58.44,6,58.44,7,0,0,0,0,0,0},
	{356,356,0,'A','H',"FD3535",358,356,500,435,26,24,24,36,97,380,2,20,7,33.76,7,33.76,8,0,0,0,0,0,0},
	{356,377,0,'B','H',"FD3735",358,356,522,458,26,26,24,38,101,394,2,20,7,41.93,7,41.93,8,0,0,0,0,0,0},
	{356,406,0,'C','H',"FD4035",358,356,566,495,29,24,27,40,106,394,2,20,7,55.23,7,55.23,8,0,0,0,0,0,0},
	{356,426,0,'D','H',"FD4235",358,356,590,519,29,26,27,42,110,402,2,20,7,65.58,7,65.58,8,0,0,0,0,0,0},
	{377,377,0,'A','H',"FD3737",379,377,522,458,26,26,24,38,101,402,2,20,8,37.66,7,37.66,8,0,0,0,0,0,0},
	{377,406,0,'B','H',"FD4037",379,377,566,495,29,24,27,40,106,404,2,20,8,50.93,7,50.93,8,0,0,0,0,0,0},
	{377,426,0,'C','H',"FD4237",379,377,590,519,29,26,27,42,110,414,2,20,8,61.34,7,61.34,8,0,0,0,0,0,0},
	{377,457,0,'D','H',"FD4537",379,377,644,557,32,24,30,44,114,414,2,20,8,81.53,7,81.53,8,0,0,0,0,0,0},
	{406,406,0,'A','H',"FD4040",408,406,566,495,29,24,27,40,106,432,2,20,8,47.20,8,47.20,9,0,0,0,0,0,0},
	{406,426,0,'B','H',"FD4240",408,406,590,519,29,26,27,42,110,444,2,20,8,57.69,8,57.69,9,0,0,0,0,0,0},
	{406,457,0,'C','H',"FD4540",408,406,644,557,32,24,30,44,114,444,2,20,8,77.70,8,77.70,9,0,0,0,0,0,0},
	{406,480,0,'D','H',"FD4840",408,406,662,582,32,26,30,46,119,460,2,20,8,89.52,8,89.52,9,0,0,0,0,0,0},
	{426,426,0,'A','H',"FD4242",428,426,590,519,29,26,27,42,115,454,2,25,10,53.37,8,53.37,9,0,0,0,0,0,0},
	{426,457,0,'B','H',"FD4542",428,426,644,557,32,24,30,44,119,455,2,25,10,73.32,8,73.32,9,0,0,0,0,0,0},
	{426,480,0,'C','H',"FD4842",428,426,662,582,32,26,30,46,124,470,2,25,10,84.83,8,84.83,9,0,0,0,0,0,0},
	{426,508,0,'D','H',"FD5042",428,426,704,615,35,24,33,48,128,470,2,25,10,104.58,8,104.58,9,0,0,0,0,0,0},
	{457,457,0,'A','H',"FD4545",459,457,644,557,32,24,30,44,119,486,2,25,10,68.41,9,68.41,10,0,0,0,0,0,0},
	{457,480,0,'B','H',"FD4845",459,457,662,582,32,26,30,46,124,502,2,25,10,79.99,9,79.99,10,0,0,0,0,0,0},
	{457,508,0,'C','H',"FD5045",459,457,704,615,35,24,33,48,128,502,2,25,10,99.51,9,99.51,10,0,0,0,0,0,0},
	{457,529,0,'D','H',"FD5245",459,457,726,640,35,26,33,50,133,510,2,25,10,113.88,9,113.88,10,0,0,0,0,0,0},
	{480,480,0,'A','H',"FD4848",482,480,662,582,32,26,30,46,124,510,2,25,10,72.59,9,72.59,10,0,0,0,0,0,0},
	{480,508,0,'B','H',"FD5048",482,480,704,615,35,24,33,48,128,512,2,25,10,92.00,9,92.00,10,0,0,0,0,0,0},
	{480,529,0,'C','H',"FD5248",482,480,726,640,35,26,33,50,133,522,2,25,10,106.52,9,106.52,10,0,0,0,0,0,0},
	{480,559,0,'D','H',"FD5548",482,480,766,677,35,28,33,52,137,538,2,25,10,132.06,9,132.06,10,0,0,0,0,0,0},
	{508,508,0,'A','H',"FD5050",510,508,704,615,35,24,33,48,128,540,2,25,10,86.66,10,86.66,11,0,0,0,0,0,0},
	{508,529,0,'B','H',"FD5250",510,508,726,640,35,26,33,50,133,552,2,25,10,101.43,10,101.43,11,0,0,0,0,0,0},
	{508,559,0,'C','H',"FD5550",510,508,766,677,35,28,33,52,137,570,2,25,10,127.32,10,127.32,11,0,0,0,0,0,0},
	{508,584,0,'D','H',"FD5850",510,508,800,704,38,26,36,54,141,570,2,25,10,148.75,10,148.75,11,0,0,0,0,0,0},
	{529,529,0,'A','H',"FD5252",531,529,726,640,35,26,33,50,133,562,2,25,12,93.69,10,93.69,11,0,0,0,0,0,0},
	{529,559,0,'B','H',"FD5552",531,529,766,677,35,28,33,52,137,580,2,25,12,119.39,10,119.39,11,0,0,0,0,0,0},
	{529,584,0,'C','H',"FD5852",531,529,800,704,38,26,36,54,141,580,2,25,12,140.50,10,140.50,11,0,0,0,0,0,0},
	{529,610,0,'D','H',"FD6152",531,529,836,731,38,26,36,56,146,594,2,25,12,169.10,10,169.10,11,0,0,0,0,0,0},
	{559,559,0,'A','H',"FD5555",561,559,766,677,35,28,33,52,137,593,2,25,12,107.08,10,107.08,11,0,0,0,0,0,0},
	{559,584,0,'B','H',"FD5855",561,559,800,704,38,26,36,54,141,594,2,25,12,127.99,10,127.99,11,0,0,0,0,0,0},
	{559,610,0,'C','H',"FD6155",561,559,836,731,38,26,36,56,146,606,2,25,12,155.65,10,155.65,11,0,0,0,0,0,0},
	{559,630,0,'D','H',"FD6355",561,559,854,757,41,26,39,56,146,606,2,25,12,164.02,10,164.02,11,0,0,0,0,0,0},
	{584,584,0,'A','H',"FD5858",586,584,800,704,38,26,36,54,141,619,2,25,12,121.85,11,121.85,12,0,0,0,0,0,0},
	{584,610,0,'B','H',"FD6158",586,584,836,731,38,26,36,56,146,636,2,25,12,150.61,11,150.61,12,0,0,0,0,0,0},
	{584,630,0,'C','H',"FD6358",586,584,854,757,41,26,39,56,146,636,2,25,12,158.99,11,158.99,12,0,0,0,0,0,0},
	{584,660,0,'D','H',"FD6658",586,584,896,787,41,26,39,56,146,646,2,25,12,186.85,11,186.85,12,0,0,0,0,0,0},
	{610,610,0,'A','H',"FD6161",612,610,836,731,38,26,36,56,146,647,2,25,12,137.77,11,137.77,12,0,0,0,0,0,0},
	{610,630,0,'B','H',"FD6361",612,610,854,757,41,26,39,56,146,647,2,25,12,146.15,11,146.15,12,0,0,0,0,0,0},
	{610,660,0,'C','H',"FD6661",612,610,896,787,41,26,39,56,146,658,2,25,12,174.41,11,174.41,12,0,0,0,0,0,0},
	{610,711,0,'D','H',"FD7161",612,610,960,853,48,24,45,58,150,668,2,25,12,221.49,11,221.49,12,0,0,0,0,0,0},
	{630,630,0,'A','H',"FD6363",632,630,854,757,41,26,39,56,146,667,2,25,12,141.01,12,141.01,13,0,0,0,0,0,0},
	{630,660,0,'B','H',"FD6663",632,630,896,787,41,26,39,56,146,684,2,25,12,170.95,12,170.95,13,0,0,0,0,0,0},
	{630,711,0,'C','H',"FD7163",632,630,960,853,48,24,45,58,150,700,2,25,12,219.68,12,219.68,13,0,0,0,0,0,0},
	{630,762,0,'D','H',"FD7663",632,630,1026,904,48,26,45,58,150,700,2,25,12,264.90,12,264.90,13,0,0,0,0,0,0},
	{660,660,0,'A','H',"FD6666",662,660,896,787,41,26,39,56,146,697,2,25,12,155.02,12,155.02,13,0,0,0,0,0,0},
	{660,711,0,'B','H',"FD7166",662,660,960,853,48,24,45,58,150,714,2,25,12,203.66,12,203.66,13,0,0,0,0,0,0},
	{660,762,0,'C','H',"FD7666",662,660,1026,904,48,26,45,58,150,718,2,25,12,250.04,12,250.04,13,0,0,0,0,0,0},
	{660,813,0,'D','H',"FD8166",662,660,1096,965,51,26,48,62,159,722,2,25,12,322.02,12,322.02,13,0,0,0,0,0,0},
	{711,711,0,'A','H',"FD7171",714,711,960,853,48,24,45,58,150,750,3,25,12,182.20,13,182.20,14,0,0,0,0,0,0},
	{711,762,0,'B','H',"FD7671",714,711,1026,904,48,26,45,58,150,764,3,25,12,231.74,13,231.74,14,0,0,0,0,0,0},
	{711,813,0,'C','H',"FD8171",714,711,1096,965,51,26,48,62,159,776,3,25,12,305.13,13,305.13,14,0,0,0,0,0,0},
	{711,864,0,'D','H',"FD8671",714,711,1150,1017,51,28,48,64,163,800,3,25,12,368.99,13,368.99,14,0,0,0,0,0,0},
	{762,762,0,'A','H',"FD7676",765,762,1026,904,48,26,45,58,155,801,3,30,12,208.79,14,208.79,15,0,0,0,0,0,0},
	{762,813,0,'B','H',"FD8176",765,762,1096,965,51,26,48,62,164,820,3,30,12,283.12,14,283.12,15,0,0,0,0,0,0},
	{762,864,0,'C','H',"FD8676",765,762,1150,1017,51,28,48,64,168,846,3,30,12,347.45,14,347.45,15,0,0,0,0,0,0},
	{762,914,0,'D','H',"FD9176",765,762,1216,1076,55,26,52,68,177,856,3,30,12,435.69,14,435.69,15,0,0,0,0,0,0},
	{813,813,0,'A','H',"FD8181",816,813,1096,965,51,26,48,62,164,855,3,30,12,254.23,15,254.23,16,0,0,0,0,0,0},
	{813,864,0,'B','H',"FD8681",816,813,1150,1017,51,28,48,64,168,886,3,30,12,320.20,15,320.20,16,0,0,0,0,0,0},
	{813,914,0,'C','H',"FD9181",816,813,1216,1076,55,26,52,68,177,904,3,30,12,410.29,15,410.29,16,0,0,0,0,0,0},
	{813,965,0,'D','H',"FD9681",816,813,1296,1138,59,28,56,74,190,904,3,30,12,528.78,15,528.78,16,0,0,0,0,0,0},
	{864,864,0,'A','H',"FD8686",867,864,1150,1017,51,28,48,64,168,907,3,30,12,282.94,16,282.94,17,0,0,0,0,0,0},
	{864,914,0,'B','H',"FD9186",867,864,1216,1076,55,26,52,68,177,928,3,30,12,372.26,16,372.26,17,0,0,0,0,0,0},
	{864,965,0,'C','H',"FD9686",867,864,1296,1138,59,28,56,74,190,928,3,30,12,487.26,16,487.26,17,0,0,0,0,0,0},
	{914,914,0,'A','H',"FD9191",917,914,1216,1076,55,26,52,68,177,959,3,30,12,335.92,17,335.92,18,0,0,0,0,0,0},
	{914,965,0,'B','H',"FD9691",917,914,1296,1138,59,28,56,74,190,963,3,30,12,449.53,17,449.53,18,0,0,0,0,0,0},
	{965,965,0,'A','H',"FD9696",968,965,1296,1138,59,28,56,74,190,1014,3,30,12,429.27,18,429.27,19,429.27,20,429.27,21,429.27,22},
	/* 因更换插板库注释掉(2011.10.22版本) wjh-2013.6.5
	{159,0,'A','H',"FD1515",161,147,268,222,22,10,20,18,60,181,2,20,6,{ 6.61,0}},
	{159,0,'B','H',"FD1615",161,147,276,230,22,10,20,18,60,190,2,20,6,{ 7.31,0}},
	{159,0,'C','H',"FD1915",161,147,310,260,22,14,20,20,64,190,2,20,6,{10.29,0}},
	{159,0,'D','H',"FD2115",161,147,335,288,22,16,20,20,73,200,2,20,6,{14.82,0}},
	{168,0,'A','H',"FD1616",170,156,276,230,22,10,20,18,60,190,2,20,6,{ 6.88,0}},
	{168,0,'B','H',"FD1916",170,156,310,260,22,14,20,20,64,200,2,20,6,{10.09,0}},
	{168,0,'C','H',"FD2116",170,156,335,288,22,16,20,24,73,210,2,20,6,{14.59,0}},
	{168,0,'D','H',"FD2716",170,156,392,346,22,22,20,30,86,220,2,20,6,{25.69,0}},
	{194,0,'A','H',"FD1919",196,180,310,260,22,14,20,20,64,218,2,20,6,{ 9.36,0}},
	{194,0,'B','H',"FD2119",196,180,335,288,22,16,20,24,73,240,2,20,6,{14.17,0}},
	{194,0,'C','H',"FD2719",196,180,392,346,22,22,20,30,86,260,2,20,6,{25.71,0}},
	{194,0,'D','H',"FD3219",196,180,466,414,26,22,24,34,95,260,2,20,6,{41.48,0}},
	{219,0,'A','H',"FD2121",221,205,335,288,22,16,20,24,73,248,2,20,6,{12.44,0}},
	{219,0,'B','H',"FD2721",221,205,392,346,22,22,20,30,86,270,2,20,6,{23.70,0}},
	{219,0,'C','H',"FD3221",221,205,466,414,22,22,24,34,95,270,2,20,6,{39.19,0}},
	{219,0,'D','H',"FD3521",221,205,500,442,26,24,24,36,99,280,2,20,6,{49.13,0}},
	{273,0,'A','H',"FD2727",275,257,392,346,22,22,20,30,86,309,2,20,6,{19.76,0}},
	{273,0,'B','H',"FD3227",275,257,466,414,26,22,24,34,95,330,2,20,6,{36.32,0}},
	{273,0,'C','H',"FD3527",275,257,500,442,22,24,24,36,99,350,2,20,6,{47.09,0}},
	{273,0,'D','H',"FD3727",275,257,544,474,29,22,27,38,104,330,2,20,6,{58.08,0}},
	{325,0,'A','H',"FD3232",327,307,466,414,26,22,24,34, 95,365,2,20,6,{30.77,0}},
	{325,0,'B','H',"FD3532",327,307,500,442,26,24,24,36, 99,390,2,20,6,{41.89,0}},
	{325,0,'C','H',"FD3732",327,307,544,474,29,22,27,38,104,380,2,20,6,{53.33,0}},
	{325,0,'D','H',"FD4032",327,307,580,505,29,24,27,38,104,380,2,20,6,{62.43,0}},
	{356,0,'A','H',"FD3535",358,338,500,442,26,24,24,36, 99,390,2,20,6,{36.07,0}},
	{356,0,'B','H',"FD3735",358,338,544,474,29,22,27,38,104,401,2,20,7,{48.46,0}},
	{356,0,'C','H',"FD4035",358,338,580,505,29,24,27,38,104,401,2,20,7,{57.56,0}},
	{356,0,'D','H',"FD4235",358,338,612,534,35,20,33,40,108,403,2,20,7,{69.10,0}},
	{377,0,'A','H',"FD3737",379,355,544,474,29,22,27,38,104,422,2,20,8,{47.44,0}},
	{377,0,'B','H',"FD4037",379,355,580,505,29,24,27,38,104,436,2,20,8,{58.22,0}},
	{377,0,'C','H',"FD4237",379,355,612,534,35,20,33,40,108,430,2,20,8,{68.78,0}},
	{377,0,'D','H',"FD4537",379,355,650,570,35,20,33,42,112,440,2,20,8,{85.21,0}},
	{406,0,'A','H',"FD4040",408,384,580,505,29,24,27,38,104,451,2,20,8,{52.43,0}},
	{406,0,'B','H',"FD4240",408,384,612,534,35,20,33,40,108,453,2,20,8,{63.70,0}},
	{406,0,'C','H',"FD4540",408,384,650,570,35,22,33,42,112,455,2,20,8,{78.88,0}},
	{406,0,'D','H',"FD4840",408,384,710,610,38,22,36,46,121,459,2,20,8,{108.63,0}},
	{426,0,'A','H',"FD4242",428,400,612,534,35,20,33,40,113,473,2,25,10,{ 63.34,0}},
	{426,0,'B','H',"FD4542",428,400,650,570,35,22,33,42,117,494,2,25,10,{ 81.20,0}},
	{426,0,'C','H',"FD4842",428,400,710,610,38,22,36,46,126,490,2,25,10,{109.75,0}},
	{426,0,'D','H',"FD5042",428,400,730,640,41,20,39,48,131,500,2,25,10,{124.01,0}},
	{457,0,'A','H',"FD4545",459,431,650,570,35,22,33,42,117,506,2,25,10,{ 73.04,0}},
	{457,0,'B','H',"FD4845",459,431,710,610,38,22,36,46,126,510,2,25,10,{102.17,0}},
	{457,0,'C','H',"FD5045",459,431,730,640,41,20,39,48,131,520,2,25,10,{116.10,0}},
	{457,0,'D','H',"FD5245",459,431,770,664,41,22,39,50,135,520,2,25,10,{138.26,0}},
	{480,0,'A','H',"FD4848",482,450,710,610,38,22,36,46,126,533,2,25,10,{100.25,0}},
	{480,0,'B','H',"FD5048",482,450,730,640,41,20,39,48,131,554,2,25,10,{116.20,0}},
	{480,0,'C','H',"FD5248",482,450,770,664,41,22,39,50,135,550,2,25,10,{137.55,0}},
	{480,0,'D','H',"FD5548",482,450,778,690,41,22,39,50,135,588,2,25,10,{148.96,0}},
	{508,0,'A','H',"FD5050",510,478,730,640,41,20,39,48,131,564,2,25,10,{106.55,0}},
	{508,0,'B','H',"FD5250",510,478,770,664,41,22,39,50,135,580,2,25,10,{131.59,0}},
	{508,0,'C','H',"FD5550",510,478,778,690,41,22,39,50,135,600,2,25,10,{139.58,0}},
	{508,0,'D','H',"FD6150",510,478,846,745,45,24,42,50,135,600,2,25,10,{172.00,0}},
	{529,0,'A','H',"FD5252",531,499,770,664,41,22,39,50,135,587,2,25,12,{123.53,0}},
	{529,0,'B','H',"FD5552",531,499,778,690,41,22,39,50,135,606,2,25,12,{131.41,0}},
	{529,0,'C','H',"FD6152",531,499,846,745,45,24,42,50,135,610,2,25,12,{164.69,0}},
	{529,0,'D','H',"FD6352",531,499,890,776,45,24,42,54,144,618,2,25,12,{203.41,0}},
	{559,0,'A','H',"FD5555",561,529,778,690,41,22,39,50,135,617,2,25,12,{119.47,0}},
	{559,0,'B','H',"FD6155",561,529,846,745,45,24,42,50,135,622,2,25,12,{153.02,0}},
	{559,0,'C','H',"FD6355",561,529,890,776,45,24,42,52,139,632,2,25,12,{184.44,0}},
	{559,0,'D','H',"FD6655",561,529,906,800,45,26,42,56,148,652,2,25,12,{213.45,0}},
	{610,0,'A','H',"FD6161",612,576,846,745,45,24,42,50,135,668,2,25,12,{141.07,0}},
	{610,0,'B','H',"FD6361",612,576,890,776,45,24,42,52,139,684,2,25,12,{173.68,0}},
	{610,0,'C','H',"FD6661",612,576,906,800,45,26,42,52,139,702,2,25,12,{187.71,0}},
	{610,0,'D','H',"FD7161",612,576,1010,880,51,24,48,62,161,694,2,25,12,{290.89,0}},
	{630,0,'A','H',"FD6363",632,596, 890,776,45,24,42,52,139,690,2,25,12,{163.90,0}},
	{630,0,'B','H',"FD6663",632,596, 906,800,45,26,42,52,139,708,2,25,12,{177.96,0}},
	{630,0,'C','H',"FD7163",632,596,1010,880,51,24,48,60,157,704,2,25,12,{271.61,0}},
	{630,0,'D','H',"FD7663",632,596,1050,926,55,22,52,66,170,730,2,25,12,{338.66,0}},
	{660,0,'A','H',"FD6666",662,626, 906,800,45,26,42,52,139,720,2,25,12,{163.40,0}},
	{660,0,'B','H',"FD7166",662,626,1010,880,51,24,48,56,148,725,2,25,12,{240.68,0}},
	{660,0,'C','H',"FD7666",662,626,1050,926,55,22,52,60,157,746,2,25,12,{293.04,0}},
	{660,0,'D','H',"FD8166",662,626,1120,986,55,24,52,68,175,760,2,25,12,{397.52,0}},
	{711,0,'A','H',"FD7171",714,672,1010,880,52,24,48,56,148,776,3,25,12,{226.08,0}},
	{711,0,'B','H',"FD7671",714,672,1050,926,55,22,52,58,153,798,3,25,12,{269.14,0}},
	{711,0,'C','H',"FD8171",714,672,1120,986,55,24,52,66,170,824,3,25,12,{374.45,0}},
	{711,0,'D','H',"FD8671",714,672,1180,1038,55,26,52,70,179,840,3,25,12,{460.27,0}},
	{762,0,'A','H',"FD7676",765,723,1050, 926,55,22,52,56,153,827,3,30,12,{233.97,0}},
	{762,0,'B','H',"FD8176",765,723,1120, 986,55,24,52,64,171,844,3,30,12,{330.17,0}},
	{762,0,'C','H',"FD8676",765,723,1180,1038,55,26,52,68,180,862,3,30,12,{413.03,0}},
	{762,0,'D','H',"FD9176",765,723,1280,1114,55,28,52,74,193,880,3,30,12,{566.36,0}},
	{813,0,'A','H',"FD8181",816,770,1120, 986,55,24,52,64,171,887,3,30,12,{307.69,0}},
	{813,0,'B','H',"FD8681",816,770,1180,1038,55,26,52,64,171,912,3,30,12,{370.37,0}},
	{813,0,'C','H',"FD9181",816,770,1280,1114,55,28,52,74,193,942,3,30,12,{550.45,0}},
	{813,0,'D','H',"FD9681",816,770,1330,1170,55,30,52,76,197,980,3,30,12,{642.94,0}},
	{864,0,'A','H',"FD8686",867,821,1180,1038,55,26,52,64,171,938,3,30,12,{332.51,0}},
	{864,0,'B','H',"FD9186",867,821,1280,1114,55,28,52,70,184,950,3,30,12,{471.99,0}},
	{864,0,'C','H',"FD9686",867,821,1330,1170,55,30,52,74,193,986,3,30,12,{573.51,0}},
	{914,0,'A','H',"FD9191",917,867,1280,1114,55,28,52,70,184,994,3,30,12,{445.73,0}},
	{914,0,'B','H',"FD9691",917,867,1330,1170,55,30,52,74,193,1034,3,30,12,{548.56,0}},
	{965,0,'A','H',"FD9696",968,918,1330,1170,55,30,52,74,193,1050,3,30,12,{493.92,0}},
	/*{219,6,'A','S',"FLD219A",221,205,350,300,26,12,24,20,62,245,8,18,10},
	{219,6,'B','S',"FLD219B",221,205,400,350,26,16,24,20,62,270,8,18,10},
	{219,6,'C','S',"FLD219C",221,205,455,400,26,20,24,24,71,270,8,18,10},
	{219,6,'D','S',"FLD219D",221,205,490,440,29,20,27,28,80,280,8,18,10},	
	{273,6,'A','S',"FLD273A",275,259,400,350,26,16,24,20,62,299,8,18,10},
	{273,6,'B','S',"FLD273B",275,259,455,400,26,20,24,24,71,310,8,18,10},
	{273,6,'C','S',"FLD273C",275,259,490,440,29,20,27,24,71,330,8,18,10},
	{273,6,'D','S',"FLD273D",275,259,525,470,29,22,27,26,75,340,8,18,10},	
	{325,7,'A','S',"FLD325A",328,308,455,400,26,20,24,20,62,351,10,18,10},	
	{325,7,'B','S',"FLD325B",328,308,490,440,29,20,27,24,71,370,10,18,10},	
	{325,7,'C','S',"FLD325C",328,308,525,470,29,22,27,26,75,370,10,18,10},	
	{325,7,'D','S',"FLD325D",328,308,555,500,29,24,27,28,80,380,10,18,10},	
	{356,7,'A','S',"FLD356A",359,339,490,440,29,20,27,22,66,385,10,18,10},
	{356,7,'B','S',"FLD356B",359,339,525,470,29,22,27,24,71,400,10,18,10},	
	{356,7,'C','S',"FLD356C",359,339,555,500,29,24,27,26,75,400,10,18,10},	
	{356,7,'D','S',"FLD356D",359,339,610,550,32,24,30,32,88,440,10,18,10},
	{377,8,'A','S',"FLD377A",380,358,525,470,29,22,27,24,71,408,11,18,10},
	{377,8,'B','S',"FLD377B",380,358,555,500,29,24,27,26,75,420,11,18,10},
	{377,8,'C','S',"FLD377C",380,358,610,550,32,24,30,32,88,430,11,18,10},
	{377,8,'D','S',"FLD377D",380,358,630,565,35,22,33,32,88,430,11,18,10},
	{406,8,'A','S',"FLD406A",409,387,555,500,29,24,27,24,71,437,11,18,10},
	{406,8,'B','S',"FLD406B",409,387,610,550,32,24,30,30,84,444,11,18,10},
	{406,8,'C','S',"FLD406C",409,387,630,565,35,22,33,32,88,447,11,18,10},
	{406,8,'D','S',"FLD406D",409,387,680,605,38,22,36,34,93,460,11,18,10},
	{426,10,'A','S',"FLD426A",430,402,610,550,32,24,30,32,90,467,14,20,10},
	{426,10,'B','S',"FLD426B",430,402,630,565,35,22,33,32,90,470,14,20,10},
	{426,10,'C','S',"FLD426C",430,402,680,605,38,22,36,34,95,490,14,20,10},
	{426,10,'D','S',"FLD426D",430,402,690,620,38,22,36,34,95,500,14,20,10},
	{457,10,'A','S',"FLD457A",461,433,630,565,35,22,33,32,90,498,14,20,12},
	{457,10,'B','S',"FLD457B",461,433,680,605,38,22,36,34,95,501,14,20,12},
	{457,10,'C','S',"FLD457C",461,433,690,620,38,22,36,34,95,510,14,20,12},
	{457,10,'D','S',"FLD457D",461,433,730,660,38,24,36,40,108,530,14,20,12},
	{480,12,'A','S',"FLD480A",485,451,680,605,38,22,36,34,95,525,17,20,12},	
	{480,12,'B','S',"FLD480B",485,451,690,620,38,22,36,34,95,530,17,20,12},	
	{480,12,'C','S',"FLD480C",485,451,730,660,38,24,36,44,117,550,17,20,12},	
	{480,12,'D','S',"FLD480D",485,451,760,680,41,22,39,44,117,550,17,20,12},
	{508,12,'A','S',"FLD508A",513,479,690,620,38,22,36,34,95,548,17,20,12},	
	{508,12,'B','S',"FLD508B",513,479,730,660,38,24,36,38,104,570,17,20,12},	
	{508,12,'C','S',"FLD508C",513,479,760,680,41,22,39,40,108,570,17,20,12},	
	{508,12,'D','S',"FLD508D",513,479,840,760,45,24,42,50,130,600,17,20,12},
	{529,12,'A','S',"FLD529A",534,500,730,660,38,24,36,38,104,578,17,20,12},
	{529,12,'B','S',"FLD529B",534,500,760,680,41,22,39,38,104,578,17,20,12},
	{529,12,'C','S',"FLD529C",534,500,840,760,45,24,42,50,130,610,17,20,12},
	{529,12,'D','S',"FLD529D",534,500,850,765,45,24,42,50,130,610,17,20,12},
	{559,12,'A','S',"FLD559A",564,530,760,680,41,22,39,38,104,602,17,20,12},
	{559,12,'B','S',"FLD559B",564,530,840,760,45,24,42,42,112,640,17,20,12},
	{559,12,'C','S',"FLD559C",564,530,850,765,45,24,42,42,112,640,17,20,12},
	{559,12,'D','S',"FLD559D",564,530,880,795,48,22,45,46,121,650,17,20,12},
	{610,14,'A','S',"FLD610A",616,576,840,760,45,24,42,42,112,665,20,20,12},
	{610,14,'B','S',"FLD610B",616,576,850,765,45,24,42,42,112,665,20,20,12},
	{610,14,'C','S',"FLD610C",616,576,880,795,48,22,45,46,121,680,20,20,12},
	{610,14,'D','S',"FLD610D",616,576,970,870,51,24,48,58,148,700,20,20,12},
	{630,14,'A','S',"FLD630A",636,596,850,765,45,24,42,42,112,681,20,20,12},
	{630,14,'B','S',"FLD630B",636,596,880,795,48,22,45,42,112,700,20,20,12},
	{630,14,'C','S',"FLD630C",636,596,970,870,51,24,48,50,130,710,20,20,12},
	{630,14,'D','S',"FLD630D",636,596,1010,920,51,24,48,66,165,710,20,20,12},
	{660,14,'A','S',"FLD660A",666,626,880,795,48,22,45,42,112,705,20,20,12},
	{660,14,'B','S',"FLD660B",666,626,970,870,51,24,48,48,126,730,20,20,12},
	{660,14,'C','S',"FLD660C",666,626,1010,920,51,24,48,62,156,760,20,20,12},
	{660,14,'D','S',"FLD660D",666,626,1070,975,51,26,48,74,183,760,20,20,12},
	{711,16,'A','S',"FLD711A",717,673,970,870,51,24,48,48,126,774,22,20,12},
	{711,16,'B','S',"FLD711B",717,673,1010,920,51,24,48,50,130,805,22,20,12},
	{711,16,'C','S',"FLD711C",717,673,1070,975,51,26,48,70,174,810,22,20,12},
	{711,16,'D','S',"FLD711D",717,673,1140,1038,59,24,56,76,187,825,22,20,12},
	{762,16,'A','S',"FLD762A",768,724,1010,920,51,24,48,48,128,824,22,22,12},
	{762,16,'B','S',"FLD762B",768,724,1070,975,51,26,48,50,132,855,22,22,12},
	{762,16,'C','S',"FLD762C",768,724,1140,1038,59,24,56,66,167,865,22,22,12},
	{762,16,'D','S',"FLD762D",768,724,1210,1100,59,26,56,80,198,870,22,22,12},
	{813,16,'A','S',"FLD813A",819,775,1070,975,51,26,48,50,132,878,22,22,12},
	{813,16,'B','S',"FLD813B",819,775,1140,1038,59,24,56,60,154,890,22,22,12},
	{813,16,'C','S',"FLD813C",819,775,1210,1100,59,26,56,68,172,915,22,22,12},
	{813,16,'D','S',"FLD813D",819,775,1280,1170,59,28,56,80,198,940,22,22,12},
	{864,20,'A','S',"FLD864A",872,816,1140,1038,59,24,56,60,156,926,28,24,12},
	{864,20,'B','S',"FLD864B",872,816,1210,1100,59,26,56,64,165,970,28,24,12},
	{864,20,'C','S',"FLD864C",872,816,1280,1170,59,28,56,80,200,990,28,24,12},
	{914,20,'A','S',"FLD914A",922,866,1210,1100,59,26,56,64,165,988,28,24,15},
	{914,20,'B','S',"FLD914B",922,866,1280,1170,59,28,56,76,191,1011,28,24,15},
	{965,20,'A','S',"FLD965A",973,917,1280,1170,59,28,56,66,169,1050,28,24,15},*/
};
//平焊法兰类型库
int flp_param_N=90;
FL_PARAM flp_param_table[200] =
{
	{159,159,5,'A','H',"FP1515",159,162,268,222,22,10,20,18,33,183,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{159,168,5,'B','H',"FP1615",159,162,276,230,22,10,20,18,33,183,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{159,194,5,'C','H',"FP1915",159,162,310,260,22,14,20,20,37,184,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{159,219,5,'D','H',"FP2115",159,162,335,288,22,16,20,24,44,186,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{168,168,5,'A','H',"FP1616",168,171,276,230,22,10,20,18,33,192,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{168,194,5,'B','H',"FP1916",168,171,310,260,22,14,20,20,37,193,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{168,219,5,'C','H',"FP2116",168,171,335,288,22,16,20,24,44,195,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{168,273,5,'D','H',"FP2716",168,171,392,346,22,22,20,30,56,210,7,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{194,194,6,'A','H',"FP1919",194,197,310,260,22,14,20,20,37,222,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{194,219,6,'B','H',"FP2119",194,197,335,288,22,16,20,24,44,232,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{194,273,6,'C','H',"FP2719",194,197,392,346,22,22,20,30,56,254,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{194,325,6,'D','H',"FP3219",194,197,466,414,26,22,24,34,63,260,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{219,219,6,'A','H',"FP2121",219,222,335,288,22,16,20,24,44,249,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{219,273,6,'B','H',"FP2721",219,222,392,346,22,22,20,30,56,270,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{219,325,6,'C','H',"FP3221",219,222,466,414,26,22,24,34,63,266,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{219,356,6,'D','H',"FP3521",219,222,500,442,26,24,24,36,67,260,8,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{273,273,7,'A','H',"FP2727",273,276,392,346,22,22,20,30,56,308,9,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{273,325,7,'B','H',"FP3227",273,276,466,414,26,22,24,34,63,330,9,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{273,356,7,'C','H',"FP3527",273,276,500,442,26,24,24,36,67,332,9,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{273,377,7,'D','H',"FP3727",273,276,544,474,29,22,27,38,70,320,9,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{325,325,8,'A','H',"FP3232",325,328,466,414,26,22,24,34,63,364,10,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{325,356,8,'B','H',"FP3532",325,328,500,442,26,24,24,36,67,380,10,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{325,377,8,'C','H',"FP3732",325,328,544,474,29,22,27,38,70,380,10,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{325,406,8,'D','H',"FP4032",325,328,580,505,29,24,27,38,70,380,10,0,6,0.00,0,0,0,0,0,0,0,0,0},
	{356,356,8,'A','H',"FP3535",356,359,500,442,26,24,24,36,67,396,10,0,7,0.00,0,0,0,0,0,0,0,0,0},
	{356,377,8,'C','H',"FP4035",356,359,580,505,29,24,27,38,70,396,10,0,7,0.00,0,0,0,0,0,0,0,0,0},
	{356,406,8,'B','H',"FP3735",356,359,544,474,29,22,27,38,70,396,10,0,7,0.00,0,0,0,0,0,0,0,0,0},
	{356,426,8,'D','H',"FP4235",356,359,612,534,35,20,33,40,74,398,10,0,7,0.00,0,0,0,0,0,0,0,0,0},
	{377,377,10,'A','H',"FP3737",377,380,544,474,29,22,27,38,70,423,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{377,406,10,'B','H',"FP4037",377,380,580,509,29,24,27,38,70,440,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{377,426,10,'C','H',"FP4237",377,380,612,534,35,20,33,40,70,440,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{377,457,10,'D','H',"FP4537",377,380,650,570,35,22,33,42,74,440,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{406,406,10,'A','H',"FP4040",406,409,580,505,29,24,27,38,70,452,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{406,426,10,'B','H',"FP4240",406,409,612,534,35,20,33,40,74,453,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{406,457,10,'C','H',"FP4540",406,409,650,570,35,22,33,42,74,460,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{406,480,10,'D','H',"FP4840",406,409,710,610,38,22,36,46,78,452,13,0,8,0.00,0,0,0,0,0,0,0,0,0},
	{426,426,12,'A','H',"FP4242",426,429,612,534,35,20,33,40,74,478,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{426,457,12,'B','H',"FP4542",426,429,650,570,35,22,33,42,74,494,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{426,480,12,'C','H',"FL4842",426,429,710,610,38,22,36,46,78,494,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{426,508,12,'D','H',"FP5042",426,429,730,640,41,20,39,48,82,500,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{457,457,12,'A','H',"FP4545",457,460,650,570,35,22,33,42,74,508,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{457,480,12,'B','H',"FP4845",457,460,710,610,38,22,36,48,78,508,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{457,508,12,'C','H',"FP5045",457,460,730,640,41,20,39,48,82,520,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{457,529,12,'D','H',"FP5245",457,460,770,664,41,22,39,50,85,510,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{480,480,14,'A','H',"FP4848",480,483,710,610,41,22,39,50,85,510,16,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{480,508,14,'B','H',"FP5048",480,483,730,640,41,20,39,48,82,560,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{480,529,14,'C','H',"FP5248",480,483,770,664,41,22,39,50,85,550,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{480,559,14,'D','H',"FP5548",480,483,778,690,41,22,39,50,85,590,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{508,508,14,'A','H',"FP5050",508,512,730,640,41,20,39,48,82,566,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{508,529,14,'B','H',"FP5250",508,512,770,664,41,22,39,50,85,570,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{508,559,14,'C','H',"FP5550",508,512,778,690,41,22,39,50,85,610,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{508,610,14,'D','H',"FP6150",508,512,846,745,45,24,42,50,85,610,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{529,529,14,'A','H',"FP5252",529,533,770,664,41,22,39,50,85,588,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{529,559,14,'B','H',"FP5552",529,533,778,690,41,22,39,50,85,620,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{529,610,14,'C','H',"FP6152",529,533,846,745,45,24,42,50,85,620,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{529,630,14,'D','H',"FP6352",529,533,890,776,45,24,42,54,86,630,18,0,10,0.00,0,0,0,0,0,0,0,0,0},
	{559,559,14,'A','H',"FP5555",559,563,778,690,41,22,39,50,85,618,18,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{559,610,14,'B','H',"FP6155",559,563,846,745,45,24,42,50,85,630,18,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{559,630,14,'C','H',"FP6355",559,563,890,776,45,24,42,52,85,640,18,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{559,660,14,'D','H',"FP6655",559,563,906,800,45,26,42,56,90,660,18,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{610,610,16,'A','H',"FP6161",610,614,846,745,45,24,42,50,85,674,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{610,630,16,'B','H',"FP6361",610,614,890,776,45,24,42,52,85,690,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{610,660,16,'C','H',"FP6661",610,614,906,800,45,26,42,52,85,704,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{610,711,16,'D','H',"FP7161",610,614,1010,880,51,24,48,60,96,700,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{630,630,16,'A','H',"FP6363",630,634,890,776,45,24,42,52,85,693,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{630,660,16,'B','H',"FP6663",630,634,908,800,45,26,42,52,85,710,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{630,711,16,'C','H',"FP7163",630,634,1010,880,51,24,48,60,96,710,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{630,762,16,'D','H',"FP7663",630,634,1050,926,55,22,52,66,106,734,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{660,660,16,'A','H',"FP6666",660,664,906,800,45,26,42,52,85,723,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{660,711,16,'B','H',"FP7166",660,664,1010,880,51,24,48,56,90,724,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{660,762,16,'C','H',"FP7666",660,664,1050,926,55,22,52,62,99,750,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{660,813,16,'D','H',"FP8166",660,664,1120,986,55,24,52,68,109,760,21,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{711,711,18,'A','H',"FP7171",711,715,1010,880,51,24,48,56,90,780,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{711,762,18,'B','H',"FP7671",711,715,1050,926,55,22,52,56,90,790,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{711,813,18,'C','H',"FP8171",711,715,1120,986,55,24,52,64,102,810,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{711,864,18,'D','H',"FP8671",711,715,1180,1038,55,26,52,70,112,830,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{762,762,18,'A','H',"FP7676",762,766,1050,926,55,22,52,56,90,831,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{762,813,18,'B','H',"FP8176",762,766,1120,986,55,24,52,64,102,833,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{762,864,18,'C','H',"FP8676",762,766,1180,1038,55,26,52,66,106,850,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{762,914,18,'D','H',"FP9176",762,766,1280,1114,55,28,52,72,115,870,23,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{813,813,20,'A','H',"FP8181",813,818,1120,986,55,24,52,64,102,891,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{813,864,20,'B','H',"FP8681",813,818,1180,1038,55,24,52,64,102,891,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{813,914,20,'C','H',"FP9183",813,818,1280,1114,55,28,52,70,112,930,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{813,965,20,'D','H',"FP9681",813,818,1330,1170,55,30,52,76,122,970,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{864,864,20,'A','H',"FP8686",864,869,1180,1038,55,26,52,64,102,942,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{864,914,20,'B','H',"FP9186",864,869,1280,1114,55,28,52,70,112,950,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{864,965,20,'C','H',"FP9686",864,869,1330,1170,55,30,52,74,118,986,26,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{914,914,22,'A','H',"FP9191",914,919,1280,1114,55,28,52,70,112,999,29,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{914,965,22,'B','H',"FP9691",914,919,1330,1170,55,30,52,74,118,1034,29,0,12,0.00,0,0,0,0,0,0,0,0,0},
	{965,965,22,'A','H',"FP9696",965,970,1330,1170,55,30,52,74,118,1051,29,0,12,0.00,0,0,0,0,0,0,0,0,0},
	/* 因更换插板库注释掉(2011.10.22版本) wjh-2013.6.5
	{159,5,'A','H',"FP1515",159,162,268,222,22,10,20,18,33,183,7,0,6,0},
	{159,5,'B','H',"FP1615",159,162,276,230,22,10,20,18,33,183,7,0,6,0},
	{159,5,'C','H',"FP1915",159,162,310,260,22,14,20,20,37,184,7,0,6,0},
	{159,5,'D','H',"FP2115",159,162,335,288,22,16,20,24,44,186,7,0,6,0},
	{168,5,'A','H',"FP1616",168,171,276,230,22,10,20,18,33,192,7,0,6,0},
	{168,5,'B','H',"FP1916",168,171,310,260,22,14,20,20,37,193,7,0,6,0},
	{168,5,'C','H',"FP2116",168,171,335,288,22,16,20,24,44,195,7,0,6,0},
	{168,5,'D','H',"FP2716",168,171,392,346,22,22,20,30,56,210,7,0,6,0},
	{194,6,'A','H',"FP1919",194,197,310,260,22,14,20,20,37,222,8,0,6,0},
	{194,6,'B','H',"FP2119",194,197,335,288,22,16,20,24,44,232,8,0,6,0},
	{194,6,'C','H',"FP2719",194,197,392,346,22,22,20,30,56,254,8,0,6,0},
	{194,6,'D','H',"FP3219",194,197,466,414,26,22,24,34,63,260,8,0,6,0},
	{219,6,'A','H',"FP2121",219,222,335,288,22,16,20,24,44,249,8,0,6,0},
	{219,6,'B','H',"FP2721",219,222,392,346,22,22,20,30,56,270,8,0,6,0},
	{219,6,'C','H',"FP3221",219,222,466,414,26,22,24,34,63,266,8,0,6,0},
	{219,6,'D','H',"FP3521",219,222,500,442,26,24,24,36,67,260,8,0,6,0},
	{273,7,'A','H',"FP2727",273,276,392,346,22,22,20,30,56,308,9,0,6,0},
	{273,7,'B','H',"FP3227",273,276,466,414,26,22,24,34,63,330,9,0,6,0},
	{273,7,'C','H',"FP3527",273,276,500,442,26,24,24,36,67,332,9,0,6,0},
	{273,7,'D','H',"FP3727",273,276,544,474,29,22,27,38,70,320,9,0,6,0},
	{325,8,'A','H',"FP3232",325,328,466,414,26,22,24,34,63,364,10,0,6,0},
	{325,8,'B','H',"FP3532",325,328,500,442,26,24,24,36,67,380,10,0,6,0},
	{325,8,'C','H',"FP3732",325,328,544,474,29,22,27,38,70,380,10,0,6,0},
	{325,8,'D','H',"FP4032",325,328,580,505,29,24,27,38,70,380,10,0,6,0},
	{356,8,'A','H',"FP3535",356,359,500,442,26,24,24,36,67,396,10,0,7,0},
	{356,8,'C','H',"FP4035",356,359,580,505,29,24,27,38,70,396,10,0,7,0},
	{356,8,'B','H',"FP3735",356,359,544,474,29,22,27,38,70,396,10,0,7,0},
	{356,8,'D','H',"FP4235",356,359,612,534,35,20,33,40,74,398,10,0,7,0},
	{377,10,'A','H',"FP3737",377,380,544,474,29,22,27,38,70,423,13,0,8,0},
	{377,10,'B','H',"FP4037",377,380,580,509,29,24,27,38,70,440,13,0,8,0},
	{377,10,'C','H',"FP4237",377,380,612,534,35,20,33,40,70,440,13,0,8,0},
	{377,10,'D','H',"FP4537",377,380,650,570,35,22,33,42,74,440,13,0,8,0},
	{406,10,'A','H',"FP4040",406,409,580,505,29,24,27,38,70,452,13,0,8,0},
	{406,10,'B','H',"FP4240",406,409,612,534,35,20,33,40,74,453,13,0,8,0},
	{406,10,'C','H',"FP4540",406,409,650,570,35,22,33,42,74,460,13,0,8,0},
	{406,10,'D','H',"FP4840",406,409,710,610,38,22,36,46,78,452,13,0,8,0},
	{426,12,'A','H',"FP4242",426,429,612,534,35,20,33,40,74,478,16,0,10,0},
	{426,12,'B','H',"FP4542",426,429,650,570,35,22,33,42,74,494,16,0,10,0},
	{426,12,'C','H',"FL4842",426,429,710,610,38,22,36,46,78,494,16,0,10,0},
	{426,12,'D','H',"FP5042",426,429,730,640,41,20,39,48,82,500,16,0,10,0},
	{457,12,'A','H',"FP4545",457,460,650,570,35,22,33,42,74,508,16,0,10,0},
	{457,12,'B','H',"FP4845",457,460,710,610,38,22,36,48,78,508,16,0,10,0},
	{457,12,'C','H',"FP5045",457,460,730,640,41,20,39,48,82,520,16,0,10,0},
	{457,12,'D','H',"FP5245",457,460,770,664,41,22,39,50,85,510,16,0,10,0},
	{480,14,'A','H',"FP4848",480,483,710,610,41,22,39,50,85,510,16,0,10,0},
	{480,14,'B','H',"FP5048",480,483,730,640,41,20,39,48,82,560,18,0,10,0},
	{480,14,'C','H',"FP5248",480,483,770,664,41,22,39,50,85,550,18,0,10,0},
	{480,14,'D','H',"FP5548",480,483,778,690,41,22,39,50,85,590,18,0,10,0},
	{508,14,'A','H',"FP5050",508,512,730,640,41,20,39,48,82,566,18,0,10,0},
	{508,14,'B','H',"FP5250",508,512,770,664,41,22,39,50,85,570,18,0,10,0},
	{508,14,'C','H',"FP5550",508,512,778,690,41,22,39,50,85,610,18,0,10,0},
	{508,14,'D','H',"FP6150",508,512,846,745,45,24,42,50,85,610,18,0,10,0},
	{529,14,'A','H',"FP5252",529,533,770,664,41,22,39,50,85,588,18,0,10,0},
	{529,14,'B','H',"FP5552",529,533,778,690,41,22,39,50,85,620,18,0,10,0},
	{529,14,'C','H',"FP6152",529,533,846,745,45,24,42,50,85,620,18,0,10,0},
	{529,14,'D','H',"FP6352",529,533,890,776,45,24,42,54,86,630,18,0,10,0},
	{559,14,'A','H',"FP5555",559,563,778,690,41,22,39,50,85,618,18,0,12,0},
	{559,14,'B','H',"FP6155",559,563,846,745,45,24,42,50,85,630,18,0,12,0},
	{559,14,'C','H',"FP6355",559,563,890,776,45,24,42,52,85,640,18,0,12,0},
	{559,14,'D','H',"FP6655",559,563,906,800,45,26,42,56,90,660,18,0,12,0},
	{610,16,'A','H',"FP6161",610,614,846,745,45,24,42,50,85,674,21,0,12,0},
	{610,16,'B','H',"FP6361",610,614,890,776,45,24,42,52,85,690,21,0,12,0},
	{610,16,'C','H',"FP6661",610,614,906,800,45,26,42,52,85,704,21,0,12,0},
	{610,16,'D','H',"FP7161",610,614,1010,880,51,24,48,60,96,700,21,0,12,0},
	{630,16,'A','H',"FP6363",630,634,890,776,45,24,42,52,85,693,21,0,12,0},
	{630,16,'B','H',"FP6663",630,634,908,800,45,26,42,52,85,710,21,0,12,0},
	{630,16,'C','H',"FP7163",630,634,1010,880,51,24,48,60,96,710,21,0,12,0},
	{630,16,'D','H',"FP7663",630,634,1050,926,55,22,52,66,106,734,21,0,12,0},
	{660,16,'A','H',"FP6666",660,664,906,800,45,26,42,52,85,723,21,0,12,0},
	{660,16,'B','H',"FP7166",660,664,1010,880,51,24,48,56,90,724,21,0,12,0},
	{660,16,'C','H',"FP7666",660,664,1050,926,55,22,52,62,99,750,21,0,12,0},
	{660,16,'D','H',"FP8166",660,664,1120,986,55,24,52,68,109,760,21,0,12,0},
	{711,18,'A','H',"FP7171",711,715,1010,880,51,24,48,56,90,780,23,0,12,0},
	{711,18,'B','H',"FP7671",711,715,1050,926,55,22,52,56,90,790,23,0,12,0},
	{711,18,'C','H',"FP8171",711,715,1120,986,55,24,52,64,102,810,23,0,12,0},
	{711,18,'D','H',"FP8671",711,715,1180,1038,55,26,52,70,112,830,23,0,12,0},
	{762,18,'A','H',"FP7676",762,766,1050,926,55,22,52,56,90,831,23,0,12,0},
	{762,18,'B','H',"FP8176",762,766,1120,986,55,24,52,64,102,833,23,0,12,0},
	{762,18,'C','H',"FP8676",762,766,1180,1038,55,26,52,66,106,850,23,0,12,0},
	{762,18,'D','H',"FP9176",762,766,1280,1114,55,28,52,72,115,870,23,0,12,0},
	{813,20,'A','H',"FP8181",813,818,1120,986,55,24,52,64,102,891,26,0,12,0},
	{813,20,'B','H',"FP8681",813,818,1180,1038,55,24,52,64,102,891,26,0,12,0},
	{813,20,'C','H',"FP9183",813,818,1280,1114,55,28,52,70,112,930,26,0,12,0},
	{813,20,'D','H',"FP9681",813,818,1330,1170,55,30,52,76,122,970,26,0,12,0},
	{864,20,'A','H',"FP8686",864,869,1180,1038,55,26,52,64,102,942,26,0,12,0},
	{864,20,'B','H',"FP9186",864,869,1280,1114,55,28,52,70,112,950,26,0,12,0},
	{864,20,'C','H',"FP9686",864,869,1330,1170,55,30,52,74,118,986,26,0,12,0},
	{914,22,'A','H',"FP9191",914,919,1280,1114,55,28,52,70,112,999,29,0,12,0},
	{914,22,'B','H',"FP9691",914,919,1330,1170,55,30,52,74,118,1034,29,0,12,0},
	{965,22,'A','H',"FP9696",965,970,1330,1170,55,30,52,74,118,1051,29,0,12,0},
	/*{219,6,'A','S',"FLP219A",219,222,350,300,26,12,24,20,37,246,8,10,8},
	{219,6,'B','S',"FLP219B",219,222,400,350,26,16,24,20,37,280,8,10,10},
	{219,6,'C','S',"FLP219C",219,205,455,400,26,20,24,20,37,320,8,10,8},
	{219,6,'D','S',"FLP219D",219,222,490,440,29,20,27,22,41,320,8,11,8},	
	{273,6,'A','S',"FLP273A",273,276,400,350,26,16,24,20,37,300,8,10,8},
	{273,6,'B','S',"FLP273B",273,276,455,400,26,20,24,20,37,330,8,10,8},
	{273,6,'C','S',"FLP273C",273,276,490,440,29,20,27,22,41,350,8,11,8},
	{273,6,'D','S',"FLP273D",273,276,525,470,29,22,27,24,44,360,8,12,8},	
	{325,7,'A','S',"FLP325A",325,328,455,400,26,20,24,20,37,356,9,10,10},	
	{325,7,'B','S',"FLP325B",325,328,490,440,29,20,27,22,41,376,9,11,10},	
	{325,7,'C','S',"FLP325C",325,328,525,470,29,22,27,24,44,386,9,12,10},	
	{325,7,'D','S',"FLP325D",325,328,555,500,29,24,27,24,44,406,9,12,10},	
	{356,7,'A','S',"FLP356A",356,359,490,440,29,20,27,22,41,388,9,11,10},
	{356,7,'B','S',"FLP356B",356,359,525,470,29,22,27,24,44,398,9,12,10},	
	{356,7,'C','S',"FLP356C",356,359,555,500,29,24,27,24,44,428,9,12,10},	
	{356,7,'D','S',"FLP356D",356,359,610,550,32,24,30,30,56,460,9,15,10},
	{377,8,'A','S',"FLP377A",377,380,525,470,29,22,27,24,44,410,10,12,10},
	{377,8,'B','S',"FLP377B",377,380,555,500,29,24,27,24,44,430,10,12,10},
	{377,8,'C','S',"FLP377C",377,380,610,550,32,24,30,32,59,440,10,16,10},
	{377,8,'D','S',"FLP377D",377,380,630,565,35,22,33,32,59,440,10,16,10},
	{406,8,'A','S',"FLP406A",406,409,555,500,29,24,27,24,44,439,10,12,10},
	{406,8,'B','S',"FLP406B",406,409,610,550,32,24,30,32,59,460,10,16,10},
	{406,8,'C','S',"FLP406C",406,409,630,565,35,22,33,32,59,460,10,16,10},
	{406,8,'D','S',"FLP406D",406,409,680,605,38,22,36,34,63,460,10,17,10},
	{426,10,'A','S',"FLP426A",426,429,610,550,32,24,30,32,59,469,13,16,10},
	{426,10,'B','S',"FLP426B",426,429,630,565,35,22,33,32,59,480,13,16,10},
	{426,10,'C','S',"FLP426C",426,429,680,605,38,22,36,34,63,500,13,17,10},
	{426,10,'D','S',"FLP426D",426,429,690,620,38,22,36,34,63,520,13,17,10},
	{457,10,'A','S',"FLP457A",457,460,630,565,35,22,33,32,59,499,13,16,12},
	{457,10,'B','S',"FLP457B",457,460,680,605,38,22,36,34,63,510,13,17,12},
	{457,10,'C','S',"FLP457C",457,460,690,620,38,22,36,34,63,520,13,17,12},
	{457,10,'D','S',"FLP457D",457,460,730,660,38,24,36,38,70,530,13,19,12},
	{480,12,'A','S',"FLP480A",480,483,680,605,38,22,36,34,63,529,16,17,12},	
	{480,12,'B','S',"FLP480B",480,483,690,620,38,22,36,34,63,540,13,17,12},	
	{480,12,'C','S',"FLP480C",480,483,730,660,38,24,36,38,70,560,16,19,12},	
	{480,12,'D','S',"FLP480D",480,483,760,680,41,22,39,38,70,570,16,19,12},
	{508,12,'A','S',"FLP508A",508,512,690,620,38,22,36,34,63,556,16,17,12},	
	{508,12,'B','S',"FLP508B",508,512,730,660,38,24,36,38,70,570,16,19,12},	
	{508,12,'C','S',"FLP508C",508,512,760,680,41,22,39,38,70,570,16,19,12},	
	{508,12,'D','S',"FLP508D",508,512,840,760,45,24,42,42,74,620,16,21,12},
	{529,12,'A','S',"FLP529A",529,533,730,660,38,24,36,38,70,581,16,19,12},
	{529,12,'B','S',"FLP529B",529,533,760,680,41,22,39,38,70,580,16,19,12},
	{529,12,'C','S',"FLP529C",529,533,840,760,45,24,42,44,75,620,16,22,12},
	{529,12,'D','S',"FLP529D",529,533,850,765,45,24,42,44,75,620,16,22,12},
	{559,12,'A','S',"FLP559A",559,563,760,680,41,22,39,38,70,608,16,19,12},
	{559,12,'B','S',"FLP559B",559,563,840,760,45,24,42,44,75,630,16,22,12},
	{559,12,'C','S',"FLP559C",559,563,850,765,45,24,42,44,75,630,16,22,12},
	{559,12,'D','S',"FLP559D",559,563,880,795,48,22,45,46,78,640,16,23,12},
	{610,14,'A','S',"FLP610A",610,614,840,760,45,24,42,42,74,666,18,21,12},
	{610,14,'B','S',"FLP610B",610,614,850,765,45,24,42,42,74,666,18,21,12},
	{610,14,'C','S',"FLP610C",610,614,880,795,48,22,45,42,74,690,18,21,12},
	{610,14,'D','S',"FLP610D",610,614,970,870,51,24,48,50,85,710,18,25,12},
	{630,14,'A','S',"FLP630A",630,634,850,765,45,24,42,42,74,682,18,21,12},
	{630,14,'B','S',"FLP630B",630,634,880,795,48,22,45,42,74,700,18,21,12},
	{630,14,'C','S',"FLP630C",630,634,970,870,51,24,48,48,82,720,18,24,12},
	{630,14,'D','S',"FLP630D",630,634,1010,920,51,24,48,64,102,740,18,32,12},
	{660,14,'A','S',"FLP660A",660,664,880,795,48,22,45,42,74,712,18,21,12},
	{660,14,'B','S',"FLP660B",660,664,970,870,51,24,48,48,82,730,18,24,12},
	{660,14,'C','S',"FLP660C",660,664,1010,920,51,24,48,60,96,760,18,30,12},
	{660,14,'D','S',"FLP660D",660,664,1070,975,51,26,48,68,109,770,18,34,12},
	{711,16,'A','S',"FLP711A",711,715,970,870,51,24,48,48,82,774,21,24,12},
	{711,16,'B','S',"FLP711B",711,715,1010,920,51,24,48,50,85,800,21,25,12},
	{711,16,'C','S',"FLP711C",711,715,1070,975,51,26,48,64,102,820,21,32,12},
	{711,16,'D','S',"FLP711D",711,715,1140,1038,59,24,56,72,115,830,21,36,12},
	{762,16,'A','S',"FLP762A",762,766,1010,920,51,24,48,48,82,824,21,24,12},
	{762,16,'B','S',"FLP762B",762,766,1070,975,51,26,48,50,85,850,21,25,12},
	{762,16,'C','S',"FLP762C",762,766,1140,1038,59,24,56,64,102,870,21,32,12},
	{762,16,'D','S',"FLP762D",762,766,1210,1100,59,26,56,76,122,880,21,38,12},
	{813,16,'A','S',"FLP813A",813,818,1070,975,51,26,48,50,85,877,21,25,12},
	{813,16,'B','S',"FLP813B",813,818,1140,1038,59,24,56,60,96,890,21,30,12},
	{813,16,'C','S',"FLP813C",813,818,1210,1100,59,26,56,64,102,930,21,32,12},
	{813,16,'D','S',"FLP813D",813,818,1280,1170,59,28,56,80,128,940,21,40,12},
	{864,20,'A','S',"FLP864A",864,869,1140,1038,59,24,56,60,96,934,26,30,12},
	{864,20,'B','S',"FLP864B",864,869,1210,1100,59,26,56,64,102,960,26,32,12},
	{864,20,'C','S',"FLP864C",864,869,1280,1170,59,28,56,72,115,1000,26,36,12},
	{914,20,'A','S',"FLP914A",914,919,1210,1100,59,26,56,64,102,988,26,32,12},
	{914,20,'B','S',"FLP914B",914,919,1280,1170,59,28,56,66,106,1020,26,33,12},
	{965,20,'A','S',"FLP965A",965,970,1280,1170,59,28,56,66,106,1042,26,33,12},*/
};

//计算刚性法兰或柔性法兰总重
double FL_PARAM2::SumWeight()
{
	return theroy_weight + nRibPlateNum*para.weight;
}
void FL_PARAM2::CopyPropertye(const FL_PARAM2 src_param)
{
	d=src_param.d;
	couple_d=src_param.couple_d;
	cLevelMark=src_param.cLevelMark;
	cMaterial=src_param.cMaterial;
	strcpy(codeName,src_param.codeName);
	fInnerD=src_param.fInnerD;
	fOuterD=src_param.fOuterD;
	fBoltCirD = src_param.fBoltCirD;
	fBoltHoleD = src_param.fBoltHoleD;
	nBoltD=src_param.nBoltD;
	nBoltNum=src_param.nBoltNum;
	fThick=src_param.fThick;
	fH=src_param.fH;
	theroy_weight=src_param.theroy_weight;
	nRibPlateNum=src_param.nRibPlateNum;
	//
	para.weight=src_param.para.weight;
	para.cMat=src_param.para.cMat;
	para.T=src_param.para.T;
	para.A=src_param.para.A;
	para.B=src_param.para.B;
	para.C=src_param.para.C;
	para.D=src_param.para.D;
	para.E=src_param.para.E;
	para.F=src_param.para.F;
	para.G=src_param.para.G;
	para.H=src_param.para.H;
}
int flg_param_N=15;
FL_PARAM2 flg_param_tbl[200] =
{
	{406,406,'C','S',"FG40c",406,555,487,29,16,27,18,9,15.89,16,{0.75,'S',6,215,74,20,20,0,0,0,0}},
	{406,406,'B','S',"FG40b",406,571,496,32,16,30,20,10,19.87,16,{1.20,'S',8,230,83,20,20,0,0,0,0}},
	{406,406,'A','S',"FG40a",406,588,505,35,16,33,22,11,24.53,16,{1.43,'S',8,250,91,20,20,0,0,0,0}},
	{426,426,'C','S',"FG42c",426,585,518,29,20,27,18,9,17.83,20,{0.81,'S',6,215,80,20,20,0,0,0,0}},
	{426,426,'B','S',"FG42b",426,608,525,35,16,33,22,11,25.52,16,{1.43,'S',8,250,91,20,20,0,0,0,0}},
	{426,426,'A','S',"FG42a",426,657,552,44,12,42,30,15,46.27,12,{3.44,'S',12,305,116,20,20,0,0,0,0}},
	{457,457,'C','S',"FG45c",457,606,538,29,20,27,18,9,17.57,20,{0.75,'S',8,215,74,20,20,0,0,0,0}},
	{457,457,'B','S',"FG45b",457,655,565,38,16,36,24,12,32.57,16,{1.68,'S',8,270,99,20,20,0,0,0,0}},
	{457,457,'A','S',"FG45a",457,695,597,41,16,39,26,13,43.59,16,{2.66,'S',10,285,119,20,20,0,0,0,0}},
	{813,965,'C','H',"FG81/96c1",813,1180,1082,41,28,39,26,13,117.24,28,{6.55,'H',16,285,184,20,20,0,0,0,0}},
	{965,813,'C','H',"FG81/96c2",965,1180,1082,41,28,39,26,13,73.92,28,{2.38,'H',10,285,108,20,20,0,0,0,0}},
	{813,965,'B','H',"FG81/96b1",813,1229,1109,50,24,48,32,16,167.59,24,{9.99,'H',18,340,208,20,20,0,0,0,0}},
	{965,813,'B','H',"FG81/96b2",965,1229,1109,50,24,48,32,16,114.27,24,{4.23,'H',12,340,132,20,20,0,0,0,0}},
	{813,965,'A','H',"FG81/96a1",813,1273,1133,58,20,56,42,21,248.47,20,{15.89,'H',22,400,230,20,20,0,0,0,0}},
	{965,813,'A','H',"FG81/96a2",965,1273,1133,58,20,56,42,21,178.49,20,{7.74,'H',16,400,154,20,20,0,0,0,0}},
};

int flr_param_N=13;
FL_PARAM2 flr_param_tbl[200] =
{
	{457,457,'D','H',"FR45d",457,606,538,29,20,27,22,11,21.4,0,{0,'S',0,0,0,0,0,0,0,0,0}},
	{377,377,'D','H',"FR37d",377,526,458,29,16,27,20,10,16.52,0,{0,'S',0,0,0,0,0,0,0,0,0}},
	{377,377,'C','H',"FR37c",377,575,485,38,12,36,28,14,32.54,0,{0,'S',0,0,0,0,0,0,0,0,0}},
	{377,377,'B','H',"FR37b",377,608,503,44,12,42,34,17,47.69,0,{0,'S',0,0,0,0,0,0,0,0,0}},
	{377,377,'C','H',"FX37c",377,575,485,38,12,36,28,14,32.54,4,{1.37,'H',8,220,99,20,20,0,0,0,0}},
	{377,377,'B','H',"FX37b",377,608,203,44,12,42,34,17,47.69,4,{1.85,'H',8,225,115.5,20,20,0,0,0,0}},
	{377,377,'A','H',"FX37a",377,675,555,50,12,48,44,22,84.89,4,{3.39,'H',10,290,149,20,20,0,0,0,0}},
	{377,406,'C','H',"FX37/40c1",377,588,505,35,16,33,30,15,34.33,4,{1.32,'H',8,200,106,20,20,0,0,0,0}},
	{406,377,'C','H',"FX37/40c2",406,588,505,35,16,33,26,13,21.86,4,{1.14,'H',8,200,91,20,20,0,0,0,0}},
	{377,406,'B','H',"FX37/40b1",377,637,532,44,12,42,38,19,43.20,4,{2.60,'H',8,255,130,20,20,0,0,0,0}},
	{406,377,'B','H',"FX37/40b2",406,637,532,44,12,42,32,16,28.99,4,{1.84,'H',8,255,116,20,20,0,0,0,0}},
	{377,406,'A','H',"FX37/40a1",377,654,541,47,12,45,44,22,68.81,4,{2.92,'H',10,255,130,20,20,0,0,0,0}},
	{406,377,'A','H',"FX37/40a2",406,654,541,47,12,45,40,20,47.53,4,{2.63,'H',10,255,116,20,20,0,0,0,0}},
};
//钢管大小头配置规则表
int fl_couple_rule_N=29;
FL_COUPLE_RULE fl_couple_rule_table[200] =
{	//d  A   B   C	 D	
	{159,159,168,180,194},
	{168,168,180,194,203},
	{194,194,203,219,245},
	{219,219,245,273,299},
	{273,273,299,325,356},
	{325,325,356,377,406},
	{356,356,377,406,426},
	{377,377,406,426,457},
	{406,406,426,457,480},
	{426,426,457,480,508},
	{457,457,480,508,529},
	{480,480,508,529,559},
	{508,508,529,559,584},
	{529,529,559,584,610},
	{559,559,584,610,630},
	{610,610,630,660,711},
	{630,630,660,711,762},
	{660,660,711,762,813},
	{711,711,762,813,864},
	{762,762,813,864,914},
	{813,813,864,914,965},
	{864,864,914,965, -1},
	{914,914,965, -1, -1},
	{965,965, -1, -1, -1},
	{180,180,194,203,219},
	{203,203,219,245,273},
	{245,245,273,299,325},
	{299,299,325,356,377},
	{584,584,610,630,660},
	/* 因更换插板库注释掉(2011.10.22版本) wjh-2013.6.5
	{159,159,168,194,219},
	{168,168,194,219,273},
	{194,194,219,273,325},
	{219,219,273,325,356},
	{273,273,325,356,377},
	{325,325,356,377,406},
	{356,356,377,406,426},
	{377,377,406,426,457},
	{406,406,426,457,480},
	{426,426,457,480,508},
	{457,457,480,508,529},
	{480,480,508,529,599},
	{508,508,529,599,610},
	{529,529,599,610,630},
	{599,599,610,630,660},
	{610,610,630,660,711},
	{630,630,660,711,762},
	{660,660,711,762,813},
	{711,711,762,813,864},
	{762,762,813,864,914},
	{813,813,864,914,965},
	{864,864,914,965, -1},
	{914,914,965, -1, -1},
	{965,965, -1, -1, -1},*/
};
int g_nJgJointRecordNum=43;
//主材宽、厚 内包角钢宽、厚 外包钢板宽、厚 外包角钢宽、厚
JG_JOINT_PARA jg_joint_table[200]=
{
	{100, 7, 90, 7, 90, 8,110, 7},
	{100, 8, 90, 8, 90, 8,110, 8},
	{100,10, 90,10, 90,10,110,10},
	{100,12, 90,12, 90,12,110,12},
	{100,14, 90,14, 85,14,110,14},
	{100,16, 90,16, 80,16,110,16},
	{110, 7,100, 7,100, 8,125, 8},
	{110, 8,100, 8,100, 8,125, 8},
	{110,10,100,10,100,10,125,10},
	{110,12,100,12,100,12,125,12},
	{125, 8,110, 8,105, 8,140, 8},
	{125,10,110, 8,105, 8,140,10},
	{125,12,110,10,105,10,140,12},
	{140,10,125, 8,115,10,160,10},
	{140,12,125,10,115,10,160,12},
	{140,14,125,12,115,10,160,14},
	{160,10,140,10,135,10,180,10},
	{160,12,140,10,135,10,180,12},
	{160,14,140,12,135,12,180,14},
	{160,16,140,14,135,12,180,16},
	{180,12,160,10,155,10,200,12},
	{180,14,160,12,155,10,200,14},
	{180,16,160,14,155,12,200,16},
	{180,18,160,14,155,14,200,18},
	{200,14,180,12,180,12,220,14},
	{200,16,180,14,180,12,220,16},
	{200,18,180,14,180,14,220,18},
	{200,20,180,16,180,16,220,20},
	{200,24,180,18,180,18,220,24},
	{220,16,200,14,180,14,250,16},
	{220,18,200,16,180,14,250,18},
	{220,20,200,16,180,16,250,20},
	{220,22,200,18,180,18,250,22},
	{220,24,200,20,180,20,250,24},
	{220,26,200,24,180,20,250,26},
	{250,18,220,16,200,16,250,18},
	{250,20,220,18,200,16,250,20},
	{250,24,220,20,200,20,250,24},
	{250,26,220,22,200,22,250,26},
	{250,28,220,24,200,24,250,28},
	{250,30,220,26,200,24,250,30},
	{250,32,220,26,200,26,250,32},
	{250,35,220,26,200,30,250,35},
};
//查找标准构件槽型插板
INSERT_PLATE_PARAM* FindRollEndParam(double d,double t,char cTubeMat,int iLevel)
{
	for(int i=0;i<rollend_param_N;i++)
	{
		if(rollend_param_table[i].d==d&&rollend_param_table[i].t==t
			&&rollend_param_table[i].cTubeMat==cTubeMat
			&&rollend_param_table[i].iLevelMark==iLevel)
			return &rollend_param_table[i];
	}
	return NULL;
}

//根据代号查找标准构件槽型插板
INSERT_PLATE_PARAM* FindRollEndParam(const char *codeName)
{
	for(int i=0;i<rollend_param_N;i++)
	{
		if(strcmp(rollend_param_table[i].codeName,codeName)==0)
			return &rollend_param_table[i];
	}
	return NULL;
}

//查找标准构件U型插板
INSERT_PLATE_PARAM* FindUEndParam(double d,double t,char cTubeMat,int iLevel)
{
	for(int i=0;i<uend_param_N;i++)
	{
		if(uend_param_table[i].d==d&&uend_param_table[i].t==t
			&&uend_param_table[i].cTubeMat==cTubeMat
			&&uend_param_table[i].iLevelMark==iLevel)
			return &uend_param_table[i];
	}
	return NULL;
}

//根据代号查找标准构件U型插板
INSERT_PLATE_PARAM* FindUEndParam(const char *codeName)
{
	for(int i=0;i<uend_param_N;i++)
	{
		if(strcmp(uend_param_table[i].codeName,codeName)==0)
			return &uend_param_table[i];
	}
	return NULL;
}

//查找标准构件十字型插板
INSERT_PLATE_PARAM* FindXEndParam(double d,double t,char cTubeMat,int iLevel)
{
	for(int i=0;i<xend_param_N;i++)
	{
		if(xend_param_table[i].d==d&&xend_param_table[i].t==t
			&&xend_param_table[i].cTubeMat==cTubeMat
			&&xend_param_table[i].iLevelMark==iLevel)
			return &xend_param_table[i];
	}
	return NULL;
}

//根据代号或基板编号查找标准构件十字型插板 
INSERT_PLATE_PARAM* FindXEndParam(const char *codeName)
{
	for(int i=0;i<xend_param_N;i++)
	{
		if(strcmp(xend_param_table[i].codeName,codeName)==0
			||strcmp(xend_param_table[i].datumPlatePartNo,codeName)==0) //wht 11-05-11
			return &xend_param_table[i];
	}
	return NULL;
}

//查找标准构件平焊法兰
FL_PARAM* FindFlPParam(double d,double t,char cMaterial,char cMark)
{
	for(int i=0;i<flp_param_N;i++)
	{
		if(flp_param_table[i].d==d&&flp_param_table[i].t==t&&flp_param_table[i].cMaterial==cMaterial&&flp_param_table[i].cLevelMark==cMark)
		{	//为了能够统一处理对焊法兰与平焊法兰 将对焊法兰主管壁厚以及S存储到cur_t，cur_S中 wht 10-01-20
			//flp_param_table[i].cur_t=t;
			//flp_param_table[i].cur_S=flp_param_table[i].S;
			return &flp_param_table[i];
		}
	}
	return NULL;
}

//查找标准构件平焊法兰，忽略材质
FL_PARAM* FindFlPParam(double d,double t,char cMark)
{
	for(int i=0;i<flp_param_N;i++)
	{
		if(flp_param_table[i].d==d&&flp_param_table[i].t==t&&flp_param_table[i].cLevelMark==cMark)
		{	//为了能够统一处理对焊法兰与平焊法兰 将对焊法兰主管壁厚以及S存储到cur_t，cur_S中 wht 10-01-20
			//flp_param_table[i].cur_t=t;
			//flp_param_table[i].cur_S=flp_param_table[i].S;
			return &flp_param_table[i];
		}
	}
	return NULL;
}

//根据代号查找标准构件平焊法兰
FL_PARAM* FindFlPParam(const char *codeName)
{
	for(int i=0;i<flp_param_N;i++)
	{
		if(strcmp(flp_param_table[i].codeName,codeName)==0)
			return &flp_param_table[i];
	}
	return NULL;
}
//查找标准构件对焊法兰
FL_PARAM* FindFlDParam(double d,double couple_d,char cMaterial,char cMark)
{
	for(int i=0;i<fld_param_N;i++)
	{
		if(fabs(fld_param_table[i].d-d)<EPS&&fabs(fld_param_table[i].couple_d-couple_d)<EPS
			&&toupper(fld_param_table[i].cLevelMark)==toupper(cMark)
			&&toupper(fld_param_table[i].cMaterial)==toupper(cMaterial))
			return &fld_param_table[i];
	}
	return NULL;
}

//查找标准构件对焊法兰,忽略材质
FL_PARAM* FindFlDParam(double d,double couple_d,char cMark)
{
	for(int i=0;i<fld_param_N;i++)
	{
		if(fabs(fld_param_table[i].d-d)<EPS&&fabs(fld_param_table[i].couple_d-couple_d)<EPS&&
			toupper(fld_param_table[i].cLevelMark)==toupper(cMark))
			return &fld_param_table[i];
	}
	return NULL;
}

//根据代号查找标准构件对焊法兰
FL_PARAM* FindFlDParam(const char *codeName)
{
	for(int i=0;i<fld_param_N;i++)
	{
		if(fld_param_table[i].IsSameCode(codeName))
			return &fld_param_table[i];
	}
	return NULL;
}

//根据主管直径、对接钢管直径、法兰级别和材质查找刚性法兰
FL_PARAM2* FindFlGParam(double d,double couple_d,char cMat,char cMark)
{
	for(int i=0;i<flg_param_N;i++)
	{
		if(fabs(flg_param_tbl[i].d-d)<EPS&&fabs(flg_param_tbl[i].couple_d-couple_d)<EPS&&
			toupper(flg_param_tbl[i].cMaterial)==toupper(cMat)&&
			toupper(flg_param_tbl[i].cLevelMark)==toupper(cMark))
			return &flg_param_tbl[i];
	}
	return NULL;
}

//根据主管直径、对接钢管直径和法兰级别查找刚性法兰
FL_PARAM2* FindFlGParam(double d,double couple_d,char cMark)
{
	for(int i=0;i<flg_param_N;i++)
	{
		if(fabs(flg_param_tbl[i].d-d)<EPS&&fabs(flg_param_tbl[i].couple_d-couple_d)<EPS&&
			toupper(flg_param_tbl[i].cLevelMark)==toupper(cMark))
			return &flg_param_tbl[i];
	}
	return NULL;
}


//根据代号查刚性法兰
FL_PARAM2* FindFlGParam(const char *codeName)
{
	for(int i=0;i<flg_param_N;i++)
	{
		if(stricmp(flg_param_tbl[i].codeName,codeName)==0)
			return &flg_param_tbl[i];
	}
	return NULL;
}

//根据主管直径、对接钢管直径、法兰级别和材质查找柔性法兰
FL_PARAM2* FindFlRParam(double d,double couple_d,char cMat,char cMark)
{
	for(int i=0;i<flr_param_N;i++)
	{
		if(fabs(flr_param_tbl[i].d-d)<EPS&&fabs(flr_param_tbl[i].couple_d-couple_d)<EPS&&
			toupper(flr_param_tbl[i].cMaterial)==toupper(cMat)&&
			toupper(flr_param_tbl[i].cLevelMark)==toupper(cMark))
			return &flr_param_tbl[i];
	}
	return NULL;
}

//根据主管直径、对接钢管直径和法兰级别查找柔性法兰
FL_PARAM2* FindFlRParam(double d,double couple_d,char cMark)
{
	for(int i=0;i<flr_param_N;i++)
	{
		if(fabs(flr_param_tbl[i].d-d)<EPS&&fabs(flr_param_tbl[i].couple_d-couple_d)<EPS&&
			toupper(flr_param_tbl[i].cLevelMark)==toupper(cMark))
			return &flr_param_tbl[i];
	}
	return NULL;
}

//根据代号查柔性法兰
FL_PARAM2* FindFlRParam(const char *codeName)
{
	for(int i=0;i<flr_param_N;i++)
	{
		if(stricmp(flr_param_tbl[i].codeName,codeName)==0)
			return &flr_param_tbl[i];
	}
	return NULL;
}

//查询钢管法兰大小头配置规则
int QueryFlCouple(double fTubeD,double fOtherTubeD,char cFlType,FL_COUPLE* couple)
{
	double main_d	=max(fTubeD,fOtherTubeD);
	double couple_d	=min(fTubeD,fOtherTubeD);
	CXhChar16 fl_name,couple_fl_name;
	if(couple)	 
	{
		couple->main_d=fTubeD;
		couple->couple_d=fOtherTubeD;
	}
	if(toupper(cFlType)=='P')	//平焊法兰
	{
		for(int i=0;i<flp_param_N;i++)
		{	//在同一条记录里同是出现两钢管直径 说明两钢管可以对接
			if(flp_param_table[i].d==main_d&&flp_param_table[i].couple_d==main_d)
				fl_name.Copy(flp_param_table[i].codeName);
			if(flp_param_table[i].d==couple_d&&flp_param_table[i].couple_d==main_d)
				couple_fl_name.Copy(flp_param_table[i].codeName);
			if(fl_name.GetLength()>0&&couple_fl_name.GetLength()>0)
				break;
		}
	}
	else if(toupper(cFlType)=='D')
	{
		for(int i=0;i<fld_param_N;i++)
		{	//在同一条记录里同是出现两钢管直径 说明两钢管可以对接
			if(fld_param_table[i].d==main_d&&fld_param_table[i].couple_d==main_d)
				fl_name.Copy(fld_param_table[i].codeName);
			if(fld_param_table[i].d==couple_d&&fld_param_table[i].couple_d==main_d)
				couple_fl_name.Copy(fld_param_table[i].codeName);
			if(fl_name.GetLength()>0&&couple_fl_name.GetLength()>0)
				break;
		}
	}
	else if(toupper(cFlType)=='G')
	{
		for(int i=0;i<flg_param_N;i++)
		{	//在同一条记录里同是出现两钢管直径 说明两钢管可以对接
			if(flg_param_tbl[i].d==main_d&&flg_param_tbl[i].couple_d==main_d)
				fl_name.Copy(flg_param_tbl[i].codeName);
			if(flg_param_tbl[i].d==couple_d&&flg_param_tbl[i].couple_d==main_d)
				couple_fl_name.Copy(flg_param_tbl[i].codeName);
			if(fl_name.GetLength()>0&&couple_fl_name.GetLength()>0)
				break;
		}
	}
	else if(toupper(cFlType)=='R')
	{
		for(int i=0;i<flr_param_N;i++)
		{	//在同一条记录里同是出现两钢管直径 说明两钢管可以对接
			if(flr_param_tbl[i].d==main_d&&flr_param_tbl[i].couple_d==main_d)
				fl_name.Copy(flr_param_tbl[i].codeName);
			if(flr_param_tbl[i].d==couple_d&&flr_param_tbl[i].couple_d==main_d)
				couple_fl_name.Copy(flr_param_tbl[i].codeName);
			if(fl_name.GetLength()>0&&couple_fl_name.GetLength()>0)
				break;
		}
	}
	if(fabs(fOtherTubeD-main_d)<EPS)
	{	//主管为小头时，掉换回对应的法兰编号
		CXhChar16 tempflname=fl_name;
		fl_name=couple_fl_name;
		couple_fl_name=tempflname;
	}
	if(fl_name.GetLength()>0&&couple)
	{
		strcpy(couple->mainFlName,fl_name);
		strcpy(couple->coupleFlName,couple_fl_name);
	}
	if(fl_name.GetLength()>0&&couple_fl_name.GetLength()>0)
		return 2;	//主管与连接管上安装的配对法兰全部找到
	else if(fl_name.GetLength()>0)
		return 1;	//主管上安装的配对法兰找到，但连接管上的配对法兰未找到
	else
		return 0;
}
//查询法兰等级标志
char QueryFlLevelMark(FL_COUPLE_RULE* pFlCoupleRule,double fTubeD)
{
	if(fabs(pFlCoupleRule->A-fTubeD)<eps2)
		return 'A';
	else if(fabs(pFlCoupleRule->B-fTubeD)<eps2)
		return 'B';
	else if(fabs(pFlCoupleRule->C-fTubeD)<eps2)
		return 'C';
	else if(fabs(pFlCoupleRule->D-fTubeD)<eps2)
		return 'D';
	else 
		return ' ';
}
//根据主管规格以及法兰对接级别查找对接钢管直径
double QueryCoupleTubeD(double fMainTubeD,char cMark)
{
	FL_COUPLE_RULE* pFlCoupleRule=NULL;
	for(int i=0;i<fl_couple_rule_N;i++)
	{	
		if(fl_couple_rule_table[i].d==fMainTubeD)
		{
			pFlCoupleRule=&fl_couple_rule_table[i];
			break;
		}
	}
	if(pFlCoupleRule==NULL)
		return 0;
	if(cMark=='A')
		return pFlCoupleRule->A;
	else if(cMark=='B')
		return pFlCoupleRule->B;
	else if(cMark=='C')
		return pFlCoupleRule->C;
	else if(cMark=='D')
		return pFlCoupleRule->D;
	return 0;
}

int QueryDiBanParaByLsD(int d,char cItem,BOOL is_weld)
{
	DIJIAOBAN_STRU *arr,*pProperItem=NULL;
	if(is_weld)
		arr=diban_weld_table;
	else
		arr=diban_lslj_table;
	int diameter;
	for(int i=0;i<12;i++)
	{
		diameter=atoi(&arr[i].guige[1]);
		if(d<=diameter)
		{
			pProperItem = &arr[i];
			break;
		}
	}
	if(pProperItem==NULL)
		pProperItem=&arr[11];
	switch(toupper(cItem))
	{
	case 'G':
		return atoi(&pProperItem->guige[1]);
	case 'D':
		return pProperItem->D;
	case 'S':
		return pProperItem->S;
	case 'L':
		return pProperItem->L;
	case 'A':
		return pProperItem->A;
	case 'B':
		return pProperItem->B;
	case 'C':
		return pProperItem->C;
	case 'E':
		return pProperItem->E;
	case 'T':
		return pProperItem->Thick;
	default:
		return atoi(&pProperItem->guige[1]);
	}
}
int QueryDiBanParaByWide(int wide,char cItem,BOOL is_weld)
{
	DIJIAOBAN_STRU *arr,*pProperItem=NULL;
	if(is_weld)
		arr=diban_weld_table;
	else
		arr=diban_lslj_table;
	for(int i=0;i<12;i++)
	{
		if(i==0)
		{
			if(2*(arr[i].L+arr[i].S)>=wide)
			{
				pProperItem = &arr[i];
				break;
			}
		}
		else if(2*(arr[i].L+arr[i].S)>=wide&&2*(arr[i-1].L+arr[i-1].S)<wide)
		{
			pProperItem = &arr[i];
			break;
		}
	}
	if(pProperItem==NULL)
		pProperItem=&arr[11];
	switch(toupper(cItem))
	{
	case 'G':
		return atoi(&pProperItem->guige[1]);
	case 'D':
		return pProperItem->D;
	case 'S':
		return pProperItem->S;
	case 'L':
		return pProperItem->L;
	case 'A':
		return pProperItem->A;
	case 'B':
		return pProperItem->B;
	case 'C':
		return pProperItem->C;
	case 'E':
		return pProperItem->E;
	case 'T':
		return pProperItem->Thick;
	default:
		return atoi(&pProperItem->guige[1]);
	}
}
static int ToInteger(double fVal)
{
	if(fVal>0)
		return (int)(fVal+0.5);
	else
		return (int)(fVal-0.5);
}
BOOL GetLsSpace(LSSPACE_STRU& LsSpace, long diameter)
{
	int i;
	for(i=0;i<lsspace_N;i++)
	{
		if(LSSPACE_TABLE[i].M_Diameter == diameter)
		{
			LsSpace = LSSPACE_TABLE[i];
			/*LsSpace.M_Diameter = diameter;
			LsSpace.SingleRowSpace = LSSPACE_TABLE[i][1];
			LsSpace.doubleRowSpace = LSSPACE_TABLE[i][2];
			LsSpace.EndSpace       = LSSPACE_TABLE[i][3];
			LsSpace.PlateEdgeSpace = LSSPACE_TABLE[i][4];
			LsSpace.RollingEdgeSpace=LSSPACE_TABLE[i][5];
			LsSpace.CutAngleSpace  = LSSPACE_TABLE[i][6];
			LsSpace.InnerFLSpace   = LSSPACE_TABLE[i][7];
			LsSpace.OutterFLSpace  = LSSPACE_TABLE[i][8];
			LsSpace.Aperture	   = LS_APERTURE[i];*/
			return TRUE;
		}
	}
	//计算默认值
	LsSpace.EndSpace=ToInteger(1.5*diameter);
	int odd=LsSpace.EndSpace%5;
	if(odd>0)
		LsSpace.EndSpace+=5-odd;
	LsSpace.PlateEdgeSpace=LsSpace.RollingEdgeSpace=LsSpace.CutAngleSpace=LsSpace.EndSpace;
	LsSpace.SingleRowSpace=LsSpace.EndSpace*2;
	LsSpace.doubleRowSpace=ToInteger(LsSpace.SingleRowSpace*1.6);
	odd=LsSpace.doubleRowSpace%20;
	if(odd>0)
		LsSpace.doubleRowSpace+=20-odd;
	return FALSE;
}
//	 _____     |  o o  |
//	|_o_o_|	or |_o_o_o_|	——>总长
int GetLsGroupLen(int M_Diameter, int nLsN, int nRowsN)//获得螺栓组长度
{
	int Len = 0;
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, M_Diameter);
	if(nRowsN==1)//单排螺栓
	{
		Len = 2*LsSpace.EndSpace+(nLsN-1)*LsSpace.SingleRowSpace;
	}
	else	//双排螺栓
		Len = 2*LsSpace.EndSpace +(nLsN-1)*LsSpace.doubleRowSpace/2;
	return Len;
}
double FindJg_r(double wing_wide)
{
	int i;
	for(i=0;i<jgguige_N;i++)
	{
		if(fabs(jgguige_table[i].wing_wide-wing_wide)<0.5)
			return jgguige_table[i].r;
	}
	return 0;
}
BYTE JGZJ::MatchStyleG(double gDist,bool bPreferMultiBoltG/*=true*/)
{
	if(!bPreferMultiBoltG&&gDist==g&&gDist>0)
		return 0;
	else if(fabs(gDist-g1)<EPS&&gDist>0)
		return 1;
	else if(fabs(gDist-g2)<EPS&&gDist>0)
		return 2;
	else if(fabs(gDist-g3)<EPS&&gDist>0)
		return 3;
	else if(fabs(gDist-g)<EPS)
		return 0;
	else
		return 4;
}
long JGZJ::ByStyleG(int indexOfStyleG,double gDefault/*=0*/)
{
	if(indexOfStyleG==0)
		return g;
	else if(indexOfStyleG==1)
		return g1;
	else if(indexOfStyleG==2)
		return g2;
	else if(indexOfStyleG==3)
		return g3;
	else if(gDefault<0)
		return (int)(gDefault-0.5);
	else //if(gDefault>0)
		return (int)(gDefault+0.5);
}
BOOL getjgzj(JGZJ& jgzj,double wing_wide)
{
	int i,index;
	for(i=0;i<zhunju_N;i++)
	{
		if(fabs(zhunju_table[i].wide-wing_wide)<0.5)
		{
			index = i;
			break;
		}
	}
	if(i==zhunju_N)
	{
		jgzj.g=ToInteger(wing_wide*0.5);
		jgzj.g1=jgzj.g2=jgzj.g3=0;
		return FALSE;
	}
	jgzj.g = zhunju_table[index].g;
	jgzj.g1 = zhunju_table[index].g1;
	jgzj.g2 = zhunju_table[index].g2;
	jgzj.g3 = zhunju_table[index].g3;
	return TRUE;
}
char QuerySteelBriefMatMark(char *steelmark)
{
	int i;
	for(i=0;i<steelmat_N;i++)
	{
		if(stricmp(steelmat_tbl[i].mark,steelmark)==0)
			return steelmat_tbl[i].cBriefMark;
	}
	return ' ';
}
STEELMAT* QuerySteelMatByBriefMark(char briefmark)
{
	int i;
	for(i=0;i<steelmat_N;i++)
	{
		if(toupper(steelmat_tbl[i].cBriefMark)==briefmark)
			return &steelmat_tbl[i];
	}
	return NULL;
}

BOOL QuerySteelMatMark(char briefmark,char* steelmark)
{
	int i;
	for(i=0;i<steelmat_N;i++)
	{
		if(toupper(steelmat_tbl[i].cBriefMark)==toupper(briefmark))
		{
			if(steelmark)
				strcpy(steelmark,steelmat_tbl[i].mark);
			return TRUE;
		}
	}
	return FALSE;
}
int QuerySteelMatIndex(char briefmark)
{
	int i;
	for(i=0;i<steelmat_N;i++)
	{
		if(toupper(steelmat_tbl[i].cBriefMark)==toupper(briefmark))
			return i;
	}
	return -1;
}
//以米为长度单位
/*PARTLIB_API double GetJgUnitLenWei(double wing_wide,double wing_thick)
{
	double wei=0;
	int i;

	for(i=0;i<jgguige_N;i++)
	{
		if(fabs(jgguige_table[i].wing_wide-wing_wide)<EPS&&fabs(jgguige_table[i].wing_thick-wing_thick)<EPS)
		{
			wei = jgguige_table[i].theroy_weight;
			break;
		}
	}
	return wei;
}
//以米为长度单位	得到钢管单位长度重量
PARTLIB_API double GetTubeUnitLenWei(double tube_d,double tube_t)
{
	double wei=0;
	int i;

	for(i=0;i<tubeguige_N;i++)
	{
		if(fabs(tubeguige_table[i].D-tube_d)<EPS&&fabs(tubeguige_table[i].thick-tube_t)<EPS)
		{
			wei = tubeguige_table[i].theroy_weight;
			break;
		}
	}
	if(wei==0)
	{
		double RO=tube_d*0.5;
		double RI=RO-tube_t;
		double area=Pi*(RO*RO-RI*RI);
		wei=area*7.85e-3;
	}
	return wei;
}*/
//查询螺帽直径及螺帽高
struct LSDH_STRU{
	long   d;		//螺栓名义直径
	double S;		//六角螺母对边距离（可由D反算出)
	double D;		//螺栓帽直径(六角螺母对角顶点距离）
	double H,H2;	//螺栓头及螺帽的单帽高度
};
LSDH_STRU LsD_H_TBL[]={
	12, 18, 20.8, 7.5,12.2,
	16, 24, 27.7,10.0,15.9,
	20, 30, 34.6,12.5,19.0,
	24, 36, 41.6,15.0,22.3,
	27, 41, 47.3,17.0,24.7,
	30, 46, 53.1,18.7,26.4,
	33, 50, 57.7,21.0,29.5,
	36, 55, 63.5,22.5,31.9,
	39, 60, 69.3,25.0,34.3,
	42, 65, 75.1,26.0,34.9,
	45, 70, 80.8,28.0,36.9,
	48, 75, 86.6,30.0,38.9,
	52, 80, 92.4,33.0,42.9,
	56, 85, 98.1,35.0,45.9,
	60, 90,103.9,38.0,48.9,
	64, 95,109.7,40.0,52.4,
	68,100,115.5,43.0,55.5,
	72,105,121.2,45.0,58.5,
};
PARTLIB_API void FindLsD_H(int d, double *D,double *H,double *H2)
{
	int n=sizeof(LsD_H_TBL)/sizeof(LSDH_STRU);
	for(int i=0;i<n;i++)
	{
		if(LsD_H_TBL[i].d==d)
		{
			*D = LsD_H_TBL[i].D;
			*H = LsD_H_TBL[i].H;
			*H2= LsD_H_TBL[i].H2;
			return;
		}
	}
	//未找到此种类型螺栓,给一经验数值
	*D =1.732*d;
	*H =0.62*d;
	*H2=0.82*d;
}
PARTLIB_API void FindBoltCapD_H(int d, double *De,double *H,int idBoltSubPart/*=0*/)
{	//查询螺栓的直径及螺帽高
	CLsSubPartFamily* pSubPartFamily=(idBoltSubPart==0)?NULL:CLsLibrary::FromLsSubPartHandle(idBoltSubPart);
	int count=pSubPartFamily->GetCount();	//脚钉
	if(pSubPartFamily==NULL||count==0)
	{
		double H2;
		FindLsD_H(d,De,H,&H2);
		return;
	}
	LS_SUB_PART* pSubPart=pSubPartFamily->FindLsSubPart(d);
	if(pSubPart)
	{
		*De=pSubPart->dfShapeDe;
		*H =pSubPart->thick;
	}
	else
	{	//未找到此种类型螺栓,给一经验数值
		*De =1.732*d;
		*H  =0.82*d;
	}
}

//以下函数目前已废弃不用,以后确认无用后可删除 wjh-2011.11.24
/*
LS_XING_HAO *FindLsGuiGe(int d, double L0,double grade)
{
	//忽略级别查找螺栓规格
	for(int i=0;i<nLsXingHaoCount;i++)
	{
		if(grade>0&&grade!=lsguige_table[i].grade)
			continue;
		if(lsguige_table[i].d==d&&L0<=lsguige_table[i].up_lim&&L0>=lsguige_table[i].down_lim)
			return &lsguige_table[i];
	}
	return NULL;
}

LS_XING_HAO *FindLsGuiGeByValidLen(long d, double valid_len,double grade)
{
	//忽略级别查找螺栓规格
	for(int i=0;i<nLsXingHaoCount;i++)
	{
		if(grade>0&&grade!=lsguige_table[i].grade)
			continue;
		if(lsguige_table[i].d==d&&valid_len==lsguige_table[i].valid_len)
			return &lsguige_table[i];
	}
	return NULL;
}
//查找指定直径螺栓的最小通厚以及最大通厚	
void FindLsMinL0AndMaxL0(int ls_d, int *min_L0, int *max_L0)
{
	for(int i=0;i<nLsXingHaoCount;i++)
	{
		if(lsguige_table[i].d==ls_d)
		{
			if(lsguige_table[i].down_lim==0&&lsguige_table[i].up_lim==0)
				continue;	//通厚范围为0~0的螺栓为脚钉 跳过
			if(lsguige_table[i].down_lim<*min_L0)
				*min_L0=lsguige_table[i].down_lim;
			if(lsguige_table[i].up_lim>*max_L0)
				*max_L0=lsguige_table[i].up_lim;
		}
	}
}*/

//输入体积单位为mm3 输出重量单位为公斤(Kg)
PARTLIB_API double CalSteelPlankWei(double volume)
{
	return volume*7.85e-6;
}
//查找垫圈重量
PARTLIB_API double FindPadWei(int thick, int d)
{
	if(thick==2&&d==12)			//M12
		return 0.004685;
	else if(thick==3&&d==16)	//M16
		return 0.01065;
	else if(thick==3&&d==20)	//M20
		return 0.01637;
	else if(thick==4&&d==20)	//M20
		return 0.02183;
	else if(thick==4&&d==24)	//M24
		return 0.03108;
	else
	{
		int EndSpace;
		if(d==12)
			EndSpace = 20;
		else if(d==16)
			EndSpace = 25;
		else if(d==20)
			EndSpace = 30;
		else// if(d==24)
			EndSpace = 40;
		return CalSteelPlankWei(4*EndSpace*EndSpace*thick);
	}
}
BIAN_PARA_TYPE* FindBianType(double wide,double thick)
{
	for(int i=0;i<bianguige_N;i++)
	{
		if(bianguige_table[i].wide==wide&&bianguige_table[i].thick==thick)
			return &bianguige_table[i];
	}
	return NULL;
}

JG_PARA_TYPE* FindJgType(double wide,double thick,char cType/*='L'*/)
{
	for(int i=0;i<jgguige_N;i++)
	{
		if(fabs(jgguige_table[i].wing_wide-wide)<EPS&&fabs(jgguige_table[i].wing_thick-thick)<EPS
			&&toupper(jgguige_table[i].cType)==toupper(cType))
			return &jgguige_table[i];
	}
	return NULL;
}

TUBE_PARA_TYPE* FindTubeType(double D,double thick)
{
	for(int i=0;i<tubeguige_N;i++)
	{
		if(fabs(tubeguige_table[i].D-D)<EPS&&fabs(tubeguige_table[i].thick-thick)<EPS)
			return &tubeguige_table[i];
	}
	return NULL;
}

//根据规格查找槽钢
SLOT_PARA_TYPE* FindSlotType(char *guige)
{
	for(int i=0;i<slotguige_N;i++)
	{
		if(strcmp(slotguige_table[i].guige,guige)==0)
			return &slotguige_table[i];
	}
	return NULL;
}
//查找宽翼缘H型钢参数
HSHAPE_PARA_TYPE* FindHWType(double wide,double height)
{
	for(int i=0;i<hw_guige_N;i++)
	{
		if(fabs(hw_guige_table[i].b=wide)<EPS && fabs(hw_guige_table->h-height)<EPS)
			return &hw_guige_table[i];
	}
	return NULL;
}
//查找中翼缘H型钢参数
HSHAPE_PARA_TYPE* FindHMType(double wide,double height)
{
	for(int i=0;i<hm_guige_N;i++)
	{
		if(fabs(hm_guige_table[i].b=wide)<EPS && fabs(hm_guige_table->h-height)<EPS)
			return &hm_guige_table[i];
	}
	return NULL;
}
//查找窄翼缘H型钢参数
HSHAPE_PARA_TYPE* FindHNType(double wide,double height)
{
	for(int i=0;i<hn_guige_N;i++)
	{
		if(fabs(hn_guige_table[i].b=wide)<EPS && fabs(hn_guige_table->h-height)<EPS)
			return &hn_guige_table[i];
	}
	return NULL;
}
//查找等翼缘H型钢参数
HSHAPE_PARA_TYPE* FindHPType(double wide,double height)
{
	for(int i=0;i<hp_guige_N;i++)
	{
		if(fabs(hp_guige_table[i].b=wide)<EPS && fabs(hp_guige_table->h-height)<EPS)
			return &hp_guige_table[i];
	}
	return NULL;
}
FITTING_EB* FindEBFitting(char* sSpec)
{
	for(int i=0;i<eb_guige_N;i++)
	{
		if(stricmp(gxarrEBLib[i].szSizeCode,sSpec)==0)
			return &gxarrEBLib[i];
	}
	return NULL;
}
FITTING_GD* FindGDFitting(char* sSpec)
{
	for(int i=0;i<gd_guige_N;i++)
	{
		if(stricmp(gxarrGDLib[i].szSizeCode,sSpec)==0)
			return &gxarrGDLib[i];
	}
	return NULL;
}
//查找角钢接头参数
JG_JOINT_PARA *FindJgJointPara(double main_w,double main_t)
{
	for(int i=0;i<g_nJgJointRecordNum;i++)
	{
		if(fabs(main_w-jg_joint_table[i].fBaseJgWidth)<EPS
			&&fabs(main_t-jg_joint_table[i].fBaseJgThick)<EPS)
			return &jg_joint_table[i];
	}
	return NULL;
}
//查找内包角钢规格
BOOL FindInnerAngleGuiGe(double main_w,double main_t,double &inner_w,double &inner_t)
{
	JG_JOINT_PARA *pJgJoinPara=FindJgJointPara(main_w,main_t);
	if(pJgJoinPara)
	{
		inner_w=pJgJoinPara->fInnerJgWidth;
		inner_t=pJgJoinPara->fInnerJgThick;
	}
	else 
		return FALSE;
	return TRUE;
}
//查找外包角钢规格
BOOL FindOuterAngleGuiGe(double main_w,double main_t,double &outer_w,double &outer_t)
{
	JG_JOINT_PARA *pJgJoinPara=FindJgJointPara(main_w,main_t);
	if(pJgJoinPara)
	{
		outer_w=pJgJoinPara->fOuterJgWidth;
		outer_t=pJgJoinPara->fOuterJgThick;
	}
	else 
		return FALSE;
	return TRUE;
}
//查找内包钢板规格
BOOL FindOuterPlateGuiGe(double main_w,double main_t,double &plate_w,double &plate_t)
{
	JG_JOINT_PARA *pJgJoinPara=FindJgJointPara(main_w,main_t);
	if(pJgJoinPara)
	{
		plate_w=pJgJoinPara->fOuterPlateWidth;
		plate_t=pJgJoinPara->fOuterPlateThick;
	}
	else 
		return FALSE;
	return TRUE;
}
