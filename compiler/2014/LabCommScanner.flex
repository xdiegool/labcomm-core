package se.lth.control.labcomm2014.compiler;

import beaver.Symbol;
import beaver.Scanner;
import se.lth.control.labcomm2014.compiler.LabCommParser.Terminals;

%%

%public 
%final 
%class LabCommScanner
%extends Scanner

%type Symbol 
%function nextToken 
%yylexthrow Scanner.Exception

%unicode
%line %column

%{
  StringBuffer strbuf = new StringBuffer(128);

  private Symbol sym(short id) {
    return new Symbol(id, yyline + 1, yycolumn + 1, len(), str());
  }

  private Symbol sym(short id, String value) {
    return new Symbol(id, yyline + 1, yycolumn + 1, len(), value);
  }

  private String str() { return yytext(); }
  private int len() { return yylength(); }
%}

LineTerminator = \n|\r|\r\n
InputCharacter = [^\r\n]

WhiteSpace = [ ] | \t | \f | {LineTerminator}

Comment = {TraditionalComment}
        | {EndOfLineComment}

TraditionalComment = "/*" [^*] ~"*/" | "/*" "*"+ "/" | "/*" "*"+ [^/*] ~"*/"
EndOfLineComment = "//" {InputCharacter}* {LineTerminator}?

Identifier = [[:letter:]_]([[:letter:]_[:digit:]])*

DecimalNumeral = 0 | {NonZeroDigit} {Digits}? 
Digits = {Digit}+
Digit = 0 | {NonZeroDigit}
NonZeroDigit = [1-9]

%%

<YYINITIAL> {
  {WhiteSpace}                   { }
  {Comment}                      { }

  "sample"                       { return sym(Terminals.SAMPLE); }
  "typedef"                      { return sym(Terminals.TYPEDEF); }
  "struct"                       { return sym(Terminals.STRUCT); }
  "void"                         { return sym(Terminals.VOID); }
  "boolean"                      { return sym(Terminals.BOOLEAN); }
  "byte"                         { return sym(Terminals.BYTE); }
  "short"                        { return sym(Terminals.SHORT); }
  "int"                          { return sym(Terminals.INT); }
  "long"                         { return sym(Terminals.LONG); }
  "float"                        { return sym(Terminals.FLOAT); }
  "double"                       { return sym(Terminals.DOUBLE); }
  "string"                       { return sym(Terminals.STRING); }

  {DecimalNumeral}               { return sym(Terminals.INTEGER_LITERAL); }
  "_"                            { return sym(Terminals.UNDERSCORE); }
  "{"                            { return sym(Terminals.LBRACE); }
  "}"                            { return sym(Terminals.RBRACE); }
  "["                            { return sym(Terminals.LBRACK); }
  "]"                            { return sym(Terminals.RBRACK); }
  "("                            { return sym(Terminals.LPAREN); }
  ")"                            { return sym(Terminals.RPAREN); }
  ";"                            { return sym(Terminals.SEMICOLON); }
  ":"                            { return sym(Terminals.COLON); }
  ","                            { return sym(Terminals.COMMA); }
  
  {Identifier}                   { return sym(Terminals.IDENTIFIER); }
}

// fall through errors
.|\n                             { throw new RuntimeException("Illegal character \""+str()+ "\" at line "+yyline+", column "+yycolumn); }
<<EOF>>                          { return sym(Terminals.EOF); }
