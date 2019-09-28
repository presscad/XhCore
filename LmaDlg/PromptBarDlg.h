#pragma once

#include ".\resource.h"
// CPromptBarDlg �Ի���

class AFX_EXT_CLASS CPromptBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CPromptBarDlg)
	BOOL m_bCanceled;
public:
	CPromptBarDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPromptBarDlg();
	CWnd *m_pWnd;
	BOOL Create();
	void SetPromptStr(CString &prompt);
	BOOL CanceledProc(){return m_bCanceled;}

// �Ի�������
	enum { IDD = IDD_PROMPT_BAR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
