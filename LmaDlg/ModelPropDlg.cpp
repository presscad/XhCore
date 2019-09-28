// ModelPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelPropDlg.h"
#include "CfgPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelPropDlg dialog


CModelPropDlg::CModelPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelPropDlg)
	m_sQuad1CfgNo = _T("");
	m_sQuad2CfgNo = _T("");
	m_sQuad3CfgNo = _T("");
	m_sQuad4CfgNo = _T("");
	m_sModelName = _T("");
	m_fBurglarLsEndZ = 0.0;
	m_fBurglarLsStartZ = 0.0;
	m_iBodyLegNo = 0;
	m_sIncPartSegStr = _T("");
	//}}AFX_DATA_INIT
	m_dwQuad1CfgNo=1;
	m_dwQuad2CfgNo=1;
	m_dwQuad3CfgNo=1;
	m_dwQuad4CfgNo=1;
}


void CModelPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelPropDlg)
	DDX_Text(pDX, IDC_E_QUAD1_LEG_CFGNO, m_sQuad1CfgNo);
	DDX_Text(pDX, IDC_E_QUAD2_LEG_CFGNO, m_sQuad2CfgNo);
	DDX_Text(pDX, IDC_E_QUAD3_LEG_CFGNO, m_sQuad3CfgNo);
	DDX_Text(pDX, IDC_E_QUAD4_LEG_CFGNO, m_sQuad4CfgNo);
	DDX_Text(pDX, IDC_E_MODEL_NAME, m_sModelName);
	DDV_MaxChars(pDX, m_sModelName, 49);
	DDX_Text(pDX, IDC_E_MODEL_BURGLAR_LS_END_Z, m_fBurglarLsEndZ);
	DDX_Text(pDX, IDC_E_MODEL_BURGLAR_LS_START_Z, m_fBurglarLsStartZ);
	DDX_Text(pDX, IDC_E_BODY_LEGNO, m_iBodyLegNo);
	DDV_MinMaxInt(pDX, m_iBodyLegNo, 1, 160);
	DDX_Text(pDX, IDC_E_MODEL_INC_PART_SEG_STR, m_sIncPartSegStr);
	DDV_MaxChars(pDX, m_sIncPartSegStr, 101);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelPropDlg, CDialog)
	//{{AFX_MSG_MAP(CModelPropDlg)
	ON_BN_CLICKED(IDC_BN_QUAD1_LEG_CFGNO, OnBnQuad1LegCfgno)
	ON_BN_CLICKED(IDC_BN_QUAD2_LEG_CFGNO, OnBnQuad2LegCfgno)
	ON_BN_CLICKED(IDC_BN_QUAD3_LEG_CFGNO, OnBnQuad3LegCfgno)
	ON_BN_CLICKED(IDC_BN_QUAD4_LEG_CFGNO, OnBnQuad4LegCfgno)
	ON_BN_CLICKED(IDC_BN_AUTO_GET_SEG_STR, OnBnAutoGetSegStr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelPropDlg message handlers

BOOL CModelPropDlg::OnInitDialog() 
{
	char cfg_str[500]="";
	if(MakeWordToString(m_dwQuad1CfgNo,cfg_str)>0)
		m_sQuad1CfgNo = cfg_str;
	if(MakeWordToString(m_dwQuad2CfgNo,cfg_str)>0)
		m_sQuad2CfgNo = cfg_str;
	if(MakeWordToString(m_dwQuad3CfgNo,cfg_str)>0)
		m_sQuad3CfgNo = cfg_str;
	if(MakeWordToString(m_dwQuad4CfgNo,cfg_str)>0)
		m_sQuad4CfgNo = cfg_str;
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModelPropDlg::OnBnQuad1LegCfgno() 
{
	CCfgPartNoDlg dlg;
	UpdateData();
	dlg.cfgword=m_dwQuad1CfgNo;
	if(dlg.DoModal()==IDOK)
	{
		m_dwQuad1CfgNo=m_dwQuad2CfgNo=m_dwQuad3CfgNo=m_dwQuad4CfgNo=dlg.cfgword;
		char cfg_str[500]="";
		if(MakeWordToString(m_dwQuad1CfgNo,cfg_str)>0)
			m_sQuad1CfgNo = cfg_str;
		if(MakeWordToString(m_dwQuad2CfgNo,cfg_str)>0)
			m_sQuad2CfgNo = cfg_str;
		if(MakeWordToString(m_dwQuad3CfgNo,cfg_str)>0)
			m_sQuad3CfgNo = cfg_str;
		if(MakeWordToString(m_dwQuad4CfgNo,cfg_str)>0)
			m_sQuad4CfgNo = cfg_str;
		UpdateData(FALSE);
	}
}

void CModelPropDlg::OnBnQuad2LegCfgno() 
{
	CCfgPartNoDlg dlg;
	UpdateData();
	dlg.cfgword=m_dwQuad2CfgNo;
	if(dlg.DoModal()==IDOK)
	{
		char cfg_str[500]="";
		m_dwQuad2CfgNo=dlg.cfgword;
		if(MakeWordToString(m_dwQuad2CfgNo,cfg_str)>0)
			m_sQuad2CfgNo = cfg_str;
		UpdateData(FALSE);
	}
}

void CModelPropDlg::OnBnQuad3LegCfgno() 
{
	CCfgPartNoDlg dlg;
	UpdateData();
	dlg.cfgword=m_dwQuad3CfgNo;
	if(dlg.DoModal()==IDOK)
	{
		char cfg_str[500]="";
		m_dwQuad3CfgNo=dlg.cfgword;
		if(MakeWordToString(m_dwQuad3CfgNo,cfg_str)>0)
			m_sQuad3CfgNo = cfg_str;
		UpdateData(FALSE);
	}
}

void CModelPropDlg::OnBnQuad4LegCfgno() 
{
	CCfgPartNoDlg dlg;
	UpdateData();
	dlg.cfgword=m_dwQuad4CfgNo;
	if(dlg.DoModal()==IDOK)
	{
		char cfg_str[500]="";
		m_dwQuad4CfgNo=dlg.cfgword;
		if(MakeWordToString(m_dwQuad4CfgNo,cfg_str)>0)
			m_sQuad4CfgNo = cfg_str;
		UpdateData(FALSE);
	}
}

void CModelPropDlg::OnBnAutoGetSegStr() 
{
	CTMAModule module;
	module.m_iBodyLegNo = m_iBodyLegNo;
	module.m_dwQuad1LegCfgNo = m_dwQuad1CfgNo;
	module.m_dwQuad2LegCfgNo = m_dwQuad2CfgNo;
	module.m_dwQuad3LegCfgNo = m_dwQuad3CfgNo;
	module.m_dwQuad4LegCfgNo = m_dwQuad4CfgNo;
	bool seg_flag_arr[201];
	memset(seg_flag_arr,0,sizeof(bool)*201);
	for(CTMAPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(module.IsSonPart(pPart)&&pPart->iSeg>=0)
			seg_flag_arr[pPart->iSeg]=true;
	}
	int start_i=0,end_i=0;
	m_sIncPartSegStr.Empty();	//清空原字符串
	for(int i=0;i<201;i++)
	{
		if(!seg_flag_arr[i])	//此段不属于本统计模型
		{
			CString seg;
			if(start_i<end_i)		//存在区间段
			{
				seg.Format("%d-%d",start_i,end_i);
				if(m_sIncPartSegStr.GetLength()==0)
					m_sIncPartSegStr=seg;
				else
					m_sIncPartSegStr+=","+seg;
			}
			else if(start_i>=0)
			{
				seg.Format("%d",start_i);
				if(m_sIncPartSegStr.GetLength()==0)
					m_sIncPartSegStr=seg;
				else
					m_sIncPartSegStr+=","+seg;
			}
			start_i=-1;
			end_i=-1;
		}
		else if(start_i==-1)
			start_i=i;
		else
			end_i=i;
	}
	GetDlgItem(IDC_E_MODEL_INC_PART_SEG_STR)->SetWindowText(m_sIncPartSegStr);
}
