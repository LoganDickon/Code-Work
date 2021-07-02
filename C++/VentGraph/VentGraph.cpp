#include "VentPins.h"
#include "VentGraph.h"
#include "VentDisplay.h"
#include <SPI.h>
#include <TimeLib.h>
#include <time.h>
#include <i2c_t3.h>

VentGraph *graph = NULL;

	/*********************************************************************************************/
	/*********************************************************************************************/
	/************************************** VentSetting ******************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/
	const float o2Scale = .735F;
	const float o2Zero = -6.5F;
	float pZero = .1F;
	float pScale = 102.0;

	

	const int hiTextY = 5;
	const int alarmTextY = 399;
	const int hiLimitY = 420;
	const int loLimitY = 90;
	const int loTextY = 394;
	const int bottomTextY = 435;
	const int leftTextX = 10;
	const int midTextX = 290;
	const int rightTextX = 480;
	const int clrwid = 30;
	


	long silenceUntil = 0;
	float o2Max = 0.0F;
	float o2Level = 50.0F;
	int bpMin;
	int bpMax;
	long loCycle, hiCycle;
	int rfMin, rfBounce, rfDir, rfBumps;
	
	
	int bootflag = 1;
	int motorFlag = 1;
	
	int prevMaskVal[64] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int prevPinVal[64] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int chkval[64] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	VentSetting *encoderSet[10];
	int encoderCount = 0;
	float pressureDisplayScaleFactor = 5;
	unsigned char StdVentFont = 1;
	unsigned char StdFontSize = 12;
	unsigned char BigFontSize = 12;
	unsigned char ErrFontSize = 14;
	IntervalTimer controlTimer; 
	long beginTouch = 0;	// ms time when the last press or turn was done
	int prevo2psi = 0;		// previous o2 supply pressure 
	int waitForHold = 0;	// btn that was pressed and indicates that we a wating for a long hold condition
	int btnWasHeld = 0;		// the btn/pin that was held which will cancel the normal setting activation
	VentSetting *wasHeld = NULL; // reference to the setting/knob that was held
	
	int requestClockSet = 0;
	int performClockSet = 0;
	int modClock = 0;
	int _year_val, _month_val, _day_val, _hour_val, _min_val;
	int _yrv = 0, _mov = 0, _dav = 0, _hrv = 0, _mnv = 0;
	int didInit = 0;
	int didAlarm = 0;
	int pinc, pval, aval;
	int o2PSI = 78;
	void (*menuFunction)(int, int);
	char limitAlarm[64];

	int pcc = 0;

	int first_peep = 1;
	float peep_val[PEEP_SAMPLE_COUNT];
	int peep_idx = 0;
	
#define msAlarmPostLag		2000	
	
#define UPPER_LABEL 		0
#define O2LO_LABEL 			1
#define O2HI_LABEL 			2
#define LOWER_LABEL 		3
#define VOLUME_LABEL 		4
#define BPM_LABEL			5
#define RATIO_LABEL			6
#define CONFIRM_LABEL		7
#define CANCEL_LABEL		8
#define EXIT_LABEL			9
#define SILENCE_LABEL		10	
#define SET_TIME_LABEL		11
#define LIST_ALARMS_LABEL	12
#define LIST_CONFIG_LABEL	13
#define LANGUAGE_LABEL		14
#define YEAR_LABEL			15
#define MONTH_LABEL			16
#define DAY_LABEL			17
#define HOUR_LABEL			18
#define MINUTE_LABEL		19
#define USE_LABEL			20
#define APPLY_LABEL			21
#define ABORT_LABEL			22
#define PEEP_LABEL			23
#define START_LABEL			24
#define STOP_LABEL			25
#define OXYGEN_LABEL 		26

/*
*	constants for mapping ml volume range to PWM/Position range
*/
#define MIN_VOL 100
//#define MAX_VOL 800
#define MAX_VOL 980
#define MIN_POS 39
//#define MAX_POS 229
#define MAX_POS 254


#define ERROR_HIDDEN 0
#define ERROR_VISIBLE 1
#define ERROR_POSTED 2

	const char *__confirm = "Confirm";
	const char *__cancel = "Cancel";
	
	const char *lbls[] = 
	{
		"Upper Alarm %2d cmH2O",
		"Low O2 psi %d",
		"High O2 psi %d",
		"Lower Alarm %2d cmH2O",
		"%2d mL",
		"%2d BPM",
		"Ratio 1:%1.1f",
		"CONFIRM use %d",
		"CANCEL use %d",
		"Exit",
		"Silence",
		"Set Time",
		"List Alarms",
		"List Config",
		"Language",
		"Year",
		"Month",
		"Day",
		"Hour",
		"Minute",
		"Use",
		"Confirm",
		"Cancel",
		"PEEP: %3.1f",
		"Start Breathing",
		"Stop Breathing",
		"Oxygen %d psi"
	};

	int ksq = 0;
	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Increment or decrement setting value based on direction of knob
	*
	*/
	int readEncoder(int pin1, int pin2)
	{
		int val, v1, v2;
		v1 = digitalRead(pin1);  
		v2 = digitalRead(pin2);

		val = 0;
		if (v1 == 1 && v2 == 0)
		{
			if (prevPinVal[pin1] == 3)
				val = -1;
			else if (prevPinVal[pin1] == 4)
				val = 1;
			prevPinVal[pin1] = prevPinVal[pin2] = 1;			
		}
		else if (v1 == 0 && v2 == 1)
		{
			prevPinVal[pin1] = prevPinVal[pin2] = 2;
		}
		else if (v1 == 1 && v2 == 1)
		{
			prevPinVal[pin1] = prevPinVal[pin2] = 3;
		}
		else
		{
			prevPinVal[pin1] = prevPinVal[pin2] = 4;
		}
		return val;
	}

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Initialize VentSettiong encoder object
	*
	*/
	VentSetting::VentSetting(const char *_label, int _btn, int _pin1, int _pin2, int _min, int _max, int _inc, int _val, bool _wrap, void (*_display)(VentSetting *), short _dispX, short _dispY)
	{
		label = (char *)malloc(strlen(_label) + 1);
		strcpy(label, _label);
		btn = _btn;
		pin1 = _pin1;
		pin2 = _pin2;
		min = _min;
		zeroMin = min;
		max = _max;
		inc = _inc;
		value = _val;
		stagedValue = _val;
		mode = CFG_IDLE;
		display = _display;
		dispX = _dispX;
		dispY = _dispY;
		wrap = _wrap;
		lblWid = 270;
		lblHgt = 40;
		lblPad = 0;
		pinMode(btn,INPUT_PULLUP);
		pinMode(pin1,INPUT_PULLUP);
		pinMode(pin2,INPUT_PULLUP);
		encoderSet[encoderCount] = this;
		++encoderCount;
		hold = NULL;
		(*display)(this);
	}

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Place setting into an active state where the staged value change be chnaged by turning the knob
	*
	*/
	void VentSetting::activate()
	{
		if (graph->activeSetting == NULL)
		{
			graph->activeSetting = this;
			mode = CFG_ACTIVE;
			stagedValue = value;
			beginTouch = millis();
		}
	}

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Change the setting state to confirm to allow the staged value to be accepted or rejected
	*
	*/
	void VentSetting::stage()
	{
		mode = CFG_CONFIRM;
	}

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Accept the staged value as the current value and place the setting into an idle state
	*
	*/
	void VentSetting::confirm()
	{
		mode = CFG_IDLE;			
		if (value != stagedValue)
		{
			value = stagedValue;
			graph->changeInSettings();		
		}
	}

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Reject the staged value and leave the current value and place the setting into an idle state
	*
	*/
	void VentSetting::abort()
	{
		stagedValue = value;
		mode = CFG_IDLE;
	}

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Change the staged value by the indicated amount when in active mode
	* change between cancel and confirm when in confirm mode
	*
	*/
	void VentSetting::adjust(int amt)
	{
		if (mode == CFG_ACTIVE)
		{
			beginTouch = millis();			
			if (stagedValue == zeroMin && zeroMin != min && amt > 0)
			{
				stagedValue = min;
			}
			else
				stagedValue += amt;
			
			if (stagedValue < min)
			{
				if (zeroMin != min)
				{
					stagedValue = zeroMin;
				}
				else
					stagedValue = min;
			}
			if (stagedValue > max)
				stagedValue = max;
		}
		else if (mode == CFG_CONFIRM)
		{
			mode = CFG_CANCEL;
		}
		else if (mode == CFG_CANCEL)
		{
			mode = CFG_CONFIRM;
		}
	}
	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*
	*
	*/
	void VentSetting::show(char *str)
	{
		
		if (mode == CFG_IDLE)
		{
			sprintf(str, "%s: %d", label, value);
		}
		else
		{
			const char *note;
			if (mode == CFG_CONFIRM)
				note = __confirm;
			else if (mode == CFG_CANCEL)
				note = __cancel;
			else
				note = "";
			
			sprintf(str, "%s = %d Current: %d %s", label, stagedValue, value, note);
		}
	}
		
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* change the state of the setting object according to knob press action 
	*
	*/
	ConfigMode VentSetting::cycle()
	{
		switch (mode)
		{
			case CFG_IDLE:
				activate();
				break;
			case CFG_ACTIVE:
				if (value != stagedValue)
					stage();
				else
					abort();
				break;
			case CFG_CONFIRM:
				confirm();
				break;
			case CFG_CANCEL:
				abort();
				break;
			case CFG_INVALID_ID:
			default:
				break;
		}
		
		return mode;
	}

	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* check the setting object for long press indicating menu activation
	*
	*/
	void VentSetting::check()
	{		
		if (graph->activeSetting == NULL || graph->activeSetting == this)
		{
			int bv = 1 - digitalRead(btn);
			
			if (bv == 0 && prevPinVal[btn] == 1)
			{
				if (btnWasHeld != btn)
				{
					waitForHold = 0;
					beginTouch = millis();
					ConfigMode retMode = CFG_INVALID_ID;
								
					retMode = cycle();
					if (retMode == CFG_IDLE)
					{
						graph->activeSetting = NULL;
					}
					if (display)
						(*display)(this);
				}
				btnWasHeld = 0;
			}
			else if (bv == 1 && prevPinVal[btn] == 0)
			{
				beginTouch = millis();
				waitForHold = btn;
			}
			else if (bv == 1)
			{
				if (graph->activeSetting == NULL)
				{
					if (waitForHold == btn)
					{
						long held = millis() - beginTouch;
						if (held > 1234)
						{
							waitForHold = 0;
							if (hold)
							{
								btnWasHeld = btn;
								wasHeld = this;
								(*hold)(this);
							}
						}
					}
				}
			}
			else
			{
				//
				// Check for timeout of active setting change
				//
				if (graph->activeSetting == this)
				{
					if (millis() - beginTouch > 10000)
					{
						mode = CFG_IDLE;
						graph->activeSetting = NULL;
						if (display)
							(*display)(this);
					}
				}
			}
			

			prevPinVal[btn] = bv;
		}
		
		if (mode != CFG_IDLE)
		{
			int k = readEncoder(pin1, pin2);
			if (k != 0)
			{
				adjust(k * inc);
				if (display)
					(*display)(this);
			}
		}
   }


	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*  Display menu and other text information at bottom of screen
	*
	*/
	void showInfo(const char *info, int reverse)
	{
		if (reverse)
		{
			graph->display.textrgb(0,0,0);				
			graph->display.rgb(250,150,0);
		}
		else
		{
			graph->display.rgb(0,0,0);				
			graph->display.textrgb(250,150,0);
		}

		graph->display.fillrect(250, 1, 300, 38);
		graph->display.setTextSize(BigFontSize);
		graph->display.text(255, 1, info);
	}


	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*  Display the current clock setting
	*
	*/
	void showClock()
	{
		char txt[64];
		
		graph->display.rgb(0,0,0);				
		graph->display.fillrect(100, 150, 550, 60);								

		graph->display.textrgb(250,150,0);
		sprintf(txt, "Set Time %04d:%02d:%02d %02d:%02d", _year_val, _month_val, _day_val, _hour_val, _min_val);
		
		graph->display.setTextSize(BigFontSize);
		graph->display.text(100, 150, txt);
	}
	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*  Return a value limited by a hi and lo rhange 
	*
	*/
	int checkRange(int n, int lo, int hi)
	{
		if (n < lo)
			return hi;
		else if (n > hi)
			return lo;
		else
			return n;
	}
	
	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Main vent graph service function
	*
	*/
	void VentSetting::service()
	{
		if (menuFunction)
		{
			int bv = 1 - digitalRead(wasHeld->btn);
			int ev = readEncoder(wasHeld->pin1, wasHeld->pin2);
			if (bv != prevPinVal[wasHeld->btn] || ev != 0)
			{
				(*menuFunction)(bv, ev);
				if (wasHeld)
					prevPinVal[wasHeld->btn] = bv;
			}
		}
		else
		{
			for (int ex = 0; ex < encoderCount; ++ex)
				encoderSet[ex]->check();
			if (graph->activeSetting == NULL)
				graph->showO2();
		}
		
	}

	/*********************************************************************************************/
	/*********************************************************************************************/
	/************************************** VentGraph ********************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/

	/*
	* [EVS-SRS-11,12,13,14,15] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* inform motor state machine of changes to settings value
	* save changes to sd card and repaint screen with new settings
	*
	*/
	void VentGraph::changeInSettings()
	{
		if (bpm->value == 0 && motorFlag != 0)
		{
			motorFlag = 0;
		}
		else if (bpm->value != 0 && motorFlag == 0)
		{
			motorFlag = 1;
		}
		
		(*graph->changeSettings)(hiPressure->value, loPressure->value, (int)volume->value, bpm->value, (float)ratio->value / 10.0, motorFlag);
		saveSettings();
		initGraph();
		prevo2psi = 0;
	}
	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Draw a line using current color from point x1, y1 to x2, y2
	*
	*/
    void VentGraph::drawLine(int x1, int y1, int x2, int y2)
    {
      int _y1 = yMax - y1;
      int _y2 = yMax - y2;
      display.line(x1, _y1, x2, _y2);
    }
	  
	/*
	* [EVS-SRS-27] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Display the current ocygen supply pressure
	*
	*/
    void VentGraph::showO2()
    {
		char info[64];

		if ((int)o2Level != (int)prevo2psi)
		{
			char errtxt[64];
			prevo2psi = o2Level;

			graph->display.rgb(0,0,0);
			graph->display.fillrect(600, hiTextY, 200, 38);
	
			if (o2Level < minO2psi)
			{
Serial.printf("o2 %f < min %f\n", o2Level, minO2psi);
				sprintf(errtxt, lbls[O2LO_LABEL], (int)o2Level);
				postAlarm(SupplyAlarm, 2, 2000, errtxt);
			}
			else if (o2Level > maxO2psi)
			{
				sprintf(errtxt, lbls[O2HI_LABEL], (int)o2Level);
				postAlarm(SupplyAlarm, 2, 2000, errtxt);
			}
			else
				postAlarm(SupplyAlarm, 0, 0, (char *)"");


			display.textrgb(200,200,0);
			
			sprintf(info, lbls[OXYGEN_LABEL], (int)o2Level);
			display.setTextSize(StdFontSize);
			display.text(600, hiTextY, info);
		}		

    }

	/*
	* 6/15/2020	Craig Cunic		Original development
	*
	* Display newly posted error message on screen
	*
	*/
    void VentGraph::showError()
    {
		if (err == ERROR_POSTED)
		{
			err = ERROR_VISIBLE;
			graph->display.rgb(22,22,22);
			graph->display.fillrect(50, 200, 700, 60);
			display.textrgb(200,0,0);			
			display.setTextSize(ErrFontSize);
			display.text(55, 205, errmsg);
		}

    }
	
    void VentGraph::postError(char *msg)
    {
		if (err == ERROR_HIDDEN || strcmp(errmsg, msg))
		{
			err = ERROR_POSTED;
			strcpy(errmsg, msg);
		}
	}
	
    void VentGraph::clearError()
    {
		if (err)
		{
			err = ERROR_HIDDEN;
			initGraph();
		}
	}

	/*
	* [EVS-SRS-25] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Display calculated PPEP value at top of screen
	*
	*/
    void VentGraph::showPEEP()
    {
		char info[32];

		if (menuFunction == NULL)
		{
			int peep_check = (int)(10 * avg_peep);
			if (peep_check != prev_peep)
			{
				prev_peep = peep_check;
				graph->display.rgb(0,0,0);
				graph->display.fillrect(385, hiTextY, 200, 34);

				display.textrgb(250,250,0);
				sprintf(info, lbls[PEEP_LABEL], avg_peep);
				
				display.setTextSize(StdFontSize);
				display.text(385, hiTextY, info);
			}
		}
    }

	int hasTone = 0;
	/*
	* [EVS-SRS-30,32] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*
	*
	*/
    void VentGraph::showAlarms()
    {
		int sound = 0;
		int freq = 0;
		long now = millis();
		
		for (int ax = 0; ax < 2; ++ax)
		{
			if (alarmSet[ax].level > 0)
			{
				if (now > alarmSet[ax].when) // make sure alarm was not a momentary anomilly
				{
					sound = 1;
					freq = alarmSet[ax].freq;
					if (strcmp(alarmSet[ax].prev, alarmSet[ax].text))
					{
						strcpy(alarmSet[ax].prev, alarmSet[ax].text);
						graph->display.rgb(255,0,0);
						graph->display.fillrect(alarmSet[ax].x, alarmSet[ax].y, alarmSet[ax].wid, alarmSet[ax].hgt);
						display.textrgb(0,0,0);
						display.setTextSize(BigFontSize);
						display.text(alarmSet[ax].x, alarmSet[ax].y - 2, alarmSet[ax].text);
					}
				}
			}
			else
			{
				strcpy(alarmSet[ax].prev, "");
				graph->display.rgb(0,0,0);
				graph->display.fillrect(alarmSet[ax].x, alarmSet[ax].y, alarmSet[ax].wid, alarmSet[ax].hgt);
			}
						
		}
		
		if (sound)
		{
			if (now > silenceUntil)
			{
				if (hasTone == 0)
				{
					hasTone = 1;
//					analogWrite(A39, freq);
					tone(ALARM_PIN, freq);
				}
			}
		}
		else
		{
			if (hasTone)
			{
				hasTone = 0;
				noTone(ALARM_PIN);
//				analogWrite(A39, 0);
			}
		}		
		
    }

	/*
	* [EVS-SRS-29,31] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Post new alarm text at the indicated level and set frequency to be used for assoicate audible alarem
	*
	*/
    void VentGraph::postAlarm(int type, int lvl, int freq, char *txt)
    {
		if (lvl)
		{
			if (alarmSet[type].level != lvl || strcmp(alarmSet[type].text, txt) != 0)
			{
				logAlarm(txt);
				if (alarmSet[type].when == 0)
					alarmSet[type].when = millis() + msAlarmPostLag;
				alarmSet[type].prev[0] = '\0';
				strcpy(alarmSet[type].text, txt);
			}
			alarmSet[type].freq = freq;
		}
		else
		{
			alarmSet[type].when = 0;
		}
		alarmSet[type].level = lvl;
		showAlarms();
    }


	int initcnt = 0;

	/*
	* [EVS-SRS-19] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Draw time scale lines at bottom of graph
	*
	*/
	void VentGraph::drawTimeScale()
	{
		int xsize = xMax - xMin;
		int secsize = xsize / 20;
//		display.rgb(180,199,231);    
//		display.fillrect(0, 570, 800, 50);
		display.rgb(20,20,20);
		drawLine(xMin, 2, xMax, 2);

		for (int sx = 0; sx < xMax; sx += secsize)
		{
			drawLine(sx, 2, sx, 7);
		}

	}


	/*
	* [EVS-SRS-19] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Convert ml volume setting to number of PWM increments/postion
	*
	*/
	int VentGraph::volToPos(int vol)
	{
		return constrain(map(vol, MIN_VOL, MAX_VOL, MIN_POS, MAX_POS), MIN_POS, MAX_POS);
	}

/*	
    int VentGraph::computeRange(float vac)
	{
		int cvr;
		float v = (float)volume->value;
		float adj;
		
		adj = (float)1.0 + (avg_peep * vac);

		if (loPressure->value & 1)
			v *= adj;
		
		cvr = volToPos((int)v);
//Serial.printf("v: %d->%d peep: %f adj: %f cvr: %d\n", volume->value, (int)v, avg_peep, adj, cvr);
		return cvr;
	}
*/


    int VentGraph::computeRange(float vac, int *adjustedVolume)
	{
		int cvr;
		float v = (float)volume->value;
		v *= vac;
		cvr = volToPos((int)v);
		*adjustedVolume = v;
		return cvr;
	}


	/*
	* [EVS-SRS-19] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Initialize graphics and repaint screen 
	*
	*/
	void VentGraph::initGraph()
	{
		prevX = xPos = 0;
		display.loadFont(StdVentFont, (char *)"VENT.ttf");
		display.setTextFont(StdVentFont);
		display.getSize(&scrWid, &scrHgt);
//		def(0, scrWid, 0, scrHgt, 4, 0, scrHgt);

		display.textsize(StdFontSize);
//		display.background(180,199,231);    
		display.background(0,0,0);    
		display.rgb(0,0,0);
		display.fillrect(0, 0, scrWid, scrHgt);
		display.rgb(255, 255, 255);
		drawLine(xMin, hiLimitY-1, xMax, hiLimitY-1);
		drawLine(xMin, hiLimitY, xMax, hiLimitY);
		drawLine(xMin, hiLimitY+1, xMax, hiLimitY+1);
		display.rgb(255, 255, 255);
		drawLine(xMin, loLimitY-1, xMax, loLimitY-1);
		drawLine(xMin, loLimitY, xMax, loLimitY);
		drawLine(xMin, loLimitY+1, xMax, loLimitY+1);

		//drawTimeScale();
		graph->display.textrgb(220,0,220);

//			display.text(250, 10, "A T O R  L A B S");

		for (int ex = 0; ex < encoderCount; ++ex)
		{
			(*encoderSet[ex]->display)(encoderSet[ex]);
		} 
		if (menuFunction)
			(*menuFunction)(0, 0);
		
		if (err == ERROR_VISIBLE)
			err = ERROR_POSTED;
	}

	/*
	* [EVS-SRS-19] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Clear the graph screen in preperation for repaint
	*
	*/
	void VentGraph::clearGraph()
	{
		prevo2psi = 0;		
		prevX = xPos = 0;
		display.rgb(0,0,0);
		if (didAlarm)
		{
			display.fillrect(0, 38, 800, 375);
			display.rgb(255, 0, 0);
			drawLine(xMin, hiLimitY, xMax, hiLimitY);
			drawLine(xMin, loLimitY, xMax, loLimitY);
			didAlarm = 0;
		}
		else
		{
			display.fillrect(0, 41, 800, 369);
		}
	}

	/*
	* [EVS-SRS-19] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Clear rectangle (old graph values) just ahead of ploted airway pressure 
	*
	*/
	void VentGraph::clearAhead(int adj)
	{
		int x = prevX + adj;
		display.rgb(0, 0, 0);
		display.fillrect(x, loLimitY - 38, clrwid, hiLimitY - loLimitY + 18);
		display.rgb(255, 255, 255);
		drawLine(x, hiLimitY-1, x + clrwid, hiLimitY-1);
		drawLine(x, hiLimitY, x + clrwid, hiLimitY);
		drawLine(x, hiLimitY+1, x + clrwid, hiLimitY+1);
	
		drawLine(x, loLimitY-1, x + clrwid, loLimitY-1);
		drawLine(x, loLimitY, x + clrwid, loLimitY);
		drawLine(x, loLimitY+1, x + clrwid, loLimitY+1);

	}
	
	/*
	* [EVS-SRS-20] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Calculate the running average peep over the last N samples
	*
	*/
	float VentGraph::calcAvgPEEP(float curr_peep)
	{
		if (curr_peep < 0)
			curr_peep = 0;
		if (first_peep)
		{
			first_peep = 0;
			for (int idx = 0; idx < PEEP_SAMPLE_COUNT; ++idx)
			{
				peep_val[idx] = curr_peep;
			}
			avg_peep = curr_peep;
			
			peep_idx = 0;
		}
		else
		{		  
			if (++peep_idx == PEEP_SAMPLE_COUNT)
				peep_idx = 0;
			peep_val[peep_idx] = curr_peep;
			
			float peep_total = 0.0f;
			
			for (int idx = 0; idx < PEEP_SAMPLE_COUNT; ++idx)
				peep_total += peep_val[idx];
		
			avg_peep = peep_total / PEEP_SAMPLE_COUNT;
		}
		return avg_peep;
	}
	
	int VentGraph::getPeep()
	{
		return avg_peep;
	}
	
	void VentGraph::getBounce(int *lo, int *hi, int *bumps)
	{
		*lo = rfMin;
		*hi = rfBounce;
		*bumps = rfBumps;
	}
	
	/*
	* [EVS-SRS-26] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Plot the current airway pressure or red line at top or bottom if value is above or below limits
	*
	*/
static int curr_peep_value = 0;
static long cycle = 0;
	void VentGraph::plotBreath(int value)
	{
		float yRange = (float)(hiLimitY - loLimitY - 2);
		float pRange = (float)(hiPressure->value - loPressure->value);		
		float yScale = yRange / pRange;
		int traceY = 0;
		
		++cycle;

		if (err)
		{
			//Serial.printf("ERROR %d/%d\n", err, errmsg);
			showError();
			return;
		}
		
		yPos = loLimitY + 1 + (int)((value - loPressure->value) * yScale);
				
				
						
		if (yPos >= hiLimitY)
		{
			yPos = hiLimitY - 1;
			traceY = hiLimitY + 5;
		}
		else if (yPos <= loLimitY)
		{
			yPos = loLimitY + 1;
			traceY = loLimitY - 5;
		}

		clearAhead(1);

		int hadAlarm = alarmSet[BreathAlarm].level;

		if (value > hiPressure->value)
		{
			didAlarm = 1;
			postAlarm(BreathAlarm, 2, 4000, (char *)"High Pressure");
		}
		else if (value  < loPressure->value)
		{
			didAlarm = 1;
			postAlarm(BreathAlarm, 2, 500, (char *)"Low Pressure");
		}
		else if (alarmSet[BreathAlarm].level)
		{
			postAlarm(BreathAlarm, 0, 0, (char *)"");
		}

		if (alarmSet[BreathAlarm].level && hadAlarm)
		{
			display.rgb(255, 0, 0);
			display.fillrect(prevX, 478 - traceY, xPos - prevX, 5);
		}
		else
		{
			switch (air_mode)
			{
				case AIR_IDLE:
					display.rgb(222, 222, 0);		
					break;
				case AIR_REFILL:
					display.rgb(0, 222, 0);		
					if (value < rfMin)
					{
						rfMin = value;
						rfBounce = 0;
					}


					if (rfDir > 0 && value < prev_breath_value)
					{
						++rfBumps;
						rfDir = -1;
					}
					else if (rfDir < 0 && value > prev_breath_value)
					{
						++rfBumps;
						rfDir = 1;
					}
					
					if (value > rfBounce)
						rfBounce = value;
					break;
				case AIR_DELIVER:
					if (value > bpMax)
					{
						bpMax = value;
						hiCycle = cycle;
					}
					else if (value < bpMin)
					{
						bpMin = value;
						loCycle = cycle;
					}
					
					display.rgb(222, 222, 0);		
					break;
				case AIR_PEEK:
					display.rgb(222, 0, 222);		
					break;
				case AIR_END:
					display.rgb(0, 0, 222);		
					bpMin = 9999;
					bpMax = 0;
					rfMin = 9999;
					rfBounce = 0;
					rfDir = -1;
					rfBumps = 0;
					break;
				default:
					display.rgb(222, 222, 222);		
					break;
			}
			drawLine(prevX, prevY, xPos, yPos);
			display.rgb(0, 255, 255);
			display.fillrect(xPos + 2, 479 - yPos, 3, 3);
		}

		if (xPos >= scrWid - xInc)
		{
			didAlarm = 0;
			prevY = yPos;
			xPos = prevX = 0;
			clearAhead(0);
		}
		else
		{
			prevY = yPos;
			prevX = xPos;
			xPos = prevX + xInc;
		}
		prev_breath_value = value;
	}


	void VentGraph::bpRange(int *min, int *max, int *samples)
	{
		*min = bpMin;
		*max = bpMax;
		*samples = hiCycle - loCycle;		
	}
	

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*
	*
	*/
	void VentGraph::log(const char* format, ...)
	{
		char msg[256];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(msg, format, argptr);
		va_end(argptr);
		
		display.log(msg);
	}

	/*
	* 6/15/2020	Craig Cunic		Original development
	*
	* Obtain size of display in pixels
	*
	*/
	void VentGraph::getSize(short *wid, short *hgt)
	{
		display.getSize(wid, hgt);
	}

	long scn = 0;
	long sdn = 0;
	int ctlPass = 4;

	/*
	* 6/15/2020	Craig Cunic		Original development
	*
	* Handle main loop activities for display
	*
	*/
	void serviceDisplay()
	{
		graph->service();
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*
	*
	*/
	void serviceControl()
	{
		if (++ctlPass == 5)
		{
			serviceDisplay();
			ctlPass = 0;
		}
		VentSetting::service();
	}


	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* One timne startup activities
	*
	*/
	void VentGraph::setup(void)
	{
		if ( !SD.begin(BUILTIN_SDCARD))
		{
			Serial.println((char *)"initialization failed!");
		}
		
		zeroTime = millis();
		pinMode(ALARM_PIN, OUTPUT); // Set ALARM_PIN

		loadSettings();
		initGraph();		
		controlTimer.priority(3);

		controlTimer.begin(serviceControl, 10000);
	}


	/*
	* [EVS-SRS-24,25,26,27] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Display the value of the indicated encoder setting on the screen at the position assigned to that setting
	*
	*/
	void displaySetting(VentSetting *vs)
	{
		char inner[62], txt[64];
		int pad = 0;
		
		if (vs->mode == CFG_IDLE)
		{
//			graph->display.rgb(180,199,231);    
			graph->display.rgb(0,0,0);
			graph->display.textrgb(255,255,0);
			sprintf(txt, vs->label, vs->value);
			pad = vs->lblPad;
		}
		else if (vs->mode == CFG_ACTIVE)
		{
			graph->display.rgb(200,200,200);
			graph->display.textrgb(0,0,0);
			sprintf(inner, vs->label, vs->stagedValue);
			sprintf(txt, "[%s]", inner);
		}
		else if (vs->mode == CFG_CONFIRM)
		{
			graph->display.rgb(0,200,0);
			graph->display.textrgb(0,0,0);
			sprintf(txt, lbls[CONFIRM_LABEL], vs->stagedValue);
		}
		else
		{
			graph->display.rgb(200,0,0);
			graph->display.textrgb(0,0,0);
			sprintf(txt, lbls[CANCEL_LABEL], vs->value);
		}

		graph->display.fillrect(vs->dispX + pad, vs->dispY - 2, vs->lblWid, vs->lblHgt);
		graph->display.textsize(StdFontSize);
		graph->display.text(vs->dispX + pad, vs->dispY, txt);
	}


	/*
	* [EVS-SRS-28] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Display the current value of the ventelation ratio 
	* 
	*/
	void displayRatio(VentSetting *vs)
	{
		char inner[62], txt[64];
		int pad = 0;
		
		if (vs->mode == CFG_IDLE)
		{
//			graph->display.rgb(180,199,231);    
			graph->display.rgb(0,0,0);
			graph->display.textrgb(255,255,0);
			sprintf(txt, vs->label, (float)(vs->value/10.0F));
			pad = vs->lblPad;
		}
		else if (vs->mode == CFG_ACTIVE)
		{
			graph->display.rgb(200,200,200);
			graph->display.textrgb(0,0,0);
			sprintf(inner, vs->label, (float)(vs->stagedValue/10.0F));
			sprintf(txt, "[%s]", inner);
		}
		else if (vs->mode == CFG_CONFIRM)
		{
			graph->display.rgb(0,200,0);
			graph->display.textrgb(0,0,0);
			sprintf(txt, "%s %s 1:%1.1f", lbls[APPLY_LABEL], lbls[USE_LABEL], (float)(vs->stagedValue/10.0F));
		}
		else
		{
			graph->display.rgb(200,0,0);
			graph->display.textrgb(0,0,0);
			sprintf(txt, "%s %s 1:%1.1f", lbls[ABORT_LABEL], lbls[USE_LABEL], (float)(vs->value/10.0F));
		}

		graph->display.fillrect(vs->dispX + pad, vs->dispY, vs->lblWid, vs->lblHgt);
		graph->display.text(vs->dispX + pad, vs->dispY, txt);
	}

	/*
	* [EVS-SRS-5,6,7,8] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Main control service function for the EVS graph display and encoder processing
	*
	*/
	void VentGraph::service()
	{	
		(*sample)(&airway, &supply);		
		plotBreath(airway);
		showAlarms();
	}
	
	
	
	/*
	* [EVS-SRS-38] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Save current settings values to permenant file on the sd card
	*
	*/
	void VentGraph::saveSettings()
	{
		File cfgFile;
		char cfg[64];
		
		SD.remove(VENT_CONFIG);
		cfgFile = SD.open(VENT_CONFIG, FILE_WRITE);
		if (cfgFile)
		{
			sprintf(cfg, "%d,%d,%d,%d,%d", hiPressure->value, loPressure->value, volume->value, bpm->value, ratio->value);
			cfgFile.println(cfg);
			cfgFile.close();
		}
		cfgFile = SD.open(HISTORY_LOG, FILE_WRITE);
		if (cfgFile)
		{
			sprintf(cfg, "%04d.%02d.%02d.%02d.%02d.%02d,%d,%d,%d,%d,%d", year(), month(), day(), hour(), minute(), second(), hiPressure->value, loPressure->value, volume->value, bpm->value, ratio->value);
			cfgFile.println(cfg);
			cfgFile.close();
		}
	}

	long lastAlarm = -70000;
	/*
	* [EVS-SRS-36] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Log and alarm to a permanant file on the SD card
	*
	*/
	void VentGraph::logAlarm(const char *text)
	{
		long now = millis();
		if (now - lastAlarm > 60000)
		{
			lastAlarm = now;
			File fd = SD.open(ALARM_LOG, FILE_WRITE);
			if (fd)
			{
				char txt[256];
				sprintf(txt, "%04d.%02d.%02d.%02d.%02d.%02d %s", year(), month(), day(), hour(), minute(), second(), text);
				fd.println(txt);
				fd.close();
			}
		}
	}

	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*
	*
	*/
	void VentGraph::checkBootMenu()
	{
	}
	

	/*
	* 6/15/2020	Craig Cunic		Original development
	* Read string from a file into preallocated buffer until a dilimiter is encountered 
	* maxiumom of 256 characters
	*
	*/
	int VentGraph::readString(File fd, char *buf, char delim)
	{
		int cnt = 0;
		while (fd.available()) 
		{
			buf[cnt] = fd.read();
			if (buf[cnt] == delim || cnt > 256)
			{
				buf[cnt] = '\0';
				return cnt;
			}
			++cnt;
		}
		return cnt;
	}

	/*
	* 6/15/2020	Craig Cunic		Original development
	*
	*  Read and integer value from a file stopping at the indicated delimiter character
	*
	*/
	int VentGraph::readInt(File fd, char delim)
	{
		char buf[32];
		int cnt = 0;
		while (fd.available()) 
		{
			buf[cnt] = fd.read();
			if (buf[cnt] == delim || cnt >= 16)
			{
				buf[cnt] = '\0';
				return atoi(buf);
			}
			++cnt;
		}
		buf[cnt] = '\0';
		return atoi(buf);
	}
	
	/*
	* 6/15/2020	Craig Cunic		Original development
	*
	*  Split a string into smaller strings sperated by the indicated delimiter character
	*
	*/
	int VentGraph::split(char *str, char delim, char *set[])
	{
		char *c;
		int cnt = 0;
		c = str;
		set[cnt] = c;
		for (;;)
		{
			if (*c == '\0')
			{				
				break;
			}
			else if (*c == delim)
			{
				*c++ = '\0';
				set[++cnt] = ++c;
			}
			else
				++c;
		}
		return cnt + 1;
	}
	
	/*
	* [EVS-SRS-39] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Restore the settings from the values saved to the file on the SD card
	*
	*/
	void VentGraph::loadSettings()
	{
		char buf[64];			
		File cfgFile;

		cfgFile = SD.open(VENT_CONFIG);
		readString(cfgFile, buf, '\n');
		cfgFile.close();


		cfgFile = SD.open(VENT_CONFIG);
		if (cfgFile)
		{
			hiPressure->value = readInt(cfgFile, ',');
			loPressure->value = readInt(cfgFile, ',');
			volume->value = readInt(cfgFile, ',');
			bpm->value = readInt(cfgFile, ',');
			ratio->value = readInt(cfgFile, '\n');
			cfgFile.close();
		}
	}


	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Change the index to a list of items and show the new item name at the bottom of the screen
	*
	*/
	int choose(const char **txt, int *_idx, int inc)
	{
		int idx = *_idx;
		idx += inc;
		if (idx < 0)
		{
			while (txt[idx + 1] != NULL)
				++idx;				
		}
		else if (txt[idx] == NULL)
			idx = 0;
		*_idx = idx;
		showInfo(txt[idx], 0);
		return idx;
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Process a mode/settings change and inform the motor control state machine of the change
	*
	*/
	void VentGraph::modeChange()
	{
		(*changeSettings)(hiPressure->value, loPressure->value, volume->value, bpm->value, (float)ratio->value / 10.0, motorFlag);
	}

	/*
	* [EVS-SRS-9] 
	* 6/15/2020	Craig Cunic		Original development
	* Exit from active menu display
	*
	* Exit the encoder menu mode when the knob is pressed
	*
	*/
	void exitMenu(int bv, int ev)
	{
		if (bv)
		{
			showInfo((char *)"", 0);
			graph->display.setTextSize(StdFontSize);
			menuFunction = NULL;
			wasHeld = NULL;
		}
	}

	/*
	* [EVS-SRS-3] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Force a start of the ventilator motor state machine
	*
	*/
	void VentGraph::startMotor()
	{		
		motorFlag = 1;
		modeChange();
	}
	
	/*
	* [EVS-SRS-3] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Menu action to send a start request to the motor state machine and exit menu system.
	*
	*/
	void _startMotor(int bv, int ev)
	{
		if (bv)
		{
			graph->startMotor();
			exitMenu(bv, ev);
		}
	}

	/*
	* [EVS-SRS-35] 
	* 6/15/2020	Craig Cunic		Original development
	*
	*
	* Menu action to send a stop request to the motor state machine and exit menu system.
	*
	*/
	void VentGraph::stopMotor()
	{
		motorFlag = 0;
		bpm->value = 0;
		(*bpm->display)(bpm);
		modeChange();
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Menu system wrapper function for calling stop motor code
	*
	*/
	void _stopMotor(int bv, int ev)
	{
		if (bv)
		{
			graph->stopMotor();
			exitMenu(bv, ev);
		}
	}



	//
	// Time menu
	//
	char __year[16], __month[16], __day[16], __hour[16], __min[16];
	char *timeMenu[] = { __year, __month, __day, __hour, __min, (char *)lbls[EXIT_LABEL], (char *)NULL };
	int timex = 0;
	int timeSlot = -1;
	void chooseTime(int bv, int ev)
	{
		if (ev || bv)
		{
			if (bv)
			{
				if (timeSlot == -1) // select time slot to adjust or exit
				{
					// if index is exit then exit
					if (timex == 5)
					{
						exitMenu(bv, 0);
						return;
					}
					else
					{
						timeSlot = timex;
					}
				}
				else // stop adusting current time slot
				{
					timeSlot = -1;
				}
			}
			_year_val = year();
			if (_year_val < 2020)
				_year_val = 2020;
			_month_val = month();
			_day_val = day();
			_hour_val = hour();
			_min_val = minute();
			switch (timeSlot)
			{
				case 0: 
					_year_val = checkRange(_year_val + ev, 2020, 2222);
					break;
				case 1: 
					_month_val = checkRange(_month_val + ev, 1, 12);
					break;
				case 2: 
					_day_val = checkRange(_day_val + ev, 1, 31);
					break;
				case 3: 
					_hour_val = checkRange(_hour_val + ev, 0, 23);
					break;
				case 4: 
					_min_val = checkRange(_min_val + ev, 1, 59);
					break;
				default:
					break;
			}
			setTime(_hour_val, _min_val, 0, _day_val, _month_val, _year_val);
		}

		sprintf(timeMenu[0], "%s:%04d", lbls[YEAR_LABEL], year());
		sprintf(timeMenu[1], "%s:%02d", lbls[MONTH_LABEL], month());
		sprintf(timeMenu[2], "%s:%02d", lbls[DAY_LABEL], day());
		sprintf(timeMenu[3], "%s:%02d", lbls[HOUR_LABEL], hour());
		sprintf(timeMenu[4], "%s:%02d", lbls[MINUTE_LABEL], minute());
		if (timeSlot == -1)
		{
			choose((const char **)timeMenu, &timex, timeSlot == -1 ? ev : 0);
		}
		else
			showInfo(timeMenu[timeSlot], 1);

	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Silence audible alarms for a period of 1 minute
	*
	*/
	void silence(int bv, int ev)
	{
		if (bv)
		{
			silenceUntil = millis() + 60000;
			exitMenu(1, 0);
		}
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Enter menu time mode for setting EVS real time clock
	*
	*/
	void timeMode(int bv, int ev)
	{
		if (bv)
		{
			menuFunction = chooseTime;
			chooseTime(0, 0);
		}
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Read a single line from a file on the SD card
	*
	*/
	int readLine(File fd, char *buf, int max)
	{
		int idx = 0;
		while (fd.available())
		{
			buf[idx] = fd.read();
			if (buf[idx++] == '\n')
				break;
		}
		buf[idx] = '\0';			
		
		return idx;
	}
	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Send the contents of the indicated file to the Serial port
	*
	*/
	void VentGraph::dumpFile(const char *path)
	{
		// Serial.printf((char *)"Dumping %s\n", path);
		File fd;
		fd = SD.open(path, FILE_READ);
		if (fd)
		{
			int pos = fd.size() - 32000;
			if (pos > 0)
			fd.seek(pos);

			for (;;)
			{
				char buf[256];
				if (readLine(fd, buf, 256) > 0)
				{
					Serial.print(buf);
				}
				else
					break;
			}
			fd.close();
		}
	}
	

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Stage a request to send the alarm log to the serial port
	*
	*/
	void listAlarms(int bv, int ev)
	{
		if (bv)
		{
			graph->pathToDump = ALARM_LOG;
			exitMenu(1, 0);
		}		
	}
	

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Stage a request to send the settings change log to the serial port
	*
	*/
	void listConfig(int bv, int ev)
	{
		if (bv)
		{
			graph->pathToDump = HISTORY_LOG;
			exitMenu(1, 0);
		}		
	}
	
	//
	// Language menu
	//
	const char *langMenu[] = { "English", "French", "Spanish", "German", lbls[EXIT_LABEL], (char *)NULL };
	int langx = 0;

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Change selected language and confirm when button has been pressed
	*
	*/
	void chooseLanguage(int bv, int ev)
	{
		choose(langMenu, &langx, ev);
		if (bv)
		{
			if (langx < 4)
				Serial.printf((char *)"Change language to %d %s\n", langx, langMenu[langx]);
			exitMenu(1, 0);
		}
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Enter lanaguage change menu mode
	*
	*/
	void languageMode(int bv, int ev)
	{
		if (bv)
		{
			showInfo((char *)"", 0);
			menuFunction = chooseLanguage;
		}
	}


	const char *mainMenu[] = { lbls[SILENCE_LABEL], lbls[SET_TIME_LABEL], lbls[LIST_ALARMS_LABEL], lbls[LIST_CONFIG_LABEL], lbls[LANGUAGE_LABEL], lbls[START_LABEL], lbls[STOP_LABEL], lbls[EXIT_LABEL], (char *)NULL };
	void (*mainFunc[])(int, int) = { silence, timeMode, listAlarms, listConfig, languageMode, _startMotor, _stopMotor, exitMenu };
	int mainx = 0;


	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Change selection in main menu mode
	*
	*/
	void _mainMenu(int bv, int ev)
	{
		choose(mainMenu, &mainx, ev);
		(*mainFunc[mainx])(bv, ev);
	}

	
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Enter main menu mode
	*
	*/
	void openMenu(VentSetting *s)
	{
		menuFunction = _mainMenu;
		_mainMenu(0, 0);
	}

	int firstLoop = 1;
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Loop function to be called from main Arduino loop function
	*
	*/
	void VentGraph::loop()
	{
		if (firstLoop)
		{
			firstLoop = 0;
			logAlarm((char *)"STARTUP");
			startMotor();
			modeChange();
		}

	    if (pathToDump)
		{
			const char *path = pathToDump;
			pathToDump = NULL;
			dumpFile(path);
		}
	}

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Initialize EVS alarm class
	*
	*/
	VentAlarm::VentAlarm()
	{
		strcpy(text, "");
		level = 0;
	}

	int pfp = 0;

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Collect current airway pressure from sensor
	*
	*/
	float VentGraph::airwayPressure()
	{
		int32_t pressure;
		float fPressure = 0.0F;
		int amt, errnum;
		char msg[64];

		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0xA5)); 
		Wire1.endTransmission();   
		Wire1.requestFrom(0x6D, 2);

		if((errnum = Wire1.getError()))
		{
			sprintf(msg, "AIRWAY INIT ERROR %d", errnum);
			postError((char *)msg);
			return -9999.0F;
		}

		if ((amt = Wire1.available()) < 2)
		{
			sprintf(msg, (char *)"AIRWAY STATUS ERROR amt: %d", amt);
			postError(msg);
			return -9999.0F;
		}

		uint16_t a5reg = 0;
		uint8_t byteval = 0;

		a5reg = Wire1.read() * 256;
		a5reg += Wire1.read();

		a5reg = a5reg & 0b0000011111111101;

		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0xA5));
		Wire1.write(highByte(a5reg));
		Wire1.write(lowByte(a5reg));
		Wire1.endTransmission();  

		if((errnum = Wire1.getError()))
		{
			postError((char *)"AIRWAY WRITE INTRO FAILED");
			return -9999.0F;
		}


		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0x30)); 
		Wire1.write(byte(0x0A));
		Wire1.endTransmission();          

		// Check if error occured
		if((errnum = Wire1.getError()))
		{
			postError((char *)"SENSOR WRITE REG1 FAILED");
			return -9999.0F;
		}


		/**********  Measurment  performed by Sensor **************/

		delay(5); //required to let sensor do it's thing

		/**********  Request Result from Sensor **************/

		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0x30)); 
		Wire1.endTransmission();  
		Wire1.requestFrom(0x6D, 1);

		if((errnum = Wire1.getError()))
		{
			postError((char *)"AIRWAY REG1 REQUEST FAILED");
			return -9999.0F;
		}

		if ((amt = Wire1.available()) < 1)
		{
			sprintf(msg, (char *)"AIRWAY REG1 READ ERROR amt: %d", amt);
			postError(msg);
			return -9999.0F;
		}


		//     uint8_t pres_status = Wire1.read();

		// Check if data Ready

		//    if(bitRead(pres_status, 3))
		 // Serial.print("Data NOT Ready\n");
		//     else
		 // Serial.print("Data Ready\n");


		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0x06)); 
		Wire1.endTransmission();  
		Wire1.requestFrom(0x6D, 1);

		if((errnum = Wire1.getError()))
		{
			postError((char *)"AIRWAY REG2 REQUEST FAILED");
			return -9999.0F;
		}


		if ((amt = Wire1.available()) < 1)
		{
			sprintf(msg, (char *)"AIRWAY REG2 READ ERROR amt: %d", amt);
			postError(msg);
			return -9999.0F;
		}


		byteval = Wire1.read();
		if(byteval & 0x80) //check sign and push to front. 24 to 32 bit conversion. 
			pressure = 0xFF;
		else 
			pressure = 0; 
		pressure = (pressure << 8) | byteval;


		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0x07)); 
		Wire1.endTransmission();  
		Wire1.requestFrom(0x6D, 1);

		if((errnum = Wire1.getError()))
		{
			postError((char *)"AIRWAY REG3 REQUEST FAILED");
			return -9999.0F;
		}


		if ((amt = Wire1.available()) < 1)
		{
			sprintf(msg, (char *)"AIRWAY REG3 READ ERROR amt: %d", amt);
			postError(msg);
			return -9999.0F;
		}


		byteval = Wire1.read();
		pressure = (pressure << 8) | byteval;

		Wire1.beginTransmission(0x6D);   
		Wire1.write(byte(0x08)); 
		Wire1.endTransmission(); 
		Wire1.requestFrom(0x6D, 1);

		if((errnum = Wire1.getError()))
		{
			postError((char *)"AIRWAY REG4 REQUEST FAILED");
			return -9999.0F;
		}


		if ((amt = Wire1.available()) < 1)
		{
			sprintf(msg, (char *)"AIRWAY REG4 READ ERROR amt: %d", amt);
			postError(msg);
			return -9999.0F;
		}


		byteval = Wire1.read();
		pressure = (pressure << 8) | byteval;



		//    Serial.print(" Data in Pa : ");
		pressure = (pressure * 8192 )/ 4194304;
		fPressure = (float)pressure;
		fPressure /= pScale;

		
		clearError();
		
	int cfp = (int)(fPressure * 100);
	if (cfp < pfp - 1 || cfp > pfp + 1)
	{
		pfp = (int)(fPressure * 100);
	}
	
	
		return pZero + fPressure;
	}
	
	int measureCycles = 99;
	int show = 10;
	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Collect current airway pressure from sensor
	*
	*/
	int O2ADDR = 0x28;
	float o2Pressure = 0.0F;
	
	float VentGraph::supplyPressure()
	{
		char msg[64];
		int amt, errnum;
		
		if (++measureCycles >= 1)
		{
			measureCycles = 0;
			Wire1.beginTransmission(O2ADDR);   
//			Wire1.endTransmission();   
			errnum = Wire1.getError();
			if (errnum)
			{
				sprintf(msg, "O2 INIT err: %d\n", errnum);
				postError((char *)msg);
				return -9999.0F;
			}

			Wire1.requestFrom(O2ADDR, 4);

			errnum = Wire1.getError();

			if (errnum)
			{
				sprintf(msg, "O2 REQ err: %d\n", errnum);
				postError((char *)msg);
				return -9999.0F;
			}

			amt = Wire1.available();
			if (amt >= 4)
			{
				int b1, b2, sts;		
				
				b1 = Wire1.read();
				sts = (b1 & 0xC0) >> 6;
				if (sts == 0)
				{
					b2 = Wire1.read();
					Wire1.read();
					Wire1.read();
					int hb = b1 & 0x3F;
					int pv = hb << 8 | b2;
					o2Pressure = o2Zero + ((float)(pv) / 100.0F) * o2Scale;
					if (o2Pressure > o2Max)
					{
						o2Max = o2Pressure;
					}

/*
if (++show >= 80)
{	
	show = 0;
	Serial.printf("supply sts: %d val: %3.2f\n", sts, o2Pressure);
}
*/
				}
			}
			Wire1.endTransmission();   
		}

		//clearError();
		return o2Pressure;
	}
	

	/*
	* [EVS-SRS-] 
	* 6/15/2020	Craig Cunic		Original development
	*
	* Initialize EVS Graph class 
	*
	*/
	VentGraph::VentGraph(void (*_sample)(float *, float *), void (*_changeSettings)(int, int, int, int, float, int))
	{
		sample = _sample;
		changeSettings = _changeSettings;
		graph = this;
		xMin = 0;
		xMax = 800;
		yMin = 0;
		yMax = 480;
		xInc = 2;
		xPos = 0;
		prevX = 0;
		prevY = loLimitY + 1;
		lowLimit = 20;
		highLimit = 80;
		prevBase = -1;
		display = VentDisplay(800, 480);
		o2Status = 1;
		alarmStart = 0;
		settingsMode = false;
		pathToDump = NULL;
		hiPressure = new VentSetting(lbls[UPPER_LABEL], 
			KNOB_LIMITHI_SW, KNOB_LIMITHI_P, KNOB_LIMITHI_D, 22, 40, 1, 40, false, displaySetting, leftTextX, hiTextY);
		hiPressure->lblWid = 370;
		loPressure = new VentSetting(lbls[LOWER_LABEL], 
			KNOB_LIMITLO_SW, KNOB_LIMITLO_P, KNOB_LIMITLO_D, 2, 20, 1, 5, false, displaySetting, leftTextX, loTextY);
		loPressure->lblWid = 370;
		volume = new VentSetting(lbls[VOLUME_LABEL], 
			KNOB_VOL_SW, KNOB_VOL_P, KNOB_VOL_D, MIN_VOL, MAX_VOL, 10, 450, false, displaySetting, leftTextX, bottomTextY);
		bpm = new VentSetting(lbls[BPM_LABEL], 
			KNOB_BPM_SW, KNOB_BPM_P, KNOB_BPM_D, 8, 40, 1, 12, false, displaySetting, midTextX, bottomTextY);
		bpm->zeroMin = 0;
		ratio = new VentSetting(lbls[RATIO_LABEL],
			KNOB_RATIO_SW, KNOB_RATIO_P, KNOB_RATIO_D, 10, 40, 1, 20, false, displayRatio, rightTextX, bottomTextY);
		ratio->lblPad = 120;
		ratio->hold = openMenu;
		prev_peep = 9999;
		prev_breath_value = 0.0F;
		activeSetting = NULL;
		setup();
		checkBootMenu();
		alarmSet[BreathAlarm].x = 365;
		alarmSet[BreathAlarm].y = alarmTextY;
		alarmSet[BreathAlarm].wid = 200;
		alarmSet[BreathAlarm].hgt = 35;
		alarmSet[BreathAlarm].when = 0;
		alarmSet[SupplyAlarm].x = 570;
		alarmSet[SupplyAlarm].y = alarmTextY;
		alarmSet[SupplyAlarm].wid = 220;
		alarmSet[SupplyAlarm].hgt = 35;
		alarmSet[SupplyAlarm].when = 0;
		strcpy(alarmSet[BreathAlarm].text, "");
		strcpy(alarmSet[SupplyAlarm].text, "");
		err = ERROR_HIDDEN;
			
	}

	long tick = 0;
	int prev_mode = 0;
	void VentGraph::setAirMode(AirMode mode)
	{
		if (mode != prev_mode)
		{
			if (mode == AIR_END)
			{
				o2Level = o2Max;
				o2Max = 0.0F;
			}
			prev_mode = mode;
		}
	
		if (mode == AIR_DELIVER && air_mode != AIR_DELIVER)
		{
			curr_peep_value = prev_breath_value;
			calcAvgPEEP(curr_peep_value);
			showPEEP();
		}
		
		air_mode = mode;
	}


#define WAV1  A0
#define AVGSZ 30
int firstSample = 1;
float amt[AVGSZ], total[AVGSZ];
int lastx = 0, firstx = 0;


float collectFakeAirway()
{
    float p, raw, val;
    
    raw = (float)analogRead(WAV1);
    val = ((float)(raw - 961.0) / 1.1);

    if (firstSample)
    {
        firstSample = 0;
        total[0] = val;
        for (int idx = 0; idx < AVGSZ; ++idx)
        {
            amt[idx] = val;
            total[idx] = (val * (AVGSZ));
        }
        p = val;
        
        firstx = 0;
        lastx = AVGSZ - 1;
    }
    else
    {
      
        if (++firstx == AVGSZ)
          firstx = 0;
        if (++lastx == AVGSZ)
          lastx = 0;
        amt[lastx] = val;
        int prevx = lastx - 1;
        if (prevx == -1)
          prevx = AVGSZ - 1;
    
        total[lastx] = total[prevx] - amt[firstx] + val;
        
        p = total[lastx] / AVGSZ;
    }
    return p;
}

