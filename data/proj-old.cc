/////////////////////////////////////////////////////////////////////////////////////////
//  
//  Projekt IMS
//  Zadanie 8 - Simulacia dialnice
//  Riesitelia:
//    Karol Troska        xtrosk00
//    Martin Vymlatil     xvymla01
//
//  Diskretizacia dialnice - kilometre
//  Diskretizacia casu     - sekundy (rychlost v m/s)
//  
//


#include <simlib.h>
#include <time.h>
#include <iostream>



/*
 * Konstanty ktore meni uzivatel podla potreby vystupu
 */
 
// histogramy 12, 96, 185 a 207 km
#define                             DEBUG

// zatazenie vjazdov a vyjazdov
 #define                             INOUT


// dlzka experimentu v dnoch
#define   EXPERIMENT_DURATION       1

// vystupny subor
#define   OUTPUT_FILE               "out"

/*
 * Ladniace konstanty
 */
 
// intervaly v sekundach 
#define GEN_CAR_MORNING             0.3       //  5:00 - 11:00
#define GEN_CAR_MIDDAY              0.15      // 11:00 - 16:00
#define GEN_CAR_AFTERNOON           0.1       // 16:00 - 20:00
#define GEN_CAR_EVENING             0.2       // 20:00 - 23:00
#define GEN_CAR_NIGHT               0.5       // 23:00 -  5:00


#define GEN_ACC                     40000    // 1 - 2 nehody / den

#define CAPACITY                    50       // auta / km -> 1000 m * 2 pruhy / (dlzka auta + rozostupy) 
#define HALF_CAPACITY               30       // -,,-                * 1 pruh  / -,,-


/*
 * Pevne dane makra a konstanty
 */


#define SECONDS_PER_DAY             86400       // sekundy za den
#define toms(speed)                 speed/3.6   // km/h -> m/s
#define tokmh(speed)                speed*3.6   // m/s  -> km/h

// denne doby

#define isMorning()     ((long)Time%SECONDS_PER_DAY >= 18000 && (long)Time%SECONDS_PER_DAY <  39600)
#define isMidday()      ((long)Time%SECONDS_PER_DAY >= 39600 && (long)Time%SECONDS_PER_DAY <  57600)
#define isAfternoon()   ((long)Time%SECONDS_PER_DAY >= 57600 && (long)Time%SECONDS_PER_DAY <  72000)
#define isEvening()     ((long)Time%SECONDS_PER_DAY >= 72000 && (long)Time%SECONDS_PER_DAY <  82800)
#define isNight()       ((long)Time%SECONDS_PER_DAY <  18000 || (long)Time%SECONDS_PER_DAY >= 82800)

/*
 * Konstanty ohladom dialnice
 */
 
// dlzka dialnice
#define HIGHWAY_LENGTH              271

// vracia true ak je na danom kilometri exit
// idem z prahy
#define isExit(i) (i==0||i==2||i==6||i==10||i==15||i==21||i==29||i==34||i==41 \
                 ||i==49||i==56||i==66||i==75||i==90||i==104||i==112||i==119 \
                 ||i==134||i==141||i==146||i==153||i==162||i==168||i==178 \
                 ||i==182||i==190||i==194||i==196||i==201||i==210||i==216 \
                 ||i==226||i==230||i==236||i==244||i==253||i==258||i==260 \
                 ||i==265||i==270)

// vracia true ak je dany kilometer v oprave
#define isInRepair(i) ((i >= 41  && i <= 49)  || \
                       (i >= 66  && i <= 75)  || \
                       (i >= 104 && i <= 112) || \
                       (i >= 154 && i <= 162))

/*
 * Zber statistik
 */

// ak sa zdrzalo auto tak kolko?
Histogram         Zdrzanie("Doba cakania v kolone v sec", 0, 600, 20); 

// na ktorom kilometri sa zdrzalo
Histogram         Kilometer("kilometre s kolonou",0,1,HIGHWAY_LENGTH);


#ifdef INOUT
  Histogram         Vjazd("Zatazenie vjazdov",0,1,HIGHWAY_LENGTH);
  Histogram         Vyjazd("Zatazenie vyjazdov",0,1,HIGHWAY_LENGTH);
#endif

// ladenie podla skutocnych statistik
#ifdef DEBUG
  Histogram         _km12 ("12 kilometer",0,3600,24);
  Histogram         _km96 ("96 kilometer",0,3600,24);
  Histogram         _km185("185 kilometer",0,3600,24);
  Histogram         _km207("207 kilometer",0,3600,24);

  #define km12(km)  if(km == 12)  _km12(Time)
  #define km96(km)  if(km == 96)  _km96(Time)
  #define km185(km) if(km == 185) _km185(Time)
  #define km207(km) if(km == 207) _km207(Time)
#endif

// pocitadlo aut a nehod
unsigned long car_counter = 0;
unsigned long acc_counter = 0;


/*
 * jeden kilometer dialnice
 */
class HighwayPart : public Store { // kilometer dialnice
  public:
  HighwayPart (const char* name, int size, int speed, bool exit) : Store(name, size) {
    Speed = toms(speed); 
    Exit = exit;
  };
  double Speed;   // rychlost m/s
  bool Exit;      // true ak je exit
};

// cela dialnica (270 km)
HighwayPart * highway[HIGHWAY_LENGTH];

// nastavenie dialnice -> vyjazdy, kapacity a max rychlosti v jednotlivych usekoch
void initHighway(){

  HighwayPart * tmp;

  for(int i = 0; i < HIGHWAY_LENGTH; i++){
    bool e = false;  
    int s  = 130;
    int c  = CAPACITY;
    
    if(isExit(i)) e = true; // exity na km
    if(isInRepair(i)){
      s = 90;   // oprava, znizenie max rychlosti
      c = HALF_CAPACITY; 
    }

    tmp = new HighwayPart("kilometer",c,s,e); // instancia
    highway[i] = tmp; // vlozenie do dialnice
  }
}

// zrusenie dialnice
void destroyHighway(){
  for(int i = 0; i< HIGHWAY_LENGTH; i++) delete highway[i];
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*
 * Ocakava rovnomerne <0,1)
 * vracia kilometer podla mapovacej funkcie
 */

 // 39 vyjazdov, hodnoty priblizne okolo 1/39 = 0,0256 ... suma 1
int vjazd(double x){
  if(x < 0.080) return  0; 
  if(x < 0.138) return  2;
  if(x < 0.172) return  6;
  if(x < 0.225) return  10;
  if(x < 0.244) return  15;
  if(x < 0.276) return  21; 
  if(x < 0.298) return  29;
  if(x < 0.325) return  34;
  if(x < 0.338) return  41; // oprava
  if(x < 0.365) return  49;
  if(x < 0.390) return  56;
  if(x < 0.415) return  66;
  if(x < 0.434) return  75;
  if(x < 0.458) return  90;
  if(x < 0.474) return  104;
  if(x < 0.500) return  112;
  if(x < 0.524) return  119;
  if(x < 0.548) return  134;
  if(x < 0.565) return  141;
  if(x < 0.585) return  146; // oprava
  if(x < 0.606) return  153;
  if(x < 0.630) return  162;
  if(x < 0.655) return  168;
  if(x < 0.677) return  178;
  if(x < 0.700) return  182;
  if(x < 0.729) return  190;
  if(x < 0.757) return  194;
  if(x < 0.780) return  196;
  if(x < 0.813) return  201;
  if(x < 0.832) return  210;
  if(x < 0.857) return  216;
  if(x < 0.875) return  226;
  if(x < 0.892) return  230;
  if(x < 0.910) return  236;
  if(x < 0.932) return  244;
  if(x < 0.950) return  253;
  if(x < 0.972) return  258;
  if(x < 0.988) return  260;
  return 265;
}

/*
 * dostava km dialnice
 * Vracia pravdepodobnost opustenia dialnice na danom km
 */
double vyjazd(int km){
  if(!isExit(km))           return 0.0;  // neni som na vyjazde
  if(km < 15)               return 0.4;  // Praha
  if(km > 185 && km < 202 ) return 0.4;  // Brno
  if(km == 270)             return 1.0;  // koniec
  return 0.3;                            // iny vyjazd
}
/*
 * Proces pohybu auta po dialnici
 */

class Car : public Process {
  double in;  // cas vstupu transakcie na kilometer
  int km;     // na ktorom som kilometri
  double spd; // rychlost auta

  void Behavior() {
    car_counter++;


    km = vjazd(Random());
    
    #ifdef INOUT
      Vjazd(km);
    #endif
    
    // prichod na dialnicu - inicializacia auta na vyjazde
    in = Time;
    highway[km]->Enter(this,1);
    
    do{
    #ifdef DEBUG
      // zapis statistik na urcitych kilometroch
      km12(km); 
      km96(km); 
      km185(km); 
      km207(km);
    #endif

      // ak hned nevstupim do useku -> zber statistik (zdznie na metode Enter())
      if((Time - in) != 0) { 
        Zdrzanie(Time - in); // cas v kolone 
        Kilometer(km);       // kilometer, do ktoreho som oneskorene vstupil
      }

      // koeficient rychlosti 
      spd = Normal(1.0,0.1);

      // koeficient sa nasobi s max rychlostou useku, 
      // ak je mensi ako 0,7 auto ide moc pomaly a dostava minimalnu rychlost
      spd = (spd < 0.7)? 0.7*highway[km]->Speed : spd*highway[km]->Speed;

      // prejazd -> 1000 m / rychlost v m/s
      Wait(1000/spd); 
      
      km++;

      // ulozim cas, kedy chcem vstupit na dalsi usek dialnice
      in = Time;
      highway[km]->Enter(this,1);
      // az ked vstupim, mozem minuly usek opustit
      highway[km-1]->Leave(1);


    // opusta dialnicu ak je tam vyjazd a nejaky koeficient
    } while(Random() > vyjazd(km));
    #ifdef INOUT
      Vyjazd(km);
    #endif

    highway[km]->Leave(1);
    Terminate();
  }
};




/*
 * Proces opravovania nehody
 */

class Accident : public Process {
  void Behavior() {
  
    // vzdy generuje nehodu v case 0.0, neviem preco
    if(Time == 0.0) return;
    
    acc_counter++;
    
    // na niektorom kilometri
    int km = Uniform(0,HIGHWAY_LENGTH-1);
    
    // zavaznost nehody blizsie 0, vacsia zavaznost
    double dangerous = Random(); 
    
    // povodna rychlost useku
    double spd = highway[km]->Speed;
    
    // rychlost sa znizi podla zavaznosti nehody
    highway[km]->Speed = spd*dangerous;
    
    //dlzka opravy zavisi od zavaznosti  
    double waiting = Exponential(1000/(dangerous*dangerous));
    
    // vypis do tabulky (1-dangerous kvoli lepsej citelnosti)
    Print("| %-9.2f|   %-5d|    %-8.2f|  %-10.2f|  %-10.2f|\n",Time,km,1-dangerous,waiting,tokmh(highway[km]->Speed));

    // oprava    
    Wait(waiting); 
    
    // povodna rychlost useku
    highway[km]->Speed = spd;
  }
};



/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////



/* 
 * Nehoda 
 */

class accGen : public Event {
  void Behavior() {
    (new Accident)->Activate(); 
    Activate(Time+Exponential(GEN_ACC));
  }
};


/* 
 * Prichod auta 
 */
class carGen : public Event {
  void Behavior() {
    (new Car)->Activate(); 

    // frekvencia generovania podla dennej doby
    if(isMorning()) 
      Activate(Time+Exponential(GEN_CAR_MORNING));
    else if(isMidday())
      Activate(Time+Exponential(GEN_CAR_MIDDAY));
    else if(isAfternoon())
      Activate(Time+Exponential(GEN_CAR_AFTERNOON));
    else if(isEvening())
      Activate(Time+Exponential(GEN_CAR_EVENING));
    else
      Activate(Time+Exponential(GEN_CAR_NIGHT));
  }
};

/*
 * Procedura spusta generatory vsetkych procesov
 */

void activateGenerators(){
  (new carGen)->Activate();
  (new accGen)->Activate();
}

int main() {
  std::cout << "***** PROJEKT IMS *****" << std::endl;
  std::cout << "*****  DIALNICA   *****" << std::endl << std::endl;
  
  
  SetOutput(OUTPUT_FILE);
  
  RandomSeed ( time(NULL) );
  
  initHighway();
  Init(0, EXPERIMENT_DURATION*SECONDS_PER_DAY);
  activateGenerators();


  Print("+----------------------------------------------------------+\n");
  Print("| NEHODY                                                   |\n");
  Print("+----------+--------+------------+------------+------------+\n");
  Print("|    Cas   |   Km   | Zavaznost  |   Oprava   |  Rychlost  |\n");
  Print("+----------+--------+------------+------------+------------+\n");
  
  Run();

  if(acc_counter == 0)
  Print("|  NO RESULTS                                              |\n");  
  Print("+----------+--------+------------+------------+------------+\n\n");

  destroyHighway();
  
  
  _Print("Dialnicou preslo %d osobnych aut\n",car_counter);
  _Print("Na dialnici sa stalo %d nehod\n\n",acc_counter);

  Zdrzanie.Output();
  Kilometer.Output();

  #ifdef DEBUG
    _km12.Output();
    _km96.Output();
    _km185.Output();
    _km207.Output();
  #endif
  
  #ifdef INOUT
    Vjazd.Output();
    Vyjazd.Output();
  #endif
  std::cout << "Vysledok je zapisany do suboru '" << OUTPUT_FILE << "'" << std::endl << std::endl;
  std::cout << "*****  KONIEC EXPERIMENTU  *****" << std::endl;
  std::cout << "********************************" << std::endl  << std::endl;
  return 0;
}
