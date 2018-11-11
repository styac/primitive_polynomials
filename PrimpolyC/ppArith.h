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

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include "Primpoly.h"

/*==============================================================================
|                                  mod                                         |
================================================================================

DESCRIPTION

     A mod function for both positive and negative arguments.

INPUT

    n (int, -infinity < n < infinity)
    p (int, p > 0)

RETURNS

    n mod p (0 <= n mod p < p)

EXAMPLE

    The C language gives -5 % 3 = - ( -(-5) mod 3 ) = -( 5 mod 3 ) = -2.
    The result is nonzero, so we add p=3 to give 1.

    C computes -3 % 3 = - ( -(-3) mod 3  ) = 0, which we leave unchanged.

METHOD

     For n >= 0, the C language defines r = n mod p by the equation

         n = kp + r    0 <= r < p

     but when n < 0, C returns the quantity

         r = - ( (-n) mod p )

     To put the result into the correct range 0 to p-1, add p to r if
     r is non-zero.

     By the way, FORTRAN's MOD function does the same thing.

BUGS

     None.
*/

int  mod( int n, int p );

/*==============================================================================
|                                       power                                  |
================================================================================

DESCRIPTION

     Raise a positive integer to a positive power exactly.

INPUT

     x (int, x > 0)   The base.
     y (int, y > 0)   The exponent.

RETURNS

     y
    x

EXAMPLE CALLING SEQUENCE

     power( 2, 3 ) => 8

METHOD

     Multiply x by itself y times.

BUGS

     None.
*/

bigint power( int x, int y );

/*==============================================================================
|                                power_mod                                     |
================================================================================

DESCRIPTION

     Raise a positive integer to a positive power modulo an integer.

INPUT

     a (int, a >= 0)   The base.
     n (int, n >= 0)   The exponent.
     p (int, p >= 2)   The modulus.

RETURNS

      n
     a  (modulo p)
    -1 if a < 0, n < 0, or p <= 1, or 0^0 case.

EXAMPLE CALLING SEQUENCE

     power_mod( 2, 3, 7 ) => 1

METHOD

     Multiplication by repeated squaring.

BUGS

     None.

*/

int power_mod( int a, int n, int p );

/*==============================================================================
|                             is_primitive_root                                |
================================================================================

DESCRIPTION

     Test whether the number a is a primitive root of the prime p.

INPUT

     a      (int, a >= 1)     The number to be tested.
     p      (int, p >= 2)     A prime number.

RETURNS

     YES    If a is a primitive root of p.
     NO     If a isn't a primitive root of p or if inputs are out of range.

EXAMPLE
                                                     1      2      3
     3 is a primitive root of the prime p = 7 since 3 = 3, 3 = 2, 3 = 6,
      4      5                                       p-1    6
     3 = 4, 3 = 5 (mod 7); all not equal to 1 until 3    = 3 = 1 (mod 7).

     So 3 is a primitive root of 7 because it has maximal period.  a = 2
                                                      3
	 isn't a primitive root of p = 7 because already 2 = 1 (mod 7).

METHOD

    From number theory, a is a primitive root of the prime p iff
     (p-1)/q
    a        != 1 (mod p) for all prime divisors q of (p-1).
                            (p-1)
    Don't need to check if a     = 1 (mod p):  Fermat's little
    theorem guarantees it.

BUGS

    We don't check if p is not a prime.


*/

int is_primitive_root( int a, int p );

/* Divide modulo p.

Do extended Euclid's algorithm on u and v to find u1, u2, and u3 such that

    u u1 + v u2 = u3 = gcd( u, v ).

Now let v = p = prime number, so gcd = u3 = 1.  Then we get

  u u1 + p ? = 1

or u u1 = 0 (mod p) which makes u (mod p) the unique multiplicative
inverse of u.


Assume 0 <= u < p, p is prime >= 2.

*/

int inverse_mod_p( int u, int p );

