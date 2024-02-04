/* 头文件声明 */
#include <STC15F2K60S2.H> //单片机专用头文件
#include "sys_init.h" // 初始化头文件
#include "key.h" //扫描键盘头文件
#include "seg.h" //数码管头文件

/* 变量声明 */
unsigned char key_val,key_up,key_down,key_old; //按键专用变量
unsigned char key_slow_down; //按键减速变量
unsigned char seg_buf[8] = {10,10,10,10,10,10,10,10};
unsigned char seg_point[8] = {0,0,0,0,0,0,0,0}; //小数点
unsigned char seg_pos;
unsigned int seg_slow_down;

unsigned char seg_mode; //模式 0-显示模式 1-设置模式
unsigned char time = 30;
unsigned int time_1000ms; //1s倒计时
unsigned char time_set[3] = {15,30,60}; //设置模式时间数据
unsigned char time_set_index = 1;
bit time_flag; //倒计时开始标志，0暂停 1开始
unsigned int time_500ms;
bit led_flag; //led闪烁标志位


/* 键盘处理函数 */
void key_proc()
{
	if(key_slow_down)return;
	key_slow_down = 1; //键盘减速程序
	
	key_val = key_read(); 
	key_down = key_val & (key_val ^ key_old);
	key_up = ~key_val & (key_val ^ key_old);
	key_old = key_val;
	
	switch(key_down)
	{
		case 4:
			if(seg_mode == 0)
				time_flag = 1;
		break;
		case 5:
			if(seg_mode == 0)
			{
				time = time_set[time_set_index];
				time_flag = 0;
			}
		break;
		case 6:
			seg_mode++;
			if(seg_mode == 1)
				time_set_index = 0;
			if(seg_mode == 2)
				{
					seg_mode = 0;
					time = time_set[time_set_index];
					time_flag = 0;
				}
			
		break;
		case 7:
			if(seg_mode == 1)
				{
					time_set_index++;
					if(time_set_index == 3)
						time_set_index = 0;
				}
			
		break;
	}
	
}


/* 信息处理函数 */
void seg_proc()
{
	if(seg_slow_down)return;
	seg_slow_down = 1; //数码管减速程序
	seg_buf[0] = seg_mode+1;
	if(seg_mode == 0)
	{
		seg_buf[6] = time / 10 % 10;
		seg_buf[7] = time %10;
	}
	if(seg_mode == 1)
	{
		seg_buf[6] = led_flag ? time_set[time_set_index] / 10 % 10 : 10;
		seg_buf[7] = led_flag ? time_set[time_set_index] %10 : 10;
	}
}


/* 其他显示函数 */
void led_proc()
{
	if(time == 0)
	{
		P0 = led_flag?0x00:0xff;
		P2 = P2 & 0x1f | 0x80;
		P2 = P2 &0x1f;
	}
	
	
}

/* 定时器0初始化函数 */
void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA = 1;
}

/* 定时器0中断服务程序 */
void Timer0_Isr(void) interrupt 1
{
	if(++key_slow_down == 10) key_slow_down = 0;
	if(++seg_slow_down == 500) seg_slow_down = 0;
	if(++seg_pos == 8) seg_pos = 0;
	seg_disp(seg_pos,seg_buf[seg_pos],seg_point[seg_pos]);
	if(time_flag==1)
	{
		if(++time_1000ms == 1000) 
		{
			time_1000ms = 0;
			time--;
			if(time == 255)
				time = 0;
		}
	}
	if(++time_500ms == 500)
	{
		time_500ms = 0;
		led_flag ^= 1; //取反
	}
	
}

/* main */
void main()
{
	sys_init(); //系统初始化
	Timer0_Init(); //定时器0初始化
	while(1)
	{
		key_proc();
		seg_proc();
		led_proc();
	}

}
