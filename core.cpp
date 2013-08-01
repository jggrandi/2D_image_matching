#include "core.h"


CGlutWindow *Core::c_pMainWindow=0;

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
	glutIdleFunc(idle);
	glutMainLoop();
}


void Core::display(void)
{ 
	c_pMainWindow->renderFrame();
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




