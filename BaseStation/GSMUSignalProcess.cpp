

#include "GSMUSignalProcess.h"
#include "stdafx.h"


extern USignalPaket SetupSignal;
extern USignalPaket AltertingSignal;
extern USignalPaket ConnectSignal;
extern USignalPaket DisconnectSignal;

bool bSimuConnectLost;
bool bSimuDisConnectLost;

//由于不能包含PhoneDlg.h 头文件，所以定义下面的全局变量来表示TIMER ID

unsigned int CONNECT_TIMER=3;
unsigned int DISCONNECT_TIMER=5;



void GSMUP_signalProcess(HWND hwnd, unsigned int WMRecvData,SOCKET* pSocketDown, char* pRecvBuf)
{
	
	
	USignalPaket* pRecvSignal; //接收到的信令
	USignalPaket* pSendSignal; //要发送的信令
	
	// socket 类型信息，占用端口为7000
	SOCKADDR_IN addrToPhone;
	addrToPhone.sin_family = AF_INET;
	addrToPhone.sin_port = htons(7000);
	addrToPhone.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");
	
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
	
	
	if( 0 != pRecvSignal->progInd.length.Val) //收到Setup
	{
		
		sprintf(tempBuf, "Phone ---> BaseStaion : %s \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val, pRecvSignal->progInd.elementId.Val,	
			pRecvSignal->progInd.length.Val,pRecvSignal->progInd.payload[0].Val);

	}

	else //收到MS发来的其他信令
	{
		sprintf(tempBuf, "Phone ---> BaseStaion : %s \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val);
	
	}


	Sleep(5000);
	
	::PostMessage(hwnd,WMRecvData,0,(LPARAM)tempBuf); //显示收到的信息 . xie 2009-7-22
	Sleep(100); //suspend一小段时间段，等待消息响应函数运行完。否则可能不能正常显示接收到得信息。
	


	if(0==strcmp(strMessageType,"Setup") )
	{

		pSendSignal=&AltertingSignal;
		//只有在bSimuConnectLost=false时才发送AltertingSignal, 即用不发来模拟发送AltertingSignal信令丢失
		if((!bSimuConnectLost))
		{
			sendto(*pSocketDown, (char*)pSendSignal,sizeof(USignalPaket),0,(SOCKADDR*)&addrToPhone,len);
		}
		
		sprintf(echoBuf, "BaseStaion ---> Phone : Alterting \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x 0x%02x\r\n",
			pSendSignal->messageType.Val, pSendSignal->progInd.elementId.Val,	
			pSendSignal->progInd.length.Val,pSendSignal->progInd.payload[0].Val,pSendSignal->progInd.payload[1].Val);
		
		Sleep(5000);
		::PostMessage(hwnd,WMRecvData,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23
		Sleep(100); //等消息处理函数运行完再发下一个信令。否则可能不能正常显示codeStream

		pSendSignal=&ConnectSignal;
		//只有在bSimuConnectLost=false时才发送ConnectSignal, 即用不发来模拟发送ConnectSignal信令丢失
		if((!bSimuConnectLost))
		{
			sendto(*pSocketDown, (char*)pSendSignal,sizeof(USignalPaket),0,(SOCKADDR*)&addrToPhone,len);
		}
		
		
		sprintf(echoBuf, "BaseStaion ---> Phone : Connect\r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val);
	    Sleep(5000);
		::PostMessage(hwnd,WMRecvData,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23


		SetTimer(hwnd,CONNECT_TIMER,3000,NULL); //启动Connect定时器，如果超时则表明没有收到Disconnect. xie 2009-7-24
	}
	
	if(  0==strcmp(strMessageType,"ConnectACK") )
	{
		::KillTimer(hwnd,CONNECT_TIMER); //成功接收Connect_ACK,关闭Connect定时器. xie 2009-7-24

		pSendSignal=&DisconnectSignal;

		//只有在bSimuConnectLost=false时才发送disConnectSignal, 即用不发来模拟发送disConnectSignal信令丢失
		if(!bSimuDisConnectLost)
		{
		
			sendto(*pSocketDown, (char*)pSendSignal,sizeof(USignalPaket),0,(SOCKADDR*)&addrToPhone,len);
		}
		
		sprintf(echoBuf, "BaseStaion ---> Phone :Disconnect \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val);
		Sleep(5000);
		::PostMessage(hwnd,WMRecvData,0,(LPARAM)echoBuf); //回显基站发送的信息 . xie 2009-7-23

		SetTimer(hwnd,DISCONNECT_TIMER,3000,NULL); //启动disConnect定时器，如果超时则表明没有收到Disconnect. xie 2009-7-24
		
	}

	if( 0==strcmp(strMessageType,"Release")  )
	{
		::KillTimer(hwnd,DISCONNECT_TIMER); //成功接收Connect_ACK,关闭Connect定时器. xie 2009-7-24		
		
	}
	
}