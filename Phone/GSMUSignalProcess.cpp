

#include "GSMUSignalProcess.h"

#include "stdafx.h"


extern USignalPaket SetupSignal;
extern USignalPaket ConnectACKSignal;
extern USignalPaket ReleaseSignal;


bool bSimuSetupLost;
bool bSimuConnectAckLost;
bool bSimuNoErr;


//���ڲ��ܰ���PhoneDlg.h ͷ�ļ������Զ��������ȫ�ֱ�������ʾTIMER ID

unsigned int SETUP_TIMER=1;
unsigned int CONNECTACK_TIMER=4;
unsigned int  AUTOCLOSE_TIMER=100;



void GSMUP_signalProcess(HWND hwnd, unsigned int WMPhoneRecvData,SOCKET* pSocketUp, char* pRecvBuf)
{
	
	
	USignalPaket* pRecvSignal; //���յ�������
	USignalPaket* pSendSignal; //Ҫ���͵�����
	
	// socket ������Ϣ��ռ�ö˿�Ϊ5000
	SOCKADDR_IN addrToBS;
	addrToBS.sin_family = AF_INET;
	addrToBS.sin_port = htons(5000);
	addrToBS.sin_addr.S_un.S_addr =inet_addr("127.0.0.1");
	
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
	
	
	if( 0 != pRecvSignal->progInd.length.Val) //�յ�Altering
	{	
		sprintf(tempBuf, "BaseStaion ---> Phone: %s \r\nCode Stream:\r\n    MessageType: 0x%02x, ElementIdentifier: 0x%02x, Length: 0x%02x, Contents: 0x%02x 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val, pRecvSignal->progInd.elementId.Val,	
			pRecvSignal->progInd.length.Val,pRecvSignal->progInd.payload[0].Val,pRecvSignal->progInd.payload[1].Val);
		
	}
	else //�յ�û�и��ص�����
	{
		
		sprintf(tempBuf, "BaseStaion ---> Phone: %s \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			strMessageType, pRecvSignal->messageType.Val);
		
	}
	
	
	Sleep(5000);
	::PostMessage(hwnd,WMPhoneRecvData,0,(LPARAM)tempBuf); //��ʾ�յ�����Ϣ . xie 2009-7-22
	Sleep(100);	//������Ϣ����������. xie 2009-7-22
	
	
	
	
	if(0==strcmp(strMessageType,"Connect"))
	{
		//::KillTimer(hwnd,SETUP_TIMER);
		
		pSendSignal=&ConnectACKSignal;
		
		sprintf(echoBuf, "Phone ---> BaseStaion: ConnectACK \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val, pSendSignal->progInd.elementId.Val);
		Sleep(5000);
		::PostMessage(hwnd,WMPhoneRecvData,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23
		
		SetTimer(hwnd,AUTOCLOSE_TIMER,5000,NULL);
		int retVal=AfxMessageBox("��ʼͨ��!\r\n����ͨ����ѡ��ȷ����\r\n5���Ӻ�ǿ�ƽ���ͨ��",MB_OK);
		
		if(IDOK==retVal )
		{
			//����ͨ��!"
			KillTimer(hwnd,AUTOCLOSE_TIMER);
			
		}
		
		
		//ֻ����bSimuConnectAckLost=falseʱ�ŷ���ConnectAck, ���ò�����ģ�ⷢ��ConnectAck���ʧ
		if(!bSimuConnectAckLost) 
		{
			sendto(*pSocketUp, (char*)pSendSignal,sizeof(SetupSignal),0,(SOCKADDR*)&addrToBS,len);
		}
		
		SetTimer(hwnd,CONNECTACK_TIMER,3000,NULL); //����ConnectAck��ʱ���������ʱ�����û���յ�Disconnect
		
	}
	
	
	if(0==strcmp(strMessageType,"DisConnect"))
	{
		::KillTimer(hwnd,CONNECTACK_TIMER);
		pSendSignal=&ReleaseSignal;
		sendto(*pSocketUp, (char*)pSendSignal,sizeof(SetupSignal),0,(SOCKADDR*)&addrToBS,len);
		
		
		sprintf(echoBuf, "Phone ---> BaseStaion: Release \r\nCode Stream:\r\n    MessageType: 0x%02x\r\n",
			pSendSignal->messageType.Val, pSendSignal->progInd.elementId.Val);
		Sleep(5000);
		::PostMessage(hwnd,WMPhoneRecvData,0,(LPARAM)echoBuf); //���Ի�վ���͵���Ϣ . xie 2009-7-23
		
	}
	
}