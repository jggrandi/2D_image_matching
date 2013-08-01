
#include "GlutWindow.h"
#include "ArcBall.h"

#include <math.h>
#include <GL/glut.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>


#define DEG2RAD(a) ((a)/57.295)

#define ZOOM_MIN (0.05)
#define ZOOM_MAX (10.0)
//#define DISABLE_DRAGGING

static void handleCgError() 
{
    fprintf(stderr, "Cg error: %s\n", cgGetErrorString(cgGetError()));
   // exit(1);
}


CGlutWindow::CGlutWindow(void)
{
	s_fragmentProfile = s_vertexProfile = CG_PROFILE_UNKNOWN;

	m_pCameraArcball = new CArcBall();
	m_pLightArcball = new CArcBall();

	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (800,600); 
	glutInitWindowPosition (0, 0);
	glutCreateWindow ("GLUT Window");

	m_dFieldOfView = 30.0;
	m_nWidth = 1;
	m_nHeight = 1;

	m_dRadius = 0.5*sqrt(12.0);
	m_vecCameraPosition.set(0.0, 0.0, 0.0,1.0);

	m_dCenter = m_dRadius/ sin(DEG2RAD(m_dFieldOfView)/2.0);

	m_dZoom = 1.0;
	m_dTranslateX = 0.0;
	m_dTranslateY = 0.0;
	m_nMouseState = 0;

	m_bDragging = false;
	m_bZooming = false;
	m_bRotating = false;
	m_bExternalMouseHandling = false;
	initializeAppParameters();
	initializeGL();
	initializeCg();
}

CGlutWindow::~CGlutWindow(void)
{
	if (NULL != m_pCameraArcball) delete m_pCameraArcball;
	if (NULL != m_pLightArcball)  delete m_pLightArcball;
}


void CGlutWindow::initializeCg() 
{

	// Basic Cg setup; register a callback function for any errors
    // and create an initial context
    cgSetErrorCallback(handleCgError);
    s_CgContext = cgCreateContext();

}


void CGlutWindow::renderFrame() {

	double dAspectRatio = double(m_nWidth)/double(m_nHeight);

	// The usual OpenGL stuff to clear the screen and set up viewing.
	glClearColor(.5, .5, 1., 1.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat fTop, fRight, fNear, fFar;

	fNear   = float(m_dCenter - m_dRadius);
	fFar    = float(m_dCenter + m_dRadius);

	if (dAspectRatio > 1.0) {
		fRight = fNear * float(tan(DEG2RAD(m_dFieldOfView)/2.0) /m_dZoom);
		fTop   = fRight * float(dAspectRatio);		
	} else {
		fTop   = fNear * float(tan(DEG2RAD(m_dFieldOfView)/2.0) /m_dZoom);
		fRight = fTop / float(dAspectRatio);
	} 

	glViewport(0,0,m_nWidth,m_nHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum( -fTop, fTop, -fRight, fRight, fNear, 10*fFar);

	CMatrix matModel = m_pCameraArcball->getRotatonMatrix().getInverse();

	double matrix[16];
	matModel.get(matrix[ 0], matrix[ 4], matrix[ 8], matrix[12],
		matrix[ 1], matrix[ 5], matrix[ 9], matrix[13],
		matrix[ 2], matrix[ 6], matrix[10], matrix[14],
		matrix[ 3], matrix[ 7], matrix[11], matrix[15]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(m_dTranslateX * m_dRadius/m_dZoom, m_dTranslateY*m_dRadius/m_dZoom, -m_dCenter);
	glMultMatrixd(matrix);
	//glTranslated(m_vecCameraPosition[0], m_vecCameraPosition[1], m_vecCameraPosition[2]);

	glClear(GL_COLOR_BUFFER_BIT);

    renderGeometry();

	glutSwapBuffers();
}


void CGlutWindow::resize(int width, int height) {
	m_nWidth = width; 
	m_nHeight = height;
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);

	m_pCameraArcball->place(CPosition(width/2, height/2), 0.5 * sqrt(double(width * width + height * height)));
}

void CGlutWindow::keyEvent(unsigned char key,int x,int y)
{
	switch (key) {
		case 'l':
			{
				m_nInteractionMode = MOVE_LIGHT;
			}
			break;
		case 'm':
			{
				m_nMode++;
			}
			break;
				
		case 'M':
			{
				m_nMode--;
			}
			break;		
		case 't':
		case 'T':
			{
				m_bDisplayTF = !m_bDisplayTF;
			}
			break;				
		case 'c':
			{
				m_nInteractionMode = MOVE_CAMERA;
			}
			break;
		case 'r':
		case 'R':
			{
				m_pCameraArcball->reset();
			}
			break;
	}

	glutPostRedisplay();
}

void CGlutWindow::mouseButtonEvent(int button, int state,int x,int y){

	if ((false == m_bDragging) && (false == m_bZooming) && (false == m_bRotating)) {
		if (handleButtonEvent(button, state,x,y)) {
			m_bExternalMouseHandling = true;		
			return;
		}
	}

	m_bExternalMouseHandling = false;	

	CArcBall *pArcball = NULL;
	switch(m_nInteractionMode) {
		default:
		case MOVE_CAMERA:
			pArcball = m_pCameraArcball;
			break;
		case MOVE_LIGHT:
			pArcball = m_pLightArcball;
			break;
	}

	switch(state) {
		case GLUT_DOWN:
			{
				m_vecMouseDown.set(double(x)/double(m_nWidth),double(y)/double(m_nHeight));

				switch (button) {
					case GLUT_LEFT_BUTTON:
						{
							m_nMouseState |= (MBUTTON_LEFT);
							pArcball->setMouse( CPosition(x, m_nHeight-1-y));
							pArcball->startDragging();
							m_bRotating = true;
						}
						break;
					case GLUT_RIGHT_BUTTON:
						{
							m_nMouseState |= (MBUTTON_RIGHT);	
							m_bDragging = false;
							m_bZooming = true;
						}
						break;
					case GLUT_MIDDLE_BUTTON:
						{
							m_nMouseState |= (MBUTTON_MIDDLE);	
#ifdef DISABLE_DRAGGING
							m_bDragging = false;
#else
							m_bDragging = true;
#endif
							m_bZooming = false;
						}
						break;
					default:
						break;
				}
				break;
		case GLUT_UP:
			{
				switch (button) {
					case GLUT_LEFT_BUTTON:
						m_nMouseState ^= MBUTTON_LEFT;
						pArcball->finishDragging();
						break;
					case GLUT_RIGHT_BUTTON:
						m_nMouseState ^= MBUTTON_RIGHT;
						break;
					case GLUT_MIDDLE_BUTTON:
						m_nMouseState ^= MBUTTON_MIDDLE;
						break;
					default:
						break;
				}
				m_bRotating = false;
				m_bDragging = false;
				m_bZooming  = false;
				m_bExternalMouseHandling = false;
				}
			}


	}
}

void CGlutWindow::mouseMoveEvent(int x,int y){
 
	//if ((false == m_bDragging) && (false == m_bZooming) && (false == m_bRotating) && (m_bExternalMouseHandling) {
	if (m_bExternalMouseHandling) {
		if (handleMoveEvent(x,y)) {
			glutPostRedisplay();
		}
		return;
	}

	
	CArcBall *pArcball = NULL;
	switch(m_nInteractionMode) {
		default:
		case MOVE_CAMERA:
			pArcball = m_pCameraArcball;
			break;
		case MOVE_LIGHT:
			pArcball = m_pLightArcball;
			break;
	}

	CPosition pos(double(x)/double(m_nWidth),double(y)/double(m_nHeight));

	CPosition drag = pos-m_vecMouseDown;

	if ((m_bRotating) && (m_nMouseState & (MBUTTON_LEFT))) {
		pArcball->setMouse(CPosition(x, m_nHeight-1- y));
		glutPostRedisplay();

	} else if ((m_bDragging) && (m_nMouseState & MBUTTON_MIDDLE)) {
		
		m_dTranslateX +=  2.0 * drag[0];
		m_dTranslateY += -2.0 * drag[1];

		if (m_dTranslateX >  m_dZoom) m_dTranslateX =  m_dZoom;
		if (m_dTranslateX < -m_dZoom) m_dTranslateX = -m_dZoom;
		if (m_dTranslateY >  m_dZoom) m_dTranslateY =  m_dZoom;
		if (m_dTranslateY < -m_dZoom) m_dTranslateY = -m_dZoom;
		glutPostRedisplay();
	} else if ((m_bZooming) && (m_nMouseState & MBUTTON_RIGHT )) {
			
		double dZoom = 10.0*drag[0]; 
		m_dZoom += dZoom;
		if (m_dZoom < ZOOM_MIN) m_dZoom = ZOOM_MIN;
		if (m_dZoom > ZOOM_MAX) m_dZoom = ZOOM_MAX;
		if (m_dTranslateX >  m_dZoom) m_dTranslateX =  m_dZoom;
		if (m_dTranslateX < -m_dZoom) m_dTranslateX = -m_dZoom;
		if (m_dTranslateY >  m_dZoom) m_dTranslateY =  m_dZoom;
		if (m_dTranslateY < -m_dZoom) m_dTranslateY = -m_dZoom;
		glutPostRedisplay();
	} 
	m_vecMouseDown = pos;
}

void CGlutWindow::idle(){
/*
    __time64_t ltime;
    struct __timeb64 tstruct;

	static bool bFirstTime = true;
	static long nStartMilliSeconds;
	static long nMilliSeconds = 0;
	static long nStartCount   = 0;

	if (bFirstTime) {
		 _time64( &ltime );
		nStartMilliSeconds = (unsigned long) ltime;
		nStartMilliSeconds *= 1000;
		_ftime64( &tstruct );
		nStartMilliSeconds += tstruct.millitm;        
		bFirstTime = false;
		nStartCount = nStartMilliSeconds;
	}

	 _time64( &ltime );
	nMilliSeconds = (unsigned long) ltime;
	nMilliSeconds *= 1000;
	_ftime64( &tstruct );
	nMilliSeconds += tstruct.millitm;        
	bFirstTime = false;

	nMilliSeconds -= nStartMilliSeconds;
    //printf( "Plus milliseconds:%u\n",nMilliSeconds-nStartCount);

	static int nFrameCount = 0;

	nFrameCount++;
	if (nFrameCount > 10) {
		double dFramesPerSecond = double(nFrameCount*1000)/double(nMilliSeconds-nStartCount);
		printf( "fps:%f\n",dFramesPerSecond);
		nFrameCount = 0;
		nStartCount = nMilliSeconds;
	}
*/	
	glutPostRedisplay();
}

void CGlutWindow::initializeGL()
{
    GLfloat ambient[] =
	{0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat diffuse[] =
	{1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specular[] =
	{0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat position[] =
	{1.0f, -5.0f, -5.0f, 1.0f};
    
    GLfloat lmodel_ambient[] =
	{0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat local_view[] =
	{0.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    /* Select Color and Projection*/
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glColor3f(1.0, 1.0, 1.0);
    glShadeModel(GL_SMOOTH);	
}

void CGlutWindow::initializeAppParameters()
{
	m_nMode = 0;
	m_bDisplayTF = false;

	m_pVertices[0] = CVector(-1.0,-1.0,-1.0, 1.0,  0.0, 0.0, 0.0);
	m_pVertices[1] = CVector( 1.0,-1.0,-1.0, 1.0,  1.0, 0.0, 0.0);
	m_pVertices[2] = CVector( 1.0, 1.0,-1.0, 1.0,  1.0, 1.0, 0.0);
	m_pVertices[3] = CVector(-1.0, 1.0,-1.0, 1.0,  0.0, 1.0, 0.0);
															
	m_pVertices[4] = CVector(-1.0,-1.0, 1.0, 1.0,  0.0, 0.0, 1.0);
	m_pVertices[5] = CVector( 1.0,-1.0, 1.0, 1.0,  1.0, 0.0, 1.0);
	m_pVertices[6] = CVector( 1.0, 1.0, 1.0, 1.0,  1.0, 1.0, 1.0);
	m_pVertices[7] = CVector(-1.0, 1.0, 1.0, 1.0,  0.0, 1.0, 1.0);
		
	m_pEdges[0]  = Edge(0,1);
	m_pEdges[1]  = Edge(1,2);
	m_pEdges[2]  = Edge(2,3);
	m_pEdges[3]  = Edge(3,0);
	m_pEdges[4]  = Edge(0,4);
	m_pEdges[5]  = Edge(1,5);
	m_pEdges[6]  = Edge(2,6);
	m_pEdges[7]  = Edge(3,7);
	m_pEdges[8]  = Edge(4,5);
	m_pEdges[9]  = Edge(5,6);
	m_pEdges[10] = Edge(6,7);
	m_pEdges[11] = Edge(7,4);

	m_nNumSlices = 512;
	m_nWidth = 0;
	m_nHeight = 0;

	m_pTransferFunction = new CTransferFunction();
}

bool CGlutWindow::loadTextures() {
	
	FILE *pFile = fopen(FILENAME,"rb");
	if (NULL == pFile) {
		return false;
	}

	int size = XMAX*YMAX*ZMAX;
	unsigned char *pVolume = new unsigned char[size];
	bool ok = (size == fread(pVolume,sizeof(unsigned char), size,pFile));
	fclose(pFile);

	glGenTextures(3,m_pTextureIds);

	glBindTexture(GL_TEXTURE_3D,m_pTextureIds[0]);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D,0,GL_INTENSITY,XMAX,YMAX,ZMAX,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pVolume);

	delete [] pVolume;

    return true;
}


void CGlutWindow::renderGeometry() {

	static bool bInit = false;
	if (false == bInit) {
		chooseProfiles();
		bInit = true;
	}

    // Now make sure that the vertex and fragment programs, loaded
    // in LoadCgPrograms() are bound.
	cgRenderGeometry();

}

void CGlutWindow::cgRenderGeometry() {
	int pViewport[4];
	glGetIntegerv(GL_VIEWPORT,pViewport);

	m_nWidth = pViewport[2];
	m_nHeight = pViewport[3];

	static bool bInit = false;

	if (false == bInit) {
		bInit = loadTextures();
		bInit &= createPrograms();
	}

	int i;
	static int nSlice = 0;
	
	const int m_pEdgeList[8][12] = {
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	}; 

#ifdef COLOR_CODE_EDGES
	double colors[12][3] = {
		{1.0,0.0,0.0},{1.0,0.0,0.0},{1.0,1.0,0.0},{1.0,0.0,0.0},
		{0.0,1.0,0.0},{0.0,1.0,0.0},{0.0,1.0,1.0},{0.0,1.0,0.0},
		{0.0,0.0,1.0},{0.0,0.0,1.0},{1.0,0.0,1.0},{0.0,0.0,1.0}
	};
#endif

	float pMatrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX,pMatrix);

	glDisable(GL_LIGHTING);

	CVector colKeyLight(0.8,0.8,0.8,0.0);
	CVector colFillLight(0.6,0.6,0.5,0.0);
	CVector colBackLight(0.3,0.3,0.2,0.0);

	CMatrix matLight = m_pLightArcball->getRotatonMatrix();
	CVector vecKeyLight  = matLight * CVector(1.0,1.0,1.0,0.0);
	CVector vecFillLight = matLight * CVector(-1.0,1.0,1.0,0.0);
	CVector vecBackLight = matLight * CVector(0.0,1.0,-1.0,0.0);

	
	CVector viewVec(-pMatrix[2],-pMatrix[6],-pMatrix[10],0.0);

	double dMaxDist = viewVec * m_pVertices[0];
	double dMinDist = dMaxDist;
	int nMaxIdx = 0;
	for(int i = 1; i < 8; ++i) {
		double dist = viewVec * m_pVertices[i];
		if ( dist > dMaxDist) {
			dMaxDist = dist;
			nMaxIdx = i;
		}
		if ( dist < dMinDist) {
			dMinDist = dist;
		}

	}

	glEnd();
	
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
#ifndef COLOR_CODE_EDGES
		glColor3f(1.0,1.0,1.0);
#endif
		
		for(int i = 0; i < 12; i++) {
#ifdef COLOR_CODE_EDGES
			glColor3dv(colors[i]);
#endif
			glVertex4dv(&(m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV1])[0]);
			glVertex4dv(&(m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV2])[0]);
		}

	}
	glEnd();

	
	CGprofile vertProfile = s_vertexProfile;
	CGprofile fragProfile = s_fragmentProfile;


	CGprogram vertProg; 
	CGprogram fragProg; 

	switch(m_nMode) {

		default:
		case 0:
			vertProg = m_pVertexPrograms[0];
			fragProg = m_pFragmentPrograms[0];
			break;

		case 1:
			vertProg = m_pVertexPrograms[0];
			fragProg = m_pFragmentPrograms[1];
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"viewVec"),&(viewVec[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"vecKeyLight"),&(vecKeyLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"vecFillLight"),&(vecFillLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"vecBackLight"),&(vecBackLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"colKeyLight"),&(colKeyLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"colFillLight"),&(colFillLight[0]));
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"colBackLight"),&(colBackLight[0]));
			cgGLSetParameter3d(cgGetNamedParameter(fragProg,"k_diff"),0.4,0.2,0.3);
			cgGLSetParameter3d(cgGetNamedParameter(fragProg,"k_spec"),0.4,0.4,0.4);
			cgGLSetParameter3d(cgGetNamedParameter(fragProg,"k_emit"),0.3,0.3,0.3);
			cgGLSetParameter1d(cgGetNamedParameter(fragProg,"shininess"),300.0);
			break;
		case 2:
			vertProg = m_pVertexPrograms[0];
			fragProg = m_pFragmentPrograms[2];
			cgGLSetParameter3dv(cgGetNamedParameter(fragProg,"viewVec"),&(viewVec[0]));
			break;

	}

	
	// Bind uniform parameters to vertex shader
    cgGLSetStateMatrixParameter(cgGetNamedParameter(vertProg, "ModelViewProj"),
                                CG_GL_MODELVIEW_PROJECTION_MATRIX,
                                CG_GL_MATRIX_IDENTITY);

	cgGLSetTextureParameter(   cgGetNamedParameter(fragProg,"volume"),m_pTextureIds[0]);
	cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"volume"));


	m_pTransferFunction->activate(fragProg);
	
	cgGLEnableProfile(vertProfile);
	cgGLEnableProfile(fragProfile);
    cgGLBindProgram(vertProg);
    cgGLBindProgram(fragProg);
	

	CVector vecStart[12];
	CVector vecDir[12];
	float lambda[12];
	float lambda_inc[12];
	double denom;

	dMinDist += EPSILON;
	dMaxDist -= EPSILON;
	double dPlaneDist    =  dMinDist;   
	double dPlaneDistInc =  (dMaxDist-dMinDist)/double(m_nNumSlices);

	float dVertices[12*3];
	float dEdges[12*3];

	for(i = 0; i < 12; i++) {
		vecStart[i] = m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV1];
		vecDir[i]   = m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV2] - m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].nV1];

		denom = vecDir[i] * viewVec;

		if (1.0 + denom != 1.0) {
			lambda_inc[i] =  dPlaneDistInc/denom;
			lambda[i]     = (dPlaneDist - vecStart[i] *viewVec)/denom;
		} else {
			lambda[i]     = -1.0;
			lambda_inc[i] =  0.0;	
		}

		dVertices[3*i]   = vecStart[i][0];
		dVertices[3*i+1] = vecStart[i][1];
		dVertices[3*i+2] = vecStart[i][2];
		dEdges[3*i]      = vecDir[i][0];
		dEdges[3*i+1]    = vecDir[i][1];
		dEdges[3*i+2]    = vecDir[i][2];
	};

	CVector intersection[6];
	float lmb[12];

	for(int n = m_nNumSlices-1; n >= 0; --n) {

		for(int e = 0; e < 12; e++) {
			lmb[e] = lambda[e] + n*lambda_inc[e];
		}

		if      ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[0] = vecStart[0] + lmb[0] * vecDir[0];
		else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[0] = vecStart[1] + lmb[1] * vecDir[1];
		else if ((lmb[3] >= 0.0) && (lmb[3] < 1.0)) intersection[0] = vecStart[3] + lmb[3] * vecDir[3];
		else continue;
		
		if	    ((lmb[2] >= 0.0) && (lmb[2] < 1.0)) intersection[1] = vecStart[2] + lmb[2] * vecDir[2];
		else if ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[1] = vecStart[0] + lmb[0] * vecDir[0];
		else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[1] = vecStart[1] + lmb[1] * vecDir[1];
		else intersection[1] = vecStart[3] + lmb[3] * vecDir[3];

		if      ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[2] = vecStart[4] + lmb[4] * vecDir[4];
		else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[2] = vecStart[5] + lmb[5] * vecDir[5];
		else intersection[2] = vecStart[7] + lmb[7] * vecDir[7];
		
		if	    ((lmb[6] >= 0.0) && (lmb[6] < 1.0)) intersection[3] = vecStart[6] + lmb[6] * vecDir[6];
		else if ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[3] = vecStart[4] + lmb[4] * vecDir[4];
		else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[3] = vecStart[5] + lmb[5] * vecDir[5];
		else intersection[3] = vecStart[7] + lmb[7] * vecDir[7];

		if	    ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[4] = vecStart[8] + lmb[8] * vecDir[8] ;
		else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[4] = vecStart[9] + lmb[9] * vecDir[9] ;
		else intersection[4] = vecStart[11]+ lmb[11]* vecDir[11];
		
		if	    ((lmb[10]>= 0.0) && (lmb[10]< 1.0)) intersection[5] = vecStart[10]+ lmb[10]* vecDir[10];
		else if ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[5] = vecStart[8] + lmb[8] * vecDir[8] ;
		else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[5] = vecStart[9] + lmb[9] * vecDir[9] ;
		else intersection[5] = vecStart[11]+ lmb[11]* vecDir[11];

		for(int i = 0; i < 6; ++i) {
			double dLength = intersection[i].getLength();
		}

		//glEnable(GL_TEXTURE_3D);
		//glBindTexture(GL_TEXTURE_3D, m_pTextureIds[0]);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//glAlphaFunc(GL_GREATER,0.2);
		//glEnable(GL_ALPHA_TEST);
		
		float c = 1.0f-float(n)/float(m_nNumSlices);
		glColor3f(c,c,c);
		glBegin(GL_TRIANGLE_FAN);
		for(int i = 0; i < 6; ++i) {
			intersection[i].glVertex(false,true);
		}
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_3D);
	}

	
	cgGLDisableProfile(vertProfile);
	cgGLDisableProfile(fragProfile);
	if (m_bDisplayTF) {
		m_pTransferFunction->render();
	}
	
}

bool CGlutWindow::handleButtonEvent(int button, int state, int x, int y) {
	if (m_bDisplayTF) {
		bool b =   m_pTransferFunction->handleButtonEvent(button, state, x, m_nHeight-1-y);
		if (b) glutPostRedisplay();
		return b;
	} 
	return false;
}

bool CGlutWindow::handleMoveEvent(int x, int y) {
	if (m_bDisplayTF) {
		bool b =  m_pTransferFunction->handleMoveEvent(x,m_nHeight-1-y);
		if (b) glutPostRedisplay();
		return b;
	} 
	return false;
}


#define NUM_FRAGPROGS 3
#define NUM_VERTPROGS 1

bool CGlutWindow::createPrograms()
{
	CGcontext context = s_CgContext;
	CGprofile vertProfile = s_vertexProfile;
	CGprofile fragProfile = s_fragmentProfile;
    assert(cgIsContext(context));

	m_pVertexPrograms   = new CGprogram[NUM_VERTPROGS];
	m_pFragmentPrograms = new CGprogram[NUM_FRAGPROGS];

	const char *strVertProgFiles[NUM_VERTPROGS] = {"../src/shader/vp_simple.cg"};
	const char *strFragProgFiles[NUM_FRAGPROGS] = {
		"../src/shader/fp_emission_absorption.cg", 
		"../src/shader/fp_emis_abs_gradlightPhong.cg",
		"../src/shader/fp_showGradient.cg"};
	
	cgGLEnableProfile(vertProfile);
	cgGLEnableProfile(fragProfile);

	bool bOK = true;
	for(int i = 0; i < NUM_VERTPROGS; ++i) {
		m_pVertexPrograms[i] = cgCreateProgramFromFile(context, CG_SOURCE, strVertProgFiles[i], vertProfile, NULL, NULL);
		if (NULL == m_pVertexPrograms[i]) {
			bOK = false;
		}

		if (!cgIsProgramCompiled(m_pVertexPrograms[i])){
			cgCompileProgram(m_pVertexPrograms[i]);
		}
		
		cgGLLoadProgram(m_pVertexPrograms[i]);
	}  
    
	for(int i = 0; i < NUM_FRAGPROGS; ++i) {
		m_pFragmentPrograms[i] = cgCreateProgramFromFile(context, CG_SOURCE, strFragProgFiles[i], fragProfile, NULL, NULL);
		if (NULL == m_pFragmentPrograms[i]) {
			bOK = false;
		}

		if (!cgIsProgramCompiled(m_pFragmentPrograms[i])){
			cgCompileProgram(m_pFragmentPrograms[i]);
		}

		cgGLLoadProgram(m_pFragmentPrograms[i]);
	}  

	return bOK;
}


void CGlutWindow::chooseProfiles()
{
    // Make sure that the appropriate profiles are available on the
    // user's system.
    if (cgGLIsProfileSupported(CG_PROFILE_VP40))
        s_vertexProfile = CG_PROFILE_VP40;
    else {
        // try VP30
        if (cgGLIsProfileSupported(CG_PROFILE_VP30))
            s_vertexProfile = CG_PROFILE_VP30;
        else {
            fprintf(stderr, "Neither arbvp1 or vp30 vertex profiles supported on this system.\n");
            //exit(1);
        }
    }
	cgGLSetOptimalOptions(s_vertexProfile);

    if (cgGLIsProfileSupported(CG_PROFILE_FP40))
        s_fragmentProfile = CG_PROFILE_FP40;
    else {
        // try FP30
        if (cgGLIsProfileSupported(CG_PROFILE_FP30))
            s_fragmentProfile = CG_PROFILE_FP30;
        else {
            fprintf(stderr, "Neither arbfp1 or fp30 fragment profiles supported on this system.\n");
            //exit(1);
        }
    }
	cgGLSetOptimalOptions(s_fragmentProfile);
}