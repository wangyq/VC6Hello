
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


#ifndef _GSMUSIGNALFORMAT_H
#define _GSMUSIGNALFORMAT_H




//������λ���ʽ��һ��Byte���ɾ������ĳһbit. 

//#pragma pack(1)  //��1�ֽڶ��뷽ʽ����
//ϵͳ��4�ֽڵķ�ʽ���룬���������Ԥ��������û�����á�������Ĭ�ϵĶ��뷽ʽ��
//��Ȼ�˷���һЩ�ռ䣬��ֻҪ��֤�շ�˫���Ķ��뷽ʽ��ͬ���ɡ�
typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    unsigned char Val;
} BYTE_VAL;


typedef struct _progress_indicator
{
	BYTE_VAL elementId;
	BYTE_VAL length; //ͨ��length�жϸ��ش�С����ack����������lenght=0. 
							//��ack������������˵payload[2]ֻ��һ��ռλ������
	BYTE_VAL payload[2];

} ProgIndicator;


//��򵥵�u�����ʽ��ֻ������ģ�⣬��������·��Э��LAPDm�������ʽ.
typedef struct _uSignalPaket
{
  BYTE_VAL messageType;
  
  ProgIndicator progInd;
  
  //16λcrcУ��ֵ����ʱδ�á����ó�ʱ�ش����ơ�
  //BYTE_VAL crc0; 
  //BYTE_VAL crc1;

 } USignalPaket;

//#pragma pack() //�ָ�ϵͳĬ�϶���ֵ




void GSMU_InitSignal(void);

//void GSMU_crcCheck(UsingalPaket*);





#endif