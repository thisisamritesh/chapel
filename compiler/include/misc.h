/* -*-Mode: c++;-*-
*/
#ifndef _misc_H_
#define _misc_H_

class d_loc_t;
class AST;
class Var;
class Loc;

void clean_exit(int status);

int show_error(char *str, d_loc_t &loc, ...);
int show_error(char *str, AST *a, ...);
int show_error(char *str, Var *v, ...);
int buf_read(char *pathname, char **buf, int *len);
void fail(char *str, ...);
char *loc_string(d_loc_t &l);
// char *dupstr(char *s, char *e = 0); in map.h because of template's in header hell


// INT_FATAL(ast, format, ...)
//   where ast         == AST* or NULL
//         format, ... == normal printf stuff
// results in something like:
// INTERNAL ERROR in compilerSrc.c (lineno): your text here (usrSrc:usrLineno)

#define INT_FATAL   setupIntError(__FILE__, __LINE__, true);  intProblem
#define INT_WARNING setupIntError(__FILE__, __LINE__, false); intProblem
void USR_FATAL(Loc* loc, char* str, ...);

void setupIntError(char* filename, int lineno, bool error);
void intProblem(char* fmt, ...);
void intProblem(AST* ast, char* fmt, ...);
void intProblem(Loc* loc, char* fmt, ...);
void myassert(char *file, int line, char *str);

void startCatchingSignals(void);
void stopCatchingSignals(void);

#endif
