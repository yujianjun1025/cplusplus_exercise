#include<stdlib.h>
#include<stdio.h>
void f(void)
{
    int* x = (int *)malloc(10 * sizeof(int));
    //x[11] = 0; // problem 1: heap block overrun
} // problem 2: memory leak -- x not freed

int main(void)
{
    int i = 10;
    f();
    printf("i=%d/n",i); //problem 3: use uninitialised value.
    return 0;
} 
