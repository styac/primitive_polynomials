/*==============================================================================
| 
|  NAME
|
|     ppOperationCount.cpp
|
|  DESCRIPTION
|
|     Collect operation counts for the primitive polynomial algorithm:
|     number of iterations for prime factoring, number of polynomials free of
|     linear factors, and whatnot.
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
|                                Includes                                      |
------------------------------------------------------------------------------*/

#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <new>          // set_new_handler()
#include <cmath>        // Basic math functions e.g. sqrt()
#include <limits>       // Numeric limits.
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;   // So we don't need to say std::vector everywhere.



/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.h"       // Global functions.
#include "ppArith.h"        // Basic arithmetic functions.
#include "ppBigInt.h"       // Arbitrary precision integer arithmetic.
#include "ppOperationCount.h"   // OperationCount collection for factoring and poly finding.
#include "ppFactor.h"       // Prime factorization and Euler Phi.
#include "ppPolynomial.h"   // Polynomial operations and mod polynomial operations.
#include "ppParser.h"       // Parsing of polynomials and I/O services.
#include "ppUnitTest.h"     // Complete unit test.


/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Default constructor.
 |
 +============================================================================*/

OperationCount::OperationCount()
    : n( 0u )
    , p( 0u )
    , maxNumPossiblePoly( 0u )
    , numPrimitivePoly( 0u )
    , numPolyTested( 0u )
    , numGCDs( 0u )
    , numPrimalityTests( 0u )
    , numSquarings( 0u )
    , numTrialDivides( 0u )
    , numFreeOfLinearFactors( 0u )
    , numConstantCoeffIsPrimitiveRoot( 0u )
    , numPassingConstantCoeffTest( 0u )
    , numIrreducibleToPower( 0u )
    , numOrderM( 0u )
    , numOrderR( 0u )
{
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     No special destruction needed.
 |
 +============================================================================*/

OperationCount::~OperationCount()
{
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Copy constructor.
 |
 +============================================================================*/

OperationCount::OperationCount( const OperationCount & statistics )
           :n( statistics.n )
           ,p( statistics.p )
           ,maxNumPossiblePoly( statistics.maxNumPossiblePoly )
           ,numPrimitivePoly( statistics.numPrimitivePoly )
           ,numPolyTested( statistics.numPolyTested )
           ,numGCDs( statistics.numGCDs )
           ,numPrimalityTests( statistics.numPrimalityTests )
           ,numSquarings( statistics.numSquarings )
           ,numTrialDivides( statistics.numTrialDivides )
           ,numFreeOfLinearFactors( statistics.numFreeOfLinearFactors )
           ,numConstantCoeffIsPrimitiveRoot( statistics.numConstantCoeffIsPrimitiveRoot )
           ,numPassingConstantCoeffTest( statistics.numPassingConstantCoeffTest )
           ,numIrreducibleToPower( statistics.numIrreducibleToPower )
           ,numOrderM( statistics.numOrderM )
           ,numOrderR( statistics.numOrderR )

{
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Assignment.
 |
 +============================================================================*/

OperationCount & OperationCount::operator=( const OperationCount & statistics )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &statistics)
        return *this ;

    n                            = statistics.n ;
    p                            = statistics.p ;

    maxNumPossiblePoly           = statistics.maxNumPossiblePoly ;
    numPrimitivePoly             = statistics.numPrimitivePoly ;

    numPolyTested                = statistics.numPolyTested ;
    numGCDs                      = statistics.numGCDs ;
    numPrimalityTests            = statistics.numPrimalityTests ;
    numSquarings                 = statistics.numSquarings ;
    numTrialDivides              = statistics.numTrialDivides ;
    numFreeOfLinearFactors       = statistics.numFreeOfLinearFactors ;
    numConstantCoeffIsPrimitiveRoot = statistics.numConstantCoeffIsPrimitiveRoot ;
    numPassingConstantCoeffTest  = statistics.numPassingConstantCoeffTest ;
    numIrreducibleToPower        = statistics.numIrreducibleToPower ;
    numOrderM                    = statistics.numOrderM ;
    numOrderR                    = statistics.numOrderR ;

    return *this ;
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Print out a report of the operation count to the console.
 |
 +============================================================================*/

ostream & operator<<( ostream & out, const OperationCount & op )
{
    out << "+--------- OperationCount --------------------------------\n" ;
    out << "|\n" ;
    out << "| Integer factorization:  Table lookup + Trial division + Pollard Rho\n" ;
    out << "|\n" ;
    out << "| Number of trial divisions :           " << op.numTrialDivides << endl ;
    out << "| Number of gcd's computed :            " << op.numGCDs << endl ;
    out << "| Number of primality tests :           " << op.numPrimalityTests << endl ;
    out << "| Number of squarings:                  " << op.numSquarings << endl ;
    out << "|\n" ;
    out << "| Polynomial Testing\n" ;
    out << "|\n" ;
    out << "| Total num. degree " << op.n << " poly mod " << op.p << " :      " << op.maxNumPossiblePoly << endl ;
    out << "| Number of possible primitive poly:    " << op.numPrimitivePoly << endl ;
    out << "| Polynomials tested :                  " << op.numPolyTested << endl ;
    out << "| Const. coeff. was primitive root :    " << op.numConstantCoeffIsPrimitiveRoot << endl ;
    out << "| Free of linear factors :              " << op.numFreeOfLinearFactors << endl ;
    out << "| Irreducible to power >=1 :            " << op.numIrreducibleToPower << endl ;
    out << "| Had order r (x^r = integer) :         " << op.numOrderR << endl ;
    out << "| Passed const. coeff. test :           " << op.numPassingConstantCoeffTest << endl ;
    out << "| Had order m (x^m != integer) :        " << op.numOrderM << endl ;
    out << "|\n" ;
    out << "+-----------------------------------------------------\n" ;
    
    return out ;
}
