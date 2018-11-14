/*==============================================================================
| 
|  File Name:     
|
|     ppPolyArith.c
|
|  Description:   
|
|     Polynomial arithmetic and exponentiation routines.
| 
|  Functions:
|
|     eval_poly
|     linear_factor
|     is_integer
|     construct_power_table
|     auto_convolve
|     convolve
|     coeff_of_square
|     coeff_of_product
|     square
|     product
|     times_x
|     x_to_power
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

#include "Primpoly.h"
#include "ppArith.h"
#include "ppHelperFunc.h"
#include "ppIO.h"
#include "ppOrder.h"
#include "ppPolyArith.h"
#include "ppFactor.h"

int eval_poly( int * f, int x, int n, int p )
{
    int val = 1;   /*  The value of f(x) which is returned. */

    while ( --n >= 0 )
        val = mod( val * x + f[ n ], p );

    return val;
} /* ========================= end of function eval_poly ==================== */

int linear_factor( int * f, int n, int p )
{
    int i;  /*  Loop counter. */

    for (i = 1;  i <= p-1;  ++i)
        if (eval_poly( f, i, n, p ) == 0)
            return( YES );

    return NO;
} /* ====================== end of function linear_factor =================== */

int is_integer( int * t, int n )
{

    while ( n >= 1 )
        if (t[ n-- ] != 0)
            return( NO );

    return( YES );
} /* ====================== end of function is_integer ====================== */

void construct_power_table( int power_table[][ MAXDEGPOLY ], int * f,
                           int n, int p )
{
    int
        i, j,                  /*  Loop counters.  */
        coeff,                 /*  Coefficient of x ^ n in t(x) */
        t[ MAXDEGPOLY + 1 ];  /*  t(x) is temporary storage for x ^ k (mod f(x),p)
                                   n <= k <= 2n-2.  Its degree can go as high as
                                   n before it is reduced again. */
    /*
                             n-1
        Initialize t( x ) = x
    */

    for (i = 0;  i <= n-2;  ++i)
        t[ i ] = 0;

    t[ n-1 ] = 1;

    /*
                                            i+n
        Fill the ith row of the table with x   (mod f(x), p).
    */

    for (i = 0;  i <= n-2;  ++i)
    {
        /*  Compute t(x) = x t(x)  */
        for (j = n;  j >= 1;  --j)
            t[ j ] = t[ j-1 ];

        t[ 0 ] = 0;

        /*  Coefficient of the nth degree term of t(x).  */
        if ( (coeff = t[ n ]) != 0)
        {
            t[ n ] = 0;               /*  Zero out the x ^ n th term. */

           /*
                     n       n                        n-1
            Replace x  with x  (mod f(x), p) = -(a   x   + ... + a )
                                                  n-1             0
           */

            for (j = 0;  j <= n-1;  ++j)

                t[ j ] = mod( t[ j ] +
                              mod( -coeff * f[ j ], p ),
                              p );
        }  /* end if */


        /*  Copy t(x) into row i of power_table. */
        for (j = 0;  j <= n-1; ++j)
            power_table[i][j] = t[ j ];

    } /* end for */
} /* ================== end of function construct_power_table =============== */

int auto_convolve( int * t, int k, int lower, int upper, int p )
{

    int
        sum = 0,      /* Convolution sum. */
        i;           /* Loop counter.    */

    for (i = lower;  i <= upper;  ++i)
       sum = mod( sum + mod( t[ i ] * t[ k - i ], p ), p );

    return sum;
} /* ==================== end of function auto_convolve ===================== */

int convolve( int * s, int * t, int k, int lower, int upper, int p )
{

    int
        sum = 0,      /* Convolution sum. */
        i;           /* Loop counter.    */

    for (i = lower;  i <= upper;  ++i)
       sum = mod( sum + mod( s[ i ] * t[ k - i ], p ), p );

    return sum;
} /* ======================= end of function convolve ======================= */

int coeff_of_square( int * t, int k, int n, int p )
{
    int
        sum = 0;      /* kth coefficient of t(x) ^ 2. */
    if (0 <= k && k <= n-1)
    {

        if (k % 2 == 0)        /* Even k */

            sum = mod( mod( 2 * auto_convolve( t, k, 0, k/2 - 1, p ), p) +
                       t[ k/2 ] * t[ k/2 ], p );

         else                  /* Odd k */

             sum = mod( 2 * auto_convolve( t, k, 0, (k-1)/2, p ), p);
    }
    else if (n <= k && k <= 2 * n - 2)
    {

        if (k % 2 == 0)        /* Even k */

            sum = mod( mod( 2 * auto_convolve( t, k, k/2 + 1, n-1, p ), p) +
                       t[ k/2 ] * t[ k/2 ], p );

         else                  /* Odd k */

             sum = mod( 2 * auto_convolve( t, k, (k+1)/2, n-1, p ), p);
    }
    return sum;
} /* ================== end of function coeff_of_square ===================== */

int coeff_of_product( int * s, int * t, int k, int n, int p )
{
    int
        sum = 0;      /* kth coefficient of t(x) ^ 2. */
    if (0 <= k && k <= n-1)
    {
        sum = convolve( s, t, k, 0, k, p );
    }
    else if (n <= k && k <= 2 * n - 2)
    {
        sum = convolve( s, t, k, k - n + 1, n - 1, p );
    }
    return sum;
} /* ================== end of function coeff_of_product ==================== */

void square( int * t, int power_table[][ MAXDEGPOLY ], int n, int p )
{
    int
        i, j,                     /* Loop counters. */
        coeff,                    /* Coefficient of x ^ k term of t(x) ^2 */
        temp[ MAXDEGPOLY + 1 ];  /* Temporary storage for the new t(x). */
    /*
                                     0        n-1
        Compute the coefficients of x , ..., x.   These terms do not require

        reduction mod f(x) because their degree is less than n.
    */

    for (i = 0;  i <= n;  ++i)
        temp[ i ] = coeff_of_square( t, i, n, p );

    /*
                                     n        2n-2             k
        Compute the coefficients of x , ..., x.    Replace t  x  with
                k                                            k
        t  * [ x  (mod f(x), p) ] from array power_table when t is
         k                                                     k
        non-zero.

    */

    for (i = n;  i <= 2 * n - 2;  ++i)
        if ( (coeff = coeff_of_square( t, i, n, p )) != 0 )
            for (j = 0;  j <= n - 1;  ++j)
                temp[ j ] = mod( temp[ j ] +
                                 mod( coeff * power_table[ i - n ] [ j ], p ),
                                 p );

    for (i = 0;  i <= n - 1;  ++i)
        t[ i ] = temp[ i ];

} /* ======================== end of function square ======================== */


void product( int  * s, int * t, int power_table[][ MAXDEGPOLY ], int n, int p )
{
    int
        i, j,                     /* Loop counters. */
        coeff,                    /* Coefficient of x ^ k term of t(x) ^2 */
        temp[ MAXDEGPOLY + 1 ];  /* Temporary storage for the new t(x). */
    /*
                                     0        n-1
        Compute the coefficients of x , ..., x.   These terms do not require

        reduction mod f(x) because their degree is less than n.
    */

    for (i = 0;  i <= n;  ++i)
        temp[ i ] = coeff_of_product( s, t, i, n, p );

    /*
                                     n        2n-2             k
        Compute the coefficients of x , ..., x.    Replace t  x  with
                k                                            k
        t  * [ x  (mod f(x), p) ] from array power_table when t is
         k                                                     k
        non-zero.

    */

    for (i = n;  i <= 2 * n - 2;  ++i)
        if ( (coeff = coeff_of_product( s, t, i, n, p )) != 0 )
            for (j = 0;  j <= n - 1;  ++j)
                temp[ j ] = mod( temp[ j ] +
                                 mod( coeff * power_table[ i - n ] [ j ], p ),
                                 p );

    for (i = 0;  i <= n - 1;  ++i)
        s[ i ] = temp[ i ];

} /* ======================== end of function product ======================= */

void times_x( int * t, int power_table[][ MAXDEGPOLY ], int n, int p )
{

    int
        i,              /* Loop counter. */
        coeff;         /* Coefficient of x ^ n term of x t(x). */

    /*
        Multiply t(x) by x.  Do it by shifting the coefficients left in the array.
    */

    coeff = t[ n - 1 ];

    for (i = n-1;  i >= 1;  --i)

        t[ i ] = t[ i-1 ];

    t[ 0 ] = 0;

    /*
                      n                          n
        Zero out t  x .  Replace it with t * [ x  (mod f(x), p) ] from array
                  n
        power_table.
    */

    if (coeff != 0)
    {
        for (i = 0;  i <= n - 1;  ++i)

            t[ i ] = mod( t[ i ] +
                          mod( coeff * power_table[ 0 ] [ i ], p ),
                          p );
    }

} /* ======================== end of function times_x ======================= */

void x_to_power( bigint m, int * g, int power_table[][ MAXDEGPOLY ], int n, int p )
{
    /*
        mask = 1000 ... 000.  That is, all bits of mask are zero except for the
        most significant bit of the computer word holding its value.  Signed or
        unsigned type for bigint shouldn't matter here, since we just mask and
        compare bits.
    */

    bigint
        mask = ((bigint)1 << (NUMBITS - 1));

    int
        bit_count = 0,  /* Number of bits in m to the right of the leading bit. */
        i;             /* Loop counter. */

    #ifdef DEBUG_PP_PRIMPOLY
    std::cout << "x to power = x^" << m << std::endl;
    #endif

    /*
        Initialize g(x) to x.  Exit right away if m = 1.
    */

    for (i = 0;  i <= n-1;  ++i)

        g[ i ] = 0;

    g[ 1 ] = 1;

    if (m == 1)
        return;

    /*
        Advance the leading bit of m up to the word's left hand boundary.
        Count how many bits were to the right of the leading bit.
    */

    while (! (m & mask))
    {
        m <<= 1;
        ++bit_count;
    }

    bit_count = NUMBITS - bit_count;

    /*
        Exponentiation by repeated squaring.  Discard the leading 1 bit.
        Thereafter, square for every 0 bit;  square and multiply by x for
        every 1 bit.
    */

    while ( --bit_count > 0 )
    {

        m <<= 1;       /*  Expose the next bit. */

        square( g, power_table, n, p );

        #ifdef DEBUG_PP_PRIMPOLY
        std::cout << "    after squaring, poly = \n" << std::endl;
        write_poly( g, n-1 );
        std::cout << std::endl;
        #endif

        if (m & mask)   /*  Leading bit is 1. */
        {
            times_x( g, power_table, n, p );

            #ifdef DEBUG_PP_PRIMPOLY
            std::cout << "    after additional times x, poly = \n" << std::endl;
            write_poly( g, n-1 );
            std::cout << std::endl;
            #endif
        }
    }
} /* ===================== end of function x_to_power ======================= */
