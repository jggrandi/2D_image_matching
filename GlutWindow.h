#pragma once

#include "mathutil/CVector.h"
#include "mathutil/CPosition.h"

class CArcBall;

class CGlutWindow
{
public:
	CGlutWindow(void);
	~CGlutWindow(void);

	enum {
		MOVE_CAMERA,
		MOVE_LIGHT,
	};

	enum {
		MBUTTON_LEFT   = 0x01, 
		MBUTTON_MIDDLE = 0x02,
		MBUTTON_RIGHT  = 0x04,
		KBUTTON_CTRL   = 0x08,
		KBUTTON_SHIFT  = 0x10
	};

	void renderFrame();
	void idle();
	void resize(int width, int height);
	void keyEvent(unsigned char key,int x,int y);
	void mouseButtonEvent(int button, int state,int x,int y);
	void mouseMoveEvent(int x,int y);
	void renderGeometry();
	bool handleButtonEvent(int button, int state, int x, int y);
	bool handleMoveEvent(int x, int y);

protected:
	int m_nWidth, m_nHeight;
	double m_dFieldOfView;
	double m_dCenter, m_dRadius, m_dZoom;
	
	CArcBall* m_pCameraArcball;
	CArcBall* m_pLightArcball;

	CPosition m_vecMouseDown;
	bool m_bDragging;
	bool m_bZooming;
	bool m_bRotating;
	bool m_bExternalMouseHandling;
	
	CVector m_vecCameraPosition;
	double m_dTranslateX, m_dTranslateY;

	int m_nMouseState;
	int m_nInteractionMode;

	void initializeGL();

};



