#include "stdafx.h"
#include "ModData.h"
#include "f_alg_fun.h"
#include "CreateFace.h"
#include "stdlib.h"

///////////////////////////////////////////////////////////////////////////////
static 	DWORD CFG_NO[32]={
		0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,
		0X00000100,0X00000200,0X00000400,0X00000800,0X00001000,0X00002000,0X00004000,0X00008000,
		0X00010000,0X00020000,0X00040000,0X00080000,0X00100000,0X00200000,0X00400000,0X00800000,
		0X01000000,0X02000000,0X04000000,0X08000000,0X10000000,0X20000000,0X40000000,0X80000000};
static DWORD _LocalGetSingleWord(long iNo)
{
	if(iNo<=0)
		return 0;
	else if(iNo<=32)
		return CFG_NO[iNo-1];
	else
		return 0;
}
BYTE SetModCfgwordSchema(BYTE cbMultiLegSchema)
{
	return MOD_CFGWORD::SetSchema(cbMultiLegSchema);
}
BYTE MOD_CFGWORD::MULTILEG_SCHEMA = 1;	//CFGLEG::MULTILEG_MAX08=1
static BYTE ValidateSchema(BYTE schema)
{
	if(schema==0)
		schema=MOD_CFGWORD::MULTILEG_SCHEMA;
	if(schema>=1&&schema<=3)
		return schema;
	else
		return 1;
}
BYTE MOD_CFGWORD::SetSchema(BYTE cbMultiLegSchema)
{
	return MULTILEG_SCHEMA = ValidateSchema(cbMultiLegSchema);
}
BYTE MOD_CFGWORD::MaxLegs(BYTE schema/*=MULTILEG_SCHEMA*/)	//ָ��ģʽ֧�������߽�����MULTILEG_DEFAULT=0
{
	switch(schema)
	{
	case 1://MULTILEG_MAX08:
		return 8;
	case 2://MULTILEG_MAX16:
		return 16;
	case 3://ZMULTILEG_MAX24:
		return 24;
	default:
		return 8;
	}
}
BYTE MOD_CFGWORD::MaxBodys(BYTE schema/*=MULTILEG_SCHEMA*/)	//ָ��ģʽ֧�������߱�����MULTILEG_DEFAULT=0
{
	switch(schema)
	{
	case 1://MULTILEG_MAX08:
		return 24;
	case 2://MULTILEG_MAX16:
		return 12;
	case 3://MULTILEG_MAX24:
		return 8;
	default:
		return 24;
	}
}
MOD_CFGWORD::MOD_CFGWORD(int iBodyNo,int iLegNo,BYTE schema/*==MULTILEG_SCHEMA*/)
{
	iBodyNo=min(MaxBodys(schema),iBodyNo);
	iLegNo=min(MaxLegs(schema),iLegNo);
	iBodyNo=max(iBodyNo,1);
	iLegNo=max(iLegNo,1);
	int offset=(iBodyNo-1)*MaxLegs(schema)+iLegNo-1;
	int indexOfBytes=offset/8;
	int odd=offset%8;
	memset(flag.bytes,0,24);
	flag.bytes[indexOfBytes]=0x01;
	flag.bytes[indexOfBytes]<<=odd;
}
BOOL MOD_CFGWORD::And(MOD_CFGWORD wcfg) const
{
	return (flag.word[0]&wcfg.flag.word[0])||(flag.word[1]&wcfg.flag.word[1])||(flag.word[2]&wcfg.flag.word[2])||
		(flag.word[3]&wcfg.flag.word[3])||(flag.word[4]&wcfg.flag.word[4])||(flag.word[5]&wcfg.flag.word[5]);
}
BOOL MOD_CFGWORD::IsHasNo(int iNo)			//��������Ƿ���ָ����iNo��λ
{
	int indexOfBytes=(iNo-1)/8;
	int indexOfBits=(iNo-1)%8;
	BYTE byteConstArr[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	return (flag.bytes[indexOfBytes]&byteConstArr[indexOfBits])>0;
}
BOOL MOD_CFGWORD::IsHasBodyNoOnly(int iBodyNo,BYTE schema/*=MULTILEG_SCHEMA*/)	//��������Ƿ��ָ���ֽ�����ֵ������Ϊ0��iByte��1Ϊ����(������ֵ+1)
{
	if(iBodyNo<=0||iBodyNo>MaxBodys(schema))
		return FALSE;
	else
	{
		int i,nMaxBodies=MaxBodys(schema),nMaxLegs=MaxLegs(schema);
		schema=ValidateSchema(schema);
		for(i=1;i<=nMaxBodies;i++)
		{
			DWORD legword=0;
			memcpy(&legword,&flag.bytes[(i-1)*schema],schema);
			if(i==iBodyNo&&legword==0)
				return FALSE;
			else if(i!=iBodyNo&&legword>0)
				return FALSE;
		}
		return TRUE;
	}
}
BOOL MOD_CFGWORD::IsHasBodyNo(int iBodyNo,BYTE schema/*=0*/)		//�������ָ���ֽ��Ƿ���ֵ������Ϊ0��iByte��1Ϊ����(������ֵ+1)
{
	if(iBodyNo<=0||iBodyNo>MaxBodys(schema))
		return FALSE;
	schema=ValidateSchema(schema);
	DWORD legword=0;
	memcpy(&legword,&flag.bytes[(iBodyNo-1)*schema],schema);
	if(legword>0)
		return TRUE;
	else
		return FALSE;
}
BOOL MOD_CFGWORD::AddBodyLegs(int iBodyNo,DWORD legword/*=0xffffff*/,BYTE schema/*=0*/)
{
	if(iBodyNo<=0||iBodyNo>MaxBodys(schema))
		return FALSE;
	schema=ValidateSchema(schema);
	if(schema==MULTILEG_MAX08)
		legword&=0xff;
	else if(schema==MULTILEG_MAX16)
		legword&=0xffff;
	else if(schema==MULTILEG_MAX24)
		legword&=0xffffff;
	BYTE* bytes=(BYTE*)&legword;
	for(int j=0;j<schema;j++)
		flag.bytes[(iBodyNo-1)*schema+j]|=bytes[j];
	if(legword>0)
		return TRUE;
	else
		return FALSE;
}
BOOL MOD_CFGWORD::SetBodyLegs(int iBodyNo,DWORD legword/*=0xffffff*/,BYTE schema/*=0*/)
{
	if(iBodyNo<=0||iBodyNo>MaxBodys(schema))
		return FALSE;
	schema=ValidateSchema(schema);
	if(schema==MULTILEG_MAX08)
		legword&=0xff;
	else if(schema==MULTILEG_MAX16)
		legword&=0xffff;
	else if(schema==MULTILEG_MAX24)
		legword&=0xffffff;
	memcpy(&flag.bytes[(iBodyNo-1)*schema],&legword,schema);
	if(legword>0)
		return TRUE;
	else
		return FALSE;
}
MOD_CFGWORD MOD_CFGWORD::SetWordByNo(int iNo)			//����ָ����iNo��λָ�������
{
	iNo--;
	Clear();
	if(iNo>=0&&iNo<192)
	{
		int iWord=iNo/32;
		int iBit=iNo%32;
		flag.word[iWord]=_LocalGetSingleWord(iBit+1);//CFG_NO[iBit];
	}
	return *this;
}
MOD_CFGWORD MOD_CFGWORD::AddSpecWord(MOD_CFGWORD cfgword)	//�൱����������ֽ��л����,���������ֵ����ǰ�����
{
	for(int i=0;i<6;i++)
		flag.word[i]=flag.word[i] | cfgword.flag.word[i];
	return *this;
}
///////////////////////////////////////////////////////////////////////////////
UCS_STRU TransToUcs(MOD_CS modCs)
{
	UCS_STRU cs;
	cs.origin.Set(modCs.origin.x,modCs.origin.y,modCs.origin.z);
	cs.axis_x.Set(modCs.axisX.x,modCs.axisX.y,modCs.axisX.z);
	cs.axis_y.Set(modCs.axisY.x,modCs.axisY.y,modCs.axisY.z);
	cs.axis_z.Set(modCs.axisZ.x,modCs.axisZ.y,modCs.axisZ.z);
	return cs;
}
MOD_CS TransToModUcs(UCS_STRU cs)
{
	MOD_CS mod_cs;
	mod_cs.origin.Set(cs.origin.x,cs.origin.y,cs.origin.z);
	mod_cs.axisX.Set(cs.axis_x.x,cs.axis_x.y,cs.axis_x.z);
	mod_cs.axisY.Set(cs.axis_y.x,cs.axis_y.y,cs.axis_y.z);
	mod_cs.axisZ.Set(cs.axis_z.x,cs.axis_z.y,cs.axis_z.z);
	return mod_cs;
}
void ANSIToUnicode(const char* src_str,wchar_t* des_str)
{
	//��ȡ��ת���ַ����Ļ�����������Ŀ��ַ�����С
	int nLen=MultiByteToWideChar(CP_ACP,0,src_str,-1,NULL,0 );
	wchar_t* sResult=(wchar_t*)malloc((nLen+1)*sizeof(wchar_t));
	memset(sResult,0,sizeof(wchar_t)*(nLen+1));
	//���ֽ��ַ���ת��Ϊ���ַ���
	MultiByteToWideChar(CP_ACP,0,src_str,-1,(LPWSTR)sResult,nLen);
	//
	memset(des_str,0,MAX_PATH);
	wcscpy(des_str,sResult);
	free(sResult);
}

void UnicodeToANSI(const wchar_t* src_str,char* des_str)
{
	int nLen=WideCharToMultiByte(CP_ACP,0,src_str,-1, NULL,0,NULL,NULL);
	char* sResult=(char*)malloc((nLen+1)*sizeof(char));
	memset(sResult,0,sizeof(char)*(nLen+1));
	WideCharToMultiByte(CP_ACP,0,src_str,-1,sResult,nLen,NULL,NULL);
	//
	memset(des_str,0,MAX_PATH);
	strcpy(des_str,sResult);
	free(sResult);
}

void UTF8ToUnicode(const char* src_str,wchar_t* des_str)
{
	int nLen=MultiByteToWideChar(CP_UTF8,0,src_str,-1, NULL,0);
	wchar_t* sResult=(wchar_t*)malloc((nLen+1)*sizeof(wchar_t));
	memset(sResult,0,(nLen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8,0,src_str,-1,(LPWSTR)sResult,nLen);
	//
	memset(des_str,0,MAX_PATH);
	wcscpy(des_str,sResult);
	free(sResult);
}

void UnicodeToUTF8(const wchar_t* src_str,char* des_str)
{
	int nLen=WideCharToMultiByte(CP_UTF8,0,src_str,-1,NULL,0,NULL,NULL);
	char* sResult=(char*)malloc((nLen+1)*sizeof(char));
	memset(sResult,0,sizeof(char)*(nLen+1));
	WideCharToMultiByte(CP_UTF8,0,src_str,-1,sResult,nLen,NULL,NULL);
	//
	memset(des_str,0,MAX_PATH);
	strcpy(des_str,sResult);
	free(sResult);
}
void ANSIToUTF8(const char* src_str,char* des_str)
{
	wchar_t sWText[MAX_PATH];
	ANSIToUnicode(src_str,sWText);	//ansi���ַ�ת���ַ�
	UnicodeToUTF8(sWText,des_str);	//���ַ�תutf8���ַ�
}

void UTF8ToANSI(const char* src_str,char* des_str)
{
	wchar_t sWText[MAX_PATH];
	UTF8ToUnicode(src_str,sWText);	//utf8���ַ�ת���ַ�
	UnicodeToANSI(sWText,des_str);	//���ַ�תansi���ַ�
}

//UTF-8�ַ��������:
//1�ֽڣ�0xxxxxxx
//2�ֽڣ�110xxxxx 10xxxxxx
//3�ֽڣ�1110xxxx 10xxxxxx 10xxxxxx
//4�ֽڣ�11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//5�ֽڣ�111110xx	10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//6�ֽڣ�1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
BOOL IsUTF8File(const char* mod_file)
{
	FILE *fp = fopen(mod_file,"rb");
	if(fp==NULL)
		return FALSE;
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	CBuffer buffer;
	buffer.Write(NULL,lSize);
	fread(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	//
	unsigned char* start = (unsigned char*)buffer.GetBufferPtr();
	unsigned char* end = (unsigned char*)start + lSize;
	BOOL bIsUTF8 = TRUE;
	while(start<end)
	{
		if(*start<0x80)
		{	//1�ֽ�(0xxxxxxx): ֵС��0x80��ΪASCII�ַ�  	
			start++; 
		}
		else if(*start<(0xC0)) 
		{	//ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ� 
			bIsUTF8=FALSE;  
			break;
		}
		else if(*start<(0xE0)) 
		{	//2�ֽ�(110xxxxx 10xxxxxx)
			if(start>=end-1)
				break;
			if((start[1]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=2;
		}
		else if(*start<(0xF0)) 
		{	//3�ֽ�(1110xxxx 10xxxxxx 10xxxxx)
			if(start>=end-2)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=3;
		}
		else if(*start<(0xF8))
		{	//4�ֽ�(11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if(start>=end-3)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80||(start[3]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=4;
		}
		else if(*start<(0xFC))
		{	//5�ֽ�(111110xx	10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if(start>=end-4)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80||(start[3]&(0xC0))!=0x80||
				(start[4]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=5;
		}
		else if(*start<(0xFE))
		{	//6�ֽ�(1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if(start>=end-5)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80||(start[3]&(0xC0))!=0x80||
				(start[4]&(0xC0))!=0x80||(start[5]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=6;
		}
		else
		{
			bIsUTF8 = FALSE;
			break;
		}
	}
	if(!bIsUTF8)
	{	//�����ַ�ƥ������ٴν����ж�
		int goodbytes = 0, asciibytes = 0;
		int rawtextlen = lSize;
		char* rawtext=buffer.GetBufferPtr();
		for(int i = 0; i < rawtextlen; i++) 
		{
			if ((rawtext[i] &  0x7F) == rawtext[i]) 
			{	//���λ��0��ASCII�ַ�
				//һλ��������
				asciibytes++;
			} 
			else if (-64 <= rawtext[i] && rawtext[i] <= -33
				//��λ��������,��һλ11000000--11011111
				//��һλ��10000000--10111111
				&&i + 1 < rawtextlen 
				&& -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65) 
			{
				goodbytes += 2;
				i++;
			} 
			else if (-32 <= rawtext[i]&& rawtext[i] <= -17
				//��λ��������,��һλ11100000--11101111
				//����λ��10000000--10111111
				&&i + 2 < rawtextlen 
				&& -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65) 
			{
				goodbytes += 3;
				i += 2;
			}
			else if(-16 <= rawtext[i]&& rawtext[i] <= -9
				//��λ��������,��һλ11110000--11110111
				//����λ��10000000--10111111
				&&i + 3 < rawtextlen 
				&&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
				&& -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65)

			{
				goodbytes += 4;
				i += 3;
			}
			else if(-8 <= rawtext[i]&& rawtext[i] <= -5
				//��λ��������,��һλ11111000--11111011
				//����λ��10000000--10111111
				&&i + 4 < rawtextlen 
				&&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
				&& -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65
				&& -128 <= rawtext[i + 4] && rawtext[i + 4] <= -65)
			{
				goodbytes += 5;
				i += 4;
			}
			else if(-4 <= rawtext[i]&& rawtext[i] <= -3
				//��λ��������,��һλ11111100--11111101
				//����λ��10000000--10111111
				&&i + 5 < rawtextlen 
				&&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
				&& -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65
				&& -128 <= rawtext[i + 4] && rawtext[i + 4] <= -65
				&& -128 <= rawtext[i + 5] && rawtext[i + 5] <= -65)
			{
				goodbytes += 6;
				i += 5;
			}
		}
		if (asciibytes == rawtextlen) 
			return true;
		int score=100*goodbytes/(rawtextlen - asciibytes);
		if (score>98) //���ƥ���ʴﵽ98%����,��ɹ�
			return true;
		else if(score>95&&goodbytes>30) 
			return true;
		else 
			return false;
	}
	return true;
}
BOOL RetrieveBodyInfo(int* pBodyNoArr,int iStart,int n,NODESET& validNodeSet,NODESET& selNodeSet,
						RODSET& validRodSet,RODSET& selRodSet)
{
	if(iStart==n-1)
		return FALSE;
	for(CModNode* pNode=validNodeSet.GetFirst();pNode;pNode=validNodeSet.GetNext())
	{
		BOOL bValid=TRUE;
		for(int i=iStart;i<n;i++)
		{
			if(!pNode->m_xCfgword.IsHasBodyNo(pBodyNoArr[i]))
			{
				bValid=FALSE;
				break;
			}
		}
		if(bValid)
			selNodeSet.append(pNode);
	}
	//
	for(CModRod* pRod=validRodSet.GetFirst();pRod;pRod=validRodSet.GetNext())
	{
		BOOL bValid=TRUE;
		for(int i=iStart;i<n;i++)
		{
			if(!pRod->m_xCfgword.IsHasBodyNo(pBodyNoArr[i]))
			{
				bValid=FALSE;
				break;
			}
		}
		if(bValid)
			selRodSet.append(pRod);
	}
	return selNodeSet.GetNodeNum()>0;
}
//////////////////////////////////////////////////////////////////////////
//
CLegItem::CLegItem()
{
	m_hTagNode=0;
	m_iLegNo=0;
	m_fSegmentH=0;
	m_fMaxLegH=0;
}
void CLegItem::InitLegItemByModule(CModHeightGroup* pModule)
{
	CModModel* pModModel=pModule?(CModModel*)pModule->BelongModel():NULL;
	if(pModModel==NULL)
		return;
	m_iLegNo=pModule->m_iNo;
	m_fMaxLegH=pModule->lowest_z;
	NODESET allLegNodeSet;
	for(CModNode* pNode=pModModel->EnumFirstNode();pNode;pNode=pModModel->EnumNextNode())
	{
		if(pNode->IsLegNode() && pNode->m_xCfgword.IsHasBodyNo(m_iLegNo))
			allLegNodeSet.append(pNode);
		else if(pNode->m_xCfgword.IsHasBodyNoOnly(m_iLegNo))
		{
			pNode->SetLayer('S');	//���߶��ж�
			segmentNodeSet.append(pNode);
			if(pNode->xLdsOrg.z>m_fSegmentH)
				m_fSegmentH=pNode->xLdsOrg.z;
		}
	}
	RODSET allLegRodSet;
	for(CModRod* pRod=pModModel->EnumFirstRod();pRod;pRod=pModModel->EnumNextRod())
	{
		if(pRod->IsLegRod() && pRod->m_xCfgword.IsHasBodyNo(m_iLegNo))
			allLegRodSet.append(pRod);
		else if(pRod->m_xCfgword.IsHasBodyNoOnly(m_iLegNo))
		{
			pRod->SetLayer('S');	//���߶��ж�
			segmentRodSet.append(pRod);
		}
	}
	//��ȡ�Ȳ���Ϣ
	int nMaxLegs=MOD_CFGWORD::MaxLegs();
	WORD activeQuadLegNo[4];
	memcpy(activeQuadLegNo,pModule->m_arrActiveQuadLegNo,8);
	for(int i=0;i<nMaxLegs;i++)
	{
		if(!pModule->m_dwLegCfgWord.And(MOD_CFGWORD(m_iLegNo,i+1)))
			break;
		char cLegMark='A'+i;
		pModule->m_arrActiveQuadLegNo[0]=(m_iLegNo-1)*nMaxLegs+cLegMark-'@';
		pModule->m_arrActiveQuadLegNo[1]=(m_iLegNo-1)*nMaxLegs+cLegMark-'@';
		pModule->m_arrActiveQuadLegNo[2]=(m_iLegNo-1)*nMaxLegs+cLegMark-'@';
		pModule->m_arrActiveQuadLegNo[3]=(m_iLegNo-1)*nMaxLegs+cLegMark-'@';
		//
		SUB_LEG_INFO* pLegInfo=subLegInfoList.append();
		for(CModNode* pNode=allLegNodeSet.GetFirst();pNode;pNode=allLegNodeSet.GetNext())
		{
			if(!pModule->IsSonNode(pNode))
				continue;
			pLegInfo->legNodeSet.append(pNode);
			if(pNode->xLdsOrg.z>pLegInfo->m_fLegH)
				pLegInfo->m_fLegH=pNode->xLdsOrg.z;
			//��¼�ú��ߵ���ȵĵ�����
			if(pNode->xLdsOrg.z<m_maxLegPt.z)
				m_maxLegPt=pNode->xLdsOrg;
		}
		pLegInfo->m_fLegH-=m_fMaxLegH;
		//�Ȳ��˼�
		for(CModRod* pRod=allLegRodSet.GetFirst();pRod;pRod=allLegRodSet.GetNext())
		{
			if(!pModule->IsSonRod(pRod))
				continue;
			//����Ȳ��˼����Ƿ���ڿ������(ʼ�˽ڵ���ն˽ڵ㶼���Ȳ��ڵ㣬������ͬһ������)
			BOOL bValidS=TRUE,bValidE=TRUE;
			CModNode* pNodeS=pModModel->FindNode(pRod->m_uiNodeS);
			if(pNodeS&&pNodeS->IsLegNode())
				bValidS=pModule->IsSonNode(pNodeS);
			CModNode* pNodeE=pModModel->FindNode(pRod->m_uiNodeE);
			if(pNodeE&&pNodeE->IsLegNode())
				bValidE=pModule->IsSonNode(pNodeE);
			if(bValidS && bValidE)
				pLegInfo->legRodSet.append(pRod);
			else
				logerr.Log("�˼�(0X%X)��ʼ�˽ڵ���ն˽ڵ㲻��ͬһ����!)");
			//��¼�ú��ߵ���ȵĵ�����
			if(pNodeS->xLdsOrg.z>m_maxLegPt.z)
				m_maxLegPt=pNodeS->xLdsOrg;
			if(pNodeE->xLdsOrg.z>m_maxLegPt.z)
				m_maxLegPt=pNodeE->xLdsOrg;
		}
	}
	memcpy(pModule->m_arrActiveQuadLegNo,activeQuadLegNo,8);
}
//////////////////////////////////////////////////////////////////////////
//
void CBodyItem::InitBodyItem(CModNode* &pTagNode,NODESET& selNodeSet,RODSET& selRodSet)
{
	//��䱾��ڵ�͸˼�
	CModModel* pModel=(CModModel*)selNodeSet.GetFirst()->BelongModel();
	for(CModNode* pNode=selNodeSet.GetFirst();pNode;pNode=selNodeSet.GetNext())
		bodyNodeSet.append(pNode);
	for(CModRod* pRod=selRodSet.GetFirst();pRod;pRod=selRodSet.GetNext())
		bodyRodSet.append(pRod);
	//���ұ��λ�ýڵ�
	for(CModRod* pRod=selRodSet.GetFirst();pRod;pRod=selRodSet.GetNext())
	{
		CModNode* pNodeS=(CModNode*)pRod->BelongModel()->FindNode(pRod->m_uiNodeS);
		CModNode* pNodeE=(CModNode*)pRod->BelongModel()->FindNode(pRod->m_uiNodeE);
		if(pNodeS==NULL&&pNodeE!=NULL)
			logerr.Log("0x%X�˼�ʼ�˽ڵ㶪ʧ",pRod->handle);
		if(pNodeS!=NULL&&pNodeE==NULL)
			logerr.Log("0x%X�˼��ն˽ڵ㶪ʧ",pRod->handle);
		if(pNodeS==NULL||pNodeE==NULL)
			continue;
		GEPOINT ptS=pNodeS->xOrg,ptE=pNodeE->xOrg;
		if((ptS.x<0 &&ptS.y>0) && (ptE.x<0 &&ptE.y>0))
		{	//�ڶ����޸˼�
			CModNode* pNode=(ptS.z<ptE.z)?pNodeS:pNodeE;
			if(pNode->xLdsOrg.z>m_fBodyH)
			{
				m_fBodyH=pNode->xLdsOrg.z;
				m_hTagNode=pNode->handle;
			}
		}
	}
	if(pTagNode)
		m_fBodyH-=pTagNode->xLdsOrg.z;
	pTagNode=(CModNode*)pModel->FindNode(m_hTagNode);
}
//////////////////////////////////////////////////////////////////////////
//CModNode
CModNode::CModNode()
{
	m_pBelongModel=NULL;
	handle=0;
	strcpy(m_sLayer,"");
}
void CModNode::Clone(IModNode* pSrcNode)
{
	m_sLayer[0]=((CModNode*)pSrcNode)->m_sLayer[0];
	m_xCfgword=((CModNode*)pSrcNode)->m_xCfgword;
	handle=((CModNode*)pSrcNode)->handle;
	xOrg=((CModNode*)pSrcNode)->xOrg;
	xLdsOrg=((CModNode*)pSrcNode)->xLdsOrg;
}
BYTE CModNode::GetLegQuad()
{
	if(m_sLayer[0]=='L')
	{
		if(xLdsOrg.x>=0&&xLdsOrg.y>=0)
			return 1;
		else if(xLdsOrg.x<=0&&xLdsOrg.y>=0)
			return 2;
		else if(xLdsOrg.x>=0&&xLdsOrg.y<=0)
			return 3;
		else if(xLdsOrg.x<=0&&xLdsOrg.y<=0)
			return 4;
		else
			return 0;
	}
	else
		return 0;
}
//////////////////////////////////////////////////////////////////////////
//CModRod
CModRod::CModRod()
{
	handle=0;
	m_pBelongModel=NULL;
	m_ciRodType=1;	//1.�Ǹ�|2.�ֹ�
	m_nFace=6;
	m_cMaterial='S';
	m_fWidth=100;
	m_fWidth2=200;
	m_fThick=10;
	m_uiNodeS=m_uiNodeE=0;
	strcpy(m_sLayer,"");
}
CModRod::~CModRod()
{

}
void CModRod::Clone(IModRod* pSrcRod)
{
	m_sLayer[0]=((CModRod*)pSrcRod)->m_sLayer[0];
	m_ciRodType=((CModRod*)pSrcRod)->m_ciRodType;
	m_cMaterial=((CModRod*)pSrcRod)->m_cMaterial;
	m_fWidth=((CModRod*)pSrcRod)->m_fWidth;
	m_fThick=((CModRod*)pSrcRod)->m_fThick;
	m_uiNodeS=((CModRod*)pSrcRod)->m_uiNodeS;
	m_uiNodeE=((CModRod*)pSrcRod)->m_uiNodeE;
	m_vWingX=((CModRod*)pSrcRod)->m_vWingX;
	m_vWingY=((CModRod*)pSrcRod)->m_vWingY;
}
MOD_LINE CModRod::GetBaseLineToLdsModel()
{
	UCS_STRU ucs=TransToUcs(m_pBelongModel->BuildUcsByModCS());
	f3dLine line=base_line;
	coord_trans(line.startPt,ucs,FALSE);
	coord_trans(line.endPt,ucs,FALSE);
	return MOD_LINE(line.startPt,line.endPt);
}
UCS_STRU CModRod::BuildUcs(BOOL bToLds/*=TRUE*/)
{
	UCS_STRU ucs;
#ifdef DISABLE_SOLID_BODY
	return ucs;
#else
	UCS_STRU lds_ucs=TransToUcs(m_pBelongModel->BuildUcsByModCS());
	f3dPoint ptS=base_line.startPt;
	f3dPoint ptE=base_line.endPt;
	if(bToLds)
	{
		coord_trans(ptS,lds_ucs,FALSE);
		coord_trans(ptE,lds_ucs,FALSE);
	}
	f3dPoint exten_vec=(ptE-ptS).normalized();
	if(m_ciRodType==1)
	{
		f3dPoint vNormX=m_vNormX,vNormY=m_vNormY;
		if(bToLds)
		{
			vector_trans(vNormX,lds_ucs,FALSE);
			vector_trans(vNormY,lds_ucs,FALSE);
		}
		ucs.origin=ptS;
		ucs.axis_z=exten_vec;
		ucs.axis_x=ucs.axis_z^vNormX;
		normalize(ucs.axis_x);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
	}
	else if(m_ciRodType==2||m_ciRodType==3)
	{
		ucs.origin=ptS;
		ucs.axis_z=exten_vec;
		ucs.axis_y=inters_vec(ucs.axis_z);
		normalize(ucs.axis_z);
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
	}
	return ucs;
#endif
}
//���ɽǸ�ʵ��
BOOL CModRod::Create3dJgSolidModel(CSolidBody *pSolidBody,BOOL bToLds/*=TRUE*/)
{
#ifdef DISABLE_SOLID_BODY
	return FALSE;
#else
	UCS_STRU ucs=BuildUcs(bToLds);
	fBody body;
	fBody* pBody=&body;
	//��ʼ��������
	f3dPoint norm_X(0,-1,0),norm_Y(-1,0,0);
	f3dPoint wing_x_vec(1,0,0),wing_y_vec(0,1,0);
	double fLen=DISTANCE(base_line.startPt,base_line.endPt);
	f3dPoint vertex_arr[12];
	vertex_arr[0].Set(0,0,0);
	vertex_arr[1].Set(m_fWidth*wing_y_vec.x,m_fWidth*wing_y_vec.y);
	vertex_arr[2].Set(vertex_arr[1].x+m_fThick*norm_Y.x,vertex_arr[1].y-m_fThick*norm_Y.y);
	vertex_arr[3].Set(m_fThick,m_fThick);
	vertex_arr[4].Set(m_fWidth,m_fThick);
	vertex_arr[5].Set(m_fWidth,0,0);
	for(int i=0;i<6;i++)
		vertex_arr[i+6].Set(vertex_arr[i].x,vertex_arr[i].y,fLen);
	//������
	for(int i=0;i<12;i++)
		pBody->vertex.append(vertex_arr[i]);
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	COLORREF color(RGB(220,220,220));
	f3dPolyFace *pFace=NULL;
	//ʼ����
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,1,0);
	createFace.NewOutterEdgeLine(pFace,2);
	createFace.NewOutterEdgeLine(pFace,3);
	createFace.NewOutterEdgeLine(pFace,4);
	createFace.NewOutterEdgeLine(pFace,5);
	createFace.NewOutterEdgeLine(pFace,0);
	//�ն���
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,11,6);
	createFace.NewOutterEdgeLine(pFace,10);
	createFace.NewOutterEdgeLine(pFace,9);
	createFace.NewOutterEdgeLine(pFace,8);
	createFace.NewOutterEdgeLine(pFace,7);
	createFace.NewOutterEdgeLine(pFace,6);
	//Y֫�ϵ���
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,2,3);
	createFace.NewOutterEdgeLine(pFace,8);
	createFace.NewOutterEdgeLine(pFace,9);
	createFace.NewOutterEdgeLine(pFace,3);
	//Y֫�µ���
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,0,1);
	createFace.NewOutterEdgeLine(pFace,6);
	createFace.NewOutterEdgeLine(pFace,7);
	createFace.NewOutterEdgeLine(pFace,1);
	//X֫�����
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,5,0);
	createFace.NewOutterEdgeLine(pFace,11);
	createFace.NewOutterEdgeLine(pFace,6);
	createFace.NewOutterEdgeLine(pFace,0);
	//X֫�ҵ���
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,3,4);
	createFace.NewOutterEdgeLine(pFace,9);
	createFace.NewOutterEdgeLine(pFace,10);
	createFace.NewOutterEdgeLine(pFace,4);
	//X֫�϶���
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,4,5);
	createFace.NewOutterEdgeLine(pFace,10);
	createFace.NewOutterEdgeLine(pFace,11);
	createFace.NewOutterEdgeLine(pFace,5);
	//8.Y֫�Ҷ���
	pFace = pBody->faceList.append();
	pFace->material = color;
	createFace.NewOutterEdgeLine(pFace,1,2);
	createFace.NewOutterEdgeLine(pFace,7);
	createFace.NewOutterEdgeLine(pFace,8);
	createFace.NewOutterEdgeLine(pFace,2);
	//����ϵ���������ת�����ṹ��ľ���������
	for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
	{
		if(coord_trans(*pVertice,ucs,TRUE)!=TRUE)
			return FALSE;
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
#endif
}
struct SLICE{double radAngle,sina,cosa;};
static SLICE SLICE16_ARR[16]={
	{0.000000, 0.0000000000, 1.0000000000},	{0.392699, 0.3826834324, 0.9238795325},
	{0.785398, 0.7071067812, 0.7071067812},	{1.178097, 0.9238795325, 0.3826834324},
	{1.570796, 1.0000000000, 0.0000000000},	{1.963495, 0.9238795325,-0.3826834324},
	{2.356194, 0.7071067812,-0.7071067812},	{2.748894, 0.3826834324,-0.9238795325},
	{3.141593, 0.0000000000,-1.0000000000},	{3.534292,-0.3826834324,-0.9238795325},
	{3.926991,-0.7071067812,-0.7071067812},	{4.319690,-0.9238795325,-0.3826834324},
	{4.712389,-1.0000000000,-0.0000000000},	{5.105088,-0.9238795325, 0.3826834324},
	{5.497787,-0.7071067812, 0.7071067812},	{5.890486,-0.3826834324, 0.9238795325}};
static SLICE SLICE24_ARR[24]={
		{0.000000, 0.0000000000, 1.0000000000},	{0.261799, 0.2588190451, 0.9659258263},
		{0.523599, 0.5000000000, 0.8660254038},	{0.785398, 0.7071067812, 0.7071067812},
		{1.047198, 0.8660254038, 0.5000000000},	{1.308997, 0.9659258263, 0.2588190451},
		{1.570796, 1.0000000000, 0.0000000000},	{1.832596, 0.9659258263,-0.2588190451},
		{2.094395, 0.8660254038,-0.5000000000},	{2.356194, 0.7071067812,-0.7071067812},
		{2.617994, 0.5000000000,-0.8660254038},	{2.879793, 0.2588190451,-0.9659258263},
		{3.141593, 0.0000000000,-1.0000000000},	{3.403392,-0.2588190451,-0.9659258263},
		{3.665191,-0.5000000000,-0.8660254038},	{3.926991,-0.7071067812,-0.7071067812},
		{4.188790,-0.8660254038,-0.5000000000},	{4.450590,-0.9659258263,-0.2588190451},
		{4.712389,-1.0000000000,-0.0000000000},	{4.974188,-0.9659258263, 0.2588190451},
		{5.235988,-0.8660254038, 0.5000000000},	{5.497787,-0.7071067812, 0.7071067812},
		{5.759587,-0.5000000000, 0.8660254038},	{6.021386,-0.2588190451, 0.9659258263}};
static SLICE SLICE48_ARR[48]={
	{0.000000, 0.0000000000, 1.0000000000},{0.130900, 0.1305261922, 0.9914448614},
	{0.261799, 0.2588190451, 0.9659258263},{0.392699, 0.3826834324, 0.9238795325},
	{0.523599, 0.5000000000, 0.8660254038},{0.654498, 0.6087614290, 0.7933533403},
	{0.785398, 0.7071067812, 0.7071067812},{0.916298, 0.7933533403, 0.6087614290},
	{1.047198, 0.8660254038, 0.5000000000},{1.178097, 0.9238795325, 0.3826834324},
	{1.308997, 0.9659258263, 0.2588190451},{1.439897, 0.9914448614, 0.1305261922},
	{1.570796, 1.0000000000, 0.0000000000},{1.701696, 0.9914448614,-0.1305261922},
	{1.832596, 0.9659258263,-0.2588190451},{1.963495, 0.9238795325,-0.3826834324},
	{2.094395, 0.8660254038,-0.5000000000},{2.225295, 0.7933533403,-0.6087614290},
	{2.356194, 0.7071067812,-0.7071067812},{2.487094, 0.6087614290,-0.7933533403},
	{2.617994, 0.5000000000,-0.8660254038},{2.748894, 0.3826834324,-0.9238795325},
	{2.879793, 0.2588190451,-0.9659258263},{3.010693, 0.1305261922,-0.9914448614},
	{3.141593, 0.0000000000,-1.0000000000},{3.272492,-0.1305261922,-0.9914448614},
	{3.403392,-0.2588190451,-0.9659258263},{3.534292,-0.3826834324,-0.9238795325},
	{3.665191,-0.5000000000,-0.8660254038},{3.796091,-0.6087614290,-0.7933533403},
	{3.926991,-0.7071067812,-0.7071067812},{4.057891,-0.7933533403,-0.6087614290},
	{4.188790,-0.8660254038,-0.5000000000},{4.319690,-0.9238795325,-0.3826834324},
	{4.450590,-0.9659258263,-0.2588190451},{4.581489,-0.9914448614,-0.1305261922},
	{4.712389,-1.0000000000,-0.0000000000},{4.843289,-0.9914448614, 0.1305261922},
	{4.974188,-0.9659258263, 0.2588190451},{5.105088,-0.9238795325, 0.3826834324},
	{5.235988,-0.8660254038, 0.5000000000},{5.366887,-0.7933533403, 0.6087614290},
	{5.497787,-0.7071067812, 0.7071067812},{5.628687,-0.6087614290, 0.7933533403},
	{5.759587,-0.5000000000, 0.8660254038},{5.890486,-0.3826834324, 0.9238795325},
	{6.021386,-0.2588190451, 0.9659258263},{6.152286,-0.1305261922, 0.9914448614}};
//���ɸֹ�ʵ��
BOOL CModRod::Create3dTubeSolidModel(CSolidBody *pSolidBody,BOOL bToLds/*=TRUE*/)
{
#ifdef DISABLE_SOLID_BODY
	return FALSE;
#else
	fBody body;
	fBody *pBody=&body;
	int slices=24;
	SLICE* pSlice=SLICE24_ARR;
	double coshalf_alfa=cos(Pi/slices);	//�������
	double D=m_fWidth/coshalf_alfa;
	double T=m_fThick/coshalf_alfa;
	double high=GetLength();
	double fOuterR=D*0.5;
	double fInnerR=fOuterR-T;
	double scaleRI=fInnerR/fOuterR;
	//
	UCS_STRU ucs=BuildUcs(bToLds);
	f3dPoint vertex_arr[200];
	for(int i=0;i<slices;i++)
	{
		vertex_arr[i].x=fOuterR*pSlice[i].cosa;
		vertex_arr[i].y=fOuterR*pSlice[i].sina;
		vertex_arr[i].z=0;
	}
	for(int i=0;i<slices;i++)
	{
		f3dPoint pt=vertex_arr[i];
		pBody->vertex.append(f3dPoint(pt.x,pt.y,0));					//������Բ����
		pBody->vertex.append(f3dPoint(pt.x,pt.y,high));					//������Բ����
		pBody->vertex.append(f3dPoint(pt.x*scaleRI,pt.y*scaleRI,0));	//������Բ����
		pBody->vertex.append(f3dPoint(pt.x*scaleRI,pt.y*scaleRI,high));	//������Բ����
	}
	for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		coord_trans(*pVertice,ucs,TRUE,TRUE);
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	//�ֹܵ��洴��
	f3dPolyFace *pFace=NULL;
	COLORREF color(RGB(220,220,220));
	for(int i=0;i<slices;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=-ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,i*4+2,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4);
		createFace.NewOutterEdgeLine(pFace,i*4);
		//
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=-ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%slices)*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4);
	}
	//�ֹܶ��洴��
	for(int i=0;i<slices;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		//
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+3,((i+1)%slices)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1);
	}
	//�ֹ���/����洴��
	for(int i=0;i<slices;i++)
	{
		//�ֹ�����洴��
		pFace=pBody->faceList.append();
		pFace->material=color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4);
		f3dPoint vertex1=createFace.VertexAt(((i+1)%slices)*4);
		f3dPoint vertex2=createFace.VertexAt(((i+1)%slices)*4+1);
		f3dPoint vertex3=createFace.VertexAt(i*4+1);
		f3dPoint vec1=(vertex3-vertex2).normalized();
		f3dPoint vec2=(vertex1-vertex2).normalized();
		pFace->poly_norm=vec1^vec2;
		//�ֹ��ڲ��洴��
		pFace=pBody->faceList.append();
		pFace->material=color;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%slices)*4+2);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*4+2);
		vertex1=createFace.VertexAt(i*4+2);
		vertex2=createFace.VertexAt(i*4+3);
		vertex3=createFace.VertexAt(((i+1)%slices)*4+3);
		vec1=(vertex3-vertex2).normalized();
		vec2=(vertex1-vertex2).normalized();
		pFace->poly_norm=vec1^vec2;
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
#endif
}
//����׵��ʵ��
BOOL CModRod::Create3dConeSolidModel(CSolidBody *pSolidBody,BOOL bToLds/*=TRUE*/)
{
#ifdef DISABLE_SOLID_BODY
	return FALSE;
#else
	fBody body;
	fBody* pBody=&body;
	UCS_STRU ucs=BuildUcs(bToLds);
	double fRS=m_fWidth*0.5;
	double fRE=m_fWidth2*0.5;
	double fLen=DISTANCE(base_line.startPt,base_line.endPt);
	double fPiceAngle=2*Pi/m_nFace;
	f3dPoint ptS_O(fRS,0,0);
	f3dPoint ptS_I(fRS-m_fThick,0,0);
	f3dPoint ptE_O(fRE,0,fLen);
	f3dPoint ptE_I(fRE-m_fThick,0,fLen);
	f3dPoint vertex;
	for(int i=0;i<m_nFace;i++)
	{
		vertex=ptS_O;
		rotate_point_around_axis(vertex,i*fPiceAngle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		pBody->vertex.append(vertex);
		//
		vertex=ptE_O;
		rotate_point_around_axis(vertex,i*fPiceAngle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		pBody->vertex.append(vertex);
		//
		vertex=ptS_I;
		rotate_point_around_axis(vertex,i*fPiceAngle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		pBody->vertex.append(vertex);
		//
		vertex=ptE_I;
		rotate_point_around_axis(vertex,i*fPiceAngle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		pBody->vertex.append(vertex);
	}
	for(f3dPoint *pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext())
		coord_trans(*pVertice,ucs,TRUE,TRUE);
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	//�ֹܵ��洴��
	f3dPolyFace *pFace=NULL;
	COLORREF color(RGB(220,220,220));
	for(int i=0;i<m_nFace;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=-ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,i*4+2,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4);
		createFace.NewOutterEdgeLine(pFace,i*4);
		//
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=-ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%m_nFace)*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4);
	}
	//�ֹܶ��洴��
	for(int i=0;i<m_nFace;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+1,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		//
		pFace=pBody->faceList.append();
		pFace->material=color;
		pFace->poly_norm=ucs.axis_z;
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+3,((i+1)%m_nFace)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+1);
	}
	//�ֹ���/����洴��
	for(int i=0;i<m_nFace;i++)
	{
		//�ֹ�����洴��
		pFace=pBody->faceList.append();
		pFace->material=color;
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4,i*4);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4+1);
		createFace.NewOutterEdgeLine(pFace,i*4);
		f3dPoint vertex1=createFace.VertexAt(((i+1)%m_nFace)*4);
		f3dPoint vertex2=createFace.VertexAt(((i+1)%m_nFace)*4+1);
		f3dPoint vertex3=createFace.VertexAt(i*4+1);
		f3dPoint vec1=(vertex3-vertex2).normalized();
		f3dPoint vec2=(vertex1-vertex2).normalized();
		pFace->poly_norm=vec1^vec2;
		//�ֹ��ڲ��洴��
		pFace=pBody->faceList.append();
		pFace->material=color;
		createFace.NewOutterEdgeLine(pFace,i*4+2,((i+1)%m_nFace)*4+2);
		createFace.NewOutterEdgeLine(pFace,i*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+3);
		createFace.NewOutterEdgeLine(pFace,((i+1)%m_nFace)*4+2);
		vertex1=createFace.VertexAt(i*4+2);
		vertex2=createFace.VertexAt(i*4+3);
		vertex3=createFace.VertexAt(((i+1)%m_nFace)*4+3);
		vec1=(vertex3-vertex2).normalized();
		vec2=(vertex1-vertex2).normalized();
		pFace->poly_norm=vec1^vec2;
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
#endif
}
BOOL CModRod::Create3dSolidModel(CSolidBody *pSolidBody,BOOL bToLds/*=TRUE*/)
{
	if(m_ciRodType==1)
		return Create3dJgSolidModel(pSolidBody,bToLds);
	else if(m_ciRodType==2)
		return Create3dTubeSolidModel(pSolidBody,bToLds);
	else if(m_ciRodType==3)
		return Create3dConeSolidModel(pSolidBody,bToLds);
	else
		return FALSE;
}
BYTE CModRod::GetLegQuad()
{
	if(m_sLayer[0]=='L')
	{
		MOD_LINE line=GetBaseLineToLdsModel();
		GEPOINT ptS(line.startPt);
		GEPOINT ptE(line.endPt);
		if(ptS.x>=0&&ptS.y>=0&&ptE.x>=0&&ptE.y>=0)
			return 1;
		else if(ptS.x<=0&&ptS.y>=0&&ptE.x<=0&&ptE.y>=0)
			return 2;
		else if(ptS.x>=0&&ptS.y<=0&&ptE.x>=0&&ptE.y<=0)
			return 3;
		else if(ptS.x<=0&&ptS.y<=0&&ptE.x<=0&&ptE.y<=0)
			return 4;
		else
			return 0;
	}
	else
		return 0;
}
//////////////////////////////////////////////////////////////////////////
//CModHeightGroup
CModHeightGroup::CModHeightGroup()
{
	m_pBelongModel=NULL;
	m_iBody=0;
	m_iNo=0;
	m_fNamedHeight=0;
	m_dwLegCfgWord.SetWordByNo(1);	//������ĺ�
	for(int i=0;i<4;i++)
		m_arrActiveQuadLegNo[i]=1;
}
BYTE CModHeightGroup::GetLegBitSerialFromSerialId(int serial)
{
	int legBitSerial=0;
	for(int i=1;i<=192;i++)
	{
		if(m_dwLegCfgWord.IsHasNo(i))
		{
			if(legBitSerial==0)
				legBitSerial=1;
		}
		if(legBitSerial==serial)
			return i;
		if(legBitSerial>0)
			legBitSerial++;
		if(legBitSerial>24)
			break;	//һ��������������24�����
	}
	return 0;
}
double CModHeightGroup::GetLegDiffDist(int iLegSerial)
{
	double fDist=0;
	hashSubLegSerial.push_stack();
	for(int* pId=hashSubLegSerial.GetFirst();pId;pId=hashSubLegSerial.GetNext())
	{
		if(*pId==iLegSerial)
		{
			fDist=atof(hashSubLegSerial.GetCursorKey());
			break;
		}
	}
	hashSubLegSerial.pop_stack();
	return fDist;
}
IModTowerInstance* CModHeightGroup::GetTowerInstance(int legSerialQuad1,int legSerialQuad2,int legSerialQuad3,int legSerialQuad4)
{
	union UNIQUEID{
		BYTE bytes[4];
		DWORD id;
	}key;
	key.bytes[0]=GetLegBitSerialFromSerialId(legSerialQuad1);
	key.bytes[1]=GetLegBitSerialFromSerialId(legSerialQuad2);
	key.bytes[2]=GetLegBitSerialFromSerialId(legSerialQuad3);
	key.bytes[3]=GetLegBitSerialFromSerialId(legSerialQuad4);
	//
	int nMaxLegs=MOD_CFGWORD::MaxLegs();
	m_arrActiveQuadLegNo[0]=(m_iNo-1)*nMaxLegs+legSerialQuad1;
	m_arrActiveQuadLegNo[1]=(m_iNo-1)*nMaxLegs+legSerialQuad2;
	m_arrActiveQuadLegNo[2]=(m_iNo-1)*nMaxLegs+legSerialQuad3;
	m_arrActiveQuadLegNo[3]=(m_iNo-1)*nMaxLegs+legSerialQuad4;
	CModTowerInstance* pModTowerInstance=hashTowerInstance.Add(key.id);
	pModTowerInstance->SetBelongHuGao(this);
	if(pModTowerInstance->GetModRodNum()==0)
	{	
		//��ȡ�ڵ㼯��
		for(CModNode* pNode=(CModNode*)m_pBelongModel->EnumFirstNode();pNode;pNode=(CModNode*)m_pBelongModel->EnumNextNode())
		{
			if(pNode->IsLegNode())
			{
				BYTE ciQuad=pNode->GetLegQuad();
				if(ciQuad>=1&&ciQuad<=4&&pNode->m_xCfgword.IsHasNo(m_arrActiveQuadLegNo[ciQuad-1]))
				{
					pModTowerInstance->AppendNode(pNode);
					pModTowerInstance->SetBaseLocation(ciQuad,GEPOINT(pNode->xOrg));
				}
			}
			else if(pNode->m_xCfgword.IsHasBodyNo(m_iNo))
				pModTowerInstance->AppendNode(pNode);
		}
		//��ȡ����װ�伯��
		for(CModRod* pRod=(CModRod*)m_pBelongModel->EnumFirstRod();pRod;pRod=(CModRod*)m_pBelongModel->EnumNextRod())
		{
			if(pRod->IsLegRod())
			{
				BYTE ciQuad=pRod->GetLegQuad();
				if(ciQuad>=1&&ciQuad<=4&&pRod->m_xCfgword.IsHasNo(m_arrActiveQuadLegNo[ciQuad-1]))
					pModTowerInstance->AppendRod(pRod);
			}
			else if(pRod->m_xCfgword.IsHasBodyNo(m_iNo))
				pModTowerInstance->AppendRod(pRod);
		}
	}
	return pModTowerInstance;
}
BOOL CModHeightGroup::IsSonNode(CModNode *pNode)
{
	if(pNode==NULL)
		return FALSE;
	MOD_CFGWORD soncfgword=pNode->m_xCfgword;
	if(pNode->IsLegNode())
	{
		BYTE ciQuad=pNode->GetLegQuad();
		if(ciQuad>=1&&ciQuad<=4&&soncfgword.IsHasNo(m_arrActiveQuadLegNo[ciQuad-1]))
			return TRUE;
		else
			return FALSE;
	}
	else if(soncfgword.IsHasBodyNo(m_iNo))	//�����ͷ���˼�
		return TRUE;
	else
		return FALSE;
}
BOOL CModHeightGroup::IsSonRod(CModRod *pRod)
{
	if(pRod==NULL)
		return FALSE;
	MOD_CFGWORD soncfgword=pRod->m_xCfgword;
	if(pRod->IsLegRod())
	{
		BYTE ciQuad=pRod->GetLegQuad();
		if(ciQuad>=1&&ciQuad<=4&&soncfgword.IsHasNo(m_arrActiveQuadLegNo[ciQuad-1]))
			return TRUE;
		else
			return FALSE;
	}
	else if(soncfgword.IsHasBodyNo(m_iNo))	//�����ͷ���˼�
		return TRUE;
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////////
//
bool CModTowerInstance::ExportModFile(const char* sFileName)
{
	if(BelongHuGao()==NULL||BelongHuGao()->BelongModel()==NULL)
		return false;
	IModModel* pModModel=CModModelFactory::CreateModModel();
	if(pModModel==NULL)
		return false;
	for(IModNode* pSrcNode=EnumModNodeFir();pSrcNode;pSrcNode=EnumModNodeNext())
	{
		IModNode* pModNode=pModModel->AppendNode(pSrcNode->GetId());
		pModNode->Clone(pSrcNode);
	}
	for(IModRod* pSrcRod=EnumModRodFir();pSrcRod;pSrcRod=EnumModRodNext())
	{
		IModRod* pModRod=pModModel->AppendRod(pSrcRod->GetId());
		pModRod->Clone(pSrcRod);
	}
	//��ʼ������������MOD�ṹ
	pModModel->InitSingleModData(BelongHuGao()->GetNamedHeight());
	//��ӹҵ�
	for(int i=0;i<BelongHuGao()->BelongModel()->GetHangNodeNum();i++)
	{
		MOD_HANG_NODE* pSrcHangNode=BelongHuGao()->BelongModel()->GetHangNodeById(i);
		MOD_HANG_NODE* pHangNode=pModModel->AppendHangNode();
		pHangNode->Clone(pSrcHangNode);
	}
	//����Mod�ļ�
	FILE *fp=fopen(sFileName,"wt,ccs=UTF-8");
	if(fp==NULL)
	{
		logerr.Log("%s�ļ���ʧ��!",sFileName);
		return false;
	}
	pModModel->WriteModFileByUtf8(fp);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//CModModel
CModModel::CModModel(long serial/*=0*/)
{
	m_iSerial=serial;
	m_fTowerHeight=0;
	Empty();
}
CModModel::~CModModel()
{

}
char CModModel::QueryBriefMatMark(const char* sMatMark)
{
	char cMat='S';
	if(strstr(sMatMark,"Q345"))
		cMat='H';
	else if(strstr(sMatMark,"Q390"))
		cMat='G';
	else if(strstr(sMatMark,"Q420"))
		cMat='P';
	else if(strstr(sMatMark,"Q460"))
		cMat='T';
	return cMat;
}
void CModModel::QuerySteelMatMark(char cMat,char* sMatMark)
{
	if(cMat=='H')
		strcpy(sMatMark,"Q345");
	else if(cMat=='G')
		strcpy(sMatMark,"Q390");
	else if(cMat=='P')
		strcpy(sMatMark,"Q420");
	else if(cMat=='T')
		strcpy(sMatMark,"Q460");
	else
		strcpy(sMatMark,"Q235");
}
void CModModel::Empty()
{
	m_listLegItem.Empty();
	m_listBodyItem.Empty();
	m_listGuaNode.Empty();
	ModHeightGroup.Empty();
	ModRods.Empty();
	ModNodes.Empty();
}
CLegItem* CModModel::FindLegItem(int module_no)
{
	CLegItem* pLegItem=NULL;
	for(pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
	{
		if(pLegItem->m_iLegNo==module_no)
			break;
	}
	return pLegItem;
}
IModHeightGroup* CModModel::GetHeightGroup(int iNo)
{
	CModHeightGroup* pHeightGroup=ModHeightGroup.GetValue(iNo);
	if(pHeightGroup)
		pHeightGroup->SetBelongModel(this);
	return pHeightGroup;
}
IModHeightGroup* CModModel::GetHeightGroup(const char* sName)
{
	CModHeightGroup* pHeightGroup=NULL;
	for(pHeightGroup=ModHeightGroup.GetFirst();pHeightGroup;pHeightGroup=ModHeightGroup.GetNext())
	{
		if(pHeightGroup->m_sHeightName.EqualNoCase(sName))
			break;
	}
	return pHeightGroup;
}
MOD_CS CModModel::BuildUcsByModCS()
{
	MOD_CS ucs;
	ucs.origin.Set(0,0,m_fTowerHeight);
	ucs.axisX.Set(1,0,0);
	ucs.axisY.Set(0,-1,0);
	ucs.axisZ.Set(0,0,-1);
	return ucs;
}
void CModModel::AmendModData()
{
	UCS_STRU ucs=TransToUcs(BuildUcsByModCS());
	for(CModNode* pNode=ModNodes.GetFirst();pNode;pNode=ModNodes.GetNext())
	{
		pNode->xLdsOrg=pNode->xOrg;
		coord_trans(pNode->xLdsOrg,ucs,FALSE);
	}
	//��ʼ������
	for(CModHeightGroup* pHeightGroup=ModHeightGroup.GetFirst();pHeightGroup;pHeightGroup=ModHeightGroup.GetNext())
	{
		if(pHeightGroup->m_fNamedHeight>0)
		{
			double height=fto_halfi(pHeightGroup->m_fNamedHeight*0.001);
			CXhChar50 name(height);
			name.Append('m');
			strcpy(pHeightGroup->m_sHeightName,name);
		}
		else
			sprintf(pHeightGroup->m_sHeightName,"����%d",pHeightGroup->m_iNo);
	}
	//��ʼ��������ĺ�
	int nMaxLegsPerBody=0;
	for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
	{
		int nLeg=pLegItem->subLegInfoList.GetNodeNum();
		if(nMaxLegsPerBody<nLeg)
			nMaxLegsPerBody=nLeg;
	}
	if(nMaxLegsPerBody<=8&&MOD_CFGWORD::MaxLegs()!=8)
		MOD_CFGWORD::SetSchema(MOD_CFGWORD::MULTILEG_MAX08);
	else if(nMaxLegsPerBody> 8&&nMaxLegsPerBody<=16&&MOD_CFGWORD::MaxLegs()!=16)
		MOD_CFGWORD::SetSchema(MOD_CFGWORD::MULTILEG_MAX16);
	else if(nMaxLegsPerBody>16&&nMaxLegsPerBody<=24&&MOD_CFGWORD::MaxLegs()!=24)
		MOD_CFGWORD::SetSchema(MOD_CFGWORD::MULTILEG_MAX24);
	const DWORD flagConstArr[24]={
		0x000001,0x000002,0x000004,0x000008,0x000010,0x000020,0x000040,0x000080,
		0x000100,0x000200,0x000400,0x000800,0x001000,0x002000,0x004000,0x008000,
		0x010000,0x020000,0x040000,0x080000,0x100000,0x200000,0x400000,0x800000	};
	for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
	{
		CModHeightGroup* pHeightGroup=ModHeightGroup.GetValue(pLegItem->m_iLegNo);
		if(pHeightGroup==NULL)
			continue;
		pHeightGroup->m_dwLegCfgWord.Clear();
		//���߽���
		if(pLegItem->subLegInfoList.GetNodeNum()>0)
		{	//��������
			pHeightGroup->m_dwLegCfgWord.AddBodyLegs(pHeightGroup->m_iNo,1);
			int legword=0;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				MOD_CFGWORD cfgword;
				cfgword.AddBodyLegs(pHeightGroup->m_iNo,flagConstArr[legword]);
				pHeightGroup->m_dwLegCfgWord.AddSpecWord(cfgword);
				for(CModNode* pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
				{
					pNode->m_xCfgword.AddBodyLegs(pHeightGroup->m_iNo,flagConstArr[legword]);
					pNode->m_sLayer[0]='L';
				}
				for(CModRod* pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					pRod->m_xCfgword.AddBodyLegs(pHeightGroup->m_iNo,flagConstArr[legword]);
					pRod->m_sLayer[0]='L';
				}
				legword++;
				//���ݼ��ȸ߶ȼ�¼�������
				double height=fto_halfi(pSubLeg->m_fLegH*0.001);
				pHeightGroup->hashSubLegSerial.SetValue(CXhChar50(height),legword);
			}
			//���߶�����
			for(CModNode* pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				pNode->m_xCfgword.SetBodyLegs(pHeightGroup->m_iNo);
				pNode->m_sLayer[0]='S';
			}
			for(CModRod* pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				pRod->m_xCfgword.SetBodyLegs(pHeightGroup->m_iNo);
				pRod->m_sLayer[0]='S';
			}
		}
		else
		{	//ƽ�����
			pHeightGroup->m_dwLegCfgWord.AddBodyLegs(pHeightGroup->m_iNo,flagConstArr[0]);
			for(CModNode* pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				pNode->m_xCfgword.AddBodyLegs(pHeightGroup->m_iNo,flagConstArr[0]);
				pNode->m_sLayer[0]='L';
			}
			for(CModRod* pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				pRod->m_xCfgword.AddBodyLegs(pHeightGroup->m_iNo,flagConstArr[0]);
				pRod->m_sLayer[0]='L';
			}
			double height=fto_halfi(pLegItem->m_fSegmentH*0.001);
			pHeightGroup->hashSubLegSerial.SetValue(CXhChar50(height),1);
		}
	}
	//����(���߹����)
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		MOD_CFGWORD cfgword;
		for(int iNo=pBodyItem->m_iLegS;iNo<=m_listLegItem.GetNodeNum();iNo++)
			cfgword.AddBodyLegs(iNo);
		for(CModNode* pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
		{
			pNode->m_xCfgword=cfgword;
			pNode->m_sLayer[0]='B';
		}
		for(CModRod* pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			pRod->m_xCfgword=cfgword;
			pRod->m_sLayer[0]='B';
		}
	}
	//�����ҵ���Ϣ
	for(MOD_HANG_NODE* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
	{
		char* pszWireType=strstr(pGuaInfo->m_sHangName,"��");
		if(pszWireType==NULL)
			pszWireType=strstr(pGuaInfo->m_sHangName,"��");
		if(pszWireType==NULL)
			pszWireType=strstr(pGuaInfo->m_sHangName,"��");
		if(pszWireType!=NULL)
		{
			pszWireType+=2;
			char* pchSpliter=strchr(pszWireType,'-');
			if(pchSpliter)
				*pchSpliter=0;
			int iPhaseSerial=atoi(pszWireType);
			if(pGuaInfo->m_ciWireType!='E'&&iPhaseSerial>0)
			{
				iPhaseSerial-=1;
				pGuaInfo->m_ciLoopSerial=1+(iPhaseSerial/3); 
				pGuaInfo->m_ciPhaseSerial=1+iPhaseSerial%3;
			}
			else
				pGuaInfo->m_ciPhaseSerial=iPhaseSerial;
			pGuaInfo->m_ciHangingStyle=0;
			pGuaInfo->m_ciHangOrder=0;
			if(pchSpliter && pGuaInfo->m_ciWireType!='T')
			{	//��ȡ�Ҵ�����
				pchSpliter++;
				if(*pchSpliter=='S'||*pchSpliter=='s')
					pGuaInfo->m_ciHangingStyle='S';
				else if(*pchSpliter=='V'||*pchSpliter=='v')
					pGuaInfo->m_ciHangingStyle='V';
			}
			else if(pchSpliter && pGuaInfo->m_ciWireType=='T')
			{	//��ȡ���߸�����
				pchSpliter++;
				pGuaInfo->m_ciHangOrder=atoi(pchSpliter);
			}
			if(pGuaInfo->m_ciHangingStyle!=0)
			{	//��ȡ��|���߸�����
				pchSpliter++;
				pGuaInfo->m_ciHangOrder=atoi(pchSpliter);
			}
		}
	}
}
void CModModel::ReadModFile(FILE* fp,BOOL bUtf8/*=FALSE*/)
{
	Empty();
	m_fTowerHeight=0;
	int iLegNo=0;
	BYTE ciReadType=0;	//0:����|1.����|2.����
	CModHeightGroup* pModuleItem=NULL;
	CBodyItem* pBodyItem=NULL;
	CLegItem* pLegItem=NULL;
	SUB_LEG_INFO* pSubLeg=NULL;
	CXhChar100 line_txt,sText,key_word;
	char sLine1[MAX_PATH]="",sLine2[MAX_PATH]="";
	while(!feof(fp))
	{
		if(fgets(sLine1,MAX_PATH,fp)==NULL)
			break;
		if(bUtf8)
			UTF8ToANSI(sLine1,sLine2);
		else
			strcpy(sLine2,sLine1);
		line_txt.Copy(sLine2);
		line_txt.Replace('\t',' ');
		line_txt.Remove(' ');
		strcpy(sText,line_txt);
		char *skey=strtok(sText,",");
		strncpy(key_word,skey,100);
		if(strstr(key_word,"HBody")||strstr(key_word,"HLeg")||strstr(key_word,"HNum"))
			continue;
		if(strstr(key_word,"HSubLeg")&&pSubLeg)
		{
			skey=strtok(NULL,",");
			pSubLeg->m_fLegH=(skey!=NULL)?atof(skey):0;
			continue;
		}
		if(stricmp(key_word,"H")==0)
		{
			skey=strtok(NULL,",");
			double height=skey!=NULL?atof(skey):0;
			if(skey!=NULL)	//�����Խӱ���
				skey=strtok(NULL,",");
			if(skey!=NULL)	//�����Խӽ��ȣ������飩
			{
				UINT idBody=0,iLeg=0;
				CXhChar50 itemstr;
				itemstr.NCopy(&skey,4,true);
				if(itemstr.EqualNoCase("Body"))
					idBody=atoi(skey);
				skey=strtok(NULL,",");
				if(skey!=NULL)
				{
					itemstr.NCopy(&skey,3,true);
					if(itemstr.EqualNoCase("Leg"))
						iLeg=atoi(skey);
					pModuleItem=ModHeightGroup.Add(iLeg);
					pModuleItem->SetBelongModel(this);
					pModuleItem->m_fNamedHeight=height;
					pModuleItem->m_iBody=idBody;
				}
			}
			continue;
		}
		else if(strstr(key_word,"Body"))
		{	//����
			ciReadType=0;
			pBodyItem=m_listBodyItem.append();
		}
		else if(strstr(key_word,"Leg")&&key_word[0]=='L')
		{	//����
			ciReadType=1;
			iLegNo+=1;
			pLegItem=m_listLegItem.append();
			pLegItem->m_iLegNo=iLegNo;
			if(pBodyItem->m_iLegS==0)
				pBodyItem->m_iLegS=iLegNo;
		}
		else if(strstr(key_word,"SubLeg"))
		{	//����
			ciReadType=2;
			pSubLeg=pLegItem->subLegInfoList.append();
		}
		else
		{
			if(stricmp(key_word,"G")!=0)
				line_txt.Replace(',',' ');
			key_word.Remove(' ');
			if(stricmp(key_word,"P")==0)
			{	//�ڵ�
				f3dPoint pos;
				sscanf(line_txt,"%s%d%lf%lf%lf",(char*)key_word,&pos.feature,&pos.x,&pos.y,&pos.z);
				if(pos.z>m_fTowerHeight)
					m_fTowerHeight=pos.z;
				CModNode* pNode=ModNodes.Add(pos.feature);
				pNode->SetBelongModel(this);
				pNode->m_xCfgword.Clear();
				pNode->handle=pos.feature;
				pNode->xOrg=pos;
				if(ciReadType==0 && pBodyItem)
					pBodyItem->bodyNodeSet.append(pNode);
				else if(ciReadType==1 && pLegItem)
					pLegItem->segmentNodeSet.append(pNode);
				else if(ciReadType==2 && pSubLeg)
					pSubLeg->legNodeSet.append(pNode);
			}
			else if(stricmp(key_word,"R")==0)
			{	//�˼�
				int indexS=0,indexE=0;
				double fWidth=0,fThick=0;
				char sMat[16]="",sSpec[16]="";
				CXhChar100 sTempLine(line_txt);
				sscanf(sTempLine,"%s%d%d%s%s",(char*)key_word,&indexS,&indexE,sSpec,sMat);
				CModNode* pNodeS=ModNodes.GetValue(indexS);
				CModNode* pNodeE=ModNodes.GetValue(indexE);
				if(pNodeS==NULL || pNodeE==NULL)
				{
					logerr.Log("Mod�˼���������(ʼ�˵��%d,�ն˵��%d)!",indexS,indexE);
					continue;
				}
				if(strstr(line_txt,"L"))
				{	//�Ǹ�
					f3dPoint ptX,ptY;
					sscanf(line_txt,"%s%d%d%s%s%lf%lf%lf%lf%lf%lf",(char*)key_word,&indexS,&indexE,sSpec,sMat,&ptX.x,&ptX.y,&ptX.z,&ptY.x,&ptY.y,&ptY.z);
					GEPOINT axis_z=(f3dPoint(pNodeE->xOrg)-f3dPoint(pNodeS->xOrg)).normalized();
					char mark,symbol;
					sscanf(sSpec,"%c%lf%c%lf",&symbol,&fWidth,&mark,&fThick);
					//
					CModRod* pRod=ModRods.Add(0);
					pRod->SetBelongModel(this);
					pRod->m_xCfgword.Clear();
					pRod->m_ciRodType=1;
					pRod->m_uiNodeS=indexS;
					pRod->m_uiNodeE=indexE;
					pRod->m_cMaterial=CModModel::QueryBriefMatMark(sMat);
					pRod->m_fWidth=fWidth;
					pRod->m_fThick=fThick;
					pRod->base_line.startPt=pNodeS->xOrg;
					pRod->base_line.endPt=pNodeE->xOrg;
					pRod->m_vWingX=ptX;	//X֫����
					pRod->m_vWingY=ptY;	//Y֫����
					RotateVectorAroundVector(ptX,-1,0,axis_z);
					RotateVectorAroundVector(ptY, 1,0,axis_z);
					pRod->m_vNormX=ptX;	//X֫����
					pRod->m_vNormY=ptY;	//Y֫����
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pRod);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pRod);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pRod);
				}
				else if(strstr(line_txt,"��")||strstr(line_txt,"��"))
				{	//�ֹ�
					sscanf(line_txt,"%s%d%d%s",(char*)key_word,&indexS,&indexE,sSpec);
					CModNode* pNodeS=ModNodes.GetValue(indexS);
					CModNode* pNodeE=ModNodes.GetValue(indexE);
					if(pNodeS==NULL || pNodeE==NULL)
					{
						logerr.Log("Mod�˼���������(ʼ�˵��%d,�ն˵��%d)!",indexS,indexE);
						continue;
					}
					char mark,symbol;
					CXhChar16 ss(sSpec);
					ss.Replace("��","G");
					sscanf(ss,"%c%lf%c%lf",&symbol,&fWidth,&mark,&fThick);
					//
					CModRod* pRod=ModRods.Add(0);
					pRod->SetBelongModel(this);
					pRod->m_xCfgword.Clear();
					pRod->m_ciRodType=2;
					pRod->m_uiNodeS=indexS;
					pRod->m_uiNodeE=indexE;
					pRod->m_fWidth=(float)fWidth;
					pRod->m_fThick=(float)fThick;
					pRod->base_line.startPt=pNodeS->xOrg;
					pRod->base_line.endPt=pNodeE->xOrg;
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pRod);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pRod);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pRod);
				}
				else if(strlen(sMat)>0)
				{	//
					int nDiameterS=0,nDiameterE,nThick=0,nFaceNum=0;
					sscanf(line_txt,"%s%d%d%s%d%d%d%d",(char*)key_word,&indexS,&indexE,sMat,&nDiameterS,&nDiameterE,&nThick,&nFaceNum);
					//
					CModRod* pRod=ModRods.Add(0);
					pRod->SetBelongModel(this);
					pRod->m_xCfgword.Clear();
					pRod->m_ciRodType=3;
					pRod->m_uiNodeS=indexS;
					pRod->m_uiNodeE=indexE;
					pRod->m_fWidth=nDiameterS;
					pRod->m_fWidth2=nDiameterE;
					pRod->m_fThick=nThick;
					pRod->m_nFace=nFaceNum;
					pRod->base_line.startPt=pNodeS->xOrg;
					pRod->base_line.endPt=pNodeE->xOrg;
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pRod);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pRod);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pRod);
				}
				else
				{
					logerr.Log("Mod�˼���������(ʼ�˵��%d,�ն˵��%d)!",indexS,indexE);
					continue;
				}
			}
			else if(stricmp(key_word,"G")==0)
			{	//�ҵ�
				CXhChar16 sType;
				MOD_HANG_NODE* pGuaInfo=m_listGuaNode.append();
				CXhChar200 szBackupText=line_txt;
				char* pszKey=strtok(line_txt,", ");
				int indexOfPrevKeyAddr=0;
				for(int i=0;pszKey!=NULL&&i<6;i++)
				{
					int indexOfCurrKey=(int)(pszKey-(char*)line_txt);
					for(int j=indexOfPrevKeyAddr;j<indexOfCurrKey-1;j++)
					{
						if(szBackupText[j]==',')
							i++;
					}
					indexOfPrevKeyAddr=indexOfCurrKey;
					if(i==1)	//��������
						pGuaInfo->m_ciWireType=*pszKey;
					else if(i==2)	//�ҵ�����
						StrCopy(pGuaInfo->m_sHangName,pszKey,50);
					else if(i==3)
						pGuaInfo->m_xHangPos.x=atof(pszKey);
					else if(i==4)
						pGuaInfo->m_xHangPos.y=atof(pszKey);
					else if(i==5)
						pGuaInfo->m_xHangPos.z=atof(pszKey);
					pszKey=strtok(NULL,", ");
				}
			}
		}
	}
	fclose(fp);
	AmendModData();
}
BOOL CModModel::ImportModFile(const char* sModFile)
{
	BOOL bUtf8=IsUTF8File(sModFile);
	FILE *fp = fopen(sModFile,"rt");
	if(fp==NULL)
		return FALSE;
	ReadModFile(fp,bUtf8);
	return TRUE;
}
void CModModel::InitSingleModData(double fNameH)
{
	//��Ӻ���
	CModHeightGroup* pHuGao=(CModHeightGroup*)AppendHeightGroup(1);
	pHuGao->SetBelongModel(this);
	pHuGao->m_fNamedHeight=fNameH;
	pHuGao->m_iBody=1;
	pHuGao->m_iNo=1;
	//���Body
	CBodyItem* pBodyItem=m_listBodyItem.append();
	CLegItem* pLegItem=m_listLegItem.append();
	SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.append();
	//��ʼ�������ֵĽڵ㼰�˼�
	double fMinBodyZ=100000,fMaxBodyZ=0;
	double fMinLegZ=100000,fMaxLegZ=0;
	double fMinSegZ=100000,fMaxSegZ=0;
	for(CModNode* pNode=EnumFirstNode();pNode;pNode=EnumNextNode())
	{
		if(pNode->IsLegNode())
		{
			pSubLeg->legNodeSet.append(pNode);
			if(fMinLegZ>pNode->xOrg.z)
				fMinLegZ=pNode->xOrg.z;
			if(fMaxLegZ<pNode->xOrg.z)
				fMaxLegZ=pNode->xOrg.z;
		}
		else if(pNode->m_sLayer[0]=='S')
		{	
			pLegItem->segmentNodeSet.append(pNode);
			if(fMinSegZ>pNode->xOrg.z)
				fMinSegZ=pNode->xOrg.z;
			if(fMaxSegZ<pNode->xOrg.z)
				fMaxSegZ=pNode->xOrg.z;
		}
		else if(pNode->m_sLayer[0]=='B')
		{
			pBodyItem->bodyNodeSet.append(pNode);
			if(fMinBodyZ>pNode->xOrg.z)
				fMinBodyZ=pNode->xOrg.z;
			if(fMaxBodyZ<pNode->xOrg.z)
				fMaxBodyZ=pNode->xOrg.z;
		}
	}
	for(CModRod* pRod=EnumFirstRod();pRod;pRod=EnumNextRod())
	{
		if(pRod->IsLegRod())
			pSubLeg->legRodSet.append(pRod);
		else if(pRod->m_sLayer[0]=='S')
			pLegItem->segmentRodSet.append(pRod);
		else if(pRod->m_sLayer[0]=='B')
			pBodyItem->bodyRodSet.append(pRod);
	}
	pBodyItem->m_fBodyH=fMaxBodyZ-fMinBodyZ;	//����߶�
	pLegItem->m_fSegmentH=fMaxSegZ-fMinSegZ;	//���ɸ߶�
	pLegItem->m_fMaxLegH=fMaxBodyZ-fMinLegZ;	//��ȸ�
	pSubLeg->m_fLegH=0;
}
void CModModel::InitMultiModData()
{
	//��ȡ�������߶��еķֶκͽ�����Ϣ
	ARRAY_LIST<int> bodyNoArr;
	for(CModHeightGroup* pHuGao=ModHeightGroup.GetFirst();pHuGao;pHuGao=ModHeightGroup.GetNext())
	{
		CLegItem* pLegItem=m_listLegItem.append();
		pLegItem->InitLegItemByModule(pHuGao);
		bodyNoArr.append(pLegItem->m_iLegNo);
	}
	//��ȡ������Ϣ
	int nNum=bodyNoArr.GetSize(),iMaxIndex=0;
	CModNode* pTagNode=NULL;
	NODESET validBodyNodeSet;
	RODSET validBodyRodSet;
	for(CModNode* pNode=EnumFirstNode();pNode;pNode=EnumNextNode())
	{
		if(pNode->handle>iMaxIndex)
			iMaxIndex=pNode->handle;
		if(!pNode->IsLegNode())
			validBodyNodeSet.append(pNode);
	}
	for(CModRod* pRod=EnumFirstRod();pRod;pRod=EnumNextRod())
	{
		if(!pRod->IsLegRod())
			validBodyRodSet.append(pRod);
	}
	for(CModNode* pNode=validBodyNodeSet.GetFirst();pNode;pNode=validBodyNodeSet.GetNext())
	{
		if(pTagNode==NULL||pTagNode->xLdsOrg.z>pNode->xLdsOrg.z)
			pTagNode=pNode;
	}
	if(nNum==1)
	{	//ֻ��һ������
		CBodyItem* pBodyItem=m_listBodyItem.append();
		pBodyItem->InitBodyItem(pTagNode,validBodyNodeSet,validBodyRodSet);
		//LEG���ֿ�Ϊ��
		CLegItem* pLegItem=m_listLegItem.GetFirst();
		pLegItem->segmentNodeSet.Empty();
		pLegItem->segmentRodSet.Empty();
		pLegItem->m_fSegmentH=0;
		pLegItem->m_hTagNode=pTagNode?pTagNode->handle:0;
	}
	else
	{	//�����
		for(int i=0;i<nNum;i++)
		{
			NODESET selNodeSet;
			RODSET selRodSet;
			if(RetrieveBodyInfo(bodyNoArr.m_pData,i,nNum,validBodyNodeSet,selNodeSet,validBodyRodSet,selRodSet))
			{
				CBodyItem* pBodyItem=m_listBodyItem.append();
				pBodyItem->InitBodyItem(pTagNode,selNodeSet,selRodSet);
				//
				CLegItem* pLegItem=FindLegItem(bodyNoArr[i]);
				pLegItem->m_hTagNode=pTagNode->handle;
				if(pLegItem->m_fSegmentH>0)
				{
					pLegItem->m_fMaxLegH-=pLegItem->m_fSegmentH;
					pLegItem->m_fSegmentH-=pTagNode->xLdsOrg.z;
				}
				else
					pLegItem->m_fMaxLegH-=pTagNode->xLdsOrg.z;
				//
				for(CModNode* pNode=selNodeSet.GetFirst();pNode;pNode=selNodeSet.GetNext())
					validBodyNodeSet.DeleteNode(pNode->handle);
				for(CModRod* pRod=selRodSet.GetFirst();pRod;pRod=selRodSet.GetNext())
					validBodyRodSet.DeleteNode(pRod->handle);
			}
			else if(pTagNode)
			{
				CLegItem* pLegItem=FindLegItem(bodyNoArr[i]);
				pLegItem->m_hTagNode=pTagNode->handle;
				if(pLegItem->m_fSegmentH>0)
				{
					pLegItem->m_fMaxLegH-=pLegItem->m_fSegmentH;
					pLegItem->m_fSegmentH-=pTagNode->xLdsOrg.z;
				}
				else
					pLegItem->m_fMaxLegH-=pTagNode->xLdsOrg.z;
			}
		}
	}
	//��ȡ���߷�����Ϣ
	int body_index=1,leg_index=1;
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
		{
			if(pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			CModHeightGroup* pModuleItem=(CModHeightGroup*)GetHeightGroup(leg_index);
			if(pModuleItem)
			{
				pModuleItem->m_iBody=body_index;
				//�������ڶ��Ǹ���mod�ļ��еĺ��������ֱ�ӻ�ȡ����Ȼ���Ͻ��������ݳ�ͻ������ʱ�淶������ wjh-2019.4.29
				//pModuleItem->m_fNamedHeight=pLegItem->m_maxLegPt.z-m_xMaxWireNodePos.z;
			}
			leg_index++;
		}
		body_index++;
	}
	//���ݵ���Ĺ淶Ҫ��,��鲢�޸�subleg���Ǵ��ڿ�εĸ˼�
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
		{		
			if(pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			CHashList<int> hashNewAddPoint;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				//���ҵ��³��ֿ�θ˼��Ķ˵�,������п����滻
				for(CModRod* pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					CModNode* pNodeS=FindNode(pRod->m_uiNodeS);
					CModNode* pNodeE=FindNode(pRod->m_uiNodeE);
					if(pNodeS==NULL||pNodeE==NULL)
						continue;
					if(pNodeS->m_sLayer[0]=='B')
					{	//ʼ�˽ڵ�λ������
						int *pNewNodeI=hashNewAddPoint.GetValue(pNodeS->handle);
						if(pNewNodeI==NULL)
						{	//����µĽڵ㣬���浽Leg
							iMaxIndex+=1;
							CModNode* pNewNode=AppendNode(iMaxIndex);
							pNewNode->Clone(pNodeS);
							pNewNode->SetLayer('S');
							pLegItem->segmentNodeSet.append(pNewNode);
							//
							pNewNodeI=hashNewAddPoint.SetValue(pNodeS->handle,pNewNode->handle);
						}
						pRod->m_uiNodeS=*pNewNodeI;
					}
					if(pNodeE->m_sLayer[0]=='B')
					{	//�ն˽ڵ�λ������
						int *pNewNodeI=hashNewAddPoint.GetValue(pNodeE->handle);
						if(pNewNodeI==NULL)
						{	//����µĽڵ㣬���浽Leg
							iMaxIndex+=1;
							CModNode* pNewNode=AppendNode(iMaxIndex);
							pNewNode->Clone(pNodeE);
							pNewNode->SetLayer('S');
							pLegItem->segmentNodeSet.append(pNewNode);
							//
							pNewNodeI=hashNewAddPoint.SetValue(pNodeE->handle,pNewNode->handle);
						}
						pRod->m_uiNodeE=*pNewNodeI;
					}
				}
			}
		}
	}
}
void CModModel::WriteModFileByUtf8(FILE* fp)
{
	if(fp==NULL)
		return;
	CModNode* pNode=NULL;
	CModRod* pRod=NULL;
	//���������Ϣ
	fwprintf(fp,L"HNum,%d\n",ModHeightGroup.GetNodeNum());
	for(CModHeightGroup* pModuleItem=ModHeightGroup.GetFirst();pModuleItem;pModuleItem=ModHeightGroup.GetNext())
		fwprintf(fp,L"H,%.0f,Body%d,Leg%d\n",pModuleItem->m_fNamedHeight,pModuleItem->m_iBody,pModuleItem->m_iNo);
	//����౾��������Ϣ
	int body_index=1,leg_index=1;
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		//������Ϣ
		fwprintf(fp,L"Body%d\n",body_index);
		fwprintf(fp,L"HBody%d,%.0f\n",body_index,pBodyItem->m_fBodyH);
		for(pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
			fwprintf(fp,L"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
		for(pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
			CXhChar16 sMat,sSpec;
			CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
			ANSIToUnicode(sMat,sWMat);
			if(pRod->m_ciRodType==1)
			{
				sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
				ANSIToUnicode(sSpec,sWSpec);
				fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat,
					pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
					pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
			}
			else if(pRod->m_ciRodType==2)
			{
				sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
				ANSIToUnicode(sSpec,sWSpec);
				fwprintf(fp,L"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat);
			}
			else if(pRod->m_ciRodType==3)
			{
				ANSIToUnicode(sSpec,sWSpec);
				fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat,pRod->m_fWidth,pRod->m_fWidth2,
					pRod->m_fThick,pRod->m_nFace);
			}
			else
				fwprintf(fp,L"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
		}
		//�ñ����µĽ�����Ϣ
		for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
		{
			if(pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			fwprintf(fp,L"Leg%d\n",leg_index);
			fwprintf(fp,L"HLeg%d,%.0f,%.0f\n",leg_index,pLegItem->m_fSegmentH,pLegItem->m_fMaxLegH);
			for(pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
				fwprintf(fp,L"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
			for(pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
				CXhChar16 sMat,sSpec;
				CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
				ANSIToUnicode(sMat,sWMat);
				if(pRod->m_ciRodType==1)
				{
					sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);	
					ANSIToUnicode(sSpec,sWSpec);
					fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat,
						pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
						pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
				}
				else if(pRod->m_ciRodType==2)
				{
					sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
					ANSIToUnicode(sSpec,sWSpec);
					fwprintf(fp,L"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat);
				}
				else if(pRod->m_ciRodType==3)
				{
					ANSIToUnicode(sSpec,sWSpec);
					fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat,pRod->m_fWidth,pRod->m_fWidth2,
						pRod->m_fThick,pRod->m_nFace);
				}
				else
					fwprintf(fp,L"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
			}
			//��������Ϣ
			int sub_leg_index=1;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				fwprintf(fp,L"SubLeg%d\n",sub_leg_index);
				fwprintf(fp,L"HSubLeg%d,%.0f\n",sub_leg_index,pSubLeg->m_fLegH);
				for(pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
					fwprintf(fp,L"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
				for(pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
					CXhChar16 sMat,sSpec;
					CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
					ANSIToUnicode(sMat,sWMat);
					if(pRod->m_ciRodType==1)
					{
						sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
						ANSIToUnicode(sSpec,sWSpec);
						fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat,
							pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
							pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
					}
					else if(pRod->m_ciRodType==2)
					{
						sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
						ANSIToUnicode(sSpec,sWSpec);
						fwprintf(fp,L"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat);
					}
					else if(pRod->m_ciRodType==3)
					{
						ANSIToUnicode(sSpec,sWSpec);
						fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sWSpec,sWMat,pRod->m_fWidth,pRod->m_fWidth2,
							pRod->m_fThick,pRod->m_nFace);
					}
					else
						fwprintf(fp,L"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
				}
				sub_leg_index++;
			}
			leg_index++;
		}
		body_index++;
	}
	//����ҵ���Ϣ
	for(MOD_HANG_NODE* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
	{
		wchar_t sWText[MAX_PATH];
		ANSIToUnicode(pGuaInfo->m_sHangName,sWText);
		fwprintf(fp,L"G,%c,%s,%f,%f,%f\n",pGuaInfo->m_ciWireType,sWText,pGuaInfo->m_xHangPos.x,pGuaInfo->m_xHangPos.y,pGuaInfo->m_xHangPos.z);
	}
	fclose(fp);
}
void CModModel::WriteModFileByAnsi(FILE* fp)
{
	if(fp==NULL)
		return;
	CModNode* pNode=NULL;
	CModRod* pRod=NULL;
	//���������Ϣ
	fprintf(fp,"HNum,%d\n",ModHeightGroup.GetNodeNum());
	for(CModHeightGroup* pModuleItem=ModHeightGroup.GetFirst();pModuleItem;pModuleItem=ModHeightGroup.GetNext())
		fprintf(fp,"H,%.2f,Body%d,Leg%d\n",pModuleItem->m_fNamedHeight,pModuleItem->m_iBody,pModuleItem->m_iNo);
	//����౾��������Ϣ
	int body_index=1,leg_index=1;
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		//������Ϣ
		fprintf(fp,"Body%d\n",body_index);
		fprintf(fp,"HBody%d,%.0f\n",body_index,pBodyItem->m_fBodyH);
		for(pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
			fprintf(fp,"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
		for(pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			CXhChar16 sMat,sSpec;
			CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
			if(pRod->m_ciRodType==1)
			{
				sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
				fprintf(fp,"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat,
					pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
					pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
			}
			else if(pRod->m_ciRodType==2)
			{
				sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
				fprintf(fp,"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat);
			}
			else if(pRod->m_ciRodType==3)
			{
				fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sMat,pRod->m_fWidth,
						pRod->m_fWidth2,pRod->m_fThick,pRod->m_nFace);
			}
			else
				fprintf(fp,"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
		}
		//�ñ����µĽ�����Ϣ
		for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
		{
			if(pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			fprintf(fp,"Leg%d\n",leg_index);
			fprintf(fp,"HLeg%d,%s,%s\n",leg_index,(char*)CXhChar16(pLegItem->m_fSegmentH),(char*)CXhChar16(pLegItem->m_fMaxLegH));
			for(pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
				fprintf(fp,"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
			for(pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				CXhChar16 sMat,sSpec;
				CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
				if(pRod->m_ciRodType==1)
				{
					sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fWidth);
					fprintf(fp,"R,%d,%d,%s,%s,%3f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat,
						pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
						pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
				}
				else if(pRod->m_ciRodType==2)
				{
					sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fWidth);
					fprintf(fp,"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat);
				}
				else if(pRod->m_ciRodType==3)
				{
					fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sMat,pRod->m_fWidth,
						pRod->m_fWidth2,pRod->m_fThick,pRod->m_nFace);
				}
				else
					fprintf(fp,"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
			}
			//��������Ϣ
			int sub_leg_index=1;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				fprintf(fp,"SubLeg%d\n",sub_leg_index);
				fprintf(fp,"HSubLeg%d,%s\n",sub_leg_index,(char*)CXhChar16(pSubLeg->m_fLegH));
				for(pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
					fprintf(fp,"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
				for(pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					CXhChar16 sMat,sSpec;
					CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
					if(pRod->m_ciRodType==1)
					{
						sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
						fprintf(fp,"R,%d,%d,%s,%s,%3f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat,
							pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
							pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
					}
					else if(pRod->m_ciRodType==2)
					{
						sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
						fprintf(fp,"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat);
					}
					else if(pRod->m_ciRodType==3)
					{
						fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE,sMat,pRod->m_fWidth,
							pRod->m_fWidth2,pRod->m_fThick,pRod->m_nFace);
					}
					else
						fprintf(fp,"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
				}
				sub_leg_index++;
			}
			leg_index++;
		}
		body_index++;
	}
	//����ҵ���Ϣ
	for(MOD_HANG_NODE* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
		fprintf(fp,"G,%c,%s,%f,%f,%f\n",pGuaInfo->m_ciWireType,pGuaInfo->m_sHangName,pGuaInfo->m_xHangPos.x,pGuaInfo->m_xHangPos.y,pGuaInfo->m_xHangPos.z);
	fclose(fp);
}
void CModModel::WriteModFile(FILE* pFile,BOOL bUtf8/*=FALSE*/)
{
	FILE* fp=fopen("C:\\test.txt","wt");
	if(fp==NULL)
		return;
	CModNode* pNode=NULL;
	CModRod* pRod=NULL;
	//���������Ϣ
	fprintf(fp,"HNum,%d\n",ModHeightGroup.GetNodeNum());
	for(CModHeightGroup* pModuleItem=ModHeightGroup.GetFirst();pModuleItem;pModuleItem=ModHeightGroup.GetNext())
		fprintf(fp,"H,%.2f,Body%d,Leg%d\n",pModuleItem->m_fNamedHeight,pModuleItem->m_iBody,pModuleItem->m_iNo);
	//����౾��������Ϣ
	int body_index=1,leg_index=1;
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		//������Ϣ
		fprintf(fp,"Body%d\n",body_index);
		fprintf(fp,"HBody%d,%.0f\n",body_index,pBodyItem->m_fBodyH);
		for(pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
			fprintf(fp,"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
		for(pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			CXhChar16 sMat,sSpec;
			CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
			if(pRod->m_ciRodType==1)
			{
				sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
				fprintf(fp,"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat,
					pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
					pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
			}
			else if(pRod->m_ciRodType==2)
			{
				sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
				fprintf(fp,"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat);
			}
			else
				fprintf(fp,"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
		}
		//�ñ����µĽ�����Ϣ
		for(CLegItem* pLegItem=m_listLegItem.GetFirst();pLegItem;pLegItem=m_listLegItem.GetNext())
		{
			if(pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			fprintf(fp,"Leg%d\n",leg_index);
			fprintf(fp,"HLeg%d,%s,%s\n",leg_index,(char*)CXhChar16(pLegItem->m_fSegmentH),(char*)CXhChar16(pLegItem->m_fMaxLegH));
			for(pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
				fprintf(fp,"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
			for(pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				CXhChar16 sMat,sSpec;
				CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
				if(pRod->m_ciRodType==1)
				{
					sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fWidth);
					fprintf(fp,"R,%d,%d,%s,%s,%3f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat,
						pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
						pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
				}
				else if(pRod->m_ciRodType==2)
				{
					sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fWidth);
					fprintf(fp,"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat);
				}
				else
					fprintf(fp,"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
			}
			//��������Ϣ
			int sub_leg_index=1;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				fprintf(fp,"SubLeg%d\n",sub_leg_index);
				fprintf(fp,"HSubLeg%d,%s\n",sub_leg_index,(char*)CXhChar16(pSubLeg->m_fLegH));
				for(pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
					fprintf(fp,"P,%d,%f,%f,%f\n",pNode->handle,pNode->xOrg.x,pNode->xOrg.y,pNode->xOrg.z);
				for(pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					CXhChar16 sMat,sSpec;
					CModModel::QuerySteelMatMark(pRod->m_cMaterial,sMat);
					if(pRod->m_ciRodType==1)
					{
						sprintf(sSpec,"L%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
						fprintf(fp,"R,%d,%d,%s,%s,%3f,%f,%f,%f,%f,%f\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat,
							pRod->m_vWingX.x,pRod->m_vWingX.y,pRod->m_vWingX.z,
							pRod->m_vWingY.x,pRod->m_vWingY.y,pRod->m_vWingY.z);
					}
					else if(pRod->m_ciRodType==2)
					{
						sprintf(sSpec,"��%.0fX%.0f",pRod->m_fWidth,pRod->m_fThick);
						fprintf(fp,"R,%d,%d,%s,%s\n",pRod->m_uiNodeS,pRod->m_uiNodeE,(char*)sSpec,(char*)sMat);
					}
					else
						fprintf(fp,"R,%d,%d\n",pRod->m_uiNodeS,pRod->m_uiNodeE);
				}
				sub_leg_index++;
			}
			leg_index++;
		}
		body_index++;
	}
	//����ҵ���Ϣ
	for(MOD_HANG_NODE* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
		fprintf(fp,"G,%c,%s,%f,%f,%f\n",pGuaInfo->m_ciWireType,pGuaInfo->m_sHangName,pGuaInfo->m_xHangPos.x,pGuaInfo->m_xHangPos.y,pGuaInfo->m_xHangPos.z);
	fclose(fp);
	//��ȡ�ļ�����
	CBuffer src_buf;
	fp=fopen("C:\\test.txt","rb");
	fseek(fp,0,SEEK_END);
	long buf_size=ftell(fp);
	fseek(fp,0,SEEK_SET);
	src_buf.Write(NULL,buf_size);
	fread(src_buf.GetBufferPtr(),src_buf.GetLength(),1,fp);
	fclose(fp);
	//���ݱ����ʽҪ�����ת���洢
	if(pFile)
	{
		src_buf.SeekToBegin();
		if(bUtf8==FALSE)
			fwrite(src_buf.GetBufferPtr(),src_buf.GetLength(),1,pFile);
		else
		{	//ANSIת��ΪUnicode
			int nLen=MultiByteToWideChar(CP_ACP,0,src_buf.GetBufferPtr(),-1,NULL,0 );
			wchar_t* sWValue=(wchar_t*)malloc((nLen)*sizeof(wchar_t));
			memset(sWValue,0,sizeof(wchar_t)*(nLen));
			MultiByteToWideChar(CP_ACP,0,src_buf.GetBufferPtr(),-1,(LPWSTR)sWValue,nLen);
			//Unicodeת��ΪUTF-8
			nLen=WideCharToMultiByte(CP_UTF8,0,sWValue,-1,NULL,0,NULL,NULL);
			char* sValue=(char*)malloc((nLen)*sizeof(char));
			memset(sValue,0,sizeof(char)*(nLen));
			WideCharToMultiByte(CP_UTF8,0,sWValue,-1,sValue,nLen,NULL,NULL);
			//
			//char utf8header[3]={(char)0xef,(char)0xbb,(char)0xbf};
			//fwrite(utf8header,sizeof(TCHAR),3,pFile);
			fwrite(sValue,nLen,1,pFile);
			free(sValue);
			free(sWValue);
		}	
		fclose(pFile);
	}
}
//////////////////////////////////////////////////////////////////////////
//
static CHashPtrList<CModModel>localHashModModels;
class CModModelsLife{
public:
	~CModModelsLife(){localHashModModels.Empty();}
};
CModModelsLife modelsLife;

IModModel* CModModelFactory::CreateModModel()
{
	int iNo=1;
	do{
		if(localHashModModels.GetValue(iNo)!=NULL)
			iNo++;
		else	//�ҵ�һ���պ�
			break;
	}while(true);
	CModModel* pModel = localHashModModels.Add(iNo);
	return pModel;
};
IModModel* CModModelFactory::ModModelFromSerial(long serial)
{
	return localHashModModels.GetValue(serial);
}
bool CModModelFactory::Destroy(long serial)
{
	for(CModModel *pModModel=localHashModModels.GetFirst();pModModel;pModModel=localHashModModels.GetNext())
	{
		if(pModModel->GetSerialId()==serial)
			return localHashModModels.DeleteCursor(TRUE)==TRUE;
	}
	return false;
}