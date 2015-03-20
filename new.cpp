#include <bits/stdc++.h>

#define pb push_back

using namespace std;

class Point3D{
public:
	int x;
	int y;
	int z;
	Point3D(){}
	Point3D(int _x, int _y, int _z) : x(_x), y(_y), z(_z){}

	bool operator<(const Point3D &pt) const {
		if(x<pt.x)
			return true;
		if(x == pt.x && y < pt.y)
			return true;
		if(x == pt.x && y==pt.y && z < pt.z)
			return true;
		return false;
	}

	bool operator==(const Point3D &pt) const {
		return x == pt.x && y == pt.y && z == pt.z;
	}
};

namespace std {
	template <>
	struct hash<Point3D> {
		std::size_t operator()(const Point3D& pt) const {
			return pt.x*1e6 + pt.y * 1e3 + pt.z;
		}
	};
}

class DCELvertex;
class DCELedge;
class DCELface;

class DCELvertex {
public:
	Point3D coordinates;
	DCELedge * leaving;
	DCELvertex * globalNext;
	DCELvertex * globalPrev;
};

class DCELedge {
public:
	Point3D origin;
	DCELedge * twin;
	DCELedge * next;
	DCELedge * prev;
	DCELface * face;
	DCELedge * globalNext;
	DCELedge * globalPrev;
};

static noFaces;

class DCELface {
public:
	DCELedge * edge;
	int FaceId;
	DCELface * globalNext;
	DCELface * globalPrev;
};

class DCEL{
public:
	DCELvertex * vertices;
	DCELedge *edges;
	DCELface *face;
	DCEL(){
		vertices=NULL;
		edges = NULL;
		face = NULL;
	}

};
DCEL globalDCEL;

typedef vector<Point3D> VPoint3D;
typedef vector<VPoint3D> VVPoint3D; // 6Neighbourhood Connected voxels list

VPoint3D voxels;
VVPoint3D conVoxels;
unordered_map<Point3D, int> voxelMap;
// unordered_map<Point3D, bool> voxelVisited;

int minX, minY, minZ, maxX, maxY, maxZ;

void input(){
	voxels.clear();
	int _minX = INT_MAX;
	int _minY = INT_MAX;
	int _minZ = INT_MAX;
	int _maxX = INT_MIN;
	int _maxY = INT_MIN;
	int _maxZ = INT_MIN;
	Point3D tmp;
	int n;
	scanf("%d",&n);
	for(int i = 0; i < n; i++) {
		scanf("%d%d%d",&tmp.x,&tmp.y,&tmp.z);
		voxels.pb(tmp);
		voxelMap[tmp] = i;
		if(tmp.x < _minX)
			_minX = tmp.x;
		else if(tmp.x > _maxX)
			_maxX = tmp.x;
		if(tmp.y < _minY)
			_minY = tmp.y;
		else if(tmp.y > _maxY)
			_maxY = tmp.y;
		if(tmp.z < _minZ)
			_minZ = tmp.z;
		else if(tmp.z > _maxZ)
			_maxZ = tmp.z;
	}
	for(int i = 0; i < n; i++) {
		voxels[i].x-=(minX-1);
		voxels[i].y-=(minY-1);
		voxels[i].z-=(minZ-1);
	}
	minX = _minX, minY = _minY, minZ = _minZ;
	maxX = _maxX, maxY = _maxY, maxZ = _maxZ;
}

DCELedge* addEdgeDCEL(Point3D x , Point3D y , DCELedge * prev = NULL, DCELedge *next =NULL){
	// Scan for edges whether exist or not
	DCELedge *edgelist = globalDCEL.edges;
	DCELedge *previous=NULL;
	while(edgelist !=NULL){
		if(edgelist.origin == x && edgelist.twin.origin == y)
			break;
		previous = edgelist;
		edgelist =edgelist.globalNext;
	}

	if(edgelist==NULL){

		DCELedge *tmp1 = new DCELedge();
		DCELedge *tmp2 = new DCELedge(); 
		tmp1.origin = x;
		tmp2.origin = y;
		tmp1.twin = tmp2;
		tmp2.twin = tmp1;
		
		if(previous!=NULL)
			previous.globalNext = tmp1;
		else
			globalDCEL.edges = tmp1;
		tmp1.globalNext = tmp2;
		tmp2.globalNext = NULL;
		tmp2.globalPrev = tmp1;
		tmp1.globalPrev = previous;

		edgelist = tmp1;
	}
	
	edgelist.next = next;
	edgelist.prev = prev;
	if(next!=NULL)
		next.prev = edgelist;
	if(prev!=NULL)
		prev.next = edgelist;

	return edgelist;
}

DCELface * addFaceDCEL(int x, DCELedge *edge){
	DCELface * facelist = globalDCEL.facelist;
	
	DCELface * tmp = new DCELface();
	tmp.FaceId = id;
	tmp.edge = edge;
	tmp.globalPrev = NULL;
	tmp.globalNext = NULL;

	if(facelist==NULL){
		globalDCEL.facelist = tmp;
		return tmp;
	}
	while(facelist.globalNext!=NULL){
		facelist = facelist.globalNext;
	}
	facelist.globalNext = tmp;
	tmp.globalPrev = facelist;
	return tmp;
}

void scanZ() {
	
	for(int z = 1; z <= maxZ - minZ; z++) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = 1; i <= maxX - minX; i++) {
			for(int j = 1; j <= maxY - minY; j++) {
				if(voxelMap[Point3D(i, j, z)] != voxelMap.end() && !voxelVisited[Point3D(i, j, z)]) {
					voxelVisited[Point3D(i, j, z)] = true;
					if(voxelMap[Point3D(i, j, z+1)] != voxelMap.end()) {
						DCELedge * e1 = addEdgeDCEL(Point3D(i, j, z+1), Point3D(i+1, j, z+1));
						DCELedge * e2 = addEdgeDCEL(Point3D(i+1, j, z+1), Point3D(i+1, j+1, z+1),e1);
						DCELedge * e3 = addEdgeDCEL(Point3D(i+1, j+1, z+1), Point3D(i, j+1, z+1).e2);
						DCELedge * e4 = addEdgeDCEL(Point3D(i, j+1, z+1), Point3D(i, j, z+1),e3,e1);
						noFaces++;
						DCELface * f = addFaceDCEL(noFaces, e);

						queue<Point3D> q;
						q.push(Point3D(i, j, z));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap[Point3D(p.x, p.y, p.z+1)] != voxelMap.end()) {
								f = mergeFace(f, p);
								if(voxelMap[Point3D(i+1, j, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i+1, j, z)] = true;
									q.push(Point3D(i+1, j, z));
								}
								if(voxelMap[Point3D(i-1, j, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i-1, j, z)] = true;
									q.push(Point3D(i-1, j, z));
								}
								if(voxelMap[Point3D(i, j+1, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i, j+1, z)] = true;
									q.push(Point3D(i, j+1, z));
								}
								if(voxelMap[Point3D(i, j-1, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i, j-1, z)] = true;
									q.push(Point3D(i, j-1, z));
								}
							}
						}
					}
				}
			}
		}
	}

	for(int z = maxZ - minZ - 1; z > 1; z--) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = 1; i <= maxX - minX; i++) {
			for(int j = 1; j <= maxY - minY; j++) {
				if(voxelMap[Point3D(i, j, z)] != voxelMap.end() && !voxelVisited[Point3D(i, j, z)]) {
					voxelVisited[Point3D(i, j, z)] = true;
					if(voxelMap[Point3D(i, j, z-1)] != voxelMap.end()) {
						DCELedge * e = addEdgeDCEL(Point3D(i, j, z), Point3D(i+1, j, z));
						addEdgeDCEL(Point3D(i+1, j, z), Point3D(i+1, j+1, z));
						addEdgeDCEL(Point3D(i+1, j+1, z), Point3D(i, j+1, z));
						addEdgeDCEL(Point3D(i, j+1, z), Point3D(i, j, z));
						noFaces++;
						DCELface * f = addFaceDCEL(noFaces, e);

						queue<Point3D> q;
						q.push(Point3D(i, j, z));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap[Point3D(p.x, p.y, p.z-1)] != voxelMap.end()) {
								f = mergeFace(f, p);
								if(voxelMap[Point3D(i+1, j, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i+1, j, z)] = true;
									q.push(Point3D(i+1, j, z));
								}
								if(voxelMap[Point3D(i-1, j, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i-1, j, z)] = true;
									q.push(Point3D(i-1, j, z));
								}
								if(voxelMap[Point3D(i, j+1, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i, j+1, z)] = true;
									q.push(Point3D(i, j+1, z));
								}
								if(voxelMap[Point3D(i, j-1, z)] != voxelMap.end()) {
									voxelVisited[Point3D(i, j-1, z)] = true;
									q.push(Point3D(i, j-1, z));
								}
							}
						}
					}
				}
			}
		}
	}


}

// void connectVoxels() {
// 	conVoxels.clear();
// 	unordered_map<Point3D, int> voxelMap;
// 	unordered_map<Point3D, bool> voxelVisited;
// 	int sz = voxels.size();
// 	for(int i = 0; i < sz; i++) {
// 		voxelMap[voxels[i]] = i;
// 	}
// 	for (int i = 0; i < sz; ++i) {
// 		if(!voxelVisited[voxels[i]]) {
// 			//BFS
// 			vector<Point3D> v;
// 			queue<int> q;
// 			q.push(voxelMap[voxels[i]]);
// 			voxelVisited[voxels[i]] = true;
// 			while(!q.empty()) {
// 				Point3D p = voxels[q.front()];
// 				v.push_back(p);
// 				q.pop();
// 				if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
// 					q.push(voxelMap[Point3D(p.x-1, p.y, p.z)]);
// 					voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
// 				}
// 				if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
// 					q.push(voxelMap[Point3D(p.x+1, p.y, p.z)]);
// 					voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
// 				}
// 				if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y-1, p.z)]) {
// 					q.push(voxelMap[Point3D(p.x, p.y-1, p.z)]);
// 					voxelVisited[Point3D(p.x, p.y-1, p.z)] = true;
// 				}
// 				if(voxelMap.find(Point3D(p.x, p.y+1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y+1, p.z)]) {
// 					q.push(voxelMap[Point3D(p.x, p.y+1, p.z)]);
// 					voxelVisited[Point3D(p.x, p.y+1, p.z)] = true;
// 				}
// 				if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z-1)]) {
// 					q.push(voxelMap[Point3D(p.x, p.y, p.z-1)]);
// 					voxelVisited[Point3D(p.x, p.y, p.z-1)] = true;
// 				}
// 				if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z+1)]) {
// 					q.push(voxelMap[Point3D(p.x, p.y, p.z+1)]);
// 					voxelVisited[Point3D(p.x, p.y, p.z+1)] = true;
// 				}
// 			}
// 			conVoxels.push_back(v);
// 		}
// 	}
// }

int main(int argc, char * argv[]) {
	noFaces = 0;
	input();
	connectVoxels();
	return 0;
}
