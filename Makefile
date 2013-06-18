CC = g++
CFLAGS = -Wall -g
PROG = imageMatch

SRC = main2.cpp handledata.cpp loaddata.cpp qualityassessment.cpp systemmessages.cpp
LIBS = -lglut
OPENCV = `pkg-config opencv --libs --cflags`

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) -o $(PROG) $(SRC) $(LIBS) $(OPENCV)


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

