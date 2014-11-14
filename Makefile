#
# Makefile IMS
#

CXX=g++ -m64
CXXFLAGS  = -Wall -std=c++98
CXXFLAGS += -g  # debugging info
CXXFLAGS += -O2 # optimization level
#CXXFLAGS += -pg # profiling support

SIMLIB_DIR = /usr/local/include
SIMLIB_DIR_SO = /usr/local/lib

CXXFLAGS += -I$(SIMLIB_DIR)

SIMLIB_DEPEND = $(SIMLIB_DIR)/simlib.h \
		$(SIMLIB_DIR)/delay.h \
		$(SIMLIB_DIR)/zdelay.h \
		$(SIMLIB_DIR)/simlib2D.h \
		$(SIMLIB_DIR)/simlib3D.h \
		$(SIMLIB_DIR_SO)/libsimlib.so 


% : %.cc  $(SIMLIB_DEPEND)
	$(CXX) $(CXXFLAGS) -o $@  $< $(SIMLIB_DIR_SO)/libsimlib.so -lm

# list of all test models
ALL_TEST_MODELS = main

#############################################################################
# RULES

all: $(ALL_TEST_MODELS)

run: all
	@for i in $(ALL_TEST_MODELS); do echo $$i; ./$$i >$$i.out; done
	@./sizeof-all >sizeof-all-`file ./sizeof-all|sed 's/.*\([36][24]\)-bit.*/\1/'`.out

#############################################################################
# cleaning, backup, etc

clean: 
	rm -f $(ALL_TEST_MODELS) *.o *~

clean-all: clean
	rm -f *.dat *.out

pack:
	tar czf tests.tar.gz  *.cc Makefile* *.txt *.output *.plt

# end of Makefile
