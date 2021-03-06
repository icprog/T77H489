// RunInfo.cpp : implementation file
//

#include "stdafx.h"
#include "FT.h"
#include "RunInfo.h"
#include "math.h"
#include "PTSC.h"
#include <direct.h>
#include "shlwapi.h"
#include "Tlhelp32.h"
#include <io.h>
#include "CompareProgram.h"
#include <stdio.h>
#include "ErrorCodeList.h"

#include "ni488.h"
#include "powermeter.h"

//#include "stdafx.h"
#include "WINSCARD.H"						//twq
//#include <fstream>
#include <math.h>
//#include <atlstr.h>
#include <winsock2.h>


#define WM_COMET WM_USER+1
#define UM_SOCK     WM_USER+4
#define UM_SOCK_UDP  WM_USER+5

#define WM_LED_CCD_SOCK_TCP  WM_USER+100

#define RST_BTN_VALUE "Upgrading firmware, don't reboot"
#define WPS_BTN_VALUE "WPS button is pressed"
#define WIFI_BTN_VALUE "Upgrading firmware, don't config wireless"

#define MICE_REESET_BUTTON "GPIO:17, reset button is pressed"

CString str;
CString	svpn;
char TxtSum[5120] = {0};
char    m_strIMEI[20] = "";
char    szIMEIFromDUT[20] = "";
char    szRssiValue[MINBUFSIZE] = "";
float   CmpRsi=0.0;
float   CmpRso=0.0;
IMPLEMENT_DYNAMIC(CRunInfo, CDialog)

static CString strRunInfo;

static INIT_INFO m_Mydas;
void *pInstMydas;

CRunInfo *pRunInfo = NULL;
HANDLE hDev=NULL;
HANDLE hThread1;
HANDLE h_golenEvt;
bool i_flag=true;
_ButtonInfo ButtonInfo;
_GoldenInfo	GoldenInfo;
CString          g_data;
_USB	USB;

_LEDCCDTESTInfo LedCcdTestInfo;

//for NFC Test
#pragma comment(lib, "Winscard.lib")  //twq

//twq
	int								retCode;
	SCARDCONTEXT	hContext;
	DWORD					size = 64;
	char							readerName[256];
	SCARDHANDLE		hCard;
	unsigned long			dwActProtocol;
	char 						errMessage[200];

//twq
	int Write_mac_flag=0;//shengzhao-zhao for R1C 20140314

void GetScardErrMsg(int code)
{
	switch( code )
	{
	// Smartcard Reader interface errors
	case SCARD_E_CANCELLED:
		sprintf_s(errMessage, sizeof(errMessage), "The action was canceled by an SCardCancel request.\n");
		break;
	case SCARD_E_CANT_DISPOSE:
		sprintf_s(errMessage, sizeof(errMessage), "The system could not dispose of the media in the requested manner.\n");
		break;
	case SCARD_E_CARD_UNSUPPORTED:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card does not meet minimal requirements for support.\n");
		break;
	case SCARD_E_DUPLICATE_READER:
		sprintf_s(errMessage, sizeof(errMessage), "The reader driver didn't produce a unique reader name.\n");
		break;
	case SCARD_E_INSUFFICIENT_BUFFER:
		sprintf_s(errMessage, sizeof(errMessage), "The data buffer for returned data is too small for the returned data.\n");
		break;
	case SCARD_E_INVALID_ATR:
		sprintf_s(errMessage, sizeof(errMessage), "An ATR string obtained from the registry is not a valid ATR string.\n");
		break;
	case SCARD_E_INVALID_HANDLE:
		sprintf_s(errMessage, sizeof(errMessage), "The supplied handle was invalid.\n");
		break;
	case SCARD_E_INVALID_PARAMETER:
		sprintf_s(errMessage, sizeof(errMessage), "One or more of the supplied parameters could not be properly interpreted.\n");
		break;
	case SCARD_E_INVALID_TARGET:
		sprintf_s(errMessage, sizeof(errMessage), "Registry startup information is missing or invalid.\n");
		break;
	case SCARD_E_INVALID_VALUE:
		sprintf_s(errMessage, sizeof(errMessage), "One or more of the supplied parameter values could not be properly interpreted.\n");
		break;
	case SCARD_E_NOT_READY:
		sprintf_s(errMessage, sizeof(errMessage), "The reader or card is not ready to accept commands.\n");
		break;
	case SCARD_E_NOT_TRANSACTED:
		sprintf_s(errMessage, sizeof(errMessage), "An attempt was made to end a non-existent transaction.\n");
		break;
	case SCARD_E_NO_MEMORY:
		sprintf_s(errMessage, sizeof(errMessage), "Not enough memory available to complete this command.\n");
		break;
	case SCARD_E_NO_SERVICE:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card resource manager is not running.\n");
		break;
	case SCARD_E_NO_SMARTCARD:
		sprintf_s(errMessage, sizeof(errMessage), "The operation requires a smart card, but no smart card is currently in the device.\n");
		break;
	case SCARD_E_PCI_TOO_SMALL:
		sprintf_s(errMessage, sizeof(errMessage), "The PCI receive buffer was too small.\n");
		break;
	case SCARD_E_PROTO_MISMATCH:
		sprintf_s(errMessage, sizeof(errMessage), "The requested protocols are incompatible with the protocol currently in use with the card.\n");
		break;
	case SCARD_E_READER_UNAVAILABLE:
		sprintf_s(errMessage, sizeof(errMessage), "The specified reader is not currently available for use.\n");
		break;
	case SCARD_E_READER_UNSUPPORTED:
		sprintf_s(errMessage, sizeof(errMessage), "The reader driver does not meet minimal requirements for support.\n");
		break;
	case SCARD_E_SERVICE_STOPPED:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card resource manager has shut down.\n");
		break;
	case SCARD_E_SHARING_VIOLATION:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card cannot be accessed because of other outstanding connections.\n");
		break;
	case SCARD_E_SYSTEM_CANCELLED:
		sprintf_s(errMessage, sizeof(errMessage), "The action was canceled by the system, presumably to log off or shut down.\n");
		break;
	case SCARD_E_TIMEOUT:
		sprintf_s(errMessage, sizeof(errMessage), "The user-specified timeout value has expired.\n");
		break;
	case SCARD_E_UNKNOWN_CARD:
		sprintf_s(errMessage, sizeof(errMessage), "The specified smart card name is not recognized.\n");
		break;
	case SCARD_E_UNKNOWN_READER:
		sprintf_s(errMessage, sizeof(errMessage), "The specified reader name is not recognized.\n");
		break;
	case SCARD_F_COMM_ERROR:
		sprintf_s(errMessage, sizeof(errMessage), "An internal communications error has been detected.\n");
		break;
	case SCARD_F_INTERNAL_ERROR:
		sprintf_s(errMessage, sizeof(errMessage), "An internal consistency check failed.\n");
		break;
	case SCARD_F_UNKNOWN_ERROR:
		sprintf_s(errMessage, sizeof(errMessage), "An internal error has been detected, but the source is unknown.\n");
		break;
	case SCARD_F_WAITED_TOO_LONG:
		sprintf_s(errMessage, sizeof(errMessage), "An internal consistency timer has expired.\n");
		break;
	case SCARD_W_REMOVED_CARD:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card has been removed and no further communication is possible.\n");
		break;
	case SCARD_W_RESET_CARD:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card has been reset, so any shared state information is invalid.\n");
		break;
	case SCARD_W_UNPOWERED_CARD:
		sprintf_s(errMessage, sizeof(errMessage), "Power has been removed from the smart card and no further communication is possible.\n");
		break;
	case SCARD_W_UNRESPONSIVE_CARD:
		sprintf_s(errMessage, sizeof(errMessage), "The smart card is not responding to a reset.\n");
		break;
	case SCARD_W_UNSUPPORTED_CARD:
		sprintf_s(errMessage, sizeof(errMessage), "The reader cannot communicate with the card due to ATR string configuration conflicts.\n");
		break;
	}
	sprintf_s(errMessage, sizeof(errMessage), "Error is not documented.\n");
}

	//twq   nfc_init
int OnInit()
{	
	//1  Init OK
	int retVal = 1;
	/*
	int								retCode;
	SCARDCONTEXT	hContext;
	DWORD					size = 64;
	char							readerName[256];
	SCARDHANDLE		hCard;
	unsigned long			dwActProtocol;
	char 						errMessage[200];
	*/

	//Establish Context
	retCode = SCardEstablishContext(SCARD_SCOPE_USER,
			NULL,
			NULL,
			&hContext);
	
	if(retCode != SCARD_S_SUCCESS)
	{	
		GetScardErrMsg(retCode);
		printf("Error Message: %s\n", errMessage);
		retVal = 0;
		return retVal;
	}
	
	//List PC/SC Card Readers
	size = 256;
	retCode = SCardListReaders(hContext, NULL, readerName, &size);
	if(retCode != SCARD_S_SUCCESS)
	{
		GetScardErrMsg(retCode);
		printf("Error Message: %s\n", errMessage);
		retVal = 0;
		return retVal;
	}		
	
	if(readerName == NULL)
	{
		GetScardErrMsg(retCode);
		printf("Error Message: %s\n", errMessage);
		retVal = 0;
		return retVal;
	}
	
	printf("ReaderName is %s\n", readerName);
	return retVal;	
}


//twq  nfc_connection
int OnConn()
{	
	// 1 Connection OK
	int retVal = 1;
	//char buffer1[100];
	
	//Connect to selected reader
	retCode = SCardConnect(hContext,
			readerName,
			SCARD_SHARE_SHARED,
			SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1,
			&hCard,
			&dwActProtocol);
	
	if(retCode != SCARD_S_SUCCESS)
	{
		//Failed to connect to reader
		GetScardErrMsg(retCode);
		printf("Failed to connect to reader: %s\n", errMessage);
		retVal = 0;
		return retVal;
	}
	
	printf("Successful connection to: %s\n", readerName);
	return retVal;		
}
//end

void Delay(unsigned long i)
{
    //TODO: Add your source code here
    unsigned long start = ::GetTickCount();
    while((::GetTickCount() - start) < i )
    {
		Sleep(1);
    }
}

CRunInfo::CRunInfo(CWnd* pParent )
	: CDialog(CRunInfo::IDD, pParent)
	, m_strSN(_T(""))
	, m_strMAC(_T(""))
	, m_nLanMAC(0)
	, m_nWanMAC(0)
	, m_strPincode(_T(""))  
	, m_strSfisStatus(_T(""))
	, m_strTestResult(_T(""))
	, m_strProductName(_T(""))
	, m_strTestTime(_T(""))
	, m_strErroCode(_T(""))
	, m_strCN(_T(""))
	, m_FlukeAddr(3)
	, m_Station(_T(""))
{
	m_Socket = INVALID_SOCKET;
	m_GoldenSocket = INVALID_SOCKET;

	h_golenEvt = CreateEvent(NULL,true,false,NULL);

	m_TestTimeCount= 0;
	m_blIsOpenSfis= false;  
	m_blIsSfis = false;

	m_hEvt = NULL;
	m_hEvt = CreateEvent(NULL,true,false,NULL);
	strcpy_s(m_IniFileInfo.szDefaultIP1,MINBUFSIZE,"192.168.1.1");
}

CRunInfo::~CRunInfo()
{
	if(NULL != m_hEvt)
		CloseHandle(m_hEvt);
	if(m_Socket!=INVALID_SOCKET)
		closesocket(m_Socket);
	if(m_GoldenSocket!=INVALID_SOCKET)
		closesocket(m_GoldenSocket);
	WSACleanup();
}

bool  CRunInfo::SendDutCommand(char *pCmd, char *pRtn, int iDelay)
{ 
	int iSubDelay = iDelay/10;
	ResetEvent(m_hEvt);
	g_data="";
	SendEthernetCmd(pCmd);
	
	while(iSubDelay--)
	{
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEvt,10))
		{
			if(pRtn==NULL)
			{
				return true;
			}
			else
			{
				if(strstr(g_data,pRtn) != NULL)
				{
					return true;
				}
			}	
			ResetEvent(m_hEvt);
		}		
	}	

	return false;
}

bool  CRunInfo::SendDutCommand1(char *pCmd, char *pRtn, long iDelay)
{ 
	long iSubDelay = iDelay/10;
	ResetEvent(m_hEvt);
	g_data="";
	SendEthernetCmd(pCmd);
	for(int i=0;i<100;i++)
	{
		Delay(1000);
	}
	while(iSubDelay--)
	{
		if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEvt,10))
		{
			if(pRtn==NULL)
			{
				return true;
			}
			else
			{
				if(strstr(g_data,pRtn) != NULL)
				{
					return true;
				}
			}	
			ResetEvent(m_hEvt);
		}		
	}	

	return false;
}

void CRunInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SN, m_strSN);
	DDX_Text(pDX, IDC_EDIT_MAC, m_strMAC);
	DDX_Text(pDX, IDC_EDIT_PINCODE, m_strPincode);
	DDX_CBString(pDX, IDC_EDIT_COM, m_strCom);
	DDX_Text(pDX, IDC_SFIS_STATUS, m_strSfisStatus);
	DDX_Text(pDX, IDC_RESULT, m_strTestResult);
	DDX_Control(pDX, IDC_BTN_START, m_BtnStart);
	DDX_Text(pDX, IDC_PRODUCTNAME, m_strProductName);
	DDX_Text(pDX, IDC_TESTTIME, m_strTestTime);
	DDX_Text(pDX, IDC_ERRORCODE, m_strErroCode);
	DDX_Control(pDX, IDC_BTN_QUIT, m_BtnQuit);
	DDX_Text(pDX, IDC_EDIT_CN, m_strCN);
	DDV_MinMaxInt(pDX, m_FlukeAddr, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_PASS_DUT, m_strPASS_2G);
	DDX_Text(pDX, IDC_EDIT_SSID_DUT, m_strSSID_2G);

	DDX_Text(pDX, IDC_STATION, m_Station);
}


BEGIN_MESSAGE_MAP(CRunInfo, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_START, &CRunInfo::OnBnClickedBtnStart)
	ON_MESSAGE(WM_COMET,CRunInfo::OnRecvComData)
	ON_MESSAGE(UM_SOCK ,CRunInfo::OnRecvEthernetData)
	ON_BN_CLICKED(IDC_BTN_QUIT, &CRunInfo::OnBnClickedBtnQuit)
	ON_CBN_SELCHANGE(IDC_COM, &CRunInfo::OnCbnSelchangeCom)
	ON_MESSAGE(WM_LED_CCD_SOCK_TCP ,CRunInfo::bRecvFromLedCCDSocket)
END_MESSAGE_MAP()


// CRunInfo message handlers

HBRUSH CRunInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(IDC_PRODUCTNAME == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Midft);
		pDC->SetTextColor(RGB(255,255,0));
		pDC->SetBkMode(TRANSPARENT);
		return m_Grancybh;
	}
	if(IDC_PASS_COUNT == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Minft);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	if(IDC_FAIL_COUNT == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Minft);
		pDC->SetTextColor(RGB(255,0,0));
		pDC->SetBkMode(TRANSPARENT);
	}
	if(IDC_STATION == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Midft);
		pDC->SetTextColor(RGB(255,255,0));
		pDC->SetBkMode(TRANSPARENT);
		return m_Grancybh;
	}
	if(IDC_RESULT==pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Maxft);
		pDC->SetBkMode(TRANSPARENT);

		if(!strcmp("StandBy", m_strTestResult))
		{
			pDC->SetTextColor(RGB(255,255,0));
			return m_Grancybh;
		}
		if(!strcmp("RUN", m_strTestResult))
		{
			pDC->SetTextColor(RGB(0,255,0));
			return m_Yellowbh;
		}
		if(!strcmp("PASS", m_strTestResult))
		{
			pDC->SetTextColor(RGB(0,0,255));
			return m_Greenbh;
		}
		if(!strcmp("Start RUN-IN OK", m_strTestResult))
		{
			pDC->SelectObject(&m_RUNINft);//for RUN-IN station
			pDC->SetTextColor(RGB(0,0,255));
			return m_Greenbh;
		}
		if(!strcmp("FAIL", m_strTestResult))
		{
			pDC->SetTextColor(RGB(255,255,255));
			return m_Redbh;
		}
	}
	if(IDC_SFIS_STATUS==pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Minft);
		pDC->SetBkMode(TRANSPARENT);
		if(!strcmp("SFIS OFF", m_strSfisStatus))
		{
			pDC->SetTextColor(RGB(255,255,0));
			return m_Redbh;
		}
		if(!strcmp("SFIS ON", m_strSfisStatus))
		{
			pDC->SetTextColor(RGB(255,255,0));
			return m_Greenbh;		
		}
	}
	if(IDC_PASS == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Minft);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	if(IDC_FAIL == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Minft);
		pDC->SetTextColor(RGB(255,0,0));
		pDC->SetBkMode(TRANSPARENT);
	}
	if(IDC_TESTTIME == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_Minft);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	if(IDC_ERROR == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_SupMinft);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	if(IDC_ERRORCODE == pWnd->GetDlgCtrlID())
	{
		pDC->SelectObject(&m_SupMinft);
		pDC->SetTextColor(RGB(255,0,0));
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CRunInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	ReadCableloss();
	m_SupMinft.CreatePointFont(100, _T("Arial"));
	m_Minft.CreatePointFont(200, _T("Arial"));
	m_Midft.CreatePointFont(350, _T("Arial"));
	m_Maxft.CreatePointFont(1000,_T("Arial"));
	m_RUNINft.CreatePointFont(500,_T("Arial"));

	m_Redbh.CreateSolidBrush(RGB(255,0,0));
	m_Greenbh.CreateSolidBrush(RGB(0,255,0));
	m_Yellowbh.CreateSolidBrush(RGB(255,255,0));
	m_Grancybh.CreateSolidBrush(RGB(82,64,111));

	m_ComSfis.SetParentWnd(m_hWnd);
	char szProductName[30] = "";	
	gethostname(szProductName,30);
	_strupr_s(szProductName,30);
	m_strPcName = szProductName;
	if(m_strPcName.GetLength() > 12)
	{
		m_strPcName.Delete(12,m_strPcName.GetLength()-12);
	}
	else
	{
		int len = m_strPcName.GetLength();
		for(int i = len; i<12; i++)
			m_strPcName +='-';
	}
	SetDlgItemText(IDC_EDIT_PCNAME,m_strPcName);
	
	InitGoldenSocket();

	((CComboBox*)GetDlgItem(IDC_COM))->AddString(_T("COM1"));
	((CComboBox*)GetDlgItem(IDC_COM))->AddString(_T("COM2"));
	((CComboBox*)GetDlgItem(IDC_COM))->AddString(_T("COM3"));
	((CComboBox*)GetDlgItem(IDC_COM))->AddString(_T("COM4"));
	((CComboBox*)GetDlgItem(IDC_COM))->AddString(_T("COM5"));
	((CComboBox*)GetDlgItem(IDC_COM))->AddString(_T("COM6"));

	//int nCount=((CComboBox*)GetDlgItem(IDC_COM))->GetCount();
	//((CComboBox*)GetDlgItem(IDC_COM))->SetCurSel(nCount-1);
	//GetDlgItemText(IDC_COM,m_strCom);

	m_strCom = _T("COM1");
	((CComboBox*)GetDlgItem(IDC_COM))->SetCurSel(0);

	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE01"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE02"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE03"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE04"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE05"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE06"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE07"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE08"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE09"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE0A"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE0B"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE0C"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE0D"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE0E"));
	((CComboBox*)GetDlgItem(IDC_SSID))->AddString(_T("MICE0F"));
	((CComboBox*)GetDlgItem(IDC_SSID))->SetCurSel(0);

	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("3"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("4"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("5"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("6"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("7"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("8"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->AddString(_T("9"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->SetCurSel(0);

	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("3"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("4"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("5"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("6"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("7"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("8"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->AddString(_T("9"));
	((CComboBox*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->SetCurSel(3);

	((CComboBox*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->AddString(_T("153"));
	((CComboBox*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->AddString(_T("44"));
	((CComboBox*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->AddString(_T("153"));
	((CComboBox*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->SetCurSel(0);

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP2))->SetAddress(10,0,0,2);
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP))->SetAddress(192,168,1,100);
	
	m_strTestResult = _T("StandBy");

	m_strSfisStatus = _T("SFIS OFF");

	m_strSN = _T("SJV0QJ8");

	m_strMAC = _T("000000000001");

	m_strPincode = _T("12345670");

	m_strCN = _T("0000000000001"); 

	m_strSSID_2G = _T("MICE");

	m_strPASS_2G = _T("MICE_PASS");

	UpdateData(false);

	HKEY   hk;
	DWORD  dwDisp;
	char RegBuf[256] = "SYSTEM\\CurrentControlSet\\Services\\NETGEAR";
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
					RegBuf,
					0,
					NULL,
					REG_OPTION_NON_VOLATILE,
					KEY_WRITE | KEY_QUERY_VALUE,
					NULL,
					&hk,
					&dwDisp))
	{
		DisplayRunTimeInfo("Create registry fail");
	}
	else
	{
		if(dwDisp == REG_CREATED_NEW_KEY)
		{
			DisplayRunTimeInfo("Create new key");
			RegCloseKey(hk);
		}
		else
		{
			DisplayRunTimeInfo("Read old key");
			char szSSID[256] = "";			
			char szGoldenIPWireless[256] = "";
			char szGoldenIPEthernet[256] = "";
			int iSSID;
			char szChannel_2G_1[256] = "";
			char szChannel_2G_2[256] = "";
			char szChannel_5G_1[256] = "";
			char szChannel_5G_2[256] = "";

			
			DWORD dwBuf1 = 256,dwBuf3 = 256,dwBuf4 = 256;
			DWORD dwBuf11 = 256,dwBuf12 = 256,dwBuf13 = 256,dwBuf14 = 256;
			if(RegQueryValueEx(hk,"SSID",NULL,NULL,(LPBYTE)szSSID,&dwBuf1)		
			 ||RegQueryValueEx(hk,"GoldenIP(W)",NULL,NULL,(LPBYTE)szGoldenIPWireless,&dwBuf3)

			 ||RegQueryValueEx(hk,"CHANNEL_2G_1",NULL,NULL,(LPBYTE)szChannel_2G_1,&dwBuf11)
			 ||RegQueryValueEx(hk,"CHANNEL_2G_2",NULL,NULL,(LPBYTE)szChannel_2G_2,&dwBuf12)
			 ||RegQueryValueEx(hk,"CHANNEL_5G_1",NULL,NULL,(LPBYTE)szChannel_5G_1,&dwBuf13)
			 ||RegQueryValueEx(hk,"CHANNEL_5G_2",NULL,NULL,(LPBYTE)szChannel_5G_2,&dwBuf14)

			 ||RegQueryValueEx(hk,"GoldenIP(E)",NULL,NULL,(LPBYTE)szGoldenIPEthernet,&dwBuf4))
			{
				DisplayRunTimeInfo("Get Info from registry fail");
				RegCloseKey(hk);
			}
			else
			{
				iSSID = atoi(szSSID);

				((CComboBox*)GetDlgItem(IDC_EDIT_SSID))->SetCurSel(iSSID);					
				
				((CIPAddressCtrl*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->SetWindowText(szChannel_2G_1);

				((CIPAddressCtrl*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->SetWindowText(szChannel_2G_2);

				((CIPAddressCtrl*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->SetWindowText(szChannel_5G_1);

				((CIPAddressCtrl*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->SetWindowText(szChannel_5G_2);

				((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP))->SetWindowText(szGoldenIPWireless);

				((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP2))->SetWindowText(szGoldenIPEthernet);

				RegCloseKey(hk);
			}
		}
	}

	if(!NewReadIniFile())
	{
		AfxMessageBox("請確保程序文件目錄下有init.ini！Plearse make sure init.ini at program path!");
	}

	if(!ReadIniFileForGeneric())
	{
		m_IniFileInfo.nChannelNum_2G_1 = 3;
		m_IniFileInfo.nChannelNum_5G_1 = 3;
		m_IniFileInfo.nChannelNum_2G_2 = 3;
		m_IniFileInfo.nChannelNum_5G_2 = 3;
	}	


	if(IDYES==AfxMessageBox(_T("是否要加入SFIS\nSFIS ON/OFF?"),MB_YESNO|MB_ICONQUESTION))
	{
		m_blIsSfis = true;
		m_BtnStart.EnableWindow(FALSE);
		m_BtnQuit.EnableWindow(FALSE);
		((CEdit*)GetDlgItem(IDC_EDIT_SN))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_MAC))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_PINCODE))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_CN))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_SSID_DUT))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_PASS_DUT))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_SSID))->EnableWindow(false);

		char szPath[MAX_PATH] = "";
		char szFullPath[MAX_PATH] = "";
		char szFullPath1[MAX_PATH] = "";
		GetCurrentDirectory(MAX_PATH, szPath);
		sprintf_s(szFullPath, MAX_PATH, "%s\\%s", szPath , m_IniFileInfo.szTestInfoFileName);
		sprintf_s(szFullPath1, MAX_PATH, "%s\\%s", m_IniFileInfo.szServerFilePath,m_IniFileInfo.szTestInfoFileName);

		////Add compare program file last wirte time and checksum at local and server
		//char szProgramState[4096];	
		//char szLocalPath[256];
		//memset(szLocalPath , 0 , 256);
		//memset(szProgramState , 0 , 4096);
		//GetCurrentDirectory(256, szLocalPath);
		//if(AutoDownLoadProgram(szLocalPath	, m_IniFileInfo.szServerFilePath , szProgramState))
		//{
		//	DisplayRunTimeInfo("程序更新完成!Program upgrade finish!");
		//}
		//DisplayRunTimeInfo(szProgramState);
		////End

		if(!CopyFile(szFullPath1,szFullPath , false))
		{
			AfxMessageBox("請确定server是否映射到本地F:盤,請關閉程式!\n");
			SendSfisResultAndShowFail(SY05);
			return true;
		}
		
		
		//char ComPort[20]="";
		//sprintf_s(ComPort,20,"Program use %s\n",m_strCom);
		//ShowMsg(ComPort);

		//char* port;
		//GetDlgItemText(IDC_COM,port);

		if(!m_ComSfis.Open("com1", 9600))
		//if(!m_ComSfis.Open(port, 9600))
		{
			AfxMessageBox("默認連接SFIS的COM1口不存在或者被其他應用程序占用\n請從新選擇COM與SFIS連接\nThe default COM1 don't exit or is used by other application!\nPls choose COM port again");
			return TRUE;
		}

		m_ComSfis.SendData(("Hello\r\n"), (int)strlen("Hello\r\n"));

		char pBuf[MINBUFSIZE] = "";
		m_ComSfis.ReadData(pBuf, MINBUFSIZE, 1500);

		//if(strstr(pBuf, "ERRO") == NULL)
		if(strstr(pBuf, "PASS") == NULL)  
		{
			AfxMessageBox(_T("請從新選擇COM與SFIS連接\n"));
			m_ComSfis.Close();
			return TRUE;
		}
		else
		{
			m_strSfisStatus = _T("SFIS ON");
			UpdateData(false);
			m_blIsOpenSfis = true;
			AfxBeginThread(ReadComForSfis, &m_ComSfis);
		}
	}
	else
	{
		if(IDOK == m_IsStartSfis.DoModal())
		{
			CString  strName = m_IsStartSfis.m_ProductName;
			if(strName.IsEmpty())
			{
				AfxMessageBox(_T("請輸入机种名稱,不能為空"));
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
			}
			else
			{
				m_strProductName = m_IsStartSfis.m_ProductName;
				_strupr_s(m_strProductName.GetBuffer(m_strProductName.GetLength()), m_strProductName.GetLength()+1);
				UpdateData(false);
				if(!ReadIniFile())
				{
					AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
					::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
					return TRUE;
				}
			}
		}
		else
		{
			::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
		}
	}

	return TRUE;  
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRunInfo::SendSfisResultAndShowFail(char* ErroCode)
{
	if(strstr(m_IniFileInfo.szTestStatuse , "FT") || strstr(m_IniFileInfo.szTestStatuse , "RC") || strstr(m_IniFileInfo.szTestStatuse , "RC1") || strstr(m_IniFileInfo.szTestStatuse , "FAN") || strstr(m_IniFileInfo.szTestStatuse , "RUN-IN") || strstr(m_IniFileInfo.szTestStatuse , "NFC"))
	{
		//turn off HDD
		//stop /dev/sda
		SendDutCommand("sdparm --command=stop /dev/sda","#",10000);
		//end
	}
	char szErrorCode[MINBUFSIZE] = "";
	char szSendSfisData[MINBUFSIZE] = "";
	strncpy_s(szErrorCode, MINBUFSIZE, ErroCode, 4);
	DisplayRunTimeInfo(ErroCode);
	if(m_blIsOpenSfis)
	{	
		ShowMsg("*****************Send test Result(Fail) to SFIS*****************************");
		if(strstr(m_IniFileInfo.szTestStatuse , "FT"))/*FT SFIS mode*/
		{
			if(strcmp(m_IniFileInfo.szIsHaveSSIDLable , "1") == 0)
			{

				/*check Version*/
				if(strcmp(m_IniFileInfo.szUpLoadVersion,"1") == 0)
				{
					//HH(25)+PCBA SN(25)+SN(25)+Eth_MAC1(12)+MAC2(12)+MAC3(12)+PC NAME(12)+ERRO CODE(4)
					/*
					CString		m_strHH;
					CString		m_strPCBASN;
					CString		m_strSN;
					CString		m_strET0MAC;
					CString		m_str2GMAC;
					CString		m_str5GMAC;
					CString		m_str2GSSID;
					CString		m_str5GSSID;
					CString		m_strPNNAME;
					CString		m_strStatus;
					*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-25s%-25s%-12s%-12s%-12s%-12s%-4s\r\n", m_strHH, m_strPCBASN, m_strSN, m_strET0MAC, m_str2GMAC, m_str5GMAC, m_strPcName , szErrorCode); //for Mice FT


					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+SSID(32)+pass(32)+SSID(32)+pass(32)+FW(30)+PCNAME(12)+Err(4)
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-32s%-32s%-32s%-32s%-30s%-30s%-60s%-12s%-4s\r\n", 
						m_strSN , 
						m_strMAC , 
						m_strPincode , 
						m_strCN , 
						m_strSSID_2G ,
						m_strPASS_2G ,
						m_strSSID_5G ,
						m_strPASS_5G ,
						szFWforSfis , 
						szBF,
						szOpenVPN,
						m_strPcName , 
						szErrorCode);//ADD
						*/
				}
				else
				{
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+SSID(32)+pass(32)+SSID(32)+pass(32)+PCNAME(12)+Err(4)*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-32s%-32s%-32s%-32s%-30s%-60s%-12s%-4s\r\n", 
						m_strSN, 
						m_strMAC, 
						m_strPincode, 
						m_strCN, 
						m_strSSID_2G ,
						m_strPASS_2G ,
						m_strSSID_5G ,
						m_strPASS_5G ,
						szBF,
						szOpenVPN,
						m_strPcName , 
						szErrorCode);//ADD
				}
			}
			else
			{
				/*check Version*/
				if(strcmp(m_IniFileInfo.szUpLoadVersion,"1") == 0)
				{
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+FW(30)+PCNAME(12)+Err(4)*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-30s%-12s%-4s\r\n", m_strSN , m_strMAC , m_strPincode , m_strCN , szFWforSfis , m_strPcName , szErrorCode);//ADD
				}
				else
				{
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+PCNAME(12)+Err(4)*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-12s%-4s\r\n", m_strSN, m_strMAC, m_strPincode, m_strCN, m_strPcName , szErrorCode);
				}
			}
		}
		if(strstr(m_IniFileInfo.szTestStatuse , "RC") || strstr(m_IniFileInfo.szTestStatuse , "NFC"))/*RC SFIS mode*/
		{
			if(strcmp(m_IniFileInfo.szIsHaveSSIDLable , "1") == 0)
			{
				sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-25s%-25s%-12s%-12s%-12s%-12s%-4s\r\n", m_strHH, m_strPCBASN, m_strSN, m_strET0MAC, m_str2GMAC, m_str5GMAC, m_strPcName , szErrorCode); //for Mice RC
				/*HH(25)+MAC(12)+PIN(12)+SSN(15)+SSID(32)+pass(32)+SSID(32)+pass(32)+PCNAME(12)+Err(4)
				sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-32s%-32s%-32s%-32s%-30s%-60s%-12s%-4s\r\n", 
					m_strSN, 
					m_strMAC, 
					m_strPincode, 
					m_strCN, 
					m_strSSID_2G ,
					m_strPASS_2G ,
					m_strSSID_5G ,
					m_strPASS_5G ,
					szBF,
					szOpenVPN,
					m_strPcName , 
					szErrorCode);//ADD
					*/
			}
			else
			{
				/*HH(25)+MAC(12)+PIN(12)+SSN(15)+PCNAME(12)+Err(4)*/
				sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-12s%-4s\r\n", m_strSN, m_strMAC, m_strPincode, m_strCN, m_strPcName , szErrorCode);
			}
		}
		if(strstr(m_IniFileInfo.szTestStatuse , "PT2"))/*PT2 SFIS mode*/
		{
			/*HH(25)+MAC(12)+PCNAME(12)+Err(4)*/
			sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-4s\r\n", m_strSN, m_strMAC, m_strPcName , szErrorCode);//ADD
		}
		ShowMsg(szSendSfisData);
		m_ComSfis.SendData(szSendSfisData, MINBUFSIZE);
	}	
	m_strRecordTestData  +=  "\t";
	m_strRecordTestData  += szErrorCode;
	TestFail(ErroCode);
}

void CRunInfo::SendSfisResultAndShowPass()
{
	if(strstr(m_IniFileInfo.szTestStatuse , "FT") || strstr(m_IniFileInfo.szTestStatuse , "RC") || strstr(m_IniFileInfo.szTestStatuse , "RC1") || strstr(m_IniFileInfo.szTestStatuse , "FAN") || strstr(m_IniFileInfo.szTestStatuse , "RUN-IN") || strstr(m_IniFileInfo.szTestStatuse , "NFC") || strstr(m_IniFileInfo.szTestStatuse , "DOWNLOAD"))
	{
		//turn off HDD
		//stop /dev/sda
		SendDutCommand("sdparm --command=stop /dev/sda","#",10000);
		//end
	}
	if(m_blIsOpenSfis)
	{
		ShowMsg("*****************Send Result(Pass) to SFIS ******************");
		char szSendSfisData[MINBUFSIZE] = "";
		if(strstr(m_IniFileInfo.szTestStatuse , "FT"))/*FT SFIS mode*/
		{
			if(strcmp(m_IniFileInfo.szIsHaveSSIDLable , "1") == 0)
			{
				/*upload Version*/
				if(strcmp(m_IniFileInfo.szUpLoadVersion,"1") == 0)
				{
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-25s%-25s%-12s%-12s%-12s%-12s\r\n", m_strHH, m_strPCBASN, m_strSN, m_strET0MAC, m_str2GMAC, m_str5GMAC, m_strPcName); //for Mice FT
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+SSID(32)+PASS(32)+SSID(32)+PASS(32)+FW(30)+PCNAME(12)
					//AfxMessageBox(szSendSfisData);
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-30s%-32s%-32s%-32s%-32s%-30s%-60s%-12s\r\n", 
						m_strSN ,
						m_strMAC , 
						m_strPincode , 
						m_strCN ,
						szFWforSfis ,
						m_strSSID_2G ,
						m_strPASS_2G ,
						m_strSSID_5G ,
						m_strPASS_5G ,
						szBF,
						szOpenVPN,
						m_strPcName) ;
						*/
				}
				else
				{
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+SSID(32)+PASS(32)+SSID(32)+PASS(32)+PCNAME(12)*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-32s%-32s%-32s%-32s%-30s%-60s%-12s\r\n", 
						m_strSN, 
						m_strMAC, 
						m_strPincode , 
						m_strCN, 
						m_strSSID_2G ,
						m_strPASS_2G ,
						m_strSSID_5G ,
						m_strPASS_5G ,
						szBF,
						szOpenVPN,
						m_strPcName) ;
				}
			}
			else
			{
				/*upload Version*/
				if(strcmp(m_IniFileInfo.szUpLoadVersion,"1") == 0)
				{
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+FW(30)+PCNAME(12)*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-30s%-12s\r\n", m_strSN , m_strMAC , m_strPincode , m_strCN , szFWforSfis , m_strPcName);//
				}
				else
				{
					/*HH(25)+MAC(12)+PIN(12)+SSN(15)+PCNAME(12)*/
					sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-12s\r\n", m_strSN, m_strMAC, m_strPincode, m_strCN, m_strPcName);//ADD
				}
			}
		}
		if(strstr(m_IniFileInfo.szTestStatuse , "RC") || strstr(m_IniFileInfo.szTestStatuse , "NFC") || strstr(m_IniFileInfo.szTestStatuse , "DOWNLOAD"))/*RC SFIS mode*/
		{
			sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-25s%-25s%-12s%-12s%-12s%-12s\r\n", m_strHH, m_strPCBASN, m_strSN, m_strET0MAC, m_str2GMAC, m_str5GMAC, m_strPcName); //for Mice RC
			/*HH(25)+MAC(12)+PIN(12)+SSN(15)+SSID(32)+PASS(32)+SSID(32)+PASS(32)+PCNAME(12)
			sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s%-15s%-32s%-32s%-32s%-32s%-30s%-60s%-12s\r\n", 
				m_strSN, 
				m_strMAC, 
				m_strPincode, 
				m_strCN, 
				m_strSSID_2G ,
				m_strPASS_2G ,
				m_strSSID_5G ,
				m_strPASS_5G ,
				szBF,
				szOpenVPN,
				m_strPcName);
				*/

		}
		if(strstr(m_IniFileInfo.szTestStatuse , "PT2"))/*PT2 SFIS mode*/
		{
			/*HH(25)+MAC(12)+PCNAME(12)*/
			sprintf_s(szSendSfisData, MINBUFSIZE, "%-25s%-12s%-12s\r\n", m_strSN, m_strMAC, m_strPcName);//ADD
		}
		ShowMsg(szSendSfisData);
		m_ComSfis.SendData(szSendSfisData, MINBUFSIZE);		
	}
	else
	{
		TestPass();
	}
}
void CRunInfo::TestFail(char* ErroCode)
{
    ShowMsg("*************** Test Function Fail **********************");
	CRunInfo*pRun=(CRunInfo*)(ErroCode);

	if(!m_blIsSfis)
	{
		m_BtnStart.EnableWindow(true);
		m_BtnQuit.EnableWindow(true);
	}

	m_strErroCode = ErroCode;
	bool  szPostToMydas = true;	
	/*if(strncmp(ErroCode, "BR", 2) ==0 || strncmp(ErroCode, "SR", 2) == 0 || strncmp(ErroCode, "SY", 2) == 0)//add
	{
		
		szPostToMydas =false;		
	}
	*/
	CountTestResult(false);

    char szRecordTestTime[MINBUFSIZE] = "";
	sprintf_s(szRecordTestTime, MINBUFSIZE, "%d(s)", m_TestTimeCount);
	m_strRecordTestData +='\t';
	m_strRecordTestData += szRecordTestTime;	

	m_strTestResult = "FAIL";
	if(m_blIsSfis)
	{
		char szRecordFailFilePath  [MAXBUFSIZE] = "";
		CTime    Time = CTime::GetCurrentTime();
		CString    strFileName = Time.Format("%Y%m%d");
		sprintf_s(szRecordFailFilePath, MAXBUFSIZE, "%s\\%s\\FAIL\\%s.txt", m_IniFileInfo.szStoreSrvPath,m_IniFileInfo.szTestStatuse, strFileName);
		fsFail.open (szRecordFailFilePath   ,fstream::out|fstream::app);
		fsFail<<m_strRecordTestData.GetBuffer(m_strRecordTestData.GetLength())<<endl;
		fsFail.close();
		ShowMsg(" ******Save Fail test log OK*******");
		
		
		if(strstr(m_IniFileInfo.szPostFlag,"1")/* && szPostToMydas*/) 
		{
			if(LinkDataBase())
			{
				ShowMsg("*******************Send FAIL Restust to MYDAS ****************");
				/*Detaillog*/
				sprintf_s(Detaillog,MINBUFSIZE,"%s,%s,%s,",
					m_str2GSSID.Trim(),
					m_str5GSSID.Trim(),
					//m_strMAC.Trim(),
					//m_strPincode.Trim(),
					//m_strCN.Trim(),
					TestDetaillog);
				/*ErrorDefine*/
				char temp[1024] = "";
				CString strErrInfo = ErroCode;
				sprintf_s(temp,1024,"%s,%s,|",strErrInfo.Trim().Left(4),strErrInfo.Trim().Mid(5,strErrInfo.GetLength()-5).Trim());
				strcpy_s(ErrorDefine,sizeof(ErrorDefine),temp);
				/*MainData*/
				SYSTEMTIME SystemTime;
				GetSystemTime(&SystemTime);
				char time[MINBUFSIZE] = "";
				sprintf_s(time,MINBUFSIZE,"%d-%d-%d %d:%d:%d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
				sprintf_s(MainData,MINBUFSIZE,"%s,0,%s,V0.0.1,%s,%s,%s,%s,%s,%s,%s,%s,%s,",
					m_strSN.Trim(),
					szRecordFailFilePath,
					m_strPCBASN.Trim(),
					m_strHH.Trim(),
					m_strPcName,
					m_TestTimeCount.Format("%H%M%S"),
					time,
					m_strET0MAC.Trim(),
					m_str2GMAC.Trim(),
					m_str5GMAC.Trim(),
					m_strPNNAME.Trim());
				if(!SendDatatoMYDAS(Detaillog,ErrorDefine,MainData))
				{
					AfxMessageBox("Send Information to MYDAS Fail...");
				}
			}
			else
			{
				AfxMessageBox("Link to MYDAS Fail...");
			}
		}
	}

	/*Recode fail test UI log*/
	char sztemp[1024] = "";
	CString strErrInfo = ErroCode;
	sprintf_s(sztemp,1024,"%s",strErrInfo.Trim().Left(4));

	char szRecordFailFilePath_UI  [MAXBUFSIZE] = "";
	sprintf_s(szRecordFailFilePath_UI, MAXBUFSIZE, "D:\\FAIL\\%s_%s.txt", m_strHH.Trim() , sztemp );
	fsFail.open (szRecordFailFilePath_UI   ,fstream::out|fstream::app);
	fsFail<<strRunInfo.GetBuffer(strRunInfo.GetLength())<<endl;
	fsFail.close();
	/*End*/

	KillTimer(1);
	IsDisplayErrorCode(true);
	UpdateData(false);

	/*
	m_strSN = "";
	m_strMAC = "";
	m_strPincode = "";
	m_strCN = "";
	*/
	DelArp();
	m_strHH="";
	m_strPCBASN="";
	m_strSN = "";
	m_strET0MAC="";
	m_str2GMAC="";
	m_str5GMAC="";
	m_str2GSSID="";
	m_str5GSSID="";
	m_strPNNAME="";

	if(m_Socket!=INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	//comment by ygc
	/*
	shutdown(m_GoldenSocket , 0);	
	if(m_GoldenSocket!=INVALID_SOCKET)		
	{
		closesocket(m_GoldenSocket);
		m_GoldenSocket=INVALID_SOCKET;
	}
	*/
	return;
}

void CRunInfo::TestPass()
{
    ShowMsg("*************** Test Function PASS **********************");
	CRunInfo*pRun=(CRunInfo*)(NULL) ;

	if(!m_blIsSfis)
	{
		m_BtnStart.EnableWindow(true);
		m_BtnQuit.EnableWindow(true);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "RUN-IN"))
	{
		m_strTestResult = "Start RUN-IN OK";
	}
	else
	{
		m_strTestResult = "PASS";
	}
	char szRecordTestTime[MINBUFSIZE] = "";
	sprintf_s(szRecordTestTime, MINBUFSIZE, "%d(s)", m_TestTimeCount);
	m_strRecordTestData +='\t';
	m_strRecordTestData += szRecordTestTime;
	if(m_blIsSfis)
	{
		char       szRecordPassFilePath[MAXBUFSIZE] = "";
		CTime      Time = CTime::GetCurrentTime();
		CString    strFileName = Time.Format("%Y%m%d");
		sprintf_s(szRecordPassFilePath, MAXBUFSIZE, "%s\\%s\\PASS\\%s.txt", m_IniFileInfo.szStoreSrvPath,m_IniFileInfo.szTestStatuse, strFileName);
		fsPass.open(szRecordPassFilePath,fstream::out|fstream::app);
		fsPass<<m_strRecordTestData.GetBuffer(m_strRecordTestData.GetLength())<<endl;

		fsPass.close();
		ShowMsg(" ******Save Pass test log OK*******");
		if(strstr(m_IniFileInfo.szPostFlag,"1"))
		{
			if(LinkDataBase())
			{
				ShowMsg("*******************Send PASS Restust to MYDAS ****************");
				/*Detaillog*/
				sprintf_s(Detaillog,MINBUFSIZE,"%s,%s,%s,",
					m_str2GSSID.Trim(),
					m_str5GSSID.Trim(),
					//m_strMAC.Trim(),
					//m_strPincode.Trim(),
					//m_strCN.Trim(),
					TestDetaillog);
				/*ErrorDefine*/
				sprintf_s(ErrorDefine,",,|");
				/*MainData*/
				SYSTEMTIME SystemTime;
				GetSystemTime(&SystemTime);
				char time[MINBUFSIZE] = "";
				sprintf_s(time,MINBUFSIZE,"%d-%d-%d %d:%d:%d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
				sprintf_s(MainData,MINBUFSIZE,"%s,1,%s,V0.0.1,%s,%s,%s,%s,%s,%s,%s,%s,%s,",
					m_strSN.Trim(),
					szRecordPassFilePath,
					m_strPCBASN.Trim(),
					m_strHH.Trim(),
					m_strPcName,
					m_TestTimeCount.Format("%H%M%S"),
					time,
					m_strET0MAC.Trim(),
					m_str2GMAC.Trim(),
					m_str5GMAC.Trim(),
					m_strPNNAME.Trim());
				if(!SendDatatoMYDAS(Detaillog,ErrorDefine,MainData))
				{
					AfxMessageBox("Send Information to MYDAS Fail...");
				}
			}
			else
			{
				AfxMessageBox("Link to MYDAS Fail...");
			}
		}
	}

	/*Recode pass test UI log*/
	char szRecordPassFilePath_UI  [MAXBUFSIZE] = "";	

	sprintf_s(szRecordPassFilePath_UI, MAXBUFSIZE, "D:\\PASS\\%s.txt", m_strHH.Trim());
	fsPass.open (szRecordPassFilePath_UI   ,fstream::out|fstream::app);
	fsPass<<strRunInfo.GetBuffer(strRunInfo.GetLength())<<endl;
	fsPass.close();
	/*End*/

	KillTimer(1);
	CountTestResult(true);	
	UpdateData(false);

	/*
	m_strSN = "";
	m_strMAC = "";
	m_strPincode = "";
	m_strCN = "";
	*/
	DelArp();
	m_strSN = "";
	m_strHH="";
	m_strPCBASN="";
	m_strET0MAC="";
	m_str2GMAC="";
	m_str5GMAC="";
	m_str2GSSID="";
	m_str5GSSID="";
	m_strPNNAME="";

	shutdown(m_Socket , 0);
	if(m_Socket!=INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	
	//comment by ygc
	/*
	shutdown(m_GoldenSocket , 0);	
	if(m_GoldenSocket!=INVALID_SOCKET)		
	{
		closesocket(m_GoldenSocket);
		m_GoldenSocket=INVALID_SOCKET;
	}
	*/

	return;
}

void CRunInfo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_TestTimeCount+=1;
	CString strTimeCount = m_TestTimeCount.Format("%M:%S");	
	m_strTestTime = strTimeCount;
	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}

int CRunInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	return 0;
}


void CRunInfo::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

bool CRunInfo::ReadIniFileForGeneric()
{
	CString  strReadLine;
	char     *token = NULL;
	char     *nextToken = NULL;
	char     Seps[] = "= \t";
	char     szTitleFomat[MINBUFSIZE] = "";
	bool     bProductName = false;
	sprintf_s(szTitleFomat,MINBUFSIZE, "[%s]", "General");
	try
	{
		CStdioFile InitFile(m_IniFileInfo.szTestInfoFileName,CFile::modeRead);
		ShowMsg("********ReadIniFileForGeneric**********");
		while(InitFile.ReadString(strReadLine)&&strcmp(szTitleFomat, strReadLine))
			;
		while(InitFile.ReadString(strReadLine))
		{
			if(strReadLine.GetAt(0) == ';')
				continue;
			if(strReadLine.IsEmpty())
				break;
			_strupr_s(strReadLine.GetBuffer(strReadLine.GetLength()), strReadLine.GetLength()+1);  //add
			token = strtok_s(strReadLine.GetBuffer(strReadLine.GetLength()), Seps, &nextToken);
			while(token != NULL)
			{
				if(!_tcscmp(token,"TEST_CHANNEL_NUM_2G_1"))
				{
					bProductName = true;
					char  szChannelNum[MINBUFSIZE] = "";
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
						strcpy_s(szChannelNum, MINBUFSIZE, token);
					m_IniFileInfo.nChannelNum_2G_1 = atoi(szChannelNum);
					if(m_IniFileInfo.nChannelNum_2G_1 < 0 ||  m_IniFileInfo.nChannelNum_2G_1 >3)
					{
						m_IniFileInfo.nChannelNum_2G_1 = 3;
					}
					break;
				}
				else if(!_tcscmp(token,"TEST_CHANNEL_NUM_5G_1"))
				{
					bProductName = true;
					char  szChannelNum[MINBUFSIZE] = "";
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
						strcpy_s(szChannelNum, MINBUFSIZE, token);
					m_IniFileInfo.nChannelNum_5G_1 = atoi(szChannelNum);
					if(m_IniFileInfo.nChannelNum_5G_1 < 0 ||  m_IniFileInfo.nChannelNum_5G_1 >3)
					{
						m_IniFileInfo.nChannelNum_5G_1 = 3;
					}
					break;
				}
				else if(!_tcscmp(token,"TEST_CHANNEL_NUM_2G_2"))
				{
					bProductName = true;
					char  szChannelNum[MINBUFSIZE] = "";
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
						strcpy_s(szChannelNum, MINBUFSIZE, token);
					m_IniFileInfo.nChannelNum_2G_2 = atoi(szChannelNum);
					if(m_IniFileInfo.nChannelNum_2G_2 < 0 ||  m_IniFileInfo.nChannelNum_2G_2 >3)
					{
						m_IniFileInfo.nChannelNum_2G_2 = 3;
					}
					break;
				}
				else if(!_tcscmp(token,"TEST_CHANNEL_NUM_5G_2"))
				{
					bProductName = true;
					char  szChannelNum[MINBUFSIZE] = "";
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
						strcpy_s(szChannelNum, MINBUFSIZE, token);
					m_IniFileInfo.nChannelNum_5G_2 = atoi(szChannelNum);
					if(m_IniFileInfo.nChannelNum_5G_2 < 0 ||  m_IniFileInfo.nChannelNum_5G_2 >3)
					{
						m_IniFileInfo.nChannelNum_5G_2 = 3;
					}
					break;
				}
				else if(!_tcscmp(token,"TQMED"))
				{
					bProductName = true;
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
						strcpy_s(m_IniFileInfo.szAllowBurn, MINBUFSIZE, token);
					break;

				}	
				else if(!_tcscmp(token,"SET_CHANNEL"))
				{
					bProductName = true;
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
						strcpy_s(m_IniFileInfo.szSetChannel, MINBUFSIZE, token);
					break;
				}
				else if (!_tcscmp(token,"MAC1"))
				{
					bProductName  = true;
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
					{
						strcpy_s(m_IniFileInfo.szMac1, MINBUFSIZE, token);
					}
					break;
				}
				else if (!_tcscmp(token,"MAC2"))
				{
					bProductName  = true;
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
					{
						strcpy_s(m_IniFileInfo.szMac2, MINBUFSIZE, token);
					}
					break;
				}
				else if (!_tcscmp(token,"MAC3"))
				{
					bProductName  = true;
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
					{
						strcpy_s(m_IniFileInfo.szMac3, MINBUFSIZE, token);
					}
					break;
				}
				else if (!_tcscmp(token,"TEST_STATION"))
				{
					bProductName  = true;
					token = _tcstok_s(NULL,Seps,&nextToken);
					if(token!= NULL)
					{
						strcpy_s(m_IniFileInfo.szTestStatuse, MINBUFSIZE, token);
						m_Station=m_IniFileInfo.szTestStatuse;
						UpdateData(false);
						ShowMsg(m_IniFileInfo.szTestStatuse);
					}
					break;
				}
				else
					break;
			}
		}
		InitFile.Close();
	}
	catch (CFileException* e) 
	{
		if( e->m_cause == CFileException::fileNotFound )
		{
			return 0; 
		}
		if( e->m_cause == CFileException::accessDenied )
		{
			return 0; 
		}
	}
	if(!bProductName)
	{
		return 0;
	}
	ShowMsg("***********Read General config file end ******");
	return 1;
}

bool CRunInfo::ReadIniFile()
{
	char szDutName[128]="",szTemp[256]="";
	char szProPath[MINBUFSIZE]="";
	if(m_blIsSfis)
	{
		m_strProductName = m_strPNNAME;
	}
	m_strProductName.Trim();
	
	//MessageBox(m_strProductName);

	ShowMsg("********ReadIniFile**********");
	lstrcpy(szDutName,m_strProductName.Trim().GetBuffer(m_strProductName.GetLength()));
	GetCurrentDirectory(sizeof(szProPath),szProPath);

	//MessageBox(szDutName);

	//char szBuf[128] = "";
	//sprintf_s(szBuf , sizeof(szBuf) , "\\%s" , m_IniFileInfo.szTestInfoFileName);

	lstrcat(szProPath,"\\");
	lstrcat(szProPath,m_IniFileInfo.szTestInfoFileName);
	GetPrivateProfileString(szDutName,"Product_Name","",szTemp,MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Product_Name","",m_IniFileInfo.szPNNAME,MINBUFSIZE,szProPath);
	ShowMsg(m_IniFileInfo.szPNNAME);
	//MessageBox(szTemp);
	if(strlen(szTemp)<=0)
	{
		return 0; // no such dut name found.
	}
	//MessageBox("11111");
	GetPrivateProfileString(szDutName,"FixIP_Address","",m_IniFileInfo.szStoreFixIPAddress, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Firmware","",m_IniFileInfo.szStoreFirmware, MINBUFSIZE,szProPath);
	//for mice version check
	GetPrivateProfileString(szDutName,"APPVER","",m_IniFileInfo.nMiceAppVer, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"CFEVER","",m_IniFileInfo.nMiceCfeVer, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"KERVER","",m_IniFileInfo.nMiceKerVer, MINBUFSIZE,szProPath);
	//end
	GetPrivateProfileString(szDutName,"BoardID","",m_IniFileInfo.szStoreBoardID, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"CURRENT_TEST","0",m_IniFileInfo.szCurrentTest, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Moduleinfor","",m_IniFileInfo.szModuleInfor, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LOCK_CODE","1",m_IniFileInfo.szLockCode, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"ModuleVersion","",m_IniFileInfo.szModuleVersion, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Record_File_Path","",m_IniFileInfo.szStoreSrvPath, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Iperf_para","",m_IniFileInfo.szStoreIperfPara, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Iperf_para(FAI)","",m_IniFileInfo.szStoreIperfParaFAI, MINBUFSIZE,szProPath);

	GetPrivateProfileString(szDutName,"Iperf_para_D","",m_IniFileInfo.szStoreIperfPara_D, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Iperf_para_R","",m_IniFileInfo.szStoreIperfPara_R, MINBUFSIZE,szProPath);


	GetPrivateProfileString(szDutName,"REWORK","",m_IniFileInfo.szRework, MINBUFSIZE,szProPath);

	GetPrivateProfileString(szDutName,"TFTP_Address","",m_IniFileInfo.szTftpAddress, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"OpenVPN_Key_Path","",m_IniFileInfo.szVPNkeyPath, MINBUFSIZE,szProPath);
	
	GetPrivateProfileString(szDutName,"TEST_RSSI","",m_IniFileInfo.szStoreTestRssi, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"DEFAULT_IP1","192.168.1.1",m_IniFileInfo.szDefaultIP1, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"DEFAULT_IP2","192.168.1.250",m_IniFileInfo.szDefaultIP2, MINBUFSIZE,szProPath);	

	GetPrivateProfileString(szDutName,"Test_ADSL","1",m_IniFileInfo.szStoreTestAdsl, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"CHECK_VERSION","1",m_IniFileInfo.szCheckVersion, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"CHECK_FAN","1",m_IniFileInfo.szCheckFAN, MINBUFSIZE,szProPath);

	GetPrivateProfileString(szDutName,"UpLoad_VERSION","1",m_IniFileInfo.szUpLoadVersion, MINBUFSIZE,szProPath);//wheather upload FW
	
	GetPrivateProfileString(szDutName,"SSID_Lable","0",m_IniFileInfo.szIsHaveSSIDLable, MINBUFSIZE,szProPath);//wheather have SSID lable
	
	GetPrivateProfileString(szDutName,"Test_Rssi","1",m_IniFileInfo.szStoreTestRssi, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"POST_MYDAS_FLAG","1",m_IniFileInfo.szPostFlag, MINBUFSIZE,szProPath);//wheather post Mydas
	GetPrivateProfileString(szDutName,"Disable_Wireless","1",m_IniFileInfo.szDisableWireless, MINBUFSIZE,szProPath);//wheather Disable_Wireless
	GetPrivateProfileString(szDutName,"Region_Code","0",m_IniFileInfo.szRegionCode, MINBUFSIZE,szProPath);//burn region code
	GetPrivateProfileString(szDutName,"Region_Code_Flag","0",m_IniFileInfo.szRegionCodeFlag, MINBUFSIZE,szProPath);//burn region code flag

	m_IniFileInfo.nPingDUTCount=GetPrivateProfileInt(szDutName,"Ping_DUT_Count",2,szProPath);
	m_IniFileInfo.nPingGoldenCount=GetPrivateProfileInt(szDutName,"Ping_Golden_Count",5,szProPath);

	/*ThroughPut */
	
	m_IniFileInfo.nGoldenSeverFlag = GetPrivateProfileInt(szDutName,"GoldenSeverFlag",0,szProPath);
	m_IniFileInfo.nTestThroughput = GetPrivateProfileInt(szDutName,"2GTHROUGHPUT",1,szProPath);
	m_IniFileInfo.GnTestThroughput = GetPrivateProfileInt(szDutName,"5GTHROUGHPUT",1,szProPath);

	GetPrivateProfileString(szDutName,"Test_Throughput","1",m_IniFileInfo.szStoreTestThroughput, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"Test_Throughput_Single","0",m_IniFileInfo.szStoreTestThroughputSingle, MINBUFSIZE,szProPath);

	m_IniFileInfo.nRetryNum=GetPrivateProfileInt(szDutName,"Retry_Num",8,szProPath);

	m_IniFileInfo.nTXThroughputSpec=GetPrivateProfileInt(szDutName,"2G_TX_Throughput_Sep",110,szProPath);
	m_IniFileInfo.nRXThroughputSpec=GetPrivateProfileInt(szDutName,"2G_RX_Throughput_Sep",110,szProPath);
	m_IniFileInfo.nTXRXThroughputSpec=GetPrivateProfileInt(szDutName,"2G_TXRX_Throughput_Sep",110,szProPath);

	m_IniFileInfo.nTXThroughputSpec5G=GetPrivateProfileInt(szDutName,"5G_TX_Throughput_Sep",130,szProPath);
	m_IniFileInfo.nRXThroughputSpec5G=GetPrivateProfileInt(szDutName,"5G_RX_Throughput_Sep",130,szProPath);
	m_IniFileInfo.nTXRXThroughputSpec5G=GetPrivateProfileInt(szDutName,"5G_TXRX_Throughput_Sep",150,szProPath);

	m_IniFileInfo.nTXThroughputSpecHigt=GetPrivateProfileInt(szDutName,"2G_TX_Throughput_Sep_Higt",300,szProPath);
	m_IniFileInfo.nRXThroughputSpecHigt=GetPrivateProfileInt(szDutName,"2G_RX_Throughput_Sep_Higt",300,szProPath);
	m_IniFileInfo.nTXRXThroughputSpecHigt=GetPrivateProfileInt(szDutName,"2G_TXRX_Throughput_Sep_Higt",300,szProPath);

	m_IniFileInfo.nTXThroughputSpecHigt5G=GetPrivateProfileInt(szDutName,"5G_TX_Throughput_Sep_Higt",450,szProPath);
	m_IniFileInfo.nRXThroughputSpecHigt5G=GetPrivateProfileInt(szDutName,"5G_RX_Throughput_Sep_Higt",450,szProPath);
	m_IniFileInfo.nTXRXThroughputSpecHigt5G=GetPrivateProfileInt(szDutName,"5G_TXRX_Throughput_Sep_Higt",450,szProPath);
	
	m_IniFileInfo.nTXThroughputSpec_2=GetPrivateProfileInt(szDutName,"2G_TX_Throughput_Sep_2",110,szProPath);
	m_IniFileInfo.nRXThroughputSpec_2=GetPrivateProfileInt(szDutName,"2G_RX_Throughput_Sep_2",110,szProPath);
	m_IniFileInfo.nTXRXThroughputSpec_2=GetPrivateProfileInt(szDutName,"2G_TXRX_Throughput_Sep_2",110,szProPath);

	m_IniFileInfo.nTXThroughputSpec5G_2=GetPrivateProfileInt(szDutName,"5G_TX_Throughput_Sep_2",130,szProPath);
	m_IniFileInfo.nRXThroughputSpec5G_2=GetPrivateProfileInt(szDutName,"5G_RX_Throughput_Sep_2",130,szProPath);
	m_IniFileInfo.nTXRXThroughputSpec5G_2=GetPrivateProfileInt(szDutName,"5G_TXRX_Throughput_Sep_2",150,szProPath);

	m_IniFileInfo.nTXThroughputSpecHigt_2=GetPrivateProfileInt(szDutName,"2G_TX_Throughput_Sep_Higt_2",300,szProPath);
	m_IniFileInfo.nRXThroughputSpecHigt_2=GetPrivateProfileInt(szDutName,"2G_RX_Throughput_Sep_Higt_2",300,szProPath);
	m_IniFileInfo.nTXRXThroughputSpecHigt_2=GetPrivateProfileInt(szDutName,"2G_TXRX_Throughput_Sep_Higt_2",300,szProPath);

	m_IniFileInfo.nTXThroughputSpecHigt5G_2=GetPrivateProfileInt(szDutName,"5G_TX_Throughput_Sep_Higt_2",450,szProPath);
	m_IniFileInfo.nRXThroughputSpecHigt5G_2=GetPrivateProfileInt(szDutName,"5G_RX_Throughput_Sep_Higt_2",450,szProPath);
	m_IniFileInfo.nTXRXThroughputSpecHigt5G_2=GetPrivateProfileInt(szDutName,"5G_TXRX_Throughput_Sep_Higt_2",450,szProPath);

	GetPrivateProfileString("General","2G_Throtghput_Golden_SSID","0",m_IniFileInfo.sz2GThrotghputGoldenSSID, MINBUFSIZE,".\\Throughput_Setting.ini");//2G Throtghput Golden SSID
	GetPrivateProfileString("General","5G_Throtghput_Golden_SSID","0",m_IniFileInfo.sz5GThrotghputGoldenSSID, MINBUFSIZE,".\\Throughput_Setting.ini");//5G Throtghput Golden SSID
	
	GetPrivateProfileString("General","2G_Throtghput_Golden_IP","0",m_IniFileInfo.sz2GThrotghputGoldenIP, MINBUFSIZE,".\\Throughput_Setting.ini");//2G Throtghput Golden IP
	GetPrivateProfileString("General","5G_Throtghput_Golden_IP","0",m_IniFileInfo.sz5GThrotghputGoldenIP, MINBUFSIZE,".\\Throughput_Setting.ini");//2G Throtghput Golden IP

	m_IniFileInfo.nTXThroughputSpec_Factroy=GetPrivateProfileInt(szDutName,"2_T_TP_S_F",220,szProPath);
	m_IniFileInfo.nRXThroughputSpec_Factroy=GetPrivateProfileInt(szDutName,"2_R_TP_S_F",220,szProPath);
	m_IniFileInfo.nTXRXThroughputSpec_Factroy=GetPrivateProfileInt(szDutName,"2_TR_TP_S_F",0,szProPath);

	m_IniFileInfo.nTXThroughputSpec5G_Factroy=GetPrivateProfileInt(szDutName,"5_T_TP_S_F",320,szProPath);
	m_IniFileInfo.nRXThroughputSpec5G_Factroy=GetPrivateProfileInt(szDutName,"5_R_TP_S_F",320,szProPath);
	m_IniFileInfo.nTXRXThroughputSpec5G_Factroy=GetPrivateProfileInt(szDutName,"5_TR_TP_S_F",0,szProPath);

	m_IniFileInfo.nThroughputFalg_Factroy=GetPrivateProfileInt(szDutName,"2_TP_F_F",0,szProPath);
	m_IniFileInfo.nThroughputFalg5G_Factroy=GetPrivateProfileInt(szDutName,"5_TP_F_F",0,szProPath);

	m_IniFileInfo.nThroughputRange_Factroy=GetPrivateProfileInt(szDutName,"2_TP_R_F",40,szProPath);
	m_IniFileInfo.nThroughputRange5G_Factroy=GetPrivateProfileInt(szDutName,"5_TP_R_F",70,szProPath); 

	/*End*/

	m_IniFileInfo.MURssiSpec=GetPrivateProfileInt(szDutName,"EXTERION_U",0,szProPath);
	m_IniFileInfo.MDRssiSpec=GetPrivateProfileInt(szDutName,"EXTERION_D",0,szProPath);
	m_IniFileInfo.NURssiSpec=GetPrivateProfileInt(szDutName,"NO_EXTERION_U",0,szProPath);
	m_IniFileInfo.NDRssiSpec=GetPrivateProfileInt(szDutName,"NO_EXTERION_D",0,szProPath);
	m_IniFileInfo.DeltaUPSpec=GetPrivateProfileInt(szDutName,"DELTA_UP",0,szProPath);
	m_IniFileInfo.DeltaDownSpec=GetPrivateProfileInt(szDutName,"DELTA_DOWN",0,szProPath);
	m_IniFileInfo.nDSLUpStream=GetPrivateProfileInt(szDutName,"ADSL_Upstream",1000000,szProPath);
	m_IniFileInfo.nDSLDownStream=GetPrivateProfileInt(szDutName,"ADSL_Downstream",20500000,szProPath);
	m_IniFileInfo.nTimeOut=GetPrivateProfileInt(szDutName,"TIMEOUT",15000,szProPath);
	m_IniFileInfo.nSleepOrNo=GetPrivateProfileInt(szDutName,"SLEEP",0,szProPath);

	/*USB*/	
	m_IniFileInfo.nTestUsb=GetPrivateProfileInt(szDutName,"USB_TEST_FLAG",0,szProPath);	//USB dect flag
	GetPrivateProfileString(szDutName,"USB_DECT_STRING","",m_IniFileInfo.szUsbDectString, MINBUFSIZE,szProPath);//USB dect string, if have:dect pass	
	GetPrivateProfileString(szDutName,"USB_DECT_CMD","",m_IniFileInfo.szUsbDectCmd, MINBUFSIZE,szProPath);//USB dect command	
	//USB 1
	GetPrivateProfileString(szDutName,"USB_RESULT_FILE_1","",m_IniFileInfo.szUsbResultFile_1, MINBUFSIZE,szProPath);//USB throughput test result file name	
	m_IniFileInfo.nUsbTXThroughputFlag_1=GetPrivateProfileInt(szDutName,"USB_TX_FLAG_1",0,szProPath);//USB TX throughput run flag
	m_IniFileInfo.nUsbRXThroughputFlag_1=GetPrivateProfileInt(szDutName,"USB_RX_FLAG_1",0,szProPath);//USB RX throughput run flag
	GetPrivateProfileString(szDutName,"USB_TX_RUN_FILE_1","",m_IniFileInfo.szUsbTXThroughputRunFie_1, MINBUFSIZE,szProPath);//USB TX throughput run file
	GetPrivateProfileString(szDutName,"USB_RX_RUN_FILE_1","",m_IniFileInfo.szUsbRXThroughputRunFie_1, MINBUFSIZE,szProPath);//USB RX throughput run file
	m_IniFileInfo.nUsbTXThroughputSpec_1=GetPrivateProfileInt(szDutName,"USB_TX_SPEC_1",0,szProPath);//USB TX throughput test SPEC
	m_IniFileInfo.nUsbRXThroughputSpec_1=GetPrivateProfileInt(szDutName,"USB_RX_SPEC_1",0,szProPath);//USB RX throughput test SPEC	
	GetPrivateProfileString(szDutName,"USB_FILE_1","data.bin",m_IniFileInfo.szUsbFilename_1, MINBUFSIZE,szProPath);//USB throughput test need file(usually is named:data.bin)

	//USB 2
	GetPrivateProfileString(szDutName,"USB_RESULT_FILE_2","",m_IniFileInfo.szUsbResultFile_2, MINBUFSIZE,szProPath);//USB throughput test result file name	
	m_IniFileInfo.nUsbTXThroughputFlag_2=GetPrivateProfileInt(szDutName,"USB_TX_FLAG_2",0,szProPath);//USB TX throughput run flag
	m_IniFileInfo.nUsbRXThroughputFlag_2=GetPrivateProfileInt(szDutName,"USB_RX_FLAG_2",0,szProPath);//USB RX throughput run flag
	GetPrivateProfileString(szDutName,"USB_TX_RUN_FILE_2","",m_IniFileInfo.szUsbTXThroughputRunFie_2, MINBUFSIZE,szProPath);//USB TX throughput run file
	GetPrivateProfileString(szDutName,"USB_RX_RUN_FILE_2","",m_IniFileInfo.szUsbRXThroughputRunFie_2, MINBUFSIZE,szProPath);//USB RX throughput run file
	m_IniFileInfo.nUsbTXThroughputSpec_2=GetPrivateProfileInt(szDutName,"USB_TX_SPEC_2",0,szProPath);//USB TX throughput test SPEC
	m_IniFileInfo.nUsbRXThroughputSpec_2=GetPrivateProfileInt(szDutName,"USB_RX_SPEC_21",0,szProPath);//USB RX throughput test SPEC
	GetPrivateProfileString(szDutName,"USB_FILE_2","",m_IniFileInfo.szUsbFilename_2, MINBUFSIZE,szProPath);//USB throughput test need file(usually is named:data.bin)


	//USB 2.0 / 3.0
	GetPrivateProfileString(szDutName,"USB_20_FileName","",USB.szUSB20FileName, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"USB_30_FileName","",USB.szUSB30FileName, MINBUFSIZE,szProPath);

	/*Lan Port*/
	m_IniFileInfo.nLanPort_Throughput_Flag=GetPrivateProfileInt(szDutName,"LAN_LAN_FLAG",0,szProPath);
	m_IniFileInfo.nLanPort_Throughout_SPEC_LOW=GetPrivateProfileInt(szDutName,"LAN_LAN_SPEC_LOW",0,szProPath);
	m_IniFileInfo.nLanPort_Throughout_SPEC_HIGH=GetPrivateProfileInt(szDutName,"LAN_LAN_SPEC_HIGH",0,szProPath);
	GetPrivateProfileString(szDutName,"LAN_LAN_IPERF","",m_IniFileInfo.szLanPort_Throughput_Iperf, MINBUFSIZE,szProPath);
	/*End*/

	/*Wan Port*/
	m_IniFileInfo.nWanPort_Flag=GetPrivateProfileInt(szDutName,"WAN_FLAG",0,szProPath);
	m_IniFileInfo.nWanPort_Throughput_Flag=GetPrivateProfileInt(szDutName,"LAN_WAN_FLAG",0,szProPath);	
	m_IniFileInfo.nWanPort_Throughout_SPEC_LOW=GetPrivateProfileInt(szDutName,"LAN_WAN_SPEC_LOW",0,szProPath);
	m_IniFileInfo.nWanPort_Throughout_SPEC_HIGH=GetPrivateProfileInt(szDutName,"LAN_WAN_SPEC_HIGH",0,szProPath);
	GetPrivateProfileString(szDutName,"LAN_WAN_IPERF","",m_IniFileInfo.szWanPort_Throughput_Iperf, MINBUFSIZE,szProPath);

	m_IniFileInfo.nWanPort_Throughput_Flag_Factroy=GetPrivateProfileInt(szDutName,"L_W_T_F",0,szProPath);
	m_IniFileInfo.nWanPort_Throughout_SPEC_LOW_Factroy=GetPrivateProfileInt(szDutName,"L_W_T_S_F",0,szProPath);
	m_IniFileInfo.nWanPort_Throughout_RANGE_Factroy=GetPrivateProfileInt(szDutName,"L_W_T_R_F",100,szProPath);

	/*End*/

	/*check String table*/
	m_IniFileInfo.nCheckSumFlag=GetPrivateProfileInt(szDutName,"CheckSum_Flag",0,szProPath);
	m_IniFileInfo.nCheckSumNum=GetPrivateProfileInt(szDutName,"CheckSum_Num",6,szProPath);

	int iFlag = m_IniFileInfo.nCheckSumNum;
	memset(m_IniFileInfo.szCkecksum[m_IniFileInfo.nCheckSumNum-iFlag],0,MINBUFSIZE);
	for(iFlag ; iFlag > 0 ; iFlag--)
	{	
		memset(m_IniFileInfo.szCkecksum[m_IniFileInfo.nCheckSumNum-iFlag+1],0,MINBUFSIZE);
		char StoreChksum[MINBUFSIZE]="Ckecksum";
		char str[2]="";
		sprintf_s(str,2,"%d",m_IniFileInfo.nCheckSumNum-iFlag+1);
		strcat_s(StoreChksum,sizeof(StoreChksum),str);		
		//Get string table checksum
		GetPrivateProfileString(szDutName,StoreChksum,"",m_IniFileInfo.szCkecksum[m_IniFileInfo.nCheckSumNum-iFlag+1], MINBUFSIZE,szProPath);
	}
	/*End*/


	//read ccd setup param
	LedCcdTestInfo.iLedCCDTestFlag = GetPrivateProfileInt(szDutName,"LED_TEST_FLAG",0,szProPath);
	GetPrivateProfileString(szDutName,"LAN1X","",LedCcdTestInfo.LAN1x, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LAN1Y","",LedCcdTestInfo.LAN1y, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LAN1W","",LedCcdTestInfo.LAN1w, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LAN1H","",LedCcdTestInfo.LAN1h, MINBUFSIZE,szProPath);

	GetPrivateProfileString(szDutName,"LAN2X","",LedCcdTestInfo.LAN2x, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LAN2Y","",LedCcdTestInfo.LAN2y, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LAN2W","",LedCcdTestInfo.LAN2w, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"LAN2H","",LedCcdTestInfo.LAN2h, MINBUFSIZE,szProPath);

	GetPrivateProfileString(szDutName,"WAN1X","",LedCcdTestInfo.WAN1x, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"WAN1Y","",LedCcdTestInfo.WAN1y, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"WAN1W","",LedCcdTestInfo.WAN1w, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"WAN1H","",LedCcdTestInfo.WAN1h, MINBUFSIZE,szProPath);

	GetPrivateProfileString(szDutName,"SYSLX","",LedCcdTestInfo.SYSLx, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"SYSLY","",LedCcdTestInfo.SYSLy, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"SYSLW","",LedCcdTestInfo.SYSLw, MINBUFSIZE,szProPath);
	GetPrivateProfileString(szDutName,"SYSLH","",LedCcdTestInfo.SYSLh, MINBUFSIZE,szProPath);
	//end


	/*disable wifi channel*/
	switch(m_IniFileInfo.nChannelNum_2G_1)
	{
	case 1:
		((CStatic*)GetDlgItem(IDC_STATIC_2G_CHANNEL2))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->EnableWindow(false);
		break;
	case 2:
		break;
	case 3:
		((CStatic*)GetDlgItem(IDC_STATIC_2G_CHANNEL1))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_STATIC_2G_CHANNEL2))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->EnableWindow(false);
		break;
	default:
		break;
	}
	switch(m_IniFileInfo.nChannelNum_5G_1)
	{
	case 1:
		((CStatic*)GetDlgItem(IDC_STATIC_5G_CHANNEL2))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->EnableWindow(false);
		break;
	case 2:
		break;
	case 3:
		((CStatic*)GetDlgItem(IDC_STATIC_5G_CHANNEL1))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_STATIC_5G_CHANNEL2))->EnableWindow(false);
		((CStatic*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->EnableWindow(false);
		break;
	default:
		break;
	}
	/*End*/


	ShowMsg("***********Read config file end ******");
	return 1;
}


int CRunInfo::RunSpecifyExeAndRead(CString& strArpOutInfo,char* RunFileName,bool bIsShow)
{
	strArpOutInfo.Empty();
	HANDLE hReadPipe,hWritePipe;
	SECURITY_ATTRIBUTES  saAttr; 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = true;
	saAttr.lpSecurityDescriptor = NULL;

	if(!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0))
	{
		DisplayRunTimeInfo("Create pipe fail");
		return 0;
	}

	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );

	GetStartupInfo(&siStartInfo);
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = hWritePipe;
	siStartInfo.hStdOutput = hWritePipe;
	siStartInfo.wShowWindow = SW_HIDE;
	siStartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;


	if(!CreateProcess(NULL, RunFileName, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo))
	{
		char error[MINBUFSIZE] = "";
		sprintf_s(error, MAXBUFSIZE, "Run %s application fail", RunFileName);
		DisplayRunTimeInfo(error);
		return 0;
	}


	CloseHandle(hWritePipe);
	CloseHandle(piProcInfo.hThread);
	CloseHandle(piProcInfo.hProcess);


	char buffer[5000] = "";
	DWORD byteRead = 0;


	int i=0;
	for(i=0;i<10;i++)
	{
		memset(buffer, 0, 5000);
		if( !ReadFile( hReadPipe, buffer, 4999, &byteRead, NULL) || (byteRead == 0)) 
		{	
				break;
		}
		strArpOutInfo+=buffer;
		//if(bIsShow)
		//{
			DisplayRunTimeInfo(buffer);	
		//}
		Sleep(200);
	}

	if(i==10){
		DisplayRunTimeInfo("read timeout");
		CloseHandle(hReadPipe);
		return 0;
	}


	CloseHandle(hReadPipe);
	return 1;
}


void CRunInfo::GetMac(IN char* source,OUT char* target)
{
	char  *next_token;
	char  sep[] = _T(" \n\t");
	char  Temp[20];
	char  szMACBuf[100];
	char* tok = _tcstok_s(source,sep,&next_token);
	while(NULL!=tok)
	{
		if(!_tcscmp(tok,m_IniFileInfo.szDefaultIP1))
		{
			tok = _tcstok_s(NULL,sep,&next_token);
			_tcscpy_s(Temp,20,tok);
			int i ,j;
			for(i = 0,j = 0;i < 17;i++)
			{
				if(Temp[i]!='-')
				{
					target[j++]=Temp[i];
				}
			}
			target[j]='\0';
			break;
		}
		tok = _tcstok_s(NULL,sep,&next_token);
	}
	_strupr_s(target,MINBUFSIZE);

	sprintf_s(szMACBuf, MINBUFSIZE, "MAC: %s", target);
	Sleep(1000);
	DisplayRunTimeInfo(szMACBuf);
	DisplayRunTimeInfo("");
}

void CRunInfo::CountTestResult(bool IsPass)
{
	static DWORD dwPass = 0;
	static DWORD dwFail = 0;
	if(IsPass)
	{
		dwPass++;
		TCHAR szTempPass[10] = _T("");
		_stprintf_s(szTempPass, 10, _T("%d"), dwPass);
		SetDlgItemText(IDC_PASS_COUNT, szTempPass);
	}
	else
	{
		dwFail++;
		TCHAR szTempFail[10] = _T("");
		_stprintf_s(szTempFail, 10, _T("%d"), dwFail);
		SetDlgItemText(IDC_FAIL_COUNT, szTempFail);
	}
}

void CRunInfo::DisplayRunTimeInfo(char* pRunInfo)
{
	strRunInfo += pRunInfo;
	strRunInfo += "\r\n";
	if(pRunInfo == NULL)
	{
		strRunInfo = "";
	}
	SetDlgItemText(IDC_EDIT_RUNINFO,strRunInfo.GetBuffer(strRunInfo.GetLength()));
	int a = ((CEdit*)GetDlgItem(IDC_EDIT_RUNINFO))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT_RUNINFO))->LineScroll(a,0);
}
void CRunInfo::DisplayRunTimeInfo(char* pRunInfo, double iData)
{
	char szBuf[1024];
	memset(szBuf, 0 , 1024);
	sprintf_s(szBuf, sizeof(szBuf), "%s:%.2f\r\n", pRunInfo, iData);
	strRunInfo += szBuf;
	SetDlgItemText(IDC_EDIT_RUNINFO,strRunInfo.GetBuffer(strRunInfo.GetLength()));
	int a = ((CEdit*)GetDlgItem(IDC_EDIT_RUNINFO))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT_RUNINFO))->LineScroll(a,0);
}

void CRunInfo::DisplayRunTimeInfo(char* pRunInfo, char* pData)
{
	char szBuf[1024];
	memset(szBuf, 0 , 1024);
	sprintf_s(szBuf, sizeof(szBuf), "%s:%s\r\n", pRunInfo, pData);
	strRunInfo += szBuf;
	SetDlgItemText(IDC_EDIT_RUNINFO,strRunInfo.GetBuffer(strRunInfo.GetLength()));
	int a = ((CEdit*)GetDlgItem(IDC_EDIT_RUNINFO))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT_RUNINFO))->LineScroll(a,0);
}

void CRunInfo::IsDisplayErrorCode(bool Flag)
{
	if(Flag)
	{
		((CStatic*)GetDlgItem(IDC_ERRORCODE))->ShowWindow(SW_SHOW);
	}
	else
	{
		((CStatic*)GetDlgItem(IDC_ERRORCODE))->ShowWindow(SW_HIDE);
	}
}


void CRunInfo::SendCmdToGolden(char* pCmd)
{
	/*
	CStringA strSendData;
	DWORD    dwNumber;	
	WSABUF   sendbuf;
	sendbuf.buf=new char[300];
	sendbuf.len=300;
	memset(sendbuf.buf,0,300);
	
	strSendData  =pCmd;
	strSendData +="\n";
	
	strcpy_s(sendbuf.buf,300,strSendData.GetBuffer(strSendData.GetLength()));
	
	WSASend(m_GoldenSocket,&sendbuf,1,&dwNumber,0,NULL,NULL);
	
	delete []sendbuf.buf;
	*/
	//modify by ygc for RF Test
	m_GoldenSocket=socket(AF_INET,SOCK_DGRAM,0);
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP2))->GetAddress(dwIP);


	SOCKADDR_IN addrTo;
	addrTo.sin_family=AF_INET;
	addrTo.sin_port=htons(20000);
	//addrTo.sin_addr.S_un.S_addr=htonl(dwIP);
	addrTo.sin_addr.S_un.S_addr=inet_addr("192.168.31.5");

	CString strSend;
	strSend = pCmd;
	sendto(m_GoldenSocket,strSend,strSend.GetLength()+1,0,
		(SOCKADDR*)&addrTo,sizeof(SOCKADDR));
}

void CRunInfo::SendCmdTo5GGolden(char* pCmd)
{
	/*
	CStringA strSendData;
	DWORD    dwNumber;	
	WSABUF   sendbuf;
	sendbuf.buf=new char[300];
	sendbuf.len=300;
	memset(sendbuf.buf,0,300);
	
	strSendData  =pCmd;
	strSendData +="\n";
	
	strcpy_s(sendbuf.buf,300,strSendData.GetBuffer(strSendData.GetLength()));
	
	WSASend(m_GoldenSocket,&sendbuf,1,&dwNumber,0,NULL,NULL);
	
	delete []sendbuf.buf;
	*/
	//modify by ygc for RF Test
	//Init5GSocket("192.168.31.5");
	//m5G_GoldenSocket=socket(AF_INET,SOCK_DGRAM,0);
	DWORD dwIP;
	//((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP2))->GetAddress(dwIP);


	sockaddr_in  SocketAddr;
	m5G_Socket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0);
	if(INVALID_SOCKET ==m5G_Socket)
	{
		DisplayRunTimeInfo("Create socket fail\n");
		return;
	}	
	SocketAddr.sin_family = AF_INET;
	//SocketAddr.sin_addr.S_un.S_addr = inet_addr(INI.GEN.szDefaultIP);
	//SocketAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.1");
	SocketAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.5");
	SocketAddr.sin_port = htons(23);
	int revt = connect(m5G_Socket,(sockaddr*)&SocketAddr,sizeof(sockaddr));
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("Create socket fail1\n");
		return;
	}


	//SOCKADDR_IN addrTo;
	//addrTo.sin_family=AF_INET;
	//addrTo.sin_port=htons(23);
	////addrTo.sin_addr.S_un.S_addr=htonl(dwIP);
	//addrTo.sin_addr.S_un.S_addr=inet_addr("192.168.31.5");
	//int revt = connect(m5G_GoldenSocket,(sockaddr*)&addrTo,sizeof(sockaddr));

	DisplayRunTimeInfo("cmd:%s",pCmd);
	CString strSend;
	strSend = pCmd;

	CStringA strSendData;
	DWORD    dwNumber;
	WSABUF   sendbuf;
	sendbuf.buf=new char[300];
	sendbuf.len=300;
	memset(sendbuf.buf,0,300);
	
	strSendData  =pCmd;
	strSendData +="\n";
	
	strcpy_s(sendbuf.buf,300,strSendData.GetBuffer(strSendData.GetLength()+1));
	
	WSASend(m5G_Socket,&sendbuf,1,&dwNumber,0,NULL,NULL);


	/*sendto(m5G_GoldenSocket,strSend,strSend.GetLength()+1,0,
		(SOCKADDR*)&SocketAddr,sizeof(SOCKADDR));*/
}

void CRunInfo::SendCmdTo2GGolden(char* pCmd)
{
	/*
	CStringA strSendData;
	DWORD    dwNumber;	
	WSABUF   sendbuf;
	sendbuf.buf=new char[300];
	sendbuf.len=300;
	memset(sendbuf.buf,0,300);
	
	strSendData  =pCmd;
	strSendData +="\n";
	
	strcpy_s(sendbuf.buf,300,strSendData.GetBuffer(strSendData.GetLength()));
	
	WSASend(m_GoldenSocket,&sendbuf,1,&dwNumber,0,NULL,NULL);
	
	delete []sendbuf.buf;
	*/
	//modify by ygc for RF Test
	//Init5GSocket("192.168.31.5");
	//m5G_GoldenSocket=socket(AF_INET,SOCK_DGRAM,0);
	DWORD dwIP;
	//((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP2))->GetAddress(dwIP);


	sockaddr_in  SocketAddr;
	m2G_Socket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0);
	if(INVALID_SOCKET ==m2G_Socket)
	{
		DisplayRunTimeInfo("Create socket fail\n");
		return;
	}	
	SocketAddr.sin_family = AF_INET;
	//SocketAddr.sin_addr.S_un.S_addr = inet_addr(INI.GEN.szDefaultIP);
	//SocketAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.1");
	SocketAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.2");
	SocketAddr.sin_port = htons(23);
	int revt = connect(m2G_Socket,(sockaddr*)&SocketAddr,sizeof(sockaddr));
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("Create socket fail1\n");
		return;
	}


	//SOCKADDR_IN addrTo;
	//addrTo.sin_family=AF_INET;
	//addrTo.sin_port=htons(23);
	////addrTo.sin_addr.S_un.S_addr=htonl(dwIP);
	//addrTo.sin_addr.S_un.S_addr=inet_addr("192.168.31.5");
	//int revt = connect(m5G_GoldenSocket,(sockaddr*)&addrTo,sizeof(sockaddr));

	DisplayRunTimeInfo("cmd:%s",pCmd);
	CString strSend;
	strSend = pCmd;

	CStringA strSendData;
	DWORD    dwNumber;
	WSABUF   sendbuf;
	sendbuf.buf=new char[300];
	sendbuf.len=300;
	memset(sendbuf.buf,0,300);
	
	strSendData  =pCmd;
	strSendData +="\n";
	
	strcpy_s(sendbuf.buf,300,strSendData.GetBuffer(strSendData.GetLength()+1));
	
	WSASend(m2G_Socket,&sendbuf,1,&dwNumber,0,NULL,NULL);


	/*sendto(m5G_GoldenSocket,strSend,strSend.GetLength()+1,0,
		(SOCKADDR*)&SocketAddr,sizeof(SOCKADDR));*/
}


bool  CRunInfo::SendGoldenCommand(char *pCmd, char *pRtn, int iDelay)
{ 
	h_golenEvt = CreateEvent(NULL,true,false,NULL);
	ResetEvent(h_golenEvt);
	m_strStoreGoldenData ="";

	GoldenInfo.szReturnInfo = pRtn;
	GoldenInfo.nDelay = iDelay;
	GoldenInfo.GoldenSocket = m_GoldenSocket;
	GoldenInfo.szGoldenData = m_strStoreGoldenData;
	GoldenInfo.bResponeOK = false;

	if(GoldenInfo.bConnectOK == true)
	{	
		/*send*/
		//os<<"enter\n";os.flush();
		CStringA strSendData;
		DWORD    dwNumber;	
		WSABUF   sendbuf;
		sendbuf.buf=new char[1024];
		sendbuf.len=1024;
		memset(sendbuf.buf,0,sendbuf.len);

		strcpy_s(sendbuf.buf,sendbuf.len,pCmd);	
		try
		{
			WSASend(m_GoldenSocket,&sendbuf,1,&dwNumber,0,NULL,NULL);
			//os<<"send out\n";os.flush();
		}
		catch(...)
		{
			pRunInfo->DisplayRunTimeInfo("WSASend error!");
		}

		delete []sendbuf.buf;


		ResetEvent(h_golenEvt);

		//OnRecvGoldenData = sendcommand + recv + commpare
		AfxBeginThread(OnRecvGoldenData, &GoldenInfo);
		if(WAIT_OBJECT_0 == WaitForSingleObject(h_golenEvt,(iDelay-1)*1000))
		{
			if(GoldenInfo.bResponeOK == true)
			{
				return true;
			}
			else
			{
				//do nothing
			}			
		}
	}else{
		pRunInfo->DisplayRunTimeInfo("golden connect fail");
	}
	return false;
}

bool CRunInfo::InitGoldenSocket()
{
	/*
	GoldenInfo.bConnectOK = false;
	m_GoldenSocket = WSASocket(AF_INET,SOCK_DGRAM, IPPROTO_UDP, NULL,0, 0);
	if(INVALID_SOCKET==m_GoldenSocket)
	{
		DisplayRunTimeInfo("Create golden socket fail");
		return false;
	}	
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_GOLDENIP2))->GetAddress(dwIP);
	SOCKADDR_IN addrSock;
	addrSock.sin_family=AF_INET;
	addrSock.sin_port=htons(2000);
	addrSock.sin_addr.S_un.S_addr=htonl(dwIP);

	int revt = connect(m_GoldenSocket,(sockaddr*)&addrSock,sizeof(sockaddr));
	
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("Bind sock fail\n");
		return FALSE;
	}

	GoldenInfo.bConnectOK = true;
	return TRUE;
	*/
	//add by ygc for Mice RF test
	m_GoldenSocket=socket(AF_INET,SOCK_DGRAM,0);
	if(INVALID_SOCKET==m_GoldenSocket)
	{
		DisplayRunTimeInfo("Create golden socket fail");
		return false;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_family=AF_INET;
	addrSock.sin_port=htons(ADDR_ANY);
	addrSock.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

	int retval;
	retval=bind(m_GoldenSocket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	int a =  WSAGetLastError();

	if(SOCKET_ERROR==retval)
	{
		closesocket(m_GoldenSocket);
		DisplayRunTimeInfo("Bind golden socket fail");
		return false;
	}
	return true;

}

bool CRunInfo::InitSocket(char* DefaultIP)
{
	sockaddr_in  SocketAddr;
	m_Socket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0);
	if(INVALID_SOCKET ==m_Socket)
	{
		DisplayRunTimeInfo("Create socket fail\n");
		return FALSE;
	}
	
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_addr.S_un.S_addr = inet_addr(DefaultIP);
	SocketAddr.sin_port = htons(23);
	
	int revt = connect(m_Socket,(sockaddr*)&SocketAddr,sizeof(sockaddr));
	
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("Bind sock fail\n");
		return FALSE;
	}
	
	if(SOCKET_ERROR ==WSAAsyncSelect(m_Socket,m_hWnd,UM_SOCK,FD_READ))
	{
		DisplayRunTimeInfo("Create network event fail\n ");
		return FALSE;
	}
	return TRUE;
}

bool CRunInfo::Init5GSocket(char* DefaultIP)
{
	sockaddr_in  SocketAddr;
	m5G_Socket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0);
	if(INVALID_SOCKET ==m5G_Socket)
	{
		DisplayRunTimeInfo("Create socket fail\n");
		return FALSE;
	}
	
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_addr.S_un.S_addr = inet_addr(DefaultIP);
	SocketAddr.sin_port = htons(23);
	
	int revt = connect(m_Socket,(sockaddr*)&SocketAddr,sizeof(sockaddr));
	
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("Bind sock fail\n");
		return FALSE;
	}
	
	if(SOCKET_ERROR ==WSAAsyncSelect(m_Socket,m_hWnd,UM_SOCK,FD_READ))
	{
		DisplayRunTimeInfo("Create network event fail\n ");
		return FALSE;
	}
	return TRUE;
}


bool CRunInfo::NOTInitSocket(char* DefaultIP)
{
	sockaddr_in  SocketAddr;
	m_Socket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0);
	if(INVALID_SOCKET ==m_Socket)
	{
		DisplayRunTimeInfo("Create socket fail\n");
		return FALSE;
	}
	
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_addr.S_un.S_addr = inet_addr(DefaultIP);
	SocketAddr.sin_port = htons(23);
	
	int revt = connect(m_Socket,(sockaddr*)&SocketAddr,sizeof(sockaddr));
	
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("should Bind sock fail after upgrade!!!\n");
		return true;
	}
	else
	{
		DisplayRunTimeInfo("should not Bind sock ok after upgrade!!!\n");
		return false;
	}
	
	/*if(SOCKET_ERROR ==WSAAsyncSelect(m_Socket,m_hWnd,UM_SOCK,FD_READ))
	{
		DisplayRunTimeInfo("Create network event fail\n ");
		return FALSE;
	}
	return TRUE;*/
}

LRESULT  CRunInfo::OnRecvComData(WPARAM wParam,LPARAM lParam)
{
	char strBuf[MINBUFSIZE] = "";
	strcpy_s(strBuf, MINBUFSIZE, (char*)lParam);
	FillMemory((char*)lParam, MINBUFSIZE, 0);
	int len = (int)strlen(strBuf);

	if(m_blIsSfis && strstr(strBuf,"M30H00"))
	{
		DisplayRunTimeInfo(NULL);
	}

	DisplayRunTimeInfo(strBuf);
	
	if(len == 25+25+4+2) //SN+ModelOrMac+PASS
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			return 0;
		}
		char szSN[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";
		char szProductName[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szProductName, MINBUFSIZE, strBuf+25, 25);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+50, 4);
			
		m_strSN = szSN;
		m_strProductName.Trim() = szProductName;
		m_strMAC = "";
		m_strPincode = "";
		m_strCN = "";
		UpdateData(false);
		static bool bReadIniOnlyOnce = true;
		UpdateData(false);
		if(bReadIniOnlyOnce)
		{
			bReadIniOnlyOnce = false;
			m_strIniProductName = m_strProductName;
			if(!ReadIniFile())
			{
				AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
				return 0;
			}

		}
		if(strncmp(szStatus, "PASS", 4))
			return 0;
	}	
	else if(len == 25+25+25+12+12+4+2) //Mice FT
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			ShowMsg(m_strTestResult);
			return 0;
		}
		char szHH[MINBUFSIZE] = "";		
		char szPCBASN[MINBUFSIZE] = "";
		char szSN[MINBUFSIZE] = "";
		char szET0MAC[MINBUFSIZE] = "";
		char szPNNAME[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";

		strncpy_s(szHH, MINBUFSIZE, strBuf, 25);
		strncpy_s(szPCBASN, MINBUFSIZE, strBuf+25, 25);
		strncpy_s(szSN, MINBUFSIZE, strBuf+50, 25);
		strncpy_s(szET0MAC, MINBUFSIZE, strBuf+75, 12);
		strncpy_s(szPNNAME, MINBUFSIZE, strBuf+87, 12);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+99, 4);

		m_strHH = szHH;
		m_strPCBASN = szPCBASN;
		m_strSN = szSN;
		m_strET0MAC = szET0MAC;
		m_strPNNAME = szPNNAME;
		m_strStatus = szStatus;

		/*
		ShowMsg(m_strHH);
		ShowMsg(m_strPCBASN);
		ShowMsg(m_strSN);
		ShowMsg(m_strET0MAC);
		ShowMsg(m_strPNNAME);
		*/

		//ShowMsg(m_strStatus);
		//ShowMsg(m_str2GMAC);
		//ShowMsg(m_str5GMAC);

		m_ET0MAC = CovertMacToNumber(m_strET0MAC.GetBuffer(m_strET0MAC.GetLength()), m_strET0MAC.GetLength());
		m_2GMAC=  m_ET0MAC+1;
		m_5GMAC=  m_ET0MAC+2;
		m_str2GMAC = CovertNumberToMac(m_2GMAC);
		m_str5GMAC = CovertNumberToMac(m_5GMAC);

		char buf[20];
		char buf1[20];
		char buf2[20];
		char buf3[20];
		memset(buf,0,20);
		memset(buf1,0,20);
		memset(buf2,0,20);
		memset(buf3,0,20);
		lstrcpy(buf,m_strET0MAC);
		memcpy_s(buf1,20,buf+8,4);
		sprintf_s(buf2,20,"Xiaomi_%s",buf1);
		sprintf_s(buf3,20,"Xiaomi_%s_5G",buf1);//modify according to customer
		//sprintf_s(buf3,20,"Xiaomi_%s_ac",buf1);
		m_str2GSSID = buf2;
		m_str5GSSID = buf3;

		//memset(buf,0,100);
		//sprintf_s(buf,100,"mac NUM is %I64d",m_5GMAC);
		//DisplayRunTimeInfo(buf);
		//ShowMsg(m_str2GMAC.GetBuffer(m_str2GMAC.GetLength()));
		//ShowMsg(m_str5GMAC.GetBuffer(m_str5GMAC.GetLength()));

		ShowMsg(m_strHH);
		ShowMsg(m_strPCBASN);
		ShowMsg(m_strSN);
		ShowMsg(m_strET0MAC);
		ShowMsg(m_str2GMAC);
		ShowMsg(m_str5GMAC);
		ShowMsg(m_str2GSSID);
		ShowMsg(m_str5GSSID);
		ShowMsg(m_strPNNAME);
		ShowMsg(m_strStatus);



		//MessageBox(m_str2GMAC,"2G",0);
		//MessageBox(m_str5GMAC,"5G",0);

		/*char m_con2g[MINBUFSIZE];
		char m_con5g[MINBUFSIZE];
		memset(m_con2g,0,sizeof(m_con2g));
		memset(m_con2g,0,sizeof(m_con2g));

		ShowMsg("After MAC convert:\r\n");
		_itoa(m_2GMAC,m_con2g,16);
		for(int i=0;i<strlen(m_con2g);i++)
		{
			if(m_con2g[i]>='a' && m_con2g[i]<='z')
				m_con2g[i]=m_con2g[i]-32;
		}
		m_str2GMAC = m_con2g;
		ShowMsg(m_con2g);

		_itoa(m_5GMAC,m_con5g,16);
		for(int i=0;i<strlen(m_con5g);i++)
		{
			if(m_con5g[i]>='a' && m_con5g[i]<='z')
				m_con5g[i]=m_con5g[i]-32;
		}
		m_str5GMAC = m_con5g;
		ShowMsg(m_con2g);*/

		UpdateData(false);

		static bool bReadIniOnlyOnce = true;
		if(bReadIniOnlyOnce)
		{
			bReadIniOnlyOnce = false;
			m_strIniProductName = m_strProductName;
			if(!ReadIniFile())
			{
				AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
				return 0;
			}

		}
		if(strncmp(szStatus, "PASS", 4))
			return 0;
		m_strTestResult = "RUN";
		UpdateData(false);
		OnBnClickedBtnStart();
	}

	else if(len == 25+25+25+12+12+12+12+4+2) 
	{
		if(strstr(m_IniFileInfo.szTestStatuse , "FT"))//Mice FT Pass
		{
			ShowMsg("FT Test Finish!");
			if(strstr(strBuf, "PASS"))
			{
				TestPass();
			}
			if(strstr(strBuf, "ERRO"))
			{
				MessageBox("SFIS Data format error!");
				TestFail(SD00);
			}
		}
		else if(strstr(m_IniFileInfo.szTestStatuse , "RC") || strstr(m_IniFileInfo.szTestStatuse , "NFC") || strstr(m_IniFileInfo.szTestStatuse , "DOWNLOAD"))//Mice RC | NFC start test
		{
			
			if(strstr(strBuf,"M30H00"))
			{
				//ShowMsg("Start test RC!");
				if(strcmp(m_strTestResult , "RUN") == 0)
				{
					ShowMsg(m_strTestResult);
					return 0;
				}
				char szHH[MINBUFSIZE] = "";		
				char szPCBASN[MINBUFSIZE] = "";
				char szSN[MINBUFSIZE] = "";
				char szET0MAC[MINBUFSIZE] = "";
				
				char sz2GMAC[MINBUFSIZE] = "";
				char sz5GMAC[MINBUFSIZE] = "";
				char sz2GSSID[MINBUFSIZE] = "";
				char sz5GSSID[MINBUFSIZE] = "";
				

				char szPNNAME[MINBUFSIZE] = "";
				char szStatus[MINBUFSIZE] = "";

				strncpy_s(szHH, MINBUFSIZE, strBuf, 25);
				strncpy_s(szPCBASN, MINBUFSIZE, strBuf+25, 25);
				strncpy_s(szSN, MINBUFSIZE, strBuf+50, 25);
				strncpy_s(szET0MAC, MINBUFSIZE, strBuf+75, 12);
				strncpy_s(sz2GMAC, MINBUFSIZE, strBuf+87, 12);
				strncpy_s(sz5GMAC, MINBUFSIZE, strBuf+99, 12);
				strncpy_s(szPNNAME, MINBUFSIZE, strBuf+111, 12);
				strncpy_s(szStatus, MINBUFSIZE, strBuf+123, 4);

				m_strHH = szHH;
				m_strPCBASN = szPCBASN;
				m_strSN = szSN;
				m_strET0MAC = szET0MAC;
				m_str2GMAC = sz2GMAC;
				m_str5GMAC = sz5GMAC;
				m_strPNNAME = szPNNAME;
				m_strStatus = szStatus;

				/*
				ShowMsg(m_strHH);
				ShowMsg(m_strPCBASN);
				ShowMsg(m_strSN);
				ShowMsg(m_strET0MAC);
				ShowMsg(m_strPNNAME);
				*/

				//ShowMsg(m_strStatus);
				//ShowMsg(m_str2GMAC);
				//ShowMsg(m_str5GMAC);

				//m_ET0MAC = CovertMacToNumber(m_strET0MAC.GetBuffer(m_strET0MAC.GetLength()), m_strET0MAC.GetLength());
				//m_2GMAC=  m_ET0MAC+1;
				//m_5GMAC=  m_ET0MAC+2;
				//m_str2GMAC = CovertNumberToMac(m_2GMAC);
				//m_str5GMAC = CovertNumberToMac(m_5GMAC);

				char buf[20];
				char buf1[20];
				char buf2[20];
				char buf3[20];
				memset(buf,0,20);
				memset(buf1,0,20);
				memset(buf2,0,20);
				memset(buf3,0,20);
				lstrcpy(buf,m_strET0MAC);
				memcpy_s(buf1,20,buf+8,4);
				sprintf_s(buf2,20,"Xiaomi_%s",buf1);
				sprintf_s(buf3,20,"Xiaomi_%s_5G",buf1);//modify according to customer
				//sprintf_s(buf3,20,"Xiaomi_%s_ac",buf1);
				m_str2GSSID = buf2;
				m_str5GSSID = buf3;

				ShowMsg(m_strHH);
				ShowMsg(m_strPCBASN);
				ShowMsg(m_strSN);
				ShowMsg(m_strET0MAC);
				ShowMsg(m_str2GMAC);
				ShowMsg(m_str5GMAC);
				ShowMsg(m_str2GSSID);
				ShowMsg(m_str5GSSID);
				ShowMsg(m_strPNNAME);
				ShowMsg(m_strStatus);



				//MessageBox(m_str2GMAC,"2G",0);
				//MessageBox(m_str5GMAC,"5G",0);

				/*char m_con2g[MINBUFSIZE];
				char m_con5g[MINBUFSIZE];
				memset(m_con2g,0,sizeof(m_con2g));
				memset(m_con2g,0,sizeof(m_con2g));

				ShowMsg("After MAC convert:\r\n");
				_itoa(m_2GMAC,m_con2g,16);
				for(int i=0;i<strlen(m_con2g);i++)
				{
				if(m_con2g[i]>='a' && m_con2g[i]<='z')
				m_con2g[i]=m_con2g[i]-32;
				}
				m_str2GMAC = m_con2g;
				ShowMsg(m_con2g);

				_itoa(m_5GMAC,m_con5g,16);
				for(int i=0;i<strlen(m_con5g);i++)
				{
				if(m_con5g[i]>='a' && m_con5g[i]<='z')
				m_con5g[i]=m_con5g[i]-32;
				}
				m_str5GMAC = m_con5g;
				ShowMsg(m_con2g);*/

				UpdateData(false);

				static bool bReadIniOnlyOnce = true;
				if(bReadIniOnlyOnce)
				{
					bReadIniOnlyOnce = false;
					m_strIniProductName = m_strProductName;
					if(!ReadIniFile())
					{
						AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
						::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
						return 0;
					}

				}
				if(strncmp(szStatus, "PASS", 4))
					return 0;
				m_strTestResult = "RUN";
				UpdateData(false);
				OnBnClickedBtnStart();
			}
			else
			{
				//ShowMsg("RC Test Finish!");
				if(strstr(strBuf, "PASS"))
				{
					TestPass();
				}
				if(strstr(strBuf, "ERRO"))
				{
					MessageBox("SFIS Data format error!");
					TestFail(SD00);
				}
			}
		}
		else
		{
			//do nothing
		}
	}

	else if(len == 25+25+25+12+12+12+12+4+4+2) //Mice FT | RC | NFC | DOWNLOAD Fail
	{
		char szBuf[10]="";
		memcpy_s(szBuf,sizeof(szBuf),strBuf+75+48,4);
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(szBuf);
		}
	}

//for NFC station(do not use)
	/*else if(len == 25+12+4+2)
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			return 0;
		}
		char szSN[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";
		char szProductName[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szProductName, MINBUFSIZE, strBuf+25, 25);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+50, 4);
			
		m_strSN = szSN;
		m_strProductName.Trim() = szProductName;
		m_strMAC = "";
		m_strPincode = "";
		m_strCN = "";
		UpdateData(false);
		static bool bReadIniOnlyOnce = true;
		UpdateData(false);
		if(bReadIniOnlyOnce)
		{
			bReadIniOnlyOnce = false;
			m_strIniProductName = m_strProductName;
			if(!ReadIniFile())
			{
				AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
				return 0;
			}

		}
		if(strncmp(szStatus, "PASS", 4))
			return 0;
	}

	else if(len == 37+12+4+2)
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			return 0;
		}
		char szSN[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";
		char szProductName[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szProductName, MINBUFSIZE, strBuf+25, 25);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+50, 4);
			
		m_strSN = szSN;
		m_strProductName.Trim() = szProductName;
		m_strMAC = "";
		m_strPincode = "";
		m_strCN = "";
		UpdateData(false);
		static bool bReadIniOnlyOnce = true;
		UpdateData(false);
		if(bReadIniOnlyOnce)
		{
			bReadIniOnlyOnce = false;
			m_strIniProductName = m_strProductName;
			if(!ReadIniFile())
			{
				AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
				return 0;
			}

		}
		if(strncmp(szStatus, "PASS", 4))
			return 0;
	}

	else if(len == 37+12+4+4+2)
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			return 0;
		}
		char szSN[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";
		char szProductName[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szProductName, MINBUFSIZE, strBuf+25, 25);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+50, 4);
			
		m_strSN = szSN;
		m_strProductName.Trim() = szProductName;
		m_strMAC = "";
		m_strPincode = "";
		m_strCN = "";
		UpdateData(false);
		static bool bReadIniOnlyOnce = true;
		UpdateData(false);
		if(bReadIniOnlyOnce)
		{
			bReadIniOnlyOnce = false;
			m_strIniProductName = m_strProductName;
			if(!ReadIniFile())
			{
				AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
				return 0;
			}

		}
		if(strncmp(szStatus, "PASS", 4))
			return 0;
	}*/
//for NFC station end

	//comment by ygc for not use
	/*
	else if(len == 25+12+12+15+4+2)
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			ShowMsg(m_strTestResult);
			return 0;
		}
		char szSN[MINBUFSIZE] = "";		
		char szMac[MINBUFSIZE] = "";
		char szPinCode[MINBUFSIZE] = "";
		char szCN[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szMac, MINBUFSIZE, strBuf+25, 12);
		strncpy_s(szPinCode, MINBUFSIZE, strBuf+37, 12);
		strncpy_s(szCN, MINBUFSIZE, strBuf+49, 15);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+64, 4);
		m_strCN = szCN;
		m_strPincode = szPinCode;
		m_strMAC = szMac;
		m_strSN = szSN;
		ShowMsg(m_strSN);
		ShowMsg(m_strMAC);
		ShowMsg(m_strPincode);
		ShowMsg(m_strCN);
		UpdateData(false);
		if(strncmp(szStatus, "PASS", 4))
			return 0;
		m_strTestResult = "RUN";
		UpdateData(false);
		OnBnClickedBtnStart();
	}
	else if(len == 25+12+12+15+12+4+2) 
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
	}

	else if(len == 25+12+12+15+30+12+4+2)
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}	
	//------------------------------------add for BF & OpenVPN---------------------------------------

	else if(len == 25+12+12+15+32+32+32+32+30+60+4+2)
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			ShowMsg(m_strTestResult);
			return 0;
		}
		char szSN[MINBUFSIZE] = "";		
		char szMac[MINBUFSIZE] = "";
		char szPinCode[MINBUFSIZE] = "";
		char szCN[MINBUFSIZE] = "";
		
		char szSSID_2G[MINBUFSIZE] = "";		
		char szPASS_2G[MINBUFSIZE] = "";
		char szSSID_5G[MINBUFSIZE] = "";
		char szPASS_5G[MINBUFSIZE] = "";

		char szBeamForm[MINBUFSIZE] = "";
		char szOpenSSL[MINBUFSIZE] = "";

		char szStatus[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szMac, MINBUFSIZE, strBuf+25, 12);
		strncpy_s(szPinCode, MINBUFSIZE, strBuf+37, 12);
		strncpy_s(szCN, MINBUFSIZE, strBuf+49, 15);

		strncpy_s(szSSID_2G, MINBUFSIZE, strBuf+64, 32);
		strncpy_s(szPASS_2G, MINBUFSIZE, strBuf+96, 32);
		strncpy_s(szSSID_5G, MINBUFSIZE, strBuf+128, 32);
		strncpy_s(szPASS_5G, MINBUFSIZE, strBuf+160, 32);
	
		strncpy_s(szBeamForm, MINBUFSIZE, strBuf+192, 30);
		strncpy_s(szOpenSSL, MINBUFSIZE, strBuf+222, 60);

		strncpy_s(szStatus, MINBUFSIZE, strBuf+282, 4);

		m_strCN = szCN;
		m_strPincode = szPinCode;
		m_strMAC = szMac;
		m_strSN = szSN;
		m_strSSID_2G = szSSID_2G;
		m_strPASS_2G = szPASS_2G;
		m_strSSID_5G = szSSID_5G;
		m_strPASS_5G = szPASS_5G ;
		strBF = szBeamForm;
		szOpenVPN = szOpenSSL;

		ShowMsg(m_strSN);
		ShowMsg(m_strMAC);
		ShowMsg(m_strPincode);
		ShowMsg(m_strCN);

		ShowMsg(m_strSSID_2G);
		ShowMsg(m_strPASS_2G);
		ShowMsg(m_strSSID_5G);
		ShowMsg(m_strPASS_5G);

		ShowMsg(strBF);
		ShowMsg(szOpenVPN);

		UpdateData(false);
		if(strncmp(szStatus, "PASS", 4))
			return 0;
		m_strTestResult = "RUN";
		UpdateData(false);
		OnBnClickedBtnStart();
	}
	else if(len == 25+12+12+15+32+32+32+32+30+60+12+4+2) 
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}
	else if(len == 25+12+12+15+32+32+32+32+30+30+60+12+4+2)
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}

	//---------------------------------------------------------------------------------------------------------

	else if(len == 25+12+12+15+32+32+30+60+4+2)
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			ShowMsg(m_strTestResult);
			return 0;
		}
		char szSN[MINBUFSIZE] = "";		
		char szMac[MINBUFSIZE] = "";
		char szPinCode[MINBUFSIZE] = "";
		char szCN[MINBUFSIZE] = "";
		
		char szSSID_2G[MINBUFSIZE] = "";		
		char szPASS_2G[MINBUFSIZE] = "";
		char szSSID_5G[MINBUFSIZE] = "";
		char szPASS_5G[MINBUFSIZE] = "";

		char szBeamForm[MINBUFSIZE] = "";
		char szOpenSSL[MINBUFSIZE] = "";

		char szStatus[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szMac, MINBUFSIZE, strBuf+25, 12);
		strncpy_s(szPinCode, MINBUFSIZE, strBuf+37, 12);
		strncpy_s(szCN, MINBUFSIZE, strBuf+49, 15);

		strncpy_s(szSSID_2G, MINBUFSIZE, strBuf+64, 32);
		strncpy_s(szPASS_2G, MINBUFSIZE, strBuf+96, 32);
	
		strncpy_s(szBeamForm, MINBUFSIZE, strBuf+128, 30);
		strncpy_s(szOpenSSL, MINBUFSIZE, strBuf+158, 60);

		strncpy_s(szStatus, MINBUFSIZE, strBuf+218, 4);

		sprintf_s(szSSID_5G,sizeof(szSSID_5G),"%s",szSSID_2G);
		sprintf_s(szPASS_5G,sizeof(szPASS_5G),"%s",szPASS_2G);

		m_strCN = szCN;
		m_strPincode = szPinCode;
		m_strMAC = szMac;
		m_strSN = szSN;
		m_strSSID_2G = szSSID_2G;
		m_strPASS_2G = szPASS_2G;
		m_strSSID_5G = szSSID_5G;
		m_strPASS_5G = szPASS_5G ;
		strBF = szBeamForm;
		szOpenVPN = szOpenSSL;

		ShowMsg(m_strSN);
		ShowMsg(m_strMAC);
		ShowMsg(m_strPincode);
		ShowMsg(m_strCN);

		ShowMsg(m_strSSID_2G);
		ShowMsg(m_strPASS_2G);
		ShowMsg(m_strSSID_5G);
		ShowMsg(m_strPASS_5G);

		ShowMsg(strBF);
		ShowMsg(szOpenVPN);

		UpdateData(false);
		if(strncmp(szStatus, "PASS", 4))
			return 0;
		m_strTestResult = "RUN";
		UpdateData(false);
		OnBnClickedBtnStart();
	}
	else if(len == 25+12+12+15+32+32+32+32+30+60+12+4+2) //SN+MAC+PinCode+SSN+SSID+PASSWORD+SSID+PASSWORD+BF+OPENVPN+PASS (RC)
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}
	else if(len == 25+12+12+15+32+32+32+32+30+30+60+12+4+2) //SN+MAC+PinCode+SSN+SSID+PASSWORD+SSID+PASSWORD+FW+BF+OPENVPN+PASS (FT)
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}

	//---------------------------------------------------------------------------------------------------------

	else if(len == 25+12+12+15+32+32+32+32+4+2) //SN+MAC+PinCode+SSN+SSID+PASSWORD+SSID+PASSWORD+PASS
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
		{
			ShowMsg(m_strTestResult);
			return 0;
		}
		char szSN[MINBUFSIZE] = "";		
		char szMac[MINBUFSIZE] = "";
		char szPinCode[MINBUFSIZE] = "";
		char szCN[MINBUFSIZE] = "";
		
		char szSSID_2G[MINBUFSIZE] = "";		
		char szPASS_2G[MINBUFSIZE] = "";
		char szSSID_5G[MINBUFSIZE] = "";
		char szPASS_5G[MINBUFSIZE] = "";

		char szStatus[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szMac, MINBUFSIZE, strBuf+25, 12);
		strncpy_s(szPinCode, MINBUFSIZE, strBuf+37, 12);
		strncpy_s(szCN, MINBUFSIZE, strBuf+49, 15);

		strncpy_s(szSSID_2G, MINBUFSIZE, strBuf+64, 32);
		strncpy_s(szPASS_2G, MINBUFSIZE, strBuf+96, 32);
		strncpy_s(szSSID_5G, MINBUFSIZE, strBuf+128, 32);
		strncpy_s(szPASS_5G, MINBUFSIZE, strBuf+160, 32);


		strncpy_s(szStatus, MINBUFSIZE, strBuf+192, 4);

		m_strCN = szCN;
		m_strPincode = szPinCode;
		m_strMAC = szMac;
		m_strSN = szSN;
		m_strSSID_2G = szSSID_2G;
		m_strPASS_2G = szPASS_2G;
		m_strSSID_5G = szSSID_5G;
		m_strPASS_5G = szPASS_5G ;

		ShowMsg(m_strSN);
		ShowMsg(m_strMAC);
		ShowMsg(m_strPincode);
		ShowMsg(m_strCN);

		ShowMsg(m_strSSID_2G);
		ShowMsg(m_strPASS_2G);
		ShowMsg(m_strSSID_5G);
		ShowMsg(m_strPASS_5G);

		UpdateData(false);
		if(strncmp(szStatus, "PASS", 4))
			return 0;
		m_strTestResult = "RUN";
		UpdateData(false);
		OnBnClickedBtnStart();
	}
	else if(len == 25+12+12+15+32+32+32+32+12+4+2) //SN+MAC+PinCode+SSN+SSID+PASSWORD+SSID+PASSWORD+PC_name+PASS
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}

	else if(len == 25+12+12+15+32+32+32+32+30+12+4+2) //SN+MAC+PinCode+SSN+SSID+PASSWORD+SSID+PASSWORD+FW+ATE_NO+PASS
	{
		if(strstr(strBuf, "PASS") == NULL)			
		{
			TestFail(SR00);
		}
		else
		{
			TestPass();
		}
	}

	else if(len == 25+12+4+2)//SN+productname+PASS
	{
		if(strcmp(m_strTestResult , "RUN") == 0)
			return 0;
		char szSN[MINBUFSIZE] = "";
		char szStatus[MINBUFSIZE] = "";
		char szProductName[MINBUFSIZE] = "";
		strncpy_s(szSN, MINBUFSIZE, strBuf, 25);
		strncpy_s(szProductName, MINBUFSIZE, strBuf+25, 12);
		strncpy_s(szStatus, MINBUFSIZE, strBuf+37, 4);
		m_strSN = szSN;
		UpdateData(false);
		m_strProductName = szProductName;	
		UpdateData(false);

		static bool bReadIniOnlyOnce = true;
		UpdateData(false);
		if(bReadIniOnlyOnce)
		{
			bReadIniOnlyOnce = false;
			m_strIniProductName = m_strProductName;
			if(!ReadIniFile())
			{
				AfxMessageBox("請重新配置INI檔,確保有匹配的機種名稱,程式自動關閉!\n");
				::PostMessage(GetParent()->GetParent()->GetSafeHwnd(),WM_CLOSE,0,0);
				return 0;
			}

		}

		if(strncmp(szStatus, "PASS", 4))
			return 0;
		OnBnClickedBtnStart();
	}

	else if(len == 25+12+12+4+2)//SN+MAC+PC+PASS
	{
		if(strstr(strBuf, "PASS") == NULL)
		{
			TestFail(SR00);
			return 0;
		}
		else
		{
			TestPass();
		}
	}
	*/

	else
	{
		//do nothing
	}
	return 1;
}
void CRunInfo::ShowMsg(CString strMsg)
{
	DisplayRunTimeInfo("");
	DisplayRunTimeInfo(strMsg.GetBuffer(strMsg.GetLength()));
	DisplayRunTimeInfo("");
}

UINT __cdecl OnRecvGoldenData(LPVOID pParam)
{
	ofstream os;
	os.open("1.txt",ios_base::app|ios_base::out);
	_GoldenInfo* pbtn = (_GoldenInfo*) pParam ;
	int iSubDelay = pbtn->nDelay*10;
	while(iSubDelay--)
	{
		/*recv*/		
		pbtn->szGoldenData.Empty();
		WSABUF  ReceiveBuff;
		ReceiveBuff.buf = new char[10240];
		ReceiveBuff.len = 10240;
		memset(ReceiveBuff.buf,0,10240);
		DWORD NumberOfBytesRecvd = 0;
		DWORD dwflag = 0;

		try
		{
			os<<"rece start\n";os.flush();
			WSARecv(pbtn->GoldenSocket,&ReceiveBuff,1,&NumberOfBytesRecvd,&dwflag,NULL,NULL);	
			os<<"rece ok\n";os.flush();
		}
		catch(...)
		{
			pRunInfo->DisplayRunTimeInfo("WSARecv error!");
		}
		
		char szbuf[10240];
		sprintf_s(szbuf,sizeof(szbuf),"%s",ReceiveBuff.buf);
		os<<"ReceiveBuff="<<szbuf<<endl;os.flush();
		os<<""<<endl;os.flush();

		pbtn->szGoldenData = ReceiveBuff.buf;			
		os<<"szGoldenData="<<pbtn->szGoldenData<<endl;os.flush();
		os<<""<<endl;os.flush();
		os.close();
		delete []ReceiveBuff.buf;
	
		if(strstr(GoldenInfo.szGoldenData,GoldenInfo.szReturnInfo) != NULL)
		{
			GoldenInfo.bResponeOK = true;
			SetEvent(h_golenEvt);
			break;
		}
		else
		{
			Sleep(100);
		}	
	}	
	if(iSubDelay = 1)
	{
		return 0;
	}
	return 1;

} 

LRESULT  CRunInfo::OnRecvEthernetData(WPARAM wParam,LPARAM lParam)
{
	switch(LOWORD(lParam)) 
	{
	case FD_READ:
		m_strStoreEthernetData.Empty();
		WSABUF  ReceiveBuff;
		ReceiveBuff.buf = new char[1024];
		ReceiveBuff.len = 1024;
		memset(ReceiveBuff.buf,0,1024);
		DWORD NumberOfBytesRecvd = 0;
		DWORD dwflag = 0;
		m_strStoreEthernetData.Empty();
		Sleep(500);
		WSARecv(m_Socket,&ReceiveBuff,1,&NumberOfBytesRecvd,&dwflag,NULL,NULL);
		DisplayRunTimeInfo(ReceiveBuff.buf);
		m_strStoreEthernetData = ReceiveBuff.buf;
		g_data += ReceiveBuff.buf;
		delete []ReceiveBuff.buf;
		SetEvent(m_hEvt);
		break;
	}
	
	return 0;
}

bool  CRunInfo::CheckMac(CString strMac)
{
	if (strMac.GetLength() == 12)
    {
        for (int iCount = 1; iCount <= 12; iCount++)
        {
            if ( ((strMac[iCount] >= '0') && (strMac[iCount] <= '9')) ||
                 ((strMac[iCount] >= 'A') && (strMac[iCount] <= 'F')) )
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void CRunInfo::GetThroughputValue(IN char* SourceValue,OUT double& TXValue,double& RXValue)
{
	char* pFindTX = strstr(SourceValue, "/sec");
	
	if(pFindTX == NULL)
	{
		TXValue = 0;
		return;
	}
	char szTX[MINBUFSIZE] = "";
	
	strncpy_s(szTX, MINBUFSIZE, pFindTX-10, 10);
	
	if(strstr(szTX, "Gbits")!=NULL)
	{
		strncpy_s(szTX, MINBUFSIZE, szTX,4);
		TXValue = _tstof(szTX)*1024;
	}
	else if(strstr(szTX, "Mbits")!=NULL)
	{
		strncpy_s(szTX, MINBUFSIZE, szTX, 4);
		TXValue = _tstof(szTX);
	}
	else if(strstr(szTX, "Kbits")!=NULL)
	{
		strncpy_s(szTX, MINBUFSIZE, szTX, 4);
		TXValue = _tstof(szTX)/1024;
	}
	else
		TXValue = 0;

	char* pFindRX = strstr(pFindTX+4, "/sec");
	if(pFindRX==NULL)
	{
		RXValue = 0;
		return;
	}
	char szRX[MINBUFSIZE] = "";
	strncpy_s(szRX, MINBUFSIZE, pFindRX-10, 10);
	if(strstr(szRX, "Gbits")!=NULL)
	{
		strncpy_s(szRX, MINBUFSIZE, szRX, 4);
		RXValue = _tstof(szRX)*1024;
	}
	else if(_tcsstr(szRX, "Mbits")!=NULL)
	{
		strncpy_s(szRX, MINBUFSIZE, szRX, 4);
		RXValue = _tstof(szRX);
	}
	else if(strstr(szRX, "Kbits")!=NULL)
	{
		strncpy_s(szRX ,MINBUFSIZE, szRX, 4);
		RXValue = _tstof(szRX)/1024;
	}
	else
		RXValue = 0;

}

void CRunInfo::SendEthernetCmd(char* SendData)
{
	CStringA strSendData;
	DWORD    dwNumber;	
	WSABUF   sendbuf;
	sendbuf.buf=new char[300];
	sendbuf.len=300;
	memset(sendbuf.buf,0,300);
	
	strSendData  =SendData;
	strSendData +="\n";
	
	strcpy_s(sendbuf.buf,300,strSendData.GetBuffer(strSendData.GetLength()+1));
	
	WSASend(m_Socket,&sendbuf,1,&dwNumber,0,NULL,NULL);
	
	delete []sendbuf.buf;
}

UINT __cdecl  CRunInfo::ReadComForSfis(LPVOID param)
{
	CCom* SfisCom = (CCom*)param;
	DWORD   dwEvt;
	DWORD   dwErrorFlags;
	COMSTAT ComStat;
	char   szReadBuf[MINBUFSIZE] = "";
	
	SetCommMask(SfisCom->m_hCom, EV_RXCHAR);

	while(1)
	{
			
		WaitCommEvent(SfisCom->m_hCom, &dwEvt, NULL);
		if((dwEvt & EV_RXCHAR) == EV_RXCHAR)
		{
			ClearCommError(SfisCom->m_hCom ,&dwErrorFlags,&ComStat);
			if(ComStat.cbInQue>0)
			{
				SfisCom->ReadData(szReadBuf, MINBUFSIZE,1200);
				::PostMessage(SfisCom->m_hParentWnd, WM_COMET, 0, (LPARAM)szReadBuf);
			}
		}
	}
	return 1;
}

__int64  CRunInfo::CovertMacToNumber(char* Mac,int lenth)
{
	__int64 dbResult = 0;
	double z       = 16.0;

	//DisplayRunTimeInfo(Mac);
	char* revMac = _strrev(_strdup( Mac ));

	//DisplayRunTimeInfo("1111111111");
	//DisplayRunTimeInfo(revMac);
	_tcsupr_s(revMac,lenth+1);
	//DisplayRunTimeInfo(revMac);

	int  IntMac[12] = {0};
	for(int i = 0; i<lenth;++i)
	{
		if('A' == revMac[i])
			IntMac[i] = 10;
		else if('B' == revMac[i])
			IntMac[i] = 11;
		else if('C' == revMac[i])
			IntMac[i] = 12;
		else if('D' == revMac[i])
			IntMac[i] = 13;	
		else if('E' == revMac[i])
			IntMac[i] = 14;
		else if('F' == revMac[i])
			IntMac[i] = 15;
		else
			IntMac[i] = revMac[i] - 48;
	}
	for(int j =0;j<lenth;++j)
	{
		dbResult+=(__int64)(IntMac[j]*pow(z , j));
	}
	Mac = _strrev(revMac);
	//DisplayRunTimeInfo(Mac);

	//char buf[100];
	//memset(buf,0,100);
	//sprintf_s(buf,100,"mac NUM is %I64d",dbResult);
	//DisplayRunTimeInfo(buf);
	return dbResult;
}

CString  CRunInfo::CovertNumberToMac(__int64 macNum)
{
	CString dbResult;
	double z = 16.0;

	char macBuf[20];
	memset(macBuf,0,sizeof(macBuf));

	__int64 convert=0;
	__int64 convert2=macNum;
 
	int lenth=12;
	for(int i = lenth-1; i>=0; i--)
	{
		convert=convert2/pow(z,(double)i);

		if( 0 == ((int)(convert)))
			macBuf[11-i] = '0';
		else if( 1 == ((int)(convert)))
			macBuf[11-i] = '1';
		else if( 2 == ((int)(convert)))
			macBuf[11-i] = '2';
		else if( 3 == ((int)(convert)))
			macBuf[11-i] = '3';
		else if( 4 == ((int)(convert)))
			macBuf[11-i] = '4';
		else if( 5 == ((int)(convert)))
			macBuf[11-i] = '5';
		else if( 6 == ((int)(convert)))
			macBuf[11-i] = '6';
		else if( 7 == ((int)(convert)))
			macBuf[11-i] = '7';
		else if( 8 == ((int)(convert)))
			macBuf[11-i] = '8';
		else if( 9 == ((int)(convert)))
			macBuf[11-i] = '9';
		else if( 10 == ((int)(convert)))
			macBuf[11-i] = 'A';
		else if( 11 == ((int)(convert)))
			macBuf[11-i] = 'B';
		else if( 12 == ((int)(convert)))
			macBuf[11-i] = 'C';
		else if( 13 == ((int)(convert)))
			macBuf[11-i] = 'D';
		else if( 14 == ((int)(convert)))
			macBuf[11-i] = 'E';
		else if( 15 == ((int)(convert)))
			macBuf[11-i] = 'F';
		else
		{
			//nothing
		}
		convert2=convert2-((int)(convert))*pow(z,(double)i);
		convert=0;
	}

	//DisplayRunTimeInfo(macBuf);
	//MessageBox(macBuf,"BUF",0);
	
	//for(int j =0;j<lenth;++j)
	//{
	//	dbResult+=(ULONG)(IntMac[j]*pow(z , j));
	//}
	//Mac = _strrev(revMac);
	dbResult = macBuf;

	return dbResult;
}

bool CRunInfo::GetIsSfis()
{
	return m_blIsSfis;
}

void CRunInfo::OnBnClickedBtnStart()
{
	/*szBF = "0x00000x00000x00000x00000x0000";
	if(DutGetBFPar())
	{
		AfxMessageBox(szBF);
	}*/
	
	IsDisplayErrorCode(false);
	//DisplayRunTimeInfo(NULL);

	//static bool bOnce = true;

	
	if(m_blIsOpenSfis&&m_blIsSfis)
	{
		/*szFWforSfis default value,if no this step program will error when DUT fail before telnet*/
		//szFWforSfis = "00000000000000000000000000000";
		//m_strIniProductName.Trim();
		//m_strProductName.Trim();
		if(!strstr(m_strPNNAME,m_IniFileInfo.szPNNAME))
		{
			SendSfisResultAndShowFail(SY15);
			return ;	
		}

		if(m_strSN.GetLength() != 25)
		{
			AfxMessageBox("SN lable length error,please scan again!SN條碼長度錯誤，請重新掃描！");
			return ;
		}

		if(!strstr(m_IniFileInfo.szTestStatuse , "PT2"))
		{


			if(m_strET0MAC.GetLength() != 12)
			{
				AfxMessageBox("MAC lable length error,please scan again!MAC條碼長度錯誤，請重新掃描！");
				return ;
			}
			if(CheckMac(m_strET0MAC))
			{
				AfxMessageBox("MAC lable rule error,please scan again!MAC條碼規則錯誤，請重新掃描！");
				return ;
			}
			
			/*
			if(m_strPincode.GetLength() != 12)
			{	
				AfxMessageBox("PinCode lable length error,please scan again!PinCode條碼長度錯誤，請重新掃描！");
				return ;
			}
			else
			{
				for(int i=0;i<8;i++)
				{
					if(atoi(m_strPincode.Mid(i,1)) < 0 || atoi(m_strPincode.Mid(i,1)) > 9)
					{
						AfxMessageBox("PinCode lable rule error,please scan again!PinCode條碼規則錯誤，請重新掃描！");
						return ;
					}	
				}
			}

			if(m_strCN.GetLength() != 15)
			{
				AfxMessageBox("SSN lable length error,please scan again!SSN條碼長度錯誤，請重新掃描！");
				return ;
			}
			*/
		}
	}
	
	else
	{
		m_BtnStart.EnableWindow(false);
		m_BtnQuit.EnableWindow(false);
		UpdateData();
		if(m_strSN.IsEmpty())
		{
			SendSfisResultAndShowFail(BR00);
			return ;
		}
		if(m_strMAC.IsEmpty())
		{
			SendSfisResultAndShowFail(BR00);
			return ;
		}
		if(m_strPincode.IsEmpty())
		{
			SendSfisResultAndShowFail(BR00);
			return ;
		}
		
		if(m_strCN.IsEmpty())
		{
			SendSfisResultAndShowFail(BR00);
			return ;
		}

	}

	DisplayRunTimeInfo(NULL);
	DisplayRunTimeInfo("Start to run program");
	m_strStoreEthernetData.Empty();
	m_strRecordTestData.Empty();
	memset(TestDetaillog,0,sizeof(TestDetaillog));
	g_data.Empty();
	i_flag=0;

	CTime Time = CTime::GetCurrentTime();
	m_strRecordTestData  += Time.Format("%H:%M:%S");
	m_strRecordTestData  +=  "\t";
	m_TestRecordinfo.szRecordTime = Time.Format("%H:%M:%S");

	m_strRecordTestData  +=  m_strSN;
	m_strRecordTestData  +=  "\t";
	m_TestRecordinfo.szRecordSN = m_strSN;

	m_strRecordTestData  +=  m_strET0MAC;
	m_strRecordTestData  +=  "\t";
	m_TestRecordinfo.szRecordMac = m_strET0MAC;


	m_strRecordTestData  +=  m_strSSID_2G;
	m_strRecordTestData  +=  "\t";
	m_strRecordTestData  +=  m_strPASS_2G;
	m_strRecordTestData  +=  "\t";
	m_strRecordTestData  +=  m_strSSID_5G;
	m_strRecordTestData  +=  "\t";
	m_strRecordTestData  +=  m_strPASS_5G;
	m_strRecordTestData  +=  "\t";

	m_strRecordTestData  +=  m_strPcName;
	m_strRecordTestData  +=  "\t";
	m_TestRecordinfo.szRecordPcName = m_strPcName;	


	/*CString		m_strHH;
	CString		m_strPCBASN;
	CString		m_strSN;
	CString		m_strET0MAC;
	CString		m_str2GMAC;
	CString		m_str5GMAC;
	CString		m_strPNNAME;
	CString		m_strStatus;*/


	//m_strRecordTestData  +=  m_strPincode;
	//m_strRecordTestData  +=  "\t";
	//m_TestRecordinfo.szRecordPincode = m_strPincode;

	//m_strRecordTestData  +=  m_strCN;
	//m_strRecordTestData  +=  "\t";
	//m_TestRecordinfo.szRecordPincode = m_strCN;

	//m_strRecordTestData  +=  m_strProductName;
	//m_strRecordTestData  +=  "\t";
	//m_TestRecordinfo.szRecordProductName = m_strProductName;

	/*m_strRecordTestData  += szBF;
	m_strRecordTestData  +=  "\t";
	m_strRecordTestData  += szOpenVPN;
	m_strRecordTestData  +=  "\t";*/

	m_TestTimeCount = 0;
	m_strTestResult = "RUN";
	m_strTestTime = "00:00";
	 
	UpdateData(false);

	SetTimer(1,1000,NULL);

	char szPassFilePath[MINBUFSIZE] = "";
	char szFailFilePath[MINBUFSIZE] = "";
	char szStationFilePath[MINBUFSIZE] = "";

	sprintf_s(szStationFilePath, MINBUFSIZE, "%s\\%s\\", m_IniFileInfo.szStoreSrvPath ,m_Station);
	sprintf_s(szPassFilePath, MINBUFSIZE, "%s\\%s\\PASS\\", m_IniFileInfo.szStoreSrvPath ,m_Station);
	sprintf_s(szFailFilePath, MINBUFSIZE, "%s\\%s\\FAIL\\", m_IniFileInfo.szStoreSrvPath ,m_Station);

	//creat local log path
	if(CreateDirectory("D:\\PASS",NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		// do nothing
	}
	else
	{
		CString strMsg="Create local log pass path failed";
		AfxMessageBox(strMsg);
	}
	
	if(CreateDirectory("D:\\FAIL",NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		// do nothing
	}
	else
	{
		CString strMsg="Create local log fail path failed";
		AfxMessageBox(strMsg);
	}

	//F:\lsy\ID\U12H197 : "F:\lsy\ID\" have yet exist,just need creat "\U12H197 "
	//if(CreateDirectory(m_IniFileInfo.szStoreSrvPath,NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	//{
	//	// do nothing
	//}
	//else
	//{
	//	CString strMsg="Create " + CString(m_IniFileInfo.szStoreSrvPath)+" failed";
	//	AfxMessageBox(strMsg);
	//}
	////F:\lsy\ID\U12H197\FT : "F:\lsy\ID\U12H197\" have yet exist,just need creat "\FT "
	//if(CreateDirectory(szStationFilePath,NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	//{
	//	// do nothing
	//}
	//else
	//{
	//	CString strMsg="Create " + CString(szStationFilePath)+" failed";
	//	AfxMessageBox(strMsg);
	//}

	//if(CreateDirectory(szPassFilePath,NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	//{
	//	// do nothing
	//}
	//else
	//{
	//	CString strMsg="Create " + CString(szPassFilePath)+" failed";
	//	AfxMessageBox(strMsg);
	//}

	//if(CreateDirectory(szFailFilePath,NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
	//{
	//	// do nothing
	//}
	//else
	//{
	//	CString strMsg=_T("Create ") +CString(szFailFilePath)+_T(" failed");
	//	AfxMessageBox(strMsg);
	//}
	//
	//if(m_blIsSfis)
	//{		
	//	if (_access(szPassFilePath,00) == -1 || _access(szFailFilePath, 00) == -1)
	//	{
	//		ShowMsg(szFailFilePath);
	//		SendSfisResultAndShowFail(SY05);
	//		return ;
	//	}
	//}

	//m_nLanMAC = CovertMacToNumber(m_strET0MAC.GetBuffer(m_strET0MAC.GetLength()), m_strET0MAC.GetLength());
    //m_nWanMAC=  m_nLanMAC+1;

	if(
		(m_IniFileInfo.szTestStatuse == "") || (
		(!strstr(m_IniFileInfo.szTestStatuse , "DB")) && 
		(!strstr(m_IniFileInfo.szTestStatuse , "FT")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "PT2")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "RC")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "NFC")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "RUN-IN")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "FAN")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "DOWNLOAD")) &&
		(!strstr(m_IniFileInfo.szTestStatuse , "DOWN")) )
	  )
	{
		AfxMessageBox("TestInformation.ini 檔案中測試站別配置不對請確認");
		SendSfisResultAndShowFail(SY45);
		return;
	}
	
	if(strstr(m_IniFileInfo.szTestStatuse , "FT"))
	{
		AfxBeginThread(FT_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "RC1"))
	{
		AfxBeginThread(RC1_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "RC") && !strstr(m_IniFileInfo.szTestStatuse , "RC1"))
	{
		AfxBeginThread(RC_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "DB"))
	{
		AfxBeginThread(Debug_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "PT2"))
	{
		AfxBeginThread(PT2_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "NFC"))
	{
		AfxBeginThread(NFC_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "RUN-IN"))
	{
		AfxBeginThread(RUNIN_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "FAN"))
	{
		AfxBeginThread(FAN_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "DOWNLOAD"))
	{
		AfxBeginThread(DOWNLOAD_FunctionTestRun, this);
	}
	if(strstr(m_IniFileInfo.szTestStatuse , "DOWN") && !strstr(m_IniFileInfo.szTestStatuse , "DOWNLOAD"))
	{
		AfxBeginThread(DOWN_FunctionTestRun, this);
	}
}
void CRunInfo::CollectServerLogData()
{
	m_strRecordTestData.Format
	("%25s\t%12s\t%12s\t%15s\t%20s\t%8s\t%12s\t%4s\t%30s\t",  
		m_TestRecordinfo.szRecordSN,
		m_TestRecordinfo.szRecordMac,
		m_TestRecordinfo.szRecordPincode,
		m_TestRecordinfo.szRecordSSN,
		m_TestRecordinfo.szRecordTime,
		m_TestRecordinfo.szRecordProductName,
		m_TestRecordinfo.szRecordPcName,
		m_TestRecordinfo.szRecordResult,
		m_TestRecordinfo.szRecordFirmware
		);		
}

bool CRunInfo::DelArp()
{
	WinExec("arp -d 192.168.31.1",SW_HIDE);
	return 1;
}

bool CRunInfo::PingSpecifyIP_RC1(char* IP, int nSuccessCount)
{
	HANDLE hWritePipe  = NULL;
	HANDLE hReadPipe   = NULL;

	char  szPing[MINBUFSIZE] = "";
	sprintf_s(szPing, MINBUFSIZE, "ping.exe %s -n 10 -w %d", IP , m_IniFileInfo.nPingPacketCount);
	char   szReadFromPipeData[MAXBUFSIZE] = "";
	DWORD  byteRead    = 0;

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle =true;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	if(!CreatePipe(&hReadPipe,&hWritePipe,&sa,0))
	{
		DisplayRunTimeInfo("Create pipe fail");
		return 0;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO        si;
	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdError  = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;

	if(!CreateProcess(NULL,szPing,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
	{
		DisplayRunTimeInfo("Start run ping.exe fail!");
		return 0;
	}

	CloseHandle(pi.hThread);
	CloseHandle(hWritePipe);
	CloseHandle(pi.hProcess);

	DWORD  dwPingFailCount    = 10;
	DWORD  dwPingSuccessCount = nSuccessCount;
	
	while(TRUE)
	{
		memset(szReadFromPipeData,0,MAXBUFSIZE);
		Sleep(100);
		int bResult = ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
		
		if(!bResult)
		{
			DisplayRunTimeInfo("Read ping.exe fail!");
			return 0;
		}

		Sleep(200);

		char IPInfo[MINBUFSIZE] ="";
		sprintf_s(IPInfo, MINBUFSIZE, "Reply from %s", IP);

		
		if(strstr(szReadFromPipeData,IPInfo))
		{
			dwPingSuccessCount--;
		}
		else
		{
			dwPingFailCount--;
		}
		DisplayRunTimeInfo(szReadFromPipeData);
		if(!dwPingSuccessCount)
		{
			ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
			DisplayRunTimeInfo(szReadFromPipeData);
			return 1;
		}
		if(!dwPingFailCount)
			return 0;
	}
}

bool CRunInfo::PingSpecifyIP(char* IP, int nSuccessCount)
{
	HANDLE hWritePipe  = NULL;
	HANDLE hReadPipe   = NULL;

	char  szPing[MINBUFSIZE] = "";
	sprintf_s(szPing, MINBUFSIZE, "ping.exe %s -n 35 -w %d", IP , m_IniFileInfo.nPingPacketCount);
	char   szReadFromPipeData[MAXBUFSIZE] = "";
	DWORD  byteRead    = 0;

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle =true;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	if(!CreatePipe(&hReadPipe,&hWritePipe,&sa,0))
	{
		DisplayRunTimeInfo("Create pipe fail");
		return 0;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO        si;
	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdError  = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;

	if(!CreateProcess(NULL,szPing,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
	{
		DisplayRunTimeInfo("Start run ping.exe fail!");
		return 0;
	}

	CloseHandle(pi.hThread);
	CloseHandle(hWritePipe);
	CloseHandle(pi.hProcess);

	DWORD  dwPingFailCount    = 35;
	DWORD  dwPingSuccessCount = nSuccessCount;
	
	while(TRUE)
	{
		memset(szReadFromPipeData,0,MAXBUFSIZE);
		Sleep(100);
		int bResult = ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
		
		if(!bResult)
		{
			DisplayRunTimeInfo("Read ping.exe fail!");
			return 0;
		}

		Sleep(200);

		char IPInfo[MINBUFSIZE] ="";
		//sprintf_s(IPInfo, MINBUFSIZE, "Reply from %s", IP);////
		sprintf_s(IPInfo, MINBUFSIZE, "%s", "TTL");

		DisplayRunTimeInfo("Aaa");

		
		if(strstr(szReadFromPipeData,IPInfo))
		{
			dwPingSuccessCount--;
		}
		else
		{
			dwPingFailCount--;
		}
		DisplayRunTimeInfo(szReadFromPipeData);
		if(!dwPingSuccessCount)
		{
			ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
			DisplayRunTimeInfo(szReadFromPipeData);
			return 1;
		}
		if(!dwPingFailCount)
			return 0;
	}
}

bool CRunInfo::PingSpecifyIP_2(char* IP, int nSuccessCount)
{
	HANDLE hWritePipe  = NULL;
	HANDLE hReadPipe   = NULL;

	char  szPing[MINBUFSIZE] = "";
	sprintf_s(szPing, MINBUFSIZE, "ping.exe %s -n 10 -w %d", IP , m_IniFileInfo.nPingPacketCount);
	char   szReadFromPipeData[MAXBUFSIZE] = "";
	DWORD  byteRead    = 0;

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle =true;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	if(!CreatePipe(&hReadPipe,&hWritePipe,&sa,0))
	{
		DisplayRunTimeInfo("Create pipe fail");
		return 0;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO        si;
	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdError  = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;

	if(!CreateProcess(NULL,szPing,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
	{
		DisplayRunTimeInfo("Start run ping.exe fail!");
		return 0;
	}

	CloseHandle(pi.hThread);
	CloseHandle(hWritePipe);
	CloseHandle(pi.hProcess);

	DWORD  dwPingFailCount    = 35;
	DWORD  dwPingSuccessCount = nSuccessCount;
	
	while(TRUE)
	{
		memset(szReadFromPipeData,0,MAXBUFSIZE);
		Sleep(100);
		int bResult = ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
		
		if(!bResult)
		{
			DisplayRunTimeInfo("Read ping.exe fail!");
			return 0;
		}

		Sleep(200);
		char IPInfo[MINBUFSIZE] ="";
		sprintf_s(IPInfo, MINBUFSIZE, "Reply from %s", IP);
		
		if(strstr(szReadFromPipeData,IPInfo))
		{
			dwPingSuccessCount--;
		}
		else
		{
			dwPingFailCount--;
			////----add by Michael for reduce EH20 error code
			//if(strcmp(IP,m_IniFileInfo.sz2GThrotghputGoldenIP))
			//{
			//	if(dwPingFailCount  == 35|| dwPingFailCount  == 20||(dwPingFailCount == 15))
			//	{
			//		
	  //              char    szGoldenCmd[256] = "";	

			//		SendGoldenCommand("wl up" , "ok" , m_IniFileInfo.nTimeOut);

			//		Sleep(600);
			//		sprintf_s(szGoldenCmd,"startconn %s %d",m_IniFileInfo.sz2GThrotghputGoldenSSID,45);
			//		SendGoldenCommand(szGoldenCmd , "success" , m_IniFileInfo.nTimeOut);
			//		memset(szGoldenCmd,0,sizeof(szGoldenCmd));
			//	}
			//}
			////----end----
		}
		DisplayRunTimeInfo(szReadFromPipeData);
		if(!dwPingSuccessCount)
		{
			ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
			DisplayRunTimeInfo(szReadFromPipeData);
			return 1;
		}
		if(!dwPingFailCount)
			return 0;
	}
}

bool CRunInfo::RunTelnetExe(char* GetMacValue)
{
	char szRunTelnetExeFullName[MAXBUFSIZE] = "";
	sprintf_s(szRunTelnetExeFullName, MAXBUFSIZE, "telnetenable %s %s Gearguy Geardog", m_IniFileInfo.szDefaultIP1, GetMacValue);
	
	STARTUPINFO si;
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	if(!CreateProcess(NULL,szRunTelnetExeFullName,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
	{
		DisplayRunTimeInfo("Not found Telnetenble.exe\n Test over");
		return false;
	}

	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess,5000))
	{
			TerminateProcess(pi.hProcess,1);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return false;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}
UINT __cdecl  CRunInfo::Debug_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	//check DUT boot up
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	//change "/"in FW to "_" for SFIS
	if(!pRun->DutCheckVer())
	{
		return 0;
	}

	//check BoardId
	if(!pRun->DutCheckBoardID())
	{
		pRun->SendSfisResultAndShowFail(CC60);
		return 0;
	}

	//if(!pRun->DutPerPT())
	//{
	//	pRun->SendSfisResultAndShowFail(BF10);
	//	return 0;
	//}

	////init goldsocket
	//if(pRun->m_IniFileInfo.nGoldenSeverFlag == 1)
	//{
	//	pRun->InitGoldenSocket();
	//}

	//// start wifi throughput test
	//iRet = pRun->DutTestWiFiThroughput_1();
	//char szBuf[128];
	//sprintf_s(szBuf , 128 , "iRet=%d" , iRet);
	//pRun->DisplayRunTimeInfo(szBuf);
	//switch (iRet)
	//{
	//case 0:
	//	break;
	//case 1:
	//	pRun->SendSfisResultAndShowFail(SY55);
	//	return 0;
	//case 2:
	//	pRun->SendSfisResultAndShowFail(EH20);
	//	return 0;
	//case 3:
	//	pRun->SendSfisResultAndShowFail(EH50);
	//	return 0;
	//case 4: 
	//	pRun->SendSfisResultAndShowFail(EH25);
	//	return 0;
	//case 5: 
	//	pRun->SendSfisResultAndShowFail(EH22);
	//	return 0;
	//case 6: 
	//	pRun->SendSfisResultAndShowFail(TP51);
	//	return 0;
	//case 7:
	//	pRun->SendSfisResultAndShowFail(TP56);
	//	return 0;
	//case 8:
	//	pRun->SendSfisResultAndShowFail(TP21);
	//	return 0;
	//case 9:
	//	pRun->SendSfisResultAndShowFail(TP26);
	//	return 0;
	//case 10:
	//	pRun->SendSfisResultAndShowFail(TP52);
	//	return 0;
	//case 11: 
	//	pRun->SendSfisResultAndShowFail(TP53);
	//	return 0;
	//case 12: 
	//	pRun->SendSfisResultAndShowFail(TP55);
	//	return 0;
	//case 13: 
	//	pRun->SendSfisResultAndShowFail(TP22);
	//	return 0;
	//case 14: 
	//	pRun->SendSfisResultAndShowFail(TP23);
	//	return 0;
	//case 15: 
	//	pRun->SendSfisResultAndShowFail(TP25);
	//	return 0;
	//default:
	//	break;
	//}

	/*if(!pRun->DutBurnBF())
	{
		pRun->SendSfisResultAndShowFail(BF00);
		return 0;
	}*/

	/*if(!pRun->DutCheckBF())
	{
		pRun->SendSfisResultAndShowFail(BF10);
		return 0;
	}*/
	/*if(!pRun->CheckFirmwareVersion())
	{
		pRun->SendSfisResultAndShowFail(CC70);
		return 0;
	}*/
	//if(!pRun->DutDownloadKey())
	//{
	//	pRun->SendSfisResultAndShowFail(DK37);
	//	return 0;
	//}
	//
	//if(!pRun->DutBurnBF())
	//{
	//	pRun->SendSfisResultAndShowFail(BF00);
	//	return 0;
	//}

	//if(!pRun->DutCheckOpenVPN())
	//{
	//	pRun->SendSfisResultAndShowFail(CK10);
	//	return 0;
	//}
	//
	//if(!pRun->DutCheckBF())
	//{
	//	pRun->SendSfisResultAndShowFail(BF10);
	//	return 0;
	//}

	////fix wan port
	//if(!pRun->DutFixWanSetting())
	//{
	//	pRun->SendSfisResultAndShowFail(SY75);
	//	return 0;
	//}

	////Sleep a time:RC find some wan port setting failed DUT
	//Sleep(7000);

	//if(IP_Ping("192.168.1.1", 2 , 64 ))
	//{		
	//	pRun->DisplayRunTimeInfo("ping ok!");
	//}
	//else
	//{
	//	AfxMessageBox("請不要提前拔下電源！");
	//	pRun->SendSfisResultAndShowFail(TQ80);
	//	return 0;
	//}
	////Check LED
	/*if(!pRun->DutCheckLed())	
	{
		return 0;
	}*/

	////Check LED
	//if(!pRun->DutCheckLed_PT2())	
	//{
	//	return 0;
	//}

	//if(!pRun->CHECK_SSID_2G( "NETGEAR82"))
	//{
	//	pRun->SendSfisResultAndShowFail(CC71);
	//	return 0;
	//}

	////check USB	
	//
	//iRet = pRun->DutCheckUsb();		
	////kill httpd process
	//pRun->SendDutCommand("killall httpd","#",10000);

	//pRun->DisplayRunTimeInfo("pRun->DutCheckUsb() return value iRet=" , iRet);
	//switch(iRet)
	//{
	//case -1:
	//	break;
	//case 1:
	//	pRun->SendSfisResultAndShowFail(UB01);
	//	return 0;
	//case 2:
	//	pRun->SendSfisResultAndShowFail(UB02);
	//	return 0;
	//case 4:
	//	pRun->SendSfisResultAndShowFail(UB03);
	//	return 0;
	//case 5:
	//	pRun->SendSfisResultAndShowFail(UB04);
	//	return 0;
	//case 6:
	//	pRun->SendSfisResultAndShowFail(UB05);
	//	return 0;
	//case 7:
	//	pRun->SendSfisResultAndShowFail(UB06);
	//	return 0;
	//case 8:
	//	pRun->SendSfisResultAndShowFail(UB07);
	//	return 0;
	//case 9:
	//	pRun->SendSfisResultAndShowFail(UB08);
	//	return 0;
	//case 10:
	//	pRun->SendSfisResultAndShowFail(UB08);
	//	return 0;
	//case 11:
	//	pRun->SendSfisResultAndShowFail(UB09);
	//	return 0;
	//case 12:
	//	pRun->SendSfisResultAndShowFail(UB10);
	//	return 0;
	//case 31:
	//	pRun->SendSfisResultAndShowFail(UB31);
	//	return 0;
	//case 32:
	//	pRun->SendSfisResultAndShowFail(UB32);
	//	return 0;
	//case 34:
	//	pRun->SendSfisResultAndShowFail(UB33);
	//	return 0;
	//case 35:
	//	pRun->SendSfisResultAndShowFail(UB34);
	//	return 0;
	//case 36:
	//	pRun->SendSfisResultAndShowFail(UB35);
	//	return 0;
	//case 37:
	//	pRun->SendSfisResultAndShowFail(UB36);
	//	return 0;
	//case 38:
	//	pRun->SendSfisResultAndShowFail(UB37);
	//	return 0;
	//case 39:
	//	pRun->SendSfisResultAndShowFail(UB38);
	//	return 0;
	//case 40:
	//	pRun->SendSfisResultAndShowFail(UB38);
	//	return 0;
	//case 41:
	//	pRun->SendSfisResultAndShowFail(UB39);
	//	return 0;
	//case 42:
	//	pRun->SendSfisResultAndShowFail(UB40);
	//	return 0;
	//case 91:
	//	pRun->SendSfisResultAndShowFail(UB00);
	//	return 0;
	//}
	// Test end, show pass
	pRun->SendSfisResultAndShowPass();

	return 1;
}

void CRunInfo::PreSetting()
{
	DisplayRunTimeInfo("\nenable wifi\r\n\n");
	SendDutCommand("\n","#",1000);

	//SendDutCommand("nvram set wl0_radio=1","#",1500);
	//SendDutCommand("nvram set wl1_radio=1","#",1500);
	//SendDutCommand("nvram commit","#",1500);
	//SendDutCommand("wifi","#",5000);
	//Sleep(2000);

	DisplayRunTimeInfo("\ndisable 5G,enable 2G\r\n\n");
	//SendDutCommand("wlconf wl0 down","#",1500);
	SendDutCommand("ifconfig wl0 down","#",1500);
	Sleep(1000);
	//SendDutCommand("wlconf wl1 down","#",1000);
	SendDutCommand("ifconfig wl1 down","#",1000);
	Sleep(1000);
	//SendDutCommand("wlconf wl1 up","wl",1500);
	SendDutCommand("ifconfig wl1 up","wl",1500);
	//SendDutCommand("wl -i wl1 up","#",1500);
	Sleep(1000);
	//SendDutCommand("ifconfig wl1 up","wl",1500);
	//Sleep(1000);
	//SendDutCommand("wlconf wl1 start","#",1000);
	//Sleep(1000);
	
	//SendDutCommand("param set wlg_secu_type=None","#",1000);
	//SendDutCommand("param save","#",1000);    //mike add
	//SendDutCommand("param show wla_wlanstate","#",1000);

	//if(strstr(m_strStoreEthernetData,"wla_wlanstate=Disable") != NULL)
	//{
	//	SendDutCommand("param set wla_wlanstate=Enable","#",1000);
	//	m_flagwifistar=true;
	//}
	//else
	//	m_flagwifistar=false;


	//SendDutCommand("param show wlg_wlanstate","#",1000);
	//if(strstr(m_strStoreEthernetData,"wlg_wlanstate=Disable") != NULL)
	//{

	//	SendDutCommand("param set wlg_wlanstate=Enable","#",1000);
	//	m_flagwifistar=true;
	//}
	//else
	//	m_flagwifistar=false;
	//if(m_flagwifistar)
	//{
	//	SendDutCommand("killall wlanconfigd","#",3000);
	//	SendDutCommand("wlanconfigd &","#",3000);
	//	//Sleep(pRun->m_IniFileInfo.nTimeOut-3000);
	//	//Sleep(3000);
	//	//Sleep(20000);//mike modify
	//	Sleep(m_IniFileInfo.nTimeOut);

	//}
}

void CRunInfo::AfterSetting()
{
	DisplayRunTimeInfo("\ndisable wifi\r\n\n");
	SendDutCommand("\n","#",1000);

	//SendDutCommand("nvram set wl0_radio=0","#",1500);
	//SendDutCommand("nvram set wl1_radio=0","#",1500);
	//SendDutCommand("nvram commit","#",1500);
	SendDutCommand("ifconfig wl0 down","#",1500);
	SendDutCommand("ifconfig wl1 down","#",1500);
	Sleep(2000);
}

UINT __cdecl  CRunInfo::FT_FunctionTestRun(LPVOID parameter)
{

	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;
	
	//pRun->DisplayRunTimeInfo("Test FT Item");
	pRun->DelArp();
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	if(!pRun->DutCheckVer())
	{
		return 0;
	}
	
	if(!pRun->DutBurnET0Mac())
	{
		pRun->SendSfisResultAndShowFail(WC10);
		return 0;
	}

	if(!pRun->DutBurnMice2GMac())
	{
		pRun->SendSfisResultAndShowFail(WC20);
		return 0;
	}

	if(!pRun->DutBurnMice5GMac())
	{
		pRun->SendSfisResultAndShowFail(WC50);
		return 0;
	}

	pRun->SendDutCommand("setssid_fact","#",100);
	pRun->SendDutCommand("bdata set SN=480800000009","#",100);
	pRun->SendDutCommand("bdata set model=R1CM","#",100);
	pRun->SendDutCommand("bdata commit","#",100);
	pRun->SendDutCommand("bdata sync","#",100);
	Delay(1500);

    pRun->PreSetting();
	iRet=pRun->CheckThroughputTest();

	pRun->Ch6Ant0();
	Sleep(100);
	if(!pRun->DutPowerMeterTest(1))
	{
		pRun->SendSfisResultAndShowFail("PW00");
		return 0;
	}

	pRun->Ch6Ant1();
	Sleep(100);
	if(!pRun->DutPowerMeterTest(2))
	{
		pRun->SendSfisResultAndShowFail("PW01");
		return 0;
	}

	pRun->SendDutCommand("iwpriv wl1 set ATE=ATESTOP","#",100);

	if(-1 != iRet)
	{
		pRun->AfterSetting();
	}
	switch(iRet)
	{
	case -1:
		break;
	case 5:
		pRun->SendSfisResultAndShowFail(EH20);
		return 0;
	case 6:
		pRun->SendSfisResultAndShowFail(TP21);
		return 0;
	case 7:
		pRun->SendSfisResultAndShowFail(TP22);
		return 0;
	case 8:
		pRun->SendSfisResultAndShowFail(TP24);
		return 0;
	}
	pRun->DisplayRunTimeInfo("2G finish\r\n\n");

	

	pRun->DisplayRunTimeInfo("enable 5G,disable 2G\r\n\n");
	iRet=pRun->Check5GThroughputTest();


	pRun->Ch153Ant0();
	Sleep(1000);
	if(!pRun->DutPowerMeterTest(3))
	{
		pRun->SendSfisResultAndShowFail("PW02");
		return 0;
	}
	pRun->Ch153Ant1();
	Sleep(1000);
	if(!pRun->DutPowerMeterTest(4))
	{
		pRun->SendSfisResultAndShowFail("PW03");
		return 0;
	}

	pRun->SendDutCommand("iwpriv wl0 set ATE=ATESTOP","#",100);

	//pRun->AfterSetting();
	switch(iRet)
	{
	case -1:
		break;
	case 9:
		pRun->SendSfisResultAndShowFail(EH50);
		return 0;
	case 10:
		pRun->SendSfisResultAndShowFail(TP51);
		return 0;
	case 11:
		pRun->SendSfisResultAndShowFail(TP52);
		return 0;
	case 12:
		pRun->SendSfisResultAndShowFail(TP53);
		return 0;
	}
	pRun->DisplayRunTimeInfo("5G finish\r\n\n");

	
	
	//if(!pRun->DutCheckHDD())
	//{
	//	pRun->SendSfisResultAndShowFail(CC60);
	//	return 0;
	//}

	//if(!pRun->DutBurnSn())
	//{
	//	pRun->SendSfisResultAndShowFail(WC40);
	//	return 0;
	//}

	//if(strcmp(pRun->m_IniFileInfo.szIsHaveSSIDLable , "1") == 0)
	//{

	//	if(!pRun->DutBurnMiceSSID())
	//	{
	//		pRun->SendSfisResultAndShowFail(WC90);
	//		return 0;
	//	}
	//}

	//if(!pRun->DutBurnET0Mac())
	//{
	//	pRun->SendSfisResultAndShowFail(WC10);
	//	return 0;
	//}

	//if(!pRun->DutBurnMice2GMac())
	//{
	//	pRun->SendSfisResultAndShowFail(WC20);
	//	return 0;
	//}

	//if(!pRun->DutBurnMice5GMac())
	//{
	//	pRun->SendSfisResultAndShowFail(WC50);
	//	return 0;
	//}

	//add 2014-02-16 for R1D
	//if(!pRun->DutBurnModel())
	//{
	//	pRun->SendSfisResultAndShowFail(WM00);
	//	return 0;
	//}
	//add end

	//do not test until S3 build
	//if(!pRun->DutCheckFAN())
	//{
	//	pRun->SendSfisResultAndShowFail(CF00);
	//	return 0;
	//}
	//end

	//if(!pRun->DutDisableWireless())
	//{
	//	pRun->SendSfisResultAndShowFail(DS10);
	//	return 0;
	//}

	//if(IP_Ping("192.168.1.1", 2 , 64 ))



	/*if(IP_Ping("192.168.31.1", 2 , 64 ))
	{		
		pRun->DisplayRunTimeInfo("ping ok!");
	}
	else
	{
		AfxMessageBox("請不要提前拔下電源！");
		pRun->SendSfisResultAndShowFail(TQ80);
		return 0;
	}*/

	pRun->SendSfisResultAndShowPass();
	return 1;
}

int CRunInfo::CheckThroughputTest()
{

	char szSetChannelCommand[MINBUFSIZE]  = "";
	//if(!strcmp(m_IniFileInfo.nTestThroughput, "1"))////2.4G
	if(1 == m_IniFileInfo.nTestThroughput)////2.4G
	{
		double     TXValue = 0;
		double     RXValue = 0;
		double     TXRXValue =0;
		char szSSID[MINBUFSIZE]="";

		char       szShowTXResult[MINBUFSIZE] = "";
		char       szShowRXResult[MINBUFSIZE] = "";
		char       szShowTXRXResult[MINBUFSIZE] = "";
		char       szCovertValueToCharChannelTX[MINBUFSIZE] = "";
		char       szCovertValueToCharChannelRX[MINBUFSIZE] = "";
		char       szCovertValueToCharChannelTXRX[MINBUFSIZE] = "";

		CString strIP;
		CString szIperfInfo;
		char    szIperfCmd[MINBUFSIZE] = "";
		char    szIperfCmdFAI[MINBUFSIZE] ="";

		char    szPingGolden[MAXBUFSIZE] = "";
		char    szarp_d[MAXBUFSIZE];
		GetDlgItemText(IDC_IPADDRESS_GOLDENIP, strIP);
		sprintf_s(szarp_d,MAXBUFSIZE,"arp -d %s","192.168.31.2");//mike add 2013-6-26
		//	AfxMessageBox("Initial OK");
		sprintf_s(szPingGolden,MAXBUFSIZE,"ping %s -t","192.168.31.2");
		sprintf_s(szIperfCmd  ,MINBUFSIZE, m_IniFileInfo.szStoreIperfPara, "192.168.31.2");
		sprintf_s(szIperfCmdFAI  ,MINBUFSIZE, m_IniFileInfo.szStoreIperfParaFAI, "192.168.31.2");

		CString strChannel1;
		CString strChannel2;
		CString strChannel3;
		//for(int i = 1; i<=pRun->m_IniFileInfo.nChannelNum; i++)
		for(int i = 1; i<=1; i++)//jack 12.28

		{
			DeleteSpecifyExe("iperf.exe"); // kill iperf.exe process to make sure the program not hang up.
			DWORD  dwTXReTestCount = 4;//modify 3times
			DWORD  dwRXReTestCount = 4;

			//ResetEvent(pRun->m_hEvt);
			//pRun->SendEthernetCmd("wl -i wl1 down");
			//if(WAIT_OBJECT_0 != WaitForSingleObject(pRun->m_hEvt,15000))
			//{
			//	pRun->TestFail("Wait Timeout");
			//	return 0;
			//}


			//SendDutCommand("wl -i wl1 down","#",3000);
			//Sleep(1000);//
			//SendDutCommand("wl -i wl0 down","#",3000);
			//Sleep(1000);//
			//SendDutCommand("wl -i wl1 isup","0",1000);
			//SendDutCommand("wl -i wl1 band b","#",3000);
			//SendDutCommand("wl -i wl1 mimo_bw_cap 1","#",3000);
			//SendDutCommand("wl -i wl1 country ALL","#",3000);
			//SendDutCommand("wl -i wl1 ampdu 1","#",3000);
			//strIP.GetBuffer(strIP.GetLength())
			SendDutCommand(szarp_d,"",2000);//mike add 2013-6-26

			CString strSSID;
			char szSetSSIDCommand[MINBUFSIZE]  = "";
			GetDlgItemText(IDC_EDIT_SSID, strSSID);
			//sprintf_s(szSetSSIDCommand,100,"wl -i wl1 ssid %s",strSSID);


			sprintf_s(szSetSSIDCommand,100,"iwpriv wl1 set Channel=%s","6");
			SendDutCommand(szSetSSIDCommand,"#",3000);//mike modify


			sprintf_s(szSetSSIDCommand,100,"iwpriv wl1 set SSID=%s","Xiaomi-1-2.4");
			SendDutCommand(szSetSSIDCommand,"#",3000);//mike modify


			/*SendDutCommand("wl -i wl1 interference 4","#",3000);*/

			switch(i)
			{
			case 1:
				/*GetDlgItemText(IDC_EDIT_CHANNEL1,strChannel1);
				if(atoi(strChannel1)==5||atoi(strChannel1)==7)
				{

					sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl1 chanspec -c %s -b 2 -w 40 -s -1",strChannel1);
				}
				else

				{

					sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl1 chanspec -c %s -b 2 -w 40 -s 1",strChannel1);
				}*/
				//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel1);			//090714

				break;
			case 2:
				GetDlgItemText(IDC_EDIT_2G_CHANNEL1,strChannel1);
				if(atoi(strChannel1)==5||atoi(strChannel1)==7)
				{
					sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl1 chanspec -c %s -b 2 -w 40 -s -1",strChannel1);
				}
				else
				{

					//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel2);		//	090714
					sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl1 chanspec -c %s -b 2 -w 40 -s 1",strChannel1);
				}

				break;
			case 3://jack 12.28
				GetDlgItemText(IDC_EDIT_2G_CHANNEL1,strChannel1);
				if(atoi(strChannel1)==5||atoi(strChannel1)==7)
				{
					//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel3);			//090714
					sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl1 chanspec -c %s -b 2 -w 40 -s -1",strChannel1);
				}
				else
				{
					//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel3);			//090714
					sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl1 chanspec -c %s -b 2 -w 40 -s 1",strChannel1);
				}

				break;


			}


			//char    szGoldenCmd[256] = ""; 
			//SendCmdToGolden("CLOSE");
			//Sleep(700);
			//SendCmdToGolden("OPEN");//===>if it fix wifi SSID
			//Sleep(500);
			//sprintf_s(szGoldenCmd,"startconn %s %d",strSSID.GetBuffer(strSSID.GetLength()),22);
			//SendCmdToGolden(szGoldenCmd);
			//memset(szGoldenCmd,0,sizeof(szGoldenCmd));
			//Sleep(3000);//mike modify

			while(dwTXReTestCount--)
			{
				//			memset(szIperfInfo,0,1024);
				szIperfInfo.Empty();
				DeleteSpecifyExe("iperf.exe"); // kill iperf.exe process to make sure the program not hang up.
				DisplayRunTimeInfo("\r\n--------------Begin ping golden PC--------------\r\nnote:pls check wireless net adapter's IP addr and ssid on golden PC\r\n\n");

				//if(!PingSpecifyIP("192.168.31.2",m_IniFileInfo.nPingGoldenCount))//3   jack 4.11
				//{
				//	//SendSfisResultAndShowFail(EH20);

				//	return 5;
				//}

				if(!IP_Ping("192.168.31.2" , 20 , 64 ))
				{
					return 9;
				}

				DisplayRunTimeInfo("\r\nping 192.168.31.2 pass\n");
				char stop[]={0X03};
				SendCmdTo2GGolden(stop);
				Sleep(100);
				SendCmdTo2GGolden(stop);
				Sleep(100);
				SendCmdTo2GGolden("iperf -s -w 128k");
				DisplayRunTimeInfo("\r\nstart iperf in golden pass\r\n");

			

				TXValue = 0;
				RXValue = 0;
				TXRXValue=0;
				DisplayRunTimeInfo("\r\nStart 2.4 G throughput test\r\n");
				szIperfInfo.Empty();
				RunSpecifyExeAndRead(szIperfInfo,szIperfCmdFAI,false);  //modyfy 06.11

				GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue, RXValue);

				SendDutCommand("iwpriv wl1 stat","#",3000);//sheng-zhao for get RSSI.

				TXRXValue=TXValue+RXValue;
				switch(i)
				{
				case 1:
					//	sprintf_s(szShowTXResult, MINBUFSIZE, "Channel 3 TX throughput: %.2f Mbit\r\n", TXValue);
					//	sprintf_s(szShowRXResult, MINBUFSIZE, "Channel 3 RX throughput: %.2f Mbit\r\n", RXValue);
					sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel %s TX+RX throughput: %.2f Mbit\r\n", strChannel1,TXRXValue);
					sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %s TX throughput: %.2f Mbit\r\n", strChannel1, TXValue);	//	090714
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %s RX throughput: %.2f Mbit\r\n", strChannel1, RXValue);
					break;
				case 2:
					//sprintf_s(szShowTXResult, MINBUFSIZE, "Channel 6 TX throughput: %.2f Mbit\r\n",TXValue);
					//sprintf_s(szShowRXResult, MINBUFSIZE, "Channel 6 RX throughput: %.2f Mbit\r\n",RXValue);
					//	//sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel 6 TX+RX throughput: %.2f Mbit\r\n", TXRXValue);
					sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %s TX throughput: %.2f Mbit\r\n", strChannel2, TXValue);		//090714
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %s RX throughput: %.2f Mbit\r\n", strChannel2, RXValue);
					break;
				case 3:
					//sprintf_s(szShowTXResult, MINBUFSIZE, "Channel 9 TX throughput: %.2f Mbit\r\n",TXValue);
					//sprintf_s(szShowRXResult, MINBUFSIZE, "Channel 9 RX throughput: %.2f Mbit\r\n",RXValue);
					//sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel 9 TX+RX throughput: %.2f Mbit\r\n", TXRXValue);
					sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %s TX throughput: %.2f Mbit\r\n",strChannel3, TXValue);		//090714
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %s RX throughput: %.2f Mbit\r\n",strChannel3, RXValue);
					break;
				}

				DisplayRunTimeInfo(szShowTXResult);
				DisplayRunTimeInfo(szShowRXResult);
				DisplayRunTimeInfo(szShowTXRXResult);
				DisplayRunTimeInfo("\r\n");
				//AfxMessageBox(m_IniFileInfo.nTXThroughputSpec);//mike add
				//if(/*TXValue >=m_IniFileInfo.nTXThroughputSpec && RXValue >=m_IniFileInfo.nRXThroughputSpec)&&*/ TXRXValue >= m_IniFileInfo.nTXRXThroughputSpec)
				if(TXValue>=m_IniFileInfo.nTXThroughputSpec && RXValue>=m_IniFileInfo.nRXThroughputSpec)//modify for NN
				{
					break;
					return -1;
				}
				else if(TXRXValue>m_IniFileInfo.nTXRXThroughputSpec)
				{
					break;
					return -1;
				}

				//else if(TXValue<m_IniFileInfo.nTXThroughputSpec)
				//
				//{	
				//	if(dwTXReTestCount)
				//		continue;
				//	else
				//	{
				//		sprintf_s(szCovertValueToCharChannelTX, MINBUFSIZE, "%.2f(TX-2.4G)", TXValue);
				//		m_strRecordTestData  +=  szCovertValueToCharChannelTX;
				//		m_TestRecordinfo.szRecord270MTx += CString(szCovertValueToCharChannelTX);
				//		m_strRecordTestData  +=  "\t";
				//		//SendSfisResultAndShowFail(TP21);
				//		return 6;
				//	}
				//}
				//else if(RXValue<m_IniFileInfo.nRXThroughputSpec)

				//{
				//	if(dwTXReTestCount)
				//		continue;
				//	else
				//	{
				//		sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX-2.4G)",TXValue);
				//		m_strRecordTestData  +=  szCovertValueToCharChannelRX;
				//		m_TestRecordinfo.szRecord270MRx += CString(szCovertValueToCharChannelRX);
				//		m_strRecordTestData  +=  "\t";
				//		//SendSfisResultAndShowFail(TP22);
				//		return 7; 
				//	}
				//}

				else if(TXRXValue<m_IniFileInfo.nTXRXThroughputSpec)
				{
					if(dwTXReTestCount)
						continue;
					else
					{
						sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX-2.4G)", TXRXValue);
						m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
						m_TestRecordinfo.szRecord270MRxTx=szCovertValueToCharChannelTXRX;
						m_strRecordTestData  +=  "\t";
						//pRun->SendSfisResultAndShowFail(TP24);
						return 8;
					}
				}
			}


			sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX-2.4G)",TXValue);
			m_strRecordTestData  +=  szCovertValueToCharChannelTX;
			m_TestRecordinfo.szRecord270MTx=szCovertValueToCharChannelTX;
			m_strRecordTestData  +=  "\t";
			sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX-2.4G)",RXValue);
			m_strRecordTestData  +=  szCovertValueToCharChannelRX;
			m_TestRecordinfo.szRecord270MRx = szCovertValueToCharChannelRX;
			m_strRecordTestData  +=  "\t";
			sprintf_s(szCovertValueToCharChannelTXRX,MINBUFSIZE,"%.2f(TXRX-2.4G)",TXRXValue);
			m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
			//m_TestRecordinfo.szRecord270MTxRx=szCovertValueToCharChannelTXRX;
			m_TestRecordinfo.szRecord270MRxTx=szCovertValueToCharChannelTXRX;
			m_strRecordTestData  +=  "\t";
			//SendCmdToGolden("CLOSE");
			//Sleep(500);//mike modify 2013-6-26
		}
	}
	/////////////////////WiFi 2.4G Test End//////////////////////////////////////////////////////////
	return -1;

}

int CRunInfo::CheckRC1Ping()
{
	for(int i=3;i>=0;i++)
	{
		if(!PingSpecifyIP_RC1("192.168.31.1", 4))
		{
			DelArp();
			if(i>1)
				continue;
			else
			{
				return 0;
			}
		}
		else
		{
			return 1;
		}
	}

	return 1;
	/*if(!PingSpecifiedIP("192.168.31.1",4))
		return 0;
	else
		return 1;*/
}

int CRunInfo::Check5GThroughputTest()
{
	char szSetChannelCommand[MINBUFSIZE]  = "";
	SendDutCommand("ifconfig wl1 down","#",3000);
	//for(int i=0;i<3;i++)
	//{
	//	if(strstr(g_data,"Usage: wlconf <ifname> up|down"))
	//	{
	//		SendDutCommand("wlconf wl1 down","#",3000);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	Sleep(1000);
	//SendDutCommand("wlconf wl0 up","wl",1000);
	SendDutCommand("ifconfig wl0 up","wl",1000);
	Sleep(1000);
	//SendDutCommand("ifconfig wl0 up","wl",1000);
	//Sleep(1000);
	//SendDutCommand("wlconf wl0 start","#",1000);
	//Sleep(1000);
	//if(!strcmp(m_IniFileInfo.GnTestThroughput, "1"))
	if(1 == m_IniFileInfo.GnTestThroughput)
	{
		double     TXValue = 0;
		double     RXValue = 0;
		double     TXRXValue =0;

		char       szShowTXResult[MINBUFSIZE] = "";
		char       szShowRXResult[MINBUFSIZE] = "";
		char       szShowTXRXResult[MINBUFSIZE] = "";
		char       szCovertValueToCharChannelTX[MINBUFSIZE] = "";
		char       szCovertValueToCharChannelRX[MINBUFSIZE] = "";
		char       szCovertValueToCharChannelTXRX[MINBUFSIZE] = "";

		CString strIP;
		CString szIperfInfo;
		char    szIperfCmd[MINBUFSIZE] = "";
		char    szIperfCmdFAI[MINBUFSIZE] ="";

		char    szPingGolden[MAXBUFSIZE] = "";

		GetDlgItemText(IDC_IPADDRESS_GOLDENIP, strIP);
		char    szarp_d[MAXBUFSIZE];
		//GetDlgItemText(IDC_IPADDRESS_GOLDENIP, strIP);
		sprintf_s(szarp_d,MAXBUFSIZE,"arp -d %s","192.168.31.5");//mike add 2013-6-26

		///////////////////
		CString strSSID;
		char szSetSSIDCommand[MINBUFSIZE]  = "";
		char szSSID[256] = "";
		GetDlgItemText(IDC_EDIT_SSID, strSSID);
		//sprintf_s(szSetSSIDCommand,100,"wl -i wl0 ssid %s",strSSID);


		sprintf_s(szSetSSIDCommand,100,"iwpriv wl0 set Channel=%s","153");
		SendDutCommand(szSetSSIDCommand,"#",3000);//mike modify


		sprintf_s(szSetSSIDCommand,100,"iwpriv wl0 set SSID=%s","Xiaomi-1-5");
		SendDutCommand(szSetSSIDCommand,"#",3000); //mike modify 2013-9-6


		//	AfxMessageBox("Initial OK");
		sprintf_s(szPingGolden,MAXBUFSIZE,"ping %s -t","192.168.31.5");
		sprintf_s(szIperfCmd  ,MINBUFSIZE, m_IniFileInfo.szStoreIperfPara, "192.168.31.5");
		sprintf_s(szIperfCmdFAI  ,MINBUFSIZE, m_IniFileInfo.szStoreIperfParaFAI, "192.168.31.5");

		CString strChannel1;
		CString strChannel2;
		CString strChannel3;

		for(int i = 1; i<=1; i++)//jack 12.28
		{
			DWORD  dwTXReTestCount = 4;
			DWORD  dwRXReTestCount = 4;
			
			SendDutCommand(szarp_d,"",2000);//mike add 2013-6-26



			switch(i)
			{
			case 1:
				//GetDlgItemText(IDC_EDIT_CHANNEL2,strChannel2);
				//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel1);		//	090714
				//sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl0 chanspec -c %s -b 5 -w 40 -s 1",strChannel2);
				//	sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wlg_channel=38");

				break;
			case 2:
				GetDlgItemText(IDC_EDIT_5G_CHANNEL1,strChannel2);
				//		sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel2);		//	090714
				sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl0 chanspec -c %s -b 5 -w 40 -s 1",strChannel2);
				//		//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wlg_channel=54");

				//		break;
			case 3:
				GetDlgItemText(IDC_EDIT_5G_CHANNEL1,strChannel2);
				//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wla_channel=%s", strChannel3);		//	090714
				sprintf_s(szSetChannelCommand, MINBUFSIZE, "wl -i wl0 chanspec -c %s -b 5 -w 40 -s 1",strChannel2);
				//		//sprintf_s(szSetChannelCommand, MINBUFSIZE, "param set wlg_channel=151");
				//	//	break;
			}


			//Sleep(3000);//DELETE FOR COSTDOWN
			//char    szGoldenCmd[256] = ""; //jack 12.29
			//SendCmdToGolden("CLOSE");
			//Sleep(1200);
			//SendCmdToGolden("OPEN");
			//Sleep(1500);
			//sprintf_s(szGoldenCmd,"startconn %s %d",strSSID.GetBuffer(strSSID.GetLength()),22);
			//SendCmdToGolden(szGoldenCmd);
			//memset(szGoldenCmd,0,sizeof(szGoldenCmd));
			//Sleep(3000);

			while(dwTXReTestCount--)
			{
				//			memset(szIperfInfo,0,1024);
				szIperfInfo.Empty();
				DeleteSpecifyExe("iperf.exe"); // kill iperf.exe process to make sure the program not hang up
				DisplayRunTimeInfo("\r\n--------------Begin ping golden PC--------------\r\nnote:pls check wireless net adapter's IP addr and ssid on golden PC\r\n\n");


				//if(!PingSpecifyIP("192.168.31.5",m_IniFileInfo.nPingGoldenCount))///3
				//{
				//	//SendSfisResultAndShowFail(EH50);
				//	return 9;
				//}

				if(!IP_Ping("192.168.31.5" , 20 , 64 ))
				{
					return 9;
				}
				DisplayRunTimeInfo("\r\nping 192.168.31.5 pass\n");
				char stop[]={0X03};
				SendCmdTo5GGolden(stop);
				Sleep(100);
				SendCmdTo5GGolden(stop);
				Sleep(100);
				SendCmdTo5GGolden("iperf -s -w 128k");
				DisplayRunTimeInfo("\r\nstart iperf in golden pass\r\n");
				Sleep(3000);


				TXValue = 0;
				RXValue = 0;
				DisplayRunTimeInfo("\r\nStart 5G throughput test\r\n");
				szIperfInfo.Empty();

				DisplayRunTimeInfo(szIperfCmdFAI);
				DisplayRunTimeInfo("\r\n%s\r\n",szIperfCmdFAI);
				RunSpecifyExeAndRead(szIperfInfo,szIperfCmdFAI,false);  //modyfy 06.11
				DisplayRunTimeInfo("\r\ntest0\r\n");
				GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue, RXValue);
				DisplayRunTimeInfo("\r\ntest1\r\n");

				SendDutCommand("iwpriv wl0 stat","#",3000);//sheng-zhao for get RSSI.

				TXRXValue = TXValue + RXValue;

				//RunSpecifyExeAndRead(szIperfInfo,szIperfCmdFAI,false);  //modyfy 06.11
				//GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue, RXValue);

				switch(i)
				{
				case 1:
					sprintf_s(szShowTXResult, MINBUFSIZE, "Channel 46 TX throughput: %.2f Mbit\r\n", TXValue);
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel 46 RX throughput: %.2f Mbit\r\n", RXValue);
					sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel 46 TX+RX throughput: %.2f Mbit\r\n", TXRXValue);
					/*sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %s TX throughput: %.2f Mbit\r\n", strChannel1, TXValue);		090714
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %s RX throughput: %.2f Mbit\r\n", strChannel1, RXValue);*/
					break;
				case 2:
					sprintf_s(szShowTXResult, MINBUFSIZE, "Channel 46TX throughput: %.2f Mbit\r\n",TXValue);
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel 46 RX throughput: %.2f Mbit\r\n",RXValue);
					sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel 54 TX+RX throughput: %.2f Mbit\r\n", TXRXValue);
					//	/*sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %s TX throughput: %.2f Mbit\r\n", strChannel2, TXValue);		090714
					//	sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %s RX throughput: %.2f Mbit\r\n", strChannel2, RXValue);*/
					//	break;
				case 3:
					sprintf_s(szShowTXResult, MINBUFSIZE, "Channel  54TX throughput: %.2f Mbit\r\n",TXValue);//151
					sprintf_s(szShowRXResult, MINBUFSIZE, "Channel  54 RX throughput: %.2f Mbit\r\n",RXValue);
					//	sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel 151 TX+RX throughput: %.2f Mbit\r\n", TXRXValue);
					///*	sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %s TX throughput: %.2f Mbit\r\n",strChannel3, TXValue);		090714
					//	sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %s RX throughput: %.2f Mbit\r\n",strChannel3, RXValue);*/
					//	break;
				}

				DisplayRunTimeInfo(szShowTXResult);
				DisplayRunTimeInfo(szShowRXResult);
				DisplayRunTimeInfo(szShowTXRXResult);
				DisplayRunTimeInfo("\r\n");
				//if(/*TXValue >=m_IniFileInfo.GnTXThroughputSpec)&& (RXValue >= m_IniFileInfo.GnRXThroughputSpec))&&*/ TXRXValue >= m_IniFileInfo.nTXRXThroughputSpec5G)
				if(TXValue>=m_IniFileInfo.nTXThroughputSpec5G && RXValue>=m_IniFileInfo.nRXThroughputSpec5G)//modify for NN
				{
					break;
					return -1;
				}

				else if(TXRXValue>m_IniFileInfo.nTXRXThroughputSpec5G)
				{
					break;
					return -1;
				}
				else if(TXRXValue<m_IniFileInfo.nTXRXThroughputSpec5G)
				{
					if(dwTXReTestCount)
						continue;
					else
					{
						sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX-5G)", TXRXValue);
						m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
						m_strRecordTestData  +=  "\t";
						m_TestRecordinfo.szRecord270MTxRx_5g=CString(szCovertValueToCharChannelTXRX);
						//pRun->SendSfisResultAndShowFail(TP54);
						return 12;
					}
				}
			}

			sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX-5G)",TXValue);
			m_strRecordTestData  +=  szCovertValueToCharChannelTX;
			m_TestRecordinfo.szRecord270MTx_5g =CString(szCovertValueToCharChannelTX);
			m_strRecordTestData  +=  "\t";
			sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX-5G)",RXValue);
			m_strRecordTestData  +=  szCovertValueToCharChannelRX;
			m_TestRecordinfo.szRecord270MRx_5g = CString(szCovertValueToCharChannelRX);
			m_strRecordTestData  +=  "\t";
			sprintf_s(szCovertValueToCharChannelTXRX,MINBUFSIZE,"%.2f(TXRX-5G)",TXRXValue);
			m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
			m_TestRecordinfo.szRecord270MTxRx_5g=CString(szCovertValueToCharChannelTXRX);
			m_strRecordTestData  +=  "\t";
			//SendCmdToGolden("CLOSE");
			//SendDutCommand("","#",3000);

		}
	}
	return -1;
}

int CRunInfo::iLedCCDInit()
{
	int iReturn = 0;
	DisplayRunTimeInfo("start init LED CCD socket!");

	//if don't test return true
	/*if(LedCcdTestInfo.iLedCCDTestFlag != 1)
	{
		DisplayRunTimeInfo("Don't need test LED by CCD.");
		iReturn = 0;
		goto __exit;
	}*/

	//if cap.exe not find ,open LEDTEST.exe
	HWND hwndfind;
	hwndfind = ::FindWindow(NULL, "cap");
	if(::IsWindow(hwndfind) == false )
	{
		IP_KillProcName("LEDTEST.exe");
		Sleep(2000);
		char InsBuf[100]="LEDTEST.exe";
		RunOneProcess(InsBuf);
		Sleep(3000);
	}
	else
	{
		
	}

	//Init socket
	bool bResult = bLedCCDSocketInit(m_hWnd, 12639, "127.0.0.1");
	if(bResult == FALSE)
	{
		DisplayRunTimeInfo("LED CCD socket init failed.");
		iReturn = 1;
		goto __exit;
	}
	
	DisplayRunTimeInfo("Init LED CCD socket OK!");

	//put test window always top

__exit:
	return iReturn;

}


UINT __cdecl  CRunInfo::RC_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	/*#if 1
		iRet = pRun->iLedCCDInit();
		if(iRet != 0)
		{
			pRun->SendSfisResultAndShowFail(CCD0);	
			return 0;
		}
	#endif*/
	pRun->DelArp();
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	if(!pRun->DutCheckVer())
	{
		return 0;
	}

	//iRet = pRun->iLedCCDTest();
	//if(iRet == 0)
	//{
	//	pRun->SendSfisResultAndShowFail(LD50);
	//	return 0;
	//}

	//if(!pRun->DutCheckHDDInfo())
	//{
	//	pRun->SendSfisResultAndShowFail(CC65);
	//	return 0;
	//}

	if(!pRun->DutCheckUSB())
	{
		pRun->SendSfisResultAndShowFail(US00);
		return 0;
	}

	//if(!pRun->DutCheckSn())
	//{
	//	pRun->SendSfisResultAndShowFail(CC40);
	//	return 0;
	//}

	if(!pRun->DutCheckMAC())
	{
		pRun->SendSfisResultAndShowFail(CC10);
		return 0;
	}

	//if(!pRun->DutCheckSSID())
	//{
	//	pRun->SendSfisResultAndShowFail(CC81);
	//	return 0;
	//}
	//add 2014-02-16 for R1D
	/*if(!pRun->DutCheckModel())
	{
		pRun->SendSfisResultAndShowFail(CM81);
		return 0;
	}*/
	//add end

	if(!pRun->DutCheckButton_Auto())
	{
		pRun->SendSfisResultAndShowFail(RS00);
		return 0;
	}

	iRet = pRun->CHECK_LAN_PORT();	
	pRun->DisplayRunTimeInfo("pRun->CHECK_LAN_PORT() return value iRet=" , iRet);
	switch (iRet)
	{
	case -1:
		break;
	case 2:
		pRun->SendSfisResultAndShowFail(IN12);
		return 0;
	case 3:
		pRun->SendSfisResultAndShowFail(IN13);
		return 0;
	case 4:
		pRun->SendSfisResultAndShowFail(IN14);
		return 0;
	case 12: 
		pRun->SendSfisResultAndShowFail(TP12);
		return 0;
	case 13: 
		pRun->SendSfisResultAndShowFail(TP13);
		return 0;
	case 14: 
		pRun->SendSfisResultAndShowFail(TP14);
		return 0;	
	default:
		break;
	}

	iRet = pRun->CHECK_WAN_PORT();	
	pRun->DisplayRunTimeInfo("pRun->CHECK_WAN_PORT() return value iRet=" , iRet);
	switch (iRet)
	{
	case -1:
		break;
	case 1:
		pRun->SendSfisResultAndShowFail(SY75);
		return 0;
	case 2:
		pRun->SendSfisResultAndShowFail(IN50);
		return 0;
	case 3:
		pRun->SendSfisResultAndShowFail(TP50);
		return 0;
	default:
		break;
	}

	if(!pRun->DutLoadDefault())
	{
		pRun->SendSfisResultAndShowFail(OS50);
		return 0;
	}
	//all items test finished
	
	//Test end, show pass
	pRun->SendSfisResultAndShowPass();

	return 1;
}

UINT __cdecl  CRunInfo::DOWNLOAD_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	pRun->DelArp();
	if(!pRun->CheckDutBootUp_2())
	{
		return 0;
	}

	if(!pRun->DutCheckVerDL())
	{
		return 0;
	}

	if(!pRun->DutLoadDefault_2())
	{
		pRun->SendSfisResultAndShowFail(OS50);
		return 0;
	}
	//all items test finished
	
	//Test end, show pass
	pRun->SendSfisResultAndShowPass();
	return 1;
}

UINT __cdecl  CRunInfo::RC1_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	/*if(!pRun->PingLanIP())
	{
		return 0;
	}*/
	pRun->DelArp();
	if(!pRun->CheckRC1Ping())
	{
		pRun->SendSfisResultAndShowFail(IN10);
		return 0;
	}
	else
	{
		pRun->DisplayRunTimeInfo("ping lan port ok!!!");
	}

	/*if(!pRun->CheckNFCTagCard())
	{
		pRun->SendSfisResultAndShowFail(NT00);
		return 0;
	}
	else
	{
		pRun->DisplayRunTimeInfo("NFC tag check ok!!!");
	}*/
	for(int i=0;i<10;i++)
	{
		Sleep(1000);
	}

	/*if(IDYES==AfxMessageBox(_T("產品系統燈是否為藍色？\nIs the system LED blue in product?"),MB_YESNO|MB_ICONQUESTION))
	{

	}
	else
	{
		pRun->SendSfisResultAndShowFail(NB00);
		return 0;
	}*/

	if(pRun->DutSocketConnectionWithDefaultPassword("192.168.31.1"))
	{
		pRun->SendSfisResultAndShowPass();
		return 1;	
	}
	else
	{
		pRun->SendSfisResultAndShowFail(TD00);
		return 0;
	}

	/*if(!pRun->DutSocketConnectionWithUserPassword("192.168.31.1"))
	{
		pRun->SendSfisResultAndShowFail(TU00);
		return 0;
	}*/
	
	//Test end, show pass

}

UINT __cdecl  CRunInfo::DOWN_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	/*if(!pRun->PingLanIP())
	{
		return 0;
	}*/
	pRun->DelArp();
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	if(!pRun->Downgrade())
	{
		pRun->SendSfisResultAndShowFail(DG00);
		return 0;
	}
	
	//Test end, show pass
	return 1;
}


UINT __cdecl  CRunInfo::NFC_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	/*if(!pRun->CheckDutBootUp())
	{
		return 0;
	}
	if(!pRun->DutCheckVer())
	{
		return 0;
	}*/

	//for NFC test
	pRun->DelArp();
	if(!pRun->CheckNFCTagCard())
	{
		pRun->SendSfisResultAndShowFail(NT00);
		return 0;
	}
	//end


	//for debug
	/*if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	if(!pRun->DutCheckHDD())
	{
		pRun->SendSfisResultAndShowFail(CC65);
		return 0;
	}

	if(!pRun->DutCheckVer())
	{
		pRun->SendSfisResultAndShowFail(CC65);
		return 0;
	}

	if(!pRun->DutCheckFAN())
	{
		pRun->SendSfisResultAndShowFail(US00);
		return 0;
	}*/
	//end

	// Test end, show pass
	pRun->SendSfisResultAndShowPass();

	return 1;
}

UINT __cdecl  CRunInfo::RUNIN_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	pRun->DelArp();
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	if(!pRun->DutCheckVer())
	{
		return 0;
	}

	if(!pRun->StartRunIn())
	{
		pRun->SendSfisResultAndShowFail(RI00);
		return 0;
	}

	// Test end, show pass
	pRun->SendSfisResultAndShowPass();
	return 1;
}

UINT __cdecl  CRunInfo::FAN_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;

	pRun->DelArp();
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	if(!pRun->DutCheckFAN())
	{
		return 0;
	}

	// Test end, show pass
	pRun->SendSfisResultAndShowPass();
	return 1;
}


UINT __cdecl  CRunInfo::PT2_FunctionTestRun(LPVOID parameter)
{
	CRunInfo* pRun = (CRunInfo*)parameter;
	int iRet=-1;
		

	// Create socket connection with DUT, telent port 23
	if(!pRun->CheckDutBootUp())
	{
		return 0;
	}

	////Check USB
	//if(!pRun->DECT_USB())
	//{
	//	pRun->SendSfisResultAndShowFail(UB00);
	//	return 0;
	//}

	
	//Check DUT Lan port
	iRet = pRun->CHECK_LAN_PORT();	
	pRun->DisplayRunTimeInfo("pRun->CHECK_LAN_PORT() return value iRet=" , iRet);
	switch (iRet)
	{
	case -1:
		break;
	case 2:
		pRun->SendSfisResultAndShowFail(IN12);
		return 0;
	case 3:
		pRun->SendSfisResultAndShowFail(IN13);
		return 0;
	case 4:
		pRun->SendSfisResultAndShowFail(IN14);
		return 0;
	case 12: 
		pRun->SendSfisResultAndShowFail(TP12);
		return 0;
	case 13: 
		pRun->SendSfisResultAndShowFail(TP13);
		return 0;
	case 14: 
		pRun->SendSfisResultAndShowFail(TP14);
		return 0;	
	default:
		break;
	}

	//Check DUT Wan_port
	iRet = pRun->CHECK_WAN_PORT();	
	pRun->DisplayRunTimeInfo("pRun->CHECK_WAN_PORT() return value iRet=" , iRet);
	switch (iRet)
	{
	case -1:
		break;
	case 1:
		pRun->SendSfisResultAndShowFail(SY55);
		return 0;
	case 2:
		pRun->SendSfisResultAndShowFail(IN50);
		return 0;
	case 3:
		pRun->SendSfisResultAndShowFail(TP50);
		return 0;
	default:
		break;
	}


	//// check dut version information
	//if(!pRun->CheckFirmwareVersion())
	//{	
	//	pRun->SendSfisResultAndShowFail(CC70);
	//	return 0;
	//}
	////check BoardId
	//if(!pRun->DutCheckBoardID())
	//{
	//	pRun->SendSfisResultAndShowFail(CC60);
	//	return 0;
	//}	

	//Check Button
	if(!pRun->DutCheckButton_Auto())
	{
		return 0;
	}
	//Check USB
	if(!pRun->DECT_USB())
	{
		pRun->SendSfisResultAndShowFail(UB00);
		return 0;
	}

	//Check LED
	if(!pRun->DutCheckLed_PT2())	
	{
		return 0;
	}
	//Disable wireless
	if(!pRun->DutDisableWireless())
	{
		pRun->SendSfisResultAndShowFail(DS10);
		return 0;
	}
	// Test end, show pass
	pRun->SendSfisResultAndShowPass();

	return 1;
}

bool CRunInfo::CheckFirmwareVersion(void)
{
	bool bResult = false;
	ResetEvent(m_hEvt);
	SendDutCommand("version", "#", 5000);
	char    Seps[] = "\r\n";
	char    *token;
	char    *Context ;
	token  = strtok_s((g_data.GetBuffer(g_data.GetLength())), Seps, &Context);

	while(1)
	{
		if(token != NULL)
		{
			token = strtok_s(NULL, Seps, &Context);

			CString str = token;
			str.Trim();

			if(strcmp(str.GetBuffer(str.GetLength()), m_IniFileInfo.szStoreFirmware) == 0)
			{
				bResult = true;
				break;

			}
		}
		else
		{
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,", TestDetaillog , m_IniFileInfo.szStoreFirmware);
			bResult = false;
			break;
		}
	}
	/*collect test data*/
	sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,", TestDetaillog , m_IniFileInfo.szStoreFirmware);
	/*End*/
	return bResult ;

}

bool CRunInfo::GetSSIS(char*  szEthernetInfo,CString& str)
{
	char    SepsPin[] = "";
	char    *token;
	char    *ContextPin ;
	token  = strtok_s(szEthernetInfo, SepsPin, &ContextPin);
	if(token != NULL)
	{
		token = strtok_s(NULL, SepsPin, &ContextPin);

		str = token;
		str.Trim();
		return true;
	}
	else
	{
		return false;
	}
}

bool CRunInfo::GetSecType(char*  szEthernetInfo,CString& str)
{
	char    SepsPin[] = "";
	char    *token;
	char    *ContextPin ;
	token  = strtok_s(szEthernetInfo, SepsPin, &ContextPin);
	if(token != NULL)
	{
		token = strtok_s(NULL, SepsPin, &ContextPin);

		str = token;
		str.Trim();
		return true;
	}
	else
	{
		return false;
	}
}


void CRunInfo::OnBnClickedBtnQuit()
{
	// TODO: Add your control notification handler code her

	shutdown(m_Socket , 0);
	if(m_Socket!=INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	if(m_GoldenSocket!=INVALID_SOCKET)
	shutdown(m_GoldenSocket , 0);
	{
		closesocket(m_GoldenSocket);
		m_GoldenSocket = INVALID_SOCKET;
	}
	
	WSACleanup();
	::PostMessage(GetParent()->GetParent()->m_hWnd,WM_CLOSE,0,0);
}


void CRunInfo::OnCbnSelchangeCom()
{
	// TODO: Add your control notification handler code here
	if(!m_blIsOpenSfis && m_blIsSfis)
	{
		GetDlgItemText(IDC_COM,m_strCom);
		//if(!m_ComSfis.Open(m_strCom.GetBuffer(m_strCom.GetLength()),115200))
		if(!m_ComSfis.Open(m_strCom.GetBuffer(m_strCom.GetLength()),9600))
		{
			AfxMessageBox("選擇的COM口不存在或者被其他應用程序占用\n請從新選擇COM與SFIS連接\nThe select COM port don't exit or is used by other application!\nPls choose COM port again");
			return;
		}
		
		m_ComSfis.SendData(("Hello\r\n"), (int)strlen("Hello\r\n"));

		char pBuf[MINBUFSIZE] = "";
		m_ComSfis.ReadData(pBuf, MINBUFSIZE, 1500);

		//if(strstr(pBuf, "PASS") == NULL)
		if(strstr(pBuf, "ERRO") == NULL)
		{
			AfxMessageBox(_T("請從新選擇COM與SFIS連接\n"));
			m_ComSfis.Close();
			return ;
		}
		else
		{
			m_strSfisStatus = "SFIS ON";
			UpdateData(false);
			m_blIsOpenSfis = true;
			AfxBeginThread(ReadComForSfis, &m_ComSfis);
		}
	}
}

bool CRunInfo::LinkDataBase(void)
{
	if(pInstMydas != NULL)
	{
		if(PTSC_GetLinkStatus(pInstMydas))
		{
			return 1;
		}
	}
	char dutName[20];
	char svip[15];
	char version[15];
	char productName[15];
	memset(dutName,0,sizeof(dutName));
	memset(svip,0,sizeof(svip));	
	memset(version,0,sizeof(version));	
	memset(productName,0,sizeof(productName));

	char szPath[MAX_PATH] = "";
	char szFullPath[MAX_PATH] = "";
	char ProductName[MINBUFSIZE] = "";
	GetCurrentDirectory(MAX_PATH, szPath);

	sprintf_s(szFullPath, MAX_PATH, "%s\\%s", szPath , m_IniFileInfo.szTestInfoFileName);

	GetPrivateProfileString("General","MYDAS_IP","10.117.32.106",svip,15,szFullPath);
	GetPrivateProfileString("General","DUT_NAME","MICE",dutName,15,szFullPath);
	GetPrivateProfileString("General","MYDAS_VERSION","1.0",version,15,szFullPath);


	strcpy_s(m_Mydas.Ip, sizeof(m_Mydas.Ip),svip);
	//strcpy_s(m_Mydas.ProductPN, sizeof(m_Mydas.ProductPN),m_strProductName.Trim());
	strcpy_s(m_Mydas.ProductPN, sizeof(m_Mydas.ProductPN),m_strPNNAME);
	strcpy_s(m_Mydas.Product,sizeof(productName),dutName);
	strcpy_s(m_Mydas.ComName, sizeof(m_Mydas.ComName),m_strPcName);	
	strcpy_s(m_Mydas.TestStaName, sizeof(m_Mydas.TestStaName),m_IniFileInfo.szTestStatuse);
	strcpy_s(m_Mydas.DaigVersion, sizeof(m_Mydas.DaigVersion),"V0.0.1");
	strcpy_s(m_Mydas.TitleVersion,sizeof(m_Mydas.TitleVersion),version);
	if (!PTSC_Connect(&hDev,&m_Mydas))
	{
		return 0;
	}

	return 1;
}
bool CRunInfo::SendDatatoMYDAS(char* DetailLog,char* ErrDef,char* SendDataBuf)
{
	HANDLE hEventRev=CreateEvent( 
		NULL,       // default security attributes
		TRUE,       // auto-reset event
		FALSE,      // initial state is unsignaled
		NULL		// object name
		); 

	PTSC_SetSendData(hDev,DetailLog,(unsigned int)strlen(DetailLog),0);
	PTSC_SetSendData(hDev,ErrDef,(unsigned int)strlen(ErrDef),1);
	PTSC_SetSendData(hDev,SendDataBuf,(unsigned int)strlen(SendDataBuf),2);
	PTSC_Send(hDev);
	CloseHandle(hEventRev);

	return TRUE;
}

void CRunInfo::GetLockCode(char* pIMEI, char* pLockCode)
{
	char szTemp[128]="";
	
	HMODULE hDLL=LoadLibrary("WPA_KEY.dll");
	if(hDLL == NULL)
	{
		DisplayRunTimeInfo("Load Library WPA_KEY.dll failed");
		return;
	}

	pFunction fnGetLockCode = (pFunction)GetProcAddress(hDLL,"_telkomGenDefaultsKey");
	fnGetLockCode(pIMEI,pLockCode);
	sprintf_s(szTemp,sizeof(szTemp),"IMEI: %s -> Lock Code: %s",pIMEI,pLockCode);
	DisplayRunTimeInfo(szTemp);

	FreeLibrary(hDLL);
	::Sleep(100);
	if(hDLL!=NULL)
	{
		DisplayRunTimeInfo("Free Library WPA_KEY.dll fail");
	}	
}
void CRunInfo::RunArpDelelte(char* pszIpAddr)
{
	CString strTemp;
	TCHAR szArpCommand[MINBUFSIZE] = "";
	sprintf_s(szArpCommand,"arp -d %s",pszIpAddr);
	RunSpecifyExeAndRead(strTemp,szArpCommand);   
}

bool CRunInfo::PingDUT(char* pszIpAddr, int iCount)
{
	DisplayRunTimeInfo("--------------Ping DUT----------------\n");
	
	if(PingSpecifyIP(pszIpAddr,iCount))
	{
		return true;
	}

	return false;
}

bool CRunInfo::RunTelnetEnable(char* pszIpAddr)
{
	if( strcmp(m_IniFileInfo.szRework,"1") == 0)
	{
		CString strArpInfo;
		char szMac[MINBUFSIZE] = "";
		char szTemp[MINBUFSIZE] ="";
		sprintf_s(szTemp,sizeof(szTemp),"arp -a %s",pszIpAddr);
		RunSpecifyExeAndRead(strArpInfo,szTemp,true);
		GetMac(strArpInfo.GetBuffer(strArpInfo.GetLength())+1,szMac);
		_strupr_s(szMac,MINBUFSIZE);

		if(!RunTelnetExe(szMac))
		{			
			return false;
		}
		Sleep(1000);
	}

	return true;
}

bool CRunInfo::DutSocketConnection(char* DefaultIP)
{
	/*
	true	:pass
	false	:failed
	*/

	/*
	ResetEvent(m_hEvt);
	if(!InitSocket(DefaultIP))
	{
		return false;
	}

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,9000))
	{
		return false;
	}
	Sleep(500);
	if(strstr(m_strStoreEthernetData,"BusyBox") == NULL)
	{
		return false;
	}

	return true;
	*/

	//for mice username and password
	ResetEvent(m_hEvt);
	if(!InitSocket(DefaultIP))
	{
		return false;
	}

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,9000))
	{
		return false;
	}
	Sleep(500);
	if(strstr(m_strStoreEthernetData,"login:"))
	{
		if(SendDutCommand("root","Password:",1000))
		{
			if(SendDutCommand("admin","BusyBox",1000))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool CRunInfo::DutSocketConnectionWithDefaultPassword(char* DefaultIP)
{
	/*
	true	:pass
	false	:failed
	*/

	/*
	ResetEvent(m_hEvt);
	if(!InitSocket(DefaultIP))
	{
		return false;
	}

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,9000))
	{
		return false;
	}
	Sleep(500);
	if(strstr(m_strStoreEthernetData,"BusyBox") == NULL)
	{
		return false;
	}

	return true;
	*/

	//for mice username and password
	//ResetEvent(m_hEvt);
	if(!NOTInitSocket(DefaultIP))
	{
		return false;
	}

	//if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,9000))
	//{
	//	return false;
	//}

	/*Sleep(500);
	if(strstr(m_strStoreEthernetData,"login:"))
	{
		if(SendDutCommand("root","Password:",1000))
		{
			if(SendDutCommand("admin","BusyBox",1000))
			{
				return false;
			}
			else
			{
				DisplayRunTimeInfo("can not telnet with default password!!!");
				return true;
			}
		}
		else
		{
			DisplayRunTimeInfo("account unavailable!!!");
			return false;
		}
	}
	else
	{
		DisplayRunTimeInfo("can not find login entry!!!");
		return false;
	}*/

	return true;
}

bool CRunInfo::DutSocketConnectionWithUserPassword(char* DefaultIP)
{
	/*
	true	:pass
	false	:failed
	*/

	/*
	ResetEvent(m_hEvt);
	if(!InitSocket(DefaultIP))
	{
		return false;
	}

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,9000))
	{
		return false;
	}
	Sleep(500);
	if(strstr(m_strStoreEthernetData,"BusyBox") == NULL)
	{
		return false;
	}

	return true;
	*/

	//for mice username and password
	ResetEvent(m_hEvt);
	if(!InitSocket(DefaultIP))
	{
		return true;
	}

	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,9000))
	{
		return false;
	}
	Sleep(500);
	if(strstr(m_strStoreEthernetData,"login:"))
	{
		if(SendDutCommand("root","Password:",1000))
		{
			if(SendDutCommand("11111111","BusyBox",1000))
			{
				return false;
			}
			else
			{
				DisplayRunTimeInfo("can not telnet with default password!!!");
				return true;
			}
		}
		else
		{
			DisplayRunTimeInfo("account unavailable!!!");
			return false;
		}
	}
	else
	{
		DisplayRunTimeInfo("can not find login entry!!!");
		return false;
	}

	return true;
}

bool CRunInfo::DutCheckFAN(void)
{
	/*
	true	:pass
	false	:failed
	*/

	if(!SendDutCommand("readfanspeed","#",5000)) 
	{
		DisplayRunTimeInfo("read fan command fail!!!");
		return false;
	}

	if(strstr(g_data.GetBuffer(),"Please check your FAN, there are may be some problem with it!!!") || strstr(g_data.GetBuffer(),"FAN"))
	{
		DisplayRunTimeInfo("Fan stop,please check product!!!");
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Fan is rolling!!!");
	}
	//Please check your FAN, there are may be some problem with it!!!

	for(int i=0;i<=2;i++)
	{
		//if(!SendDutCommand("fancycle 1","#",5000))
		if(!SendDutCommand("fancycle 10","#",5000)) 
		{
			DisplayRunTimeInfo("set fan command fail!!!");
			return false;
		}
		if(!SendDutCommand("readfanspeed","#",5000)) 
		{
			DisplayRunTimeInfo("read fan command fail!!!");
			return false;
		}
		char *pTok,*pTok1;
		pTok=strstr(g_data.GetBuffer(),"Real-time Fan Speed=");
		if(pTok==NULL)
		{
			DisplayRunTimeInfo("can not get real time fan speed!!!");
			return false;
		}
		pTok1=strstr(pTok,"\n");
		if(pTok1==NULL)
		{
			DisplayRunTimeInfo("can not get real time fan speed!!!");
			return false;
		}
		char speedbuf[50];
		memset(speedbuf,0,50);
		memcpy_s(speedbuf,50,pTok+strlen("Real-time Fan Speed="),pTok1-pTok+1);
		//DisplayRunTimeInfo(speedbuf);
		if(atoi(speedbuf)>1800)
		{
			DisplayRunTimeInfo("Fan rolls normally!!!");
			break;
		}
		else
		{
			if(i<2)
				continue;
			else
			{
				DisplayRunTimeInfo("Fan rolls abnormally!!!");
				return false;
			}
		}
	}
	//judge temperature
	//Temperature: 
	char *pTok,*pTok1;
	if(!SendDutCommand("readtmp","#",5000)) 
	{
		DisplayRunTimeInfo("read fan command fail!!!");
		return false;
	}
	pTok=strstr(g_data.GetBuffer(),"Temperature: ");
	if(pTok==NULL)
	{
		DisplayRunTimeInfo("can not get real time temperature!!!");
		return false;
	}
	pTok1=strstr(pTok,"\n");
	if(pTok1==NULL)
	{
		DisplayRunTimeInfo("can not get real time temperature!!!");
		return false;
	}
	char tmpbuf[50];
	memset(tmpbuf,0,50);
	memcpy_s(tmpbuf,50,pTok+strlen("Temperature: "),pTok1-pTok+1);
	DisplayRunTimeInfo(tmpbuf);
	if(atoi(tmpbuf)<60)
	{
		DisplayRunTimeInfo("Temperature is under control!!!");
	}
	else
	{
		DisplayRunTimeInfo("Temperature is not under control!!!");
		return false;
	}
	return true;
}

bool CRunInfo::DutCheckVer(void)
{
	/*
	true	:pass
	false	:failed
	*/


///////////////////////////////////////////////////////////////
	DisplayRunTimeInfo("---Start check version---");
	//if(!SendDutCommand("cat /etc/xiaoqiang_version","#",5000))
	if(!SendDutCommand("bootinfo","#",5000)) //changed from 0.1.99 version
		return false;

	if(!strstr(g_data.GetBuffer(),m_IniFileInfo.nMiceAppVer))
	{
		DisplayRunTimeInfo(m_IniFileInfo.nMiceAppVer);
		DisplayRunTimeInfo("Check App version Fail!");
		SendSfisResultAndShowFail(VE00);
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Check App version OK!");
	}

	if(!strstr(g_data.GetBuffer(),m_IniFileInfo.nMiceCfeVer))
	{
		DisplayRunTimeInfo(m_IniFileInfo.nMiceCfeVer);
		DisplayRunTimeInfo("Check CFE version Fail!");
		SendSfisResultAndShowFail(VE01);
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Check CFE version OK!");
	}

	if(!strstr(g_data.GetBuffer(),m_IniFileInfo.nMiceKerVer))
	{
		DisplayRunTimeInfo(m_IniFileInfo.nMiceKerVer);
		DisplayRunTimeInfo("Check Kernel version Fail!");
		SendSfisResultAndShowFail(VE02);
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Check Kernel version OK!");
	}

	/*if(strcmp(m_IniFileInfo.szCheckVersion, "1") == 0)
	{
		ResetEvent(m_hEvt);
		SendEthernetCmd("cat /etc/xiaoqiang_version");
		if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,15000))
		{
			TestFail(SY55);
			return false;
		}
		CString szFW;
		if(strstr(m_strStoreEthernetData,"0.1.49"))
		{
			m_strRecordTestData  += m_strStoreEthernetData;
			m_strRecordTestData  +=  "\t";
			m_TestRecordinfo.szRecordFirmware = m_strStoreEthernetData;
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,",m_strStoreEthernetData);
			DisplayRunTimeInfo("Check version OK!!!\n");
			return true;
		}
		else
		{
			m_strRecordTestData  += m_strStoreEthernetData;
			m_strRecordTestData  +=  "\t";
			m_TestRecordinfo.szRecordFirmware = m_strStoreEthernetData;
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,",m_strStoreEthernetData);
			DisplayRunTimeInfo("Check version Fail!!!\n");
			return false;
		}*/
			


		/*Change '/' to '_'*/
		//szFW = CustomerFirmwareVersion(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()));
		/*End*/
		/*if(szFW.Trim().GetLength() > 30)
		{
			szFWforSfis = szFW.Trim().Left(30);
		}
		else
		{
			szFWforSfis = szFW.Trim();
		}*/
		/*End*/

	return true;
}

bool CRunInfo::DutCheckVerDL(void)
{
	/*
	true	:pass
	false	:failed
	*/

	DisplayRunTimeInfo("---Start check version---");

	/*if(!SendDutCommand("readtmp","#",5000)) 
		return false;

	if(!SendDutCommand("cat /tmp/fan_speed","#",5000)) 
		return false;*/

	if(!SendDutCommand("bootinfo","#",5000)) 
		return false;

	if(!strstr(g_data.GetBuffer(),m_IniFileInfo.nMiceAppVer))
	{
		DisplayRunTimeInfo(m_IniFileInfo.nMiceAppVer);
		DisplayRunTimeInfo("Check App version Fail!");
		SendSfisResultAndShowFail(VE00);
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Check App version OK!");
	}

	if(!strstr(g_data.GetBuffer(),m_IniFileInfo.nMiceCfeVer))
	{
		DisplayRunTimeInfo(m_IniFileInfo.nMiceCfeVer);
		DisplayRunTimeInfo("Check CFE version Fail!");
		SendSfisResultAndShowFail(VE01);
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Check CFE version OK!");
	}

	if(!strstr(g_data.GetBuffer(),m_IniFileInfo.nMiceKerVer))
	{
		DisplayRunTimeInfo(m_IniFileInfo.nMiceKerVer);
		DisplayRunTimeInfo("Check Kernel version Fail!");
		SendSfisResultAndShowFail(VE02);
		return false;
	}
	else
	{
		DisplayRunTimeInfo("Check Kernel version OK!");
	}

	/*if(strcmp(m_IniFileInfo.szCheckVersion, "1") == 0)
	{
		ResetEvent(m_hEvt);
		SendEthernetCmd("cat /etc/xiaoqiang_version");
		if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,15000))
		{
			TestFail(SY55);
			return false;
		}
		CString szFW;
		if(strstr(m_strStoreEthernetData,"0.1.49"))
		{
			m_strRecordTestData  += m_strStoreEthernetData;
			m_strRecordTestData  +=  "\t";
			m_TestRecordinfo.szRecordFirmware = m_strStoreEthernetData;
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,",m_strStoreEthernetData);
			DisplayRunTimeInfo("Check version OK!!!\n");
			return true;
		}
		else
		{
			m_strRecordTestData  += m_strStoreEthernetData;
			m_strRecordTestData  +=  "\t";
			m_TestRecordinfo.szRecordFirmware = m_strStoreEthernetData;
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,",m_strStoreEthernetData);
			DisplayRunTimeInfo("Check version Fail!!!\n");
			return false;
		}*/
			


		/*Change '/' to '_'*/
		//szFW = CustomerFirmwareVersion(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()));
		/*End*/
		/*if(szFW.Trim().GetLength() > 30)
		{
			szFWforSfis = szFW.Trim().Left(30);
		}
		else
		{
			szFWforSfis = szFW.Trim();
		}*/
		/*End*/

	return true;
}

bool CRunInfo::StartRunIn(void)
{
	/*
	true	:pass
	false	:failed
	*/
	if(!SendDutCommand("badblocks_test 10 /dev/sda4 &","#",5000)) 
		return false;

	Sleep(3000);
	if(IDYES==AfxMessageBox(_T("產品上面燈是否為橙燈閃爍?\nIs the LED orange and blink?"),MB_YESNO|MB_ICONQUESTION))
	{
		DisplayRunTimeInfo("---Start Run In---");
	}
	else
	{
		return false;
	}

	return true;
}

bool CRunInfo::DutCheckButton(void)
{
		ResetEvent(m_hEvt);
		/*Button test Command*/
		if(IDNO == MessageBox("請按 WPS 按鈕，持續1秒鐘！然後檢查 WPS 燈變綠色并不斷閃爍！", "LED Check" , MB_YESNO))
		{
			SendSfisResultAndShowFail(LD10);
			return false;
		}
		if(!SendDutCommand("nvram set wla_wlanstate=Enable","#",1000))
			return false;
		SendDutCommand("nvram set TE_TEST=1","#",2000);
		SendDutCommand("killall swresetd","#",2000);
		SendDutCommand("swresetd","#",2000);
		/*End*/
		if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,15000))
		{
			TestFail(SY55);
			return false;
		}
		/*button 1*/

		bool blPassY = false;
		for(int i = 0; i <3; i++)
		{
			if(IDOK == AfxMessageBox("請按一下WPS按鈕,持續一秒鐘!!!"))
			{
				if(strstr(m_strStoreEthernetData,"WPS button is pressed"))
				{
					blPassY = true;
					break;
				}
			}
		}
		if(!blPassY)
		{
			SendSfisResultAndShowFail(BC30);  
			return false;
		}
		/*button 2*/
		blPassY = false;
		for(int i = 0; i <3; i++)
		{
			if(IDOK == AfxMessageBox("請按一下Reset按鈕,持續一秒鐘!!!"))
			{
				if(strstr(m_strStoreEthernetData,"reset button is pressed"))
				{
					blPassY = true;
					break;
				}
			}
		}
		if(!blPassY)
		{
			SendSfisResultAndShowFail(BC20);  
			return false;
		}
		
	return true;
}
bool CRunInfo::DutCheckButton_Auto(void)
{
	/*
	true	:pass
	false	:failed
			BC20
			BC30
			BC40
	*/
	ResetEvent(m_hEvt);

	/*Button test Command*/
	//SendDutCommand("reset_no_reboot","#",5000);
	SendDutCommand("killall swresetd","#",5000);
	SendDutCommand("swresetd &","#",5000);
	/*End*/
	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hEvt,15000))
	{
		TestFail(SY55);
		return false;
	}
	char*  szText;
	char*  szWindowName;
	char*  szReturnValue;
	int	   iRty;

	//int iButtonRelaseState = CheckButtonRelease();
	//if( iButtonRelaseState != 0 )
	//{
	//	if(iButtonRelaseState == 1)//reset button fail
	//	{
	//		SendSfisResultAndShowFail(BC21);
	//		return false;
	//	}
	//	else if(iButtonRelaseState == 2)//WPS button fail
	//	{
	//		SendSfisResultAndShowFail(BC31);
	//		return false;
	//	}
	//	else if(iButtonRelaseState == 3)//WIFI button fail
	//	{
	//		SendSfisResultAndShowFail(BC41);
	//		return false;
	//	}
	//}

	/*button 1*/
	szText = "請按一下治具上面的綠色按鈕，然後松開!!!";	/*change Text you nend*/
	szWindowName = "Reset Button Test";			/*change WindowName code you nend*/	
	//szWindowName = "reset button is pressed";
	//szReturnValue = MICE_REESET_BUTTON;
	szReturnValue = "gpio tester: signal SIGUSR1 received";
	//szReturnValue = "WPS button is pressed";	/*change ReturnValue you nend*/

	ButtonInfo.bTestOK = false;
	ButtonInfo.szReturnInfo = szReturnValue;
	ButtonInfo.szWindowName = szWindowName;

	iRty = 3;
	while(iRty)
	{
		iRty--;
		AfxBeginThread( AutoPressButton , &ButtonInfo);
		MessageBox(szText , szWindowName ,NULL);
		if(!ButtonInfo.bTestOK)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	if(!ButtonInfo.bTestOK)
	{
		SendSfisResultAndShowFail(BC20);
		return false;	
	}

	/*button 2*/
	//szText = "請按一下WiFi按鈕,持續一秒鐘!!!";	/*change Text you nend*/
	//szWindowName = "WiFi Button Check";		/*change WindowName code you nend*/
	//szReturnValue = WIFI_BTN_VALUE;
	////szReturnValue = "Upgrading firmware, don't config wireless";

	//ButtonInfo.bTestOK = false;
	//ButtonInfo.szReturnInfo = szReturnValue;
	//ButtonInfo.szWindowName = szWindowName;

	//iRty = 3;
	//while(iRty)
	//{
	//	iRty--;
	//	AfxBeginThread( AutoPressButton , &ButtonInfo);
	//	MessageBox(szText , szWindowName ,NULL);
	//	if(!ButtonInfo.bTestOK)
	//	{
	//		continue;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	//if(!ButtonInfo.bTestOK)
	//{
	//	SendSfisResultAndShowFail(BC40);
	//	return false;	
	//}

	///*button 3*/
	//szText = "請按一下Reset按鈕,持續一秒鐘!!!";	/*change Text you nend*/
	//szWindowName = "Reset Button Check";		/*change WindowName code you nend*/
	//szReturnValue = RST_BTN_VALUE;
	////szReturnValue = "Upgrading firmware, don't reboot";

	//ButtonInfo.bTestOK = false;
	//ButtonInfo.szReturnInfo = szReturnValue;
	//ButtonInfo.szWindowName = szWindowName;

	//iRty = 3;
	//while(iRty)
	//{
	//	iRty--;
	//	AfxBeginThread( AutoPressButton , &ButtonInfo);
	//	MessageBox(szText , szWindowName ,NULL);
	//	if(!ButtonInfo.bTestOK)
	//	{
	//		continue;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	//if(!ButtonInfo.bTestOK)
	//{
	//	SendSfisResultAndShowFail(BC20);
	//	return false;	
	//}
	//
	//Sleep(500);

	//iButtonRelaseState = CheckButtonRelease();
	//if( iButtonRelaseState != 0 )
	//{
	//	if(iButtonRelaseState == 1)//reset button fail
	//	{
	//		SendSfisResultAndShowFail(BC22);
	//		return false;
	//	}
	//	else if(iButtonRelaseState == 2)//WPS button fail
	//	{
	//		SendSfisResultAndShowFail(BC32);
	//		return false;
	//	}
	//	else if(iButtonRelaseState == 3)//WIFI button fail
	//	{
	//		SendSfisResultAndShowFail(BC42);
	//		return false;
	//	}
	//}

	return true;
}
bool CRunInfo::DutBurnSn(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		//DisplayRunTimeInfo("cancel burn action!!!");
		return true;
	}

	DisplayRunTimeInfo("---Start burn SN---");
	char    szBurnCNCmd[MINBUFSIZE]  = "";
	sprintf_s(szBurnCNCmd,MINBUFSIZE,"bdata set SN=%s",m_strSN);
	
	if(SendDutCommand(szBurnCNCmd,"#",8000))
	{
		if(SendDutCommand("bdata commit","#",8000))
		{
			if(SendDutCommand("bdata sync","#",8000))
			{
				return true;
			}
		}	
	}

	return false;
}

bool CRunInfo::DutBurnMiceSSID(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		return true;
	}

	/*
	CString		m_strSN;
	CString		m_strET0MAC;
	CString		m_str2GMAC;
	CString		m_str5GMAC;
	CString		m_str2GSSID;
	CString		m_str5GSSID;
	*/

	DisplayRunTimeInfo("---Start burn 2G SSID---");
	char  szBurnSSIDCmd[MINBUFSIZE]  = "";
	sprintf_s(szBurnSSIDCmd,MINBUFSIZE,"bdata set wl1_ssid=%s",m_str2GSSID.GetBuffer(m_str2GSSID.GetLength()));
	DisplayRunTimeInfo(szBurnSSIDCmd);
	SendDutCommand("\n","#",500);
	if(!SendDutCommand(szBurnSSIDCmd,"#",8000))
	{
		return false;
	}

	DisplayRunTimeInfo("---Start burn 5G SSID---");
	memset(szBurnSSIDCmd,0,512);
	sprintf_s(szBurnSSIDCmd,MINBUFSIZE,"bdata set wl0_ssid=%s",m_str5GSSID.GetBuffer(m_str5GSSID.GetLength()));
	DisplayRunTimeInfo(szBurnSSIDCmd);
	SendDutCommand("\n","#",500);
	if(!SendDutCommand(szBurnSSIDCmd,"#",8000))
	{
		return false;
	}

	/*
	rm -f /etc/config/wireless
	wifi detect > /etc/config/wireless
	wifi up 2>/dev/null
	*/
	if(SendDutCommand("bdata commit","#",8000))
	{
		if(SendDutCommand("bdata sync","#",8000))
		{
			if(SendDutCommand("rm -f /etc/config/wireless","#",8000))
			{
				if(SendDutCommand("wifi detect > /etc/config/wireless","#",8000))
				{
					//if(SendDutCommand("wifi up 2 > /dev/null","#",8000))
					//{
						SendDutCommand("wifi up 2 > /dev/null\n","#",500);
						Sleep(1000);
						return true;
					//}
				}
			}
		}
	}



	return false;
}

bool CRunInfo::DutBurnMac(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		//return true;
	}

	DisplayRunTimeInfo("---Start check SSN---");

	if(!SendDutCommand("bdata get SN","#",5000)) 
		return false;	

	char a[50];
	memset(a,0,50);
	strcpy_s(a,50,"123123");
	DisplayRunTimeInfo(g_data.GetBuffer());
	DisplayRunTimeInfo(a);

	if(strstr(g_data,a))
	{
		DisplayRunTimeInfo("Check SN OK!");
		return true;
	}
	return false;

	/*DisplayRunTimeInfo("---Start burn ethermac---");
	char  szBurnMacCmd[MINBUFSIZE]  = "";

	sprintf_s(szBurnMacCmd,MINBUFSIZE,"burnethermac %s",m_strMAC);
	SendDutCommand("\n","#",500);
	if(SendDutCommand(szBurnMacCmd,"BurnEtherMAC OK",8000))
	{
		return true;
	}

	return false;*/
}

bool CRunInfo::DutBurnMice2GMac(void)
{
	// No sfis on, no write action, return directly
	//if(!GetIsSfis()/* || (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	//{
	//	return true;
	//}
		if(Write_mac_flag!=1)
		{
			DisplayRunTimeInfo("Do not need write 2.4g mac!");
			return true;
		}
	/*
	CString		m_strSN;
	CString		m_strET0MAC;
	CString		m_str2GMAC;
	CString		m_str5GMAC;
	CString		m_str2GSSID;
	CString		m_str5GSSID;
	*/

	DisplayRunTimeInfo("---Start burn 2G mac---");
	char  szBurnMacCmd[MINBUFSIZE]  = "";
	//char  macConvert[50] = "";
	//lstrcpy(macConvert,m_strET0MAC);
	//memcpy_s(macConvert,50,m_strET0MAC,2);
	//strcat_s(macConvert,50,":");

	//CString mac1="6863590E8BEF";
	char macConvert[50]="";
	char buf[50]="";
	char buf1[50]="";
	lstrcpy(macConvert,m_str2GMAC);
	//printf("before convert,macConvert is %s;\n",macConvert);

	for(int i=0;i<6;i++)
	{
		memcpy_s(buf,50,macConvert+i*2,2);
		strcat_s(buf1,50,buf);
		if(i<5)
		{
			strcat_s(buf1,50,":");
		}
	}
	//printf("after convert,buf is %s;\n",buf1);


	sprintf_s(szBurnMacCmd,MINBUFSIZE,"setmac wl1 %s",buf1);
	DisplayRunTimeInfo(szBurnMacCmd);
	//MessageBox(szBurnMacCmd);
	SendDutCommand("\n","#",500);
	if(SendDutCommand(szBurnMacCmd,"#",8000))
	{
		return true;
	}

	return false;
}

bool CRunInfo::DutBurnMice5GMac(void)
{
	// No sfis on, no write action, return directly
	//if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	//{
	//	return true;
	//}

		if(Write_mac_flag!=1)
		{
			DisplayRunTimeInfo("Do not need write 5g mac!");
			return true;
		}

	/*
	CString		m_strSN;
	CString		m_strET0MAC;
	CString		m_str2GMAC;
	CString		m_str5GMAC;
	CString		m_str2GSSID;
	CString		m_str5GSSID;
	*/

	DisplayRunTimeInfo("---Start burn 5G mac---");
	char  szBurnMacCmd[MINBUFSIZE]  = "";
	//char  macConvert[50] = "";
	//lstrcpy(macConvert,m_strET0MAC);
	//memcpy_s(macConvert,50,m_strET0MAC,2);
	//strcat_s(macConvert,50,":");

	//CString mac1="6863590E8BEF";
	char macConvert[50]="";
	char buf[50]="";
	char buf1[50]="";
	lstrcpy(macConvert,m_str5GMAC);
	//printf("before convert,macConvert is %s;\n",macConvert);

	for(int i=0;i<6;i++)
	{
		memcpy_s(buf,50,macConvert+i*2,2);
		strcat_s(buf1,50,buf);
		if(i<5)
		{
			strcat_s(buf1,50,":");
		}
	}
	//printf("after convert,buf is %s;\n",buf1);


	sprintf_s(szBurnMacCmd,MINBUFSIZE,"setmac wl0 %s",buf1);
	DisplayRunTimeInfo(szBurnMacCmd);
	//MessageBox(szBurnMacCmd);
	SendDutCommand("\n","#",500);
	if(SendDutCommand(szBurnMacCmd,"#",8000))
	{
		//if(SendDutCommand("bdata commit","#",8000))
		//{
		//	if(SendDutCommand("bdata sync","#",8000))
		//	{
				return true;
		//	}
		//}
	}

	return false;
}

bool CRunInfo::DutBurnModel(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		//DisplayRunTimeInfo("cancel burn action!!!");
		return true;
	}

	DisplayRunTimeInfo("---Start burn Model---");
	char    szBurnCNCmd[MINBUFSIZE]  = "";
	//sprintf_s(szBurnCNCmd,MINBUFSIZE,"bdata set model=R1D");
	sprintf_s(szBurnCNCmd,MINBUFSIZE,"bdata set model=R1CM");
	
	if(SendDutCommand(szBurnCNCmd,"#",8000))
	{
		if(SendDutCommand("bdata commit","#",8000))
		{
			if(SendDutCommand("bdata sync","#",8000))
			{
				return true;
			}
		}	
	}

	return false;
}

bool CRunInfo::DutBurnET0Mac(void)
{
	// No sfis on, no write action, return directly
	//if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	//{
	//	return true;
	//}

	/*
	CString		m_strSN;
	CString		m_strET0MAC;
	CString		m_str2GMAC;
	CString		m_str5GMAC;
	CString		m_str2GSSID;
	CString		m_str5GSSID;
	*/

	//////////////get eth wl0 wl1 mac///////////////////sheng-zhao for R1C 20140313


	char FileLineBuf[512]="";
	char *pToke=NULL;
	char tempbuf_MAC[MINBUFSIZE]  = "";
	int MAC_num=0;
	ifstream fin;
		Write_mac_flag=0;
	
	fin.open("C:\\R1C_MAC.txt",ios_base::in);
	
		while(fin.getline(FileLineBuf, sizeof(FileLineBuf))) 
		{ 
		 // fin.getline(FileLineBuf, sizeof(FileLineBuf));
		  DisplayRunTimeInfo(FileLineBuf);


		  if(strstr(FileLineBuf, "NEED_WRITE"))

			Write_mac_flag=1;

		  if(Write_mac_flag==1)
		  {
			if(pToke=strstr(FileLineBuf, "ETH_MAC:"))
			{
				//gTI.TestStaInfo.IQSN = pToke+strlen("Tester 1 SN:	");
				//strcpy_s(m_strET0MAC,sizeof(m_strET0MAC),pToke+strlen("ETH_MAC:"));
				m_strET0MAC=pToke+strlen("ETH_MAC:");
				sprintf_s(tempbuf_MAC,MINBUFSIZE,"ETH_MAC:%s",m_strET0MAC);
				DisplayRunTimeInfo(tempbuf_MAC);
				MAC_num++;
			}
			
			if(pToke=strstr(FileLineBuf, "WL1_MAC:"))
			{
				//gTI.TestStaInfo.IQSN = pToke+strlen("Tester 1 SN:	");
				//strcpy_s(m_str2GMAC,sizeof(m_str2GMAC),pToke+strlen("WL1_MAC:"));
				m_str2GMAC=pToke+strlen("WL1_MAC:");
				sprintf_s(tempbuf_MAC,MINBUFSIZE,"WL1_MAC:%s",m_str2GMAC);
				DisplayRunTimeInfo(tempbuf_MAC);
				MAC_num++;
			}

			if(pToke=strstr(FileLineBuf, "WL0_MAC:"))
			{
				//gTI.TestStaInfo.IQSN = pToke+strlen("Tester 1 SN:	");
				//strcpy_s(m_str5GMAC,sizeof(m_str5GMAC),pToke+strlen("WL0_MAC:"));
				m_str5GMAC=pToke+strlen("WL0_MAC:");
				sprintf_s(tempbuf_MAC,MINBUFSIZE,"WL0_MAC:%s",m_str5GMAC);
				DisplayRunTimeInfo(tempbuf_MAC);	
				MAC_num++;
			}
		 }

		} 

		fin.clear();
		fin.close();

		
		if(Write_mac_flag!=1)
		{
			DisplayRunTimeInfo("Do not need write eth mac!");
			return true;
		}
		if(MAC_num!=3)
		{
		  DisplayRunTimeInfo("Get mac fail from C:\\R1C_MAC.txt");
          return false;
		}
	
		//////////////////////////////////////////////

	DisplayRunTimeInfo("---Start burn ether mac---");
	SendDutCommand("ifconfig wl1 up","wl",1000);
	Sleep(1000);
	SendDutCommand("ifconfig wl0 up","wl",1000);
	Sleep(1000);
	char  szBurnMacCmd[MINBUFSIZE]  = "";
	//char  macConvert[50] = "";
	//lstrcpy(macConvert,m_strET0MAC);
	//memcpy_s(macConvert,50,m_strET0MAC,2);
	//strcat_s(macConvert,50,":");

	//CString mac1="6863590E8BEF";
	char macConvert[50]="";
	char buf[50]="";
	char buf1[50]="";
	lstrcpy(macConvert,m_strET0MAC);
	//printf("before convert,macConvert is %s;\n",macConvert);

	for(int i=0;i<6;i++)
	{
		memcpy_s(buf,50,macConvert+i*2,2);
		strcat_s(buf1,50,buf);
		if(i<5)
		{
			strcat_s(buf1,50,":");
		}
	}
	//printf("after convert,buf is %s;\n",buf1);


	//sprintf_s(szBurnMacCmd,MINBUFSIZE,"bdata set et0macaddr=%s",buf1);
	sprintf_s(szBurnMacCmd,MINBUFSIZE,"setmac eth %s",buf1);
	DisplayRunTimeInfo(szBurnMacCmd);
	//MessageBox(szBurnMacCmd);
	SendDutCommand("\n","#",500);
	if(SendDutCommand(szBurnMacCmd,"#",8000))
	{
		return true;
	}

	return false;
}

bool CRunInfo::DutBurnPin(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() && (0 != strcmp(m_IniFileInfo.szAllowBurn, "1")))
	{
		return true;
	}

	DisplayRunTimeInfo("---Start burn Pincode---");
	char    szBurnPincodeCmd[MINBUFSIZE]  = "";
	sprintf_s(szBurnPincodeCmd,MINBUFSIZE,"burnpin %s",m_strPincode);

	if(SendDutCommand(szBurnPincodeCmd,"burnpin OK",8000))
	{
		return true;
	}

	return false;
}

bool CRunInfo::DutBurnRegion(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() && (0 != strcmp(m_IniFileInfo.szAllowBurn, "1")) && (0 != strcmp(m_IniFileInfo.szRegionCodeFlag, "1")))
	{
		return true;
	}

	DisplayRunTimeInfo("---Start burn region code---");
	char    szBurnRegionCmd[MINBUFSIZE]  = "";
	sprintf_s(szBurnRegionCmd,MINBUFSIZE,"burnsku %s",m_IniFileInfo.szRegionCode);

	if(SendDutCommand(szBurnRegionCmd,"burnsku OK",8000))
	{
		return true;
	}

	return false;
}


bool  CRunInfo::Write_Code( char* BurnCmd  , char* BurnWhat , char* ReturnValue)
{
	// No sfis on, no write action, return directly
	//In debug mode can burn code 

	if(!GetIsSfis() && (0 != strcmp(m_IniFileInfo.szAllowBurn, "1")))
	{
		return true;//1：don't burn code , return directly
	}
	//if any value is NULL,return directly
	if(BurnCmd == NULL)
	{
		return true;//1：don't burn code , return directly
	}
	if(BurnWhat == NULL)
	{
		return true;//1：don't burn code , return directly
	}
	if(ReturnValue == NULL)
	{
		return true;//1：don't burn code , return directly
	}
	char szBuf[MINBUFSIZE] = "";
	
	sprintf_s(szBuf,MINBUFSIZE,"*****Use %s burn %s*****", BurnCmd , BurnWhat);

	DisplayRunTimeInfo(szBuf);

	char szBurnCmd[MINBUFSIZE]  = "";

	sprintf_s(szBurnCmd,MINBUFSIZE,"%s %s", BurnCmd , BurnWhat);

	if(!SendDutCommand(szBurnCmd , ReturnValue , 10000))
	{
		return false;//2.BurnCode fail
	}

	return true;
}

bool CRunInfo::DutCheckLedAll(void)
{

	if(!SendDutCommand("leddown","#",5000)) return false;
	if(!SendDutCommand("extled set 0 1;extled set 2 1;extled set 5 1;extled set 6 1;extled set 7 1;extled set 4 1;gpio 15 0","#",2000))
	{
		return false;
	}
	
	if(AfxMessageBox("測試PASS請檢查燈是否全亮 ALL lighted?",MB_YESNO) == IDNO)
	{
		
		return false;
	}

	return true;
}

/******don't test WAN LED in PT2 station******/
bool CRunInfo::DutCheckLed_PT2(void)
{
	/*
	true	:pass
	false	:failed
	*/
	if(!SendDutCommand("leddown;ledup","#",5000))
	{
		return false;
	}
	
	if(AfxMessageBox("請檢查產品背面中間位置2個LED亮白色?\n",MB_YESNO) == IDNO)
	{		
		SendSfisResultAndShowFail(LD00);
		return false;
	}

	if(AfxMessageBox("請檢查wireless和USB燈<藍色>\n其餘所有燈(2個)<綠色>?\nwireless and USB lighted Blue,other 2 LED lighted Green?\n",MB_YESNO) == IDNO)//change text you need
	{		
		SendSfisResultAndShowFail(LD00);
		return false;
	}
	return true;
}
bool CRunInfo::DutCheckLed(void)
{
	/*
	true	:pass
	false	:failed
			LD00
			LD20
			LD30
			LD40
	*/
	//if(AfxMessageBox("請檢查前面板\"NETGEAR\"logo是否均勻亮白色?\n",MB_YESNO) == IDNO)//change text you need
	//{		
	//	SendSfisResultAndShowFail(LD00);
	//	return false;
	//}

	//if(!SendDutCommand("leddown;gpio 14 0;gpio 15 0;gpio 17 1;gpio 18 1;gpio 12 1;gpio 13 1;gpio 3 1;et robowr 0x0 0x1a 0x0;et robowr 0x0 0x18 0x0","#",5000))
	if(!SendDutCommand("leddown","#",5000))
	{
		return false;
	}
	if(AfxMessageBox("請檢查除Power LED為暗橘黃色其他LED均滅?\n please check power LED on orange,others are dark?\n",MB_YESNO) == IDNO)//change text you need
	{		
		SendSfisResultAndShowFail(LD30);
		return false;
	}	
	
	//if(!SendDutCommand("ledup;gpio 14 1;gpio 15 1;gpio 17 0;gpio 18 0;gpio 12 0;gpio 13 0;gpio 3 0;et robowr 0x0 0x18 0x1ff;et robowr 0x0 0x1a 0x0","#",5000))
	if(!SendDutCommand("ledup","#",5000))
	{
		
		return false;
	}	
	if(AfxMessageBox("請檢查所有LED均被點亮，其中6顆為橘黃色,6顆為亮白色.\n Please check LED colors:\n six LED<orange>?\n six LED<white>?\n ",MB_YESNO) == IDNO)//change text you need
	{		
		SendSfisResultAndShowFail(LD00);
		return false;
	}


	return true;
}

bool CRunInfo::DutFixWanSetting(void)
{
	ShowMsg("***********Set WAN Port ***************");

	if(!SendDutCommand("nvram set wan0_ipaddr=111.111.111.111;nvram set wan0_gateway=111.111.111.1;nvram set wan0_netmask=255.255.255.0","#",5000))	
		return false;
	if(!SendDutCommand("nvram set wan_ipaddr=111.111.111.111;nvram set wan_gateway=111.111.111.1;nvram set wan_netmask=255.255.255.0","#",5000))
		return false;	
	if(!SendDutCommand("nvram set wan0_dns1=111.111.111.1;nvram set wan_dns1=111.111.111.1;nvram set static0_ipaddr=111.111.111.111","#",5000))
		return false;	
	if(!SendDutCommand("nvram set static0_netmask=255.255.255.0;nvram set static0_gateway=111.111.111.1;nvram set wan0_proto=static","#",5000))
		return false;	
	if(!SendDutCommand("nvram set wan_proto=static;nvram set wla_wlanstate=Disable","#",5000))
		return false;	

	if(!SendDutCommand("nvram set nmrp_test_mode=1","#",8000))
	{
		return false;
	}

	if(!SendDutCommand("nvram commit","#",8000))
		return false;	
	
	

	return true;
}

bool CRunInfo::DutTestLockCode(void)
{
	// If no lockcode need, return true directly
	if(0 != strcmp(m_IniFileInfo.szLockCode,"1"))
		return true;

	// lock
	ShowMsg("***************Lock Code****************");
	CString tempIMEI = szIMEIFromDUT;
	tempIMEI.Trim();
	char LockCode[12]="";
	GetLockCode(tempIMEI.GetBuffer(tempIMEI.GetLength()),LockCode);

	m_strLockCode="";
	m_strLockCode=LockCode;
	m_strLockCode.Trim();

	char szTemp[200] = "";
	sprintf_s(szTemp,200,"wwan -ph1 %s",m_strLockCode.Trim());
	if(!SendDutCommand(szTemp,"Lock status: 1",10000))
	{
		if(!SendDutCommand(szTemp,"Lock status: 1",10000))
		{
			return false;
		}
	}

	// unlock
	ShowMsg("**************unLock code*****************");
	sprintf_s(szTemp,200,"wwan -ph0 %s",m_strLockCode.Trim());
	if(!SendDutCommand(szTemp,"Lock status: 0",10000))
	{
		if(!SendDutCommand(szTemp,"Lock status: 0",10000))
		{
			return false;
		}
	}			

	// lock again
	SendDutCommand("\n","#",20);
	ShowMsg("**************Lock code*****************");		
	sprintf_s(szTemp,200,"wwan -ph1 %s",m_strLockCode.Trim());
	if(!SendDutCommand(szTemp,"Lock status: 1",10000))
	{
		if(!SendDutCommand(szTemp,"Lock status:1",10000))
		{
			return false;
		}
	}

	sprintf_s(szTemp,200,"nvram set wsy=%s",m_strLockCode.Trim());
	if(!SendDutCommand(szTemp,"#",1000))
	{
		return false;
	}

	m_strRecordTestData  +=  m_strLockCode.Trim();
	m_strRecordTestData  +=  "\t";

	return true;
}

bool CRunInfo::DutGetIMEI(void)
{
	ShowMsg("************Get IMEI *******************");
	char szTemp[128]="";
	char SepsIMEI[]     = "";
	char *tokenIMEI;
	char *ContextIMEI ;  
	memset(szIMEIFromDUT,0,20);

	if(!SendDutCommand("nvram get wwan_runtime_imei","#",2500))
	{
		return false;
	}
	tokenIMEI = strtok_s(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()),SepsIMEI,&ContextIMEI);
	tokenIMEI = _tcstok_s(NULL,SepsIMEI,&ContextIMEI);
	strncpy_s(szIMEIFromDUT, MINBUFSIZE, tokenIMEI, 15);
	if(strlen(szIMEIFromDUT) != 15)
		return false;

	m_strRecordTestData  +=  szIMEIFromDUT;
	m_strRecordTestData  +=  "\t";
	m_strimei="";
    m_strimei=szIMEIFromDUT;

	return true;
}

bool CRunInfo::DutClearSSID(void)
{
	ShowMsg("*************Clear SSID**********************");
	if(!SendDutCommand("nvram set wla_ssid=""","#",5000)) return false;
	if(!SendDutCommand("nvram set wla_passphrase=""","#",5000)) return false;
	if(!SendDutCommand("nvram commit","#",4000)) return false;

	return true;
}

bool CRunInfo::DutCheckSimCard(void)
{
	if(!SendDutCommand("sim","inserted",5000))
	{
		if(IDOK == AfxMessageBox("請確認SIM是否有插好PLS check the SIM card insert status !!!"))
		{
			if(!SendDutCommand("sim","inserted",5000))
			{
				return false;
			}
		}
	}

	return true;
}

bool CRunInfo::DutCheckSimRW(void)
{
	if(!DutCheckSimCard())
	{
		return false;
	}

	if(!SendDutCommand("nvram get wwan_SIM_rw","OK",5000))
	{
		return false;
	}
	
	return true;
}

bool CRunInfo::DutCheck3gModelType(void)
{
	if(!SendDutCommand("nvram get wwan_runtime_modem_type",m_IniFileInfo.szModuleInfor,5000))
	{
		return false;
	}

	return true;
}

bool CRunInfo::DutCheck3gModuleVer(void)
{
	if(!SendDutCommand("nvram get wwan_runtime_fw_ver",m_IniFileInfo.szModuleVersion,5000))
	{
		return false;
	}

	return true;
}
bool CRunInfo::DutDetect3gModule(void)
{

	ShowMsg("**********Check 3G Module Info*****************");
	SendDutCommand("killall heartbeat;killall wwan;killall queryusb","#",5000);
	SendDutCommand("killall cald;killall pppd","#",5000);
	SendDutCommand("nvram set wwan_pin_code=0000","#",5000);	// for sim card read. write

	if(SendDutCommand("3gmodule","Modem detected done",20000))
	{
		return true;
	}

	return false;
}

int CRunInfo::DutTestRssi(void)
{
	// if not test, return true directly
	if(0 != strcmp(m_IniFileInfo.szStoreTestRssi,"1"))
	{
		return -1;
	}
	
	char sep[]=":";
	char *pRet, *pNext;

	ShowMsg("*********************Rssi Test(Main Antenna out side)********************************");
	char rssi_token[5120]="";
	float d_RssiValue = 0;

	if(!SendDutCommand("\n","#",1000))	return 0;
	if(!SendDutCommand("cald","#",1000))	return 0;
	if(!SendDutCommand("queryusb","#",1000))	return 0;
	if(!SendDutCommand("killall wwan","#",1000))	return 0;
	if(!SendDutCommand("killall heartbeat","#",1000))	return 0;
	if(!SendDutCommand("\n","#",1000))	return 0;

	// change to main attenna with external antenna
	if(!SendDutCommand("gpio 0 0","#",1000))	return 0;
	if(!SendDutCommand("\n","#",1000))	return 0;
	if(!SendDutCommand("wwan -rx0","Init done",20000))	
	{
		// the following command is to solve initialize module fail issue
		if(!SendDutCommand("","#",1000))	return 0;
		if(SendDutCommand("gpio 5 0","#",1000))
		{
			Sleep(1000);
		}
		else
		{
			return 0;
		}
	
		if(SendDutCommand("gpio 5 1","#",1000))
		{
			Sleep(6000);
		}
		else
		{
			return 0;
		}

		if(!SendDutCommand("wwan -rx0","Init done",20000))
		{
			return 0;
		}
	}
	// start to get rssi, here need optimization
	if(!SendDutCommand("wwan -rx1","Main:",20000))	
	{
		if(!SendDutCommand("wwan -rx1","Main:",20000))
		{
			return 0;
		}
	}

	strcpy_s(rssi_token,sizeof(rssi_token),m_strStoreEthernetData);
	char *pFindRssi = strstr(rssi_token,"Main:");
	pRet = strtok_s(pFindRssi,sep,&pNext);
	pRet = strtok_s(NULL,sep,&pNext);

	char szRssi[24]="";
	strcpy_s(szRssi, MINBUFSIZE, pRet);
	d_RssiValue = (float)atof(szRssi);
	memset(rssi_token,0,MINBUFSIZE);  
	sprintf_s(szRssiValue,MINBUFSIZE,"%.2f(RSSI)",d_RssiValue);

    m_strMainrssi="";
	m_strMainrssi=szRssi;
	m_strRecordTestData += szRssiValue;
	m_strRecordTestData  +=  "\t";

	m_strStoreEthernetData.Empty();
	if(d_RssiValue<m_IniFileInfo.MURssiSpec || d_RssiValue>m_IniFileInfo.MDRssiSpec)
	{
		return 1;
	}	

	// remove external antenna and check rssi
	ShowMsg("*****Rssi Test(Without Main Antenna out side)******");
	AfxMessageBox("請打開屏蔽箱線擰下天線，確認屏蔽箱關閉，再點擊此按鈕!!!Open shielding box,take off antenna,and then shut down!!!");
	d_RssiValue=0.0;

	if(!SendDutCommand("wwan -rx1","Main:",20000))
	{
		if(!SendDutCommand("wwan -rx1","Main:",20000))
		{
			return 0;
		}
	}

	strcpy_s(rssi_token,sizeof(rssi_token),m_strStoreEthernetData);
	pFindRssi = strstr(rssi_token,"Main:");
	pRet = strtok_s(pFindRssi,sep,&pNext);
	pRet = strtok_s(NULL,sep,&pNext);	
	memset (szRssi,0,sizeof(szRssi));
	strcpy_s(szRssi, MINBUFSIZE, pRet);

	d_RssiValue = (float)atof(szRssi);
	memset(rssi_token,0,MINBUFSIZE);  

	CmpRsi=0.0;		
	CmpRsi=d_RssiValue;	

    m_strwizoutrssi="";
    m_strwizoutrssi=szRssi;
	sprintf_s(szRssiValue,MINBUFSIZE,"%.2f(RSSI)",d_RssiValue);
	m_strRecordTestData += szRssiValue;
	m_strRecordTestData  +=  "\t";	

	m_strStoreEthernetData.Empty();	

	if(d_RssiValue<m_IniFileInfo.NURssiSpec || d_RssiValue>m_IniFileInfo.NDRssiSpec)
	{
		return 1;
	}

	// start to test anx antenna 	
	ShowMsg("*****Rssi Test(Aux Antenna)******");
	if(!SendDutCommand("gpio 0 1","#",1000))	return 0;
	if(!SendDutCommand("wwan -rx2","Aux:",20000))	return 0;

	strcpy_s(rssi_token,sizeof(rssi_token),m_strStoreEthernetData);
	pFindRssi = strstr(rssi_token,"Aux:");
	pRet = strtok_s(pFindRssi,sep,&pNext);
	pRet = strtok_s(NULL,sep,&pNext);	
	memset (szRssi,0,sizeof(szRssi));
	strcpy_s(szRssi, MINBUFSIZE, pRet);

	d_RssiValue = (float)atof(szRssi);
	m_strauxrssi="";
	m_strauxrssi=szRssi;
	memset(rssi_token,0,MINBUFSIZE);    
	sprintf_s(szRssiValue,MINBUFSIZE,"%.2f(RSSI)",d_RssiValue);

	m_strRecordTestData += szRssiValue;
	m_strRecordTestData  +=  "\t";		
	m_strStoreEthernetData.Empty();
	if(d_RssiValue<m_IniFileInfo.MURssiSpec || d_RssiValue>m_IniFileInfo.MDRssiSpec)
	{
		return 3;
	}	

	// Test main printed antenna 
	ShowMsg("*****Rssi Test(inside Main Antenna)******");
	if(!SendDutCommand("wwan -rx1","Main:",20000))	return 0;

	strcpy_s(rssi_token,sizeof(rssi_token), m_strStoreEthernetData);
	pFindRssi = strstr(rssi_token,"Main:");
	pRet = strtok_s(pFindRssi,sep,&pNext);
	pRet = strtok_s(NULL,sep,&pNext);	

	memset(szRssi,0,sizeof(szRssi));
	strncpy_s(szRssi, MINBUFSIZE, pRet, 6);
	d_RssiValue = (float)atof(szRssi);

	memset(rssi_token,0,MINBUFSIZE); 
	char soi[50]="111111";
	CmpRso=d_RssiValue-CmpRsi;
	sprintf_s(soi,MINBUFSIZE,"RSSI delta: %.2f(RSSI)",CmpRso);
	ShowMsg(soi);
	sprintf_s(szRssiValue,MINBUFSIZE,"%.2f(RSSI)",d_RssiValue);
	m_strinsidrssi="";

	m_strRecordTestData += szRssiValue;
	m_strRecordTestData  +=  "\t";		
	m_strStoreEthernetData.Empty();

	if( d_RssiValue<m_IniFileInfo.MURssiSpec || d_RssiValue>m_IniFileInfo.MDRssiSpec )
	{
		return 2;
	}

	return -1;
}

int CRunInfo::DutTestWiFiThroughput_1(void)
{
	// if test channel number from ini file is 1, read channel from UI
	// if test channel number is 3, fix channel to 1, 6, 11

	// if no throughput test, return directly
	ShowMsg("***************Test WiFi Throughput****************");
	int iRst=-1;	

	char szSetChannelCommand[MINBUFSIZE]  = "";

	//2G
	char szBuf_1[256] = "";
	sprintf_s(szBuf_1 , 256 , "m_IniFileInfo.nChannelNum_2G_1 = %d " , m_IniFileInfo.nChannelNum_2G_1);
	DisplayRunTimeInfo(szBuf_1) ;

	if(m_IniFileInfo.nChannelNum_2G_1 == 0)
	{
		// no test channel, skip
		//return -1; 
		//2.4G needn't ship,if just need test 5G,ship will not test
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=0!");
	}
	else if (m_IniFileInfo.nChannelNum_2G_1 == 1)
	{
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=1!");
		// run random channel from UI
		((CEdit*)GetDlgItem(IDC_STATIC_2G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->EnableWindow(false);
		CString m_wifiChannel;
		GetDlgItemText(IDC_EDIT_2G_CHANNEL1,m_wifiChannel);
		int iChannel = atoi(m_wifiChannel.GetBuffer(m_wifiChannel.GetLength()));

		iRst = RunThroughputTest(iChannel);
		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
	}
	else if (m_IniFileInfo.nChannelNum_2G_1 == 2)
	{
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=2!");
		// run random channel from UI
		//CHANNEL 1
		CString m_wifiChannel_1;
		GetDlgItemText(IDC_EDIT_2G_CHANNEL1,m_wifiChannel_1);
		int iChannel_1 = atoi(m_wifiChannel_1.GetBuffer(m_wifiChannel_1.GetLength()));

		iRst = RunThroughputTest(iChannel_1);

		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_1, iRst);
		DisplayRunTimeInfo(szBuffer) ;
		
		SendDutCommand("","#",5000);
		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
		//CHANNEL 2
		CString m_wifiChannel_2;
		GetDlgItemText(IDC_EDIT_2G_CHANNEL2,m_wifiChannel_2);
		int iChannel_2 = atoi(m_wifiChannel_2.GetBuffer(m_wifiChannel_2.GetLength()));

		iRst = RunThroughputTest(iChannel_2);

		
		memset(szBuffer , 0 , 256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_2, iRst);
		DisplayRunTimeInfo(szBuffer) ;
		
		SendDutCommand("","#",5000);
		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
	}
	else if(m_IniFileInfo.nChannelNum_2G_1 == 3)
	{
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=3!");
		// run fixed channel

		((CEdit*)GetDlgItem(IDC_STATIC_2G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_STATIC_2G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->EnableWindow(false);

		iRst = RunThroughputTest(3);
		
		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(3) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}

		iRst = RunThroughputTest(6);

		memset(szBuffer , 0 , 256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(6) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{	
			return iRst;
		}
		
		iRst = RunThroughputTest(9);

		memset(szBuffer , 0 , 256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(9) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{	
			return iRst;
		}	
	}	

	//5G
	char szBuf_2[256] = "";
	sprintf_s(szBuf_2 , 256 , "m_IniFileInfo.nChannelNum_5G_1 = %d " , m_IniFileInfo.nChannelNum_5G_1);
	DisplayRunTimeInfo(szBuf_2) ;

	if(m_IniFileInfo.nChannelNum_5G_1 == 0)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=0!");
		
		if(m_IniFileInfo.nChannelNum_2G_1 == 0)
		{
			return -1; // no test channel, skip
		}
	}
	else if (m_IniFileInfo.nChannelNum_5G_1 == 1)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=1!");
		// run random channel from UI
		((CEdit*)GetDlgItem(IDC_STATIC_5G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->EnableWindow(false);
		CString m_wifiChannel;
		GetDlgItemText(IDC_EDIT_5G_CHANNEL1,m_wifiChannel);
		int iChannel = atoi(m_wifiChannel.GetBuffer(m_wifiChannel.GetLength()));
		
		iRst = RunThroughputTest(iChannel);
		
		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		SendDutCommand("wl -i eth2 down","#",5000);	
		Sleep(3000);
		if(iRst != -1)
		{			
			return iRst;
		}
	}
	else if (m_IniFileInfo.nChannelNum_5G_1 == 2)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=2!");
		// run random channel from UI
		//CHANNEL 1
		CString m_wifiChannel_1;
		GetDlgItemText(IDC_EDIT_5G_CHANNEL1,m_wifiChannel_1);
		int iChannel_1 = atoi(m_wifiChannel_1.GetBuffer(m_wifiChannel_1.GetLength()));

		iRst = RunThroughputTest(iChannel_1);

		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_1, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		SendDutCommand("wl -i eth2 down","#",5000);	
		Sleep(3000);
		if(iRst != -1)
		{
			return iRst;
		}
		//CHANNEL 2
		CString m_wifiChannel_2;
		GetDlgItemText(IDC_EDIT_5G_CHANNEL2,m_wifiChannel_2);
		int iChannel_2 = atoi(m_wifiChannel_2.GetBuffer(m_wifiChannel_2.GetLength()));

		iRst = RunThroughputTest(iChannel_2);

		memset(szBuffer , 0 ,256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_2, iRst);
		DisplayRunTimeInfo(szBuffer) ;
		
		SendDutCommand("","#",5000);
		SendDutCommand("wl -i eth2 down","#",5000);	
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
	}
	else if(m_IniFileInfo.nChannelNum_5G_1 == 3)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=3!");
		// run fixed channel

		((CEdit*)GetDlgItem(IDC_STATIC_5G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_STATIC_5G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->EnableWindow(false);

		iRst = RunThroughputTest(36);
		Sleep(3000);

		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(36) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		SendDutCommand("wl -i eth2 down","#",5000);	
		if(iRst != -1)
		{	
			return iRst;
		}

		iRst = RunThroughputTest(44);
		Sleep(3000);

		memset(szBuffer , 0 ,256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(44) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		SendDutCommand("wl -i eth2 down","#",5000);	
		if(iRst != -1)
		{	
			return iRst;
		}
		Sleep(3000);
		
		iRst = RunThroughputTest(157);

		memset(szBuffer , 0 ,256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(157) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("","#",5000);
		SendDutCommand("wl -i eth2 down","#",5000);	
		if(iRst != -1)
		{
			return iRst;
		}	
	}
	SendDutCommand("","#",5000);
	return -1;
}

int CRunInfo::DutTestWiFiThroughput_2(void)
{
	// if test channel number from ini file is 1, read channel from UI
	// if test channel number is 3, fix channel to 1, 6, 11

	// if no throughput test, return directly
	ShowMsg("***************Test WiFi Throughput****************");
	int iRst=-1;

	char szSetChannelCommand[MINBUFSIZE]  = "";

	//2G
	char szBuf_1[256] = "";
	sprintf_s(szBuf_1 , 256 , "m_IniFileInfo.nChannelNum_2G_2 = %d " , m_IniFileInfo.nChannelNum_2G_2);
	DisplayRunTimeInfo(szBuf_1) ;

	if(m_IniFileInfo.nChannelNum_2G_2 == 0)
	{
		// no test channel, skip
		//return -1; 
		//2.4G needn't ship,if just need test 5G,ship will not test
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=0!");
	}
	else if (m_IniFileInfo.nChannelNum_2G_2 == 1)
	{
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=1!");
		// run random channel from UI
		((CEdit*)GetDlgItem(IDC_STATIC_2G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->EnableWindow(false);
		CString m_wifiChannel;
		GetDlgItemText(IDC_EDIT_2G_CHANNEL1,m_wifiChannel);
		int iChannel = atoi(m_wifiChannel.GetBuffer(m_wifiChannel.GetLength()));

		iRst = RunThroughputTest(iChannel , 2);
		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{			
			return iRst;
		}
	}
	else if (m_IniFileInfo.nChannelNum_2G_2 == 2)
	{
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=2!");
		// run random channel from UI
		//CHANNEL 1
		CString m_wifiChannel_1;
		GetDlgItemText(IDC_EDIT_2G_CHANNEL1,m_wifiChannel_1);
		int iChannel_1 = atoi(m_wifiChannel_1.GetBuffer(m_wifiChannel_1.GetLength()));

		iRst = RunThroughputTest(iChannel_1 , 2);

		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_1, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
		//CHANNEL 2
		CString m_wifiChannel_2;
		GetDlgItemText(IDC_EDIT_2G_CHANNEL2,m_wifiChannel_2);
		int iChannel_2 = atoi(m_wifiChannel_2.GetBuffer(m_wifiChannel_2.GetLength()));

		iRst = RunThroughputTest(iChannel_2 , 2);

		
		memset(szBuffer , 0 , 256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_2, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
	}
	else if(m_IniFileInfo.nChannelNum_2G_2 == 3)
	{
		ShowMsg("At test config file:TestInformation.ini find 2.4G WiFi throughput test channel number=3!");
		// run fixed channel

		((CEdit*)GetDlgItem(IDC_STATIC_2G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_2G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_STATIC_2G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_2G_CHANNEL2))->EnableWindow(false);

		iRst = RunThroughputTest(3 , 2);
		
		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(3) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}

		iRst = RunThroughputTest(6 , 2);

		memset(szBuffer , 0 , 256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(6) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
		
		iRst = RunThroughputTest(9 , 2);

		memset(szBuffer , 0 , 256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(9) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}	
	}	

	//5G
	char szBuf_2[256] = "";
	sprintf_s(szBuf_2 , 256 , "m_IniFileInfo.nChannelNum_5G_2 = %d " , m_IniFileInfo.nChannelNum_5G_2);
	DisplayRunTimeInfo(szBuf_2) ;

	if(m_IniFileInfo.nChannelNum_5G_2 == 0)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number!");
		
		if(m_IniFileInfo.nChannelNum_2G_2 == 0)
		{
			return -1; // no test channel, skip
		}
	}
	else if (m_IniFileInfo.nChannelNum_5G_2 == 1)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=1!");
		// run random channel from UI
		((CEdit*)GetDlgItem(IDC_STATIC_5G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->EnableWindow(false);
		CString m_wifiChannel;
		GetDlgItemText(IDC_EDIT_5G_CHANNEL1,m_wifiChannel);
		int iChannel = atoi(m_wifiChannel.GetBuffer(m_wifiChannel.GetLength()));
		
		iRst = RunThroughputTest(iChannel , 2);
		
		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl -i eth2 down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{			
			return iRst;
		}
	}
	else if (m_IniFileInfo.nChannelNum_5G_2 == 2)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=2!");
		// run random channel from UI
		//CHANNEL 1
		CString m_wifiChannel_1;
		GetDlgItemText(IDC_EDIT_5G_CHANNEL1,m_wifiChannel_1);
		int iChannel_1 = atoi(m_wifiChannel_1.GetBuffer(m_wifiChannel_1.GetLength()));

		iRst = RunThroughputTest(iChannel_1 , 2);

		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_1, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl -i eth2 down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
		//CHANNEL 2
		CString m_wifiChannel_2;
		GetDlgItemText(IDC_EDIT_5G_CHANNEL2,m_wifiChannel_2);
		int iChannel_2 = atoi(m_wifiChannel_2.GetBuffer(m_wifiChannel_2.GetLength()));

		iRst = RunThroughputTest(iChannel_2 , 2);

		memset(szBuffer , 0 ,256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(%d) return value = %d " ,iChannel_2, iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl -i eth2 down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
	}
	else if(m_IniFileInfo.nChannelNum_5G_2 == 3)
	{
		ShowMsg("At test config file:TestInformation.ini find 5G WiFi throughput test channel number=3!");
		// run fixed channel

		((CEdit*)GetDlgItem(IDC_STATIC_5G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_5G_CHANNEL1))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_STATIC_5G_CHANNEL2))->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT_5G_CHANNEL2))->EnableWindow(false);

		iRst = RunThroughputTest(38 , 2);

		char szBuffer[256] = "";
		sprintf_s(szBuffer , 256 , "RunThroughputTest(38) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;
		
		if(!SendDutCommand("wl -i eth2 down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{	
			return iRst;
		}

		iRst = RunThroughputTest(44 , 2);

		memset(szBuffer , 0 ,256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(44) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		if(!SendDutCommand("wl -i eth2 down","#",5000))
		{	
			return 1;
		}
		Sleep(3000);

		if(iRst != -1)
		{
			return iRst;
		}
		
		iRst = RunThroughputTest(157 , 2);

		memset(szBuffer , 0 ,256);
		sprintf_s(szBuffer , 256 , "RunThroughputTest(157) return value = %d " , iRst);
		DisplayRunTimeInfo(szBuffer) ;

		SendDutCommand("wl -i eth2 down","#",5000);	
		if(iRst != -1)
		{
			return iRst;
		}	
	}

	if(!SendDutCommand("wl -i eth2 down","#",5000))
	{	
		return 1;
	}	
	if(!SendDutCommand("wl -i eth2 down","#",5000))
	{	
		return 1;
	}	
	
	SendDutCommand("\n","#",5000);
	return -1;
}


int CRunInfo::RunThroughputTest(int iChannel , int iChipsetIndex)
{
	SendDutCommand("","#",2000);
	double     TXValue = 0;
	double     RXValue = 0;
	double     TXRXValue = 0;
	double     TXValue_Rand = 0;
	double     RXValue_Rand = 0;
	double     TXRXValue_Rand = 0;
	char       szShowTXResult[MINBUFSIZE] = "";
	char       szShowRXResult[MINBUFSIZE] = "";
	char       szShowTXRXResult[MINBUFSIZE] = "";
	char       szCovertValueToCharChannelTX[MINBUFSIZE] ;
	char       szCovertValueToCharChannelRX[MINBUFSIZE] ;
	char       szCovertValueToCharChannelTXRX[MINBUFSIZE] ;
	memset(szCovertValueToCharChannelTX , 0, MINBUFSIZE);
	memset(szCovertValueToCharChannelRX , 0 , MINBUFSIZE);
	memset(szCovertValueToCharChannelTXRX , 0 , MINBUFSIZE);

	char szstrIP[MINBUFSIZE] = "";
	CString szIperfInfo;
	CString strSSID;
	char    szIperfCmd_D[MINBUFSIZE] = "";
	char    szIperfCmd_R[MINBUFSIZE] = "";
	char    szPingGolden[MAXBUFSIZE] = "";

	if(iChannel > 14)
	{
		sprintf_s(szstrIP, MINBUFSIZE, "%s", m_IniFileInfo.sz5GThrotghputGoldenIP);
	}
	else
	{
		sprintf_s(szstrIP, MINBUFSIZE, "%s", m_IniFileInfo.sz2GThrotghputGoldenIP);
	}
	
	sprintf_s(szPingGolden  ,MAXBUFSIZE,"ping %s -t",szstrIP);
	sprintf_s(szIperfCmd_D  ,MINBUFSIZE, m_IniFileInfo.szStoreIperfPara_D, szstrIP);
	sprintf_s(szIperfCmd_R  ,MINBUFSIZE, m_IniFileInfo.szStoreIperfPara_R, szstrIP);
	
	//setting DUT
	int SettingDUTValue = SettingDUT(iChannel , iChipsetIndex);
	if(SettingDUTValue != -1)
	{
		return SettingDUTValue;	
	}

	//send Golden setting command
	DisplayRunTimeInfo("m_IniFileInfo.nGoldenSeverFlag = " , m_IniFileInfo.nGoldenSeverFlag);
	if(m_IniFileInfo.nGoldenSeverFlag == 1)
	{		
		int SettingGoldenServerValue = SettingGoldenServer( iChannel );
		if( SettingGoldenServerValue != -1 )
		{
			return SettingGoldenServerValue;
		}	
	}
	//Sleep or not

	if(m_IniFileInfo.nSleepOrNo == 1)
	{
		Sleep(m_IniFileInfo.nTimeOut*1000);
	}

// for tx+rx
#if 1
	// for bi-direction throughput test
	if(0 == strcmp(m_IniFileInfo.szStoreTestThroughput,"1") )
	{
		int  dwReTestCount = m_IniFileInfo.nRetryNum ;
		while(dwReTestCount--)
		{
			//send Golden setting command

			//int SettingGoldenServerValue = SettingGoldenServer( iChannel );
			//if( SettingGoldenServerValue != -1 )
			//{
			//	return SettingGoldenServerValue;
			//}

			DeleteSpecifyExe("iperf.exe"); // kill iperf.exe process to make sure the program not hang up.
			szIperfInfo.Empty();
			DisplayRunTimeInfo("-Begin ping golden PC");
			if(!PingSpecifyIP_2(szstrIP, m_IniFileInfo.nPingGoldenCount))
			{	
				if(dwReTestCount)
				{
					continue;
				}
				else
				{
					if(iChannel > 14)//5G
						return 4;	// ping 5G golden failed
					else
						return 5;   // ping 2G golden failed
				}
			}
			DeleteSpecifyExe("ping.exe");

			TXValue = 0;
			RXValue = 0;
			DisplayRunTimeInfo("Start TX+RX throughput test");
			
			RunSpecifyExeAndRead(szIperfInfo,szIperfCmd_D,false);  
			GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue, RXValue);

			TXRXValue_Rand = TXValue + RXValue;

			//201205010 add	
			
			if(iChannel > 14)//5G
			{
				if(m_IniFileInfo.nThroughputFalg5G_Factroy == 1)
				{
					TXRXValue = __RandResult(TXRXValue_Rand, m_IniFileInfo.nTXRXThroughputSpec5G, m_IniFileInfo.nTXRXThroughputSpec5G_Factroy, m_IniFileInfo.nThroughputRange5G_Factroy);
				}
				else
				{
					TXRXValue = TXRXValue_Rand;
				}			
			}
			if(iChannel < 14)//2G
			{
				if(m_IniFileInfo.nThroughputFalg_Factroy == 1)
				{
					TXRXValue = __RandResult(TXRXValue_Rand, m_IniFileInfo.nTXRXThroughputSpec, m_IniFileInfo.nTXRXThroughputSpec_Factroy, m_IniFileInfo.nThroughputRange_Factroy);
				}
				else
				{
					TXRXValue = TXRXValue_Rand;
				}			
			}
			//end

			sprintf_s(szShowTXRXResult, MINBUFSIZE, "Channel %d TX+RX throughput: %.2f Mbit", iChannel, TXRXValue);
			DisplayRunTimeInfo(szShowTXRXResult);
			szIperfInfo.Empty();
			DisplayRunTimeInfo("");


			if( iChipsetIndex == 1)
			{
				if(iChannel > 14)//5G
				{
					if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpec5G && TXRXValue <= m_IniFileInfo.nTXRXThroughputSpecHigt5G )
					{
						break;
					}
					else if(TXRXValue <= m_IniFileInfo.nTXRXThroughputSpec5G)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTXRX);
							return 6; //5G TX+RX LOWER THAN SPEC
						}
					}
					else if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpecHigt5G)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s," , TestDetaillog , szCovertValueToCharChannelTXRX);
							return 7; //5G TX+RX HIGTER THAN SPEC
						}
					}
				}
				else//2.4G
				{
					if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpec && TXRXValue <= m_IniFileInfo.nTXRXThroughputSpecHigt )
					{
						break;
					}
					else if(TXRXValue <= m_IniFileInfo.nTXRXThroughputSpec)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTXRX);
							return 8; //2G TX+RX LOWER THAN SPEC
						}
					}
					else if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpecHigt)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s," , TestDetaillog , szCovertValueToCharChannelTXRX);
							return 9; //2G TX+RX HIGTER THAN SPEC
						}
					}
				}
			}
			else if(iChipsetIndex == 2)
			{		
				if(iChannel > 14)//5G
				{
					if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpec5G_2 && TXRXValue <= m_IniFileInfo.nTXRXThroughputSpecHigt5G_2 )
					{
						break;
					}
					else if(TXRXValue <= m_IniFileInfo.nTXRXThroughputSpec5G_2)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTXRX);
							return 6; //5G TX+RX LOWER THAN SPEC
						}
					}
					else if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpecHigt5G_2)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s," , TestDetaillog , szCovertValueToCharChannelTXRX);
							return 7; //5G TX+RX HIGTER THAN SPEC
						}
					}
				}
				else//2.4G
				{
					if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpec_2 && TXRXValue <= m_IniFileInfo.nTXRXThroughputSpecHigt_2 )
					{
						break;
					}
					else if(TXRXValue <= m_IniFileInfo.nTXRXThroughputSpec_2)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTXRX);
							return 8; //2G TX+RX LOWER THAN SPEC
						}
					}
					else if(TXRXValue >= m_IniFileInfo.nTXRXThroughputSpecHigt_2)
					{	
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTXRX, MINBUFSIZE, "%.2f(TXRX)", TXRXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s," , TestDetaillog , szCovertValueToCharChannelTXRX);
							return 9; //2G TX+RX HIGTER THAN SPEC
						}
					}
				}
			}
			else
			{
				//do nothing
			}
		}
		sprintf_s(szCovertValueToCharChannelTXRX,MINBUFSIZE,"%.2f(TXRX)",TXRXValue);
		m_strRecordTestData  +=  szCovertValueToCharChannelTXRX;
		m_strRecordTestData  +=  "\t";
		/*collect test data*/
		sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTXRX);
		/*End*/
		SendDutCommand("\n","#",1000);
	}

#endif
// for tx, rx
#if 1
	if(0 == strcmp(m_IniFileInfo.szStoreTestThroughputSingle,"1"))
	{
		int  dwReTestCount = m_IniFileInfo.nRetryNum ;
		while(dwReTestCount--)
		{
			////send Golden setting command

			//int SettingGoldenServerValue = SettingGoldenServer( iChannel );
			//if( SettingGoldenServerValue != -1 )
			//{
			//	return SettingGoldenServerValue;
			//}

			DeleteSpecifyExe("iperf.exe"); // kill iperf.exe process to make sure the program not hang up.
			szIperfInfo.Empty();
			DisplayRunTimeInfo("Begin ping golden PC");
			if(!PingSpecifyIP_2(szstrIP, m_IniFileInfo.nPingGoldenCount))
			{
				if(dwReTestCount)
				{
					continue;
				}
				else
				{
					if(iChannel > 14)//5G
						return 4;	// ping 5G golden failed
					else
						return 5;   // ping 2G golden failed
				}
			}
			TXValue = 0;
			RXValue = 0;
			DisplayRunTimeInfo("Start TX/RX throughput test");

			//while(1)
			//{
			//	RunSpecifyExeAndRead(szIperfInfo,szIperfCmd_R,false);  //20130821
			//	Delay(8000);
			//	DeleteSpecifyExe("iperf.exe");
			//	break;
			//}

			RunSpecifyExeAndRead(szIperfInfo,szIperfCmd_R,false);  //modyfy 06.11

			GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue_Rand, RXValue_Rand);


			//201205010 add
			if(iChannel > 14)//5G
			{
				if(m_IniFileInfo.nThroughputFalg5G_Factroy == 1)
				{
					TXValue = __RandResult(TXValue_Rand, m_IniFileInfo.nTXThroughputSpec5G, m_IniFileInfo.nTXThroughputSpec5G_Factroy, m_IniFileInfo.nThroughputRange5G_Factroy);
					RXValue = __RandResult(RXValue_Rand, m_IniFileInfo.nRXThroughputSpec5G, m_IniFileInfo.nRXThroughputSpec5G_Factroy, m_IniFileInfo.nThroughputRange5G_Factroy);
				}
				else
				{
					TXValue = TXValue_Rand ;
					RXValue = RXValue_Rand ;
					
				}			
			}
			if(iChannel < 14)//2G
			{
				if(m_IniFileInfo.nThroughputFalg_Factroy == 1)
				{
					TXValue = __RandResult(TXValue_Rand, m_IniFileInfo.nTXThroughputSpec, m_IniFileInfo.nTXThroughputSpec_Factroy, m_IniFileInfo.nThroughputRange_Factroy);
					RXValue = __RandResult(RXValue_Rand, m_IniFileInfo.nRXThroughputSpec, m_IniFileInfo.nRXThroughputSpec_Factroy, m_IniFileInfo.nThroughputRange_Factroy);
				}
				else
				{
					TXValue = TXValue_Rand ;
					RXValue = RXValue_Rand ;					
				}			
			}
			//end


			sprintf_s(szShowTXResult, MINBUFSIZE, "Channel %d TX throughput: %.2f Mbit", iChannel, TXValue);
			sprintf_s(szShowRXResult, MINBUFSIZE, "Channel %d RX throughput: %.2f Mbit", iChannel, RXValue);
			DisplayRunTimeInfo(szShowTXResult);
			DisplayRunTimeInfo(szShowRXResult);

			DisplayRunTimeInfo("\n");

			if(iChipsetIndex == 1)
			{
				if(iChannel > 14)//5G
				{
					if(TXValue >= m_IniFileInfo.nTXThroughputSpec5G 
						&& RXValue >= m_IniFileInfo.nRXThroughputSpec5G
						&& TXValue <= m_IniFileInfo.nTXThroughputSpecHigt5G
						&& RXValue <= m_IniFileInfo.nRXThroughputSpecHigt5G)
					{
						break;
					}
					else if(TXValue < m_IniFileInfo.nTXThroughputSpec5G)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX, MINBUFSIZE, "%.2f(TX)", TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";		
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTX);
							/*End*/
							return 10;	// 5G TX LOWER THAN SPEC
						}
					}
					else if(RXValue < m_IniFileInfo.nRXThroughputSpec5G)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelRX);
							/*End*/
							return 11; 	// 5G RX LOWER THAN SPEC						
						}
					}
					else if(TXValue >= m_IniFileInfo.nTXThroughputSpecHigt
						|| RXValue >= m_IniFileInfo.nRXThroughputSpecHigt)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX)",TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,%s,",TestDetaillog,szCovertValueToCharChannelTX,szCovertValueToCharChannelRX);
							/*End*/
							return 12; 	// 5G TX/RX HIGTER THAN SPEC					
						}
					}
				}
				else
				{
					if(TXValue >= m_IniFileInfo.nTXThroughputSpec 
						&& RXValue >= m_IniFileInfo.nRXThroughputSpec
						&& TXValue <= m_IniFileInfo.nTXThroughputSpecHigt
						&& RXValue <= m_IniFileInfo.nRXThroughputSpecHigt)
					{
						break;
					}
					else if(TXValue < m_IniFileInfo.nTXThroughputSpec)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX, MINBUFSIZE, "%.2f(TX)", TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";		
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTX);
							/*End*/
							return 13;	// 2G TX LOWER THAN SPEC
						}
					}
					else if(RXValue < m_IniFileInfo.nRXThroughputSpec)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelRX);
							/*End*/
							return 14; 		//2G RX LOWER THAN SPEC						
						}
					}
					else if(TXValue >= m_IniFileInfo.nTXThroughputSpecHigt
						|| RXValue >= m_IniFileInfo.nRXThroughputSpecHigt)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX)",TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,%s,",TestDetaillog,szCovertValueToCharChannelTX,szCovertValueToCharChannelRX);
							/*End*/
							return 15; 		// 2G TX/RX HIGTER THAN SPEC						
						}
					}
				}
			}
			else if(iChipsetIndex == 2)
			{
				if(iChannel > 14)//5G
				{
					if(TXValue >= m_IniFileInfo.nTXThroughputSpec5G_2 
						&& RXValue >= m_IniFileInfo.nRXThroughputSpec5G_2
						&& TXValue <= m_IniFileInfo.nTXThroughputSpecHigt5G_2
						&& RXValue <= m_IniFileInfo.nRXThroughputSpecHigt5G_2)
					{
						break;
					}
					else if(TXValue < m_IniFileInfo.nTXThroughputSpec5G_2)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX, MINBUFSIZE, "%.2f(TX)", TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";		
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTX);
							/*End*/
							return 10;	// 5G TX LOWER THAN SPEC
						}
					}
					else if(RXValue < m_IniFileInfo.nRXThroughputSpec5G_2)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelRX);
							/*End*/
							return 11; 	// 5G RX LOWER THAN SPEC						
						}
					}
					else if(TXValue >= m_IniFileInfo.nTXThroughputSpecHigt_2
						|| RXValue >= m_IniFileInfo.nRXThroughputSpecHigt_2)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX)",TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,%s,",TestDetaillog,szCovertValueToCharChannelTX,szCovertValueToCharChannelRX);
							/*End*/
							return 12; 	// 5G TX/RX HIGTER THAN SPEC					
						}
					}
				}
				else
				{
					if(TXValue >= m_IniFileInfo.nTXThroughputSpec_2 
						&& RXValue >= m_IniFileInfo.nRXThroughputSpec_2
						&& TXValue <= m_IniFileInfo.nTXThroughputSpecHigt_2
						&& RXValue <= m_IniFileInfo.nRXThroughputSpecHigt_2)
					{
						break;
					}
					else if(TXValue < m_IniFileInfo.nTXThroughputSpec_2)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX, MINBUFSIZE, "%.2f(TX)", TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelTX;
							m_strRecordTestData  +=  "\t";		
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelTX);
							/*End*/
							return 13;	// 2G TX LOWER THAN SPEC
						}
					}
					else if(RXValue < m_IniFileInfo.nRXThroughputSpec_2)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,",TestDetaillog,szCovertValueToCharChannelRX);
							/*End*/
							return 14; 		//2G RX LOWER THAN SPEC						
						}
					}
					else if(TXValue >= m_IniFileInfo.nTXThroughputSpecHigt_2
						|| RXValue >= m_IniFileInfo.nRXThroughputSpecHigt_2)
					{
						if(dwReTestCount)
							continue;
						else
						{
							sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX)",TXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
							m_strRecordTestData  +=  szCovertValueToCharChannelRX;
							m_strRecordTestData  +=  "\t";
							/*collect test data*/
							sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,%s,",TestDetaillog,szCovertValueToCharChannelTX,szCovertValueToCharChannelRX);
							/*End*/
							return 15; 		// 2G TX/RX HIGTER THAN SPEC						
						}
					}
				}

			}
			else
			{
				//do nothing
			}
		}
		sprintf_s(szCovertValueToCharChannelTX,MINBUFSIZE,"%.2f(TX)",TXValue);
		m_strRecordTestData  +=  szCovertValueToCharChannelTX;
		m_strRecordTestData  +=  "\t";
		sprintf_s(szCovertValueToCharChannelRX,MINBUFSIZE,"%.2f(RX)",RXValue);
		m_strRecordTestData  +=  szCovertValueToCharChannelRX;
		m_strRecordTestData  +=  "\t";

		/*collect test data*/
		sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,%s,", TestDetaillog , szCovertValueToCharChannelTX,szCovertValueToCharChannelRX);
		/*End*/
	}
#endif 
	
	return -1;
}
void CRunInfo::DeleteSpecifyExe(char* exeName)
{
	if(exeName == NULL)
		return;

	HANDLE hProcess = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(INVALID_HANDLE_VALUE == hProcess)
		return;
	BOOL fPOK = Process32First(hProcess,&pe);
	for(; fPOK;fPOK = Process32Next(hProcess,&pe))
	{
		if(strstr(_strlwr(pe.szExeFile),exeName) != NULL)
		{
			HANDLE hPr = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID );
			if( hPr == NULL )
				return;
			else
			{
				TerminateProcess(hPr,0);
			}
		}
	}
	if(INVALID_HANDLE_VALUE != hProcess)
		CloseHandle(hProcess);
	
}

void CRunInfo::InitTestRecordData(void)
{
	m_TestRecordinfo.szRecordTestTime = "000(s)";
	m_TestRecordinfo.szRecordFirmware = "NULL";
	m_TestRecordinfo.szRecordMac = "NULL";
	m_TestRecordinfo.szRecordPcName = "NULL";
	m_TestRecordinfo.szRecordPincode = "NULL";
	m_TestRecordinfo.szRecordSSN = "NULL";
	m_TestRecordinfo.szRecordProductName = "NULL";
	m_TestRecordinfo.szRecordResult = "NULL";
	m_TestRecordinfo.szRecordSN = "0";
	m_TestRecordinfo.szRecordTime = "00:00:00";
	
}

CString CRunInfo::CustomerFirmwareVersion(char*  DutFirmwareVersion)
{
	char    Seps[] = "\r\n";
	char    *token;
	char    *Context;
	token  = strtok_s(DutFirmwareVersion, Seps, &Context);
	while(token != NULL)
	{
		token = strtok_s(NULL, Seps, &Context);

		CString str = token;
		str.Trim();
		if(strstr(str , "/"))
		{
			int nPostion = str.Find("/");
			str.Delete(0 , nPostion + 1);
			str.Replace('/','_');
			str.Trim();
			return str;
		}
		else
		{
			continue;
		}
	}
	return DutFirmwareVersion;
}
bool CRunInfo::DutCheckBoardID(void)
{
	/*
	true	:pass
	false	:failed
	*/
	DisplayRunTimeInfo("---Start check boardid---");

	if(!SendDutCommand("burnboardid","#",5000)) return false;
	if(strstr(m_strStoreEthernetData, m_IniFileInfo.szStoreBoardID ) == NULL || (!strcmp("", m_IniFileInfo.szStoreBoardID)) || strlen(m_IniFileInfo.szStoreBoardID)==0 )
	{
		return false;
	}
	/*collect test data*/
	sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%s,", TestDetaillog , m_IniFileInfo.szStoreBoardID);
	/*End*/

	return true;
}

bool CRunInfo::DutCheckHDD(void)
{
	/*
	true	:pass
	false	:failed
	*/

	//customer read and write test command for HDD and USB
	/*
	HDD Test Command:
	mount -t ext4 /dev/sda1 /mnt
		cd /mnt
		time dd if=/dev/zero bs=1024 count=50000 of=1Gb.file //写测试 0.6s
		time dd if=1Gb.file bs=64k | dd of=/dev/null //读测试  0.5s
		time dd if=1Gb.file of=2.Gb.file bs=64k //读写同步测试  0.9s
		rm 1Gb.file 2.Gb.file  //0.1s

	USB Test Command:
	mount /dev/sdb1 /mnt
		cd /mnt
		time dd if=/dev/zero bs=1024 count=30000 of=1Gb.file //写测试 0.5s
		time dd if=1Gb.file bs=64k | dd of=/dev/null //读测试  0.4s
		time dd if=1Gb.file of=2.Gb.file bs=64k //读写同步测试  3s
		rm 1Gb.file 2.Gb.file  //0.1s
	*/
	//

	//old HDD test method 
	/*DisplayRunTimeInfo("---Start check HDD---");

	if(!SendDutCommand("ls /dev","sda1",5000)) 
		return false;

	if(!SendDutCommand("mkdir /mnt/hdd","#",5000)) 
		return false;

	if(!SendDutCommand("mount /dev/sda1 /mnt/hdd","#",5000)) 
		return false;

	if(!SendDutCommand("mkdir /mnt/hdd/zzz","#",5000)) 
		return false;

	if(!SendDutCommand("ls /mnt/hdd","zzz",5000))
		return false;

	if(!SendDutCommand("rmdir /mnt/hdd/zzz","#",5000))
		return false;

	if(SendDutCommand("ls /mnt/hdd","zzz",5000))
		return false;

	DisplayRunTimeInfo("---Check HDD OK!!!---");*/
	//end


	if(!SendDutCommand("fdisk -l","#",5000)) 
		return false;

	if( !strstr(g_data,"sda") || !strstr(g_data,"sda1") || !strstr(g_data,"sda2") || !strstr(g_data,"sda3") || !strstr(g_data,"sda4") ) 
	{	
		DisplayRunTimeInfo("HDD partition Wrong!!!");
		return false;
	}
	char * pTok,*pTok1;
	pTok=strstr(g_data.GetBuffer(),"Disk /dev/sda:");
	if(pTok==NULL)
	{
		return false;
	}
	pTok1=strstr(pTok," GB,");
	if(pTok1==NULL)
	{
		return false;
	}
	char szHDCapacity[20];
	memset(szHDCapacity,0,sizeof(szHDCapacity));
	memcpy_s(szHDCapacity,sizeof(szHDCapacity),pTok+14,pTok1-pTok-14);
	DisplayRunTimeInfo("HDD Capacity is:");
	DisplayRunTimeInfo(szHDCapacity);

	if(atof(szHDCapacity)>990 && atof(szHDCapacity)<1010)
	{
		DisplayRunTimeInfo("HDD Capacity is correct!");
	}
	else
	{
		DisplayRunTimeInfo("HDD Capacity is wrong!");
		return false;
	}

//move to RC for blance time
	//new hdd read & write test method
	/*DisplayRunTimeInfo("---Start check HDD Read & Write---");
	if(SendDutCommand("mount -t ext4 /dev/sda4 /mnt","#",10000))
	{
		if(SendDutCommand("cd /mnt","#",10000))
		{
			if(SendDutCommand("ls /mnt","lost+found",10000))
			{
				if(SendDutCommand("time dd if=/dev/zero bs=1024 count=50000 of=1Gb.file","#",10000))
				{
					if(SendDutCommand("time dd if=1Gb.file bs=64k | dd of=/dev/null","#",10000))
					{
						if(SendDutCommand("time dd if=1Gb.file of=2.Gb.file bs=64k","#",10000))
						{
							DisplayRunTimeInfo("---Check Write&Read action OK!!!---");
						}
						else
						{
							DisplayRunTimeInfo("HDD Write&Read action at the same time failed!");
							return false;
						}
					}
					else
					{
						DisplayRunTimeInfo("HDD Read action failed!");
						return false;
					}
				}
				else
				{
					DisplayRunTimeInfo("HDD Write action failed!");
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else 
		{
			return false;
		}

	}
	else
	{
		return false;
	}
	if(SendDutCommand("ls /mnt","1Gb.file",10000))
	{
			if(SendDutCommand("ls /mnt","2.Gb.file",10000))
			{
				if(SendDutCommand("rm 1Gb.file 2.Gb.file","#",10000))
				{
					DisplayRunTimeInfo("remove the temporary file OK!");
				}
				else
				{
					DisplayRunTimeInfo("remove the temporary file Fail!");
					return false;
				}
			}
			else
			{
				DisplayRunTimeInfo("No 2.Gb.file!");
				return false;
			}
	}
	else
	{
		DisplayRunTimeInfo("No 1Gb.file File!");
		return false;
	}
	if(SendDutCommand("ls /mnt","#",10000))
	{
		if(strstr(g_data,"1GB.file"))
		{
			DisplayRunTimeInfo("remove 1Gb.file file Fail!");
			return false;
		}
	}
	if(SendDutCommand("ls /mnt","#",10000))
	{
		if(strstr(g_data,"2.Gb.file"))
		{
			DisplayRunTimeInfo("remove 2.Gb.file file Fail!");
			return false;
		}
	}
	if(SendDutCommand("cd /","#",10000))
	{
		if(SendDutCommand("umount -l /mnt","#",10000))
		{
			DisplayRunTimeInfo("umount HDD OK!");
		}
		else
		{
			DisplayRunTimeInfo("umount HDD Fail!");
			return false;
		}
	}
	else
	{
		DisplayRunTimeInfo("cd / fail!");
		return false;
	}*/
	//end
//end

	

	//format HDD
	
	/*if(SendDutCommand("mkfs.ext4 /dev/sda4","Writing superblocks and filesystem accounting information",40000))
	{
		if(SendDutCommand("","done",10000))
		{
			DisplayRunTimeInfo("---Format HDD OK!!!---");
		}
		else
		{
			return false;
		}
	}
	else 
	{
		return false;
	}*/
	
	//end

	return true;
}

bool CRunInfo::DutCheckHDDInfo(void)
{
	/*
	true	:pass
	false	:failed
	*/

	//check hdd information
	DisplayRunTimeInfo("---Start check HDD Info---");

	if(!SendDutCommand("ls /dev","#",5000)) 
		return false;

	if( !strstr(g_data,"sda") || !strstr(g_data,"sda1") || !strstr(g_data,"sda2") || !strstr(g_data,"sda3") || !strstr(g_data,"sda4") ) 
	{	
		DisplayRunTimeInfo("HDD partition Wrong!!!");
		return false;
	}

	Sleep(1000);

	if(!SendDutCommand("fdisk -l","#",5000)) 
		return false;
	char * pTok,*pTok1;
	pTok=strstr(g_data.GetBuffer(),"Disk /dev/sda:");
	if(pTok==NULL)
	{
		return false;
	}
	pTok1=strstr(pTok," GB,");
	if(pTok1==NULL)
	{
		return false;
	}
	char szHDCapacity[20];
	memset(szHDCapacity,0,sizeof(szHDCapacity));
	memcpy_s(szHDCapacity,sizeof(szHDCapacity),pTok+14,pTok1-pTok-14);
	DisplayRunTimeInfo("HDD Capacity is:");
	DisplayRunTimeInfo(szHDCapacity);

	if(atof(szHDCapacity)>990 && atof(szHDCapacity)<1010)
	{
		DisplayRunTimeInfo("HDD Capacity is correct!");
	}
	else
	{
		DisplayRunTimeInfo("HDD Capacity is wrong!");
		return false;
	}

	//move to RC for blance time
	//new hdd read & write test method
	DisplayRunTimeInfo("---Start check HDD Read & Write---");

	/*if(!SendDutCommand("ls /dev","#",10000)) 
		return false;

	if(!strstr(g_data,"sda") || !strstr(g_data,"sda1") || !strstr(g_data,"sda2") || !strstr(g_data,"sda3") || !strstr(g_data,"sda4"))
	{
		DisplayRunTimeInfo("HDD partition Wrong!!!");
		return false;
	}*/

	if(SendDutCommand("mount -t ext4 /dev/sda4 /mnt","#",10000))
	{
		if(SendDutCommand("cd /mnt","#",10000))
		{
			if(SendDutCommand("ls /mnt","lost+found",10000))
			{
				if(SendDutCommand("time dd if=/dev/zero bs=1024 count=50000 of=1Gb.file","#",10000))
				{
					if(SendDutCommand("time dd if=1Gb.file bs=64k | dd of=/dev/null","#",10000))
					{
						if(SendDutCommand("time dd if=1Gb.file of=2.Gb.file bs=64k","#",10000))
						{
							DisplayRunTimeInfo("---Check Write&Read action OK!!!---");
						}
						else
						{
							DisplayRunTimeInfo("HDD Write&Read action at the same time failed!");
							return false;
						}
					}
					else
					{
						DisplayRunTimeInfo("HDD Read action failed!");
						return false;
					}
				}
				else
				{
					DisplayRunTimeInfo("HDD Write action failed!");
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else 
		{
			return false;
		}

	}
	else
	{
		return false;
	}
	//check read&write action and remove the temporary file in read&write action
	if(SendDutCommand("ls /mnt","1Gb.file",10000))
	{
			if(SendDutCommand("ls /mnt","2.Gb.file",10000))
			{
				if(SendDutCommand("rm 1Gb.file 2.Gb.file","#",10000))
				{
					DisplayRunTimeInfo("remove the temporary file OK!");
					//MessageBox("OK2");
				}
				else
				{
					DisplayRunTimeInfo("remove the temporary file Fail!");
					return false;
				}
			}
			else
			{
				DisplayRunTimeInfo("No 2.Gb.file!");
				return false;
			}
	}
	else
	{
		DisplayRunTimeInfo("No 1Gb.file File!");
		return false;
	}

	//check whether delete temporary file
	if(SendDutCommand("ls /mnt","#",10000))
	{
		if(strstr(g_data,"1GB.file"))
		{
			DisplayRunTimeInfo("remove 1Gb.file file Fail!");
			return false;
		}
	}
	if(SendDutCommand("ls /mnt","#",10000))
	{
		if(strstr(g_data,"2.Gb.file"))
		{
			DisplayRunTimeInfo("remove 2.Gb.file file Fail!");
			return false;
		}
	}

	//umount device which be mounted in /mnt folder,here /dev/sda4 is specified.
	/*if(SendDutCommand("cd /","#",10000))
	{
		if(SendDutCommand("umount -l /mnt","#",10000))
		{
			DisplayRunTimeInfo("umount HDD OK!");
		}
		else
		{
			DisplayRunTimeInfo("umount HDD Fail!");
			return false;
		}
	}
	else
	{
		DisplayRunTimeInfo("cd / fail!");
		return false;
	}*/
	SendDutCommand("cd /","#",10000);
	//end
	//end

	return true;
}

bool CRunInfo::Downgrade(void)
{
	DisplayRunTimeInfo("---Start down grade---");
	if(!SendDutCommand("ls /dev","sda1",5000))
		return false;

	if(SendDutCommand("mount -t vfat /dev/sda1 /mnt","#",10000))
	{

	}
	else 
	{
		return false;
	}

	if(SendDutCommand("cd /mnt","#",10000))
	{

	}
	else
	{
		return false;
	}

	if(SendDutCommand1("mtd write brcm4709_nor_0.1.99.bin firmware","#",50000))
	{
		
	}
	else
	{
		return false;
	}

	if(SendDutCommand("mtd erase board_data","#",10000))
	{
		
	}
	else
	{
		return false;
	}

	if(SendDutCommand("mtd write nvram_0.1.99.bin nvram","#",10000))
	{
		
	}
	else
	{
		return false;
	}

	//umount device which be mounted in /mnt folder,here /dev/sdb1 is specified.
	SendDutCommand("nvram set flag_package_update=2","#",10000);
	SendDutCommand("nvram set flag_try_sys1_failed=0","#",10000);
	SendDutCommand("nvram set flag_try_sys2_failed=0","#",10000);
	SendDutCommand("nvram set boot_wait=on","#",10000);
	SendDutCommand("nvram commit","#",10000);

	return true;
	/*if(SendDutCommand("umount -l /mnt","#",10000))
	{
		DisplayRunTimeInfo("umount USB OK!");
	}
	else
	{
		DisplayRunTimeInfo("umount USB Fail!");
		return false;
	}*/
}

bool CRunInfo::DutCheckUSB(void)
{
	DisplayRunTimeInfo("---Start check USB Info---");

	//if(!SendDutCommand("fdisk -l","#",5000)) 
	if(!SendDutCommand("cat /proc/partitions","#",5000)) 
		return false;

	char * pTok,*pTok1;
	pTok=strstr(g_data.GetBuffer(),"sda");
	if(pTok==NULL)
	{
		DisplayRunTimeInfo("Do not get USB!");
		return false;
	}
	//pTok1=strstr(pTok," MB,");
	//if(pTok1==NULL)
	//{
	//	return false;
	//}
	//char szUSBCapacity[20];
	//memset(szUSBCapacity,0,sizeof(szUSBCapacity));
	//memcpy_s(szUSBCapacity,sizeof(szUSBCapacity),pTok+14,pTok1-pTok-14);
	//DisplayRunTimeInfo("USB Capacity is:");
	//DisplayRunTimeInfo(szUSBCapacity);

	//if(atof(szUSBCapacity)>990)
	//{
	//	DisplayRunTimeInfo("USB Capacity is correct!");
	//}
	//else
	//{
	//	DisplayRunTimeInfo("USB Capacity is wrong!");
	//	return false;
	//}

	//if( (!strstr(g_data.GetBuffer(),"sdb1")) ) 
	//{	
	//	DisplayRunTimeInfo("USB partition have problem!");
	//	return false;
	//}

	/*if(SendDutCommand("mount -t ext4 /dev/sdb1 /mnt","#",10000))
	{

	}
	else
	{
		return false;
	}*/
	//if(SendDutCommand("cd /extdisks/sdb1","#",10000))
	//{
		//if(SendDutCommand("ls /extdisks/sdb1","#",10000))
	if(SendDutCommand("mount /dev/sda1 /mnt","#",10000))
		{
			if(SendDutCommand(" time dd if=/dev/zero bs=1024 count=10000 of=/mnt/1Gb.file","#",10000))
			{
				if(strstr(g_data.GetBuffer(),"10000+0 records in"))
				{

				}
				else
				{
				DisplayRunTimeInfo("USB Write action failed!");
				return false;
			    }

				//if(SendDutCommand("time dd if=1Gb.file bs=64k | dd of=/dev/null","#",10000))

				//{
					if(SendDutCommand("time dd if=/mnt/1Gb.file of=/mnt/2.Gb.file bs=64k","#",10000))
					{
						if(strstr(g_data.GetBuffer(),"records in"))
						{

						}
						else
						{
						DisplayRunTimeInfo("USB Write&Read action at the same time failed!");
						return false;
						}
						DisplayRunTimeInfo("---Check USB Write&Read action OK!!!---");
					}
					else
					{
						DisplayRunTimeInfo("USB Write&Read action at the same time failed!");
						return false;
					}
				//}
				//else
				//{
				//	DisplayRunTimeInfo("USB Read action failed!");
				//	return false;
				//}
			}
			else
			{
				DisplayRunTimeInfo("USB Write action failed!");
				return false;
			}
		}
		else
		{
			return false;
		}
	


	if(!SendDutCommand("umount /mnt/","#",10000))
	{
		DisplayRunTimeInfo("remove the USB fail!");
		return false;
		//MessageBox("OK2");
	}
	//else 
	//{
	//	return false;
	//}


	////check read&write action and remove the temporary file in read&write action
	//if(SendDutCommand("ls /extdisks/sdb1","1Gb.file",10000))
	//{
	//		if(SendDutCommand("ls /extdisks/sdb1","2.Gb.file",10000))
	//		{
	//			if(SendDutCommand("rm 1Gb.file 2.Gb.file","#",10000))
	//			{
	//				DisplayRunTimeInfo("remove the temporary file action OK!");
	//				//MessageBox("OK2");
	//			}
	//			else
	//			{
	//				DisplayRunTimeInfo("remove the temporary file Fail!");
	//				return false;
	//			}
	//		}
	//		else
	//		{
	//			DisplayRunTimeInfo("No 2.Gb.file!");
	//			return false;
	//		}
	//}
	//else
	//{
	//	DisplayRunTimeInfo("No 1Gb.file File!");
	//	return false;
	//}

	////check whether delete temporary file
	//if(SendDutCommand("ls /extdisks/sdb1","#",10000))
	//{
	//	if(strstr(g_data,"1Gb.file"))
	//	{
	//		DisplayRunTimeInfo("remove 1Gb.file file Fail!");
	//		return false;
	//	}
	//}
	//if(SendDutCommand("ls /extdisks/sdb1","#",10000))
	//{
	//	if(strstr(g_data,"2.GB.file"))
	//	{
	//		DisplayRunTimeInfo("remove 2.Gb.file file Fail!");
	//		return false;
	//	}
	//}

	////umount device which be mounted in /mnt folder,here /dev/sdb1 is specified.
	//SendDutCommand("cd /","#",10000);

	return true;
	/*if(SendDutCommand("umount -l /mnt","#",10000))
	{
		DisplayRunTimeInfo("umount USB OK!");
	}
	else
	{
		DisplayRunTimeInfo("umount USB Fail!");
		return false;
	}*/
}

bool CRunInfo::DutCheckHDDANDUSBInfo(void)
{
	/*
	true	:pass
	false	:failed
	*/

	//check hdd and usb information
	DisplayRunTimeInfo("---Start check HDD Info---");

	if(!SendDutCommand("fdisk -l","Disk /dev/sda:",5000)) 
		return false;

	Sleep(1000);
	char * pTok,*pTok1;
	pTok=strstr(g_data.GetBuffer(),"Disk /dev/sda:");
	pTok1=strstr(pTok," GB,");
	char szHDCapacity[20];
	memset(szHDCapacity,0,sizeof(szHDCapacity));
	memcpy_s(szHDCapacity,sizeof(szHDCapacity),pTok+14,pTok1-pTok-14);
	DisplayRunTimeInfo("HDD Capacity is:");
	DisplayRunTimeInfo(szHDCapacity);

	if(atof(szHDCapacity)>990 && atof(szHDCapacity)<1010)
	{
		DisplayRunTimeInfo("HDD Capacity is correct!");
	}
	else
	{
		DisplayRunTimeInfo("HDD Capacity is wrong!");
		return false;
	}

	if( (!strstr(g_data.GetBuffer(),"sda1")) || (!strstr(g_data.GetBuffer(),"sda2")) || (!strstr(g_data.GetBuffer(),"sda3")) || (!strstr(g_data.GetBuffer(),"sda4")) ) 
	{	
		DisplayRunTimeInfo("HDD partition have problem!");
		return false;
	}

	pTok=strstr(g_data.GetBuffer(),"Disk /dev/sdb:");
	if(pTok==NULL)
	{
		return false;
	}
	pTok1=strstr(pTok," MB,");
	if(pTok1==NULL)
	{
		return false;
	}
	char szUSBCapacity[20];
	memset(szUSBCapacity,0,sizeof(szUSBCapacity));
	memcpy_s(szUSBCapacity,sizeof(szUSBCapacity),pTok+14,pTok1-pTok-14);
	DisplayRunTimeInfo("USB Capacity is:");
	DisplayRunTimeInfo(szUSBCapacity);

	if(atof(szUSBCapacity)>1024)
	{
		DisplayRunTimeInfo("USB Capacity is correct!");
	}
	else
	{
		DisplayRunTimeInfo("USB Capacity is wrong!");
		return false;
	}

	if( (!strstr(g_data.GetBuffer(),"sdb1")) ) 
	{	
		DisplayRunTimeInfo("USB partition have problem!");
		return false;
	}



	return true;
}


bool CRunInfo::DutCheckMAC(void)
{
	/*
	true	:pass
	false	:false
	*/

	//if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	//{
	//	DisplayRunTimeInfo("SFIS OFF,do not check!!!");
	//	return true;
	//}

	DisplayRunTimeInfo("---Start check Ethernet & 2.4G & 5G MAC---");

	
	//////////////get eth wl0 wl1 mac///////////////////sheng-zhao for R1C 20140313


	char FileLineBuf[512]="";
	char *pToke=NULL;
	char tempbuf_MAC[MINBUFSIZE]  = "";
	int MAC_num=0;
	
		
	ifstream fin("C:\\R1C_MAC.txt", ios::in); 
		
		while(fin.getline(FileLineBuf, sizeof(FileLineBuf))) 
		{ 
		  if(strstr(FileLineBuf, "NEED_WRITE_MAC"))
			Write_mac_flag=1;

		  if(Write_mac_flag==1)
		  {
			if(pToke=strstr(FileLineBuf, "ETH_MAC:"))
			{
				//gTI.TestStaInfo.IQSN = pToke+strlen("Tester 1 SN:	");
				//strcpy_s(m_strET0MAC,sizeof(m_strET0MAC),pToke+strlen("ETH_MAC:"));
				m_strET0MAC=pToke+strlen("ETH_MAC:");
				sprintf_s(tempbuf_MAC,MINBUFSIZE,"ETH_MAC:%s",m_strET0MAC);
				DisplayRunTimeInfo(tempbuf_MAC);
				MAC_num++;
			}
			
			if(pToke=strstr(FileLineBuf, "WL1_MAC:"))
			{
				//gTI.TestStaInfo.IQSN = pToke+strlen("Tester 1 SN:	");
				//strcpy_s(m_str2GMAC,sizeof(m_str2GMAC),pToke+strlen("WL1_MAC:"));
				m_str2GMAC=pToke+strlen("WL1_MAC:");
				sprintf_s(tempbuf_MAC,MINBUFSIZE,"WL1_MAC:%s",m_str2GMAC);
				DisplayRunTimeInfo(tempbuf_MAC);
				MAC_num++;
			}

			if(pToke=strstr(FileLineBuf, "WL0_MAC:"))
			{
				//gTI.TestStaInfo.IQSN = pToke+strlen("Tester 1 SN:	");
				//strcpy_s(m_str5GMAC,sizeof(m_str5GMAC),pToke+strlen("WL0_MAC:"));
				m_str5GMAC=pToke+strlen("WL0_MAC:");
				sprintf_s(tempbuf_MAC,MINBUFSIZE,"WL0_MAC:%s",m_str5GMAC);
				DisplayRunTimeInfo(tempbuf_MAC);	
				MAC_num++;
			}
		 }

		} 

		fin.clear();
		fin.close();
		if(Write_mac_flag!=1)
		{
			DisplayRunTimeInfo("Do not need check mac!");
			return true;
		}
		if(MAC_num!=3)
		{
		  DisplayRunTimeInfo("Get mac fail from C:\\R1C_MAC.txt");
          return false;
		}
	
		//////////////////////////////////////////////
	
	char szEt0mac[50];
	char sz2Gmac[50];
	char sz5Gmac[50];
	memset(szEt0mac,0,50);
	memset(sz2Gmac,0,50);
	memset(sz5Gmac,0,50);

	for(int i=0;i<6;i++)
	{
		char buf[50]="";
		memcpy_s(buf,50,m_strET0MAC.GetBuffer()+i*2,2);
		strcat_s(szEt0mac,50,buf);
		if(i<5)
		{
			strcat_s(szEt0mac,50,":");
		}
	}

	for(int i=0;i<6;i++)
	{
		char buf[50]="";
		memcpy_s(buf,50,m_str2GMAC.GetBuffer()+i*2,2);
		strcat_s(sz2Gmac,50,buf);
		if(i<5)
		{
			strcat_s(sz2Gmac,50,":");
		}
	}

	for(int i=0;i<6;i++)
	{
		char buf[50]="";
		memcpy_s(buf,50,m_str5GMAC.GetBuffer()+i*2,2);
		strcat_s(sz5Gmac,50,buf);
		if(i<5)
		{
			strcat_s(sz5Gmac,50,":");
		}
	}
	DisplayRunTimeInfo(szEt0mac);
	DisplayRunTimeInfo(sz2Gmac);
	DisplayRunTimeInfo(sz5Gmac);

	//printf("after convert,buf is %s;\n",buf1);

	//if(!SendDutCommand("bdata get et0macaddr","#",5000)) 
	if(!SendDutCommand("ifconfig eth0","#",5000)) 
		return false;

	if(strstr(g_data,szEt0mac))
	{
		DisplayRunTimeInfo("Check Ethernet MAC OK!");
	}
	else
	{
		DisplayRunTimeInfo("Check Ethernet MAC Fail!");
		return false;
	}

	if(!SendDutCommand("ifconfig wl1","#",5000)) 
		return false;

	if(strstr(g_data,sz2Gmac))
	{
		DisplayRunTimeInfo("Check 2G MAC OK!");
	}
	else
	{
		DisplayRunTimeInfo("Check 2G MAC Fail!");
		return false;
	}


	if(!SendDutCommand("ifconfig wl0","#",5000)) 
		return false;

	if(strstr(g_data,sz5Gmac))
	{
		DisplayRunTimeInfo("Check 5G MAC OK!");
	}
	else
	{
		DisplayRunTimeInfo("Check 5G MAC Fail!");
		return false;
	}


	/*char    Seps[]     = "-\n";
	char    *token,*DelSpaceToken;
	char    szMACFromDUT[MINBUFSIZE] = "";
	char    *Context ;
	token = strtok_s(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()),Seps,&Context);
	while(token!=NULL)
	{
		token = _tcstok_s(NULL,Seps,&Context);
		if(strcmp(token,"WAN mac address ") == 0)
		{
			token = strtok_s(NULL, Seps, &Context);
			char subSeps[] = " ";
			DelSpaceToken = strtok_s(token,subSeps,&Context);
			strncpy_s(szMACFromDUT, MINBUFSIZE, DelSpaceToken, 12);
			break;
		}
	}

	__int64  lgGetWMACFromDUT = CovertMacToNumber(szMACFromDUT, (int)strlen(szMACFromDUT));
	if(lgGetWMACFromDUT != m_nWanMAC)
	{
		return false;
	}*/
	return true;
}

bool CRunInfo::DutCheckSSID(void)
{
	/*
	true	:pass
	false	:false
	*/

	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		DisplayRunTimeInfo("SFIS OFF,do not check!!!");
		return true;
	}

	DisplayRunTimeInfo("---Start check 2.4G & 5G SSID---");

	if(!SendDutCommand("bdata get wl1_ssid","#",5000)) 
		return false;

	if(strstr(g_data,m_str2GSSID.Trim()))
	{
		DisplayRunTimeInfo("Check 2G SSID OK!");
	}
	else
	{
		DisplayRunTimeInfo("Check 2G SSID Fail!");
		return false;
	}

	if(!SendDutCommand("bdata get wl0_ssid","#",5000)) 
		return false;

	if(strstr(g_data,m_str5GSSID.Trim()))
	{
		DisplayRunTimeInfo("Check 5G SSID OK!");
	}
	else
	{
		DisplayRunTimeInfo("Check 5G SSID Fail!");
		return false;
	}

	return true;
}

bool CRunInfo::DutCheckPin(void)
{
	/*
	true	:pass
	false	:failed
	*/
	DisplayRunTimeInfo("---Start check pincode---");

	if(!SendDutCommand("burnpin","#",5000)) return false;

	char    SepsPin[] = "-\n";
	char    *tokenPin;
	char    *tokenPin2;
	char    szPincodeFromDUT[MINBUFSIZE] = "";
	char    *ContextPin ;
	tokenPin  = strtok_s(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()), SepsPin, &ContextPin);
	while(tokenPin != NULL)
	{
		tokenPin = strtok_s(NULL, SepsPin, &ContextPin);
		if(strcmp(tokenPin, "WSC PIN ") == 0)
		{
			char SepsPin2[] = " ";
			tokenPin = strtok_s(NULL, SepsPin, &ContextPin);
			tokenPin2 = strtok_s(tokenPin, SepsPin2, &ContextPin);
			strncpy_s(szPincodeFromDUT, MINBUFSIZE, tokenPin2, 8);
			break;
		}
	}
	if(strcmp(szPincodeFromDUT, m_strPincode.Trim()) == 0)
	{
		return true;
	}
	return false;
}
bool CRunInfo::DutCheckSn(void)
{
	/*
	true	:pass
	false	:failed
	*/

	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		DisplayRunTimeInfo("SFIS OFF,do not check!!!");
		return true;
	}

	DisplayRunTimeInfo("---Start check SSN---");

	if(!SendDutCommand("bdata get SN","#",5000)) 
		return false;	

	if(strstr(g_data,m_strSN.Trim()))
	{
		DisplayRunTimeInfo("Check SN OK!");
		return true;
	}

	//experiment
	/*CString a="123";
	char b[50];
	memset(b,0,50);
	strcpy_s(b,50,"123");

	if(!SendDutCommand("bdata get SN","#",5000)) 
		return false;	

	if(strstr(g_data.GetBuffer(),a.GetBuffer()))
	{
		DisplayRunTimeInfo("Check a SN OK!");
	}
	else
	{
		DisplayRunTimeInfo("Check a SN Fail!");
		return false;
	}

	if(strstr(g_data,b))
	{
		DisplayRunTimeInfo("Check b SN OK!");
		return true;
	}
	else
	{
		DisplayRunTimeInfo("Check b SN Fail!");
		return false;
	}*/
	//end


	/*char    SepsCN[] = "-\n";
	char    *tokenCN;
	char    *tokenCN2;
	char    szSNFromDUT[MINBUFSIZE] = "";
	char    *ContextCN ;
	tokenCN  = strtok_s(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()), SepsCN, &ContextCN);
	while(tokenCN != NULL)
	{
		tokenCN = strtok_s(NULL, SepsCN, &ContextCN);
		if(strcmp(tokenCN, "serial number ") == 0)
		{
			char SepsCN2[] = " ";
			tokenCN = strtok_s(NULL, SepsCN, &ContextCN);
			tokenCN2 = strtok_s(tokenCN, SepsCN2, &ContextCN);
			strncpy_s(szSNFromDUT, MINBUFSIZE, tokenCN2, 13);
			break;
		}
	}
	if(strcmp(szSNFromDUT, m_strSN.Trim()) == 0)
	{		
		return true;
	}*/
	DisplayRunTimeInfo("Check SN Fail!");
	return false;
}

bool CRunInfo::DutCheckModel(void)
{
	/*
	true	:pass
	false	:failed
	*/

	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		DisplayRunTimeInfo("SFIS OFF,do not check!!!");
		return true;
	}

	DisplayRunTimeInfo("---Start check Model---");

	if(!SendDutCommand("bdata get model","#",5000)) 
		return false;	

	if(strstr(g_data,"R1D"))
	{
		DisplayRunTimeInfo("Check Model OK!");
		return true;
	}
	DisplayRunTimeInfo("Check Model Fail!");
	return false;
}

bool CRunInfo::DutCheckRegion(void)
{
	/*
	true	:pass
	false	:failed
	*/
	DisplayRunTimeInfo("---Start check region code---");
	if( 0 == m_IniFileInfo.nCheckSumFlag )
	{
		DisplayRunTimeInfo("Don't need check sku region code!");
		return true;
	}
	if(!SendDutCommand("burnsku","#",8000)) return false;

	char    SepsCN[] = "-\n";
	char    *tokenCN;
	char    *tokenCN2;
	char    szCNFromDUT[MINBUFSIZE] = "";
	char    *ContextCN ;
	tokenCN  = strtok_s(m_strStoreEthernetData.GetBuffer(m_strStoreEthernetData.GetLength()), SepsCN, &ContextCN);
	while(tokenCN != NULL)
	{
		tokenCN = strtok_s(NULL, SepsCN, &ContextCN);
		if(strcmp(tokenCN, "region_num ") == 0)
		{
			char SepsCN2[] = " ";
			tokenCN = strtok_s(NULL, SepsCN, &ContextCN);
			tokenCN2 = strtok_s(tokenCN, SepsCN2, &ContextCN);
			strncpy_s(szCNFromDUT, MINBUFSIZE, tokenCN2, 6);
			break;
		}
	}
	if(strcmp(szCNFromDUT, m_IniFileInfo.szRegionCode) == 0)
	{
		return true;
	}
	return false;
}
bool CRunInfo::DutDisableWireless(void)
{
	/*
	true	:pass
	false	:failed
	*/

	if(!GetIsSfis() /*|| (0 != strcmp(m_IniFileInfo.szAllowBurn, "1"))*/)
	{
		return true;
	}

	if(strstr(m_IniFileInfo.szDisableWireless , "1") != NULL)
	{
		ShowMsg("***********Disable Wireless***************");
		if(!SendDutCommand("wlconf wl0 down","#",5000))
			return false;
		if(!SendDutCommand("wlconf wl1 down","#",5000))
			return false;
		if(!SendDutCommand("nvram set wl0_radio=0","#",5000))
			return false;
		if(!SendDutCommand("nvram set wl1_radio=0","#",5000))
			return false;
		if(!SendDutCommand("nvram set boot_wait=off","#",5000))
			return false;
		if(!SendDutCommand("nvram commit","#",5000))
			return false;
		Sleep(3000);
	}
	else
	{
		return true;
	}
	return true;
}
bool CRunInfo::DutPot(void)
{
	/*
	true	:pass
	false	:failed
	*/
	ShowMsg("***********Pot Time***************");
	//pot stop
	if(!SendDutCommand("pot stop 1","Stop done",5000))
	{
		return false;
	}

	//pot erase
	if(!SendDutCommand("pot doerase","Erase done",5000))
	{
		return false;
	}
	/*End*/
	return true;
}

bool CRunInfo::DutLoadDefault(void)
{
	//if(!GetIsSfis())
	//{
	//	return true;
	//}
///////////////////////////////////////////////////////////////
	DisplayRunTimeInfo("---Start dump rom file---");
	if(!SendDutCommand("hexdump -C /dev/mtd3","#",5000)) //dump R1C ROM FILE--sheng zhao 20140414
	{
		DisplayRunTimeInfo("dump R1C ROM FILE FAIL");
		return false;
	}
/////////////////////////////////////////////////////////////////////

	ShowMsg("***********Load Default**************");
	if(SendDutCommand("nvram set restore_defaults=1","#",8000))
	{
		if(SendDutCommand("nvram commit","#",8000))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CRunInfo::DutLoadDefault_2(void)
{
	ShowMsg("***********Load Default**************");
	if(SendDutCommand("nvram set restore_defaults=1","#",8000))
	{
		if(SendDutCommand("nvram commit","#",8000))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/*just use for H163*/
bool CRunInfo::PingSpecifyIPFail(char* IP, int nSuccessCount)
{
	HANDLE hWritePipe  = NULL;
	HANDLE hReadPipe   = NULL;

	char  szPing[MINBUFSIZE] = "";
	sprintf_s(szPing, MINBUFSIZE, "ping.exe %s -n 15 -w 100", IP);
	char   szReadFromPipeData[MAXBUFSIZE] = "";
	DWORD  byteRead    = 0;

	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle =true;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);

	if(!CreatePipe(&hReadPipe,&hWritePipe,&sa,0))
	{
		DisplayRunTimeInfo("Create pipe fail");
		return 0;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO        si;
	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdError  = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow=SW_HIDE;

	if(!CreateProcess(NULL,szPing,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
	{
		DisplayRunTimeInfo("Start run ping.exe fail!");
		return 0;
	}

	CloseHandle(pi.hThread);
	CloseHandle(hWritePipe);
	CloseHandle(pi.hProcess);

	DWORD  dwPingFailCount    = nSuccessCount;
	DWORD  dwPingSuccessCount = 2;
	
	while(TRUE)
	{
		memset(szReadFromPipeData,0,MAXBUFSIZE);
		Sleep(100);
		int bResult = ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
		
		if(!bResult)
		{
			DisplayRunTimeInfo("Read ping.exe fail!");
			return 0;
		}

		Sleep(200);
		char IPInfo[MINBUFSIZE] ="";
		sprintf_s(IPInfo, MINBUFSIZE, "Reply from %s", IP);
		if(!strstr(szReadFromPipeData,IPInfo))
		{
			dwPingSuccessCount--;
		}
		else
		{
			dwPingFailCount--;
		}
		DisplayRunTimeInfo(szReadFromPipeData);
		if(!dwPingSuccessCount)
		{
			ReadFile(hReadPipe,szReadFromPipeData,MAXBUFSIZE,&byteRead,NULL);
			DisplayRunTimeInfo(szReadFromPipeData);
			return 1;
		}
		if(!dwPingFailCount)
			return 0;
	}
}

bool CRunInfo::DutCheckPowerButton(void)
{
	/*if PowerButton press,DUT will turn off*/
	/*button 1*/
	bool blPassY = false;
	for(int i = 0; i <3; i++)
	{
		if(IDOK == AfxMessageBox("請按一下POWER按鈕,持續一秒鐘!!!"))
		{
			if((PingSpecifyIPFail(m_IniFileInfo.szDefaultIP1,m_IniFileInfo.nPingDUTCount)) || (PingSpecifyIPFail(m_IniFileInfo.szDefaultIP2,m_IniFileInfo.nPingDUTCount)))
			{
				blPassY = true;
				break;
			}
		}
	}
	if(!blPassY)
	{
		return false;
	}
	/*End*/	

	return true;
}

UINT __cdecl AutoPressButton(LPVOID pParam)
{
	_ButtonInfo* pbtn = (_ButtonInfo*) pParam ;
	HWND m_hwndSG = NULL;
	int bWait = 300;
	while(::FindWindow(NULL,pbtn->szWindowName)==NULL)
	{
		bWait--;
		::Sleep(100);
		if(::FindWindow(NULL,pbtn->szWindowName)!=NULL)
			break;
	}
	while(::FindWindow(NULL,pbtn->szWindowName) != NULL)
	{
		m_hwndSG = ::FindWindow(NULL,pbtn->szWindowName);
		bWait--;
		::Sleep(100);
		if(m_hwndSG!=NULL)
		{
			if(strstr(g_data.GetBuffer(g_data.GetLength()), pbtn->szReturnInfo) != NULL)
			{
				::PostMessage(m_hwndSG,WM_CLOSE,0,0);
				pbtn->bTestOK = true;
				break;
			}
		}
		else
		{
			return 0;
		}
		if(bWait<1)
		{	
			::PostMessage(m_hwndSG,WM_CLOSE,0,0);
			return 0;
		}
	}
	return 1;
}

bool CRunInfo::CheckNFCTagCard()
{
	if(1==OnInit())
	{
		ShowMsg("init NFC Reader OK!\n");
	}
	else
	{
		ShowMsg("init NFC Reader Failed!\n");
		return 0;
	}

	if(1==OnConn())
	{
		ShowMsg("connect to Tag Card OK!\n");
		return 1;
	}
	else
	{
		ShowMsg("connect to Tag Card Failed!");
		return 0;
	}
}

bool CRunInfo::CheckDutBootUp()
{
	/*
	true	:pass
	false	:failed
			IN30
			in10
			SY25
			IN20
	*/
	int iRetryNum = 4;
	char szMAC[128];
	char szDutIp[128]="";
	memset( szMAC , 0 , sizeof(szMAC));

	//ShowMsg("Check ethernet cable connection ... ");
	//while(iRetryNum--)
	//{
	//	if(IP_Exits(m_IniFileInfo.szStoreFixIPAddress))
	//		break;
	//	else
	//	{
	//		AfxMessageBox("請插入網線!Please insert Ethernet Cable!");
	//		Sleep(3000);
	//		if(iRetryNum == 0)
	//		{
	//			SendSfisResultAndShowFail(IN30);//fail
	//			return false;
	//		}
	//	}
	//}

	iRetryNum=5;
	while(iRetryNum--)
	{
		ShowMsg("Check network connection ... ");
		if(IP_Ping(m_IniFileInfo.szDefaultIP1 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP1);
			break;
		}
		else if(IP_Ping(m_IniFileInfo.szDefaultIP2 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP2);
			break;
		}
		else
		{
			// reserved
		}
		if(iRetryNum == 0)
		{
			SendSfisResultAndShowFail(IN10);//Telnet fail
			return false;
		}
	}

	DeleteSpecifyExe("ping.exe");

	ShowMsg(CString("DUT_IP: ")+CString(szDutIp));
	ShowMsg("Get DUT Mac Address ... ");
	if(!IP_ArpMac(szDutIp , szMAC ))
	{
		SendSfisResultAndShowFail(SY25);//arp -a fail
		return false;
	}
	else
	{
		ShowMsg(CString("MAC: ")+CString(szMAC));
	}

	iRetryNum=15;//modify for Xiaomi,before is 5
	while(iRetryNum--)
	{
		/*
		ShowMsg("Run telnetenable start ... ");
		if(!IP_TelnetEnable(szDutIp , szMAC ))
		{
			continue;
		}
		*/
		Sleep(1000);
		ShowMsg("Run telnet start ... ");
		if(DutSocketConnection(szDutIp))
		{
			break;
		}
		else
		{
			Sleep(500);
		}
	}
	if(iRetryNum == 0)
	{
		SendSfisResultAndShowFail(IN20);//Telnet fail
		return false;
	}
	//m_strRecordTestData  +=  "12312312312312312";

	return true;
}

bool CRunInfo::CheckDutBootUp_2()
{
	/*
	true	:pass
	false	:failed
			IN30
			in10
			SY25
			IN20
	*/
	int iRetryNum = 4;
	char szMAC[128];
	char szDutIp[128]="";
	memset( szMAC , 0 , sizeof(szMAC));

	//ShowMsg("Check ethernet cable connection ... ");
	//while(iRetryNum--)
	//{
	//	if(IP_Exits(m_IniFileInfo.szStoreFixIPAddress))
	//		break;
	//	else
	//	{
	//		AfxMessageBox("請插入網線!Please insert Ethernet Cable!");
	//		Sleep(3000);
	//		if(iRetryNum == 0)
	//		{
	//			SendSfisResultAndShowFail(IN30);//fail
	//			return false;
	//		}
	//	}
	//}

	iRetryNum=5;
	while(iRetryNum--)
	{
		ShowMsg("Check network connection ... ");
		if(IP_Ping(m_IniFileInfo.szDefaultIP1 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP1);
			break;
		}
		else if(IP_Ping(m_IniFileInfo.szDefaultIP2 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP2);
			break;
		}
		else
		{
			// reserved
		}
		if(iRetryNum == 0)
		{
			SendSfisResultAndShowFail(IN10);//Telnet fail
			return false;
		}
	}

	DeleteSpecifyExe("ping.exe");

	ShowMsg(CString("DUT_IP: ")+CString(szDutIp));
	ShowMsg("Get DUT Mac Address ... ");
	if(!IP_ArpMac(szDutIp , szMAC ))
	{
		SendSfisResultAndShowFail(SY25);//arp -a fail
		return false;
	}
	else
	{
		ShowMsg(CString("MAC: ")+CString(szMAC));
	}

	iRetryNum=3;//modify for Xiaomi,before is 5
	while(iRetryNum--)
	{
		/*
		ShowMsg("Run telnetenable start ... ");
		if(!IP_TelnetEnable(szDutIp , szMAC ))
		{
			continue;
		}
		*/
		Sleep(1000);
		ShowMsg("Run telnet start ... ");
		if(DutSocketConnection(szDutIp))
		{
			break;
		}
		else
		{
			Sleep(500);
		}

		if(iRetryNum <= 1)
		{
			SendSfisResultAndShowFail(IN20);//Telnet fail
			return false;
		}
	}

	//m_strRecordTestData  +=  "12312312312312312";

	return true;
}

bool CRunInfo::PingLanIP()
{
	/*
	true	:pass
	false	:failed
			IN30
			in10
			SY25
			IN20
	*/
	int iRetryNum = 4;
	char szMAC[128];
	char szDutIp[128]="";
	memset( szMAC , 0 , sizeof(szMAC));

	//ShowMsg("Check ethernet cable connection ... ");
	//while(iRetryNum--)
	//{
	//	if(IP_Exits(m_IniFileInfo.szStoreFixIPAddress))
	//		break;
	//	else
	//	{
	//		AfxMessageBox("請插入網線!Please insert Ethernet Cable!");
	//		Sleep(3000);
	//		if(iRetryNum == 0)
	//		{
	//			SendSfisResultAndShowFail(IN30);//fail
	//			return false;
	//		}
	//	}
	//}

	iRetryNum=3;
	while(iRetryNum--)
	{
		ShowMsg("Check network connection ... ");
		if(IP_Ping(m_IniFileInfo.szDefaultIP1 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP1);
			ShowMsg("Ping Lan IP OK ... ");
			break;
		}
		else if(IP_Ping(m_IniFileInfo.szDefaultIP2 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP2);
			ShowMsg("Ping Lan IP OK ... ");
			break;
		}
		else
		{
			// reserved
		}
		if(iRetryNum == 0)
		{
			SendSfisResultAndShowFail(IN10);//Telnet fail
			return false;
		}
	}

	DeleteSpecifyExe("ping.exe");
	//m_strRecordTestData  +=  "12312312312312312";

	return true;
}

bool CRunInfo::TelnetwithUserPassword()
{
	/*
	true	:pass
	false	:failed
			IN30
			in10
			SY25
			IN20
	*/
	int iRetryNum = 4;
	char szMAC[128];
	char szDutIp[128]="";
	memset( szMAC , 0 , sizeof(szMAC));

	//ShowMsg("Check ethernet cable connection ... ");
	//while(iRetryNum--)
	//{
	//	if(IP_Exits(m_IniFileInfo.szStoreFixIPAddress))
	//		break;
	//	else
	//	{
	//		AfxMessageBox("請插入網線!Please insert Ethernet Cable!");
	//		Sleep(3000);
	//		if(iRetryNum == 0)
	//		{
	//			SendSfisResultAndShowFail(IN30);//fail
	//			return false;
	//		}
	//	}
	//}
	ShowMsg("Get DUT Mac Address ... ");
	if(!IP_ArpMac(szDutIp , szMAC ))
	{
		SendSfisResultAndShowFail(SY25);//arp -a fail
		return false;
	}
	else
	{
		ShowMsg(CString("MAC: ")+CString(szMAC));
	}

	iRetryNum=15;//modify for Xiaomi,before is 5
	while(iRetryNum--)
	{
		/*
		ShowMsg("Run telnetenable start ... ");
		if(!IP_TelnetEnable(szDutIp , szMAC ))
		{
			continue;
		}
		*/
		Sleep(1000);
		ShowMsg("Run telnet start ... ");
		if(DutSocketConnection(szDutIp))
		{
			break;
		}
		else
		{
			Sleep(500);
		}
	}
	if(iRetryNum == 0)
	{
		SendSfisResultAndShowFail(IN20);//Telnet fail
		return false;
	}
	//m_strRecordTestData  +=  "12312312312312312";

	return true;
}

bool CRunInfo::TelnetwithDefaultPassword()
{
	/*
	true	:pass
	false	:failed
			IN30
			in10
			SY25
			IN20
	*/
	int iRetryNum = 4;
	char szMAC[128];
	char szDutIp[128]="";
	memset( szMAC , 0 , sizeof(szMAC));

	//ShowMsg("Check ethernet cable connection ... ");
	//while(iRetryNum--)
	//{
	//	if(IP_Exits(m_IniFileInfo.szStoreFixIPAddress))
	//		break;
	//	else
	//	{
	//		AfxMessageBox("請插入網線!Please insert Ethernet Cable!");
	//		Sleep(3000);
	//		if(iRetryNum == 0)
	//		{
	//			SendSfisResultAndShowFail(IN30);//fail
	//			return false;
	//		}
	//	}
	//}

	iRetryNum=5;
	while(iRetryNum--)
	{
		ShowMsg("Check network connection ... ");
		if(IP_Ping(m_IniFileInfo.szDefaultIP1 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP1);
			break;
		}
		else if(IP_Ping(m_IniFileInfo.szDefaultIP2 , 2 , 64 ))
		{
			strcpy_s(szDutIp,sizeof(szDutIp)-1,m_IniFileInfo.szDefaultIP2);
			break;
		}
		else
		{
			// reserved
		}
		if(iRetryNum == 0)
		{
			SendSfisResultAndShowFail(IN10);//Telnet fail
			return false;
		}
	}

	DeleteSpecifyExe("ping.exe");

	ShowMsg(CString("DUT_IP: ")+CString(szDutIp));
	ShowMsg("Get DUT Mac Address ... ");
	if(!IP_ArpMac(szDutIp , szMAC ))
	{
		SendSfisResultAndShowFail(SY25);//arp -a fail
		return false;
	}
	else
	{
		ShowMsg(CString("MAC: ")+CString(szMAC));
	}

	iRetryNum=15;//modify for Xiaomi,before is 5
	while(iRetryNum--)
	{
		/*
		ShowMsg("Run telnetenable start ... ");
		if(!IP_TelnetEnable(szDutIp , szMAC ))
		{
			continue;
		}
		*/
		Sleep(1000);
		ShowMsg("Run telnet start ... ");
		if(DutSocketConnection(szDutIp))
		{
			break;
		}
		else
		{
			Sleep(500);
		}
	}
	if(iRetryNum == 0)
	{
		SendSfisResultAndShowFail(IN20);//Telnet fail
		return false;
	}
	//m_strRecordTestData  +=  "12312312312312312";

	return true;
}

bool CRunInfo::DutTestAdsl()
{
	if(!strcmp(m_IniFileInfo.szStoreTestAdsl, "1"))
	{
		ShowMsg("----------Begin ADSL test-------");
		bool   bFlagDSL = false;
		char   szDBRValue[MINBUFSIZE] = "" ;
		char   szUBRValue[MINBUFSIZE] = "";
		int CovertszDBRValue =0;
		int CovertszUBRValue =0;
		int i = 0;
		for(i = 0; i<3; ++i)
		{
			SendDutCommand(" " , "#" ,2000);
			SendDutCommand("adsl info --state" , "#" ,2000);
			CString strAdslInfo  = m_strStoreEthernetData;
			const char* stradsl =NULL ;
			if(
				((stradsl = strstr(strAdslInfo ,"Channel:        ")) != NULL) || 
				((stradsl = strstr(strAdslInfo ,"Path:        ")) != NULL)
			  )
			{
				const char*  pDBR    = strstr(stradsl,_T("Downstream rate = "));
				const char*  pDBRValueB = pDBR   +   strlen(_T("Downstream rate = "));
				const char*  pDBRValueE = strstr(pDBRValueB,_T(" Kbps"));

				strncpy_s(szDBRValue,MINBUFSIZE,pDBRValueB,pDBRValueE - pDBRValueB+1);
				CovertszDBRValue  = atoi(szDBRValue);

				const char*  pUBR       = strstr(stradsl, "Upstream rate = ");
				const char*  pUBRValueB = pUBR   +   strlen("Upstream rate = ");
				const char*  pUBRValueE = strstr(pUBRValueB," Kbps");

				strncpy_s(szUBRValue,MINBUFSIZE,pUBRValueB,pUBRValueE - pUBRValueB+1);
				CovertszUBRValue  = atoi(szUBRValue);

				if(CovertszDBRValue <0)
					CovertszDBRValue = 0;
				if(CovertszUBRValue <0)
					CovertszUBRValue = 0;
			}
			DisplayRunTimeInfo(szUBRValue);
			DisplayRunTimeInfo("");
			DisplayRunTimeInfo(szDBRValue);
			DisplayRunTimeInfo("");

			if((CovertszDBRValue >= m_IniFileInfo.nDSLDownStream) && (CovertszUBRValue >= m_IniFileInfo.nDSLUpStream))
			{
				break;
			}
			else
			{
				continue;
			}
			Sleep(500);
		}
		char szFomat[MINBUFSIZE] = "";
		sprintf_s(szFomat, MINBUFSIZE, "%12s",  szDBRValue);
		m_strRecordTestData += szFomat;
		m_strRecordTestData += "vdsl_down";
		m_strRecordTestData +="\t";
		sprintf_s(szFomat, MINBUFSIZE, "%12s",  szUBRValue);
		m_strRecordTestData += szFomat;
		m_strRecordTestData += "vdsl_up";


		if(CovertszDBRValue < m_IniFileInfo.nDSLDownStream )
		{
			SendSfisResultAndShowFail(DS51);
			return false;
		}
		if(CovertszUBRValue < m_IniFileInfo.nDSLUpStream)
		{
			SendSfisResultAndShowFail(DS52);
			return false;
		}
	}
	return true;
}
bool CRunInfo::CheckWithToken(char* DutInforFromIniFile)
{
	char    Seps[] = "\r\n";
	char    *token;
	char    *Context ;
	char	szBuf[MINBUFSIZE] = "" ;
	sprintf_s(szBuf , MINBUFSIZE , "%s" , g_data);
	token  = strtok_s(szBuf, Seps, &Context);
	while(token != NULL)
	{
		token = strtok_s(NULL, Seps, &Context);

		CString str = token;
		str.Trim();

		if(strcmp(str.GetBuffer(str.GetLength()), DutInforFromIniFile) == 0)
		{
			return true;
			
		}
	}
	return false;
}

bool CRunInfo::CheckWithTokenLength(char* DutInforFromSFIS , char* KeyWord )
{
	char    Seps[] = "-\n";
	char    *token;
	char    *token2;
	char    szFromDUT[MINBUFSIZE] = "";
	char    *Context ;
	char	szBuf[MINBUFSIZE] = "" ;
	sprintf_s(szBuf , MINBUFSIZE , "%s" , g_data);
	token  = strtok_s(szBuf, Seps, &Context);
	while(token != NULL)
	{
		token = strtok_s(NULL, Seps, &Context);
		if(strcmp(token, KeyWord) == 0)
		{
			char Seps2[] = " ";
			token = strtok_s(NULL, Seps, &Context);
			token2 = strtok_s(token, Seps2, &Context);
			
			//strncpy_s(szFromDUT, sizeof(DutInforFromSFIS),token2, sizeof(DutInforFromSFIS));
			strncpy_s(szFromDUT, MINBUFSIZE, token2, strlen(DutInforFromSFIS));
			break;
		}
	}
	if(strcmp(szFromDUT, DutInforFromSFIS) == 0)
	{
		return true;
	}
	return false;

}

bool CRunInfo::CHECK_SSID_2G(char* pSsid)
{	
	/*
	true	:pass
	false	:failed
	*/
	ShowMsg("***Check DUT 2.4G SSID***");
	if(!SendDutCommand("burnssid" , "#" , 8000))
	{
		return false;
	}
	if(!CheckWithTokenLength(pSsid , "SSID " ))
	{
		return false;
	}
	return true;
}

bool CRunInfo::CHECK_SSID_5G(char* pSsid)
{	
	/*
	true	:pass
	false	:failed
	*/
	ShowMsg("***Check DUT 5G SSID***");
	if(!SendDutCommand("burn5gssid" , "#" , 8000))
	{
		return false;
	}
	if(!CheckWithTokenLength(pSsid , "SSID_5G " ))
	{
		return false;
	}
	return true;
}

bool CRunInfo::CHECK_WPA_PASSEPHRASE_2G(char* pPassphrase)
{
	/*
	true	:pass
	false	:failed
	*/
	ShowMsg("***Check DUT 2.4G Passphrase***");
	if(!SendDutCommand("burnpass" , "#" , 8000))
	{
		return false;
	}
	if(!CheckWithTokenLength(pPassphrase , "Passphrase " ))
	{
		return false;
	}
	return true;
}
bool CRunInfo::CHECK_WPA_PASSEPHRASE_5G(char* pPassphrase)
{
	/*
	true	:pass
	false	:failed
	*/
	ShowMsg("***Check DUT 5G Passphrase***");
	if(!SendDutCommand("burn5gpass" , "#" , 8000))
	{
		return false;
	}
	if(!CheckWithTokenLength(pPassphrase , "Passphrase_5G " ))
	{
		return false;
	}
	return true;
}

int CRunInfo::CHECK_LAN_PORT(void)
{
	/*
	-1	:pass

	2	:ping szLanPort_IP[2] failed
	3	:ping szLanPort_IP[3] failed
	4	:ping szLanPort_IP[4] failed

	12	:lan to szLanPort_IP[2] throughput failed
	13	:lan to szLanPort_IP[3] throughput failed
	14	:lan to szLanPort_IP[4] throughput failed
	
	*/
	ShowMsg("***Check DUT Lan Port***");
	//get setting value
	m_IniFileInfo.nLanPort_Num=GetPrivateProfileInt("General","Lan_Port_Num",2,".\\DUT_LAN_WAN_IP.ini");
	m_IniFileInfo.nPingSuccessCount = GetPrivateProfileInt("General","Ping_Num",2,".\\DUT_LAN_WAN_IP.ini");

	int iTime= m_IniFileInfo.nLanPort_Num;
	//get lan port IP adress
	for(iTime; iTime>1; iTime--)
	{
		char szBuf[MINBUFSIZE];
		memset(szBuf, 0, MINBUFSIZE);
		sprintf_s(szBuf, MINBUFSIZE, "Lan_IP_%d", iTime);
		GetPrivateProfileString("General",szBuf,"192.168.31.5",m_IniFileInfo.szLanPort_IP[iTime], MINBUFSIZE,".\\DUT_LAN_WAN_IP.ini");
	}
	//ping lan port address
	iTime= m_IniFileInfo.nLanPort_Num;
	for(iTime; iTime>1; iTime--)
	{
		DisplayRunTimeInfo("iTime=", iTime);
		DisplayRunTimeInfo("m_IniFileInfo.szLanPort_IP[iTime]=", m_IniFileInfo.szLanPort_IP[iTime]);
		if(!PingSpecifyIP(m_IniFileInfo.szLanPort_IP[iTime] , m_IniFileInfo.nPingSuccessCount))
		{
			char szBuf[MINBUFSIZE];
			sprintf_s(szBuf, MINBUFSIZE, "ping %s failed", m_IniFileInfo.szLanPort_IP[iTime]);
			DisplayRunTimeInfo(szBuf);
			return iTime;//ping szLanPort_IP[iTime] failed
		}
		else
		{
			DisplayRunTimeInfo("Ping another lan port OK!!!");
		}
	}
	//check lan to lan throughput or not
	/*if(0 == m_IniFileInfo.nLanPort_Throughput_Flag)
	{
		ShowMsg("don't need check DUT Lan port throughput!\n");
		return -1;
	}*/
	
	//test lan to lan throughput
	/*iTime= m_IniFileInfo.nLanPort_Num;
	for(iTime; iTime>= 1; iTime--)
	{
		DisplayRunTimeInfo("iTime=", iTime);
		DisplayRunTimeInfo("m_IniFileInfo.szLanPort_IP[iTime]=", m_IniFileInfo.szLanPort_IP[iTime]);
		bool bResult = DutTestLanLanThroughput(m_IniFileInfo.szLanPort_IP[iTime]);
		if(false == bResult)
		{
			return (iTime+10);
		}
	}*/
	return -1;	
}

int CRunInfo::CHECK_WAN_PORT(void)
{
	/*
	-1	:pass

	1	:send DUT command failed
	2	:ping DUT wan port failed
	3	:Test DUT lan to wan throughput failed
	
	*/
	ShowMsg("***Check DUT Wan Port***");
	//check Wan port or not?
	if(0 == m_IniFileInfo.nWanPort_Flag)
	{
		ShowMsg("don't need check DUT Wan port!\n");
		return -1;//pass
	}
	//get Wan port IP setting value
	GetPrivateProfileString("General","Wan_IP","111.111.111.222",m_IniFileInfo.szWanPort_IP, MINBUFSIZE,".\\DUT_LAN_WAN_IP.ini");
	GetPrivateProfileString("General","Wan_IP_Prefix","111.111.111.",m_IniFileInfo.szWanPort_IP_PRE, MINBUFSIZE,".\\DUT_LAN_WAN_IP.ini");
	m_IniFileInfo.nPingSuccessCount = GetPrivateProfileInt("General","Ping_Num",2,".\\DUT_LAN_WAN_IP.ini");

	/*
	MICE FIX Wan Port IP Command
	/usr/sbin/wan.monitor stop
	uci set network.wan.proto=static
	ifdown wan
	ifup wan
	ifconfig eth0.2 192.168.xxx.xxx
	*/
	//if(!SendDutCommand("nvram get wan_ipaddr" , m_IniFileInfo.szWanPort_IP_PRE , 8000))
	if(SendDutCommand("/usr/sbin/wan.monitor stop","#",10000))
	{
		if((strstr(g_data,"stop pid") && strstr(g_data,"ok")) || strstr(g_data,"#"))
		{
			ShowMsg("Stop wan process OK!!!");
		}
		else 
		{
			return 1;//send DUT command failed
		}
	}
	else
	{
		return 1;
	}

	if(SendDutCommand("uci set network.wan.proto=static","#",10000))
	{
		if(SendDutCommand("ifdown wan","#",10000))
		{
			if(SendDutCommand("ifup wan","#",10000))
			{
				if(SendDutCommand("ifconfig eth0.2 111.111.111.111 netmask 255.255.255.0" , "#", 10000))
				{
					ShowMsg("Fix wan port IP OK!!!");
				}
				else 
				{
					return 1;//send DUT command failed
				}
			}
			else
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}

	}
	else
	{
		return 1;
	}

	//ping wan port IP	
	if(!PingSpecifyIP(m_IniFileInfo.szWanPort_IP , m_IniFileInfo.nPingSuccessCount))
	{
		return 2;//ping DUT WAN port failed
	}
	else
	{
		ShowMsg("ping wan port ok!!!");
	}
	//check lan wan throughput or not?
	ShowMsg("***Check DUT Wan Port throughput***");
	if(0 == m_IniFileInfo.nWanPort_Throughput_Flag)
	{
		ShowMsg("don't need check DUT Wan port throughput!\n");
		return -1;//pass
	}
	//test lan wan throughput
	bool bResult = DutTestLanWanThroughput();
	if(false == bResult)
	{
		return 3;//lan wan throughput test failed
	}
	return -1;	
}

bool CRunInfo::DutTestLanWanThroughput(void)
{
	//prepare
	char szLanWanThroughputIperfCmd[MINBUFSIZE];
	sprintf_s(szLanWanThroughputIperfCmd, MINBUFSIZE, m_IniFileInfo.szWanPort_Throughput_Iperf, m_IniFileInfo.szWanPort_IP);

	//run iperf.exe
	double TXValue = 0;
	double RXValue = 0;
	double TXRXValue = 0;
	double TXRXValue_Rand;
	CString szIperfInfo;
	DisplayRunTimeInfo("Start LAN to WAN TX+RX throughput test");
	DisplayRunTimeInfo("szLanWanThroughputIperfCmd:", szLanWanThroughputIperfCmd);

	RunSpecifyExeAndRead(szIperfInfo,szLanWanThroughputIperfCmd,false);  

	//get run result	
	GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue, RXValue);
	TXRXValue_Rand = TXValue + RXValue;

	//20120510 add	
	if(m_IniFileInfo.nWanPort_Throughput_Flag_Factroy == 1)
	{
		TXRXValue = __RandResult(TXRXValue_Rand, m_IniFileInfo.nWanPort_Throughout_SPEC_LOW, m_IniFileInfo.nWanPort_Throughout_SPEC_LOW_Factroy, m_IniFileInfo.nWanPort_Throughout_RANGE_Factroy);
	}
	else
	{
		TXRXValue = TXRXValue_Rand;
	}

	//end

	DisplayRunTimeInfo("LAN to WAN throughput:", TXRXValue);


	//recode test data
	sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,TXRXValue);
	char szBuf[64];
	sprintf_s(szBuf, 64, "%d", TXRXValue);
	m_strRecordTestData += szBuf;
	m_strRecordTestData += "\t";

	//adjust result
	
	if((TXRXValue >= m_IniFileInfo.nWanPort_Throughout_SPEC_HIGH) || (TXRXValue <= m_IniFileInfo.nWanPort_Throughout_SPEC_LOW))
	{
		return false;//lan to wan throughput value out SPEC
	}
	else
	{
		//do nothing
	}
	return true;
}

bool CRunInfo::DutTestLanLanThroughput(char* LAN_IP)
{
	//prepare
	char szLanLanThroughputIperfCmd[MINBUFSIZE];
	sprintf_s(szLanLanThroughputIperfCmd, MINBUFSIZE, m_IniFileInfo.szLanPort_Throughput_Iperf, LAN_IP);

	//run iperf.exe
	double TXValue = 0;
	double RXValue = 0;
	double TXRXValue = 0;
	CString szIperfInfo;
	DisplayRunTimeInfo("Start LAN to LAN TX+RX throughput test");
	RunSpecifyExeAndRead(szIperfInfo,szLanLanThroughputIperfCmd,false);  

	//get run result	
	GetThroughputValue(szIperfInfo.GetBuffer(szIperfInfo.GetLength()), TXValue, RXValue);
	TXRXValue = TXValue + RXValue;
	DisplayRunTimeInfo("LAN to LAN throughput value: %.2f", TXRXValue);

	//recode test data
	sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,TXRXValue);	
	char szBuf[64];
	sprintf_s(szBuf, 64, "%d", TXRXValue);
	m_strRecordTestData += szBuf;
	m_strRecordTestData += "\t";

	//adjust result
	if((TXRXValue >= m_IniFileInfo.nLanPort_Throughout_SPEC_HIGH) || (TXRXValue <= m_IniFileInfo.nLanPort_Throughout_SPEC_LOW))
	{
		return false;//lan to wan throughput value out SPEC
	}
	else
	{
		//do nothing
	}
	return true;
}

int CRunInfo::DutCheckUsb()
{
	int iResult = -1 ;
	if(!DECT_USB())
	{
		iResult = 91;//USB dect failed
	}
	if(iResult != -1)
	{
		return iResult;
	}

	USB.bUSB_Stoage_USB20_Flag = false;
	
	if( (m_IniFileInfo.nUsbTXThroughputFlag_1 != 1)  || 
		(m_IniFileInfo.nUsbRXThroughputFlag_1 != 1) ||
		(m_IniFileInfo.nUsbTXThroughputFlag_2 != 1) || 
		(m_IniFileInfo.nUsbRXThroughputFlag_2 != 1))
	{
		int iRty=3;
		while(iRty--)
		{	
			
			SendDutCommand("ls -l /tmp/shares/USB_Storage/", "#",8000);
			
			if(g_data.Find(USB.szUSB20FileName) != -1)
			{
				USB.bUSB_Stoage_USB20_Flag = true;
				break;
			}
			else
			{
				Sleep(100);
			}
			if(g_data.Find(USB.szUSB30FileName) != -1)
			{
				USB.bUSB_Stoage_USB20_Flag = false;
				break;
			}
			else
			{
				Sleep(100);
			}

			if(iRty <= 0)
			{
				return 1;//At USB no test file
			}
		}	
	}
	iResult = USBThroughtPut_1();
	if(iResult != -1)
	{
		return iResult;
	}
	
	iResult = USBThroughtPut_2();
	if(iResult != -1)
	{
		return iResult;
	}
    return iResult;
}
bool CRunInfo::DECT_USB()
{
	ShowMsg("DECT DUT USB port!\n");
    if(m_IniFileInfo.nTestUsb == 0)
	{
		ShowMsg("don't need check DUT USB port!\n");
	}
   //if(!SendDutCommand(m_IniFileInfo.szUsbDectCmd,m_IniFileInfo.szUsbDectString,8000))
	int iRty = 3;
	while(iRty--)
	{
		if(SendDutCommand(m_IniFileInfo.szUsbDectCmd,"I:* If#= 0 Alt= 0 #EPs= 2 Cls=08(stor.) Sub=06 Prot=50 Driver=usb-storage",8000))
		{
			return true;
		}
	}
    return false;
}


int CRunInfo::USBThroughtPut_1()
{
	/*
	-1	:pass
	1	:At USB no test file
	2	:start ftp process
	4	:Delete USB_Result_File failed
	5	:USB_TX_ThroughPut_RUN_File not exist 
	6	:USB_RX_ThroughPut_RUN_File not exist 
	7	:TimeOut too shorter for find test windows
	8	:imeOut too shorter for test finished
	10	:USB TX/RX throughput test failed
	11	:USB TX throughput test failed
	12	:USB RX throughput test failed
	*/	

	if((m_IniFileInfo.nUsbTXThroughputFlag_1 != 1)  && (m_IniFileInfo.nUsbRXThroughputFlag_1 != 1))
	{
		DisplayRunTimeInfo("Don't need test USB throughput!");
		return -1;
	}

	double     TXValue = 0;//recode TX value
	double     RXValue = 0;//recode RX value 

	//check at USB have data.bin file 
	int iRty=3;
    while(iRty--)
    {
		if(SendDutCommand("ls -l /tmp/shares/USB_Storage/", m_IniFileInfo.szUsbFilename_1,8000))
        {
            break;
        }
        if(iRty==0)
        {
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 1;//At USB 2.0 no test file
			}
			else
			{
				return 31;//At USB 3.0 no test file
			}
        }
	}

	//USB throughput setting command
	iRty = 3;
	while(iRty--)
	{
		if(SendDutCommand("/usr/sbin/bftpd -D -c /tmp/bftpd.conf &","bftpd",8000))
		{
			break;//start ftp process
		}
		if(iRty==0)
		{
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 2;//start ftp process
			}
			else
			{
				return 32;//start ftp process
			}
		}
	}

	//find ftp process
	iRty = 3;
	while(iRty--)
    {
        if(SendDutCommand("ps","/tmp/bftpd.conf",8000))
        {
            break;
        }
		else
		{
			Sleep(200);	
		}
		if(iRty==0)
		{
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 2;// ftp process not start
			}
			else
			{
				return 32;// ftp process not start
			}
		}
	}


	//find result file,if have delete it
	if( PathFileExists (m_IniFileInfo.szUsbResultFile_1) )
	{	
		if(!DeleteFile(m_IniFileInfo.szUsbResultFile_1))
		{
			DisplayRunTimeInfo("Delete USB_Result_File failed!");
			
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 4;// Delete USB_Result_File failed	
			}
			else
			{
				return 44;
			}
		}
	}


	//check USB test file exist
	if(m_IniFileInfo.nUsbTXThroughputFlag_1 != 0)
	{   
		if(!PathFileExists(m_IniFileInfo.szUsbTXThroughputRunFie_1))
        {
            DisplayRunTimeInfo("File nUsbTXThroughput doesn't exist!" );
            
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 5;// USB_TX_ThroughPut_RUN_File not exist 
			}
			else
			{
				return 35;
			}      
        }
	}
	else if(m_IniFileInfo.nUsbRXThroughputFlag_1 != 0)
	{
		if(!PathFileExists(m_IniFileInfo.szUsbRXThroughputRunFie_1))
        {
            DisplayRunTimeInfo("File nUsbRXThroughput doesn't exist!" );
            
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 6;// USB_RX_ThroughPut_RUN_File not exist  
			}
			else
			{
				return 36;
			}			
        }
	}
	else
	{
		//do nothing
	}

	int iRtyNumber = 4;
	while( iRtyNumber-- )
	{

		//run USB throughput file
		if(m_IniFileInfo.nUsbTXThroughputFlag_1 != 0)
		{   
			WinExec(m_IniFileInfo.szUsbTXThroughputRunFie_1 , SW_HIDE);

			//wait test finished
			//find test windows
			int TimeOut = 50;
			HWND hwndfind;
			while(TimeOut--)
			{
				hwndfind = ::FindWindow(NULL,"USB_TEST");
				if(::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput start...");
					break;
				}
				else
				{
					Sleep(100);
				}
			} 
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for find test windows!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 7;//TimeOut too shorter for find test windows!
				}
				else
				{
					return 37;
				}				
			}

			//check test finished or not
			TimeOut = 100;
			while(TimeOut--)
			{
				//check whether have test window
				DisplayRunTimeInfo("USB throughput testing ..."); 
				if(!::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput test finised!"); 
					break;
				}
				//when test result file exist ,but windows don't close , close it
				if(PathFileExists((m_IniFileInfo.szUsbResultFile_1)))
				{
					for(int iTimeout=0;iTimeout<5;iTimeout++)
					{
						if(!::IsWindow(hwndfind))
						{
							break;
						}
						DisplayRunTimeInfo("test windows not close!"); 
						Sleep(1000);
					}
					::PostMessage(hwndfind,WM_CLOSE,0,0);
				}
				else
					Sleep(200);
			}
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for test finished!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 8;//imeOut too shorter for test finished!
				}
				else
				{
					return 38;
				}				
			}

			//anaysis test result
			char szReadLine[512]="";
			char szResult[512]="";
			memset(szReadLine , 0 , 512);
			memset(szResult , 0 , 512);
			ifstream is;
			is.open(m_IniFileInfo.szUsbResultFile_1,ios_base::in);
			while(!is.eof())
			{
				is.getline(szReadLine,sizeof(szReadLine));
				if(szReadLine == NULL)
				{
					continue;
				}
				if(strstr(szReadLine , "Kbytes/sec") == NULL)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo(szReadLine); 
					char *szBuf;
					szBuf = GetValueBetween(CString(szReadLine), "Seconds ","Kbytes/sec");				
					sprintf_s(szResult, 512, "%s", szBuf);			
					DisplayRunTimeInfo("szResult=", szResult);
				}
			}
			TXValue = atof(szResult);
			DisplayRunTimeInfo("TXValue=", TXValue);
			is.close();
			DisplayRunTimeInfo("USB TX throughput valuse is:", TXValue); 
			//recode test data
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,TXValue);		
			char szBuf[64];
			sprintf_s(szBuf, 64, "%.2f", TXValue);
			m_strRecordTestData += szBuf;
			m_strRecordTestData += "\t";
		}	
		else if(m_IniFileInfo.nUsbRXThroughputFlag_1 != 0)
		{
			WinExec(m_IniFileInfo.szUsbRXThroughputRunFie_1 , SW_HIDE);
			//wait test finished
			//find test windows
			int TimeOut = 50;
			HWND hwndfind;
			while(TimeOut--)
			{
				hwndfind = ::FindWindow(NULL,"USB_TEST");
				if(::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput start...");
					break;
				}
				else
				{
					Sleep(100);
				}
			}
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for find test windows!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 7;//TimeOut too shorter for find test windows!
				}
				else
				{
					return 37;
				}
			}

			//check test finished or not
			TimeOut = 100;
			while(TimeOut--)
			{
				//check whether have test window
				DisplayRunTimeInfo("USB throughput testing ..."); 
				if(!::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput test finised!"); 
					break;
				}
				//when test result file exist ,but windows don't close , close it
				if(PathFileExists(m_IniFileInfo.szUsbResultFile_1))
				{
					for(int iTimeout=0;iTimeout<5;iTimeout++)
					{
						if(!::IsWindow(hwndfind))
						{
							break;
						}
						DisplayRunTimeInfo("test windows not close!"); 
						Sleep(1000);
					}
					::PostMessage(hwndfind,WM_CLOSE,0,0);
				}
				else
					Sleep(200);
			}
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for test finished!"); 
				
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 8;//imeOut too shorter for test finished
				}
				else
				{
					return 38;
				}
				
			}

			//anaysis test result
			char szReadLine[512]="";
			char szResult[512]="";

			memset(szReadLine , 0 , 512);
			memset(szResult , 0 , 512);

			ifstream is;
			is.open(m_IniFileInfo.szUsbResultFile_1,ios_base::in);
			while(!is.eof())
			{
				is.getline(szReadLine,sizeof(szReadLine));
				if(szReadLine == NULL)
				{
					continue;
				}
				if(strstr(szReadLine , "Kbytes/sec") == NULL)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo(szReadLine); 
					char *szBuf;
					szBuf = GetValueBetween(CString(szReadLine), "Seconds ","Kbytes/sec");				
					sprintf_s(szResult, 512, "%s", szBuf);			
					DisplayRunTimeInfo("szResult=", szResult);
				}

			}
			RXValue = atof(szResult);
			is.close();
			DisplayRunTimeInfo("USB RX throughput valuse is:", RXValue); 
			//recode test data
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,RXValue);
			char szBuf[64];
			sprintf_s(szBuf, 64, "%.2f", RXValue);
			m_strRecordTestData += szBuf;
			m_strRecordTestData += "\t";
			/*End*/
		}
		else
		{
			//do nothing
		}	

		//adjust result
		if(m_IniFileInfo.nUsbTXThroughputFlag_1  && m_IniFileInfo.nUsbRXThroughputFlag_1 )
		{
			if((TXValue >= m_IniFileInfo.nUsbTXThroughputSpec_1) && (RXValue >= m_IniFileInfo.nUsbRXThroughputSpec_1))
			{
				break;
			}
			else
			{
				if(iRtyNumber > 0)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo("USB TX/RX throughput test failed!"); 

					if(USB.bUSB_Stoage_USB20_Flag == true)
					{
						return 10;//USB TX/RX throughput test failed!
					}
					else
					{
						return 40;
					}
				}

			}
		}
		else if(m_IniFileInfo.nUsbTXThroughputFlag_1)
		{
			if(TXValue >= m_IniFileInfo.nUsbTXThroughputSpec_1 )
			{
				break;
			}
			else
			{
				if(iRtyNumber > 0)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo("USB TX throughput test failed!"); 
					if(USB.bUSB_Stoage_USB20_Flag == true)
					{
						return 11;//USB TX throughput test failed!
					}
					else
					{
						return 41;
					}					
				}
			}
		}
		else if(m_IniFileInfo.nUsbRXThroughputFlag_1)
		{
			if(RXValue >= m_IniFileInfo.nUsbRXThroughputSpec_1 )
			{
				break;
			}
			else
			{
				if(iRtyNumber > 0)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo("USB RX throughput test failed!"); 
					
					if(USB.bUSB_Stoage_USB20_Flag == true)
					{
						return 12;//USB RX throughput test failed!
					}
					else
					{
						return 42;
					}					
				}
			}
		}
		else
		{
			//do nothing
		}
	}

	DisplayRunTimeInfo("USB throughput test passed!"); 
	return -1;
}

int CRunInfo::USBThroughtPut(int usbTxFlag,
							 int usbRxFlag, 
							 char* usbFileName, 
							 char* usbResultFile,
							 char* usbTxRunFileName,
							 char* usbRxRunFileName,
							 int usbTxSpec, 
							 int usbRxSpec
							 )
{
	/*
	-1	:pass
	1	:At USB no test file
	2	:start ftp process
	3	:ftp process not start
	4	:Delete USB_Result_File failed
	5	:USB_TX_ThroughPut_RUN_File not exist 
	6	:USB_RX_ThroughPut_RUN_File not exist 
	7	:TimeOut too shorter for find test windows
	8	:imeOut too shorter for test finished
	10	:USB TX/RX throughput test failed
	11	:USB TX throughput test failed
	12	:USB RX throughput test failed
	*/	

	if(usbTxFlag  || usbRxFlag )
	{
		DisplayRunTimeInfo("Don't need test USB throughput!");
		return -1;
	}

	double     TXValue = 0;//recode TX value
	double     RXValue = 0;//recode RX value 

	//check at USB have (data.bin) file 
	int iRty=5;
    while(iRty--)
    {
        if(SendDutCommand("ls -l /tmp/shares/USB_Storage/", usbFileName, 2000))
        {
            break;
        }
        if(iRty==1)
        {
            return 1;//At USB no test file
        }
    }
	
	//USB throughput setting command
	if(!SendDutCommand("/usr/sbin/bftpd -D -c /tmp/bftpd.conf &","bftpd",5000))
	{
		return 2;//start ftp process
	}

	//find ftp process
	int i = 20;
	for(i;i>0;i--)
    {
        if(SendDutCommand("ps","/tmp/bftpd.conf",5000))
        {
            break;
        }
		else
		{
			Sleep(1000);	
		}
	}
	if(i<=0)
	{
		return  3;// ftp process not start
	}

	//find result file,if have delete it
	if( PathFileExists (usbResultFile) )
	{	
		if(!DeleteFile(usbResultFile))
		{
			DisplayRunTimeInfo("Delete USB_Result_File failed!");
			return 4; // Delete USB_Result_File failed			
		}
	}


	//check USB test file exist
	if(usbTxFlag != 0)
	{   
		if(!PathFileExists(usbTxRunFileName))
        {
            DisplayRunTimeInfo("File nUsbTXThroughput doesn't exist!" );
            return 5;// USB_TX_ThroughPut_RUN_File not exist        
        }
	}
	else if(usbRxFlag != 0)
	{
		if(!PathFileExists(usbRxRunFileName))
        {
            DisplayRunTimeInfo("File nUsbRXThroughput doesn't exist!" );
            return 6;// USB_RX_ThroughPut_RUN_File not exist        
        }
	}
	else
	{
		//do nothing
	}

	//run USB throughput file
	if(usbTxFlag != 0)
	{   
		WinExec(usbTxRunFileName , SW_HIDE);

		//wait test finished
		//find test windows
		int TimeOut = 50;
		HWND hwndfind;
		while(TimeOut--)
		{
			hwndfind = ::FindWindow(NULL,"USB_TEST");
			if(::IsWindow(hwndfind))
			{
				DisplayRunTimeInfo("USB throughput start...");
				break;
			}
			else
			{
				Sleep(500);
			}
		} 
		if(TimeOut == 0)
		{
			DisplayRunTimeInfo("TimeOut too shorter for find test windows!"); 
			return 7;//TimeOut too shorter for find test windows!

		}

		//check test finished or not
		TimeOut = 100;
		while(TimeOut--)
		{
			//check whether have test window
			DisplayRunTimeInfo("USB throughput testing ..."); 
			if(!::IsWindow(hwndfind))
			{
				DisplayRunTimeInfo("USB throughput test finised!"); 
				break;
			}
			//when test result file exist ,but windows don't close , close it
			if(PathFileExists((usbResultFile)))
			{
				for(int iTimeout=0;iTimeout<5;iTimeout++)
				{
					if(!::IsWindow(hwndfind))
					{
						break;
					}
					DisplayRunTimeInfo("test windows not close!"); 
					Sleep(1000);
				}
				::PostMessage(hwndfind,WM_CLOSE,0,0);
			}
			else
				Sleep(200);
		}
		if(TimeOut == 0)
		{
			DisplayRunTimeInfo("TimeOut too shorter for test finished!"); 
			return 8;//imeOut too shorter for test finished!
		}

		//anaysis test result
		char szReadLine[512]="";
		char szResult[512]="";
		memset(szReadLine , 0 , 512);
		memset(szResult , 0 , 512);
		ifstream is;
		is.open(usbResultFile,ios_base::in);
		while(!is.eof())
		{
			is.getline(szReadLine,sizeof(szReadLine));
			if(szReadLine == NULL)
			{
				continue;
			}
			if(strstr(szReadLine , "Kbytes/sec") == NULL)
			{
				continue;
			}
			else
			{
				DisplayRunTimeInfo(szReadLine); 
				char *szBuf;
				szBuf = GetValueBetween(CString(szReadLine), "Seconds ","Kbytes/sec");				
				sprintf_s(szResult, 512, "%s", szBuf);			
				DisplayRunTimeInfo("szResult=", szResult);
			}
		}
		TXValue = atof(szResult);
		DisplayRunTimeInfo("TXValue=", TXValue);
		is.close();
		DisplayRunTimeInfo("USB TX throughput valuse is:", TXValue); 
		//recode test data
		sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,TXValue);		
		char szBuf[64];
		sprintf_s(szBuf, 64, "%.2f", TXValue);
		m_strRecordTestData += szBuf;
		m_strRecordTestData += "\t";
	}	
	else if(usbRxFlag != 0)
	{
		WinExec(usbRxRunFileName , SW_HIDE);
		//wait test finished
		//find test windows
		int TimeOut = 50;
		HWND hwndfind;
		while(TimeOut--)
		{
			hwndfind = ::FindWindow(NULL,"USB_TEST");
			if(::IsWindow(hwndfind))
			{
				DisplayRunTimeInfo("USB throughput start...");
				break;
			}
			else
			{
				Sleep(500);
			}
		}
		if(TimeOut == 0)
		{
			DisplayRunTimeInfo("TimeOut too shorter for find test windows!"); 
			return 7;//TimeOut too shorter for find test windows!

		}

		//check test finished or not
		TimeOut = 100;
		while(TimeOut--)
		{
			//check whether have test window
			DisplayRunTimeInfo("USB throughput testing ..."); 
			if(!::IsWindow(hwndfind))
			{
				DisplayRunTimeInfo("USB throughput test finised!"); 
				break;
			}
			//when test result file exist ,but windows don't close , close it
			if(PathFileExists(usbResultFile))
			{
				for(int iTimeout=0;iTimeout<5;iTimeout++)
				{
					if(!::IsWindow(hwndfind))
					{
						break;
					}
					DisplayRunTimeInfo("test windows not close!"); 
					Sleep(1000);
				}
				::PostMessage(hwndfind,WM_CLOSE,0,0);
			}
			else
				Sleep(200);
		}
		if(TimeOut == 0)
		{
			DisplayRunTimeInfo("TimeOut too shorter for test finished!"); 
			return 8;//imeOut too shorter for test finished!
		}

		//anaysis test result
		char szReadLine[512]="";
		char szResult[512]="";
		
		memset(szReadLine , 0 , 512);
		memset(szResult , 0 , 512);

		ifstream is;
		is.open(usbResultFile,ios_base::in);
		while(!is.eof())
		{
			is.getline(szReadLine,sizeof(szReadLine));
			if(szReadLine == NULL)
			{
				continue;
			}
			if(strstr(szReadLine , "Kbytes/sec") == NULL)
			{
				continue;
			}
			else
			{
				DisplayRunTimeInfo(szReadLine); 
				char *szBuf;
				szBuf = GetValueBetween(CString(szReadLine), "Seconds ","Kbytes/sec");				
				sprintf_s(szResult, 512, "%s", szBuf);			
				DisplayRunTimeInfo("szResult=", szResult);
			}

		}
		RXValue = atof(szResult);
		is.close();
		DisplayRunTimeInfo("USB RX throughput valuse is:", RXValue); 
		//recode test data
		sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,RXValue);
		char szBuf[64];
		sprintf_s(szBuf, 64, "%.2f", RXValue);
		m_strRecordTestData += szBuf;
		m_strRecordTestData += "\t";
		/*End*/
	}
	else
	{
		//do nothing
	}	

	//adjust result
	if(usbTxFlag  && usbRxFlag )
	{
		if((TXValue >= usbTxSpec) && (RXValue >= usbRxSpec))
		{
			//do nothing
		}
		else
		{
			DisplayRunTimeInfo("USB TX/RX throughput test failed!"); 
			return 10;//USB TX/RX throughput test failed!
		}
	}
	else if(usbTxFlag)
	{
		if(TXValue >= usbTxSpec )
		{
			//do nothing
		}
		else
		{
			DisplayRunTimeInfo("USB TX throughput test failed!"); 
			return 11;//USB TX throughput test failed!
		}
	}
	else if(usbRxFlag)
	{
		if(RXValue >= usbRxSpec )
		{
			//do nothing
		}
		else
		{
			DisplayRunTimeInfo("USB RX throughput test failed!"); 
			return 12;//USB RX throughput test failed!
		}
	}
	else
	{
		//do nothing
	}
	DisplayRunTimeInfo("USB throughput test passed!"); 
	return -1;
}

int CRunInfo::USBThroughtPut_2()
{
	/*
	-1	:pass
	1	:At USB no test file
	2	:start ftp process
	4	:Delete USB_Result_File failed
	5	:USB_TX_ThroughPut_RUN_File not exist 
	6	:USB_RX_ThroughPut_RUN_File not exist 
	7	:TimeOut too shorter for find test windows
	8	:imeOut too shorter for test finished
	10	:USB TX/RX throughput test failed
	11	:USB TX throughput test failed
	12	:USB RX throughput test failed
	*/	

	if((m_IniFileInfo.nUsbTXThroughputFlag_2 != 1)  && (m_IniFileInfo.nUsbRXThroughputFlag_2 != 1))
	{
		DisplayRunTimeInfo("Don't need test USB throughput!");
		return -1;
	}

	double     TXValue = 0;//recode TX value
	double     RXValue = 0;//recode RX value 

	//check at USB have data.bin file 
	int iRty=3;
    while(iRty--)
    {
		if(SendDutCommand("ls -l /tmp/shares/T_Drive/", m_IniFileInfo.szUsbFilename_2, 8000))
        { 
            break;
        }
        if(iRty==0)
        {
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 31;//At USB 3.0 no test file
			}
			else
			{
				return 1;
			}
        }
	}

	////USB throughput setting command
	//iRty = 5;
	//while(iRty--)
	//{
	//	if(SendDutCommand("/usr/sbin/bftpd -D -c /tmp/bftpd.conf &","bftpd",2000))
	//	{
	//		break;//start ftp process
	//	}
	//	if(iRty==1)
	//	{
	//		return 2;//start ftp process
	//	}
	//}

	////find ftp process
	//iRty = 5;
	//while(iRty--)
 //   {
 //       if(SendDutCommand("ps","/tmp/bftpd.conf",5000))
 //       {
 //           break;
 //       }
	//	else
	//	{
	//		Sleep(200);	
	//	}
	//	if(iRty==1)
	//	{
	//		return 2;// ftp process not start
	//	}
	//}


	//find result file,if have delete it
	if( PathFileExists (m_IniFileInfo.szUsbResultFile_2) )
	{	
		if(!DeleteFile(m_IniFileInfo.szUsbResultFile_2))
		{
			DisplayRunTimeInfo("Delete USB_Result_File failed!");
			
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 34;// Delete USB_Result_File failed	
			}
			else
			{
				return 4;
			}				
		}
	}


	//check USB test file exist
	if(m_IniFileInfo.nUsbTXThroughputFlag_2 != 0)
	{   
		if(!PathFileExists(m_IniFileInfo.szUsbTXThroughputRunFie_2))
        {
            DisplayRunTimeInfo("File nUsbTXThroughput doesn't exist!" );
            if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 35;// USB_TX_ThroughPut_RUN_File not exist  
			}
			else
			{
				return 5;
			}			
        }
	}
	else if(m_IniFileInfo.nUsbRXThroughputFlag_2 != 0)
	{
		if(!PathFileExists(m_IniFileInfo.szUsbRXThroughputRunFie_2))
        {
            DisplayRunTimeInfo("File nUsbRXThroughput doesn't exist!" );
            
			if(USB.bUSB_Stoage_USB20_Flag == true)
			{
				return 36;// USB_RX_ThroughPut_RUN_File not exist
			}
			else
			{
				return 6;
			}			
        }
	}
	else
	{
		//do nothing
	}

	int iRtyNumber = 4;
	while( iRtyNumber-- )
	{

		//run USB throughput file
		if(m_IniFileInfo.nUsbTXThroughputFlag_2 != 0)
		{   
			WinExec(m_IniFileInfo.szUsbTXThroughputRunFie_2 , SW_HIDE);

			//wait test finished
			//find test windows
			int TimeOut = 50;
			HWND hwndfind;
			while(TimeOut--)
			{
				hwndfind = ::FindWindow(NULL,"USB_TEST");
				if(::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput start...");
					break;
				}
				else
				{
					Sleep(100);
				}
			} 
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for find test windows!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 37;//TimeOut too shorter for find test windows!
				}
				else
				{
					return 7;
				}

			}

			//check test finished or not
			TimeOut = 100;
			while(TimeOut--)
			{
				//check whether have test window
				DisplayRunTimeInfo("USB throughput testing ..."); 
				if(!::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput test finised!"); 
					break;
				}
				//when test result file exist ,but windows don't close , close it
				if(PathFileExists((m_IniFileInfo.szUsbResultFile_2)))
				{
					for(int iTimeout=0;iTimeout<5;iTimeout++)
					{
						if(!::IsWindow(hwndfind))
						{
							break;
						}
						DisplayRunTimeInfo("test windows not close!"); 
						Sleep(1000);
					}
					::PostMessage(hwndfind,WM_CLOSE,0,0);
				}
				else
					Sleep(200);
			}
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for test finished!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 38;//imeOut too shorter for test finished!
				}
				else
				{
					return 8;
				}				
			}

			//anaysis test result
			char szReadLine[512]="";
			char szResult[512]="";
			memset(szReadLine , 0 , 512);
			memset(szResult , 0 , 512);
			ifstream is;
			is.open(m_IniFileInfo.szUsbResultFile_2,ios_base::in);
			while(!is.eof())
			{
				is.getline(szReadLine,sizeof(szReadLine));
				if(szReadLine == NULL)
				{
					continue;
				}
				if(strstr(szReadLine , "Kbytes/sec") == NULL)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo(szReadLine); 
					char *szBuf;
					szBuf = GetValueBetween(CString(szReadLine), "Seconds ","Kbytes/sec");				
					sprintf_s(szResult, 512, "%s", szBuf);			
					DisplayRunTimeInfo("szResult=", szResult);
				}
			}
			TXValue = atof(szResult);
			DisplayRunTimeInfo("TXValue=", TXValue);
			is.close();
			DisplayRunTimeInfo("USB TX throughput valuse is:", TXValue); 
			//recode test data
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,TXValue);		
			char szBuf[64];
			sprintf_s(szBuf, 64, "%.2f", TXValue);
			m_strRecordTestData += szBuf;
			m_strRecordTestData += "\t";
		}	
		else if(m_IniFileInfo.nUsbRXThroughputFlag_2 != 0)
		{
			WinExec(m_IniFileInfo.szUsbRXThroughputRunFie_2 , SW_HIDE);
			//wait test finished
			//find test windows
			int TimeOut = 50;
			HWND hwndfind;
			while(TimeOut--)
			{
				hwndfind = ::FindWindow(NULL,"USB_TEST");
				if(::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput start...");
					break;
				}
				else
				{
					Sleep(100);
				}
			}
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for find test windows!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 37;//TimeOut too shorter for find test windows!
				}
				else
				{
					return 7;
				}					
			}

			//check test finished or not
			TimeOut = 100;
			while(TimeOut--)
			{
				//check whether have test window
				DisplayRunTimeInfo("USB throughput testing ..."); 
				if(!::IsWindow(hwndfind))
				{
					DisplayRunTimeInfo("USB throughput test finised!"); 
					break;
				}
				//when test result file exist ,but windows don't close , close it
				if(PathFileExists(m_IniFileInfo.szUsbResultFile_2))
				{
					for(int iTimeout=0;iTimeout<5;iTimeout++)
					{
						if(!::IsWindow(hwndfind))
						{
							break;
						}
						DisplayRunTimeInfo("test windows not close!"); 
						Sleep(1000);
					}
					::PostMessage(hwndfind,WM_CLOSE,0,0);
				}
				else
					Sleep(200);
			}
			if(TimeOut == 0)
			{
				DisplayRunTimeInfo("TimeOut too shorter for test finished!"); 
				if(USB.bUSB_Stoage_USB20_Flag == true)
				{
					return 38;//imeOut too shorter for test finished!
				}
				else
				{
					return 8;
				}
			}

			//anaysis test result
			char szReadLine[512]="";
			char szResult[512]="";

			memset(szReadLine , 0 , 512);
			memset(szResult , 0 , 512);

			ifstream is;
			is.open(m_IniFileInfo.szUsbResultFile_2,ios_base::in);
			while(!is.eof())
			{
				is.getline(szReadLine,sizeof(szReadLine));
				if(szReadLine == NULL)
				{
					continue;
				}
				if(strstr(szReadLine , "Kbytes/sec") == NULL)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo(szReadLine); 
					char *szBuf;
					szBuf = GetValueBetween(CString(szReadLine), "Seconds ","Kbytes/sec");				
					sprintf_s(szResult, 512, "%s", szBuf);			
					DisplayRunTimeInfo("szResult=", szResult);
				}

			}
			RXValue = atof(szResult);
			is.close();
			DisplayRunTimeInfo("USB RX throughput valuse is:", RXValue); 
			//recode test data
			sprintf_s(TestDetaillog,MAXBUFSIZE*10,"%s,%f,",TestDetaillog,RXValue);
			char szBuf[64];
			sprintf_s(szBuf, 64, "%.2f", RXValue);
			m_strRecordTestData += szBuf;
			m_strRecordTestData += "\t";
			/*End*/
		}
		else
		{
			//do nothing
		}	

		//adjust result
		if(m_IniFileInfo.nUsbTXThroughputFlag_2  && m_IniFileInfo.nUsbRXThroughputFlag_2 )
		{
			if((TXValue >= m_IniFileInfo.nUsbTXThroughputSpec_2) && (RXValue >= m_IniFileInfo.nUsbRXThroughputSpec_2))
			{
				break;
			}
			else
			{
				if(iRtyNumber > 0)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo("USB TX/RX throughput test failed!"); 
					if(USB.bUSB_Stoage_USB20_Flag == true)
					{
						return 40;//USB TX/RX throughput test failed!
					}
					else
					{
						return 10;
					}					
				}

			}
		}
		else if(m_IniFileInfo.nUsbTXThroughputFlag_2)
		{
			if(TXValue >= m_IniFileInfo.nUsbTXThroughputSpec_2 )
			{
				break;
			}
			else
			{
				if(iRtyNumber > 0)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo("USB TX throughput test failed!"); 
					if(USB.bUSB_Stoage_USB20_Flag == true)
					{
						return 41;//USB TX throughput test failed!
					}
					else
					{
						return 11;
					}
				}
			}
		}
		else if(m_IniFileInfo.nUsbRXThroughputFlag_2)
		{
			if(RXValue >= m_IniFileInfo.nUsbRXThroughputSpec_2 )
			{
				break;
			}
			else
			{
				if(iRtyNumber > 0)
				{
					continue;
				}
				else
				{
					DisplayRunTimeInfo("USB RX throughput test failed!"); 
					if(USB.bUSB_Stoage_USB20_Flag == true)
					{
						return 42;//USB RX throughput test failed!
					}
					else
					{
						return 12;
					}					
				}
			}
		}
		else
		{
			//do nothing
		}
	}

	DisplayRunTimeInfo("USB throughput test passed!"); 
	return -1;
}

int fnCompareFileWriteTime(char* lpFilename1, char* lpFilename2)
{
	FILETIME ftCreateTime1, ftCreateTime2;
	FILETIME ftAccessTime1, ftAccessTime2;
	FILETIME ftWriteTime1, ftWriteTime2;
	
	HANDLE hFile = CreateFile(lpFilename1,GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	GetFileTime(hFile,&ftCreateTime1,&ftAccessTime1,&ftWriteTime1);
	CloseHandle(hFile);
	hFile = CreateFile(lpFilename2,GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	GetFileTime(hFile,&ftCreateTime2,&ftAccessTime2,&ftWriteTime2);
	CloseHandle(hFile);
	long lValue = CompareFileTime(&ftWriteTime1,&ftWriteTime2);
	
	return lValue;
}

bool CRunInfo::NewReadIniFile(void)
{
	// Read ini file with API , revised by daixin	
	char szProPath[MINBUFSIZE]="";	
	char szProPath1[MINBUFSIZE]="";	
	DisplayRunTimeInfo("********Read Init.ini File**********");	
	GetCurrentDirectory(sizeof(szProPath),szProPath);
	lstrcat(szProPath,"\\init.ini");	

	GetPrivateProfileString("General","TestInfoFileName","TestInformation.ini",m_IniFileInfo.szTestInfoFileName, MINBUFSIZE,szProPath);
	GetPrivateProfileString("General","ServerFilePath","",m_IniFileInfo.szServerFilePath, MINBUFSIZE,szProPath);
	//change place to funtion LinkToMydas()
//	GetPrivateProfileString("General","MydasVersion","",m_IniFileInfo.szMydasVersion, MINBUFSIZE,szProPath);
	return 1;
}
bool CRunInfo::DutCheckStringTableChecksum(void)
{
	/*
	true	:pass
	false	:failed
	*/
	DisplayRunTimeInfo("---Start check String table checksum---");
	//check if need test or not
	if( 0 == m_IniFileInfo.nCheckSumFlag )
	{
		DisplayRunTimeInfo("Don't need check String table checksum!");
		return true;
	}	

	//send DUT command to get DUT string table checksum
	char szBuf[MINBUFSIZE];
	sprintf_s(szBuf , sizeof(szBuf),"getchksum %d" , m_IniFileInfo.nCheckSumNum);
	if(!SendDutCommand(szBuf,"#",5000)) return false;

	//check DUT's checksum with ini file
	for(int iTime=1 ; iTime <= m_IniFileInfo.nCheckSumNum ; iTime++)
	{
		char Info[MINBUFSIZE] = "";
		char DestBuffer[MINBUFSIZE] = "";
		const char* flag=NULL;
		sprintf_s(Info,"chksum%d=",iTime);
		if((flag=strstr(g_data,Info))!=NULL )
		{
			strncpy_s(DestBuffer,MINBUFSIZE,flag+8,10);
			//AfxMessageBox(DestBuffer);
			if( (strstr(DestBuffer,m_IniFileInfo.szCkecksum[iTime]) == NULL) ||
				(strcmp("",m_IniFileInfo.szCkecksum[iTime]) == 0) ||
				(strlen(m_IniFileInfo.szCkecksum[iTime] ) == 0)
			   )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

int CRunInfo::SettingGoldenServer( int iChannel )
{
	/*
	-1	:pass
	2	:Get 2G link status failed 
	3	:Get 5G link status failed 
	*/
	CString strSSID;
	GetDlgItemText(IDC_EDIT_SSID, strSSID);

	if(iChannel > 14)//5G
	{
		char szSSIDCommand[MINBUFSIZE] = "";

		if(!SendGoldenCommand("wl band a" , "ok" , 5))
		{		
			return 3; //Get 5G link status failed  
		}
		if(!SendGoldenCommand("wl up" , "ok" , 5))
		{
			return 3; //Get 5G link status failed  
		}
		sprintf_s(szSSIDCommand, MINBUFSIZE, "startconn %s %d", m_IniFileInfo.sz5GThrotghputGoldenSSID , (m_IniFileInfo.nTimeOut));
		if(!SendGoldenCommand(szSSIDCommand , "success" , m_IniFileInfo.nTimeOut))
		{
			return 3; //Get 5G link status failed  
		}
	}
	else//2.4G
	{
		char szSSIDCommand[MINBUFSIZE] = "";
		if(!SendGoldenCommand("wl band b" , "ok" , m_IniFileInfo.nTimeOut))
		{		
			return 2; //Get 2G link status failed  
		}
		if(!SendGoldenCommand("wl up" , "ok" , m_IniFileInfo.nTimeOut))
		{
			return 2; //Get 2G link status failed  
		}
		sprintf_s(szSSIDCommand, MINBUFSIZE, "startconn %s %d", m_IniFileInfo.sz2GThrotghputGoldenSSID , m_IniFileInfo.nTimeOut);
		if(!SendGoldenCommand(szSSIDCommand , "success" , m_IniFileInfo.nTimeOut))
		{
			return 2; //Get 2G link status failed  
		}
	}
	return -1;
}

int CRunInfo::SettingDUT(int iChannel , int iChipsetIndex)
{
	/*
	-1	:pass
	1	:send dut setting command failed
	*/
	CString strSSID;
	GetDlgItemText(IDC_EDIT_SSID, strSSID);

	if(iChannel > 14)//5G
	{	
		
		/*SendDutCommand("wl -i eth2 up","#",2000);
		if(!SendDutCommand("nvram set wlg_wlanstate=Enable","#",5000))		return 1;	
		if(!SendDutCommand("nvram set wla_wlanstate=Disable","#",5000))		return 1;
		char szSSID[512]="";
		sprintf_s(szSSID,sizeof(szSSID),"nvram set wlg_ssid=%s",m_IniFileInfo.sz5GThrotghputGoldenSSID);
		if(!SendDutCommand(szSSID,"#",5000))	return 1;			
		char szChan[64]="";
		sprintf_s(szChan,sizeof(szChan),"nvram set wlg_channel=%d",iChannel);
		if(!SendDutCommand(szChan,"#",5000))	return 1;
		if(!SendDutCommand("wlg_mode=HT80","#",5000))	return 1;
		if(!SendDutCommand("nvram set wlg_secu_type=None","#",5000))	return 1;
		if(!SendDutCommand("killall -SIGUSR1 wlanconfigd","#",5000))	return 1;*/		
		
	}
	else//2.4G
	{	
		//2.4G setting
		//SendDutCommand("wl down","#",2000);	
		SendDutCommand("wl obss_coex 0","#",2000);		
		SendDutCommand("wl up","#",2000);
		if(!SendDutCommand("nvram set wla_wlanstate=Enable","#",5000))		return 1;
		if(!SendDutCommand("nvram set wlg_wlanstate=Enable","#",5000))		return 1;
		if(!SendDutCommand("nvram set wl_obss_coex=0","#",5000)) return 1;
		if(!SendDutCommand("nvram set wl0_obss_coex=0","#",5000)) return 1;		
		if(!SendDutCommand("nvram set wl1_obss_coex=0","#",5000)) return 1;
		char szSSID[512]="";
		sprintf_s(szSSID,sizeof(szSSID),"nvram set wla_ssid=%s",m_IniFileInfo.sz2GThrotghputGoldenSSID);
		if(!SendDutCommand(szSSID,"#",5000))	return 1;			
		char szChan[64]="";
		sprintf_s(szChan,sizeof(szChan),"nvram set wla_channel=%d",8);
		//sprintf_s(szChan,sizeof(szChan),"nvram set wla_channel=%d",iChannel);
		if(!SendDutCommand(szChan,"#",5000))	return 1;
		if(!SendDutCommand("nvram set wla_mode=300Mbps","#",5000))	return 1;	
		if(!SendDutCommand("nvram set wla_secu_type=None","#",5000))	return 1;
		if(!SendDutCommand("killall -SIGUSR1 wlanconfigd","#",5000))	return 1;


		//5G setting
		SendDutCommand("wl -i eth2 up","#",2000);
		if(!SendDutCommand("nvram set wlg_wlanstate=Enable","#",5000))		return 1;	
		if(!SendDutCommand("nvram set wla_wlanstate=Enable","#",5000))		return 1;
		//char szSSID[512]="";
		sprintf_s(szSSID,sizeof(szSSID),"nvram set wlg_ssid=%s",m_IniFileInfo.sz5GThrotghputGoldenSSID);
		if(!SendDutCommand(szSSID,"#",5000))	return 1;			
		//char szChan[64]="";
		sprintf_s(szChan,sizeof(szChan),"nvram set wlg_channel=%d",iChannel);
		if(!SendDutCommand(szChan,"#",5000))	return 1;
		if(!SendDutCommand("wlg_mode=HT80","#",5000))	return 1;
		if(!SendDutCommand("nvram set wlg_secu_type=None","#",5000))	return 1;
		if(!SendDutCommand("killall -SIGUSR1 wlanconfigd","#",5000))	return 1;	
	}	
	return -1;
}

bool CRunInfo::Check_Passphrase(char * pszPWD)
{
	/*
	true	:pass
	false	:failed
	*/
	ShowMsg("***Check DUT Passphrase format***");
    char szPwdVal[256]="";
	strcpy(szPwdVal,pszPWD);
	for(int i=0;i<100;i++)
	{
		int iAdjLen=strlen(Adjectives[i]);
		if(strncmp(szPwdVal, Adjectives[i],iAdjLen) == 0)
		{
			char szTemp[256]="";
			strcpy(szTemp,szPwdVal+iAdjLen);

			for(int j=0;j<100;j++)
			{
				int iNouLen=strlen(Nouns[j]);
				if(strncmp(szTemp,Nouns[j],iNouLen) == 0)
				{
					char szTempNum[256]="";
					strcpy(szTempNum,szTemp+iNouLen);
					if(strlen(szTempNum) == 3)
					{
						if((szTempNum[0]>='0' && szTempNum[0]<='9') &&
							(szTempNum[1]>='0' && szTempNum[1]<='9') &&
							(szTempNum[2]>='0' && szTempNum[2]<='9') )
						{
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return false;
}

char* CRunInfo::GetValueBetween(CString strBuffer,char*szBeginWord, char* szEndWord)
{
    CString strBuf = strBuffer.Trim();
	char szBuf[512];
	sprintf_s(szBuf, 512, "%s", strBuf);

    char  strRtv[512];
	
	int iBeginPos = strBuf.Find(szBeginWord);
    int iEndPos = strBuf.Find(szEndWord);

	strncpy_s(strRtv, sizeof(strRtv), &szBuf[iBeginPos + strlen(szBeginWord)], iEndPos-iBeginPos-strlen(szBeginWord));

    return strRtv;
}

float CRunInfo::__RandResult(float throughput_value , float throughput_spec_customer , float throughput_spec_our , float throughput_rand)
{
    //if throughput_value > throughput_spec_customer,return
    if(throughput_value > throughput_spec_customer)
    {
       return  throughput_value ;
    }
    //if throughput_value < throughput_spec_our,return
    if(throughput_value < throughput_spec_our)
    {
       return  throughput_value ;
    }
	
	//default rand range,if no this,may cause all range data is the same.
	srand( (unsigned)time( NULL ) );

    //if  throughput_value < throughput_spec_customer,return [throughput_spec_customer,throughput_spec_customer + throughput_rand ]
    int RANGE_MIN = 1;
    int RANGE_MAX = throughput_rand;

    float rand100 = (((double) rand() / (double) RAND_MAX) * RANGE_MAX + RANGE_MIN);

	throughput_value = rand100 + throughput_spec_customer;

    return  throughput_value ;
}
int CRunInfo::CheckButtonRelease()
{
	int iReleaseState = 0 ;
	
	int iRty = 5 ;
	while(iRty--)
	{
		SendDutCommand("\r\n","#",5000);
		if((strstr(g_data, RST_BTN_VALUE) == NULL) && (strstr(g_data, WPS_BTN_VALUE) == NULL) && (strstr(g_data, WIFI_BTN_VALUE) == NULL))
		{
			iReleaseState = 0;
			break;
		}
		if(strstr(g_data, RST_BTN_VALUE) != NULL)
		{
			iReleaseState = 1;//Reset button
			AfxMessageBox("請不要一直按壓Reset Button！\r\nPlease release Release Reset Button！");
			Sleep(500);
			continue ;
		}
		
		if(strstr(g_data, WPS_BTN_VALUE) != NULL)
		{
			iReleaseState = 2;//WPS button
			AfxMessageBox("請不要一直按壓WPS Button！\r\nPlease release Release WPS Button！");
			Sleep(500);
			continue ;
		}
		
		if(strstr(g_data, WIFI_BTN_VALUE) != NULL)
		{
			iReleaseState = 3;//WIFI button
			AfxMessageBox("請不要一直按壓WIFI Button！\r\nPlease release Release WIFI Button！");
			Sleep(500);
			continue ;
		}	
	}
	return iReleaseState;
}


bool CRunInfo::GainKey(void)
{
	char szKeyPath[MINBUFSIZE] = "";
	char szCurtPath[MINBUFSIZE] = "";
	GetCurrentDirectory(sizeof(szCurtPath),szCurtPath);
	sprintf_s(szKeyPath,sizeof(szKeyPath),"%s\\%s",szCurtPath,"keys");

	char szKeyCa[MINBUFSIZE] = "";
	char szKeyClitC[MINBUFSIZE] = "";
	char szKeySerC[MINBUFSIZE] = "";
	char szKeyClitK[MINBUFSIZE] ="";
	char szKeySerK[MINBUFSIZE] ="";
	char szKeyDh[MINBUFSIZE] = "";
	char szKeySum[MINBUFSIZE] = "";

	char szSKeyCa[MINBUFSIZE] = "";
	char szSKeyClitC[MINBUFSIZE] = "";
	char szSKeySerC[MINBUFSIZE] = "";
	char szSKeyClitK[MINBUFSIZE] ="";
	char szSKeySerK[MINBUFSIZE] ="";
	char szSKeyDh[MINBUFSIZE] = "";
	char szSKeySum[MINBUFSIZE] = "";

	sprintf_s(szSKeyCa,sizeof(szSKeyCa),"%s\\%s\\%s", m_IniFileInfo.szVPNkeyPath, m_strMAC, "ca.crt");
	sprintf_s(szSKeyClitC,sizeof(szSKeyClitC),"%s\\%s\\%s", m_IniFileInfo.szVPNkeyPath, m_strMAC, "client.crt");
	sprintf_s(szSKeySerC,sizeof(szSKeySerC),"%s\\%s\\%s", m_IniFileInfo.szVPNkeyPath, m_strMAC, "server.crt");
	sprintf_s(szSKeyClitK,sizeof(szSKeyClitK),"%s\\%s\\%s", m_IniFileInfo.szVPNkeyPath, m_strMAC, "client.key");
	sprintf_s(szSKeySerK,sizeof(szSKeySerK),"%s\\%s\\%s", m_IniFileInfo.szVPNkeyPath, m_strMAC, "server.key");
	sprintf_s(szSKeyDh,sizeof(szSKeyDh),"%s\\%s\\%s", m_IniFileInfo.szVPNkeyPath, m_strMAC, "dh1024.pem");
	sprintf_s(szSKeySum,sizeof(szSKeySum),"%s\\%s\\%s",m_IniFileInfo.szVPNkeyPath,m_strMAC,"Sum.txt");

	sprintf_s(szKeyCa,sizeof(szKeyCa),"%s\\%s", szKeyPath, "ca.crt");
	sprintf_s(szKeyClitC,sizeof(szKeyClitC),"%s\\%s", szKeyPath, "client.crt");
	sprintf_s(szKeySerC,sizeof(szKeySerC),"%s\\%s", szKeyPath, "server.crt");
	sprintf_s(szKeyClitK,sizeof(szKeyClitK),"%s\\%s", szKeyPath, "client.key");
	sprintf_s(szKeySerK,sizeof(szKeySerK),"%s\\%s", szKeyPath, "server.key");
	sprintf_s(szKeyDh,sizeof(szKeyDh),"%s\\%s", szKeyPath, "dh1024.pem");
	sprintf_s(szKeySum,sizeof(szKeySum),"%s\\%s",szKeyPath,"Sum.txt");

	if(PathFileExists(szSKeyCa))
	{
		CopyFile(szSKeyCa,szKeyCa,false);
		Delay(100);
	}
	else
	{
		DisplayRunTimeInfo("Form server copy ca.crt file fail");
		return false;
	}
	if(PathFileExists(szSKeyClitC))
	{
		CopyFile(szSKeyClitC,szKeyClitC,false);
		Delay(100);
	}
	else
	{
		DisplayRunTimeInfo("Form server copy client.crt file fail");
		return false;
	}
	if(PathFileExists(szSKeySerC))
	{
		CopyFile(szSKeySerC,szKeySerC,false);
		Delay(100);
	}
	else
	{
		DisplayRunTimeInfo("Form server copy server.crt file fail");
		return false;
	}
	if(PathFileExists(szSKeyClitK))
	{
		CopyFile(szSKeyClitK,szKeyClitK,false);
		Delay(100);
	}
	else
	{
		DisplayRunTimeInfo("Form server copy client.key file fail");
		return false;
	}
	if(PathFileExists(szSKeySerK))
	{
		CopyFile(szSKeySerK,szKeySerK,false);
		Delay(100);
	}
	else
	{
		DisplayRunTimeInfo("Form server copy server.key file fail");
		return false;
	}
	if(PathFileExists(szSKeyDh))
	{
		CopyFile(szSKeyDh,szKeyDh,false);
		Delay(100);
	}
	else
	{
		DisplayRunTimeInfo("Form server copy dh1024.pem file fail");
		return false;
	}
	if(PathFileExists(szSKeySum))
	{
		CopyFile(szSKeySum,szKeySum,false);
		Delay(100);
		return true;
	}
	else
	{
		DisplayRunTimeInfo("Form server copy dh1024.pem file fail");
		return false;
	}

	//char Path[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\openssl.cnf.txt";

	//char a_Openssl[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\openssl1.cnf.txt";
	//char b_Openssl[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\openssl2.cnf.txt";
	//char c_Openssl[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\openssl.cnf";
	//char a_Index[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\index.txt.start";
	//char b_Index[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\index.txt";
	//char a_Serial[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\serial.start";
	//char b_Serial[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\serial.";
	//char add_Key[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys";

	//char add_Burnrf[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\burnrf";
	//char add_Dumprf[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\dumprf";

	//char add_DH[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\dh1024.pem";
	//char add_Ca[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\ca.crt";
	//char aa_Server[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\server.crt";
	//char ab_Server[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\server.key";
	//char aa_Client[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\client.crt";
	//char ab_Client[256] = "D:\\Program Files\\OpenVPN\\easy-rsa\\keys\\client.key";
	//

	//ShellExecute(NULL,"open","cmd.exe","/c copy openssl.cnf.sample openssl.cnf.txt","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(1000);

	//CString strServerFile("D:\\Program Files\\OpenVPN\\easy-rsa\\openssl1.cnf.txt");
	//CString strServerFind("challengePassword_default=000000000001");
	//CString strServerReplace;

	//strServerReplace.Format("challengePassword_default=%s", m_strSN);

	////ReplaceTxt(strServerFile,strServerFind,strServerReplace);
	//if(ReplaceTxt(strServerFile,strServerFind,strServerReplace))
	//{	
	//	DisplayRunTimeInfo("Verify Server PassWord OK");
	//}
	//else
	//{
	//	DisplayRunTimeInfo("Verify Server PassWord FAIL");
	//	return false;
	//}
	//
	//if(PathFileExists(c_Openssl))
	//{
	//	if(!DeleteFile(c_Openssl))
	//	{
	//		DisplayRunTimeInfo("Delete old openssl.cnf fail");
	//		return false;
	//	}
	//}

	//Delay(500);
	//
	////-------------------------------------------create key---------------------------------------------------------------------

	//ShellExecute(NULL,"open","cmd.exe","/c copy openssl.cnf.txt openssl.cnf","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(500);

	//

	//if(!PathFileExists(c_Openssl))
	//{
	//	DisplayRunTimeInfo("Create new openssl.cnf fail");
	//	return false;
	//}
	//else
	//{
	//	ShellExecute(NULL,"open","cmd.exe","/c del openssl.cnf.txt","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	Delay(500);
	//	//ShellExecute(NULL,"open","cmd.exe","/c del openssl.cnf.txt","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	Delay(500);
	//}
	////-------------End-------------

	//

	//ShellExecute(NULL,"open","cmd.exe","/c rmdir /s /q keys","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(500);

	//ShellExecute(NULL,"open","cmd.exe","/c mkdir keys && copy index.txt.start keys\\index.txt && copy serial.start keys\\serial","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(500);

	//ShellExecute(NULL,"open","cmd.exe","/c copy burnrf.sample keys\\burnrf && copy dumprf.sample keys\\dumprf","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(1000);


	//if(!PathFileExists(add_Key))
	//{
	//	DisplayRunTimeInfo("Create KEYS Folder fail");
	//	return false;
	//}

	//if((!PathFileExists(add_Burnrf))&&(!PathFileExists(add_Dumprf)))
	//{
	//	DisplayRunTimeInfo("load cmd burnrf dumprf fail");
	//	return false;
	//}
	////-------------End-------------

	//ShellExecute(NULL,"open","cmd.exe","/c openssl dhparam -out keys\\dh1024.pem 1024","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//DisplayRunTimeInfo("Create dh1024.pem ing ");
	//Delay(500);

	//while(!PathFileExists(add_DH))
	//{
	//		
	//}
	//
	//if(PathFileExists(add_DH))
	//{
	//	ShellExecute(NULL,"open","cmd.exe","/c openssl req -days 3650 -nodes -new -x509 -keyout keys\\ca.key -out keys\\ca.crt -config openssl.cnf -batch","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	DisplayRunTimeInfo("Create dh1024.pem OK");
	//	DisplayRunTimeInfo("Create ca.crt ing");
	//}
	//
	//Delay(500);


	//if(!PathFileExists(add_Ca))
	//{
	//	DisplayRunTimeInfo("Create ca.crt fail");
	//	return false;	
	//}
	//else
	//{
	//	DisplayRunTimeInfo("Create ca.crt OK");
	//}
	//
	////-------------End-------------
	//if(PathFileExists(add_Ca))
	//{
	//	ShellExecute(NULL,"open","cmd.exe","/c openssl req -days 3650 -nodes -new -keyout keys\\server.key -out keys\\server.csr -config openssl.cnf -batch && openssl ca -days 3650 -out keys\\server.crt -in keys\\server.csr -extensions server -config openssl.cnf -batch","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	DisplayRunTimeInfo("Create Server KEYs ing");
	//}
	//
	//Delay(500);

	//ShellExecute(NULL,"open","cmd.exe","/c del keys\\*.old","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);

	//if((!PathFileExists(aa_Server)) && (!PathFileExists(ab_Server)))
	//{
	//	DisplayRunTimeInfo("Create Server KEYS fail");
	//	return false;
	//}
	//else
	//{
	//	DisplayRunTimeInfo("Create Server KEY ok");
	//}

	////---------------------Create Client Key----------------------------

	//ShellExecute(NULL,"open","cmd.exe","/c del openssl.cnf","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(500);

	//ShellExecute(NULL,"open","cmd.exe","/c copy openssl.cnf.sample openssl.cnf.txt","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(1000);

	//CString strClientFile("D:\\Program Files\\OpenVPN\\easy-rsa\\openssl2.cnf.txt");
	//CString strClientFind("challengePassword_default=000000000001");
	//CString strClientReplace;

	//strClientReplace.Format("challengePassword_default=%s", m_strSN);

	//if(ReplaceTxt(strClientFile,strClientFind,strClientReplace))
	//{	
	//	DisplayRunTimeInfo("Verify Client PassWord OK");
	//}
	//else
	//{
	//	DisplayRunTimeInfo("Verify Client PassWord FAIL");
	//	return false;
	//}

	////-------------Start-------------
	//

	//ShellExecute(NULL,"open","cmd.exe","/c copy openssl.cnf.txt openssl.cnf","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//Delay(1000);

	//if(!PathFileExists(c_Openssl))
	//{
	//	DisplayRunTimeInfo("Create Client openssl.cnf fail");
	//	return false;
	//}
	//if(PathFileExists(c_Openssl))
	//{
	//	ShellExecute(NULL,"open","cmd.exe","/c openssl req -days 3650 -nodes -new -keyout keys\\client.key -out keys\\client.csr -config openssl.cnf -batch && openssl ca -out keys\\client.crt -in keys\\client.csr -config openssl.cnf -batch","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	DisplayRunTimeInfo("Create Client KEYS ing");
	//}
	//
	//Delay(500);

	//ShellExecute(NULL,"open","cmd.exe","/c del keys\\*.old","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);

	//if((!PathFileExists(aa_Client)) && (!PathFileExists(ab_Client)))
	//{
	//	DisplayRunTimeInfo("Create Client KEYS fail");
	//	return false;
	//}
	//else
	//{
	//	//ShellExecute(NULL,"open","cmd.exe","/c del openssly.cnf.txt","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	Delay(500);
	//	ShellExecute(NULL,"open","cmd.exe","/c del openssl.cnf.txt","D:\\Program Files\\OpenVPN\\easy-rsa\\",SW_HIDE);
	//	DisplayRunTimeInfo("Create Client KEYS ok");
	//}
	//
	////-------------End-------------

	//return true;
}

bool CRunInfo::ReplaceTxt(CString StrFileName, CString StrFind,CString StrReplace)
{
	CStdioFile TempFile, File;
	if(!File.Open(StrFileName, CFile::modeRead))
		return false;

	CString StrTempFileName("D:\\Program Files\\OpenVPN\\easy-rsa\\openssl.cnf.txt");
	if(!TempFile.Open(StrTempFileName, CFile::modeRead|CFile::modeReadWrite))
		return false;

	CString Str;
	while(File.ReadString(Str))
	{
		Str.Replace(StrFind,StrReplace);
		TempFile.WriteString(Str+"\n");
	}

	File.Close();
	TempFile.Close();
	CFile::Rename(StrTempFileName,"D:\\Program Files\\OpenVPN\\easy-rsa\\openssl.cnf.txt");

	return true;
}

bool CRunInfo::DutDownloadKey(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() && (0 != strcmp(m_IniFileInfo.szAllowBurn, "1")))
	{
		return true;
	}
	
	DisplayRunTimeInfo("---Start create keys---");

	if(!GainKey())
	{
		DisplayRunTimeInfo("Gain Keys fail");
		return false;
	}

	char szRmDefaultKey[MINBUFSIZE] = "";
	char	szDownPath[MINBUFSIZE] = "";
	
	sprintf_s(szDownPath,MINBUFSIZE, "cd /tmp/openvpn");
	sprintf_s(szRmDefaultKey,MINBUFSIZE,"rm %s %s %s %s %s %s","ca.crt","dh1024.pem","server.key","server.crt","client.key","client.crt");
		
	if(!SendDutCommand(szDownPath,"#",8000))
	{
		return false;
	}
	
	
	if(SendDutCommand("ls","#",8000))
	{
		if(!SendDutCommand(szRmDefaultKey,"#",8000))
		{
			return false;
		}
	}

	DisplayRunTimeInfo("---Start TFTP Down KEYS---");

	char    szDownKeyCa[MINBUFSIZE]  = "";
	char	szDownKeyDh[MINBUFSIZE]	 = "";
	char	szDownSerKey[MINBUFSIZE] = "";
	char	szDownSerCrt[MINBUFSIZE] = "";
	char	szDownCliKey[MINBUFSIZE] = "";
	char	szDownCliCrt[MINBUFSIZE] = "";

	/*char	szLoadBurnrf[MINBUFSIZE] = "";
	char	szLoadDumprf[MINBUFSIZE] = "";*/

	sprintf_s(szDownKeyCa,MINBUFSIZE,"tftp %s -g -r ca.crt",m_IniFileInfo.szTftpAddress);
	sprintf_s(szDownKeyDh,MINBUFSIZE,"tftp %s -g -r dh1024.pem",m_IniFileInfo.szTftpAddress);
	sprintf_s(szDownSerKey,MINBUFSIZE,"tftp %s -g -r server.key",m_IniFileInfo.szTftpAddress);
	sprintf_s(szDownSerCrt,MINBUFSIZE,"tftp %s -g -r server.crt",m_IniFileInfo.szTftpAddress);
	sprintf_s(szDownCliKey,MINBUFSIZE,"tftp %s -g -r client.key",m_IniFileInfo.szTftpAddress);
	sprintf_s(szDownCliCrt,MINBUFSIZE,"tftp %s -g -r client.crt",m_IniFileInfo.szTftpAddress);

	/*sprintf_s(szLoadBurnrf,MINBUFSIZE,"tftp %s -g -r burnrf",m_IniFileInfo.szTftpAddress);
	sprintf_s(szLoadDumprf,MINBUFSIZE,"tftp %s -g -r dumprf",m_IniFileInfo.szTftpAddress);*/

	if(!SendDutCommand(szDownPath,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("cd /tmp/openvpn fail");
		return false;
	}

	if(!SendDutCommand(szDownKeyCa,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("Copy ca.crt fail");
		return false;
	}

	if(!SendDutCommand(szDownKeyDh,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("Copy dh1024.pem fail");
		return false;
	}

	if(!SendDutCommand(szDownSerKey,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("Copy server.key fail");
		return false;
	}

	if(!SendDutCommand(szDownSerCrt,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("Copy server.crt fail");
		return false;
	}

	if(!SendDutCommand(szDownCliKey,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("Copy client.key fail");
		return false;
	}

	if(!SendDutCommand(szDownCliCrt,"#",8000))
	{
		Delay(100);
		DisplayRunTimeInfo("Copy client.crt fail");
		return false;
	}

	/*if(!SendDutCommand(szLoadBurnrf,"#",8000))
	{
		Delay(1000);
		DisplayRunTimeInfo("Copy burnrf fail");
		return false;
	}

	if(!SendDutCommand(szLoadDumprf,"#",8000))
	{
		Delay(1000);
		DisplayRunTimeInfo("Copy dumprf fail");
		return false;
	}*/
	
	Delay(1000);
	DisplayRunTimeInfo("Get OpenVPN checksum pass!");
	
	if(DutGetVPNPar())
	{
		if(GetTxtSum())
		{
			DisplayRunTimeInfo("Check SumTxt & DUTSum OK!");
			return true;
		}
		else
		{
			DisplayRunTimeInfo("Check SumTxt & DUTSum NG!");
			return false;
		}
	}
	else
	{
		return false;
	}

					
}

bool CRunInfo::DutBurnBF(void)
{
	// No sfis on, no write action, return directly
	if(!GetIsSfis() && (0 != strcmp(m_IniFileInfo.szAllowBurn, "1")))
	{
		return true;
	}

	if(!SendDutCommand("burnrf","burnrf OK",8000))
	{
		return false;
	}

	DisplayRunTimeInfo("Burn beamoforming parameter ok");

	if(DutGetBFPar())
	{
		DisplayRunTimeInfo("Get BeamForming parameter pass!");
	}
	else
	{
		DisplayRunTimeInfo("Get BeamForming parameter failed!");
		return false;
	}
	return true;
	
	/*bool BurnrfFlag = true;

	DisplayRunTimeInfo("---Start burn beamforming---");

	if(SendDutCommand("dumprf","rpcal5gb3",3000))
	{
		BurnrfFlag = false;
		DisplayRunTimeInfo("board_date haven implicit beam forming parameter!");
		DisplayRunTimeInfo("Don't Need burnrf BF parameter.");
		DisplayRunTimeInfo("Start getting implicit beam forming parameter");

		if(DutGetBFPar())
		{
			DisplayRunTimeInfo("Get BeamForming parameter pass!");
		}
		else
		{
			DisplayRunTimeInfo("Get BeamForming parameter failed!");
			return false;
		}

		return true;
	}
	else
	{
		BurnrfFlag = true;
		DisplayRunTimeInfo("board_date haven't implicit beam forming parameter!");
		DisplayRunTimeInfo("Need burnrf BF parameter.");
	}

	if(BurnrfFlag)
	{
		char szBurnBF[MINBUFSIZE] = "";

		sprintf_s(szBurnBF, MINBUFSIZE, "burnrf");

		if(!SendDutCommand(szBurnBF,"burnrf OK",8000))
		{
			return false;
		}

		DisplayRunTimeInfo("Burn beamoforming parameter ok");

		if(DutGetBFPar())
		{
			DisplayRunTimeInfo("Get BeamForming parameter pass!");
		}
		else
		{
			DisplayRunTimeInfo("Get BeamForming parameter failed!");
			return false;
		}
		return true;
	}*/
}

bool CRunInfo::DutCheckBF(void)
{
	DisplayRunTimeInfo("---Start check BF---");

	//szBF.Trim();
	strBF2G = strBF.Mid(0,6);
	strBF5Gb0 = strBF.Mid(6,6);
	strBF5Gb1 = strBF.Mid(12,6);
	strBF5Gb2 = strBF.Mid(18,6);
	strBF5Gb3 = strBF.Mid(24,6);

	ShowMsg(strBF2G);
	ShowMsg(strBF5Gb0);
	ShowMsg(strBF5Gb1);
	ShowMsg(strBF5Gb2);
	ShowMsg(strBF5Gb3);

	if((strBF2G == "")&&(strBF5Gb0 == "")&&(strBF5Gb1 == "")&&(strBF5Gb2 == "")&&(strBF5Gb3 == ""))
	{
		DisplayRunTimeInfo("BeamForming parameter is empty");
		return false;
	}

	/*if(!SendDutCommand("cd /tmp/openvpn","#",8000))
	{
		Delay(300);
		return false;
	}

	if(!SendDutCommand("chmod 777 dumprf","#",8000))
	{
		Delay(300);
		return false;
	}*/

	if(SendDutCommand("dumprf","#",8000))
	{
		str = m_strStoreEthernetData;
	}
	else
	{
		return false;
	}


	GetChksumValue(str,"2g=","pci/2/1/rpcal5gb0",szBF2g);
	char *bf2g = szBF2g.GetBuffer();
	DisplayRunTimeInfo(bf2g);

	GetChksumValue(str,"5gb0=","pci/2/1/rpcal5gb1",szBF5gb0);
	char *bf5gb0 = szBF5gb0.GetBuffer();
	DisplayRunTimeInfo(bf5gb0);

	GetChksumValue(str,"5gb1=","pci/2/1/rpcal5gb2",szBF5gb1);
	char *bf5gb1 = szBF5gb1.GetBuffer();
	DisplayRunTimeInfo(bf5gb1);

	GetChksumValue(str,"5gb2=","pci/2/1/rpcal5gb3",szBF5gb2);
	char *bf5gb2 = szBF5gb2.GetBuffer();
	DisplayRunTimeInfo(bf5gb2);

	GetChksumValue(str,"5gb3=","#",szBF5gb3);
	char *bf5gb3 = szBF5gb3.GetBuffer();
	DisplayRunTimeInfo(bf5gb3);

	//2.4G BF
	szBF2g.Trim();
	int l_BF2gLength = 0;
	l_BF2gLength = szBF2g.GetLength();

	int iTemp = 6 - l_BF2gLength;
	if(iTemp != 0)
	{		
		for(int i = 0; i < iTemp; i++)
		{
			szBF2g = "0" + szBF2g;
		}
	}
	
	//5G band1 BF
	szBF5gb0.Trim();
	int l_BF5gb0Length = 0;
	l_BF5gb0Length = szBF5gb0.GetLength();

	iTemp = 6 - l_BF5gb0Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb0 = "0" + szBF5gb0;
		}
	}

	//5G band2 BF
	szBF5gb1.Trim();
	int l_BF5gb1Length = 0;
	l_BF5gb1Length = szBF5gb1.GetLength();

	iTemp = 6 - l_BF5gb1Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb1 = "0" + szBF5gb1;
		}
	}

	//5G band3 BF
	szBF5gb2.Trim();
	int l_BF5gb2Length = 0;
	l_BF5gb2Length = szBF5gb2.GetLength();

	iTemp = 6 - l_BF5gb2Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb2 = "0" + szBF5gb2;
		}
	}

	//5G band4 BF
	szBF5gb3.Trim();
	int l_BF5gb3Length = 0;
	l_BF5gb3Length = szBF5gb3.GetLength();

	iTemp = 6 - l_BF5gb3Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb3 = "0" + szBF5gb3;
		}
	}

	if(strstr(strBF2G,szBF2g) != NULL)
	{
		if(strstr(strBF5Gb0,szBF5gb0) != NULL)
		{
			if(strstr(strBF5Gb1,szBF5gb1) != NULL)
			{
				if(strstr(strBF5Gb2,szBF5gb2) != NULL)
				{
					if(strstr(strBF5Gb3,szBF5gb3) != NULL)
					{
						DisplayRunTimeInfo("BeamForming parameter Check ok !");
						return true;
					}else{return false;}
				}else{return false;}
			}else{return false;}
		}else{return false;}
	}
	else
	{
		DisplayRunTimeInfo("BeamForming parameter check failed !");
		return false;
	}
}

bool CRunInfo::DutCheckOpenVPN(void)
{
	DisplayRunTimeInfo("---Start check OpenVPN---");
	
	szOpenVPN.Trim();
	szVPNca = szOpenVPN.Mid(0,10);
	szVPNclitc = szOpenVPN.Mid(10,10);
	szVPNserc = szOpenVPN.Mid(20,10);
	szVPNclitk = szOpenVPN.Mid(30,10);
	szVPNserk = szOpenVPN.Mid(40,10);
	szVPNdh = szOpenVPN.Mid(50,10);

	ShowMsg(szVPNca);
	ShowMsg(szVPNclitc);
	ShowMsg(szVPNserc);
	ShowMsg(szVPNclitk);
	ShowMsg(szVPNserk);
	ShowMsg(szVPNdh);

	if((szVPNca == "")&&(szVPNclitc == "")&&(szVPNserc == "")&&(szVPNclitk == "")&&(szVPNserk == "")&&(szVPNdh == ""))
	{
		DisplayRunTimeInfo("OpenVPN keys checksum are empty");
		return false;
	}

	SendDutCommand("cd /tmp/openvpn","#",8000);
	SendDutCommand("getopenvpnsum","#",8000);

	if(strstr(m_strStoreEthernetData,szVPNca) != NULL)
	{
		if(strstr(m_strStoreEthernetData,szVPNclitc) != NULL)
		{
			if(strstr(m_strStoreEthernetData,szVPNserc) != NULL)
			{
				if(strstr(m_strStoreEthernetData,szVPNclitk) != NULL)
				{
					if(strstr(m_strStoreEthernetData,szVPNserk) != NULL)
					{
						if(strstr(m_strStoreEthernetData,szVPNdh) != NULL)
						{
							DisplayRunTimeInfo("OpenVPN chksum check ok!");
							return true;
						}else{return false;}
					}else{return false;}
				}else{return false;}
			}else{return false;}
		}else{return false;}
	}
	else
	{
		DisplayRunTimeInfo("OpenVPN chksum check failed!");
		return false;
	}
}

bool CRunInfo::DutGetBFPar(void)
{
	szBF2g = "0x0000";
	szBF5gb0 = "0x0000";
	szBF5gb1 = "0x0000";
	szBF5gb2 = "0x0000";
	szBF5gb3 = "0x0000";
	szBF = "0x00000x00000x00000x00000x0000";

	/*if(!SendDutCommand("cd /tmp/openvpn","#",8000))
	{
		return false;
	}
	Delay(200);

	if(!SendDutCommand("chmod 777 dumprf","#",8000))
	{
		return false;
	}
	Delay(200);*/	

	//if(SendDutCommand("./dumprf","#",8000))
	if(SendDutCommand("dumprf","#",8000))
	{
		str = m_strStoreEthernetData;
	}
	else
	{
		return false;
	}

	GetChksumValue(str,"2g=","pci/2/1/rpcal5gb0",szBF2g);
	char *bf2g = szBF2g.GetBuffer();
	DisplayRunTimeInfo(bf2g);

	GetChksumValue(str,"5gb0=","pci/2/1/rpcal5gb1",szBF5gb0);
	char *bf5gb0 = szBF5gb0.GetBuffer();
	DisplayRunTimeInfo(bf5gb0);

	GetChksumValue(str,"5gb1=","pci/2/1/rpcal5gb2",szBF5gb1);
	char *bf5gb1 = szBF5gb1.GetBuffer();
	DisplayRunTimeInfo(bf5gb1);

	GetChksumValue(str,"5gb2=","pci/2/1/rpcal5gb3",szBF5gb2);
	char *bf5gb2 = szBF5gb2.GetBuffer();
	DisplayRunTimeInfo(bf5gb2);

	GetChksumValue(str,"5gb3=","#",szBF5gb3);
	char *bf5gb3 = szBF5gb3.GetBuffer();
	DisplayRunTimeInfo(bf5gb3);

	//2.4G BF
	szBF2g.Trim();
	int l_BF2gLength = 0;
	l_BF2gLength = szBF2g.GetLength();

	int iTemp = 6 - l_BF2gLength;
	if(iTemp != 0)
	{		
		for(int i = 0; i < iTemp; i++)
		{
			szBF2g = "0" + szBF2g;
		}
	}
	
	//5G band1 BF
	szBF5gb0.Trim();
	int l_BF5gb0Length = 0;
	l_BF5gb0Length = szBF5gb0.GetLength();

	iTemp = 6 - l_BF5gb0Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb0 = "0" + szBF5gb0;
		}
	}

	//5G band2 BF
	szBF5gb1.Trim();
	int l_BF5gb1Length = 0;
	l_BF5gb1Length = szBF5gb1.GetLength();

	iTemp = 6 - l_BF5gb1Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb1 = "0" + szBF5gb1;
		}
	}

	//5G band3 BF
	szBF5gb2.Trim();
	int l_BF5gb2Length = 0;
	l_BF5gb2Length = szBF5gb2.GetLength();

	iTemp = 6 - l_BF5gb2Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb2 = "0" + szBF5gb2;
		}
	}

	//5G band4 BF
	szBF5gb3.Trim();
	int l_BF5gb3Length = 0;
	l_BF5gb3Length = szBF5gb3.GetLength();

	iTemp = 6 - l_BF5gb3Length;
	if(iTemp != 0)
	{
		for(int i = 0; i < iTemp; i++)
		{
			szBF5gb3 = "0" + szBF5gb3;
		}
	}


	szBF = szBF2g + szBF5gb0 + szBF5gb1 + szBF5gb2 + szBF5gb3;
	
	//szBF.Trim();
	int BF_len = szBF.GetLength();
	char *BF = szBF.GetBuffer();

	if((BF_len = 30)&&(szBF2g != "")&&(szBF5gb0 != "")&&(szBF5gb1 != "")&&(szBF5gb2 != "")&&(szBF5gb3 != ""))
	{
		//AfxMessageBox(szBF);
		DisplayRunTimeInfo(BF);
		return true;
	}
	else
	{
		return false;
	}
	/*if((szBF2g != "")&&(szBF5gb0 != "")&&(szBF5gb1 != "")&&(szBF5gb2 != "")&&(szBF5gb3 != ""))
	{
		ShowMsg(szBF2g);
		ShowMsg(szBF5gb0);
		ShowMsg(szBF5gb1);
		ShowMsg(szBF5gb2);
		ShowMsg(szBF5gb3);
		return true;
	}
	else
	{
		return false;
	}*/
}

void CRunInfo::GetChksumValue(CString str_data, CString str_bkey, CString str_akey, CString &str_chksum)
{
	CString str = str_data;
	CString str_result;

	int Before_key = -1;
	Before_key = str.Find(str_bkey);
	int str_len = str_bkey.GetLength();

	int After_key = -1;
	After_key = str.Find(str_akey);

	str_result = str.Mid(Before_key+str_len,After_key-Before_key-str_len);
	str_result.Trim();
	str_chksum = str_result;
	return;
}

bool CRunInfo::DutGetVPNPar(void)
{
	szVPNca = "0x00000000";
	szVPNclitc = "0x00000000";
	szVPNserc = "0x00000000";
	szVPNclitk = "0x00000000";
	szVPNserk = "0x00000000";
	szVPNdh = "0x00000000";
	szOpenVPN = "0x000000000x000000000x000000000x000000000x000000000x00000000";

	SendDutCommand("cd /tmp/openvpn","#",8000);
	Delay(500);

	if(SendDutCommand("getopenvpnsum","#",8000))
	{
		svpn = m_strStoreEthernetData;
	}

	GetChksumValue(svpn,"ca.crt = ","sum of client.crt",szVPNca);
	char *vpnca = szVPNca.GetBuffer();
	DisplayRunTimeInfo(vpnca);

	GetChksumValue(svpn,"client.crt = ","sum of server.crt",szVPNclitc);
	char *vpnclitc = szVPNclitc.GetBuffer();
	DisplayRunTimeInfo(vpnclitc);

	GetChksumValue(svpn,"server.crt = ","sum of client.key",szVPNserc);
	char *vpnserc = szVPNserc.GetBuffer();
	DisplayRunTimeInfo(vpnserc);

	GetChksumValue(svpn,"client.key = ","sum of server.key",szVPNclitk);
	char *vpnclitk = szVPNclitk.GetBuffer();
	DisplayRunTimeInfo(vpnclitk);

	GetChksumValue(svpn,"server.key = ","sum of dh1024.pem",szVPNserk);
	char *vpnserk = szVPNserk.GetBuffer();
	DisplayRunTimeInfo(vpnserk);

	GetChksumValue(svpn,"dh1024.pem = ","#",szVPNdh);
	char *vpndh = szVPNdh.GetBuffer();
	DisplayRunTimeInfo(vpndh);

	szOpenVPN = szVPNca + szVPNclitc + szVPNserc + szVPNclitk + szVPNserk + szVPNdh;

	szOpenVPN.Trim();
	int VPN_len = szOpenVPN.GetLength();
	char *VPN = szOpenVPN.GetBuffer();

	if((VPN_len = 30)&&(szVPNca != "")&&(szVPNclitc != "")&&(szVPNserc != "")&&(szVPNclitk != "")&&(szVPNserk != "")&&(szVPNdh != ""))
	{
		//AfxMessageBox(szOpenVPN);
		DisplayRunTimeInfo(VPN);
		return true;
	}
	else
	{
		return false;
	}
}

bool CRunInfo::GetTxtSum(void)
{
	char f_SumPath[MINBUFSIZE] = "";
	char f_CurrentPath[MINBUFSIZE] = "";

	memset(TxtSum,0,5120);

	GetCurrentDirectory(sizeof(f_CurrentPath), f_CurrentPath);
	sprintf_s(f_SumPath,sizeof(f_SumPath),"%s\\keys\\Sum.txt",f_CurrentPath);

	FILE *ff;
	char buf[5120] = {0};
	fopen_s(&ff, f_SumPath, "r");
	fread(buf,sizeof(char),5120,ff);

	bool iResult = false;

	if(strstr(buf,szVPNca) != NULL)
	{
		//AfxMessageBox(szVPNca);
		if(strstr(buf,szVPNclitc) != NULL)
		{
			//AfxMessageBox(szVPNclitc);
			if(strstr(buf,szVPNserc) != NULL)
			{
				//AfxMessageBox(szVPNserc);
				if(strstr(buf,szVPNclitk) != NULL)
				{
					//AfxMessageBox(szVPNclitk);
					if(strstr(buf,szVPNserk) != NULL)
					{
						//AfxMessageBox(szVPNserk);
						if(strstr(buf,szVPNdh) != NULL)
						{
							//AfxMessageBox(szVPNdh);
							DisplayRunTimeInfo("OpenVPN chksum check ok!");
							iResult = true;
						}						
					}
				}
			}
		}
	}

	fclose(ff);
	return iResult;
}

bool CRunInfo::DutPerPT(void)
{
	CString rpcal2g;
	CString rpcal5gb0;
	CString rpcal5gb1;
	CString rpcal5gb2;
	CString rpcal5gb3;

	if(SendDutCommand("nvram get 0:rpcal2g","#",3000))
	{
		GetMatchString(m_strStoreEthernetData,"0:rpcal2g","#",rpcal2g);
		Delay(200);
	}
	else
	{
		return false;
	}

	if(SendDutCommand("nvram get 1:rpcal5gb0","#",3000))
	{
		GetMatchString(m_strStoreEthernetData,"1:rpcal5gb0","#",rpcal5gb0);
		Delay(200);
	}
	else
	{
		return false;
	}

	if(SendDutCommand("nvram get 1:rpcal5gb1","#",3000))
	{
		GetMatchString(m_strStoreEthernetData,"1:rpcal5gb1","#",rpcal5gb0);
		Delay(200);
	}
	else
	{
		return false;
	}

	if(SendDutCommand("nvram get 1:rpcal5gb2","#",3000))
	{
		GetMatchString(m_strStoreEthernetData,"1:rpcal5gb2","#",rpcal5gb0);
		Delay(200);
	}
	else
	{
		return false;
	}

	if(SendDutCommand("nvram get 1:rpcal5gb3","#",3000))
	{
		GetMatchString(m_strStoreEthernetData,"1:rpcal5gb3","#",rpcal5gb0);
		Delay(200);
	}
	else
	{
		return false;
	}

	char m_rpcal2g[128] = "";
	char m_rpcal5gb0[128] = "";
	char m_rpcal5gb1[128] = "";
	char m_rpcal5gb2[128] = "";
	char m_rpcal5gb3[128] = "";

	sprintf_s(m_rpcal2g, sizeof(m_rpcal2g), "nvram set pci/1/1/rpcal2g=%s", rpcal2g);
	sprintf_s(m_rpcal5gb0, sizeof(m_rpcal5gb0), "nvram set pci/2/1/rpcal5gb0=%s", rpcal5gb0);
	sprintf_s(m_rpcal5gb1, sizeof(m_rpcal5gb1), "nvram set pci/2/1/rpcal5gb1=%s", rpcal5gb1);
	sprintf_s(m_rpcal5gb2, sizeof(m_rpcal5gb2), "nvram set pci/2/1/rpcal5gb2=%s", rpcal5gb2);
	sprintf_s(m_rpcal5gb3, sizeof(m_rpcal5gb3), "nvram set pci/2/1/rpcal5gb3=%s", rpcal5gb3);
	Delay(200);

	SendDutCommand(m_rpcal2g, "#", 2000);
	Delay(100);

	SendDutCommand(m_rpcal5gb0, "#", 2000);
	Delay(100);

	SendDutCommand(m_rpcal5gb1, "#", 2000);
	Delay(100);

	SendDutCommand(m_rpcal5gb2, "#", 2000);
	Delay(100);

	SendDutCommand(m_rpcal5gb3, "#", 2000);
	Delay(100);
	
	SendDutCommand("nvram commit", "#", 2000);
	Delay(7000);

	return true;

}

void CRunInfo::GetMatchString(CString str_info, CString str_before, CString str_after, CString &str_match)
{
	CString  str=str_info;

	int before_pos=-1;
	before_pos=str.Find(str_before);

	int str_len_before=str_before.GetLength();
	CString  str_finsub;
	str_finsub=str.Mid(before_pos+str_len_before,str.GetLength());

	int after_pos=-1;
	after_pos=str_finsub.Find(str_after);
	int str_len_after=str_after.GetLength();

	CString  str_find="";
	if(after_pos>=0)
	{
		str_find=str_finsub.Mid(0,after_pos);
	}
	str_find.Replace("\r\n","");
	str_find.Trim();
	str_match=str_find;
}



//add for LED Test
VOID CRunInfo::bLedCCDSocketClose()
{
	if(s_LedCCDSocket != INVALID_SOCKET)
	{
		closesocket(s_LedCCDSocket);
	}
	WSACleanup();
}


BOOL CRunInfo::bLedCCDSocketInit( HWND hWnd, UINT iport, const char* cServerIP)
{
	WSADATA wsaData ;
	if(WSAStartup( MAKEWORD(2, 2), &wsaData ) != 0 )
	{
		return FALSE;
	}

	
	if(inet_addr(cServerIP) == INADDR_NONE)
	{
		return FALSE;
	}

	s_LedCCDSocket = socket(AF_INET, SOCK_STREAM, 0);
	//sock = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0);
	if(INVALID_SOCKET == s_LedCCDSocket)
	{
		DisplayRunTimeInfo("Create socket fail\n");
		return FALSE;
	}
	assert(s_LedCCDSocket != INVALID_SOCKET);

	sockaddr_in addr;
	addr.sin_family = AF_INET ;
	addr.sin_addr.S_un.S_addr = inet_addr (cServerIP) ;
	addr.sin_port = htons(iport);

	
	int revt = connect(s_LedCCDSocket, (sockaddr*)&addr, sizeof(addr));
	
	if(SOCKET_ERROR==revt)
	{
		DisplayRunTimeInfo("Bind sock fail\n");
		return FALSE;
	}
	
	if(SOCKET_ERROR ==WSAAsyncSelect(s_LedCCDSocket, hWnd, WM_LED_CCD_SOCK_TCP, FD_READ))
	{
		DisplayRunTimeInfo("Create network event fail\n ");
		return FALSE;
	}

	return TRUE;
}

BOOL CRunInfo::bSendCmdToLedCCDSocket( char *cCmdBuffer, char* cReturnKeyWord,int iTimeOutWaitSocketRespones )
{
	int iLength = strlen(cCmdBuffer);
	int i = 0 ;
	int ret ;
	s_LedCCDBuffer.Empty();	
	if(iLength == 0)
	{
		return TRUE;
	}
	while(iLength > 0)
	{
		ret = send(s_LedCCDSocket, &(cCmdBuffer[i]), iLength, 0);
		if(ret == 0)
		{
			break;
		}
		else if(ret == SOCKET_ERROR)
		{
			return FALSE;
		}
		iLength -= ret ;
		i += ret ;
	}

	int iRtyNum = iTimeOutWaitSocketRespones / 200 ;
	while(iRtyNum > 0)
	{
		iRtyNum--;
		if(strstr(s_LedCCDBuffer, cReturnKeyWord) == NULL )
		{
			Sleep(200);
		}
		else
		{
			break;
		}
	}

	if(iRtyNum <= 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

LRESULT CRunInfo::bRecvFromLedCCDSocket( WPARAM wParam, LPARAM lParam )
{
	switch(WSAGETSELECTEVENT(lParam)) 
	{
	case FD_READ:
		s_LedCCDBuffer.Empty();
		//char  ReceiveBuff[MAXBUFSIZE*10 + 1];
		//memset(ReceiveBuff,0,MAXBUFSIZE*10 + 1);
		char  ReceiveBuff[MAXBUFSIZE];
		memset(ReceiveBuff,0,MAXBUFSIZE);
		DWORD NumberOfBytesRecvd = 0;
		DWORD dwflag = 0;
		Sleep(200);
		int iretCode;
		iretCode = recv(s_LedCCDSocket, ReceiveBuff, MAXBUFSIZE*10, 0);
		DisplayRunTimeInfo(ReceiveBuff);
		s_LedCCDBuffer += ReceiveBuff;
		delete []ReceiveBuff;		
		break;
	}	

	return 0 ;
}

int CRunInfo::JudgeColor(char *szBuf)
{
	/*
	2 represents green
	3 represents red
	4 represents orange
	5 represents blue
	6 represents black
	*/

	//100.000000_126.000000_109.000000
	int ColorResult=0;
	double R_Value=-1;
	double G_Value=-1;
	double B_Value=-1;
	char seps[]   = "_";
	char *token1,
	*next_token1;
	token1 = strtok_s(szBuf,seps,&next_token1);
	if(token1!=NULL)
	{
		R_Value=atof(token1);
	}
	//DisplayRunTimeInfo("red");
	//DisplayRunTimeInfo(token1);
	token1 = strtok_s( NULL, seps, &next_token1);
	if(token1!=NULL)
	{
		G_Value=atof(token1);
	}
	//DisplayRunTimeInfo("green");
	//DisplayRunTimeInfo(token1);
	token1 = strtok_s( NULL, seps, &next_token1);
	if(token1!=NULL)
	{
		B_Value=atof(token1);
	}
	//DisplayRunTimeInfo("blue");
	//DisplayRunTimeInfo(token1);

	//original judge in Gray Image
	/*if(R_Value==G_Value && G_Value==B_Value)
	{
		if(R_Value>100)
		{
			return 1;//Green in Gray mode
		}
		if(R_Value<100 && R_Value>40)
		{
			return 2;//Red in Gray mode
		}
		if(R_Value<40)
		{
			return 3;//Black in Gray mode
		}
	}*/

	//original judge in Color mode
	/*if(R_Value>150 && G_Value<100 && B_Value<100)
	{
		ColorResult=2;//Red in Color mode
		break;
	}
	if(R_Value<100 && G_Value>150 && B_Value<100)
	{
		ColorResult=1;//Green in Color mode
	}
	if(R_Value<100 && G_Value<100 && B_Value<100)
	{
		ColorResult=3;//Black in Color mode
		break;
	}*/
	//

	//modify RGB spec according to usb camera
	if(R_Value<254 && G_Value>150 && B_Value<221)
	{
		ColorResult=2;//Green in Color mode
	}
	if(R_Value>150 && G_Value<220 && B_Value<253)
	{
		ColorResult=3;//Red in Color mode
	}
	if(R_Value>180 && G_Value>80 && B_Value<200)
	{
		ColorResult=4;//Orange in Color mode
	}
	if(R_Value<169 && G_Value>77 && B_Value>120)
	{
		ColorResult=5;//Blue in Color mode
	}
	if(R_Value<100 && G_Value<100 && B_Value<100)
	{
		ColorResult=6;//Black in Color mode
	}
	//end

	return ColorResult;
}

int  CRunInfo::iLedCCDTestSixLED()
{
	/*
	2 represents green
	3 represents red
	4 represents yellow
	5 represents blue
	6 represents black
	*/
	int iReturn = 0;
//for manual judge LED

	//check 3 ethernet led light
	//if(IDYES==AfxMessageBox(_T("圖片上面前三個燈是否為綠色？\nAre the first three LED green in pic?"),MB_YESNO|MB_ICONQUESTION))
	if(IDYES==AfxMessageBox(_T("產品連接網線的三個燈是否為綠色？\nAre the three Ethernet LED green in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		
	}
	else
	{
		return 0;
	}
	
	//start test system LED
	
	SendDutCommand("gpio 1 1","#",1500);
	SendDutCommand("gpio 2 1","#",1500);
	SendDutCommand("gpio 3 1","#",1500);
	DisplayRunTimeInfo("System LED All Off!");
	
	//only red light
	
	SendDutCommand("gpio 1 0","#",1500);
	if(IDYES==AfxMessageBox(_T("產品上面系統指示燈是否為紅色？\nIs the system LED red in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		
	}
	else
	{
		return 0;
	}


	//only orange light
	SendDutCommand("gpio 1 1","#",1500);
	SendDutCommand("gpio 2 0","#",1500);
	
	if(IDYES==AfxMessageBox(_T("產品上面系統指示燈是否為橙色？\nIs the system LED orange in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		
	}
	else
	{
		return 0;
	}


	//only blue light
	SendDutCommand("gpio 2 1","#",1500);
	SendDutCommand("gpio 3 0","#",1500);
	
	if(IDYES==AfxMessageBox(_T("產品上面系統指示燈是否為藍色？\nIs the system LED blue in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		iReturn=1;
	}
	else
	{
		return 0;
	}

	return iReturn;	
//for manual judge LED end



//for auto judge LED
	//int iReturn = 0;

	char szTempBuffer[MAXBUFSIZE];
	memset(szTempBuffer, 0, MAXBUFSIZE);
	sprintf_s(szTempBuffer, MAXBUFSIZE, "DetectColorX:%sY:%sW:%sH:%s\r\n",LedCcdTestInfo.LAN1x,LedCcdTestInfo.LAN1y,LedCcdTestInfo.LAN1w,LedCcdTestInfo.LAN1h);
	
	DisplayRunTimeInfo(szTempBuffer);
	
	int iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		
		if(iRtyNum<2)
		{
			return 0; //represent fail
		}
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "_", 6000 );
		if(bResult == false)
		{
			DisplayRunTimeInfo("Send LAN1 LED CCD cmd failed!");
			Sleep(500);
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Send LAN1 LED CCD cmd passed!");
		}

		Sleep(1000);		
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
		int judgement;
		judgement=JudgeColor(s_LedCCDBuffer.GetBuffer());
		if(6 == judgement)
		{
			DisplayRunTimeInfo("LAN1 LED Test failed!");
			return 0; //represent fail
		}
		else
		{
			if(2 == judgement)
			{
				DisplayRunTimeInfo("LAN1 LED green in range!");
			}
			else
			{
				DisplayRunTimeInfo("LAN1 LED green not in range!");
			}
			DisplayRunTimeInfo("LAN1 LED Test passed!");
			break;
		}
	}

	memset(szTempBuffer, 0, MAXBUFSIZE);
	sprintf_s(szTempBuffer, MAXBUFSIZE, "DetectColorX:%sY:%sW:%sH:%s\r\n",LedCcdTestInfo.LAN2x,LedCcdTestInfo.LAN2y,LedCcdTestInfo.LAN2w,LedCcdTestInfo.LAN2h);
	
	DisplayRunTimeInfo(szTempBuffer);
	
	iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		if(iRtyNum<2)
		{
			return 0; //represent fail
		}
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "_", 6000 );
		if(bResult == false)
		{
			DisplayRunTimeInfo("Send LAN2 LED CCD cmd failed!");
			Sleep(500);
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Send LAN2 LED CCD cmd passed!");
		}

		Sleep(1000);		
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
		int judgement;
		judgement=JudgeColor(s_LedCCDBuffer.GetBuffer());
		if(6 == judgement)
		{
			DisplayRunTimeInfo("LAN2 LED Test failed!");
			return 0; //represent fail
		}
		else
		{
			if(2 == judgement)
			{
				DisplayRunTimeInfo("LAN2 LED green in range!");
			}
			else
			{
				DisplayRunTimeInfo("LAN2 LED green not in range!");
			}
			DisplayRunTimeInfo("LAN2 LED Test passed!");
			break;
		}
	}


	memset(szTempBuffer, 0, MAXBUFSIZE);
	sprintf_s(szTempBuffer, MAXBUFSIZE, "DetectColorX:%sY:%sW:%sH:%s\r\n",LedCcdTestInfo.WAN1x,LedCcdTestInfo.WAN1y,LedCcdTestInfo.WAN1w,LedCcdTestInfo.WAN1h);
	
	DisplayRunTimeInfo(szTempBuffer);
	
	iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		if(iRtyNum<2)
		{
			return 0; //represent fail
		}
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "_", 6000 );
		if(bResult == false)
		{
			DisplayRunTimeInfo("Send WAN1 LED CCD cmd failed!");
			Sleep(500);
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Send WAN1 LED CCD cmd passed!");
		}

		Sleep(1000);		
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
		int judgement;
		judgement=JudgeColor(s_LedCCDBuffer.GetBuffer());
		if(6 == judgement)
		{
			DisplayRunTimeInfo("WAN1 LED Test failed!");
			return 0; //represent fail
		}
		else
		{
			if(2 == judgement)
			{
				DisplayRunTimeInfo("WAN1 LED green in range!");
			}
			else
			{
				DisplayRunTimeInfo("WAN1 LED green not in range!");
			}
			DisplayRunTimeInfo("WAN1 LED Test passed!");
			break;
		}
	}
	
	

	//check 3 ethernet led light
	//if(IDYES==AfxMessageBox(_T("圖片上面前三個燈是否為綠色？\nAre the first three LED green in pic?"),MB_YESNO|MB_ICONQUESTION))
	/*if(IDYES==AfxMessageBox(_T("產品連接網線的三個燈是否為綠色？\nAre the three Ethernet LED green in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		
	}
	else
	{
		return 0;
	}*/
	
	//end

	
	//start test system LED
	memset(szTempBuffer, 0, MAXBUFSIZE);
	sprintf_s(szTempBuffer, MAXBUFSIZE, "DetectColorX:%sY:%sW:%sH:%s\r\n",LedCcdTestInfo.SYSLx,LedCcdTestInfo.SYSLy,LedCcdTestInfo.SYSLw,LedCcdTestInfo.SYSLh);
	DisplayRunTimeInfo(szTempBuffer);
	
	SendDutCommand("gpio 1 1","#",1500);
	SendDutCommand("gpio 2 1","#",1500);
	SendDutCommand("gpio 3 1","#",1500);
	DisplayRunTimeInfo("System LED All Off!");
	
	//only red light
	
	SendDutCommand("gpio 1 0","#",1500);
	
	iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		if(iRtyNum<2)
		{
			return 0; //represent fail
		}
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "_", 6000 );
		if(bResult == false)
		{
			DisplayRunTimeInfo("Set LED Red CCD cmd failed!");
			Sleep(500);
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Set LED Red CCD cmd passed!");
		}

		Sleep(1000);
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
		int judgement;
		judgement=JudgeColor(s_LedCCDBuffer.GetBuffer());
		if(6 == judgement)
		{
			DisplayRunTimeInfo("Red LED Test failed!");
			return 0; //represent fail
		}
		else
		{
			if(3 == judgement)
			{
				DisplayRunTimeInfo("Red LED in range!");
			}
			else
			{
				DisplayRunTimeInfo("Red LED not in range!");
			}
			DisplayRunTimeInfo("Red LED Test passed!");
			break;
		}
	}
	

	/*if(IDYES==AfxMessageBox(_T("產品上面系統指示燈是否為紅色？\nIs the system LED red in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		
	}
	else
	{
		return 0;
	}*/


	//only orange light
	SendDutCommand("gpio 1 1","#",1500);
	SendDutCommand("gpio 2 0","#",1500);
	
	iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		if(iRtyNum<2)
		{
			return 0; //represent fail
		}
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "_", 6000 );
		if(bResult == false)
		{
			DisplayRunTimeInfo("Set LED Orange CCD cmd failed!");
			Sleep(500);
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Set LED Orange CCD cmd passed!");
		}

		Sleep(1000);
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
		int judgement;
		judgement=JudgeColor(s_LedCCDBuffer.GetBuffer());
		if(6 == judgement)
		{
			DisplayRunTimeInfo("Orange LED Test failed!");
			return 0; //represent fail
		}
		else
		{
			if(4 == judgement)
			{
				DisplayRunTimeInfo("Orange LED in range!");
			}
			else
			{
				DisplayRunTimeInfo("Orange LED not in range!");
			}
			DisplayRunTimeInfo("Orange LED Test passed!");
			break;
		}
	}

	/*if(IDYES==AfxMessageBox(_T("產品上面系統指示燈是否為橙色？\nIs the system LED orange in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		
	}
	else
	{
		return 0;
	}*/


	//only blue light
	SendDutCommand("gpio 2 1","#",1500);
	SendDutCommand("gpio 3 0","#",1500);
	
	iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		if(iRtyNum<2)
		{
			return 0; //represent fail
		}
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "_", 6000 );
		if(bResult == false)
		{
			DisplayRunTimeInfo("Set LED Blue CCD cmd failed!");
			Sleep(500);
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Set LED Blue CCD cmd passed!");
		}

		Sleep(1000);
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
		int judgement;
		judgement=JudgeColor(s_LedCCDBuffer.GetBuffer());
		if(6 == judgement)
		{
			DisplayRunTimeInfo("Blue LED Test failed!");
			return 0; //represent fail
		}
		else
		{
			if(5 == judgement)
			{
				DisplayRunTimeInfo("Blue LED in range!");
			}
			else
			{
				DisplayRunTimeInfo("Blue LED not in range!");
			}
			iReturn=1;
			DisplayRunTimeInfo("Blue LED Test passed!");
			break;
		}
	}

	/*if(IDYES==AfxMessageBox(_T("產品上面系統指示燈是否為藍色？\nIs the system LED blue in product?"),MB_YESNO|MB_ICONQUESTION))
	{
		iReturn=1;
	}
	else
	{
		return 0;
	}*/

	return iReturn;	
//for auto judge LED end

}

int  CRunInfo::iLedCCDTestByGroup( int iGroup )
{

	int iReturn = 1;
	char szTempBuffer[MAXBUFSIZE];
	memset(szTempBuffer, 0, MAXBUFSIZE);
	sprintf_s(szTempBuffer, MAXBUFSIZE, "GetColorGroup:%d\r\n", iGroup);
	
	int iRtyNum = 4;
	for(iRtyNum; iRtyNum > 0; iRtyNum--)
	{
		//send CCD test command to CCD socket
		bool bResult = bSendCmdToLedCCDSocket(szTempBuffer, "1", 6000 );
		//judge result
		if(bResult == false)
		{
			DisplayRunTimeInfo("Send LED CCD cmd failed:", szTempBuffer);
			iReturn = 1;
			continue;
		}
		else
		{
			DisplayRunTimeInfo("Send LED CCD cmd passed:", szTempBuffer);
			iReturn = 0;
		}

		Sleep(500);		
		//judge CCD socket return result
		DisplayRunTimeInfo(s_LedCCDBuffer.GetBuffer());
        if(! ( (s_LedCCDBuffer.IsEmpty()) || (strstr(s_LedCCDBuffer, "ERROR") != NULL) ) )
        {
		   if(strstr(s_LedCCDBuffer, "0") == NULL )//s_LedCCDBuffer = "1111111"
		   {
			   	DisplayRunTimeInfo("At \'s_LedCCDBuffer\' not find failed flag \'0\'.");
			    iReturn = 0;	
				break;	
		   }
		   else 
		   {
				DisplayRunTimeInfo("At \'s_LedCCDBuffer\' find failed flag \'0\'.");		      
			    iReturn = 2;	
			    continue;
           }
        }
		else
		{
			iReturn = 2;
			continue;
		}
	}
	if(iRtyNum <= 0)
	{
		DisplayRunTimeInfo("Send  LED CCD cmd timeout or at \'s_LedCCDBuffer\' find pass flag \'1\' timeout" );
	}
	return iReturn;	
}

int CRunInfo::iLedCCDTest()
{
	int iReturn = 1;
	if(LedCcdTestInfo.iLedCCDTestFlag != 1)
	{
		DisplayRunTimeInfo("LED need not be tested!");
		return iReturn;
	}
	else
	{
		//start test led including 3 ethernet LED and 1 system LED
		DisplayRunTimeInfo("start test LED!");
		iReturn = iLedCCDTestSixLED();
		if(0 == iReturn)
			return 0;  //represent fail
		//if (LedCcdTestInfo.iLedupFlag == 1)
		//{
		//	if(!DutLedup())
		//	{
		//		DisplayRunTimeInfo("Up all led failed!");
		//		return 1;
		//	}
		//	Sleep(2000);
		//iReturn = iLedCCDTestByGroup(0);
		//}
		//else
		//{
		/*for(int iLoopNum = 0; iLoopNum < LedCcdTestInfo.iGroupNum; iLoopNum++)
		{
			
			SendDutCommand(LedCcdTestInfo.cGroupLEDCmdList[iLoopNum], "#", 6000);
			Sleep(LedCcdTestInfo.iGroupTimeSleep);
			iReturn = iLedCCDTestByGroup(iLoopNum);
			char szTemp[LEDCCDBUFSIZE];
			memset(szTemp, 0, LEDCCDBUFSIZE );
			sprintf_s(szTemp, LEDCCDBUFSIZE, "iLedCCDTestByGroup_%d test result ", iLoopNum );
			if(iReturn == 0)
			{
				DisplayRunTimeInfo(szTemp, "is PASS");
				continue;
			}
			else
			{
				DisplayRunTimeInfo(szTemp, "is FAIL");
			}
		}*/
		//}
	}
	
	return iReturn;
}

int CRunInfo::RunOneProcess(char *pCommandLine)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	int iRet;
    
    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;
	char LocalPathBuf[2048];
	char cmdTempBuf[2048];
	
    iRet = 1;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;


    memset(cmdTempBuf, 0, sizeof(cmdTempBuf));
	strcpy_s(cmdTempBuf, pCommandLine);
	GetCurrentDirectory(sizeof(LocalPathBuf), LocalPathBuf);
	strncat_s(LocalPathBuf, sizeof(LocalPathBuf), "\\", strlen("\\"));
    strncat_s(LocalPathBuf, sizeof(LocalPathBuf), pCommandLine, strlen(cmdTempBuf));

    
    ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL,		// No module name (use command line). 
        LocalPathBuf,				// Command line. 
        NULL,						// Process handle not inheritable. 
        NULL,						// Thread handle not inheritable. 
        TRUE,						// Set handle inheritance to FALSE. 
        0,							// No creation flags. 
        NULL,						// Use parent's environment block. 
        NULL,						// Use parent's starting directory. 
        &si,						// Pointer to STARTUPINFO structure.
        &pi )						// Pointer to PROCESS_INFORMATION structure.
		)		
	{
		iRet = 0;
        
	}		  
	return iRet;
}

void CRunInfo::Ch6Ant0(void)
{

	SendDutCommand("iwpriv wl1 set ATE=ATESTART","#",100);
	SendDutCommand("iwpriv wl1 set ATEAUTOALC=0","#",100);
	SendDutCommand("iwpriv wl1 set ATECHANNEL=6","#",100);
	SendDutCommand("iwpriv wl1 set ATETXMODE=1","#",100);
	SendDutCommand("iwpriv wl1 set ATETXMCS=7","#",100);
	SendDutCommand("iwpriv wl1 set ATETXBW=0","#",100);
	SendDutCommand("iwpriv wl1 set ATETXFREQOFFSET=19","#",100);
	SendDutCommand("iwpriv wl1 set ATETXANT=1","#",100);
	SendDutCommand("iwpriv wl1 set ATETXPOW0=20","#",100);
	SendDutCommand("iwpriv wl1 set ATETXCNT=0","#",100);
	SendDutCommand("iwpriv wl1 set ATEVCOCAL=1","#",100);
	SendDutCommand("iwpriv wl1 set ATE=TXFRAME","#",100);
}


void CRunInfo::Ch6Ant1(void)
{

	SendDutCommand("iwpriv wl1 set ATE=ATESTART","#",100);
	SendDutCommand("iwpriv wl1 set ATEAUTOALC=0","#",100);
	SendDutCommand("iwpriv wl1 set ATECHANNEL=6","#",100);
	SendDutCommand("iwpriv wl1 set ATETXMODE=1","#",100);
	SendDutCommand("iwpriv wl1 set ATETXMCS=7","#",100);
	SendDutCommand("iwpriv wl1 set ATETXBW=0","#",100);
	SendDutCommand("iwpriv wl1 set ATETXFREQOFFSET=19","#",100);
	SendDutCommand("iwpriv wl1 set ATETXANT=2","#",100);
	SendDutCommand("iwpriv wl1 set ATETXPOW0=23","#",100);
	SendDutCommand("iwpriv wl1 set ATETXCNT=0","#",100);
	SendDutCommand("iwpriv wl1 set ATEVCOCAL=1","#",100);
	SendDutCommand("iwpriv wl1 set ATE=TXFRAME","#",100);
}

void CRunInfo::Ch153Ant0(void)
{

	SendDutCommand("iwpriv wl0 set ATE=ATESTART","#",100);
	SendDutCommand("iwpriv wl0 set ATEAUTOALC=0","#",100);
	SendDutCommand("iwpriv wl0 set ATECHANNEL=153","#",100);
	SendDutCommand("iwpriv wl0 set ATETXMODE=1","#",100);
	SendDutCommand("iwpriv wl0 set ATETXMCS=7","#",100);
	SendDutCommand("iwpriv wl0 set ATETXBW=0","#",100);
	SendDutCommand("iwpriv wl0 set ATETXFREQOFFSET=43","#",100);
	SendDutCommand("iwpriv wl0 set ATETXANT=1","#",100);
	SendDutCommand("iwpriv wl0 set ATETXPOW0=33","#",100);
	SendDutCommand("iwpriv wl0 set ATETXCNT=0","#",100);
	SendDutCommand("iwpriv wl0 set ATEVCOCAL=1","#",100);
	SendDutCommand("iwpriv wl0 set ATE=TXFRAME","#",100);
}

void CRunInfo::Ch153Ant1(void)
{

	SendDutCommand("iwpriv wl0 set ATE=ATESTART","#",100);
	SendDutCommand("iwpriv wl0 set ATEAUTOALC=0","#",100);
	SendDutCommand("iwpriv wl0 set ATECHANNEL=153","#",100);
	SendDutCommand("iwpriv wl0 set ATETXMODE=1","#",100);
	SendDutCommand("iwpriv wl0 set ATETXMCS=7","#",100);
	SendDutCommand("iwpriv wl0 set ATETXBW=0","#",100);
	SendDutCommand("iwpriv wl0 set ATETXFREQOFFSET=43","#",100);
	SendDutCommand("iwpriv wl0 set ATETXANT=2","#",100);
	SendDutCommand("iwpriv wl0 set ATETXPOW0=34","#",100);
	SendDutCommand("iwpriv wl0 set ATETXCNT=0","#",100);
	SendDutCommand("iwpriv wl0 set ATEVCOCAL=1","#",100);
	SendDutCommand("iwpriv wl0 set ATE=TXFRAME","#",100);
}

float g2Ant0CableLoss=-9999;
float g2Ant1CableLoss=-9999;
float g2MinCableLoss=-9999;
float g2MaxCableLoss=-9999;
float g5Ant0CableLoss=-9999;
float g5Ant1CableLoss=-9999;
float g5MinCableLoss=-9999;
float g5MaxCableLoss=-9999;

void CRunInfo::ReadCableloss()
{
	CStdioFile cFile;
	CString content;
	CString fileName=_T("path_loss.csv");
	if(cFile.Open(fileName,CFile::modeRead)==TRUE)
	{
		cFile.ReadString(content);
		cFile.ReadString(content);
		int find=content.Find(',',0);
		if(find!=-1)
		{
			content=content.Right(content.GetLength()-find-1);	
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g2Ant0CableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("2gAnt0:%f",g2Ant0CableLoss);
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g2Ant1CableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("2gAnt1:%f",g2Ant1CableLoss);
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g2MinCableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("2gMin:%f",g2MinCableLoss);
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g2MaxCableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("2gMax:%f",g2MaxCableLoss);
		}
		else
		{
			return;
		}

		cFile.ReadString(content);
		find=content.Find(',',0);
		if(find!=-1)
		{
			content=content.Right(content.GetLength()-find-1);	
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g5Ant0CableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("5gAnt0:%f",g5Ant0CableLoss);
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g5Ant1CableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("5gAnt1:%f",g5Ant1CableLoss);
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g5MinCableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("5gMin:%f",g5MinCableLoss);
		}
		else
		{
			return;
		}

		find=content.Find(',',0);
		if(find!=-1)
		{
			g5MaxCableLoss=_ttof(content.Left(find));
			content=content.Right(content.GetLength()-find-1);
			DisplayRunTimeInfo("5gMax:%f",g5MaxCableLoss);
		}
		else
		{
			return;
		}
		
		cFile.Close();
	}

}

bool CRunInfo::DutPowerMeterTest(int flag)
{
	bool ret=false;
	int dev=PM_InitialDevice();
	//PM_SetParam("6",30.00);
	//PM_Preset();
	float d=PM_Measure();
	char c[100] = "";
	sprintf_s(c, 100, "%.2f", d);
	DisplayRunTimeInfo(c);

	if(flag==1)
	{
		DisplayRunTimeInfo("flag=1");
		DisplayRunTimeInfo("%f",g2Ant0CableLoss);
		DisplayRunTimeInfo("%f",g2MinCableLoss);
		DisplayRunTimeInfo("%f",g2MaxCableLoss);
		if((d+g2Ant0CableLoss>=g2MinCableLoss)&&(d+g2Ant0CableLoss<=g2MaxCableLoss))
		{
			ret=true;
		}
	}
	else if(flag==2)
	{
		if((d+g2Ant1CableLoss>=g2MinCableLoss)&&(d+g2Ant1CableLoss<=g2MaxCableLoss))
		{
			ret=true;
		}
	}
	else if(flag==3)
	{
		if((d+g5Ant0CableLoss>=g5MinCableLoss)&&(d+g5Ant0CableLoss<=g5MaxCableLoss))
		{
			ret=true;
		}
	}
	else if(flag==4)
	{
		if((d+g5Ant1CableLoss>=g5MinCableLoss)&&(d+g5Ant1CableLoss<=g5MaxCableLoss))
		{
			ret=true;
		}
	}

	//CString str=(CString)c;
	//AfxMessageBox(str);

	return ret;	
}
//end
