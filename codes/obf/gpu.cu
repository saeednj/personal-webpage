#include <stdio.h>

__device__ unsigned A(unsigned a, unsigned b)
{
    while( a & b )
    {
        unsigned X = a ^ b;
        unsigned Y = (a&b)<<1;

        a = X;
        b = Y;
    }
    return a ^ b;
}

__device__ unsigned G(unsigned a, unsigned b)
{
    for(;b;b^=a^=b^=a%=b);
    return !--a;
}

__device__ unsigned F(unsigned a, unsigned b)
{
    unsigned s = 0;
    while( b )
    {
        s += G(a, b);
        b--;
    }
    return s;
}

__device__ unsigned S(unsigned a, unsigned b)
{
    for( ; ; )
    {
        if ( a )
        {
            if ( a >= b )
            {
                unsigned x = A(a, 1+~b);
                unsigned y = b + 2;

                a = x;
                b = y;
            }
            else return 0;
        }
        else return 1;
    }
}

__global__ void kernel(char* result)
{
    int t, sum, x = blockIdx.x*blockDim.x + threadIdx.x + 1;
    int X = x;
    for( sum=0; X; X/=10 ) sum += X % 10;
    if ( x % sum ) t = 0;
    else if ( S(F(x,x),1) ) t = 1;
    else t = 2;
    result[x] = t;
}

int main()
{
    char str[10][10] = { ":Ugly", ":Good", ":Bad"};
    char *devR;
    char result[200005];

    cudaMalloc( (void**)&devR, 200005 );

    kernel<<<200, 1000>>>(devR);

    cudaMemcpy( result, devR, 200005, cudaMemcpyDeviceToHost );

    for( unsigned x=1; x<=200000; x++ )
    {
        printf("%d", x);
        puts( str[result[x]] );
    }

    printf("Who's 25?\n");

    cudaFree( devR );
    return 0;
}
