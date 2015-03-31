#include <bits/stdc++.h>

using namespace std;

static int noFaces;
static int v_index;
static int colorCount;

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

class DCELface {
public:
	DCELedge * edge;
	int FaceId;
	DCELface * globalNext;
	DCELface * globalPrev;

	DCELface();
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
	DCELedge *edgeEnd;
	DCELface *faceEnd;
	DCEL(){
		vertices=NULL;
		edges = NULL;
		face = NULL;
		edgeEnd =NULL;
		faceEnd = NULL;
	}

};
typedef DCELedge* EdgePtr;

class MapVertex{
public:
	Point3D p;
	mutable vector<EdgePtr> edges;
	bool operator<(const MapVertex &pt) const {
		if(p<pt.p)
			return true;
		return false;
	}

	bool operator==(const MapVertex &pt) const {
		return p==pt.p;
	}
};
set<MapVertex> edgeVertexs;

DCEL globalDCEL_x, globalDCEL_y, globalDCEL_z;
DCEL globalDCEL_x_rev, globalDCEL_y_rev, globalDCEL_z_rev;

typedef vector<Point3D> VPoint3D;
VPoint3D voxels;

unordered_map<Point3D, int> voxelMap;

int minX, minY, minZ, maxX, maxY, maxZ;

void input() {
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
}

DCELedge* addEdgeDCEL(DCEL &dcel, Point3D x , Point3D y , DCELedge * prev = NULL, DCELedge *next =NULL){
	MapVertex v,v2;
	DCELedge *e=NULL;
	pair<set<MapVertex>::iterator,bool> ret,ret2;
	set<MapVertex>::iterator it,it2;
	
	v.p = x;
	v2.p = y;
	ret.second = ret2.second = false;
	
	it = edgeVertexs.find(v);
	if(it==edgeVertexs.end()){
		ret = edgeVertexs.insert(v);
		it = ret.first;
	}
	it2 = edgeVertexs.find(v2);
	if(it2==edgeVertexs.end()){
		ret2 = edgeVertexs.insert(v2);
		it2 = ret2.first;
	}
	
	if(!ret.second && !ret2.second){
		int size = (*it).edges.size();
		for(int i=0;i<size;i++){
			if((*it).edges[i]->twin->origin == y){
				DCELedge *e = (*it).edges[i];
				e->next = next;
				e->prev = prev;
				if(next!=NULL)
					next->prev = e;
				if(prev!=NULL)
					prev->next = e;
				return e;
			}
		}
	}
	
	DCELedge * tmp1 = new DCELedge();
	DCELedge * tmp2 = new DCELedge();
	tmp1->origin = x;
	tmp2->origin = y;
	tmp1->twin = tmp2;
	tmp2->twin = tmp1;

	if(dcel.edgeEnd !=NULL){
		dcel.edgeEnd->globalNext = tmp1;
	}
	else{
		dcel.edges = tmp1;
	}
	tmp1->globalNext = tmp2;
	tmp2->globalNext = NULL;
	tmp2->globalPrev = tmp1;
	tmp1->globalPrev = dcel.edgeEnd;
	dcel.edgeEnd = tmp2;

	tmp1->next = next;
	tmp1->prev = prev;
	if(next!=NULL)
		next->prev = tmp1;
	if(prev!=NULL)
		prev->next = tmp1;

	(*it).edges.push_back(tmp1);
	(*it2).edges.push_back(tmp2);

	return tmp1;
}

bool DeleteFace(DCEL &dcel, DCELface *face) {
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

DCELface * mergeFace(DCELface * f1,DCELface *f2) {
	//cout<<"Called Merge Face\n";
	if(f1==NULL) {
		return f2;
	}
	DCELedge *e1 = f1->edge,*s1;
	DCELedge *e2 =  f2->edge,*s2;
	DCELedge *tmp = e2;
	DCELedge *t1,*t2;
	do {
		tmp->face = f1;
		tmp = tmp->next;
	} while(tmp!=e2);
	bool flag = false;
	do {
		if(e1==tmp->twin) {
			e2 = tmp;
			flag = true;
		}
		tmp = tmp->next;
	} while(!flag && tmp!=e2);
	while(e1 ==e2->twin) {
		e1 = e1->prev;
		e2 = e2->next;
	}
	s1 = e1;
	s2 = e2;
	do {
		flag = false;
		tmp = e2;
		do {
			if(e1==tmp->twin) {
				flag = true;
				break;
			}
			tmp = tmp->next;
		} while(!flag && tmp!=e2);
		if(flag) {
			t1= e1;
			t2 = e2 = tmp;
			while(e1->next == e2->prev->twin) {
				e1 = e1->next;
				e2 = e2->prev;
			}
			t1->prev->next = t2->next;
			t2->next->prev = t1->prev;
			e2->prev->next = e1->next;
			e1->next->prev = e2->prev;
			f1->edge = e1->next;
			break;
		}
		e1 = e1->next;
	} while(e1!=s1 && flag!=true);
	return f1;
}

void printFace(DCELface *face) {
	cout<<"Face id = "<<face->FaceId<<endl;
	DCELedge *e,*start;
	e = start = face->edge;
	do {
		e->origin.printData();
		cout<<endl;
		e = e->next;
	} while(e!=start);
	face = face->globalNext;
}

void printFace(DCEL &dcel) {
	DCELface * face= dcel.face;
	while(face!=NULL) {
		cout<<"Face id = "<<face->FaceId<<endl;
		DCELedge *e,*start;
		e = start = face->edge;
		do {
			e->origin.printData();
			cout<<endl;
			e = e->next;
		} while(e!=start);
		face = face->globalNext;
	}
}

bool sortVertexZ(const Point3D &a ,const Point3D &b){
	return(a.z<b.z);
}
bool sortVertexY(const Point3D &a ,const Point3D &b){
	return(a.y<b.y);
}
bool sortVertexX(const Point3D &a ,const Point3D &b){
	return(a.x<b.x);
}

void scanZ() {
	edgeVertexs.clear();
	cout<<"In scan z"<<endl;
	int f_id = 1;
	sort(voxels.begin(),voxels.end(),sortVertexZ);
	int size = voxels.size();
	int x,y,z;
	unordered_map<Point3D, bool> voxelVisited;
	
	for(int i = 0; i<size;){
		z = voxels[i].z;
		do{
			y = voxels[i].y;
			x = voxels[i].x;
			if(voxelMap.find(Point3D(x, y, z)) != voxelMap.end() && !voxelVisited[Point3D(x, y, z)]) {
				voxelVisited[Point3D(x, y, z)] = true;
				if(voxelMap.find(Point3D(x, y, z+1)) == voxelMap.end()) {
					noFaces++;
					DCELface *f = NULL;
					queue<Point3D> q;
					q.push(Point3D(x, y, z));
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
			i++;
		}while(i<size && voxels[i].z == z);
	}
	
	cout<<"Scan z reverse"<<endl;
	edgeVertexs.clear();
	voxelVisited.clear();
	
	for(int i = size-1; i>=0;){
		z = voxels[i].z;
		do{
			y = voxels[i].y;
			x = voxels[i].x;
			if(voxelMap.find(Point3D(x, y, z)) != voxelMap.end() && !voxelVisited[Point3D(x, y, z)]) {
				voxelVisited[Point3D(x, y, z)] = true;
				if(voxelMap.find(Point3D(x, y, z-1)) == voxelMap.end()) {
					noFaces++;
					DCELface * f = NULL;
					queue<Point3D> q;
					q.push(Point3D(x, y, z));
					while(!q.empty()) {
						Point3D p = q.front();
						q.pop();
						if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) == voxelMap.end()) {
							DCELedge * e1 = addEdgeDCEL(globalDCEL_z_rev, Point3D(p.x, p.y, p.z), Point3D(p.x+1, p.y, p.z));
							DCELedge * e2 = addEdgeDCEL(globalDCEL_z_rev, Point3D(p.x+1, p.y, p.z), Point3D(p.x+1, p.y+1, p.z), e1);
							DCELedge * e3 = addEdgeDCEL(globalDCEL_z_rev, Point3D(p.x+1, p.y+1, p.z), Point3D(p.x, p.y+1, p.z), e2);
							DCELedge * e4 = addEdgeDCEL(globalDCEL_z_rev, Point3D(p.x, p.y+1, p.z), Point3D(p.x, p.y, p.z), e3, e1);
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
					addFaceDCEL(globalDCEL_z_rev,f);
				}
			}
			i--;
		}while(i>=0 && voxels[i].z == z);
	}
	cout<<"Scan z completed"<<endl;
}

void scanY() {
	edgeVertexs.clear();
	cout<<"In scan Y"<<endl;
	int f_id = 1;
	sort(voxels.begin(),voxels.end(),sortVertexY);
	int size = voxels.size();
	int x,y,z;
	unordered_map<Point3D, bool> voxelVisited;
	
	for(int i = 0; i<size;){
		y = voxels[i].y;
		do{
			z = voxels[i].z;
			x = voxels[i].x;
			if(voxelMap.find(Point3D(x, y, z)) != voxelMap.end() && !voxelVisited[Point3D(x, y, z)]) {
				voxelVisited[Point3D(x, y, z)] = true;
				if(voxelMap.find(Point3D(x, y+1, z)) == voxelMap.end()) {
					noFaces++;
					DCELface *f = NULL;
					queue<Point3D> q;
					q.push(Point3D(x, y, z));
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
			i++;
		}while(i<size && voxels[i].y == y);
	}
	
	cout<<"Scan Y reverse"<<endl;
	edgeVertexs.clear();
	voxelVisited.clear();
	
	for(int i = size-1; i>=0;){
		y = voxels[i].y;
		do{
			z = voxels[i].z;
			x = voxels[i].x;
			if(voxelMap.find(Point3D(x, y, z)) != voxelMap.end() && !voxelVisited[Point3D(x, y, z)]) {
					voxelVisited[Point3D(x, y, z)] = true;
					if(voxelMap.find(Point3D(x, y-1, z)) == voxelMap.end()) {
						noFaces++;
						DCELface * f = NULL;
						queue<Point3D> q;
						q.push(Point3D(x, y, z));
						while(!q.empty()) {
							Point3D p = q.front();
							q.pop();
							if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) == voxelMap.end()) {
								DCELedge * e1 = addEdgeDCEL(globalDCEL_y_rev, Point3D(p.x, p.y, p.z), Point3D(p.x, p.y, p.z+1));
								DCELedge * e2 = addEdgeDCEL(globalDCEL_y_rev, Point3D(p.x, p.y, p.z+1), Point3D(p.x+1, p.y, p.z+1), e1);
								DCELedge * e3 = addEdgeDCEL(globalDCEL_y_rev, Point3D(p.x+1, p.y, p.z+1), Point3D(p.x+1, p.y, p.z), e2);
								DCELedge * e4 = addEdgeDCEL(globalDCEL_y_rev, Point3D(p.x+1, p.y, p.z), Point3D(p.x, p.y, p.z), e3, e1);
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
						addFaceDCEL(globalDCEL_y_rev,f);
					}
				}
			i--;
		}while(i>=0 && voxels[i].y == y);
	}
	cout<<"Scan y completed"<<endl;
}

void scanX() {
	
	edgeVertexs.clear();
	cout<<"In scan X"<<endl;
	int f_id = 1;
	sort(voxels.begin(),voxels.end(),sortVertexX);
	int size = voxels.size();
	int x,y,z;
	unordered_map<Point3D, bool> voxelVisited;
	
	for(int i = 0; i<size;){
		x = voxels[i].x;
		do{
			z = voxels[i].z;
			y = voxels[i].y;
			if(voxelMap.find(Point3D(x, y, z)) != voxelMap.end() && !voxelVisited[Point3D(x, y, z)]) {
					voxelVisited[Point3D(x, y, z)] = true;
					if(voxelMap.find(Point3D(x+1, y, z)) == voxelMap.end()) {
						noFaces++;
						DCELface *f = NULL;
						queue<Point3D> q;
						q.push(Point3D(x, y, z));
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
			i++;
		}while(i<size && voxels[i].x == x);
	}
	
	cout<<"Scan X reverse"<<endl;
	edgeVertexs.clear();
	voxelVisited.clear();
	
	for(int i = size-1; i>=0;){
		x = voxels[i].x;
		do{
			y = voxels[i].y;
			z = voxels[i].z;
			if(voxelMap.find(Point3D(x, y, z)) != voxelMap.end() && !voxelVisited[Point3D(x, y, z)]) {
				voxelVisited[Point3D(x, y, z)] = true;
				if(voxelMap.find(Point3D(x-1, y, z)) == voxelMap.end()) {
					noFaces++;
					DCELface * f = NULL;
					queue<Point3D> q;
					q.push(Point3D(x, y, z));
					while(!q.empty()) {
						Point3D p = q.front();
						q.pop();
						if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) == voxelMap.end()) {
							DCELedge * e1 = addEdgeDCEL(globalDCEL_x_rev, Point3D(p.x, p.y, p.z), Point3D(p.x, p.y+1, p.z));
							DCELedge * e2 = addEdgeDCEL(globalDCEL_x_rev, Point3D(p.x, p.y+1, p.z), Point3D(p.x, p.y+1, p.z+1), e1);
							DCELedge * e3 = addEdgeDCEL(globalDCEL_x_rev, Point3D(p.x, p.y+1, p.z+1), Point3D(p.x, p.y, p.z+1), e2);
							DCELedge * e4 = addEdgeDCEL(globalDCEL_x_rev, Point3D(p.x, p.y, p.z+1), Point3D(p.x, p.y, p.z), e3, e1);
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
					addFaceDCEL(globalDCEL_x_rev,f);
				}
			}
			i--;
		}while(i>=0 && voxels[i].x == x);
	}
	cout<<"Scan X completed"<<endl;
}

bool checkEdgeEquality(DCELedge* e) {
	int x1,x2,x3;
	int y1,y2,y3;
	int z1,z2,z3;

	x1 = e->origin.x;
	y1 = e->origin.y;
	z1 = e->origin.z;

	x2 = e->prev->origin.x;
	y2 = e->prev->origin.y;
	z2 = e->prev->origin.z;

	x3 = e->prev->prev->origin.x;
	y3 = e->prev->prev->origin.y;
	z3 = e->prev->prev->origin.z;

	if(x1==x2 && x2==x3) {
		if(y1==y2 && y2==y3)
			return true;
		else if(z1==z2 && z2==z3)
			return true;
	}
	if(y1==y2 && y1==y3) {
		if(z1==z2 && z2==z3)
			return true;
	}
	return false;
}

void reduceEdges(DCEL &dcel) {
	DCELface *face = dcel.face;
	DCELedge *e,*s;
	while(face!=NULL){
		e = s = face->edge;
		do{
			if(checkEdgeEquality(e->next->next)) {
				if(e->next==s) {
					s->prev = e->prev;
					e->prev->next = s;
					e = e->next;
				}
				e->next = e->next->next;
				e->next->next->prev = e;
			}
			else
				e=e->next;
		}while(e->next->next!=s);
		face = face->globalNext;
	}
}

void DCEL2OBJ(DCEL &dcel, fstream &fout, set<Vertex> &vertexSet) {
	DCELface *face = dcel.face;
	DCELedge *e,*s;
	Vertex v;
	set<Vertex>::iterator it;
	while(face!=NULL) {
		e = s = face->edge;
		do{
			v.id = v_index;
			v.p = e->origin;
			if(vertexSet.find(v)==vertexSet.end()) {
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
	while(face!=NULL) {
		e = s = face->edge;
		// fout<<"usemtl col"<<colorCount%9<<endl;
		fout<<"f";
		do {
			v.id = 0;
			v.p = e->origin;
			it = vertexSet.find(v);
			fout<<" "<<(*it).id;
			e = e->next;
		} while(e!=s);
		face = face->globalNext;
		fout<<endl;
		colorCount++;
	}
}

int main(int argc, char * argv[]) {
	
	cout<<argc<<endl;
	return 0;
	noFaces = 0;
	v_index = 1;
	colorCount = 0;
	input();
	scanX();
	cout<<"next"<<endl;
	scanY();
	cout<<"next"<<endl;
	scanZ();
	cout<<"next"<<endl;
	reduceEdges(globalDCEL_x);
	reduceEdges(globalDCEL_x_rev);
	reduceEdges(globalDCEL_y);
	reduceEdges(globalDCEL_y_rev);
	reduceEdges(globalDCEL_z);
	reduceEdges(globalDCEL_z_rev);
	fstream fout;
	if(argc >= 2)
		fout.open (argv[1], fstream::out);
	else
		fout.open ("out.obj", fstream::out);
	// fout<<"mtllib assignment3.mtl\n";
	set<Vertex> vertexSet;
	DCEL2OBJ(globalDCEL_x, fout, vertexSet);
	DCEL2OBJ(globalDCEL_x_rev, fout, vertexSet);
	DCEL2OBJ(globalDCEL_y, fout, vertexSet);
	DCEL2OBJ(globalDCEL_y_rev, fout, vertexSet);
	DCEL2OBJ(globalDCEL_z, fout, vertexSet);
	DCEL2OBJ(globalDCEL_z_rev, fout, vertexSet);
	fout.close();
	cout<<"VertexSet size = "<<vertexSet.size()<<endl;
	return 0;
}
