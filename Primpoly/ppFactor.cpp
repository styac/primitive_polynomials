/*==============================================================================
|
|  NAME
|
|     ppFactor.cpp
|
|  DESCRIPTION
|
|     Classes for factoring integers.
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
|     with !DOT! replaced by . and the !AT! replaced by @
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
#include <algorithm>    // Iterators, sorting, merging, union.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()
#include <regex>        // Regular expressions.

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
#include "ppParser.h"         // Parsing of polynomials and I/O services.
#include "ppUnitTest.h"       // Complete unit test.

/*=============================================================================
 |
 | NAME
 |
 |    Factorization<IntType>::factorTable( ppuint p, ppuint n )
 |
 | DESCRIPTION
 |                                                 n
 |    Table lookup for the prime factorization of p
 |    False means the integer didn't occur in the table (or there was no table).
 |
 +============================================================================*/

template <typename IntType>
bool Factorization<IntType>::factorTable( ppuint p, ppuint n )
{
    // Clear out the factorization.
    factor_.clear() ;

    // List of factor table names for each p.
    vector< string > factorTableName  {
        "", // p = 0
        "", // p = 1
        "c02minus.txt",
        "c03minus.txt",
        "", // p = 4
        "c05minus.txt",
        "c06minus.txt",
        "c07minus.txt",
        "", // p = 8
        "", // p = 9
        "c10minus.txt",
        "c11minus.txt",
        "c12minus.txt"
    } ;
    
    // Check if p is in any table.
    if (p > factorTableName.size() - 1 || factorTableName[ p ].length() == 0)
        return false ;
    
    string factorizationTableDir = PolyParser<PolySymbol, PolyValue>::factorizationTableLocation_ ;
    
    // Open an input stream from the file.
    ifstream fin( factorTableName[ p ] ) ;
    
    // At this point, we ought to have a table for p as one of the named files above in the same directory
    // as the executable.
    if (!fin)
    {
        ostringstream os ;
        os << "Missing the factor table for p = " << p << " named " << factorTableName[ p ]
        << " It ought to be located in the current directory which contains the executable! "
        << " at " << __FILE__ << ": line " << __LINE__ ;
        throw FactorError( os.str() ) ;
    }
    
    // The header pattern right before the factorizations, e.g.
    //    n  #Fac  Factorisation
    regex headerPattern                      { R"(^\s*n\s*#Fac\s+Factorisation)" } ;
    
    // The first factorization line.  It must have at least three numbers, whitespace separated, where third number and subsequent numbers are separated by dots and carets, e.g.
    //     84    14  3^2.5.7^2.13.29.43.113.127.337.1429.5419.14449
    regex beginFactorizationLinePattern      { R"(\s*\d+\s+\d+\s+(\d+|\^|\.)+)" } ;
    
    // A continuation line either ends in a backslash, e.g.
    //     306    19  3^3.7.19.73.103.307.919.2143.2857.6529.11119.43691.123931.131071.26159806891.27439122228481.755824884241793\
    //                47083438319
    // Or it ends with a period separating the factors, e.g.,
    //     300    28  3^2.5^3.7.11.13.31.41.61.101.151.251.331.601.1201.1321.1801.4051.8101.63901.100801.268501.10567201.13334701.
    //                1182468601.1133836730401
    regex continuationLinePattern { R"(.*(\\|\.)$)" } ;
    
    // Scan through the lines of the file.
    bool foundHeader           { false } ;
    bool continuationLineState { false } ;
    vector<string> lineOfTable ;
    
    for (string line ;  getline( fin, line ) ; )
    {
        // Skip initial lines in the comment section until we encounter the header line
        //   n  #Fac  Factorisation
        // Skip this line too, but tag as having found the factorization lines.
        if (regex_match( line, headerPattern ))
        {
            foundHeader = true ;
            continue ;
        }
        else if (!foundHeader)
            continue ;
        
        // Not in a continuation sequence.
        if (!continuationLineState)
        {
            // NOW we are in a continuation pattern.
            if (regex_match( line, continuationLinePattern ))
                continuationLineState = true ;
            
            // Either not a continuation line (i.e. list of factors) or the first continuation line in the sequence.
            // Either way, it's the start of the factorization line.
            lineOfTable.push_back( line ) ;
        }
        // In a continuation sequence.
        else
        {
            // Could be a continuation line, or the non-continuation line which terminates the sequence.
            // Either way, concatenate to the end of the current line.
            lineOfTable.back() += line ;

            // This is the first non-continuation line, so ends the sequence.
            if (!regex_match( line, continuationLinePattern ))
                continuationLineState = false ;
        }
    }

    // Set up the factorization parser.
    FactorizationParser< FactorizationSymbol, FactorizationValue<IntType> > parser ;

    // Parse the factorization lines until we see the one which matches p and n.
    for (auto & line : lineOfTable)
    {
        #ifdef DEBUG_PP_FACTOR
        // Look for + in the table (incomplete factorization).
        // We don't handle incomplete factorizations in the table.  The composite number is likely to be too large to factor
        // with Pollard's method.
        cout << "Line of factor table for " << p << " ^ n - 1 = " << line << ((line.find( "+" ) != string::npos) ? "WARNING:  incomplete factorization" : "") << endl ;
        #endif // DEBUG_PP_FACTOR

        // The factorization is complete.
        if (line.find( "+" ) == string::npos)
        {
            // Parse a factorization line.  For example p = 3 and n = 295 has the line
            //  295     9  2.5^2.1181.3221.106185841.70845409351.33083146850190391025301565142735000331370209599...68349655382191991676711\3033493902702...03521
            FactorizationValue<IntType> v = parser.parse( line ) ;
            
            if (FactorizationValue<IntType>::numberStringToInteger( v.numberString_ ) == static_cast<IntType>( n ))
            {
                // Copy the factors over.
                factor_.clear() ;
                for (auto & f : v.factor_)
                    factor_.push_back( f ) ;
                
                // Multiply the factors together, whilst testing each distinct prime factor
                // is probably prime.
                IntType prod = static_cast<IntType>( 1u ) ;
                
                for (unsigned int i = 0 ;  i < factor_.size() ;  ++i)
                {
                    // Test if the prime factor is really prime.
                    if (!isAlmostSurelyPrime( factor_[ i ].prime_ ))
                    {
                        ostringstream os ;
                        os << "Distinct prime factor isn't prime "
                        << " p " << factor_[ i ].prime_
                        << " at " << __FILE__ << ": line " << __LINE__ ;
                        throw FactorError( os.str() ) ;
                    }
                    else
                        for (int j = 1 ;  j <= factor_[ i ].count_ ;  ++j)
                            prod *= factor_[ i ].prime_ ;
                }
                
                IntType prod2 = static_cast<IntType>( 1u ) ;

                //          n
                // Compute p  - 1
                for (int j = 1 ;  j <= n ;  ++j)
                    prod2 *= p ;

                prod2 -= static_cast<IntType>( 1u ) ;
                
                // Test whether the product of factors equals the original number again.
                if (prod == prod2)
                    return true ;
                else
                {
                    ostringstream os ;
                    os << "Product of factors doesn't equal the number "
                    << " p^n - 1 " << n
                    << " at " << __FILE__ << ": line " << __LINE__ ;
                    throw FactorError( os.str() ) ;
                }
            } // Found n
        } // no plus sign
    } // line of table

    return false ;
}


/*=============================================================================
 |
 | NAME
 |
 |    Factor
 |
 | DESCRIPTION
 |
 |    Factor a large integer into primes using tables of prime factors, trial
 |    division and Pollard's rho methods.  Factoring algorithm type defaults to 
 |    Automatic, which decides when to use each algorithm for best speed.
 |
 | NOTES
 |
 |    Tables of prime factors takes negligible time.
 |
 |                                       ---          1/2
 |    Trial division takes max( p    , \/ p  )  = O( N   ) operations, while
 |                               t-1       t
 |
 |                                       ---          1/4
 |    Pollard rho takes                \/ p       = O( N  ) operations.
 |                                         t-1
 | 
 +============================================================================*/

template <typename IntType>
Factorization<IntType>::Factorization( const IntType n, FactoringAlgorithm type, ppuint p, ppuint m )
    : n_()
    , numFactors_()
    , factor_()
    , statistics_()
    , distinctPrimeFactors_()
{
    factor_.clear() ;
    distinctPrimeFactors_.clear() ;

    // Initialize the unfactored remainder to n to begin with.
    n_ = n ;

    // For unit testing only, specify the type of algorithm.
    if (type == FactoringAlgorithm::FactorTable)
    {
        if (!factorTable( p, m ))
        #ifdef DEBUG_PP_FACTOR
            cout << "Table lookup factoring failed for p = " << p << " ^ m =" << m << endl ;
        #endif
        ;
    }
    else if (type == FactoringAlgorithm::PollardRhoAlgorithm)
    {
        if (!PollardRho())
        #ifdef DEBUG_PP_FACTOR
            cout << "Pollard Rho factoring failed for p = " << p << " ^ m =" << m << endl ;
        #endif
        ;
    }
    else if (type == FactoringAlgorithm::TrialDivisionAlgorithm)
    {
        trialDivision() ;
        #ifdef DEBUG_PP_FACTOR
            cout << "Trial division factoring always succeeds for p = " << p << " ^ m =" << m << endl ;
        #endif
        ;
    }
    // type == Automatic
    else
    {
        #ifdef DEBUG_PP_FACTOR
        cout << "Try best factoring methods on n = " << n_ << endl ;
        #endif
        
        // Try table lookup first.
        if (!factorTable( p, m ))
        {
            #ifdef DEBUG_PP_FACTOR
            cout << "Table lookup factoring failed on n =" << n_ << endl ;
            cout << "Try again using Pollard Rho" << endl ;
            #endif

            // Try Pollard's method first.  If it fails, keep the
            // factors found so far, and retry on the unfactored
            // remainder.
            if (!PollardRho())
            {
                #ifdef DEBUG_PP_FACTOR
                cout << "Pollard Rho failed on n =" << n_ << endl ;
                cout << "Try again with different random seed" << endl ;
                #endif

                // Try again with random c, but avoid using c in { 0, 1, -2 }
                if (!PollardRho( 5u ))
                {
                    #ifdef DEBUG_PP_FACTOR
                    cout << "Pollard Rho failed once again on n =" << n_ << endl ;
                    cout << "switching to trial division" << endl ;
                    #endif
                 
                    // Still fails?  Fall back onto trial division.  This WILL succeed
                    // but can be very slow.
                    trialDivision() ;
                }
            }
        }
     }
	 
	#ifdef DEBUG_PP_FACTOR
    cout << "     Unsorted prime factors." << endl ;
    for (auto & i : factor)
    {
        PrimeFactor<IntType> e = *i ;
        cout << e.prime_ << " " << e.count_ << endl ;
    }
    #endif

    // Sort primes into ascending order.
    sort( factor_.begin(), factor_.end(), CompareFactor<IntType>() ) ;

    // Merge duplicated prime factors into unique primes to powers. e.g.
    //  2     1           5      0     3     0     6
    // 2     2     3     3  =>  2     2     3     3
    for (unsigned int i = 1 ;  i < factor_.size() ;  ++i)
    {
        // This prime factor is a duplicate of the previous factor.
        if (factor_[ i ].prime_ == factor_[ i-1 ].prime_)
        {
            // Merge into current prime power and zero the previous power.
            factor_[ i ].count_ += factor_[ i-1 ].count_ ;
            factor_[ i-1 ].count_ = 0 ;
        }
    }

    // Remove factors of 1 (including powers of 0 introduced in the previous step).
    typename vector< PrimeFactor<IntType> >::iterator last =
        remove_if( factor_.begin(), factor_.end(), Unit<IntType>() ) ;
    factor_.erase( last, factor_.end() ) ;
    numFactors_ = factor_.size() ;

    // Record a vector of the distinct prime factors.
    for (unsigned int i = 0 ;  i < numFactors_ ;  ++i)
        distinctPrimeFactors_.push_back( factor_[ i ].prime_ ) ;

    #ifdef DEBUG_PP_FACTOR
    numFactors_ = factor_.size() ;
    cout << numFactors_ << " sorted unique factors" << endl ;
    for (int i = 0 ;  i < numFactors_ ;  ++i)
        cout << factor_[ i ].prime_ << " ^ " << factor_[ i ].count_ << endl ;
    #endif
 }



/*=============================================================================
 |
 | NAME
 |
 |    Factorization< IntType >
 |
 | DESCRIPTION
 |
 |   Factor a generic integer type n into primes.  Record all the distinct
 |   prime factors and how many times each occurs.  Return the number of 
 |   primes - 1.
 |
 |Factorization<BigInt> f ;
 | 
 | 
 |     primes (bigint *)   List of distinct prime factors.
 |     count  (ppuint *)     List of how many times each factor occurred.
 | 
 |      When n = 1, t = 0, and primes[ 0 ] = count[ 0 ] = 1.
 | 
 | RETURNS
 | 
 |      t (int)          Number of prime factors - 1.
 | 	                     Prime factors and their multiplicites are in locations 0 to t.
 | 
 | EXAMPLE
 |                    2
 |     For n = 156 = 2  * 3 * 13 we have
 | 
 |     k   primes[ k ]   count[ k ]
 |     ----------------------------
 |     0        2            2
 |     1        3            1
 |     2       13            1
 | 
 | METHOD
 | 
 |     Method described by D.E. Knuth, ART OF COMPUTER PROGRAMMING, vol. 2, 3rd ed.,
 | 	                                                            -----
 | 	   in Algorithm A, pgs. 364-365.  The running time is O( max \/ p   , p  )
 |                                                                   t-1   t
 |     where p  is the largest prime divisor of n and p    is the next largest.
 | 	          t                                        t-1
 | 
 |     (1)  First divide out all multiples of 2 and 3 and count them.
 | 
 | 	   (2)  Next, divide n by all integers d >= 5 except multiples of 2 and 3.
 | 
 |     (3)  Halt either when all prime factors have been divided out (leaving n = 1)
 | 	        or when the current value of n is prime.  The stopping test
 | 
 |           (d > | n/d | AND r != 0)
 |                --   --
 |          detects when n is prime.
 | 
 |     In the example above, we divided out 2's and 3's first, leaving
 |     n = 13.  We continued with trial divisor d = 3.  But now the stopping
 |     test was activated because 5 > | 13/5 | = 2, and remainder = 3 (non-zero).
 |                                    --    --
 |     n = 13 itself is the last prime factor of 156.  We return t = 2.  There
 |     are 2 + 1 = 3 distinct primes.
 | 
 | 
 |     If we start with d = 5, and add 2 and 4 in succession, we will run through all
 | 	   the integers except multiples of 2 and 3.  To see this, observe the pattern:
 | 
 |        Integers:     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
 |     Mult. of 2:      x     x     x     x     x     x     x     x
 | 	   Mult. of 3:         x        x        x        x        x
 |     d:                        x     x           x     x          x
 | 	   Separation:                  2       4         2       4
 | 
 |     The sequence of divisors d, includes all the primes, so is safe to use for
 | 	   factorization testing.
 | 
 |     Theorem.  Suppose no divisor d in the sequence divides n.  Suppose at some point,
 | 	   q < d with r != 0.  Then n must be prime.
 | 
 |     Proof.  We begin with an integer n which has all powers of 2 and 3 removed.
 |     Assume, to the contrary, that n is composite:  n = p ... p
 | 	      t                                             1     t
 |     n >= p    since p  is the smallest prime factor.
 | 	      1          1
 | 
 | 	      2
 |     n >= p    since n is composite, so has at least 2 prime factors.
 | 	      1
 | 		        2
 | 	   >= (d + 1)   since p  > d implies p >= (d + 1).  p > d because we assumed
 |                           1              1              1
 |    that no d in the sequence divided n, so d couldn't be any of the prime divisors p
 | 	                                                                                   i
 |            2           2               2
 | 	  = d + 2d + 1 = d + d + d + 1 > d + d
 |
 |       2
 |    > d + n mod d   since n mod d < d
 |                                                                               2
 |    > | n / d | d + n mod d  because our test said q < d, so | n / d | d < d
 | 	    --     --                                              --     --
 | 
 |    = n.  So we get the contradiction n > n.  Thus n must be prime.
 | 	                                               ---
 |    Note that this is sharper than the bound d > \/ n
 | 
 |    Theorem.  Conversely, if n is a prime, no d will divide it, so at some point,
 | 	  d will be large enough that q = | n / d | < d.  r != 0 since n is prime.
 | 	                                --     --
 | 
 |    Theorem.  The factoring algorithm works.
 | 
 | 	  Proof.  If n == 1 we exit immediately.  If n is composite, we divide out all powers
 | 	  of 2 and 3 (if any).  Since d runs through all possible prime divisors, we divide
 | 	  these out also.  Composite trial d causes no problem;  prime factors of d are divided
 | 	  out of n before we try to divide by d, so such a d cannot divide n.
 | 
 |     We end in one of two ways (1) All divisors of n have been divided out in which case n = 1.
 | 	  (2) n is a prime, so the stopping test is activiated and n != 1 is recorded as a prime
 | 	  divisor.
 | 
 | 
 | TODO:
 | 
 |     Can be slow when n is a prime.  We could do a probabilistic test for
 |     the primality of n at the statement, "n_is_prime = (r != 0 && q < d) ;"
 |     which might speed up the test.
 | 
 +============================================================================*/

template <typename IntType>
void Factorization<IntType>::trialDivision()
{
    #ifdef DEBUG_PP_FACTOR
    cout << "trial division on n =" << n_ << endl ;
    #endif

    // Remove factors of 2.
    int cnt = 0 ;
    while( n_ % static_cast<IntType>( 2u ) == static_cast<IntType>( 0u ))
    {
        n_ /= static_cast<IntType>( 2u ) ;
        ++cnt ;
        ++statistics_.numTrialDivides ;
    }

    if (cnt != 0)
    {
        PrimeFactor<IntType> f( static_cast<IntType>( 2u ), cnt ) ;
        factor_.push_back( f ) ;
    }

    #ifdef DEBUG_PP_FACTOR
    cout << "after removing powers of 2, n =" << n_ << endl ;
    #endif

    // Remove factors of 3.
    cnt = 0 ;
    while( n_ % static_cast<IntType>( 3u ) == static_cast<IntType>( 0u ))
    {
        n_ /= static_cast<IntType>( 3u ) ;
        ++cnt ;
        ++statistics_.numTrialDivides ;
    }

    if (cnt != 0)
    {
        PrimeFactor<IntType> f( 3u, cnt ) ;
        factor_.push_back( f ) ;
    }

    #ifdef DEBUG_PP_FACTOR
    cout << "after removing powers of 3, n =" << n_ << endl ;
    #endif


    //  Factor the rest of n.  Continue until n = 1 (all factors divided out)
    //  or until n is prime (so n itself is the last prime factor).

    bool new_d = true ;       //  true if current divisor is different from
                              //  the previous one.
    bool n_is_prime = false ; //  true if current value of n is prime.
    bool flag = true ;        //  Alternately true and false.
    IntType d = static_cast<IntType>( 5u ) ;  // First trial divisor.

    do {
        //  Integer divide to get quotient and remainder:  n = qd + r
        // TODO:  We can speed this up by 2x using a combined divMod call.
        IntType q = n_ / d ;
        IntType r = n_ % d ;
        ++statistics_.numTrialDivides ;

        #ifdef DEBUG_PP_FACTOR
        cout << "n = " << n_ << endl ;
        cout << "d = " << d  << endl ;
        cout << "q = " << q  << endl ;
        cout << "r = " << r  << endl ;
        #endif

        // Stopping test.
        n_is_prime = (r != static_cast<IntType>( 0u ) && q < d) ;

        // d | n.
        if (r == static_cast<IntType>( 0u ))
        {
            n_ = q ;     //  Divide d out of n.

            if (new_d)  //  New prime factor.
            {
                PrimeFactor<IntType> f( d, 1u ) ;
                factor_.push_back( f ) ;

                new_d = false ;
                #ifdef DEBUG_PP_FACTOR
                cout << "after dividing out new prime divisor d = " << d << " n =" << n_ << endl ;
                #endif
             }
             else
             {
                 ++factor_[ factor_.size() - 1 ].count_ ; //  Same divisor again.  Increment its count.

                #ifdef DEBUG_PP_FACTOR
                cout << "after dividing out repeated factor d = " << d << " n =" << n_ << endl ;
                #endif
             }
        }
        else 
        {
            d += (flag ? static_cast<IntType>( 2u ) : static_cast<IntType>( 4u )) ;  // d did not divide n.  Try a new divisor.
            flag = !flag ;
            new_d = true ;

            #ifdef DEBUG_PP_FACTOR
            cout << "next trial divisor d  = " << d << endl ;
            #endif
        }

    } while( !n_is_prime && (n_ != static_cast<IntType>( 1u )) ) ;

    if (n_ == static_cast<IntType>( 1u ))       //  All factors were divided out.
    {
        numFactors_ = factor_.size() ;
        return ;
    }
    else
    {  //  Current value of n is prime.  It is the last prime factor.
        PrimeFactor<IntType> f( n_, 1u ) ;
        factor_.push_back( f ) ;
        numFactors_ = factor_.size() ;
        return ;
    }
} 




/*=============================================================================
 |
 | NAME
 |
 |    ~Factor
 |
 | DESCRIPTION
 |
 |    Destructor for factor.
 |
 +============================================================================*/

template <typename IntType>
Factorization<IntType>::~Factorization()
{
   // Do nothing.
}


/*=============================================================================
 |
 | NAME
 |
 |    Factor copy constructor
 |
 | DESCRIPTION
 |
 |    Copy a factor object.
 |
 +============================================================================*/

template <typename IntType>
Factorization<IntType>::Factorization( const Factorization<IntType> & f )
        : n_( f.n_ )
		, factor_( f.factor_ )
		, distinctPrimeFactors_( f.distinctPrimeFactors_ )
		, numFactors_( f.numFactors_ )
        , statistics_( f.statistics_ )
{
}

/*=============================================================================
 |
 | NAME
 |
 |    Factor assignment operator
 |
 | DESCRIPTION
 |
 |    Assignment constructor.
 |
 +============================================================================*/
 
// Assignment operator.
template <typename IntType>
Factorization<IntType> & Factorization<IntType>::operator=( const Factorization<IntType> & g )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &g)
        return *this ;
		
    n_          = g.n_ ;
	factor_     = g.factor_ ;
	distinctPrimeFactors_ = g.distinctPrimeFactors_ ;
	numFactors_ = g.numFactors_ ;
    statistics_ = g.statistics_ ;
	
    // Return a reference to assigned object to make chaining possible.
    return *this ;
}


/*=============================================================================
 |
 | NAME
 |
 |    operator[]
 |
 | DESCRIPTION
 |
 |    Return a reference to the ith prime factor and its multiplicity.
 |
 +============================================================================*/

template <typename IntType>
PrimeFactor<IntType> & Factorization<IntType>::operator[]( int i )
{
    // We throw our own exception here.
    if (i > numFactors_)
	 {
		ostringstream os ;
		os << "Error accessing Factor "
		   << " at index i = " << i
		   << " at " << __FILE__ << ": line " << __LINE__ ;
		throw FactorRangeError( os.str() ) ;
	 }

    return factor_[ i ] ;
}


/*=============================================================================
 |
 | NAME
 |
 |    num_distinct_factors
 |
 | DESCRIPTION
 |
 |    Return the number of distinct prime factors.
 |
 +============================================================================*/

template <typename IntType>
ppuint Factorization<IntType>::num_distinct_factors() const
{
    return numFactors_ ;
}

 /*=============================================================================
  |
  | NAME
  |
  |    prime_factor
  |
  | DESCRIPTION
  |
  |    Return the ith distinct prime factor.
  |
  +============================================================================*/
 
 template <typename IntType>
 IntType Factorization<IntType>::prime_factor( int i ) const
 {
     // We throw our own exception here.
     if (i > numFactors_)
     {
         ostringstream os ;
         os << "Error accessing distinct prime factor "
         << " at index i = " << i
         << " at " << __FILE__ << ": line " << __LINE__ ;
         throw FactorRangeError( os.str() ) ;
     }
     
     return factor_[ i ].prime_ ;
 }


/*=============================================================================
 |
 | NAME
 |
 |    multiplicity
 |
 | DESCRIPTION
 |
 |    Return the multiplicity for the ith prime factor.
 |
 +============================================================================*/

template <typename IntType>
int Factorization<IntType>::multiplicity( int i ) const
{
    // We throw our own exception here.
    if (i > numFactors_)
	 {
		ostringstream os ;
		os << "Error accessing multiplicity "
		   << " at index i = " << i
		   << " at " << __FILE__ << ": line " << __LINE__ ;
		throw FactorRangeError( os.str() ) ;
	 }

    return factor_[ i ].count_ ;
}


/*=============================================================================
 |
 | NAME
 |
 |    getDistinctPrimeFactors
 |
 | DESCRIPTION
 |
 |    Return a vector of only the distinct prime factors.
 |
 +============================================================================*/

template <typename IntType>
vector<IntType>Factorization<IntType>::getDistinctPrimeFactors()
{
    // TODO:  do I want really copy on the stack?
    return distinctPrimeFactors_ ;
}



 /*=============================================================================
  |
  | NAME
  |
  |    factorRAndFindNumberOfPrimitivePolynomials
  |
  | DESCRIPTION
  |
  |    This is the most time consuming step for large n due to the integer
  |    factorization.
  |
  |                                             n
  |    Find the maximum number of polynomials  p
  |
  |    Find
  |              n
  |             p  - 1
  |        r = --------
  |             p - 1
  |
  |    Compute the prime factorization of r.
  |                                                 n
  |    Find number of primitive polynomials = Phi( p - 1 ) / n
  |
  | EXAMPLE
  |
  |    See the examples in the code below.
  |
  +============================================================================*/
 
 void factorRAndFindNumberOfPrimitivePolynomials( ppuint p, int n,
                                                 BigInt & maxNumPossiblePoly, BigInt & r, Factorization<BigInt> & factorsOfR, BigInt & numPrimitivePoly )
 {
     //  n
     // p  = total number of possible polynomials of degree n modulo p
     maxNumPossiblePoly = power( p, n ) ;
     
     //  n
     // p - 1
     BigInt p_to_n_minus_1 = maxNumPossiblePoly - static_cast<BigInt>( 1u ) ;
     
     //         n
     // Factor p  - 1 into primes.
     // Pass in p and n in case we can do a fast table lookup.
     Factorization<BigInt> factors_of_p_to_n_minus_1( p_to_n_minus_1, FactoringAlgorithm::Automatic, p, n ) ;
     
     //                                                     n
     // Compute the number of primitive polynomials = Phi( p - 1 ) / n
     //
     // Recall Euler's totient is
     //
     //              -----               -----             -----
     // Phi[ n ] = n  | | (1 - 1/p ) = n  | |  (p - 1)  /   | |  p
     //                           i              i                i
     //           p = all distinct
     //            i
     //           prime factors of n
     //
     // For example,
     //
     //   8                     5  2
     // 19 - 1 = 16983563040 = 2  3  5  17 181 3833
     //
     //        8           8
     // Phi( 19 - 1 ) = (19 - 1) (2-1) (3-1) (5-1) (17-1) (181-1) (3833-1) / (2 3 5 17 181 3833) = 4237885440
     //
     // You can check with Wolfram Alpha on the web,
     //
     //     http://www.wolframalpha.com/input/?i=eulerphi%28+19^8-1%29
     //                                                   8
     // then the number of primitive polynomials = Phi( 19 - 1) / 8 = 529735680
   
    numPrimitivePoly = p_to_n_minus_1 ;
    vector<BigInt> distinct_factors_of_p_to_n_minus_1 = factors_of_p_to_n_minus_1.getDistinctPrimeFactors() ;
     
//    for (unsigned int i = 0u ;  i < distinct_factors_of_p_to_n_minus_1.size() ; ++i)
//         numPrimitivePoly *= (distinct_factors_of_p_to_n_minus_1[ i ] - static_cast<BigInt>( 1u )) ;
    for (auto & f : distinct_factors_of_p_to_n_minus_1)
        numPrimitivePoly *= (f - static_cast<BigInt>( 1u )) ;

//   for (unsigned int i = 0u ;  i < distinct_factors_of_p_to_n_minus_1.size() ; ++i)
//     numPrimitivePoly /=  distinct_factors_of_p_to_n_minus_1[ i ] ;
    for (auto & f : distinct_factors_of_p_to_n_minus_1)
        numPrimitivePoly /=  f ;
   
     numPrimitivePoly /= static_cast<BigInt>( n ) ;
     
     //       n
     //      p - 1
     // r = -------
     //      p - 1
     r = p_to_n_minus_1 / (p - 1u) ;
     
     // Factor p - 1 into primes.
     Factorization<BigInt> factors_of_p_minus_1( p - 1u ) ;
     
    #ifdef DEBUG_PP_FACTOR
     cout << "p = " << p << endl ;
     cout << "n = " << n << endl ;
     cout << "p^n = " << maxNumPossiblePoly << endl ;
     cout << "r = (p^n-1)/(p-1) = " << r << endl ;
     cout << "factorization of p^n - 1 = " << endl ;
     for (unsigned int i = 0 ;  i < factors_of_p_to_n_minus_1.num_distinct_factors() ;  ++i)
         cout << factors_of_p_to_n_minus_1.prime_factor( i ) << " ^ " << factors_of_p_to_n_minus_1.multiplicity( i ) << endl ;
     cout << "factorization of p - 1 = " << endl ;
     for (unsigned int i = 0 ;  i < factors_of_p_minus_1.num_distinct_factors() ;  ++i)
         cout << factors_of_p_minus_1.prime_factor( i ) << " ^ " << factors_of_p_minus_1.multiplicity( i ) << endl ;
    #endif // DEBUG_PP_FACTOR

     //                                                 n
     // Factor r by starting with the factorization of p - 1
     
     // Now we have to divide out all factors of (p - 1).
     // e.g.
     //
     //  n        8       5  2
     // p - 1 = 19 - 1 = 2  3  5  17 181 3833
     //
     //                      2
     // p - 1 = 19 - 1 = 2  3
     //
     //  n        8       4
     // p - 1 = 19 - 1 = 2     5  17 181 3833
     // -----   ------
     // p - 1   19 - 1
     //
     
     //                           n
     // Copy over the factors of p  - 1
     factorsOfR = factors_of_p_to_n_minus_1 ;
     
     // We're done if p - 1 = 1.
     if (p > 2)
     {
         //                                             n                   n
         // p-1 cannot have more distinct factors than p - 1 since p - 1 | p  - 1
         if (factors_of_p_minus_1.num_distinct_factors() > factors_of_p_to_n_minus_1.num_distinct_factors())
         {
             ostringstream os ;
             os << "factorRAndFindNumberOfPrimitivePolynomials "
                << " number of distinct prime factors for p-1   = " << factors_of_p_minus_1.num_distinct_factors() << " > "
                << " number of distinct prime factors for p^n-1 = " << factors_of_p_to_n_minus_1.num_distinct_factors()
                << " which is not possible since (p-1) | (p^n - 1)"
                << " at " << __FILE__ << ": line " << __LINE__ ;
             throw BigIntUnderflow( os.str() ) ;
         }

         // Divide out p-1, one prime factor at a time.
         for (int i = 0, j = 0 ;  i < factors_of_p_minus_1.num_distinct_factors() ;  ++i)
         {
             BigInt factor_of_p_m_1 = factors_of_p_minus_1.prime_factor( i ) ;
             BigInt factor_of_r     = factorsOfR.prime_factor( j ) ;
             
             // Divide out the common prime factor.  Advance to next prime factor in the numerator.
             if (factor_of_p_m_1 == factor_of_r)
             {
                 factorsOfR[ j ].count_ -= factors_of_p_minus_1[ i ].count_ ;
                 ++j ;
             }
             // Factor in denominator < factor in numerator.  Advance to next factor in denominator.
             else if (factor_of_p_m_1 > factor_of_r)
                 continue ;
             // Factor in denominator > factor in numerator.  Advance to next factor in numerator.  All smaller factors in numerator should have been divided out already.
             else
                 ++j ;
             
             #ifdef DEBUG_PP_FACTOR
             cout <<  " i = " << i << " prime factor of p-1 = " << factor_of_p_m_1  << "    j = " << j << " prime factor num = " << factor_of_r << endl ;
             #endif // DEBUG_PP_FACTOR
         }

        #ifdef DEBUG_PP_FACTOR
         cout << "factorization of r = " << endl ;
         for (unsigned int i = 0 ;  i < factorsOfR.num_distinct_factors() ;  ++i)
             cout << factorsOfR.prime_factor( i ) << " ^ " << factorsOfR.multiplicity( i ) << endl ;
         cout << "numPrimitivePoly = " << numPrimitivePoly << endl ;
        #endif // DEBUG_PP_FACTOR
     }
     
     return ;
}


/*=============================================================================
 |
 | NAME
 |
 |    isProbablyPrime
 |
 | DESCRIPTION
 |
 |      Test whether the number n is probably prime.  If n is composite
 |      we are correct always.  If n is prime, we will be wrong no more
 |      than about 1/4 of the time on average, probably less for
 |      any x and n.
 | 
 | INPUT
 | 
 |      n      (int, n >= 0)    Number to test for primality.
 |      x      (int, 1 < x < n for n > 6) A random integer.
 | 
 | RETURNS
 | 
 |      Probably        If n is probably prime.
 | 
 |      DefinitelyNot   If n is definitely not prime or n < 0,
 |                      or x is out of range.
 | 
 | METHOD
 | 
 |      Miller-Rabin probabilistic primality test, described by 
 |
 |          D. E. Knuth, THE ART OF COMPUTER PROGRAMMING, VOL. 2, 3rd ed.,
 |          Addison-Wesley, 1981, pgs. 250-265.
 |
 |          Errata for Volume 2: http://www-cs-faculty.stanford.edu/~knuth/taocp.html
 | 
 |      Let
 |                   k
 |          n - 1 = 2  q
 | 
 |      for odd q.  Now suppose n is prime and
 |
 |            q
 |           x  mod n != 1
 | 
 |      Then the sequence
 |                                          k
 |             q          2q               2 q          n-1
 |          { x  mod n, x    mod n, ..., x     mod n = x    mod n }
 | 
 |      must end with 1 by Fermat's theorem, and the element in the sequence just before the 1 first appears 
 |      must be n-1, since in the field GF( n ), the polynomial equation
 | 
 |           2
 |          y = 1 (mod n)
 | 
 |      has only two solutions, y = +1 or y = -1.  So if these conditions fail, n is definitely composite.  
 |      If the conditions succeed, we cannot tell for sure n is prime, but the probability the algorithm was 
 |      fooled in this case is bounded above by about 1/4 for any n.
 | 
 | EXAMPLE
 | 
 |      If k = 5, q = 3 then n = 1 + 2^5 3 = 97, which is prime.  
 |      
 |      For x = 10, we get                    For x = 9, we get
 | 
 |       q                                   q  
 |      x      =   30                       x     =  50
 | 
 |          1                                   1
 |       q 2                                 q 2
 |      x      =   27                       x     = 75
 | 
 |          2                                   2
 |       q 2                                 q 2
 |      x      =   50                       x     =  96 = n-1 => prob prime
 | 
 |          3                                   3
 |       q 2                                 q 2
 |      x      =   75                       x     = 1
 | 
 |          4                                   4
 |       q 2                                 q 2
 |      x      =   96 = n-1 => prob prime   x     = 1
 | 
 |          5                                   5
 |       q 2                                 q 2
 |      x      =    1                       x     =  1
 | 
 | 
 |      If k = 4, q = 3 then n = 1 + 2^4 3 = 49, which is composite.  
 |      
 |      For x = 10, we get
 | 
 |       q
 |      x      =   20
 | 
 |          1
 |       q 2
 |      x      =   8
 | 
 |          2
 |       q 2
 |      x      =   15
 | 
 |          3       
 |       q 2       
 |      x      =  29
 | 
 |          4      
 |       q 2      
 |      x      =  8  didn't find n-1 => composite.
 | 
 +============================================================================*/

template <typename IntType>
Primality isProbablyPrime( const IntType & n, const IntType & x )
{
    // Small composite numbers.
    if (n == static_cast<IntType>( 0u ) || n == static_cast<IntType>( 1u ) || n == static_cast<IntType>( 4u ))
        return Primality::Composite ;

    // Small primes.
    if (n == static_cast<IntType>( 2u ) || n == static_cast<IntType>( 3u ) || n == static_cast<IntType>( 5u ))
        return Primality::Prime ;

    // Composites aren't prime.
    if (n % static_cast<IntType>( 2u ) == static_cast<IntType>( 0u ) ||
        n % static_cast<IntType>( 3u ) == static_cast<IntType>( 0u ) ||
        n % static_cast<IntType>( 5u ) == static_cast<IntType>( 0u ))
        return Primality::Composite ;

    #ifdef DEBUG_PP_PRIMALITY_TESTING
    cout << "Begin the Miller-Rabin primality test on n = " << n << " for random x = " << x << endl ;
    #endif

    //                                                     k
    // Factor out powers of 2 to find odd q where n - 1 = 2  q.
    IntType reducedn = n - static_cast<IntType>( 1u ) ;
    IntType k   = static_cast<IntType>( 0u ) ;

    while( reducedn % static_cast<IntType>( 2u ) == static_cast<IntType>( 0u ))
    {
        reducedn /= static_cast<IntType>( 2u ) ;
        ++k ;
    }

    IntType q = reducedn ;

    #ifdef DEBUG_PP_PRIMALITY_TESTING
    cout << "after factoring out powers of 2, q = " << q << endl ;
    #endif

    //       q
    //  y = x  (mod n)
    PowerMod<IntType> power_mod( n ) ;
    IntType y = power_mod( x, q ) ;

    #ifdef DEBUG_PP_PRIMALITY_TESTING
    cout << "y = x ^ q (mod n) = " << y << endl ;
    #endif

    for (IntType j = 0u ;  j < k ;  ++j)
    {
        Primality primality = Primality::Undefined ;

        //           q                          q   2q
        // Check if x  = 1 mod n already OR if x , x  ,... = n-1 mod n at any point.
        if ( (j == static_cast<IntType>( 0u ) && y == static_cast<IntType>( 1u )) || 
             (y == n - static_cast<IntType>( 1u )) )
            primality = Primality::ProbablyPrime ;
        
        // Found a 1 BUT never found an n-1 term:  n can't be prime.
        //                 2
        //           2q   2 q
        // Check if x  , x   , ... = 1 mod n
        else if (j > static_cast<IntType>( 0u ) && y == static_cast<IntType>( 1 ))
            primality = Primality::Composite ;

        #ifdef DEBUG_PP_PRIMALITY_TESTING
        cout << "j = " << j << " until k = " << k << " y = " << y << endl ;
        cout << "primality = " ;    
        switch( primality )
        {
            case Primality::Prime: cout << "Prime" ; break ;
            case Primality::Composite: cout << "Composite" ; break ;
            case Primality::ProbablyPrime: cout << "ProbablyPrime" ; break ;
            case Primality::Undefined: cout << "Undefined" ; break ;
            default: cout << "Undefined" ;
        }
        cout << endl ;
        #endif

        if (primality != Primality::Undefined)
            return primality ;

        //          2
        // Compute y  (mod n)
        y = power_mod( y, static_cast<IntType>( 2u ) ) ;
    }

    // If we got here j = k and the sequence had no 1 or n-1 terms so n is composite.
    return Primality::Composite ;
}




/*=============================================================================
 |
 | NAME
 |
 |    isAlmostSurelyPrime
 |
 | DESCRIPTION
 |
 |      Test whether the number n >= 0 is almost surely prime.  If n is
 |      composite, we always return false.  If n is prime, the
 |      probability of returning true successfully is
 | 
 |                  NUM_PRIME_TEST_TRIALS
 |         1 - (1/4)
 | 
 | METHOD
 | 
 |     Use the Miller-Rabin probabilistic primality test for lots of
 |     random integers x.  If the test shows n is composite for any
 |     given x, is is non-prime for sure.
 | 
 |     But if n is prime, P( failure | n=prime ) <= 1/4, on each
 |     trial, so if the random numbers are independent,
 | 
 |                                    NUM_PRIME_TEST_TRIALS 
 |     P( failure | n=prime ) <= (1/4)
 | 
 |     for 25 trials, P <= 0.8817841970012523e-16
 |     and for 14 trials, P <= 3.7252902984619141e-09
 |
 |     The algorithm is from
 |
 |        D. E. Knuth, THE ART OF COMPUTER PROGRAMMING, VOL. 2, 3rd ed.,
 |        Addison-Wesley, 1981, pgs. 250-265.
 |
 |        Errata for Volume 2: http://www-cs-faculty.stanford.edu/~knuth/taocp.html
 | TODO:
 | 
 |     The system pseudorandom number generator needs to be a good one
 |     (i.e. passes the spectral test and other statistical tests).
 |
 +============================================================================*/

template <typename IntType>
bool isAlmostSurelyPrime( const IntType & n )
{
	IntType
		trial = 0u,
		x     = 3u ;

	constexpr ppuint NUM_PRIME_TEST_TRIALS = 14u ;

	// Seed the random-number generator.
	srand( 314159u );

	for (trial = static_cast<IntType>( 1u ) ;  trial <= static_cast<IntType>( NUM_PRIME_TEST_TRIALS ) ;  ++trial)
	{
		//  Generate a new random integer such that 1 < x < n.
		x = static_cast<IntType>( rand() ) % n ;
		
		// Clip away from 1.
		if (x <= static_cast<IntType>( 1u )) 
			x = static_cast<IntType>( 3u ) ;

        #ifdef DEBUG_PP_PRIMALITY_TESTING
        cout << "isAlmostSurelyPrime( n = " << n << " ) at trial number" << trial << " with random integer x = " << x << endl ;
        #endif

		if (isProbablyPrime( n, x ) == Primality::Prime)
            return true ;
        else if (isProbablyPrime( n, x ) == Primality::Composite)
			return false ;
        // else it's probably prime.
	}

	// Almost surely prime because it passed the probably prime test
	// above many times.
	return true ;
} 



/*=============================================================================
 | 
 | NAME
 | 
 |     skip_test
 | 
 | DESCRIPTION
 | 
 |      Make the test p  | (p - 1).
 |                     i
 | 
 | INPUT
 | 
 |     i      (int)       ith prime divisor of r.
 | 
 |     primes (bigint *)  Array of distict prime factors of r.  primes[ i ] = p
 |                                                                             i
 |     p      (int)        p >= 2.
 | 
 | RETURNS
 | 
 |     true    if the test succeeds for some i.
 |     false   otherwise
 | 
 | EXAMPLE
 | 
 |     Suppose i = 0, primes[ 0 ] = 2 and p = 5.  Return true since 2 | 5-1.
 | 
 | METHOD
 | 
 |     Test if (p - 1) mod p = 0 for all i.
 |                          i
 +============================================================================*/

template <typename IntType>
bool Factorization<IntType>::skip_test( ppuint p, int i ) const
{

// p  cannot divide the smaller number (p-1)
//  i
if ( static_cast<IntType>( p-1 ) < static_cast<IntType>( factor_[ i ].prime_ ) )
    return false ;
else
    return( 
        (static_cast<IntType>(p - 1) % static_cast<IntType>( factor_[ i ].prime_ ) == 
         static_cast<IntType>( 0u )) 
        ? true : false ) ;

} // ====================== end of function skip_test =======================



/*=============================================================================
 |
 | NAME
 |
 |    PollardRho
 |
 | DESCRIPTION
 |
 |    Factor an integer using Pollard's rho method as modified by Brent and
 |    detailed in
 |
 |        D. E. Knuth, THE ART OF COMPUTER PROGRAMMING, VOL. 2, 3rd ed.,
 |        Addison-Wesley, 1981, pgs. 250-265.
 |
 |        Errata for Volume 2:
 |        http://www-cs-faculty.stanford.edu/~knuth/taocp.html
 | 
 +============================================================================*/
 
template <typename IntType>
bool Factorization<IntType>::PollardRho( const IntType & c )
{
    IntType x  = static_cast<IntType>( 5u ) ;
    IntType xp = static_cast<IntType>( 2u ) ;
    IntType k  = static_cast<IntType>( 1u ) ;
    IntType l  = static_cast<IntType>( 1u ) ;
    IntType g  = static_cast<IntType>( 0u ) ;

    bool status = true ;

    #ifdef DEBUG_PP_FACTOR
	cout << "Pollard rho n = " << n_ << " c = " << c << endl ;
    #endif

    // Seed 1 as a factor and early out if n == 1.
    PrimeFactor<IntType>f( 1u, 1u ) ;
    factor_.push_back( f ) ;
    numFactors_ = 1u ;

    if (n_ == static_cast<IntType>( 1u ))
        return status ;

    while( true )
    {
        if (isAlmostSurelyPrime( n_ ))
        {
            #ifdef DEBUG_PP_FACTOR
            cout << "    >>>>>prime factor n_ = " << n_ << endl ;
            #endif

            PrimeFactor<IntType>f( n_, 1u ) ;
            factor_.push_back( f ) ;
            ++numFactors_ ;

            status = true ;
            ++statistics_.numPrimalityTests ;
            goto Exit ;
        }

        while (true)
        {
            // TODO:   We can speed up by not checking gcd when k > l/2
            // if (k > l/2u)
            //     continue ;
            // TODO:  We can speed up by accumulating gcd products.
            IntType absDiff = (xp > x) ? (xp - x) : (x - xp) ;
            g = gcd( absDiff, n_ ) ;
            ++statistics_.numGCDs ;

            #ifdef DEBUG_PP_FACTOR
            cout << "    inner while loop, gcd = g = " << g << " |x -xp| = " << absDiff << " n_ = " << n_
                 << " k = " << k << " l = " << l << endl ;
            #endif

            if  (g == static_cast<IntType>( 1u )) 
            {
                --k ;
                if (k == static_cast<IntType>( 0u ))
                {
                    xp = x ;
                    l = l * static_cast<IntType>( 2u ) ;
                    k = l ;
                }
                x = (x * x + c) % n_ ;
                ++statistics_.numSquarings ;
            } 
            else if (g == n_)
            {
                #ifdef DEBUG_PP_FACTOR
                cout << "    failure:  g equals non-prime n_ = " << n_ << endl ;
                #endif

                status = false ;
                goto Exit ;
            }
            else if (isAlmostSurelyPrime( g ))
            {
                #ifdef DEBUG_PP_FACTOR
                cout << "    >>>>>prime factor g = " << g << endl ;
                #endif

                PrimeFactor<IntType> f( g, 1u ) ;
                factor_.push_back( f ) ;
                ++numFactors_ ;

                ++statistics_.numPrimalityTests ;
            }
            else
            {
                #ifdef DEBUG_PP_FACTOR
                cout << "    g = " << g << " isn't prime " << endl ;
                #endif

                status = false ;
                goto Exit ;
            }

            n_ /= g ;
            x = x % n_ ;
            xp = xp % n_ ;

            #ifdef DEBUG_PP_FACTOR
            cout << "    after division, n_ = " << n_ << " x = " << x << " xp = " << xp << endl ;
            #endif

            break ;
        } // end gcd loop
    }


Exit:	
    return status ;
}

/*==============================================================================
|                     Forced Template Instantiations                           |
==============================================================================*/

// C++ doesn't automatically generate templated classes or functions until they
// are first used.  So depending on the order of compilation, the linker may say
// the templated functions are undefined.
//
// Therefore, explicitly instantiate the templates here:

template Factorization<ppuint>::Factorization( const ppuint, FactoringAlgorithm, ppuint p, ppuint m ) ;
template Factorization<BigInt>::Factorization( const BigInt, FactoringAlgorithm, ppuint p, ppuint m ) ;

template Factorization<ppuint>::~Factorization() ;
template Factorization<BigInt>::~Factorization() ;

template Factorization<ppuint>::Factorization( const Factorization<ppuint> & ) ;
template Factorization<BigInt>::Factorization( const Factorization<BigInt> & ) ;
		
template Factorization<ppuint> & Factorization<ppuint>::operator=( const Factorization<ppuint> & ) ;
template Factorization<BigInt> & Factorization<BigInt>::operator=( const Factorization<BigInt> & ) ;

// TODO factorTable only uses BigInt type:
template bool Factorization<ppuint>::factorTable( ppuint, ppuint ) ;
template bool Factorization<BigInt>::factorTable( ppuint, ppuint ) ;

template void Factorization<ppuint>::trialDivision() ;
template void Factorization<BigInt>::trialDivision() ;

template bool Factorization<ppuint>::PollardRho( const ppuint & ) ;
template bool Factorization<BigInt>::PollardRho( const BigInt & ) ;

template ppuint Factorization<ppuint>::num_distinct_factors() const ;
template ppuint Factorization<BigInt>::num_distinct_factors() const ;

template PrimeFactor<ppuint> & Factorization<ppuint>::operator[]( int ) ;
template PrimeFactor<BigInt> & Factorization<BigInt>::operator[]( int ) ;

template ppuint Factorization<ppuint>::prime_factor( int ) const ;
template BigInt Factorization<BigInt>::prime_factor( int ) const ;

template int Factorization<ppuint>::multiplicity( int ) const  ;
template int Factorization<BigInt>::multiplicity( int ) const  ;

template Primality isProbablyPrime( const ppuint &, const ppuint & ) ;
template Primality isProbablyPrime( const BigInt &, const BigInt & ) ;

template bool isAlmostSurelyPrime( const ppuint & ) ;
template bool isAlmostSurelyPrime( const BigInt & ) ;

template bool Factorization<int>::skip_test( ppuint, int ) const ;
template bool Factorization<BigInt>::skip_test( ppuint, int ) const ;

template vector<ppuint>Factorization<ppuint>::getDistinctPrimeFactors() ;
template vector<BigInt>Factorization<BigInt>::getDistinctPrimeFactors()  ;
