#include <stdio.h>
#define N 500

int p[N+5], PHI[200005], pn;
char ip[N/2+1];

int P(int a)
{
    return a?P(a/10)+putchar(a%10+48)-48:0;
}

void sieve()
{
    int i, j;
    p[0] = 2;
    pn = 1;
    for( i=3; i*i<=N; i+=2 )
        if ( !ip[i/2] )
            for( j=i*i; j<N; j+=2*i ) ip[j/2] = 1;
    for( i=3; i<N; i+=2 )
        if ( !ip[i/2] ) p[pn++] = i;
}

int phi(int n)
{
    int i, m;
    if ( PHI[n] ) return PHI[n];
    for( i=0; i<pn && p[i]*p[i]<=n; i++ )
        if ( n % p[i] == 0 )
        {
            m = n / p[i];
            PHI[n] = phi(m)*( m % p[i] == 0 ? p[i] : p[i]-1 );
            return PHI[n];
        }
    return (PHI[n] = n-1);
}

int isSquare(int n)
{
    long long l = 0, h = n, m;
    while( l<=h )
    {
        m = (l + h) / 2;
        if ( m*m == n ) return 1;
        if ( m*m < n ) l = m+1;
        else h = m - 1;
    }
    return 0;
}

int main()
{
    int x;
    sieve();
    for( x=1; x<=200000; x++ )
    {
        PHI[x] = phi(x);
        int y = P(x);
        if ( x % y ) { puts(":Ugly"); continue; }
        puts( isSquare(PHI[x]) ? ":Good" : ":Bad" );
    }
    puts("Who's 25?");
    return 0;
}
