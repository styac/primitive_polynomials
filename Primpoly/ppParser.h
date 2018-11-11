/*==============================================================================
|
|  NAME
|
|     ppParser.h
|
|  DESCRIPTION
|
|     Header file for polynomial parser classes.
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
#ifndef __PPPARSER_H__


// Some miscellaneous constants.
#define MAX_NUM_COMMAND_LINE_ARGS 3
const ppuint minModulus = 2 ;
const ppuint minDegree  = 2 ;


/*=============================================================================
 |
 | NAME
 |
 |     Action
 |
 | DESCRIPTION
 |
 |     Types of LR parser actions.
 |
 +============================================================================*/

enum class Action
{
    Shift, Reduce, Accept, Error
} ;


/*=============================================================================
 |
 | NAME
 |
 |     enumToString
 |
 | DESCRIPTION
 |
 |     Helper function to translate enum types into human readable format.
 |
 +============================================================================*/

template< typename SymbolType >
string enumToString( const SymbolType & st ) ;


/*=============================================================================
 |
 | NAME
 |
 |     Symbol
 |
 | DESCRIPTION
 |
 |     Define the VALUE of a symbol.  Used in the parser's syntax directed
 |     translation and also for the final polynomial value returned from the
 |     parser.
 |
 +============================================================================*/

// Tokens and other types of symbols used in parsing.
template< typename SymbolType, typename ValueType >
class Symbol
{
    public:
        Symbol() ;
        
        Symbol( SymbolType type, int state ) ;
        
        Symbol( SymbolType type, int state, ValueType value )  ;
        
        Symbol( const Symbol< SymbolType, ValueType > & s ) ;
    
        // Destructor.  Virtual so derived class destructor
        // automatically calls its base class destructor.
        virtual ~Symbol()
        {
        }
    
        Symbol< SymbolType, ValueType > & operator=( const Symbol<SymbolType,ValueType> & s ) ;

        // Print function for Symbol class.
        // Define this templated non-member friend function here, otherwise the compiler will generate
        // a link error.  If the compiler sees only a declaration here, it doesn't know yet it's a template.
        // See https://isocpp.org/wiki/faq/templates#template-friends
        friend ostream & operator<<( ostream & out, const Symbol<SymbolType,ValueType> & s )
        {
            out << enumToString<SymbolType>( s.type_ ) << "   " ;
            out << s.value_ << "   " ;
            out << "state " << s.state_ ;

            return out ;
        }

    // Allow direct access to this simple data type for convenience.
    public:
        SymbolType type_ ;  // Type of terminal or nonterminal.
        ValueType  value_ ; // Value of the symbol.
        int        state_ ; // State or production number.
} ;

/*=============================================================================
 |
 | NAME
 |
 |     ActionState
 |
 | DESCRIPTION
 |
 |     A pair consisting of action and state for the LR parser.
 |     Actions are (shift s, reduce p, accept, error) and the new state
 |     number to transition to.
 |
 +============================================================================*/

class ActionState
{
    public:
        ActionState() ;
        
        ActionState( Action type, int state ) ;
    
        // Destructor.  Virtual so derived class destructor
        // automatically calls its base class destructor.
        virtual ~ActionState()
        {
        }

        ActionState( const ActionState & as ) ;
        
        ActionState & operator=( const ActionState & as ) ;
    
        friend ostream & operator<<( ostream & out, const ActionState & as ) ;
    
    // Allow direct access to this simple data type for convenience.
    public:
        Action action_ ;
        int    state_ ;
} ;

/*=============================================================================
 |
 | NAME
 |
 |     ParserError
 |
 | DESCRIPTION
 |
 |     Exceptions for the parser class.
 |
 +============================================================================*/

class ParserError : public runtime_error
{
    public:
        // Throw with an error message.
        ParserError( const string & description )
            : runtime_error( description )
        {
        } ;

        // Throw with default error message.
        ParserError()
            : runtime_error( "Parser error:  " )
        {
        } ;

} ; 



/*=============================================================================
|
| NAME
|
|     Parser
|
| DESCRIPTION
|
|     Base class for parsing using an LALR(1) parser.
|
| EXAMPLE
|
|     Example of how to use the PolyParser child class for polynomials,
|
|        // Create a parser and load the parse tables.
|        PolyParser<PolySymbol, PolyValue> parser() ;
|
|        // Sample polynomial.
|        string s = "2 x ^ 3 + 3 x + 4, 5" ;
|
|        try 
|        {
|            PolyValue v = parser.parse( s ) ;
|            cout << "Parsing the string = " << s << endl ;
|            cout << "We have a polynomial = " << v << endl ;
|        }
|        catch( ParserError & e )
|        {
|            cout << "Parser error:  " << e.what() << endl ;
|        }
|
| NOTES
|
|     I used a parser generator I wrote in Common LISP to generate the LALR(1) parse tables.  See my
|     notes,
|         http://seanerikoconnor.freeservers.com/ComputerScience/Compiler/ParserGeneratorAndParser/overview.html
|     You can also use Flex and Bison (Yacc) to generate the equivalent tables, but I like my own parser generator.
|
|     While the LALR(1) parser machinery stays the same in this base class, the grammar,
|     i.e. parse tables and error messages, will be redefined in the child class, also the syntax
|     directed translation.  We try to use the base class lexical analyzer for all child classes, if possible.
|  
+============================================================================*/

template< typename SymbolType, typename ValueType >
class Parser
{
    // The basic member functions to create and initialize the parser,
    // make copies and do a syntax-directed translation of a string.
    public:
        // Default construct the parser.
        Parser() ;
        
        // Destructor.  Virtual so derived class destructor
        // automatically calls its base class destructor.
        virtual ~Parser() ;

        // Copy constructor.
        Parser( const Parser & parser ) ;

        // Assignment operator.
        virtual Parser & operator=( const Parser & parser ) ;

        // Parse an input string, doing a syntax-directed translation, returning the value.
        ValueType parse( string s ) ;

    // Pure virtual functions to be redefined in the child classes, since the lexical analyzer, parse tables and
    // syntax-directed translation depend uniquely on the grammar.
    //
	// Defined within the scope of the Parser class or its derived classes only,
    // so not visible from outside the parser.
    protected:
        // Parse string into tokens.
        virtual void tokenize( string sentence ) = 0 ;
    
        // NOTE:  call this in the child class constructor.  It fills in the ACTION, GOTO, and ERROR_MESSAGE tables.
        virtual void initializeTables() = 0 ;

        // Reduce using production A -> beta, computing A's value from values of the tokens in beta.
        virtual void syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & as ) = 0 ;
    
    // These member values and functions are called directly in the child classes.
    protected:
        // Number of states, productions, etc.
        int NumStates_ ;        // Includes the 0 state.
        int NumProductions_ ;   // Number of productions.
        int NumTerminals_ ;     // Number of terminal symbols.
        int NumNonTerminals_ ;  // Number of nonterminal symbols.
    
        void insertAction(       int state,   SymbolType terminal, Action actionType, int actionState ) ;
        void insertGoto(         int state,   SymbolType nonterm,  int    newState ) ;
        void insertProduction(   int prodNum, SymbolType nonTerm,  int    rhsLength ) ;
        void insertErrorMessage( int state,   string errorMessage ) ;

    protected:
        // Parser stack containing symbols tokenized by lexical analyzer.
        vector< Symbol<SymbolType, ValueType> > inputStack_ ;

        // Parse stack containing symbols and states.
        vector< Symbol<SymbolType, ValueType> > parseStack_ ;

        // Action table.  
        //     action = ACTION_TABLE[ <state at top of parse stack> ][ <lookahead input token> ]
        vector< vector<ActionState> > actionTable_ ;

        // GOTO table.
        // For reduce actions,
        //    new goto state to push = GOTO[ current state ][ nonterminal production symbol ]
        vector< vector<int> > gotoTable_ ;

        // Error messages.
        vector< string > errorMessage_ ;
        
        // Productions:  Left side non-terminal symbol + production number.
        vector< Symbol<SymbolType, ValueType> > production_ ;
} ;



// --------------- Derived class parsers and their symbols and values ---------



/*=============================================================================
 | 
 | NAME
 | 
 |     PolyValue
 | 
 | DESCRIPTION
 | 
 |     Define a symbol with a polynomial value.  Used in the parser's syntax directed
 |     translation and also for the final polynomial value returned from the 
 |     parser.
 |
 +============================================================================*/

class PolyValue
{
    public:
        PolyValue() ;
    
        PolyValue( const PolyValue & v ) ;
    
        PolyValue & operator=( const PolyValue & v ) ;
    
        friend ostream & operator<<( ostream & , const PolyValue & v ) ;

    // Allow direct access to this simple data type for convenience.
    public:
        ppuint           scalar_ ;  // Number which can be a polynomial coefficient, power of x, or the modulus p.
                                    // Let it be signed for maximum flexibility.  We'll check if it exceeds the allowed
                                    // range for internal primitive polynomial computations in the parser. 
        vector< ppuint > f_ ;       // Polynomial f(x).
} ;


/*=============================================================================
 |
 | NAME
 |
 |     PolySymbol
 |
 | DESCRIPTION
 |
 |     Define polynomial symbol (tokens).  Used in the parser's syntax directed
 |     translation.
 |
 +============================================================================*/

enum class PolySymbol
{
    // Terminal symbols or tokens.
    Dollar = 0, Integer, Comma, Ecks, Plus, Exp,
    NumTerminals,
    
    // Nonterminal symbols.
    S, Mod, Poly, Term, Multiplier, Power,
    NumSymbols
} ;

/*=============================================================================
|
| NAME
|
|     PolyParser
|
| DESCRIPTION
|
|     Child class for parsing a polynomial grammar.
|
| EXAMPLE
|
|     Here is the LALR(1) grammar.  Note the use of epsilon productions to
|     make the grammar simpler.
|
|     Productions:
|
|         S          -> Poly Mod
|         Mod        -> comma integer | EPSILON
|         Poly       -> Poly + Term | Term
|         Term       -> Multiplier Power
|         Multiplier -> integer | EPSILON
|         Power      -> x | x ^ integer | EPSILON
|
|     Terminal symbols:
|   
|         comma + integer ^ x $
|
+============================================================================*/

template< typename SymbolType, typename ValueType >
class PolyParser : public Parser< SymbolType, ValueType >
{
    public:
        // Construct the parser.
        PolyParser() ;
        
        // Destructor.
        ~PolyParser() ;
        
        // Copy constructor.
        PolyParser( const PolyParser< SymbolType, ValueType > & parser ) ;
        
        // Assignment operator.
        PolyParser< SymbolType, ValueType > & operator=( const PolyParser< SymbolType, ValueType > & parser ) ;
        
        // Command line argument parsing.
        void parseCommandLine( int argc, const char * argv[] ) ;
        
        // Allow direct access to these command line parameters for convenience.
        bool   testPolynomialForPrimitivity_ ;
        bool   listAllPrimitivePolynomials_ ;
        bool   printOperationCount_ ;
        bool   printHelp_ ;
        bool   slowConfirm_ ;
        ppuint p ;
        int    n ;
        Polynomial testPolynomial_ ;
    
        // Location of the factorization tables.
        static string factorizationTableLocation_ ;
    
    private:
        // Parse string into tokens.
        void tokenize( string sentence ) ;
        
        // Fill in the ACTION, GOTO, and ERROR_MESSAGE tables.
        void initializeTables() ;

        // Syntax directed translation.  Reduce using production A -> beta, computing A's value from values of 
        // the tokens in beta.
        void syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & as ) ;

        // Even though these member variables are declared in the base class as protected, they are not visible in this
        // child class.  Because we have a templated class, we must tell the C++ compiler explicitly that these symbols
        // are from the base class.
        using Parser<SymbolType, ValueType>::NumStates_ ;
        using Parser<SymbolType, ValueType>::NumProductions_ ;
        using Parser<SymbolType, ValueType>::NumTerminals_ ;
        using Parser<SymbolType, ValueType>::NumNonTerminals_ ;

        using Parser<SymbolType, ValueType>::insertAction ;
        using Parser<SymbolType, ValueType>::insertGoto ;
        using Parser<SymbolType, ValueType>::insertProduction ;
        using Parser<SymbolType, ValueType>::insertErrorMessage ;

        using Parser< SymbolType, ValueType >::inputStack_ ;
        using Parser< SymbolType, ValueType >::parseStack_ ;
        using Parser< SymbolType, ValueType >::actionTable_ ;
        using Parser< SymbolType, ValueType >::gotoTable_ ;
        using Parser< SymbolType, ValueType >::errorMessage_ ;
        using Parser< SymbolType, ValueType >::production_ ;
} ;

// Static member variable needs to be initialized outside of the class declaration.
template< typename SymbolType, typename ValueType >
string PolyParser<SymbolType, ValueType>::factorizationTableLocation_  = "" ;


// --------------- Derived class parsers and their symbols and values ---------



/*=============================================================================
 | 
 | NAME
 | 
 |     FactorizationValue
 | 
 | DESCRIPTION
 | 
 |     Define a symbol with a factorization line value.  Used in the parser's syntax directed
 |     translation and also for the final value returned from the parser.
 |
 +============================================================================*/

template< typename IntType >
class FactorizationValue
{
    public:
        FactorizationValue() ;
    
        FactorizationValue( const FactorizationValue<IntType> & v ) ;
    
        FactorizationValue( const IntType & p, const int & count ) ;

        FactorizationValue<IntType> & operator=( const FactorizationValue<IntType> & v ) ;
    
        // Print out the factorization.
        // Define this templated non-member friend function here, otherwise the compiler will generate
        // a link error.  If the compiler sees only a declaration here, it doesn't know yet it's a template.
        // See https://isocpp.org/wiki/faq/templates#template-friends
        friend ostream & operator<<( ostream & out, const FactorizationValue<IntType> & factorization )
        {
            if (factorization.factor_.size() > 0)
            {
                for (auto & f : factorization.factor_)
                    out << f ;
            }
            if (factorization.numberString_.length() > 0)
                out << factorization.numberString_ ;

            return out ;
        }
    
        static IntType numberStringToInteger( const string & numberString ) ;

    // Allow direct access to data for convenience.
    public:
        string                         numberString_ ;  // Sequence of digits representing an integer.
		vector< PrimeFactor<IntType> > factor_ ;        // Vector of distinct prime factors to powers.
} ;


/*=============================================================================
 |
 | NAME
 |
 |     FactorizationSymbols
 |
 | DESCRIPTION
 |
 |     Define factoriziton symbols (tokens).  Used in the parser's syntax directed
 |     translation.
 |
 +============================================================================*/

enum class FactorizationSymbol
{
    // Terminal symbols or tokens.
    Dollar = 0, Integer, Period, Caret, Backslash,
    NumTerminals,
    
    // Nonterminal symbols.
    S, Factorization, Factor, BigInteger,
    
    // Count number of total symbols.
    NumSymbols
} ;



/*=============================================================================
|
| NAME
|
|     FactorizationParser
|
| DESCRIPTION
|
|     Child class for parsing a factorization table of Cunningham numbers p ^ n - 1.
|
| EXAMPLE
|
|     Each table for p has one line containing the factorization of p ^ n - 1 of the type
|
|          n <num factors p1 ^ m1 . p2 ^ m2 ...
|
|     Dots separate the factors.  Long numbers are continued on the next line with backslashes.
|     For example, in the table for p = 3, the line below is for n = 398 and has the 12 factors
|     2 ^ 4, 3, 3583 ... 11881576435363115110426317067813673631182057431802975257340612015708984828478898969687279497327343
|       
|         398    12  2^4.3.3583.4588543.34266607.2146612951394313997.8670122322845042\
|                    61471.3742361194240057889227626965547117.118815764353631151\
|                    104263170678136736311820574318029752573406120157089\
|                    84828478898969687279497327343
|
|     Here is the LALR(1) grammar.  
| 
|     Productions:
|
|         S             -> integer integer Factorization
|         Factorization -> Factorization . Factor | Factor
|         Factor        -> BigInteger ^ BigInteger | BigInteger
|         BigInteger    -> BigInteger \ integer | integer
|
|     Terminal symbols:
|         integer . ^ \ $
|
+============================================================================*/

template< typename SymbolType, typename ValueType >
class FactorizationParser : public Parser< SymbolType, ValueType >
{
    public:
        // Construct the parser.
        FactorizationParser() ;
        
        // Destructor.
        ~FactorizationParser() ;
        
        // Copy constructor.
        FactorizationParser( const FactorizationParser< SymbolType, ValueType > & parser ) ;
        
        // Assignment operator.
        FactorizationParser< SymbolType, ValueType > & operator=( const FactorizationParser< SymbolType, ValueType > & parser ) ;
        
    private:
        // Parse string into tokens.
        void tokenize( string sentence ) ;
        
        // Fill in the ACTION, GOTO, and ERROR_MESSAGE tables.
        void initializeTables() ;

        // Syntax directed translation.  Reduce using production A -> beta, computing A's value from values of 
        // the tokens in beta.
        void syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & as ) ;

        // Even though these member variables are declared in the base class as protected, they are not visible in this
        // child class.  Because we have a templated class, we must tell the C++ compiler explicitly that these symbols
        // are from the base class.
        using Parser<SymbolType, ValueType>::NumStates_ ;
        using Parser<SymbolType, ValueType>::NumProductions_ ;
        using Parser<SymbolType, ValueType>::NumTerminals_ ;
        using Parser<SymbolType, ValueType>::NumNonTerminals_ ;

        using Parser<SymbolType, ValueType>::insertAction ;
        using Parser<SymbolType, ValueType>::insertGoto ;
        using Parser<SymbolType, ValueType>::insertProduction ;
        using Parser<SymbolType, ValueType>::insertErrorMessage ;

        using Parser< SymbolType, ValueType >::inputStack_ ;
        using Parser< SymbolType, ValueType >::parseStack_ ;
        using Parser< SymbolType, ValueType >::actionTable_ ;
        using Parser< SymbolType, ValueType >::gotoTable_ ;
        using Parser< SymbolType, ValueType >::errorMessage_ ;
        using Parser< SymbolType, ValueType >::production_ ;
} ;

#endif // __PPPARSER_H__
