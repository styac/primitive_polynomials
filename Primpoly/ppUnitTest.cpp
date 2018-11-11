/*==============================================================================
|
|  NAME
|
|     ppUnitTest.cpp
|
|  DESCRIPTION
|
|     Unit test for exercising all classes and methods.
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
|                                Includes                                      |
------------------------------------------------------------------------------*/

#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <iomanip>      // I/O manipulators.
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
#include <array>        // array type.

using namespace std ;   // So we don't need to say std::vector everywhere.



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

#ifdef SELF_CHECK
#include "ppUnitTest.h"       // Complete unit test.

/*=============================================================================
 |
 | NAME
 |
 |     unitTest
 |
 | DESCRIPTION
 |
 |     In the spirit of EXTREME programming, test each class and its member
 |     functions.  Run the tests every time we run this application.
 |     Place the test results into the current directory in the file
 |     unitTest.txt.  If we can't write the file, log an error message
 |     to the console and skip the tests.
 |
 +============================================================================*/

bool unitTest()
{
    // Place results into a log file in the current directory.
    // If the file can't be opened, just print results to the console.
    const char * unitTestLogFileName = "unitTest.log" ;
    bool status = true ;

    ofstream fout( unitTestLogFileName ) ;

    // Test this section of code by making the existing file read only:
    //     chmod 000 unitTest.log
    if (!fout)
    {
        cerr << "Unit test:  cannot open output log file" << unitTestLogFileName << (string)__FILE__ << endl ;
        cerr << "Trying standard output to the console instead." << endl ;

        fout.open( "/dev/stdout", fstream::out ) ;

        if (fout.fail())
        {
            cerr << "Standard output to console does not work either." << endl ;
            cerr << "Skipping the self check." << endl ;
            return false ;
        }
    }

    fout << legalNotice ;
    fout << "\nBegin unit testing..." ;
    
    try
    {
        array<bool,8> unitTestStatus ;
        unitTestStatus.fill( true ) ;
        int i = 0 ;

        // Go thorough all the unit tests in order.
        unitTestStatus[i++] = unitTestSystemFunctions( fout ) ;
        unitTestStatus[i++] = unitTestBigIntBase10( fout ) ;
        unitTestStatus[i++] = unitTestBigIntDefaultBase( fout ) ;
        unitTestStatus[i++] = unitTestModPArithmetic( fout ) ;
        unitTestStatus[i++] = unitTestFactoring( fout ) ;
        unitTestStatus[i++] = unitTestPolynomials( fout ) ;
        unitTestStatus[i++] = unitTestPolynomialOrder( fout ) ;
        unitTestStatus[i++] = unitTestParser( fout ) ;

        // True only if everyone passes.
        for (i = 0 ;  i < unitTestStatus.size() ;  ++i)
            status = status && unitTestStatus[ i ] ;
        
    // These catch blocks should be in order from more specific to less specific.
    // i.e. up the class hierarchy.
    }
    catch( PrimpolyError & e )
    {
        fout << ".........FAIL!\n    caught exception type  PrimpolyError:  " << e.what() << endl ;
        status = false ;
    }
    catch( ParserError & e )
    {
        fout << ".........FAIL!\n    caught exception type  ParserError:  " << e.what() << endl ;
        status = false ;
    }
    catch( FactorError & e )
    {
        fout << ".........FAIL!" << endl ;
        fout << "    caught FactorError:  " << e.what() << endl ;
        status = false ;
    }
    catch( BigIntRangeError & e )
    {
        fout << ".........FAIL!\n    caught exception type  BigIntRangeError:  " << e.what() << endl ;
        status = false ;
    }
    catch( BigIntDomainError & e )
    {
        fout << ".........FAIL!\n    caught exception type  BigIntDomainError:  " << e.what() << endl ;
        status = false ;
    }
    catch( BigIntUnderflow & e )
    {
        fout << ".........FAIL!\n    caught exception type  BigIntUnderflow:  " << e.what() << endl ;
        status = false ;
    }
    catch( BigIntOverflow & e )
    {
        fout << ".........FAIL!\n    caught exception type  BigIntOverflow:  " << e.what() << endl ;
        status = false ;
    }
    catch( BigIntZeroDivide & e )
    {
        fout << ".........FAIL!\n    caught exception type  BigIntZeroDivide:  " << e.what() << endl ;
        status = false ;
    }
    catch( BigIntMathError & e )
    {
        fout << ".........FAIL!\n    caught exception type  BigIntMathError:  " << e.what() << endl ;
        status = false ;
    }
    catch( ArithModPException & e )
    {
        fout << ".........FAIL!\n    caught exception type  ArithModPException:  " << e.what() << endl ;
        status = false ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << ".........FAIL!\n    caught exception type  PolynomialRangeError:  " << e.what() << endl ;
        status = false ;
    }
    catch( PolynomialError & e )
    {
        fout << ".........FAIL!\n    caught exception type  PolynomialError:  " << e.what() << endl ;
        status = false ;
    }
    //
    // Standard library exceptions.
    //
    catch( bad_alloc & e )
    {
        fout << ".........FAIL!\n    caught exception type  Caught bad_alloc exception:  " << e.what() << endl ;
        status = false ;
    }
    catch( length_error & e )
    {
        fout << ".........FAIL!\n    caught exception type  Caught length_error exception:  " << e.what() << endl ;
        status = false ;
    }
    catch( exception & e )
    {
        fout << ".........FAIL!\n    caught exception type  Standard library error: " << e.what() << endl ;
        status = false ;
    }
    // Catch all other uncaught exceptions, which would otherwise call terminate()
    // which in turn calls abort() and which would halt this program.
    //
    // Limitations:
    // We can't handle the case where terminate() gets called because the
    // exception mechanism finds a corrupt stack or catches a destructor
    // throwing an exception.
    //
    // Also we can't catch exceptions which are thrown by initializing or
    // destructing global variables.
    catch( ... )
    {
        fout << ".........FAIL!\n    caught exception type  uncaught exception" << endl ;
        status = false ;
    }
    
    fout << "\nEnd unit testing..." ;
    
    if (status)
        fout << "\nCONGRATULATIONS!  All tests passed!" << endl ;
    else
        fout << "\nSORRY.  One or more unit tests failed!" << endl ;
    
    fout.close() ;
    
    return status ;
}



bool unitTestSystemFunctions( ostream & fout )
{
    bool status = true ;

    #ifdef DEBUG_PP_FORCE_MEMORY_OVERLOAD
    // Test resize() exceptions.
    fout << "\nTEST:  C++ resize a vector to > max_size dimensions.  Did resize throw a length_error exception?" ;

    //            30
    // Typically 2  - 1.
    vector< ppuint > testVector ;
    size_t maxSize = testVector.max_size() ;

    try
    {
        // Overload the vector.
        testVector.resize( maxSize + 1 ) ;

        fout << ".........FAIL!" << endl ;
        fout << "    resize did not throw an exception " << endl ;
        status = false ;
    }
    catch( length_error & e )
    {
        fout << ".........PASS!" ;
    }
    catch( ... )
    {
        fout << ".........FAIL!" << endl ;
        fout << "    resize did not throw a length_error exception" << endl ;
        status = false ;
    }
    #endif // DEBUG_PP_FORCE_MEMORY_OVERLOAD

    #ifdef DEBUG_PP_FORCE_MEMORY_OVERLOAD
    fout << "\nTEST:  C++ overload memory for a vector.  Did push_back throw an exception?" ;
    try
    {
        // Overload memory.
        for (int i = 1 ;  i <= maxSize + 1 ;  ++i)
            testVector.push_back( 1000 ) ;

        fout << ".........FAIL!" << endl ;
        fout << "    push_back did not throw an exception" << endl ;
        status = false ;
    }
    catch( bad_alloc & e )
    {
        fout << ".........PASS!" ;
    }
    catch( ... )
    {
        fout << ".........FAIL!" << endl ;
        fout << "    push_back did not throw a bad_alloc exception" << endl ;
        status = false ;
    }
    #endif // DEBUG_OVERLOADING_MEMORY

    return status ;
}



bool unitTestBigIntBase10( ostream & fout )
{
    bool status = true ;

    // ------------------- Set to base 10 ----------------------
    ppuint oldBase = 0 ; // Save the old default base.
    {
        // Instantiate a BigInt object and set its to base 10 to change the base for all BigInts.
        BigInt w ;
        oldBase = BigInt::getBase() ;
        setBase( w, 10 ) ;
    }

    #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
    { BigInt dummy ; setBase( dummy, 11 ) ; }
    #endif

    fout << "\nTEST:  BigInt switching from base = " << oldBase << " to new base = " << 10 ;
    {
        // Check all BigInts have the new base of 10.
        if (BigInt::getBase() != 10)
        {
            fout << ".........FAIL!" << endl ;
            fout << "    Current base is not 10 but rather = " << BigInt::getBase() << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt u default constructor which gives u = 0." ;
    {
        BigInt u ;
        if (getNumDigits(u) != 0)
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Constructor BigInt u( d ) from ppuint d = 1234" ;
    {
        ppuint d = 1234 ;
        BigInt u( d ) ;
        if (getNumDigits( u ) == 4 && getDigit( u, 3 ) == 1 && getDigit( u, 2 ) == 2 && getDigit( u, 1 ) == 3 && getDigit( u, 0 ) == 4)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  Constructor BigInt u( s ) from string s = \"1234\"" ;
    {
        string s = "1234" ;
        BigInt u( s ) ;
        if (getNumDigits( u ) == 4 && getDigit( u, 3 ) == 1 && getDigit( u, 2 ) == 2 && getDigit( u, 1 ) == 3 && getDigit( u, 0 ) == 4)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  Constructor BigInt u( s ) from INVALID string s = \"12x34\"" ;
    try
    {
        string s = "12x34" ;
        BigInt u( s ) ;
        fout << ".........FAIL!" << endl ;
        fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
        status = false ;
    }
    catch( BigIntRangeError & e )
    {
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Copy constructor BigInt v( u ) from BigInt u( 123 )" ;
    {
        BigInt u( 123 ) ;
        BigInt v( u ) ;

        if (getNumDigits( u ) != getNumDigits( v ) ||
            getDigit( u, 0 ) != getDigit( v, 0 ) ||
            getDigit( u, 1 ) != getDigit( v, 1 ) ||
            getDigit( u, 2 ) != getDigit( v, 2 ))
        {
            fout << ".........FAIL!" << endl ;
            fout << " u = " ;  printNumber( u, fout ) ; fout << endl ;
            fout << " v = " ;  printNumber( v, fout ) ; fout << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Assignment operator v = u from BigInt v and BigInt u( 123 )" ;
    {
        BigInt u( 123 ) ;
        BigInt v ; // Don't use BigInt v = u because that would call the copy constructor.
        v = u ;

        if (getNumDigits( u ) != getNumDigits( v ) ||
            getDigit( u, 0 ) != getDigit( v, 0 ) ||
            getDigit( u, 1 ) != getDigit( v, 1 ) ||
            getDigit( u, 2 ) != getDigit( v, 2 ))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    v = " ; printNumber( v, fout ) ; fout << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Implicit casting ppuint d = u from BigInt u( \"01234\" )" ;
    {
        BigInt u( "01234" ) ;
        ppuint d = u ;
        if (d != static_cast<ppuint>(1234u))
        {
            fout << ".........FAIL!" << endl ;
            fout << " u = " ;  printNumber( u, fout ) ; fout << endl ;
            fout << " d = "  << d << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;

        fout << "\nTEST:  Explicit casting static_cast< ppuint >( u ) from BigInt u( \"01234\" )" ;
        if (static_cast< ppuint >( u ) != static_cast< ppuint >( 1234u ))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " << endl ; printNumber( u, fout ) ; fout << endl ;
            fout << "    static_cast< ppuint > u  = "  << static_cast< ppuint >( u ) << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Check overflow during ui = static_cast< ppuint >(u) on BigInt u( \"3141592653589793238462643383279\" )" ;
    try
    {
        BigInt u( "3141592653589793238462643383279" ) ;
        ppuint ui = static_cast<ppuint>(u) ;

        fout << ".........FAIL!" << endl ;
        fout << "    Casting BigInt to ppuint didn't overflow." << endl ;
        fout << "    u = " ;  printNumber( u, fout ) ;  fout << endl ;
        fout << "    ui = " << ui << endl ;
        status = false ;
    }
    catch( BigIntOverflow & e )
    {
        // Should overflow!
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Stream output os << u from BigInt u( \"1234567890\" )" ;
    {
        BigInt u( "1234567890" ) ;
        ostringstream os ;
        os << u ;
        if (os.str() != "1234567890")
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = |" << u << "|" << endl ;
            fout << "    os.str() = |" << os.str() << "|" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Stream input is >> u for BigInt u where we've loaded the stream is.str( \"314159265358979323846264\" )" ;
    {
        BigInt u ;
        istringstream is ;
        is.clear() ;
        is.str( "314159265358979323846264" ) ;
        is >> u ;

        // Test by streaming out the BigInt and checking its string value.
        ostringstream os ;
        os << u ;
        if (os.str() != "314159265358979323846264" )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    BigInt = |" << u << "| is.str() = |" << is.str() << "|" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Equality test BigInt u == ppuint d?" ;
    {
        BigInt u( "9" ) ;
        ppuint d = 9 ;
        if (u == d)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  Equality test BigInt u == BigInt v" ;
    {
        BigInt u( "1234" ) ;
        BigInt v( "1234" ) ;
        if (u == v)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    v = " ; printNumber( v, fout ) ; fout << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  BigInt u > BigInt v" ;
    {
        BigInt u( "3844035" ) ;
        BigInt v( "933134" ) ;

        if (u > v)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    v = " ; printNumber( v, fout ) ; fout << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  BigInt u( \"1234\" ) -= ppuint d" ;
    try
    {
        BigInt u( "1234" ) ;
        ppuint d = static_cast<ppuint>(5u) ;
        u -= d ;

        if (getNumDigits( u ) != static_cast<ppuint>(4) ||
            getDigit( u, 3 )  != static_cast<ppuint>(1) || getDigit( u, 2 ) != static_cast<ppuint>(2) ||
            getDigit( u, 1 )  != static_cast<ppuint>(2) || getDigit( u, 0 ) != static_cast<ppuint>(9)
           )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( BigIntMathError & e )
    {
        fout << ".........FAIL!" << endl ;
        fout << "BigIntMathError:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  BigInt u -= static_cast<ppuint>(5) underflow" ;
    {
        try
        {
            BigInt u( "4" ) ;
            ppuint d = static_cast<ppuint>(5u) ;
            u -= d ;

            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
        catch( BigIntUnderflow & e )
        {
           // Caught underflow;  works correctly.
           // fout << "Underflow: " << e.what() << endl ;
            fout << ".........PASS!" ;
        }
        catch( ... )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    Didn't catch a BigIntUnderflow exception" << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  BigInt u += ppuint d" ;
    {
        BigInt u( "9994" ) ;
        ppuint d = static_cast<ppuint>(6u) ;
        u += d ;
        if (getNumDigits( u ) != 5 ||
            getDigit( u, 4 )  != 1 ||
            getDigit( u, 3 )  != 0 || getDigit( u, 2 ) != 0 ||
            getDigit( u, 1 )  != 0 || getDigit( u, 0 ) != 0
           )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt v = BigInt u * ppuint d" ;
    {
        BigInt u( "123" ) ;
        ppuint d = 4 ;
        BigInt v = u * d ;

        if (getNumDigits( v ) != 3 ||
            getDigit( v, 2 ) != 4 ||
            getDigit( v, 1 ) != 9 || getDigit( v, 0 ) != 2
           )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    v = " ; printNumber( v, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt u /= ppuint d" ;
    {
        BigInt u( "12" ) ;
        ppuint d = 4 ;
        u /= d ;

        if (getNumDigits( u ) != 1 || getDigit( u, 0 ) != 3)
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt u /= ppuint d underflow to zero." ;
    {
        BigInt u( "3" ) ;
        ppuint d = 4 ;
        u /= d ;

        if (getNumDigits( u ) != 1 || getDigit( u, 0 ) != 0)
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt v = ++BigInt u" ;
    {
        BigInt u( "123" ) ;
        BigInt v = ++u ;

        if (getNumDigits( u ) != 3 || getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 4
           || (u != v))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    v = " ; printNumber( v, fout ) ; fout << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt v = --BigInt u" ;
    {
        BigInt u( "123" ) ;
        BigInt v = --u ;

        if (getNumDigits( u ) != 3 ||
            getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 2 ||
            (u != v))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    u = " ; printNumber( u, fout ) ; fout << endl ;
            fout << "    v = " ; printNumber( v, fout ) ; fout << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt++" ;
    {
        BigInt u( "123" ) ;
        BigInt v = u++ ;

        if (getNumDigits( u ) != 3 ||
            getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 4
           )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    ++BigInt failed." << endl ;
            printNumber( u, fout ) ;
            status = false ;
        }
        else if (getNumDigits( v ) != 3 ||
            getDigit( v, 2 ) != 1 || getDigit( v, 1 )  != 2 || getDigit( v, 0 ) != 3
           )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    BigInt++ failed." << endl ;
            printNumber( u, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt--" ;
    {
        BigInt u( "123" ) ;
        BigInt v = u-- ;

        if (getNumDigits( u ) != 3 ||
            getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 2
           )
        {
            fout << "\n\tERROR:  BigInt-- failed." << endl ;
            printNumber( u, fout ) ;
            status = false ;
        }
        else if (getNumDigits( v ) != 3 ||
            getDigit( v, 2 ) != 1 || getDigit( v, 1 )  != 2 || getDigit( v, 0 ) != 3
           )
        {
            fout << "\n\tERROR:  BigInt-- failed." << endl ;
            printNumber( u, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  one digit BigInt + ppuint" ;
    {
        BigInt u( "3" ) ;
        ppuint d = 4 ;
        BigInt w = u + d ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 7)
        {
            fout << "\n\tERROR:  BigInt + BigInt 3 + 4 = 7 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  two digit BigInt + ppuint" ;
    {
        BigInt u( "3" ) ;
        ppuint d = 9 ;
        BigInt w = u + d ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 1
                                   || getDigit( w, 0 ) != 2)
        {
            fout << "\n\tERROR:  BigInt + BigInt 3 + 9 = 12 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt + BigInt" ;
    {
        BigInt u( "9999" ) ;
        BigInt v(  "999" ) ;
        BigInt w = u + v ;

        if (getNumDigits( w ) != 5 || getDigit( w, 4 ) != 1
                                   || getDigit( w, 3 ) != 0
                                   || getDigit( w, 2 ) != 9
                                   || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 8)
        {
            fout << "\n\tERROR:  BigInt + BigInt 9999 + 999 = 10998 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt + BigInt" ;
    {
        BigInt u(  "999" ) ;
        BigInt v( "9999" ) ;
        BigInt w = u + v ;

        if (getNumDigits( w ) != 5 || getDigit( w, 4 ) != 1
                                   || getDigit( w, 3 ) != 0
                                   || getDigit( w, 2 ) != 9
                                   || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 8)
        {
            fout << "\n\tERROR:  BigInt + BigInt 999 + 9999 = 10998 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt - BigInt" ;
    {
        BigInt u( "103" ) ;
        BigInt v(   "9" ) ;
        BigInt w = u - v ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 4)
        {
            fout << "\n\tERROR:  BigInt - BigInt 103 - 9 = 94 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt - BigInt < 0" ;
    try
    {
        BigInt u( "9" ) ;
        BigInt v( "103" ) ;
        BigInt w = u - v ;

        fout << "\n\tERROR:  BigInt - BigInt 9 - 103 failed didn't catch range exception." << endl ;
        printNumber( w, fout ) ;
        status = false ;
    }
    catch( BigIntUnderflow & e )
    {
        // Caught underflow;  works correctly.
        // fout << "Underflow: " << e.what() << endl ;
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt - ppuint" ;
    {
        BigInt u( "103" ) ;
        ppuint d = 9 ;
        BigInt w = u - d ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 4)
        {
            fout << "\n\tERROR:  BigInt - ppuint 103 - 9 = 94 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  one digit BigInt * BigInt" ;
    {
        BigInt u( "3" ) ;
        BigInt v( "3" ) ;
        BigInt w = u * v ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 9)
        {
            fout << "\n\tERROR:  BigInt * BigInt 3 * 3 = 9 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  two digit BigInt * BigInt" ;
    {
        BigInt u( "3" ) ;
        BigInt v( "4" ) ;
        BigInt w = u * v ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 1 || getDigit( w, 0 ) != 2)
        {
            fout << "\n\tERROR:  BigInt * BigInt = 3 * 4 = 12 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt multidigit *" ;
    {
        BigInt w ;
        BigInt u( "329218104" ) ;
        BigInt v(      "3606" ) ;

        w = u * v ;

        string s = w.to_string() ;

        if (s != "1187160483024" )
        {
            fout << "\n\tERROR:  BigInt multidigit * failed." << endl ;
            fout << "u = " ; printNumber( u, fout ) ;
            fout << "v = " ; printNumber( v, fout ) ;
            fout << "w = " ; printNumber( w, fout ) ;
            status = false ;
        }
        else
        {
            fout << ".........PASS!" ;
        }
    }

    fout << "\nTEST:  BigInt multidigit *=" ;
    {
        BigInt u( "329218104" ) ;
        BigInt v(      "3606" ) ;

        u *= v ;

        string s = u.to_string() ;

        if (s != "1187160483024" )
        {
            fout << "\n\tERROR:  BigInt multidigit *= failed." << endl ;
            fout << "u = " ; printNumber( u, fout ) ;
            fout << "v = " ; printNumber( v, fout ) ;
            status = false ;
        }
        else
        {
            fout << ".........PASS!" ;
        }
    }

    fout << "\nTEST:  BigInt / BigInt one digit divisor." ;
    {
        BigInt u( "12" ) ;
        BigInt v( "4" ) ;
        BigInt w = u / v ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 3)
        {
            fout << "\n\tERROR:  BigInt / BigInt = 12/4 = 3 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt / BigInt multidigit" ;
    {
        BigInt u( "398765" ) ;
        BigInt v( "3457" ) ;
        BigInt w = u / v ;

        if (getNumDigits( w ) != 3 ||
                getDigit( w, 2 ) != 1 || getDigit( w, 1 ) != 1 || getDigit( w, 0 ) != 5)
        {
            fout << "\n\tERROR:  BigInt / BigInt = 398765/3457 = 215 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt / BigInt leading zero digit." ;
    {
        BigInt u( "120" ) ;
        BigInt v( "40" ) ;
        BigInt w = u / v ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 3)
        {
            fout << "\n\tERROR:  BigInt / BigInt = 120/40 = 3 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt / 0 " ;
    try
    {
        BigInt u( "120" ) ;
        BigInt v( "0" ) ;
        BigInt w = u / v ;

        fout << "\n\tERROR:  BigInt / 0 = 120/0 failed." << endl ;
        status = false ;
    }
    catch( BigIntZeroDivide & e )
    {
        // Should catch zero divide here.
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt % BigInt with u > v" ;
    {
        BigInt u( "398765" ) ;
        BigInt v( "3457" ) ;
        BigInt r = u % v ;

        if (getNumDigits( r ) != 4 ||
                getDigit( r, 3 ) != 1 || getDigit( r, 2 ) != 2 ||
                getDigit( r, 1 ) != 1 || getDigit( r, 0 ) != 0)
        {
            fout << "\n\tERROR:  BigInt % BigInt = 398765 / 3457 = 1210 failed." << endl ;
            printNumber( r, fout ) ;
            status = false ;
        }
        else
        {
            fout << ".........PASS!" ;
        }
    }

    fout << "\nTEST:  BigInt multidigit mod with normalizing constant d = 1" ;
    {
        BigInt w ;
        BigInt u( "1369244731822264511994463394" ) ;
        BigInt v(  "954901783703457032047844259" ) ;

        w = u % v ;

        string s = w.to_string() ;

        if (s != "414342948118807479946619135" )
        {
            fout << "\n\tERROR:  BigInt multidigit mod failed." << endl ;
            fout << "u = " ; printNumber( u, fout ) ;
            fout << "v = " ; printNumber( v, fout ) ;
            fout << "w = " ; printNumber( w, fout ) ;
            status = false ;
        }
        else
        {
            fout << ".........PASS!" ;
        }
    }

    fout << "\nTEST:  BigInt % BigInt with u < v" ;
    {
        BigInt u( "12" ) ;
        BigInt v( "34567" ) ;
        BigInt r = u % v ;

        if (getNumDigits( r ) != 2 ||
            getDigit( r, 1 ) != 1 || getDigit( r, 0 ) != 2)
        {
            fout << "\n\tERROR:  BigInt % BigInt = 12 mod 345 = 12 failed." << endl ;
            printNumber( r, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt % ppuint = 314159 / 9 = 5 with ppuint < base " ;
    {
        BigInt u( "314159" ) ;
        ppuint v = 9u  ;
        BigInt r = u % v ;

        if (getNumDigits( r ) != 1 || getDigit( r, 0 ) != 5)
        {
            fout << "\n\tERROR:  BigInt % ppuint = 314159 / 9 = 5 failed." << endl ;
            printNumber( r, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt % ppuint = 398765 % 3457u with ppuint > base overflow?" ;
    try
    {
        BigInt u( "398765" ) ;
        ppuint v = 3457u  ;
        BigInt r = u % v ;

        fout << "\n\tERROR:  BigInt % ppuint = 398765 % 3457u with ppuint > base didn't overflow." << endl ;
    }
    catch( BigIntOverflow & e )
    {
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt / BigInt low probability if branch." ;
    {
        BigInt u( "4100" ) ;
        BigInt v( "588" ) ;
        BigInt w = u / v ;

        if (w != BigInt("6"))
            fout << "error" << endl;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 6)
        {
            fout << "\n\tERROR:  BigInt / BigInt = 4100/588 = 6 failed." << endl ;
            printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Switching back from base " << 10 << " to oldBase " << oldBase ;
    BigInt dummy ;
    setBase( dummy, oldBase ) ;
    if (BigInt::getBase() != oldBase)
    {
        fout << "\n\tERROR: Changing back to default base for all BigInt objects "
             << "       failed.  base = " << BigInt::getBase() << endl ;
        status = false ;
    }
    else
        fout << ".........PASS!" ;

    return status ;
}



bool unitTestBigIntDefaultBase( ostream & fout )
{
    bool status = true ;

    fout << "\nTEST:  Decimal string to BigInt conversion and back to decimal string using default base." ;
    {
        BigInt x( "3141592653589793238462643383279" ) ;
        string s = x.to_string() ;

        if (s != "3141592653589793238462643383279" )
        {
            fout << "\n\tERROR:  BigInt default base conversion failed." << endl ;
            fout << "x = " << x << " " ; printNumber( x, fout ) ;
            fout << " NOT EQUAL TO s = 3141592653589793238462643383279 (decimal digits)" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt z = x * y then x =? z / y multidigit with default base." ;
    {
        BigInt w ;
        BigInt x( "3141592653589793238462643383279" ) ;
        BigInt y( "2718281828459045" ) ;
        BigInt z ;

        z = x * y ;
        w = z / y ;

        if (w != x)
        {
            fout << "\n\tERROR:  BigInt z = x * y then x =? z / y multidigit with default base failed." << endl ;
            fout << "x = " ; printNumber( x, fout ) ;
            fout << "y = " ; printNumber( y, fout ) ;
            fout << "z = " ; printNumber( z, fout ) ;
            fout << "w = " ; printNumber( w, fout ) ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt testBit" ;
    {
        BigInt u( "31415926535897932" ) ;

        if (u.testBit( 0 ) == false  &&
            u.testBit( 1 ) == false  &&
            u.testBit( 2 ) == true   &&
            u.testBit( 3 ) == true   &&
            u.testBit( 4 ) == false  &&
            u.testBit( 5 ) == false  &&
            u.testBit( 6 ) == true   &&
            u.testBit( 7 ) == false)
        {
            fout << ".........PASS!" ;
        }
        else
        {
            fout << "\n\tERROR:  BigInt testBit failed." << endl ;
            printNumber( u, fout ) ;
            fout << "testBit 0 = " << (u.testBit( 0 ) == true) << endl ;
            fout << "testBit 1 = " << (u.testBit( 1 ) == true) << endl ;
            fout << "testBit 2 = " << (u.testBit( 2 ) == true) << endl ;
            fout << "testBit 3 = " << (u.testBit( 3 ) == true) << endl ;
            fout << "testBit 4 = " << (u.testBit( 4 ) == true) << endl ;
            fout << "testBit 5 = " << (u.testBit( 5 ) == true) << endl ;
            fout << "testBit 6 = " << (u.testBit( 6 ) == true) << endl ;
            fout << "testBit 7 = " << (u.testBit( 7 ) == true) << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  testBit()" ;
    {
        ppuint u = 0x1 | 0x4 | 0x20 ; // Set bits 0, 2, 5

        if (testBit( u, 0 ) == true   &&
            testBit( u, 1 ) == false  &&
            testBit( u, 2 ) == true   &&
            testBit( u, 3 ) == false  &&
            testBit( u, 4 ) == false  &&
            testBit( u, 5 ) == true   &&
            testBit( u, 6 ) == false  &&
            testBit( u, 7 ) == false)
        {
            fout << ".........PASS!" ;
        }
        else
        {
            fout << "\n\tERROR:  ppuint testBit failed for u = " << u << endl ;
            fout << "testBit 0 = " << (testBit( u, 0 ) == true) << endl ;
            fout << "testBit 1 = " << (testBit( u, 1 ) == true) << endl ;
            fout << "testBit 2 = " << (testBit( u, 2 ) == true) << endl ;
            fout << "testBit 3 = " << (testBit( u, 3 ) == true) << endl ;
            fout << "testBit 4 = " << (testBit( u, 4 ) == true) << endl ;
            fout << "testBit 5 = " << (testBit( u, 5 ) == true) << endl ;
            fout << "testBit 6 = " << (testBit( u, 6 ) == true) << endl ;
            fout << "testBit 7 = " << (testBit( u, 7 ) == true) << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  BigInt power( ppuint 2, ppuint 100 )" ;
    {
        ppuint p = 2u  ;
        int n = 100  ;
        BigInt u = power( p, n ) ;
        string s = u.to_string() ;

        BigInt v = static_cast<ppuint>(1u) ;
        for (int i = 1 ; i <= n ; ++i)
            v *= p ;
        string sv = v.to_string() ;

        if (s != sv)
        {
            fout << "\n\tERROR:  BigInt power( 2, 100 ) = " << u << endl ;
            fout << "correct answer = " << v << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt ceilLg( 6 )" ;
    {
        BigInt u = 6 ;

        int ceilingOfLog2 = u.ceilLg() ;
        if (ceilingOfLog2 != 3)
        {
            fout << "\n\tERROR:  BigInt ceilingOfLog2( 6 ) = " << ceilingOfLog2 << endl ;
            fout << "correct answer = 3" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt eval 2 ^ 1198 - 1 = 3 * 366994123 * 16659379034607403556537 * 148296291984475077955727317447564721950969097 * "
            "839804700900123195473468092497901750422530587828620063507554515144683510250490874819119570309824866293030799718783 * "
            "18844604989678054320016126723693071015074748359764319259483333877486701203536294532613478431402128085705057673867712"
            "90423087216156597588216186445958479269565424431335013281" << endl ;
    {
        BigInt largePowerOf2Minus1 = power( 2, 1198 ) - static_cast< BigInt >( 1u ) ;
        /// cout << "largePowerOf2Minus1 = " << largePowerOf2Minus1 << endl ;
        BigInt f1 = BigInt( 3u ) ;
        BigInt f2 = BigInt( "366994123" ) ;
        BigInt f3 = BigInt( "16659379034607403556537" ) ;
        BigInt f4 = BigInt( "148296291984475077955727317447564721950969097" ) ;
        BigInt f5 = BigInt( "839804700900123195473468092497901750422530587828620063507554515144683510250490874819119570309824866293030799718783" ) ;
        BigInt f6 = BigInt( "188446049896780543200161267236930710150747483597643192594833338774867012035362945326134784314021280857050576738677"
                            "1290423087216156597588216186445958479269565424431335013281" ) ;
        BigInt product = f1 * f2 * f3 * f4 * f5 * f6 ;
        bool allFactorsPrime = false ;
        allFactorsPrime = isAlmostSurelyPrime( f1 ) && isAlmostSurelyPrime( f2 ) && isAlmostSurelyPrime( f3 ) && 
                          isAlmostSurelyPrime( f4 ) && isAlmostSurelyPrime( f5 ) && isAlmostSurelyPrime( f6 ) ;
        /// cout << "all factors prime = " << allFactorsPrime << endl ;
        /// cout << "product = " << product << endl ;

        if (product != largePowerOf2Minus1 || !allFactorsPrime)
        {
            fout << "\nERROR:  BigInt eval 2 ^ 1198 - 1 != 3 * 366994123 * 16659379034607403556537 * 148296291984475077955727317447564721950969097 * "
            "839804700900123195473468092497901750422530587828620063507554515144683510250490874819119570309824866293030799718783 * "
            "18844604989678054320016126723693071015074748359764319259483333877486701203536294532613478431402128085705057673867712"
            "90423087216156597588216186445958479269565424431335013281" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    return status ;
}



bool unitTestModPArithmetic( ostream & fout )
{
    bool status = true ;

    fout << "\nTEST:  ModP 10 = 3 (mod 7)" ;
    {
        ModP<ppuint,ppsint> modp( 7 ) ;
        if (modp( 10 ) != 3)
        {
            fout << ".........FAIL!" << endl ;
            fout << "    ModP modp( 7 );  modp( 10 ) = " << modp( 10 ) << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  ModP -10 = 4 (mod 7)" ;
    {
        ModP<ppuint,ppsint> modp( 7 ) ;
        if (modp( -10 ) != 4)
        {
            fout << ".........FAIL!" << endl ;
            fout << "    ModP modp( 7 );  modp( -10 ) = " << modp( -10 ) << endl ;
            ppsint n = -10 ;
            ppsint p = 7 ;
            fout << "+ + + + + + + +" << endl ;
            fout <<   n << endl ;
            fout <<   p << endl ;
            fout <<   (n % p) << endl ;
            fout << ( (n % p) + p ) << endl ;
            fout << "+ + + + + + + +" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    
    fout << "\nTEST:  ModP( 0 ) throwing ArithModPException" ;
    {
        try
        {
            ModP<ppuint,ppsint> modp( 0 ) ;
            modp( 10 ) ;
                
            status = false ;
            fout << ".........FAIL!\n    did not catch ArithModPException when p <= 0" << endl ;                
        }
        catch( ArithModPException & e )
        {
            // Since we cannot debug trace exceptions in XCode, break on this statement so we can see it works OK.
            fout << ".........PASS!" ;
        }
    }
    
    fout << "\nTEST:  ppuint gcd( 85, 25 ) = 5" ;
    {
        ppuint u = 85u ;
        ppuint v = 25u ;
        ppuint g = gcd( u, v ) ;
        if (g != static_cast<ppuint>(5u))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    ppuint gcd( 85, 25 ) = " << g << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  BigInt gcd( 779953197883173551166308319545, 1282866356929526866866376009397 ) = 1" ;
    {
        BigInt u( "779953197883173551166308319545" ) ;
        BigInt v( "1282866356929526866866376009397" ) ;
        BigInt g = gcd( u, v ) ;
        if (g != static_cast<BigInt>( static_cast<ppuint>(1u) ))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    BigInt gcd = " << g << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  c,r = addMod( a, b, n ) for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout << "of 64 bits " ;
            ppuint a = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint b = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint n = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint c = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint r = addMod( a, b, n ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "b = " << b << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }           
        else if (8 * sizeof( ppuint ) == 32)
        {
            fout << "of 32 bits " ;
            ppuint a = static_cast<ppuint>(4294967295u) ; // 2^32-1
            ppuint b = static_cast<ppuint>(4294967294u) ; // (2^32-1)-1
            ppuint n = static_cast<ppuint>(4294967295u) ; // 2^32-1
            ppuint c = static_cast<ppuint>(4294967294u) ; // (2^32-1)-1
            ppuint r = addMod( a, b, n ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "b = " << b << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
    }

    fout << "\nTEST:  c,r = timesTwoMod( a, n ) for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout << "of 64 bits " ;
            ppuint a = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint n = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint c = static_cast<ppuint>(18446744073709551613ULL) ; // 2^64-1-2
            ppuint r = timesTwoMod( a, n ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
        else if (8 * sizeof( ppuint ) == 32) 
        {
            fout << "of 32 bits " ;
            ppuint a = static_cast<ppuint>(4294967294u) ; // 2^32-1-1
            ppuint n = static_cast<ppuint>(4294967295u) ; // 2^32-1
            ppuint c = static_cast<ppuint>(4294967293u) ; // (2^32-1)-2
            ppuint r = timesTwoMod( a, n ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
    }
    
    fout << "\nTEST:  c,r = multiplyMod( a, b, n ) for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout << "of 64 bits " ;
            ppuint a = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint b = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint n = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint c = static_cast<ppuint>(1ULL) ;                    // 1
            ppuint r = multiplyMod( a, b, n ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "b = " << b << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
        else if (8 * sizeof( ppuint ) == 32) 
        {
            fout << "of 32 bits " ;
            ppuint a = static_cast<ppuint>(4294967294u) ; // 2^32-1-1
            ppuint b = static_cast<ppuint>(4294967294u) ; // 2^32-1-1
            ppuint n = static_cast<ppuint>(4294967295u) ; // 2^32-1
            ppuint c = static_cast<ppuint>(1u) ; // 1
            ppuint r = multiplyMod( a, b, n ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "b = " << b << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
    }
    
    fout << "\nTEST:  PowerMod ppuint 3^10 = 4 (mod 7)" ;
    {
        PowerMod<ppuint> powermod( static_cast<ppuint>(7u) ) ;
        if (powermod( static_cast<ppuint>(3u), static_cast<ppuint>(10u) ) != static_cast<ppuint>(4u))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    PowerMod powermod( 7 );  powermod( 3, 10 ) = " << powermod( 3, 10 ) << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  c,r = PowerMod( a, b ) modulo n for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout << "of 64 bits " ;
            ppuint a = static_cast<ppuint>(2323123u) ;
            ppuint b = static_cast<ppuint>(10u) ;
            ppuint n = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint c = static_cast<ppuint>(17955139022230052569ULL) ;
            PowerMod<ppuint> powermod( n ) ;
            ppuint r = powermod( a, b ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "b = " << b << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
        else if (8 * sizeof( ppuint ) == 32) 
        {
            fout << "of 32 bits " ;
            ppuint a = static_cast<ppuint>(4294967290u) ;
            ppuint b = static_cast<ppuint>(10u) ;
            ppuint n = static_cast<ppuint>(4294967295u) ; // 2^32-1
            ppuint c = static_cast<ppuint>(9765625u) ;
            PowerMod<ppuint> powermod( n ) ;
            ppuint r = powermod( a, b ) ;
            if (r != c)
            {
                fout << ".........FAIL!" << endl ;
                fout << "a = " << a << endl ;
                fout << "b = " << b << endl ;
                fout << "n = " << n << endl ;
                fout << "c = " << c << endl ;
                fout << "r = " << r << endl ;
                status = false ;
            }
            else
                fout << ".........PASS!" ;
        }
    }

    fout << "\nTEST:  PowerMod BigInt 3^10 = 4 (mod 7)" ;
    PowerMod<BigInt> powermod( static_cast<BigInt>(static_cast<ppuint>(7u)) ) ;
    if (powermod( static_cast<BigInt>(static_cast<ppuint>(3u)), 
                  static_cast<BigInt>(static_cast<ppuint>(10u)) ) != static_cast<BigInt>(static_cast<ppuint>(4u)))
    {
        fout << "\n\tERROR:  PowerMod powermod( 7 );  powermod( 3, 10 ) = " << powermod( 3, 10 ) << " failed." << endl ;
        status = false ;
    }
    else
        fout << ".........PASS!" ;

    fout << "\nTEST:  PowerMod with out of range inputs." ;
    try
    {
        PowerMod<BigInt> powermod( static_cast<ppuint>(7u) ) ;
        powermod( 0, 0 ) ;

        fout << "\n\tERROR:  PowerMod on 0^0 didn't throw exception." << endl ;
        status = false ;
    }
    catch( ArithModPException & e )
    {
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  InverseModP 3 * 5 = 1 (mod 7)" ;
    InverseModP imodp( 7 ) ;
    if (imodp( 3 ) != 5)
    {
        fout << "\n\tERROR:  InverseModP imodp( 7 );  imodp( 3 ) = " << imodp( 3 ) << " failed." << endl ;
        status = false ;
    }
    else
        fout << ".........PASS!" ;

    fout << "\nTEST:  IsPrimitiveRoot.   3 is a primitive root of 7." ;
    IsPrimitiveRoot isroot( 7 ) ;
    if (isroot( 3 ))
        fout << ".........PASS!" ;
    else
    {
        fout << "\n\tERROR:  IsPrimitiveRoot( 7 ) isroot ;  isroot( 3 ) = " << isroot( 3 ) << " failed." << endl ;
        status = false ;
    }

    fout << "\nTEST:  IsPrimitiveRoot.   2 is a primitive root of 11." ;
    IsPrimitiveRoot isroot11( 11 ) ;
    if (isroot11( 2 ))
        fout << ".........PASS!" ;
    else
    {
        fout << "\n\tERROR:  IsPrimitiveRoot( 11 ) isroot11 ;  isroot11( 2 ) = " << isroot11( 2 ) << " failed." << endl ;
        status = false ;
    }

    fout << "\nTEST:  IsPrimitiveRoot.   3 is NOT a primitive root of 11." ;
    if (isroot11( 3 ))
    {
        fout << "\n\tERROR:  IsPrimitiveRoot( 11 ) isroot11 ;  isroot11( 3 ) = " << isroot11( 3 ) << " failed." << endl ;
        status = false ;
    }
    else
        fout << ".........PASS!" ;

    fout << "\nTEST:  IsPrimitiveRoot.   5 is a primitive root of 65003." ;
    IsPrimitiveRoot isroot65003( 65003 ) ;
    if (isroot65003( 5 ))
        fout << ".........PASS!" ;
    else
    {
        fout << "\n\tERROR:  IsPrimitiveRoot65003( 5 ) should have said true.  It failed." << endl ;
        status = false ;
    }

    fout << "\nTEST:  IsPrimitiveRoot.   8 is NOT a primitive root of 65003." ;
    if (isroot65003( 8 ))
    {
        fout << "\n\tERROR:  IsPrimitiveRoot65003( 8 ) should have said false.  It failed." << endl ;
        status = false ;
    }
    else
        fout << ".........PASS!" ;

    fout << "\nTEST:  constant coefficient test." ;
    ArithModP arith1( 5 ) ;
    if (arith1.const_coeff_test( 4, 1, 11 ))
        fout << ".........PASS!" ;
    else
    {
        fout << "\n\tERROR:  constant coefficient test failed = " << arith1.const_coeff_test( 4, 1, 11 ) << " failed." << endl ;
        status = false ;
    }

    fout << "\nTEST:  constant coefficient is primitive root." ;
    ArithModP arith2( 7 ) ;
    if (arith2.const_coeff_is_primitive_root( 4, 11 ))
        fout << ".........PASS!" ;
    else
    {
        fout << "\n\tERROR:  constant coefficient test failed = " << arith1.const_coeff_is_primitive_root( 4, 11 ) << " failed." << endl ;
        status = false ;
    }

    fout << "\nTEST:  isProbablyPrime on ppuint prime 97 with random x = 10" ;
    if ( isProbablyPrime( static_cast<ppuint>( 97u ), static_cast<ppuint>( 10u ) ) == Primality::ProbablyPrime)
        fout << ".........PASS!" ;
    else
    {
        fout << ".........FAIL!" << endl ;
        status = false ;
    }

    fout << "\nTEST:  isAlmostSurelyPrime for ppuint prime 97" ;
    if ( isAlmostSurelyPrime( static_cast<ppuint>( 97u ) ))
        fout << ".........PASS!" ;
    else
    {
        fout << ".........FAIL!" << endl ;
        status = false ;
    }

    fout << "\nTEST:  isAlmostSurelyPrime for BigInt prime 97" ;
    if ( isAlmostSurelyPrime( static_cast<BigInt>( "97" ) ))
        fout << ".........PASS!" ;
    else
    {
        fout << ".........FAIL!" << endl ;
        status = false ;
    }

    fout << "\nTEST:  isAlmostSurelyPrime for non-prime BigInt 49" ;
    if (isAlmostSurelyPrime( static_cast<BigInt>( 49u ) ))
    {
        fout << ".........FAIL!" << endl ;
        status = false ;
    }
    else
        fout << ".........PASS!" ;

    fout << "\nTEST:  isAlmostSurelyPrime on the 10000th prime number 104729 of ppuint type" ;
    if (isAlmostSurelyPrime( static_cast<ppuint>( 104729u )))
        fout << ".........PASS!" ;
    else
    {
        fout << ".........FAIL!" << endl ;
        status = false ;
    }

    return status ;
}



bool unitTestFactoring( ostream & fout )
{
    bool status = true ;

    fout << "\nTEST:  Factor table method used on unsigned int 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181" ;
    
    // 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181
    ppuint num { 3486784400 } ;
    ppuint   p { 3 } ;
    ppuint   n { 20 } ;
    
    Factorization<ppuint> f( num, FactoringAlgorithm::FactorTable, p, n ) ;
    vector<ppuint> df = f.getDistinctPrimeFactors() ;

    if (!(f.multiplicity(0) == 4 && f.prime_factor(0) ==    2u && f.prime_factor(0) == df[0] &&
          f.multiplicity(1) == 2 && f.prime_factor(1) ==    5u && f.prime_factor(1) == df[1] &&
          f.multiplicity(2) == 2 && f.prime_factor(2) ==   11u && f.prime_factor(2) == df[2] &&
          f.multiplicity(3) == 1 && f.prime_factor(3) ==   61u && f.prime_factor(2) == df[2] &&
          f.multiplicity(4) == 1 && f.prime_factor(4) == 1181u && f.prime_factor(2) == df[2] ))
    {
        fout << "\n\tERROR:  Table lookup factoring on unsigned int." << endl ;
        status = false ;

        fout << "\tFactor<ppuint> failed on 337500 = 2^2 3^3 5^5." << endl ;
        fout << "\tNumber of factors = " << f.num_distinct_factors() << endl ;
        fout << "\tFactors = " << endl ;
        for (unsigned int i = 0 ;  i < f.num_distinct_factors() ;  ++i)
            fout << f.prime_factor( i ) << " ^ " << f.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;

    
    fout << "\nTEST:  Factor table method used on BigInt 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181" ;
    
    // 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181
    BigInt num1 { 3486784400 } ;
    ppuint   p1 { 3 } ;
    ppuint   n1 { 20 } ;
    
    Factorization<BigInt> f1( num1, FactoringAlgorithm::FactorTable, p1, n1 ) ;
    vector<BigInt> df1 = f1.getDistinctPrimeFactors() ;

    if (!(f1.multiplicity(0) == 4 && f1.prime_factor(0) == static_cast<BigInt>(    2u ) && f1.prime_factor(0) == df[0] &&
          f1.multiplicity(1) == 2 && f1.prime_factor(1) == static_cast<BigInt>(    5u ) && f1.prime_factor(1) == df[1] &&
          f1.multiplicity(2) == 2 && f1.prime_factor(2) == static_cast<BigInt>(   11u ) && f1.prime_factor(2) == df[2] &&
          f1.multiplicity(3) == 1 && f1.prime_factor(3) == static_cast<BigInt>(   61u ) && f1.prime_factor(2) == df[2] &&
          f1.multiplicity(4) == 1 && f1.prime_factor(4) == static_cast<BigInt>( 1181u ) && f1.prime_factor(2) == df[2] ))
    {
        fout << "\n\tERROR:  Table lookup factoring on unsigned int." << endl ;
        status = false ;

        fout << "\tFactor<BigInt> failed on 337500 = 2^2 3^3 5^5." << endl ;
        fout << "\tNumber of factors = " << f.num_distinct_factors() << endl ;
        fout << "\tFactors = " << endl ;
        for (unsigned int i = 0 ;  i < f.num_distinct_factors() ;  ++i)
            fout << f.prime_factor( i ) << " ^ " << f.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;



    fout << "\nTEST:  Trial division factoring on unsigned int 337500 = 2^2 3^3 5^5." ;

    Factorization<ppuint> f2( 337500u, FactoringAlgorithm::TrialDivisionAlgorithm ) ;
    vector<ppuint> df2 = f2.getDistinctPrimeFactors() ;

    if (!(f2.multiplicity(0) == 2 && f2.prime_factor(0) == 2u && f2.prime_factor(0) == df2[0] &&
          f2.multiplicity(1) == 3 && f2.prime_factor(1) == 3u && f2.prime_factor(1) == df2[1] &&
          f2.multiplicity(2) == 5 && f2.prime_factor(2) == 5u && f2.prime_factor(2) == df2[2] ))
    {
        fout << "\n\tERROR:  Trial division factoring on unsigned int." << endl ;
        status = false ;

        fout << "\tFactor<ppuint> failed on 337500 = 2^2 3^3 5^5." << endl ;
        fout << "\tNumber of factors = " << f2.num_distinct_factors() << endl ;
        fout << "\tFactors = " << endl ;
        for (unsigned int i = 0 ;  i < f2.num_distinct_factors() ;  ++i)
            fout << f2.prime_factor( i ) << " ^ " << f2.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;



    fout << "\nTEST:  Trial division factoriing on BigInt 337500 = 2^2 3^3 5^5." ; 

    Factorization<BigInt> f3( static_cast<BigInt>("337500"), FactoringAlgorithm::TrialDivisionAlgorithm ) ;
    vector<BigInt> df3 = f3.getDistinctPrimeFactors() ;

    if (!(f3.multiplicity(0) == 2u  && f3.prime_factor( 0 ) == static_cast<BigInt>( 2u ) && f3.prime_factor( 0 ) == df3[0] &&
          f3.multiplicity(1) == 3u  && f3.prime_factor( 1 ) == static_cast<BigInt>( 3u ) && f3.prime_factor( 1 ) == df3[1] &&
          f3.multiplicity(2) == 5u  && f3.prime_factor( 2 ) == static_cast<BigInt>( 5u ) && f3.prime_factor( 2 ) == df3[2] ))
    {
        fout << "\n\tERROR:Factorization<BigInt> failed on 337500 = 2^2 3^3 5^5." << endl ;
        status = false ;

        fout << "Number of factors = " << f3.num_distinct_factors() << endl ;
        fout << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < f3.num_distinct_factors() ;  ++i)
            fout << f3.prime_factor( i ) << " ^ " << f3.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;



    fout << "\nTEST:  Pollard Rho factorization on unsigned int 25852 = 2^2 23 281" ; 
    
    Factorization<ppuint> fr( 25852u, FactoringAlgorithm::PollardRhoAlgorithm ) ;
    vector<ppuint> dfr = fr.getDistinctPrimeFactors() ;

    if ( !(fr.multiplicity(0) == 2 && fr.prime_factor(0) ==  2u  && fr.prime_factor(0) == dfr[0] &&
           fr.multiplicity(1) == 1 && fr.prime_factor(1) == 23u  && fr.prime_factor(1) == dfr[1] &&
           fr.multiplicity(2) == 1 && fr.prime_factor(2) == 281u && fr.prime_factor(2) == dfr[2] ))
    {
        fout << "\n\tERROR:Factorization<ppuint> failed on 25852 = 2^2 23 281." << endl ;

        status = false ;
        fout << "Number of factors = " << fr.num_distinct_factors() << endl ;
        fout << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < fr.num_distinct_factors() ;  ++i)
            fout << fr.prime_factor( i ) << " ^ " << fr.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;


    fout << "\nTEST:  Pollard Rho factorization on BigInt 25852 = 2^2 23 281" ; 
    
    Factorization<BigInt> frb( 25852u, FactoringAlgorithm::PollardRhoAlgorithm ) ;
    vector<BigInt> dfrb = frb.getDistinctPrimeFactors() ;

    if ( !(frb.multiplicity(0) == 2 && frb.prime_factor(0) == static_cast<BigInt>( 2u   ) && frb.prime_factor(0) == dfrb[0] &&
           frb.multiplicity(1) == 1 && frb.prime_factor(1) == static_cast<BigInt>( 23u  ) && frb.prime_factor(1) == dfrb[1] &&
           frb.multiplicity(2) == 1 && frb.prime_factor(2) == static_cast<BigInt>( 281u ) && frb.prime_factor(2) == dfrb[2] ))
    {
        fout << "\n\tERROR:Factorization<BigInt> failed on 25852 = 2^2 23 281." << endl ;

        status = false ;
        fout << "Number of factors = " << frb.num_distinct_factors() << endl ;
        fout << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < frb.num_distinct_factors() ;  ++i)
            fout << frb.prime_factor( i ) << " ^ " << frb.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;



    fout << "\nTEST:  BigInt computation of p^n, r, factors of r, EulerPhi[ p^n - 1]/n for p = 2" ;
    {
        ppuint p = 2u ;
        int  n = 36u ;Factorization<BigInt> factorsOfR ;
        BigInt maxNumPossiblePoly ;
        BigInt r ;
        BigInt numPrimitivePoly ;
        try
        {
            factorRAndFindNumberOfPrimitivePolynomials( p, n, maxNumPossiblePoly, r, factorsOfR, numPrimitivePoly ) ;

            if ( !( maxNumPossiblePoly == static_cast<BigInt>( "68719476736" ) && r == static_cast<BigInt>( "68719476735" ) &&
                    factorsOfR.prime_factor(0) == static_cast<BigInt>(  3) && factorsOfR.multiplicity(0) == 3 &&
                    factorsOfR.prime_factor(1) == static_cast<BigInt>(  5) && factorsOfR.multiplicity(1) == 1 &&
                    factorsOfR.prime_factor(2) == static_cast<BigInt>(  7) && factorsOfR.multiplicity(2) == 1 &&
                    factorsOfR.prime_factor(3) == static_cast<BigInt>( 13) && factorsOfR.multiplicity(3) == 1 &&
                    factorsOfR.prime_factor(4) == static_cast<BigInt>( 19) && factorsOfR.multiplicity(4) == 1 &&
                    factorsOfR.prime_factor(5) == static_cast<BigInt>( 37) && factorsOfR.multiplicity(5) == 1 &&
                    factorsOfR.prime_factor(6) == static_cast<BigInt>( 73) && factorsOfR.multiplicity(6) == 1 &&
                    factorsOfR.prime_factor(7) == static_cast<BigInt>(109) && factorsOfR.multiplicity(7) == 1 &&
                    numPrimitivePoly == static_cast<BigInt>( "725594112" )
                ))
            {
                fout << "\n\tERROR:  BigInt computation of p^n, r, factors of r, EulerPhi[ p^n - 1]/n for p = 2" << endl ;
                status = false ;

                fout << "p = " << p << endl ;
                fout << "n = " << n << endl ;
                fout << "maxNumPossiblePoly = " << maxNumPossiblePoly << endl ;
                fout << "r = " << r << endl ;
                fout << "r:  Number of factors = " << factorsOfR.num_distinct_factors() << endl ;
                fout << "Factors = " << endl ;
                for (unsigned int i = 0 ;  i < factorsOfR.num_distinct_factors() ;  ++i)
                    fout << factorsOfR.prime_factor( i ) << " ^ " << factorsOfR.multiplicity( i ) << " " ;
                fout << endl ;

                fout << "numPrimitivePoly = " << numPrimitivePoly << endl ;
            }
            else
                fout << ".........PASS!" ;
        }
        catch( BigIntMathError & e )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    caught BigIntMathError:  " << e.what() << endl ;
            status = false ;
        }
        catch( FactorError & e )
        {
            fout << ".........FAIL!" << endl ;
            fout << "    caught FactorError:  " << e.what() << endl ;
            status = false ;
        }
    }


    fout << "\nTEST:  Factor Copy constructor" ;
    Factorization<BigInt> fact( f3 ) ;
    vector<BigInt> dfact = fact.getDistinctPrimeFactors() ;
    if (!(fact.multiplicity(0) == 2 && fact.prime_factor(0) == static_cast<BigInt>( 2u ) && fact.prime_factor(0) == dfact[0] &&
          fact.multiplicity(1) == 3 && fact.prime_factor(1) == static_cast<BigInt>( 3u ) && fact.prime_factor(1) == dfact[1] &&
          fact.multiplicity(2) == 5 && fact.prime_factor(2) == static_cast<BigInt>( 5u ) && fact.prime_factor(2) == dfact[2] ))
    {
        fout << "\n\tERROR:  Factor copy constructor failed on 337500 = 2^2 3^3 5^5." << endl ;
        status = false ;

        fout << "Number of factors = " << fact.num_distinct_factors() << endl ;
        fout << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < fact.num_distinct_factors() ;  ++i)
            fout << fact.prime_factor( i ) << " ^ " << fact.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;



    fout << "\nTEST:  Factor assignment operator" ;
    Factorization<BigInt> fact1 ;
    fact1 = f3 ;
    vector<BigInt> dfact1 = fact1.getDistinctPrimeFactors() ;
    if (!(fact1.multiplicity(0) == 2 && fact1.prime_factor(0) == static_cast<BigInt>( 2u ) && dfact1[0] == fact1.prime_factor(0) &&
          fact1.multiplicity(1) == 3 && fact1.prime_factor(1) == static_cast<BigInt>( 3u ) && dfact1[1] == fact1.prime_factor(1) &&
          fact1.multiplicity(2) == 5 && fact1.prime_factor(2) == static_cast<BigInt>( 5u ) && dfact1[2] == fact1.prime_factor(2) ))
    {
        fout << "\n\tERROR:  Factor assignment operator failed on 337500 = 2^2 3^3 5^5." << endl ;
        status = false ;

        fout << "Number of factors = " << fact1.num_distinct_factors() << endl ;
        fout << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < fact1.num_distinct_factors() ;  ++i)
            fout << fact1.prime_factor( i ) << " ^ " << fact1.multiplicity( i ) << " " ;
        fout << endl ;
    }
    else
        fout << ".........PASS!" ;

    return status ;
}



bool unitTestPolynomials( ostream & fout )
{
    bool status = true ;

    fout << "\nTEST:  Polynomial() default constructor." ;
    {
        Polynomial p ;
        if (p.deg() != 0)
        {
            fout << "\n\tERROR: Polynomial default constructor failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial() from string." ;
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        if (p.deg() != 2 || p.modulus() != 3 || p[0] != 1 || p[1] != 0 || p[2] != 2)
        {
            fout << "\n\tERROR: Polynomial p( \"2x^2 + 1, 3\" ) failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial = string." ;
    {
        Polynomial p ;
        p = "2x^2 + 1, 3" ;
        if (p.deg() != 2 || p.modulus() != 3 || p[0] != 1 || p[1] != 0 || p[2] != 2)
        {
            fout << "\n\tERROR: Polynomial p = string \"2x^2 + 1, 3\" ) failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial() from string with negative constant." ;
    try
    {
        Polynomial p( "x^4-1, 5" ) ;
        fout << "\n\tERROR: Polynomial with negative constant failed to throw an exception " << endl ;
        status = false ;
    }
    catch( PolynomialRangeError & e )
    {
        string firstPartOfMessage = "Error in parser converting polynomial from string: x^4-1, 5" ;
        string errorMessage = e.what() ;
        size_t len = firstPartOfMessage.size() ;
        if (firstPartOfMessage.compare( 0, len, errorMessage, 0, len ) != 0)
        {
            fout << "\n\tERROR: Parser failed while parsing polynomial x^4-1, 5" << endl ;
            fout << "Incorrect error message:  " << endl ;
            fout << "Polynomial range error:  |" << e.what() << "|" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial() to string." ;
    {
        Polynomial p ; // ( "2x^2 + 1, 3" )
        Polynomial q( p ) ;
        p[0] = 1 ; p[2] = 2 ;
        p.setModulus( 3 ) ;
        string s = p ;
        if (s != "2 x ^ 2 + 1, 3")
        {
            fout << "\n\tERROR: Polynomial p( \"2x^2 + 1, 3\" ) to string s = " << s << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial() copy constructor." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( p ) ;
        if (static_cast<string>(q) != "2 x ^ 2 + 1, 3")
        {
            fout << "\n\tERROR: Polynomial copy constructor p( q ) = " << q << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  copy constructor failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial equality test operator==()." ;
    try
    {
        Polynomial p1( "2x^2 + 1, 3" ) ;
        Polynomial p2( "2x^2 + 1, 3" ) ;
        if (p1 == p2)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: Polynomial " << p1 << " == " << p2 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  operator()== failed." << e.what() << endl ;
        status = false ;
    }
    
    fout << "\nTEST:  Polynomial inequality test operator!=()." ;
    try
    {
        Polynomial p1( "2x^2 + 1, 3" ) ;
        Polynomial p2( "2x^2 + x + 1, 3" ) ;
        if (p1 != p2)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: Polynomial " << p1 << " != " << p2 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  operator()!= failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial assignment operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q ;
        q = p ; // If we did Polynomial q = p it would call the copy constructor.
        if (static_cast<string>(q) != "2 x ^ 2 + 1, 3")
        {
            fout << "\n\tERROR: Polynomial assignment operator p = q " << q << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  assignment operator p = q failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial()[] read only operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        if (p[ 0 ] == 1 && p[ 1 ] == 0 && p[ 2 ] == 2)
           fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR:  Polynomial [] read only failed on reading p[0], p[1], p[2]." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  Polynomial [] read only failed on reading p[0], p[1], p[2]." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial()[] accessing coeff higher than its degree." ;
    try
    {
        const Polynomial p( "2x^2 + 1, 3" ) ;
        ppuint z = p[ 3 ] ;

        fout << "\n\tERROR:  Polynomial [] failed to throw exception on accessing p[3] = " << z << endl ;
        fout << "poly = " << p << " deg of p = " << p.deg() << endl ;
        status = false ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial()[] lvalue operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        int oldDeg = p.deg() ;
        p[ 5 ] = 2 ;
        p[ 1 ] = 1 ;
        int newDeg = p.deg() ;
        if (static_cast<string>(p) != "2 x ^ 5 + 2 x ^ 2 + x + 1, 3" || oldDeg != 2 || newDeg != 5)
        {
            fout << "\n\tERROR: Polynomial [] lvalue operator " << p << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  Polynomial [] failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial() += operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( " x^2 + 1, 3" ) ;
        p += q ;
        if (p.deg() != 1 && static_cast<string>(p) != "2, 3")
        {
            fout << "\n\tERROR: Polynomial += " << p << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  Polynomial += failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial() += operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( " x^2 + 2, 3" ) ;
        p += q ;
        if (p.deg() != 0 && static_cast<string>(p) != "0, 3")
        {
            fout << "\n\tERROR: Polynomial += " << p << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  Polynomial += failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial() + operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( " x^2 + 1, 3" ) ;
        Polynomial r = p + q ;
        if (static_cast<string>(r) != "2, 3")
        {
            fout << "\n\tERROR: Polynomial + " << r << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  Polynomial + failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial * scalar" ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        ppuint k = 2 ;

        Polynomial q = p * k ;
        if (static_cast<string>(q) != "x ^ 2 + 2, 3")
        {
            fout << "\n\tERROR: Polynomial * scalar operator " << q << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  * scalar failed." << e.what() << endl ;
        status = false ;
    }


    fout << "\nTEST:  Polynomial evaluation x^4 + 3x + 3 (mod 5)" ;
    {
        Polynomial p( "x^4 + 3x + 3, 5" ) ;
        ppuint f2 = p( 2 ) ;
        ppuint f3 = p( 3 ) ;
        ppuint f0 = p( 0 ) ;
        if (f2 != 0 || f3 != 3 || f0 != 3)
        {
            fout << "\n\tERROR: Polynomial operator() = " << f2 << f3 << f0 << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial evaluation x^4 + x + 1 (mod 2)" ;
    {
        Polynomial p( "x^4 + x + 1, 2" ) ;
        ppuint f0 = p( 0 ) ;
        ppuint f1 = p( 1 ) ;
        if (f0 != 1 || f1 != 1)
        {
            fout << "\n\tERROR: Polynomial operator() = " << f0 << f1 << " failed." << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }

    fout << "\nTEST:  Polynomial hasLinearFactor is true" ;
    {
        Polynomial p( "x^4 + 3x + 3, 5" ) ;
        bool linFac = p.hasLinearFactor() ;
            if (linFac)
                fout << ".........PASS!" ;
            else
            {
                fout << "\n\tERROR: Polynomial hasLinearFactor = " << linFac << " failed." << endl ;
                status = false ;
            }
        }

    fout << "\nTEST:  Polynomial hasLinearFactor is false" ;
    {
        Polynomial p( "x^4 + 3x^2 + x + 1, 5" ) ;
        bool linFac = p.hasLinearFactor() ;
        if (!linFac)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: Polynomial hasLinearFactor = " << linFac << " failed." << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  Polynomial isInteger" ;
    try
    {
        Polynomial p( "x^4 + 3x + 3, 5" ) ;
        bool isInt = p.isInteger() ;
        if (!isInt)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: Polynomial " << p << " isInteger = " << isInt << " failed." << endl ;
            status = false ;
        }

        Polynomial q( "3, 5" ) ;
        isInt = q.isInteger() ;
        if (isInt)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: Polynomial " << q << " isInteger = " << isInt << " failed." << endl ;
            status = false ;
        }

    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error: polynomial operator() failed." << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  Polynomial initial and next trial polynomials" ;
    try {
        Polynomial p ;
        p.initial_trial_poly( 4, 5 ) ;

        int i = 0 ;
        for (i = 1 ;  i <= 20 ; ++i)
        {
            p.next_trial_poly() ;
        }
        if (static_cast<string>(p) == "x ^ 4 + 3 x + 4, 5")
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: Polynomial " << p << " (20th iteration from initial) failed." << endl ;
            status = false ;
        }

    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error: polynomial operator() failed." << e.what() << endl ;
        status = false ;
    }

    ////////////////////////////////////////////////////////////////////////
    // Test polynomial mod
    ////////////////////////////////////////////////////////////////////////

    fout << "\nTEST:  PolyMod constructor from polynomials." ;
    try {
        Polynomial g( "x^4 + x^2 + 1,2" ) ;
        Polynomial f( "x^4 + x + 1,2" ) ;

        PolyMod p( g, f ) ;
        if (static_cast<string>(p) == "x ^ 2 + x, 2" &&
            static_cast<string>( p.getf() ) == "x ^ 4 + x + 1, 2" && p.getModulus() == 2)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod constructor from polynomials, g(x)  = " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod constructor from string and polynomial." ;
    try {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyMod p( "x^6 + 2x^2 + 3x + 2, 5", f ) ;

        if (static_cast<string>(p) == "3 x ^ 3, 5" &&
            static_cast<string>( p.getf() ) == "x ^ 4 + x ^ 2 + 2 x + 3, 5" && p.getModulus() == 5)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod constructor from string and polynomial failed." << endl ;
            fout << "\ng(x) mod f(x), p = " << p << endl ;
            fout << "\nf(x) = " << f << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod timesX." ;
    try {
        Polynomial g( "2x^3 + 4x^2 + 3x, 5" ) ;
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;

        PolyMod p( g, f ) ;

        p.timesX() ;
        if (static_cast<string>(p) == "4 x ^ 3 + x ^ 2 + x + 4, 5" )
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod timesX " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod autoconvolve." ;
    try {
        Polynomial t( "4x^3 + x^2 + 3x + 3, 5" ) ;
        int k = 3 ; int lower = 1 ; int upper = 3 ;
        ppuint c = autoConvolve( t, k, lower, upper ) ;

        if (c == 3)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod autoconvolve = " << c
                 << " for t = " << t << " deg = " << t.deg() << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError autoconvolve:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod convolve." ;
    try {
        Polynomial s( "4x^3 + x^2 + 3x + 3, 5" ) ;
        Polynomial t( "4x^3 + x^2 + 3x + 3, 5" ) ;
        int k = 3 ; int lower = 1 ; int upper = 3 ;
        ppuint c = convolve( s, t, k, lower, upper ) ;

        if (c == 3)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod convolve = " << c
                 << " for t = " << t << " deg = " << t.deg() << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError convolve:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod coeffOfSquare." ;
    try {
        Polynomial g( "4x^3 + x^2 + 3x + 3, 5" ) ;
        int n = 4 ;
        ppuint c0 = coeffOfSquare( g, 0, n ) ;
        ppuint c1 = coeffOfSquare( g, 1, n ) ;
        ppuint c2 = coeffOfSquare( g, 2, n ) ;
        ppuint c3 = coeffOfSquare( g, 3, n ) ;
        ppuint c4 = coeffOfSquare( g, 4, n ) ;
        ppuint c5 = coeffOfSquare( g, 5, n ) ;
        ppuint c6 = coeffOfSquare( g, 6, n ) ;

        if (c0 == 4 && c1 == 3 && c2 == 0 && c3 == 0 && c4 == 0 && c5 == 3 && c6 == 1)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod coeffOfSquare (c0 ... c6) = " << c0 << " " << c1 << " " << c2
                 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError coeffOfSquare:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod coeffOfProduct." ;
    try {
        Polynomial s( "4x^3 + x^2 + 4, 5" ) ;
        Polynomial t( "3x^2 + x + 2, 5" ) ;
        int n = 4 ;
        ppuint c0 = coeffOfProduct( s, t, 0, n ) ;
        ppuint c1 = coeffOfProduct( s, t, 1, n ) ;
        ppuint c2 = coeffOfProduct( s, t, 2, n ) ;
        ppuint c3 = coeffOfProduct( s, t, 3, n ) ;
        ppuint c4 = coeffOfProduct( s, t, 4, n ) ;
        ppuint c5 = coeffOfProduct( s, t, 5, n ) ;
        ppuint c6 = coeffOfProduct( s, t, 6, n ) ;

        if (c0 == 3 && c1 == 4 && c2 == 4 && c3 == 4 && c4 == 2 && c5 == 2 && c6 == 0)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod coeffOfProduct (c0 ... c6) = " << c0 << " " << c1 << " " << c2
                 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError coeffOfProduct:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod square." ;
    try {
        Polynomial g( "4x^3 + x^2 + 4, 5" ) ;
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;

        PolyMod p( g, f ) ;

        p.square() ;
        if (static_cast<string>(p) == "2 x ^ 3 + 4 x ^ 2 + x + 1, 5" )
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod square " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod operator* and implicitly, operator*=" ;
    try {
        Polynomial s( "4x^3 + x^2 + 4, 5" ) ;
        Polynomial t( "3x^2 + x + 2, 5" ) ;
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;

        PolyMod smodf( s, f ) ;
        PolyMod tmodf( t, f ) ;

        PolyMod p = smodf * tmodf ;
        if (static_cast<string>(p) == "2 x ^ 3 + 3 x ^ 2 + 4 x + 2, 5" )
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod operator* " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyMod x_to_power and isInteger()" ;
    try {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyMod x( "x, 5", f ) ; // g(x) = 0, modulus = 5.

        PolyMod p = power( x, 156u ) ;
        if (static_cast<string>(p) == "3, 5" && p.isInteger())
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyMod x_to_power = |" << static_cast<string>(p) << "| failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    return status ;
}



bool unitTestPolynomialOrder( ostream & fout )
{
    bool status = true ;

    fout << "\nTEST:  PolyOrder reduced Q-I matrix" ;
    try {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        order.hasMultipleDistinctFactors( false ) ;
        string s = order.printQMatrix() ;
        string t = "\n(    0   0   0   0 )\n(    0   4   0   0 )\n(    4   0   0   0 )\n(    0   0   4   0 )\n" ;
        if (s == t)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder reduced Q-I failed = " << s << endl ;
            fout << "\n                   true reduced Q-I = " << t << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyOrder 3 distinct factors out of 4" ;
    try {
        Polynomial f( "x^4 + 3 x^3 + 3 x^2 + 3 x + 2, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;
        if (multipleFactors && order.getNullity() == 3)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder 3 distinct factors out of 4 failed" << endl ;
            fout << " f( x ) = " << f << endl ;
            fout << " nullity = " << order.getNullity() << endl ;
            fout << "\n    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << "\n\tERROR:  PolynomialRangeError error:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyOrder, reducible polynomial x^3 + 3 mod 5 with 2 distinct factors" ;
    try {
        Polynomial f( "x^3 + 3, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;

        if (multipleFactors && order.getNullity() == 2)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    PolyOrder 2 distinct factors failed" << endl ;
            fout << "    f( x ) = " << f << endl ;
            fout << "    nullity = " << order.getNullity() << endl ;
            fout << "    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << ".........FAIL!" << endl ;
        fout << "    PolynomialRangeError:  " << e.what() << endl ;
        status = false ;
    }

    fout << "\nTEST:  PolyOrder, irreducible polynomial x^4 + x^2 + 2x + 3 mod 5 (nullity = 1)" ;
    try 
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;

        if (multipleFactors == false && order.getNullity() == 1)
            fout << ".........PASS!" ;
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    PolyOrder irreducible" << endl ;
            fout << "    f( x ) = " << f << endl ;
            fout << "    nullity = " << order.getNullity() << endl ;
            fout << "    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout << ".........FAIL!" << endl ;
        fout << "    PolynomialRangeError:  " << e.what() << endl ;
        status = false ;
    }


    fout << "\nTEST:  PolyOrder 1 distinct factor 4 times" ;
    {
        Polynomial f( "x^4 + 4x^3 + x^2 + 4x + 1, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;
        if (multipleFactors == false && order.getNullity() == 1)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder 1 distinct factor 4 times" << endl ;
            fout << " f( x ) = " << f << endl ;
            fout << " nullity = " << order.getNullity() << endl ;
            fout << "\n    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  PolyOrder order_m()" ;
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        if (order.order_m())
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder order_m failed." << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  PolyOrder order_r() is true" ;
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        ppuint a = order.order_r() ;
        if (a == 3)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder order_r failed." << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  PolyOrder order_r() is false" ;
    {
        Polynomial f( "x^4 + x + 3, 5" ) ;
        PolyOrder order( f ) ;

        ppuint a = order.order_r() ;
        if (a == 0)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder order_r failed." << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  PolyOrder isPrimitive on non-primitive poly" ;
    {
        Polynomial f( "x^5 + x + 1, 2" ) ;
        PolyOrder order( f ) ;

        bool isPrimitive = order.isPrimitive() ;
        if (!isPrimitive)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder isPrimitive on non-primitive poly" << endl ;
            fout << " f( x ) = " << f << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  PolyOrder isPrimitive on primitive poly" ;
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        bool isPrimitive = order.isPrimitive() ;
        if (isPrimitive)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder isPrimitive on primitive poly" << endl ;
            fout << " f( x ) = " << f << endl ;
            status = false ;
        }
    }

    fout << "\nTEST:  PolyOrder isPrimitive on primitive poly, part II" ;
    {
        Polynomial f0( "x^4+4, 5" ) ;
        PolyOrder order( f0 ) ;

        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        order.newPolynomial( f ) ;

        bool isPrimitive = order.isPrimitive() ;
        if (isPrimitive)
            fout << ".........PASS!" ;
        else
        {
            fout << "\n\tERROR: PolyOrder isPrimitive on primitive poly, part II" << endl ;
            fout << " f( x ) = " << f << endl ;
            status = false ;
        }
    }
    
    return status ;
}



bool unitTestParser( ostream & fout )
{
    bool status = true ;
    
    // Create a parser with parse tables.
    string s ;
    PolyValue  v ;
    PolyParser< PolySymbol, PolyValue > p ;
    
    fout << "\nTEST:  Parsing command line options for test polynomial x^4 + 1, 2 with -s -t and -c options." ;
    {
#ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        const char * argv[ 5 ] { "Primpoly", "-s", "-t",  "-c", "x^3 + 1, 2" } ;
        const Polynomial truePoly( "x^3 + 1", 2 ) ;
#else
        const char * argv[ 5 ] { "Primpoly", "-s", "-t",  "-c", "x^4 + 1, 2" } ;
        const Polynomial truePoly( "x^4 + 1", 2 ) ;
#endif
        
        p.parseCommandLine( 5, argv ) ;
        
        if (p.testPolynomialForPrimitivity_ && p.printOperationCount_ && p.slowConfirm_ &&
            !p.listAllPrimitivePolynomials_ && !p.printHelp_ &&
            p.testPolynomial_ == truePoly)
        {
            fout << ".........PASS!" ;
        }
        else
        {
            fout << ".........FAIL!" << endl ;
            fout << "    Test polynomial = " << p.testPolynomial_ ; fout << " deg = " << p.testPolynomial_.deg() << endl ;
            fout << "    p = " << p.p << "    n = " << p.n << endl ;
            status = false ;
        }
    }
    
    fout << "\nTEST:  parsing constant 0" ;
    {
        s = "0" ;
#ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "2" ;
#endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 2 && (v.f_.size()-1) == 0 && v.f_[0] == 0))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    parsing input " << s << endl << " value = "  << v << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    
    fout << "\nTEST:  parsing polynomial with a specified modulus:  2 x ^ 3 + 3 x + 4, 5" ;
    {
#ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "2 x ^ 3 + 3 x + 4, 7" ;
#else
        s = "2 x ^ 3 + 3 x + 4, 5" ;
#endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 5 && (v.f_.size()-1) == 3 && v.f_[0] == 4 && v.f_[1] == 3 && v.f_[2] == 0 && v.f_[3] == 2))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    parsing input " << s << endl << " value = " << v << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    
    fout << "\nTEST:  parsing polynomial 2x without a modulus, which will be defaulted to p=2:  2x" ;
    {
        s = "2x" ;
#ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "2x,3" ;
#endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 2 && (v.f_.size()-1) == 1 && v.f_[0] == 0 && v.f_[1] == 2 ))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    parsing input " << s << endl << " value = " << v << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    
    fout << "\nTEST:  parsing bad syntax x 1" ;
    try
    {
        s = "x 1" ;
#ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "x+1" ;
#endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 0 && (v.f_.size()-1) == 0 &&
              v.f_[0] == 0 && v.f_[1] == 2 ))
        {
            fout << ".........FAIL!" << endl ;
            fout << "    Parser did not throw a parsing error " << endl ;
            fout << "    while parsing input " << s << endl << " value = " << v << endl ;
            status = false ;
        }
    }
    catch( ParserError & e )
    {
        if ((string)(e.what()) != "Expecting to see x^ or x or x ^ integer in sentence x 1")
        {
            fout << ".........FAIL!" << endl ;
            fout << "    Parser correctly threw a parse error exception while parsing " << s << endl << " value = " << v << endl ;
            fout << "    but the error message was incorrect!  error = |" << e.what() << "|" << endl ;
            status = false ;
        }
        else
            fout << ".........PASS!" ;
    }
    
    return status ;
}
#endif // SELF_CHECK


