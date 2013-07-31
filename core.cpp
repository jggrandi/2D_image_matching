#include "core.h"


CGlutWindow *Core::c_pMainWindow=0;
int   Core::core_argc=0;
char  **Core::core_argv=NULL;


Core::Core(int c_argc, char **c_argv)
{
	core_argc = c_argc;
	core_argv = c_argv;
	glutInit(&core_argc, core_argv);
	c_pMainWindow = new CGlutWindow();	
}

Core::~Core(){}

bool Core::setup( DATAINFO c_img1, DATAINFO c_img2, OPT c_options)
{
	if(!c_handleData.loadData(c_img1,c_img2,c_options))
		return 0;
	return 1;	
}


void Core::run()
{
	c_handleData.similarityCheck();	
	
	glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(move);
	glutIdleFunc(idle);
	printf("AA\n");
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




