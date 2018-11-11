/*==============================================================================
|
|  NAME
|
|     ppOperationCount.h
|
|  DESCRIPTION
|
|     Header file for operation counts for the primitive polynomial algorithm.
|     collection classes.
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
#ifndef __PP_STATISTICS_H__
#define __PP_STATISTICS_H__


/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Class for collecting statistics on operation counts for
 |     factoring and polynomial testing.
 |
 +============================================================================*/

class OperationCount
{
    public:
        OperationCount() ;

        ~OperationCount() ;

        OperationCount( const OperationCount & statistics ) ;

        OperationCount & operator=( const OperationCount & statistics ) ;

        friend ostream & operator<<( ostream & , const OperationCount & ) ;

    // Allow direct access to this simple data type for convenience.
    public:
        ppuint n ;                            // Degree of the polynomial.
        ppuint p ;                            // Modulus of the polynomial.

        BigInt maxNumPossiblePoly ;           // Number of possible degree n modulo p polynomials.
        BigInt numPrimitivePoly ;             // Number of primitive degree n modulo p polynomials.
        BigInt numPolyTested ;                // Number of polynomials tested.
        
        BigInt numGCDs ;                      // Number of gcd computations.
        BigInt numPrimalityTests ;            // Number primality tests.
        BigInt numSquarings ;                 // Number of squarings.
        BigInt numTrialDivides ;              // Number of trial divisions.

        BigInt numFreeOfLinearFactors ;       // Number of polynomials which have no linear factors.
        BigInt numConstantCoeffIsPrimitiveRoot ;  // Number of polynomials whose constant is a primitive root of p.
        BigInt numPassingConstantCoeffTest ; // Number of polynomials whose constant term passes a consistency check.
        BigInt numIrreducibleToPower ;       // Number of polynomials which are of the form irreducible poly to a power >= 1.
        BigInt numOrderM ;                   // The number of polynomials which pass the x^m not an integer test.
        BigInt numOrderR ;                   // The number of polynomials which pass the x^r = integer test.
} ;

#endif // __PP_STATISTICS_H__
