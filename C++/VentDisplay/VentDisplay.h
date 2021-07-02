
#ifndef VENTDISPLAY_h
#define VENTDISPLAY_h

#define DISPLAY_BAUD 115200
#define DISPLAY_CTS 18
#define DISPLAY_RTS 19

class VentDisplay
{
    private:
		int sfd; // serial port file descriptor
        short width, height;
		char waitForSerialByte();
    
    public:
        VentDisplay(int _width, int _height);

		void log(char *msg);
		void buzz(short freq, short durr);
		void background(int r, int g, int b);
		void rgb(int r, int g, int b);
		void textrgb(int r, int g, int b);
		void textsize(int size);
		void slideleft(short x, short y, short wid, short hgt, short amt);
		void line(short x1, short y1, short x2, short y2);
		void fillrect(short x1, short y1, short wid, short hgt);
		void text(short x, short y, const char *msg);
		void loadFont(unsigned char id, char *name);
		void setTextFont(unsigned char id);
		void setTextSize(int size);
		void getSize(short int *wid, short int *hgt);
		void tone(int pin, int vol);
		short getWidth();
		short getHeight();
		int readLine(unsigned char *str);

};


#endif

