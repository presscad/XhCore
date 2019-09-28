// PromptBarDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PromptBarDlg.h"

// CPromptBarDlg �Ի���

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

// CPromptBarDlg ��Ϣ�������

BOOL CPromptBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bCanceled=FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CPromptBarDlg::SetPromptStr(CString &prompt)
{
	CWnd *pWnd=GetDlgItem(IDC_S_PROMPT_TEXT);
	GetDlgItem(IDC_S_PROMPT_TEXT)->SetWindowText(prompt);
	MSG msg; //����һ��MSG���͵ı������������
    while(PeekMessage(&msg,m_hWnd,0,0,PM_REMOVE)) //��ȡ��Ϣ���Ѹ���Ϣ����Ϣ�������Ƴ�����ֹ�ظ���Ӧ����
    {
        TranslateMessage(&msg);//������Ϣ �ں��ʵĻ������char��Ϣ
        DispatchMessage(&msg); //����Ϣ�ƽ������̺���
    }
}
void CPromptBarDlg::OnBnClickedCancel()
{
	m_bCanceled=TRUE;
	OnCancel();
}
