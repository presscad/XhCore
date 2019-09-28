#if !defined(AFX_PARTNORULEDLG_H__D1A3E349_639D_48FD_BC5B_7C65367F9812__INCLUDED_)
#define AFX_PARTNORULEDLG_H__D1A3E349_639D_48FD_BC5B_7C65367F9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartNoRuleDlg.h : header file
//
#include ".\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPartNoRuleDlg dialog

class AFX_EXT_CLASS CPartNoRuleDlg : public CDialog
{
// Construction
public:
	CListCtrl* m_pListRule;
	CPartNoRuleDlg(CWnd* pParent = NULL);   // standard constructor
	CStringArray rule_txt_arr;
	bool SetPartOrderRule(int i,int order,bool bActive);
	bool GetPartOrderRule(int i,int *order,bool *bActive);
// Dialog Data
	//{{AFX_DATA(CPartNoRuleDlg)
	enum { IDD = IDD_CODE_NO_RULE_DLG };
	BOOL	m_bJgFirstCodeNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartNoRuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartNoRuleDlg)
	afx_msg void OnBnMoveToBottom();
	afx_msg void OnBnMoveDown();
	afx_msg void OnBnMoveToTop();
	afx_msg void OnBnMoveUp();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int AppendRecord(CListCtrl *pListCtrl,CString sItem,DWORD iParam,BOOL bCheck);
	int InsertRecord(CListCtrl* pListCtrl, CString sItem, DWORD iParam,int iInserPos,BOOL bCheck);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTNORULEDLG_H__D1A3E349_639D_48FD_BC5B_7C65367F9812__INCLUDED_)
