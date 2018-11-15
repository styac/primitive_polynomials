/*==============================================================================
| 
|  File Name:     
|
|     ppHelperFunc.c
|
|  Description:   
|
|     Higher level helper functions which don't belong elsewhere.
| 
|  Functions:
|
|     initial_trial_poly
|     next_trial_poly
|     const_coeff_test
|     const_coeff_is_primitive_root
|     skip_test
|     has_multi_irred_factors
|     generate_Q_matrix
|     find_nullity
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

#include <stdlib.h>
#include <memory.h>

#include "Primpoly.h"
#include "ppArith.h"
#include "ppHelperFunc.h"
#include "ppIO.h"
#include "ppOrder.h"
#include "ppPolyArith.h"
#include "ppFactor.h"

void initial_trial_poly( int * f, int n )
{
    int i;
    f[ 0 ] = -1;

    for (i = 1;  i <= n-1;  ++i)

        f[ i ] = 0;

    f[ n ] = 1;

} /* ================= end of function initial_trial_poly ==================== */

void next_trial_poly( int * f, int n, int p )
{
    int digit_num;   /*  Loop counter and digit number. */

    ++f[ 0 ];     /* Add 1, i.e. increment the coefficient of the x term. */

    /*
         Sweep through the number from right to left, propagating carries.  Skip
         the constant and the nth degree terms.
    */

    for (digit_num = 0;  digit_num <= n - 2;  ++digit_num)
    {
        if (f[ digit_num ] == p)   /*  Propagate carry to next digit. */
        {
            f[ digit_num ] = 0;
            ++f[ digit_num + 1 ];
        }
    }
} /* ================= end of function next_trial_poly ====================== */

int const_coeff_test( int * f, int n, int p, int a )
{
    int constantCoeff = f[ 0 ];
    if (n % 2 != 0)
        constantCoeff = -constantCoeff;    /* (-1)^n < 0 for odd n. */

    return (mod( a - constantCoeff, p ) == 0) ? YES : NO;
} /* =================== end of function const_coeff_test ================ */

int const_coeff_is_primitive_root( int * f, int n, int p )
{
    int constantCoeff = f[ 0 ];
    /* (-1)^n < 0 for odd n. */
    if (n % 2 != 0)
        constantCoeff = -constantCoeff;

    return is_primitive_root( mod( constantCoeff, p ), p );
} /* ============= of function const_coeff_is_primitive_root ================ */


int skip_test( int i, bigint * primes, int p )
{
    /*
        p  cannot divide the smaller number (p-1)
         i
    */
    if ( (bigint)( p-1 ) < primes[ i ] )
        return NO;
    else
        return( ((bigint)(p-1) % primes[ i ] == 0) ? YES : NO );
} /* ====================== end of function skip_test ======================= */

int has_multi_irred_factors( int power_table[][ MAXDEGPOLY ], int n, int p )
{
    int ** Q;
    int row;
    int nullity = 0;


    /* Allocate space for the Q matrix. */
    Q = (int **) calloc( n, sizeof( int * ) );

    for (row = 0;  row < n;  ++row)
    {
        Q[ row ] = (int *)calloc( n, sizeof( int ) );
    }


	/* Generate the Q-I matrix. */
    generate_Q_matrix( Q, power_table, n, p );


	/* Find nullity of Q-I */
    nullity = find_nullity( Q, n, p );

    /* Free space for the Q matrix. */
    for (row = 0;  row < n;  ++row)
    {
        free( Q[ row ] );
    }

    free( Q );

	/* If nullity >= 2, f( x ) is a reducible polynomial modulo p since it has  */
	/* two or more distinct irreducible factors.                                */
	/*                                     e                                    */
	/* Nullity of 1 implies f( x ) = p( x )  for some power e >= 1 so we cannot */
	/* determine reducibility.                                                  */
	if (nullity >= 2)
        return 1;

    return 0;

} /* =============== end of function has_multi_irred_factors ================ */

void generate_Q_matrix( int ** Q, int power_table[][ MAXDEGPOLY ], int n, int p )
{
    int xp[ MAXDEGPOLY ]; /* x^p (mod f(x),p)         */
    int q [ MAXDEGPOLY ]; /* Current row of Q matrix. */

    int row = 0;
    /* Check for invalid inputs. */
    if (n < 2 || p < 2 || Q == (int **)0)
    {
        return;
    }

    /* Row 0 of Q = 1. */
    Q[ 0 ][ 0 ] = 1;

    /*       p
       Find x  (mod f(x),p) save the value into q(x) and
       set row 1 of Q to this value.
    */
    x_to_power( (bigint) p, xp, power_table, n, p );

    memcpy( q,     xp, n * sizeof( int ) );
    memcpy( Q[ 1 ], q, n * sizeof( int ) );

    /*               pk
       Row k of Q = x   (mod f(x), p) 2 <= k <= n-1, computed by
                                         p
       multiplying each previous row by x (mod f(x),p).
    */
    for (row = 2;  row <= n-1;  ++row)
    {
        product( q, xp, power_table, n, p );
        memcpy( Q[ row ], q, n * sizeof( int ) );
    }


    /*  Subtract Q - I */

    for (row = 0;  row < n;  ++row)
    {
        Q[ row ][ row ] = mod( Q[ row ][ row ] - 1, p );
    }
} /* ================== end of function generate_Q_matrix =================== */


int find_nullity( int ** Q, int n, int p )
{
    int colFlag[ MAXDEGPOLY ]; /* Is -1 if the column has no pivotal element. */
    int nullity = 0;
    int row, col;
    int r;
    int found = NO;
    int pivotCol = -1;
    int q = 0;
    int t = 0;

    /*  Initialize column flags to -1. */
    memset( colFlag, -1, n * sizeof( int ) );

    /* Sweep through each row. */
    for (row = 0;  row < n;  ++row)
    {
        /* Search for a pivot in this row:  a non-zero element
           in a column which had no previous pivot.
         */
        found = NO;
        for (col = 0;  col < n;  ++col)
        {
            if (Q[ row ][ col ] > 0 && colFlag[ col ] < 0)
            {
                found = YES;
                pivotCol = col;
                break;
            }
        }

        /* No pivot;  increase nullity by 1. */
        if (found == NO)
        {
            /* Early out. */
            if (++nullity >= 2)
            {
                return nullity;
            }
        }
        /* Found a pivot, q. */
        else
        {
            q = Q[ row ][ pivotCol ];

            /* Compute -1/q (mod p) */
            t = mod( -inverse_mod_p( q, p ), p );

            /* Normalize the pivotal column. */
            for (r = 0;  r < n;  ++r)
            {
                Q[ r ][ pivotCol ] = mod( t * Q[ r ][ pivotCol ], p );
            }

            /* Do column reduction:  Add C times the pivotal column to the other
               columns where C = element in the other column at current row. */
            for (col = 0;  col < n;  ++col)
            {
                if (col != pivotCol)
                {
                    q = Q[ row ][ col ];

                    for (r = 0;  r < n;  ++r)
                    {
                        t = mod( q * Q[ r ][ pivotCol ], p );
                        Q[ r ][ col ] = mod( t + Q[ r ][ col ], p );
                    }
                }
            }

            /* Record the presence of a pivot in this column. */
            colFlag[ pivotCol ] = row;

        } /* found a pivot */

    } /* end for row */

    return nullity;
} /* ===================== end of function find_nullity ===================== */
