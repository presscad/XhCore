#if !defined(AFX_ReportVectorDlg_H__EC6E766D_68DF_497B_ADD8_410931DC93D2__INCLUDED_)
#define AFX_ReportVectorDlg_H__EC6E766D_68DF_497B_ADD8_410931DC93D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportVectorDlg.h : header file
//
#include "f_ent.h"
#include ".\resource.h"
/////////////////////////////////////////////////////////////////////////////
// CReportVectorDlg dialog

class AFX_EXT_CLASS CReportVectorDlg : public CDialog
{
// Construction
public:
	CString m_sCaption;
	CReportVectorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReportVectorDlg)
	enum { IDD = IDD_REPORT_VEC_DLG };
	double	m_fVectorX;
	double	m_fVectorY;
	double	m_fVectorZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportVectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	BOOL	m_bReadOnlyX;
	BOOL	m_bReadOnlyY;
	BOOL	m_bReadOnlyZ;
private:
	void CorrectNegativeZero();
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReportVectorDlg)
	afx_msg void OnBnCopyVec();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPasteVec();
	afx_msg void OnBnReverseVector();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ReportVectorDlg_H__EC6E766D_68DF_497B_ADD8_410931DC93D2__INCLUDED_)
