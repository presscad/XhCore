#if !defined(AFX_MODELPROPDLG_H__036462CE_50DC_4D23_B7C2_47B6391F3948__INCLUDED_)
#define AFX_MODELPROPDLG_H__036462CE_50DC_4D23_B7C2_47B6391F3948__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelPropDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CModelPropDlg dialog

class AFX_EXT_CLASS CModelPropDlg : public CDialog
{
// Construction
public:
	CFGWORD m_dwQuad1CfgNo;
	CFGWORD m_dwQuad2CfgNo;
	CFGWORD m_dwQuad3CfgNo;
	CFGWORD m_dwQuad4CfgNo;

	CModelPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelPropDlg)
	enum { IDD = IDD_MODEL_PROP_DLG };
	CString	m_sQuad1CfgNo;
	CString	m_sQuad2CfgNo;
	CString	m_sQuad3CfgNo;
	CString	m_sQuad4CfgNo;
	CString	m_sModelName;
	double	m_fBurglarLsEndZ;
	double	m_fBurglarLsStartZ;
	short	m_iBodyLegNo;
	CString	m_sIncPartSegStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBnQuad1LegCfgno();
	afx_msg void OnBnQuad2LegCfgno();
	afx_msg void OnBnQuad3LegCfgno();
	afx_msg void OnBnQuad4LegCfgno();
	afx_msg void OnBnAutoGetSegStr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL Refresh(BOOL bFreshData=TRUE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELPROPDLG_H__036462CE_50DC_4D23_B7C2_47B6391F3948__INCLUDED_)
