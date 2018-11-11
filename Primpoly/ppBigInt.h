/*==============================================================================
|
|  NAME
|
|      ppBigInt.h
|
|  DESCRIPTION
|
|      Header for non-negative multiple precision integer arithmetic classes.
|
|      User manual and technical documentation are described in detail in my web page at
|      http://seanerikoconnor.freeservers.com/Mathematics/AbstractAlgebra/PrimitivePolynomials/overview.html
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
#ifndef __PP_BIGINT_H__
#define __PP_BIGINT_H__


/*=============================================================================
|
| NAME
|
|     BigIntMathError       General math error, including internal memory errors.
|     BigIntUnderflow       Underflow.
|     BigIntOverflow        Overflow.
|     BigIntZeroDivide      Zero divide.
|     BigIntRangeError      Input range error.
|     BigIntDomainError     Input domain error, i.e. log(-1).
|
| DESCRIPTION
|
|     Exception classes for the BigInt class
|     derived from the STL exception class runtime_error.
|
+============================================================================*/

class BigIntMathError : public runtime_error
{
    public:
        // Throw with an error message.
        BigIntMathError( const string & description )
			: runtime_error( description )
        {
        } ;

        // Default throw with no error message.
        BigIntMathError()
			: runtime_error( "BigInt math error:  " )
        {
        } ;

} ; // end class BigIntMathError



class BigIntRangeError : public BigIntMathError
{
    public:
        // Throw with an error message.
        BigIntRangeError( const string & description )
			: BigIntMathError( description )
        {
        } ;

        // Default throw with no error message.
        BigIntRangeError()
			: BigIntMathError( "BigInt range error:  " )
        {
        } ;

} ; // end class BigIntRangeError



class BigIntDomainError : public BigIntMathError
{
    public:
        // Throw with an error message.
        BigIntDomainError( const string & description )
			: BigIntMathError( description )
        {
        } ;

        // Default throw with no error message.
        BigIntDomainError()
			: BigIntMathError( "BigInt domain error:  " )
        {
        } ;

} ; // end class BigIntDomainError



class BigIntOverflow : public BigIntMathError
{
      public:
          BigIntOverflow( const string & description )
              : BigIntMathError( description )
          {
          } ;

          BigIntOverflow()
              : BigIntMathError( "BigInt overflow. " )
          {
          }
} ;



class BigIntUnderflow : public BigIntMathError
{
      public:
          BigIntUnderflow( const string & description )
              : BigIntMathError( description )
          {
          } ;

          BigIntUnderflow()
              : BigIntMathError( "BigInt underflow. " )
          {
          }
} ;



class BigIntZeroDivide : public BigIntMathError
{
      public:
          BigIntZeroDivide( const string & description )
              : BigIntMathError( description )
          {
          } ;

          BigIntZeroDivide()
              : BigIntMathError( "BigInt zero divide. " )
          {
          }
} ;



/*=============================================================================
|
| NAME
|
|     BigInt
|
| DESCRIPTION
|
|     Class for multiprecision integer arithmetic.
|
|     We throw one of the following exceptions,
|
|     BigIntMathError       General math error.
|     BigIntRangeError      Range error on input.
|     BigIntUnderflow       Underflow.
|     BigIntOverflow        Overflow.
|     BigIntZeroDivide      Zero divide.
|
| NOTES
|
|     The member functions and friends are documented in detail ppBigInt.cpp
|
+============================================================================*/

class BigInt
{
    public:
        //-----------------< Basic mathematical operations >------------------

        // *** Constructors.

		// Default constructor.  
        // e.g. BigInt u ;
        BigInt() ;

        // Destructor.
        ~BigInt() ;

        // Constructor from unsigned integer.
        // e.g. ppuint u ;  BigInt w( u ) ;
        BigInt( const ppuint d ) ;

        // Constructor from decimal number string.
        // e.g. string s = "314159" ;  BigInt w( s ) ;
        BigInt( const string & s ) ;

        // Copy constructor.
        // e.g. BigInt u ;  BigInt v( u ) ; 
        BigInt( const BigInt & u ) ;

        // Assignment.
        // e.g. BigInt u( "123" ) ;  BigInt v ;  v = u ;
        BigInt & operator=( const BigInt & n ) ;

		// Conversion to int.
        // e.g. BigInt( w ) = "123" ;  ppuint u = static_cast<int>( w ) ;
        operator ppuint() const ;


        // *** Input and output operators.
        
        // e.g. ostringstream os ;  os << u ;
        friend ostream & operator<<( ostream & out, const BigInt & u ) ;

        // e.g. istringstream is ;  is >> u ;
        friend istream & operator>>( istream & in,        BigInt & u ) ;

        // u + v and other additions.
        friend const BigInt operator+( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator+( const BigInt & u, const ppuint d ) ;

        BigInt & operator+=( const BigInt & u ) ;

        BigInt & operator+=( const ppuint d ) ;

        // ++u and other increments.
        BigInt & operator++() ;

        const BigInt operator++( int ) ;

        // u - v and other subtractions.
        friend const BigInt operator-( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator-( const BigInt & u, const ppuint d ) ;

        BigInt & operator-=( const BigInt & u ) ;

        BigInt & operator-=( const ppuint d ) ;

        // --u and other decrements
        BigInt & operator--() ;

        BigInt operator--( int ) ;

        // u * v, and other multiplies.
        friend const BigInt operator*( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator*( const BigInt & u, const ppuint d ) ;

        BigInt & operator*=( const BigInt & u ) ;

        BigInt & operator*=( ppuint d ) ;

        // | u / v |, and other integer divides.
        // --     --
        friend const BigInt operator/( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator/( const BigInt & u, const ppuint d ) ;

        BigInt & operator/=( const BigInt & u ) ;

        BigInt & operator/=( ppuint d ) ;

        // u % v, and other integer moduli.
        friend BigInt operator%( const BigInt & u, const BigInt & v ) ;

        friend ppuint   operator%( const BigInt & u, const ppuint d ) ;

        // --    --
        // | lg() |
        int ceilLg() ;

        // Comparisions including
        // ==, == d, !=, != d, >, > d,
        // <, < d, >=, >= d, <=, <= d,
        friend bool operator==( const BigInt & u, const BigInt & v ) ;
        friend bool operator==( const BigInt & u, const ppuint d     ) ;
        friend bool operator!=( const BigInt & u, const BigInt & v ) ;
        friend bool operator!=( const BigInt & u, const ppuint d     ) ;
        friend bool operator> ( const BigInt & u, const BigInt & v ) ;
        friend bool operator> ( const BigInt & u, const ppuint d     ) ;
        friend bool operator< ( const BigInt & u, const BigInt & v ) ;
        friend bool operator< ( const BigInt & u, const ppuint d     ) ;
        friend bool operator>=( const BigInt & u, const BigInt & v ) ;
        friend bool operator>=( const BigInt & u, const ppuint d     ) ;
        friend bool operator<=( const BigInt & u, const BigInt & v ) ;
        friend bool operator<=( const BigInt & u, const ppuint d     ) ;

        // Bit masking, testing and setting.
        friend BigInt operator& ( const BigInt & u, const BigInt & v ) ;

        friend BigInt operator<<( const BigInt & u, ppuint n ) ;

        const bool testBit( const int bitNum ) const ;
 
        //-----------------< Helper functions >-------------------------------

        // Conversion to decimal number string.
        // e.g. BigInt u( "123" ) ; string s = u.to_string() ;
        string to_string() const ;
    
        // Helper functions for division and mod.
        friend void divMod( const BigInt & u, const BigInt & v,
                            BigInt &       q, BigInt &       r ) ;

        friend void divMod( const BigInt & u, const ppuint d,
                            BigInt &       q, ppuint & r ) ;

        // Highest bit number in a BigInt, 0 is smallest bit.
        int maxBitNumber() const ;

        // Used by the PolyParser and main for input range checking, so we use a class function
        // not requiring us to instantiate a BigInt object.
        static const ppuint getBase() ;

        //-----------------< Unit Test Functions >----------------------------

        friend const ppuint getDigit( const BigInt & u, const int n ) ;

        friend const int getNumDigits( const BigInt & u ) ;

        friend void setBase( const BigInt & u, const ppuint base ) ;

        friend void printNumber( const BigInt & u, ostream & out ) ;
    
        friend void printNumber( const BigInt & u ) ;

    // Class variables shared among all classes.
    // We use static functions instead of static variables to work 
    // around the C++ static member initialization order problem.
    private:
        // Base of the number system (a power of 2) and
        // corresponding number of bits per digit.
        static ppuint & base_() ;

        // Pointer to number system base.  Used for unit testing only.
        static ppuint * pbase ;

        static int & numBitsPerDigit_() ;

    // Private data for member functions only.
    private:
		//  Numbers are n-place quantities with base b digits,
        //         2
        //  where b  is guaranteed to fit into a digit and
        //  b is a power of 2.
        //
    	//         (u     . . . u )
		//           n-1         0
		//
		//  For programming ease, digits are stored in a vector of
        //  length n with the least significant digit at digit[ 0 ],
		//
		//  +----+----+----+------+------+
		//  | u  | u  | u  |      | u    |
		//  |  0 |  1 |  2 | ...  |  n-1 |
		//  +----+----+----+------+------+
	   	//
    	vector< ppuint > digit_ ;
} ;



/*=============================================================================
|
| NAME
|
|     power
|
| DESCRIPTION
|
|           n
|     Does p  for low precision p and n and high precision result.
|
+============================================================================*/

//  Don't quite fit into BigInt class.
BigInt power( ppuint p, ppuint n ) ;
    
    
/*=============================================================================
|
| NAME
|
|     testBit
|
| DESCRIPTION
|
|     Bit test for low precision integers.
|
+============================================================================*/
    
const bool testBit( const ppuint n, const int bitNum ) ;

#endif // __PP_BIGINT_H__ --- End of wrapper for header file.
