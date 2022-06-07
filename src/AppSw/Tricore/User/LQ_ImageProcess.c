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
int yuanhuan_flag3_1=0;
int yuanhuan_flag3=0;//Բ�������׶εı�־ ���������ֻ��ֱ�ߺ���� ��ʼ����
int yuanhuan_flag4=0;//Բ�����Ľ׶εı�־ ��������
int yuanhuan_flag5=0;//Բ������׶εı�־ ������ ���
int Bin_image[60][94];//ͼ��Ķ�ά����
int huandao_flag = 0;  //�����ı�־ ������֮��ű��0
int continueleftrukou1;//��߽����� �жϳ���һ�����
int continueleftrukou2;//�жϳ���


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
            roundabout();

            TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Bin_Image);
//            sprintf(tstr,"left_t: %d %d",Road_Left_Top[0],Road_Left_Top[1]);
//            TFTSPI_P8X16Str(1, 4, tstr, u16RED, u16GREEN);
//
//            sprintf(tstr,"right_t: %d %d",Road_Right_Top[0],Road_Right_Top[1]);
//            TFTSPI_P8X16Str(1, 5, tstr, u16RED, u16GREEN);
//
//
//            sprintf(tstr,"left_b: %d %d",Road_Left_Bottom[0],Road_Left_Bottom[1]);
//            TFTSPI_P8X16Str(1, 6, tstr, u16RED, u16GREEN);
//
////            sprintf(tstr,"right_b: %d %d",Road_Right_Bottom[0],Road_Right_Bottom[1]);
//            sprintf(tstr,"l:%d %d %d %d %d",line_elements[59].left,line_elements[49].left,line_elements[39].left,line_elements[29].left,line_elements[9].left);
//            TFTSPI_P8X16Str(0, 7, tstr, u16RED, u16GREEN);



//             sprintf(tstr,"left_b: %d %d",Road_Left_Bottom[0],Road_Left_Bottom[1]);
//             TFTSPI_P8X16Str(1, 6, tstr, u16RED, u16GREEN);

// //            sprintf(tstr,"right_b: %d %d",Road_Right_Bottom[0],Road_Right_Bottom[1]);
//             sprintf(tstr,"l:%d %d %d %d %d",line_elements[59].left,line_elements[49].left,line_elements[39].left,line_elements[29].left,line_elements[9].left);
//             TFTSPI_P8X16Str(0, 7, tstr, u16RED, u16GREEN);


            TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Bin_Image);
//             dots2line(3,3,60,25,1);
//              TFTSPI_BinRoad(0, 0, LCDH, LCDW, (unsigned char *)Bin_Image);
// //             sint16 break_line=servo_control();
//              sprintf(tstr,"%d",servo_control());
//              TFTSPI_P8X16Str(1, 4, tstr, u16RED, u16GREEN);

//              sprintf(tstr,"left_t: %d %d",Road_Left_Top[0],Road_Left_Top[1]);
//              TFTSPI_P8X16Str(1, 5, tstr, u16RED, u16GREEN);

//              sprintf(tstr,"right_t: %d %d",Road_Right_Top[0],Road_Right_Top[1]);
//              TFTSPI_P8X16Str(1, 6, tstr, u16RED, u16GREEN);

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
//        if (Game_Over)
//        {
//            OutInGarage(IN_GARAGE, ReadOutInGarageMode());
//        }
    }
}
int My_Abs(int a, int b)//�����ֵ
{

            if ((a - b) > 0)
                return ((int)(a - b));
            else return ((int)(b - a));
}
//������������ʼ

int Right_con;//�ұ������ı�־
void right_continue()//�ұ߽������ж�
{
     int i=0;
     Right_con=1;
     for(i=0;i<60;i++)
     {
         if(My_Abs(line_elements[i].left, line_elements[i + 1].left) > 5)
         {

         }
     }

}
bool no_black()//�����׶� �·�û�к�ɫ����
{
   int flag=0;
   for(int i=60;i>30;i--)
   {
       for(int j=0;j<40;j++)
       {
           if(Bin_Image[i][j]==0)
           {
               flag++;
           }
       }
   }
   if(flag<100)
   {
       return true;
   }
   return false;
}
bool lose_left_line()//��ߵı߽��߶�����
{
    bool ok=false;
    int flag=0;
    for(int i=60;i>30;i--)
    {
        if(line_elements[i].left==-1)
        {
            flag++;
        }
    }
    if(flag>15)
    {
        ok=true;
    }
    return ok;
}
bool have_black_area()//�ж���û��Բ����һ��ĺ�ɫ����
{
    //����Bin_Image�����ж�
    int flag=0;
    for(int i=20;i<45;i++)
    {
        for(int j=0;j<50;j++)
        {
            if(Bin_Image[i][j]==0)
            {
                flag++;
            }
        }
    }
    if(flag>250)
    {
        return true;
    }
    return false;
}

void first_stage()//Բ���õ�һ�׶θ��ݿ���ж�
{
    int turn_wid=0;//��·�仯���
    int turn_narrow=0;
    int turn_nar1=0;
    for(int i=60;i>50;i--)
    {
        //����·�ı仯���
        if(line_elements[i].width>line_elements[i+1].width)
        {
            turn_nar1=1;

        }
    }
//    int flag=0;
    for(int i=30;i>0;i--)
    {
        if(line_elements[i].width>line_elements[i+1].width)
               {
                   turn_narrow=1;

               }
    }
    for(int j=50;j>30;j--)
    {
        if(line_elements[j].width<line_elements[j+1].width)
        {
            turn_wid=1;
        }
    }
    //�ұ߽���ֱ�������������1
    if(turn_wid&&turn_narrow&&turn_nar1&&!have_black_area()&&!lose_left_line()&&!yuanhuan_flag2)//���ܵ���
    {
        yuanhuan_flag1=1;
    }
}
bool turn_wid_nar()
{
    int turn_nar1=0;
    int turn_nar=0;
    for(int i=60;i>10;i--)
    {
        //����·�ı仯���
        if(My_Abs(line_elements[i].width,line_elements[i+10].width)>20)
        {
            turn_nar1=1;

        }
        if(line_elements[i].width>line_elements[i+1].width)
        {
            turn_nar=1;
        }
    }
   if(turn_nar1&&turn_nar)
   {
        return true;
   }
  return false;
}



void second_stage()//�ڶ��׶��жϺ���
{
  //  int result=Bisa_variance(Road_Right);
     if(lose_left_line()&&have_black_area()&&yuanhuan_flag1)//�����˲����к�ɫ����
     {
         yuanhuan_flag2=1;
         yuanhuan_flag1=0;
     }
}
struct point
{
        int x;
        int y;
};
struct point p1,p2,p3,p4;//�����ҹյ�
void find_inflection_point()//��Բ��ʱ��Ĺյ� ��������߽����Сֵ
{
    p1.x=0;
    p1.y=line_elements[0].left;
    for(int i=0;i<40;i++)
    {
      if(line_elements[i].left<p1.y&&line_elements[i].left!=0)
      {
          p1.x=i;
          p1.y=line_elements[i].left;//�ҵ���͵ĵ�

      }
    }
}
void find_point()
{
   p2.x=0;
   p2.y=line_elements[0].right;
   for(int i=0;i<60;i++)
   {
     if(line_elements[i].right>p2.y&&line_elements[i].right!=94)
     {
         p2.x=i;
         p2.y=line_elements[i].right;//�ҵ���ĵ�
     }
   }

}
void buxian(struct point po1,struct point po2)//���� �뻷
{
  int x1,x2,y1,y2;//����������
  x1=po1.x;
  y1=po1.y;
  y2=po2.y;
  x2=po2.x;
  int k=(y1-y2)/(x1-x2);

  for(int i=x1;i<x2;i++)
  {
      line_elements[i].right=k*i;//�߽纯��Ҳ����
      Bin_Image[i][k*i]=4;
   }
}
bool is_align()//�б߽���ֱ��
{
    int flag=0;
    for(int i=0;i<60;i++)
    {
        if(My_Abs(line_elements[i].right,line_elements[i+1].right)<3)
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
void third_stage2()
{
    if(lose_left_line()&&yuanhuan_flag2&&no_black()&&!have_black_area()&&is_align())//�����Ҿ����˵ڶ��׶� ���·�û�к�ɫ������
    {
       yuanhuan_flag3=1;
       yuanhuan_flag2=0;
       if(!ruhuan_flag)
       {buxian(p1,p2);}//���в��ߴ�Ƕ�
    }

 }
//�ұ��ǻ��� ֱ��
void success_in()//�ɹ��뻷�ı�־  δ����
{
   if(yuanhuan_flag3&&lose_left_line()&&!is_align())
   {
       ruhuan_flag=1;
       yuanhuan_flag3=0;
   }
}
void chuhuan()
{

     p3.x=0;
     p3.y=line_elements[0].left;
     for(int i=0;i<60;i++)
     {
        if(line_elements[i].left<p3.y)
         {
             p3.x=i;
             p3.y=line_elements[i].left;//�ҵ���͵ĵ�
         }
       }
     p4.x=0;
     p4.y=line_elements[0].right;
     for(int i=0;i<60;i++)
     {
        if(line_elements[i].right>p4.y)
         {
             p4.x=i;
             p4.y=line_elements[i].right;//�ҵ���͵ĵ�
         }
       }

}
void forth_stage()//���г�Բ��
{
    //int result=Bisa_variance(Road_Right);//�ұ߽߱��ƫ��ֵ��
    if(yuanhuan_flag3&&ruhuan_flag)
    {
        yuanhuan_flag4=1;
        yuanhuan_flag3=0;
        buxian(p3,p4);
    }
}
void all_clear()//���б�־����
{
    yuanhuan_flag1=0;
    yuanhuan_flag2=0;
    yuanhuan_flag3=0;
    yuanhuan_flag4=0;
    yuanhuan_flag5=0;
    ruhuan_flag=0;
}
void fifth_stage()
{
   if(lose_left_line()&&no_black()&&yuanhuan_flag4)//����
    {
        //�ص��˳�ʼλ�ã�����ǰ��
       all_clear();//ȫ������
    }
}
void roundabout()
{
    //���õ�һ�׶��жϺ�
    first_stage();
    second_stage();//�ڶ��׶�
//    //third_stage1();
    third_stage2();//�����׶� û�к�ɫ����
    char tstr[10];
    sprintf(tstr,"flag1: %d",yuanhuan_flag1);
    TFTSPI_P8X16Str(1, 4, tstr, u16RED, u16GREEN);
    sprintf(tstr,"flag2: %d",yuanhuan_flag2);
    TFTSPI_P8X16Str(1, 5, tstr, u16RED, u16GREEN);
    sprintf(tstr,"flag3: %d",yuanhuan_flag3);
    TFTSPI_P8X16Str(1, 6, tstr, u16RED, u16GREEN);
//    all_clear();
//    success_in();
//    forth_stage();
//    fifth_stage();

}


/********************************                  ����                            *************************************************/

/*
����������ж�����ҪѰ�ҽ�������ʱ�������յ��Լ�������ײ����Ǹ���

1.�ҵ���������ʱ�������յ㣺��߽������ȱ���ٱ�С��
ͬ�ұ߽������ȱ�С�����ҵ���   ����С�仯���Ǹ��㡣
���յ�������ж��������߽�Ķ�����Ŀ����С��һ����ֵ����Ϊ״̬һ��

 2.����ײ��ĵ㣺�ҵ�������������յ�󣬼������յ���������������ӳ���2����¼����
 �����Թ̶�����ͷ���м�����������Һڰ׵������(Ҳ�����Ըղ����յ�������������2)
 ���ҵ��ײ��ĵ�󣬼�¼������������յ�1/2�������������������֮��С��һ����ֵ���ж����������

*/

//extern int Bin_image[60][94];

//����յ�
int Left_Point_Row = 0;//��յ���
int Left_Point_Col = 0;//��յ���
int Left_Point_Flag = 0;//�Ƿ��ҵ���յ��־λ
//extern int leftline[60];//��߽�
void Find_Left_Point()
{
    int i;

    int  Find_Start_Line = 0;
    int  Find_End_Line = 94;

    for (i = Find_End_Line-1; i >= Find_Start_Line; i--)
    {
        if (line_elements[i].left != 0 && line_elements[i-1].left != 0 && line_elements[i+1].left != 0
            && line_elements[i-2].left != 0                       //����4�в����ߣ�������4�ж�С��70 178�ĳ�70
            && line_elements[i].left < 178 && line_elements[i-1].left < 178 && line_elements[i+1].left < 178
            && line_elements[i-2].left < 178)
        {
            if ((line_elements[i].left - line_elements[i-1].left >= 0) && (line_elements[i].left - line_elements[i-2].left >= 0)
                && (line_elements[i].left - line_elements[i-3].left >= 2)
                && (line_elements[i].left - line_elements[i+1].left >= 0)
                && (line_elements[i].left - line_elements[i+2].left >= 0)
                && (line_elements[i].left - line_elements[i+3].left >= 0)) //�ҵ��յ�
            {
                Left_Point_Row = i;           //��¼�ùյ����
                Left_Point_Col = line_elements[i].left; //��¼�ùյ����
                Left_Point_Flag = 1;          //����ҵ���յ�
                break;                              //�ҵ��˳�
            }
        }
    }
}

//���ҹյ�
int Right_Point_Row = 0;//�ҹյ���
int Right_Point_Col = 0;//�ҹյ���
int Right_Point_Flag = 0;//�Ƿ��ҵ��ҹյ��־λ
//extern int rightline[60];//���ұ߽�
void Find_Right_Point()
{
    int i;
    int cutCOL = 94;
    int  Find_Start_Line = 0;
    int  Find_End_Line = 94;


    for (i = Find_End_Line-1; i >= Find_Start_Line; i--)
    {
        //����ɨ��̫Զ�����������

        if ((line_elements[i].right != (cutCOL - 1) && line_elements[i-1].right != (cutCOL - 1)
            && line_elements[i-2].right != (cutCOL - 1)
            && line_elements[i-3].right != (cutCOL - 1))
            && (line_elements[i].right > 10 && line_elements[i-1].right > 10
                && line_elements[i+1].right > 10 && line_elements[i-2].right > 10))
            //�������в�����,��δΪ�˷�ֹ���䣬��4�б������10
        {
            if ((line_elements[i].right - line_elements[i+1].right <= 0)
                && (line_elements[i].right - line_elements[i+2].right) <= 0
                && (line_elements[i].right - line_elements[i+3].right) <= 0
                && (line_elements[i].right - line_elements[i-1].right <= 0)
                && (line_elements[i].right - line_elements[i-2].right <= 0)
                && (line_elements[i].right - line_elements[i-3].right <= -2))
            {
                Right_Point_Row = i;           //��¼�ùյ����
                Right_Point_Col = line_elements[i].right; //��¼�ùյ����
                Right_Point_Flag = 1;          //����ҵ��ҹյ�
                break;                              //�ҵ��˳�
            }
        }
    }

}


/*
�������ƣ�Sancha_judge
��������: �ж�����
*/
int point_flag;
int distance;
int Sancha_Num = 0; //���ؽ�������������˵�żȻ���
int Sancha_Start_Row = 0;
int Sancha_Start_Col = 0;
int Sancha_Button_Row = 0;//�����м�ײ��Ǹ��յ�
int Sancha_Button_Col = 0;
int Sancha_Button_Flag = 0;
int Sancha_In_Flag = 0;//�������־


int Sancha_Go_Right = 0;
int Sancha_Go_Left = 0;


void Sancha_judge()
{
    int i;
    //  San.Sancha_In_Flag=0;

    Find_Left_Point();
    Find_Right_Point();
    char tstr[10];
    sprintf(tstr,"Right: %d,%d,%d",Right_Point_Row,Right_Point_Col,Right_Point_Flag);
    TFTSPI_P8X16Str(1, 4, tstr, u16RED, u16GREEN);
    sprintf(tstr,"Left: %d,%d,%d",Left_Point_Row,Left_Point_Col,Left_Point_Flag);
    TFTSPI_P8X16Str(1, 5, tstr, u16RED, u16GREEN);
    if (Right_Point_Flag && Left_Point_Flag /* && sum_lost_line < 40 */) //�����·��յ����
    {
        point_flag = 1;
        Sancha_Start_Row = ((Right_Point_Row + Left_Point_Row) / 2);
        Sancha_Start_Col = ((Right_Point_Col + Left_Point_Col) / 2);
        Sancha_Button_Col = Sancha_Start_Col;
    }
    if (point_flag == 1 /*&& sum_lost_line > 70*/)
    {
      //  Sancha_In_Flag = 1;

        for (i = Sancha_Start_Row; i > 0; i--)//��mid
        {
            if ((Bin_image[i][Sancha_Start_Col] == 1)&& (Bin_image[i - 1][Sancha_Start_Col] == 0))
            {
                Sancha_Button_Row = i;
                distance = Sancha_Start_Row - Sancha_Button_Row;
                Sancha_In_Flag = 1;

                if (distance > 0 && distance <= 40 && Sancha_Start_Row > 55)
                {
                    Sancha_Button_Flag = 1;
                    Sancha_In_Flag = 1;
                    //special_LOCK = 1;
                    Sancha_Num++;
                    break;
                }
            }
        }
        if (Sancha_In_Flag == 1)
        {
          // sprintf(tstr,"Mid��%d,%d",Sancha_Button_Row,Sancha_Button_Col);
        //   TFTSPI_P8X16Str(1, 5, tstr, u16RED,u16GREEN);
           float k = 0;
           Sancha_Go_Right = 1;
           if (Sancha_Go_Right)//��������·  ���Ϲյ�����յ�
           {
              k = (Sancha_Button_Col - Left_Point_Col) / (Sancha_Button_Row - Left_Point_Row);
              for (int i = Sancha_Start_Row; i >= Sancha_Button_Row; i++)
              {
                  Bin_Image[i][line_elements[i].left] = 0;
                  line_elements[i].left = i * k + Sancha_Start_Row;
                  Bin_Image[i][line_elements[i].left] = 2;
               }

            }
                           //��֮ǰ���е�������ص�flag��Ϊ0
             Sancha_In_Flag = 0;
             Bin_Image[Left_Point_Row][Left_Point_Col] = 2;
             Bin_Image[Right_Point_Row][Right_Point_Col] = 2;


         }
    }
}


/*
int Sancha_Go_Right = 0;
int Sancha_Go_Left = 0;
void Sancha_buxian()
{
    float k = 0;
    Sancha_Go_Right = 1;
    if (Sancha_Go_Right)//��������·  ���Ϲյ�����յ�
    {
        k = (Sancha_Button_Col - Left_Point_Col) / (Sancha_Button_Row - Left_Point_Row);
        for (int i = Sancha_Start_Row; i >= Sancha_Button_Row; i++)
        {
            Bin_Image[i][line_elements[i].left] = 0;
            line_elements[i].left = i * k + Sancha_Start_Row;
            Bin_Image[i][line_elements[i].left] = 2;
        }

    }

    else
    {

    }

}*/

/* xxxxxxxxzzzzzzzz
int Sancha_Find_Flag = 0;
void Road_Type_Judge()
{
    //����


}
*/
