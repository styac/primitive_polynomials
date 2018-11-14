#pragma once

/*==============================================================================
| 
|  File Name:     
|
|     ppIO.c
|
|  Description:   
|
|     Command line parsing and polynomial pretty printing.
| 
|  Functions:
|
|      parse_command_line
|      write_poly
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
|     The author's address is artificer!AT!seanerikoconnor!DOT!freeservers!DOT!com
|     with !DOT! replaced by . and the !AT! replaced by @
|
==============================================================================*/

/*------------------------------------------------------------------------------
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include <stdlib.h> /* for _MAX_PATH */
#include "Primpoly.h"

/*==============================================================================
|                               parse_command_line                             |
================================================================================

DESCRIPTION

     Parse the command line.

INPUT
                                                                     n
     a[]  (int *)  Coefficients of the nth degree polynomial a(x) = a x  + ...
                                                                     n
                   + a  x  +  a.    a  is stored at array location a[i], 
                      1        0     i
                   0 <= i < n .

     n    (int)    The polynomial's degree.

OUTPUT

  testPolynomialForPrimitivity 
    

EXAMPLE CALLING SEQUENCE

   pp -h                   Prints help.
   pp -s 2 4               Prints search statistics.
   pp -t 2 4 x^3+x^2+1     Checks a polynomial for primitivity.  No blanks, please!
   pp -a 2 4               Lists all primitive polynomials of degree 4 modulo 2.
   pp -c 2 4               Does a time-consuming double check on primitivity.

METHOD

    

BUGS

    None.

*/

int parse_command_line( int    argc, 
					    char * argv[], 
                        int *  testPolynomialForPrimitivity,
                        int *  listAllPrimitivePolynomials,
                        int *  printStatistics,
                        int *  printHelp,
                        int *  selfCheck,
                        int *  p,
                        int *  n,
                        int *  testPolynomial,
                        int *  print_hex
                        );

/*==============================================================================
|                                    write_poly                                |
================================================================================

DESCRIPTION

     Print a polynomial in a nice format.  Omit terms with coefficients of zero.
     Suppress coefficients of 1, but not the constant term.  Put every
     NUMTERMSPERLINE terms in the polynomial on a new line.

INPUT
                                                                     n
     a[]  (int *)  Coefficients of the nth degree polynomial a(x) = a x  + ...
                                                                     n
                   + a  x  +  a.    a  is stored at array location a[i], 
                      1        0     i
                   0 <= i < n .

     n    (int)    The polynomial's degree.

OUTPUT

    Standard output    A pretty-printed polynomial.

EXAMPLE CALLING SEQUENCE

    If array a[] contains 

    a[0] = 1
    a[1] = 2
    a[2] = 0
    a[3] = 1

    and n = 3,

    write_poly( a, n ) prints

    x ^ 3  +  2 x  + 1

METHOD

    Sheer hacking ingenuity (vulgarity?).

BUGS

    None.
*/

void write_poly( int * a, int n );

void write_poly_hex( int * a, int n, std::ofstream& file_hex);


