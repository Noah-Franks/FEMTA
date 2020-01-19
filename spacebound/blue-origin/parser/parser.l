%option noyywrap
%{

#include "y.tab.h"

void yyerror(char * format, ...);
%}

%option yylineno

white  [ \t\r\n]+
letter [A-Za-z_]
digit  [0-9]

%%

\/\/.* {}                                                                    // 1 line comment
\/\*(([^*/]|\n)|(\*+([^*/]|\n)|\/([^*/]|\n)*|\/\*([^*/]|\n)*))*\**\*\/ {}    // multi comment

if        { return IF;     }
set       { return SET;    }
pin       { return PIN;    }
pos       { return POS;    }
neg       { return NEG;    }
enter     { return ENTER;  }
leave     { return LEAVE;  }
state     { return STATE;  }
State     { return STATE;  }
after     { return AFTER;  }
define    { return DEFINE; }
sensor    { return SENSOR; }
Sensor    { return SENSOR; }

(-|\+)?{digit}+{letter}+ {
  // note, units may never exceed 7 characters
  
  yylval.numeric = malloc(sizeof(*yylval.numeric));
  
  sscanf(yytext, "%d%s", &yylval.numeric -> integer, yylval.numeric -> units);
  yylval.numeric -> is_decimal = false;
  
  return NUMERIC;
}

(-|\+)?{digit}+\.{digit}+{letter}+ {
  // note, units may never exceed 7 characters
  
  yylval.numeric = malloc(sizeof(*yylval.numeric));
  
  sscanf(yytext, "%f%s", &yylval.numeric -> decimal, yylval.numeric -> units);
  yylval.numeric -> is_decimal = true;
  
  return NUMERIC;
}

(-|\+)?{digit}+ {
  yylval.numeric = calloc(1, sizeof(*yylval.numeric));
  
  yylval.numeric -> integer    = atoi(yytext);
  yylval.numeric -> units[0]   = 'i';             // the generic 32-bit int
  yylval.numeric -> is_decimal = false;
  
  return NUMERIC;
}

(-|\+)?{digit}+\.{digit}+ {
  yylval.numeric = calloc(1, sizeof(*yylval.numeric));
  
  yylval.numeric -> decimal    = atof(yytext);
  yylval.numeric -> units[0]   = 'f';             // the generic 32-bit float
  yylval.numeric -> is_decimal = true;
  
  return NUMERIC;
}

({letter}|{digit})+ {
  yylval.string = strdup(yytext);
  return ID;
}

[/=\{\}\+\-,\[\]><\(\)] {
  return yytext[0];
}

[:\|] {
  return ':';    // make ':' equal to '|'
}

{white} {}    // skip all whitespace
[;]     {}    // skip semicolons to make them optional

. {
  printf("%s\n", yytext);
  yyerror("Unexpected symbol found");
}

%%

void yyerror(char * format, ...) {
  
  va_list args;
  va_start(args, format);
  
  printf(RED "Line %d : ", yylineno);
  vprintf(format, args);
  printf("\n" RESET);
  
  va_end(args);
  
  gpioTerminate();
  exit(ERROR_EXPERIMENTER);
}
