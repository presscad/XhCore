//<LOCALE_TRANSLATE BY hxr /> 2015-5-26
// HeadCtrlPtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "f_ent.h"
#include "DefCtrlPtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefCtrlPtDlg dialog

BOOL ReadClipPoint(f3dPoint &point);
void WritePointToClip(f3dPoint point);

CDefCtrlPtDlg::CDefCtrlPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefCtrlPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefCtrlPtDlg)
	m_fCoordX = 0.0;
	m_fCoordY = 0.0;
	m_fCoordZ = 0.0;
	m_nMirType = 4;
	m_sCurItemText = _T("");
	//}}AFX_DATA_INIT
	m_pListCtrl=NULL;
	m_nNodeNum=0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("X");
	m_arrHeaderLabel.Add("Y.");
	m_arrHeaderLabel.Add("Z");
	m_arrHeaderLabel.Add("Mirror Info");
#else 
	m_arrHeaderLabel.Add("X-坐标");
	m_arrHeaderLabel.Add("Y-坐标");
	m_arrHeaderLabel.Add("Z-坐标");
	m_arrHeaderLabel.Add("对称信息");
#endif
}


void CDefCtrlPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefCtrlPtDlg)
	DDX_Text(pDX, IDC_EDIT_X_COOR_VAL, m_fCoordX);
	DDX_Text(pDX, IDC_EDIT_Y_COOR_VAL, m_fCoordY);
	DDX_Text(pDX, IDC_EDIT_Z_COOR_VAL, m_fCoordZ);
	DDX_CBIndex(pDX, IDC_CMB_MIR_MSG, m_nMirType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefCtrlPtDlg, CDialog)
	//{{AFX_MSG_MAP(CDefCtrlPtDlg)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnBnAddItem)
	ON_BN_CLICKED(IDC_BN_DELETE_ITEM, OnBnDeleteItem)
	ON_BN_CLICKED(IDC_BN_MODIFIY_ITEM, OnBnModifyItem)
	ON_BN_CLICKED(IDC_BN_PASTE_ITEM, OnBnPasteItem)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL_POINT, OnDblclkListCtrlPoint)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL_POINT, OnKeydownListCtrlPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefCtrlPtDlg message handlers

BOOL CDefCtrlPtDlg::OnInitDialog() 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL_POINT);
	if(m_pListCtrl)
		m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	else
		return FALSE;
	RefreshListViewHeader();
	return CDialog::OnInitDialog();
}

void CDefCtrlPtDlg::SimplifiedNumString(CString &num_str)
{
	int str_len = num_str.GetLength();
	int pointPos = -1;
	for(int i=str_len-1;i>=0;i--)
	{
		if(num_str[i]=='.')
		{
			pointPos=i;
			break;
		}
	}
	if(pointPos<0)
		return;
	int iPos=str_len-1;
	while(iPos>=pointPos)
	{
		if(num_str[iPos]=='0'||num_str[iPos]=='.')
			iPos--;
		else
			break;
	}
	num_str=num_str.Left(iPos+1);
}

void CDefCtrlPtDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		lvc.cx = 64;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CDefCtrlPtDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CDefCtrlPtDlg::OnBnAddItem() 
{
	CString tempStr;
	CStringArray str_arr;
	UpdateData();
	tempStr.Format("%f",m_fCoordX);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fCoordY);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%f",m_fCoordZ);
	SimplifiedNumString(tempStr);
	str_arr.Add(tempStr);
	tempStr.Format("%d",m_nMirType);
	str_arr.Add(tempStr);
	if(m_pListCtrl->GetItemCount()>100)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The program can only deal with the data within 100");
#else 
		AfxMessageBox("程序只能处理100个以内的数据");
#endif
		return;
	}
	AppendRecord(-1,str_arr);
}

void CDefCtrlPtDlg::OnBnDeleteItem() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		while(iCurSel>=0)
		{
			m_pListCtrl->DeleteItem(iCurSel);
			pos=m_pListCtrl->GetFirstSelectedItemPosition();
			if(pos==NULL)
				break;
			iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
		}
	}
}

void CDefCtrlPtDlg::OnBnModifyItem() 
{
	UpdateData();
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		if(iCurSel>=0)
		{
			//更新控制点列表
			CString ss;
			ss.Format("%f",m_fCoordX);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,0,ss);
			ss.Format("%f",m_fCoordY);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Format("%f",m_fCoordZ);
			SimplifiedNumString(ss);
			m_pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Format("%d",m_nMirType);
			m_pListCtrl->SetItemText(iCurSel,3,ss);
		}
	}
}

void CDefCtrlPtDlg::ModifyItem(int iCurSel)
{
	char sText[50];
	m_pListCtrl->GetItemText(iCurSel,0,sText,50);
	m_fCoordX=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,1,sText,50);
	m_fCoordY=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,2,sText,50);
	m_fCoordZ=atof(sText);
	m_pListCtrl->GetItemText(iCurSel,3,sText,50);
	m_nMirType=atoi(sText);
	UpdateData(FALSE);
}

void CDefCtrlPtDlg::OnDblclkListCtrlPoint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}
	*pResult = 0;
}

void CDefCtrlPtDlg::OnKeydownListCtrlPoint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				ModifyItem(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				m_pListCtrl->DeleteItem(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

void CDefCtrlPtDlg::OnOK() 
{
	m_nNodeNum = m_pListCtrl->GetItemCount();
	for(int i=0;i<m_nNodeNum;i++)
	{
		char sText[100]="";
		m_pListCtrl->GetItemText(i,0,sText,100);
		DataList[i].fPosX=atof(sText);
		m_pListCtrl->GetItemText(i,1,sText,100);
		DataList[i].fPosY=atof(sText);
		m_pListCtrl->GetItemText(i,2,sText,100);
		DataList[i].fPosZ=atof(sText);
		m_pListCtrl->GetItemText(i,3,sText,100);
		DataList[i].iMirMsg=atoi(sText);
	}
	CDialog::OnOK();
}

CString CDefCtrlPtDlg::DoubleToString(int nLen)
{
	char buffer[50];
	int nStrLen[5];
	CString str[5];
	CString retStr;
	double Val[4];
	nLen = 7;
	Val[0] = m_fCoordX;
	Val[1] = m_fCoordY;
	Val[2] = m_fCoordZ;
	Val[3] = m_nMirType;

	str[0] = _gcvt(Val[0],6,buffer);
	str[1] = _gcvt(Val[1],6,buffer);
	str[2] = _gcvt(Val[2],6,buffer);
	str[3] = _itoa((int)Val[3],buffer,10);
	for(int i=0;i<4;i++)
	{
		nStrLen[i] = str[i].GetLength();
		if(nStrLen[i]>nLen)
			str[i] = str[i].Left(nLen);
		else
		{
			for(int j=0;j<nLen-nStrLen[i];j++)
				str[i].Insert(0," ");
		}
	}
	retStr = str[0] + " " + str[1] + " " + str[2] + " " + str[3];
	m_sCurItemText = retStr;
	return retStr;
}

void CDefCtrlPtDlg::StringToDouble(CString source)
{
	CString TemStr;
	int nStrLen;
	double retVal[4];

	//得到正面开口宽
	for(int i=0;i<4;i++)
	{
		source.TrimLeft();
		TemStr = source.SpanExcluding(" ");
		nStrLen= TemStr.GetLength();
		source = source.Right(source.GetLength()-nStrLen);
		retVal[i] = atof(TemStr);
	}
	m_fCoordX = retVal[0];
	m_fCoordY = retVal[1];
	m_fCoordZ = retVal[2];
	m_nMirType = (int)retVal[3];
}

void CDefCtrlPtDlg::OnBnPasteItem() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fCoordX = point.x;
	m_fCoordY = point.y;
	m_fCoordZ = point.z;
	UpdateData(FALSE);
}
