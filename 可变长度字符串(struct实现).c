#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_SIZE 1000
typedef struct{
    char str[MAX_SIZE];
    int length;
}sqstring;
int Length(char *s){
    int n=0,i=0;
    while(s[i]){
        n++;
        i++;
    }
    return n;
}
void Generate_next(sqstring *s,int *next){
    int j=0,i=1;
    next[0]=0;
    while(i<s->length) {
        if(s->str[j]==s->str[i]){
            next[i]=next[i-1]+1;
            j++;
        }else{
            next[i]=0;
            j=0;
        }
        i++;
    }
}
int Match_String(sqstring *s,sqstring *t,int *next){
    int i=0,j=0;
    while(j<(s->length)&&i<(t->length)){
        if(s->str[j]==t->str[i]){
            printf("j=%d)%c==%c(i=%d\n",j,s->str[j],t->str[i],i);
            i++;
            j++;
        }else{
            printf("j=%d)%c!=%c(i=%d\n",j,s->str[j],t->str[i],i);
            if(next[i]) {
                j += next[i];
            }else{
                j++;
            }
            printf("j=%d)=>%c(i=%d\n",j,s->str[j],0);
            i=0;
            if(i==0&&t->length>((s->length)-j)){
                printf("j=%d)t.length<s.length-j(i=%d\n",j,i);
                return -1;
            }
        }
    }
    if(i==t->length)return j-i;
    else return -1;
}
int main(){
    sqstring *s=(sqstring *)malloc(sizeof(sqstring));
    strcpy(s->str,"ABCDABCDABC");
    printf("原字符串:%s\n",s->str);
    s->length=Length(s->str);
    printf("总长度:%d\n",s->length);
    sqstring *t=(sqstring *)malloc(sizeof(sqstring));
    strcpy(t->str,"CDABCD");
    printf("待查找子串:%s\n",t->str);
    t->length=Length(t->str);
    printf("总长度:%d\n",t->length);
    int	next[MAX_SIZE];
    Generate_next(t,next);
    printf("next前缀表:");
    for(int k=0;k<t->length;k++){
        next[k]+=1;
        printf("%d ",next[k]);
    }
    printf("\n");
    printf("子串的索引为:%d\n",Match_String(s,t,next));
}
