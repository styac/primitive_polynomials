/*==============================================================================
|
|  File Name:
|
|     ppOrder.c
|
|  Description:
|
|     Routines for testing the order of polynomials.
|
|  Functions:
|
|     order_m
|     order_r
|     maximal_order
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


int order_m( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r,
             bigint * primes, int prime_count )
{    
    int
        i,                  /*  Loop counter.  */
        g[ MAXDEGPOLY ];   /* g(x) = x ^ m (mod f(x), p) */

    bigint
        m;                 /*  Exponent of m. */

    for (i = 0;  i <= prime_count;  ++i)

        if (!skip_test( i, primes, p ))
        {
            m = r / primes[ i ];

            x_to_power( m, g, power_table, n, p );

            #ifdef DEBUG_PP_PRIMPOLY
            std::cout << "    order m test for prime = " << primes[i] << ", x^ m = x ^ " << m << std::endl;
            write_poly( g, n-1 );
            std::cout << std::endl;
            #endif

            if (is_integer( g, n-1 ))
                return( NO );
        }

    return YES;
} /* ====================== end of function order_m ========================= */

int order_r( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r, int * a )
{
    int
        g[ MAXDEGPOLY ];   /* g(x) = x ^ m (mod f(x), p) */
    x_to_power( r, g, power_table, n, p );

    #ifdef DEBUG_PP_PRIMPOLY
    std::cout << "    order r test for x^r = x ^ %lld = " << r << std::endl;
    write_poly( g, n-1 );
    std::cout <<  std::endl;
    #endif

    /*  Return the value a = constant term of g(x) */
    *a = g[ 0 ];

    return( is_integer( g, n-1 ) ? YES : NO  );
} /* ====================== end of function order_r ========================= */

int maximal_order( int * f, int n, int p )
{
    int g[ MAXDEGPOLY ];   /* g(x) = x ^ m (mod f(x), p) */
    bigint maxOrder;
    bigint k;
    int power_table[ MAXDEGPOLY - 1 ] [ MAXDEGPOLY ];    /*  x ^ n , ... , x ^ 2n-2 (mod f(x), p) */

    /*                         n         2n-2
        Precompute the powers x ,  ..., x     (mod f(x), p)
        for use in all later computations.
    */
    construct_power_table( power_table, f, n, p );

    /*  Highest possible order for x. */
    maxOrder = power( p, n ) - 1;

    for (k = 1;  k <= maxOrder;  ++k)
    {
        x_to_power( k, g, power_table, n, p );

        if (is_integer( g, n-1 ) &&
            g[0] == 1 &&
            k < maxOrder)
        {
            return 0;
        }

    } /* end for k */

    return 1;
} /* ================= end of function maximal_order ======================== */
