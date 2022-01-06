#include<stdio.h>
#define int long long 

signed main(){
    int x=0,a=1,b=2,m=1000;
    freopen("arraydata.txt","w",stdout);
    int arr[101];
    arr[0]=7;arr[1]=5;arr[2]=3;
    for(int i=0;i<251;i++){
        x=arr[x%101];a=arr[a%101];b=arr[b%101];
        for(int j=0;j<101;j++){
            arr[j]=x+1;
            printf("%lld ",arr[j]);
            x=(a*x+b)%m;
        }
        printf("\n");
    }
}