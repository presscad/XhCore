// PromptBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PromptBarDlg.h"

// CPromptBarDlg 对话框

IMPLEMENT_DYNAMIC(CPromptBarDlg, CDialog)
CPromptBarDlg::CPromptBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPromptBarDlg::IDD, pParent)
{
	m_bCanceled=FALSE;
}

CPromptBarDlg::~CPromptBarDlg()
{
}

BOOL CPromptBarDlg::Create()
{
	return CDialog::Create(CPromptBarDlg::IDD);
}

void CPromptBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPromptBarDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

// CPromptBarDlg 消息处理程序

BOOL CPromptBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bCanceled=FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CPromptBarDlg::SetPromptStr(CString &prompt)
{
	CWnd *pWnd=GetDlgItem(IDC_S_PROMPT_TEXT);
	GetDlgItem(IDC_S_PROMPT_TEXT)->SetWindowText(prompt);
	MSG msg; //定义一个MSG类型的变量，不多解释
    while(PeekMessage(&msg,m_hWnd,0,0,PM_REMOVE)) //获取消息并把该消息从消息队列中移除（防止重复响应）。
    {
        TranslateMessage(&msg);//翻译消息 在合适的机会产生char消息
        DispatchMessage(&msg); //将消息移交给过程函数
    }
}
void CPromptBarDlg::OnBnClickedCancel()
{
	m_bCanceled=TRUE;
	OnCancel();
}
