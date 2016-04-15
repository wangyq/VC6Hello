

#include "GSMUSignalProcess.h"
#include "stdafx.h"


extern USignalPaket SetupSignal;
extern USignalPaket AltertingSignal;
extern USignalPaket ConnectSignal;
extern USignalPaket DisconnectSignal;

bool bSimuConnectLost;
bool bSimuDisConnectLost;

//���ڲ��ܰ���PhoneDlg.h ͷ�ļ������Զ��������ȫ�ֱ�������ʾTIMER ID

unsigned int CONNECT_TIMER=3;
unsigned int DISCONNECT_TIMER=5;



void GSMUP_signalProcess(HWND hwnd, unsigned int WMRecvData,SOCKET* pSocketDown, char* pRecvBuf)
{
	
	
	USignalPaket* pRecvSignal; //���յ�������
	USignalPaket* pSendSignal; //Ҫ���͵�����
	
	// socket ������Ϣ��ռ�ö˿�Ϊ7000
	SOCKADDR_IN addrToPhone;
	addrToPhone.sin_family = AF_INET;
	addrToPhone.sin_port = htons(7000);
	addrToPhone.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");
	
	int len = sizeof(SOCKADDR);
	
	char strMessageType[15]={0}; //�洢���յ�����������
	char tempBuf[200]={0}; //�洢��ʽ���ַ���
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
	
	
	if( 0 != pRecvSignal->progInd.length.Val) //�յ�Setup
	{
		
		sprintf(tempBuf, "Phone ---> BaseStaion : %s \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val, pRecvSignal->progInd.elementId.Val,	
			pRecvSignal->progInd.length.Val,pRecvSignal->progInd.payload[0].Val);

	}

	else //�յ�MS��������������
	{
		sprintf(tempBuf, "Phone ---> BaseStaion : %s \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val);
	
	}


	Sleep(5000);
	
	::PostMessage(hwnd,WMRecvData,0,(LPARAM)tempBuf); //��ʾ�յ�����Ϣ . xie 2009-7-22
	Sleep(100); //suspendһС��ʱ��Σ��ȴ���Ϣ��Ӧ���������ꡣ������ܲ���������ʾ���յ�����Ϣ��
	


	if(0==strcmp(strMessageType,"Setup") )
	{

		pSendSignal=&AltertingSignal;
		//ֻ����bSimuConnectLost=falseʱ�ŷ���AltertingSignal, ���ò�����ģ�ⷢ��AltertingSignal���ʧ
		if((!bSimuConnectLost))
		{
			sendto(*pSocketDown, (char*)pSendSignal,sizeof(USignalPaket),0,(SOCKADDR*)&addrToPhone,len);
		}
		
		sprintf(echoBuf, "BaseStaion ---> Phone : Alterting \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x 0x%02x\r\n",
			pSendSignal->messageType.Val, pSendSignal->progInd.elementId.Val,	
			pSendSignal->progInd.length.Val,pSendSignal->progInd.payload[0].Val,pSendSignal->progInd.payload[1].Val);
		
		Sleep(5000);
		::PostMessage(hwnd,WMRecvData,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23
		Sleep(100); //����Ϣ�������������ٷ���һ�����������ܲ���������ʾcodeStream

		pSendSignal=&ConnectSignal;
		//ֻ����bSimuConnectLost=falseʱ�ŷ���ConnectSignal, ���ò�����ģ�ⷢ��ConnectSignal���ʧ
		if((!bSimuConnectLost))
		{
			sendto(*pSocketDown, (char*)pSendSignal,sizeof(USignalPaket),0,(SOCKADDR*)&addrToPhone,len);
		}
		
		
		sprintf(echoBuf, "BaseStaion ---> Phone : Connect\r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val);
	    Sleep(5000);
		::PostMessage(hwnd,WMRecvData,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23


		SetTimer(hwnd,CONNECT_TIMER,3000,NULL); //����Connect��ʱ���������ʱ�����û���յ�Disconnect. xie 2009-7-24
	}
	
	if(  0==strcmp(strMessageType,"ConnectACK") )
	{
		::KillTimer(hwnd,CONNECT_TIMER); //�ɹ�����Connect_ACK,�ر�Connect��ʱ��. xie 2009-7-24

		pSendSignal=&DisconnectSignal;

		//ֻ����bSimuConnectLost=falseʱ�ŷ���disConnectSignal, ���ò�����ģ�ⷢ��disConnectSignal���ʧ
		if(!bSimuDisConnectLost)
		{
		
			sendto(*pSocketDown, (char*)pSendSignal,sizeof(USignalPaket),0,(SOCKADDR*)&addrToPhone,len);
		}
		
		sprintf(echoBuf, "BaseStaion ---> Phone :Disconnect \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val);
		Sleep(5000);
		::PostMessage(hwnd,WMRecvData,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23

		SetTimer(hwnd,DISCONNECT_TIMER,3000,NULL); //����disConnect��ʱ���������ʱ�����û���յ�Disconnect. xie 2009-7-24
		
	}

	if( 0==strcmp(strMessageType,"Release")  )
	{
		::KillTimer(hwnd,DISCONNECT_TIMER); //�ɹ�����Connect_ACK,�ر�Connect��ʱ��. xie 2009-7-24		
		
	}
	
}