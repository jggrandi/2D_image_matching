#include "TransferFunction.h"

#include <assert.h>

#define OFFSET_X (10)
#define OFFSET_Y (10)
#define MARGIN_LEFT  (4)
#define MARGIN_TOP   (8)
#define MARGIN_RIGHT (4)
#define MARGIN_BOTTOM (32)

CTransferFunction::CTransferFunction() 
{
	m_nNumTrapezoids = 1;
	m_nTexId = 0;
	float fvalue = 0.0;
	float fstep  = 255.0/float(TF_TABLE_SIZE);

	CPosition pPos[4];

	m_pTrapezoids[0].setTrapez(32,200,255,255,255);
	

}
CTransferFunction::~CTransferFunction() 
{

}

void CTransferFunction::render() 
{
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(OFFSET_X,OFFSET_Y,MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE, 256+MARGIN_TOP+MARGIN_BOTTOM);
	// setup for planar 2D rendering
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0,float(MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE),float(256+MARGIN_TOP+MARGIN_BOTTOM),0.0,0.0,2.0);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	// blending
	glEnable(GL_BLEND);
	glAlphaFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// draw TF area
	glBegin(GL_QUADS);
	glColor4f(0.0,0.0,0.0,0.5);
	glVertex2i(0,0);
	glVertex2i(MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE,0);
	glVertex2i(MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE, 256+MARGIN_TOP+MARGIN_BOTTOM);
	glVertex2i(0,256+MARGIN_TOP+MARGIN_BOTTOM);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor4f(0.0,0.0,0.0,1.0);
	glVertex2i(0,0);
	glVertex2i(MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE,0);
	glVertex2i(MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE, 256+MARGIN_TOP+MARGIN_BOTTOM);
	glVertex2i(0,256+MARGIN_TOP+MARGIN_BOTTOM);
	glEnd();

	glTranslated(MARGIN_LEFT,MARGIN_TOP,0.0);

	for(int i = 0; i < m_nNumTrapezoids; ++i) {
		m_pTrapezoids[i].render();
	}

	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glColor4f(1.0,1.0,1.0,1.0);
	glVertex2i(-1,-1);
	glVertex2i(TF_TABLE_SIZE+2, -1);
	glVertex2i(TF_TABLE_SIZE+2, 257);
	glVertex2i(-1,257);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


}


bool CTransferFunction::handleButtonEvent(int button, int state, int x, int y) {
	if ((x < OFFSET_X+MARGIN_LEFT+MARGIN_RIGHT+TF_TABLE_SIZE) && ( y < OFFSET_Y+MARGIN_TOP+MARGIN_BOTTOM+256)) {
		m_pTrapezoids[0].handleButtonEvent(button, state, x-OFFSET_X-MARGIN_LEFT, y-OFFSET_Y-MARGIN_BOTTOM);
		return true;
	}
	return false;

}

bool CTransferFunction::handleMoveEvent(int x, int y) {

	int nOldStart, nOldEnd, nStart,nEnd;
	//if ((x < TF_TABLE_SIZE) && ( y < 256)) {
		for(int i = 0; i < m_nNumTrapezoids; ++i) {
			nOldStart = m_pTrapezoids[i].start();
			nOldEnd = m_pTrapezoids[i].end();
			if (m_pTrapezoids[i].handleMoveEvent(x-OFFSET_X-MARGIN_LEFT, y-OFFSET_Y-MARGIN_BOTTOM)) {
				nStart = m_pTrapezoids[i].start();
				nEnd = m_pTrapezoids[i].end();
				if (nStart > nOldStart) nStart = nOldStart;
				if (nEnd < nOldEnd) nEnd = nOldEnd;
				updateColorTable(nStart,nEnd);
				break;
			}
		}
		return true;
	//}
	//return false;
}

void CTransferFunction::activate(CGprogram fragProg) {

	static bool bInit = false;
	if (bInit == false) {
		initTexture();
		updateColorTable(0,TF_TABLE_SIZE-1);
		bInit = true;
	}
	
	cgGLSetTextureParameter( cgGetNamedParameter(fragProg,"colortable"),m_nTexId);
	cgGLEnableTextureParameter(cgGetNamedParameter(fragProg,"colortable"));

}

void CTransferFunction::initTexture() {

	glGenTextures(1,&m_nTexId);
	glBindTexture(GL_TEXTURE_1D,m_nTexId);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,TF_TABLE_SIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,m_pColorTable);

}


void CTransferFunction::updateColorTable(int nStart, int nEnd)
{
	if (nStart < 0) nStart = 0;
	if (nEnd < 0) return;
	if (nStart > TF_TABLE_SIZE) return;
	if (nEnd > TF_TABLE_SIZE) nEnd = TF_TABLE_SIZE;
		
	double dAlphaMax, dAlphaSum, dRed, dGreen, dBlue, dValue, dHeight, dFactor;
	CVector color;
	int nIndex = nStart * 4;
	
	for(int nStep = nStart; nStep <= nEnd; nStep++) {
		
		dAlphaMax = 0.0;
		dAlphaSum = 0.0;
		dRed      = 0.0;
		dGreen    = 0.0;
		dBlue     = 0.0;
		
		for(int p = 0; p < m_nNumTrapezoids; p++) {
			dHeight = m_pTrapezoids[p].height()/255.0;
			dValue  = m_pTrapezoids[p].value(nStep)/255.0;
			dFactor = dValue/dHeight;
			if (dValue > dAlphaMax) dAlphaMax = dValue;
			dAlphaSum += dValue;
			if (dValue > 0.0) {
				color = m_pTrapezoids[p].color();
				dRed   += dValue * dFactor * color[0];
				dGreen += dValue * dFactor * color[1];
				dBlue  += dValue * dFactor * color[2];
			}
		}

		dRed   /=dAlphaSum;
		dGreen /= dAlphaSum;
		dBlue  /= dAlphaSum;
		m_pColorTable[nIndex++] = (unsigned char) (dRed      * 255.0);
		m_pColorTable[nIndex++] = (unsigned char) (dGreen    * 255.0);
		m_pColorTable[nIndex++] = (unsigned char) (dBlue     * 255.0);
		m_pColorTable[nIndex++] = (unsigned char) (dAlphaMax * 255.0);
	}
	
	assert(m_nTexId != 0);
	glBindTexture(GL_TEXTURE_1D,m_nTexId);

	// set the texture parameters
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,TF_TABLE_SIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,m_pColorTable);

	updateMinMax();
}


void CTransferFunction::updateMinMax() {

	m_nMin = m_pTrapezoids[0].start();
	m_nMax = m_pTrapezoids[0].end();

	for(int i = 1; i < m_nNumTrapezoids; ++i) {
		if (m_pTrapezoids[i].start() < m_nMin) m_nMin = m_pTrapezoids[i].start();
		if (m_pTrapezoids[i].end() < m_nMax) m_nMax = m_pTrapezoids[i].end();
	}
}