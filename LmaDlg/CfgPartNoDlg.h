#if !defined(AFX_CFGPARTNODLG_H__72069E84_1130_49E1_8A36_F88323C3AF72__INCLUDED_)
#define AFX_CFGPARTNODLG_H__72069E84_1130_49E1_8A36_F88323C3AF72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CfgPartNoDlg.h : header file
//
#include ".\resource.h"
//#include "ToolKits.h"
#ifdef _LEGACY_LICENSE
#include "Lic.h"
#else
#include "XhLdsLm.h"
#endif
/////////////////////////////////////////////////////////////////////////////
// CCfgPartNoDlg dialog
enum CFG_NO_STYLE {CFG_WORD_NO,CFG_BODY_NO,CFG_LEG_NO,CFG_LSRAY_NO};
class AFX_EXT_CLASS CCfgPartNoDlg : public CDialog
{
	CFGWORD _cfgword;
	// ±¾ÌåºÅ
	int m_iBodyNo;
// Construction
public:
	CFG_NO_STYLE cfg_style;
	CListBox* m_pListBoxAttach;
	CListBox* m_pListBoxRemain;
	CFGWORD cfgword;
	CCfgPartNoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCfgPartNoDlg)
	enum { IDD = IDD_CFG_PART_NO_DLG };
	CString	m_sLeftStr;
	CString	m_sRightStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCfgPartNoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCfgPartNoDlg)
	afx_msg void OnBnAddLegI();
	afx_msg void OnBnDelLegI();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFGPARTNODLG_H__72069E84_1130_49E1_8A36_F88323C3AF72__INCLUDED_)
