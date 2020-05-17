#include<stdio.h>

int main() {
  int a=oxff;//1111 1111
  a<<=1;
  printf("%d\n", a);//510//1 1111 1110
}

void printBinary(int n) {
  while (n) {
    if (n & 1)
        printf("1");
    else
        printf("0");

    n >>= 1;
  }
  printf("\n");
}
