// ╔═════════════════════════════════╗
// ║           Projekt IMS           ║
// ║            Zadani 8             ║
// ║   Model dopravy na dálnici D1   ║
// ╠═════════════════════════════════╣
// ║       xherma25 / xstodu06       ║
// ╚═════════════════════════════════╝


// =========================
// ======= Libraries =======
#include <simlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <iterator>
// ======= Libraries =======
// =========================


// =========================
// ====== Definitions ======
#define DEBUG

#define OUTPUT_FILE				"output.out"
#define SIMULATION_DURATION		1
#define DAY_IN_SECONDS			86400

#define CAR_LENGTH				5.05
// #define CAR_SPACING				72
#define CAR_SPACING				72
#define CAPACITY				round( (1000 / (CAR_LENGTH + CAR_SPACING)) * 2)
#define CAPACITY_LIMITED		CAPACITY / 2
#define HIGHWAY_LENGTH			204

// #define GENERATE_CAR_DAY			0.67	//  6:00 - 18:00	// TODO
// #define GENERATE_CAR_EVENING		0.98	// 18:00 - 22:00	// TODO
// #define GENERATE_CAR_NIGHT		3.125	// 22:00 -  6:00	// TODO
// #define GENERATE_ACCIDENT		40000	// TODO

#define GENERATE_CAR_DAY		1.34		//  6:00 - 18:00	// TODO
#define GENERATE_CAR_EVENING	1.96		// 18:00 - 22:00	// TODO
#define GENERATE_CAR_NIGHT		6.25 		// 22:00 -  6:00	// TODO

// #define GENERATE_CAR_DAY		1		//  6:00 - 18:00	// TODO
// #define GENERATE_CAR_EVENING	2		// 18:00 - 22:00	// TODO
// #define GENERATE_CAR_NIGHT		6		// 22:00 -  6:00	// TODO

#define GENERATE_ACCIDENT		60000

#define PRAHA_BRNO				1
#define BRNO_PRAHA				2

const int EXITS_PRAHA_BRNO[]	= {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 112/*2x*/, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201, 203};
const int EXITS_BRNO_PRAHA[]	= {0, 2, 7/*2x*/, 9/*2x*/, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 91, 99, 113, 122, 137, 147, 154, 162, 169, 174, 182, 188, 191, 195, 197, 201/*2x*/, 202, 203};
const int ENTRIES_PRAHA_BRNO[]	= {0, 1, 2, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201};
const int ENTRIES_BRNO_PRAHA[]	= {0, 2, 7/*2x*/, 9/*2x*/, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 99, 113, 128, 137, 147, 154, 162, 169, 174, 182, 188, 193, 195, 197, 201};
// ====== Definitions ======
// =========================


// =========================
// ======== Macros =========
#define isDay(value)			((long)value%DAY_IN_SECONDS >= 21600 && (long)value%DAY_IN_SECONDS < 64800)
#define isEvening(value)		((long)value%DAY_IN_SECONDS >= 64800 && (long)value%DAY_IN_SECONDS < 79200)
// #define isNight(value)			((long)value%DAY_IN_SECONDS >= 79200 || (long)value%DAY_IN_SECONDS < 21600) // Not needed
#define km2ms(value)			value/3.6
#define ms2km(value)			value*3.6

#define isExitPrahaBrno(km) 	(std::find(std::begin(EXITS_PRAHA_BRNO), std::end(EXITS_PRAHA_BRNO), km) != std::end(EXITS_PRAHA_BRNO))
#define isExitBrnoPraha(km) 	(std::find(std::begin(EXITS_BRNO_PRAHA), std::end(EXITS_BRNO_PRAHA), km) != std::end(EXITS_BRNO_PRAHA))

#define isEntryPrahaBrno(km) 	(std::find(std::begin(ENTRIES_PRAHA_BRNO), std::end(ENTRIES_PRAHA_BRNO), km) != std::end(ENTRIES_PRAHA_BRNO))
#define isEntryBrnoPraha(km) 	(std::find(std::begin(ENTRIES_BRNO_PRAHA), std::end(ENTRIES_BRNO_PRAHA), km) != std::end(ENTRIES_BRNO_PRAHA))

// #define isUnderContruction(km) ((km > 0 && km < 1) || (km > 0 && km < 1)) ? true : false // TODO
#define isUnderContructionPrahaBrno(i) ((i == 38) || (i >= 40  && i <= 50) || (i >= 65 && i <= 77))
#define isUnderContructionBrnoPraha(i) ((i >= 127 && i <= 138 ) || (i >= 153  && i <= 163))

#define najezdPrahaBrno(x) \
((x < 0.4819277108) ?   0 : ((x < 0.4939759036) ?   1 : ((x < 0.5060240964) ?   2 : ((x < 0.5180722892) ?   6 : ((x < 0.5301204819) ?   8 : \
        ((x < 0.5421686747) ?  12 : ((x < 0.5542168675) ?  15 : ((x < 0.5662650602) ?  21 : ((x < 0.578313253) ?  29 : ((x < 0.5903614458) ?  34 : \
        ((x < 0.6024096386) ?  41 : ((x < 0.6144578313) ?  49 : ((x < 0.6265060241) ?  56 : ((x < 0.6385542169) ?  66 : ((x < 0.6506024096) ?  81 : \
        ((x < 0.6626506024) ?  90 : ((x < 0.6746987952) ? 104 : ((x < 0.686746988) ? 112 : ((x < 0.6987951807) ? 119 : ((x < 0.7108433735) ? 134 : \
        ((x < 0.7228915663) ? 141 : ((x < 0.734939759) ? 146 : ((x < 0.7469879518) ? 153 : ((x < 0.7590361446) ? 162 : ((x < 0.7771084337) ? 168 : \
        ((x < 0.8012048193) ? 178 : ((x < 0.8373493976) ? 182 : ((x < 0.8855421687) ? 190 : ((x < 0.9457831325) ? 194 : ((x < 0.9819277108) ? 196 : 201))))))))))))))))))))))))))))))
							

#define sjezdPrahaBrno(x) \
((isExitPrahaBrno(x)==0) ? 0   : ((x==2)   ? 0.4958677686 : ((x==6)   ? 0.03305785124  : ((x==8)   ? 0.03305785124 : ((x==10)  ? 0.03305785124 : \
        ((x==15)  ? 0.02479338843 : ((x==21)  ? 0.02479338843 : ((x==29)  ? 0.02479338843 : ((x==34)  ? 0.02479338843 : ((x==41)  ? 0.02479338843 : \
        ((x==49)  ? 0.02479338843 : ((x==56)  ? 0.02479338843 : ((x==66)  ? 0.02479338843 : ((x==75)  ? 0.01652892562 : ((x==90)  ? 0.01652892562 : \
        ((x==104) ? 0.01652892562 : ((x==112) ? 0.01652892562 : ((x==119) ? 0.01652892562 : ((x==134) ? 0.01652892562 : ((x==141) ? 0.01652892562 : \
        ((x==146) ? 0.01652892562 : ((x==153) ? 0.00826446281 : ((x==162) ? 0.00826446281 : ((x==168) ? 0.00826446281 : ((x==178) ? 0.00826446281 : \
        ((x==182) ? 0.00826446281 : ((x==190) ? 0.00826446281 : ((x==194) ? 0.00826446281 : ((x==196) ? 0.00826446281 : ((x==201) ? 0.00826446281 : 1))))))))))))))))))))))))))))))					
							
#define najezdBrnoPraha(x) \
	((x < 0.0222) ?   0 : ((x < 0.0535) ?   2 : ((x < 0.1414) ?   7 : ((x < 0.2210) ?   9 : ((x < 0.2519) ?  13 : \
	((x < 0.2791) ?  21 : ((x < 0.3058) ?  25 : ((x < 0.3316) ?  35 : ((x < 0.3555) ?  41 : ((x < 0.3805) ?  50 : \
	((x < 0.4044) ?  57 : ((x < 0.4285) ?  62 : ((x < 0.4522) ?  69 : ((x < 0.4763) ?  84 : ((x < 0.5006) ?  99 : \
	((x < 0.5237) ? 113 : ((x < 0.5467) ? 128 : ((x < 0.5707) ? 137 : ((x < 0.5955) ? 147 : ((x < 0.6205) ? 154 : \
	((x < 0.6470) ? 162 : ((x < 0.6743) ? 169 : ((x < 0.7021) ? 174 : ((x < 0.7452) ? 182 : ((x < 0.7938) ? 188 : \
	((x < 0.8402) ? 193 : ((x < 0.8865) ? 195 : ((x < 0.9402) ? 197 : 201))))))))))))))))))))))))))))

#define sjezdBrnoPraha(x) \
	((isExitPrahaBrno(x)==0) ? 0 : ((x==0)   ? 0.0192 : ((x==2)   ? 0.0192 : ((x==7)   ? 0.0541 : ((x==9) ? 0.0759 : \
	((x==13)  ? 0.0344 : ((x==21)  ? 0.0266 : ((x==25)  ? 0.0235 : ((x==35)  ? 0.0230 : ((x==41)  ? 0.0222 : \
	((x==50)  ? 0.0206 : ((x==57)  ? 0.0215 : ((x==62)  ? 0.0206 : ((x==69)  ? 0.0208 : ((x==84)  ? 0.0204 : \
	((x==91)  ? 0.0207 : ((x==99)  ? 0.0207 : ((x==113) ? 0.0199 : ((x==122) ? 0.0199 : ((x==137) ? 0.0198 : \
	((x==147) ? 0.0207 : ((x==154) ? 0.0214 : ((x==162) ? 0.0216 : ((x==169) ? 0.0229 : ((x==174) ? 0.0236 : \
	((x==182) ? 0.0240 : ((x==188) ? 0.0372 : ((x==191) ? 0.0420 : ((x==195) ? 0.0400 : ((x==198) ? 0.0463 : \
	((x==201) ? 0.0926 : ((x==202) ? 0.0516 : ((x==203) ? 0.0516 : 1)))))))))))))))))))))))))))))))))
// ======== Macros =========
// =========================


// =========================
// ======== Global =========
int gDirection = PRAHA_BRNO;
unsigned long gCounterCar = 0;
unsigned long gCounterAccident = 0;

unsigned long gCounterKm1to2 = 0;
unsigned long gCounterKm2to6 = 0;
unsigned long gCounterKm12to15 = 0;
unsigned long gCounterKm41to49 = 0;
unsigned long gCounterKm104to112 = 0;
unsigned long gCounterKm153to162 = 0;
unsigned long gCounterKm194to196 = 0;
// ======== Global =========
// =========================

#define debugKm1to2(km)		if(km == 1)			 	gCounterKm1to2++
#define debugKm2to6(km)		if(km == 4) 			gCounterKm2to6++
#define debugKm12to15(km)	if(km == 14) 			gCounterKm12to15++
#define debugKm41to49(km)	if(km == 45) 			gCounterKm41to49++
#define debugKm104to112(km)	if(km == 107) 			gCounterKm104to112++
#define debugKm153to162(km)	if(km == 158) 			gCounterKm153to162++
#define debugKm194to196(km)	if(km == 195) 			gCounterKm194to196++

// =========================
// ======= Histogram =======
Histogram					hZdrzeni("Doba cakania v kolone v sec", 0, 600, 20);
Histogram					hKilometry("kilometre s kolonou", 0, 1, HIGHWAY_LENGTH);
// ======= Histogram =======
// =========================

// =========================
// ======== Classes ========
class HighwayPart : public Store
{
public:
	double mMaxSpeed;
	bool mExit;

	HighwayPart (int capacity, int maxSpeed, bool exit) : Store(capacity)
	{
		mMaxSpeed = km2ms(maxSpeed);
		mExit = exit;
	};
};
HighwayPart *gHighway[HIGHWAY_LENGTH];

class Car : public Process
{
	double mEntryTime;
	int mCurrentPosition;
	double mCurrentSpeed;

	void Behavior()
	{
		gCounterCar++;
		if (gDirection == PRAHA_BRNO)
			mCurrentPosition = najezdPrahaBrno(Random());
		else
			mCurrentPosition = najezdBrnoPraha(Random());

		mEntryTime = Time;
		gHighway[mCurrentPosition]->Enter(this, 1);

		if (gDirection == PRAHA_BRNO)
			do
			{
				debugKm1to2(mCurrentPosition);
				debugKm2to6(mCurrentPosition);
				debugKm12to15(mCurrentPosition);
				debugKm41to49(mCurrentPosition);
				debugKm104to112(mCurrentPosition);
				debugKm153to162(mCurrentPosition);
				debugKm194to196(mCurrentPosition);

				if ((Time - mEntryTime) != 0)
				{
					hZdrzeni(Time - mEntryTime);
					hKilometry(mCurrentPosition);
				}

				mCurrentSpeed = Normal(1.0, 0);
				mCurrentSpeed = 80 + mCurrentSpeed * 50;
				Wait(1000 / mCurrentSpeed);

				mCurrentPosition++;
				mEntryTime = Time;
				gHighway[mCurrentPosition]->Enter(this, 1);
				gHighway[mCurrentPosition - 1]->Leave(1);
			}
			while (Random() >= sjezdPrahaBrno(mCurrentPosition));
		else
			do
			{
				debugKm1to2(mCurrentPosition);
				debugKm2to6(mCurrentPosition);
				debugKm12to15(mCurrentPosition);
				debugKm41to49(mCurrentPosition);
				debugKm153to162(mCurrentPosition);
				debugKm194to196(mCurrentPosition);

				if ((Time - mEntryTime) != 0)
				{
					hZdrzeni(Time - mEntryTime);
					hKilometry(mCurrentPosition);
				}

				mCurrentSpeed = Normal(1.0, 0);
				mCurrentSpeed = 80 + mCurrentSpeed * 50;
				Wait(1000 / mCurrentSpeed);

				mCurrentPosition++;
				mEntryTime = Time;
				gHighway[mCurrentPosition]->Enter(this, 1);
				gHighway[mCurrentPosition - 1]->Leave(1);
			}
			while (Random() > sjezdBrnoPraha(mCurrentPosition));

		gHighway[mCurrentPosition]->Leave(1);
		Terminate();
	}
};

class Accident : public Process
{
	void Behavior()
	{
		gCounterAccident++;

		int km = Uniform(0, HIGHWAY_LENGTH - 1);
		double dangerous = Random();
		double spd = gHighway[km]->mMaxSpeed;
		gHighway[km]->mMaxSpeed = spd * dangerous;
		double waiting = Exponential(1000 / (dangerous * dangerous));
		Wait(waiting);
		gHighway[km]->mMaxSpeed = spd;
	}
};

class GeneratorAccident : public Event
{
	void Behavior()
	{
		(new Accident)->Activate();
		Activate(Time + Exponential(GENERATE_ACCIDENT));
	}
};


class GeneratorCar : public Event
{
	void Behavior()
	{
		(new Car)->Activate();

		if (isDay(Time))
			Activate(Time + Exponential(GENERATE_CAR_DAY));
		else if (isEvening(Time))
			Activate(Time + Exponential(GENERATE_CAR_EVENING));
		else
			Activate(Time + Exponential(GENERATE_CAR_NIGHT));
	}
};

class GeneratorProgress : public Event
{
	void Behavior()
	{
		if(Time == 0)
			std::cout << "Progress: ";
		std::cout << "䷀" << std::flush;

		Activate(Time + 3600);
	}
};
// ======== Classes ========
// =========================


// =========================
// ======= Functions =======
void initHighway()
{
	HighwayPart *tmp;

	for (int i = 0; i < HIGHWAY_LENGTH; i++)
	{
		bool exit = false;
		int maxSpeed = 130;
		int capacity = CAPACITY;

		if (gDirection == PRAHA_BRNO)
		{
			if (isExitPrahaBrno(i))
				exit = true;

			if (isUnderContructionPrahaBrno(i))
			{
				maxSpeed = 90;
				capacity = CAPACITY_LIMITED;
			}
		}
		else
		{
			if (isExitBrnoPraha(i))
				exit = true;

			if (isUnderContructionBrnoPraha(i))
			{
				maxSpeed = 90;
				capacity = CAPACITY_LIMITED;
			}
		}

		tmp = new HighwayPart(capacity, maxSpeed, exit);
		gHighway[i] = tmp;
	}
}

void destroyHighway()
{
	for (int i = 0; i < HIGHWAY_LENGTH; i++)
		delete gHighway[i];
}
// ======= Functions =======
// =========================


// =========================
// ========= MAIN ==========
int main()
{
	SetOutput(OUTPUT_FILE);
	RandomSeed ( time(NULL) );

	gDirection = PRAHA_BRNO;
	initHighway();
	Init(0, SIMULATION_DURATION * DAY_IN_SECONDS);
	(new GeneratorProgress)->Activate();
	(new GeneratorCar)->Activate();
	(new GeneratorAccident)->Activate(Time + Exponential(GENERATE_ACCIDENT));

	Run();

	destroyHighway();

	_Print("\n\nDalnici projelo %d aut\n", gCounterCar);
	_Print("Na dalnici se staly %d nehody\n\n", gCounterAccident);


	// _Print("Na km   1 -   2 projelo %d aut. Mělo by projet 88 460\n",gCounterKm1to2);
	// _Print("Na km   2 -   6 projelo %d aut. Mělo by projet 79 360\n",gCounterKm2to6);
	// _Print("Na km  12 -  15 projelo %d aut. Mělo by projet 73 397\n",gCounterKm12to15);
	// _Print("Na km  41 -  49 projelo %d aut. Mělo by projet 37 054\n",gCounterKm41to49);
	// _Print("Na km 153 - 162 projelo %d aut. Mělo by projet 35 357\n",gCounterKm153to162);
	// _Print("Na km 194 - 196 projelo %d aut. Mělo by projet 65 016\n",gCounterKm194to196);

	_Print("Na km   1 -   2 projelo %d aut. Mělo by projet 34 499\n", gCounterKm1to2);
	_Print("Na km   2 -   6 projelo %d aut. Mělo by projet 24 007\n", gCounterKm2to6);
	_Print("Na km  12 -  15 projelo %d aut. Mělo by projet 22 753\n", gCounterKm12to15);
	_Print("Na km  41 -  49 projelo %d aut. Mělo by projet 11 487\n", gCounterKm41to49);
	_Print("Na km 104 - 112 projelo %d aut. Mělo by projet 14 596\n", gCounterKm104to112);
	_Print("Na km 153 - 162 projelo %d aut. Mělo by projet 15 204\n", gCounterKm153to162);
	_Print("Na km 194 - 196 projelo %d aut. Mělo by projet 30 558\n\n", gCounterKm194to196);

	// hZdrzeni.Output();
	// hKilometry.Output();

	return 0;
}
// ========= MAIN ==========
// =========================