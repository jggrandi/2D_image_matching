#ifndef CORE
#define CORE

#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
#include <handle3ddataset.h>

#include "GlutWindow.h"
#include "handledata.h"
#include "utils.h"

using namespace std;



class Core
{
public:
	Core(int c_argc=0, char **c_argv=NULL);
	~Core();
	bool setup(DATAINFO c_img1, DATAINFO c_img2, OPT c_options);
	void run();
	static void display(void);
	static void idle(void);
	static void reshape(int width, int height);
	static void keyboard(unsigned char key, int x, int y);
	static void mouse(int button, int state, int x, int y);
	static void move(int x, int y);
	static Core* getInstance();
private:
	static CGlutWindow  *c_pMainWindow;
	HandleData   c_handleData;
	static int   core_argc;
	static char  **core_argv;	
};



#endif // CORE