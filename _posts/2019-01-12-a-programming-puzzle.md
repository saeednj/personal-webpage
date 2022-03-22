---
layout: post
title: ! 'The Good, the Bad and the Ugly: A Programming Puzzle'
categories: Technical
tags: Programming Puzzle Obfuscation
comments: true
---

I love programming puzzles and challenges. Although I haven't spent much time on it in recent years.
When I was doing my Master's in Shahid Beheshti University, the Science branch of the student association in our department
(for those who know Farsi, I am trying to translate this: انجمن علمی دانشکده),
asked me to write a programming challenge for a take-home contest.
Now, after more than ten years I wanted to write an editorial on that challenge.
You can find all of the codes listed in this post [here][1].

## The challenge
The challenge that I wrote was about reading and understanding an obfuscated C program and optimizing it.
The task was:
> Given the following C code, write a code that generates the same
> output in under 10 seconds.
> The source code of your solution should not exceed 2KB in size.


```c
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
    for( x=1; x<=200000; x++ )
    {
        float y = P(x,*d);
        if ( x % y ) { puts(":Ugly");continue; }
        puts( S(F(x,x),1) ? ":Good" : ":Bad" );
    }
    printf("Who's %d?\n", (*s)[a][e][e][d]);
    return 0;
}
```

I have seen functions `A` and `G` somewhere else and haven't written them myself. At the moment I don't remember the source. I will add a reference later when I find it. Having a quick look at the code, you can see that all of the functions are one-liners and except function `G` they all have recursive calls. There are some small weird syntaxes in the code too, like: variable `y` defined in the main function is a `float`, but in the next line we have `x % y` which
doesn't make sense. For example `5 % 2.5` doesn't have a meaning.
Or you can see that `s`, `a`, `e` and `d` are defined as arrays, but they are used as indices of another array in `(*s)[a][e][e][d]` without having an index themselves. Or if you look at the output, there are some numbers printed, but there is no `printf` function applied on a number directly in the code!

In the [repository][1], the main challenge code is called `obf.c`, you can compile it
like: `gcc obf.c`. Running it on my machine with `Intel Core i7-3770 / 16GB RAM / 8MB cache / gcc 4.9.2`, it takes _5 minutes and 2.313 seconds_ to generate the output. 

I did not limit the choice of programming language and even running platform. I doubted that they could have a winning impact, but I was curious to see solutions on different platforms/languages.
I was very excited to see how people approached the problem and what are the solutions that I might not have thought about. Well, the number of submitted solutions was, umm, **zero**. Not sure if no one looked at it, no one was interested, or it was too hard. Maybe it doesn't matter, because I enjoyed writing it.


*At this point if you want to give this challenge a try, please stop reading, as in the following section I talk about my initial solution and later solutions that I thought about.*

****

## My solution

I will try to explain how each function works but I might not go into details. Let me know if you have any questions or comments on any part.

Before trying to understand the code, one easy thing to do is compiling the code with a compiler optimizer to see how much of the jargon will be pruned away. Compiling like `gcc -O3 obf.c` on my machine, gives an executable that generates the desired output in _2 minutes 23.497 seconds_, which is nearly a 2X speed-up already.

Let's de-obfuscate the code a bit.

In C preprocessor directives, `x##y` means concatenation of `x` and `y`. So `#define Const(x,y,z)` just concats its inputs. Therefore `Const(un,sign,ed)` is the word `unsigned`. The next line just defines `float` as `unsigned`, so you can replace all `float`s in the code with `unsigned`. Now, taking remainder over `y` in `x % y` makes sense!

### Function `A`(dd)

This function performs some bitwise operations on its inputs. For understanding bitwise functions, it is usually helpful to think of the operands with one or two bits first, as the bitwise operations are applied on all bits independent of each other. Writing the code
a bit more cleaner it looks like:
```c
if ( (a & b) == 0 )
    return a ^ b;
else
    return A((a&b) << 1, a^b);
```
You can see that the code is computing `and` and `xor` of input bits. If you are familiar with logical circuits, you see that we are mimicking a half-adder, with `xor` as the sum output and `and` as the carry output. The code checks if there is any carry signal: `if ( (a&b) == 0 )`, and if there are no carry values, the sum of two inputs is equal to their `xor`. Otherwise we need to add the carry values to the next digit position. That's what `(a&b) << 1` is doing (putting carry values in their next digit position), and then calling the `A` function again to add up the carry and sum values. Basically `A` is doing addition (`a + b`). If you want to read more about this type of addition, I refer you to [carry-save addition][6] (where one of the three operands is always zero).

### Function `P`(rint)

The first thing about this function is, it doesn't use the second argument. So you can remove the second argument and update the calls to this function as well. This function calls `putchar` which prints one character and returns the ascii value of the printed character. The number 48 is the ascii value of character `'0'`, so `putchar(a%10+48)-48`, prints the least significant digit of `a` and evaluates to the value of that digit. Now if we write the code cleaner, we will get:
```c
if ( a == 0 )
    return 0;
else {
    int s = P(a/10); // drop the rightmost digit and print
    int d = a % 10;
    putchar(d + '0'); // print the rightmost digit
    return s + d;
}
```
Now it is easy to see that the code prints the given number digit by digit, but also returning the sum of digit values.

### Function `G`(CD)

Maybe you have seen this code before: `b ^= a ^= b ^= a`. It does these steps (I labeled the updated variables as _new_ to make it easier to follow):
```
new_b <- a xor b
new_a <- a xor (new_b) = a xor (a xor b) = b
newer_b <- new_b xor new_a = (a xor b) xor b = a
```
which is effectively swapping `a` and `b`. Now we extend this to
`b ^= a ^= b ^= a %= b`, and repeat it until `b` is non-zero. It means that we are doing this:
```c
while( b != 0 ){
    a = a % b;
    swap(a, b);
}
```
This is [Euclidean algorithm][2] for computing _Greatest Common Divisor_ or _GCD_ in short. The GCD value is stored in `a`. But we are returning `!--a`. If the GCD is equal to 1, `--a` makes it zero and `!--a` return 1, and if GCD is larger than 1, `--a` is still non-zero and `!--a` returns zero. So the function actually return whether the two inputs are relatively prime or not.

### Function `F`(i)

In this function you can see a pattern of recursive call which was used in the function `P` as well. This pattern is called _tail recursion_ and it simulates a loop by performing the body of the loop in the function and passing a next step value as an argument back to the function. The recursive call termination condition is the same as loop condition in this case. Let's rewrite this function with a loop:
```c
unsigned r = 0;
while( b ) {
    r += G(a, b);
    b--;
}
return r;
```
and knowing that `G` is calculating if `a` and `b` are relatively prime, `F` is calculating how many value from 1 to `b` are relatively prime to `a`. This might sound familiar to you. If we set both `a` and `b` to the same value, `F` would represent
[Euler's phi function][3].

### Function `S`(quare)

Let's look at the `A(a,1+~b)` first. `~b` inverts all of the bits in `b` (in other terms 1's complement of `b`). Adding one to 1's complement of a number gives us its [2's complement][4]. Negative numbers are represented using 2's complement notation. So `1+~b` is equivalent to `-b`, and we knew that `A(x,y)=x+y`, therefore `A(a,1+~b) = a+(-b) = a-b`.

Again we can rewrite the tail recursion as a loop. Note that function `S` is only invoked with `b` set to 1, so we can fix the starting value of `b`.
```c
if ( a == 0 )
    return 1;
else {
    unsigned r = 0;
    b = 1;
    while( b <= a ) {
        a -= b;
        b += 2;
    }
    if ( a == 0 )
        return 1; // this is simulating the termination condition
                  // in the recursive call
    else
        return 0;
}
```

You can see that `b` values follow this sequence: 1, 3, 5, ... (odd numbers)
and each value is deducted from `a`, and if at some point `a` reaches
exactly zero, the function returns 1. In other words, if `a` is equal to sum of some odd values, it returns 1, otherwise 0. If you compute the sum of first `k` odd numbers you will get `k^2`.

```
1 = 1 = 1^2
1 + 3 = 4 = 2^2
1 + 3 + 5 = 9 = 3^2
...
```
So, after all, `S` checks if `a` is a perfect square or not.

### `main` function

Now that we understand each function, we can look at what the main program does. There is a loop over the interval [1, 200000], and for each number first the number is printed using `P` and the sum of digits is recorded (`y = P(x,*d);`, recall that second argument is not used). Each number can be either _Good_, _Bad_ or _Ugly_. If the number is not divisible by its sum of digits, it is _Ugly_ (e.g. 11 is Ugly, because it is not divisible by 1+1=2). If the number is beautiful (not Ugly :P), we compute the phi(x). If it is a perfect square, the number is Good, otherwise it is Bad. For example 12 is a Good number: divisible by 1+2, phi(12) = 4 = 2^2.

Finally there is a line that was intended to serve as an "easter egg" if you will. If you see the output generated by this line, it says: "Who's 25?" and the answer is in the code itself that says: `(*s)[a][e][e][d]`. This part was only written as a signature thing, otherwise in a solution one might simply write: `printf("Who's 25?\n")`. It showcases one of the weird things that is valid to a C compiler. When a C compiler wants to generate address to an array element, it adds the index to the base pointer. So `x[3]` is replaced with `*(x+3)`, and you know that addition is commutative, so it is equal to `*(3+x)` which in array notation is the same as `3[x]` (`x[3]` === `3[x]`). Using this idea, we can swap the index and array name in `(*s)[a][e][e][d]` and get: `d[(*s)[a][e][e]]`, and repeating this, we get: `d[e[e[a[*s]]]]`. `*s` points to the first element in `s`. The arrays `a`, `e`, `d` and `s` are declared in the global scope, so by default they are initialized with zero. Therefore `d[e[e[a[*s]]]] = d[e[e[a[0]]]] = d[e[e[0]]] = d[e[0]] = d[0]`, and we know the value of that, because of the line `*d = 25;`

### Optimized version

The algorithms for computing each of the functions is simply very inefficient (on purpose of course).

#### Square function

The function `S` detects a number being of the form $$k^2$$ by doing $$k$$ subtractions. So the time complexity of this function is $$O(sqrt(n))$$. We can do this more efficiently. $$y=x^2$$ is a strictly increasing function (resembles a sorted array if you build `y=a[i]=i*i`), so we can do a binary search. We have an input number `n` and want to find a `x` that `x*x == n`. The interval that this solution (if exists) is definitely in, is `[1, n]`. The following code shows how it can be implemented. The resulting time complexity is $$O(log(n))$$.
```c
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
```

#### Phi function

The main improvement we can make is the computation of phi of each number. The trivial implementation given in the code, performs $$O(n)$$ calls to GCD for each number, therefore, we have $$O(n^2)$$ calls in total, which is the most expensive part of the code. We can compute the phi of a number `n` based on the phi of smaller numbers, and because we are computing the phi of all the numbers in the interval anyway, we can use memoization to dramatically improve the computation of later phi values. ~~Writing math equation is not very easy in Markdown, so~~ I refer you to the Wikipedia page for the [Euler's phi function][3]. The relation I mainly used is $$\phi(p^k) = p^k * (p-1)$$ where $$p$$ is a prime number. So scanning prime factors of the input number $$n$$, if ($$p$$ divides $$n / p$$) ($$n$$ has prime factor $$p$$ of power more than one),
$$\phi(n) = \phi(n / p) *  p$$ and $$\phi(n) = \phi(n / p) * (p-1)$$ otherwise.

Now we can store all the computed values of Phi (memoization) to avoid computing them multiple times. This way, computing some of the Phi values takes only one division and one multiplication. Considering that I have computed the prime numbers in the array `p`, and the array `PHI` is used for recording previously computed values, the code for computing Phi is:
```c
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

```

Computing the prime numbers up to some value can be done using [Sieve of Eratosthenes][5]. This is my implementation of this sieve, with modified loops to cross out even numbers other than 2:

```c
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
```

where `ip[x]` represents `isPrime(x)`, and array `p` will contain primes numbers between 1 and N, and `pn` is the number of prime numbers in `p` after the function is done.

And finally the main function becomes:
```c
int main() {
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

```

The full version can be found in the [repo][1] in the file `ans.c`, and on the machine specification I mentioned above, it generates the same output in _0.05 seconds_.

## Other solutions

After I released the challenge, I realized that for each number, the property of being Good, Bad or Ugly is computed independent of other numbers. Which calls for an embarrassingly parallel computation. I did
a quick parallelization with OpenMP, without optimizing any of the functions, and only doing the digit sum of `P` inline:

```c
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

```
This is listed as `mp.c` in the repository.
On the same machine, using 8 cores, and with compiler optimization (`gcc -fopenmp -O2 mp.c`), it runs at _37.826 seconds_, which gets pretty close to the 10 seconds target without any manual optimizations.

Finally I tried parallelizing the code with a GPU. You can find the code in `gpu.cu`. I tried to keep the code as close to what it is and not optimizing it, but I had to unroll the recursive calls to loops. Using CUDA and a very cheap laptop GPU, the results were ready in _1.3 seconds_, which reaches the goal of the challenge. Although not beating my initial solution :P

The table below summarizes the runtime of my solutions. The time limit to match was 10 seconds.

| Version | Runtime (s)  |
| ------- | -----------: |
| Original code                                    | 302.313 |
| gcc optimization only                            | 143.97 |
| OpenMP 8-core parallelization + gcc optimization | 37.826 |
| Nvidia G105M parallelization                     | 1.3 |
| Algorithmically optimized                        | 0.05 |

<br>
I'd be happy to hear your comments on this editorial.


[1]: {{site.blog_code_path}}/obf
[2]: https://en.wikipedia.org/wiki/Euclidean_algorithm
[3]: https://en.wikipedia.org/wiki/Euler%27s_totient_function
[4]: https://en.wikipedia.org/wiki/Two%27s_complement
[5]: https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
[6]: https://en.wikipedia.org/wiki/Carry-save_adder
