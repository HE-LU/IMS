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

#define                             DEBUG
#define                             INOUT
#define   EXPERIMENT_DURATION       1
#define   OUTPUT_FILE               "output.out"

int main() {
  SetOutput(OUTPUT_FILE);
  
  RandomSeed ( time(NULL) );
  
  return 0;
}
