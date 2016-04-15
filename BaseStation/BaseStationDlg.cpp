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
	ON_MESSAGE(WM_RECVDATA,OnRecvData)     //消息映射. xie 2009-7-22
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
		//本程序的初始化工作
	InitSocket(); //added by xie. 2009-7-22

	GSMU_InitSignal(); //add by xie . 2009-7-23
	
	/**************测试U信令Setup是否初始化成功
	char tempchar[5]={0};
	itoa(SetupSignal.messageType.Val,tempchar,16);
	MessageBox(	tempchar); //是0x45，则成功初始化
	*****************************************************/
	
	RECVPARAM *pRecvParam = new RECVPARAM; 
	pRecvParam->socketUp=m_socketUp; //套接字句柄
	pRecvParam->socketDown=m_socketDown; //套接字句柄
	pRecvParam->hwnd = m_hWnd; //窗口句柄

	//创建接收进程
	HANDLE hThread = CreateThread(NULL,0,RecvProc,(LPVOID)pRecvParam,0,NULL);
	//关闭该接收进程的句柄，释放其引用计数
	CloseHandle(hThread);


	//设置无差错传输情景为默认值
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
	//创建套接字，类型为UDP,上行链路，BS为Server
	m_socketUp = socket(AF_INET, SOCK_DGRAM, 0);

	if(INVALID_SOCKET == m_socketUp)
	{
		MessageBox("Fail to create socket!");
		return FALSE;
	}
	// socket 类型信息，占用端口为5000，接收任意ip地址的请求
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


	//创建套接字，类型为UDP,下行链路，BS为Client
	m_socketDown = socket(AF_INET, SOCK_DGRAM, 0);

	if(INVALID_SOCKET == m_socketDown)
	{
		MessageBox("Fail to create socket!");
		return FALSE;
	}


		// socket 类型信息，占用端口为7000
	m_addrToPhone.sin_family = AF_INET;
	m_addrToPhone.sin_port = htons(7000);
	m_addrToPhone.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");


	return TRUE;
}


//独立的接收线程
DWORD WINAPI CBaseStationDlg::RecvProc(LPVOID lpParameter)
{
	SOCKET socketUp = ( (RECVPARAM*) lpParameter)->socketUp; //上行socket，模仿上行链路
	SOCKET socketDown = ( (RECVPARAM*) lpParameter)->socketDown; //下行socket，模仿下行链路
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd; //窗口句柄
	delete lpParameter;

	SOCKADDR_IN addrFromPhone;
	int len = sizeof(SOCKADDR);

	unsigned int wmRecvData= WM_RECVDATA; //UINT Msg,需要利用此变量将WM_RECVDATA传递给信令处理函数
												//GSMUP_signalProcess

	char recvBuf[100]={0};  //接收Buf

	int retval;  //return value


	while(TRUE)
	{
		//接收来自MS的信令
		retval = recvfrom(socketUp,recvBuf,100,0,(SOCKADDR*)&addrFromPhone,&len);
		if(SOCKET_ERROR == retval)
		{
			AfxMessageBox("Socket Wrong !");
			break;
		}

		//处理接收到的信令
		GSMUP_signalProcess(hwnd,wmRecvData,&socketDown,recvBuf);	

	
	} //end while


		return 0;
}


//接收消息响应函数. xie 2009-7-22
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
		
		KillTimer(TIMER_CONNECT); //计数器先暂停计数
		
		//只有在connect信令传输错误的情景下才做以下动作
		if(bSimuConnectLost)
		{
			if(IDYES==MessageBox("Altering 或 Connect 信令丢失\r\n是否在“无差错情景”重新发送Altering和Connect建立连接?",
				"Connect Transmit Erro!",MB_YESNO| MB_ICONWARNING)) //选择是. xie 2009-7-24
			{
				//设置无差错传输情景
				CBaseStationDlg::OnRadioNoErr();
				CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_DISCONNECTLOST , IDC_RADIO_NOERR); 
				
				CBaseStationDlg::SendAlteringSignal();	
				Sleep(100); //等消息处理函数运行完再发下一个信令。否则可能不能正常显示codeStream
				CBaseStationDlg::SendConnectSignal(); //重新发送接入信令setup . xie 2009-7-24
				
				::SetTimer(m_hWnd,TIMER_CONNECT,3000,NULL); //启动ConnectAck定时器，如果超时则表明没有收到Disconnect
				
			}
			else //选择否. xie 2009-7-24
			{
				//正常退出
			}	
		}
		break;
		
	case TIMER_DISCONNECT:
		KillTimer(TIMER_DISCONNECT); //计数器先暂停计数

		//只有在DisConnect信令传输错误的情景下才做以下动作
		if(bSimuDisConnectLost)
		{
			if(IDYES==
				MessageBox("DisConnect 信令在传输过程中丢失\r\n是否在“无差错情景”重新发送DisConnect?",
				"DisConnect Transmit Erro!",MB_YESNO| MB_ICONWARNING)) //选择是. xie 2009-7-24
			{
				//设置无差错传输情景
				CBaseStationDlg::OnRadioNoErr();
				CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_DISCONNECTLOST , IDC_RADIO_NOERR); 
				
				CBaseStationDlg::SendDisConnectSignal();
				
				::SetTimer(m_hWnd,TIMER_DISCONNECT,3000,NULL); //启动disConnect定时器，如果超时则表明没有收到Disconnect
			}
			else //选择否. xie 2009-7-24
			{
				//正常退出
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
	::PostMessage(m_hWnd,WM_RECVDATA,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23

}

void CBaseStationDlg::SendDisConnectSignal()
{

	static char echoBuf[200]={0};
	
	sendto(m_socketDown, (char*)&DisconnectSignal,sizeof(USignalPaket),0,(SOCKADDR*)&m_addrToPhone,sizeof(SOCKADDR));
	
	sprintf(echoBuf, "BaseStaion ---> Phone :Disconnect \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		DisconnectSignal.messageType.Val);
		Sleep(5000);
	::PostMessage(m_hWnd,WM_RECVDATA,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23
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
	::PostMessage(m_hWnd,WM_RECVDATA,0,(LPARAM)echoBuf); //回显发送给基站的信息 . xie 2009-7-22

}
