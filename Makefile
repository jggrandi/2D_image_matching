CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = ../bin/volumeRendering

SRC = main.cpp core.cpp GlutWindow.cpp ArcBall.cpp TransferFunction.cpp Trapez.cpp mathutil/CMatrix.cpp mathutil/CPosition.cpp mathutil/CQuaternion.cpp mathutil/CVector.cpp
OPENGL = -lglut -lGL -lGLU -lGL
CG = -lCg -lCgGL

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) -o $(PROG) $(SRC) $(OPENGL) $(CG)

clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

