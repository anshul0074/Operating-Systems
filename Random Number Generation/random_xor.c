#include<stdio.h>
#define int long long

int fun(){
  static int x=3;
  static int y=31;
  static int z=7;
  static int w=23;
  int t;
  t=x^(x<<11);   
  x=y; 
  y=z;
  z=w;   
  return w=w^(w>>19)^(t^(t>>5));
}
signed main(){
      freopen("file.txt","w",stdout) ;
      for(int i=0;i<100000;i++){
        printf("%lld ",fun());
      }
}
