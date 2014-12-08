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
#include <cstring>
// ======= Libraries =======
// =========================


// =========================
// ====== Definitions ======
#define DEBUG

#define OUTPUT_FILE             "output.out"
#define SIMULATION_DURATION     1
#define DAY_IN_SECONDS          86400

#define CAPACITY				140 // CAR_LENGTH = 5.1m
#define CAPACITY_LIMITED        70
#define HIGHWAY_LENGTH          204

#define GENERATE_CAR_DAY        0.31768        //  6:00 - 18:00 Kolem 180k aut denne
#define GENERATE_CAR_EVENING    0.528        // 18:00 - 22:00
#define GENERATE_CAR_NIGHT      1.595       // 22:00 -  6:00  // Brno-Praha*/

#define GENERATE_ACCIDENT       60000

#define PRAHA_BRNO              1
#define BRNO_PRAHA              2

const int EXITS_PRAHA_BRNO[]    = {2, 6, 8, 10, 15, 21, 29, 34, 41, 49, 56, 66, 75, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194, 196, 201, 203};
const int EXITS_BRNO_PRAHA[]    = {0, 2, 7, 9, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 91, 99, 113, 122, 137, 147, 154, 162, 169, 174, 182, 188, 191, 195, 198, 201, 202, 203};
const int ENTRIES_PRAHA_BRNO[]  = {0, 1, 2, 6, 8, 12, 15, 21, 29, 34, 41, 49, 56, 66, 81, 90, 104, 112, 119, 134, 141, 146, 153, 162, 168, 178, 182, 190, 194, 196, 201};
const int ENTRIES_BRNO_PRAHA[]  = {0, 2, 7, 9, 13, 21, 25, 35, 41, 50, 57, 62, 69, 84, 99, 113, 128, 137, 147, 154, 162, 169, 174, 182, 188, 193, 195, 197, 201};
// ====== Definitions ======
// =========================


// =========================
// ======== Macros =========
#define isDay(value)            ((long)value%DAY_IN_SECONDS >= 21600 && (long)value%DAY_IN_SECONDS < 64800)
#define isEvening(value)        ((long)value%DAY_IN_SECONDS >= 64800 && (long)value%DAY_IN_SECONDS < 79200)
#define km2ms(value)            value/3.6
#define ms2km(value)            value*3.6

#define isExitPrahaBrno(km)     (std::find(std::begin(EXITS_PRAHA_BRNO), std::end(EXITS_PRAHA_BRNO), km) != std::end(EXITS_PRAHA_BRNO))
#define isExitBrnoPraha(km)     (std::find(std::begin(EXITS_BRNO_PRAHA), std::end(EXITS_BRNO_PRAHA), km) != std::end(EXITS_BRNO_PRAHA))

#define isEntryPrahaBrno(km)    (std::find(std::begin(ENTRIES_PRAHA_BRNO), std::end(ENTRIES_PRAHA_BRNO), km) != std::end(ENTRIES_PRAHA_BRNO))
#define isEntryBrnoPraha(km)    (std::find(std::begin(ENTRIES_BRNO_PRAHA), std::end(ENTRIES_BRNO_PRAHA), km) != std::end(ENTRIES_BRNO_PRAHA))

#define isUnderConstructionPrahaBrno(i) ((i == 38)  || (i >= 40 && i <= 50) || (i >= 65 && i <= 77))
#define isUnderConstructionBrnoPraha(i) ((i >= 127 && i <= 138 ) || (i >= 153  && i <= 163))

#define najezdPrahaBrno(x) \
	((x < 0.2088943582)	?   0 : ((x < 0.2846706608)	?   1 : ((x < 0.3099612859)	?   2 : ((x < 0.32984191)	?   6 : ((x < 0.3652313413)	?   8 : \
	((x < 0.452720046)	?  12 : ((x < 0.4294568351)	?  15 : ((x < 0.4344420591)	?  21 : ((x < 0.4422387698)	?  29 : ((x < 0.4653145182)	?  34 : \
	((x < 0.4820666673)	?  41 : ((x < 0.502142299)	?  49 : ((x < 0.5069478752)	?  56 : ((x < 0.5177267379)	?  66 : ((x < 0.5254336247)	?  81 : \
	((x < 0.5633842036)	?  90 : ((x < 0.5845377215)	? 104 : ((x < 0.590187642)	? 112 : ((x < 0.6062212002)	? 119 : ((x < 0.6234224685)	? 134 : \
	((x < 0.6391416432)	? 141 : ((x < 0.6594687817)	? 146 : ((x < 0.6694482121)	? 153 : ((x < 0.6935030405)	? 162 : ((x < 0.7203154614)	? 168 : \
	((x < 0.743651699)	? 178 : ((x < 0.7516909341)	? 182 : ((x < 0.8762317096)	? 190 : ((x < 0.9647710839)	? 194 : ((x < 0.9878468324)	? 196 : 201))))))))))))))))))))))))))))))

#define sjezdPrahaBrno(x) \
	((isExitPrahaBrno(x)==0)	? 0 : ((x==2)	? 0.4 : ((x==6)	? 0.3 : ((x==8)	? 0.15 : ((x==10)	? 0.1500: \
	((x==15)	? 0.210 : ((x==21)	? 0.292 : ((x==29)	? 0.110 : ((x==34)	? 0.200 : ((x==41)	? 0.150 : \
	((x==49)	? 0.080 : ((x==56)	? 0.135 : ((x==66)	? 0.100 : ((x==75)	? 0.150 : ((x==90)	? 0.120 : \
	((x==104)	? 0.120 : ((x==112)	? 0.100 : ((x==119)	? 0.100 : ((x==134)	? 0.120 : ((x==141)	? 0.100 : \
	((x==146)	? 0.080 : ((x==153)	? 0.100 : ((x==162)	? 0.100 : ((x==168)	? 0.100 : ((x==178)	? 0.100 : \
	((x==182)	? 0.180 : ((x==190)	? 0.300 : ((x==194)	? 0.300 : ((x==196)	? 0.310 : ((x==201)	? 0.360 : 1))))))))))))))))))))))))))))))

#define najezdBrnoPraha(x) \
	((x < 0.103) ?   0 : ((x < 0.147) ?   2 : ((x < 0.210) ?   7 : ((x < 0.240) ?   9 : ((x < 0.260) ?  13 : \
    ((x < 0.270) ?  21 : ((x < 0.280) ?  25 : ((x < 0.290) ?  35 : ((x < 0.300) ?  41 : ((x < 0.310) ?  50 : \
    ((x < 0.320) ?  57 : ((x < 0.330) ?  62 : ((x < 0.340) ?  69 : ((x < 0.360) ?  84 : ((x < 0.380) ?  99 : \
    ((x < 0.410) ? 113 : ((x < 0.420) ? 128 : ((x < 0.460) ? 137 : ((x < 0.480) ? 147 : ((x < 0.500) ? 154 : \
    ((x < 0.540) ? 162 : ((x < 0.580) ? 169 : ((x < 0.630) ? 174 : ((x < 0.690) ? 182 : ((x < 0.750) ? 188 : \
    ((x < 0.810) ? 193 : ((x < 0.850) ? 195 : ((x < 0.920) ? 197 : 201))))))))))))))))))))))))))))

#define sjezdBrnoPraha(x) \
	((isExitBrnoPraha(x)==0) 	  ? 0 	  : ((x==2)   ? 0.129 : ((x==7)   ? 0.114 : ((x==9)   ? 0.126 : \
	((x==13)  ? 0.174 : ((x==21)  ? 0.206 : ((x==25)  ? 0.263 : ((x==35)  ? 0.123 : ((x==41)  ? 0.102 : \
	((x==50)  ? 0.080 : ((x==57)  ? 0.101 : ((x==62)  ? 0.100 : ((x==69)  ? 0.060 : ((x==84)  ? 0.100 : \
	((x==91)  ? 0.100 : ((x==99)  ? 0.100 : ((x==113) ? 0.190 : ((x==122) ? 0.130 : ((x==137) ? 0.290 : \
	((x==147) ? 0.100 : ((x==154) ? 0.101 : ((x==162) ? 0.191 : ((x==169) ? 0.402 : ((x==174) ? 0.153 : \
	((x==182) ? 0.154 : ((x==188) ? 0.050 : ((x==191) ? 0.110 : ((x==195) ? 0.190 : ((x==198) ? 0.160 : \
	((x==201) ? 0.100 : ((x==202) ? 0.120 : 1)))))))))))))))))))))))))))))))

#define debugArray(km)  \
	((km == 0)   ? gCounterArray[0]++  : ((km == 4)   ? gCounterArray[1]++  : ((km == 10)  ? gCounterArray[2]++  : ((km == 14)  ? gCounterArray[3]++  : \
	((km == 18)  ? gCounterArray[4]++  : ((km == 25)  ? gCounterArray[5]++  : ((km == 32)  ? gCounterArray[6]++  : ((km == 37)  ? gCounterArray[7]++  : \
	((km == 45)  ? gCounterArray[8]++  : ((km == 53)  ? gCounterArray[9]++  : ((km == 61)  ? gCounterArray[10]++ : ((km == 70)  ? gCounterArray[11]++ : \
	((km == 78)  ? gCounterArray[12]++ : ((km == 85)  ? gCounterArray[13]++ : ((km == 100) ? gCounterArray[14]++ : ((km == 108) ? gCounterArray[15]++ : \
	((km == 115) ? gCounterArray[16]++ : ((km == 129) ? gCounterArray[17]++ : ((km == 137) ? gCounterArray[18]++ : ((km == 143) ? gCounterArray[19]++ : \
	((km == 150) ? gCounterArray[20]++ : ((km == 157) ? gCounterArray[21]++ : ((km == 165) ? gCounterArray[22]++ : ((km == 175) ? gCounterArray[23]++ : \
	((km == 180) ? gCounterArray[24]++ : ((km == 189) ? gCounterArray[25]++ : ((km == 193) ? gCounterArray[26]++ : ((km == 195) ? gCounterArray[27]++ : \
	((km == 198) ? gCounterArray[28]++ : ((km == 202) ? gCounterArray[29]++ : gCounterArray[30]))))))))))))))))))))))))))))))
// ======== Macros =========
// =========================


// =========================
// ======== Global =========
int gDirection = BRNO_PRAHA;
double gDayCoefficient = 0;
int gDay = 0;
int gTime = 0;
unsigned long gCounterCar = 0;
unsigned long gCounterAccident = 0;
unsigned long gAvgSpeed = 0;
unsigned long gPartCounter = 0;
unsigned long gCounterArray[30];
unsigned long gCarCounter[24];
// ======== Global =========
// =========================


// =========================
// ======= Histogram =======
Histogram 	hZdrzeniKolona("Doba cekani v kolone", 0, 900 , 12);
Histogram 	hKilometryKolona("Kolony na km: ", 0, 1, HIGHWAY_LENGTH);
Histogram 	hKilometryPomalaKolona("Pomala kolona pod 50kmh na km: ", 0, 1, HIGHWAY_LENGTH);
Histogram 	hKilometryRychlaKolona("Rychla kolona 50-70 kmh na km: ", 0, 1, HIGHWAY_LENGTH);
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
    double mTimeBeforeEntry;
    int mCurrentPosition;
    double mCurrentSpeed;
	int mPartCapacity;

    void Behavior()
    {
        double randomNumber = Random();

        if (gDirection == PRAHA_BRNO)
            mCurrentPosition = najezdPrahaBrno(randomNumber);
        else
            mCurrentPosition = najezdBrnoPraha(randomNumber);

        mTimeBeforeEntry = Time;
        gHighway[mCurrentPosition]->Enter(this, 1);

		if(((gTime == 1 && (Time <= 21600 || Time >= 79200)) || (gTime == 2 && (Time >= 21600 && Time <= 64800)) || (gTime == 3 && (Time >= 64800 && Time <= 79200)) || gTime == 0))
        	gCounterCar++;
        do
        {
			#ifdef DEBUG
				if(((gTime == 1 && (Time <= 21600 || Time >= 79200)) || (gTime == 2 && (Time >= 21600 && Time <= 64800)) || (gTime == 3 && (Time >= 64800 && Time <= 79200)) || gTime == 0)){
	            	debugArray(mCurrentPosition);
					if(mCurrentPosition == 195)
					{
						gCarCounter[((int)Time/3600)]++;
					}
}
			#endif

            if(((gTime == 1 && (Time <= 21600 || Time >= 79200)) || (gTime == 2 && (Time >= 21600 && Time <= 64800)) || (gTime == 3 && (Time >= 64800 && Time <= 79200)) || gTime == 0))
			{
				gPartCounter++;

				if ((Time - mTimeBeforeEntry) > 5)
	            {
	            	hZdrzeniKolona(Time - mTimeBeforeEntry);
	                hKilometryKolona(mCurrentPosition);
	            }
			}
			
			mPartCapacity = gHighway[mCurrentPosition]->Used();

			if(mPartCapacity >= 0 && mPartCapacity <= 18)
				mCurrentSpeed = Uniform(130,140);
			else if(mPartCapacity >= 18 && mPartCapacity <= 23)
				mCurrentSpeed = Uniform(120,130);		
			else if(mPartCapacity >= 23 && mPartCapacity <= 25)
				mCurrentSpeed = Uniform(110,120);		
			else if(mPartCapacity >= 25 && mPartCapacity <= 28)
			    mCurrentSpeed = Uniform(100,110);
			else if(mPartCapacity >= 28 && mPartCapacity <= 40)
				mCurrentSpeed = Uniform(90,100);		
			else if(mPartCapacity >= 40 && mPartCapacity <= 43)
				mCurrentSpeed = Uniform(80,90);
			else if(mPartCapacity >= 43 && mPartCapacity <= 50)
				mCurrentSpeed = Uniform(70,80);
			else if(mPartCapacity >= 50 && mPartCapacity <= 55)
				mCurrentSpeed = Uniform(60,70);
			else if(mPartCapacity >= 55 && mPartCapacity <= 62)
				mCurrentSpeed = Uniform(50,60);
			else if(mPartCapacity >= 62 && mPartCapacity <= 102)
				mCurrentSpeed = Uniform(40,50);
			else if(mPartCapacity >= 102 && mPartCapacity <= 120)
				mCurrentSpeed = Uniform(30,40);
			else if(mPartCapacity >= 120)
				mCurrentSpeed = 30;		

			if(ms2km(gHighway[mCurrentPosition]->mMaxSpeed) != 130)			
			{				
				if (mCurrentSpeed > ms2km(gHighway[mCurrentPosition]->mMaxSpeed))					
					mCurrentSpeed = gHighway[mCurrentPosition]->mMaxSpeed;				
				else					
					mCurrentSpeed = km2ms(mCurrentSpeed);			
			}			
			else
				mCurrentSpeed = km2ms(mCurrentSpeed);

			if(((gTime == 1 && (Time <= 21600 || Time >= 79200)) || (gTime == 2 && (Time >= 21600 && Time <= 64800)) || (gTime == 3 && (Time >= 64800 && Time <= 79200)) || gTime == 0))
        	{		
				if(ms2km(mCurrentSpeed) < 50)
					hKilometryPomalaKolona(mCurrentPosition);
				else if(ms2km(mCurrentSpeed) >= 50 && ms2km(mCurrentSpeed) <= 70)
					hKilometryRychlaKolona(mCurrentPosition);
					
				gAvgSpeed+=ms2km(mCurrentSpeed);
			}

	        Wait(1000 / mCurrentSpeed);
            mTimeBeforeEntry = Time;
            mCurrentPosition++;
            gHighway[mCurrentPosition]->Enter(this, 1);
            gHighway[mCurrentPosition - 1]->Leave(1);	
        }
        while (Random() >= ((gDirection == PRAHA_BRNO) ? sjezdPrahaBrno(mCurrentPosition) : sjezdBrnoPraha(mCurrentPosition)));

        gHighway[mCurrentPosition]->Leave(1);
        Terminate();
    }
};

class Accident : public Process
{
    void Behavior()
    {
        gCounterAccident++;

        int position = Uniform(0, HIGHWAY_LENGTH - 1);
        double speedBackup = gHighway[position]->mMaxSpeed;
        double repairTime;

        // std::cout << std::endl << "Nehoda na " << position << "km" << std::endl;
        // std::cout << "Time: " << Time / 3600 << std::endl;

        int newSpeed = Uniform(20, 70);
		
		//int newSpeed = 15;
        // std::cout << "Accident! Speed: " << newSpeed << std::endl;
        repairTime = Uniform(5400, 7200);
        gHighway[position]->mMaxSpeed = km2ms(newSpeed);

        // std::cout << "Repair time: " << repairTime << std::endl;
        Wait(repairTime);
        gHighway[position]->mMaxSpeed = speedBackup;
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
            Activate(Time + Exponential(GENERATE_CAR_DAY*gDayCoefficient));
        else if (isEvening(Time))
            Activate(Time + Exponential(GENERATE_CAR_EVENING*gDayCoefficient));
        else
            Activate(Time + Exponential(GENERATE_CAR_NIGHT*gDayCoefficient));
    }
};

class GeneratorProgress : public Event
{
    void Behavior()
    {
        if (Time == 0)
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

            if (isUnderConstructionPrahaBrno(i))
            {
                maxSpeed = 80;
                capacity = CAPACITY_LIMITED;
            }
        }
        else
        {
            if (isExitBrnoPraha(i))
                exit = true;

            if (isUnderConstructionBrnoPraha(i))
            {
                maxSpeed = 80;
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
int main(int argc, char *argv[])
{
	bool help = true;
	if(argc == 7)
	{
		if(strcmp(argv[1], "-d")==0 && strcmp(argv[3], "-t")==0 && strcmp(argv[5],"-s")==0)
		{
			if((*argv[2] >= '0' && *argv[2] <= '6') && (*argv[4] >= '0' && *argv[4] <= '3') && (*argv[6] >= '0' && *argv[6] <= '1'))
			{
				help = false;

				gDay=atoi(argv[2]);
				gTime=atoi(argv[4]);

				if(*argv[6] == '0')
					gDirection = PRAHA_BRNO;
				else
					gDirection = BRNO_PRAHA;

				if(gDay == 0)
					gDayCoefficient = 1.15; // Sunday
				else if(gDay == 1)
					gDayCoefficient = 0.9; // Monday
				else if(gDay == 2)
					gDayCoefficient = 1.0; // Tuesday
				else if(gDay == 3)
					gDayCoefficient = 1.0; // Wednesday
				else if(gDay == 4)
					gDayCoefficient = 1.0; // Thursday
				else if(gDay == 5)
					gDayCoefficient = 0.9; // Friday
				else
					gDayCoefficient = 1.3; // Saturday
			}
		}
	}

	if(help)
	{
		std::cout<<"Usage: "<< argv[0] << " -d <day> -t <time> -s <direction>\n\n";
		std::cout<<"Day:\n" << "\tSunday = 0\n" << "\tMonday = 1\n" << "\tTuesday = 2\n" << "\tWednesday = 3\n" << "\tThursday = 4\n" << "\tFriday = 5\n" << "\tSaturday = 6\n\n";
		std::cout<<"Time:\n" << "\t0:00 - 24:00 = 0\n" << "\t22:00 - 6:00 = 1\n" << "\t6:00 - 18:00 = 2\n" << "\t18:00 - 22:00 = 3\n\n";
		std::cout<<"Direction:\n" << "\tPraha -> Brno = 0\n" << "\tBrno -> Praha = 1\n";
		return 0;
	}


    SetOutput(OUTPUT_FILE);
    RandomSeed ( time(NULL) );
	
    initHighway();
    Init(0, SIMULATION_DURATION * DAY_IN_SECONDS);
    (new GeneratorProgress)->Activate();
    (new GeneratorCar)->Activate();
    (new GeneratorAccident)->Activate(Time + Exponential(GENERATE_ACCIDENT));
    Run();

    destroyHighway();

#ifdef DEBUG
    if(gDirection == PRAHA_BRNO)
    {
    	// Praha-Brno
	    std::cout << "\nNa km   0 -   2   (1) projelo " << gCounterArray[0]  << " aut. Statisticky 34 499\n";
	    std::cout << "Na km   2 -   6   (4) projelo " << gCounterArray[1]  << " aut. Statisticky 27 776\n";
	    std::cout << "Na km   6 -  12  (10) projelo " << gCounterArray[2]  << " aut. Statisticky 21 263\n";
	    std::cout << "Na km  12 -  15  (14) projelo " << gCounterArray[3]  << " aut. Statisticky 22 310\n";
	    std::cout << "Na km  15 -  21  (18) projelo " << gCounterArray[4]  << " aut. Statisticky 18 494\n";
	    std::cout << "Na km  21 -  29  (25) projelo " << gCounterArray[5]  << " aut. Statisticky 12 332\n";
	    std::cout << "Na km  29 -  34  (32) projelo " << gCounterArray[6]  << " aut. Statisticky 12 128\n";
	    std::cout << "Na km  34 -  41  (37) projelo " << gCounterArray[7]  << " aut. Statisticky 11 769\n";
	    std::cout << "Na km  41 -  49  (45) projelo " << gCounterArray[8]  << " aut. Statisticky 11 487\n";
	    std::cout << "Na km  49 -  56  (53) projelo " << gCounterArray[9]  << " aut. Statisticky 13 211\n";
	    std::cout << "Na km  56 -  66  (61) projelo " << gCounterArray[10] << " aut. Statisticky 11 372\n";
	    std::cout << "Na km  66 -  75  (70) projelo " << gCounterArray[11] << " aut. Statisticky 11 540\n";
	    std::cout << "Na km  75 -  81  (78) projelo " << gCounterArray[12] << " aut. Statisticky 11 324\n";
	    std::cout << "Na km  81 -  90  (85) projelo " << gCounterArray[13] << " aut. Statisticky 11 958\n";
	    std::cout << "Na km  90 - 104  (97) projelo " << gCounterArray[14] << " aut. Statisticky 14 067\n";
	    std::cout << "Na km 104 - 112 (108) projelo " << gCounterArray[15] << " aut. Statisticky 14 596\n";
	    std::cout << "Na km 112 - 119 (115) projelo " << gCounterArray[16] << " aut. Statisticky 13 884\n";
	    std::cout << "Na km 119 - 134 (127) projelo " << gCounterArray[17] << " aut. Statisticky 14 041\n";
	    std::cout << "Na km 134 - 141 (137) projelo " << gCounterArray[18] << " aut. Statisticky 14 637\n";
	    std::cout << "Na km 141 - 146 (143) projelo " << gCounterArray[19] << " aut. Statisticky 14 868\n";
	    std::cout << "Na km 146 - 153 (150) projelo " << gCounterArray[20] << " aut. Statisticky 15 905\n";
	    std::cout << "Na km 153 - 162 (157) projelo " << gCounterArray[21] << " aut. Statisticky 15 204\n";
	    std::cout << "Na km 162 - 168 (165) projelo " << gCounterArray[22] << " aut. Statisticky 17 149\n";
	    std::cout << "Na km 168 - 178 (173) projelo " << gCounterArray[23] << " aut. Statisticky 18 552\n";
	    std::cout << "Na km 178 - 182 (180) projelo " << gCounterArray[24] << " aut. Statisticky 18 932\n";
	    std::cout << "Na km 182 - 190 (185) projelo " << gCounterArray[25] << " aut. Statisticky 13 712\n";
	    std::cout << "Na km 190 - 194 (192) projelo " << gCounterArray[26] << " aut. Statisticky 26 511\n";
	    std::cout << "Na km 194 - 196 (195) projelo " << gCounterArray[27] << " aut. Statisticky 30 558\n";
	    std::cout << "Na km 196 - 201 (198) projelo " << gCounterArray[28] << " aut. Statisticky 22 272\n";
	    std::cout << "Na km 201 - 203 (202) projelo " << gCounterArray[29] << " aut. Statisticky 14 483\n";
    }
    else
    {
		// Brno-Praha
	    std::cout << "\nNa km   0 -   2   (1) projelo " << gCounterArray[0]  << " aut. Statisticky 18 433\n";
	    std::cout << "Na km   2 -   6   (4) projelo " << gCounterArray[1]  << " aut. Statisticky 24 128\n";
	    std::cout << "Na km   6 -  12  (10) projelo " << gCounterArray[2]  << " aut. Statisticky 34 458\n";
	    std::cout << "Na km  12 -  15  (14) projelo " << gCounterArray[3]  << " aut. Statisticky 32 403\n";
	    std::cout << "Na km  15 -  21  (18) projelo " << gCounterArray[4]  << " aut. Statisticky 31 996\n";
	    std::cout << "Na km  21 -  29  (25) projelo " << gCounterArray[5]  << " aut. Statisticky 21 349\n";
	    std::cout << "Na km  29 -  34  (32) projelo " << gCounterArray[6]  << " aut. Statisticky 20 920\n";
	    std::cout << "Na km  34 -  41  (37) projelo " << gCounterArray[7]  << " aut. Statisticky 20 959\n";
	    std::cout << "Na km  41 -  49  (45) projelo " << gCounterArray[8]  << " aut. Statisticky 20 153\n";
	    std::cout << "Na km  49 -  56  (53) projelo " << gCounterArray[9]  << " aut. Statisticky 21 083\n";
	    std::cout << "Na km  56 -  66  (61) projelo " << gCounterArray[10] << " aut. Statisticky 20 532\n";
	    std::cout << "Na km  66 -  75  (70) projelo " << gCounterArray[11] << " aut. Statisticky 21 063\n";
	    std::cout << "Na km  75 -  81  (78) projelo " << gCounterArray[12] << " aut. Statisticky 21 062\n";
	    std::cout << "Na km  81 -  90  (85) projelo " << gCounterArray[13] << " aut. Statisticky 21 716\n";
	    std::cout << "Na km  90 - 104  (97) projelo " << gCounterArray[14] << " aut. Statisticky 21 005\n";
	    std::cout << "Na km 104 - 112 (108) projelo " << gCounterArray[15] << " aut. Statisticky 22 003\n";
	    std::cout << "Na km 112 - 119 (115) projelo " << gCounterArray[16] << " aut. Statisticky 22 209\n";
	    std::cout << "Na km 119 - 134 (127) projelo " << gCounterArray[17] << " aut. Statisticky 21 981\n";
	    std::cout << "Na km 134 - 141 (137) projelo " << gCounterArray[18] << " aut. Statisticky 22 400\n";
	    std::cout << "Na km 141 - 146 (143) projelo " << gCounterArray[19] << " aut. Statisticky 24 166\n";
	    std::cout << "Na km 146 - 153 (150) projelo " << gCounterArray[20] << " aut. Statisticky 23 485\n";
	    std::cout << "Na km 153 - 162 (157) projelo " << gCounterArray[21] << " aut. Statisticky 25 567\n";
	    std::cout << "Na km 162 - 168 (165) projelo " << gCounterArray[22] << " aut. Statisticky 27 462\n";
	    std::cout << "Na km 168 - 178 (173) projelo " << gCounterArray[23] << " aut. Statisticky 28 298\n";
	    std::cout << "Na km 178 - 182 (180) projelo " << gCounterArray[24] << " aut. Statisticky 28 774\n";
	    std::cout << "Na km 182 - 190 (189) projelo " << gCounterArray[25] << " aut. Statisticky 45 278\n";
	    std::cout << "Na km 190 - 194 (193) projelo " << gCounterArray[26] << " aut. Statisticky 49 659\n";
	    std::cout << "Na km 194 - 196 (195) projelo " << gCounterArray[27] << " aut. Statisticky 47 327\n";
	    std::cout << "Na km 196 - 201 (198) projelo " << gCounterArray[28] << " aut. Statisticky 51 584\n";
	    std::cout << "Na km 201 - 203 (202) projelo " << gCounterArray[29] << " aut. Statisticky 53 961\n";
    }

	std::cout << "\nDalnici projelo " << gCounterCar << " aut\n";
    std::cout << "Za cely den se na dalnici staly " << gCounterAccident << " nehody\n";
	std::cout << "Prumerna rychlost: " << gAvgSpeed/gPartCounter << std::endl;
#endif

    hZdrzeniKolona.Output();
    hKilometryKolona.Output();
	hKilometryPomalaKolona.Output();
	hKilometryRychlaKolona.Output();
	for(int i=0; i < 24;i ++)
	{
		std::cout << "Hour:" << i  << " : " << gCarCounter[i] << std::endl;
	}
    
    return 0;
}
