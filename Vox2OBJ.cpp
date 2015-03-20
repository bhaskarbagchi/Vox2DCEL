#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <climits>

using namespace std;

int main(){
	int k =0;
	int faces[][4] = {{1,2,3,4},{5,8,7,6},{1,4,8,5},{2,6,7,3},{4,3,7,8},{2,1,5,6}};
	int n;
	cin>>n;
	int x,y,z;
	while(n--){
		cin>>x>>y>>z;
		cout<<"v "<<x<<" "<<y<<" "<<z<<endl;
		cout<<"v "<<x+1<<" "<<y<<" "<<z<<endl;
		cout<<"v "<<x+1<<" "<<y+1<<" "<<z<<endl;
		cout<<"v "<<x<<" "<<y+1<<" "<<z<<endl;
		cout<<"v "<<x<<" "<<y<<" "<<z+1<<endl;
		cout<<"v "<<x+1<<" "<<y<<" "<<z+1<<endl;
		cout<<"v "<<x+1<<" "<<y+1<<" "<<z+1<<endl;
		cout<<"v "<<x<<" "<<y+1<<" "<<z+1<<endl;

		for(int i=0;i<6;i++){
			cout<<"f "<<faces[i][0]+k<<" "<<faces[i][1]+k<<" "<<faces[i][2]+k<<" "<<faces[i][3]+k<<endl;
		}
		k+=8;
	}
}
