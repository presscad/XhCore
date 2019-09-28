//<LOCALE_TRANSLATE BY hxr /> 2015-5-26
// ReportVectorDg.cpp : implementation file
//

#include "stdafx.h"
#include "ReportVectorDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportVectorDlg dialog

BOOL ReadClipPoint(f3dPoint &point);
void WritePointToClip(f3dPoint point);

CReportVectorDlg::CReportVectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReportVectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReportVectorDlg)
	m_fVectorX = 0.0;
	m_fVectorY = 0.0;
	m_fVectorZ = 0.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = "Vector Direction";
#else 
	m_sCaption = "Ê¸Á¿·½Ïò";
#endif
	m_bReadOnlyX = FALSE;
	m_bReadOnlyY = FALSE;
	m_bReadOnlyZ = FALSE;
}


void CReportVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportVectorDlg)
	DDX_Text(pDX, IDC_E_VECTOR_X, m_fVectorX);
	DDX_Text(pDX, IDC_E_VECTOR_Y, m_fVectorY);
	DDX_Text(pDX, IDC_E_VECTOR_Z, m_fVectorZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CReportVectorDlg)
	ON_BN_CLICKED(IDC_BN_COPY_VEC, OnBnCopyVec)
	ON_BN_CLICKED(IDC_BN_PASTE_VEC, OnBnPasteVec)
	ON_BN_CLICKED(IDC_BN_REVERSE_VECTOR, OnBnReverseVector)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportVectorDlg message handlers

BOOL CReportVectorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_sCaption);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_X))->SetReadOnly(m_bReadOnlyX);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_Y))->SetReadOnly(m_bReadOnlyY);
	((CEdit*)GetDlgItem(IDC_E_VECTOR_Z))->SetReadOnly(m_bReadOnlyZ);
	if(m_bReadOnlyX || m_bReadOnlyY || m_bReadOnlyZ)
	{
		GetDlgItem(IDC_BN_REVERSE_VECTOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_PASTE_VEC)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BN_REVERSE_VECTOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BN_PASTE_VEC)->EnableWindow(TRUE);
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReportVectorDlg::OnBnCopyVec() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fVectorX,m_fVectorY,m_fVectorZ);
	WritePointToClip(point);
}

void CReportVectorDlg::OnBnPasteVec() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fVectorX=point.x;
		m_fVectorY=point.y;
		m_fVectorZ=point.z;
		UpdateData(FALSE);
	}
	
}

void CReportVectorDlg::OnBnReverseVector() 
{
	CorrectNegativeZero();
	UpdateData();
	m_fVectorX *= -1.0;
	m_fVectorY *= -1.0;
	m_fVectorZ *= -1.0;
	UpdateData(FALSE);
}
void CReportVectorDlg::CorrectNegativeZero()
{
	CString sValue=_T("");
	GetDlgItemText(IDC_E_VECTOR_X,sValue);
	if(sValue.CompareNoCase("-0")==0)
		SetDlgItemText(IDC_E_VECTOR_X,"0");
	GetDlgItemText(IDC_E_VECTOR_Y,sValue);
	if(sValue.CompareNoCase("-0")==0)
		SetDlgItemText(IDC_E_VECTOR_Y,"0");
	GetDlgItemText(IDC_E_VECTOR_Z,sValue);
	if(sValue.CompareNoCase("-0")==0)
		SetDlgItemText(IDC_E_VECTOR_Z,"0");
}

void CReportVectorDlg::OnOK()
{
	CorrectNegativeZero();
	CDialog::OnOK();
}
