// PhoneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Phone.h"
#include "PhoneDlg.h"

extern USignalPaket SetupSignal;
extern USignalPaket ConnectACKSignal;
extern USignalPaket ReleaseSignal;

extern bool  bSimuSetupLost;
extern bool  bSimuConnectAckLost;
extern bool  bSimuNoErr;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



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
// CPhoneDlg dialog

CPhoneDlg::CPhoneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPhoneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhoneDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPhoneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhoneDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPhoneDlg, CDialog)
	//{{AFX_MSG_MAP(CPhoneDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_SETUPLOST, OnRadioSetuplost)
	ON_BN_CLICKED(IDC_RADIO_CONNECTACKLOST, OnRadioConnectacklost)
	ON_BN_CLICKED(IDC_RADIO_NOERR, OnRadioNoErr)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PHONERECVDATA,OnPhoneRecvData)     //消息映射. xie 2009-7-22
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhoneDlg message handlers

BOOL CPhoneDlg::OnInitDialog()
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


	m_bSetupSendSuccess=FALSE;
	m_bConnectACKSendSuccess=FALSE;

	//设置无差错传输情景为默认值
	CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_CONNECTACKLOST , IDC_RADIO_NOERR); 
	CPhoneDlg::OnRadioNoErr();

	static CRect rectStatic1;
	static CRect rectStatic2;
	GetDlgItem(IDC_STATIC1)->SetWindowPos(NULL,170,130,rectStatic1.Width(),rectStatic1.Height(),SWP_NOSIZE);
	GetDlgItem(IDC_STATIC2)->SetWindowPos(NULL,300,160,rectStatic2.Width(),rectStatic2.Height(),SWP_NOSIZE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPhoneDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPhoneDlg::OnPaint() 
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
HCURSOR CPhoneDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CPhoneDlg::InitSocket()
{
 
	//创建套接字，类型为UDP,下行链路，Phone为Server
	m_socketDown = socket(AF_INET, SOCK_DGRAM, 0);

	if(INVALID_SOCKET == m_socketDown)
	{
		MessageBox("Fail to create socket!");
		return FALSE;
	}
	// socket 类型信息，占用端口为6000，接收任意ip地址的请求
	SOCKADDR_IN addrPhone;
	addrPhone.sin_family = AF_INET;
	addrPhone.sin_port = htons(7000);
	addrPhone.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retval;
	retval = bind(m_socketDown, (SOCKADDR*)&addrPhone, sizeof(SOCKADDR) );
	if(SOCKET_ERROR == retval)
	{
		closesocket(m_socketDown);
		MessageBox("Fail to m_socketDown Bind!");
		return FALSE;
	}


	//创建套接字，类型为UDP,上行链路，Phone为Client
	m_socketUp = socket(AF_INET, SOCK_DGRAM, 0);

	if(INVALID_SOCKET == m_socketUp)
	{
		MessageBox("Fail to create Phone Up socket!");
		return FALSE;
	}


	//socket 类型信息
	m_addrToBS.sin_family = AF_INET;
	m_addrToBS.sin_port = htons(5000);
	m_addrToBS.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");


	return TRUE;
}


//独立的接收进程
DWORD WINAPI CPhoneDlg::RecvProc(LPVOID lpParameter)
{
	SOCKET socketUp = ( (RECVPARAM*) lpParameter)->socketUp;  //上行socket，模仿上行链路
	SOCKET socketDown = ( (RECVPARAM*) lpParameter)->socketDown;  //下行socket，模仿下行链路
	HWND hwnd = ((RECVPARAM*)lpParameter)->hwnd;  //窗口句柄
	delete lpParameter;

	SOCKADDR_IN addrFromBS;
	int len = sizeof(SOCKADDR);

	char recvBuf[100]={0};  //接收Buf


	unsigned int wmPhoneRecvData= WM_PHONERECVDATA; //UINT Msg,需要利用此变量将WM_PHONERECVDATA传递给信令处理函数
												//GSMUP_signalProcess

	int retval;  //return value

	
	while(TRUE)
	{
	
		retval = recvfrom(socketDown,recvBuf,100,0,(SOCKADDR*)&addrFromBS,&len);
		if(SOCKET_ERROR == retval)
		{
			AfxMessageBox("Socket Wrong !");
			break;
		}

		//处理接收到的信令
		
		GSMUP_signalProcess(hwnd,wmPhoneRecvData,&socketUp,recvBuf);	
	
		

		
		
	}//end while


		return 0;
}


//接收消息响应函数. xie 2009-7-22

void CPhoneDlg::OnPhoneRecvData(WPARAM wParam, LPARAM lParam)
{
	CString str = (char*)lParam;
	CString strTemp;

	char strStatic[200] = {0};
	char strMessageType[15]={0};
	int type=0,ident=0,len=0,content=0,content1=0;

	GetDlgItemText(IDC_EDIT_RECV, strTemp);
	strTemp+="\r\n";
	strTemp+=str;



	SetDlgItemText(IDC_EDIT_RECV,strTemp);
	//从收到的格式化消息中提取出关键信息，用于模拟信令传输过程
	sscanf((char *)lParam,"Phone ---> BaseStaion: Setup\r\nCode Stream:\r\n    MessageType: 0x%x, ElementIdentifier: 0x%x, Length: 0x%x, Contents: 0x%x\r\n",
		&type,&ident,&len,&content);
	sscanf((char *)lParam,"BaseStaion ---> Phone: %s \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x 0x%02x\r\n",
		strMessageType,	&type,&ident,&len,&content,&content1);
	sscanf((char *)lParam, "BaseStaion ---> Phone: %s \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		strMessageType,&type);

	sscanf((char *)lParam, "Phone ---> BaseStaion: ConnectACK\r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		&type);
	sscanf((char *)lParam, "Phone ---> BaseStaion: ReleaseSignal\r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		&type);
	sscanf((char *)lParam, "Phone ---> BaseStaion: Release\r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		&type);
	switch(type)		//根据不同MessageType进行不同格式化，组成信令显示出来
	{
	case 0x45:
		SetTimer(TIMER_MOVE_PHONEMSG,100,NULL);
		sprintf(strStatic,"%02xH-%02xH-%02xH-%02xH-Setup",content,len,ident,type);
		SetDlgItemText(IDC_STATIC1,strStatic);
		break;
	case 0x01:
		SetTimer(TIMER_MOVE_BSMSG,100,NULL);
		sprintf(strStatic,"Altering-%02xH-%02xH-%02xH-%02xH-%02xH",type,ident,len,content,content1);
		SetDlgItemText(IDC_STATIC2,strStatic);
		break;
	case 0x07:
		SetTimer(TIMER_MOVE_BSMSG,100,NULL);
		sprintf(strStatic,"Connect-%02xH",type);
		SetDlgItemText(IDC_STATIC2,strStatic);
		break;
	case 0x8f:
		SetTimer(TIMER_MOVE_PHONEMSG,100,NULL);
		sprintf(strStatic,"%02xH-ConnectACK",type);
		SetDlgItemText(IDC_STATIC1,strStatic);
		break;
	case 0xe5:
		SetTimer(TIMER_MOVE_BSMSG,100,NULL);
		sprintf(strStatic,"Disconnect-%02xH",type);
		SetDlgItemText(IDC_STATIC2,strStatic);
		break;
	case 0x2d:
		SetTimer(TIMER_MOVE_PHONEMSG,100,NULL);
		sprintf(strStatic,"%02xH-Realease",type);
		SetDlgItemText(IDC_STATIC1,strStatic);
			
		break;
	}


	
}

void CPhoneDlg::OnBtnSend() 
{
	// TODO: Add your control notification handler code here

	static char echoBuf[200]={0}; //如果采用动态分配或自动分配，可能由于某种原因导致echo信息不能显示. xie 2009-7-24
	static char strSend[200]={0};
	

	int len = sizeof(SOCKADDR);
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);

	if(bSimuSetupLost)
	{
		GetDlgItem(IDC_RADIO_NOERR)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_CONNECTACKLOST)->EnableWindow(FALSE);
	}
	 if(bSimuConnectAckLost)
	{
		GetDlgItem(IDC_RADIO_NOERR)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SETUPLOST)->EnableWindow(FALSE);
	}
	if(bSimuNoErr)
	{
		GetDlgItem(IDC_RADIO_SETUPLOST)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_CONNECTACKLOST)->EnableWindow(FALSE);
	}

	//只有在bSimuSetupLost=false时才发送setup, 即用不发来模拟发送setup信令丢失

	if(!bSimuSetupLost)
	{
		sendto(m_socketUp, (char*)(&SetupSignal),sizeof(SetupSignal),0,(SOCKADDR*)&m_addrToBS,len);
	}
	SetTimer(TIMER_SETUP,3000,NULL);
	::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)("开始连接."));
	
	sprintf(strSend, "Setup \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x\r\n",
		SetupSignal.messageType.Val, SetupSignal.progInd.elementId.Val,	
		SetupSignal.progInd.length.Val,SetupSignal.progInd.payload[0].Val);	
	sprintf(echoBuf, "Phone ---> BaseStaion: %s",strSend); 
//	Sleep(5000);
	::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)echoBuf); //回显发送给基站的信息 . xie 2009-7-22


	
}


void CPhoneDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	static int second=5;
	
	char tempBuf[100]={0};
	int retVal=0;
	CString str;
	
	CWnd *pWnd1,*pWnd2; 
	pWnd1 = GetDlgItem(IDC_STATIC1);
	pWnd2 = GetDlgItem(IDC_STATIC2);
	static CRect rectStatic1;
	static CRect rectStatic2;

	switch(nIDEvent)
	{
	case TIMER_SETUP:
		
		KillTimer(TIMER_SETUP); //计数器先暂停计数

		//只有在SetupLost信令传输错误的情景下才做以下动作
		if( bSimuSetupLost)
		{
			if(IDYES==
				MessageBox("Setup 信令发送超时\r\n是否在“无差错情景”重新发送Setup建立连接?",
				"Setup Timeout!",MB_YESNO| MB_ICONWARNING)) //选择是. xie 2009-7-24
			{
				//设置无差错传输情景
				CPhoneDlg::OnRadioNoErr();
				CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_CONNECTACKLOST , IDC_RADIO_NOERR); 	
				
				//发送Setup
				CPhoneDlg::OnBtnSend(); 

				
			}	
		}//end outer if

		break;

		
	case TIMER_CONNECTACK:

		KillTimer(TIMER_CONNECTACK); //计数器先暂停计数
		if(bSimuConnectAckLost)
		{
			if(IDYES==
				MessageBox("Connect_Ack 信令在传输中丢失\r\n是否在“无差错情景”重新发送Connect_Ack?",
				"Connect_Ack Lost!",MB_YESNO| MB_ICONWARNING)) //选择是. xie 2009-7-24
			{
				//设置无差错传输情景
				CPhoneDlg::OnRadioNoErr();
				CheckRadioButton( IDC_RADIO_NOERR,IDC_RADIO_CONNECTACKLOST , IDC_RADIO_NOERR); 	

				//发送Connect_ack
				CPhoneDlg::SendConnectAckSignal();
				
			}	
		}

		
		break;
	case TIMER_AUTOCLOSE:
		KillTimer(TIMER_AUTOCLOSE);
		keybd_event(VK_RETURN,0,0,0);
		keybd_event(VK_RETURN,0,KEYEVENTF_KEYUP,0);//模拟"回车"命令 关闭对话框 .xie 2009-7-25	

		break;

	case TIMER_MOVE_PHONEMSG:		//模拟手机到基站信令传输过程

		pWnd1->ShowWindow(SW_SHOW);
		
		pWnd1->GetWindowRect(&rectStatic1);
        ScreenToClient(&rectStatic1);
		if(bSimuSetupLost)			//Setup丢失情况
		{
			if(rectStatic1.right <= 360)			//发送信令为Setup,模拟Setup中途丢失
			{
				rectStatic1.left = rectStatic1.left + 5;
				rectStatic1.right = rectStatic1.right + 5;
				pWnd1->MoveWindow(rectStatic1);
			}
			else
			{
				KillTimer(TIMER_MOVE_PHONEMSG);
				pWnd1->SetWindowPos(NULL,170,130,rectStatic1.Width(),rectStatic1.Height(),SWP_NOSIZE|SWP_HIDEWINDOW);
				::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)("Setup lost,通话结束!"));
				GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO_NOERR)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO_CONNECTACKLOST)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO_SETUPLOST)->EnableWindow(TRUE);
			}
		}
		else if(bSimuConnectAckLost)//ConnectACK丢失情况
		{
			if(GetDlgItem(IDC_STATIC1)->GetWindowText(str),str == "8fH-ConnectACK")//发送信令为ConnectACK,模拟ConnectACK中途丢失
				
			{
				if(rectStatic1.right <= 360)
				{
					rectStatic1.left = rectStatic1.left + 5;
					rectStatic1.right = rectStatic1.right + 5;
					pWnd1->MoveWindow(rectStatic1);
				}
				else
				{
					KillTimer(TIMER_MOVE_PHONEMSG);
					pWnd1->SetWindowPos(NULL,170,130,rectStatic1.Width(),rectStatic1.Height(),SWP_NOSIZE|SWP_HIDEWINDOW);
					::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)("ConnectACK lost,通话结束!"));
					GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
					GetDlgItem(IDC_RADIO_NOERR)->EnableWindow(TRUE);
					GetDlgItem(IDC_RADIO_CONNECTACKLOST)->EnableWindow(TRUE);
					GetDlgItem(IDC_RADIO_SETUPLOST)->EnableWindow(TRUE);
				}
			}
			
			else												//发送信令为其它信令，信令到达基站
			{
				if(rectStatic1.right <= 470)
				{
					rectStatic1.left = rectStatic1.left + 5;
					rectStatic1.right = rectStatic1.right + 5;
					pWnd1->MoveWindow(rectStatic1);
				}
				else
				{
					
					KillTimer(TIMER_MOVE_PHONEMSG);
					pWnd1->SetWindowPos(NULL,170,130,rectStatic1.Width(),rectStatic1.Height(),SWP_NOSIZE|SWP_HIDEWINDOW);
				}
			}
		}
		else
		{
			if(rectStatic1.right <= 470)
			{
				rectStatic1.left = rectStatic1.left + 5;
				rectStatic1.right = rectStatic1.right + 5;
				pWnd1->MoveWindow(rectStatic1);
			}
			else
			{
				
				KillTimer(TIMER_MOVE_PHONEMSG);
				pWnd1->SetWindowPos(NULL,170,130,rectStatic1.Width(),rectStatic1.Height(),SWP_NOSIZE|SWP_HIDEWINDOW);
				if(GetDlgItem(IDC_STATIC1)->GetWindowText(str),str == "2dH-Realease")//基站接收到 Realease，通话结束
				{
					GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
					GetDlgItem(IDC_RADIO_NOERR)->EnableWindow(TRUE);
					GetDlgItem(IDC_RADIO_CONNECTACKLOST)->EnableWindow(TRUE);
					GetDlgItem(IDC_RADIO_SETUPLOST)->EnableWindow(TRUE);
					::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)("正常通话结束!"));
				}
			}
		}
		break;
	case TIMER_MOVE_BSMSG:			//模拟基站到手机信令的传输过程

		pWnd2->ShowWindow(SW_SHOW);
		
		pWnd2->GetWindowRect(&rectStatic2);
        ScreenToClient(&rectStatic2);
		if(rectStatic2.left >= 170)
		{	
			rectStatic2.left = rectStatic2.left - 5;
			rectStatic2.right = rectStatic2.right - 5;
			pWnd2->MoveWindow(rectStatic2);
		}
		else 

		{
			KillTimer(TIMER_MOVE_BSMSG);
			pWnd2->SetWindowPos(NULL,300,160,rectStatic2.Width(),rectStatic2.Height(),SWP_NOSIZE|SWP_HIDEWINDOW);
		
		}
		
		break;
		

	default:
		MessageBox("Wrong Timer ID!");
		break;
	}//end switch

	

	CDialog::OnTimer(nIDEvent);
}

void CPhoneDlg::SendConnectAckSignal()
{

	static char echoBuf[200]={0};

	
	sendto(m_socketUp, (char*)&ConnectACKSignal,sizeof(ConnectACKSignal),0,(SOCKADDR*)&m_addrToBS, sizeof(SOCKADDR));
	
	sprintf(echoBuf, "Phone ---> BaseStaion: ConnectACK \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		ConnectACKSignal.messageType.Val, ConnectACKSignal.progInd.elementId.Val);

	Sleep(5000);
	::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23
}

void CPhoneDlg::SendReleaseSignal()
{
	static char echoBuf[200]={0};

	
	sendto(m_socketUp, (char*)&ReleaseSignal,sizeof(ReleaseSignal),0,(SOCKADDR*)&m_addrToBS, sizeof(SOCKADDR));
	
	sprintf(echoBuf, "Phone ---> BaseStaion: ReleaseSignal \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
		ReleaseSignal.messageType.Val, ReleaseSignal.progInd.elementId.Val);
	Sleep(5000);
	::PostMessage(m_hWnd,WM_PHONERECVDATA,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23

}



void CPhoneDlg::OnRadioSetuplost() 
{
	// TODO: Add your control notification handler code here
	bSimuSetupLost=true;
	bSimuConnectAckLost=false;
	bSimuNoErr = false;
	
}

void CPhoneDlg::OnRadioConnectacklost() 
{
	// TODO: Add your control notification handler code here
	bSimuConnectAckLost=true;
	bSimuSetupLost=false;
	bSimuNoErr = false;
	
}

void CPhoneDlg::OnRadioNoErr() 
{
	// TODO: Add your control notification handler code here
	bSimuNoErr = true;
	bSimuSetupLost=false;
	bSimuConnectAckLost=false;
	
}

void CPhoneDlg::OnBtnClear() 
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_EDIT_RECV," ");
	
}
