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
#include<stdbool.h>
/*************************************************************************
 *  �������ƣ�void TFT_Show_Camera_Info(void)
 *  ����˵������ʾ����������Ϣ
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2020��11��18��
 *  ��    ע��
 *************************************************************************/
bool isleft();
bool isright();
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
void CameraCar (void)
{
    // ����ͷ��ʼ��
    CAMERA_Init(50);
    MotorInit();
    TFTSPI_P8X16Str(2, 3, "LQ 9V034 Car", u16RED, u16GREEN);
    TFTSPI_P8X16Str(1, 5, "K2 Show Video", u16RED, u16GREEN);
    delayms(500);
    short duty = 1500;//900ʱ���ٶȺ���

    // �м�CPU0,CPU1...������ͬʱ������Ļ��ʾ�������ͻ����ʾ
    mutexCpu0TFTIsOk = 0;           // CPU1�� 0ռ��/1�ͷ� TFT
    // CircleNumber = SetCircleNum();  // ������Ҫ����Բ���ĸ�����

    // ��������Ҫ���ó���⣬�����ǹ̶�ִ�У�
    // �������Ҫ�ɻɹܺ��ⲿ�ж����ʵ��
    // ���������У��ɻɹ���ͨ��Բ������������������ܴ����Ŀ�����
    //OutInGarage(OUT_GARAGE, ReadOutInGarageMode()); // ���Գ��⣬��������������⣬��֮�Ҳ�����
    //OutInGarage(IN_GARAGE,ReadOutInGarageMode());  // �������

    TFTSPI_CLS(u16BLUE);            // ����
    // �м�CPU0,CPU1...������ͬʱ������Ļ��ʾ�������ͻ����ʾ
    mutexCpu0TFTIsOk = 1;           // CPU1�� 0ռ��/1�ͷ� TFT
    MotorCtrl4w(duty, duty,duty,duty); // ���PWM�̶��������

    RAllPulse = 0;                  // ȫ�ֱ����������������
    NowTime = STM_GetNowUs(STM0);   // ��ȡSTM0 ��ǰʱ��
    while (1)
    {
        LED_Ctrl(LED1, RVS);     // LED��˸ ָʾ��������״̬
        if (Camera_Flag == 2)
        {
            Camera_Flag = 0;     // �������ͷ�ɼ���ɱ�־λ  �����������򲻻��ٴβɼ�����
            Get_Use_Image();     // ȡ����������ʾ����ͼ������
            Get_Bin_Image(0);    // ת��Ϊ01��ʽ���ݣ�0��1ԭͼ��2��3������ȡ
            Bin_Image_Filter();  // �˲������汻Χ�����ݽ����޸�Ϊͬһ��ֵ
            Seek_Road();
            Seek_Road_Edge();
            TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *) Bin_Image);
              // ͨ���ڰ�����������������ƫ��ֵ

            // FindEdge();

            // ��������ƫ��ֵ��ϵ��Խ����Խ�磬��ֵ������ķ�Χ�йأ��˴�Ϊ��160���ң�Ĭ��Ϊ7��
            ServoDuty = Servo_Center_Mid - (OFFSET1 + OFFSET2 + OFFSET2) * 1 / 3;
            if(isright())//����ת��
            {
               ServoDuty=1900;
            }
            if(isleft())//����ת��
            {
                ServoDuty=1900;
            }

//            if(iscircle())
//            {
//
//            }
            // Բ������������Ϊ��������ֵԽ��˵��Խƫ��ߣ�
            if((OFFSET2 < -300)||(OFFSET2 > 300))
                ServoDuty = Servo_Center_Mid - OFFSET2 / 7;

            ServoCtrl(ServoDuty);     // ���PWM�����ת��

            // SPEED������ʶ���򣬸���Ϊ����
            MotorDuty1 = MtTargetDuty + ECPULSE1 * 4 - (OFFSET1 + OFFSET2 + OFFSET2) / 10;        // ���PWM
            MotorDuty2 = MtTargetDuty - ECPULSE2 * 4 + (OFFSET1 + OFFSET2 + OFFSET2) / 10;        // ˫�����֣���Ҫȥ��abs

            MotorCtrl(MotorDuty1, MotorDuty2);        // ���ֵ������
           // TFT_Show_Camera_Info();
        }
        if (Game_Over)
        {
            OutInGarage(IN_GARAGE, ReadOutInGarageMode());
        }
    }
}
bool isright()//�ж��Ƿ�Ϊ��ֱ��ת�� ����ת���return true
{
     int i;
     int flag=0;
     for(i=0;i<60;i++)
     {
         if(right_edge[i]==93)
         {
             flag++;
         }
     }
     if(flag>50)
     {
       return true;
     }
    return false;
}
bool isleft()//�ж��Ƿ�Ϊ��ֱ��ת�� ����ת���return true
{
     int i;
     int flag=0;
     for(i=0;i<60;i++)
     {
         if(left_edge[i]==0)
         {
             flag++;
         }
     }
     if(flag>50)
     {
       return true;
     }
    return false;
}

bool is_circel()//�ж��Ƿ���Բ��
{
    int i;
    int flag=0;
    for(i=15;i<40;i++)
    {
        if(left_edge[i]==0)
        {
            flag++;
        }
    }
    if(flag>20)
    {
      return true;
    }
   return false;
}
int getsize(int x[])
{   int i;
    int flag=0;
    for(i=0;x[i]!='\0';i++)
    {
        flag++;
    }

    return flag;
}


void deal(int x[])//���ÿ���Ĭ�� ���㷽��
{
    int i;
    int n=getsize(x);//x�����С
    float a0,a1,temp,temp0,temp1;
    float sy=0,sx=0,sxx=0,syy=0,sxy=0,sxxy=0,sxxx=0,sxxxx=0;//������ر���
    for(i=0;i<n;i++)
    {
        sx+=i;//����xi�ĺ�
        sy+=x[i];//����yi�ĺ�
        sxx+=i*i;//����xi��ƽ���ĺ�
        sxxx+=pow(i,3);//����xi�������ĺ�
        sxxxx+=pow(i,4);//����xi��4�η��ĺ�
        sxy+=i*x[i];//����xi��yi�ĵĺ�
        sxxy+=i*i*x[i];//����xiƽ����yi�ĺ�
    }
    temp=n*sxx-sx*sx;//���̵�ϵ������ʽ
    temp0=sy*sxx-sx*sxy;
    temp1=n*sxy-sy*sx;
    a0=temp0/temp;
    a1=temp1/temp;
    printf("f(x)=%3.3fx+%3.3f\n",a1,a0);
}
