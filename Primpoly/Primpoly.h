/*==============================================================================
| 
|  NAME     
|
|     Primpoly.h
|
|  DESCRIPTION   
|
|     Global header file for primitive polynomial routines.
|     Constants, message strings, data types and algorithm control parameters.
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

// Wrap this header file to prevent duplication if it is included
// accidentally more than once.
#ifndef PP_H 
#define PP_H


/*=============================================================================
 |
 | NAME
 |
 |     Debug macros.
 |
 +============================================================================*/

// Let's do this always, so we can gather bug information from users.
#define SELF_CHECK

// These debugging flags are normally be defined in the makefile or build file,
// but you could alternately define them here.
//
// High level algorithmic debugging:
//     #define DEBUG_PP_POLYNOMIAL
//
// Debug the LALR(1) parser for polynomials and factor tables:
//     #define DEBUG_PP_PARSER
//
// Testing primality:
//     #define DEBUG_PP_PRIMALITY_TESTING
//
// Factoring into primes:
//     #define DEBUG_PP_FACTOR
//
// Arbitrary precision arithmetic debugging:
//     #define DEBUG_PP_BIGINT
//
// Modulo p arithmetic debugging:
//     #define DEBUG_PP_ARITH
//
// Forces one or more unit tests to fail and generate test error messages.
// Default is to leave undefined.
//
//     #define DEBUG_PP_FORCE_UNIT_TEST_FAIL
//
// Turn on to check memory exceptions.  Default is to leave it off.
// This may crash some on machines with buggy C++ compilers and OS's.
//
//     #define DEBUG_PP_FORCE_MEMORY_OVERLOAD

// Define the basic integer types we will use for all modulus p calculations, 
// multiple precision arithmetic, polynomial operations and factoring.
// Higher precision will decrease your computation time according to profiling.



/*=============================================================================
 |
 | NAME
 |
 |     Basic integer types.
 |
 +============================================================================*/

// Microsoft Windows 7 64-bit + Visual C++ compiler (64 bit integers).
#if defined( _MSC_VER )   
    typedef unsigned long long ppuint ;
    typedef   signed long long ppsint ; 
// Microsoft Windows 7 64-bit + Cygwin (64 bit integers)
#elif defined( __CYGWIN__ )  
    typedef unsigned long long ppuint ;
    typedef   signed long long ppsint ;
// Mac OS X (64 bits) or other Unix system (hopefully 64 bits).
#else
    typedef unsigned long int ppuint ;
    typedef   signed long int ppsint ;
#endif

// Check if we have at least 64-bit arithmetic.
static_assert( 8 * sizeof( ppuint ) >= 64 || 8 * sizeof( ppsint ) >= 64,
               "Error:  basic integer types ppuint and ppsint must be at least 64-bits.  Sorry, you'll have to run on a computer with a 64-bit CPU." ) ;


/*=============================================================================
 |
 | NAME
 |
 |     PrimpolyError
 |
 | DESCRIPTION
 |
 |     Return status fed back to the Unix shell.
 |
 +============================================================================*/

enum class ReturnStatus
{
    Success       = 0,
    AskForHelp    = 1,
    PNotPrime     = 2,
    RangeError    = 3,
    InternalError = 4,
    Reserved      = 5
} ;


/*=============================================================================
 |
 | NAME
 |
 |     PrimpolyError
 |
 | DESCRIPTION
 |
 |     Top level error class for main.
 |
 +============================================================================*/

class PrimpolyError : public runtime_error
{
    public:
        // Throw with an error message.
        PrimpolyError( const string & description )
            : runtime_error( description )
        {
        } ;

        // Default throw with no error message.
        PrimpolyError()
            : runtime_error( "Polynomial error:  " )
        {
        } ;

} ; // end class PrimpolyError



/*=============================================================================
 |
 | NAME
 |
 |     Message strings.
 |
 | DESCRIPTION
 |
 |
 +============================================================================*/

static const string legalNotice
(
    "\n"
    "Primpoly Version 13.0 - A Program for Computing Primitive Polynomials.\n"
    "Copyright (C) 1999-2018 by Sean Erik O'Connor.  All Rights Reserved.\n"
   "\n"
    "Primpoly comes with ABSOLUTELY NO WARRANTY; for details see the\n"
    "GNU General Public License.  This is free software, and you are welcome\n"
    "to redistribute it under certain conditions; see the GNU General Public License\n"
    "for details.\n\n"
)  ;

static const string helpText
(
     "This program generates a primitive polynomial of degree n modulo p.\n"
     "\n"
     "Usage:  Primpoly p n\n"
     "          where p is a prime >= 2 and n is an integer >= 2\n"
     "\n"
     "        Primpoly -t ""<Polynomial to test>, p""\n"
     "          If you leave off the ,p we default to p = 2\n"
     "\n"
     "        Primpoly -a p n\n"
     "          Same, but list all primitive polynomials of degree n mod p\n"
     "\n"
     "        Primpoly -s p n\n"
     "          Same, but print search statistics too.\n"
     "\n"
     "        Primpoly -h\n"
     "          Print this help message.\n"
     "\n"
     "Examples:  \n"
     "        Primpoly 2 4 \n"
     "          Self-check passes...\n"
     "          Primitive polynomial modulo 2 of degree 4\n"
     "          x ^ 4 + x + 1, 2\n"
     "\n"
     "        Primpoly -t ""x^4 + x + 1, 2""\n"
     "          Self-check passes...\n"
     "          x ^ 4 + x + 1, 2 is primitive!\n"
     "\n"
     "        Primpoly -a 2 4\n"
     "          Self-check passes...\n"
     "          Primitive polynomial modulo 2 of degree 4\n"
     "          x ^ 4 + x + 1, 2\n"
     "          Primitive polynomial modulo 2 of degree 4\n"
     "          x ^ 4 + x ^ 3 + 1, 2\n"
     "\n"
     "        Primpoly.exe -s 13 19\n"
     "          Self-check passes...\n"
     "          Primitive polynomial modulo 13 of degree 19\n"
     "          x ^ 19 + 9 x + 2, 13\n"
     "\n"
     "          +--------- OperationCount --------------------------------\n"
     "          |\n"
     "          | Integer factorization:  Table lookup + Trial division + Pollard Rho\n"
     "          |\n"
     "          | Number of trial divisions :           0\n"
     "          | Number of gcd's computed :            9027\n"
     "          | Number of primality tests :           2\n"
     "          | Number of squarings:                  9026\n"
     "          |\n"
     "          | Polynomial Testing\n"
     "          |\n"
     "          | Total num. degree 19 poly mod 13 :      1461920290375446110677\n"
     "          | Number of possible primitive poly:    6411930599771980992\n"
     "          | Polynomials tested :                  120\n"
     "          | Const. coeff. was primitive root :    46\n"
     "          | Free of linear factors :              11\n"
     "          | Irreducible to power >=1 :            1\n"
     "          | Had order r (x^r = integer) :         1\n"
     "          | Passed const. coeff. test :           1\n"
     "          | Had order m (x^m != integer) :        1\n"
     "          |\n"
     "          +-----------------------------------------------------\n"
     "\n\n"
     "Primitive polynomials find many uses in mathematics and communications\n"
     "engineering:\n"
     "   * Generation of pseudonoise (PN) sequences for spread spectrum\n"
     "     communications and chip fault testing.\n"
     "   * Generating Sobol sequences for high dimensional numerical integration.\n"
     "   * Generation of CRC and Hamming codes.\n"
     "   * Generation of Galois (finite) fields for use in decoding Reed-Solomon\n"
     "     and BCH error correcting codes.\n"
     "\n"
     "For detailed technical information, see my web page\n"
     "    http://seanerikoconnor.freeservers.com/Mathematics/AbstractAlgebra/PrimitivePolynomials/overview.html\n"
     "\n"
) ;

static const string writeToAuthorMessage
(
    "Dear User,"
    "    Sorry you got an error message.  Please email the author at\n"
    "        seanerikoconnor!AT!gmail!DOT!com\n"
    "    with !DOT! replaced by . and the !AT! replaced by @\n"
#ifdef SELF_CHECK
    "    Attach the unitTest.log file which should be located\n"
    "    in the current directory and all console output from this program.\n"
#else
    "    It looks like you have the unit test self check compiled off."
    "    Please set #define SELF_CHECK in the Primpoly.h header file, compile and rerun."
#endif
    "Thanks for your help,\n"
    "Sean E. O'Connor\n"
    "\n"
) ;

static const string confirmWarning
(
    "Confirming polynomial is primitive with an independent check.\n"
    "Warning:  You may wait an impossibly long time!  If you lose patience,\n"
    "you can hit control-C in your console window to stop this program.\n"
) ;

#endif  //  End of wrapper for header.
