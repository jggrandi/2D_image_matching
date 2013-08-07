#include "core.h"


CGlutWindow *Core::c_pMainWindow=0;
unsigned int Core::fpsCurrent=0;
unsigned int Core::fpsCount=0;
Core::Core(int c_argc, char **c_argv)
{

	glutInit(&c_argc, c_argv);
	c_pMainWindow = new CGlutWindow();	
}

Core::~Core(){}


void Core::run()
{
//	c_handleData.similarityCheck();	
	
	glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(move);
    glutTimerFunc(1000, countFPS, 1);
	glutIdleFunc(idle);
	glutMainLoop();
}


void Core::display(void)
{ 
	c_pMainWindow->renderFrame();
	fpsCurrent++;
}

void Core::idle(void)
{ 
	c_pMainWindow->idle();
}

void Core::reshape(int width, int height)
{ 
	c_pMainWindow->resize(width, height);
}

void Core::keyboard(unsigned char key, int x, int y)  
{ 
	c_pMainWindow->keyEvent(key,x,y);
}

void Core::mouse(int button, int state, int x, int y)
{ 
	c_pMainWindow->mouseButtonEvent(button, state,x,y);
}

void Core::move(int x, int y)                         
{ 
	c_pMainWindow->mouseMoveEvent(x,y);
}


void Core::countFPS(int value) {
	char title[120];
	fpsCount = fpsCurrent;
	fpsCurrent = 0;
	
	snprintf(title, 120, "%s. FPS: %d", "Image Match", fpsCount);
	glutSetWindowTitle(title);
	glutTimerFunc(1000, countFPS, 1);
}



