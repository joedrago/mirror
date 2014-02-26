/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library

    Copyright (c) 2001 Daniel C. Nuffer
    Copyright (c) 2001-2009 Hartmut Kaiser. 
    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    This is a lexer conforming to the Standard with a few exceptions.
    So it does allow the '$' to be part of identifiers. If you need strict 
    Standards conforming behaviour, please include the lexer definition 
    provided in the file strict_cpp.re.

    TODO: 
        handle errors better.
=============================================================================*/

/*!re2c
re2c:indent:string = "    "; 
re2c:yyfill:enable = 0;
any                = [\t\v\f\r\n\040-\377];
anyctrl            = [\001-\037];
OctalDigit         = [0-7];
Digit              = [0-9];
HexDigit           = [a-fA-F0-9];
Integer            = (("0" [xX] HexDigit+) | ("0" OctalDigit*) | ([1-9] Digit*));
ExponentStart      = [Ee] [+-];
ExponentPart       = [Ee] [+-]? Digit+;
FractionalConstant = (Digit* "." Digit+) | (Digit+ ".");
FloatingSuffix     = [fF] [lL]? | [lL] [fF]?;
IntegerSuffix      = [uU] [lL]? | [lL] [uU]?;
LongIntegerSuffix  = [uU] ([lL] [lL]) | ([lL] [lL]) [uU]?;
Backslash          = [\\] | "??/";
EscapeSequence     = Backslash ([abfnrtv?'"] | Backslash | "x" HexDigit+ | OctalDigit OctalDigit? OctalDigit?);
HexQuad            = HexDigit HexDigit HexDigit HexDigit;
UniversalChar      = Backslash ("u" HexQuad | "U" HexQuad HexQuad);
Newline            = "\r\n" | "\n" | "\r";
PPSpace            = ([ \t\f\v]|("/*"(any\[*]|Newline|("*"+(any\[*/]|Newline)))*"*"+"/"))*;
Pound              = "#" | "??=" | "%:";
NonDigit           = [a-zA-Z_$] | UniversalChar;
*/

/*!re2c
    "/*"            { goto ccomment; }
    "//"            { goto cppcomment; }
    "."? Digit      { goto pp_number; }

    "asm"           { BOOST_WAVE_RET(T_ASM); }
    "auto"          { BOOST_WAVE_RET(T_AUTO); }
    "bool"          { BOOST_WAVE_RET(T_BOOL); }
    "break"         { BOOST_WAVE_RET(T_BREAK); }
    "case"          { BOOST_WAVE_RET(T_CASE); }
    "catch"         { BOOST_WAVE_RET(T_CATCH); }
    "char"          { BOOST_WAVE_RET(T_CHAR); }
    "class"         { BOOST_WAVE_RET(T_CLASS); }
    "const"         { BOOST_WAVE_RET(T_CONST); }
    "const_cast"    { BOOST_WAVE_RET(T_CONSTCAST); }
    "continue"      { BOOST_WAVE_RET(T_CONTINUE); }
    "default"       { BOOST_WAVE_RET(T_DEFAULT); }
    "delete"        { BOOST_WAVE_RET(T_DELETE); }
    "do"            { BOOST_WAVE_RET(T_DO); }
    "double"        { BOOST_WAVE_RET(T_DOUBLE); }
    "dynamic_cast"  { BOOST_WAVE_RET(T_DYNAMICCAST); }
    "else"          { BOOST_WAVE_RET(T_ELSE); }
    "enum"          { BOOST_WAVE_RET(T_ENUM); }
    "explicit"      { BOOST_WAVE_RET(T_EXPLICIT); }
    "export"        { BOOST_WAVE_RET(T_EXPORT); }
    "extern"        { BOOST_WAVE_RET(T_EXTERN); }
    "false"         { BOOST_WAVE_RET(T_FALSE); }
    "float"         { BOOST_WAVE_RET(T_FLOAT); }
    "for"           { BOOST_WAVE_RET(T_FOR); }
    "friend"        { BOOST_WAVE_RET(T_FRIEND); }
    "goto"          { BOOST_WAVE_RET(T_GOTO); }
    "if"            { BOOST_WAVE_RET(T_IF); }
    "inline"        { BOOST_WAVE_RET(T_INLINE); }
    "int"           { BOOST_WAVE_RET(T_INT); }
    "long"          { BOOST_WAVE_RET(T_LONG); }
    "mutable"       { BOOST_WAVE_RET(T_MUTABLE); }
    "MM_EXPOSED"    { BOOST_WAVE_RET(T_MM_EXPOSED); }
    "MM_IGNORE_START" { BOOST_WAVE_RET(T_MM_IGNORE_START); }
    "MM_IGNORE_END" { BOOST_WAVE_RET(T_MM_IGNORE_END); }
    "namespace"     { BOOST_WAVE_RET(T_NAMESPACE); }
    "new"           { BOOST_WAVE_RET(T_NEW); }
    "operator"      { BOOST_WAVE_RET(T_OPERATOR); }
    "private"       { BOOST_WAVE_RET(T_PRIVATE); }
    "protected"     { BOOST_WAVE_RET(T_PROTECTED); }
    "public"        { BOOST_WAVE_RET(T_PUBLIC); }
    "register"      { BOOST_WAVE_RET(T_REGISTER); }
    "reinterpret_cast" { BOOST_WAVE_RET(T_REINTERPRETCAST); }
    "return"        { BOOST_WAVE_RET(T_RETURN); }
    "short"         { BOOST_WAVE_RET(T_SHORT); }
    "signed"        { BOOST_WAVE_RET(T_SIGNED); }
    "sizeof"        { BOOST_WAVE_RET(T_SIZEOF); }
    "static"        { BOOST_WAVE_RET(T_STATIC); }
    "static_cast"   { BOOST_WAVE_RET(T_STATICCAST); }
    "struct"        { BOOST_WAVE_RET(T_STRUCT); }
    "switch"        { BOOST_WAVE_RET(T_SWITCH); }
    "template"      { BOOST_WAVE_RET(T_TEMPLATE); }
    "this"          { BOOST_WAVE_RET(T_THIS); }
    "throw"         { BOOST_WAVE_RET(T_THROW); }
    "true"          { BOOST_WAVE_RET(T_TRUE); }
    "try"           { BOOST_WAVE_RET(T_TRY); }
    "typedef"       { BOOST_WAVE_RET(T_TYPEDEF); }
    "typeid"        { BOOST_WAVE_RET(T_TYPEID); }
    "typename"      { BOOST_WAVE_RET(T_TYPENAME); }
    "union"         { BOOST_WAVE_RET(T_UNION); }
    "unsigned"      { BOOST_WAVE_RET(T_UNSIGNED); }
    "using"         { BOOST_WAVE_RET(T_USING); }
    "virtual"       { BOOST_WAVE_RET(T_VIRTUAL); }
    "void"          { BOOST_WAVE_RET(T_VOID); }
    "volatile"      { BOOST_WAVE_RET(T_VOLATILE); }
    "wchar_t"       { BOOST_WAVE_RET(T_WCHART); }
    "while"         { BOOST_WAVE_RET(T_WHILE); }

    "{"             { BOOST_WAVE_RET(T_LEFTBRACE); }
    "??<"           { BOOST_WAVE_RET(T_LEFTBRACE_TRIGRAPH); }
    "<%"            { BOOST_WAVE_RET(T_LEFTBRACE_ALT); }
    "}"             { BOOST_WAVE_RET(T_RIGHTBRACE); }
    "??>"           { BOOST_WAVE_RET(T_RIGHTBRACE_TRIGRAPH); }
    "%>"            { BOOST_WAVE_RET(T_RIGHTBRACE_ALT); }
    "["             { BOOST_WAVE_RET(T_LEFTBRACKET); }
    "??("           { BOOST_WAVE_RET(T_LEFTBRACKET_TRIGRAPH); }
    "<:"            { BOOST_WAVE_RET(T_LEFTBRACKET_ALT); }
    "]"             { BOOST_WAVE_RET(T_RIGHTBRACKET); }
    "??)"           { BOOST_WAVE_RET(T_RIGHTBRACKET_TRIGRAPH); }
    ":>"            { BOOST_WAVE_RET(T_RIGHTBRACKET_ALT); }
    "#"             { BOOST_WAVE_RET(T_POUND); }
    "%:"            { BOOST_WAVE_RET(T_POUND_ALT); }
    "??="           { BOOST_WAVE_RET(T_POUND_TRIGRAPH); }
    "##"            { BOOST_WAVE_RET(T_POUND_POUND); }
    "#??="          { BOOST_WAVE_RET(T_POUND_POUND_TRIGRAPH); }
    "??=#"          { BOOST_WAVE_RET(T_POUND_POUND_TRIGRAPH); }
    "??=??="        { BOOST_WAVE_RET(T_POUND_POUND_TRIGRAPH); }
    "%:%:"          { BOOST_WAVE_RET(T_POUND_POUND_ALT); }
    "("             { BOOST_WAVE_RET(T_LEFTPAREN); }
    ")"             { BOOST_WAVE_RET(T_RIGHTPAREN); }
    ";"             { BOOST_WAVE_RET(T_SEMICOLON); }
    ":"             { BOOST_WAVE_RET(T_COLON); }
    "..."           { BOOST_WAVE_RET(T_ELLIPSIS); }
    "?"             { BOOST_WAVE_RET(T_QUESTION_MARK); }
    "::"            
        { 
            if (s->c99) {
                --YYCURSOR;
                BOOST_WAVE_RET(T_COLON);
            }
            else {
                BOOST_WAVE_RET(T_COLON_COLON); 
            }
        }
    "."             { BOOST_WAVE_RET(T_DOT); }
    ".*" 
        { 
            if (s->c99) {
                --YYCURSOR;
                BOOST_WAVE_RET(T_DOT);
            }
            else {
                BOOST_WAVE_RET(T_DOTSTAR); 
            }
        }
    "+"             { BOOST_WAVE_RET(T_PLUS); }
    "-"             { BOOST_WAVE_RET(T_MINUS); }
    "*"             { BOOST_WAVE_RET(T_STAR); }
    "/"             { BOOST_WAVE_RET(T_DIVIDE); }
    "%"             { BOOST_WAVE_RET(T_PERCENT); }
    "^"             { BOOST_WAVE_RET(T_XOR); }
    "??'"           { BOOST_WAVE_RET(T_XOR_TRIGRAPH); }
    "xor"           { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_XOR_ALT); }
    "&"             { BOOST_WAVE_RET(T_AND); }
    "bitand"        { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_AND_ALT); }
    "|"             { BOOST_WAVE_RET(T_OR); }
    "bitor"         { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_OR_ALT); }
    "??!"           { BOOST_WAVE_RET(T_OR_TRIGRAPH); }
    "~"             { BOOST_WAVE_RET(T_COMPL); }
    "??-"           { BOOST_WAVE_RET(T_COMPL_TRIGRAPH); }
    "compl"         { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_COMPL_ALT); }
    "!"             { BOOST_WAVE_RET(T_NOT); }
    "not"           { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_NOT_ALT); }
    "="             { BOOST_WAVE_RET(T_ASSIGN); }
    "<"             { BOOST_WAVE_RET(T_LESS); }
    ">"             { BOOST_WAVE_RET(T_GREATER); }
    "+="            { BOOST_WAVE_RET(T_PLUSASSIGN); }
    "-="            { BOOST_WAVE_RET(T_MINUSASSIGN); }
    "*="            { BOOST_WAVE_RET(T_STARASSIGN); }
    "/="            { BOOST_WAVE_RET(T_DIVIDEASSIGN); }
    "%="            { BOOST_WAVE_RET(T_PERCENTASSIGN); }
    "^="            { BOOST_WAVE_RET(T_XORASSIGN); }
    "xor_eq"        { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_XORASSIGN_ALT); }
    "??'="          { BOOST_WAVE_RET(T_XORASSIGN_TRIGRAPH); }
    "&="            { BOOST_WAVE_RET(T_ANDASSIGN); }
    "and_eq"        { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_ANDASSIGN_ALT); }
    "|="            { BOOST_WAVE_RET(T_ORASSIGN); }
    "or_eq"         { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_ORASSIGN_ALT); }
    "??!="          { BOOST_WAVE_RET(T_ORASSIGN_TRIGRAPH); }
    "<<"            { BOOST_WAVE_RET(T_SHIFTLEFT); }
    ">>"            { BOOST_WAVE_RET(T_SHIFTRIGHT); }
    ">>="           { BOOST_WAVE_RET(T_SHIFTRIGHTASSIGN); }
    "<<="           { BOOST_WAVE_RET(T_SHIFTLEFTASSIGN); }
    "=="            { BOOST_WAVE_RET(T_EQUAL); }
    "!="            { BOOST_WAVE_RET(T_NOTEQUAL); }
    "not_eq"        { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_NOTEQUAL_ALT); }
    "<="            { BOOST_WAVE_RET(T_LESSEQUAL); }
    ">="            { BOOST_WAVE_RET(T_GREATEREQUAL); }
    "&&"            { BOOST_WAVE_RET(T_ANDAND); }
    "and"           { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_ANDAND_ALT); }
    "||"            { BOOST_WAVE_RET(T_OROR); }
    "??!|"          { BOOST_WAVE_RET(T_OROR_TRIGRAPH); }
    "|??!"          { BOOST_WAVE_RET(T_OROR_TRIGRAPH); }
    "or"            { BOOST_WAVE_RET(s->c99 ? T_IDENTIFIER : T_OROR_ALT); }
    "??!??!"        { BOOST_WAVE_RET(T_OROR_TRIGRAPH); }
    "++"            { BOOST_WAVE_RET(T_PLUSPLUS); }
    "--"            { BOOST_WAVE_RET(T_MINUSMINUS); }
    ","             { BOOST_WAVE_RET(T_COMMA); }
    "->*"
        { 
            if (s->c99) {
                --YYCURSOR;
                BOOST_WAVE_RET(T_ARROW);
            }
            else {
                BOOST_WAVE_RET(T_ARROWSTAR); 
            }
        }
    "->"            { BOOST_WAVE_RET(T_ARROW); }
    "??/"           { BOOST_WAVE_RET(T_ANY_TRIGRAPH); }


    ([a-zA-Z_$] | UniversalChar) ([a-zA-Z_0-9$] | UniversalChar)*
        { BOOST_WAVE_RET(T_IDENTIFIER); }

    "L"? (['] (EscapeSequence|any\[\n\r\\']|UniversalChar)+ ['])
        { BOOST_WAVE_RET(T_CHARLIT); }

    "L"? (["] (EscapeSequence|any\[\n\r\\"]|UniversalChar)* ["])
        { BOOST_WAVE_RET(T_STRINGLIT); }


    Pound PPSpace ( "include" | "include_next") PPSpace "<" (any\[\n\r>])+ ">" 
        { BOOST_WAVE_RET(T_PP_HHEADER); }

    Pound PPSpace ( "include" | "include_next") PPSpace "\"" (any\[\n\r"])+ "\"" 
        { BOOST_WAVE_RET(T_PP_QHEADER); } 

    Pound PPSpace ( "include" | "include_next") PPSpace
        { BOOST_WAVE_RET(T_PP_INCLUDE); } 

    Pound PPSpace "if"        { BOOST_WAVE_RET(T_PP_IF); }
    Pound PPSpace "ifdef"     { BOOST_WAVE_RET(T_PP_IFDEF); }
    Pound PPSpace "ifndef"    { BOOST_WAVE_RET(T_PP_IFNDEF); }
    Pound PPSpace "else"      { BOOST_WAVE_RET(T_PP_ELSE); }
    Pound PPSpace "elif"      { BOOST_WAVE_RET(T_PP_ELIF); }
    Pound PPSpace "endif"     { BOOST_WAVE_RET(T_PP_ENDIF); }
    Pound PPSpace "define"    { BOOST_WAVE_RET(T_PP_DEFINE); }
    Pound PPSpace "undef"     { BOOST_WAVE_RET(T_PP_UNDEF); }
    Pound PPSpace "line"      { BOOST_WAVE_RET(T_PP_LINE); }
    Pound PPSpace "error"     { BOOST_WAVE_RET(T_PP_ERROR); }
    Pound PPSpace "pragma"    { BOOST_WAVE_RET(T_PP_PRAGMA); }

    Pound PPSpace "warning"   { BOOST_WAVE_RET(T_PP_WARNING); }

    Pound PPSpace "region"    { BOOST_WAVE_RET(T_MSEXT_PP_REGION); }
    Pound PPSpace "endregion" { BOOST_WAVE_RET(T_MSEXT_PP_ENDREGION); }

    [ \t\v\f]+
        { BOOST_WAVE_RET(T_SPACE); }

    Newline
    {
        s->line++;
        s->col = 1;
        BOOST_WAVE_RET(T_NEWLINE);
    }

    "\000"
    {
        BOOST_WAVE_RET(T_EOF);
    }

    any        { BOOST_WAVE_RET(UnknownTokenType); }

    anyctrl
    {
        // flag the error
        BOOST_WAVE_UPDATE_CURSOR();     // adjust the input cursor
        (*s->errorFunc)(s, LEX_ERROR,
            "invalid character '0x%x' in input stream", *--YYCURSOR);
    }
*/

ccomment:
/*!re2c
    "*/"            { BOOST_WAVE_RET(T_CCOMMENT); }
    
    Newline
    {
        /*if(cursor == s->eof) BOOST_WAVE_RET(T_EOF);*/
        /*s->tok = cursor; */
        s->line ++; // = count_backslash_newlines(s, cursor) +1;
        s->col = 1;
        goto ccomment;
    }

    any            { goto ccomment; }

    "\000"
    {
        if(*YYCURSOR == 0) 
        {
            BOOST_WAVE_UPDATE_CURSOR();   // adjust the input cursor
            (*s->errorFunc)(s, LEX_WARNING, 
                "Unterminated 'C' style comment", 0);

            return T_EOF;
        }
    }

    anyctrl
    {
        // flag the error
        BOOST_WAVE_UPDATE_CURSOR();     // adjust the input cursor
        (*s->errorFunc)(s, LEX_ERROR,
            "invalid character '0x%x' in input stream", *--YYCURSOR);
    }
*/

cppcomment:
/*!re2c
    Newline
    {
        /*if(cursor == s->eof) BOOST_WAVE_RET(T_EOF); */
        /*s->tok = cursor; */
        s->line++;
        s->col = 1;
        BOOST_WAVE_RET(T_CPPCOMMENT);
    }

    any            { goto cppcomment; }

    "\000"
    {
        --YYCURSOR;                         // next call returns T_EOF
        if (!s->single_line_only)
        {
            BOOST_WAVE_UPDATE_CURSOR();     // adjust the input cursor
            (*s->errorFunc)(s, LEX_WARNING,
                "Unterminated 'C++' style comment", 0);
        }
        BOOST_WAVE_RET(T_CPPCOMMENT);
    }

    anyctrl
    {
        // flag the error
        BOOST_WAVE_UPDATE_CURSOR();     // adjust the input cursor
        (*s->errorFunc)(s, LEX_ERROR,
            "invalid character '0x%x' in input stream", *--YYCURSOR);
    }
*/

/* this subscanner is called whenever a pp_number has been started */
pp_number:
{
	Parser ss = {0};
	ss.text   = s->cur;
	ss.end    = s->end;
	ss.cur    = ss.text;
	ss.token  = ss.cur;
	ss.errorFunc  = s->errorFunc;

    /*!re2c
        ((FractionalConstant ExponentPart?) | (Digit+ ExponentPart)) FloatingSuffix?
            { BOOST_WAVE_RET(T_FLOATLIT); }
            
        Integer { 
        
	s->text   = ss.cur;
	s->end    = ss.end;
	s->cur    = s->text;
	s->token  = s->cur;
	s->errorFunc  = ss.errorFunc;
        
        goto integer_suffix; } 
    */
}

/* this subscanner is called, whenever a Integer was recognized */
integer_suffix:
{
    /*!re2c
        LongIntegerSuffix
            { BOOST_WAVE_RET(T_LONGINTLIT); }

        IntegerSuffix?
            { BOOST_WAVE_RET(T_INTLIT); }
    */
}

