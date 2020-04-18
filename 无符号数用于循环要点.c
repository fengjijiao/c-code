#include <stdio.h>

int main()
{
    unsigned int a = 5;
    //printf("%x", a - 1);
    while( a>= 0) {//由于a为无符号数，故当0-1后的值依然>=0会导致无限循环;故应改为a != 0xffffffff(以具体int位数有关,这里以32位)
        printf("%x\n", a);
        a--;
    }
    return 0;
}
