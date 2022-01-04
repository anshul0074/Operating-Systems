#include<stdio.h>
#define int long long 

signed main(){
    int x=7,a=3,b=5,m=1000007 ;
    freopen("file.txt","w",stdout);
    for(int i=0;i<100000;i++){
        printf("%d ",x);
        x=(a*x+b)%m;
    }
}
