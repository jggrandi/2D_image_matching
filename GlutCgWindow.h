#pragma once
#include "GlutWindow.h"
#include <Cg/cg.h>

class CGlutCgWindow :
	public CGlutWindow
{
public:
	CGlutCgWindow(void);
	virtual ~CGlutCgWindow(void);

	virtual void renderGeometry();
	virtual void cgRenderGeometry();
	static inline CGcontext context()     { return s_CgContext;}
	static inline CGprofile fragProfile() { return s_fragmentProfile;}
	static inline CGprofile vertProfile() { return s_vertexProfile;}

private:
	static CGcontext s_CgContext;
	static CGprofile s_vertexProfile, s_fragmentProfile;
	
	void chooseProfiles();
	

	virtual void initializeCg();
};
