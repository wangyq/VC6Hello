// BaseStationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BaseStation.h"
#include "BaseStationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern USignalPaket SetupSignal;
extern USignalPaket AltertingSignal;
extern USignalPaket ConnectSignal;
extern USignalPaket DisconnectSignal;

extern bool  bSimuConnectLost;
extern bool  bSimuDisConnectLost;



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseStationDlg dialog

CBaseStationDlg::CBaseStationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBaseStationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBaseStationDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBaseStationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBaseStationDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBaseStationDlg, CDialog)
	//{{AFX_MSG_MAP(CBaseStationDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_NOERR, OnRadioNoErr)
	ON_BN_CLICKED(IDC_RADIO_CONNECTLOST, OnRadioConnectLost)
	ON_BN_CLICKED(IDC_RADIO_DISCONNECTLOST, OnRadioDisconnectLost)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECVDATA,OnRecvData)     //��Ϣӳ��. xie 2009-7-22
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseStationDlg message handlers

BOOL CBaseStationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
		//������ĳ�ʼ������
	InitSocket(); //added by xie. 2009-7-22

	GSMU_InitSignal(); //add by xie . 2009-7-23
	
	/**************����U����Setup�Ƿ��ʼ���ɹ�
	char tempchar[5]={0};
	itoa(SetupSignal.messageType.Val,tempchar,16);
	MessageBox(	tempchar); //��0x45����ɹ���ʼ��
	*****************************************************/
	
	RECVPARAM *pRecvParam = new RECVPARAM; 
	pRecvParam->socketUp=m_socketUp; //�׽��־��
	pRecvParam->socketDown=m_socketDown; //�׽��־��
	pRecvParam->hwnd = m_hWnd; //���ھ��

	//�������ս���
	HANDLE hThread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvParam,0,NULL);
	//�رոý��ս��̵ľ�����ͷ������ü���
	CloseHandle(hThread);


	//�����޲�����龰ΪĬ��ֵ
	CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_DISCONNECTLOST , IDC_RADIO_NOERR); 
	CBaseStationDlg::OnRadioNoErr();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBaseStationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBaseStationDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBaseStationDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CBaseStationDlg::InitSocket()
{
	//�����׽��֣�����ΪUDP,������·��BSΪServer
	m_socketUp = socket(AF_INET, SOCK_DGRAM, 0);

	if(INVALID_SOCKET == m_socketUp)
	{
		MessageBox("Fail to create socket!");
		return FALSE;
	}
	// socket ������Ϣ��ռ�ö˿�Ϊ5000����������ip��ַ������
	SOCKADDR_IN addrBS;
	addrBS.sin_family = AF_INET;
	addrBS.sin_port = htons(5000);
	addrBS.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retval;
	retval = bind(m_socketUp, (SOCKADDR*)&addrBS, sizeof(SOCKADDR) );
	if(SOCKET_ERROR == retval)
	{
		closesocket(m_socketUp);
		MessageBox("Fail to Bind!");
		return FALSE;
	}


	//�����׽��֣�����ΪUDP,������·��BSΪClient
	m_socketDown = socket(AF_INET, SOCK_DGRAM, 0);

	if(INVALID_SOCKET == m_socketDown)
	{
		MessageBox("Fail to create socket!");
		return FALSE;
	}


		// socket ������Ϣ��ռ�ö˿�Ϊ7000
	m_addrToPhone.sin_family = AF_INET;
	m_addrToPhone.sin_port = htons(7000);
	m_addrToPhone.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");


	return TRUE;
}


//�����Ľ����߳�
DWORD WINAPI CBaseStationDlg::RecvProc(LPVOID lpParameter)
{
	SOCKET socketUp = ( (RECVPARAM*) lpParameter)->socketUp; //����socket��ģ��������·
	SOCKET socketDown = ( (RECVPARAM*) lpParameter)->socketDown; //����socket��ģ��������·
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd; //���ھ��
	delete lpParameter;

	SOCKADDR_IN addrFromPhone;
	int len = sizeof(SOCKADDR);

	unsigned int wmRecvData= WM_RECVDATA; //UINT Msg,��Ҫ���ô˱�����WM_RECVDATA���ݸ��������
												//GSMUP_signalProcess

	char recvBuf[100]={0};  //����Buf

	int retval;  //return value


	while(TRUE)
	{
		//��������MS������
		retval = recvfrom(socketUp,recvBuf,100,0,(SOCKADDR*)&addrFromPhone,&len);
		if(SOCKET_ERROR == retval)
		{
			AfxMessageBox("Socket Wrong !");
			break;
		}

		//������յ�������
		GSMUP_signalProcess(hwnd,wmRecvData,&socketDown,recvBuf);	

	
	} //end while


		return 0;
}


//������Ϣ��Ӧ����. xie 2009-7-22
void CBaseStationDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
	CString str = (char*)lParam;
	CString strTemp;
	GetDlgItemText(IDC_EDIT_RECV, strTemp);
	strTemp+="\r\n";
	strTemp+=str;

	SetDlgItemText(IDC_EDIT_RECV,strTemp);
}

void CBaseStationDlg::OnRadioNoErr() 
{
	// TODO: Add your control notification handler code here
	bSimuConnectLost=false;
	bSimuDisConnectLost=false;
	
}

void CBaseStationDlg::OnRadioConnectLost() 
{
	// TODO: Add your control notification handler code here
		bSimuConnectLost=true;
	bSimuDisConnectLost=false;
	
}

void CBaseStationDlg::OnRadioDisconnectLost() 
{
	// TODO: Add your control notification handler code here
	bSimuConnectLost=false;
	bSimuDisConnectLost=true;
	
}

void CBaseStationDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TIMER_CONNECT:
		
		KillTimer(TIMER_CONNECT); //����������ͣ����
		
		//ֻ����connect����������龰�²������¶���
		if(bSimuConnectLost)
		{
			if(IDYES==MessageBox("Altering �� Connect ���ʧ\r\n�Ƿ��ڡ��޲���龰�����·���Altering��Connect��������?",
				"Connect Transmit Erro!",MB_YESNO| MB_ICONWARNING)) //ѡ����. xie 2009-7-24
			{
				//�����޲�����龰
				CBaseStationDlg::OnRadioNoErr();
				CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_DISCONNECTLOST , IDC_RADIO_NOERR); 
				
				CBaseStationDlg::SendAlteringSignal();	
				Sleep(100); //����Ϣ�������������ٷ���һ�����������ܲ���������ʾcodeStream
				CBaseStationDlg::SendConnectSignal(); //���·��ͽ�������setup . xie 2009-7-24
				
				::SetTimer(m_hWnd,TIMER_CONNECT,3000,NULL); //����ConnectAck��ʱ���������ʱ�����û���յ�Disconnect
				
			}
			else //ѡ���. xie 2009-7-24
			{
				//�����˳�
			}	
		}
		break;
		
	case TIMER_DISCONNECT:
		KillTimer(TIMER_DISCONNECT); //����������ͣ����

		//ֻ����DisConnect����������龰�²������¶���
		if(bSimuDisConnectLost)
		{
			if(IDYES==
				MessageBox("DisConnect �����ڴ�������ж�ʧ\r\n�Ƿ��ڡ��޲���龰�����·���DisConnect?",
				"DisConnect Transmit Erro!",MB_YESNO| MB_ICONWARNING)) //ѡ����. xie 2009-7-24
			{
				//�����޲�����龰
				CBaseStationDlg::OnRadioNoErr();
				CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_DISCONNECTLOST , IDC_RADIO_NOERR); 
				
				CBaseStationDlg::SendDisConnectSignal();
				
				::SetTimer(m_hWnd,TIMER_DISCONNECT,3000,NULL); //����disConnect��ʱ���������ʱ�����û���յ�Disconnect
			}
			else //ѡ���. xie 2009-7-24
			{
				//�����˳�
			}	
		}
		
		break;
		
	default:
		MessageBox("Wrong Timer ID!");
		break;
	}//end switch
	
	CDialog::OnTimer(nIDEvent);
}

void CBaseStationDlg::SendConnectSignal()
{
	static char echoBuf[200]={0};

	sendto(m_socketDown, (char*)&ConnectSignal,sizeof(USignalPaket),0,(SOCKADDR*)&m_addrToPhone,sizeof(SOCKADDR));
	
	sprintf(echoBuf, "BaseStaion ---> Phone : Connect \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x\r\n",
		ConnectSignal.messageType.Val, ConnectSignal.progInd.elementId.Val,	
		ConnectSignal.progInd.length.Val,ConnectSignal.progInd.payload[0].Val);	
	Sleep(5000);
	::PostMessage(m_hWnd,WM_RECVDATA,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23

}

void CBaseStationDlg::SendDisConnectSignal()
{

	static char echoBuf[200]={0};
	
	sendto(m_socketDown, (char*)&DisconnectSignal,sizeof(USignalPaket),0,(SOCKADDR*)&m_addrToPhone,sizeof(SOCKADDR));
	
	sprintf(echoBuf, "BaseStaion ---> Phone :Disconnect \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		DisconnectSignal.messageType.Val);
		Sleep(5000);
	::PostMessage(m_hWnd,WM_RECVDATA,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23
}

void CBaseStationDlg::OnBtnClear() 
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_EDIT_RECV," ");
	
}

void CBaseStationDlg::SendAlteringSignal()
{
	static char echoBuf[200]={0};
	
	sendto(m_socketDown, (char*)&AltertingSignal,sizeof(USignalPaket),0,(SOCKADDR*)&m_addrToPhone,sizeof(SOCKADDR));
	
	sprintf(echoBuf, "BaseStaion ---> Phone :Alterting \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x 0x%02x\r\n",
		AltertingSignal.messageType.Val, AltertingSignal.progInd.elementId.Val,	
		AltertingSignal.progInd.length.Val,AltertingSignal.progInd.payload[0].Val,AltertingSignal.progInd.payload[1].Val);	
		Sleep(5000);
	::PostMessage(m_hWnd,WM_RECVDATA,0,(LPARAM)echoBuf); //���Է��͸���վ����Ϣ . xie 2009-7-22

}
