// PartNoRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PartNoRuleDlg.h"
//#include "GlobalVAR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartNoRuleDlg dialog

typedef struct {
	int iOrder;
	bool bActive;
}PARTORDERRULE_STRU;
static PARTORDERRULE_STRU rule_arr[4];

CPartNoRuleDlg::CPartNoRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartNoRuleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartNoRuleDlg)
	m_bJgFirstCodeNo = FALSE;
	//}}AFX_DATA_INIT
	for(int i=0;i<4;i++)
	{
		rule_arr[i].bActive = true;
		rule_arr[i].iOrder  = i;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	rule_txt_arr.Add("Main Material First Then Other Parts");
	rule_txt_arr.Add("Bottom To Top");
	rule_txt_arr.Add("Left To Right");
	rule_txt_arr.Add("From Front,Side To Section");
#else 
	rule_txt_arr.Add("先主材后其它构件");
	rule_txt_arr.Add("由下向上");
	rule_txt_arr.Add("由左向右");
	rule_txt_arr.Add("由正面、侧面到断面");
#endif
}


void CPartNoRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartNoRuleDlg)
	DDX_Check(pDX, IDC_CHK_JG_FIRST, m_bJgFirstCodeNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartNoRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CPartNoRuleDlg)
	ON_BN_CLICKED(IDC_BN_MOVE_TO_BOTTOM, OnBnMoveToBottom)
	ON_BN_CLICKED(IDC_BN_MOVE_DOWN, OnBnMoveDown)
	ON_BN_CLICKED(IDC_BN_MOVE_TO_TOP, OnBnMoveToTop)
	ON_BN_CLICKED(IDC_BN_MOVE_UP, OnBnMoveUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartNoRuleDlg message handlers
bool CPartNoRuleDlg::SetPartOrderRule(int i,int order,bool bActive)
{
	if(i<0||i>=4||order<0||order>=4)
		return false;
	rule_arr[i].iOrder=order;
	rule_arr[i].bActive=bActive;
	return true;
}
bool CPartNoRuleDlg::GetPartOrderRule(int i,int *order,bool *bActive)
{
	if(i<0||i>=4||order==NULL||bActive==NULL)
		return false;
	*order=rule_arr[i].iOrder;
	*bActive=rule_arr[i].bActive;
	return true;
}
BOOL CPartNoRuleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pListRule = (CListCtrl*)GetDlgItem(IDC_LIST_RULE);
	m_pListRule->SetExtendedStyle(LVS_EX_CHECKBOXES);

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(rule_arr[j].iOrder==i)
			{
				AppendRecord(m_pListRule,rule_txt_arr[j],j,rule_arr[j].bActive);
				break;
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartNoRuleDlg::OnBnMoveToBottom() 
{
	int iCurSel=-1;
	POSITION pos = m_pListRule->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		iCurSel = m_pListRule->GetNextSelectedItem(pos);
		if(iCurSel<0)
			return;
	}
	else
		return;
	DWORD iParam = m_pListRule->GetItemData(iCurSel);
	BOOL bCheck = m_pListRule->GetCheck(iCurSel);
	m_pListRule->DeleteItem(iCurSel);
	AppendRecord(m_pListRule,rule_txt_arr[iParam],iParam,bCheck);
	//选中插入项
	int nItemCount=m_pListRule->GetItemCount();
	for(int i=0;i<nItemCount;i++)
	{
		UINT uFlag=LVIS_SELECTED | LVIS_FOCUSED;
		if(i==nItemCount-1)
			m_pListRule->SetItemState(i,uFlag,uFlag);
		else 
			m_pListRule->SetItemState(i,0,uFlag);
	}
}

void CPartNoRuleDlg::OnBnMoveDown() 
{
	int nRows=0, iCurSel=-1;
	POSITION pos = m_pListRule->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		iCurSel = m_pListRule->GetNextSelectedItem(pos);
		if(iCurSel<0)
			return;
	}
	else
		return;
	nRows = m_pListRule->GetItemCount();
	if(iCurSel>=nRows-1)
		return;
	DWORD iParam = m_pListRule->GetItemData(iCurSel+1);
	BOOL bCheck = m_pListRule->GetCheck(iCurSel+1);
	m_pListRule->DeleteItem(iCurSel+1);
	int iItem=InsertRecord(m_pListRule,rule_txt_arr[iParam],iParam,iCurSel,bCheck);
	//选中插入项
	int nItemCount=m_pListRule->GetItemCount();
	for(int i=0;i<nItemCount;i++)
	{
		UINT uFlag=LVIS_SELECTED | LVIS_FOCUSED;
		if(i==(iItem+1)&&(iItem+1)<nItemCount)
			m_pListRule->SetItemState(i,uFlag,uFlag);
		else 
			m_pListRule->SetItemState(i,0,uFlag);
	}
}

void CPartNoRuleDlg::OnBnMoveToTop() 
{
	int nRows=0, iCurSel=-1;
	POSITION pos = m_pListRule->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		iCurSel = m_pListRule->GetNextSelectedItem(pos);
		if(iCurSel<0)
			return;
	}
	else 
		return;
	nRows = m_pListRule->GetItemCount();
	DWORD iParam = m_pListRule->GetItemData(iCurSel);
	BOOL bCheck = m_pListRule->GetCheck(iCurSel);
	m_pListRule->DeleteItem(iCurSel);
	InsertRecord(m_pListRule,rule_txt_arr[iParam],iParam,-1,bCheck);
	//选中插入项
	int nItemCount=m_pListRule->GetItemCount();
	for(int i=0;i<nItemCount;i++)
	{
		UINT uFlag=LVIS_SELECTED | LVIS_FOCUSED;
		if(i==0)
			m_pListRule->SetItemState(i,uFlag,uFlag);
		else 
			m_pListRule->SetItemState(i,0,uFlag);
	}
}

void CPartNoRuleDlg::OnBnMoveUp() 
{
	int iCurSel;
	POSITION pos = m_pListRule->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		iCurSel = m_pListRule->GetNextSelectedItem(pos);
		if(iCurSel<0)
			return;
	}
	else
		return;
	if(iCurSel<=0)
		return;
	DWORD iParam = m_pListRule->GetItemData(iCurSel-1);
	BOOL bCheck = m_pListRule->GetCheck(iCurSel-1);
	m_pListRule->DeleteItem(iCurSel-1);
	int iItem=InsertRecord(m_pListRule,rule_txt_arr[iParam],iParam,iCurSel,bCheck);
	//选中插入项
	int nItemCount=m_pListRule->GetItemCount();
	for(int i=0;i<nItemCount;i++)
	{
		UINT uFlag=LVIS_SELECTED | LVIS_FOCUSED;
		if(i==(iItem-1)&&iItem>=1)
			m_pListRule->SetItemState(i,uFlag,uFlag);
		else 
			m_pListRule->SetItemState(i,0,uFlag);
	}
}

int CPartNoRuleDlg::AppendRecord(CListCtrl *pListCtrl,CString sItem,
								 DWORD iParam,BOOL bCheck)
{
	LV_ITEM lvi;
	int iItem=pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = sItem.GetBuffer(1);//_gszItem[i][0];
	lvi.lParam = iParam;
	iItem = pListCtrl->InsertItem(&lvi);
	pListCtrl->SetCheck(iItem,bCheck);
	pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

int CPartNoRuleDlg::InsertRecord(CListCtrl *pListCtrl, CString sItem, 
								 DWORD iParam,int iInsertPos,BOOL bCheck)
{
	LV_ITEM lvi;
	int nRows = pListCtrl->GetItemCount();
	if(iInsertPos<0)
		iInsertPos = 0;
	else if(iInsertPos>=nRows)
		iInsertPos = nRows;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iInsertPos;
	lvi.iSubItem = 0;
	lvi.pszText = sItem.GetBuffer(1);//_gszItem[i][0];
	lvi.lParam = iParam;
	iInsertPos = pListCtrl->InsertItem(&lvi);
	pListCtrl->SetCheck(iInsertPos,bCheck);
	pListCtrl->RedrawItems(iInsertPos,iInsertPos);
	return iInsertPos;
}
void CPartNoRuleDlg::OnOK() 
{
	int i;
	DWORD iParam;
	UpdateData();
	for(i=0;i<4;i++)
	{
		iParam = m_pListRule->GetItemData(i);
		rule_arr[iParam].iOrder = i;
		if(m_pListRule->GetCheck(i))
			rule_arr[iParam].bActive = true;
		else
			rule_arr[iParam].bActive = false;
	}
	
	CDialog::OnOK();
}
