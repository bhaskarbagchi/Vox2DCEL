#include <bits/stdc++.h>
using namespace std;
#define SIDE_LENGTH 10

typedef vector<int> Face;
int faces[][4] = {{1, 2, 3, 4},
				{1, 5, 6, 2},
				{1, 4, 8, 5},
				{6, 7, 3, 2},
				{3, 7, 8, 4},
				{7, 6, 5, 8}};

struct Point2D
{
	double x,y;
	Point2D (double _x = 0, double _y = 0) : x(_x), y(_y){}
	bool operator < (const Point2D &pt) const
	{
		return (x < pt.x || (x == pt.x && y < pt.y));
	}
};

struct Point3D{
  double x;
  double y;
  double z;
  Point3D(){}
  Point3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z){}
};

struct Square
{
	vector<Point2D> vertices;
	Point2D center;
	Face face;
};

struct Cube
{
	vector<Point3D> vertices;
	vector<Face> faces;
};

struct OrthogonalPolygon
{
	vector<Point2D> vertices;
	Face face;
};

struct DCELnode
{
	int vID;
	DCELnode *next;
	DCELnode *prev;
	DCELnode(int _id = 0)
	{
		vID = _id;
		next = NULL;
		prev = NULL;
	}
};

Square getSquare(Point2D center)
{
	static int vertexOffset = 0;
	Square sq;
	sq.center = center;
	sq.vertices.push_back(Point2D(center.x - SIDE_LENGTH/2, center.y - SIDE_LENGTH/2));
	sq.vertices.push_back(Point2D(center.x + SIDE_LENGTH/2, center.y - SIDE_LENGTH/2));
	sq.vertices.push_back(Point2D(center.x + SIDE_LENGTH/2, center.y + SIDE_LENGTH/2));
	sq.vertices.push_back(Point2D(center.x - SIDE_LENGTH/2, center.y + SIDE_LENGTH/2));
	for(int i=1;i<=4;i++)
	{
		sq.face.push_back(i+vertexOffset);
	}
	
	vertexOffset += 4;
	return sq;
}

void printSquareToObj(Square &sq, ofstream &out, int z = 0)
{
	for(int i=0;i<sq.vertices.size(); i++)
	{
		out << "v "<<sq.vertices[i].x<<" "<<sq.vertices[i].y<<" "<<z<<endl;
	}
	out << "f ";
	for(int i=0;i<sq.face.size(); i++)
	{
		out <<sq.face[i]<<" ";
	}
	out << endl;
}

void mergeFaces(vector<Square> &squares, char axis = 'z', int distFromOrigin = 0)
{
	bool flag = false;
	DCELnode *start = NULL;

	map<int, DCELnode* > currSq, prevSq;
	for(int i = 0; i < squares.size(); i++)
	{
		Square sq = squares[i];
		for(int j=0; j < sq.face.size(); j++)
		{
			DCELnode *temp = new DCELnode(sq.face[j]);
			currSq[sq.face[j]] = temp;
			cout << "v " << sq.vertices[j].x << " " << sq.vertices[j].y << " " << distFromOrigin << endl;
		}
		for(int j = 0; j < sq.face.size(); j++)
		{
			DCELnode *curr = currSq[sq.face[j]];
			DCELnode *prev = currSq[sq.face[(j + sq.face.size() - 1) % sq.face.size()]];
			DCELnode *next = currSq[sq.face[(j + 1) % sq.face.size()]];
			curr->next = next;
			curr->prev = prev;
		}
		if(flag == false)
		{
			start = currSq[sq.face[0]];
			flag = true;
		}
		else
		{
			Square lastSq = squares[i-1];
			int a1 = 0, a2 = 0, b1 = 0, b2 = 0;
			int count = 0;
			for(int j=0;j<lastSq.vertices.size(); j++)
			{
				for(int k = 0; k < sq.vertices.size(); k++)
				{
					Point2D p1 = lastSq.vertices[j];
					Point2D p2 = sq.vertices[k];
					if(abs(p1.x-p2.x) + abs(p1.y-p2.y) == SIDE_LENGTH)
					{
						if(count == 0)
						{
							a1 = lastSq.face[j];
							b1 = sq.face[k];
						}
						else
						{
							a2 = lastSq.face[j];
							b2 = sq.face[k];	
						}
						count++;
					}
				}
			}
			// cout << "count = " << count << endl;
			// cout << "a1 = " << a1 << " b1 = " << b1 << " a2 = " << a2 << " b2 = " << b2 << endl;
			// cout << prevSq[a1]->next->vID << "\t" << prevSq[a1]->prev->vID << endl;
			if(count == 2)
			{
				if(prevSq[a1]->next->vID == a2)
				{
					prevSq[a1]->next = currSq[b1];
					currSq[b1]->prev = prevSq[a1];
					prevSq[a2]->prev = currSq[b2];
					currSq[b2]->next = prevSq[a2];
				} 
				else if(prevSq[a1]->prev->vID == a2)
				{
					prevSq[a1]->prev = currSq[b1];
					currSq[b1]->next = prevSq[a1];
					prevSq[a2]->next = currSq[b2];
					currSq[b2]->prev = prevSq[a2];
				} 
			}
			else
			{
				cout << "f ";
				DCELnode *temp = start->next;
				cout << start->vID << " ";
				while(temp != start)
				{
					cout << temp->vID << " ";
					temp = temp->next;
				}
				cout<<endl;
				start = currSq[sq.face[0]];
			}
		}
		prevSq = currSq;
		currSq.clear();
	}
		cout << "f ";
		// start = currSq[sq.face[0]];
		DCELnode *temp = start->next;
		cout << start->vID << " ";
		while(temp != start)
		{
			cout << temp->vID << " ";
			temp = temp->next;
		}
		cout<<endl;
}
int main()
{
	int n;
	cin >> n;
	ofstream out("cube.obj");
	int prev_x, prev_y, prev_z;
	vector<Square> squares;
	for(int i=0;i<n;i++)
	{
		int x,y;
		cin >> x >> y ;
		x *= 2*SIDE_LENGTH, y *= 2*SIDE_LENGTH;
		// if(i)
		// {
		// 	int d = abs(x-prev_x) + abs(y-prev_y);
		// 	if(d == 2*SIDE_LENGTH)
		// 	{
		// 		Square sq = getSquare(Point2D((prev_x+x)/2, (prev_y+y)/2));
		// 		printSquareToObj(sq, out);
		// 		// squares.push_back(sq);
		// 	}
		// }
		Square sq = getSquare(Point2D(x,y));
		printSquareToObj(sq, out);
		squares.push_back(sq);
		prev_x = x, prev_y = y;
	}
	mergeFaces(squares);
}
