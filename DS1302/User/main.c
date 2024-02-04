/*头文件引入*/
#include <STC15F2K60S2.H>
#include <key.h>
#include <seg.h>
#include <sys_init.h>
#include <led.h>
#include <ds1302.h>

/* 变量定义 */
unsigned char key_val, key_down, key_up, key_old;
unsigned char key_slow_down;
unsigned char seg_buf[8] = {10, 10, 10, 10, 10, 10, 10, 10};
unsigned char seg_point[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int seg_slow_down;
unsigned char seg_pos;
unsigned char ucled[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* 按键处理程序 */
void key_proc()
{
	if (key_slow_down)
		return;
	key_slow_down = 1;
	key_val = key_read();
	key_down = key_val & (key_val ^ key_old);
	key_up = ~key_val & (key_val ^ key_old);
	key_old = key_val;
	
}

/* 显示处理 */
void seg_proc()
{
	if (seg_slow_down)
		return;
	seg_slow_down = 1;
}

/* 其他显示处理 */
void led_proc()
{
	
}

/* 定时器0初始化 */
void Timer0_Init(void) // 1毫秒@12.000MHz
{
	AUXR &= 0x7F; // 定时器时钟12T模式
	TMOD &= 0xF0; // 设置定时器模式
	TL0 = 0x18;	  // 设置定时初始值
	TH0 = 0xFC;	  // 设置定时初始值
	TF0 = 0;	  // 清除TF0标志
	TR0 = 1;	  // 定时器0开始计时
	ET0 = 1;	  // 使能定时器0中断
	EA = 1;		  // 打开总中断
}
P
/* 定时器0处理程序 */
void Timer0_Isr(void) interrupt 1
{
	if (++key_slow_down == 10)
		key_slow_down = 0;
	if (++seg_slow_down == 500)
		seg_slow_down = 0;
	if (++seg_pos == 8)
		seg_pos = 0;
	seg_disp(seg_pos, seg_buf[seg_pos], seg_point[seg_pos]);
	led_disp(seg_pos, ucled[seg_pos]);

}

/* main*/
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