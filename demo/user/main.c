#include <stc15.h>
#include <intrins.h>

code unsigned char Seg_Table[] =
	{
		0xc0, // 0
		0xf9, // 1
		0xa4, // 2
		0xb0, // 3
		0x99, // 4
		0x92, // 5
		0x82, // 6
		0xf8, // 7
		0x80, // 8
		0x90, // 9
		0x88, // A
		0x83, // b
		0xc6, // C
		0xa1, // d
		0x86, // E
		0x8e  // F
};

// 延时程序
unsigned char scan_key()
{
	unsigned char keycode = 0;
	if (P30 == 0)
		keycode = 7;
	if (P31 == 0)
		keycode = 6;
	if (P32 == 0)
		keycode = 5;
	if (P33 == 0)
		keycode = 4;
	return keycode;
}

void Delay500ms(void) //@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
	do
	{
		do
		{
			while (--k)
				;
		} while (--j);
	} while (--i);
}

void send_p0(channel, dat)
{
	P0 = dat;
	switch (channel)
	{
	case 4:
		P2 = P2 & 0x1f | 0x80;
		P2 = P2 & 0x1f;
		break;
	case 5:
		P2 = P2 & 0x1f | 0xa0;
		P2 = P2 & 0x1f;
		break;
	case 6:
		P2 = P2 & 0x1f | 0xc0;
		P2 = P2 & 0x1f;
		break;
	case 7:
		P2 = P2 & 0x1f | 0xe0;
		P2 = P2 & 0x1f;
		break;
	}
}

void main()
{
	// 初始化代码
	while (1)
	{
		send_p0(4, 0xff);
		send_p0(7, Seg_Table[scan_key()]);
		send_p0(6, 0xff);
	}
}