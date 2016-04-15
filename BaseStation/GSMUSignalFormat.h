
//*****************************************************************************
//**  FILENAME: GSMUSignalFormat.h
//**  Purpose: 定义演示软件所用到得数据结构
//**  Comments: 所以信令都按USignalPaket的格式发送，如果没有相应的部分，则设其为0。
//**            在接收端通过检测lenth的值，即可确定如何显示信令。这样可极大简化数据结构的重复定义。
//**            仅仅从演示的角度考虑这种设计是可行的。
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




//定义了位域格式的一个Byte，可具体操作某一bit. 

//#pragma pack(1)  //按1字节对齐方式对齐
//系统按4字节的方式对齐，上面的那条预编译宏好像没起作用。暂且用默认的对齐方式。
//虽然浪费了一些空间，但只要保证收发双方的对齐方式相同即可。
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
	BYTE_VAL length; //通过length判断负载大小，像ack这样的信令lenght=0. 
							//对ack这样的信令来说payload[2]只起一个占位的作用
	BYTE_VAL payload[2];

} ProgIndicator;


//最简单的u信令格式，只是用来模拟，不符合链路层协议LAPDm的信令格式.
typedef struct _uSignalPaket
{
  BYTE_VAL messageType;
  
  ProgIndicator progInd;
  
  //16位crc校验值，暂时未用。采用超时重传机制。
  //BYTE_VAL crc0; 
  //BYTE_VAL crc1;

 } USignalPaket;

//#pragma pack() //恢复系统默认对齐值




void GSMU_InitSignal(void);

//void GSMU_crcCheck(UsingalPaket*);





#endif