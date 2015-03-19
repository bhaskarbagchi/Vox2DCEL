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

int main(int argc, char * argv[]) {
	input();
	connectVoxels();
	return 0;
}
