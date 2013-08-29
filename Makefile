CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
PROG = ../bin/imageMatch

SRC = main.cpp core.cpp handledata.cpp similarityassessment.cpp logdata.cpp GlutWindow.cpp ArcBall.cpp TransferFunction.cpp Trapez.cpp mathutil/CMatrix.cpp mathutil/CPosition.cpp mathutil/CQuaternion.cpp mathutil/CVector.cpp
OPENGL = -lglut -lGL -lGLU -lGL
OPENCV = `pkg-config opencv --libs --cflags`
CG = -lCg -lCgGL

MY_INCLUDE_HANDLE3DDATASETS =-I../../my_libs/Handle3DDataset/src 
#MY_LIB_HANDLE3DDATASETS = -lhandle3ddataset
MY_LIB_PATCH_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin
#MY_R_PATCH_HANDLE3DDATASETS = -Wl,-R/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/Handle3DDataset/bin

MY_INCLUDE_QUALITYASSESSMENT =-I/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/QualityAssessment/src 
MY_LIB_QUALITYASSESSMENT = -lqualityassessment
MY_LIB_PATCH_QUALITYASSESSMENT = -L/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/QualityAssessment/bin
MY_R_PATCH_QUALITYASSESSMENT = -Wl,-R/home/jeronimo/Dropbox/UFRGS/Mestrado/Devel/my_libs/QualityAssessment/bin




$(PROG): $(SRC)
	 #$(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) -o $(PROG) $(SRC) $(OPENGL) $(OPENCV) $(CG) $(MY_INCLUDE_HANDLE3DDATASETS) $(MY_INCLUDE_QUALITYASSESSMENT) $(MY_LIB_HANDLE3DDATASETS) $(MY_LIB_QUALITYASSESSMENT) $(MY_LIB_PATCH_HANDLE3DDATASETS) $(MY_LIB_PATCH_QUALITYASSESSMENT) #$(MY_R_PATCH_HANDLE3DDATASETS) $(MY_R_PATCH_QUALITYASSESSMENT)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) -o $(PROG) $(SRC) $(OPENGL) $(OPENCV) $(CG) $(MY_INCLUDE_HANDLE3DDATASETS) $(MY_INCLUDE_QUALITYASSESSMENT) $(MY_LIB_PATCH_HANDLE3DDATASETS) $(MY_LIB_QUALITYASSESSMENT) $(MY_LIB_PATCH_QUALITYASSESSMENT) $(MY_R_PATCH_QUALITYASSESSMENT)

clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

