/*==============================================================================
|
|  NAME
|
|     Primpoly.cpp
|
|  DESCRIPTION
|
|     Program for finding primitive polynomials of degree n modulo p for
|     any prime p >= 2 and any n >= 2.
|
|     Useful for generating PN sequences and finite fields for error control coding.
|
|     Please see the user manual and complete technical documentation at
|     http://seanerikoconnor.freeservers.com/Mathematics/AbstractAlgebra/PrimitivePolynomials/overview.html
|
|     main() is called by the OS.
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
#include <limits>       // Numeric limits.
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;   // I don't want to use the std:: prefix everywhere.

#include "ctype.h"      // C string functions.



/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.h"         // Global functions.
#include "ppArith.h"          // Basic arithmetic functions.
#include "ppBigInt.h"         // Arbitrary precision integer arithmetic.
#include "ppOperationCount.h" // OperationCount collection for factoring and poly finding.
#include "ppFactor.h"         // Prime factorization and Euler Phi.
#include "ppPolynomial.h"     // Polynomial operations and mod polynomial operations.
#include "ppParser.h"         // Parsing of polynomials and I/O services.
#include "ppUnitTest.h"       // Complete unit test.



/*=============================================================================
|
| NAME
|
|     main
|
| DESCRIPTION
|
|    Program for finding primitive polynomials of degree n modulo p for
|    any prime p >= 2 and any n >= 2.
|
|    Useful for generating PN sequences and finite fields for error control coding.
|
|    Run the program by typing
|
|        $ Primpoly p n
|
|    You will get an nth degree primitive polynomial modulo p.
|
|    Please see the user manual and complete technical documentation at
|    http://seanerikoconnor.freeservers.com/Mathematics/AbstractAlgebra/PrimitivePolynomials/overview.html
|
|    The author's address is seanerikoconnor!AT!gmail!DOT!com
|    with the !DOT! replaced by . and the !AT! replaced by @
|
+============================================================================*/

int main( int argc, const char * argv[] )
{
    try
    {
        //  Show the legal notice first.
        cout << legalNotice ;

        #ifdef SELF_CHECK
        //  Do a self check first, always.
        bool unitTestStatus = unitTest() ;
        if (!unitTestStatus)
            throw PrimpolyError( "Self-check failed!" ) ;
        else
            cout << "Self-check passes..." << endl ;
        #endif

        // Read the parameters p and n from the command line.  Throw a parsing exception if there is a problem.
        PolyParser<PolySymbol, PolyValue> parser ;
        parser.parseCommandLine( argc, argv ) ;

        // Did user ask for help?
        if (parser.printHelp_)
        {
            cout << helpText ;
            return static_cast<int>( ReturnStatus::AskForHelp ) ;
        }

        // The user input a polynomial.  Test it for primitivity.
        if (parser.testPolynomialForPrimitivity_)
        {
            // Test for primitivity with the quick test.
            Polynomial f( parser.testPolynomial_ ) ;
            PolyOrder order( f ) ;
            cout << f << " is " << (order.isPrimitive() ? "" : "NOT") << " primitive!" << endl ;

            if (parser.printOperationCount_)
                cout << order.statistics_ << endl ;

            // Do a very slow maximal order test for primitivity, if asked to do so.
            if (parser.slowConfirm_)
            {
                cout << confirmWarning ;
                cout << " confirmed " << (order.maximal_order() ? "" : "NOT") << " primitive!" << endl ;
            }
        }
        else
        {
            //  Find a primitive polynomial.
            Polynomial f = findPrimitivePolynomial( parser.p, parser.n,
                                                    parser.printOperationCount_, parser.listAllPrimitivePolynomials_, parser.slowConfirm_ ) ;
        }

        return static_cast<int>( ReturnStatus::Success ) ;
    }
    // Catch all exceptions and report what happened to the user.
    // First do the user-defined exceptions.
    catch( PrimpolyError & e )
    {
        cerr << "\nError: " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( ParserError & e )
    {
        cerr << "Inputs are incorrect or out of range: " << e.what() << endl << helpText ;
        return static_cast<int>( ReturnStatus::RangeError ) ;
    }
    catch( FactorError & e )
    {
        cerr << "Error in the prime Factorization tables:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( BigIntRangeError & e )
    {
        cerr << "Internal range error in multiple precision arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( BigIntDomainError & e )
    {
        cerr << "Internal domain error in multiple precision arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( BigIntUnderflow & e )
    {
        cerr << "Internal underflow error in multiple precision arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( BigIntOverflow & e )
    {
        cerr << "Internal overflow error in multiple precision arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( BigIntZeroDivide & e )
    {
        cerr << "Internal zero divide error in multiple precision arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( BigIntMathError & e )
    {
        cerr << "Internal math error in multiple precision arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch ( ArithModPException & e )
    {
        cerr << "Internal modulo p arithmetic error:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch (PolynomialRangeError & e)
    {
        cout << "Error.  Polynomial has bad syntax or coefficients are out of range. " << e.what() << endl << helpText ;
        return static_cast<int>( ReturnStatus::RangeError ) ;
    }
    catch ( PolynomialError & e )
    {
        cerr << "Error in polynomial arithmetic:  " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    //
    //  Standard library exceptions.
    //
    catch ( bad_alloc & e )
    {
        cerr << "Error allocating memory:  " << e.what() << endl ;
        cerr << "Try again with smaller p and n or try on a computer with more RAM or virtual memory." << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( exception & e )
    {
        cerr << "Standard library error: " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    // Catch all other uncaught exceptions, which would otherwise call terminate()
    // which in turn calls abort() and which would halt this program.
    //
    // Limitations:
    //     We can't handle the case where terminate() gets called because the
    //     exception mechanism finds a corrupt stack or catches a destructor
    //     throwing an exception.
    // 
    //     Also we can't catch exceptions which are thrown by initializing or
    //     destructing global variables.
    catch( ... )
    {
        cerr << "Unexpected exception: " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }

return static_cast<int>( ReturnStatus::Success ) ;

} //========================== end of function main ========================
