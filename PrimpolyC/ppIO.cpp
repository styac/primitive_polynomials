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
#include <fstream>
#include <iomanip>

#include "Primpoly.h"
#include "ppIO.h"

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


--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

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
                        )
{
    int    input_arg_index;
    char * input_arg_string;
    char * option_ptr;

    int    num_arg;
    char * arg_string[ _MAX_PATH ];

    /*  Initialize to defaults. */
    *testPolynomialForPrimitivity = NO;
    *listAllPrimitivePolynomials  = NO;
    *printStatistics              = NO;
    *printHelp                    = NO;
    *selfCheck                    = NO;
    *print_hex                    = NO;
    *p                            = 0;
    *n                            = 0;
    testPolynomial                = (int *) 0;

    /*
     *  Parse the command line to get the options and their inputs.
     */

    for (input_arg_index = 0, num_arg = 0;  input_arg_index < argc;
         ++input_arg_index)
    {
        /*  Get next argument string. */
        input_arg_string = argv[ input_arg_index ];

        /* We have an option:  a hyphen followed by a non-null string. */
        if (input_arg_string[ 0 ] == '-' && input_arg_string[ 1 ] != '\0')
        {
            /* Scan all options. */
            for (option_ptr = input_arg_string + 1;  *option_ptr != '\0';
                 ++option_ptr)
            {
                switch( *option_ptr )
                {
                    /* Test a given polynomial for primitivity. */
                    case 't':
                        *testPolynomialForPrimitivity = YES;
                    break;

                    /* List all primitive polynomials.  */
                    case 'a':
                       *listAllPrimitivePolynomials = YES;
                    break;

                    /* Print statistics on program operation. */
                    case 's':
                       *printStatistics = YES;
                    break;

                    /* Print help. */
                    case 'h':
                    case 'H':
                        *printHelp = YES;
                    break;

                    /* Turn on all self-checking (slow!).  */
                    case 'c':
                        *selfCheck = YES;
                    break;

                    /* hex print */
                    case 'x':
                        *print_hex = YES;
                    break;

                    default:
                        std::cout << "Cannot recognize the option " << *option_ptr << std::endl;
                    break;
                }
            }
        }
        else  /* Not an option, but an argument. */
        {
            arg_string[ num_arg++ ] = input_arg_string;
        }
    }


    /* Assume the next two arguments are p and n. */
    if (num_arg == 3)
    {
        *p = atoi( arg_string[ 1 ] );
        *n = atoi( arg_string[ 2 ] );

    }
    else
    {
        std::cout <<"ERROR:  Expecting two arguments, p and n.\n\n"  << std::endl;
        *printHelp = YES;
    }

    return 0;
} /* ================ end of function parse_command_line ==================== */

void write_poly( int * a, int n )
{

    int
        k,                         /*  Loop conter. */
        coeff,                     /*  Coefficient a sub k.  */
        first_time_through = YES,  /* = NO after printing the first coefficient. */
        num_terms = 0;            /*  Number of terms printed so far.  */

        for (k = n;  k >= 0;  --k)
        {

            /*
                              k
            Print the term a x  if a  is nonzero.
                            k       k
            */

            if ( (coeff = a[ k ]) != 0)
            {

               /* After the first time thorough, print leading plus signs "+" to
                  separate the terms of the polynomial. */

                if (!first_time_through)
                    std::cout << " + ";

                first_time_through = NO;

                /* Always print the constant term a  but print terms of higher degree
                                                   0
                   only if a  is not 1.
                            k             */

                if ( (coeff != 1) || (k == 0) )
                    std::cout << coeff;

                /*                  k                             1      0
                    Print the term x.  Exceptions:  print x, not x, omit x  */

                if (k > 1)
                    std::cout << "x^" << k;

                else if (k == 1)
                    std::cout << " x";

                /*  Start a new line. */

                if (++num_terms % NUMTERMSPERLINE == 0)

                    std::cout << std::endl;

            } /* end if coeff > 0 */

        } /* end for */

        std::cout << std::endl;
} /* ======================= end of function write_poly ===================== */

void write_poly_hex( int * a, int n, std::ofstream& file )
{
    std::uint64_t high = 0u;
    std::uint64_t low = 0u;
    int width = n/4;

    if( n > 64 )
    {
        width = 16;
        for( int i=MAXDEGPOLY; i>64; --i )
        {
            high *= 2;
            high |= a[i+1];
        }
    }
    else
    {
        if( width <= 2 )
        {
            width = 2;
        }
        else if( width <= 4 )
        {
            width = 4;
        }
        else if( width <= 8 )
        {
            width = 8;
        }
        else
        {
            width = 16;
        }
    }

    for( int i=63; i>=0; --i )
    {
        low *= 2;
        low |= a[i+1];
    }

    if( high == 0 )
    {
        file << std::hex << std::setw(width) << std::uppercase << std::setfill('0')
             << low << std::endl;
    }
    else
    {
        file << std::hex << std::setw(width) << std::uppercase << std::setfill('0')
             << high << " " << std::setw(width)
             << low << std::endl;
    }
} /* ======================= end of function write_poly_hex ===================== */


