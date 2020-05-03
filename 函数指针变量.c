/*
在C语言中，一个函数总是占用一段连续的内存区，而函数名就是该函数所占内存区的首地址。我们可以把函数的这个首地址（或称入口地址）赋予一个指针变量，使该指针变量指向该函数。然后通过指针变量就可以找到并调用这个函数。我们把这种指向函数的指针变量称为“函数指针变量”。
*/
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
