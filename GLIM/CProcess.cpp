// CProcess.cpp: 구현 파일
#include "pch.h"
#include "GLIM.h"
#include "CProcess.h"

// CProcess

CProcess::CProcess()
{
}

CProcess::~CProcess()
{
}

// CProcess 멤버 함수

// (x, y) 가 정원 가장자리에 해당하는 좌표인지 확인한다.
__inline static bool isThickPos(int x, int y, int centerX, int centerY, double dRadius, int nThick)
{
	bool bRet = false;
	double dDist = sqrt((centerX - x) * (centerX - x) + (centerY - y) * (centerY - y));
	if (dRadius - nThick < dDist && dDist < dRadius) {
		bRet = true;
	}
	return bRet;
}

void CProcess::getStartInfo(CImage *pImage, CRect rect, POINT centerPos, double dRadius, int nThick, int nBlack)
{
	int nSttx = rect.left;
	int nStty = rect.top;
	int nEndx = rect.right;
	int nEndy = rect.bottom;

	unsigned char *fm = (unsigned char *)pImage->GetBits();
	int nPitch = pImage->GetPitch();
	for (int j = nStty; j < nEndy; j++) {
		for (int i = nSttx; i < nEndx; i++) {
			if (isThickPos(i, j, centerPos.x, centerPos.y, dRadius, nThick)) {
				fm[j * nPitch + i] = nBlack;
			}
		}
	}
}