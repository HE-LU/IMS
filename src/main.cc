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

#define GENERATE_CAR_DAY		0.67	//  6:00 - 18:00	// TODO
#define GENERATE_CAR_EVENING	0.98	// 18:00 - 22:00	// TODO
#define GENERATE_CAR_NIGHT		3.125	// 22:00 -  6:00	// TODO
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

#define najezdPrahaBrno(x) \ // 34
	((x < 0.0206457889) ?   0 : ((x < 0.0206457889) ?   1 : ((x < 0.0206457889) ?   2 : ((x < 0,0555657859
) ?   6 : ((x < 0.) ?   8 : ((x < 0.) ?  12 : \
	((x < 0.) ?  15 : ((x < 0.) ?  21 : ((x < 0.) ?  29 : ((x < 0.) ?  34 : ((x < 0.) ?  41 : ((x < 0.) ?  49 : \
	((x < 0.) ?  56 : ((x < 0.) ?  66 : ((x < 0.) ?  81 : ((x < 0.) ?  90 : ((x < 0.) ? 104 : ((x < 0.) ? 112 : \
	((x < 0.) ? 119 : ((x < 0.) ? 134 : ((x < 0.) ? 141 : ((x < 0.) ? 146 : ((x < 0.) ? 153 : ((x < 0.) ? 162 : \
	((x < 0.) ? 168 : ((x < 0.) ? 178 : ((x < 0.) ? 182 : ((x < 0.) ? 190 : ((x < 0.) ? 194 : ((x < 0.) ? 196 : ((x < 0.) ? 201 : 203)))))))))))))))))))))))))))))))

#define sjezdPrahaBrno(x) \ // 33
	((x < 0.) ?   2 : ((x < 0.) ?   6 : ((x < 0.) ?   8 : ((x < 0.) ?  10 : ((x < 0.) ?  15 : ((x < 0.) ?  21 : \
	((x < 0.) ?  29 : ((x < 0.) ?  34 : ((x < 0.) ?  41 : ((x < 0.) ?  49 : ((x < 0.) ?  56 : ((x < 0.) ?  66 : \
	((x < 0.) ?  75 : ((x < 0.) ?  90 : ((x < 0.) ? 104 : ((x < 0.) ? 112 : ((x < 0.) ? 119 : ((x < 0.) ? 134 : \
	((x < 0.) ? 141 : ((x < 0.) ? 146 : ((x < 0.) ? 153 : ((x < 0.) ? 162 : ((x < 0.) ? 168 : ((x < 0.) ? 178 : \
	((x < 0.) ? 182 : ((x < 0.) ? 190 : ((x < 0.) ? 194 : ((x < 0.) ? 196 : ((x < 0.) ? 201 : 203)))))))))))))))))))))))))))))

#define najezdBrnoPraha(x) \ // 31
	((x < 0.) ?   2 : ((x < 0.) ?   7 : ((x < 0.) ?   9 : ((x < 0.) ?  13 : ((x < 0.) ?  21 : ((x < 0.) ?  25 : \
	((x < 0.) ?  35 : ((x < 0.) ?  41 : ((x < 0.) ?  50 : ((x < 0.) ?  57 : ((x < 0.) ?  62 : ((x < 0.) ?  69 : \
	((x < 0.) ?  84 : ((x < 0.) ?  99 : ((x < 0.) ? 113 : ((x < 0.) ? 128 : ((x < 0.) ? 137 : ((x < 0.) ? 147 : \
	((x < 0.) ? 154 : ((x < 0.) ? 162 : ((x < 0.) ? 169 : ((x < 0.) ? 174 : ((x < 0.) ? 182 : ((x < 0.) ? 188 : \
	((x < 0.) ? 193 : ((x < 0.) ? 195 : ((x < 0.) ? 197 : 201)))))))))))))))))))))))))))

#define sjezdBrnoPraha(x) \ // 34
	((x < 0.) ?   0 : ((x < 0.) ?   2 : ((x < 0.) ?   7 : ((x < 0.) ?   9 : ((x < 0.) ?  13 : ((x < 0.) ?  21 : \
	((x < 0.) ?  25 : ((x < 0.) ?  35 : ((x < 0.) ?  41 : ((x < 0.) ?  50 : ((x < 0.) ?  57 : ((x < 0.) ?  62 : \
	((x < 0.) ?  69 : ((x < 0.) ?  84 : ((x < 0.) ?  91 : ((x < 0.) ?  99 : ((x < 0.) ? 113 : ((x < 0.) ? 122 : \
	((x < 0.) ? 137 : ((x < 0.) ? 147 : ((x < 0.) ? 154 : ((x < 0.) ? 162 : ((x < 0.) ? 169 : ((x < 0.) ? 174 : \
	((x < 0.) ? 182 : ((x < 0.) ? 188 : ((x < 0.) ? 191 : ((x < 0.) ? 195 : ((x < 0.) ? 197 : ((x < 0.) ? 201 : 202))))))))))))))))))))))))))))))
// ======== Macros =========
// =========================

// double vyjazd(int km)
// {
// 	if (!isExit(km))           return 0.0; // neni som na vyjazde
// 	if (km < 15)               return 0.4; // Praha
// 	if (km > 185 && km < 202 ) return 0.4; // Brno
// 	if (km == 270)             return 1.0; // koniec
// 	return 0.3;                            // iny vyjazd
// }

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
		// mCurrentPosition = vjazd(Random());
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
		while (Random() > 0.5);
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
	return 0;
}
// ========= MAIN ==========
// =========================