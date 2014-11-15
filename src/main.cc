// ╔═════════════════════════════════╗
// ║           Projekt IMS           ║
// ║            Zadani 8             ║
// ║   Model dopravy na dálnici D1   ║
// ╠═════════════════════════════════╣
// ║       xherma25 / xstodu06       ║
// ╚═════════════════════════════════╝

#include <simlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <iterator>

#define DEBUG
#define INOUT
#define SIMULATION_DURATION			1
#define OUTPUT_FILE					"output.out"

#define GENERATE_CAR_DAY			0.22		//  6:00 - 18:00	// TODO
#define GENERATE_CAR_EVENING		0.3			// 18:00 - 22:00	// TODO
#define GENERATE_CAR_NIGHT			0.5			// 22:00 -  6:00	// TODO

#define CAPACITY					50			// TODO
#define CAPACITY_LIMITED			50			// TODO

#define SECONDS_PER_DAY				86400		// sekundy za den
#define toms(speed)					speed/3.6	// km/h -> m/s
#define tokmh(speed)				speed*3.6	// m/s  -> km/h

// denni doby
#define isDay()			((long)Time%SECONDS_PER_DAY >= 21600 && (long)Time%SECONDS_PER_DAY < 64800)
#define isEvening()		((long)Time%SECONDS_PER_DAY >= 64800 && (long)Time%SECONDS_PER_DAY < 79200)
#define isNight()		((long)Time%SECONDS_PER_DAY >= 79200 || (long)Time%SECONDS_PER_DAY < 21600)

// dlzka dialnice
#define HIGHWAY_LENGTH				203

#define PRAHA_BRNO					1
#define BRNO_PRAHA					2


// =========================
// ========= Exits =========

const int EXITS_PRAHA_BRNO[] = {0,1,2,10,12,15,21,29,34,41,49,56,66,75,81,90,104,112,119,134,141,146,153,162,168,178,182,190,194,196,201,203};
const int EXITS_BRNO_PRAHA[] = {0,1,2,10,12,15,21,29,34,41,49,56,66,75,81,90,104,112,119,134,141,146,153,162,168,178,182,190,194,196,201,203};

bool isExit(int km, int direction)
{
	bool exists;
	if(direction == PRAHA_BRNO)
		exists = std::find(std::begin(EXITS_PRAHA_BRNO), std::end(EXITS_PRAHA_BRNO), km) != std::end(EXITS_PRAHA_BRNO);
	else
		exists = std::find(std::begin(EXITS_BRNO_PRAHA), std::end(EXITS_BRNO_PRAHA), km) != std::end(EXITS_BRNO_PRAHA);	
	return exists;
}

// ========= Exits =========
// =========================



// =========================
// ========= MAIN ==========

int main() {
  SetOutput(OUTPUT_FILE);
  
  RandomSeed ( time(NULL) );
  
  return 0;
}

// ========= MAIN ==========
// =========================
