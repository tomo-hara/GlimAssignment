#pragma once

// CProcess 명령 대상

class CProcess : public CObject
{
public:
	CProcess();
	virtual ~CProcess();

	void getStartInfo(CImage *pImage, CRect rect, POINT centerPos, double dRadius, int nThick, int nBlack);
};


