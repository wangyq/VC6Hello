

#include "GSMUSignalProcess.h"

#include "stdafx.h"


extern USignalPaket SetupSignal;
extern USignalPaket ConnectACKSignal;
extern USignalPaket ReleaseSignal;


bool bSimuSetupLost;
bool bSimuConnectAckLost;
bool bSimuNoErr;


//由于不能包含PhoneDlg.h 头文件，所以定义下面的全局变量来表示TIMER ID

unsigned int SETUP_TIMER=1;
unsigned int CONNECTACK_TIMER=4;
unsigned int  AUTOCLOSE_TIMER=100;



void GSMUP_signalProcess(HWND hwnd, unsigned int WMPhoneRecvData,SOCKET* pSocketUp, char* pRecvBuf)
{
	
	
	USignalPaket* pRecvSignal; //接收到的信令
	USignalPaket* pSendSignal; //要发送的信令
	
	// socket 类型信息，占用端口为5000
	SOCKADDR_IN addrToBS;
	addrToBS.sin_family = AF_INET;
	addrToBS.sin_port = htons(5000);
	addrToBS.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");
	
	int len = sizeof(SOCKADDR);
	
	char strMessageType[15]={0}; //存储接收到得信令类型
	char tempBuf[200]={0}; //存储格式化字符串
	char echoBuf[200]={0};
	
	pRecvSignal= (USignalPaket*)pRecvBuf;
	
	switch(pRecvSignal->messageType.Val)
	{
	case 0x45:
		strcpy(strMessageType,"Setup");
		break;
	case 0x01:
		strcpy(strMessageType,"Altering");
		break;
	case 0x07:
		strcpy(strMessageType,"Connect");
		break;
	case 0x8f:
		strcpy(strMessageType,"ConnectACK");
		break;
	case 0xe5:
		strcpy(strMessageType,"DisConnect");
		break;
	case 0x2d:
		strcpy(strMessageType,"Release");
		break;
	default:
		strcpy(strMessageType,"WrongType");
		break;
	}//end switch
	
	
	if( 0 != pRecvSignal->progInd.length.Val) //收到Altering
	{	
		sprintf(tempBuf, "BaseStaion ---> Phone: %s \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val, pRecvSignal->progInd.elementId.Val,	
			pRecvSignal->progInd.length.Val,pRecvSignal->progInd.payload[0].Val,pRecvSignal->progInd.payload[1].Val);
		
	}
	else //收到没有负载的信令
	{
		
		sprintf(tempBuf, "BaseStaion ---> Phone: %s \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val);
		
	}
	
	
	Sleep(5000);
	::PostMessage(hwnd,WMPhoneRecvData,0,(LPARAM)tempBuf); //显示收到的信息 . xie 2009-7-22
	Sleep(100);	//先让消息处理函数运行. xie 2009-7-22
	
	
	
	
	if(0==strcmp(strMessageType,"Connect"))
	{
		//::KillTimer(hwnd,SETUP_TIMER);
		
		pSendSignal=&ConnectACKSignal;
		
		sprintf(echoBuf, "Phone ---> BaseStaion: ConnectACK \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val, pSendSignal->progInd.elementId.Val);
		Sleep(5000);
		::PostMessage(hwnd,WMPhoneRecvData,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23
		
		SetTimer(hwnd,AUTOCLOSE_TIMER,5000,NULL);
		int retVal=AfxMessageBox("开始通话!\r\n结束通话请选“确定”\r\n5秒钟后强制结束通话",MB_OK);
		
		if(IDOK==retVal )
		{
			//结束通话!"
			KillTimer(hwnd,AUTOCLOSE_TIMER);
			
		}
		
		
		//只有在bSimuConnectAckLost=false时才发送ConnectAck, 即用不发来模拟发送ConnectAck信令丢失
		if(!bSimuConnectAckLost) 
		{
			sendto(*pSocketUp, (char*)pSendSignal,sizeof(SetupSignal),0,(SOCKADDR*)&addrToBS,len);
		}
		
		SetTimer(hwnd,CONNECTACK_TIMER,3000,NULL); //启动ConnectAck定时器，如果超时则表明没有收到Disconnect
		
	}
	
	
	if(0==strcmp(strMessageType,"DisConnect"))
	{
		::KillTimer(hwnd,CONNECTACK_TIMER);
		pSendSignal=&ReleaseSignal;
		sendto(*pSocketUp, (char*)pSendSignal,sizeof(SetupSignal),0,(SOCKADDR*)&addrToBS,len);
		
		
		sprintf(echoBuf, "Phone ---> BaseStaion: Release \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val, pSendSignal->progInd.elementId.Val);
		Sleep(5000);
		::PostMessage(hwnd,WMPhoneRecvData,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23
		
	}
	
}