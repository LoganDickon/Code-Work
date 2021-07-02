#include "VentDisplay.h"
#include <SPI.h>


	void setuv(unsigned char *p, int idx, unsigned short v)
	{
		unsigned short *sp = (unsigned short *)(p + idx);
		unsigned short sv = (v << 8) | (v >> 8);
		*sp = sv;		
//		Serial1.printf("setuv p %p idx %d %p = %hd %hd\n", p, idx, sp, v, *sp);
	}
	
	void setsv(unsigned char *p, int idx, short v)
	{
		unsigned short *sp = (unsigned short *)(p + idx);
		unsigned short sv = (v << 8) | (v >> 8);
		*sp = sv;
//		Serial1.printf("setsv p %p idx %d %p = %hd %hd\n", p, idx, sp, v, *sp);
	}

	int txt_r, txt_g, txt_b;


	VentDisplay::VentDisplay(int _width, int _height)
	{
		width = _width;
		height = _height;
		Serial1.begin(DISPLAY_BAUD, SERIAL_8N1);
		Serial1.attachCts(DISPLAY_CTS);
		Serial1.attachRts(DISPLAY_RTS);
/*
  unsigned char cmd[10];
  cmd[0] = 0xFE;
  cmd[1] = 0x39;
  cmd[2] = 0;
  cmd[3] = 0;
  cmd[4] = 37;
  cmd[5] = 128;
  Serial1.write(cmd, 6);
  Serial1.end();
  Serial1.attachCts(29);
  Serial1.attachRts(30);
  Serial1.begin(9600, SERIAL_8N1);
*/
	}

	void VentDisplay::background(int _r, int _g, int _b)
	{
		unsigned char cmd[5];
	
		cmd[0] = 0xFE;
		cmd[1] = 0x56;
		cmd[2] = _r;
		cmd[3] = _g;
		cmd[4] = _b;
		Serial1.write(cmd, 5);
		//Serial1.flush();
/*
		rgb(_r, _g, _b);
		fillrect(0,0,width,height);
*/		
	}


	void VentDisplay::rgb(int _r, int _g, int _b)
	{
		unsigned char cmd[5];
	
		cmd[0] = 0xFE;
		cmd[1] = 0x63;
		cmd[2] = _r;
		cmd[3] = _g;
		cmd[4] = _b;
		Serial1.write(cmd, 5);		
		//Serial1.flush();
	}

	void VentDisplay::textrgb(int _r, int _g, int _b)
	{
		unsigned char cmd[5];
	
		cmd[0] = 0xFE;
		cmd[1] = 0x2E;
		cmd[2] = txt_r = _r;
		cmd[3] = txt_g = _g;
		cmd[4] = txt_b = _b;
		Serial1.write(cmd, 5);		
	}

	void VentDisplay::textsize(int size)
	{
		unsigned char cmd[3];
	
		cmd[0] = 0xFE;
		cmd[1] = 0x33;
		cmd[2] = size;
		Serial1.write(cmd, 3);
	}

	void VentDisplay::slideleft(short x, short y, short wid, short hgt, short amt)
	{
		unsigned char cmd[14];

		cmd[0] = 0xFE;
		cmd[1] = 0x59;
		
		setuv(cmd, 2, x);
		setuv(cmd, 4, y);
		setuv(cmd, 6, wid);
		setuv(cmd, 8, hgt);
		setuv(cmd, 10, amt);
		setuv(cmd, 12, 0);
		
		Serial1.write(cmd, 14);
	}

	void VentDisplay::buzz(short freq, short durr)
	{
		unsigned char cmd[6];

		cmd[0] = 0xFE;
		cmd[1] = 0xBB;
		setuv(cmd, 2, freq);
		setuv(cmd, 4, durr);
		
		Serial1.write(cmd, 6);
	}

	void VentDisplay::log(char *msg)
	{
/*
		char cmd[256];
		cmd[0] = 0xFE;
		cmd[1] = 0x11;
		strcpy(&cmd[2], msg);
		Serial1.write(cmd, 3 + strlen(msg));		
		Serial1.println(msg);
*/
	// 
	}

	void VentDisplay::loadFont(unsigned char id, char *filePath)
	{
		unsigned char cmd[256];
		cmd[0] = 0xFE;
		cmd[1] = 0x28;
		cmd[2] = id;
		int len = strlen(filePath);
		strcpy((char *)&cmd[3], filePath);
		Serial1.write(cmd, len + 4);
		//Serial1.flush();
	}

	void VentDisplay::setTextFont(unsigned char id)
	{
		unsigned char cmd[3];
		cmd[0] = 0xFE;
		cmd[1] = 0x31;
		cmd[2] = id;
		Serial1.write(cmd, 3);				
		//Serial1.flush();
	}


	void VentDisplay::line(short x1, short y1, short x2, short y2)
	{
		unsigned char cmd[10];

		cmd[0] = 0xFE;
		cmd[1] = 0x6C;
		
		setuv(cmd, 2,x1);
		setuv(cmd, 4,y1);
		setuv(cmd, 6,x2);
		setuv(cmd, 8,y2);
		Serial1.write(cmd, 10);
		//Serial1.flush();
	}
	
	void VentDisplay::fillrect(short x1, short y1, short wid, short hgt)
	{
		unsigned char cmd[10];	

		cmd[0] = 0xFE;
		cmd[1] = 0x78;
		
		setuv(cmd, 2, x1);
		setuv(cmd, 4, y1);
		setuv(cmd, 6, wid);
		setuv(cmd, 8, hgt);
		Serial1.write(cmd, 10);
		//Serial1.flush();
	}

	void VentDisplay::setTextSize(int _size)
	{
		unsigned char cmd[10];	

		cmd[0] = 0xFE;
		cmd[1] = 0x33;
		cmd[2] = (unsigned char)_size;
		Serial1.write(cmd, 3);		
		//Serial1.flush();
	}


	void VentDisplay::text(short x, short y, const char *msg)
	{
		unsigned char loc[6];
		unsigned char cmd[256];
		loc[0] = 0xFE;
		loc[1] = 0x79;
		setuv(loc, 2, x);
		setuv(loc, 4, y);
		Serial1.write(loc, 6);

		cmd[0] = 0xFE;
		cmd[1] = 0x25;
		int len = strlen(msg);
		strcpy((char *)&cmd[2], msg);
		Serial1.write(cmd, len + 3);
		//Serial1.flush();
	}


	void VentDisplay::tone(int pin, int vol)
	{
		
	}

	int VentDisplay::readLine(unsigned char *str)
	{
		char c;
		int len = 0;
		for (;;)
		{			
			c = Serial1.read();
			if (c >= 0)
			{
				str[len++] = c;
				if (c == '\n')
					return 1;	
			}
			delay(10);
		}
//		log("Display::readLine failed");
//		return 0;		
	}

	void VentDisplay::getSize(short int *wid, short int *hgt)
	{
		width = *wid = (unsigned short)800;
		height = *hgt = (unsigned short)480;
	}
