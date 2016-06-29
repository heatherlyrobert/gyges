/*============================----beg-of-source---============================*/

/*===[[ MODULE ]]=============================================================*

 *   application   : s
 *   module        : s_calc
 *   purpose       : provide all code for building and executing formulas
 *   dependencies  : CELL_printable()
 *
 */
/*===[[ EXPLANATION ]]========================================================*

 *   a spreadsheet requires wicked fast calculation of formulas in order to
 *   produce reasonable performance results.  we are, by no means, attempting
 *   to compete with kitchen-sink spreadsheets from huge development teams, but
 *   what's the point of having a slow spreadsheet ;)
 *
 *   in the tradition of some of the open-source spreadsheets we have seen,
 *   calculations will be byte-compiled into a linked list of elemental
 *   operations.  besides, i bloody want to learn how to do this byte-compiling
 *   thing so that i can reuse the concept with scripting applications.
 *
 *
 *
 *
 *
 *   in the tradition of gnumeric, oleo, sc, and, i am sure, many others, s will
 *   convert all formulas to RPN for clarity, performance, and lightness.  it
 *   will then byte-compile the formulas for rapid execution.  finally, we will
 *   maintain a dependency graph to drive efficient recalculation.
 *
 *   the main parts of this module then are...
 *      - conversion from normal infix notation to postfix (rpn__output)
 *      - byte-compiling postfix calculations for execution
 *      - dependency graph driven recalculation
 *
 *   while i could likely get away with a simplier model, i have a firm belief
 *   that learning to byte-compile will be an excellent skill for any
 *   application of scripting in the future.  simply can't pass up the
 *   opportunity to learn...
 *
 *   so, we must specify an operator language for s...
 *      - support all c right-to-left operators and presidence
 *      - slightly alter meanings where necessary to use doubles vs. integers
 *      - add several useful operators using c-like notation
 *
 *
 */


#include   "gyges.h"

/*
 * calculation types
 *    f = function call
 *    n = constant number
 *    l = constant string literal
 *    s = string from another cell source field
 *    m = string from another cell modified field
 *    v = value from another cell value field
 *    x = noop
 */
struct cCALC {
   char      t;               /* type of calculation element                  */
   double    v;               /* numeric literal                              */
   char     *s;               /* string literal                               */
   tCELL    *r;               /* pointer to a cell                            */
   void    (*f) (void);       /* function pointer                             */
   tCELL    *owner;           /* pointer to the cell that owns this calc      */
   tCALC    *next;            /* pointer to next calc                         */
   tCALC    *prev;            /* pointer to next calc                         */
};
int     errornum;
char    errorstr [MAX_STR];





/*====================------------------------------------====================*/
/*===----                       private variables                      ----===*/
/*====================------------------------------------====================*/


const double DEG2RAD = 3.1415927 / 180.0;
const double RAD2DEG = 180.0 / 3.1415927;

typedef struct cTRIG  tTRIG;
struct  cTRIG {
   double      si;
   double      co;
   double      ta;
};
tTRIG   trig [3600];



PRIV   double      a, b, c, d, e;
PRIV   int         m, n, o, len;
PRIV   double      tot, min, max;
PRIV   int         cnt, cnta, cnts, cntb, cntr;
PRIV   double      entries     [1000];

PRIV   char   *q, *r, *s;
PRIV   char    t[MAX_STR];
#define  MAX   1000000000

PRIV   tCELL      *s_me;


char      nada[5] = "";


/*====================------------------------------------====================*/
/*===----                         execution stack                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___STACK___________o () { return; }
/*
 *   as the rpn calculation is read from right-to-left, the execution stack
 *   stores all initial and intermediate values until required by an operator.
 *   the results of each interim calculation are also placed back on the stack
 *   until needed by the next operator.  as a result, the size of the stack will
 *   vary wildly during the calculation.
 *
 *   as stated before, a stack is an incredibly simple and fast data structure,
 *   especially when implemented within an array.  this is one of the myriad of
 *   benefits to using postfix/rpn calculations -- they only require a stack
 *   data structure and no transformation at execution time making it fast.
 *
 *
 *
 *   the execution stack handles all values during calculation.  when a value
 *   is read, it is pushed on the stack.  when an operator is read, enough
 *   values are then popped off the stack to fulfill the operators requirements.
 *   at completion, the stack should be exactly empty.
 *
 *
 *
 *   the execution stack handles all values as they come before the operators
 *   and functions in rpn/postfix notation.  the results of interim calculations
 *   are also pushed on the stack so the size of the stack varies through-out
 *   the calculation.
 *
 *   at the end of the calculation, the stack must be empty or the calculation
 *   was misformed and an error is reported.  also, if a calculation needs
 *   values and none are left, this should also report as an error.
 *
 *   it is critical that a simple stack structure is used so that the
 *   calculations are quick and reliable.  in this case i chose an array of
 *   value entries.  i also capped the array at an arbitrarily large value.  you
 *   can change this value to whatever you like.
 *
 *   the stack can only hold three types of values: string literal, numeric
 *   literal, and a reference to a cell that contains either the string or
 *   numeric value desired.  the reference is the equivalent of using variables
 *   in a programming language.
 *
 *   if you wanted to have fun, cells could act as pointers allowing a wide
 *   variety of redirection styles, but i don't have an obvious use case that
 *   justifies that yet.  it would be dangerous to build it out before really
 *   understanding its uses or power.
 *
 *   the three value types could have been handled as a union to save space, but
 *   i opted for having separate items so that i would not make dangerous
 *   mistakes with pointers.  this is certainly not a lot of space sacrified
 *   for simplicity.  this can also be easily changed if you like.
 *
 *   the calc_stack structure can only be used by the five dedicated functions
 *   which in turn can only be used within the CALC module.  i could have made
 *   them static to ensure this, but i have an external unit test that needs
 *   access.
 *
 */

#define     MAX_STACK         1000
typedef  struct cSTACK tSTACK;
struct cSTACK {
   char      typ;            /* type : r=ref, v=val, s=str                    */
   tCELL    *ref;            /* cell reference                                */
   double    num;            /* constant value                                */
   char     *str;            /* literal string                                */
};
tSTACK      calc__stack   [MAX_STACK];
int         calc__nstack  = 0;

/*
 *   type is one of...
 *      -- v         numeric value
 *      -- s         string
 *      -- r         cell reference
 *
 */


char         /*--> add a vstring to the stack ------------[ ------ [ ------ ]-*/
CALC_pushstr       (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_new)      /* string literal                               */
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* very simply adds a stack entry with a string literal as its value       */
   /*---(defense: stack overflow)--------*//*---------------------------------*/
   if (calc__nstack >= MAX_STACK)  return -1;
   /*---(update stack item)--------------*/
   calc__stack[calc__nstack].typ = 's';
   calc__stack[calc__nstack].ref = NULL;
   calc__stack[calc__nstack].num = 0;
   calc__stack[calc__nstack].str = strndup (a_new, MAX_STR);
   /*---(update stack counter)-----------*/
   ++calc__nstack;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> add a value to the stack --------------[ ------ [ ------ ]-*/
CALC_pushval       (
      /*----------+-----------+-----------------------------------------------*/
      double      a_new)      /* numeric literal                              */
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* very simply adds a stack entry with a numeric literal as its value      */
   /*---(defense: stack overflow)--------*//*---------------------------------*/
   if (calc__nstack >= MAX_STACK)  return -1;
   /*---(update stack item)--------------*/
   calc__stack[calc__nstack].typ = 'v';
   calc__stack[calc__nstack].ref = NULL;
   calc__stack[calc__nstack].num = a_new;
   calc__stack[calc__nstack].str = NULL;
   /*---(update stack counter)-----------*/
   ++calc__nstack;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> add a reference to the stack ----------[ ------ [ ------ ]-*/
CALC_pushref       (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_new)      /* reference to cell which will have the value  */
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* adds a cell reference for later intepretation in the calculation        */
   /*---(defense: stack overflow)--------*//*---------------------------------*/
   if (calc__nstack >= MAX_STACK)  return -1;
   if (a_new        == NULL     )  return -2;
   /*---(update stack item)--------------*/
   calc__stack[calc__nstack].typ = 'r';
   calc__stack[calc__nstack].ref = a_new;
   calc__stack[calc__nstack].num = 0;
   calc__stack[calc__nstack].str = NULL;
   /*---(update stack counter)-----------*/
   ++calc__nstack;
   /*---(complete)-----------------------*/
   return 0;
}

double       /*--> get an numeric off the stack ----------[ ------ [ ------ ]-*/
CALC__popval          (void)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* always returns a value for the stack entry.                             */
   /* -- for a numeric literal, it returns the number field on the stack item */
   /* -- for a string literal, it returns 0.0 (since strings have no value)   */
   /* -- for a reference, it returns the value contained in that cell         */
   /* -- if it can't figure it out, it returns a 0.0                          */
   /*---(prepare)------------------------*/
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'v' :  return  calc__stack[calc__nstack].num;           break;
   case 's' :  return  0.0;                                     break;
   case 'r' :  return  calc__stack[calc__nstack].ref->v_num;    break;
   }
   /*---(complete)-----------------------*/
   return 0.0;
}

char*        /*--> get an string off the stack -----------[ ------ [ ------ ]-*/
CALC__popstr       (void)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* always returns a string for the stack entry.                            */
   /* -- for a numeric literal, it returns an empty string                    */
   /* -- for a string literal, it returns the string from the stack           */
   /* -- for a reference that's unused, it returns an empty string            */
   /* -- for a reference that's numeric, it returns an empty string           */
   /* -- for a reference that's modified, it returns the string value         */
   /* -- for a reference that's unmodified, it returns the source value       */
   /* -- for a reference that's unknown, it returns an empty string           */
   /* -- if it can't figure it out, it returns an empty string                */
   /*---(prepare)------------------------*/
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'v' :  return  strndup (nada, MAX_STR);                 break;
   case 's' :  return  calc__stack[calc__nstack].str;           break;
   case 'r' :  if        (calc__stack[calc__nstack].ref->t == '-') {
                  return  strndup (nada, MAX_STR);
               } else if (calc__stack[calc__nstack].ref->v_str != NULL) {
                  return  strndup (calc__stack[calc__nstack].ref->v_str, MAX_STR);
               } else if (calc__stack[calc__nstack].ref->s != NULL) {
                  return  strndup (calc__stack[calc__nstack].ref->s, MAX_STR);
               } else {
                  return strndup (nada, MAX_STR);
               }
   }
   /*---(complete)-----------------------*/
   return strndup (nada, MAX_STR);
}

tCELL*       /*--> get a reference off the stack ---------[ ------ [ ------ ]-*/
CALC__popref          (void)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /*---(prepare)------------------------*/
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'r' :  return  calc__stack[calc__nstack].ref;           break;
   }
   /*---(complete)-----------------------*/
   return NULL;
}

tCELL*       /*--> get a reference off the stack ---------[ ------ [ ------ ]-*/
CALC__popprint        (void)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /*---(prepare)------------------------*/
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'r' :  return  strndup (calc__stack[calc__nstack].ref->p, MAX_STR);        break;
   }
   /*---(complete)-----------------------*/
   return NULL;
}



/*====================------------------------------------====================*/
/*===----                        error setting                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ERROR___________o () { return; }

PR void
CALC__seterror     (int a_num, char *a_str)
{
   errornum = a_num;
   strncpy (errorstr     , a_str, MAX_STR);
   return;
}



/*====================------------------------------------====================*/
/*===----                      special operators                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SPECIAL_________o () { return; }

PRIV void
CALC__noop        (void)
{
   return;
}



/*====================------------------------------------====================*/
/*===----                     arithmetic operators                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ARITHMETIC______o () { return; }

PRIV void
CALC__add         (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b + a);
   return;
}

PRIV void
CALC__subtract     (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b - a);
   return;
}

PRIV void
CALC__multiply     (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b * a);
   return;
}

PRIV void
CALC__divide       (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   if (a != 0)  CALC_pushval (b / a);
   else         CALC_pushval (0);
   return;
}

PRIV void
CALC__modulus      (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (((int) b) % ((int) a));
   return;
}

PRIV void
CALC__increment    (void)
{
   a = CALC__popval ();
   CALC_pushval (a + 1);
   return;
}

PRIV void
CALC__decrement    (void)
{
   a = CALC__popval ();
   CALC_pushval (a - 1);
   return;
}



/*====================------------------------------------====================*/
/*===----                     relational operators                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___RELATIONAL______o () { return; }

PRIV void
CALC__equal        (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b == a);
   return;
}

PRIV void
CALC__notequal      (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b != a);
   return;
}

PRIV void
CALC__greater      (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b > a);
   return;
}

PRIV void
CALC__lesser       (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b < a);
   return;
}

PRIV void
CALC__gequal       (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b >= a);
   return;
}

PRIV void
CALC__lequal       (void)
{
   a = CALC__popval( );
   b = CALC__popval ();
   CALC_pushval (b <= a);
   return;
}



/*====================------------------------------------====================*/
/*===----                       logical operators                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LOGICAL_________o () { return; }

PRIV void
CALC__not          (void)
{
   a = CALC__popval ();
   CALC_pushval (!a);
   return;
}

PRIV void
CALC__and          (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b && a);
   return;
}

PRIV void
CALC__or           (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (b || a);
   return;
}



/*====================------------------------------------====================*/
/*===----                       string functions                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___STRING__________o () { return; }

PRIV void
CALC__concat       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr();
   s = CALC__popstr();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (s == NULL)  s = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strncpy (t, s,   MAX_STR);
   strncat (t, r,   MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   free (s);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__concatplus   (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr();
   s = CALC__popstr();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (s == NULL)  s = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strncpy (t, s,   MAX_STR);
   strncat (t, " ", MAX_STR);
   strncat (t, r,   MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   free (s);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__lower        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strncpy (t, r, MAX_STR);
   len = strlen(t);
   int i;
   for (i = 0; i < len; ++i) t[i] = tolower(t[i]);
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__upper        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   len = strlen(r);
   strncpy (t, r, MAX_STR);
   int i;
   for (i = 0; i < len; ++i) t[i] = toupper(t[i]);
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__char         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval ();
   /*---(defense)------------------------*/
   if (n     <  32 )  n = '#';
   if (n     >= 127)  n = '#';
   /*---(process)------------------------*/
   sprintf (t, "%c", n);
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__code         (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushval (r[0]);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__len          (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   len = strlen (r);
   /*---(return result)------------------*/
   CALC_pushval ((double) len);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__left         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval ();
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   len = strlen (r);
   strncpy (t, r, MAX_STR);
   if (n >= len)  n = len;
   t[n] = '\0';
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__right        (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval ();
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   len = strlen (r);
   m = len - n;
   if (m >= len)  m = len;
   int i;
   for (i = m; i < len; ++i) t[i - m] = r[i];
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__mid          (void)
{
   /*---(get arguments)------------------*/
   m = CALC__popval ();
   n = CALC__popval ();
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (m     <  0  )  m = 0;
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   len = strlen (r);
   if (n     >= len)  n = len;
   if (n + m >= len)  m = len - n;
   int i;
   for (i = n; i < n + m; ++i) t[i - n] = r[i];
   /*---(return result)------------------*/
   CALC_pushstr (t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

/*> #define    ySTR_HEAD      'h'                                                     <* 
 *> #define    ySTR_TAIL      't'                                                     <* 
 *> #define    ySTR_BOTH      'b'                                                     <* 
 *> #define    ySTR_EVERY     'e'                                                     <* 
 *> #define    ySTR_COMPRESS  'c'                                                     <*/

PRIV void
CALC__trim         (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__ltrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_HEAD, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__rtrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_TAIL, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__strim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_SINGLE, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__etrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_EVERY, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__mtrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_MAX, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__print        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popprint ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, MAX_STR);
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__lpad         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval ();
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (r);
   } else {
      strncpy (t, empty, n - m);
      t [m] = '\0';
      strcat  (t, r);
      CALC_pushstr (t);
   }
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__rpad         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval ();
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (r);
   } else {
      strcpy  (t, r);
      strncat (t, empty, n - m);
      t [n] = '\0';
      CALC_pushstr (t);
   }
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__lppad        (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval   ();
   r = CALC__popprint ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, MAX_STR);
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (r);
   } else {
      strncpy (t, empty, n - m);
      t [n-m-1] = '\0';
      strcat  (t, r);
      CALC_pushstr (t);
   }
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__rppad        (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval   ();
   r = CALC__popprint ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, MAX_STR);
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (r);
   } else {
      strcpy  (t, r);
      strncat (t, empty, n - m);
      t [n] = '\0';
      CALC_pushstr (t);
   }
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__value        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   a = atof (r);
   /*---(return result)------------------*/
   CALC_pushval (a);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salpha       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALPHA, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salphac      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALPHA, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salnum       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALNUM, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salnumc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALNUM, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sbasic       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_BASIC, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sbasicc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_BASIC, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__swrite       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_WRITE, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__swritec      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_WRITE, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sexten       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_EXTEN, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sextenc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_EXTEN, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sprint       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_PRINT, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sprintc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_PRINT, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sseven       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_SEVEN, '-', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__ssevenc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   /*---(process)------------------------*/
   strlclean (r, ySTR_SEVEN, 'y', MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__replace      (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval ();
   s = CALC__popstr ();
   q = CALC__popstr ();
   r = CALC__popstr ();
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, MAX_STR);
   if (q == NULL)  q = strndup (nada, MAX_STR);
   if (s == NULL)  s = strndup (nada, MAX_STR);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   strlrepl (r, q, s, n, MAX_STR);
   /*---(return result)------------------*/
   CALC_pushstr (r);
   /*---(clean up)-----------------------*/
   free (q);
   free (r);
   free (s);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__prev         (void)
{
   /*---(complete)-----------------------*/
   return;
}




/*====================------------------------------------====================*/
/*===----                     mathmatical functions                    ----===*/
/*====================------------------------------------====================*/

/*
 *  included...     abs, trunc, round, ceil, floor, sqrt, rand
 *  as operator...  exp
 *  not included... roundto, roundup, rounddown, ...
 *
 *
 *
 */

PRIV void  o___MATH____________o () { return; }

PRIV void
CALC__power        (void)
{
   int   i = 0;
   a = CALC__popval ();
   b = CALC__popval ();
   c = b;
   for (i = 1; i <  a; ++i)  c *= b;
   CALC_pushval (c);
   return;
}

PRIV void
CALC__abs           (void)
{
   a = CALC__popval ();
   CALC_pushval (fabs(a));
   return;
}

PRIV void
CALC__rtrunc        (void)
{
   int i = 0;
   n = CALC__popval ();
   a = CALC__popval ();
   for (i = 0; i < n; ++i)  a *= 10;
   a = trunc (a);
   for (i = 0; i < n; ++i)  a /= 10;
   CALC_pushval (a);
   return;
}

PRIV void
CALC__trunc         (void)
{
   a = CALC__popval ();
   CALC_pushval (trunc(a));
   return;
}

PRIV void
CALC__rround        (void)
{
   int i = 0;
   n = CALC__popval ();
   a = CALC__popval ();
   for (i = 0; i < n; ++i)  a *= 10;
   a = round (a);
   for (i = 0; i < n; ++i)  a /= 10;
   CALC_pushval (a);
   return;
}

PRIV void
CALC__round         (void)
{
   a = CALC__popval ();
   CALC_pushval (round(a));
   return;
}

PRIV void
CALC__ceiling       (void)
{
   a = CALC__popval ();
   CALC_pushval (ceil(a));
   return;
}

PRIV void
CALC__floor         (void)
{
   a = CALC__popval ();
   CALC_pushval (floor(a));
   return;
}

PRIV void       /* PURPOSE : rand between n-m, not just 0-1 ---------------*/
CALC__rand          (void)
{
   CALC_pushval ((double) rand());
   return;
}

PRIV void       /* PURPOSE : rand between n-m, not just 0-1 ---------------*/
CALC__randr         (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (((double) rand() / (double) RAND_MAX) * (b - a) + a);
   return;
}

PRIV void
CALC__sqrt          (void)
{
   a = CALC__popval ();
   CALC_pushval (sqrt(a));
   return;
}

PRIV void
CALC__cbrt          (void)
{
   a = CALC__popval ();
   CALC_pushval (cbrt(a));
   return;
}

PRIV void
CALC__sqr           (void)
{
   a = CALC__popval ();
   CALC_pushval (a * a);
   return;
}

PRIV void
CALC__cube          (void)
{
   a = CALC__popval ();
   CALC_pushval (a * a * a);
   return;
}



/*====================------------------------------------====================*/
/*===----                         trig functions                       ----===*/
/*====================------------------------------------====================*/

PRIV void  o___TRIG____________o () { return; }

PRIV void
CALC__degrees       (void)
{
   a = CALC__popval ();
   CALC_pushval (a * RAD2DEG);
   return;
}

PRIV void
CALC__radians       (void)
{
   a = CALC__popval ();
   CALC_pushval (a * DEG2RAD);
   return;
}

PRIV void
CALC__pi            (void)
{
   CALC_pushval (3.1415927);
   return;
}

PRIV void
CALC__hypot         (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (sqrt(a * a + b * b));
   return;
}

PRIV void
CALC__side          (void)
{
   b = CALC__popval ();
   c = CALC__popval ();
   CALC_pushval (sqrt(c * c - b * b));
   return;
}

PRIV void
CALC__sin           (void)
{
   int i;
   a = CALC__popval ();
   i = round (a * 10.0);
   i = i % 3600;
   if (i < 0)  i = 3600 + i;
   CALC_pushval (trig[i].si);
   return;
}

PRIV void
CALC__sinr          (void)
{
   CALC__degrees ();
   CALC__sin     ();
   return;
}

PRIV void
CALC__csc           (void)
{
   CALC__sin     ();
   a = CALC__popval ();
   CALC_pushval (1);
   CALC_pushval (a);
   CALC__divide ();
   return;
}

PRIV void
CALC__cscr          (void)
{
   CALC__degrees ();
   CALC__csc     ();
   return;
}

PRIV void
CALC__cos           (void)
{
   int i;
   a = CALC__popval ();
   i = round (a * 10.0);
   i = i % 3600;
   if (i < 0)  i = 3600 + i;
   CALC_pushval (trig[i].co);
   return;
}

PRIV void
CALC__cosr          (void)
{
   CALC__degrees ();
   CALC__cos     ();
   return;
}

PRIV void
CALC__sec           (void)
{
   CALC__cos     ();
   a = CALC__popval ();
   CALC_pushval (1);
   CALC_pushval (a);
   CALC__divide ();
   return;
}

PRIV void
CALC__secr          (void)
{
   CALC__degrees ();
   CALC__sec     ();
   return;
}

PRIV void
CALC__tan           (void)
{
   int i;
   a = CALC__popval ();
   i = round (a * 10.0);
   i = i % 3600;
   if (i < 0)  i = 3600 + i;
   CALC_pushval (trig[i].ta);
   return;
}

PRIV void
CALC__tanr          (void)
{
   CALC__degrees ();
   CALC__tan     ();
   return;
}

PRIV void
CALC__cot           (void)
{
   CALC__tan     ();
   a = CALC__popval ();
   CALC_pushval (1);
   CALC_pushval (a);
   CALC__divide ();
   return;
}

PRIV void
CALC__cotr          (void)
{
   CALC__degrees ();
   CALC__cot     ();
   return;
}

PRIV void
CALC__crd           (void)
{
   a = CALC__popval ();
   CALC_pushval (a / 2.0);
   CALC__sin     ();
   a = CALC__popval ();
   CALC_pushval (a * 2.0);
   return;
}

PRIV void
CALC__crdr          (void)
{
   CALC__degrees ();
   CALC__crd     ();
   return;
}

PRIV void
CALC__asin          (void)
{
   a = CALC__popval ();
   CALC_pushval (asin(a) * RAD2DEG);
   return;
}

PRIV void
CALC__asinr         (void)
{
   a = CALC__popval ();
   CALC_pushval (asin(a));
   return;
}

PRIV void
CALC__acos          (void)
{
   a = CALC__popval ();
   CALC_pushval (acos(a) * RAD2DEG);
   return;
}

PRIV void
CALC__acosr         (void)
{
   a = CALC__popval ();
   CALC_pushval (acos(a));
   return;
}

PRIV void
CALC__atan          (void)
{
   a = CALC__popval ();
   CALC_pushval (atan(a) * RAD2DEG);
   return;
}

PRIV void
CALC__atanr         (void)
{
   a = CALC__popval ();
   CALC_pushval (atan(a));
   return;
}

PRIV void
CALC__atan2         (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (atan2(b,a) * RAD2DEG);
   return;
}

PRIV void
CALC__atanr2        (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   CALC_pushval (atan2(b,a));
   return;
}



/*====================------------------------------------====================*/
/*===----                    cell address functions                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ADDRESS_________o () { return; }

PRIV void
CALC__offset        (int a_tab, int a_col, int a_row)
{
   char   rc;
   int    x_tab;
   int    x_col;
   int    x_row;
   int    x_abs;
   tCELL *x_base;
   tCELL *x_new;
   x_base = CALC__popref ();
   if (x_base == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_base, &x_tab, &x_col, &x_row);
   DEBUG_CALC   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_CALC   yLOG_value   ("x_bcol"    , x_col);
   DEBUG_CALC   yLOG_value   ("x_brow"    , x_row);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   x_tab += a_tab;
   x_col += a_col;
   x_row += a_row;
   rc     = LOC_legal  (x_tab, x_col, x_row, CELL_FIXED);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   x_new  = LOC_cell   (x_tab, x_col, x_row);
   if (x_new == NULL)                           CALC_pushval (0);
   else if (x_new->s == NULL)                   CALC_pushval (0);
   else if (x_new->t == 'n' || x_new->t == 'f') CALC_pushval (x_new->v_num);
   else                                         CALC_pushstr (x_new->s);
   return;
}

PRIV void
CALC__offs          (void)
{
   n = CALC__popval ();
   m = CALC__popval ();
   o = CALC__popval ();
   CALC__offset  (    o,    m,    n);
   return;
}

PRIV void
CALC__offt          (void)
{
   n = CALC__popval ();
   CALC__offset  (    n,    0,    0);
   return;
}

PRIV void
CALC__offc          (void)
{
   n = CALC__popval ();
   CALC__offset  (    0,    n,    0);
   return;
}

PRIV void
CALC__offr          (void)
{
   n = CALC__popval ();
   CALC__offset  (    0,    0,    n);
   return;
}

PRIV void
CALC__loc           (void)
{
   char   rc;
   tCELL *x_new;
   n = CALC__popval ();
   m = CALC__popval ();
   o = CALC__popval ();
   rc = LOC_legal  (    o,    m,    n, CELL_FIXED);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   x_new  = LOC_cell   (o, m, n);
   CALC_pushref (x_new);
   return;
}

PRIV void
CALC__address       (char a_type)
{
   char   rc;
   int    x_tab;
   int    x_col;
   int    x_row;
   tCELL *x_base;
   x_base = CALC__popref ();
   if (x_base == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_base, &x_tab, &x_col, &x_row);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   switch (a_type) {
   case 't' :  CALC_pushval (x_tab);  break;
   case 'c' :  CALC_pushval (x_col);  break;
   default  :  CALC_pushval (x_row);  break;
   }
   return;
}

PRIV void
CALC__addr          (void)
{
   tCELL *x_base;
   x_base = CALC__popref ();
   if (x_base == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   CALC_pushstr (x_base->label);
   return;
}

PRIV void
CALC__cell          (a_type)
{
   tCELL *x_base;
   char   x_type  = a_type;
   x_base = CALC__popref ();
   if (x_base == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   if (x_base->t == a_type)  CALC_pushval (TRUE);
   else                      CALC_pushval (FALSE);
   return;
}

PRIV void
CALC__isnum         (void)
{
   CALC__cell ('n');
   return;
}

PRIV void
CALC__isstr         (void)
{
   CALC__cell ('s');
   return;
}

PRIV void
CALC__me            (void)
{
   CALC_pushstr (s_me->label);
   return;
}

PRIV void
CALC__filename      (void)
{
   CALC_pushstr (f_title);
   return;
}

PRIV void
CALC__filebase      (void)
{
   CALC_pushstr (f_name);
   return;
}

PRIV void
CALC__tabname       (void)
{
   CALC__address ('t');
   n = CALC__popval ();
   CALC_pushstr (tabs[n].name);
   return;
}

PRIV void
CALC__tab           (void)
{
   CALC__address ('t');
   return;
}

PRIV void
CALC__col           (void)
{
   CALC__address ('c');
   return;
}

PRIV void
CALC__row           (void)
{
   CALC__address ('r');
   return;
}

PRIV void        /* PURPOSE : total the numeric cells in a range ----------*/
CALC__rangestat    (char a_type)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_beg       = NULL;
   int         x_btab      = 0;
   int         x_bcol      = 0;
   int         x_brow      = 0;
   tCELL      *x_end       = NULL;
   int         x_etab      = 0;
   int         x_ecol      = 0;
   int         x_erow      = 0;
   char        rc          = 0;
   /*---(get values)---------------------*/
   x_end = CALC__popref ();
   DEBUG_CALC   yLOG_point   ("x_end"     , x_end);
   x_beg = CALC__popref ();
   DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
   /*---(defense)------------------------*/
   if (x_beg == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_beg, &x_btab, &x_bcol, &x_brow);
   DEBUG_CALC   yLOG_value   ("x_btab"    , x_btab);
   DEBUG_CALC   yLOG_value   ("x_bcol"    , x_bcol);
   DEBUG_CALC   yLOG_value   ("x_brow"    , x_brow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   if (x_end == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_end, &x_etab  , &x_ecol, &x_erow);
   DEBUG_CALC   yLOG_value   ("x_etab"    , x_etab);
   DEBUG_CALC   yLOG_value   ("x_ecol"    , x_ecol);
   DEBUG_CALC   yLOG_value   ("x_erow"    , x_erow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   /*---(process)------------------------*/
   switch (a_type) {
   case 't' :  CALC_pushval (x_etab - x_btab + 1);  break;
   case 'c' :  CALC_pushval (x_ecol - x_bcol + 1);  break;
   default  :  CALC_pushval (x_erow - x_brow + 1);  break;
   }
   return;
}

PRIV void
CALC__tabs          (void)
{
   CALC__rangestat ('t');
   return;
}

PRIV void
CALC__cols          (void)
{
   CALC__rangestat ('c');
   return;
}

PRIV void
CALC__rows          (void)
{
   CALC__rangestat ('r');
   return;
}



/*====================------------------------------------====================*/
/*===----                      information functions                   ----===*/
/*====================------------------------------------====================*/
PRIV void  o___INFO____________o () { return; }



/*====================------------------------------------====================*/
/*===----                         logic functions                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LOGIC___________o () { return; }

/* >> there are only six logic functions in excel (office.microsoft.com)...
 *     - VALUED         : IF
 *     - MADE CONSTANT  : TRUE, FALSE exactly like C
 *     - MADE OPERATOR  : AND (&&), OR (||), NOT (!) exactly like C
 *
 */

PRIV void
CALC__if            (void)
{
   a = CALC__popval ();
   b = CALC__popval ();
   c = CALC__popval ();
   if (c) CALC_pushval (b);
   else   CALC_pushval (a);
   return;
}


/*====================------------------------------------====================*/
/*===----                    date and time functions                   ----===*/
/*====================------------------------------------====================*/
PRIV void  o___TIME____________o () { return; }

/* >> there are only six logic functions in excel (office.microsoft.com)...
 *     - VALUED         : NOW, TODAY, DATEVALUE, TIMEVALUE
 *     - HELPERS        : YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, WEEKDAY
 *     - left off       : YEARFRAC, WORKDAY, NETWORKDAYS, EOMONTH, EDATE,
 *                        DAYS360
 */

PRIV void
CALC__now           (void)
{
   CALC_pushval (time(NULL));
   return;
}

PRIV void
CALC__year          (void)
{
   char temp[100];
   a = CALC__popval ();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%Y", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__month         (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%m", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__day           (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%d", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__hour          (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%H", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__minute        (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%M", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__second        (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%S", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__weekday       (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%w", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__weeknum       (void)
{
   char temp[100];
   a = CALC__popval();
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%W", localtime(&xtime));
   CALC_pushval (atoi(temp));
   return;
}

PRIV void
CALC__timevalue     (void)
{
   /*---(locals)-----------+-----------+-*/
   int         se = 0;
   int         mn = 0;
   int         hr = 0;
   int         mo = 0;
   int         dy = 0;
   int         yr = 0;
   int         rc = 0;
   struct tm  *temp;
   time_t      x_time;
   ullong      x_now;
   /*---(set defaults)-------------------*/
   x_now = time (NULL);
   temp = localtime(&x_now);
   se = temp->tm_sec;
   mn = temp->tm_min;
   hr = temp->tm_hour + 1;
   dy = temp->tm_mday;
   mo = temp->tm_mon + 1;
   yr = temp->tm_year + 1900;
   /*---(pop string)---------------------*/
   r   = CALC__popstr();
   /*---(attempt to read)----------------*/
   rc  = sscanf (r, "%d/%d/%d %d:%d:%d", &mo, &dy, &yr, &hr, &mn, &se);
   if (rc < 6) {
      rc  = sscanf (r, "%d/%d/%d %d:%d", &mo, &dy, &yr, &hr, &mn);
      if (rc < 5) {
         rc  = sscanf (r, "%d/%d/%d", &mo, &dy, &yr);
         if (rc < 3) {
            rc  = sscanf (r, "%d/%d", &mo, &dy);
            if (rc < 2) {
               rc  = sscanf (r, "%d:%d", &hr, &mn);
               if (rc < 2) {
                  CALC__seterror ( -1, "#.range");
                  return;
               }
            }
         }
      }
   }
   /*---(convert)------------------------*/
   temp->tm_sec  = se;
   temp->tm_min  = mn;
   temp->tm_hour = hr - 1;
   temp->tm_mday = dy;
   temp->tm_mon  = mo - 1;
   if      (yr <  50  ) temp->tm_year = 100 + yr;
   else if (yr <  100 ) temp->tm_year = yr;
   else if (yr <  1900) { CALC__seterror ( -1, "#.year");  return; }
   else if (yr >= 1900) temp->tm_year = yr - 1900;
   time_t  xtime = mktime(temp);
   /*---(complete)-----------------------*/
   CALC_pushval ((double) xtime);
   return;
}


/*====================------------------------------------====================*/
/*===----                        range functions                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___RANGE___________o () { return; }
/*
 *  as part of building this spreadsheet, i developed separate functions for
 *  each of the range applications.  this was done to increase speed, but
 *  along the way i had to debug and update causing a fair amount of duplicate
 *  updates to each function and potential for mistakes.
 *
 *  i have now seen how fast the spreadsheet is so i have decided to trade the
 *  speed of separate functions for increased testability and maintainability.
 *  i could either have passed a function pointer to perform the operation or
 *  perform all the calculations and then return the appropriate value.  since
 *  the calculations are simple and fast, i opted for the clarity of wrapper
 *  function.
 *
 */

PRIV void        /* PURPOSE : total the numeric cells in a range ----------*/
CALC__gather       (void)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_beg       = NULL;
   int         x_tab       = 0;
   int         x_bcol      = 0;
   int         x_brow      = 0;
   tCELL      *x_end       = NULL;
   int         x_ecol      = 0;
   int         x_erow      = 0;
   int         x_crow      = 0;
   int         x_ccol      = 0;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   /*---(get values)---------------------*/
   x_end = CALC__popref ();
   DEBUG_CALC   yLOG_point   ("x_end"     , x_end);
   x_beg = CALC__popref ();
   DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
   /*---(defense)------------------------*/
   if (x_beg == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_beg, &x_tab, &x_bcol, &x_brow);
   DEBUG_CALC   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_CALC   yLOG_value   ("x_bcol"    , x_bcol);
   DEBUG_CALC   yLOG_value   ("x_brow"    , x_brow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   if (x_end == NULL)   { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_end, NULL  , &x_ecol, &x_erow);
   DEBUG_CALC   yLOG_value   ("x_ecol"    , x_ecol);
   DEBUG_CALC   yLOG_value   ("x_erow"    , x_erow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )   { CALC__seterror ( -1, "#.range");  return; }
   /*---(process)------------------------*/
   cnt = cnta = cnts = cntb = cntr = 0;
   tot = 0;
   min =   MAX;
   max = -(MAX);
   for (x_ccol = x_bcol; x_ccol <= x_ecol; ++x_ccol) {
      for (x_crow = x_brow; x_crow <= x_erow; ++x_crow) {
         DEBUG_CALC   yLOG_value   ("x_ccol"    , x_ccol);
         DEBUG_CALC   yLOG_value   ("x_crow"    , x_crow);
         x_curr = tabs[x_tab].sheet[x_ccol][x_crow];
         DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
         if (x_curr == NULL)                    continue;
         DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
         ++cntr;
         if (strchr("nf", x_curr->t) != 0   ) {
            DEBUG_CALC   yLOG_note    ("numeric");
            ++cnt;
            ++cnta;
            c    = x_curr->v_num;
            tot += c;
            if (c < min)  min = c;
            if (c > max)  max = c;
            entries [cnt - 1] = c;
            DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
            DEBUG_CALC   yLOG_value   ("entry"     , entries [cnt - 1]);
         } else if (strchr("sm", x_curr->t) != 0   ) {
            DEBUG_CALC   yLOG_note    ("string");
            ++cnts;
            ++cnta;
         } else {
            DEBUG_CALC   yLOG_note    ("blank");
            ++cntb;
         }
         DEBUG_CALC   yLOG_value   ("c"         , c);
      }
   }
   if (min ==   MAX )  min = 0;
   if (max == -(MAX))  max = 0;
   /*---(complete)-----------------------*/
   return;
}

PRIV void    /*--> find max of all numeric cells in range [ ------ [ ------ ]-*/
CALC__rangesort    (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =  0;            /* loop iterator -- entry         */
   int         tele        = -1;            /* teleport point to speed sort   */
   int         mid         =  0;            /* midpoint of entries            */
   int         runs        =  0;
   /*---(prepare)------------------------*/
   DEBUG_CALC   yLOG_note    ("before sorting");
   for (i = 0; i < cnt; ++i)  DEBUG_CALC   yLOG_complex ("entry"     , "%3d = %10.2lf", i, entries [i]);
   i = 1;
   /*---(sort)---------------------------*/
   DEBUG_CALC   yLOG_note    ("sorting");
   while (i < cnt) {
      ++runs;
      if (runs > 500)  break;
      /*---(load vars)-------------------*/
      a = entries [i - 1];
      b = entries [i];
      DEBUG_CALC   yLOG_complex ("current"   , "compare i,%3d (t,%3d) =  a,%10.2lf to b,%10.2lf", i, tele, a, b);
      /*---(compare)---------------------*/
      if (i == 0 || a <= b) {
         if (tele >= 0) {
            i    = tele;
            tele = -1;
         } else {
            ++i;
         }
         continue;
      }
      /*---(swap)------------------------*/
      entries [i - 1] = b;
      entries [i]     = a;
      a = entries [i - 1];
      b = entries [i];
      DEBUG_CALC   yLOG_complex ("swapped"   , "now     i,%3d (t,%3d) =  a,%10.2lf to b,%10.2lf", i, tele, a, b);
      /*---(update)----------------------*/
      if (tele < 0) tele = i;
      if (i > 1) --i;
   }
   /*---(wrapup)-------------------------*/
   DEBUG_CALC   yLOG_note    ("after sorting");
   for (i = 0; i < cnt; ++i)  DEBUG_CALC   yLOG_complex ("entry"     , "%3d = %10.2lf", i, entries [i]);
   /*---(complete)-----------------------*/
   return;
}


PRIV void    /*--> total all numeric cells in a range ----[ ------ [ ------ ]-*/
CALC__sum          (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (tot);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count all numeric cells in a range ----[ ------ [ ------ ]-*/
CALC__count        (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (cnt);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count all filled cells in a range -----[ ------ [ ------ ]-*/
CALC__counts       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (cnts);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count all filled cells in a range -----[ ------ [ ------ ]-*/
CALC__counta       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (cnta);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count non-filled cells in a range -----[ ------ [ ------ ]-*/
CALC__countb       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (cntb);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count non-filled cells in a range -----[ ------ [ ------ ]-*/
CALC__countr       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (cntr);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> average all numeric cells in a range --[ ------ [ ------ ]-*/
CALC__average      (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0) {
      if (cnt > 0)  CALC_pushval   (tot / cnt);
      else          CALC_pushval   (cnt);
   }
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find min of all numeric cells in range [ ------ [ ------ ]-*/
CALC__minimum      (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (min);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find max of all numeric cells in range [ ------ [ ------ ]-*/
CALC__maximum      (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (max);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find max of all numeric cells in range [ ------ [ ------ ]-*/
CALC__range        (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum == 0)  CALC_pushval   (max - min);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

double    q1, q2, q3, qr;

PRIV void    /*--> find the quartile values --------------[ ------ [ ------ ]-*/
CALC__quarters     (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_qtr       =  0;
   int         x_1st1      =  0;            /* first quartile point           */
   int         x_1st2      =  0;            /* first quartile point           */
   double      x_1stw      =  0;            /* first quartile point           */
   int         x_mid1      =  0;            /* midpoint of entries            */
   int         x_mid2      =  0;            /* midpoint of entries            */
   int         x_3rd1      =  0;            /* third quartile point           */
   int         x_3rd2      =  0;            /* third quartile point           */
   double      x_3rdw      =  0;            /* third quartile point           */
   /*---(start)--------------------------*/
   CALC__gather ();
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
   if      (cnt == 0)  {
      DEBUG_CALC   yLOG_value   ("pushing"   , 0);
      CALC_pushval ( 0);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   CALC__rangesort ();
   /*---(figure out median)--------------*/
   switch (cnt % 2) {
   case 0 :  /* even means average needed   */
      x_mid1 = (cnt / 2.0) - 1;
      x_mid2 = x_mid1 + 1;
      break;
   case 1 :  /* odd means an exact middle   */
      x_mid1 = (cnt / 2);
      x_mid2 = x_mid1;
      break;
   }
   q2 = (entries [x_mid1] + entries [x_mid2]) / 2.0;
   /*---(figure out quarters)------------*/
   x_qtr = cnt / 4;
   switch (cnt % 4) {
   case 0 :
      DEBUG_CALC   yLOG_note    ("zero");
      x_1st1 = x_qtr        - 1;
      x_1st2 = x_1st1 + 1;
      x_1stw = 2.0;
      x_3rd1 = x_qtr  * 3.0 - 1;
      x_3rd2 = x_3rd1 + 1;
      x_3rdw = 2.0;
      break;
   case 1 :
      DEBUG_CALC   yLOG_note    ("one");
      x_1st1 = x_qtr        - 1;
      x_1st2 = x_1st1 + 1;
      x_1stw = 1.0;
      x_3rd1 = x_qtr  * 3.0;
      x_3rd2 = x_3rd1 + 1;
      x_3rdw = 3.0;
      break;
   case 2 :  /* even in each half, so quartile is exact element  */ 
      DEBUG_CALC   yLOG_note    ("two");
      x_1st1 = x_qtr;
      x_1st2 = x_1st1;
      x_1stw = 2.0;
      x_3rd1 = x_qtr  * 3.0 + 1;
      x_3rd2 = x_3rd1;
      x_3rdw = 2.0;
      break;
   case 3 :
      DEBUG_CALC   yLOG_note    ("three");
      x_1st1 = x_qtr;
      x_1st2 = x_1st1 + 1;
      x_1stw = 3.0;
      x_3rd1 = x_qtr  * 3.0 + 1;
      x_3rd2 = x_3rd1 + 1;
      x_3rdw = 1.0;
      break;
   }
   DEBUG_CALC   yLOG_complex ("x_1st1"    , "i=%3d, v=%10.5lf, w=%4.2lf, f=%10.5lf", x_1st1, entries [x_1st1], x_1stw      , entries [x_1st1] * x_1stw);
   DEBUG_CALC   yLOG_complex ("x_1st2"    , "i=%3d, v=%10.5lf, w=%4.2lf, f=%10.5lf", x_1st2, entries [x_1st2], 4.0 - x_1stw, entries [x_1st2] * (4.0 - x_1stw));
   q1 = ((entries [x_1st1] * x_1stw) + (entries [x_1st2] * (4.0 - x_1stw))) / 4.0;
   DEBUG_CALC   yLOG_complex ("x_1st"     , "%10.5lf", q1);
   DEBUG_CALC   yLOG_complex ("x_3rd1"    , "i=%3d, v=%10.5lf, w=%4.2lf, f=%10.5lf", x_3rd1, entries [x_3rd1], x_3rdw      , entries [x_3rd1] * x_3rdw);
   DEBUG_CALC   yLOG_complex ("x_3rd2"    , "i=%3d, v=%10.5lf, w=%4.2lf, f=%10.5lf", x_3rd2, entries [x_3rd2], 4.0 - x_3rdw, entries [x_3rd2] * (4.0 - x_3rdw));
   q3 = ((entries [x_3rd1] * x_3rdw) + (entries [x_3rd2] * (4.0 - x_3rdw))) / 4.0;
   DEBUG_CALC   yLOG_complex ("x_3rd"     , "%10.5lf", q3);
   qr = q3 - q1;
   /*---(complete)-----------------------*/
   return;
}

PRIV void    /*--> find the interquartile range ----------[ ------ [ ------ ]-*/
CALC__quarter1     (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__quarters ();
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   CALC_pushval ( q1 );
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find the interquartile range ----------[ ------ [ ------ ]-*/
CALC__median       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__quarters ();
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   CALC_pushval ( q2 );
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find the interquartile range ----------[ ------ [ ------ ]-*/
CALC__quarter3     (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__quarters ();
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   CALC_pushval ( q3 );
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find the interquartile range ----------[ ------ [ ------ ]-*/
CALC__rangeq       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__quarters ();
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   CALC_pushval ( qr );
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find the most common value ------------[ ------ [ ------ ]-*/
CALC__mode         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =  0;            /* loop iterator -- entry         */
   int         mid         =  0;            /* midpoint of entries            */
   int         x_mode      =  0;
   int         x_highest   =  0;
   int         x_count     =  0;
   int         x_curr      =  0;
   int         x_save      =  0;
   /*---(start)--------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
   CALC__rangesort ();
   if      (cnt == 0)  {
      DEBUG_CALC   yLOG_value   ("pushing"   , 0);
      CALC_pushval ( 0);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   x_save = x_curr;
   for (i = 0; i < cnt; ++i) {
      x_curr = (int) entries [i];
      DEBUG_CALC   yLOG_complex ("entry"     , "i=%3d, x_curr=%3d, x_save=%3d", i, x_curr, x_save);
      if (x_curr != x_save)  x_count = 0;
      ++x_count;
      DEBUG_CALC   yLOG_value   ("count"     , x_count);
      if (x_count > x_highest) {
         x_mode    = x_curr;
         x_highest = x_count;
         DEBUG_CALC   yLOG_complex ("new_mode"  , "i=%3d, mode=%3d, highest=%3d", i, x_mode, x_highest);
      }
      x_save = x_curr;
   }
   CALC_pushval ((double) x_mode);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find the measure of variance ----------[ ------ [ ------ ]-*/
CALC__stddev       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =  0;            /* loop iterator -- entry         */
   double      x_mean      =  0;
   double      x_curr      =  0;
   double      x_diff      =  0;
   double      x_var       =  0;
   double      x_total     =  0;
   double      x_stddev    =  0;
   /*---(start)--------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   CALC__gather ();
   /*---(defense: bad range)-------------*/
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
   /*---(defense: too few values)--------*/
   if      (cnt <= 1)  {
      DEBUG_CALC   yLOG_value   ("pushing"   , 0);
      CALC_pushval ( 0);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   /*---(calc mean)----------------------*/
   x_mean = tot / ((double) cnt);
   /*---(gather variances)---------------*/
   for (i = 0; i < cnt; ++i) {
      x_curr   = entries [i];
      x_diff   = x_curr - x_mean;
      x_var    = x_diff * x_diff;
      x_total += x_var;
      DEBUG_CALC   yLOG_complex ("entry"     , "i=%3d, x_curr=%5.2lf, x_diff=%5.2lf, x_var=%10.2lf, x_total=%10.2lf", i, x_curr, x_diff, x_var, x_total);
   }
   /*---(final calc)---------------------*/
   DEBUG_CALC   yLOG_llong   ("variance"  , x_total / ((double) cnt));
   x_stddev   = sqrt(x_total / ((double) cnt));
   DEBUG_CALC   yLOG_llong   ("stddev"    , x_stddev);
   CALC_pushval (x_stddev);
   /*---(complete)-----------------------*/
   x_mean = tot / ((double) cnt);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}



/*====================------------------------------------====================*/
/*===----                          spreedsheet                         ----===*/
/*====================------------------------------------====================*/
PR void  o___SPREADSHEET_____o () { return; }

PRIV void        /* PURPOSE : search left column in range for string ------*/
CALC__clookup      (void)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_beg       = NULL;
   int         x_btab      = 0;
   int         x_bcol      = 0;
   int         x_brow      = 0;
   tCELL      *x_end       = NULL;
   int         x_etab      = 0;
   int         x_ecol      = 0;
   int         x_erow      = 0;
   int         x_crow      = 0;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   /*---(get values)---------------------*/
   n = CALC__popval ();
   r = CALC__popstr ();
   if (r == NULL)  r = strndup (nada, MAX_STR);
   x_end = CALC__popref ();
   DEBUG_CALC   yLOG_point   ("x_end"     , x_end);
   x_beg = CALC__popref ();
   DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
   /*---(defense)------------------------*/
   if (x_beg == NULL)     { CALC__seterror ( -1, "#.range");  return; }
   rc = LOC_coordinates (x_beg, &x_btab, &x_bcol, &x_brow);
   DEBUG_CALC   yLOG_value   ("x_btab"    , x_btab);
   DEBUG_CALC   yLOG_value   ("x_bcol"    , x_bcol);
   DEBUG_CALC   yLOG_value   ("x_brow"    , x_brow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )     { CALC__seterror ( -1, "#.range");    return; }
   if (x_end == NULL)     { CALC__seterror ( -1, "#.range");    return; }
   rc = LOC_coordinates (x_end, &x_etab  , &x_ecol, &x_erow);
   DEBUG_CALC   yLOG_value   ("x_etab"    , x_etab);
   DEBUG_CALC   yLOG_value   ("x_ecol"    , x_ecol);
   DEBUG_CALC   yLOG_value   ("x_erow"    , x_erow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   if (rc    <  0   )     { CALC__seterror ( -1, "#.range");    return; }
   if (x_btab != x_etab)  { CALC__seterror ( -1, "#.range");    return; }
   /*---(process)------------------------*/
   for (x_crow = x_brow; x_crow <= x_erow; ++x_crow) {
      DEBUG_CALC   yLOG_value   ("x_crow"    , x_crow);
      x_curr = tabs[x_btab].sheet[x_bcol][x_crow];
      DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr == NULL)                                       continue;
      DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
      if (strchr ("s", x_curr->t) == 0   )                      continue;
      if (x_curr->s == NULL)                                    continue;
      if (x_curr->s [0] != r [0])                               continue;
      if (strcmp (x_curr->s, r) != 0)                           continue;
      /*> CALC_pushref (LOC_cell (x_btab, x_bcol, x_crow));                           <*/
      if (x_bcol + n >  x_ecol)              { CALC_pushval (0); return; }
      x_curr = tabs[x_btab].sheet[x_bcol + n][x_crow];
      if (x_curr == NULL)                    { CALC_pushval (0); return; }
      if (x_curr->s == NULL)                 { CALC_pushval (0); return; }
      if (x_curr->t == 'n' || x_curr->t == 'f') CALC_pushval (x_curr->v_num);
      else                                      CALC_pushstr (x_curr->s);
      return;
   }
   /*---(nothing found)------------------*/
   CALC_pushval (0);
   /*> CALC__seterror ( -1, "#.nope");                                                <*/
   /*---(complete)-----------------------*/
   return;
}

PR void        /* PURPOSE : total the numeric cells in a range ----------*/
CALC__vlookup       (void)
{
   int xrow, xcol;
   tCELL  *curr   = NULL;
   int OFF = ((int) CALC__popval()) - 1;
   int R2  =  (int) CALC__popval();
   int C2  =  (int) CALC__popval();
   int T2  =  (int) CALC__popval();
   int R1  =  (int) CALC__popval();
   int C1  =  (int) CALC__popval();
   int T1  =  (int) CALC__popval();
   r        =  CALC__popstr();
   /*---(defenses)-----------------------*/
   if (OFF      <  0)  { CALC__seterror ( -1, "#.off_s");  return; }
   if (C1 + OFF > C2)  { CALC__seterror ( -1, "#.off_l");  return; }
   /*> printf ("_CALC__vlookup searching for <<%s>>\n", r);                           <*/
   s =  "";
   int xfound = 0;
   for (xrow = R1; xrow <= R2; ++xrow) {
      /*> printf ("_CALC__vlookup testing row   %d\n", xrow);                         <*/
      if (tabs[T1].sheet[C1][xrow]    == NULL)         continue;
      if (tabs[T1].sheet[C1][xrow]->s == NULL)         continue;
      /*> printf ("_CALC__vlookup comparing to  <<%s>>\n", tabs[T1].sheet[C1][xrow]->s);   <*/
      if (strcmp(tabs[T1].sheet[C1][xrow]->s, r) != 0) continue;
      curr = tabs[T1].sheet[C1 + OFF][xrow];
      if (curr    == NULL) { CALC__seterror (  1, "#.dest");   return; }
      if (curr->s == NULL) { CALC__seterror (  1, "#.sourc");  return; }
      /*> printf ("_CALC__vlookup offset yields <<%s>>\n", curr->s);                  <*/
      xfound = 1;
      if (curr->t == 'n' || curr->t == 'f') CALC_pushval (curr->v_num);
      else                                  CALC_pushstr (curr->s);
      break;
   }
   if (xfound == 0) CALC__seterror (  1, "#.null");
   /*> printf ("_CALC__vlookup done\n");                                              <*/
   return;
}



/*====================------------------------------------====================*/
/*===----                         execution stack                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___DRIVERS_________o () { return; }

#define     MAX_FUNCS       1000
struct  cFUNCS {
   char        n           [20];       /* operator symbol/name                */
   char        l;                      /* length of name                      */
   void   (*f) (void);                 /* function pointer                    */
   char        h           [20];       /* descriptive label                   */
} funcs [MAX_FUNCS] = {
   /*---(arithmetic operators)------------*/
   { "+"          ,  0, CALC__add               , "-----"                    },
   { "-"          ,  0, CALC__subtract          , "-----"                    },
   { "*"          ,  0, CALC__multiply          , "-----"                    },
   { "/"          ,  0, CALC__divide            , "-----"                    },
   { "%"          ,  0, CALC__modulus           , "-----"                    },
   { "++"         ,  0, CALC__increment         , "-----"                    },
   { "--"         ,  0, CALC__decrement         , "-----"                    },
   /*---(relational operators)------------*/
   { "=="         ,  0, CALC__equal             , "-----"                    },
   { "!="         ,  0, CALC__notequal          , "-----"                    },
   { ">"          ,  0, CALC__greater           , "-----"                    },
   { "<"          ,  0, CALC__lesser            , "-----"                    },
   { ">="         ,  0, CALC__gequal            , "-----"                    },
   { "<="         ,  0, CALC__lequal            , "-----"                    },
   /*---(logical operators)---------------*/
   { "!"          ,  0, CALC__not               , "-----"                    },
   { "&&"         ,  0, CALC__and               , "-----"                    },
   { "||"         ,  0, CALC__or                , "-----"                    },
   /*---(string operators)----------------*/
   { "#"          ,  0, CALC__concat            , "-----"                    },
   { "##"         ,  0, CALC__concatplus        , "-----"                    },
   /*---(string functions)----------------*/
   { "lower"      ,  0, CALC__lower             , "-----"                    },
   { "low"        ,  0, CALC__lower             , "-----"                    },
   { "upper"      ,  0, CALC__upper             , "-----"                    },
   { "up"         ,  0, CALC__upper             , "-----"                    },
   { "len"        ,  0, CALC__len               , "-----"                    },
   { "char"       ,  0, CALC__char              , "-----"                    },
   { "chr"        ,  0, CALC__char              , "-----"                    },
   { "code"       ,  0, CALC__code              , "-----"                    },
   { "left"       ,  0, CALC__left              , "-----"                    },
   { "right"      ,  0, CALC__right             , "-----"                    },
   { "mid"        ,  0, CALC__mid               , "-----"                    },
   { "trim"       ,  0, CALC__trim              , "-----"                    },
   { "ltrim"      ,  0, CALC__ltrim             , "-----"                    },
   { "rtrim"      ,  0, CALC__rtrim             , "-----"                    },
   { "strim"      ,  0, CALC__strim             , "-----"                    },
   { "etrim"      ,  0, CALC__etrim             , "-----"                    },
   { "mtrim"      ,  0, CALC__mtrim             , "-----"                    },
   { "print"      ,  0, CALC__print             , "-----"                    },
   { "p"          ,  0, CALC__print             , "-----"                    },
   { "lpad"       ,  0, CALC__lpad              , "-----"                    },
   { "rpad"       ,  0, CALC__rpad              , "-----"                    },
   { "lppad"      ,  0, CALC__lppad             , "-----"                    },
   { "rppad"      ,  0, CALC__rppad             , "-----"                    },
   { "value"      ,  0, CALC__value             , "-----"                    },
   { "salpha"     ,  0, CALC__salpha            , "-----"                    },
   { "salphac"    ,  0, CALC__salphac           , "-----"                    },
   { "salnum"     ,  0, CALC__salnum            , "-----"                    },
   { "salnumc"    ,  0, CALC__salnumc           , "-----"                    },
   { "sbasic"     ,  0, CALC__sbasic            , "-----"                    },
   { "sbasicc"    ,  0, CALC__sbasicc           , "-----"                    },
   { "swrite"     ,  0, CALC__swrite            , "-----"                    },
   { "swritec"    ,  0, CALC__swritec           , "-----"                    },
   { "sexten"     ,  0, CALC__sexten            , "-----"                    },
   { "sextenc"    ,  0, CALC__sextenc           , "-----"                    },
   { "sprint"     ,  0, CALC__sprint            , "-----"                    },
   { "sprintc"    ,  0, CALC__sprintc           , "-----"                    },
   { "sseven"     ,  0, CALC__sseven            , "-----"                    },
   { "ssevenc"    ,  0, CALC__ssevenc           , "-----"                    },
   { "replace"    ,  0, CALC__replace           , "-----"                    },
   /*---(math functions)------------------*/
   { "exp"        ,  0, CALC__power             , "-----"                    },
   { "abs"        ,  0, CALC__abs               , "-----"                    },
   { "trunc"      ,  0, CALC__trunc             , "-----"                    },
   { "rtrunc"     ,  0, CALC__rtrunc            , "-----"                    },
   { "round"      ,  0, CALC__round             , "-----"                    },
   { "rround"     ,  0, CALC__rround            , "-----"                    },
   { "ceil"       ,  0, CALC__ceiling           , "-----"                    },
   { "floor"      ,  0, CALC__floor             , "-----"                    },
   { "sqrt"       ,  0, CALC__sqrt              , "-----"                    },
   { "cbrt"       ,  0, CALC__cbrt              , "-----"                    },
   { "sqr"        ,  0, CALC__sqr               , "-----"                    },
   { "cube"       ,  0, CALC__cube              , "-----"                    },
   { "rand"       ,  0, CALC__rand              , "-----"                    },
   { "randr"      ,  0, CALC__randr             , "-----"                    },
   /*---(trig functions)------------------*/
   { "radians"    ,  0, CALC__radians           , "-----"                    },
   { "rad"        ,  0, CALC__radians           , "-----"                    },
   { "degrees"    ,  0, CALC__degrees           , "-----"                    },
   { "deg"        ,  0, CALC__degrees           , "-----"                    },
   { "pi"         ,  0, CALC__pi                , "-----"                    },
   { "hypotenuse" ,  0, CALC__hypot             , "-----"                    },
   { "hypot"      ,  0, CALC__hypot             , "-----"                    },
   { "hyp"        ,  0, CALC__hypot             , "-----"                    },
   { "side"       ,  0, CALC__side              , "-----"                    },
   { "sid"        ,  0, CALC__side              , "-----"                    },
   { "sine"       ,  0, CALC__sin               , "-----"                    },
   { "sin"        ,  0, CALC__sin               , "-----"                    },
   { "sinr"       ,  0, CALC__sinr              , "-----"                    },
   { "cosecant"   ,  0, CALC__csc               , "-----"                    },
   { "csc"        ,  0, CALC__csc               , "-----"                    },
   { "cscr"       ,  0, CALC__cscr              , "-----"                    },
   { "cosine"     ,  0, CALC__cos               , "-----"                    },
   { "cos"        ,  0, CALC__cos               , "-----"                    },
   { "cosr"       ,  0, CALC__cosr              , "-----"                    },
   { "secant"     ,  0, CALC__sec               , "-----"                    },
   { "sec"        ,  0, CALC__sec               , "-----"                    },
   { "secr"       ,  0, CALC__secr              , "-----"                    },
   { "tangent"    ,  0, CALC__tan               , "-----"                    },
   { "tan"        ,  0, CALC__tan               , "-----"                    },
   { "tanr"       ,  0, CALC__tanr              , "-----"                    },
   { "cotangent"  ,  0, CALC__cot               , "-----"                    },
   { "cot"        ,  0, CALC__cot               , "-----"                    },
   { "cotr"       ,  0, CALC__cotr              , "-----"                    },
   { "chord"      ,  0, CALC__crd               , "-----"                    },
   { "crd"        ,  0, CALC__crd               , "-----"                    },
   { "crdr"       ,  0, CALC__crdr              , "-----"                    },
   { "asin"       ,  0, CALC__asin              , "-----"                    },
   { "asinr"      ,  0, CALC__asinr             , "-----"                    },
   { "acos"       ,  0, CALC__acos              , "-----"                    },
   { "acosr"      ,  0, CALC__acosr             , "-----"                    },
   { "atan"       ,  0, CALC__atan              , "-----"                    },
   { "atanr"      ,  0, CALC__atanr             , "-----"                    },
   { "atan2"      ,  0, CALC__atan2             , "-----"                    },
   { "atanr2"     ,  0, CALC__atanr2            , "-----"                    },
   /*---(address functions)---------------*/
   { "offs"       ,  0, CALC__offs              , "-----"                    },
   { "offt"       ,  0, CALC__offt              , "-----"                    },
   { "offc"       ,  0, CALC__offc              , "-----"                    },
   { "offr"       ,  0, CALC__offr              , "-----"                    },
   { "loc"        ,  0, CALC__loc               , "-----"                    },
   { "isnum"      ,  0, CALC__isstr             , "-----"                    },
   { "isstr"      ,  0, CALC__isnum             , "-----"                    },
   { "me"         ,  0, CALC__me                , "-----"                    },
   { "addr"       ,  0, CALC__addr              , "-----"                    },
   { "filename"   ,  0, CALC__filename          , "-----"                    },
   { "filebase"   ,  0, CALC__filebase          , "-----"                    },
   { "tabname"    ,  0, CALC__tabname           , "-----"                    },
   { "tab"        ,  0, CALC__tab               , "-----"                    },
   { "col"        ,  0, CALC__col               , "-----"                    },
   { "row"        ,  0, CALC__row               , "-----"                    },
   { "tabs"       ,  0, CALC__tabs              , "-----"                    },
   { "cols"       ,  0, CALC__cols              , "-----"                    },
   { "rows"       ,  0, CALC__rows              , "-----"                    },
   /*---(look for logic functions)--------*/
   { "if"         ,  0, CALC__if                , "-----"                    },
   /*---(time functions)------------------*/
   { "today"      ,  0, CALC__now               , "-----"                    },
   { "now"        ,  0, CALC__now               , "-----"                    },
   { "year"       ,  0, CALC__year              , "-----"                    },
   { "month"      ,  0, CALC__month             , "-----"                    },
   { "day"        ,  0, CALC__day               , "-----"                    },
   { "hour"       ,  0, CALC__hour              , "-----"                    },
   { "minute"     ,  0, CALC__minute            , "-----"                    },
   { "second"     ,  0, CALC__second            , "-----"                    },
   { "weekday"    ,  0, CALC__weekday           , "-----"                    },
   { "weeknum"    ,  0, CALC__weeknum           , "-----"                    },
   { "datevalue"  ,  0, CALC__timevalue         , "timevalue"                },
   { "tv"         ,  0, CALC__timevalue         , "timevalue"                },
   /*---(range functions)-----------------*/
   { "sum"        ,  0, CALC__sum                , "sum"                      },
   { "su"         ,  0, CALC__sum                , "sum"                      },
   { "count"      ,  0, CALC__count              , "count"                    },
   { "co"         ,  0, CALC__count              , "count"                    },
   { "countn"     ,  0, CALC__count              , "countn"                   },
   { "counts"     ,  0, CALC__counts             , "counts"                   },
   { "counta"     ,  0, CALC__counta             , "counta"                   },
   { "countb"     ,  0, CALC__countb             , "countb"                   },
   { "countr"     ,  0, CALC__countr             , "reqs"                     },
   { "reqs"       ,  0, CALC__countr             , "reqs"                     },
   { "average"    ,  0, CALC__average            , "mean"                     },
   { "avg"        ,  0, CALC__average            , "mean"                     },
   { "mean"       ,  0, CALC__average            , "mean"                     },
   { "qtr0"       ,  0, CALC__minimum            , "0q/min"                   },
   { "qtr1"       ,  0, CALC__quarter1           , "1q"                       },
   { "qtr2"       ,  0, CALC__average            , "2q/med"                   },
   { "qtr3"       ,  0, CALC__quarter3           , "3q"                       },
   { "qtr4"       ,  0, CALC__maximum            , "4q/max"                   },
   { "min"        ,  0, CALC__minimum            , "min"                      },
   { "max"        ,  0, CALC__maximum            , "max"                      },
   { "range"      ,  0, CALC__range              , "range"                    },
   { "rangeq"     ,  0, CALC__rangeq             , "qrange"                   },
   { "median"     ,  0, CALC__median             , "median"                   },
   { "mode"       ,  0, CALC__mode               , "mode"                     },
   { "stddev"     ,  0, CALC__stddev             , "stddev"                   },
   { "sd"         ,  0, CALC__stddev             , "stddev"                   },
   /*---(lookup functions)----------------*/
   { "vlookup"    ,  0, CALC__vlookup           , "vlookup"                  },
   { "vl"         ,  0, CALC__vlookup           , "vlookup"                  },
   { "clookup"    ,  0, CALC__clookup           , "vlookup"                  },
   /*---(end-of-funcs)--------------------*/
   { "END"        ,  0, NULL                     , "-----"                    },
};

char         /*--> initialize calculation capability -----[ ------ [ ------ ]-*/
CALC_init          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i;
   double      a;
   /*---(randomizer)---------------------*/
   srand(time(NULL));
   /*---(load trig table)----------------*/
   for (i = 0; i < 3600; ++i) {
      a = ((double) i) / 10.0;
      trig[i].si = sin ((double) a * DEG2RAD);
      trig[i].co = cos ((double) a * DEG2RAD);
      trig[i].ta = tan ((double) a * DEG2RAD);
      if ( i % 900 == 0)  trig[i].ta = 0.0;
   }
   /*---(finish function table)----------*/
   for (i = 0; i < MAX_FUNCS; ++i) {
      if (funcs [i].f == NULL)              break;
      if (strcmp (funcs[i].n, "END") == 0)  break;
      funcs [i].l  = strlen (funcs [i].n);
   }
   /*---(error reporting)----------------*/
   errornum = 0;
   strcpy(errorstr, "");
   /*---(complete)-----------------------*/
   return 0;
}

tCALC*       /*--> create a new calculation entry --------[ ------ [ ------ ]-*/
CALC_new           (tCELL *a_curr, tCALC  *a_tail)
{
   /*---(locals)-----------+-----------+-*/
   tCALC      *new         = NULL;          /* current calculation element    */
   char        tries       = 0;
   /*---(allocate)-----------------------*/
   while (new == NULL && tries < 10)  {
      ++tries;
      new = (tCALC *) malloc (sizeof (tCALC));
   }
   if (new == NULL)   return  NULL;
   /*---(set to defaults)----------------*/
   new->t     = '-';
   new->v     = 0.0;
   new->s     = NULL;
   new->r     = NULL;
   new->f     = NULL;
   /*---(tie into linked list)-----------*/
   new->owner = a_curr;
   new->next  = NULL;
   if (a_curr->calc ==  NULL) {
      a_curr->calc  = new;
      new->prev     = NULL;
   } else {
      a_tail->next  = new;
      new->prev     = a_tail;
   }
   /*---(complete)-----------------------*/
   return new;
}

char               /* PURPOSE : clear an existing calculation ----------------*/
CALC_free          (tCELL *a_cell)
{
   tCALC   *curr = a_cell->calc;
   if (curr == NULL) return 0;
   tCALC   *next = curr->next;
   while (curr != NULL) {
      next = curr->next;
      if (curr->s  != NULL)  free (curr->s);
      free (curr);
      curr = next;
   }
   a_cell->calc = NULL;
   return 0;
}

char               /* PURPOSE : evaluate a calculation -----------------------*/
CALC_eval          (tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   int         neval       = 0;
   double      result      = 0;
   char       *resstr      = NULL;
   tCALC      *curr        = NULL;
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (a_curr       == NULL)  return rce;
   --rce;  if (strchr ("fpml", a_curr->t) == 0)  return rce;  /* not a calculation        */
   --rce;  if (a_curr->calc == NULL)  return rce;
   /*---(beginning)----------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_CALC   yLOG_complex ("calc"     , "cell=%9p, col=%3d, row=%3d, calc=%9p", a_curr, a_curr->col, a_curr->row, a_curr->calc);
   /*---(prep)---------------------------*/
   errornum = 0;
   strcpy(errorstr, "");
   s_me = a_curr;
   /*---(main loop)----------------------*/
   DEBUG_CALC   yLOG_info    ("rpn"       , a_curr->rpn);
   curr        = a_curr->calc;
   while (curr != NULL) {
      ++neval;
      DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", curr->t, curr->v, curr->s, curr->r, curr->f);
      switch (curr->t) {
      case 'v' : CALC_pushval (curr->v);    break;
      case 's' : CALC_pushstr (curr->s);    break;
      case 'r' : CALC_pushref (curr->r);    break;
      case 'f' : curr->f();                 break;
      case 'x' : break;
      default  : CALC__seterror (  -1, "#badcalc");
      }
      if (errornum != 0) break;
      curr = curr->next;
   }
   /*---(check errors)-------------------*/
   if        (errornum <  0) {
      a_curr->t = 'E';
      a_curr->v_str = strndup (errorstr, MAX_STR);
      DEBUG_CALC   yLOG_value   ("errornum"  , errornum);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   } else if (errornum >  0) {
      a_curr->t = 'E';
      a_curr->v_str = strndup (errorstr, MAX_STR);
      DEBUG_CALC   yLOG_value   ("errornum"  , errornum);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(results)------------------------*/
   if (a_curr->rpn[0] == '#') {
      resstr        = CALC__popstr();
      a_curr->v_str = strndup (resstr, MAX_STR);
   } else {
      result       = CALC__popval();
      a_curr->v_num = result;
   }
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle ranges in rpn ------------------[ ------ [ ------ ]-*/
CALC_range         (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell owning the calculation                  */
      tCALC      *a_calc)     /* current token                                */
{  /*---(design notes)--------------------------------------------------------*/
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_beg       = NULL;
   int         x_tab       = 0;
   int         x_bcol      = 0;
   int         x_brow      = 0;
   tCELL      *x_end       = NULL;
   int         x_ecol      = 0;
   int         x_erow      = 0;
   int         x_crow      = 0;
   int         x_ccol      = 0;
   char        rc          = 0;
   tCALC      *x_curr      = NULL;
   char        rce         = -10;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   /*---(get values)---------------------*/
   --rce;  if (a_cell == NULL)     return rce;
   DEBUG_CALC   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_calc == NULL)     return rce;
   DEBUG_CALC   yLOG_point   ("a_calc"    , a_calc);
   x_curr = a_calc->prev;
   DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr    == NULL)  return rce;
   DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
   --rce;  if (x_curr->t != 'r' )  return rce;
   x_end  = x_curr->r;
   DEBUG_CALC   yLOG_point   ("x_end"     , x_end);
   --rce;  if (x_end     == NULL)  return rce;
   rc = LOC_coordinates (x_end, &x_tab, &x_ecol, &x_erow);
   DEBUG_CALC   yLOG_value   ("x_tab"     , x_tab);
   DEBUG_CALC   yLOG_value   ("x_ecol"    , x_ecol);
   DEBUG_CALC   yLOG_value   ("x_erow"    , x_erow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   -rce;   if (rc    <  0   )      return rce;
   x_curr = x_curr->prev;
   DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr    == NULL)  return rce;
   DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
   --rce;  if (x_curr->t != 'r' )  return rce;
   x_beg  = x_curr->r;
   DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
   --rce;  if (x_beg     == NULL)  return rce;
   rc = LOC_coordinates (x_beg, NULL  , &x_bcol, &x_brow);
   DEBUG_CALC   yLOG_value   ("x_bcol"    , x_bcol);
   DEBUG_CALC   yLOG_value   ("x_brow"    , x_brow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   -rce;   if (rc    <  0   )      return rce;
   /*---(set dependencies)---------------*/
   rc = DEP_range (a_cell, x_tab, x_bcol, x_brow, x_tab, x_ecol, x_erow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   -rce;   if (rc    <  0   )      return rce;
   /*---(complete)-------------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*- verify a well formed pointer ------------[ ------ [ ------ ]-*/
CALC_checkpointer  (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell)     /* cell to interpret                            */
{  /*---(design notes)--------------------------------------------------------*/
   tCALC      *x_calc      = NULL;
   char        rce         = -10;
   /*---(defense: empty)-----------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_CALC   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell       == NULL) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: formatting)------------*/
   DEBUG_CALC   yLOG_info    ("source"    , a_cell->s);
   DEBUG_CALC   yLOG_char    ("source[0]" , a_cell->s[0]);
   --rce;  if (a_cell->s[0] != '&' ) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_CALC   yLOG_point   ("calc"      , a_cell->calc);
   --rce;  if (a_cell->calc == NULL) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check wrong size)---------------*/
   DEBUG_CALC   yLOG_value   ("nrpn"      , a_cell->nrpn);
   --rce;  if (a_cell->nrpn == 0   ) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (a_cell->nrpn == 2   ) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (a_cell->nrpn >  3   ) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check address pointer)----------*/
   x_calc = a_cell->calc;
   DEBUG_CALC   yLOG_point   ("1st calc"  , x_calc);
   DEBUG_CALC   yLOG_char    ("type"      , x_calc->t);
   --rce;  if (x_calc->t    != 'r' )    return rce;
   if (a_cell->nrpn == 1) {
      a_cell->t = 'a';
      a_cell->a = '<';
      DEBUG_CALC   yLOG_note    ("mark as an address pointer");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   /*---(check range pointer)------------*/
   else if (a_cell->nrpn == 3) {
      x_calc = x_calc->next;
      DEBUG_CALC   yLOG_point   ("2nd calc"  , x_calc);
      DEBUG_CALC   yLOG_char    ("type"      , x_calc->t);
      --rce;  if (x_calc->t    != 'r' )  {
         DEBUG_CALC   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      x_calc = x_calc->next;
      DEBUG_CALC   yLOG_point   ("3rd calc"  , x_calc);
      DEBUG_CALC   yLOG_char    ("type"      , x_calc->t);
      --rce;  if (x_calc->t    != 'x' ) {
         DEBUG_CALC   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      a_cell->t = 'p';
      a_cell->a = '<';
      DEBUG_CALC   yLOG_note    ("mark as an range pointer");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 3;
   }
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_note    ("failed as a pointer");
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return  --rce;
}

char         /*--> interpret a pointer -------------------[ ------ [ ------ ]-*/
CALC_pointer       (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell owning the calculation                  */
      tCALC      *a_calc)     /* current token                                */
{  /*---(design notes)--------------------------------------------------------*/
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_ref       = NULL;
   char        rc          = 0;
   tCALC      *x_curr      = NULL;
   char        rce         = -10;
   tCALC      *x_temp      = NULL;
   tCELL      *x_beg       = NULL;
   tCELL      *x_end       = NULL;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   /*---(get values)---------------------*/
   DEBUG_CALC   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell == NULL)     return rce;
   DEBUG_CALC   yLOG_point   ("a_calc"    , a_calc);
   --rce;  if (a_calc == NULL)     return rce;
   x_curr = a_calc->prev;
   DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
   --rce;  if (x_curr    == NULL)  return rce;
   /*---(look at referenced cell)--------*/
   DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
   --rce;  if (x_curr->t != 'r' )  return rce;
   x_ref = x_curr->r;
   DEBUG_CALC   yLOG_point   ("x_ref"     , x_ref);
   --rce;  if (x_ref     == NULL)  return rce;
   /*---(look at address pointer)--------*/
   if (x_ref->t  == 'a' )  {
      DEBUG_CALC   yLOG_value   ("nrpn"      , x_ref->nrpn);
      x_temp = x_ref->calc;
      DEBUG_CALC   yLOG_point   ("x_temp"    , x_temp);
      --rce;  if (x_temp    == NULL)  return rce;
      x_beg  = x_temp->r;
      DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
      --rce;  if (x_beg     == NULL)  return rce;
      DEBUG_CALC   yLOG_value   ("tab"       , x_beg->tab);
      DEBUG_CALC   yLOG_value   ("col"       , x_beg->col);
      DEBUG_CALC   yLOG_value   ("row"       , x_beg->row);
      /*---(pass address through)--------*/
      a_calc->t   = 'x';
      x_curr->r   = x_beg;
      x_curr->t   = 'r';
      DEBUG_CALC   yLOG_complex ("1st calc"  , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_curr->t, x_curr->v, x_curr->s, x_curr->r, x_curr->f);
      DEBUG_CALC   yLOG_complex ("2nd calc"  , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", a_calc->t, a_calc->v, a_calc->s, a_calc->r, a_calc->f);
      DEBUG_CALC   yLOG_note    ("integrated address pointer");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(look at range pointer)----------*/
   if (x_ref->t  == 'p' )  {
      DEBUG_CALC   yLOG_value   ("nrpn"      , x_ref->nrpn);
      x_temp = x_ref->calc;
      DEBUG_CALC   yLOG_point   ("x_temp"    , x_temp);
      --rce;  if (x_temp    == NULL)  return rce;
      x_beg  = x_temp->r;
      DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
      --rce;  if (x_beg     == NULL)  return rce;
      DEBUG_CALC   yLOG_value   ("tab"       , x_beg->tab);
      DEBUG_CALC   yLOG_value   ("col"       , x_beg->col);
      DEBUG_CALC   yLOG_value   ("row"       , x_beg->row);
      x_end  = x_temp->next->r;
      DEBUG_CALC   yLOG_point   ("x_end"     , x_end);
      --rce;  if (x_end     == NULL)  return rce;
      DEBUG_CALC   yLOG_value   ("tab"       , x_end->tab);
      DEBUG_CALC   yLOG_value   ("col"       , x_end->col);
      DEBUG_CALC   yLOG_value   ("row"       , x_end->row);
      /*---(pass range through)----------*/
      a_calc->r   = x_end;
      a_calc->t   = 'r';
      x_curr->r   = x_beg;
      x_curr->t   = 'r';
      DEBUG_CALC   yLOG_complex ("1st calc"  , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_curr->t, x_curr->v, x_curr->s, x_curr->r, x_curr->f);
      DEBUG_CALC   yLOG_complex ("2nd calc"  , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", a_calc->t, a_calc->v, a_calc->s, a_calc->r, a_calc->f);
      DEBUG_CALC   yLOG_note    ("integrated range pointer");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return --rce;
}

char         /*--> clear out the calculation -------------[ ------ [ ------ ]-*/
CALC_cleanse       (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell)     /* cell owning the calculation                  */
{  /*---(design notes)--------------------------------------------------------*/
   /* the calculation byte-code is cleared and dependencies elimenated.       */
   /*---(clear calc and deps)------------*//*---------------------------------*/
   if (a_cell->calc     != NULL)  CALC_free   (a_cell);
   a_cell->calc     = NULL;
   if (a_cell->requires != NULL)  DEP_cleanse (a_cell);
   a_cell->requires = NULL;
   /*---(complete)-----------------------*/
   return 0;
}


char*
CALC_strtok        (char *a_str)
{
   /*---(locals)-----------+-----------+-*/
   static char *x_str       = NULL;
   static int  x_pos       = 0;
   static int  x_next      = 0;
   static int  x_len       = 0;
   static char x_lit       = '-';
   int         i           = 0;
   /*---(defense)------------------------*/
   if (a_str == NULL && x_str == NULL) return NULL;
   /*---(start new string)---------------*/
   if (a_str != NULL) {
      /*> printf ("CALC_strtok : new string <<%s>>\n", a_str);                        <*/
      x_str   = a_str;
      x_pos   = 0;
      x_next  = 0;
      x_len   = strllen (a_str, MAX_STR);
      x_lit   = '-';
      /*> printf ("   -- a_str = %p\n", a_str);                                       <*/
      /*> printf ("   -- x_str = %p <<%s>>\n", x_str, x_str);                         <*/
   }
   /*---(or, continue)-------------------*/
   if (a_str == NULL) {
      x_pos  = x_next;
      /*> printf ("CALC_strtok : existing string <<%s>>\n", x_str + x_pos);           <*/
   }
   /*---(defense at end)-----------------*/
   if (x_pos >= x_len)                 return NULL;
   if (x_str + x_pos == '\0')          return NULL;
   /*---(search for delimiter)-----------*/
   for (i = x_pos; i < x_len; ++i) {
      /*> printf ("   -- looking at %3d %c : ", i, x_str[i]);                         <*/
      /*---(quoted strings)--------------*/
      if (x_str [i] == '"' )  {
         if (x_lit == 'y') {
            /*> printf ("exiting quote,  ");                                          <*/
            x_lit = '-';
         }
         else {
            /*> printf ("entering quote, ");                                          <*/
            x_lit = 'y';
         }
      }
      if (x_lit == 'y') {
         /*> printf ("next\n");                                                       <*/
         continue;
      }
      if (x_str [i] == ',') {
         x_str [i] = '\0';
         x_next = i + 1;
         /*> printf ("COMMA, x_next = %3d, return %p\n", x_str + x_pos);              <*/
         return x_str + x_pos;
      }
      /*> printf ("next\n");                                                          <*/
   }
   /*---(complete)-----------------------*/
   x_next = x_len;
   return x_str + x_pos;
}

char               /* PURPOSE : build a new calculation ----------------------*/
CALC_build         (tCELL *a_cell)
{
   /*---(locals)-----------+-----------+-*/
   char        work        [MAX_STR];       /* working copy of source string  */
   int         x_ntoken    = -1;
   char       *p;                           /* strtok current pointer         */
   char       *q           = ",";           /* strtok delimiter               */
   tCALC      *x_calc      = NULL;          /* current calculation element    */
   tCALC      *x_tail      = NULL;          /* last calculation element       */
   int         t1, t2, c1, c2, r1, r2;
   int         range       = 0;
   char        mode        = '-';
   int         x_tab       = a_cell->tab;
   int         x_col       = 0;
   int         x_row       = 0;
   tCELL      *dest        = NULL;
   char        rc          = 0;
   char        rce         = -10;
   char        msg         [1000];
   int         i           = 0;
   int         len         = 0;
   char       *valid       = "0123456789.-+";    /* only digits               */
   char        label       [20]        = "";
   /*---(defense: starting conditions)---*/
   --rce;  if (a_cell      == NULL)  return rce;  /* cell does not exist      */
   --rce;  if (strchr ("fpml", a_cell->t) == 0)  return rce;  /* not a calculation        */
   --rce;  if (a_cell->rpn == NULL)  return rce;  /* nothing without rpn      */
   /*---(beginning)----------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   /*---(clear calc and deps)------------*/
   CALC_cleanse (a_cell);
   /*---(initialize)-------------------------*/
   strncpy (work, a_cell->rpn + 2, MAX_STR);
   DEBUG_CALC   yLOG_info    ("source"    , work);
   if (a_cell->s [0] == '&')  strcpy (label, "");
   /*---(read first tokens)------------------*/
   --rce;
   /*> p = strtok (work, q);                                                          <*/
   p = CALC_strtok (work);
   ++x_ntoken;
   if (p == NULL) {
      a_cell->t = 'E';                 /* turn formula type off and error on  */
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;
   /*---(process tokens)---------------------*/
   while (p != NULL) {
      /*---(debug token header)--------------*/
      DEBUG_CALC   yLOG_info    ("token"     , p);
      /*---(create and null)-----------------*/
      DEBUG_CALC   yLOG_note    ("allocate calc entry");
      x_calc   = CALC_new (a_cell, x_tail);
      x_tail   = x_calc;
      /*---(literal string)------------------*/
      DEBUG_CALC   yLOG_note    ("check for literal string");
      if (p[0] == '"') {
         int   len = strlen(p);
         p[len - 1] = '\0';
         x_calc->t = 's';
         x_calc->s = strndup(p + 1, MAX_STR);
         /*---(read next)---------------------*/
         DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_calc->t, x_calc->v, x_calc->s, x_calc->r, x_calc->f);
         /*> p = strtok (NULL, q);                                                    <*/
         p = CALC_strtok (NULL);
         ++x_ntoken;
         continue;
      }
      /*---(range operators)-----------------*/
      DEBUG_CALC   yLOG_note    ("check for range operator");
      if ((strncmp(p, ":"          ,  2) == 0)
            || (strncmp(p, ";"          ,  2) == 0)
            || (strncmp(p, ".."         ,  2) == 0)) {
         DEBUG_CALC   yLOG_note    ("found a range");
         rc = CALC_range (a_cell, x_calc);
         if (rc < 0) {
            a_cell->t = 'E';
            a_cell->v_str = strndup ("#.badrng", MAX_STR);
            DEBUG_CALC   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         x_calc->t = 'x';
         /*---(read next)---------------------*/
         DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_calc->t, x_calc->v, x_calc->s, x_calc->r, x_calc->f);
         /*> p = strtok (NULL, q);                                                    <*/
         p = CALC_strtok (NULL);
         ++x_ntoken;
         continue;
      }
      /*---(pointer operator)----------------*/
      DEBUG_CALC   yLOG_note    ("check for pointer operator");
      if (strncmp(p, "*:"         ,  2) == 0) {
         DEBUG_CALC   yLOG_note    ("found a pointer");
         rc = CALC_pointer (a_cell, x_calc);
         if (rc < 0) {
            a_cell->t = 'E';
            a_cell->v_str = strndup ("#.badptr", MAX_STR);
            DEBUG_CALC   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         /*---(read next)---------------------*/
         DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_calc->t, x_calc->v, x_calc->s, x_calc->r, x_calc->f);
         /*> p = strtok (NULL, q);                                                    <*/
         p = CALC_strtok (NULL);
         ++x_ntoken;
         continue;
      }
      /*---(function table)------------------*/
      DEBUG_CALC   yLOG_note    ("check for function");
      len = strlen (p);
      for (i = 0; i < MAX_FUNCS; ++i) {
         /*> DEBUG_CALC   yLOG_complex ("check"     , "%3d, %-12.12s, %3d, %-10.10p   vs %-12.12s, %3d", i, funcs[i].n, funcs[i].l, funcs[i].f, p, len);   <*/
         if (funcs [i].f == NULL)              break;
         if (strcmp (funcs[i].n, "END") == 0)  break;
         if (len  != funcs [i].l  )            continue;
         if (p[0] != funcs[i].n[0])            continue;
         if (strcmp (funcs[i].n, p) != 0)      continue;
         DEBUG_CALC   yLOG_complex ("found it"  , "%3d, %-12.12s, %3d, %-10.10p   vs %-12.12s, %3d", i, funcs[i].n, funcs[i].l, funcs[i].f, p, len);
         x_calc->t = 'f';
         x_calc->f = funcs[i].f;
         if (label [0] == '\0')  strncpy (label, funcs[i].h, 19);
         /*---(read next)---------------------*/
         DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_calc->t, x_calc->v, x_calc->s, x_calc->r, x_calc->f);
         /*> p = strtok (NULL, q);                                                    <*/
         p = CALC_strtok (NULL);
         ++x_ntoken;
         break;
      }
      if (x_calc->t != '-')  continue;
      /*---(look for tab names)--------------*/
      DEBUG_CALC   yLOG_note    ("look for reference");
      rc = LOC_parse (p, &x_tab, &x_col, &x_row, NULL);
      DEBUG_CALC   yLOG_value   ("rc"        , rc);
      /*---(handle cell reference)--------*/
      if (rc == 0) {
         DEBUG_CALC   yLOG_note    ("check if legal");
         rc = LOC_legal (x_tab, x_col, x_row, CELL_FIXED);
         DEBUG_CALC   yLOG_value   ("rc"        , rc);
         if (rc == 0) {
            dest = LOC_cell (x_tab, x_col, x_row);
            DEBUG_CALC   yLOG_point   ("dest"      , dest);
            if (dest == NULL)  dest = CELL_change (CHG_NOHIST, x_tab, x_col, x_row, "");
            DEBUG_CALC   yLOG_point   ("dest"      , dest);
            if (dest == NULL)  {
               a_cell->t = 'E';
               a_cell->v_str = strndup ("#.badmal", MAX_STR);
               DEBUG_CALC   yLOG_exit    (__FUNCTION__);
               return rce - 1;
            }
            rc = DEP_create (DEP_REQUIRE, a_cell, dest);
            DEBUG_CALC   yLOG_value   ("rc"        , rc);
            if (rc < 0) {
               DEBUG_CALC   yLOG_info    ("error"     , "dependency can not be created");
               a_cell->t = 'E';
               a_cell->v_str = strndup ("#.baddep", MAX_STR);
               DEBUG_CALC   yLOG_exit    (__FUNCTION__);
               return rce - 2;
            }
            x_calc->t = 'r';
            x_calc->r = dest;
         } else {
            a_cell->t = 'E';
            a_cell->v_str = strndup ("#.badref", MAX_STR);
            DEBUG_CALC   yLOG_exit    (__FUNCTION__);
            return rce - 3;
         }
         /*---(read next)---------------------*/
         DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_calc->t, x_calc->v, x_calc->s, x_calc->r, x_calc->f);
         /*> p = strtok (NULL, q);                                                    <*/
         p = CALC_strtok (NULL);
         ++x_ntoken;
         continue;
      }
      if (rc > 0) {
         a_cell->t = 'E';
         a_cell->v_str = strndup ("#.bigref", MAX_STR);
         DEBUG_CALC   yLOG_exit    (__FUNCTION__);
         return rce - 4;
      }
      /*---(handle cell reference)--------*/
      x_calc->t = 'v';
      DEBUG_CALC   yLOG_note    ("check for numeric value");
      for (i = 0; i < len; ++i) {
         if (strchr (valid, p[i])  != 0) continue;
         DEBUG_CALC   yLOG_char    ("non-value" , p[i]);
         x_calc->t = '-';
      }
      if (x_calc->t == 'v') {
         x_calc->v = atof (p);
         /*---(read next)---------------------*/
         DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", x_calc->t, x_calc->v, x_calc->s, x_calc->r, x_calc->f);
         /*> p = strtok (NULL, q);                                                    <*/
         p = CALC_strtok (NULL);
         ++x_ntoken;
         continue;
      }
      /*---(fall through)-----------------*/
      a_cell->t = 'E';
      a_cell->v_str = strndup ("#.unknown", MAX_STR);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce - 4;
   }
   /*---(auto-labeling)--------------------*/
   /*> rc = CALC_checkpointer (a_cell);                                                  <* 
    *> if (rc == 1)   strcpy (label, "pointer");                                         <* 
    *> if (rc == 3)   strcpy (label, "dataset");                                         <* 
    *> if (label [0] == '\0' || label [0] == '-')  {                                     <* 
    *>    strcpy (label, a_cell->s);                                                     <* 
    *>    label [0] = ':';                                                               <* 
    *> }                                                                                 <* 
    *> if (label [0] != '-') {                                                           <* 
    *>    rc = LOC_legal (a_cell->tab, a_cell->col + 1, a_cell->row, CELL_FIXED);        <* 
    *>    if (rc == 0) {                                                                 <* 
    *>       dest = LOC_cell (a_cell->tab, a_cell->col + 1, a_cell->row);                <* 
    *>       if (dest == NULL || dest->t == 'l') {                                       <* 
    *>          dest = CELL_change (a_cell->tab, a_cell->col + 1, a_cell->row, label);   <* 
    *>          dest->t = 'l';                                                           <* 
    *>       }                                                                           <* 
    *>    }                                                                              <* 
    *> }                                                                                 <*/
   /*---(complete)-------------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*============================----end-of-source---============================*/
