//<LOCALE_TRANSLATE BY hxr /> 2015-1-9
// CfgPartNoDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "LmaDlg.h"
#include "CfgPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCfgPartNoDlg dialog


CCfgPartNoDlg::CCfgPartNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgPartNoDlg::IDD, pParent)
	, m_iBodyNo(0)
{
	//{{AFX_DATA_INIT(CCfgPartNoDlg)
#ifdef AFX_TARG_ENU_ENGLISH
	m_sLeftStr = _T("Vest In Leg:");
	m_sRightStr = _T("Remain Leg:");
#else 
	m_sLeftStr = _T("从属接腿:");
	m_sRightStr = _T("剩余接腿:");
#endif
	//}}AFX_DATA_INIT
	m_pListBoxAttach=m_pListBoxRemain=NULL;
	cfg_style=CFG_WORD_NO;
}


void CCfgPartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCfgPartNoDlg)
	DDX_Text(pDX, IDC_STR_LEFT, m_sLeftStr);
	DDX_Text(pDX, IDC_STR_RIGHT, m_sRightStr);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_E_BODY_NO, m_iBodyNo);
	DDV_MinMaxInt(pDX, m_iBodyNo, 0, 24);
}


BEGIN_MESSAGE_MAP(CCfgPartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CCfgPartNoDlg)
	ON_BN_CLICKED(IDC_BN_ADD_LEG_I, OnBnAddLegI)
	ON_BN_CLICKED(IDC_BN_DEL_LEG_I, OnBnDelLegI)
	ON_LBN_DBLCLK(IDC_LIST_ATTACH_LEG_I, OnBnDelLegI)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_LEG_I, OnBnAddLegI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCfgPartNoDlg message handlers

void CCfgPartNoDlg::OnBnAddLegI() 
{
	m_pListBoxAttach=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_LEG_I);
	m_pListBoxRemain=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_LEG_I);
	int nSel=m_pListBoxRemain->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=m_pListBoxRemain->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		m_pListBoxRemain->GetText(rgIndex[i],ss);
		int iSel=m_pListBoxAttach->AddString(ss);
		DWORD dw=m_pListBoxRemain->GetItemData(rgIndex[i]);
		m_pListBoxAttach->SetItemData(iSel,dw);
		_cfgword.AddSpecNoWord(dw);
		ss_arr.Add(ss);
	}
	delete []rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=m_pListBoxRemain->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			m_pListBoxRemain->DeleteString(ii);
	}
}

void CCfgPartNoDlg::OnBnDelLegI() 
{
	m_pListBoxAttach=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_LEG_I);
	m_pListBoxRemain=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_LEG_I);
	
	int nSel=m_pListBoxAttach->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=m_pListBoxAttach->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		m_pListBoxAttach->GetText(rgIndex[i],ss);
		int iSel=m_pListBoxRemain->AddString(ss);
		DWORD dw=m_pListBoxAttach->GetItemData(rgIndex[i]);
		m_pListBoxRemain->SetItemData(iSel,dw);
		_cfgword.RemoveSpecNoWord(dw);
		ss_arr.Add(ss);
	}
	delete []rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=m_pListBoxAttach->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			m_pListBoxAttach->DeleteString(ii);
	}
}

BOOL CCfgPartNoDlg::OnInitDialog() 
{
	int n;
	GetDlgItem(IDC_S_BODY_NO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_E_BODY_NO)->ShowWindow(SW_HIDE);
	if(cfg_style==CFG_WORD_NO)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sLeftStr="Vest In Cfgword:";
		m_sRightStr="Remain Cfgword:";
		SetWindowText("Cfgword Design");
#else 
		m_sLeftStr="从属配材:";
		m_sRightStr="剩余配材:";
		SetWindowText("配材号设定");
#endif
		n=192;
		_cfgword=cfgword;
	}
	else if(cfg_style==CFG_BODY_NO)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sLeftStr="Vest In Body:";
		m_sRightStr="Remain Body:";
		SetWindowText("Body No. Design");
#else 
		m_sLeftStr="从属本体:";
		m_sRightStr="剩余本体:";
		SetWindowText("本体号设定");
#endif
#ifdef _LEGACY_LICENSE
		n=24;
#else
		n=CFGLEG::MaxBodys();	//24
#endif
		_cfgword.Clear();
		_cfgword.flag.word[0]=cfgword.ToCfgLeg().BodyFlag();
	}
	else if(cfg_style==CFG_LEG_NO)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sLeftStr="Vest In Leg:";
		m_sRightStr="Remain Leg:";
		SetWindowText("Leg Label Design");
#else 
		m_sLeftStr="从属接腿:";
		m_sRightStr="剩余接腿:";
		SetWindowText("接腿号设定");
#endif
#ifdef _LEGACY_LICENSE
		n=8;
		_cfgword.Clear();
		_cfgword.flag.word[0]=cfgword.ToCfgLeg().LegFlag();
#endif
		char bodyStr[51]={0};
		cfgword.GetBodyScopeStr(bodyStr);
		if(strlen(bodyStr)<=2)
			m_iBodyNo=atoi(bodyStr);
		else
			m_iBodyNo=0;
#ifndef _LEGACY_LICENSE
		n=CFGLEG::MaxLegs();
		_cfgword.Clear();
		_cfgword.flag.word[0]=cfgword.GetBodyLegs(m_iBodyNo);//.ToCfgLeg().LegFlag();
#endif
		GetDlgItem(IDC_E_BODY_NO)->SetWindowText(bodyStr);
		GetDlgItem(IDC_S_BODY_NO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_BODY_NO)->ShowWindow(SW_SHOW);
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sLeftStr="Vest In Ray No.:";
		m_sRightStr="Remain Ray No.:";
		SetWindowText("Bolt Ray No. Design");
#else 
		m_sLeftStr="从属射线号:";
		m_sRightStr="剩余射线号:";
		SetWindowText("螺栓射线号设定");
#endif
		n=32;
		_cfgword=cfgword;
	}
	m_pListBoxAttach=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_LEG_I);
	m_pListBoxRemain=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_LEG_I);
	for(int i=0;i<n;i++)
	{
		CString sText;
		if(cfg_style!=CFG_LEG_NO)
			sText.Format("%3d",i+1);
		else
			sText.Format("%C",i+'A');
		if(_cfgword.IsHasNo(i+1))
		{
			int iCurSel=m_pListBoxAttach->AddString(sText);
			m_pListBoxAttach->SetItemData(iCurSel,i+1);
		}
		else
		{
			int iCurSel=m_pListBoxRemain->AddString(sText);
			m_pListBoxRemain->SetItemData(iCurSel,i+1);
		}
	}
	
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCfgPartNoDlg::OnOK() 
{
	if(!UpdateData())
		return;
	if(cfg_style==CFG_WORD_NO)
		cfgword=_cfgword;
	else if(cfg_style==CFG_BODY_NO)
	{
#ifdef _LEGACY_LICENSE
		for(int i=0;i<24;i++)
		{
			if(_cfgword.IsHasNo(i+1))
				cfgword.flag.bytes[i]=0xff;
			else
				cfgword.flag.bytes[i]=0;
		}
#else
		for(int i=0;i<CFGLEG::MaxBodys();i++)
		{
			if(_cfgword.IsHasNo(i+1))
				cfgword.AddBodyLegs(i+1);//flag.bytes[i]=0xff;
			else
				cfgword.SetBodyLegs(i+1,0);
		}
#endif
	}
	else if(cfg_style==CFG_LEG_NO)
	{
		for(int i=0;i<24;i++)
		{
#ifdef _LEGACY_LICENSE
			if((m_iBodyNo==0&&cfgword.flag.bytes[i]>0)||i+1==m_iBodyNo)
				cfgword.flag.bytes[i]=_cfgword.flag.bytes[0];
			else
				cfgword.flag.bytes[i]=0;
#else
			if((m_iBodyNo==0&&cfgword.IsHasBodyNo(i+1))||i+1==m_iBodyNo)
				cfgword.SetBodyLegs(i+1,_cfgword.flag.word[0]);
			else
				cfgword.SetBodyLegs(i+1,0);
#endif
		}
	}
	else
		cfgword=_cfgword;
	if(cfgword.IsNull())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Cfgword can't be null!");
#else 
		MessageBox("配材号不能为空!");
#endif
		return;
	}
	CDialog::OnOK();
}
