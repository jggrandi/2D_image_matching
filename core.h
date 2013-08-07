#ifndef CORE
#define CORE

#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
#include <handle3ddataset.h>

#include "GlutWindow.h"
#include "utils.h"

using namespace std;

class Core
{
public:
	Core(int c_argc, char **c_argv);
	~Core();
	void run();
	static void display(void);
	static void idle(void);
	static void reshape(int width, int height);
	static void keyboard(unsigned char key, int x, int y);
	static void mouse(int button, int state, int x, int y);
	static void move(int x, int y);
	static void countFPS(int value);
private:
	static CGlutWindow  *c_pMainWindow;
	static unsigned int fpsCurrent,fpsCount;
};



#endif // CORE