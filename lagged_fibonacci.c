#include <stdio.h>
#include<time.h>

signed main(){
	freopen("file.txt","w",stdout);
	int mod=100007;
	int x=time(0);
	int arr[7];
	int i=6;
	while(x>0 && i>=0){
		arr[i]=x%10;
		x=x/10;
        i--;
	}
	int p=3,q=5;
	int n=100000;
//	scanf("%d",&n);
	for(int i=0;i<n;i++){	
		int res=(arr[p]+ arr[q])%mod;
		printf("%d ",res);
		for(int i=0;i<6;i++){
			arr[i]=arr[i+1];
		}
		arr[6]=res;
	}
}
