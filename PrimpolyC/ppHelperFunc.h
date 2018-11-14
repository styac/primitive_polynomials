#pragma once
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

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <memory.h>
#include "Primpoly.h"

/*==============================================================================
|                             initial_trial_poly                               |
================================================================================

DESCRIPTION

    Return the initial monic polynomial in the sequence for f(x).

INPUT
                   
     f (int *)                Monic polynomial f(x). 
     n      (int, n >= 1)     Degree of f(x).

RETURNS
                                             n
     f (int *)                Sets f( x ) = x  - 1

  EXAMPLE 
                             4
     Let n = 4.  Set f(x) = x  - 1.

METHOD

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void initial_trial_poly( int * f, int n );

/*==============================================================================
|                                next_trial_poly                               |
================================================================================

DESCRIPTION

    Return the next monic polynomial in the sequence after f(x).

INPUT
                   
    f (int *)           Monic polynomial f(x). 
    n (int, n >= 1)     Degree of monic polynomial f(x).
    p (int, p >= 2)     Modulo p coefficient arithmetic.

RETURNS

     f (int *)                Overwrites f(x) with the next polynomial 
                              after it in the sequence (explained below).
EXAMPLE 
                                           3
     Let n = 3 and p = 5.  Suppose f(x) = x  + 4 x + 4.  As a mod p number,
     this is 1 0 4 4.  Adding 1 gives 1 0 4 5.  We reduce modulo
     5 and propagate the carry to get the number 1 0 5 0.  Propagating
     the carry again and reducing gives 1 1 0 0.  The next polynomial
                     3    2
     after f(x) is  x  + x .

METHOD

     Think of the polynomial coefficients as the digits of a number written
     in base p.  The "next" polynomial is the one you would get by adding 1
     to this number in multiple precision arithmetic.  Our intention is to
     run through all possible monic polynomials modulo p.

     Propagate carries in digits 1 through n-2 when any digit exceeds p.  No
     carries take place in the n-1 st digit because our polynomial is monic.

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void 
    next_trial_poly( int * f, int n, int p );


/*==============================================================================
|                               const_coeff_test                               |
================================================================================

DESCRIPTION

                  n
  Test if a = (-1)  a  (mod p) where a  is the constant coefficient
                     0                0
  of polynomial f(x) and a is the number from the order_r() test.

INPUT

    f (int *)  nth degree monic mod p polynomial f(x). 
    n (int)    Its degree.
    p (int)    Modulus for coefficient arithmetic.
    a (int)

RETURNS

    YES    if we pass the test.
    NO     otherwise

EXAMPLE 
                                 11     3
    Let p = 5, n = 11, f( x ) = x  + 2 x + 1, and a = 4.
    Then return YES since

        11
    (-1)  * 1 (mod 5) = -1 (mod 5) = 4 (mod 5).

METHOD
                        n
    We test if (a - (-1) a ) mod p = 0.
                          0
BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int const_coeff_test( int * f, int n, int p, int a );



/*==============================================================================
|                         const_coeff_is_primitive_root                        |
================================================================================

DESCRIPTION

              n
  Test if (-1)  a  (mod p) is a primitive root of the prime p where
                 0
  a  is the constant term of the polynomial f(x).
   0

INPUT

    f (int *)  nth degree monic mod p polynomial f(x). 
    n (int)    Its degree.
    p (int)    Modulus for coefficient arithmetic.

EXAMPLE 
                                 11     3
    Let p = 7, n = 11, f( x ) = x  + 2 x + 4.
    Then return YES since

        11
    (-1)  * 4 (mod 7) = -4 (mod 7) = 3 (mod 7), and 3 is a
                                         1      2      3
    primitive root of the prime 7 since 3 = 3, 3 = 2, 3 = 6,
     4      5                                       7-1
    3 = 4, 3 = 5 (mod 7); all not equal to 1 until 3   = 1 (mod 7).

METHOD

                   n
    We test if (-1) a  mod p is a primitive root mod p.
                     0
BUGS

    None.
*/

int const_coeff_is_primitive_root( int * f, int n, int p );

/*==============================================================================
|                                    skip_test                                 |
================================================================================

DESCRIPTION

     Make the test p  | (p - 1).
                    i

INPUT

    i      (int)       ith prime divisor of r.

    primes (bigint *)  Array of distict prime factors of r.  primes[ i ] = p
                                                                            i 
    p      (int)        p >= 2.

RETURNS
                      
    YES    if the test succeeds for some i.
    NO     otherwise

EXAMPLE 
   
    Suppose i = 0, primes[ 0 ] = 2 and p = 5.  Return YES since 2 | 5-1.

METHOD

    Test if (p - 1) mod p = 0 for all i.
                         i
BUGS

    None.
*/

int  skip_test( int i, bigint * primes, int p );


/*==============================================================================
|                              has_multi_irred_factors                         |
================================================================================

DESCRIPTION

   Find out if the monic polynomial f( x ) has two or more distinct 
   irreducible factors.

INPUT

    power_table (int **)   x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.
    n (int, n >= 1)        Degree of monic polynomial f(x).
    p (int, p >= 2)        Modulo p coefficient arithmetic.

RETURNS
                      
   1 if the monic polynomial f( x ) has two or more distinct rreducible factors
   0 otherwise.

EXAMPLE 
   
   Let n = 4, p = 5

             4
   f( x ) = x  + 2 is irreducible, so has one distinct factor.

             4    3   2                  4
   f( x ) = x + 4x + x + 4x + 1 = (x + 1)  has one distinct factor.

             3         2
   f( x ) = x  + 3 = (x + 3x + 4)(x + 2) has two distinct irreducible factors.

             4    3    2                          2
   f( x ) = x + 3x + 3x + 3x + 2 = (x + 1) (x + 2) (x + 3) has 3 distinct
   irreducible factors.

           4
   f(x) = x + 4 = (x+1)(x+2)(x+3)(x+4) has 4 distinct irreducible factors.

METHOD

      Berlekamp's method for factoring polynomials over GF( p ), modified to test
	  for irreducibility only.

      See my notes;  I skip the polynomial GCD step which ensures polynomials
	  are square-free due to time constraints, but this requires a proof that 
	  the method is still valid.	 

BUGS

    None.
*/

int has_multi_irred_factors( int power_table[][ MAXDEGPOLY ], int n, int p );


/*==============================================================================
|                              generate_Q_matrix                               |
================================================================================

DESCRIPTION


    Generate n x n matrix Q - I, where rows of Q are the powers,

        p                2p                      (n-1) p
    1, x  (mod f(x),p), x  (mod f(x), p), ... , x       (mod f(x), p)

    for monic polynomial f(x).


INPUT

    Q (int **)             Memory is allocated for this matrix already and 
	                       all entries are 0.

                            k
    power_table (int **)   x (mod f(x), p) for n <= k <= 2n-2, f monic.
  
    n (int, n >= 1)        Degree of monic polynomial f(x).

    p (int, p >= 2)        Modulo p coefficient arithmetic.

RETURNS
                      

EXAMPLE 
   
            4
    f(x) = x + 2, n = 4, p = 5

        (   1      )    (    1   )      ( 1    0    0    0 )
	    (     5    )    (        )      (                  )
    Q = (    x     )    (   3x   )      ( 0    3    0    0 )
        (     10   ) =  (     2  )   =  (                  )
	    (    x     )    (   4x   )      ( 0    0    4    0 )
	    (     15   )    (     3  )      (                  )
	    (    x     )    (   2x   )      ( 0    0    0    2 )

                                                    2    3
                                          1   x    x    x

		     ( 0 0 0 0 )
             ( 0 2 0 0 )
    Q - I =  ( 0 0 3 0 )
             ( 0 0 0 1 )

METHOD

      Modified from ART OF COMPUTER PROGRAMMING, Vol. 2, 2nd ed., Donald E. Knuth, 
	  Addison-Wesley.

BUGS

    None.

*/

void generate_Q_matrix( int ** Q, int power_table[][ MAXDEGPOLY ], int n, int p );


/*==============================================================================
|                                  find_nullity                                |
================================================================================

DESCRIPTION


INPUT

    Q (int **)          Matrix of integers mod p. 
    n (int, n >= 1)     Degree of monic polynomial f(x).
    p (int, p >= 2)     Modulo p coefficient arithmetic.

RETURNS

    Nullity of Q.  However if the nullity is 2 or more, we just return 2.

EXAMPLE 

    Let p = 5 and n = 3.  We use the facts that -1 = 4 (mod 5), 1/2 = 3, -1/2 = 2, 
	1/3 = 2, -1/3 = 3, 1/4 = 4, -1/4 = 1.

	Consider the matrix

        ( 2 3 4 )
    Q = ( 0 2 1 )
        ( 3 3 3 )

    Begin with row 1.  No pivotal columns have been selected yet.  Scan row 1 and
	pick column 1 as the pivotal column because it contains a nonzero element.

    Normalizing column 1 by multiplying by -1/pivot = -1/2 = 2 gives

        ( 4 3 4 )
        ( 0 2 1 )
        ( 1 3 3 )

     Now perform column reduction on column 2 by multiplying the pivotal column 1 
	 by 3 (the column 2 element in the current row) and adding back to row 2.

        ( 4 0 4 )
        ( 0 2 1 )
        ( 1 1 3 )

     Column reduce column 3 by multiplying the pivotal column by 4 and adding back to row 3,

        ( 4 0 0 )
        ( 0 2 1 )
        ( 1 1 2 )

     For row 2, pick column 2 as the pivotal column, normalize it and reduce columns 1, then 3,

        ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )
        ( 0 2 1 ) => ( 0 4 1 ) => ( 0 4 1 ) => ( 0 4 0 )
        ( 1 1 2 )    ( 1 2 2 )    ( 1 2 2 )    ( 1 2 4 )
                 norm.         c.r. 1      c.r. 3

     For row 3, we must pick column 3 as pivotal column because we've used up columns 1 and 2,

        ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )   
        ( 0 4 0 ) => ( 0 4 0 ) => ( 0 4 0 ) => ( 0 4 0 )  
        ( 1 2 4 )    ( 1 2 4 )    ( 1 2 4 )    ( 0 0 4 )  
		         norm.        c.r. 1        c.r. 2

     The nullity is zero, since we were always able to find a pivot in each row.

METHOD

      Modified from ART OF COMPUTER PROGRAMMING, Vol. 2, 2nd ed., Donald E. Knuth, Addison-Wesley.

      We combine operations of normalization of columns,

      (       c       )                         (        C       )
	  (       c       )                         (        C       )
	  (       .       )                         (        C       )
	  ( . . . q . . . ) row  ================>  ( . . . -1 . . . ) row
	  (       c       )                         (        C       )
      (       c       )      column times       (        C       )
	  (       c       )      -1/a modulo p      (        C       )
           pivotCol                                   pivotCol

      and column reduction,

      (        C      b       )                         (       C        B       )
	  (        C      b       )                         (       C        B       )
	  (        C      b       )                         (       C        B       )
	  ( . . . -1 . . .e . . . ) row  ================>  ( . . . -1 . . . 0 . . . )
	  (        C      b       )                         (       C        B       )
      (        C      b       )    pivotCol times       (       C        B       )
	  (        C      b       )    e added back to col  (       C        B       )
           pivotCol  col                                       col


      to reduce the matrix to a form in which columns having pivots are zero until
	  the pivotal row.

  	  The column operations don't change the left nullspace of the
	  matrix.

      The matrix rank is the number of pivotal rows since they are linearly 
	  independent.  The nullity is then the number of non-pivotal rows.
 
BUGS

*/

int find_nullity( int ** Q, int n, int p );

