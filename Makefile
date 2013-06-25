CC = g++
CFLAGS = -Wall -g -O0
WARNINGOFF = -w
PROG = imageMatch

SRC = main2.cpp handledata.cpp loaddata.cpp qualityassessment.cpp systemmessages.cpp logdata.cpp
LIBS = -lglut
OPENCV = `pkg-config opencv --libs --cflags`

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(WARNINGOFF) -o $(PROG) $(SRC) $(LIBS) $(OPENCV)


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

