#if !defined(AFX_DEFCTRLPTDLG_H__A7E206E8_3DF0_4A9C_9548_414636DD0BFA__INCLUDED_)
#define AFX_DEFCTRLPTDLG_H__A7E206E8_3DF0_4A9C_9548_414636DD0BFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeadCtrlPtDlg.h : header file
//
#include ".\resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDefCtrlPtDlg dialog

class AFX_EXT_CLASS CDefCtrlPtDlg : public CDialog
{
// Construction
public:
	struct CTRL_PT_COORD
	{
		double fPosX,fPosY,fPosZ;
		int iMirMsg;
	} DataList[100];
	CDefCtrlPtDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nNodeNum;
// Dialog Data
	//{{AFX_DATA(CDefCtrlPtDlg)
	enum { IDD = IDD_DEF_CTRL_PT_DLG };
	double	m_fCoordX;
	double	m_fCoordY;
	double	m_fCoordZ;
	int		m_nMirType;
	//}}AFX_DATA
	CString	m_sCurItemText;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefCtrlPtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefCtrlPtDlg)
	afx_msg void OnBnAddItem();
	afx_msg void OnBnDeleteItem();
	afx_msg void OnBnModifyItem();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPasteItem();
	afx_msg void OnDblclkListCtrlPoint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListCtrlPoint(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void RefreshListViewHeader();
	int AppendRecord(int iItem, CStringArray &str_arr);
	void SimplifiedNumString(CString &num_str);
	void ModifyItem(int iCurSel);
private:
	CString DoubleToString(int nLen);
	void StringToDouble(CString source);
	CListCtrl *m_pListCtrl;
	CStringArray m_arrHeaderLabel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADCTRLPTDLG_H__A7E206E8_3DF0_4A9C_9548_414636DD0BFA__INCLUDED_)
