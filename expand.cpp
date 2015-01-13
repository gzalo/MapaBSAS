#include "expand.h"

bool Expand::Process(const Vector2dVector &contour,Vector2dVector &result, double distance){
	double cX=0.0, cY=0.0;

	for(size_t i=0;i<contour.size();i++){
		cX += contour[i].x;
		cY += contour[i].y;
	}
	cX/=contour.size();
	cY/=contour.size();
	
	for(size_t i=0;i<contour.size();i++){
		double x = (contour[i].x - cX) * distance + cX;
		double y = (contour[i].y - cY) * distance + cY;
		
		result.push_back((Point){x,y});
	}
		
	return true;
}