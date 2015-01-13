#include "mapa.h"
#include "data.h"

#ifndef EXPAND_H
#define EXPAND_H

typedef std::vector< Point > Vector2dVector;

class Expand{
	public:
		static bool Process(const Vector2dVector &contour, Vector2dVector &result, double distance);
};


#endif
