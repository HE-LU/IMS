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
#define HIGHWAY_LENGTH			203

// #define GENERATE_CAR_DAY			0.67	//  6:00 - 18:00	// TODO
// #define GENERATE_CAR_EVENING		0.98	// 18:00 - 22:00	// TODO
// #define GENERATE_CAR_NIGHT		3.125	// 22:00 -  6:00	// TODO
// #define GENERATE_ACCIDENT		40000	// TODO

#define GENERATE_CAR_DAY		1.1		//  6:00 - 18:00	// TODO
#define GENERATE_CAR_EVENING	1.2		// 18:00 - 22:00	// TODO
#define GENERATE_CAR_NIGHT		3 		// 22:00 -  6:00	// TODO
#define GENERATE_ACCIDENT		40000	// TODO

#define PRAHA_BRNO				1
#define BRNO_PRAHA				2

const int EXITS_PRAHA_BRNO[] =   {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 112/*2x*/, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201,202};
const int EXITS_BRNO_PRAHA[] =   {0, 2, 7/*2x*/, 9/*2x*/, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 91, 99, 113, 122, 137, 147, 154, 162, 169, 174, 182, 188, 191, 195, 197, 201/*2x*/, 202};
const int ENTRIES_PRAHA_BRNO[] = {1, 2, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201, 203};
const int ENTRIES_BRNO_PRAHA[] = {2, 7/*2x*/, 9/*2x*/, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 99, 113, 128, 137, 147, 154, 162, 169, 174, 182, 188, 193, 195, 197, 201};
// const int EXITS_BRNO_PRAHA[] =   {1, 2/*2x*/, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201, 203};
// const int ENTRIES_BRNO_PRAHA[] = {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201};
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

#define isUnderContruction(km) ((km > 0 && km < 1) || (km > 0 && km < 1)) ? true : false // TODO

#define najezdPrahaBrno(x) \
	((x < 0.0544595447339664) ?   0 : ((x < 0.108919089467933) ?   1 : ((x < 0.157776307081403) ?   2 : ((x < 0.200003078201715) ?   6 : ((x < 0.242229849322026) ?   8 : \
	((x < 0.286536869161036)  ?  12 : ((x < 0.325797485109199) ?  15 : ((x < 0.351103997044926) ?  21 : ((x < 0.375991873547474) ?  29 : ((x < 0.400144059840241) ?  34 : \
	((x < 0.42295599710649)   ?  41 : ((x < 0.445547535129977) ?  49 : ((x < 0.467426161636372) ?  56 : ((x < 0.488320994874794) ?  66 : ((x < 0.509355578471057) ?  81 : \
	((x < 0.531561725639881)  ?  90 : ((x < 0.55347913748788)  ? 104 : ((x < 0.575395933695535) ? 112 : ((x < 0.597006756652764) ? 119 : ((x < 0.61898511689471)  ? 134 : \
	((x < 0.640778785033783)  ? 141 : ((x < 0.6635500900374)   ? 146 : ((x < 0.685317285641728) ? 153 : ((x < 0.708778107829406) ? 162 : ((x < 0.733078663444816) ? 168 : \
	((x < 0.757877272097641)  ? 178 : ((x < 0.786016960891447) ? 182 : ((x < 0.822286796053745) ? 190 : ((x < 0.902339741123236) ? 194 : ((x < 0.959471164945439) ? 196 : \
	((x < 0.97973558247272)   ? 201 : 203)))))))))))))))))))))))))))))))

#define sjezdPrahaBrno(x) \
	((isExitPrahaBrno(x)==0) ? 0   : ((x==2)   ? 0.0570318181583739 : ((x==6)   ? 0.051164877787119  : ((x==8)   ? 0.0442212571499306 : ((x==10)  ? 0.0442212571499306 : \
	((x==15) ? 0.0463997617119603  : ((x==21)  ? 0.0411150023467761 : ((x==29)  ? 0.0265018077912968 : ((x==34)  ? 0.026063399060258  : ((x==41)  ? 0.0252929601873295 : \
	((x==49)  ? 0.0238894075292831 : ((x==56)  ? 0.0236585982267656 : ((x==66)  ? 0.0229120139465554 : ((x==75)  ? 0.0218817534286142 : ((x==90)  ? 0.0220281045785345 : \
	((x==104) ? 0.0232550043067211 : ((x==112) ? 0.0459052624520974 : ((x==119) ? 0.0229519865073266 : ((x==134) ? 0.022631561302435  : ((x==141) ? 0.0230164583795382 : \
	((x==146) ? 0.0228230427629034 : ((x==153) ? 0.0238468560936235 : ((x==162) ? 0.0227953198578524 : ((x==168) ? 0.0245689410623933 : ((x==178) ? 0.0254483373993594 : \
	((x==182) ? 0.0259699148455512 : ((x==190) ? 0.0294688033504743 : ((x==194) ? 0.0759659175894741  : ((x==196) ? 0.0838340648741767 : ((x==201) ? 0.0299149487061785 : 1))))))))))))))))))))))))))))))

#define najezdBrnoPraha(x) \
	((x < 0.022242614514056) ?   0 : ((x < 0.053596889439691) ?   2 : ((x < 0.141464541964048) ?   7 : ((x < 0.221085479592016) ?   9 : ((x < 0.251972143348501) ?  13 : \
	((x < 0.279191573268318) ?  21 : ((x < 0.305864330592988) ?  25 : ((x < 0.331615380353033) ?  35 : ((x < 0.355507472994103) ?  41 : ((x < 0.380501695428358) ?  50 : \
	((x < 0.404422844832829) ?  57 : ((x < 0.428546715842423) ?  62 : ((x < 0.452267170857823) ?  69 : ((x < 0.476323467999043) ?  84 : ((x < 0.500697362321622) ?  99 : \
	((x < 0.523785325929039) ? 113 : ((x < 0.546719896855247) ? 128 : ((x < 0.570734298198075) ? 137 : ((x < 0.595531204936677) ? 147 : ((x < 0.620570026124058) ? 154 : \
	((x < 0.647079930425945) ? 162 : ((x < 0.674397342454905) ? 169 : ((x < 0.702174256788809) ? 174 : ((x < 0.745267464128412) ? 182 : ((x < 0.79389970145865)  ? 188 : \
	((x < 0.840248617776522) ? 193 : ((x < 0.886597534094395) ? 195 : ((x < 0.940224156036171) ? 197 : 201))))))))))))))))))))))))))))

#define sjezdBrnoPraha(x) \
	((isExitPrahaBrno(x)==0) ? 0   : ((x==0)   ? 0.0192204382699036 : ((x==2)   ? 0.0192204382699036 : ((x==7)   ? 0.0541881356011379  : ((x==9)   ? 0.0759287893155945 : \
	((x==13)  ? 0.0344012911724724 : ((x==21)  ? 0.0266899924790605 : ((x==25)  ? 0.0235210376093689 : ((x==35)  ? 0.023048643194484  : ((x==41)  ? 0.0222521710289673 : \
	((x==50)  ? 0.0206457964488086 : ((x==57)  ? 0.0215981762889536 : ((x==62)  ? 0.0206709051754341 : ((x==69)  ? 0.0208460823379378 : ((x==84)  ? 0.0204974797845554 : \
	((x==91)  ? 0.0207876899504365 : ((x==99)  ? 0.0207882738743116 : ((x==113) ? 0.019950927037544  : ((x==122) ? 0.019950927037544  : ((x==137) ? 0.0198183763179162 : \
	((x==147) ? 0.0207514866701858 : ((x==154) ? 0.02142767051745   : ((x==162) ? 0.0216367152647044 : ((x==169) ? 0.0229079175406061 : ((x==174) ? 0.0236057065712457 : \
	((x==182) ? 0.0240027748062541 : ((x==188) ? 0.037237993357282  : ((x==191) ? 0.0420244173607575 : ((x==195) ? 0.0400513385870911 : ((x==198) ? 0.0463401987209731 : \
	((x==201) ? 0.0926803974419463 : ((x==202) ? 0.0516539059835847 : ((x==203) ? 0.0516539059835847 : 1)))))))))))))))))))))))))))))))))

// ======== Macros =========
// =========================


// =========================
// ======== Global =========
unsigned long gCounterCar = 0;
unsigned long gCounterAccident = 0;

unsigned long gCounterKm1to2 = 0;
unsigned long gCounterKm2to6 = 0;
unsigned long gCounterKm12to15 = 0;
unsigned long gCounterKm41to49 = 0;
unsigned long gCounterKm153to162 = 0;
unsigned long gCounterKm194to196 = 0;
// ======== Global =========
// =========================

#define debugKm1to2(km)		if(km >= 1 && km <= 2) 	gCounterKm1to2++
#define debugKm2to6(km)		if(km == 4) 			gCounterKm2to6++
#define debugKm12to15(km)	if(km == 14) 			gCounterKm12to15++
#define debugKm41to49(km)	if(km == 45) 			gCounterKm41to49++
#define debugKm153to162(km)	if(km == 158) 			gCounterKm153to162++
#define debugKm194to196(km)	if(km == 195) 			gCounterKm194to196++

// =========================
// ======= Histogram =======
Histogram		hZdrzeni("Doba cakania v kolone v sec", 0, 600, 20); 
Histogram		hKilometry("kilometre s kolonou",0,1,HIGHWAY_LENGTH);

// Histogram		hKm1to2("KM 1 - 2",0,1,1000000);
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
		mCurrentPosition = najezdPrahaBrno(Random());
		mEntryTime = Time;
		gHighway[mCurrentPosition]->Enter(this, 1);
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
		while (Random() > sjezdPrahaBrno(mCurrentPosition));

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
		int maxSpeed  = 130;
		int capacity  = CAPACITY;

		if (isExitPrahaBrno(i))
			exit = true;

		if (isUnderContruction(i))
		{
			maxSpeed = 90;
			capacity = CAPACITY_LIMITED;
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

	initHighway();
	Init(0, SIMULATION_DURATION * DAY_IN_SECONDS);
	(new GeneratorCar)->Activate();
	(new GeneratorAccident)->Activate(Time + Exponential(GENERATE_ACCIDENT));

	Run();

	destroyHighway();

	_Print("Dialnicou preslo %d osobnych aut\n",gCounterCar);
  	_Print("Na dialnici sa stalo %d nehod\n\n",gCounterAccident);


  	_Print("Na km   1 -   2 projelo %d aut. Mělo by projet 88 460\n",gCounterKm1to2);
  	_Print("Na km   2 -   6 projelo %d aut. Mělo by projet 79 360\n",gCounterKm2to6);
  	_Print("Na km  12 -  15 projelo %d aut. Mělo by projet 73 397\n",gCounterKm12to15);
  	_Print("Na km  41 -  49 projelo %d aut. Mělo by projet 37 054\n",gCounterKm41to49);
  	_Print("Na km 153 - 162 projelo %d aut. Mělo by projet 35 357\n",gCounterKm153to162);
  	_Print("Na km 194 - 196 projelo %d aut. Mělo by projet 65 016\n",gCounterKm194to196); 	

	// hZdrzeni.Output();
	// hKilometry.Output();

	return 0;
}
// ========= MAIN ==========
// =========================