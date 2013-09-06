CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = ../bin/volumeRendering

SRC = main.cpp core.cpp GlutWindow.cpp ArcBall.cpp TransferFunction.cpp Trapez.cpp mathutil/CMatrix.cpp mathutil/CPosition.cpp mathutil/CQuaternion.cpp mathutil/CVector.cpp
OPENGL = -lglut -lGL -lGLU -lGL
OPENCV = `pkg-config opencv --libs --cflags`
CG = -lCg -lCgGL

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) -o $(PROG) $(SRC) $(OPENGL) $(OPENCV) $(CG)

clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

