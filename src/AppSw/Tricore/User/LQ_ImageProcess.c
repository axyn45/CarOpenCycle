/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�TC2/3xx���İ�
����    д��chiusir
��E-mail��chiusir@163.com
������汾��V1.1 ��Ȩ���У���λʹ��������ϵ��Ȩ
�������¡�2020��10��28��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://longqiu.taobao.com
------------------------------------------------
��dev.env.��AURIX Development Studio1.2.2�����ϰ汾
��Target �� TC2/3xx
��Crystal�� 20.000Mhz
��SYS PLL�� 200/300MHz
________________________________________________________________
����iLLD_1_0_1_11_0�ײ����,
ʹ�����̵�ʱ�򣬽������û�пո��Ӣ��·����
=================================================================
����������Ƶ��ַ��https://space.bilibili.com/95313236
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <LQ_CAMERA.h>

#include <LQ_DMA.h>
#include <LQ_GPIO_LED.h>
#include <LQ_GPIO_KEY.h>
#include <LQ_GTM.h>
#include <LQ_TFT18.h>
#include <IfxCpu.h>
#include <LQ_ADC.h>
#include <LQ_CCU6.h>
#include <LQ_STM.h>
#include <Main.h>
#include <Platform_Types.h>
#include <LQ_MotorServo.h>
#include <LQ_Inductor.h>
#include <LQ_GPT12_ENC.h>
#include "LQ_MT9V034.h"
#include "LQ_ImageProcess.h"
#include <stdbool.h>
#include <math.h>
/*************************************************************************
 *  �������ƣ�void TFT_Show_Camera_Info(void)
 *  ����˵������ʾ����������Ϣ
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��11��18��
 *  ��    ע��
 *************************************************************************/

void roundabout();
int ruhuan_flag=0;//���뻷���ı��
int yuanhuan_flag1=0;//Բ����һ�׶εı�־ ����һ�����ں����
int yuanhuan_flag2=0;//Բ���ڶ��׶εı�־ ֻ�������
int yuanhuan_flag3=0;//Բ�������׶εı�־ ���������ֻ��ֱ�ߺ���� ��ʼ����
int yuanhuan_flag4=0;//Բ�����Ľ׶εı�־ ��������
int yuanhuan_flag5=0;//Բ������׶εı�־ ������ ���
int Bin_image[60][94];//ͼ��Ķ�ά����
int huandao_flag = 0;  //�����ı�־ ������֮��ű��0
int continueleftrukou1;//��߽����� �жϳ���һ�����
int continueleftrukou2;//�жϳ���
void buxian();

void TFT_Show_Camera_Info (void)
{
    char txt[16] = "X:";

    sint16 mps = 0, dmm = 0;    // �ٶȣ�m/s,������ֵ
    sint16 pulse100 = 0;
    uint16 bat = 0;

    dmm = (sint16) (RAllPulse * 100 / 579);         // ����512�����������1��5790�����壬��ֵ̫�󣬳���100
    pulse100 = (sint16) (RAllPulse / 100);
    sprintf(txt, "AP:%05d00", pulse100);           //
    TFTSPI_P8X16Str(3, 4, txt, u16RED, u16BLUE);   // ��ʾ����ƫ�����

    NowTime = (STM_GetNowUs(STM0) - NowTime) / 1000;  // ��ȡSTM0 ��ǰʱ�䣬�õ�����
    mps = (sint16) (dmm / (NowTime / 1000));          // �����ٶ�mm/s
    // TFTSPI_Road(18, 0, LCDH, LCDW, (unsigned char *)Image_Use); // TFT1.8��̬��ʾ����ͷ�Ҷ�ͼ��
    TFTSPI_BinRoad(18, 0, LCDH, LCDW, (unsigned char *) Bin_Image);  // TFT1.8��̬��ʾ����ͷ������ͼ��
    sprintf(txt, "%04d,%04d,%04d", OFFSET0, OFFSET1, OFFSET2);
    TFTSPI_P8X16Str(0, 5, txt, u16RED, u16BLUE);       // ��ʾ����ƫ�����
    BatVolt = ADC_Read(ADC7);  // ˢ�µ�ص�ѹ
    bat = BatVolt * 11 / 25;  // x/4095*3.3*100*5.7
    sprintf(txt, "B:%d.%02dV %d.%02dm/s", bat / 100, bat % 100, mps / 1000, (mps / 10) % 100);  // *3.3/4095*3
    TFTSPI_P8X16Str(0, 6, txt, u16WHITE, u16BLUE);   // �ַ�����ʾ
    // ����Ͷ��������ʾ
    sprintf(txt, "Sv:%04d Rno:%d", ServoDuty, CircleNumber);
    TFTSPI_P8X16Str(1, 7, txt, u16RED, u16BLUE);     // ��ʾ��������1��������1��ֵ
    sprintf(txt, "M1:%04d, M2:%04d ", MotorDuty1, MotorDuty2);
    TFTSPI_P8X16Str(0, 8, txt, u16RED, u16BLUE);     // ���1-2��ֵ
    sprintf(txt, "E1:%04d, E2:%04d ", ECPULSE1, ECPULSE2);
    TFTSPI_P8X16Str(0, 9, txt, u16RED, u16BLUE);     // ������1-2��ֵ
}
/*************************************************************************
 *  �������ƣ�void CameraCar(void)
 *  ����˵������ų�˫������ٿ���
 -->1.�����㷨���򵥵ķֶα��������㷨����ѧ��ʾ�����㷨��
 2.�����㷨��PID����Ӧ�ÿ����㷨����ѧ��ʾ�����㷨��
 3.�߶��㷨���Ľ�����ȺЭͬ�����㷨��
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��10��28��
 *  ��    ע������2�����
 *************************************************************************/

void CameraCar(void)
{
    // ����ͷ��ʼ��
    CAMERA_Init(50);
    MotorInit();
    TFTSPI_P8X16Str(2, 0, "LQ 9V034 Car", u16RED, u16GREEN);
    TFTSPI_P8X16Str(1, 2, "K2 Show Video77", u16RED, u16GREEN);
    delayms(500);
    short duty = 1500; // 900ʱ���ٶȺ���

    // �м�CPU0,CPU1...������ͬʱ������Ļ��ʾ�������ͻ����ʾ
    mutexCpu0TFTIsOk = 0; // CPU1�� 0ռ��/1�ͷ� TFT
    // CircleNumber = SetCircleNum();  // ������Ҫ����Բ���ĸ�����

    // ��������Ҫ���ó���⣬�����ǹ̶�ִ�У�
    // �������Ҫ�ɻɹܺ��ⲿ�ж����ʵ��
    // ���������У��ɻɹ���ͨ��Բ������������������ܴ����Ŀ�����
    // OutInGarage(OUT_GARAGE, ReadOutInGarageMode()); // ���Գ��⣬��������������⣬��֮�Ҳ�����
    // OutInGarage(IN_GARAGE,ReadOutInGarageMode());  // �������

    TFTSPI_CLS(u16BLUE); // ����
    // �м�CPU0,CPU1...������ͬʱ������Ļ��ʾ�������ͻ����ʾ
    mutexCpu0TFTIsOk = 1;                // CPU1�� 0ռ��/1�ͷ� TFT
    MotorCtrl4w(duty, duty, duty, duty); // ���PWM�̶��������

    RAllPulse = 0;                // ȫ�ֱ����������������
    NowTime = STM_GetNowUs(STM0); // ��ȡSTM0 ��ǰʱ��
    sint16 tduty=1300;
    sint16 sduty_offset=0;

    char tstr[10];
    while (1)
    {
        LED_Ctrl(LED1, RVS); // LED��˸ ָʾ��������״̬
        if (Camera_Flag == 2)
        {
            Camera_Flag = 0;    // �������ͷ�ɼ���ɱ�־λ  �����������򲻻��ٴβɼ�����
            Get_Use_Image();    // ȡ����������ʾ����ͼ������
            Get_Bin_Image(0);   // ת��Ϊ01��ʽ���ݣ�0��1ԭͼ��2��3������ȡ
            Bin_Image_Filter(); // �˲������汻Χ�����ݽ����޸�Ϊͬһ��ֵ

            Seek_Road_Edge();
            buxian();
            TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Bin_Image);
            sprintf(tstr,"OFFSET0: %d",OFFSET0);
            TFTSPI_P8X16Str(1, 4, tstr, u16RED, u16GREEN);
            sprintf(tstr,"OFFSET1: %d",OFFSET1);
            TFTSPI_P8X16Str(1, 5, tstr, u16RED, u16GREEN);
            sprintf(tstr,"OFFSET2: %d",OFFSET2);
            TFTSPI_P8X16Str(1, 6, tstr, u16RED, u16GREEN);
            // ͨ���ڰ�����������������ƫ��ֵ


            // ��������ƫ��ֵ��ϵ��Խ����Խ�磬��ֵ������ķ�Χ�йأ��˴�Ϊ��160���ң�Ĭ��Ϊ7��
            sduty_offset=(OFFSET0 + OFFSET1 + OFFSET2) * 1 / 3;
//            if((OFFSET0>0&&OFFSET1<0)&&(OFFSET0<0&&OFFSET1>0)){
//                sduty_offset+=OFFSET0*1.2;
//            }
//            if((sduty_offset>=250||sduty_offset<=-250)&&(sduty_offset<300||sduty_offset>-300)){
//                sduty_offset=0.95*sduty_offset;
//            }
//            else if((sduty_offset>=300||sduty_offset<=-300)&&(sduty_offset<350||sduty_offset>-350)){
//                sduty_offset=0.90*sduty_offset;
//            }
//            else if((sduty_offset>=350||sduty_offset<=-350)&&(sduty_offset<400||sduty_offset>-400)){
//                sduty_offset=0.83*sduty_offset;
//            }
//            else if((sduty_offset>=400||sduty_offset<=-400)){
//                sduty_offset=0.78*sduty_offset;
//            }
            ServoDuty = Servo_Center_Mid - sduty_offset;

            ServoCtrl(ServoDuty); // ���PWM�����ת��

//             SPEED������ʶ���򣬸���Ϊ����
             MotorDuty1 = MtTargetDuty + ECPULSE1 * 4 - (OFFSET1 + OFFSET2 + OFFSET2) / 10;        // ���PWM
             MotorDuty2 = MtTargetDuty - ECPULSE2 * 4 + (OFFSET1 + OFFSET2 + OFFSET2) / 10;        // ˫�����֣���Ҫȥ��abs

            MotorCtrl(tduty, tduty); // ���ֵ������
                                   // TFT_Show_Camera_Info();
        }
        if (Game_Over)
        {
            OutInGarage(IN_GARAGE, ReadOutInGarageMode());
        }
    }
}
int Bisa_variance(int a[])
{
    int i;
    int avg=0;
    int result=0;
    for(i=0;i<60;i++)
    {
      avg=avg+a[i];
    }
    avg=avg/60;
    for(i=0;i<60;i++)
    {
        result=(a[i]-avg)*(a[i]-avg);
    }
    result=sqrt(result/60);
    return result;
}
//
int My_Abs(int a, int b)
{

            if ((a - b) > 0)
                return ((int)(a - b));
            else return ((int)(b - a));
}
//������������ʼ
void continuepanduan()
{
    int i = 0;
    continueleftrukou1 = 1;
    continueleftrukou2 = 1;
     //����������г��ִ���5�����䣬��Ϊ������
     //�����15�б����Բ���1-185��Χ�ڣ���Ϊ������
        for (i = 20; i <= 60; i++)
        {
            if (My_Abs(Road_Left[i], Road_Left[i + 1]) > 5)
            {
                continueleftrukou1 = 0;//���ڱ�־
                break;
            }
            if (Road_Left[30]==0)
            {
                continueleftrukou1 = 0;
                break;
            }
        }
        for(i=0;i<30;i++)
        {
            if (My_Abs(Road_Left[i], Road_Left[i + 1]) > 5)
            {
                continueleftrukou2 = 0;//����ҵ���
                break;
            }
            if(Road_Left[15]==0)
            {
                continueleftrukou2 = 0;//����ҵ���
                break;
            }
        }

}
bool left_edge_circle()//�ж���Բ����������һ�׶�
{
    bool result=false;
    int i;
    int flag=0;
    for(i=20;i<45;i++)
    {
        if(Road_Left[i]==0)
        {
          flag++;
        }
    }
    if(flag>16)
    {
        result=true;
    }
    return result;
}
bool left_circle_third()//�����׶�ͼ����ж�
{
    int i;
    int flag=0;
    int continue_left=0;//�жϵ����׶ε�һ����־
   for(i=10;i<60;i++)
   {
      flag++;
   }
   for(i=0;i<20;i++)
   {
      if(My_Abs(Road_Left[i], Road_Left[i + 1]) > 5)
      {
          continue_left=1;
      }
   }
   if(flag>45&&continue_left==1)
   {
     return true;
   }
   return false;
}
bool left_circle_forth()//���Ľ׶ε��ж� ���ұ߽綼������
{
    int i;
    int flag=0;
    for(i=0;i<60;i++)
    {
        if(Road_Left[i]==0&&Road_Right[i]!=0)
        {
            flag++;
        }
    }
    if(flag>45)
    {
        return true;
    }
    return false;
}
void buxian()//���� �뻷
{
  int x1,x2,y1,y2;//����������
  int i;
  for(int i=0;i<30;i++)
  {
      if(My_Abs(Road_Left[i], Road_Left[i + 1]) > 5)
      {
          x1=i;
          y1=Road_Left[i];
          break;
      }
  }
  x2=50;
  y2=Road_Left[50];
  for(i=x1;i<x2;i++)
  {
      Bin_image[i][Road_Left[i]]=0;
  }
  int k=(y1-y2)/(x1-x2);

  for(int i=x1;i<x2;i++)
  {
      Bin_image[i][k*i]=2;
    }
}

void chuhuan()//����
{
  int x1,x2,y1,y2;//����������
  int i;
  for(int i=0;i<40;i++)
  {
      if(My_Abs(Road_Left[i], Road_Left[i + 1]) > 5)
      {
          x1=i;
          y1=Road_Left[i];
          break;
      }
  }
    x2=35;
  y2=Road_Left[35];
  for(i=x1;i<x2;i++)
  {
      Bin_image[i][Road_Left[i]]=0;
  }
  int k=(y1-y2)/(x1-x2);

  for(int i=x1;i<x2;i++)
  {
      Bin_image[i][k*i]=2;
    }
}

void roundabout()
{
    continuepanduan();//�ж���߽��Ƿ�����
    if((continueleftrukou1==0&&continueleftrukou2==0)||left_edge_circle())
    {
        yuanhuan_flag1=1;
        continueleftrukou1=1;
        continueleftrukou2=1;
    }//��һ�׶�
    continuepanduan();//���ڶ��׶�
    if(yuanhuan_flag1&&continueleftrukou1==0&&continueleftrukou2==1)
    {
        yuanhuan_flag2=1;
    }//�ڶ��׶�
    if((yuanhuan_flag1&&yuanhuan_flag2)||left_circle_third())
    {
        yuanhuan_flag3=1;
        buxian();
        ServoDuty=1900;
        ServoCtrl(ServoDuty);
    }
    if((yuanhuan_flag1&&yuanhuan_flag2&&yuanhuan_flag3)||left_circle_forth())
    {
        yuanhuan_flag4=1;//����
        chuhuan();
        ServoDuty=1900;
        ServoCtrl(ServoDuty);
    }
    if(yuanhuan_flag1&&yuanhuan_flag2&&yuanhuan_flag3&&yuanhuan_flag4)
    {
       //��������
       yuanhuan_flag5=1;//���
       yuanhuan_flag1=0;
       yuanhuan_flag2=0;
       yuanhuan_flag4=0;
    }
}

