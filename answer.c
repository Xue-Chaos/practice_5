/* ����ͷ�ļ� */
#include "ioCC2530.h"
#include <stdio.h>
#include <stdlib.h>

#include "hal_i2c.h"
#include "sht.h"
#include "sht1x.h"
#include "sht3x.h"


#define LED1 P1_0     // P1_0����ΪP1_0  led�ƶ˿�
#define uint16 unsigned short

#define uint unsigned int  //4Bytes
//uint tempereture,huimidity;
int8 sensor_tem;
uint8 sensor_humi;
uint counter=0; //ͳ���жϴ���

unsigned char tempHuim[2];//�����ʪֵ
char switchFlag;
void InitLED()
{
  P1SEL&=~0X01;          //���� P1_0Ϊ GPIO
  P1DIR |= 0x01;         //���� p1_0 Ϊ�����
  LED1=0;                //���� p1_0 ����͵�ƽ
}


/**********����ͨ�ų�ʼ��************************/
void initUART0(void)
{
  PERCFG &= ~0x01;	//������ƼĴ���������λ��1
  //=====���հ״���ʼ
  P0SEL = 0x3c;	        //���� p0's 2,3,4,5 Ϊ1����Ϊ����ڣ�����λΪ0
  U0CSR |= 0x80;        //����uart�첽ģʽ
  U0BAUD = 216;    //115200
  U0GCR = 11;
  //��=====�հ״�����
  U0UCR |= 0x80;        //ֹͣ��������������
  UTX0IF = 0;           // ����UART0 TX�жϱ�־
  IEN0 |= 0X04;         //UART0 �����ж�ʹ��
  EA = 1;               //ʹ��ȫ���ж�
}

/*************************************************
* �������ƣ�initTimer1
* ��    �ܣ���ʼ����ʱ��T1����״̬�Ĵ���

******************��ʱ����ʼ��*****************************/
void initTimer1()
{
  
//=====���հ״���ʼ
  CLKCONCMD &= 0x80;   //ʱ���ٶ�����Ϊ32MHz
  T1CTL = 0x0E;        // ����128��Ƶ��ģ�Ƚϼ�������ģʽ������ʼ����
  T1CCTL0 |= 0x04;    //�趨timer1ͨ��0�Ƚ�ģʽ
  T1CC0L =50000 & 0xFF;    // 0.2S,��50000�ĵ�8λд��T1CC0L
  T1CC0H = (50000 & 0xFF00) >> 8; //��50000�ĸ�8λд��T1CC0H
  //��=====�հ״�����
  T1IF=0;             //���timer1�жϱ�־(ͬIRCON &= ~0x02)
  T1STAT &= ~0x01;    //���ͨ��0�жϱ�־
  TIMIF &= ~0x40;     //��������ʱ��1������ж�
  IEN1 |= 0x02;       //ʹ�ܶ�ʱ��1���ж�
  EA = 1;             //ʹ��ȫ���ж�
}


/**************��Ƭ���������ݵ�����******************/
void UART0SendData(unsigned char *str,int len )
{
  for(int i=0;i<len;i++)
  {
    U0DBUF = str[i];   // ��Ҫ���͵�1�ֽ�����д��U0DBUF
    while (!UTX0IF) ;  // �ȴ�TX�жϱ�־����U0DBUF����
    UTX0IF = 0;        // ����TX�жϱ�־UART0SendByte(*str++); 
  }

}


/******************************************
* ��    �ܣ���ʱ��T1�жϷ����ӳ���
************************************/
#pragma vector = T1_VECTOR //�жϷ����ӳ���
__interrupt void T1_ISR(void)
{
  EA = 0;   //��ֹȫ���ж�
  counter++;
  T1STAT &= ~0x01;  //���ͨ��0�жϱ�־
  EA = 1;   //ʹ��ȫ���ж�
}
#pragma vector = URX0_VECTOR  
__interrupt void UART0_RX_ISR(void)  
{   
    URX0IF = 0;  
    switchFlag = U0DBUF;  
}  

/******************************************
* �������ƣ�main
* ��    �ܣ�main�������
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
**************************************************/
void main(void)
{
  InitLED();
  initTimer1();  //��ʼ��Timer1
  initUART0();  // UART0��ʼ��
  SHT_Init();//��ʼ����ʪ��
  while(1)
  {
       
 //�ɼ���ʪ��
 //=====���հ״���ʼ 
    SHT_SmpSnValue((int8 *)(&sensor_tem), (uint8 *)(&sensor_humi));
  tempHuim[0]= sensor_tem;
  tempHuim[1]= sensor_humi;  
  //��=====�հ״�����
/*  
  //ֻ֧�־���ʪ��
     call_sht11(&tempereture, &huimidity); 
     tempHuim[0]=tempereture&0xff00>>8;
     tempHuim[1]=huimidity&0x00ff;
    
     if(tempereture>=28)
  */
  if(sensor_tem>=0x16)// 0x16��22��
      LED1 = 1;
    if(counter>=10)     //����
    {
      U0CSR &=~0X40;                 //��ֹ���� 
      counter=0;       //���־λ
       //������ʪ�����ݵ�����
       //=====���հ״���ʼ
      UART0SendData(tempHuim,2);
       //��=====�հ״�����
    }
    else//����
    {
      U0CSR |=0X40; //�������,ÿ�ν��ն�ʹ�ܽ����� 
      if(switchFlag==1)
        LED1 = 1;    //ָʾ��Ϩ��
      else
        LED1 = 0;    //ָʾ��Ϩ��
    }
   
  }
}
