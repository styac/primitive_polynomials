/*==============================================================================
| 
|  NAME     
|
|     ppFactor.h
|
|  DESCRIPTION   
|
|     Header for integer factoring classes.
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

// Wrap this header file to prevent duplication if it is included
// accidentally more than once.
#ifndef __PPFACTOR_H__
#define __PPFACTOR_H__


/*=============================================================================
|
| NAME
|
|     FactorError           General factor error, including internal memory errors.
|     FactorRangeError      Input range error.
|
| DESCRIPTION
|
|     Exception classes for the Factor class
|     derived from the STL exception class runtime_error.
|
+============================================================================*/

class FactorError : public runtime_error
{
    public:
        // Throw with an error message.
        FactorError( const string & description )
			: runtime_error( description )
        {
        } ;

        // Default throw with no error message.
        FactorError()
			: runtime_error( "Factor error:  " )
        {
        } ;

} ; // end class BigIntMathError



class FactorRangeError : public FactorError
{
    public:
        // Throw with an error message.
        FactorRangeError( const string & description )
			: FactorError( description )
        {
        } ;

        // Default throw with no error message.
        FactorRangeError()
			: FactorError( "Factor range error:  " )
        {
        } ;

} ; // end class FactorRangeError



/*=============================================================================
 |
 | NAME
 |
 |     PrimeFactor
 |
 | DESCRIPTION
 |
 |     Class to neatly package up unique prime factors to powers.
 |
 +============================================================================*/

template <typename IntType>
class PrimeFactor
{
    public:
        PrimeFactor( IntType prime = 0, int count = 0 )
            : prime_( prime )
            , count_( count )
        {
        } ;

        ~PrimeFactor()
        {
        } ;

        PrimeFactor( const PrimeFactor & factor )
            : prime_( factor.prime_ )
            , count_( factor.count_ )
        {
        }

        PrimeFactor & operator=( const PrimeFactor & factor )
        {
            // Check for assigning to oneself:  just pass back a reference to the unchanged object.
            if (this == &factor)
                return *this ;

            prime_ = factor.prime_ ;
            count_ = factor.count_ ;
			
			return *this ;
        } ;
    
        // Print function for a factor.
        friend ostream & operator<<( ostream & out, const PrimeFactor & factor )
        {
            out << factor.prime_ << " ^ " << factor.count_ << " " ;
        
            return out ;
        }

    // Allow direct access to this simple data type for convenience.
    public:
        IntType prime_ ;
        int     count_ ;
} ;

/*=============================================================================
 |
 | NAME
 |
 |     CompareFactor
 |
 | DESCRIPTION
 |
 |     Class to sort unique prime factors to powers into order by prime size.
 |
 +============================================================================*/

template <typename IntType>
class CompareFactor
{
    public:
        bool operator()( const PrimeFactor<IntType> & s1, const PrimeFactor<IntType> & s2 )
        {
            return s1.prime_ < s2.prime_ ;
        }
} ;



/*=============================================================================
 |
 | NAME
 |
 |     Unit
 |
 | DESCRIPTION
 |                                                  0     e
 |     Class to check for unit factors of the form p  or 1  = 1
 |
 +============================================================================*/

template <typename IntType>
class Unit
{
    public:
        bool operator()( const PrimeFactor<IntType> & s )
        {
            return s.count_ == 0u || s.prime_ == static_cast<IntType>( 1u ) ;
        }
} ;



/*=============================================================================
|
| NAME
|
|     Factor
|
| DESCRIPTION
|
|     Class for single and multiprecision factoring.
|
|     BigInt n ;
|
|     Factor fact( n )        Factor n into primes.
|     int numDistinctFactors = fact.num() ;
|     BigInt primeFactor = fact[ i ] ;
|
| NOTES
|
|     The member functions and friends are documented in detail ppBigInt.cpp
|
+============================================================================*/

// Different flavors of factoring algorithm.
enum class FactoringAlgorithm
{ 
    Automatic,
    TrialDivisionAlgorithm,
    PollardRhoAlgorithm,
    FactorTable
} ;


// We need both single precision and multi-precision factoring, so use
// a template with an integer type.
template <typename IntType>
class Factorization
{
    public:
		// Factor n into distinct primes.  Default constructor factors nothing.
        Factorization( const IntType num = 1u, FactoringAlgorithm type = FactoringAlgorithm::Automatic, ppuint p = 0, ppuint m = 0 ) ;

       ~Factorization() ;
	   
        // Copy constructor.
        Factorization( const Factorization<IntType> & f ) ;
		
        // Assignment operator.
        Factorization<IntType> & operator=( const Factorization<IntType> & g ) ;

	    // Return the number of distinct factors.
		ppuint num_distinct_factors() const ;

        // Return the ith prime factor along with its multiplicity as an lvalue so we can change either.
		PrimeFactor<IntType> & operator[]( int i ) ;
    
        // Return the ith prime factor.
        IntType prime_factor( int i ) const ;

        // Return the multiplicity of the ith prime factor.
        int multiplicity( int i ) const ;
        
        // True if p  | (p - 1).
        //          i
        bool skip_test( ppuint p, int i ) const ;

        // Factoring with tables.
        bool factorTable( ppuint p, ppuint n ) ;

        //                                     ---
        // Factoring by trial division up to \/ n
        void trialDivision() ;

        // Fast probabilistic factoring method.
        bool PollardRho( const IntType & c = static_cast<IntType>( 2u )) ;

        // Return a vector of only the distinct prime factors.
        vector<IntType> getDistinctPrimeFactors() ;
        
    // Allow direct access to this simple data type for convenience.
    public:
        OperationCount statistics_ ;

    private:
        // The unfactored remainder.
        IntType n_ ;

		// Total number of distinct factors.
		ppuint             numFactors_ ;

        // Array of distinct prime factors of n with their multiplicities.
		vector< PrimeFactor<IntType> > factor_ ;

        // Distinct prime factors.
        vector<IntType> distinctPrimeFactors_ ;
} ;



/*=============================================================================
 |
 | NAME
 |
 |    factorRAndFindNumberOfPrimitivePolynomials
 |
 | DESCRIPTION
 |
 |    This is the most time consuming step for large n.
 |                                             n
 |    Find the maximum number of polynomials, p
 |
 |    Compute
 |              n
 |             p  - 1
 |        r = --------,  and factor r into the product of primes.
 |             p - 1
 |                                                  n
 |     Find number of primitive polynomials = Phi( p - 1 ) / n
 |
+============================================================================*/

void factorRAndFindNumberOfPrimitivePolynomials( ppuint p, int n, 
         BigInt & maxNumPossiblePoly, BigInt & r, Factorization<BigInt> & factorsOfR, BigInt & numPrimitivePoly ) ;

/*=============================================================================
|
| NAME
|
|     Primality
|
| DESCRIPTION
|
|     What confidence a number is prime?
|
+============================================================================*/

enum class Primality
{
    Prime = 0,
    Composite,
    ProbablyPrime, 
    Undefined

}  ;


/*=============================================================================
 |
 | NAME
 |
 |     isProbablyPrime
 |
 | DESCRIPTION
 |
 |     True if n is likely to be prime.  Tests on a random integer x.
 |
 +============================================================================*/

template <typename IntType>
Primality isProbablyPrime( const IntType & n, const IntType & x ) ;




/*=============================================================================
|
| NAME
|
|     isAlmostSurelyPrime
|
| DESCRIPTION
|
|     True if n is probabilistically prime.
|
+============================================================================*/

template <typename IntType>
bool isAlmostSurelyPrime( const IntType & n ) ;

#endif // __PPFACTOR_H__
