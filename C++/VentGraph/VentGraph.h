
#ifndef VENTGRAPH_h
#define VENTGRAPH_h

#include <VentDisplay.h>
#include <SD.h>
#include <VentPins.h>
//#include <CraigPins.h>

#define YELLOW 0xFFE0

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

#define MAX_SETTINGS 16
#define HIGH_PRESSURE 0
#define LOW_PRESSURE 1
#define O2_PRESSURE 2


#define VENT_CONFIG	"config.txt"
#define HISTORY_LOG	"history.log"
#define ALARM_LOG	"alarms.log"
#define PEEP_SAMPLE_COUNT 3

enum ConfigMode { CFG_IDLE, CFG_ACTIVE, CFG_CONFIRM, CFG_CANCEL, CFG_INVALID_ID };
enum AirMode { AIR_IDLE, AIR_DELIVER, AIR_REFILL, AIR_PEEK, AIR_END };

class VentSetting
{
	private:
	public:
		int btn, pin1, pin2, min, max, inc, value, stagedValue, zeroMin;
		int lblWid, lblHgt;
		bool wrap;
		void (*display)(VentSetting *);
		void (*hold)(VentSetting *);
		short dispX, dispY;
		int lblPad;
		char *label;
		ConfigMode mode;
		VentSetting(const char *_label, int _btn, int _pin1, int _pin2, int _min, int _max, int _inc, int _val, bool _wrap, void (*_display)(VentSetting *), short _dispX, short _dispY);
		ConfigMode cycle();
		void show(char *txt);
		void activate();
		void adjust(int amt);
		void stage();
		void abort();
		void confirm();
		void check();
		static void service();
};

class VentAlarm
{
	public:
		char text[32];
		char prev[32];
		int level;	
		int freq;
		int x, y, wid, hgt;
		long when;
		VentAlarm();
};

#define BreathAlarm 0
#define SupplyAlarm 1

class VentGraph
{
    private:
        int xMin, xMax, yMin;
        int yMax;
        int highLimit;
        int lowLimit;
        int yPos;
        int prevX;
        int prevY;
        int prevBase;
		long alarmStart;
		float airway, supply;

		short scrHgt;
		short scrWid;
		const int yMargin = 50;
		int minO2psi = 30;
		int maxO2psi = 70;
		int o2Status;
		long zeroTime;
//		VentSetting *settings[MAX_SETTINGS];
		bool settingsMode;
		int prev_trend;
		float prev_breath_value;
		float avg_peep;
		int prev_peep;
		VentAlarm alarmSet[2];
		char err;
		char errmsg[64];
		AirMode air_mode;
    public:
		const char *pathToDump;
		void (*sample)(float *, float *);
		void (*changeSettings)(int, int, int, int, float, int);
		void (*changeMode)(int);
		void changeInSettings();
		void modeChange();
		void startMotor();
		void stopMotor();
		VentDisplay display = VentDisplay(800, 600);
		VentSetting *hiPressure, *loPressure, *volume, *bpm, *ratio;
		VentSetting *activeSetting;		
        int xPos;
        int xInc;
		
		VentGraph(void (*_sample)(float *, float *), void (*_changeSettings)(int, int, int, int, float, int));

		void setup(void);
		void service();
		void drawLine(int x1, int y1, int x2, int y2);
		void setLimits(int _lo, int _hi);

		void showO2();

		
		void initGraph();
		void clearGraph();

		void showError();
		void postError(char *msg);
		void clearError();
		void bpRange(int *min, int *max, int *samples);
		int getPeep();
		void getBounce(int *lo, int *hi, int *bumps);


		float calcAvgPEEP(float curr_peep);
		int computeRange(float vac, int *adjustedVolume);
		void showPEEP();
		void postAlarm(int type, int level, int freq, char *txt);
		void showAlarms();
		int volToPos(int vol);
		
		void setAirMode(AirMode mode);
	
		void clearAhead(int adj);
		void plotBreath(int value);

		void log(const char* format, ...);
//		void log(char* msg);

		void getSize(short *wid, short *hgt);

		void drawTimeScale();

		void saveSettings();
		void logAlarm(const char *text);
		void loadSettings();
		void checkBootMenu();
		int split(char *str, char delim, char *set[]);
		int readString(File fd, char *buf, char delim);
		int readInt(File fd, char delim);
		void dumpFile(const char *path);
		void loop();
		float airwayPressure();
		float supplyPressure();
};


float collectFakeAirway();


#endif

