/*==============================================================================
|
|  NAME
|
|     ppParser.cpp
|
|  DESCRIPTION
|
|     Polynomial parser classes.
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
|                                Include Files                                 |
------------------------------------------------------------------------------*/

#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <new>          // set_new_handler()
#include <limits>       // numeric_limits
#include <cmath>        // Basic math functions e.g. sqrt()
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;

#include "ctype.h"       // C string functions.


/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.h"		  // Global functions.
#include "ppArith.h"		  // Basic arithmetic functions.
#include "ppBigInt.h"         // Arbitrary precision integer arithmetic.
#include "ppOperationCount.h" // OperationCount collection for factoring and poly finding.
#include "ppFactor.h"         // Prime factorization and Euler Phi.
#include "ppPolynomial.h"	  // Polynomial operations and mod polynomial operations.
#include "ppParser.h"	      // Parsing of polynomials and I/O services.
#include "ppUnitTest.h"       // Complete unit test.


/*------------------------- Parser Helper Classes ----------------------------*/


/*=============================================================================
 |
 | NAME
 |
 |     Symbol
 |
 | DESCRIPTION
 |
 |     Default constructor for a parser symbol, which has a type (terminal or non-terminal)
 |     and a state (or production number).
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Symbol< SymbolType, ValueType >::Symbol()
: type_(), value_(), state_()
{
}


/*=============================================================================
 |
 | NAME
 |
 |     Symbol
 |
 | DESCRIPTION
 |
 |     Constructor for a parser symbol, which has a type (terminal or non-terminal)
 |     and a state (or production number).
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Symbol< SymbolType, ValueType >::Symbol( SymbolType type, int state )
: type_( type ), value_(), state_( state )
{
}


/*=============================================================================
 |
 | NAME
 |
 |     Symbol
 |
 | DESCRIPTION
 |
 |     Constructor for a parser symbol, which has a type (terminal or non-terminal)
 |     a value (see above) and a state (or production number).
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Symbol< SymbolType, ValueType >::Symbol( SymbolType type, int state, ValueType value )
: type_( type ), state_( state ), value_( value )
{
}

/*=============================================================================
 |
 | NAME
 |
 |     Symbol
 |
 | DESCRIPTION
 |
 |     Copy constructor.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Symbol< SymbolType, ValueType >::Symbol( const Symbol< SymbolType, ValueType > & s )
: type_( s.type_ )
, value_( s.value_ )
, state_( s.state_ )
{
}


/*=============================================================================
 |
 | NAME
 |
 |     operator=
 |
 | DESCRIPTION
 |
 |     Assignment operator.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Symbol< SymbolType, ValueType > & Symbol< SymbolType, ValueType >::operator=( const Symbol< SymbolType, ValueType > & s )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &s)
        return *this ;
    
    type_  = s.type_ ;
    value_ = s.value_ ;
    state_ = s.state_ ;
    
    // Pass back the object so we can concatenate assignments.
    return *this ;
}


/*=============================================================================
 |
 | NAME
 |
 |     ActionState
 |
 | DESCRIPTION
 |
 |     Default constructor for an action/state pair.  Action is shift, reduce, 
 |     etc. and state is a number.
 |
 +============================================================================*/

ActionState::ActionState()
    : action_( Action::Error )
    , state_( 0 )
{
}


/*=============================================================================
 |
 | NAME
 |
 |     ActionState
 |
 | DESCRIPTION
 |
 |     Constructor for an action/state pair.
 |
 +============================================================================*/

ActionState::ActionState( Action type, int state )
    : action_( type )
    , state_( state )
{
}


/*=============================================================================
 |
 | NAME
 |
 |     ActionState
 |
 | DESCRIPTION
 |
 |     Copy constructor for an action/state pair.
 |
 +============================================================================*/

ActionState::ActionState( const ActionState & as )
    : action_(  as.action_ )
    , state_( as.state_ )
{
}


/*=============================================================================
 |
 | NAME
 |
 |     ActionState
 |
 | DESCRIPTION
 |
 |     Assignment operator for an action/state pair.
 |
 +============================================================================*/

ActionState & ActionState::operator=( const ActionState & as )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &as)
        return *this ;
    
    action_  = as.action_ ;
    state_ = as.state_ ;
    
    // Pass back the object so we can concatenate assignments.
    return *this ;
}


/*=============================================================================
 |
 | NAME
 |
 |     Operator << for ActionState
 |
 | DESCRIPTION
 |
 |     Debug print to an output stream for an action state.
 |
 +============================================================================*/

ostream & operator<<( ostream & out, const ActionState & as )
{
    string s ;
    
    out << "Action/State: " ;
    switch( as.action_ )
    {
        case Action::Shift:  s = "Shift" ;   break ;
        case Action::Reduce: s = "Reduce" ;  break ;
        case Action::Accept: s = "Accept" ;  break ;
        case Action::Error:  s = "Error" ;   break ;
    }
    out << s ;
    out << " " << as.state_ ;
    
    return out ;
}


/*----------------------------- Parser Base Class ----------------------------*/


/*=============================================================================
 |
 | NAME
 |
 |     Parser
 |
 | DESCRIPTION
 |
 |     Default constructor for the class.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Parser< SymbolType, ValueType >::Parser()
    : inputStack_()
    , parseStack_()
    , NumStates_       ( 0 ) // Includes the 0 state.
    , NumProductions_  ( 0 ) // Number of productions.
    , NumTerminals_    ( 0 ) // Number of terminal symbols.
    , NumNonTerminals_ ( 0 ) // Number of nonterminal symbols.
{
}

/*=============================================================================
 |
 | NAME
 |
 |     ~Parser
 |
 | DESCRIPTION
 |
 |     Default deeeestructor for the class.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Parser< SymbolType, ValueType >::~Parser()
{
    // Private data destroy themselves.
}


/*=============================================================================
 |
 | NAME
 |
 |     Parser( const Parser & Parser )
 |
 | DESCRIPTION
 |
 |     Copy constructor.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Parser< SymbolType, ValueType >::Parser( const Parser & Parser )
: inputStack_( Parser.inputStack_ )
, parseStack_( Parser.parseStack_ )
{
}

/*=============================================================================
 |
 | NAME
 |
 |    operator=( const PolyParser & PolyParser )
 |
 |
 | DESCRIPTION
 |
 |    Assignment operator.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
Parser< SymbolType, ValueType > & Parser< SymbolType, ValueType >::operator=( const Parser & Parser )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &Parser)
        return *this ;
    
    inputStack_ = Parser.inputStack_ ;
    parseStack_ = Parser.parseStack_ ;
    
    // Return reference to this object.
    return *this ;
}

/*=============================================================================
 |
 | NAME
 |
 |     insertAction
 |
 | DESCRIPTION
 |
 |     Add a new entry to the Action Table.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
void Parser< SymbolType, ValueType >::insertAction( int    state,      SymbolType terminal,
             Action actionType, int      actionState )
{
    try
    {
        // Create a new action.
        ActionState as( actionType, actionState ) ;
        
        // Get space for actionTable[ state ] which is an vector<ActionState> indexed by terminal.
        actionTable_[ static_cast<int>( state ) ].resize( NumTerminals_ ) ;
        
        // Insert it into the ACTION table.
        actionTable_[ static_cast<int>( state ) ][ static_cast<int>( terminal ) ] = as ;
    }
    catch( length_error & e )
    {
        ostringstream os ;
        os << "Error:  Cannot initialize PolyParser action tables "
        << " at " << __FILE__ << ": line " << __LINE__  ;
        throw ParserError( os.str() ) ;
    }
    
}

/*=============================================================================
 |
 | NAME
 |
 |    insertGoto
 |
 | DESCRIPTION
 |
 |     Add a new entry to the Goto Table.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
void Parser< SymbolType, ValueType >::insertGoto( int state, SymbolType nonterm, int newState )
{
    try
    {
        // Insert into the GOTO table.
        gotoTable_[ static_cast<int>( state ) ].resize( static_cast<int>( PolySymbol::NumSymbols ) ) ;
        gotoTable_[ static_cast<int>( state ) ][ static_cast<int>( nonterm ) ] = newState ;
    }
    catch( length_error & e )
    {
        ostringstream os ;
        os << "Error:  Cannot initialize Parser goto tables "
        << " at " << __FILE__ << ": line " << __LINE__  ;
        throw ParserError( os.str() ) ;
    }
    
}

/*=============================================================================
 |
 | NAME
 |
 |    insertProduction
 |
 | DESCRIPTION
 |
 |    Add a new entry to the list of productions.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
void Parser< SymbolType, ValueType >::insertProduction( int prodNum, SymbolType nonTerm, int rhsLength )
{
    // Insert into the production table.
    production_[ prodNum ].type_  = nonTerm ;
    production_[ prodNum ].state_ = rhsLength ;
}

/*=============================================================================
 |
 | NAME
 |
 |     insertErrorMessage
 |
 | DESCRIPTION
 |
 |     Insert an error message into the list.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
void Parser< SymbolType, ValueType >::insertErrorMessage( int state, string errorMessage )
{
    errorMessage_[ state ] = errorMessage ;
}

/*=============================================================================
 | 
 | NAME
 | 
 |     parse
 | 
 | DESCRIPTION
 | 
 |      Standard LALR(1) parser algorithm, parameterized with the type of
 |      symbols and values of its tokens.
 |
 |      TODO:  factor out the syntax directed translation!  then move this to
 |      the parent class.
 |
 |        The initial parser configuration is
 | 
 |             (s0 | a1 ... an $)
 | 
 |         a1 ... an is the set of input tokens
 |         s0 = 0 is the initial state
 | 
 |         The parse stack is to the left of the bar and the unprocessed
 |         input is to the right.  Now suppose the configuration is
 | 
 |             (s0 X1 ... Xm-r sm-r Xm-r+1 sm-r+1 ... Xm sm   |   ai ai+1 ... an $)
 | 
 |         There are four possible things we can do:
 | 
 |         (1)  Shift the input token and push a new state.
 | 
 |              ACTION[ sm, ai ] = shift s
 |
 |                  (s0 X1 ... Xm sm ai s   |   ai+1 ... an $)
 | 
 |         (2)  Reduce.  Pop the stack, push the reduction and new state.
 |              Also do a syntax-directed evaluation using the values
 |              of the symbols in the production A -> beta which we just reduced.
 | 
 |              ACTION[ sm, ai ] = reduce( A -> beta = Xm-r+1 ... Xm)
 |
 |                  (s0 X1 ... Xm-r sm-r A s   |   ai+1 ... an $)
 | 
 |              where s = GOTO[ sm-r, A ] and r = length( beta )
 | 
 |         (3)  Accept.  The sentence is in the grammar;  halt.
 | 
 |              ACTION[ sm, ai ] = accept
 | 
 |         (4)  Error state.   Produce an error message using the current
 |              parsing state and lookahead symbol ai.
 | 
 |              ACTION[ sm, ai ] = error
 | 
 +============================================================================*/

template< typename SymbolType, typename ValueType >
ValueType Parser< SymbolType, ValueType >::parse( string sentence )
{
    ValueType retVal ; // Default to f(x) = 0, mod 0.

    // Null string check.
    if (sentence.size() == 0)
        return retVal ;

    // Tokenize the input string.  This can throw an exception.
    tokenize( sentence ) ;

    // Initialize the parse stack to the zero state.
    parseStack_.clear() ;

    Symbol<SymbolType, ValueType> s( SymbolType::S, 0 ) ;
    parseStack_.push_back( s ) ;

    #ifdef DEBUG_PP_PARSER
    cout << "Parser:  sentence = " << sentence << endl ;
    #endif

    // Parse until we accept, error out or run out of input.
    while( inputStack_.size() > 0 )
    {
        // Grab the current state and the lookahead token.
        int                             currentState = parseStack_.back().state_ ;
        Symbol< SymbolType, ValueType > lookahead    = inputStack_.back() ;


        // Dump the parse and input stacks for debugging.
        #ifdef DEBUG_PP_PARSER
        cout << endl << "    Parse stack: " << endl ;

        for (auto & i : parseStack_)
            cout << "        " << i << endl ;
 
        cout << "    --------" << endl ;
        
        cout << "    Input stack: " << endl ;
		auto i = inputStack_.end()-1 ;
        while (i >= inputStack_.begin())
        {
            cout << "        " << *i <<  endl ;
			if (i == inputStack_.begin())
			    break ;
			--i ;
        }
        #endif

        // Look up the action (shift 3), (reduce 2), etc.
        ActionState actionState = actionTable_[ currentState ][ static_cast<int>( lookahead.type_ ) ] ;

        // Debug dump the action.
        #ifdef DEBUG_PP_PARSER
        cout << endl << actionState << endl ;
        #endif

        try
		{
			// Look at the action required.
			switch( actionState.action_ )
			{
				case Action::Shift:
				{
					//  Shift the next token off the input stack.
					if (inputStack_.size() == 0)
						// Throw a parser error if parse stack is empty (shouldn't get here).
						throw ParserError( "Wanted to shift another token, but the parse stack is empty." ) ;

					inputStack_.pop_back() ;

					//  Push the input symbol and new state onto the parse stack.
					int newState = actionState.state_ ;

					Symbol< SymbolType, ValueType > s( lookahead.type_, newState, lookahead.value_ ) ;
					parseStack_.push_back( s ) ;
				}
				break ;

				case Action::Reduce:
				{
					// Fetch the production A -> beta, beta = Xm-r+1 ... Xm
					int productionNum = actionState.state_ ;

					// Get the left hand side non-terminal A.
					SymbolType prodNonTerm = production_[ productionNum ].type_ ;

					// Get r, the length of beta.  If the production is A -> EPSILON, beta = EPSILON, r = 0.
					int rhsProductionLength = production_[ productionNum ].state_ ;

					// Carry out the syntax directed translation using
					// the symbols of beta and its states
					// which are on the top of the stack and the nonterminal A.

					// Creat a new symbol/state  A s
                    Symbol< SymbolType, ValueType > A( prodNonTerm,  0 ) ;
					int topIndex = static_cast<int>( parseStack_.size() ) - 1 ;
					int i = 0 ;
                    
                    // Carry out the syntax directed translation f() using the values in the right hand
                    // side of the production:
                    //     A.value = f( Xm-r+1.value ... Xm.value )
                    syntaxDirectedTranslation( productionNum, topIndex, A ) ;

					// Pop the symbols of beta and its states
					//     Xm-r+1 sm-r+1 ... Xm sm
					// off the stack.
					for (i = 1 ;  i <= rhsProductionLength ;  ++i)
					{
						if (inputStack_.size() == 0)
							return retVal ;
						parseStack_.pop_back() ;
					}

					// Get the GOTO state s.
					currentState = parseStack_.back().state_ ;
					int gotoState = gotoTable_[ static_cast<int>( currentState ) ][ static_cast<int>( prodNonTerm ) ] ;

					// Push nonterminal A and goto state s.
					A.state_ = gotoState ;
					parseStack_.push_back( A ) ;
				}
				break ;

				// Parsed successfully.
				case Action::Accept:
					#ifdef DEBUG_PP_PARSER
					cout << "    Accept:  S.value = " << parseStack_.back().value_ << endl ;
					#endif

					return parseStack_.back().value_ ;
				break ;

				case Action::Error:
				{
                    #ifdef DEBUG_PP_PARSER
                    cout << "    Error: " << s << endl ;
                    #endif

					// Throw a parser error with an error message based on the current state.
                    ostringstream os ;
                    os << errorMessage_[ currentState ] << " in sentence " << sentence ;
                    throw ParserError( os.str() ) ;
                    
					return retVal ;
				}
				break ;

				default:
					// Internal error.
					return retVal ;
			}   // end switch
		
		}
		catch( length_error & e )
	    {
			ostringstream os ;
			os << "Error:  parser cannot resize poly or power terms. "
			<< " at " << __FILE__ << ": line " << __LINE__  ;
			throw ParserError( os.str() ) ;
	    }  

    } // end while

    // Shouldn't get here.
    return retVal ;
}



/*------------------------------- Parser Child Classes ------------------------*/



/*=============================================================================
 |
 | NAME
 |
 |     operator=( const PolyValue & v )
 |
 |
 | DESCRIPTION
 |
 |     Assignment operator for PolyValue.
 |
 +============================================================================*/

PolyValue & PolyValue::operator=( const PolyValue & v )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &v)
        return *this ;
    
    scalar_ = v.scalar_ ;
    f_ = v.f_ ;
    
    // Pass back the object so we can concatenate assignments.
    return *this ;
}


/*=============================================================================
 |
 | NAME
 |
 |     Operator << for PolyValue
 |
 | DESCRIPTION
 |
 |     Print the scalar and polynomial parts of a PolyValue type.
 |
 +============================================================================*/

ostream & operator<<( ostream & out, const PolyValue & poly )
{
    out << poly.scalar_ << " f( x ) = " ;
    
    // Convert into a Polynomial type from a vector, then send to output.
    Polynomial p( poly.f_ ) ;
    out << p ;
        
    return out ;
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyValue
 |
 | DESCRIPTION
 |
 |     Default constructor for a value which contains a scalar or polynomial.
 |
 +============================================================================*/

PolyValue::PolyValue()
: scalar_( 0 )
, f_(1, 0) // f(x) = 0
{
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyValue
 |
 | DESCRIPTION
 |
 |     Copy constructor for PolyValue.
 |
 +============================================================================*/

PolyValue::PolyValue( const PolyValue & v )
: scalar_( v.scalar_ )
, f_( v.f_ )
{
}


/*=============================================================================
|
| NAME
|
|     PolyParser
|
| DESCRIPTION
|
|     Default constructor for the class.
|
+============================================================================*/

template < typename SymbolType, typename ValueType >
PolyParser< SymbolType, ValueType >::PolyParser()
    : Parser< SymbolType, ValueType >()
    , testPolynomial_()
    , testPolynomialForPrimitivity_( false )
    , listAllPrimitivePolynomials_( false )
    , printOperationCount_( false )
    , printHelp_( false )
    , slowConfirm_( false )
    , p( 0 )
    , n( 0 )
{
    // Initialize all the parse tables.
    initializeTables() ;
}

/*=============================================================================
 | 
 | NAME
 | 
 |     ~PolyParser
 | 
 | DESCRIPTION
 | 
 |     Default deeeestructor for the class.
 | 
 +============================================================================*/

template < typename SymbolType, typename ValueType >
PolyParser< SymbolType, ValueType >::~PolyParser()
{
    // Private data destroy themselves.
}



/*=============================================================================
 |
 | NAME
 |
 |     PolyParser( const PolyParser & PolyParser )
 |
 | DESCRIPTION
 |
 |     Copy constructor.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
PolyParser<SymbolType, ValueType>::PolyParser( const PolyParser< SymbolType, ValueType > & PolyParser )
        : Parser<SymbolType, ValueType>( PolyParser )
{
}


/*=============================================================================
 | 
 | NAME
 | 
 |    operator=( const PolyParser & PolyParser )
 | 
 | 
 | DESCRIPTION
 | 
 |    Assignment operator.
 | 
 +============================================================================*/
        
template < typename SymbolType, typename ValueType >
PolyParser< SymbolType, ValueType > & PolyParser< SymbolType, ValueType >::operator=( const PolyParser & PolyParser )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &PolyParser)
             return *this ;

    inputStack_ = PolyParser.inputStack_ ;
    parseStack_ = PolyParser.parseStack_ ;

    // Return reference to this object.
    return *this ;
}


/*=============================================================================
 | 
 | NAME
 | 
 |     initializeTables
 | 
 | DESCRIPTION
 | 
 |     Initialize ACTION, GOTO, and ERROR_MESSAGE tables for an LALR(1) parser.
 |     And also information about the productions.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
    void PolyParser< SymbolType, ValueType >::
        initializeTables()
{
    // Need these so we can preallocate the vector lengths.  Vector's operator[] is unchecked!
    NumStates_       = 15 ; // Including the 0 state.
    NumProductions_  = 11 ;
    NumTerminals_    = static_cast<int>(PolySymbol::NumTerminals) ;
    NumNonTerminals_ = static_cast<int>(PolySymbol::NumSymbols) - static_cast<int>(PolySymbol::NumTerminals) - 1 ;  // Don't count NumTerminals enum itself.

    // Expand the rows of the ACTION table to hold all the states.
    actionTable_.resize( NumStates_ ) ;
    
    // Expand the rows of the GOTO table to hold all the states.
    gotoTable_.resize( NumStates_ ) ;
    
    // Expand the rows of the ACTION table to hold all the states.
    // Allow space in 0th slot (unused).
    production_.resize( NumProductions_ + 1 ) ;
    
    errorMessage_.resize( NumStates_ ) ;
    
    // TODO:  does resize or operator[] throw any exceptions?  use vector.at( i ) for checked access?
    try
	{
		// For each state and each terminal, insert
        //    if Shift, the new state to push on the stack,
        //    if Reduce, the production number.
		insertAction( 0,    PolySymbol::Integer,    Action::Shift,       3 ) ;
		insertAction( 0,    PolySymbol::Ecks,       Action::Reduce,      8 ) ;
		insertAction( 0,    PolySymbol::Comma,      Action::Reduce,      8 ) ;
		insertAction( 0,    PolySymbol::Dollar,     Action::Reduce,      8 ) ;
		insertAction( 0,    PolySymbol::Plus,       Action::Reduce,      8 ) ;

		insertAction( 1,    PolySymbol::Dollar,     Action::Accept,      0 ) ;

		insertAction( 2,    PolySymbol::Comma,      Action::Shift,       7 ) ;
		insertAction( 2,    PolySymbol::Plus,       Action::Shift,       8 ) ;
		insertAction( 2,    PolySymbol::Dollar,     Action::Reduce,      3 ) ;

		insertAction( 3,    PolySymbol::Ecks,       Action::Reduce,      7 ) ;
		insertAction( 3,    PolySymbol::Comma,      Action::Reduce,      7 ) ;
		insertAction( 3,    PolySymbol::Dollar,     Action::Reduce,      7 ) ;
		insertAction( 3,    PolySymbol::Plus,       Action::Reduce,      7 ) ;

		insertAction( 4,    PolySymbol::Comma,      Action::Reduce,      5 ) ;
		insertAction( 4,    PolySymbol::Dollar,     Action::Reduce,      5 ) ;
		insertAction( 4,    PolySymbol::Plus,       Action::Reduce,      5 ) ;

		insertAction( 5,    PolySymbol::Ecks,       Action::Shift,      10 ) ;
		insertAction( 5,    PolySymbol::Comma,      Action::Reduce,     11 ) ;
		insertAction( 5,    PolySymbol::Dollar,     Action::Reduce,     11 ) ;
		insertAction( 5,    PolySymbol::Plus,       Action::Reduce,     11 ) ;

		insertAction( 6,    PolySymbol::Dollar,     Action::Reduce,      1 ) ;

		insertAction( 7,    PolySymbol::Integer,    Action::Shift,      11 ) ;

		insertAction( 8,    PolySymbol::Integer,    Action::Shift,       3 ) ;
		insertAction( 8,    PolySymbol::Ecks,       Action::Reduce,      8 ) ;
		insertAction( 8,    PolySymbol::Comma,      Action::Reduce,      8 ) ;
		insertAction( 8,    PolySymbol::Dollar,     Action::Reduce,      8 ) ;
		insertAction( 8,    PolySymbol::Plus,       Action::Reduce,      8 ) ;

		insertAction( 9,    PolySymbol::Comma,      Action::Reduce,      6 ) ;
		insertAction( 9,    PolySymbol::Dollar,     Action::Reduce,      6 ) ;
		insertAction( 9,    PolySymbol::Plus,       Action::Reduce,      6 ) ;

		insertAction( 10,   PolySymbol::Comma,      Action::Reduce,      9 ) ;
		insertAction( 10,   PolySymbol::Exp,        Action::Shift,      13 ) ;
		insertAction( 10,   PolySymbol::Dollar,     Action::Reduce,      9 ) ;
		insertAction( 10,   PolySymbol::Plus,       Action::Reduce,      9 ) ;

		insertAction( 11,   PolySymbol::Dollar,     Action::Reduce,      2 ) ;

		insertAction( 12,   PolySymbol::Comma,      Action::Reduce,      4 ) ;
		insertAction( 12,   PolySymbol::Dollar,     Action::Reduce,      4 ) ;
		insertAction( 12,   PolySymbol::Plus,       Action::Reduce,      4 ) ;

		insertAction( 13,   PolySymbol::Integer,    Action::Shift,      14 ) ;

		insertAction( 14,   PolySymbol::Comma,      Action::Reduce,     10 ) ;
		insertAction( 14,   PolySymbol::Dollar,     Action::Reduce,     10 ) ;
		insertAction( 14,   PolySymbol::Plus,       Action::Reduce,     10 ) ;


		// For each state and symbol insert a new state.
		insertGoto( 0,  PolySymbol::S,               1 ) ;
		insertGoto( 0,  PolySymbol::Poly,            2 ) ;
		insertGoto( 0,  PolySymbol::Term,            4 ) ;
		insertGoto( 0,  PolySymbol::Multiplier,      5 ) ;

		insertGoto( 2,  PolySymbol::Mod,             6 ) ;

		insertGoto( 5,  PolySymbol::Power,           9 ) ;

		insertGoto( 8,  PolySymbol::Term,           12 ) ;
		insertGoto( 8,  PolySymbol::Multiplier,      5 ) ;


		// For each production (given by a unique number), insert the single nonterminal
        // which starts the production, and the length in symbols of the right hand side.
        // We'll need to know which production is being reduced so we can apply the syntax
        // directed translation, and pop the production off the stack.
		insertProduction(  1,      PolySymbol::S,           2 ) ; // S          -> Poly Mod
		insertProduction(  2,      PolySymbol::Mod,         2 ) ; // Mod        -> , integer
		insertProduction(  3,      PolySymbol::Mod,         0 ) ; // Mod        -> EPSILON
		insertProduction(  4,      PolySymbol::Poly,        3 ) ; // Poly       -> Poly + Term
		insertProduction(  5,      PolySymbol::Poly,        1 ) ; // Poly       -> Term
		insertProduction(  6,      PolySymbol::Term,        2 ) ; // Term       -> Multiplier Power
		insertProduction(  7,      PolySymbol::Multiplier,  1 ) ; // Multiplier -> integer
		insertProduction(  8,      PolySymbol::Multiplier,  0 ) ; // Multiplier -> EPSILON
		insertProduction(  9,      PolySymbol::Power,       1 ) ; // Power      -> x
		insertProduction( 10,      PolySymbol::Power,       3 ) ; // Power      -> x ^ integer
		insertProduction( 11,      PolySymbol::Power,       0 ) ; // Power      -> EPSILON

        // For each state, provide an error message.
		insertErrorMessage(  0, "Expecting to see the start of the polynomial or next term or coefficient" ) ;
		insertErrorMessage(  1, "Expecting to see end of the polynomial" ) ;
		insertErrorMessage(  2, "Expecting to see mod or + term or , integer or end of polynomial" ) ;
		insertErrorMessage(  3, "Expecting to see x or , or end of the polynomial" ) ;
		insertErrorMessage(  4, "Expecting to see + or end of the polynomial" ) ;
		insertErrorMessage(  5, "Expecting to see a power after a coefficient or x or ," ) ;
		insertErrorMessage(  6, "Expecting to see ," ) ;
		insertErrorMessage(  7, "Expecting to see mod after ," ) ;
		insertErrorMessage(  8, "Expecting to see a term after a + or a term or coefficient" ) ;
		insertErrorMessage(  9, "Expecting to see , or end of polynomial after a term" ) ;
		insertErrorMessage( 10, "Expecting to see x^ or x or x ^ integer" ) ;
		insertErrorMessage( 11, "Expecting to see end of the polynomial after , integer" ) ;
		insertErrorMessage( 12, "Expecting to see , end of polynomial or + after a term" ) ;
		insertErrorMessage( 13, "Expecting to see an exponent after x ^" ) ;
		insertErrorMessage( 14, "Expecting to see , or + end of polynomial after x ^ integer" ) ;
	}
	catch( length_error & e)
	{
		ostringstream os ;
		os << "Error:  Cannot initialize PolyParser tables "
		<< " at " << __FILE__ << ": line " << __LINE__  ;
		throw ParserError( os.str() ) ;
    }  
}


/*=============================================================================
 |
 | NAME
 |
 |    tokenize
 |
 | DESCRIPTION
 |
 |    This is the lexical analyzer.  Given an input string, convert it into tokens
 |    to fill the input stack.
 |
 |    Each token has a type, which is a nonterminal symbol, and its value.
 |
 |    This specific lexer converts a polynomial string.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
void PolyParser< SymbolType, ValueType >::tokenize( string sentence )
{
	Symbol< SymbolType, ValueType > tok( PolySymbol::NumSymbols, 0 ) ;

    // Starting position to scan.
    unsigned int pos = 0 ;

    bool minusSignDetected = false ;

    // Clear the token stack.
    inputStack_.clear() ;
    
    #ifdef DEBUG_PP_PARSER
    cout << "Lexical analyzer:  sentence = " << sentence << endl ;
    #endif

    // Scan sentence to end.
    while( pos < sentence.length() )
    {
        // Skip whitespace.
        while (pos < sentence.length() && iswspace( sentence[ pos ]))
            ++pos ; // Advance to next token.

        // Read an integer.
        if (pos < sentence.size() && isdigit( sentence[ pos ]))
        {
            unsigned int num = 0 ;
            while( pos < sentence.size() && isdigit( sentence[ pos ]))
            {
				char asciiDigit[ 2 ] = "\0" ; asciiDigit[ 0 ] = sentence[ pos ] ;
                int digit = atoi( asciiDigit ) ;

                // Stop reading the next decimal digit if we're about to overflow.
                if (num > (BigInt::getBase() - digit) / 10)
                {
                    ostringstream os ;
                    os << "Error:  number about to overflow in tokenizer "
                       << " at digit = " << digit
                       << " at " << __FILE__ << ": line " << __LINE__  ;
                    throw ParserError( os.str() ) ;
                }

                num = 10 * num + digit ;
                ++pos ; // Advance to next token.
            }

            tok.type_          = PolySymbol::Integer ;
            tok.value_.scalar_ = num ;

            // Apply a minus sign to this number.
            if (minusSignDetected)
            {
                // Don't allow a minus sign for polynomial coefficients.
                ostringstream os ;
                os << "Error:  negative number for a polynomial coefficient"
                   << " digit = " << tok.value_.scalar_ 
                   << " is not allowed.  Numbers must be >= 0"
                   << " at " << __FILE__ << ": line " << __LINE__  ;
                throw ParserError( os.str() ) ;

                // TODO If we do handle negative coefficients in the future, we'll do something like this:
                //     ModP modp( p ) ;
                //     tok.value_.scalar_ = modp( -tok.value_.scalar_ ) ;
                //     minusSignDetected = false ;
            }
        }
        // A non-digit symbol.
        else
        {
            tok.value_.scalar_ = 0 ;

            // Read another type of token.
            switch( sentence[ pos ] )
            {
                case '+' :
                    tok.type_ = PolySymbol::Plus ;
                break ;

                // Make this look like a plus symbol to the parser:  we'll let this lexer
                // apply it to the next integer we see.  If there isn't an integer following
                // the minus sign, it's an error for the parser anyway!
                case '-' :
                    tok.type_ = PolySymbol::Plus ;
                    minusSignDetected = true ;
                break ;

                case '^' :
                    tok.type_ = PolySymbol::Exp ;
                break ;

                case 'x' :
                case 'X' :
                    tok.type_ = PolySymbol::Ecks ;
                break ;

                case ',' :
                    tok.type_ = PolySymbol::Comma ;
                break ;
                
                default:
                    // Throw a parser error of unexpected input if we see a bad symbol.
                    ostringstream os ;
                    os << "Error:  Unexpected symbol in lexical analyzer" << sentence[ pos ]
                    << " at " << __FILE__ << ": line " << __LINE__  ;
                    throw ParserError( os.str() ) ;
            } 
            // ----------------< from user >----------------------

            ++pos ; // Advance to next token.
        } // end else

        // Push token on stack.
        inputStack_.push_back( tok ) ;

        #ifdef DEBUG_PP_PARSER
        cout << "    Push token " << tok << endl ;
        #endif

    } // end while

    // At end of input, push the $ terminator symbol.
    tok.type_ = PolySymbol::Dollar ;
    tok.value_.scalar_ = 0 ;

    inputStack_.push_back( tok ) ;

    #ifdef DEBUG_PP_PARSER
    cout << "    Last token " << tok << endl ;
    #endif

    // Reverse all the symbols into our preferred order.
    reverse( inputStack_.begin(), inputStack_.end() ) ;
}

/*=============================================================================
 |
 | NAME
 |
 |    syntaxDirectedTranslation
 |
 | DESCRIPTION
 |
 |     We have a reduce action   
 | 
 |         ACTION[ sm, ai ] = reduce( A -> beta = Xm-r+1 ... Xm )  
 |
 |     using the production
 |                        
 |         A -> Xm-r+1 ... Xm
 |                              
 |     Here's the stack before the reduction:
 |
 |                                parseStack_[ topIndex-1 ]   parseStack_[ topIndex ]
 |                                                        v   v
 |                                                        v   v
 |         (s0 X1 ...    Xm-r sm-r      Xm-r+1 sm-r+1 ... Xm sm     |     ai ai+1 ... an $)
 | 
 |     and after the reduction:
 |
 |         (s0 X1 ...    Xm-r sm-r      A s                         |      ai+1 ... an $)
 |
 |     Carry out the syntax directed translation f () using the values in the right hand
 |     side of the ith production,                i
 |
 |         A.value = f ( Xm-r+1.value ... Xm.value )
 |                    i 
 |
 +============================================================================*/

template <typename SymbolType, typename ValueType>
void PolyParser<SymbolType, ValueType>::syntaxDirectedTranslation( int productionNum, int topIndex, 
                                                                   Symbol<SymbolType,ValueType> & symbol )
{
    switch( productionNum )
    {
        // Reduce (S -> POLY MOD)
        case 1:
        {
            // I'll explain this syntax-directed translation in detail.  The other cases are similar.
            // Looking at the production above, we have the right hand side symbols on the parse stack
            // along with their values:
            //     parse stack = (POLY MOD |
            // Now we compute the value of S (denoted by the variable as) from POLY and MOD.
            // S is the final result.  Its polynomial value is the value of POLY and its modulus is the
            // value of MOD.
            symbol.value_.f_      = parseStack_[ topIndex - 1 ].value_.f_ ;
            symbol.value_.scalar_ = parseStack_[ topIndex     ].value_.scalar_ ;

            // ----------------< debug print from parse tables >----------------
            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (S -> POLY MOD)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (MOD -> COMMA INTEGER)
        case 2:
        {
            // The value of MOD is INTEGER (we ignore the comma).
            symbol.value_.scalar_ = parseStack_[ topIndex ].value_.scalar_ ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (MOD -> COMMA INTEGER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (MOD -> EPSILON)
        case 3:
        {
            // There isn't any value for MOD, so use the default modulus of 2.
            symbol.value_.scalar_ = 2 ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (MOD -> EPSILON)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (POLY -> POLY + TERM)
        case 4:
        {
            // A.poly = POLY + TERM

            // Degree of POLY.  e.g. x+1 has degree 1, 3 has degree 0.
            int degPoly = static_cast<int>(parseStack_[ topIndex - 2 ].value_.f_.size()) - 1 ;

            // Degree of TERM
            int degTerm = static_cast<int>(parseStack_[ topIndex ].value_.f_.size()) - 1 ;

            // A.poly = POLY
            symbol.value_.f_ = parseStack_[ topIndex - 2 ].value_.f_ ;

            // Make POLY large enough to handle rhs POLY and TERM.
            symbol.value_.f_.resize( degTerm > degPoly ? degTerm+1 : degPoly+1, 0 ) ;

            // A.poly += TERM
            for (int i = degTerm ;  i >= 0 ;  --i)
            {
                symbol.value_.f_[ i ] +=
                    parseStack_[ topIndex ].value_.f_[ i ] ;
            }

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (POLY -> POLY + TERM)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (POLY -> TERM)
        case 5:
        {
            // Degree of POLY
            int degPoly = static_cast<int>(symbol.value_.f_.size()) - 1 ;

            // Degree of TERM
            int degTerm = static_cast<int>(parseStack_[ topIndex ].value_.f_.size()) - 1 ;

            // Make POLY large enough to handle term's power.
            symbol.value_.f_.resize( degTerm > degPoly ? degTerm+1 : degPoly+1, 0 ) ;

            // Zero the polynomial.
            for (int i = static_cast<int>(symbol.value_.f_.size()) - 1 ;  i >= 0 ;  --i)
                symbol.value_.f_[ i ] = 0 ;

            // Add term.
            for (int i = static_cast<int>(symbol.value_.f_.size()) - 1 ;  i >= 0 ;  --i)
                symbol.value_.f_[ i ]  += parseStack_[ topIndex ].value_.f_[ i ] ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (POLY -> TERM) " << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (TERM -> MULTIPLIER POWER)
        case 6:
        {
            // Degree of POWER.
            unsigned int degPower = static_cast<int>( parseStack_[ topIndex ].value_.scalar_ ) ;

            // Make TERM large enough to handle the power.
            symbol.value_.f_.resize( degPower + 1, 0 ) ;

            // Zero TERM.
            for (int i = static_cast<int>(symbol.value_.f_.size()) - 1 ;  i >= 0 ;  --i)
                symbol.value_.f_[ i ] = 0 ;

            // TERM = MULTIPLIER x ^ POWER
            symbol.value_.f_[ degPower ] =
            static_cast<int>( parseStack_[ topIndex - 1 ].value_.scalar_ ) ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (TERM -> MULTIPLIER POWER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (MULTIPLIER -> INTEGER)
        case 7:
        {
            // Set value of MULTIPLIER.
            symbol.value_.scalar_ = parseStack_[ topIndex ].value_.scalar_ ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (MULTIPLIER -> INTEGER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (MULTIPLIER -> EPSILON)
        case 8:
        {
            // Multiplier default = 1
            symbol.value_.scalar_ = 1 ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (MULTIPLIER -> EPSILON)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (POWER -> X)
        case 9:
        {
            // POWER is the exponent of x, so we
            // interpret as a scalar.
            symbol.value_.scalar_ = 1 ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce  (POWER -> X)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (POWER -> X ^ INTEGER)
        case 10:
        {
            // POWER = value of INTEGER.
            symbol.value_.scalar_ = parseStack_[ topIndex ].value_.scalar_ ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (POWER -> X ^ INTEGER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (POWER -> EPSILON)
        case 11:
        {
            // Default power is 0.
            symbol.value_.scalar_ = 0 ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (POWER -> EPSILON)" << "Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;
    }
}



/*------------------------------- Parser Child Classes ------------------------*/



/*=============================================================================
 |
 | NAME
 |
 |     operator=( const FactorizationValue & v )
 |
 |
 | DESCRIPTION
 |
 |     Assignment operator for FactorizationValue.
 |
 +============================================================================*/

template<typename IntType>
FactorizationValue<IntType> & FactorizationValue<IntType>::operator=( const FactorizationValue<IntType> & v )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &v)
        return *this ;
    
    numberString_ = v.numberString_ ;
    factor_ = v.factor_ ;

    // Pass back the object so we can concatenate assignments.
    return *this ;
}



/*=============================================================================
 |
 | NAME
 |
 |     numberStringToInteger
 |
 |
 | DESCRIPTION
 |
 |     Convert a number string (sequence of decimal digits) to an integer.
 |
 +============================================================================*/

template<typename IntType>
IntType FactorizationValue<IntType>::numberStringToInteger( const string & numberString )
{
    // First define a generic integer type.
    IntType u ;
    
    // Create a string stream and load it with the number string.
    istringstream is ;
    is.clear() ;
    is.str( numberString ) ;
    
    // Read the generic integer from the stream.
    // We have two IntType's:
    //     BigInt class has a definition of operator>>
    //     ppunint built-in type uses the C++ language operator>>
    is >> u ;

    return u ;
}


/*=============================================================================
 |
 | NAME
 |
 |     FactorizationValue
 |
 | DESCRIPTION
 |
 |     Default constructor for a value which contains a scalar or polynomial.
 |
 +============================================================================*/

template<typename IntType>
FactorizationValue<IntType>::FactorizationValue()
    : numberString_ { "" }
	, factor_ { 0 }
{
}

/*=============================================================================
 |
 | NAME
 |
 |     FactorizationValue
 |
 | DESCRIPTION
 |
 |     Default constructor for a value which contains a scalar or polynomial.
 |
 +============================================================================*/

template<typename IntType>
FactorizationValue<IntType>::FactorizationValue( const IntType & p, const int & count )
: numberString_{ "" }
{
    PrimeFactor<IntType> factor( p, count ) ;
    factor_.clear() ;
    factor_.push_back( factor ) ;
}

/*=============================================================================
 |
 | NAME
 |
 |     FactorizationValue
 |
 | DESCRIPTION
 |
 |     Copy constructor for FactorizationValue.
 |
 +============================================================================*/

template<typename IntType>
FactorizationValue<IntType>::FactorizationValue( const FactorizationValue<IntType> & v )
: numberString_( v.numberString_ )
, factor_( v.factor_ )
{
}




/*=============================================================================
|
| NAME
|
|     FactorizationParser
|
| DESCRIPTION
|
|     Default constructor for the class.
|
+============================================================================*/

template < typename SymbolType, typename ValueType >
FactorizationParser< SymbolType, ValueType >::FactorizationParser()
    : Parser<SymbolType,ValueType>()
{
    // Initialize all the parse tables.
    initializeTables() ;
}


/*=============================================================================
 | 
 | NAME
 | 
 |     ~FactorizationParser
 | 
 | DESCRIPTION
 | 
 |     Default deeeestructor for the class.
 | 
 +============================================================================*/

template < typename SymbolType, typename ValueType >
FactorizationParser< SymbolType, ValueType >::~FactorizationParser()
{
    // Private data destroy themselves.
}



/*=============================================================================
 |
 | NAME
 |
 |     FactorizationParser( const FactorizationParser & FactorizationParser )
 |
 | DESCRIPTION
 |
 |     Copy constructor.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
FactorizationParser<SymbolType, ValueType>::FactorizationParser( const FactorizationParser< SymbolType, ValueType > & FactorizationParser )
        : Parser<SymbolType, ValueType>( FactorizationParser )
{
}


/*=============================================================================
 | 
 | NAME
 | 
 |    operator=( const FactorizationParser & FactorizationParser )
 | 
 | 
 | DESCRIPTION
 | 
 |    Assignment operator.
 | 
 +============================================================================*/
        
template < typename SymbolType, typename ValueType >
FactorizationParser< SymbolType, ValueType > & FactorizationParser< SymbolType, ValueType >::operator=( const FactorizationParser & FactorizationParser )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &FactorizationParser)
             return *this ;

    inputStack_ = FactorizationParser.inputStack_ ;
    parseStack_ = FactorizationParser.parseStack_ ;

    // Return reference to this object.
    return *this ;
}


/*=============================================================================
 | 
 | NAME
 | 
 |     initializeTables
 | 
 | DESCRIPTION
 | 
 |     Initialize ACTION, GOTO, and ERROR_MESSAGE tables for an LALR(1) parser.
 |     And also information about the productions.
 |
 +============================================================================*/

template < typename SymbolType, typename ValueType >
void FactorizationParser<SymbolType,ValueType>::initializeTables()
{
    // Need these so we can preallocate the vector lengths.  Vector's operator[] is unchecked!
    NumStates_       = 16 ; // Including the 0 state.
    NumProductions_  =  7 ;
    NumTerminals_    = static_cast<int>(FactorizationSymbol::NumTerminals) ;
    NumNonTerminals_ = static_cast<int>(FactorizationSymbol::NumSymbols) - static_cast<int>(FactorizationSymbol::NumTerminals) - 1 ;  // Don't count the NumTerminals enum itself.

    // Expand the rows of the ACTION table to hold all the states.
    actionTable_.resize( NumStates_ ) ;
    
    // Expand the rows of the GOTO table to hold all the states.
    gotoTable_.resize( NumStates_ ) ;
    
    // Expand the rows of the ACTION table to hold all the states.
    // Allow space in 0th slot (unused).
    production_.resize( NumProductions_ + 1 ) ;
    
    errorMessage_.resize( NumStates_ ) ;
    
    // TODO:  does resize or operator[] throw any exceptions?  use vector.at( i ) for checked access?
    try
	{
		// For each state and each terminal, insert
        //    if Shift, the new state to push on the stack,
        //    if Reduce, the production number.
		insertAction(  0,  FactorizationSymbol::Integer,   Action::Shift,   2 ) ;
		insertAction(  1,  FactorizationSymbol::Dollar,    Action::Accept,  0 ) ;
		insertAction(  2,  FactorizationSymbol::Integer,   Action::Shift,   3 ) ;
		insertAction(  3,  FactorizationSymbol::Integer,   Action::Shift,   4 ) ;

		insertAction(  4,  FactorizationSymbol::Caret,     Action::Reduce,  7 ) ;
		insertAction(  4,  FactorizationSymbol::Dollar,    Action::Reduce,  7 ) ;
        insertAction(  4,  FactorizationSymbol::Period,    Action::Reduce,  7 ) ;
        insertAction(  4,  FactorizationSymbol::Backslash, Action::Reduce,  7 ) ;

		insertAction(  5,  FactorizationSymbol::Dollar,    Action::Reduce,  1 ) ;
		insertAction(  5,  FactorizationSymbol::Period,    Action::Shift,   8 ) ;

		insertAction(  6,  FactorizationSymbol::Dollar,    Action::Reduce,  3 ) ;
		insertAction(  6,  FactorizationSymbol::Period,    Action::Reduce,  3 ) ;

		insertAction(  7,  FactorizationSymbol::Caret,     Action::Shift,   9 ) ;
        insertAction(  7,  FactorizationSymbol::Dollar,    Action::Reduce,  5 ) ;
        insertAction(  7,  FactorizationSymbol::Period,    Action::Reduce,  5 ) ;
		insertAction(  7,  FactorizationSymbol::Backslash, Action::Shift,  10 ) ;

		insertAction(  8,  FactorizationSymbol::Integer,   Action::Shift,   4 ) ;
 
		insertAction(  9,  FactorizationSymbol::Integer,   Action::Shift,   4 ) ;

		insertAction( 10,  FactorizationSymbol::Integer,   Action::Shift,  14 ) ;

		insertAction( 11,  FactorizationSymbol::Dollar,    Action::Reduce,  2 ) ;
		insertAction( 11,  FactorizationSymbol::Period,    Action::Reduce,  2 ) ;

		insertAction( 13,  FactorizationSymbol::Dollar,    Action::Reduce,  4 ) ;
		insertAction( 13,  FactorizationSymbol::Period,    Action::Reduce,  4 ) ;
		insertAction( 13,  FactorizationSymbol::Backslash, Action::Shift,  10 ) ;

		insertAction( 14,  FactorizationSymbol::Caret,     Action::Reduce,  6 ) ;
		insertAction( 14,  FactorizationSymbol::Dollar,    Action::Reduce,  6 ) ;
        insertAction( 14,  FactorizationSymbol::Period,    Action::Reduce,  6 ) ;
        insertAction( 14,  FactorizationSymbol::Backslash, Action::Reduce,  6 ) ;


		// For each state and symbol insert a new state.
		insertGoto( 0,  FactorizationSymbol::S,               1 ) ;

		insertGoto( 3,  FactorizationSymbol::Factorization,   5 ) ;
		insertGoto( 3,  FactorizationSymbol::Factor,          6 ) ;
		insertGoto( 3,  FactorizationSymbol::BigInteger,      7 ) ;

		insertGoto( 8,  FactorizationSymbol::Factor,         11 ) ;
		insertGoto( 8,  FactorizationSymbol::BigInteger,      7 ) ;

		insertGoto( 9,  FactorizationSymbol::BigInteger,     13 ) ;


		// For each production (given by a unique number), insert the single nonterminal
        // which starts the production, and the length in symbols of the right hand side.
        // We'll need to know which production is being reduced so we can apply the syntax
        // directed translation, and pop the production off the stack.
		insertProduction(  1,      FactorizationSymbol::S,               3 ) ; // S -> INTEGER INTEGER FACTORIZATION
		insertProduction(  2,      FactorizationSymbol::Factorization,   3 ) ; // FACTORIZATION -> FACTORIZATION PERIOD FACTOR
		insertProduction(  3,      FactorizationSymbol::Factorization,   1 ) ; // FACTORIZATION -> FACTOR
		insertProduction(  4,      FactorizationSymbol::Factor,          3 ) ; // FACTOR -> BIGINTEGER ^ BIGINTEGER
		insertProduction(  5,      FactorizationSymbol::Factor,          1 ) ; // FACTOR -> BIGINTEGER
		insertProduction(  6,      FactorizationSymbol::BigInteger,      3 ) ; // BIGINTEGER -> BIGINTEGER \ INTEGER
		insertProduction(  7,      FactorizationSymbol::BigInteger,      1 ) ; // BIGINTEGER -> INTEGER

        // For each state, provide an error message.
        insertErrorMessage(  0, "Expecting to see the power n." ) ;
        insertErrorMessage(  1, "Expecting end of input." ) ;
        insertErrorMessage(  2, "Expecting to see the number of prime factors." ) ;
        insertErrorMessage(  3, "Expecting an integer." ) ;
        insertErrorMessage(  4, "Expecting integer continuation \\ or . followed by a factor or ^ followed by a power or end of input." ) ;
        insertErrorMessage(  5, "Expecting another factor after the . or the end of the factorization." ) ;
        insertErrorMessage(  6, "Expecting a ." ) ;
        insertErrorMessage(  7, "Expecting integer continuation \\ or . followed by a factor or a ^ follwed by a power or end of input." ) ;
        insertErrorMessage(  8, "Expecting factor or an integer." ) ;
        insertErrorMessage(  9, "Expecting an integer." ) ;
        insertErrorMessage( 10, "Expecting an integer after the continuation \\." ) ;
        insertErrorMessage( 11, "Expecting . and another factor or end of input." ) ;
        insertErrorMessage( 13, "Expecting integer continuation \\ or . and next factor or end of input." ) ;
        insertErrorMessage( 14, "Expecting . and next factor or ^ and power or end of input after integer continuation \\." ) ;

	}
	catch( length_error & e)
	{
		ostringstream os ;
		os << "Error:  Cannot initialize FactorizationParser tables "
		<< " at " << __FILE__ << ": line " << __LINE__  ;
		throw ParserError( os.str() ) ;
    }  
}


/*=============================================================================
 |
 | NAME
 |
 |    tokenize
 |
 | DESCRIPTION
 |
 |    This is the lexical analyzer.  Given an input string, convert it into tokens
 |    to fill the input stack.
 |
 |    Each token has a type, which is a nonterminal symbol, and its value.
 |
 |    This specific lexer converts one line of a factorization table.
 |
 +============================================================================*/

template <typename SymbolType, typename ValueType> 
void FactorizationParser<SymbolType, ValueType>::tokenize( string sentence )
{
	Symbol<SymbolType, ValueType> tok( FactorizationSymbol::NumSymbols, 0 ) ;

    // Starting position to scan.
    unsigned int pos = 0 ;

    // Clear the token stack.
    inputStack_.clear() ;

    // Scan sentence to end.
    while( pos < sentence.length() )
    {
        // Skip whitespace.
        while (pos < sentence.length() && iswspace( sentence[ pos ]))
            ++pos ; // Advance to next token.

        #ifdef DEBUG_PP_PARSER
        cout << "tokenize:  sentence[ " << pos << " ] = " << sentence[ pos ] << endl ;
        #endif

        // We see a digit.
        if (pos < sentence.size() && isdigit( sentence[ pos ]))
        {
            string numberString = "" ;

            // Read a sequence of digits until the next non-digit.
            while( pos < sentence.size() && isdigit( sentence[ pos ]))
            {
                numberString += sentence[ pos ] ;
                ++pos ; // Advance to next token.
            }

            tok.type_                = FactorizationSymbol::Integer ;
            tok.value_.numberString_ = numberString ;
        }
        // A non-digit symbol.
        else
        {
            tok.value_.numberString_ = "" ;

            // Read another type of token.
            switch( sentence[ pos ] )
            {
                case '.' :
                    tok.type_ = FactorizationSymbol::Period ;
                break ;

                case '^' :
                    tok.type_ = FactorizationSymbol::Caret ;
                break ;

                case '\\' :
                    tok.type_ = FactorizationSymbol::Backslash ;
                break ;

                default:
                    // Throw a parser error of unexpected input if we see a bad symbol.
                    ostringstream os ;
                    os << "Error:  Unexpected symbol in lexical analyzer" << sentence[ pos ]
                    << " at " << __FILE__ << ": line " << __LINE__  ;
                    throw ParserError( os.str() ) ;
            } 

            ++pos ; // Advance to next token.
        } // end else

        // Push token on stack.
        inputStack_.push_back( tok ) ;

        #ifdef DEBUG_PP_PARSER
        cout << "    Push token " << tok << endl ;
        #endif

    } // end while

    // At end of input, push the $ terminator symbol.
    tok.type_ = FactorizationSymbol::Dollar ;
    tok.value_.numberString_ = "" ;

    inputStack_.push_back( tok ) ;

    #ifdef DEBUG_PP_PARSER
    cout << "    Push last token " << tok << endl ;
    #endif

    // Reverse all the symbols into our preferred order.
    reverse( inputStack_.begin(), inputStack_.end() ) ;
}

/*=============================================================================
 |
 | NAME
 |
 |    syntaxDirectedTranslation
 |
 | DESCRIPTION
 |
 |     We have a reduce action   
 | 
 |         ACTION[ sm, ai ] = reduce( A -> beta = Xm-r+1 ... Xm )  
 |
 |     using the production
 |                        
 |         A -> Xm-r+1 ... Xm
 |                              
 |     Here's the stack before the reduction:
 |
 |                                parseStack_[ topIndex-1 ]   parseStack_[ topIndex ]
 |                                                        v   v
 |                                                        v   v
 |         (s0 X1 ...    Xm-r sm-r      Xm-r+1 sm-r+1 ... Xm sm     |     ai ai+1 ... an $)
 | 
 |     and after the reduction:
 |
 |         (s0 X1 ...    Xm-r sm-r      A s                         |      ai+1 ... an $)
 |
 |     Carry out the syntax directed translation f () using the values in the right hand
 |     side of the ith production,                i
 |
 |         A.value = f ( Xm-r+1.value ... Xm.value )
 |                    i 
 |
 +============================================================================*/

template <typename SymbolType, typename ValueType>
void FactorizationParser<SymbolType,ValueType>::
         syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & symbol )
{
    switch( productionNum )
    {
        // Reduce (S -> INTEGER INTEGER FACTORIZATION)
        case 1:
        {
            // I'll explain this syntax-directed translation in detail.  The other cases are similar.
            // The parse stack contains the right hand side symbols (types and values) of the production:
            //     parse stack = (INTEGER INTEGER FACTORIZATION |
            // We compute the value of S (denoted by the variable symbol) from the right hand side.
            //
            symbol.value_.factor_       = parseStack_[ topIndex     ].value_.factor_ ; // Factorization.
            symbol.value_.numberString_ = parseStack_[ topIndex - 2 ].value_.numberString_ ; // Value of n.

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (S -> INTEGER INTEGER FACTORIZATION)" << " Value = " << symbol << endl ;
            #endif
        }
        break ;

        // Reduce (FACTORIZATION -> FACTORIZATION PERIOD FACTOR)
        case 2:
        {
            // Append the new prime factor to the end of the factorization.
            symbol.value_.factor_ = parseStack_[ topIndex - 2 ].value_.factor_ ;
            symbol.value_.factor_.push_back( parseStack_[ topIndex ].value_.factor_[ 0 ] ) ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (FACTORIZATION -> FACTORIZATION PERIOD FACTOR)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (FACTORIZATION -> FACTOR)
        case 3:
        {
            // Factorization has only one factor.
            symbol.value_.factor_.clear() ;
            symbol.value_.factor_.push_back( parseStack_[ topIndex ].value_.factor_[ 0 ] ) ;
            
            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (FACTORIZATION -> FACTOR)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (FACTOR -> BIGINTEGER ^ BIGINTEGER)
        case 4:
        {
            // Create a new value and load it with the prime factor and its count.
            auto primeFactor = ValueType::numberStringToInteger( parseStack_[ topIndex - 2 ].value_.numberString_ ) ;
            auto primeCount  = static_cast<int>( ValueType::numberStringToInteger( parseStack_[ topIndex ].value_.numberString_ ) ) ;
            ValueType value( primeFactor, primeCount ) ;
            
            // Factorization has only one factor.
            symbol.value_.factor_.clear() ;
            symbol.value_.factor_.push_back( value.factor_[ 0 ] ) ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (FACTOR -> BIGINTEGER ^ BIGINTEGER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (FACTOR -> BIGINTEGER)
        case 5:
        {
            // Create a new value which has a single prime factor.
            auto primeFactor = ValueType::numberStringToInteger( parseStack_[ topIndex ].value_.numberString_ ) ;
            ValueType value( primeFactor, 1 ) ;
            
            // Factorization has only one factor.
            symbol.value_.factor_.clear() ;
            symbol.value_.factor_.push_back( value.factor_[ 0 ] ) ;

            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (FACTOR -> BIGINTEGER) " << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (BIGINTEGER -> BIGINTEGER \ INTEGER)
        case 6:
        {
            // Concatenate the number strings.  Remember, the production right hand side is sitting on the parse stack and the rightmost
            // token is at the top of the stack.
            symbol.value_.numberString_ = parseStack_[ topIndex - 2 ].value_.numberString_ + parseStack_[ topIndex ].value_.numberString_ ;
            
            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (BIGINTEGER -> BIGINTEGER \\ INTEGER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;

        // Reduce (BIGINTEGER -> INTEGER)
        case 7:
        {
            // Initial number string.
            symbol.value_.numberString_ = parseStack_[ topIndex ].value_.numberString_ ;
            
            #ifdef DEBUG_PP_PARSER
            cout << "    Reduce (BIGINTEGER -> INTEGER)" << " Value = " << symbol.value_ << endl ;
            #endif
        }
        break ;
    }
}



/*=============================================================================
 | 
 | NAME
 | 
 |     parseCommandLine
 | 
 | DESCRIPTION
 | 
 |      Parse the command line.
 | 
 | 
 | EXAMPLE
 | 
 |    pp -h
 |    pp -s 2 4
 |    pp -t 2 4 x^3+x^2+1                 // No blanks, please!  Looks like
 |                                        // several command line arguments.
 |    pp -t 2 4 "x^  3  +  x ^ 2  +  1"   // Blanks OK now.
 | 
 +============================================================================*/

template < typename SymbolType, typename PolyValueType >
void PolyParser< SymbolType, PolyValueType >::parseCommandLine( int argc, const char * argv[] )
{
    int          input_arg_index ;
    const char * input_arg_string ;
    const char * option_ptr ;

    int          num_arg ;
    const char * arg_string[ MAX_NUM_COMMAND_LINE_ARGS ] ;

    /*  Initialize to defaults. */
    testPolynomialForPrimitivity_ = false ;
    listAllPrimitivePolynomials_  = false ;
    printOperationCount_          = false ;
    printHelp_                    = false ;
    slowConfirm_                  = false ;
    p                             = 0 ;
    n                             = 0 ;

    factorizationTableLocation_ = argv[ 0 ] ;
    
    //  Parse the command line to get the options and their inputs.
    for (input_arg_index = 0, num_arg = 0 ;  input_arg_index < argc ;
         ++input_arg_index)
    {
        /*  Get next argument string. */
        input_arg_string = argv[ input_arg_index ] ;

        /* We have an option:  a hyphen followed by a non-null string. */
        if (input_arg_string[ 0 ] == '-' && input_arg_string[ 1 ] != '\0')
        {
            /* Scan all options. */
            for (option_ptr = input_arg_string + 1 ;  *option_ptr != '\0' ;
                 ++option_ptr)
            {
                switch( *option_ptr )
                {
                    /* Test a given polynomial for primitivity. */
                    case 't':
                        testPolynomialForPrimitivity_ = true ;
                    break ;

                    /* List all primitive polynomials.  */
                    case 'a':
                       listAllPrimitivePolynomials_ = true ;
                    break ;

                    /* Print statistics on program operation. */
                    case 's':
                       printOperationCount_ = true ;
                    break ;

                    /* Print help. */
                    case 'h':
                    case 'H':
                        printHelp_ = true ;
                    break ;

                    /* Turn on all self-checking (slow!).  */
                    case 'c':
                        slowConfirm_ = true ;
                    break ;

                    default:
                       ostringstream os ;
                       os << "Cannot recognize the option" << *option_ptr ;
                       throw ParserError( os.str() ) ;
                    break ;
                }
            }
        }
        else  /* Not an option, but an argument. */
        {
            arg_string[ num_arg++ ] = input_arg_string ;
        }
    }

    // User specified a polynomial to test.  First argument is program name, next is
    // the polynomial.
    if (testPolynomialForPrimitivity_)
    {
        string testPoly ;
        testPoly.assign( arg_string[ 1 ] ) ; // stringify it.
        testPolynomial_ = testPoly ;         // We've range checked n and p here already.
      
        n = testPolynomial_.deg() ;
        p = testPolynomial_.modulus() ;
    }
    //  Otherwise assume the next two arguments are p and n.
    else if (num_arg == MAX_NUM_COMMAND_LINE_ARGS)
    {
        // TODO:  convert numeric decimal string to unsigned int with overflow check.
        p = atoi( arg_string[ 1 ] ) ;
        n = atoi( arg_string[ 2 ] ) ;
    }
    else
    {
        ostringstream os ;
        os << "ERROR:  Expecting two arguments, p and n.\n\n" ;
        printHelp_ = true ;
        throw ParserError( os.str() ) ;
    }

    // Check ranges on n and p.
    if (p < minModulus)
    {
        ostringstream os ;
        os << "Error.  Polynomial modulus p must be >= " << minModulus << endl ;
        printHelp_ = true ;
        throw ParserError( os.str() ) ;
    }

    if (p >= BigInt::getBase())
    {
        ostringstream os ;
        os << "Error.  Polynomial modulus p must be < " << BigInt::getBase() << endl ;
        printHelp_ = true ;
        throw ParserError( os.str() ) ;
    }

    if (n < minDegree)
    {
        ostringstream os ;
        os << "Error.  Polynomial degree n must be >= " << minDegree << " but n = " << n << endl ;
        printHelp_ = true ;
        throw ParserError( os.str() ) ;
    }

    //  Check to see if p is a prime.
    if (!isAlmostSurelyPrime( static_cast<ppuint>( p )))
        throw ParserError( "ERROR:  p must be a prime number.\n\n" ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     enumToString
 |
 | DESCRIPTION
 |
 |     Helper functions to translate enum types into human readable format.
 |     One generic for any SymbolType, the other two template specializations.
 |
 +============================================================================*/

template< typename SymbolType >
string enumToString( const SymbolType & st )
{
    ostringstream os ;
    
    // Just return the integer number of the enumerated type since we have no
    // further information.
    os << static_cast<int>( st ) ;
    return os.str() ;
}

// Specialized for PolySymbol.
template<>
string enumToString<PolySymbol>( const PolySymbol & st )
{
    string name[] { "$", "Integer", ",", "x", "+", "^", "", "S", "Mod", "Poly", "Term", "Multiplier", "Power", "" } ;
    return name[ static_cast<int>( st ) ] ;
}

// Specialized for FactorizationSymbol.
template<>
string enumToString<FactorizationSymbol>( const FactorizationSymbol & st )
{
    string name[] { "$", "Integer", ".", "^", "\\", "", "S", "Factorization", "Factor", "BigInteger", "" } ;
    return name[ static_cast<int>( st ) ] ;
}

/*==============================================================================
 |                     Forced Template Instantiations                           |
 ==============================================================================*/

// C++ doesn't automatically generate templated classes or functions until they
// are first used.  So depending on the order of compilation, the linker may say
// the templated functions are undefined.  Best to explicitly instantiate the templates here.

// General purpose LALR(1) parser.  Template versions for both the polynomial grammar and factorization grammar.
template PolyValue                                                    Parser<PolySymbol, PolyValue>::parse( string s ) ;
template FactorizationValue<BigInt>         Parser<FactorizationSymbol, FactorizationValue<BigInt>>::parse( string s ) ;
template FactorizationValue<ppuint>         Parser<FactorizationSymbol, FactorizationValue<ppuint>>::parse( string s ) ;

// Generate all symbols for the polynomial grammar.
template                                    Symbol<PolySymbol, PolyValue>::Symbol( PolySymbol, int ) ;
template                                    Symbol<PolySymbol, PolyValue>::Symbol( PolySymbol, int, PolyValue )  ;
template                                    Symbol<PolySymbol, PolyValue>::~Symbol() ;
template                                    Symbol<PolySymbol, PolyValue>::Symbol(    const Symbol< PolySymbol, PolyValue > & ) ;
template Symbol<PolySymbol, PolyValue> &    Symbol<PolySymbol, PolyValue>::operator=( const Symbol< PolySymbol, PolyValue > & ) ;

// Generate all the symbols for the factorization grammar.
template FactorizationValue<BigInt>::FactorizationValue() ;
template FactorizationValue<BigInt>::FactorizationValue( const FactorizationValue<BigInt> & ) ;
template FactorizationValue<BigInt> & FactorizationValue<BigInt>::operator=( const FactorizationValue<BigInt> & ) ;
template FactorizationValue<ppuint>::FactorizationValue() ;
template FactorizationValue<ppuint>::FactorizationValue( const FactorizationValue<ppuint> & ) ;
template FactorizationValue<ppuint> & FactorizationValue<ppuint>::operator=( const FactorizationValue<ppuint> & ) ;

template Symbol<FactorizationSymbol, FactorizationValue<BigInt>>::Symbol( FactorizationSymbol, int ) ;
template Symbol<FactorizationSymbol, FactorizationValue<BigInt>>::Symbol( FactorizationSymbol, int, FactorizationValue<BigInt> )  ;
template Symbol<FactorizationSymbol, FactorizationValue<BigInt>>::~Symbol() ;
template Symbol<FactorizationSymbol, FactorizationValue<BigInt>>::Symbol(    const Symbol<FactorizationSymbol, FactorizationValue<BigInt>> & ) ;
template Symbol<FactorizationSymbol, FactorizationValue<BigInt>> &
         Symbol<FactorizationSymbol, FactorizationValue<BigInt>>::operator=( const Symbol<FactorizationSymbol, FactorizationValue<BigInt>> & ) ;
template Symbol<FactorizationSymbol, FactorizationValue<ppuint>>::Symbol( FactorizationSymbol, int ) ;
template Symbol<FactorizationSymbol, FactorizationValue<ppuint>>::Symbol( FactorizationSymbol, int, FactorizationValue<ppuint> )  ;
template Symbol<FactorizationSymbol, FactorizationValue<ppuint>>::~Symbol() ;
template Symbol<FactorizationSymbol, FactorizationValue<ppuint>>::Symbol(    const Symbol<FactorizationSymbol, FactorizationValue<ppuint>> & ) ;
template Symbol<FactorizationSymbol, FactorizationValue<ppuint>> &
         Symbol<FactorizationSymbol, FactorizationValue<ppuint>>::operator=( const Symbol<FactorizationSymbol, FactorizationValue<ppuint>> & ) ;

// Generate all child class member functions for the polynomial grammar.
template                                      PolyParser<PolySymbol, PolyValue>::PolyParser() ;
template                                      PolyParser<PolySymbol, PolyValue>::~PolyParser() ;
template                                      PolyParser<PolySymbol, PolyValue>::PolyParser( const PolyParser<PolySymbol, PolyValue> & ) ;
template PolyParser<PolySymbol, PolyValue> &  PolyParser<PolySymbol, PolyValue>::operator=(  const PolyParser<PolySymbol, PolyValue> & ) ;

template void                                 PolyParser<PolySymbol, PolyValue>::parseCommandLine( int argc, const char * argv[] ) ;

// Generate all child class member functions for the factorization grammar.
template FactorizationParser<FactorizationSymbol, FactorizationValue<BigInt>>::FactorizationParser() ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<BigInt>>::~FactorizationParser() ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<BigInt>>::FactorizationParser( const FactorizationParser<FactorizationSymbol,
                                                                                                          FactorizationValue<BigInt>> & ) ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<BigInt>> &
         FactorizationParser<FactorizationSymbol, FactorizationValue<BigInt>>::operator=(  const FactorizationParser<FactorizationSymbol, FactorizationValue<BigInt>> & ) ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<ppuint>>::FactorizationParser() ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<ppuint>>::~FactorizationParser() ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<ppuint>>::FactorizationParser( const FactorizationParser<FactorizationSymbol,
                                                                                                          FactorizationValue<ppuint>> & ) ;
template FactorizationParser<FactorizationSymbol, FactorizationValue<ppuint>> &
         FactorizationParser<FactorizationSymbol, FactorizationValue<ppuint>>::operator=(  const FactorizationParser<FactorizationSymbol, FactorizationValue<ppuint>> & ) ;


