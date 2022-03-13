#include <stdio.h>
#define Const(x,y,z) x##y##z
#define float Const(un,sign,ed)

float x, s[1], a[1], e[1], d[1];
float A(float a, float b){return a&b?A(a^b,(a&b)<<1):a^b;}
float P(float a, float b){return a?P(a/10,'-')+putchar(a%10+48)-48:0;}
float G(float a, float b){for(;b;b^=a^=b^=a%=b);return !--a;}
float F(float a, float b){return b?G(a,b)+F(a,b-1):0;}
float S(float a, float b){return a?a<b?0:S(A(a,1+~b),b+2):1;}
int main()
{
    *d=25;
    char str[10][10] = { ":Ugly", ":Good", ":Bad"};
    int result[200005];

#pragma omp parallel for private(x) schedule(dynamic, 8)
    for( x=1; x<=200000; x++ )
    {
        int t, X = x, sum;
        for( sum=0; X; X/=10 ) sum += X % 10;
        float y = sum;
        if ( x % y ) t = 0;
        else if ( S(F(x,x),1) ) t = 1;
        else t = 2;
        result[x] = t;
    }
    for( x=1; x<=200000; x++ )
    {
        printf("%d", x);
        puts( str[result[x]] );
    }
    printf("Who's %d?\n", (*s)[a][e][e][d]);
    return 0;
}
