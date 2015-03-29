#include <bits/stdc++.h>

using namespace std;

char here[10] = {"here\n"};

class Point3D{
public:
	int x;
	int y;
	int z;
	Point3D(){x=y=z=0;}
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

	void printData(){
		cout<<x<<" "<<y<<" "<<z<<" ";
	}
};

class Vertex{
public:
	int id;
	Point3D p;
	Vertex(){id=-1;};
	Vertex(int _id ,Point3D _p){id=_id; p = _p;}

	bool operator<(const Vertex &pt) const {
		if(p<pt.p)
			return true;
		return false;
	}

	bool operator==(const Vertex &pt) const {
		return p==pt.p;
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

static int noFaces;

class DCELface {
public:
	DCELedge * edge;
	int FaceId;
	DCELface * globalNext;
	DCELface * globalPrev;

	DCELface(){}
	DCELface(int _id, DCELedge *e){
		FaceId = _id;
		edge = e;
		globalNext = NULL;
		globalPrev = NULL;
	}

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
DCEL globalDCEL_x, globalDCEL_y, globalDCEL_z;

typedef vector<Point3D> VPoint3D;
typedef vector<VPoint3D> VVPoint3D; // 6Neighbourhood Connected voxels list

VPoint3D voxels;
VVPoint3D conVoxels;
unordered_map<Point3D, int> voxelMap;

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
		voxels.push_back(tmp);
		voxelMap[tmp] = i;
		if(tmp.x < _minX)
			_minX = tmp.x;
		if(tmp.x > _maxX)
			_maxX = tmp.x;
		if(tmp.y < _minY)
			_minY = tmp.y;
		if(tmp.y > _maxY)
			_maxY = tmp.y;
		if(tmp.z < _minZ)
			_minZ = tmp.z;
		if(tmp.z > _maxZ)
			_maxZ = tmp.z;
	}
	minX = _minX, minY = _minY, minZ = _minZ;
	maxX = _maxX, maxY = _maxY, maxZ = _maxZ;

	//cout<<minX<<" "<<maxX<<" "<<minY<<" "<<maxY<<" "<<minZ<<" "<<maxZ<<endl;
}

DCELedge* addEdgeDCEL(DCEL &dcel, Point3D x , Point3D y , DCELedge * prev = NULL, DCELedge *next =NULL){
	// Scan for edges whether exist or not
	DCELedge * edgelist = dcel.edges;
	DCELedge * previous=NULL;
	while(edgelist !=NULL){
		if(edgelist->origin == x && edgelist->twin->origin == y)
			break;
		previous = edgelist;
		edgelist =edgelist->globalNext;
	}

	if(edgelist==NULL) {

		DCELedge * tmp1 = new DCELedge();
		DCELedge * tmp2 = new DCELedge();
		tmp1->origin = x;
		tmp2->origin = y;
		tmp1->twin = tmp2;
		tmp2->twin = tmp1;

		if(previous!=NULL)
			previous->globalNext = tmp1;
		else
			dcel.edges = tmp1;
		tmp1->globalNext = tmp2;
		tmp2->globalNext = NULL;
		tmp2->globalPrev = tmp1;
		tmp1->globalPrev = previous;

		edgelist = tmp1;
	}

	edgelist->next = next;
	edgelist->prev = prev;
	if(next!=NULL)
		next->prev = edgelist;
	if(prev!=NULL)
		prev->next = edgelist;

	return edgelist;
}

bool DeleteFace(DCEL &dcel, DCELface *face){
	DCELface *facelist = dcel.face;
	if(facelist == face){
		dcel.face = facelist->globalNext;
		if(facelist->globalNext!=NULL)
			facelist->globalNext->globalPrev = NULL;
		delete facelist;
		return true;
	}
	while(facelist!=NULL){
		if(facelist == face){
			if(facelist->globalNext!=NULL){
				facelist->globalNext->globalPrev = facelist->globalPrev;
			}
			facelist->globalPrev->globalNext = facelist->globalNext;
			delete facelist;
			return true;
		}
		facelist = facelist->globalNext;
	}
	return false;
}

DCELface * faceListEnd(DCELface *face){
	if(face==NULL)
		return face;
	while(face->globalNext!=NULL)
		face = face->globalNext;
	return face;
}

DCELedge * edgeListEnd(DCELedge *edge){
	if(edge==NULL)
		return edge;
	while(edge->globalNext!=NULL)
		edge = edge->globalNext;
	return edge;
}

void addFaceDCEL(DCEL &dcel,DCELface * face){
	//cout<<"Called addFaceDCEL\n";
	DCELface * facelist = dcel.face;

	if(facelist==NULL) {
		dcel.face = face;
		return ;
	}
	while(facelist->globalNext != NULL){
		facelist = facelist->globalNext;
	}
	facelist->globalNext = face;
	face->globalPrev = facelist;
}

/*bool deleteEdge(DCELedge * e) {
	DCELedge * edgelist = dcel.edges;
	if(e == edgelist) {
		dcel.edges = edgelist->globalNext;
		delete(e);
		return true;
	}
	while(edgelist == NULL) {
		if(edgelist == e) {
			e->globalPrev->globalNext = e->globalNext;
			e->globalNext->globalPrev = e->globalPrev;
			delete(e);
			return true;
		}
		edgelist = edgelist->globalNext;
	}
	return false;
}*/

DCELface * mergeFace(DCELface * f1,DCELface *f2) {
	//cout<<"Entering mergeFace\n";
	if(f1==NULL){
		//cout<<"f1 is null\n";
		return f2;
	}
	DCELedge *e1 = f1->edge,*s1;
	DCELedge *e2 =  f2->edge,*s2;
	DCELedge *tmp = e2;
	DCELedge *t1,*t2;
	//
	do{
		tmp->face = f1;
		tmp = tmp->next;
	}while(tmp!=e2);

	// go back till no faces common
	bool flag = false;

	do{
		if(e1==tmp->twin){
			e2 = tmp;
			flag = true;
		}
		tmp = tmp->next;
	}while(!flag && tmp!=e2);

	while(e1 ==e2->twin){
		e1 = e1->prev;
		e2 = e2->next;
	}

	s1 = e1;
	s2 = e2;

	// find First Matching
	do{
		flag = false;
		tmp = e2;
		do{
			if(e1==tmp->twin){
				flag = true;
				break;
			}
			tmp = tmp->next;
		}while(!flag && tmp!=e2);

		if(flag){
			t1= e1;
			t2 = e2 = tmp;
			while(e1->next == e2->prev->twin){
				e1 = e1->next;
				e2 = e2->prev;
			}
			t1->prev->next = t2->next;
			t2->next->prev = t1->prev;

			e2->prev->next = e1->next;
			e1->next->prev = e2->prev;

			f1->edge = e1->next;
			// Delete edges from t1 to e1 both included

			break;
		}
		e1 = e1->next;
		//this_thread::sleep_for (chrono::seconds(2));
	}while(e1!=s1 && flag!=true);
	return f1;
}
/*
DCELface * mergeFaceV2(DCELface * f, Point3D v, int level) {
	Point3D a(v.x, v.y, v.z+level);
	Point3D b(v.x+1, v.y, v.z+level);
	Point3D c(v.x+1, v.y+1, v.z+level);
	Point3D d(v.x, v.y+1, v.z+level);

	DCELedge * e = f->edge;
	DCELedge * start = f->edge;
	 do {
		if(e->origin == a && e->twin->origin == d) {
			DCELedge * prev = e->prev;
			DCELedge * next = e->next;
			deleteEdge(e);
			DCELedge * e1 = addEdgeDCEL(a, b, prev);
			e1->face = f;
			DCELedge * e2 = addEdgeDCEL(b, c, e1);
			e2->face = f;
			DCELedge * e3 = addEdgeDCEL(c, d, e2, next);
			e3->face = f;
		}
		if(e->origin == b && e->twin->origin == a) {
			DCELedge * prev = e->prev;
			DCELedge * next = e->next;
			deleteEdge(e);
			DCELedge * e1 = addEdgeDCEL(b, c, prev);
			e1->face = f;
			DCELedge * e2 = addEdgeDCEL(c, d, e1);
			e2->face = f;
			DCELedge * e3 = addEdgeDCEL(d, a, e2, next);
			e3->face = f;
		}
		if(e->origin == c && e->twin->origin == b) {
			DCELedge * prev = e->prev;
			DCELedge * next = e->next;
			deleteEdge(e);
			DCELedge * e1 = addEdgeDCEL(c, d, prev);
			e1->face = f;
			DCELedge * e2 = addEdgeDCEL(d, a, e1);
			e2->face = f;
			DCELedge * e3 = addEdgeDCEL(a, b, e2, next);
			e3->face = f;
		}
		if(e->origin == d && e->twin->origin == c) {
			DCELedge * prev = e->prev;
			DCELedge * next = e->next;
			deleteEdge(e);
			DCELedge * e1 = addEdgeDCEL(d, a, prev);
			e1->face = f;
			DCELedge * e2 = addEdgeDCEL(a, b, e1);
			e2->face = f;
			DCELedge * e3 = addEdgeDCEL(b, c, e2, next);
			e3->face = f;
		}
		e = e->next;
	}while(e!=start);
	return f;
}*/

void printFace(DCELface *face){
	cout<<"Face id = "<<face->FaceId<<endl;
	DCELedge *e,*start;
	e = start = face->edge;
	do{
		e->origin.printData();
		cout<<endl;
		e = e->next;
	}while(e!=start);
	face = face->globalNext;
}

void printFace(DCEL &dcel){
	DCELface * face= dcel.face;
	while(face!=NULL){
		cout<<"Face id = "<<face->FaceId<<endl;
		DCELedge *e,*start;
		e = start = face->edge;
		do{
			e->origin.printData();
			cout<<endl;
			e = e->next;
		}while(e!=start);
		face = face->globalNext;
	}
}

void scanZ() {
	cout<<"In scan z"<<endl;
	int f_id = 1;
	for(int z = minZ; z <= maxZ; z++) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = minX; i <= maxX ; i++) {
			for(int j = minY; j <= maxY ; j++) {
				if(voxelMap.find(Point3D(i, j, z)) != voxelMap.end() && !voxelVisited[Point3D(i, j, z)]) {
					voxelVisited[Point3D(i, j, z)] = true;
					if(voxelMap.find(Point3D(i, j, z+1)) == voxelMap.end()) {
						noFaces++;
						DCELface *f = NULL;
						queue<Point3D> q;
						q.push(Point3D(i, j, z));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) == voxelMap.end()) {
								DCELedge * e1 = addEdgeDCEL(globalDCEL_z, Point3D(p.x, p.y, p.z+1), Point3D(p.x, p.y+1, p.z+1));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_z, Point3D(p.x, p.y+1, p.z+1), Point3D(p.x+1, p.y+1, p.z+1), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_z, Point3D(p.x+1, p.y+1, p.z+1), Point3D(p.x+1, p.y, p.z+1), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_z, Point3D(p.x+1, p.y, p.z+1), Point3D(p.x, p.y, p.z+1), e3, e1);
								DCELface * f1 = new DCELface(0,e1);
									e1->face = e2->face = e3->face = e4->face = f1;
								f = mergeFace(f, f1);
								if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
									q.push(Point3D(p.x+1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
									q.push(Point3D(p.x-1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y+1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y+1, p.z)]) {
									voxelVisited[Point3D(p.x, p.y+1, p.z)] = true;
									q.push(Point3D(p.x, p.y+1, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y-1, p.z)]) {
									voxelVisited[Point3D(p.x, p.y-1, p.z)] = true;
									q.push(Point3D(p.x, p.y-1, p.z));
								}
							}
						}
						f->FaceId = f_id++;
						addFaceDCEL(globalDCEL_z,f);
					}
				}
			}
		}
	}
	cout<<"Scan z reverse"<<endl;
	for(int z = maxZ; z >= minZ; z--) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = minX; i <= maxX; i++) {
			for(int j = minY; j <= maxY; j++) {
				if(voxelMap.find(Point3D(i, j, z)) != voxelMap.end() && !voxelVisited[Point3D(i, j, z)]) {
					voxelVisited[Point3D(i, j, z)] = true;
					if(voxelMap.find(Point3D(i, j, z-1)) == voxelMap.end()) {
						noFaces++;
						DCELface * f = NULL;

						queue<Point3D> q;
						q.push(Point3D(i, j, z));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) == voxelMap.end()) {

								DCELedge * e1 = addEdgeDCEL(globalDCEL_z, Point3D(p.x, p.y, p.z), Point3D(p.x+1, p.y, p.z));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_z, Point3D(p.x+1, p.y, p.z), Point3D(p.x+1, p.y+1, p.z), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_z, Point3D(p.x+1, p.y+1, p.z), Point3D(p.x, p.y+1, p.z), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_z, Point3D(p.x, p.y+1, p.z), Point3D(p.x, p.y, p.z), e3, e1);
								DCELface * f1 = new DCELface(0,e1);
								e1->face = e2->face = e3->face = e4->face = f1;

								f = mergeFace(f, f1);
								if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
									q.push(Point3D(p.x+1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
									q.push(Point3D(p.x-1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y+1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y+1, p.z)]) {
									voxelVisited[Point3D(p.x, p.y+1, p.z)] = true;
									q.push(Point3D(p.x, p.y+1, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y-1, p.z)]) {
									voxelVisited[Point3D(p.x, p.y-1, p.z)] = true;
									q.push(Point3D(p.x, p.y-1, p.z));
								}
							}
						}
						f->FaceId = f_id++;
						addFaceDCEL(globalDCEL_z,f);
					}
				}
			}
		}
	}
	cout<<"Scan z completed"<<endl;

}

void scanY() {
	cout<<"In scan y"<<endl;
	int f_id = 1;
	for(int y = minY; y <= maxY; y++) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = minX; i <= maxX ; i++) {
			for(int j = minZ; j <= maxZ ; j++) {
				if(voxelMap.find(Point3D(i, y, j)) != voxelMap.end() && !voxelVisited[Point3D(i, y, j)]) {
					voxelVisited[Point3D(i, y, j)] = true;
					if(voxelMap.find(Point3D(i, y+1, j)) == voxelMap.end()) {
						noFaces++;
						DCELface *f = NULL;
						queue<Point3D> q;
						q.push(Point3D(i, y, j));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x, p.y+1, p.z)) == voxelMap.end()) {
								DCELedge * e1 = addEdgeDCEL(globalDCEL_y, Point3D(p.x, p.y+1, p.z), Point3D(p.x+1, p.y+1, p.z));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_y, Point3D(p.x+1, p.y+1, p.z), Point3D(p.x+1, p.y+1, p.z+1), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_y, Point3D(p.x+1, p.y+1, p.z+1), Point3D(p.x, p.y+1, p.z+1), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_y, Point3D(p.x, p.y+1, p.z+1), Point3D(p.x, p.y+1, p.z), e3, e1);
								DCELface * f1 = new DCELface(0,e1);
									e1->face = e2->face = e3->face = e4->face = f1;
								f = mergeFace(f, f1);
								if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
									q.push(Point3D(p.x+1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
									q.push(Point3D(p.x-1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z+1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z+1)] = true;
									q.push(Point3D(p.x, p.y, p.z+1));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z-1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z-1)] = true;
									q.push(Point3D(p.x, p.y, p.z-1));
								}
							}
						}
						f->FaceId = f_id++;
						addFaceDCEL(globalDCEL_y,f);
					}
				}
			}
		}
	}
	cout<<"Scan y reverse"<<endl;
	for(int y = maxY; y >= minY; y--) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = minX; i <= maxX; i++) {
			for(int j = minZ; j <= maxZ; j++) {
				if(voxelMap.find(Point3D(i, y, j)) != voxelMap.end() && !voxelVisited[Point3D(i, y, j)]) {
					voxelVisited[Point3D(i, y, j)] = true;
					if(voxelMap.find(Point3D(i, y-1, j)) == voxelMap.end()) {
						noFaces++;
						DCELface * f = NULL;

						queue<Point3D> q;
						q.push(Point3D(i, y, j));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) == voxelMap.end()) {
								
								DCELedge * e1 = addEdgeDCEL(globalDCEL_y, Point3D(p.x, p.y, p.z), Point3D(p.x, p.y, p.z+1));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_y, Point3D(p.x, p.y, p.z+1), Point3D(p.x+1, p.y, p.z+1), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_y, Point3D(p.x+1, p.y, p.z+1), Point3D(p.x+1, p.y, p.z), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_y, Point3D(p.x+1, p.y, p.z), Point3D(p.x, p.y, p.z), e3, e1);
								DCELface * f1 = new DCELface(0,e1);
								e1->face = e2->face = e3->face = e4->face = f1;

								f = mergeFace(f, f1);
								if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
									q.push(Point3D(p.x+1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
									q.push(Point3D(p.x-1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z+1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z+1)] = true;
									q.push(Point3D(p.x, p.y, p.z+1));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z-1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z-1)] = true;
									q.push(Point3D(p.x, p.y, p.z-1));
								}
							}
						}
						f->FaceId = f_id++;
						addFaceDCEL(globalDCEL_y,f);
					}
				}
			}
		}
	}
	cout<<"Scan y completed"<<endl;
}

void scanX() {
	cout<<"In scan x"<<endl;
	int f_id = 1;
	for(int x = minX; x <= maxX; x++) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = minY; i <= maxY ; i++) {
			for(int j = minZ; j <= maxZ ; j++) {
				if(voxelMap.find(Point3D(x, i, j)) != voxelMap.end() && !voxelVisited[Point3D(x, i, j)]) {
					voxelVisited[Point3D(x, i, j)] = true;
					if(voxelMap.find(Point3D(x+1, i, j)) == voxelMap.end()) {
						noFaces++;
						DCELface *f = NULL;
						queue<Point3D> q;
						q.push(Point3D(x, i, j));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) == voxelMap.end()) {
								DCELedge * e1 = addEdgeDCEL(globalDCEL_x, Point3D(p.x+1, p.y, p.z), Point3D(p.x+1, p.y, p.z+1));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_x, Point3D(p.x+1, p.y, p.z+1), Point3D(p.x+1, p.y+1, p.z+1), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_x,  Point3D(p.x+1, p.y+1, p.z+1), Point3D(p.x+1, p.y+1, p.z), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_x, Point3D(p.x+1, p.y+1, p.z), Point3D(p.x+1, p.y, p.z), e3, e1);
								DCELface * f1 = new DCELface(0,e1);
									e1->face = e2->face = e3->face = e4->face = f1;
								f = mergeFace(f, f1);
								if(voxelMap.find(Point3D(p.x, p.y+1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y+1, p.z)]) {
									voxelVisited[Point3D(p.x, p.y+1, p.z)] = true;
									q.push(Point3D(p.x, p.y+1, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y-1, p.z)]) {
									voxelVisited[Point3D(p.x, p.y-1, p.z)] = true;
									q.push(Point3D(p.x, p.y-1, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z+1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z+1)] = true;
									q.push(Point3D(p.x, p.y, p.z+1));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z-1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z-1)] = true;
									q.push(Point3D(p.x, p.y, p.z-1));
								}
							}
						}
						f->FaceId = f_id++;
						addFaceDCEL(globalDCEL_x,f);
					}
				}
			}
		}
	}
	cout<<"Scan x reverse"<<endl;
	for(int x = maxX; x >= minX; x--) {
		unordered_map<Point3D, bool> voxelVisited;
		for(int i = minY; i <= maxY; i++) {
			for(int j = minZ; j <= maxZ; j++) {
				if(voxelMap.find(Point3D(x, i, j)) != voxelMap.end() && !voxelVisited[Point3D(x, i, j)]) {
					voxelVisited[Point3D(x, i, j)] = true;
					if(voxelMap.find(Point3D(x-1, i, j)) == voxelMap.end()) {
						noFaces++;
						DCELface * f = NULL;

						queue<Point3D> q;
						q.push(Point3D(x, i, j));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) == voxelMap.end()) {
								
								DCELedge * e1 = addEdgeDCEL(globalDCEL_x, Point3D(p.x, p.y, p.z), Point3D(p.x, p.y+1, p.z));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_x, Point3D(p.x, p.y+1, p.z), Point3D(p.x, p.y+1, p.z+1), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_x, Point3D(p.x, p.y+1, p.z+1), Point3D(p.x, p.y, p.z+1), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_x, Point3D(p.x, p.y, p.z+1), Point3D(p.x, p.y, p.z), e3, e1);
								DCELface * f1 = new DCELface(0,e1);
								e1->face = e2->face = e3->face = e4->face = f1;

								f = mergeFace(f, f1);
								if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
									q.push(Point3D(p.x+1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
									voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
									q.push(Point3D(p.x-1, p.y, p.z));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z+1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z+1)] = true;
									q.push(Point3D(p.x, p.y, p.z+1));
								}
								if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z-1)]) {
									voxelVisited[Point3D(p.x, p.y, p.z-1)] = true;
									q.push(Point3D(p.x, p.y, p.z-1));
								}
							}
						}
						f->FaceId = f_id++;
						addFaceDCEL(globalDCEL_x,f);
					}
				}
			}
		}
	}	
	cout<<"Scan x completed"<<endl;
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

bool checkEdgeEquality(DCELedge* e){
	int x1,x2,x3;
	int y1,y2,y3;
	int z1,z2,z3;

	x1 = e->origin.x;
	y1 = e->origin.y;
	z1 = e->origin.z;

	x2 = e->next->origin.x;
	y2 = e->next->origin.y;
	z2 = e->next->origin.z;

	x3 = e->next->next->origin.x;
	y3 = e->next->next->origin.y;
	z3 = e->next->next->origin.z;

	if(x1==x2 && x2==x3){
		if(y1==y2 && y2==y3)
			return true;
		else if(z1==z2 && z2==z3)
			return true;
	}
	if(y1==y2 && y1==y3){
		if(x1==x2 && x2==x3)
			return true;
		if(z1==z2 && z2==z3)
			return true;
	}
	return false;
}

/*void reduceEdges(DCEL &dcel){
	DCELface *face = dcel.face;
	DCELedge *e,*s;

	// Creating vertexSet with Ids
	while(face!=NULL){
		e = s = face->edge;
		do{
			if(checkEdgeEquality(e)){
				cout<<"return true\n";
				if(e->next==s){
					s->prev = e->prev;
					e->prev->next = s;
					e = e->next;
				}
				e->next = e->next->next;
				e->next->next->prev = e;
				//can delete e;
			}
			else
				e=e->next;
		}while(e!=s);
		face = face->globalNext;
	}
}*/
static int v_index;

void DCEL2OBJ(DCEL &dcel, fstream &fout){
	set<Vertex> vertexSet;
	

	DCELface *face = dcel.face;
	DCELedge *e,*s;
	Vertex v;
	set<Vertex>::iterator it;

	// fstream fout;
	// fout.open ("out.obj", fstream::out);

	// Creating vertexSet with Ids
	while(face!=NULL){
		e = s = face->edge;
		do{
			v.id = v_index;
			v.p = e->origin;
			if(vertexSet.find(v)==vertexSet.end()){
				vertexSet.insert(v);
				it = vertexSet.find(v);
				fout<<"v "<<(*it).p.x<<" "<<(*it).p.y<<" "<<(*it).p.z<<endl;
				v_index++;
			}
			e = e->next;
		}while(e!=s);
		face = face->globalNext;
	}

	face = dcel.face;
	while(face!=NULL){
		e = s = face->edge;
		fout<<"f";
		do{
			v.id = 0;
			v.p = e->origin;
			it = vertexSet.find(v);
			fout<<" "<<(*it).id;
			e = e->next;
		}while(e!=s);
		face = face->globalNext;
		fout<<endl;
	}
	// fout.close();
}

int main(int argc, char * argv[]) {
	noFaces = 0;
	v_index = 1;
	input();
	scanX();
	cout<<"next"<<endl;
	scanY();
	cout<<"next"<<endl;
	scanZ();
	cout<<"next"<<endl;
	// printFace(globalDCEL);
	// reduceEdges(globalDCEL);
	fstream fout;
	fout.open ("out.obj", fstream::out);
	// set<Vertex> vertexSet;
	DCEL2OBJ(globalDCEL_x, fout);
	DCEL2OBJ(globalDCEL_y, fout);
	DCEL2OBJ(globalDCEL_z, fout);
	fout.close();
	return 0;
}
