int max(int a,int b){
    if(a>b)return a;
    else return b;
}
main(){
    int max(int a,int b);
    int(*pmax)();
    int x,y,z;
    pmax=max;
    printf("input two numbers:\n");
    scanf("%d%d",&x,&y);
    z=(*pmax)(x,y);
    printf("maxmum=%d",z);
}
