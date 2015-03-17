#include <iostream>

using namespace std;

class Point3D {
public:
	double x, y, z;
};

enum Normal {
	X+, X-,
	Y+, Y-,
	Z+, Z-
};

class DCELvertex;
class DCELedge;
class DCELface;

class DCELvertex {
public:
	Point3D coordinates;
	DCELedge * 	leaving;
};

class DCELedge {
public:
	Point3D origin;
	DCELedge * twin;
	DCELedge * next;
	DCELedge * prev;
	DCELface * face;
};

class DCELface {
public:
	DCELedge * edge;
	Normal normalTo;
};

