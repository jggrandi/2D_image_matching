#pragma once

#include "mathutil/CVector.h"
#include "Trapez.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include "utils.h"

#define MAX_NUM_TRAPEZOIDS (8)


class CTransferFunction;
typedef CTransferFunction *PTRANSFERFUNCTION;

class CTransferFunction{

public:
	CTransferFunction();
	virtual ~CTransferFunction();

	virtual void render();
	virtual void activate(CGprogram fragmentShader);
	virtual bool handleButtonEvent(int button, int state, int x, int y);
	virtual bool handleMoveEvent(int x, int y);
	inline int getMin() {return m_nMin;}
	inline int getMax() {return m_nMax;}

protected:
	unsigned char m_pColorTable[TF_TABLE_SIZE*4];
	int m_nNumTrapezoids;
	int m_nMin, m_nMax;
	CTrapez m_pTrapezoids[MAX_NUM_TRAPEZOIDS];

private:
	void updateColorTable(int nStart, int nEnd);
	void updateMinMax();
	void initTexture();

	GLuint m_nTexId;

};
