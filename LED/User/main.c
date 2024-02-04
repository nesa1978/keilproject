/* 头文件引用区 */
#include <STC15F2K60S2.H>
#include "seg.h"
#include "key.h"
#include "led.h"
#include "sys_init.h"

/* 变量定义区 */
unsigned char key_val, key_up, key_down, key_old;
unsigned char key_slow_down;
unsigned char seg_buf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; // 10-熄灭 11-R 12-S 13显示-,14-A,15-D
unsigned char seg_point[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char seg_pos;
unsigned int seg_slow_down;
bit led_flow;
unsigned char led_buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char seg_mode; // 显示模式
unsigned char led_mode; // 流水灯模式
unsigned int time_count;
unsigned int time_flow[4] = {400, 400, 400, 400};
unsigned int time_set[4] = {400, 400, 400, 400};
unsigned char led_scan;

/* 按键处理函数 */
void key_proc()
{
	if (key_slow_down)
		return;
	key_slow_down = 1;
	key_val = key_read();
	key_down = key_val & (key_val ^ key_old);
	key_up = ~key_val & (key_val ^ key_old);
	key_old = key_val;
	switch (key_down)
	{
	case 4:
		led_mode++;
		if (led_mode == 4)
			led_mode = 0;
		break;
	case 6:
		seg_mode = 1;

		break;
	case 7:
		led_flow ^= 1;
		break;
	}
}

/* 数码管显示函数 */
void seg_proc()
{
	if (seg_slow_down)
		return;
	seg_slow_down = 1;
	switch (seg_mode)
	{
	case 0:
		if (led_flow)
			seg_buf[2] = 11;
		else
			seg_buf[2] = 12;
		seg_buf[3] = led_mode + 1;
		seg_buf[4] = time_flow[led_mode] / 1000 % 10 ? (time_flow[led_mode] / 1000 % 10) : 10;
		seg_buf[5] = time_flow[led_mode] / 100 % 10;
		seg_buf[6] = time_flow[led_mode] / 10 % 10;
		seg_buf[7] = time_flow[led_mode] % 10;
		break;
	case 1:
		break;
	case 2:
		break;
	}
}
/* 其他显示函数 */
void led_proc()
{
	unsigned char i;
	switch (led_mode)
	{
	case 0: // 模式1
		for (i = 0; i < 8; i++)
		{
			if (led_scan == i)
				led_buf[led_scan] = 1;
			else
				led_buf[i] = 0;
		}
		if (led_buf[7] == 1 && led_scan == 0)
			led_mode = 1;
		break;
	case 1: // 模式2

		for (i = 0; i < 8; i++)
		{
			if ((7 - led_scan) == i)
				led_buf[7 - led_scan] = 1;
			else
				led_buf[i] = 0;
		}
		// if (led_buf[0] == 1)
		// 	led_mode = 2;
		break;
	case 2: // 模式3
		for (i = 0; i < 8; i++)
		{
			if (led_scan % 4 == i || 7 - (led_scan % 4) == i)
			{
				led_buf[led_scan % 4] = 1;
				led_buf[7 - led_scan % 4] = 1;
			}
			else
				led_buf[i] = 0;
		}
		if (led_buf[3] == 1 && led_buf[4] == 1)
			led_mode = 3;
		break;
	case 3: // 模式4
		for (i = 0; i < 8; i++)
		{
			if (3 - (led_scan % 4) == i || (led_scan % 4) + 4 == i)
			{
				led_buf[3 - led_scan % 4] = 1;
				led_buf[led_scan % 4 + 4] = 1;
			}
			else
				led_buf[i] = 0;
		}
		if (led_buf[0] == 1 && led_buf[7] == 1)
			led_mode = 0;
		break;
	}
}

/* 定时器0初始化函数 */

void Timer0_Init(void) // 1毫秒@12.000MHz
{
	AUXR &= 0x7F; // 定时器时钟12T模式
	TMOD &= 0xF0; // 设置定时器模式
	TL0 = 0x18;	  // 设置定时初始值
	TH0 = 0xFC;	  // 设置定时初始值
	TF0 = 0;	  // 清除TF0标志
	TR0 = 1;	  // 定时器0开始计时
	ET0 = 1;	  // 使能定时器0中断
	EA = 1;
}

/* 定时器0服务函数 */
void Timer0_Isr(void) interrupt 1
{
	if (++key_slow_down == 10)
		key_slow_down = 0;
	if (++seg_slow_down == 500)
		seg_slow_down = 0;
	if (++seg_pos == 8)
		seg_pos = 0;
	seg_disp(seg_pos, seg_buf[seg_pos], seg_point[seg_pos]);
	led_disp(seg_pos, led_buf[seg_pos]);
	if (++time_count == time_flow[led_mode])
	{
		time_count = 0;
		led_scan++;
		if (led_scan == 8)
		{
			led_scan = 0;
		}
	}
}

/* 主函数 */
void main()
{
	sys_init();
	Timer0_Init();
	while (1)
	{
		key_proc();
		seg_proc();
		led_proc();
	}
}
