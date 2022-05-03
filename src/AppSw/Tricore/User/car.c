/*
 * car.c
 *
 *  Created on: 2022��4��14��
 *      Author: d'y't'q'wa's
 */

#include <stdio.h>
#include <stdlib.h>
#include <LQ_TFT18.h>
#include <LQ_CAMERA.h>
extern unsigned char Bin_Image[LCDH][LCDW];

int whitePoint[60] = { 0 };//ÿһ�еİ׵����
int width[60] = { 0 };//�׵���

int L_black[60] = { 0 };//�ٸ����� L_black[i]=j  ���ǵ�i�е���߽�㣨�ڰ�����㣩�ٵ�j�� ����һ������(i,j)  0<=i<=60  0<=j<=188
int R_black[60] = { 0 };
int mid[60] = { 0 };

void FindEdge()//�ұ�Ե
{
    int i;
    int j;
    int start = 0;
    int sum = 0;//��������׵����
    for (i = 59; i >= 0; i--)
    {
        if (i == 59)
        {
            mid[i] = 72;// 144/2
            continue;
        }
        start = mid[i + 1];//start����һ�е��ص����ڵ�������
        if (start < L_black[i + 1] + 5 || start > R_black[i + 1] - 5)
        {
            start = mid[i + 1];
        }
        for (j = start; !Bin_Image[i][j] || Bin_Image[i][j - 1] || Bin_Image[i][j - 2]; j--)//��ɨ��
            L_black[i] = j;
        for (j = start; !Bin_Image[i][j] || Bin_Image[i][j + 1] || Bin_Image[i][j + 2]; j++)//��ɨ��
            R_black[i] = j;
        mid[i] = (R_black[i] + L_black[i]) >> 1;

        for (sum = 0, j = L_black[i]; j < R_black[i]; j++)
        {
            sum += Bin_Image[i][j];
        }
        whitePoint[i] = sum;//ÿ���м�׵����
        width[i] = R_black[i] - L_black[i];
    }
}
void Paint()
{
int i=0;
int j=0;
for(i=0;i<60;i++)
{
    for(j=0;j<94;j++)
        {
            TFTSPI_Draw_Dot(i,L_black[j],u16YELLOW);//Ū�ɻ�ɫ
            TFTSPI_Draw_Dot(i,R_black[j],u16YELLOW);//Ū�ɻ�ɫ
        }
}

}




