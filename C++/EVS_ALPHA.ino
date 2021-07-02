#include <i2c_t3.h>

#include <SPI.h>  
#include <TimeLib.h>
#include <time.h>

#include <VentDisplay.h>
#include <VentGraph.h>


//pin assignments 
#define VOL_MAX 200
#define VOL_MIN 800

//EEPROM addresses
#define EEA_BREATHVARS 0
//#define VOL_ADJ_CONST 0.000967841 // ml of water to atmospheres factor
#define VOL_ADJ_CONST 0.0256 // swag

uint32_t last_vg;

// Create an IntervalTimer object 
IntervalTimer MotorTickTimer, BreathTickTimer;
enum class MoveState{IDLE,MOVING};
enum class MoveDirection{UP,DOWN};
enum class BreathState{IDLE,OUT,RETRACT,REST};
//enum class MachineState{STARTUP,UINPUT,RUNNING};

//MachineState main_state;

struct moveVars{
volatile MoveState state;
MoveDirection direction;
uint8_t target, position, start, end;
uint16_t speed;
}move;

long start_time = 0;
long breath_count = 0;
int move_count = 0;
double total_vol = 0;
double vol_inc = 0.0;
int cycle_index = 0;


//ramp rate for out breath in ms (how long should outbreath last)

struct breathVars{
  BreathState state;
  uint16_t period, range, ramp;
  uint16_t period_new, range_new, ramp_new;
  uint32_t last_start;
  bool update;
}breath;


VentGraph *vg;

int32_t pressure;
float fPressure = 0.0F;
float fSupply = 0.0F;

int vol_set, rate_set;
float ratio_set;
int settingsChanged = 1;
struct compensation *volComp;
int currPeep = 0;
int prevPeep = -1;

struct compensation
        {
            const void* key;
            float val;
        };
struct compensation compensationSet[] = {
{"200.10.1:01.10", 0.39454544},
{"200.10.1:01.15", 0.25074998},
{"200.10.1:01.5", 0.22993751},
{"200.10.1:02.10", 0.24150002},
{"200.10.1:02.15", 0.1770143},
{"200.10.1:02.5", 0.16162354},
{"200.16.1:01.10", 0.1675},
{"200.16.1:01.15", 0.22513458},
{"200.16.1:01.5", 0.15501252},
{"200.16.1:02.10", 0.1675},
{"200.16.1:02.15", 0.18030769},
{"200.16.1:02.5", 0.113387525},
{"200.22.1:01.10", 0.12965912},
{"200.22.1:01.15", 0.1788523},
{"200.22.1:01.5", 0.1091169},
{"200.22.1:02.10", 0.31045455},
{"200.22.1:02.15", 0.18566361},
{"300.10.1:01.10", 0.3022857},
{"300.10.1:01.15", 0.15085001},
{"300.10.1:01.5", 0.1564},
{"300.10.1:02.10", 0.15236364},
{"300.10.1:02.15", 0.12785716},
{"300.10.1:02.5", 0.17275788},
{"300.16.1:01.10", 0.13666667},
{"300.16.1:01.15", 0.13628124},
{"300.16.1:01.5", 0.10010712},
{"300.16.1:02.10", 0.10010712},
{"300.16.1:02.15", 0.12433329},
{"300.16.1:02.5", 0.075},
{"300.22.1:01.10", 0.11776622},
{"300.22.1:01.15", 0.10623372},
{"300.22.1:01.5", 0.07970907},
{"300.22.1:02.10", 0.17831166},
{"300.22.1:02.15", 0.12713633},
{"300.22.1:02.5", 0.07387879},
{"400.10.1:01.10", 0.2489},
{"400.10.1:01.15", 0.13722725},
{"400.10.1:01.5", 0.12171249},
{"400.10.1:02.10", 0.12587501},
{"400.10.1:02.15", 0.106954575},
{"400.10.1:02.5", 0.0634375},
{"400.16.1:01.10", 0.13479462},
{"400.16.1:01.15", 0.11546875},
{"400.16.1:01.5", 0.08771873},
{"400.16.1:02.10", 0.08425003},
{"400.16.1:02.15", 0.0634375},
{"400.16.1:02.5", 0.06760002},
{"400.22.1:01.10", 0.100899965},
{"400.22.1:01.15", 0.09181816},
{"400.22.1:01.5", 0.17355454},
{"400.22.1:02.10", 0.16447273},
{"400.22.1:02.15", 0.10443184},
{"400.22.1:02.5", -0.03431816},
{"500.10.1:01.10", 0.12354401},
{"500.10.1:01.15", 0.09720001},
{"500.10.1:01.5", 0.10311999},
{"500.10.1:02.10", 0.09423999},
{"500.10.1:02.15", 0.082400024},
{"500.10.1:02.5", 0.067599975},
{"500.16.1:01.10", 0.100900024},
{"500.16.1:01.15", 0.057609987},
{"500.16.1:01.5", 0.073150024},
{"500.16.1:02.10", 0.067599975},
{"500.16.1:02.15", 0.07426001},
{"500.16.1:02.5", 0.059275024},
{"500.22.1:01.10", 0.12209088},
{"500.22.1:01.15", 0.062756345},
{"500.22.1:01.5", 0.07163635},
{"500.22.1:02.10", 0.16447271},
{"500.22.1:02.15", 0.091818176},
{"500.22.1:02.5", 0.061545473},
{"600.10.1:01.10", 0.21031427},
{"600.10.1:01.15", 0.08980001},
{"600.10.1:01.5", 0.09931427},
{"600.10.1:02.10", 0.08980001},
{"600.10.1:02.15", 0.07253337},
{"600.10.1:02.5", 0.0676},
{"600.16.1:01.10", 0.0865625},
{"600.16.1:01.15", 0.05650004},
{"600.16.1:01.5", 0.07731252},
{"600.16.1:02.10", 0.375625},
{"600.16.1:02.15", 0.062049966},
{"600.16.1:02.5", 0.07037496},
{"600.22.1:01.10", 0.075},
{"600.22.1:01.15", 0.04893178},
{"600.22.1:01.5", 0.06154541},
{"600.22.1:02.10", 0.14479543},
{"600.22.1:02.15", 0.09181814},
{"600.22.1:02.5", 0.0682727},
};


int slot(int n, int start, int inc)
{
    int v;
    n = n + (inc / 2);
    if (start != 0)
    {
        int mod = ((n - start) % inc);
        v = (n - mod);
    }
    else
        v = (n - (n % inc));
    return v;
}

  /*
  * [EVS-SRS-] 
  * 6/15/2020 Craig Cunic   Original development
  *
  * Find volume compensation entry using settings and measured peep as key to pre calculated 
  * list of compensation values
  *
  */

int fakePeep = 2;
  
struct compensation *findCompensation()
{
    int cnt, bot, top, idx;
    char key[64];

    currPeep = vg->getPeep();
    if (settingsChanged == 0 && currPeep == prevPeep)
    {
        return volComp;
    }
    else
    {
        settingsChanged = 0;
        prevPeep = currPeep;
      cnt = sizeof(compensationSet) / sizeof(struct compensation);
  
      int volKey = slot(vol_set, 0, 100);
      int rateKey = slot(rate_set, 10, 6);
      int peepKey = slot(currPeep, 0, 5);
      char ratioKey[12];
      int ratioSlot = (int)(ratio_set + .5F);
      sprintf(ratioKey, "1:%02d", ratioSlot);
      ratioKey[1] = ':';
      sprintf(key, "%d.%d.%s.%d", volKey, rateKey, ratioKey, peepKey);
  
      Serial.printf("compensation key [%s]\n", key);
      bot = 0;
      top = cnt - 1;
      idx = top / 2;
      for (;;)
      {
        //Serial.printf("bot %d idx %d top %d Compare key [%s] to [%s]\n", bot, idx, top, key, compensationSet[idx].key);
        int d = strcmp(key, (char *)compensationSet[idx].key);
        if (d == 0)
        {
            volComp = &compensationSet[idx];          
            return volComp;
        }
        else if (bot == top)
        {
            char raw[64];
            sprintf(raw, "%d.%d.%f.%d", vol_set, rate_set, ratio_set, currPeep);
            Serial.printf("Compensation not found raw [%s] key [%s] using 0\n", raw, key);
            return NULL;
        }
        else if (d < 0)
        {
          if (idx > bot)
            top = idx - 1;
          else
            top = idx;
          idx = (bot + idx) / 2;
        }
        else
        {
          if (idx < top)
            bot = idx + 1;
          else
            bot = idx;
          idx = (idx + top) / 2;
        }
      }
    }
    Serial.printf("AFTER FIND LOOP ?");
    return NULL;
}


void sample(float *airway, float *supply)
{
    *airway = fPressure;
//    *airway = collectFakeAirway();
//    *supply = 51.0F;
    *supply = fSupply;
}


const char *sMoveState(MoveState n)
{
    return ((int)n == 0 ? "IDLE" : "Moving");
}

const char *_sBreathState[] = { "IDLE", "OUT", "RETRACT", "REST" };
const char *sBreathState(BreathState n)
{
    return _sBreathState[(int)n];
}

void changeSettings(int upper, int lower, int vol, int rate, float ratio, int motorFlag)
{
	
    Serial.printf("CHANGE %d %d %d %d %f %s\n", upper, lower, vol, rate, ratio, motorFlag ? "Start" : "Stop");


    settingsChanged = 1;
  	vol_set = vol;
	  rate_set = rate;
	  ratio_set = ratio;

    if (motorFlag && rate)
    {
      Serial.printf("changeSettings() call start\n");
      startBreathing(vol, ratio, rate);
    }
    else
    {
      Serial.printf("changeSettings() call stop\n");
      stopBreathing();
    }
}


void sensor_setup() {
  // put your setup code here, to run once:
  Wire1.begin(I2C_MASTER, 0x00, I2C_PINS_37_38, I2C_PULLUP_EXT, 400000);
  Wire1.setDefaultTimeout(200000); // 200ms
}

void sensor_loop()
{
      /**********  Request a measurment **************/
      //Serial.println("start");

      Wire1.beginTransmission(0x6D);
      Wire1.write(byte(0xA5)); 
      Wire1.endTransmission();   

      Wire1.requestFrom(0x6D, 2);

      uint16_t a5reg = 0;
      uint8_t temp = 0;

      a5reg = Wire1.read() * 256 ;
      a5reg += Wire1.read();

      a5reg = a5reg & 0b0000011111111101;

      Wire1.beginTransmission(0x6D);   
      Wire1.write(byte(0xA5));
      Wire1.write(highByte(a5reg));
      Wire1.write(lowByte(a5reg));
      Wire1.endTransmission();  

      int err;
      if((err = Wire1.getError()))
      {
          Serial.printf((char *)"Request 1 FAIL err %d\n", err);
//          vg->postError((char *)"SENSOR WRITE FAILED");
      }

       
      Wire1.beginTransmission(0x6D);   
      Wire1.write(byte(0x30)); 
      Wire1.write(byte(0x0A));
      Wire1.endTransmission();          

      // Check if error occured
      if((err = Wire1.getError()))
      {
          Serial.printf("Request 2 FAIL err %d\n", err);
//          vg->postError((char *)"SENSOR WRITE FAILED");
      }
      else
          //Serial.print("Request OK\n");

/**********  Measurment  performed by Sensor **************/
      
      delay(5); //required to let sensor do it's thing

/**********  Request Result from Sensor **************/

     Wire1.beginTransmission(0x6D);   
      Wire1.write(byte(0x30)); 
      Wire1.endTransmission();  

     Wire1.requestFrom(0x6D, 1);

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


     temp = Wire1.read();
     if(temp & 0x80) //check sign and push to front. 24 to 32 bit conversion. 
      pressure = 0xFF;
     else 
      pressure = 0; 
     pressure = (pressure << 8) | temp;


    Wire1.beginTransmission(0x6D);   
    Wire1.write(byte(0x07)); 
    Wire1.endTransmission();  
    Wire1.requestFrom(0x6D, 1);

    temp = Wire1.read();
//Serial.printf("temp a %d\n", (int)temp);
    pressure = (pressure << 8) | temp;

    Wire1.beginTransmission(0x6D);   
    Wire1.write(byte(0x08)); 
    Wire1.endTransmission();  

    Wire1.requestFrom(0x6D, 1);

    temp = Wire1.read();
Serial.printf("temp b %d\n", (int)temp);
    pressure = (pressure << 8) | temp;


//    Serial.print(" Data in Pa : ");
    pressure = (pressure * 8192 )/ 4194304;
    fPressure = (float)pressure;
    fPressure /= 98.0665;
Serial.printf("fPressure:%f\n", fPressure);
   
   // Serial.println(pressure, DEC); 
}


void motor_setup() 
{
    move.state = MoveState::IDLE;
    // setup pins
    pinMode(MOTEN, OUTPUT);
    digitalWrite(MOTEN, HIGH);
    pinMode(MOTA, OUTPUT);
    digitalWrite(MOTA, LOW);
    pinMode(MOTB, OUTPUT);
  
    pinMode(KNOB_VOL_P, INPUT_PULLUP);
    pinMode(KNOB_VOL_D, INPUT_PULLUP);
    pinMode(KNOB_VOL_SW, INPUT_PULLUP);

    pinMode(A7, OUTPUT);
  
    move.position = 255;
    breath.update = false;  
}

#define TICKS_PER_MIN 56000

void startBreathing(uint16_t volume, float ratio, uint16_t breaths_min)
{
//  Serial.printf("startBreathing(%d, %f, %d)\n", (int)volume, ratio, (int)breaths_min);
  //convert volume to PWM ticks.
  //TODO, requires measurments of piston chamber and motor settings. passthrough now
    
    uint16_t volume_pwm = constrain(map(volume, 100, 800, 39, 229), 0, 250);

    vol_inc = (double)volume / (double)(255-volume_pwm) / (double)breaths_min;
//Serial.printf("compute inc: %f from vol: %f pwm: %d\n", (float)vol_inc, (float)volume, (int)volume_pwm);

  // zero BPM stops breathing
  if (breaths_min == 0)
  {
    Serial.printf("BPM IS ZERO Breathing Halted\n");
    return;
  }

  //order first move, start state machine
  //if not already running that is
  if(breath.state == BreathState::IDLE)
  {    
//    Serial.printf("BREATH STATE IS IDLE start timer %d bpm ratio %f\n", (int)breaths_min, ratio);
    breath.range = volume_pwm;
    // convert breath/min to period in ms
    breath.period = TICKS_PER_MIN / breaths_min;

    //convert ratio and breaths/min to ramp time
    breath.ramp = (breath.period/2)/ratio; 
    breath.last_start = millis()-breath.ramp;
    breath.state = BreathState::RETRACT;
    
    startMove(255-breath.range, breath.period/2);
    vg->setAirMode(AIR_REFILL);

    //setup timer to change PWM
    BreathTickTimer.begin(serviceBreathing, 1000);
  }
  else{//save settings for next breath. 
    Serial.println("queue parameter change");
    //TODO, requires measurments of piston chamber and motor settings. passthrough now
    breath.range_new = volume_pwm;
  
   //convert breath/min to period in ms
    breath.period_new = TICKS_PER_MIN / breaths_min;

    //convert ratio and breaths/min to ramp time
    breath.ramp_new = (breath.period_new/2)/ratio; 

    //flag update. 
    breath.update = true;
  }
}

void resetInfo()
{
    breath_count = 0;
    total_vol = 0.0;
    start_time = millis();  
    setTime(0);
}

void showInfo()
{
      long now_time = millis() - start_time;
      Serial.printf("%02d:%02d:%02d  bc: %4ld moves: %3d  ms: %7ld range: %d period: %d ramp: %d speed: %d vol: %8.3f\n", 
      hour(), minute(), second(),
      ++breath_count, move_count, now_time, (int)breath.range, (int)breath.period, (int)breath.ramp, (int)move.speed, (float)total_vol);
      move_count = 0;
} 

void stopBreathing()
{
Serial.printf("stopBreathing() was called -----------------------\n");
    resetInfo();
  //stop any current motor movememnts
    MotorTickTimer.end();
    move.state = MoveState::IDLE;
  
//stop the breathing state machine
    breath.state = BreathState::IDLE;
    vg->setAirMode(AIR_IDLE);

  //schedule a move to the rest position, slowly.
  startMove(255, 1000);

}

long sbc = 0;
BreathState pbs = BreathState::IDLE;
int adjustedVolume = 0;
float prevCompensationFactor = 0.0F;

void serviceBreathing()
{
  
  //service the breathing machine, if running
/*  
 if (++sbc % 10000 == 0 || breath.state != pbs)
  Serial.printf("serviceBreathing() cnt %d move state %s breath state %s->%s\n", sbc, sMoveState(move.state), sBreathState(pbs),
  sBreathState(breath.state));
  pbs = breath.state;
*/  
    switch(breath.state)
    {
        case BreathState::IDLE:
          //machine not breathing yet
          break;
        case BreathState::OUT:
          //machine pumping air out
          //check to see if out breath finished, order retract 
          if(move.state == MoveState::IDLE)
          {
             //check for update
              if(breath.update == true)
              {
                Serial.println("apply new parameters");
                resetInfo();
                breath.update = false;
  
                breath.period = breath.period_new;
                breath.range = breath.range_new;
                breath.ramp = breath.ramp_new;
                //fix time keeping to avoid blip
                breath.last_start = millis() - breath.period/2;
              }
  
            //retract back in at least half of period time
            
            float volumeCompensationFactor;

            // volumeCompensationFactor = VOL_ADJ_CONST;
            volComp = findCompensation();

            if (volComp == NULL)
              volumeCompensationFactor = 0.0F;
            else
              volumeCompensationFactor = volComp->val;

            int cvr = vg->computeRange(volumeCompensationFactor, &adjustedVolume);
            if (volumeCompensationFactor != prevCompensationFactor)
            {
              Serial.printf("# Change compensation factor from %f to %f adjusted volume %d\n", prevCompensationFactor, volumeCompensationFactor, adjustedVolume);
              prevCompensationFactor = volumeCompensationFactor;
            }
            
//            startMove(255-breath.range, breath.period/2);
            startMove(255-cvr, breath.period/2);
            breath.state = BreathState::RETRACT;
            vg->setAirMode(AIR_REFILL);
            //showInfo();
          }
  
        break;
        case BreathState::RETRACT:
          //machine retracting piston
          //check to see if retract finished, order rest or next out
          if(move.state == MoveState::IDLE)
          {
            if((millis() - breath.last_start) > breath.period)
            {
              //skip to next out
//Serial.printf("breath time > period\n");  
              
              //schedule move
//Serial.printf("RETRACT\n");
              startMove(255, breath.ramp);
              breath.last_start = millis();
              breath.state = BreathState::OUT;
              vg->setAirMode(AIR_DELIVER);           
            }
            else
            {
//Serial.printf("breath time < period\n");  
              breath.state = BreathState::REST;
			  int bpMin, bpMax, peep, loBounce, hiBounce, bumps, bpSamples;
			  vg->bpRange(&bpMin, &bpMax, &bpSamples);
        vg->getBounce(&loBounce, &hiBounce, &bumps);
        peep = vg->getPeep();
//Serial.printf("%8d: vol: %3d  bpm: %2d  ratio: %2.2f  peep: %2d  lo: %2d  hi: %2d bounce: %2d/%2d bumps: %2d\n", ++cycle_index, vol_set, rate_set, ratio_set, peep, bpMin, bpMax, 
Serial.printf("%8d,%3d,%3d,%2d,%2.2f,%2d,%2d,%2d,%2d,%3d%,%2d,%2d\n", ++cycle_index, vol_set, adjustedVolume, rate_set, ratio_set, peep, bpMin, bpMax, bpSamples, loBounce, hiBounce, bumps);
              vg->setAirMode(AIR_END);
            }
          }
          else
          {
//Serial.printf("BSR MS %d/%s != %d/IDLE ???\n", move.state, sMoveState(move.state), MoveState::IDLE);            
          }
    
  
        break;
        case BreathState::REST:
          //machine resting till next breath
          if((millis() - breath.last_start) > breath.period)
          {
              //schedule move
//Serial.printf("REST\n");
              startMove(255, breath.ramp);
              breath.last_start = millis();
              breath.state = BreathState::OUT;
              vg->setAirMode(AIR_DELIVER);
            }
        break;
    }
}

unsigned char a7out = 0;
char pad[101];
int smc = 0;
MoveState pms = MoveState::IDLE;
void serviceMove()
{
/*
  if (++smc % 10000 == 0 || move.state != pms)
{
  Serial.printf("serviceMove() cnt %d state: %s->%s\n", smc, sMoveState(pms), sMoveState(move.state));
}
  pms = move.state;
*/
  
  //see if we achieved goal, then cancel timer
  if(move.target==move.position)
  {      
//Serial.printf("end of path position: %d target: %d\n", move.position, move.target);
    MotorTickTimer.end();
    move.state = MoveState::IDLE;
    return;
  }

  //else change position

  if(move.direction == MoveDirection::DOWN)
  {
    analogWrite(MOTB,--move.position);
  }
  else
  {
    analogWrite(MOTB,++move.position);
    ++move_count;
    total_vol += vol_inc;
  }

a7out = 255 - move.position;
//int pcnt = map(a7out, 0, 255, 1, 64);
//memset(pad, ' ', pcnt);
//pad[pcnt] = '\0';
//Serial.printf("%s:%d\n", pad, a7out);
      analogWrite(A7, a7out);

    
    //analogWrite(A21,move.position);
 // Serial.println(move.position);

}



void startMove(uint16_t target_pos, uint32_t travel_time)
{    
  //block if recalled during move

//  Serial.printf("startMove(%d, %d) movestate: %s breathstate: %s\n", (int)target_pos, travel_time, sMoveState(move.state), sBreathState(breath.state));
  
  if (move.state != MoveState::IDLE)
  {    
    return;
  }
  
  //validate request TODO
/*
Serial.print("Move : ");
Serial.print(target_pos);
Serial.print(" : ");
Serial.println(travel_time);
*/

  move.speed = 0;
  //convert mS to uS for accuracy in calcs.
  travel_time *= 1000;

  //Serial.print("Speed: ");

  //record direction requested
  if(target_pos < move.position){
    move.direction = MoveDirection::DOWN;
      //calculate speed as in how many ms between single positions to get to goal in requested time
    move.speed = travel_time / (move.position - target_pos); 
  }
  else{
    move.direction = MoveDirection::UP;
     //calculate speed as in how many ms between single positions to get to goal in requested time
    move.speed = travel_time / (target_pos - move.position); 
  }
 
  //setup timer to change PWM
  if (move.speed > 0)
  {
    MotorTickTimer.begin(serviceMove, move.speed);
    //set state
    move.state = MoveState::MOVING;
    move.target = target_pos;
  }  
}

void setup() 
{
  Serial.begin(115200);
  delay(2000); //wait for USB
  Serial.printf("motor init\n");
  motor_setup();  
  Serial.printf("sensor init\n");
  sensor_setup();
  Serial.printf("graph init\n");
  vg = new VentGraph(sample, changeSettings);
  Serial.printf("setup complete\n");
}


int loopCount = 0;
unsigned char tmp[32];
int tx = 0;
int lostep = 0, histep = 0;

void loop()
{
  fPressure = vg->airwayPressure();
  fSupply = vg->supplyPressure();
//  fSupply = 55.0F;
  
  vg->loop();
  delay(10);

  if (Serial.available() > 0) 
  {
     unsigned char ch = Serial.read();
//     Serial.printf("reading ch [%c]\n", ch);
     if (ch == ',')
     {
        Serial.printf("got lo [%s]\n", tmp);
        lostep = atoi((const char *)tmp);
        tx = 0;
     }
     else if (ch == '\n')
     {
      Serial.printf("got hi [%s]\n", tmp);
       histep = atoi((const char *)tmp);
       tx = 0;
     }
     else
     {
        tmp[tx] = ch;
        tmp[++tx] = '\0';
        Serial.printf("store %c at %d\n", ch, tx);
     }
  }
  if (histep != 0)
  {
    Serial.printf("lo %d hi %d\n", lostep, histep);
    lostep = histep = 0;
  }

}
  
