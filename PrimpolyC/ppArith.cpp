/*==============================================================================
|
|  File Name:
|
|     ppArith.c
|
|  Description:
|
|     Routines for integer arithmetic modulo p.
|
|  Functions:
|
|      mod
|      power
|      power_mod
|      is_primitive_root
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

int  mod( int n, int p )
{
    int raw_mod = n % p;

    if (raw_mod == 0)
        return 0;
    else if (n >= 0)        /*  mod is not 0.  n >= 0. */
        return raw_mod;
    else
        return raw_mod + p;  /* mod is not 0.  n < 0. */
} /* =========================== end of function mod ======================== */


bigint power( int x, int y )
{
    bigint  pow;   /*  x to the y.  */

    for (pow = 1;  y > 0;  --y)
        pow *= x;

    return pow;
} /* ========================= end of function power ======================== */

int power_mod( int a, int n, int p )
{
    /*
        mask = 1000 ... 000.  That is, all bits of mask are zero except for the
        most significant bit of the computer word holding its value.  Signed or
        unsigned type for bigint shouldn't matter here, since we just mask and
        compare bits.
    */

    int
        mask = (1 << (8 * sizeof( int ) - 1)),
        bit_count = 0;  /* Number of bits in exponent to the right of the leading bit. */

    /*  Use extra precision since we need to square an integer within the
        calculations below. */
    long int product = a;

    /*
        Out of range conditions.
    */

    if (a < 0 || n < 0 || p <= 1 || (a == 0 && n == 0))
        return -1;

    /*  Quick return for 0 ^ n, a^0 and a^1.  */
    if (a == 0)
        return 0;

    if (n == 0)
        return 1;

    if (n == 1)
        return a % p;

    /*
        Advance the leading bit of the exponent up to the word's left
        hand boundary.  Count how many bits were to the right of the
        leading bit.
    */

    while (! (n & mask))
    {
        n <<= 1;
        ++bit_count;
    }

    bit_count = (8 * sizeof( int )) - bit_count;

    /*
        Exponentiation by repeated squaring.  Discard the leading 1 bit.
        Thereafter, square for every 0 bit;  square and multiply by x for
        every 1 bit.
    */

    while ( --bit_count > 0 )
    {
        /*  Expose the next bit. */
        n <<= 1;

        /* Square modulo p. */
        product = (product * product) % p;

        /*  Leading bit is 1: multiply by a modulo p. */
        if (n & mask)
            product = (a * product) % p;
    }

    /*  We don't lose precision because product is always reduced modulo p. */
    return product;
} /* ========================= end of function power_mod ==================== */

int is_primitive_root( int a, int p )
{
    bigint
        primes[ MAXNUMPRIMEFACTORS ]; /* The distinct prime factors of p-1. */

    int
        count[ MAXNUMPRIMEFACTORS ],   /* ... and their multiplicities.      */
        t,                             /*  Primes are indexed from 0 to t.   */
        i;                            /*  Loop index.                       */

    /*  Error for out of range inputs, including p
        being an even number greater than 2.
    */
    if (p < 2 || a < 1 || (p > 2 && (p % 2 == 0)))
        return NO;

    /*  Special cases:
        1 is the only primitive root of 2;  i.e. 1 generates the unit elements
        of GF( 2 );  2 is the only primitive root of 3, and 2 and 3 are the only
        primitive roots of 5, etc.  See http://mathworld.wolfram.com/PrimitiveRoot.html
    */
    if ( (p == 2  && a == 1) ||
         (p == 3  && a == 2) ||
         (p == 5  && (a == 2 || a == 3)) ||
         (p == 7  && (a == 3 || a == 5)) ||
         (p == 11 && (a == 2 || a == 6 || a == 7 || a ==  8)) ||
         (p == 13 && (a == 2 || a == 6 || a == 7 || a == 11)) )
        return YES;

    /*  Reduce a down modulo p. */
    a = a % p;

    /*  a = 0 (mod p);  Zero can't be a primitive root of p. */
    if (a == 0)
        return NO;

    /*  Factor p-1 into primes. */
    t = factor( (bigint)(p - 1), primes, count );


    /*  Check if a^(p-1)/q <> 1 (mod p) for all primes q | (p-1).
        If so, we have a primitive root of p, otherwise, we exit early.
     */
    for (i = 0;  i <= t;  ++i)
    {
        /*  The prime factors can be safely cast from bigint to int
            because p-1 needs only integer precision.  */
        if (power_mod( a, (p-1) / (int)primes[ i ], p ) == 1)
            return NO;
    }

    return YES;
} /* =============== end of function is_primitive_root ===================== */

int inverse_mod_p( int u, int p )
{
    /*  Do Euclid's algorithm to find the quantities */
    int t1 = 0;
    int t3 = 0;
    int q  = 0;

    int u1 = 1;
    int u3 = u;
    int v1 = 0;
    int v3 = p;

    int inv_v = 0;

	while( v3 != 0)
	{
        q = (int)(u3 / v3);

        t1 = u1 - v1 * q;
        t3 = u3 - v3 * q;

        u1 = v1;
        u3 = v3;

        v1 = t1;
        v3 = t3;
	}

    inv_v = mod( u1, p );

	/* Self check. */
	if ( mod( u * inv_v, p ) != 1)
	{
        return 0;
	}
    return inv_v;
}
