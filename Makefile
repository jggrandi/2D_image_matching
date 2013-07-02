CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = imageMatch

SRC = main2.cpp handledata.cpp qualityassessment.cpp logdata.cpp
OPENGL = -lglut
OPENCV = `pkg-config opencv --libs --cflags`

MY_INCLUDE =-I/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/src 
MY_LIB = -lhandle3ddataset
MY_LIB_PATCH = -L/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/bin
MY_R_PATCH = -Wl,-R/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/bin


$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(COPTIMIZATION) $(WARNINGOFF) -o $(PROG) $(SRC) $(OPENGL) $(OPENCV) $(MY_INCLUDE) $(MY_LIB) $(MY_LIB_PATCH) $(MY_R_PATCH)


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

