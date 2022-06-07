/* 包含头文件 */
#include "ioCC2530.h"
#include <stdio.h>
#include <stdlib.h>

#include "hal_i2c.h"
#include "sht.h"
#include "sht1x.h"
#include "sht3x.h"


#define LED1 P1_0     // P1_0定义为P1_0  led灯端口
#define uint16 unsigned short

#define uint unsigned int  //4Bytes
//uint tempereture,huimidity;
int8 sensor_tem;
uint8 sensor_humi;
uint counter=0; //统计中断次数

unsigned char tempHuim[2];//存放温湿值
char switchFlag;
void InitLED()
{
  P1SEL&=~0X01;          //设置 P1_0为 GPIO
  P1DIR |= 0x01;         //设置 p1_0 为输出口
  LED1=0;                //设置 p1_0 输出低电平
}


/**********串口通信初始化************************/
void initUART0(void)
{
  PERCFG &= ~0x01;	//外设控制寄存器，启用位置1
  P0SEL |= 0x0c;
  U0BAUD = 216;
  U0GCR = 11;
  U0CSR |= 0x80;
  U0UCR |= 0x80;        //停止其它操作，就绪
  UTX0IF = 0;           // 清零UART0 TX中断标志
  IEN0 |= 0X04;         //UART0 接收中断使能
  EA = 1;               //使能全局中断
}

/*************************************************
* 函数名称：initTimer1
* 功    能：初始化定时器T1控制状态寄存器

******************定时器初始化*****************************/
void initTimer1()
{
  
//=====》空白处开始
 


  //《=====空白处结束
  T1IF=0;             //清除timer1中断标志(同IRCON &= ~0x02)
  T1STAT &= ~0x01;    //清除通道0中断标志
  TIMIF &= ~0x40;     //不产生定时器1的溢出中断
  IEN1 |= 0x02;       //使能定时器1的中断
  EA = 1;             //使能全局中断
}


/**************单片机发送数据到串口******************/
void UART0SendData(unsigned char *str,int len )
{
  for(int i=0;i<len;i++)
  {
    U0DBUF = str[i];   // 将要发送的1字节数据写入U0DBUF
    while (!UTX0IF) ;  // 等待TX中断标志，即U0DBUF就绪
    UTX0IF = 0;        // 清零TX中断标志UART0SendByte(*str++); 
  }

}


/******************************************
* 功    能：定时器T1中断服务子程序
************************************/
#pragma vector = T1_VECTOR //中断服务子程序
__interrupt void T1_ISR(void)
{
  EA = 0;   //禁止全局中断
  counter++;
  T1STAT &= ~0x01;  //清除通道0中断标志
  EA = 1;   //使能全局中断
}
#pragma vector = URX0_VECTOR  
__interrupt void UART0_RX_ISR(void)  
{   
    URX0IF = 0;  
    switchFlag = U0DBUF;  
}  

/******************************************
* 函数名称：main
* 功    能：main函数入口
* 入口参数：无
* 出口参数：无
* 返 回 值：无
**************************************************/
void main(void)
{
  InitLED();
  initTimer1();  //初始化Timer1
  initUART0();  // UART0初始化
  SHT_Init();//初始化温湿度
  while(1)
  {
       

/*  
  //只支持旧温湿度
     call_sht11(&tempereture, &huimidity); 
     tempHuim[0]=tempereture&0xff00>>8;
     tempHuim[1]=huimidity&0x00ff;
    
     if(tempereture>=28)
  */
    
     //采集温湿度
 //=====》空白处开始 


        
  //《=====空白处结束
  if(sensor_tem>=0x16)// 0x16是22度
      LED1 = 1;
    if(counter>=10)     //发送
    {
      U0CSR &=~0X40;                 //禁止接收 
      counter=0;       //清标志位
       //发送温湿度数据到串口
       //=====》空白处开始
     
      
      
       //《=====空白处结束
    }
    else//接收
    {
      U0CSR |=0X40; //允许接收,每次接收都使能接收器 
      if(switchFlag==1)
        LED1 = 1;    //指示灯熄开
      else
        LED1 = 0;    //指示灯熄灭
    }
   
  }
}
