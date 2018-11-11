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

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> /* for rand() function */

#include "Primpoly.h"


/*==============================================================================
|                                    factor                                    |
================================================================================

DESCRIPTION

     Factor n into primes.  Record all the distinct prime factors and how
     many times each occurs.  Return the number of primes - 1.

INPUT
                   
     n      (int, n >= 1)     The integer to be factored.

OUTPUT

     primes (bigint *)   List of distinct prime factors.
     count  (int *)      List of how many times each factor occurred.

     When n = 1, t = 0, and primes[ 0 ] = count[ 0 ] = 1.

RETURNS

     t (int)             Number of prime factors - 1.  
	                     Prime factors and their multiplicites are in locations 0 to t.

EXAMPLE 
                   2
    For n = 156 = 2  * 3 * 13 we have

    k   primes[ k ]   count[ k ]
    ----------------------------
    0        2            2
    1        3            1
    2       13            1

METHOD

    Method described by D.E. Knuth, ART OF COMPUTER PROGRAMMING, vol. 2, 2nd ed.,
	                                                            -----
	in Algorithm A, pgs. 364-365.  The running time is O( max \/ p   , p  )
                                                                  t-1   t
    where p  is the largest prime divisor of n and p    is the next largest.
	       t                                        t-1

    (1)  First divide out all multiples of 2 and 3 and count them.  
	
	(2)  Next, divide n by all integers d >= 5 except multiples of 2 and 3. 

    (3)  Halt either when all prime factors have been divided out (leaving n = 1) 
	     or when the current value of n is prime.  The stopping test 

          (d > | n/d | AND r != 0)
               --   --
         detects when n is prime.

    In the example above, we divided out 2's and 3's first, leaving
    n = 13.  We continued with trial divisor d = 3.  But now the stopping
    test was activated because 5 > | 13/5 | = 2, and remainder = 3 (non-zero).
                                   --    --
    n = 13 itself is the last prime factor of 156.  We return t = 2.  There
    are 2 + 1 = 3 distinct primes.


    If we start with d = 5, and add 2 and 4 in succession, we will run through all
	the integers except multiples of 2 and 3.  To see this, observe the pattern:

    Integers:     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
	Mult. of 2:      x     x     x     x     x     x     x     x
	Mult. of 3:         x        x        x        x        x
	d:                        x     x           x     x          x
	Separation:                  2       4         2       4

    The sequence of divisors d, includes all the primes, so is safe to use for
	factorization testing.

    Theorem.  Suppose no divisor d in the sequence divides n.  Suppose at some point,
	q < d with r != 0.  Then n must be prime.

    Proof.  We begin with an integer n which has all powers of 2 and 3 removed.
    Assume, to the contrary, that n is composite:  n = p ... p
	      t                                             1     t
    n >= p    since p  is the smallest prime factor.
	      1          1

	      2                                      
    n >= p    since n is composite, so has at least 2 prime factors.
	      1
		        2
	  >= (d + 1)   since p  > d implies p >= (d + 1).  p > d because we assumed
                          1              1              1
      that no d in the sequence divided n, so d couldn't be any of the prime divisors
	  p
	   i
         2            2               2
	  = d + 2d + 1 = d + d + d + 1 > d + d
         2
      > d + n mod d   since n mod d < d
                                                                              2
      > | n / d | d + n mod d  because our test said q < d, so | n / d | d < d
	    --     --                                              --     --

      = n.  So we get the contradiction n > n.  Thus n must be prime. 
	                                               ---
    Note that this is sharper than the bound d > \/ n

    Theorem.  Conversely, if n is a prime, no d will divide it, so at some point, 
	d will be large enough that q = | n / d | < d.  r != 0 since n is prime.
	                                --     --

    Theorem.  The factoring algorithm works.
	
	Proof.  If n == 1 we exit immediately.  If n is composite, we divide out all powers
	of 2 and 3 (if any).  Since d runs through all possible prime divisors, we divide
	these out also.  Composite trial d causes no problem;  prime factors of d are divided
	out of n before we try to divide by d, so such a d cannot divide n.

    We end in one of two ways (1) All divisors of n have been divided out in which case n = 1.
	(2) n is a prime, so the stopping test is activiated and n != 1 is recorded as a prime
	divisor.


BUGS

    Can be slow when n is a prime.  We could do a probabilistic test for
    the primality of n at the statement, "n_is_prime = (r != 0 && q < d) ;"
    which would speed up the test.  
    
    Or we could implement the Pollard rho algorithm instead.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int factor( bigint n, bigint * primes, int * count );


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

bigint EulerPhi( bigint n );


/*==============================================================================
|                             is_probably_prime                                |
================================================================================

DESCRIPTION

     Test whether the number n is probably prime.  If n is composite
     we are correct always.  If n is prime, we will be wrong no more
     than about 1/4 of the time on average, probably less for
     any x and n.

INPUT
                   
     n      (int, n >= 0)    Number to test for primality.
     x      (int, 1 < x < n for n > 6) A random integer.

RETURNS


     YES    If n is probably prime.
     NO     If n is definitely not prime or n < 0,
            or x is out of range.

EXAMPLE 
 
METHOD

     Miller-Rabin probabilistic primality test, described by Knuth.
     If n = 1 + 2^k q is prime, and x^q mod n != 1 the sequence, 
     { x^q mod n, x^(2q) mod n, ..., x^(2^k q) mod n} 
     will end with 1 and the element in the sequence just before 
     1 first appears = n-1, since y^2 = 1 (mod p) satisfies 
     y = +1 or y = -1 only. The probability the algorithm fails is 
     bounded above by about 1/4 for all n.

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int is_probably_prime( int n, int x );


/*==============================================================================
|                           is_almost_surely_prime                             |
================================================================================

DESCRIPTION

     Test whether the number n is almost surely prime.  If n is
     composite, we always return NO.  If n is prime, the 
     probability of returning YES successfully is

                 NUM_PRIME_TEST_TRIALS
        1 - (1/4)

INPUT
                   
     n      (int, n >= 0)

RETURNS


     YES    If n is probably prime with a very high degree of
            probability.
     NO     If n is definitely not prime.
	 -1     If inputs are out of range.

EXAMPLE
  
METHOD

    Use the Miller-Rabin probabilitic primality test for lots of
    random integers x.  If the test shows n is composite for any 
    given x, is is non-prime for sure.  
    
    If it passes the the primality test for several random values
    of x, the probability is it prime is approximately
    
         NUM_PRIME_TEST_TRIALS
    1 - 2

    assuming the random number generator is really random.

BUGS

    The system pseudorandom number generator needs to be a good one
    (i.e. passes the spectral test and other statistical tests).

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int is_almost_surely_prime( int n );

