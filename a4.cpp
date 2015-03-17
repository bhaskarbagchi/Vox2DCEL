#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <climits>

using namespace std;

class point {
public:
	int x, y, z;
	
	point() : x(0), y(0), z(0) {};
	point(int a, int b, int c) : x(a), y(b), z(c) {};
};

/*
 * Implement DCEL here
 */

void voxelToGrid(point voxel, vector<point> &vertices, vector<vector<int> > &faces) {
	int noVertices = vertices.size();
	int x = voxel.x, y = voxel.y, z = voxel.z;
	vertices.push_back(voxel);											//LFB
	vertices.push_back(point(voxel.x + 1, voxel.y, voxel.z));			//RFB
	vertices.push_back(point(voxel.x + 1, voxel.y + 1, voxel.z));		//RFT
	vertices.push_back(point(voxel.x, voxel.y + 1, voxel.z));			//LFT
	vertices.push_back(point(voxel.x, voxel.y, voxel.z + 1));			//LBB
	vertices.push_back(point(voxel.x + 1, voxel.y, voxel.z + 1));		//RBB
	vertices.push_back(point(voxel.x + 1, voxel.y + 1, voxel.z + 1));	//RBT
	vertices.push_back(point(voxel.x, voxel.y + 1, voxel.z + 1));		//LBT
	vector<int> f1, f2, f3, f4, f5, f6;
	f1.push_back(noVertices + 1), f1.push_back(noVertices + 2), f1.push_back(noVertices + 3), f1.push_back(noVertices + 4);		//Front
	f2.push_back(noVertices + 8), f2.push_back(noVertices + 7), f2.push_back(noVertices + 6), f2.push_back(noVertices + 5);		//Back
	f3.push_back(noVertices + 1), f3.push_back(noVertices + 4), f3.push_back(noVertices + 8), f3.push_back(noVertices + 5);		//Left
	f4.push_back(noVertices + 4), f4.push_back(noVertices + 3), f4.push_back(noVertices + 7), f4.push_back(noVertices + 8);		//Top
	f5.push_back(noVertices + 3), f5.push_back(noVertices + 2), f5.push_back(noVertices + 6), f5.push_back(noVertices + 7);		//Right
	f6.push_back(noVertices + 2), f6.push_back(noVertices + 1), f6.push_back(noVertices + 5), f6.push_back(noVertices + 6);		//Bottom
	faces.push_back(f1);
	faces.push_back(f2);
	faces.push_back(f3);
	faces.push_back(f4);
	faces.push_back(f5);
	faces.push_back(f6);
	return;
}

void convertToDCEL(vector<point> &vertices, vector<vector<int> > &faces) {

}

void minimizeFaces() {

}

void createObjFile(vector<point> vertices, vector<vector<int> > faces) {
	FILE * fp = fopen("3D.obj", "w");
	int noVertices = vertices.size();
	int noFaces = faces.size();
	for(int i = 0; i < noVertices; i++) {
		fprintf(fp, "v %d %d %d\n", vertices[i].x, vertices[i].y, vertices[i].z);
	}
	for(int i = 0; i < noFaces; i++) {
		fprintf(fp, "f ");
		int no = faces[i].size();
		for(int j = 0; j < no; j++) {
			fprintf(fp, "%d ", faces[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return;
}

bool compare (point a, point b) {
	if(a.z == b.z) {
		if(a.y == b.y)
			return a.x < b.x;
		return a.y < b.y;
	}
	return a.z < b.z;
}

int main(int argc, char * argv[]) {
	int n;
	cin>>n;
	vector<point> voxels(n);
	vector<point> vertices;
	vector<vector<int> > faces;
	int minX = INT_MAX, minY = INT_MAX, minZ = INT_MAX, maxX = INT_MIN, maxY = INT_MIN, maxZ = INT_MIN;
	for(int i = 0; i < n; i++) {
		cin>>voxels[i].x;
		cin>>voxels[i].y;
		cin>>voxels[i].z;
		// if(voxels[i].x < minX)
		// 	minX = voxels[i].x;
		// else if(voxels[i].x > maxX)
		// 	maxX = voxels[i].x;
		// if(voxels[i].y < minY)
		// 	minY = voxels[i].y;
		// else if(voxels[i].y > maxY)
		// 	maxY = voxels[i].y;
		// if(voxels[i].z < minZ)
		// 	minZ = voxels[i].z;
		// else if(voxels[i].z > maxZ)
		// 	maxZ = voxels[i].z;
	}
	// cout<<"X : "<<minX<<" - "<<maxX<<endl;
	// cout<<"Y : "<<minY<<" - "<<maxY<<endl;
	// cout<<"Z : "<<minZ<<" - "<<maxZ<<endl;
	// for(int i = 0; i < n; i++) {
	// 	voxels[i].x-=minX;
	// 	voxels[i].y-=minY;
	// 	voxels[i].z-=minZ;
	// }
	sort(voxels.begin(), voxels.end(), compare);
	// int vox[100][100][100];
	// for(int i = 0; i < 100; i++)
	// 	for(int j = 0; j < 100; j++)
	// 		for(int k = 0; k < 100; k++)
	// 			vox[i][j][k] = 0;
	// for(int i = 0; i < n; i++) {
	// 	vox[voxels[i].x][voxels[i].y][voxels[i].z] = 1;
	// }
	
	// for(int i = 0; i < n; i++) {
	// 	voxelToGrid(voxels[i], vertices, faces);
	// }
	
	createObjFile(vertices, faces);
	return 0;
}