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

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> /* for rand() function */

#include "Primpoly.h"


/*==============================================================================
|                                order_m                                       |
================================================================================

DESCRIPTION
                m
    Check that x  (mod f(x), p) is not an integer for m = r / p  but skip
                                           n                   i
                                          p  - 1
    this test if p  | (p-1).  Recall r = -------.
                  i                       p - 1

INPUT

     power_table (int **)     x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.
     n      (int, n >= 1)     Degree of f(x).
     p      (int)             Modulo p coefficient arithmetic.
     r (int)                  See above.
     primes (bigint *)        Distinct prime factors of r.
     prime_count              Number of primes.

RETURNS

     YES    if all tests are passed.
     NO     if any one test fails.

EXAMPLE
                                           2
     Let n = 4 and p = 5.  Then r = 156 = 2 * 3 * 13, and p = 2, p = 3,
                                                            1      2
     and p = 13.  m = { 156/2, 156/3, 156/13 } = { 78, 52, 12 }.  We can
          3
     skip the test for m = 78 because p = 2 divides p-1 = 4.  Exponentiation
                                       1
            52       3   2                                      12
     gives x    = 2 x + x + 4 x, which is not an integer and x =
        3       2
     4 x  +  2 x  +  4 x  + 3 which is not an integer either, so we return

     YES.

METHOD

    Exponentiate x with x_to_power and test the result with is_integer.
    Return right away if the result is not an integer.

BUGS

    None.

*/

int order_m( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r,
             bigint * primes, int prime_count );

/*==============================================================================
|                                order_r                                       |
================================================================================

DESCRIPTION
              r
    Check if x  (mod f(x), p) = a, where a is an integer.

INPUT

     power_table (int **)     x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.
     n      (int, n >= 1)     Degree of f(x).
     p      (int)             Modulo p coefficient arithmetic.
     r (int)                  See above.
     a (int *)                Pointer to value of a.

RETURNS

     YES    if the formula above is true.
     NO     if it isn't.

EXAMPLE
                          4    2
     Let r = 156, f(x) = x  + x  + 2 x + 3, n = 4 and p = 5.  It turns
               156                           4
     out that x    = 3 (mod f(x), 5) = (-1)  * 3, so we return YES.

METHOD
                          r
    First compute g(x) = x (mod f(x), p).
    Then test if g(x) is a constant polynomial,

BUGS

    None.
*/

int order_r( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r, int * a );

/*==============================================================================
|                                  maximal_order                               |
================================================================================

DESCRIPTION
              k                                  n
    Check if x  = 1 (mod f(x), p) only when k = p  - 1 and not for any smaller
    power of k, i.e. that f(x) is a primitive polynomial.

INPUT

     f (int *)                Monic polynomial f(x).
     n      (int, n >= 1)     Degree of f(x).
     p      (int)             Modulo p coefficient arithmetic.

RETURNS

     YES    if f( x ) is primitive.
     NO     if it isn't.

EXAMPLE

               4
     f( x ) = x  + x  +  1 is a primitive polynomial modulo p = 2,
                                         4
     because it generates the group GF( 2  ) with the exception of
                            2         15
     zero.  The powers {x, x , ... , x  } modulo f(x), mod 2 are
                                        16       4
     distinct and not equal to 1 until x   (mod x  + x + 1, 2) = 1.

METHOD

    Confirm f(x) is primitive using the definition of primitive
    polynomial as a generator of the Galois group
         n                   n
    GF( p ) by testing that p - 1 is the smallest power for which
     k
    x = 1 (mod f(x), p).

BUGS

    None.
*/

int maximal_order( int * f, int n, int p );

