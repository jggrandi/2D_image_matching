#include "Trapez.h"
#include <GL/glut.h>

#define OFFSET (5)

CTrapez::CTrapez() 
{
	
	m_nStart	= 0;
	m_nMid1		= 0;
	m_nMid2		= 0;
	m_nEnd		= 0;
	m_nHeight	= 0;
	for(int i = 0; i < 4; ++i) {
		m_pPoints[i].set(0.0,0.0);
	}
	m_color.set(1.0,1.0,1.0,0.0);
}

CTrapez::~CTrapez() 
{

}

void CTrapez::setTrapez(int nStart, int nMid1, int nMid2,  int nEnd,  int nHeight) {
	
	m_nStart = nStart;
	m_nMid1 = nMid1;
	m_nMid2 = nMid2;
	m_nEnd = nEnd;
	m_nHeight = nHeight;

	m_pPoints[0].set(nStart,0);
	m_pPoints[1].set(nMid1,nHeight);
	m_pPoints[2].set(nMid2,nHeight);
	m_pPoints[3].set(nEnd,0);

}

void CTrapez::render() 
{
	glLineWidth(1.0);
	glColor4f(m_color[0],m_color[1],m_color[2],0.5);
	glBegin(GL_QUADS);
	glVertex2i(m_nStart,256);
	glVertex2i(m_nMid1, 256-m_nHeight);
	glVertex2i(m_nMid2, 256-m_nHeight);
	glVertex2i(m_nEnd, 256);
	glEnd();

	glColor4f(m_color[0],m_color[1],m_color[2],1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2i(m_nStart,256);
	glVertex2i(m_nMid1, 256-m_nHeight);
	glVertex2i(m_nMid2, 256-m_nHeight);
	glVertex2i(m_nEnd, 256);
	glEnd();

	glPointSize(8.0);
	glBegin(GL_POINTS);
	glVertex2i(m_nStart,256);
	glVertex2i(m_nMid1, 256-m_nHeight);
	glVertex2i(m_nMid2, 256-m_nHeight);
	glVertex2i(m_nEnd, 256);
	glEnd();
	
}

bool
CTrapez::handleButtonEvent(int button, int state, int x, int y) {
	
	switch(state) {
	case GLUT_DOWN:
		{
			int nDummy;
			m_ptMouseDown = CPosition(x,y);
			m_nDragMode = classifyPoint(m_ptMouseDown, nDummy);
			m_bDragValid = true;
		}
		return (m_nDragMode != NO_INTERACTION);

	case GLUT_UP:
		{
			m_nDragMode	= NO_INTERACTION;
			m_bDragValid = false;
		}
		return true;
	}

	return false;
}

bool 
CTrapez::handleMoveEvent(int x, int y) {
	
	CPosition point(x,y);

	bool retval = false;
	int nSel = m_nSelectedPoint;
	//double dW= dEnd-dStart;
	
	InteractionState nOldState = m_nState;
	m_nState = classifyPoint(point,m_nSelectedPoint);
	
	if ((nOldState != m_nState) || (nSel != m_nSelectedPoint)) {
		retval = true;	
	}	
	
	if (m_bDragValid) {
		int nMoveX  = int(point[0] - m_ptMouseDown[0]);
		int nMoveY  = -int(point[1] - m_ptMouseDown[1]);
		
		switch(m_nDragMode) {
		case MOVE:
			if ((m_nEnd + nMoveX) > TF_TABLE_SIZE-1)   { nMoveX = TF_TABLE_SIZE-1-m_nEnd;}
			if ((m_nStart + nMoveX) < 0) { nMoveX = -m_nStart; }
			if ((m_nMid2 + nMoveX) > TF_TABLE_SIZE-1)   { nMoveX = TF_TABLE_SIZE-1-m_nMid2;}
			if ((m_nMid1 + nMoveX) < 0)   { nMoveX = -m_nMid1; }
			m_nStart += nMoveX;
			m_nMid1  += nMoveX;
			m_nEnd   += nMoveX;
			m_nMid2  += nMoveX;
			retval = true; 
			break;

		case MOVE_0:
			m_nStart += nMoveX;
			if (m_nStart > m_nMid1) m_nStart = m_nMid1;
			if (m_nStart < 0)		m_nStart = 0;
			retval = true; 
			break;

		case MOVE_1:
			m_nHeight -= nMoveY;
			if (m_nHeight > 255) m_nHeight = 255;
			if (m_nHeight < 0) m_nHeight = 0;
			m_nMid1 += nMoveX;
			if (m_nMid1 > m_nMid2)   m_nMid1 = m_nMid2;
			if (m_nMid1 < m_nStart)	 m_nMid1 = m_nStart;
			retval = true; 
			break;
		
		case MOVE_2:
			m_nHeight -= nMoveY;
			if (m_nHeight > 255) m_nHeight = 255;
			if (m_nHeight < 0)   m_nHeight = 0;
			//asymmetric movement
			m_nMid2 += nMoveX;
			if (m_nMid2 > m_nEnd)  m_nMid2 = m_nEnd;
			if (m_nMid2 < m_nMid1) m_nMid2 = m_nMid1;
			
			retval = true; 
			break;
		
		case MOVE_3:
			m_nEnd += nMoveX;
			if (m_nEnd > TF_TABLE_SIZE-1) m_nEnd = TF_TABLE_SIZE-1;
			if (m_nEnd < m_nMid2)         m_nEnd = m_nMid2;
			retval = true; 
			break;

		case MOVE_0_AND_1:
			if ((m_nMid1 + nMoveX) > m_nMid2) { nMoveX = m_nMid2-m_nMid1;}
			if ((m_nStart + nMoveX) < 0)      { nMoveX = -m_nStart; }
			m_nStart += nMoveX;
			m_nMid1 += nMoveX;
			retval = true;
			break;
			
		case MOVE_2_AND_3:
			if ((m_nEnd + nMoveX) > TF_TABLE_SIZE-1)   { nMoveX = TF_TABLE_SIZE-1-m_nEnd;}
			if ((m_nMid2 + nMoveX) < m_nMid1) { nMoveX = m_nMid1-m_nMid2; }
			m_nEnd += nMoveX;
			m_nMid2 += nMoveX;
			retval = true; 
			break;

		case MOVE_1_AND_2:
			{
			m_nHeight -= nMoveY;
			if (m_nHeight > 255) m_nHeight = 255;
			if (m_nHeight < 0)   m_nHeight = 0;
			
			int nTmp = m_nMid2 + nMoveX;
			if (nTmp > m_nEnd) nMoveX = m_nEnd - m_nMid2;
			nTmp = m_nMid1 + nMoveX;
			if (nTmp < m_nStart) nMoveX = m_nStart - m_nMid1;

			m_nMid2 += nMoveX;			
			m_nMid1 += nMoveX;
			retval = true; 
			}
			break;
		default:
			break;
		}
	} else {
		m_bDragValid = false;
	}

	m_pPoints[0].set(m_nStart,0);
	m_pPoints[1].set(m_nMid1,m_nHeight);
	m_pPoints[2].set(m_nMid2,m_nHeight);
	m_pPoints[3].set(m_nEnd,0);
	m_ptMouseDown = point;
	return retval;
}

CTrapez::InteractionState 
CTrapez::classifyPoint(CPosition point, int &nSelectedPoint) {
	
	int i;
	CPosition dist[4];
	nSelectedPoint = -1;
	
	// is the point close to one of the selection points?
	for(i = 0; i < 4; i++) {
		dist[i] = m_pPoints[i] - point;
		if (dist[i][0] < 0) dist[i][0] = - dist[i][0];
		if (dist[i][1] < 0) dist[i][1] = - dist[i][1];
		
		if ((dist[i][0] < OFFSET) && (dist[i][1] < OFFSET)) {
			nSelectedPoint = i;
			return InteractionState(int(1<< i) );
		}
	}	

	// have we selected the left edge? CASE: edge is almost vertical
	if ((point[1] < m_pPoints[1][1]) && (dist[0][0] < OFFSET) && (dist[1][0] < OFFSET)) {
		return MOVE_0_AND_1;
	}
	
	// have we selected the left edge? CASE: edge is not almost vertical
	if (m_pPoints[1][0]-m_pPoints[0][0] > 1.0) {
		double dAlpha = (m_pPoints[1][1]-point[1])/ (m_pPoints[1][1]-m_pPoints[0][1]);
		double dX3 = m_pPoints[1][0]  + dAlpha * (m_pPoints[0][0] - m_pPoints[1][0]);
		
		dX3 -= point[0];
		if (dX3 < 0.0) dX3 = -dX3;
		if (dX3 < OFFSET) {
			return MOVE_0_AND_1;
		} 
	}
	
	// have we selected the right edge? CASE: edge is almost vertical
	if ((point[1] < m_pPoints[1][1]) && (dist[2][0] < OFFSET) && (dist[3][0] < OFFSET)) {
		return MOVE_2_AND_3;
	}
	
	// have we selected the right edge? CASE: edge is not almost vertical
	if (m_pPoints[3][0]-m_pPoints[2][0] > 1.0) {
		double dAlpha = (m_pPoints[3][1]-point[1]) /(m_pPoints[3][1]-m_pPoints[2][1]);
		double dX3 = m_pPoints[3][0]  + dAlpha * (m_pPoints[2][0] - m_pPoints[3][0]);
		dX3 -= point[0];
		if (dX3 < 0.0) dX3 = -dX3;
		if (dX3 < OFFSET) {
			return MOVE_2_AND_3;
		} 
	}
	
	// have we selected the top edge?
	if ((dist[1][1] < 3) && (point[0] > m_pPoints[1][0]) && (point[0] < m_pPoints[2][0]) ) {
		return MOVE_1_AND_2;
	} 
	
	// have we selected whole region?
	if ((point[1] < m_pPoints[1][1]) && (point[0] > m_pPoints[1][0]) && (point[0] < m_pPoints[2][0])) {
		return MOVE;
	}
	if ((point[0] > m_pPoints[2][0]) && (point[0] < m_pPoints[3][0])) {
		double dAlpha = (m_pPoints[3][0]-point[0]) /(m_pPoints[3][0]-m_pPoints[2][0]);
		if (point[1] < dAlpha*m_pPoints[2][1])
			return MOVE;
	}
	if ((point[0] > m_pPoints[0][0]) && (point[0] < m_pPoints[1][0])) {
		double dAlpha = (m_pPoints[0][0]-point[0]) /(m_pPoints[0][0]-m_pPoints[1][0]);
		if (point[1] < (dAlpha)*m_pPoints[1][1])
			return MOVE;
	}

	// seems we haven't selected anything!
	return NO_INTERACTION;
}

int CTrapez::value(int nPos)
{
	if ((nPos > m_nEnd) || (nPos < m_nStart)) return 0;
	
	if (nPos < m_nMid1) {
		double dAlpha = double(nPos-m_nStart)/double(m_nMid1-m_nStart);
		return int(dAlpha * double(m_nHeight));
	}

	if (nPos > m_nMid2) {
		double dAlpha = double(nPos-m_nEnd)/double(m_nMid2-m_nEnd);
		return int(dAlpha * double(m_nHeight));
	}

	return m_nHeight;

}