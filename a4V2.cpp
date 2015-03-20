#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <climits>

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
		bool operator>(const Point3D &pt) const {
			if(x>pt.x)
				return true;
			if(x == pt.x && y > pt.y)
				return true;
			if(x == pt.x && y==pt.y && z > pt.z)
				return true;
			return false;
		}

		bool operator==(const Point3D &pt) const {
			return x == pt.x && y == pt.y && z == pt.z;
		}
};

class Vertex{
	public:
		int id;
		Point3D p;
		Vertex(){}
		Vertex(int _id, Point3D _point) : id(_id), p(_point){}

		bool operator<(const Vertex &pt) const {
			if(p<pt.p)
				return true;
			return false;
		}

		bool operator>(const Vertex &pt) const {
			if(p>pt.p)
				return true;
			return false;
		}

		bool operator==(const Vertex &pt) const {
			return p == pt.p;
		}
};

class Face{
	public:
		vector<int> vertex;	
		Face(){}
};

class DCELEdge{
	public:
	Point3D sVertex;
	DCELEdge *next;
	DCELEdge *prev;

	DCELEdge(Point3D p){
		sVertex = p;
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

typedef vector<Point3D> VPoint3D;
typedef vector<VPoint3D> VVPoint3D; // 6Neighbourhood Connected voxels list

VPoint3D voxels;
VVPoint3D conVoxels;

set<Vertex> vertices;

int insertVertices(Point3D &p,int k){
	pair<std::set<Vertex>::iterator,bool> ret;
	Point3D z;
	z = p;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.x += 1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.y+=1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.x-=1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.y-=1;
	z.z+=1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.x += 1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.y+=1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	z.x-=1;
	ret = vertices.insert(Vertex(k,z));
	if(ret.second!=false)
		k++;
	return k;
}

void input(){
	voxels.clear();
	int index=0;
	Point3D tmp;
	int n;
	scanf("%d",&n);
	while(n--){
		scanf("%d%d%d",&tmp.x,&tmp.y,&tmp.z);
		voxels.pb(tmp);
		index = insertVertices(tmp,index);
	}
	cout<<vertices.size()<<endl;
}

void connectVoxels() {
	conVoxels.clear();
	unordered_map<Point3D, int> voxelMap;
	unordered_map<Point3D, bool> voxelVisited;
	int sz = voxels.size();
	for(int i = 0; i < sz; i++) {
		voxelMap[voxels[i]] = i;
	}
	for (int i = 0; i < sz; ++i) {
		if(!voxelVisited[voxels[i]]) {
			//BFS
			vector<Point3D> v;
			queue<int> q;
			q.push(voxelMap[voxels[i]]);
			voxelVisited[voxels[i]] = true;
			while(!q.empty()) {
				Point3D p = voxels[q.front()];
				v.push_back(p);
				q.pop();
				if(voxelMap.find(Point3D(p.x-1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x-1, p.y, p.z)]) {
					q.push(voxelMap[Point3D(p.x-1, p.y, p.z)]);
					voxelVisited[Point3D(p.x-1, p.y, p.z)] = true;
				}
				if(voxelMap.find(Point3D(p.x+1, p.y, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x+1, p.y, p.z)]) {
					q.push(voxelMap[Point3D(p.x+1, p.y, p.z)]);
					voxelVisited[Point3D(p.x+1, p.y, p.z)] = true;
				}
				if(voxelMap.find(Point3D(p.x, p.y-1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y-1, p.z)]) {
					q.push(voxelMap[Point3D(p.x, p.y-1, p.z)]);
					voxelVisited[Point3D(p.x, p.y-1, p.z)] = true;
				}
				if(voxelMap.find(Point3D(p.x, p.y+1, p.z)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y+1, p.z)]) {
					q.push(voxelMap[Point3D(p.x, p.y+1, p.z)]);
					voxelVisited[Point3D(p.x, p.y+1, p.z)] = true;
				}
				if(voxelMap.find(Point3D(p.x, p.y, p.z-1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z-1)]) {
					q.push(voxelMap[Point3D(p.x, p.y, p.z-1)]);
					voxelVisited[Point3D(p.x, p.y, p.z-1)] = true;
				}
				if(voxelMap.find(Point3D(p.x, p.y, p.z+1)) != voxelMap.end() && !voxelVisited[Point3D(p.x, p.y, p.z+1)]) {
					q.push(voxelMap[Point3D(p.x, p.y, p.z+1)]);
					voxelVisited[Point3D(p.x, p.y, p.z+1)] = true;
				}
			}
			conVoxels.push_back(v);
		}
	}
}

bool sortVertexZ(Point3D &a , Point3D &b){
	return(a.z<b.z);
}

void ZaxisScan(){
	//must be removed 
	conVoxels.pb(voxels);

	VPoint3D vList;
	int sz = conVoxels.size();
	cout<<"Total Number Of List ="<<sz<<endl;
	for(int i=0;i<sz;i++){
		vList = conVoxels[i];
		int zmin = INT_MAX;
		int zmax = INT_MIN;
		int sz2 = vList.size();

		for(int j=0;j<sz2;j++){
			if(vList[j].z < zmin) zmin = vList[j].z;
			if(vList[j].z >zmax) zmax = vList[j].z;
		}

		VPoint3D zSorted;
		unordered_map<Point3D, bool> voxelVisited;
		zSorted = sort(vList.begin(),vList.end(),sortVertexZ);
		int index=0;
		
		// Front
		while(index<sz2){
			if(voxelVisited.find(zSorted[index])==voxelVisited.end()){
				index++; continue;
			}

			int z = zSorted[index].z;

			queue<Point3D> q;
			q.push(zSorted[index]);
			while(!q.empty()){
				// Create Faces and Joing them
			}
			index++;
		}

		// Back
		voxelVisited.clear();
		index = sz2-1
		while(index>=0){
			if(voxelVisited.find(zSorted[index])==voxelVisited.end()){
				index++; continue;
			}
			int z = zSorted[index].z;
			queue<Point3D> q;
			q.push(zSorted[index]);
			while(!q.empty()){
				// Create Faces and Joing them
			}
			index++;
		}

	}

}

int main(int argc, char * argv[]) {
	input();
	//connectVoxels();
	//ZaxisScan();
	return 0;
}
