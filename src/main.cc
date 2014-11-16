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
#define OUTPUT_FILE				"output.out"
#define SIMULATION_DURATION		1
#define DAY_IN_SECONDS			86400

#define CAR_LENGTH				5.05
#define CAR_SPACING				72
#define CAPACITY				round( (1000 / (CAR_LENGTH + CAR_SPACING)) * 2)
#define CAPACITY_LIMITED		CAPACITY / 2
#define HIGHWAY_LENGTH			203

// #define GENERATE_CAR_DAY		0.67	//  6:00 - 18:00	// TODO
// #define GENERATE_CAR_EVENING	0.98	// 18:00 - 22:00	// TODO
// #define GENERATE_CAR_NIGHT		3.125	// 22:00 -  6:00	// TODO
// #define GENERATE_ACCIDENT		40000	// TODO

#define GENERATE_CAR_DAY		0.1	//  6:00 - 18:00	// TODO
#define GENERATE_CAR_EVENING	0.25	// 18:00 - 22:00	// TODO
#define GENERATE_CAR_NIGHT		1	// 22:00 -  6:00	// TODO
#define GENERATE_ACCIDENT		40000	// TODO

#define PRAHA_BRNO				1
#define BRNO_PRAHA				2

const int EXITS_PRAHA_BRNO[] =   {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 112/*2x*/, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201};
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

#define isExit(km,direction) (direction == PRAHA_BRNO) ? \
	(std::find(std::begin(EXITS_PRAHA_BRNO), std::end(EXITS_PRAHA_BRNO), km) != std::end(EXITS_PRAHA_BRNO)) : \
	(std::find(std::begin(EXITS_BRNO_PRAHA), std::end(EXITS_BRNO_PRAHA), km) != std::end(EXITS_BRNO_PRAHA))
#define isEntry(km,direction) (direction == PRAHA_BRNO) ? \
	(std::find(std::begin(ENTRIES_PRAHA_BRNO), std::end(ENTRIES_PRAHA_BRNO), km) != std::end(ENTRIES_PRAHA_BRNO)) : \
	(std::find(std::begin(ENTRIES_BRNO_PRAHA), std::end(ENTRIES_BRNO_PRAHA), km) != std::end(ENTRIES_BRNO_PRAHA))
#define isUnderContruction(km) ((km > 0 && km < 1) || (km > 0 && km < 1)) ? true : false // TODO

#define sjezdPrahaBrno(x) \
	((x < 0.0570318181583739) ?   2 : ((x < 0.108196695945493) ?   6 : ((x < 0.152417953095424) ?   8 : ((x < 0.196639210245354) ?  10 : ((x < 0.243038971957314) ?  15 : \
	((x < 0.284153974304091)  ?  21 : ((x < 0.310655782095387) ?  29 : ((x < 0.336719181155645) ?  34 : ((x < 0.362012141342975) ?  41 : ((x < 0.385901548872258) ?  49 : \
	((x < 0.409560147099024)  ?  56 : ((x < 0.432472161045579) ?  66 : ((x < 0.454353914474193) ?  75 : ((x < 0.476382019052728) ?  90 : ((x < 0.499637023359449) ? 104 : \
	((x < 0.545542285811546)  ? 112 : ((x < 0.568494272318873) ? 119 : ((x < 0.591125833621308) ? 134 : ((x < 0.614142292000846) ? 141 : ((x < 0.636965334763749) ? 146 : \
	((x < 0.660812190857373)  ? 153 : ((x < 0.683607510715225) ? 162 : ((x < 0.708176451777618) ? 168 : ((x < 0.733624789176978) ? 178 : ((x < 0.759594704022529) ? 182 : \
	((x < 0.789063507373003)  ? 190 : ((x < 0.865029424962477) ? 194 : ((x < 0.948863489836654) ? 196 : ((x < 0.978778438542832) ? 201 : 203)))))))))))))))))))))))))))))

#define najezdPrahaBrno(x) \
	((x < 0.0544595447339664) ?   0 : ((x < 0.108919089467933) ?   1 : ((x < 0.157776307081403) ?   2 : ((x < 0.200003078201715) ?   6 : ((x < 0.242229849322026) ?   8 : \
	((x < 0.286536869161036)  ?  12 : ((x < 0.325797485109199) ?  15 : ((x < 0.351103997044926) ?  21 : ((x < 0.375991873547474) ?  29 : ((x < 0.400144059840241) ?  34 : \
	((x < 0.42295599710649)   ?  41 : ((x < 0.445547535129977) ?  49 : ((x < 0.467426161636372) ?  56 : ((x < 0.488320994874794) ?  66 : ((x < 0.509355578471057) ?  81 : \
	((x < 0.531561725639881)  ?  90 : ((x < 0.55347913748788)  ? 104 : ((x < 0.575395933695535) ? 112 : ((x < 0.597006756652764) ? 119 : ((x < 0.61898511689471)  ? 134 : \
	((x < 0.640778785033783)  ? 141 : ((x < 0.6635500900374)   ? 146 : ((x < 0.685317285641728) ? 153 : ((x < 0.708778107829406) ? 162 : ((x < 0.733078663444816) ? 168 : \
	((x < 0.757877272097641)  ? 178 : ((x < 0.786016960891447) ? 182 : ((x < 0.822286796053745) ? 190 : ((x < 0.902339741123236) ? 194 : ((x < 0.959471164945439) ? 196 : \
	((x < 0.97973558247272)   ? 201 : 203)))))))))))))))))))))))))))))))

#define najezdBrnoPraha(x) \
	((x < 0.022242614514056) ?   0 : ((x < 0.053596889439691) ?   2 : ((x < 0.141464541964048) ?   7 : ((x < 0.221085479592016) ?   9 : ((x < 0.251972143348501) ?  13 : \
	((x < 0.279191573268318) ?  21 : ((x < 0.305864330592988) ?  25 : ((x < 0.331615380353033) ?  35 : ((x < 0.355507472994103) ?  41 : ((x < 0.380501695428358) ?  50 : \
	((x < 0.404422844832829) ?  57 : ((x < 0.428546715842423) ?  62 : ((x < 0.452267170857823) ?  69 : ((x < 0.476323467999043) ?  84 : ((x < 0.500697362321622) ?  99 : \
	((x < 0.523785325929039) ? 113 : ((x < 0.546719896855247) ? 128 : ((x < 0.570734298198075) ? 137 : ((x < 0.595531204936677) ? 147 : ((x < 0.620570026124058) ? 154 : \
	((x < 0.647079930425945) ? 162 : ((x < 0.674397342454905) ? 169 : ((x < 0.702174256788809) ? 174 : ((x < 0.745267464128412) ? 182 : ((x < 0.79389970145865)  ? 188 : \
	((x < 0.840248617776522) ? 193 : ((x < 0.886597534094395) ? 195 : ((x < 0.940224156036171) ? 197 : 201))))))))))))))))))))))))))))

#define sjezdBrnoPraha(x) \
	((x < 0.0192204382699036) ?   0 : ((x < 0.0384408765398073) ?   2 : ((x < 0.0926290121409452) ?   7 : ((x < 0.16855780145654)  ?   9 : ((x < 0.202959092629012) ?  13 : \
	((x < 0.229649085108073)  ?  21 : ((x < 0.253170122717442)  ?  25 : ((x < 0.276218765911926)  ?  35 : ((x < 0.298470936940893) ?  41 : ((x < 0.319116733389701) ?  50 : \
	((x < 0.340714909678655)  ?  57 : ((x < 0.361385814854089)  ?  62 : ((x < 0.382231897192027)  ?  69 : ((x < 0.402729376976582) ?  84 : ((x < 0.423517066927019) ?  91 : \
	((x < 0.44430534080133)   ?  99 : ((x < 0.464256267838874)  ? 113 : ((x < 0.484207194876418)  ? 122 : ((x < 0.504025571194335) ? 137 : ((x < 0.52477705786452)  ? 147 : \
	((x < 0.54620472838197)   ? 154 : ((x < 0.567841443646675)  ? 162 : ((x < 0.590749361187281)  ? 169 : ((x < 0.614355067758526) ? 174 : ((x < 0.63835784256478)  ? 182 : \
	((x < 0.675595835922062)  ? 188 : ((x < 0.71762025328282)   ? 191 : ((x < 0.757671591869911)  ? 195 : ((x < 0.804011790590884) ? 198 : ((x < 0.896692188032831) ? 201 : \
	((x < 0.948346094016415)  ? 202 : 203))))))))))))))))))))))))))))))) 
// ======== Macros =========
// =========================


// =========================
// ======== Global =========
unsigned long gCounterCar = 0;
unsigned long gCounterAccident = 0;
// ======== Global =========
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
			if ((Time - mEntryTime) != 0)
			{
				// Zdrzanie(Time - mEntryTime);
				// Kilometer(mCurrentPosition);
			}

			mCurrentSpeed = Normal(1.0, 0);
			mCurrentSpeed = 80 + mCurrentSpeed * 50;
			// mCurrentSpeed = (mCurrentSpeed < 0.7) ? 0.7 * gHighway[mCurrentPosition]->mMaxSpeed : mCurrentSpeed * gHighway[mCurrentPosition]->mMaxSpeed;
			Wait(1000 / mCurrentSpeed);

			mCurrentPosition++;
			mEntryTime = Time;
			gHighway[mCurrentPosition]->Enter(this, 1);
			gHighway[mCurrentPosition - 1]->Leave(1);
		}
		while (Random() > sjezdPrahaBrno(mCurrentPosition));
		// while (Random() > vyjazd(mCurrentPosition));

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

		if (isExit(i, BRNO_PRAHA))
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

	return 0;
}
// ========= MAIN ==========
// =========================