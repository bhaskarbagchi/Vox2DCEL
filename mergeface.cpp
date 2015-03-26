
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
	int origin;
	DCELedge * twin;
	DCELedge * next;
	DCELedge * prev;
	DCELface * face;
	DCELedge * globalNext;
	DCELedge * globalPrev;
	
	DCELedge(int o,DCELedge * _next,DCELedge * _prev,DCELedge *_twin){
		origin = o; next = _next;prev = _prev;twin = _twin;
	}
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


DCELface * mergeFace(DCELface * f1,DCELface *f2) {
	cout<<"Entering mergeFace\n";
	if(f1==NULL){
		cout<<"f1 is null\n";
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
	f1->edge = s1;
	
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
			while(e1->next != e2->prev->twin){
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
		//e1->origin.printData();
		//e1->next->origin.printData();
		e1 = e1->next;
		//this_thread::sleep_for (chrono::seconds(2));	
	}while(e1!=s1 && flag!=true);
	return f1;
}

void printFace(DCELface *face){
	cout<<"Face id = "<<face->FaceId<<endl;
	DCELedge *e,*start;
	e = start = face->edge;
	do{
		cout<<e->origin<<endl;
		e = e->next;
	}while(e!=start);
	face = face->globalNext;
}

int main(int argc, char * argv[]) {
	DCELedge *e1 =  new DCELedge(1,NULL,NULL,NULL);
	DCELedge *e2 =  new DCELedge(2,NULL,e1,NULL);e1->next = e2;
	DCELedge *e3 =  new DCELedge(3,NULL,e2,NULL);e2->next = e3;
	DCELedge *e4 =  new DCELedge(4,e2,e3,NULL); e1->prev = e4;
	
	DCELedge *e5 =  new DCELedge(2,NULL,NULL,NULL);
	DCELedge *e6 =  new DCELedge(5,NULL,e5,NULL);e5->next = e6;
	DCELedge *e7 =  new DCELedge(6,NULL,e6,NULL);e6->next = e7;
	DCELedge *e8 =  new DCELedge(1,e5,e7,e2); e5->prev = e8; e2->twin = e8;
	
	
	DCELedge *e9 =  new DCELedge(4,NULL,NULL,e3); e3->twin = e9;
	DCELedge *e10 =  new DCELedge(3,NULL,e9,e7);e9->next = e10;e7->twin = e10;
	DCELedge *e11 =  new DCELedge(6,NULL,e10,e6);e10->next = e11;e6->twin = e11;
	DCELedge *e12 =  new DCELedge(5,NULL,e11,e5);e11->next = e12;e5->twin = e12;
	DCELedge *e13 =  new DCELedge(2,NULL,e12,e1);e12->next = e13;e1->twin = e13;
	DCELedge *e14 =  new DCELedge(1,e9,e13,e4);e9->prev = e14;e4->twin = e14;
	
	DCELface *f1,*f2,*f3;
	f1 = new DCELface(1,e1);
	f2 = new DCELface(2,e5);
	f3 = mergeFace(f1,f2);
	printFace(f3);
	
	return 0;
}
