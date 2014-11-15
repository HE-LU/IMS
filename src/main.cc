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

#define CAPACITY				50		// TODO
#define CAPACITY_LIMITED		50		// TODO
#define HIGHWAY_LENGTH			203

#define GENERATE_CAR_DAY		0.22	//  6:00 - 18:00	// TODO
#define GENERATE_CAR_EVENING	0.3		// 18:00 - 22:00	// TODO
#define GENERATE_CAR_NIGHT		0.5		// 22:00 -  6:00	// TODO
#define GENERATE_ACCIDENT		40000	// TODO

#define PRAHA_BRNO				1
#define BRNO_PRAHA				2
#define LENGTH_CAR_SMALL		4
#define LENGTH_CAR_MEDIUM		4
#define LENGTH_CAR_LARGE		4

const int EXITS_PRAHA_BRNO[] = {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 112/*2x*/, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201};
const int EXITS_BRNO_PRAHA[] = {1, 2/*2x*/, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201, 203};
const int ENTRIES_PRAHA_BRNO[] = {1, 2, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201, 203};
const int ENTRIES_BRNO_PRAHA[] = {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194/*2x*/, 196/*2x*/, 201};

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
#define isRepair(km) ((km > 0 && km < 1) || (km > 0 && km < 1)) ? true : false // TODO
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
	HighwayPart (const char *name, int size, int speed, bool exit) : Store(name, size)
	{
		Speed = km2ms(speed);
		Exit = exit;
	};

	double Speed;
	bool Exit;
};
HighwayPart *highway[HIGHWAY_LENGTH];

class Car : public Process
{
	double in;
	int km;
	double spd;

	void Behavior()
	{

	}
};

class Accident : public Process
{
	void Behavior()
	{
		if (Time == 0.0) return;

		gCounterAccident++;

		int km = Uniform(0, HIGHWAY_LENGTH - 1);
		double dangerous = Random();
		double spd = highway[km]->Speed;
		highway[km]->Speed = spd * dangerous;
		double waiting = Exponential(1000 / (dangerous * dangerous));
		Wait(waiting);
		highway[km]->Speed = spd;
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
		bool e = false;
		int step  = 130;
		int capacity  = CAPACITY;

		if (isExit(i, BRNO_PRAHA))
			e = true;
		if (isRepair(i))
		{
			step = 90;
			capacity = CAPACITY_LIMITED;
		}

		tmp = new HighwayPart("kilometr", capacity, step, e);
		highway[i] = tmp;
	}
}

void destroyHighway()
{
	for (int i = 0; i < HIGHWAY_LENGTH; i++)
		delete highway[i];
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
	(new GeneratorAccident)->Activate();

	Run();

	destroyHighway();
	return 0;
}
// ========= MAIN ==========
// =========================