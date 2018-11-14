#pragma once

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

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include "Primpoly.h"

/*==============================================================================
|                                   eval_poly                                  |
================================================================================

DESCRIPTION

     Evaluate the polynomial f( x ) with modulo p arithmetic.

INPUT

     f (int *)      nth degree mod p polynomial
         
                              n         n-1     
                    f( x ) = x  +  a   x  + ... + a    0 <= a  < p
                                    n-1            0         i          
     x (int)        0 <= x < p

     n (int)        n >= 1

     p (int)         

RETURNS

     f( x )  (int)

EXAMPLE 
                                  4
     Let n = 4, p = 5 and f(x) = x  + 3x + 3.  Then f(x) = 
     (((x + 0)x + 0)x + 3)x + 3.  f(2) = (((2 + 0)2 + 0)2 + 3) =
     (8 + 3)2 + 3 = 1 + 2 + 3 (mod 5) = 0.

METHOD

     By Horner's rule, f(x) = ( ... ( (x + a   )x + ... )x + a .
                                            n-1               0
     We evaluate recursively, f := f * x + a (mod p), starting
                                            i 
     with f = 1 and i = n-1.

BUGS

     None.
*/

int
    eval_poly( int * f, int x, int n, int p );


/*==============================================================================
|                                     linear_factor                            |
================================================================================

DESCRIPTION

     Check if the polynomial f(x) has any linear factors.

INPUT

    f (int *)  nth degree monic mod p polynomial f(x). 

    n (int)    Its degree.

    p (int)    Modulus for coefficient arithmetic.

RETURNS

    YES    if f( a ) = 0 (mod p) for a = 1, 2, ... p-1.
    NO     otherwise

    i.e. check if f(x) contains a linear factor (x - a).  We don't need to test
    for the root a = 0 because f(x) was chosen in main to have a non-zero 
    constant term, hence no zero root.

EXAMPLE 
                                 4      
    Let n = 4, p = 5 and f(x) = x  + 3x + 3.  Then f(1) = 2 (mod 5), but 
    f(2) = 0 (mod 5), so we exit immediately with a yes.

METHOD

    Evaluate f(x) at x = 1, ..., p-1 by Horner's rule.  Return instantly the 
    moment f(x) evaluates to 0.

BUGS

    None.
*/

int  linear_factor( int * f, int n, int p );
/*==============================================================================
|                                 is_integer                                   |
================================================================================

DESCRIPTION

     Test if a polynomial is a constant.

INPUT

    t (int *)  mod p polynomial t(x). 
    n (int)    Its degree.

RETURNS

    YES    if t(x) is a constant polynomial.
    NO     otherwise

EXAMPLE 
                               2
    Let n = 3.  Then t(x) = 2 x  is not constant but t(x) = 2 is.

METHOD

    A constant polynomial is zero in its first through n th degree
    terms.  Return immediately with NO if any such term is non-zero.

BUGS

    None.
*/

int is_integer( int * t, int n );


/*==============================================================================
|                               construct_power_table                          |
================================================================================

DESCRIPTION

     Construct a table of powers of x:

      n                     2n-2
     x  (mod f(x), p)  ... x    (mod f(x), p)

INPUT

    f (int *)   Coefficients of f(x), a monic polynomial of degree n.
    n (int, -infinity < n < infinity)
    p (int, p > 0)

RETURNS

    power_table (int *)  power_table[i][j] is the coefficient of 
     j       n+i
    x   in  x   (mod f(x), p) where 0 <= i <= n-2 and 0 <= j <= n-1. 

EXAMPLE 
                                  4     2                     4
     Let n = 4, p = 5 and f(x) = x  +  x  +  2x  +  3.  Then x  = 

         2                  2
     -( x  +  2x  + 3) = 4 x  + 3 x + 2 (mod f(x), 5), and we get

      4                    2
     x  (mod f(x), 5) = 4 x  + 3 x + 2 = power_table[0].

      5                       2                 3      2
     x  (mod f(x), 5) = x( 4 x  + 3 x + 2) = 4 x  + 3 x  + 2x 
                      = power_table[1].

      6                       3      2           4      3      2
     x  (mod f(x), 5) = x( 4 x  + 3 x + 2 x) = 4x  + 3 x  + 2 x 

                              2                 3      2
                      = 4 ( 4x  + 3 x + 2) + 3 x  + 2 x  =

                           3     2
                      = 3 x + 3 x + 2 x + 3 = power_table[2].

                                    j
     power_table[i][j]:       | 0  1  2  3
                           ---+-------------
                            0 | 2  3  4  0 
                        i   1 | 0  2  3  4 
                            2 | 3  2  3  3 

METHOD
                              n-1
     Beginning with t( x ) = x,    compute the next power of x from the last
                                                         n
     one by multiplying by x.  If necessary, substitute x  =
             n-1
     -( a   x    + ... + a ) to reduce the degree.  This formula comes from
         n-1              0
                               n         n-1
     the observation f( x ) = x   + a   x    + ... + a    = 0 (mod f(x), p).
                                     n-1              0
BUGS

     None.
*/

void construct_power_table( int power_table[][ MAXDEGPOLY ], int * f,
                           int n, int p );

/*==============================================================================
|                                 autoconvolve                                  |
================================================================================

DESCRIPTION

     Compute a convolution-like sum for use in function coeff_of_square.

INPUT
                                             n-1
    t (int *)   Coefficients of t(x) = t    x    + ... + t x  + t
                                        n-1               1      0
    k (int), 0 <= k <= 2n - 2)

    lower (int, 0 <= lower <= n-1) 

    uppper (int, 0 <= upper <= n-1) 

    p (int, p > 0)

RETURNS

     upper
     ---
     \    t  t       But define the sum to be 0 when lower > upper to catch
     /     i  k-i    the special cases that come up in function coeff_of_square.
     ---
     i=lower

EXAMPLE 
                       3      2      
     Suppose t(x) = 4 x  +  x  +  3 x  +  3, lower = 1, upper = 3, n = 3,

     and p = 5.  For k = 3, auto_convolve = t[ 1 ] t[ 2 ] + t[ 2 ] t[ 1 ] +

     t[ 3 ] t[ 0 ] = 3 * 1 + 1 * 3 + 4 * 3 = 18 mod 5 = 3.  For lower = 0,

     upper = -1, or for lower = 3 and upper = 2, auto_convolve = 0, no matter what

     k is.

METHOD

    A "for" loop in the C language is not executed when its lower limit 

    exceeds its upper limit, leaving sum = 0.

BUGS

     None.
*/

int auto_convolve( int * t, int k, int lower, int upper, int p );

/*==============================================================================
|                                 convolve                                     |
================================================================================

DESCRIPTION

     Compute a convolution-like sum.

INPUT
                                             n-1
    s (int *)   Coefficients of s(x) = s    x    + ... + s x  + s
                                        n-1               1      0  
										     n-1
    t (int *)   Coefficients of t(x) = t    x    + ... + t x  + t
                                        n-1               1      0

    k (int), 0 <= k <= 2n - 2)

    lower (int, 0 <= lower <= n-1) 

    uppper (int, 0 <= upper <= n-1) 

    p (int, p > 0)

RETURNS

     upper
     ---
     \    s  t       But define the sum to be 0 when lower > upper to catch
     /     i  k-i    the special cases
     ---
     i=lower

EXAMPLE 
                       3      2      
     Suppose s(x) = 4 x  +  x  +  3 x  +  3, 

	                    3      2      
     Suppose t(x) = 4 x  +  x  +  3 x  +  3, 

	   
	 lower = 1, upper = 3, n = 3,

     and p = 5.  For k = 3, convolve = t[ 1 ] t[ 2 ] + t[ 2 ] t[ 1 ] +

     t[ 3 ] t[ 0 ] = 3 * 1 + 1 * 3 + 4 * 3 = 18 mod 5 = 3.  For lower = 0,

     upper = -1, or for lower = 3 and upper = 2, convolve = 0, no matter what

     k is.

METHOD

    A "for" loop in the C language is not executed when its lower limit 

    exceeds its upper limit, leaving sum = 0.

BUGS

     None.
*/

int  convolve( int * s, int * t, int k, int lower, int upper, int p );

/*==============================================================================
|                               coeff_of_square                                |
================================================================================

DESCRIPTION
                                    th                2
     Return the coefficient of the k   power of x in t( x )  modulo p.
 
INPUT

    t (int *)  Coefficients of t(x), of degree <= n-1.

    k (int, 0 <= k <= 2n-2)

    n (int, -infinity < n < infinity)

    p (int, p > 0)

RETURNS
                        th                2
    coefficient of the k   power of x in t(x) mod p.

EXAMPLE 
                                    3     2           2
    Let n = 4, p = 5, and t(x) = 4 x  +  x  + 4.  t(x) = 

             0                   1                         2
    (4 * 4) x  +  (2 * 1 * 4) x  +  (2 * 1 * 4 + 1 * 1) x  +

                             3                        4
    (2 * 1 * 1 + 2 * 4 * 4) x  + (2 * 4 * 1 + 1 * 1) x  +

                 5            6     
    (2 * 4 * 1) x  + (4 * 4) x  =  

     6      5     4     3     2
    x  + 3 x + 4 x + 4 x + 4 x  + 3 x + 1, all modulo 5.

            k        |  0  1  2  3  4  5  6
     ----------------+---------------------
     coeff_of_square |  1  3  4  4  4  3  1

METHOD
                                                         2
    The formulas were gotten by writing out the product t (x) explicitly.

    The sum is 0 in two cases:  

	    (1) when k = 0 and the limits of summation are 0 to -1 

        (2) k = 2n - 2, when the limits of summation are n to n-1.

    To derive the formulas, let 

                      n-1           
    Let t (x) = t    x     +  ... + t x + t
                 n-1                 1     0

    Look at the formulas in coeff_of_product for each power of x,
	replacing s with t, and observe that half of the terms are
	duplicates, so we can save computation time.

    Inspection yields the formulas,


    for 0 <= k <= n-1, even k,

     k/2-1
      ---             2
   2  \   t  t     + t
	  /    i  k-i     k/2
 	  ---
	  i=0

    for 0 <= k <= n-1, odd k,

      (k-1)/2
      ---  
    2 \   t  t    
	  /    i  k-i
	  ---
	  i=0


    and for n <= k <= 2n-2, even k,

      n-1
      ---            2
   2  \   t  t    + t
	  /    i  k-i    k/2
	  ---
	  i=k/2+1

      and for n <= k <= 2n-2, odd k,

      n-1
      ---  
   2  \   t  t    
	  /    i  k-i   
	  ---
	  i=(k+1)/2



BUGS

     None.
*/

int coeff_of_square( int * t, int k, int n, int p );

/*==============================================================================
|                               coeff_of_product                               |
================================================================================

DESCRIPTION
                                    th
     Return the coefficient of the k   power of x in s( x ) t( x )  modulo p.
 
INPUT

    t (int *)  Coefficients of t(x), of degree <= n-1.

    k (int, 0 <= k <= 2n-2)

    n (int, -infinity < n < infinity)

    p (int, p > 0)

RETURNS
                        th
    coefficient of the k   power of x in s(x) t(x) mod p.

EXAMPLE  
                              3     2                  2
  Let n = 4, p = 5, t(x) = 4 x  +  x  + 4, s( x ) = 3 x  + x + 2

  We'll do the case k=3,

  t3 s0 + t2 s1 + t1 s2 + t0 s3 = 4 * 2 + 1 * 1 + 0 * 3 + 4 * 0 = 9 = 4 (mod 5).


            k         |  0  1  2  3  4  5  6
     -----------------+---------------------
     coeff_of_product |  3  4  4  4  2  2  0

METHOD
                                                         
    The formulas were gotten by writing out the product s(x) t (x) explicitly.

    The sum is 0 in two cases:  

	    (1) when k = 0 and the limits of summation are 0 to -1 

        (2) k = 2n - 2, when the limits of summation are n to n-1.


    To derive the formulas, let 

                      n-1           
    Let s (x) = s    x     +  ... + s x + s  and
                 n-1                 1     0

                      n-1           
        t (x) = t    x     +  ... + t x + t 
                 n-1                 1     0

    and multiply out the terms, collecting like powers of x:


    Power of x     Coefficient
	==========================
     2n-2
    x              s    t
                    n-1  n-1

     2n-3
    x              s    t    +  s    t
                    n-2  n-1     n-1  n-2     

     2n-4
    x              s    t    +  s    t    +  s    t
                    n-3  n-1     n-2  n-2     n-3  n-1

     2n-5
    x              s    t    +  s    t    +  s    t    +  s    t
                    n-4  n-1     n-3  n-2     n-2  n-3     n-1  n-4

     . . .

     n
    x              s  t    +  s  t    + ...  +  s    t
                    1  n-1     2  n-2            n-1  1

     n-1
    x              s  t    +  s  t    + ...  +  s    t
                    0  n-1     1  n-2            n-1  0

    . . .

     3
    x              s  t  +  s  t  +  s  t  +  s  t
                    0  3     1  2     2  1     3  0

     2
    x              s  t  +  s  t  +  s  t 
                    0  2     1  1     2  0

     
    x              s  t  +  s  t 
                    0  1     1  0 

    1              s  t
                    0  0


    Inspection yields the formulas,


    for 0 <= k <= n-1,

      k
     ---
     \   s  t
	 /    i  k-i
	 ---
	 i=0


    and for n <= k <= 2n-2,

     n-1
     ---
     \   s  t
	 /    i  k-i
	 ---
	 i=k-n+1

BUGS

     None.

*/

int coeff_of_product( int * s, int * t, int k, int n, int p );

/*==============================================================================
|                                  square                                      |
================================================================================

DESCRIPTION
              2
     Compute t ( x ) (mod f(x), p)

     Uses a precomputed table of powers of x.

INPUT

    t (int *)              Coefficients of t (x), of degree <= n-1.

    power_table (int **)   x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, -infinity < n < infinity)  Degree of f(x).

    p (int, p > 0)         Mod p coefficient arithmetic.

OUTPUT
                                             2
    t (int *)              Overwritten with t (x) (mod f(x), p)

EXAMPLE 
                                    3     2        
    Let n = 4, p = 5, and t(x) = 4 x  +  x  + 4.  Let f(x) = 

     4    2                   2       6      5      4      3     2
    x  + x  + 2 x + 3.  Then t (x) = x  + 3 x  + 4 x  + 4 x + 4 x + 3 x

           2                            3     2
    + 1.  t (x) (mod f(x), p) = 1 * (3 x + 3 x + 2 x + 3) +

            3      2                 2                3     2
    3 * (4 x  + 3 x + 2 x) + 4 * (4 x + 3 x + 2) + 4 x + 4 x + 3 x + 1

         3     2
    = 4 x + 2 x + 3 x + 2.


METHOD
         2            2n-2              n         n-1
    Let t (x) = t    x     +  ... + t  x  +  t   x   +  ... + t .
                 2n-2                n        n-1              0

    Compute the coefficients t  using the function coeff_of_square.
                              k

                                2
    The next step is to reduce t (x) modulo f(x).  To do so, replace

                           k                                      k
    each non-zero term t  x,  n <= k <= 2n-2, by the term t * [ x   mod f(x), p)]
                        k                                  k

    which we get from the array power_table.

BUGS

     None.
*/

void square( int * t, int power_table[][ MAXDEGPOLY ], int n, int p );

/*==============================================================================
|                                  product                                     |
================================================================================

DESCRIPTION

     Compute s( x ) t( x ) (mod f(x), p)

     Uses a precomputed table of powers of x.

INPUT

    s (int *)              Coefficients of s(x), of degree <= n-1.

    t (int *)              Coefficients of t(x), of degree <= n-1.

    power_table (int **)    x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, -infinity < n < infinity)  Degree of f(x).

    p (int, p > 0)         Mod p coefficient arithmetic.

OUTPUT

    s (int *)              Overwritten with s( x ) t( x ) (mod f(x), p)

EXAMPLE 
                                     3    2                 2
    Let n = 4 and p = 5, t( x ) = 4 x  + x + 4, s( x ) = 3 x + x + 2

                            5      4     3     2
	Then s( x ) t( x ) = 2 x  + 2 x + 4 x + 4 x + 4 x + 3, modulo 5, 

                                         4   2    
    and after reduction modulo f( x ) = x + x + 2 x + 3, using the power

	                   4      2               5      3      2
	table entries for x  = 4 x + 3 x + 2 and x  = 4 x  + 3 x + 2 x, we get
	
	                                   3      2
    s( x ) t( x ) (mod f( x ), p) = 2 x  + 3 x  + 4 x + 2	  
		

METHOD

    Compute the coefficients using the function coeff_of_product.
                                 
    The next step is to reduce s(x) t(x) modulo f(x) and p.  To do so, replace

                           k                                      k
    each non-zero term t  x,  n <= k <= 2n-2, by the term t * [ x   mod f(x), p)]
                        k                                  k

    which we get from the array power_table.

BUGS

     None.
*/

void product( int  * s, int * t, int power_table[][ MAXDEGPOLY ], int n, int p );


/*==============================================================================
|                                  times_x                                     |
================================================================================

DESCRIPTION

     Compute x t(x) (mod f(x), p)

     Uses a precomputed table of powers of x.

INPUT
                                            2
    t (int *)              Coefficients of t (x), of degree <= n-1.

    power_table (int **)    x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, -infinity < n < infinity)  Degree of f(x).

    p (int, p > 0)         Mod p coefficient arithmetic.

OUTPUT

    t (int *)              Overwritten with x t(x) (mod f(x), p)

EXAMPLE 
                                    3       2        
    Let n = 4, p = 5, and t(x) = 2 x  +  4 x  + 3 x.  Let f(x) = 
     4    2                                  4      3      2
    x  + x  + 2 x + 3.  Then x t (x) = 2 x  + 4 x  + 3 x  and
                                     2                3     2
    x t(x) (mod f(x), p) = 2 * (4 x + 3 x + 2) + 4 x + 3 x  =
       3    2 
    4 x  + x + x + 4.
         3     2
    = 4 x + 2 x + 3 x + 2.

METHOD
                          n-1
    Multiply t(x) = t    x    + ... + t  by shifting the coefficients
                     n-1               0
                             n
    to the left.  If an     x   term appears, eliminate it by 

    substitution using power_table.

BUGS

     None.
*/

void times_x( int * t, int power_table[][ MAXDEGPOLY ], int n, int p );

/*==============================================================================
|                                  x_to_power                                  |
================================================================================

DESCRIPTION
                     m
     Compute g(x) = x (mod f(x), p).

INPUT

    m (int)       1 <= m <= r.  The exponent.

    power_table (int **)    x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, n >= 1)     Degree of monic polynomial f(x).

    p (int, p >= 2)     Modulo p coefficient arithmetic.

OUTPUT

    g (int *)     Polynomial of degree <= n-1.

EXAMPLE 
                              4   2 
    Let n = 4, p = 5, f(x) = x + x + 2 x + 3, and m = 156.

    156 = 0  . . . 0  1  0  0  1  1  1  0  0 (binary representation)
          |<- ignore ->| S  S SX SX SX  S  S (exponentiation rule,
		                                      S = square, X = multiply by x)
     m
    x (mod f(x), p) =

         2     2
    S   x  =  x

         4       2
    S   x  =  4 x  + 3 x + 2

         8       3      2
    S   x  =  4 x  + 4 x + 1

         9       3     2
    X   x  =  4 x  +  x + 3 x + 3

         18       2  
    S   x  =  2 x  +  x + 2

         19      3     2
    X   x  =  2 x  +  x  + 2 x

         38      3       2
    S   x  =  2 x  +  4 x  +  3 x

         39      3       2
    X   x  =  4 x  +  2 x  +  x + 4

         78      3       2
    S   x  =  4 x  +  2 x  +  3 x + 2

         156 
    S   x    = 3

METHOD

    Exponentiation by repeated squaring, using precomputed table of
	powers.  See ART OF COMPUTER PROGRAMMING, vol. 2, 2nd id,
	D. E. Knuth,  pgs 441-443.

     n         2n-2
    x,  ... , x    (mod f(x), p)

                    m
    to find g(x) = x   (mod f(x), p), expand m into binary,

           k        k-1
    m = a 2  + a   2    + . . . + a 2 + a
         k      k-1                1     0

                            m
	where a = 1, and split x   apart into
           k

            k      k
           2      2  a             2 a    a
     m                k-1             1    0
    x  =  x     x           . . .  x     x


    Then to raise x to the mth power, do


    t( x ) = x

    return if m = 1


    for i = k-1 downto 0 do

                   2
        t(x) = t(x)  (mod f(x), p)       // Square each time.

        if a = 1 then
		    i

            t(x) = x t(x) (mod f(x), p)  // Times x only if current bit is 1
        endif

    endfor
                                                        k
                                                       2
    The initial t(x) = x gets squared k times to give x  .  If a  = 1 for
	                                                            i
    0 <= i <= k-1, we multiply by x which then gets squared i times more

              i
             2
    to give x .  On a binary computer, we use bit shifting and masking to

    identify the k bits  { a    . . .  a  } to the right of the leading 1
	                        k-1         0

    bit.  There are log ( m ) - 1 squarings and (number of 1 bits) - 1
		                   2
	multiplies.  

BUGS

     None.
*/

void x_to_power( bigint m, int * g, int power_table[][ MAXDEGPOLY ], int n, int p );

