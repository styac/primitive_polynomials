/*==============================================================================
|
|  NAME
|
|     ppArith.cpp
|
|  DESCRIPTION
|
|     Miscellaneous integer multiple precision math routines.
|
|     User manual and technical documentation are described in detail in my web page at
|     http://seanerikoconnor.freeservers.com/Mathematics/AbstractAlgebra/PrimitivePolynomials/overview.html
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
|     The author's address is seanerikoconnor!AT!gmail!DOT!com
|     with the !DOT! replaced by . and the !AT! replaced by @
|
==============================================================================*/

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <new>          // set_new_handler()
#include <cmath>        // Basic math functions e.g. sqrt()
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;

/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.h"		  // Global functions.
#include "ppArith.h"		  // Basic arithmetic functions.
#include "ppBigInt.h"         // Arbitrary precision integer arithmetic.
#include "ppOperationCount.h" // OperationCount collection for factoring and poly finding.
#include "ppFactor.h"         // Prime factorization and Euler Phi.
#include "ppPolynomial.h"	  // Polynomial operations and mod polynomial operations.
#include "ppParser.h"	      // Parsing of polynomials and I/O services.
#include "ppUnitTest.h"       // Complete unit test.

/*=============================================================================
 | 
 | NAME
 | 
 |     ModP::operator()
 | 
 | DESCRIPTION
 | 
 |     Computes k = n mod p where 0 <= k < p for both positive and negative 
 |     arguments n for p >= 1.
 |     Takes a surprising proportion of the total compute time according to the 
 |     profiler, so worth optimizing.
 | 
 | EXAMPLE
 | 
 |     The C language gives -5 % 3 = - ( -(-5) mod 3 ) = -( 5 mod 3 ) = -2.
 |     The result is nonzero, so we add p=3 to give 1.
 | 
 |     C computes -3 % 3 = - ( -(-3) mod 3  ) = 0, which we leave unchanged.
 | 
 | METHOD
 | 
 |      For n >= 0, the C language defines r = n mod p by the equation
 | 
 |          n = kp + r    0 <= r < p
 | 
 |      but when n < 0, C returns the quantity
 | 
 |          r = - ( (-n) mod p )
 | 
 |      To put the result into the correct range 0 to p-1, add p to r if
 |      r is non-zero.
 | 
 |      By the way, dear old FORTRAN's MOD function does the same thing.
 | 
 +============================================================================*/

template <typename UIntType, typename SIntType>
inline UIntType ModP<UIntType,SIntType>::operator()( SIntType n )
{
    if (p_ <= 0)
    {
        ostringstream os ;
        os << "ModP::operator() "
        << "Out of range:  p = " << p_ << " n = " << n
        << " at " << __FILE__ << ": line " << __LINE__ ;
        throw ArithModPException( os.str() ) ;
    }
    
    if (n >= 0)
    {
        // Avoid integer division in this special case.
        if (p_ == 2)
            return n - ((n >> 1) << 1) ;
        else
            return n % p_ ;
    }
    // Both types must be signed if we want n % p to compute correctly for n < 0.
     else
        return( (n % static_cast<SIntType>(p_)) + p_ ) ;
}

/*=============================================================================
|
| NAME
|
|     PowerMod::operator()
|
| DESCRIPTION
|
|               n 
|     Compute  a  (modulo p)  for generic integer types.
|                                                           0
|     where a >= 0, p >= 2, n >= 0.  Other cases including 0
|     will throw an ArithModPException.
|
|     We have two versions:  one is generic for any integer type, and
|     the other is specialized for ppuint.
|
| EXAMPLE
|
|     BigInt a = 2 ;
|     BigInt n = 3 ;
|     BigInt p = 7 ;
|     try 
|     {
|         PowerMod<BigInt> powermod( p ) ;
|         BigInt pwr = powermod( a, n ) ;
|     } 
|     catch( ArithModPException e ) 
|     { 
|         cout << "should have gotten pwr = 1\n" ;
|     }
|
| METHOD
|
|     Multiplication by repeated squaring.
|
+============================================================================*/

// Generic.
template <typename IntType>
IntType PowerMod<IntType>::operator()( const IntType & a, const IntType & n )
{
    IntType a1 = a ;

    //  Out of range conditions.
    if (a  <  static_cast<IntType>( 0u ) || 
        n  <  static_cast<IntType>( 0u ) || 
        p_ <= static_cast<IntType>( 1u ) || 
       (a  == static_cast<IntType>( 0u ) && n == static_cast<IntType>( 0u )))
    {
        ostringstream os ;
        os << "PowerMod::operator() "
           << "out of range a = " << a << " n = " << n  << " p_ = " << p_
           << " at " << __FILE__ << ": line " << __LINE__ ;
        throw ArithModPException( os.str() ) ;
    }

    //  Quick return for 0 ^ n, a^0 and a^1.
    if (a == static_cast<IntType>( 0u ))
        return static_cast<IntType>( 0u ) ;

    if (n == static_cast<IntType>( 0u ))
        return static_cast<IntType>( 1u ) ;

    if (n == static_cast<IntType>( 1u ))
        return a % static_cast<IntType>( p_ ) ;

    int bitNum = n.maxBitNumber() ; // Index to highest bit.

    #ifdef DEBUG_PP_ARITH
    cout << "initial max bitNum = " << bitNum << endl ;
    cout << "a = " << a << endl ;
    #endif
    
    // Find the index of the leading 1 bit.
    while (!n.testBit( bitNum ))
        --bitNum ;

    #ifdef DEBUG_PP_ARITH
    cout << "after skipping leading 0 bits, bitNum = " << bitNum << endl ;
    #endif

    if (bitNum == -1)
    {
        ostringstream os ;
        os << "PowerMod::operator() " << "bitNum == -1 internal error in PowerMod"
           << " at " << __FILE__ << ": line " << __LINE__ ;
        throw ArithModPException( os.str() ) ;
    }

    #ifdef DEBUG_PP_ARITH
    cout << "\nAfter skipping zero bits, bitNum = " << bitNum << endl ;
    #endif

    //  Exponentiation by repeated squaring.  Discard the leading 1 bit.
    //  Thereafter, square for every 0 bit;  square and multiply by a for
    //  every 1 bit.
    while ( --bitNum >= 0 )
    {
        a1 = (a1 * a1) % static_cast<IntType>( p_ ) ; // Square mod p.

        if (n.testBit( bitNum ))
            a1 = (a1 * a) % static_cast<IntType>( p_ ) ; // Times a mod p.

        #ifdef DEBUG_PP_ARITH
        cout << "S " ;
        if (n.testBit( bitNum ))
            cout << "X " ;
        cout << "Bit num = " << bitNum << " a1 = " << a1 << endl ;
        #endif
    }

    #ifdef DEBUG_PP_ARITH
        cout << "Out of the loop bitNum = " << bitNum << " a1 = " << a1 << endl ;
    #endif

    return a1 ;
}



/*=============================================================================
 | 
 | NAME
 | 
 |     addMod()
 | 
 | DESCRIPTION
 | 
 |     Computes (x + y) mod p for generic integer types, up to the maximum
 |     unsigned type.  We extend the range to the limit by testing and compensating
 |     for carries internally.
 |
 |     Example:
 |         a           = 4294967290
 |         n           = 4294967294
 |         (a + b) % n = 4294967286
 | 
 +============================================================================*/

template <typename IntType>
IntType addMod( IntType a, IntType b, IntType n )
{
    #ifdef DEBUG_PP_ARITH
    cout << "addMod" << endl ;
    cout << "    sizeof IntType = " << 8 * sizeof( IntType ) << " bits" << endl ;
    cout << "    a = " << a << " b = " << b << " n = " << n << endl ;
    #endif

    // Make sure.
    a %= n ;
    b %= n ;

    // Add with carry.  Carry bit thrown away by the language.
    IntType c = a + b ;

    #ifdef DEBUG_PP_ARITH
    cout << "    a mod n = " << a << " b mod n = " << b << endl ;
    cout << "    c = a + b (discarding carry bit) = " << c << endl ;
    #endif

    // Test for a carry.  Most computer languages don't give us access to
    // the carry bit, so we must infer whether a carry occurred during addition.
    // Logically, 
    //     (no carry for a+b)  =>  a+b <= a && a+b <= b
    // The contrapositive is
    //      a+b < a || a+b < b => carry
    if (c < a || c < b)
    {
        // Lemma 1.  If a carry occured, n < c < 2n.
        // Pf.
        //                                       m
        //     When a carry occurs, c = a + b > 2  > n.  But 0 <= a, b < n.  So n < c and a + b < n + n = 2n.
        //
        // Lemma 2.  If a carry occured, we can simply compute c - n as usual, discarding the carry.
        // Pf.
        //     Since a carry occurred, we an write
        //                  m
        //     c = a + b = 2  + c'
        //
        //     Let's assume the computer hardware does subtraction in two's complement.
        //     Now use two's complement arithmetic on c' - n and see the results.
        //
        //     c' + TwosComplement( n ) <discarding carry bit> = 
        //
        //     c' + (~n + 1) <discarding carry bit> =
        //             m           m
        //     (c' + (2 - n)) mod 2  = 
        //           m   m     m           m          m           m
        //     (c - 2 + 2  + (2 - n)) mod 2  = (c + (2 - n)) mod 2 =
        //
        //     c + TwosComplement( n ) <discarding carry bit> =
        //
        //     c - n done the usual way in computer hardware.
        //
        // Th. If a carry occurs, c mod n = c - n.
        // Pf. See Lemma 1 and 2.
        //
        // e.g.  Let m = 4 bits and
        //
        //       a  =   1 1 0 1 = 13
        //       b  =   1 1 0 1 = 13
        //       n  =   1 1 1 0 = 14
        //
        //       c  = 1 1 0 1 0
        //       c' =   1 0 1 0
        //      ~n  =   0 0 0 1 + 1 = 
        //              0 0 1 0
        //       c' -  n = c' + ~n =
        //              1 0 1 0
        //           +  0 0 1 0
        //        =     1 1 0 0 = 12 = 26 mod 14
        
        c -= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    Carry!" << " c < a = " << (c < a ) << "|| c < b = " << (c < b) << endl ;
        #endif
    }
    else
    {
        c %= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    No carry" << endl ;
        #endif
    }

    return c ;
}



/*=============================================================================
 | 
 | NAME
 | 
 |     timesTwoMod()
 | 
 | DESCRIPTION
 | 
 |     Computes (2 * x) mod p for generic integer types, up to the maximum
 |     unsigned type.  We extend the range to the limit by testing and compensating
 |     for carries internally.
 |
 |     Example:
 |         a           = 10376293541461622782
 |         n           = 18446744073709551610
 |         (2 * a) % n =  2305843009213693954
 | 
 +============================================================================*/

template <typename IntType>
IntType timesTwoMod( IntType a, IntType n )
{
    #ifdef DEBUG_PP_ARITH
    cout << "timesTwoMod" << endl ;
    cout << "    sizeof IntType = " << 8 * sizeof( IntType ) << " bits" << endl ;
    cout << "    a = " << a << " n = " << n << endl ;
    #endif

    // Make sure.
    a %= n ;

    #ifdef DEBUG_PP_ARITH
    cout << "    a mod n = " << a << endl ;
    #endif

    // High bit mask.
    IntType mask = ((IntType)1 << (8 * sizeof( IntType ) - 1)) ;

    IntType c = (a << 1) ; // Lose the carry bit.

    #ifdef DEBUG_PP_ARITH
    cout << "    mask = " << hex << mask << dec << endl ;
    cout << "    c = 2 a (losing the carry bit) = " << c << endl ;
    #endif

    // High bit is set so (2 a) will have a carry.
    if (mask & a)
    {
        // See notes above for why this works correctly.
        c -= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    Carry!" << endl ;
        #endif
    }
    else
    {
        c %= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    No carry" << endl ;
        #endif
    }

    return c ;
}



/*=============================================================================
 | 
 | NAME
 | 
 |     multiplyMod()
 | 
 | DESCRIPTION
 | 
 |     Computes (x * y) mod p for generic integer types, up to the maximum
 |     unsigned type.  We extend the range this far by testing and compensating
 |     for carries internally.
 |
 |     Multiply a * b mod n 
 |
 |                   m-1         m-2
 |     a b = a (b   2    + b    2    + ... + b  2 + b )
 |               m-1        m-2               1      0
 |
 |     Expanding using Horner's rule,
 |
 |     a b = (0 2 + a b   ) 2 + a b   ) 2 + ... + a b ) 2 + a b
 |                     m-1         m-2               1         0
 |
 |     where b  = 0 or 1
 |            i
 |     At each step call specialized functions to do r = 2 a mod n and r = r + a mod n
 |     which will work even when carries occur.
 |
 |     The process takes O( ln n ) operations.  An example,
 |
 |          a = 4294967290 
 |          b = 4294967290
 |          n = 4294967294
 |         (a * b) % n = 16
 | 
 |     Reference:  "Comments on 'A Computer Algorithm for Calculating the Product
 |     A B Modulo M'" K. R. Sloane, IEEE Transactions on Computers, Vol C-34, No. 3,
 |     March 1985.
 | 
 +============================================================================*/

template<typename IntType>
IntType multiplyMod( const IntType a, const IntType b, const IntType n )
{
    // High bit mask.
    const int numBits = 8 * sizeof( IntType ) ;
    IntType mask = ((IntType)1 << (numBits - 1)) ;

    IntType r = 0 ;

    for (int i = numBits-1 ;  i >= 0 ;  --i, mask >>= 1)
    {
        r = timesTwoMod( r, n ) ;

        if (mask & b)
            r = addMod( r, a, n ) ;
    }

    return r ;
}



/*=============================================================================
 | 
 | NAME
 | 
 |     PowerMod::operator()
 | 
 | DESCRIPTION
 | 
 |     Specialized for ppuint type.
 | 
 +============================================================================*/

template<>
ppuint PowerMod<ppuint>::operator()( const ppuint & a, const ppuint & n )
{
    //  mask = 1000 ... 000.  That is, all bits of mask are zero except for the
    //  most significant bit of the computer word holding its value.
    ppuint mask = (static_cast<ppuint>(1) << (8 * sizeof( ppuint ) - 1)) ;
    int  bit_count = 0 ;  // Number of bits in exponent to the right of the leading bit.
    ppuint n1 = n ;
    ppuint product = a ;

    // Out of range conditions. 
    if (p_ <= 1 || (a == 0 && n1 == 0))
    {
        ostringstream os ;
        os << "PowerMod<ppuint>::operator() "
           << "out of range a = " << a << " n = " << n  << " p_ = " << p_
           << " at " << __FILE__ << ": line " << __LINE__ ;
        throw ArithModPException( os.str() ) ;
    }

    //                    n   0      1
    //  Quick return for 0 , a  and a
    if (a == 0)
        return 0 ;

    if (n == 0)
        return 1 ;

    if (n == 1)
        return a % p_ ;

    #ifdef DEBUG_PP_ARITH
    cout << "a                    = " << a << endl ;
    cout << "n                    = " << n << endl ;
    cout << "p                    = " << p_ << endl ;
    cout << "n1 (before shifting) = " << n1 << endl ;
    #endif

    // Advance the leading bit of the exponent up to the word's left hand boundary.  
    // Count how many bits were to the right of the leading bit.
    while (! (n1 & mask))
    {
        n1 <<= 1 ;
        ++bit_count ;
    }

    bit_count = (8 * sizeof( ppuint )) - bit_count ;

    #ifdef DEBUG_PP_ARITH
    cout << "n1        = " << n1 << endl ;
    cout << "mask      = " << mask << endl ;
    cout << "bit_count = " << bit_count << endl ;
    #endif

    // Exponentiation by repeated squaring.  Discard the leading 1 bit.
    // Thereafter, square for every 0 bit;  square and multiply by x for every 1 bit.
    while ( --bit_count > 0 )
    {
        #ifdef DEBUG_PP_ARITH
        cout << "product (before squaring) = " << product << " n1 = " << n1 << endl ;
        #endif

        // Expose the next bit.
        n1 <<= 1 ;

        // Out of range conditions. 
        if (product > BigInt::getBase() || a > BigInt::getBase())
        {
            // Square modulo p.
            product = multiplyMod( product, product, p_ ) ;

            //  Leading bit is 1: multiply by a modulo p.
            if (n1 & mask)
                product = multiplyMod( a, product, p_ ) ;

            // ostringstream os ;
            // os << "PowerMod<ppuint>::operator() "
            //    << "product before squaring " << product << " or a = " << a << " is > base = " << BigInt::getBase()
            //    << " at " << __FILE__ << ": line " << __LINE__ ;
            // throw ArithModPException( os.str() ) ;
        }
        else
        {
            // Square modulo p.
            product = (product * product) % p_ ;

            //  Leading bit is 1: multiply by a modulo p.
            if (n1 & mask)
                product = (a * product) % p_ ;
        }

        #ifdef DEBUG_PP_ARITH
        cout << "S " ;
        if (n1 & mask)
            cout << "X " ;
        cout << "product = " << product << " n1 = " << n1 << endl ;
        #endif
    }

    return product ;
}



/*=============================================================================
|
| NAME
|
|     IsPrimitiveRoot::operator()
|
| DESCRIPTION
|
|     Returns true if a is a primitive root of prime p, and false otherwise.
|     Throws ArithModPException if a < 1 or p < 2 or p is even.
|     p must be a prime number, but we only test that p is 2 or odd.
|
| EXAMPLE
|
|     ppuint p = 7 ;  ppuint a = 3 ;
|     IsPrimitiveRoot isRoot( p ) ;
|     try 
|     {
|        bool isPrim = isRoot( a ) ;
|     } 
|     catch( ArithModPException e ) 
|     {  
|         cout << "out of range" << endl ; 
|     }
|                                                     1      2      3
|     3 is a primitive root of the prime p = 7 since 3 = 3, 3 = 2, 3 = 6,
|      4      5                                       p-1    6
|     3 = 4, 3 = 5 (mod 7); all not equal to 1 until 3    = 3 = 1 (mod 7).
|
|     So 3 is a primitive root of 7 because it has maximal period.  a = 2
|                                                      3
|	  isn't a primitive root of p = 7 because already 2 = 1 (mod 7).
|
| METHOD
|
|    From number theory, a is a primitive root of the prime p iff
|     (p-1)/q
|    a        != 1 (mod p) for all prime divisors q of (p-1).
|                            (p-1)
|    Don't need to check if a     = 1 (mod p):  Fermat's little
|    theorem guarantees it.
|
+============================================================================*/

bool IsPrimitiveRoot::operator()( ppuint a )
{
    PowerMod<ppuint> powermod( p_ ) ;

    //  a = 0 (mod p);  Zero can't be a primitive root of p.
    if (a == 0)
        return false ;


    //  Error for out of range inputs, including p
    //  being an even number greater than 2.
    if (p_ < 2 || a < 1 || (p_ > 2 && (p_ % 2 == 0)))
    {
        ostringstream os ;
        os << "IsPrimitiveRoot::operator() " << "inputs out of range, p = " << p_
           << " a = " << a
           << " at " << __FILE__ << ": line " << __LINE__ ;
        throw ArithModPException( os.str() ) ;
    }

    //  Special cases:
    //  1 is the only primitive root of 2;  i.e. 1 generates the unit elements
    //  of GF( 2 );  2 is the only primitive root of 3, and 2 and 3 are the only
    //  primitive roots of 5.
    //
    if ( (p_ == 2  &&  a == 1) ||
         (p_ == 3  &&  a == 2) ||
         (p_ == 5  && (a == 2  || a == 3)) ||
         (p_ == 7  && (a == 3  || a == 5)) ||
         (p_ == 11 && (a == 2  || a == 6   || a == 7 || a == 8)) ||
         (p_ == 13 && (a == 2  || a == 6   || a == 7 || a == 11)))
    {
        return true ;
    }

    //  Reduce a down modulo p.
    a = a % p_ ;

    //  a = 0 (mod p);  Zero can't be a primitive root of p.
    if (a == 0)
        return false ;

    //  Factor p-1 into distinct primes.
    Factorization<ppuint> factorization( p_ - 1 ) ;

    //            p-1
    //           a    
    //  Test    ---    != 1 (mod p) for all primes q | (p-1).
    //           q
    //  If so, we have a primitive root of p, otherwise, we exit early.
    //
    for (unsigned int i = 0 ;  i < factorization.num_distinct_factors() ;  ++i)
    {
        if (powermod( a, (p_ - 1) / factorization.prime_factor( i )) == 1)
        {
            return false ;
        }
    }

    return true ;
} 



/*=============================================================================
 | 
 | NAME
 | 
 |     inverse_mod_p
 | 
 | DESCRIPTION
 | 
 |                                                             -1
 |      Find the inverse of u modulo p.  In other words, find u   (mod p)
 |      p >= 2.
 | 
 | EXAMPLE
 |                           -1
 |      For p=7, and u = 2, u   = 4 because 2 * 4 = 8 (mod 7) = 1.
 | 
 | METHOD
 | 
 |     Do extended Euclid's algorithm on u and v to find u1, u2, and u3 such that
 | 
 |         u u1 + v u2 = u3 = gcd( u, v ).
 | 
 |     Now let v = p = prime number, so gcd = u3 = 1.  Then we get
 | 
 |         u u1 + p ? = 1
 | 
 |     or u u1 = 0 (mod p) which makes u (mod p) the unique multiplicative
 |     inverse of u.
 | 
 +============================================================================*/

ppsint InverseModP::operator()( ppsint u )
{
    ModP<ppsint,ppsint> mod( p_ ) ;

    //  Do Euclid's algorithm to find the quantities.
	ppsint t1 = 0 ;
	ppsint t3 = 0 ;
	ppsint q  = 0 ;

    ppsint u1 = 1 ;
    ppsint u3 = u ;
    ppsint v1 = 0 ;
    ppsint v3 = p_ ;

	ppsint inv_v = 0 ;

	while( v3 != 0)
	{
        // Casting to do a floor function.
        q = static_cast<ppsint>(u3 / v3) ;

		t1 = u1 - v1 * q ;
		t3 = u3 - v3 * q ;

		u1 = v1 ;
		u3 = v3 ;

		v1 = t1 ;
		v3 = t3 ;
	}

    inv_v = mod( u1 ) ;

    //                       -1
	// Self check:  does u  u   = 1 (mod p)?
    //
	if ( mod( u * inv_v ) != 1)
	{
        ostringstream os ;
        os << "InverseModP::operator() "
           << "inverse self check failed:  u = " << u << " inv_v = " << inv_v  << " != 1" 
           << " at " << __FILE__ << ": line " << __LINE__ ;
        throw ArithModPException( os.str() ) ;
	}

	return inv_v ;
}


/*=============================================================================
 | 
 | NAME
 | 
 |     const_coeff_test
 | 
 | DESCRIPTION
 | 
 |                   n
 |   Test if a = (-1)  a  (mod p) where a  is the constant coefficient
 |                      0                0
 |   of polynomial f(x) and a is the number from the order_r() test.
 |   Return true if we pass the test, false otherwise. 
 | 
 | EXAMPLE
 |                                  11     3
 |     Let p = 5, n = 11, f( x ) = x  + 2 x + 1, and a = 4.
 |     Thus a  = 1.
 |           0
 |     Then return true since
 | 
 |         11
 |     (-1)  * 1 (mod 5) = -1 (mod 5) = 4 (mod 5).
 | 
 | METHOD
 |                         n
 |     We test if (a - (-1) a ) mod p = 0.
 |                           0
 | 
 +============================================================================*/

bool ArithModP::const_coeff_test( ppsint a, ppsint a0, int n )
{
    ppsint constantCoeff = a0 ;

    ModP<ppuint,ppsint> mod( p_ ) ; // Initialize the functionoid.

    if (n % 2 != 0)
        constantCoeff = -constantCoeff ;    // (-1)^n < 0 for odd n.

    return( (mod( a - constantCoeff ) == 0) ? true : false ) ;
} 



/*=============================================================================
 | 
 | NAME
 | 
 |     const_coeff_is_primitive_root
 | 
 | DESCRIPTION
 | 
 |               n
 |   Test if (-1)  a  (mod p) is a primitive root of the prime p where
 |                  0
 |   a  is the constant term of the polynomial f(x).
 |    0
 | 
 | EXAMPLE
 |                                  11     3
 |     Let p = 7, n = 11, f( x ) = x  + 2 x + 4.  Thus a  = 4.
 |                                                      0
 |     Then return true since
 | 
 |         11
 |     (-1)  * 4 (mod 7) = -4 (mod 7) = 3 (mod 7), and 3 is a
 |                                          1      2      3
 |     primitive root of the prime 7 since 3 = 3, 3 = 2, 3 = 6,
 |      4      5                                       7-1
 |     3 = 4, 3 = 5 (mod 7); all not equal to 1 until 3   = 1 (mod 7).
 | 
 | METHOD
 | 
 |                    n
 |     We test if (-1) a  mod p is a primitive root mod p.
 |                      0
 |
 +============================================================================*/

bool ArithModP::const_coeff_is_primitive_root( ppuint a0, int n )
{
    ppsint constantCoeff = a0 ;

    ModP<ppuint,ppsint> mod( p_ ) ;
    IsPrimitiveRoot isroot( p_ ) ;

    // (-1)^n < 0 for odd n.
    if (n % 2 != 0)
        constantCoeff = -constantCoeff ;

    return isroot( mod( constantCoeff ) ) ;
} 



/*=============================================================================
|
| NAME
|
|     gcd
|
| DESCRIPTION
|
|    Euclid's method for computing greatest common divisor.
|
| METHOD
|
|     Iterate the equation 
|
|        gcd( u, v ) = gcd( v, r )
|   
|     where r = | u / v | = u - q v
|              --     --
|     until v is zero, terminating in gcd( u, 0 ) = u.
|
|     Let's use Mathematica to illustrate the method for large integers,
|
|     gcd[ u_, v_] :=
|         (* Compute GCD using Euclid's method. *)
|         Module[ {u2, v2, r}, u2 = u ; v2 = v ; 
|                  Print[ "Computing GCD( u = ", u, " v = ", v , " )" ] ;
|                  While[ v2 != 0, 
|                         r = Mod[u2, v2]; u2 = v2; v2 = r;
|                         Print[ "r = ", r, " u2 = ", u2, " v2 = ", v2]];
|                         Return[ u2 ]]
|
|     gcd[ 779953197883173551166308319545, 1282866356929526866866376009397 ]
|
|     Computing GCD( u = 779953197883173551166308319545 v = 1282866356929526866866376009397 )
|        u = 779953197883173551166308319545  v = 1282866356929526866866376009397
|        r = 779953197883173551166308319545 u2 = 1282866356929526866866376009397  v2 = 779953197883173551166308319545
|        r = 502913159046353315700067689852 u2 =  779953197883173551166308319545  v2 = 502913159046353315700067689852
|        r = 277040038836820235466240629693 u2 =  502913159046353315700067689852  v2 = 277040038836820235466240629693
|        r = 225873120209533080233827060159 u2 =  277040038836820235466240629693  v2 = 225873120209533080233827060159
|        r =  51166918627287155232413569534 u2 =  225873120209533080233827060159  v2 =  51166918627287155232413569534
|        r =  21205445700384459304172782023 u2 =   51166918627287155232413569534  v2 =  21205445700384459304172782023
|        r =   8756027226518236624068005488 u2 =   21205445700384459304172782023  v2 =   8756027226518236624068005488
|        r =   3693391247347986056036771047 u2 =    8756027226518236624068005488  v2 =   3693391247347986056036771047
|        r =   1369244731822264511994463394 u2 =    3693391247347986056036771047  v2 =   1369244731822264511994463394
|        r =    954901783703457032047844259 u2 =    1369244731822264511994463394  v2 =    954901783703457032047844259
|        r = 414342948118807479946619135 u2 =  954901783703457032047844259 v2 = 414342948118807479946619135
|        r = 126215887465842072154605989 u2 =  414342948118807479946619135 v2 = 126215887465842072154605989
|        r = 35695285721281263482801168 u2 =  126215887465842072154605989 v2 = 35695285721281263482801168
|        r = 19130030301998281706202485 u2 =  35695285721281263482801168 v2 = 19130030301998281706202485
|        r = 16565255419282981776598683 u2 =  19130030301998281706202485 v2 = 16565255419282981776598683
|        r = 2564774882715299929603802 u2 =  16565255419282981776598683 v2 = 2564774882715299929603802
|        r = 1176606122991182198975871 u2 =  2564774882715299929603802 v2 = 1176606122991182198975871
|        r = 211562636732935531652060 u2 =  1176606122991182198975871 v2 = 211562636732935531652060
|        r = 118792939326504540715571 u2 =  211562636732935531652060 v2 = 118792939326504540715571
|        r = 92769697406430990936489 u2 =  118792939326504540715571 v2 = 92769697406430990936489
|        r = 26023241920073549779082 u2 =  92769697406430990936489 v2 = 26023241920073549779082
|        r = 14699971646210341599243 u2 =  26023241920073549779082 v2 = 14699971646210341599243
|        r = 11323270273863208179839 u2 =  14699971646210341599243 v2 = 11323270273863208179839
|        r = 3376701372347133419404 u2 =  11323270273863208179839 v2 = 3376701372347133419404
|        r = 1193166156821807921627 u2 =  3376701372347133419404 v2 = 1193166156821807921627
|        r = 990369058703517576150 u2 =  1193166156821807921627 v2 = 990369058703517576150
|        r = 202797098118290345477 u2 =  990369058703517576150 v2 = 202797098118290345477
|        r = 179180666230356194242 u2 =  202797098118290345477 v2 = 179180666230356194242
|        r = 23616431887934151235 u2 =  179180666230356194242 v2 = 23616431887934151235
|        r = 13865643014817135597 u2 =  23616431887934151235 v2 = 13865643014817135597
|        r = 9750788873117015638 u2 =  13865643014817135597 v2 = 9750788873117015638
|        r = 4114854141700119959 u2 =  9750788873117015638 v2 = 4114854141700119959
|        r = 1521080589716775720 u2 =  4114854141700119959 v2 = 1521080589716775720
|        r = 1072692962266568519 u2 =  1521080589716775720 v2 = 1072692962266568519
|        r = 448387627450207201 u2 =  1072692962266568519 v2 = 448387627450207201
|        r = 175917707366154117 u2 =  448387627450207201 v2 = 175917707366154117
|        r = 96552212717898967 u2 =  175917707366154117 v2 = 96552212717898967
|        r = 79365494648255150 u2 =  96552212717898967 v2 = 79365494648255150
|        r = 17186718069643817 u2 =  79365494648255150 v2 = 17186718069643817
|        r = 10618622369679882 u2 =  17186718069643817 v2 = 10618622369679882
|        r = 6568095699963935 u2 =  10618622369679882 v2 = 6568095699963935
|        r = 4050526669715947 u2 =  6568095699963935 v2 = 4050526669715947
|        r = 2517569030247988 u2 =  4050526669715947 v2 = 2517569030247988
|        r = 1532957639467959 u2 =  2517569030247988 v2 = 1532957639467959
|        r = 984611390780029 u2 =  1532957639467959 v2 = 984611390780029
|        r = 548346248687930 u2 =  984611390780029 v2 = 548346248687930
|        r = 436265142092099 u2 =  548346248687930 v2 = 436265142092099
|        r = 112081106595831 u2 =  436265142092099 v2 = 112081106595831
|        r = 100021822304606 u2 =  112081106595831 v2 = 100021822304606
|        r = 12059284291225 u2 = 100021822304606  v2 = 12059284291225
|        r = 3547547974806 u2 = 12059284291225  v2 = 3547547974806
|        r = 1416640366807 u2 = 3547547974806 v2  = 1416640366807
|        r = 714267241192 u2 = 1416640366807 v2  = 714267241192
|        r = 702373125615 u2 = 714267241192 v2 =  702373125615
|        r = 11894115577 u2 = 702373125615 v2 =  11894115577
|        r = 620306572 u2 = 11894115577 v2 =  620306572
|        r = 108290709 u2 = 620306572 v2 =  108290709
|        r = 78853027 u2 = 108290709 v2 =  78853027
|        r = 29437682 u2 = 78853027 v2 =  29437682
|        r = 19977663 u2 = 29437682 v2 =  19977663
|        r = 9460019 u2 = 19977663 v2 = 9460019
|        r = 1057625 u2 = 9460019 v2 = 1057625
|        r = 999019 u2 = 1057625 v2 = 999019
|        r = 58606 u2 = 999019 v2 = 58606
|        r = 2717 u2 = 58606 v2 = 2717
|        r = 1549 u2 = 2717 v2 = 1549
|        r = 1168 u2 = 1549 v2 = 1168
|        r = 381 u2 = 1168 v2 = 381
|        r = 25 u2 = 381 v2 = 25
|        r = 6 u2 = 25 v2 = 6
|        r = 1 u2 = 6 v2 = 1
|        r = 0 u2 = 1 v2 = 0
|        ---> 1
|
+============================================================================*/

template <typename IntType>
IntType gcd( const IntType & u, const IntType & v )
{
    IntType r ;
    IntType u2 = u ;
    IntType v2 = v ;

    #ifdef DEBUG_PP_ARITH
    cout << "gcd:  u = " << u << " v = " << v << endl ;
    #endif

    while (v2 != static_cast<IntType>(0u))
    {
        r  = u2 % v2 ;
        u2 = v2 ;
        v2 = r ;

       #ifdef DEBUG_PP_ARITH
       cout << "  r = " << r << " u2 = " << u2 << " v2 = " << v2 << endl ;
       #endif
    }

    return u2 ;
}


/*==============================================================================
|                     Forced Template Instantiations                           |
==============================================================================*/

// C++ doesn't automatically generate templated classes or functions unless
// we USE them on particular data types.
template ppuint   gcd( const ppuint   &, const ppuint   & ) ;
template BigInt   gcd( const BigInt &, const BigInt & ) ;

template ppuint addMod( ppuint a, ppuint b, ppuint n ) ;
template ppuint timesTwoMod( ppuint a, ppuint n ) ;
template ppuint multiplyMod( const ppuint a, const ppuint b, const ppuint n ) ;

// We already specialized this function for ppuint in the source code implementation above, so we can omit
// the template instantiation:   template ppuint PowerMod<ppuint>::operator()( const ppuint & a, const ppuint & n ) ;
template BigInt  PowerMod<BigInt>::operator()( const BigInt & a, const BigInt & n ) ;

template ModP<ppuint,ppsint>::ModP( ppuint p ) ;
template ModP<ppuint,ppsint>::ModP( const ModP & ) ;
template ppuint ModP<ppuint,ppsint>::operator()( ppsint ) ;

