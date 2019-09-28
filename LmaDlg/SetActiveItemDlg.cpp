// SetActiveItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetActiveItemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetActiveItemDlg dialog


CSetActiveItemDlg::CSetActiveItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetActiveItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetActiveItemDlg)
	m_sActiveItem = _T("");
	m_sPrompt = _T("��ǰ������ֻ��ʾ�������������Ӧ���Ȳ����ġ�");
	//}}AFX_DATA_INIT
}


void CSetActiveItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetActiveItemDlg)
	DDX_LBString(pDX, IDC_LIST_VALID_ITEM, m_sActiveItem);
	//DDX_Text(pDX, IDC_S_PROMPT, m_sPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetActiveItemDlg, CDialog)
	//{{AFX_MSG_MAP(CSetActiveItemDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetActiveItemDlg message handlers

BOOL CSetActiveItemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_VALID_ITEM);
	if(pListBox==NULL)
		return FALSE;
	for(int i=0;i<m_arrStrList.GetSize();i++)
	{
		pListBox->AddString(m_arrStrList[i]);
		if(m_arrStrList[i].CompareNoCase(m_sActiveItem)==0)
			pListBox->SetCurSel(i);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
