/*==============================================================================
|
|  NAME
|
|     ppUnitTest.h
|
|  DESCRIPTION
|
|     Header file for unit test routines.
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
#ifndef __PP_UNITTEST_H__
#define __PP_UNITTEST_H__


/*=============================================================================
|
| NAME
|
|     unitTest
|
| DESCRIPTION
|
|     In the spirit of extreme programming, test each class and its member
|     functions.  Run the tests every time we run this application.
|
+============================================================================*/

bool unitTest() ;

// Subtest functions.
bool unitTestSystemFunctions( ostream & fout ) ;
bool unitTestBigIntBase10( ostream & fout ) ;
bool unitTestBigIntDefaultBase( ostream & fout ) ;
bool unitTestModPArithmetic( ostream & fout ) ;
bool unitTestFactoring( ostream & fout ) ;
bool unitTestPolynomials( ostream & fout ) ;
bool unitTestPolynomialOrder( ostream & fout ) ;
bool unitTestParser( ostream & fout ) ;

#endif // __PP_UNITTEST_H__ -- End of wrapper for header file.
