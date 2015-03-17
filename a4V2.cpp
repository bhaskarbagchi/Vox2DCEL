#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <climits>

#define pb push_back

using namespace std;

class Point3D{
  int x;
  int y;
  int z;
  Point3D(){}
  Point3D(int _x, int _y, int _z) : x(_x), y(_y), z(_z){}
};

typedef vector<Point3D> VPoint3D;

typedef vector<VPoint3D> VVPoint3D; // 6Neighbourhood Connected voxels list

VPoint3D voxels;
VVPoint3D conVoxels;


void input(){
	voxels.clear();

	Point3D tmp;

	int n;
	scanf("%d",&n);
	while(n--){
		scanf("%d%d%d",&tmp.x,&tmp.y,&tmp.z);
		voxels.pb(tmp);
	}
}
 
void connectVoxels(){


	
}


int main(){
	return 0;
}
