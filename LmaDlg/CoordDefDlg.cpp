//<LOCALE_TRANSLATE BY hxr /> 2015-5-26
// CoordDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CoordDefDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoordDefDlg dialog

BOOL ReadClipPoint(f3dPoint &point);
void WritePointToClip(f3dPoint point);
BOOL ReadClipUCS(UCS_STRU &ucs,FILE *fp=NULL);
void WriteUCSToClip(UCS_STRU &ucs,FILE *fp=NULL);

CCoordDefDlg::CCoordDefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCoordDefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCoordDefDlg)
	m_fAxisX_X = 1.0;
	m_fAxisX_Y = 0.0;
	m_fAxisX_Z = 0.0;
	m_fAxisY_X = 0.0;
	m_fAxisY_Y = 1.0;
	m_fAxisY_Z = 0.0;
	m_fAxisZ_X = 0.0;
	m_fAxisZ_Y = 0.0;
	m_fAxisZ_Z = 1.0;
	m_fOriginPosX = 0.0;
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	m_bCanModifyAxisX = TRUE;
	m_bCanModifyAxisY = FALSE;
	//}}AFX_DATA_INIT
	ucs.axis_x.Set(1.0,0.0,0.0);
	ucs.axis_y.Set(0.0,1.0,0.0);
	ucs.axis_z.Set(0.0,0.0,1.0);
	m_bCanModifyAxisZ = TRUE;
}

void CCoordDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoordDefDlg)
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisX_X);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisX_Y);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisX_Z);
	DDX_Text(pDX, IDC_E_AXIS_Y_X, m_fAxisY_X);
	DDX_Text(pDX, IDC_E_AXIS_Y_Y, m_fAxisY_Y);
	DDX_Text(pDX, IDC_E_AXIS_Y_Z, m_fAxisY_Z);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZ_X);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZ_Y);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZ_Z);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_X, m_bCanModifyAxisX);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Y, m_bCanModifyAxisY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCoordDefDlg, CDialog)
	//{{AFX_MSG_MAP(CCoordDefDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Y, OnBnPasteAxisY)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Y, OnBnCopyAxisY)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	ON_BN_CLICKED(IDC_BN_CPY_UCS, OnBnCpyUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_UCS, OnBnPasteUcs)
	ON_BN_CLICKED(IDC_BN_TO_RIGHT_HAND, OnBnToRightHand)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_X, OnChkEnableAxisX)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Y, OnChkEnableAxisY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoordDefDlg message handlers
void CCoordDefDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisX_X = point.x;
		m_fAxisX_Y = point.y;
		m_fAxisX_Z = point.z;
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		UpdateData(FALSE);
	}
}

void CCoordDefDlg::OnBnPasteAxisY() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisY_X = point.x;
		m_fAxisY_Y = point.y;
		m_fAxisY_Z = point.z;
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		UpdateData(FALSE);
	}
}

void CCoordDefDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZ_X = point.x;
		m_fAxisZ_Y = point.y;
		m_fAxisZ_Z = point.z;
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		UpdateData(FALSE);
	}
}

void CCoordDefDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
		UpdateData(FALSE);
	}
}

void CCoordDefDlg::OnOK() 
{
	UpdateUcs();
	if(!is_ucs_valid(ucs))
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("UCS is invalid(Not right hand spiral orthogonal UCS)");
#else 
		AfxMessageBox("����ϵ���Ϸ�(����������������������ϵ)");
#endif
	else
		CDialog::OnOK();
}

void CCoordDefDlg::UpdateUcs(BOOL bFlag)
{
	if(bFlag)	//����������ucs����
	{
		UpdateData(TRUE);
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	}
	else	//ucs�򽻻��������
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

BOOL CCoordDefDlg::OnInitDialog() 
{
	m_bCanModifyAxisY = !m_bCanModifyAxisX;
	CDialog::OnInitDialog();
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);

	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);

	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);

	UpdateUcs(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCoordDefDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	WritePointToClip(point);
}

void CCoordDefDlg::OnBnCopyAxisY() 
{
	f3dPoint point;
	point.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	WritePointToClip(point);
}

void CCoordDefDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WritePointToClip(point);
}

void CCoordDefDlg::OnBnCopyOriginPos() 
{
	f3dPoint point;
	point.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

void CCoordDefDlg::OnBnCpyUcs() 
{
	UCS_STRU ucs;
	UpdateData();
	ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WriteUCSToClip(ucs);
}

void CCoordDefDlg::OnBnPasteUcs() 
{
	UCS_STRU;
	if(ReadClipUCS(ucs))
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CCoordDefDlg::OnBnToRightHand() 
{
	UpdateUcs();
	if(m_bCanModifyAxisY)
	{
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_z);
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else //if(m_bCanModifyAxisX)
	{
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		normalize(ucs.axis_z);
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	UpdateUcs(FALSE);
}

void CCoordDefDlg::OnChkEnableAxisX() 
{
	UpdateData();
	m_bCanModifyAxisY = !m_bCanModifyAxisX;
	UpdateData(FALSE);

	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);

	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
}

void CCoordDefDlg::OnChkEnableAxisY() 
{
	UpdateData();
	m_bCanModifyAxisX = !m_bCanModifyAxisY;
	UpdateData(FALSE);

	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);

	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
}
