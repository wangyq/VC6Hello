//*****************************************************************************
//**  FILENAME: GSMUSignalFormat.h
//**  Purpose: ������ʾ������õ������ݽṹ
//**  Comments: ���������USignalPaket�ĸ�ʽ���ͣ����û����Ӧ�Ĳ��֣�������Ϊ0��
//**            �ڽ��ն�ͨ�����lenth��ֵ������ȷ�������ʾ��������ɼ�������ݽṹ���ظ����塣
//**            ��������ʾ�ĽǶȿ�����������ǿ��еġ�
//**            
//**
//**  Create Date:         2009-7-23
//**  Last Modification:   2009-7-23
//**  
//**  Author: WeiGuang Xie
//**  E-Mail: wg_xie@hotmail.com
//**           hotcoffee@126.com
//**
//******************************************************************************

#include "GSMUSignalFormat.h"
#include "stdafx.h"



USignalPaket SetupSignal;
USignalPaket AltertingSignal;
USignalPaket ConnectSignal;
USignalPaket ConnectACKSignal;
USignalPaket DisconnectSignal;
USignalPaket ReleaseSignal;

//��ʼ��u����
void GSMU_InitSignal(void)
{
	SetupSignal.messageType.Val=0x45;
	SetupSignal.progInd.elementId.Val=0x04;
	SetupSignal.progInd.length.Val=0x01;
	SetupSignal.progInd.payload[0].Val=0x88;
	SetupSignal.progInd.payload[1].Val=0x00;


	AltertingSignal.messageType.Val=0x01;
	AltertingSignal.progInd.elementId.Val=0x1e;
	AltertingSignal.progInd.length.Val=0x02;
	AltertingSignal.progInd.payload[0].Val=0xe4;
	AltertingSignal.progInd.payload[1].Val=0x88;


	ConnectSignal.messageType.Val=0x07;
	ConnectSignal.progInd.elementId.Val=0x00;
	ConnectSignal.progInd.length.Val=0x00;
	ConnectSignal.progInd.payload[0].Val=0x00;
	ConnectSignal.progInd.payload[1].Val=0x00;

	ConnectACKSignal.messageType.Val=0x8f;
	ConnectACKSignal.progInd.elementId.Val=0x00;
	ConnectACKSignal.progInd.length.Val=0x00;
	ConnectACKSignal.progInd.payload[0].Val=0x00;
	ConnectACKSignal.progInd.payload[1].Val=0x00;

	DisconnectSignal.messageType.Val=0xe5;
	DisconnectSignal.progInd.elementId.Val=0x00;
	DisconnectSignal.progInd.length.Val=0x00;
	DisconnectSignal.progInd.payload[0].Val=0x00;
	DisconnectSignal.progInd.payload[1].Val=0x00;

	ReleaseSignal.messageType.Val=0x2d;
	ReleaseSignal.progInd.elementId.Val=0x00;
	ReleaseSignal.progInd.length.Val=0x00;
	ReleaseSignal.progInd.payload[0].Val=0x00;
	ReleaseSignal.progInd.payload[1].Val=0x00;
	

}
