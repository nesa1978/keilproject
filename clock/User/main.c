/*头文件引入*/
#include <STC15F2K60S2.H>
#include <key.h>
#include <seg.h>
#include <sys_init.h>

/* 变量定义 */
unsigned char key_val,key_down,key_up,key_old; //按键扫描专用变量
unsigned char key_slow_down;  //按键减速变量
unsigned char seg_buf[8] = {10,10,10,10,10,10,10,10}; //数码管8位数据
unsigned char seg_point[8] = {0,0,0,1,0,1,0,1}; //数码管小数点变量
unsigned int seg_slow_down;  //数码管减速变量
unsigned char seg_pos; //数码管扫描位置变量
unsigned char seg_mode; //显示模式变量  0-显示界面 1-时钟设置界面 2-闹钟设置界面
unsigned char seg_time[3] = {23,59,55}; //时钟显示变量
unsigned int time_1000ms; //1s定时时间
unsigned int time_500ms; //0.5s定时时间
bit seg_flag; //闪烁标志位
unsigned char set_time[3]; //时钟设置变量
unsigned char set_index; //设置索引
unsigned char alarm[3] = {0,0,0}; //闹钟值
unsigned char alarm_set[3]; //闹钟设置变量
unsigned char alarm_enable = 1; //闹钟开启或关闭 1-开启 0关闭
bit alarm_flag;  //闹钟时间到达标志


/* 按键处理程序 */
void key_proc()
{
	if(key_slow_down)return;
	key_slow_down = 1;  //按键减速
	key_val = key_read(); //读取按键
	key_down = key_val & (key_val ^ key_old);  //捕捉下降沿
	key_up = ~key_val & (key_val ^ key_old); //捕捉上升沿
	key_old = key_val;
	
	if(key_down !=0)  //按下任意按键，关闭闹钟，熄灭led
	{
		alarm_flag = 0;
		P0 = 0xff;
		P2 = P2 & 0x1f | 0x80;
		P2 &= 0x1f;
		P0 = 0x00;
		P2 = P2 & 0x1f | 0xa0;
		P2 &= 0x1f;
	}
	switch(key_down)
	{
		case 4: //切换时钟设置
			seg_mode = 1;
			set_index = 0;
			set_time[0] = seg_time[0];
			set_time[1] = seg_time[1];
			set_time[2] = seg_time[2];
		break;
		case 5:  //切换闹钟模式
			seg_mode = 2;
			set_index = 0;
			alarm_set[0] = alarm[0]; 
			alarm_set[1] = alarm[1];
			alarm_set[2] = alarm[2]; 
		break;
		case 6: //设置选中切换
			if(seg_mode != 0)
			{
				set_index++;
				if(set_index == 3)
					set_index =0;
			}
			
		break;
		case 7: //闹钟开启或关闭
			alarm_enable ^= 1;
		break;
		case 8:
			if(seg_mode == 1)
			{
				set_time[set_index]++; //设置时间模式下，增加参数
				if(set_time[set_index] == ((set_index == 0) ? 24 : 60))
					set_time[set_index] = 0;
			}
			if(seg_mode == 2)
			{
				alarm_set[set_index]++; //设置闹钟模式下，增加参数
				if(alarm_set[set_index] == ((set_index == 0) ? 24 : 60))
					alarm_set[set_index] = 0;
			}
		break;
		case 9:
			if(seg_mode == 1) //设置时间模式下，减少参数
			{
				set_time[set_index]--;
				if(set_time[set_index] == 255)
					set_time[set_index] = (set_index == 0) ? 23 : 59;	
			}
			if(seg_mode == 2) //设置闹钟模式下，减少参数
			{
				alarm_set[set_index]--;
				if(alarm_set[set_index] == 255)
					alarm_set[set_index] = (set_index == 0) ? 23 : 59;	
			}

		break;
		case 10: //保存
			if(seg_mode == 1)  //设置时间模式下保存
			{			
				seg_time[0] = set_time[0];
				seg_time[1] = set_time[1];
				seg_time[2] = set_time[2];
			}
			if(seg_mode == 2)  //设置闹钟模式下保存
			{
				alarm[0] = alarm_set[0]; 
				alarm[1] = alarm_set[1];
				alarm[2] = alarm_set[2]; 
			}
			seg_mode = 0;
		break;
		case 11: //退出
			seg_mode = 0;
		break;

	}
}

/* 显示处理 */
void seg_proc()
{
	unsigned char i; //循环专用变量
	if(seg_slow_down)return;
	seg_slow_down = 1;  //数码管减速
	seg_buf[0] = seg_mode + 1;  //第一位显示模式
	seg_buf[1] = alarm_enable; //第二位显示闹钟开启或关闭，1-开启 ，0-关闭
	if(seg_mode == 0) //时钟显示界面
	{
		for(i=0;i<3;i++)
		{
			seg_buf[2+2*i] = seg_time[i] / 10 % 10;
			seg_buf[3+2*i] = seg_time[i] % 10;
		}
	}
	if(seg_mode == 1) //时钟设置界面
	{
		for(i=0;i<3;i++)
		{
			seg_buf[2+2*i] = set_time[i] / 10 % 10;
			seg_buf[3+2*i] = set_time[i] % 10;
		}
		seg_buf[2+2*set_index] = seg_flag ? set_time[set_index] / 10 % 10 : 10;
		seg_buf[3+2*set_index] = seg_flag ? set_time[set_index] % 10 : 10;
	}
	if(seg_mode == 2) //闹钟设置界面
	{
		for(i=0;i<3;i++)
		{
			seg_buf[2+2*i] = alarm_set[i] / 10 % 10;
			seg_buf[3+2*i] = alarm_set[i] % 10;
		}
		seg_buf[2+2*set_index] = seg_flag ? alarm_set[set_index] / 10 % 10 : 10;
		seg_buf[3+2*set_index] = seg_flag ? alarm_set[set_index] % 10 : 10;
	}
		
}

/* 其他显示处理 */
void led_proc()
{
	if(alarm_enable)
	{
		if(alarm[0]==seg_time[0] && alarm[1]==seg_time[1] && alarm[2]==seg_time[2])
		{
			alarm_flag = 1; //到达闹钟时间后标志位置1
		}
	}
	if(alarm_flag) //大于12点，前4个灯闪烁，小于12点后4个灯闪烁
	{
		if(alarm[0]>=12)
		{
			P0 = seg_flag? 0x0f : 0xff;
			P2 = P2 & 0x1f | 0x80;
			P2 &= 0x1f;
		}
		else
		{
			P0 = seg_flag? 0xf0 : 0xff;
			P2 = P2 & 0x1f | 0x80;
			P2 &= 0x1f;
		}
		P0 = 0x40;
		P2 = P2 & 0x1f | 0xa0;
		P2 &= 0x1f;
	}
}

/* 定时器0初始化 */
void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA = 1;                 //打开总中断
}

/* 定时器0处理程序 */
void Timer0_Isr(void) interrupt 1
{
	if(++key_slow_down==10) key_slow_down = 0; //按键减速
	if(++seg_slow_down==500) seg_slow_down = 0;  //数码管减速
	if(++seg_pos == 8) seg_pos = 0;  //数码管扫描变量
	seg_disp(seg_pos,seg_buf[seg_pos],seg_point[seg_pos]); //数码管扫描
	if(++time_500ms == 500)
	{
		time_500ms = 0;
		seg_flag ^= 1; //500ms，标志位取反
	}
	if(++time_1000ms == 1000)  //计时程序
	{
		time_1000ms = 0;
		seg_time[2]++;
		if(seg_time[2] == 60)
		{
			seg_time[2] = 0;
			seg_time[1]++;
			if(seg_time[1] == 60)
			{
				seg_time[1] = 0;
				seg_time[0]++;
				if(seg_time[0] == 24)
				{
					seg_time[0] = 0;
				}
			}
		}
	}
}

/* main*/
void main()
{
	sys_init();
	Timer0_Init();
	while(1)
	{
		key_proc();
		seg_proc();
		led_proc();
	}
}