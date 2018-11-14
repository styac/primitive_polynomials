/*==============================================================================
| 
|  File Name:     
|
|     ppFactor.c
|
|  Description:   
|
|     Routines for integer factorization and primality prime testing.
| 
|  Functions:
|
|     factor
|     is_probably_prime
|     is_almost_surely_prime
|
|  LEGAL
|
|     Primpoly Version 13.0 - A Program for Computing Primitive Polynomials.
|     Copyright (C) 1999-2018 by Sean Erik O'Connor.  All Rights Reserved.
|
|     This program is free software: you can redistribute it and/or modify
|     it under the terms of the GNU General Public License as published by
|     the Free Software Foundation, either version 3 of the License, or
|     (at your option) any later version.
|
|     This program is distributed in the hope that it will be useful,
|     but WITHOUT ANY WARRANTY; without even the implied warranty of
|     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|     GNU General Public License for more details.
|
|     You should have received a copy of the GNU General Public License
|     along with this program.  If not, see <http://www.gnu.org/licenses/>.
|     
|     The author's address is artificer!AT!seanerikoconnor!DOT!freeservers!DOT!com
|     with !DOT! replaced by . and the !AT! replaced by @
|
==============================================================================*/


#include <stdlib.h> /* for rand() function */

#include "Primpoly.h"
#include "ppArith.h"
#include "ppHelperFunc.h"
#include "ppIO.h"
#include "ppOrder.h"
#include "ppPolyArith.h"
#include "ppFactor.h"

int factor( bigint n, bigint * primes, int * count )
{

    int
        t = 0,              /*  Array index for primes and count.         */
        cnt = 0,            /*  Counter for factors of 2 and 3.           */
        new_d = YES,        /*  YES if current divisor is different from
                                the previous one.                         */
        n_is_prime = NO,    /*  YES if current value of n is prime.       */
        flag = YES;        /*  Alternately YES and NO.                   */

    bigint
        q,                  /*  The quotient, floor( n / d )              */
        r,                  /*  The remainder, n mod d                    */
        d = 5;             /*  Trial divisor and its first value.        */


    /*  Handle special cases for speed.
        These are cases where r = (p^n - 1)/(p-1) is a large prime or has large prime
        factors and p^n is less than the maximum size allowed by this program.
        (I don't have all these cases).
    */
    if (n == 1)
    {
        primes[ 0 ] = count[ 0 ] = 1;
        return( 0 );
    }
    else if (n == (bigint)4611686018427387903ULL) /*  (2^62 - 1)/(2 - 1)  */
    {
        primes[ 0 ] = 3;
        count [ 0 ] = 1;

        primes[ 1 ] = 715827883ULL;
        count [ 1 ] = 1;

        primes[ 2 ] = 2147483647ULL;
        count [ 2 ] = 1;

        return( 2 );
    }
    else if (n == (bigint)2305843009213693951ULL) /*  (2^61 - 1)/(2 - 1)  */
    {
        primes[ 0 ] = (bigint)2305843009213693951ULL;
        count [ 0 ] = 1;

        return( 0 );
    }

    while( n % 2 == 0 )       /*  Remove factors of 2. */
    {
        n /= 2;
        ++cnt;
    }

    if (cnt != 0)
    {
        primes[ t ] = 2;
        count[ t++ ] = cnt;
    }

    cnt = 0;

    while( n % 3 == 0 )       /*  Remove factors of 3. */
    {
        n /= 3;
        ++cnt;
    }

    if (cnt != 0)
    {
        primes[ t ] = 3;
        count[ t++ ] = cnt;
    }


    /*
        Factor the rest of n.  Continue until n = 1 (all factors divided out)
        or until n is prime (so n itself is the last prime factor).
    */

    do {

        /*  Integer divide to get quotient and remainder:  n = qd + r */

        q = n / d;

        r = n % d;

        n_is_prime = (r != 0 && q < d);   /* n is prime. */

        if (r == 0)
        {
            n = q;     /*  Divide d out of n. */

            if (new_d)  /*  New prime factor. */
            {
                primes[ t ] = d;
                count[ t++ ] = 1;
                new_d = NO;
             }
             else

                 ++count[ t-1 ];   /*  Same divisor again.  Increment its count. */
        }
        else {

            d += (flag ? 2 : 4);  /* d did not divide n.  Try a new divisor. */

            flag = !flag;
            new_d = YES;
        }

    } while ( ! n_is_prime && (n != 1));

    if (n == 1)       /*  All factors were divided out. */
    {
        return t - 1;
    }
    else
    {  /*  Current value of n is prime.  It is the last prime factor. */

        primes[ t ] = n;
        count[ t ] = 1;
        return t;
    }
} /* ========================= end of function factor ======================= */

/*
      4
Phi[ 5  - 1 ] / 4 

   4              4
  5  - 1 = 624 = 2  3  13

  Phi = 624 (1 - 1/2) (1 - 1/3)(1 - 1/13) = 192

  192 / 4 = 48


  Handle special cases.

  Warning: can take a long time due to factorization.
*/

bigint EulerPhi( bigint n )
{
    int prime_count = 0;
    int i = 0;
    bigint phi;

	/* Factor n into primes. */
    bigint primes[ MAXNUMPRIMEFACTORS ];
    int    count [ MAXNUMPRIMEFACTORS ];

    /* Error return and special cases. */
	if (n <= 0)
        return 0;
	else if (n == 1)
        return 1;

	/* Factor n >= 2 into distinct primes. */
    prime_count = factor( n, primes, count );

	/* Compute Euler phi[ n ] =   */
	/*                            */
	/*      num prime factors     */
	/*        -------             */
	/*     n   |  |   (1 - 1/p )  */
	/*         i = 1          i   */
    phi = n;
    for (i = 0;  i <= prime_count; ++i)
	{
        phi /=  primes[ i ];
        phi *= (primes[ i ] - 1);
	}
    return phi;
}



int is_probably_prime( int n, int x )
{
    int
        j,
        k = 0,
        nm1,
        q = 0,
        y = 0;


    /*  Not prime if input is out of range. */
    if (n < 0)
        return NO;

    /*  Handle special cases. */
    if (n == 0 || n == 1 || n == 4)
        return NO;

    if (n == 2 || n == 3 || n == 5)
        return YES;

    /* Even numbers aren't prime. */
    if (n % 2 == 0)
        return NO;

    /* Return not prime if x is out of range. */
    if (x <= 1 || x >= n)
        return NO;

    /* Factor out powers of 2 to get n = 1 + 2^k q, q odd. */
    nm1 = n - 1;
    k = 0;
    while( nm1 % 2 == 0)
    {
        nm1 /= 2;
        ++k;
    }

    q = nm1;

    /*  y = x^q (mod n) */
    y = power_mod( x, q, n );

    for (j = 0;  j < k;  ++j)
    {
        if ( (j == 0 && y == 1) || (y == n-1))
            return YES;
        else if (j > 0 && y == 1)
            return NO;

        /* Compute y^2 (mod n) */
        y = power_mod( y, 2, n );
    }

    /*  Shouldn't get here, but do it anyway for safety. */
    return NO;

} /* =============== end of function is_probably_prime ===================== */


int is_almost_surely_prime( int n )
{    
    int
        trial = 0,
        x     = 3;

    /* Seed the random-number generator. */
    srand( 314159 );

    for (trial = 1;  trial <= NUM_PRIME_TEST_TRIALS;  ++trial)
    {
        /*  Generate a new random integer such that 1 < x < n. */
        x = rand() % n;
        if (x <= 1) x = 3;

        /* Definitely not prime. */
        if (is_probably_prime( n, x ) == NO)
            return NO;
    }

    /* Almost surely prime because it passed the probably prime test
     * above many times.
     */
    return YES;
} /* ============== end of function is_almost_surely_prime ================ */
