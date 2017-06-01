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

#define     MAX_TERM     20
typedef     struct cTERMS   tTERMS;
struct cTERMS {
   char        type;
   char       *abbr;
   char       *desc;
};
tTERMS      s_terms [MAX_TERM] = {
   { 'v' , "val"    , "numeric or cell address"        },
   { 's' , "str"    , "string or cell address"         },
   { '?' , "run"    , "string or numeric (runtime)"    },
   { 'a' , "adr"    , "cell address or pointer"        },
   { 'r' , "rng"    , "range of cells or pointer"      },
   { 't' , "tof"    , "true or false"                  },
   { '-' , "---"    , "-----"                          },
};


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
PRIV   char    t[LEN_RECD];
#define  MAX   1000000000

PRIV   tCELL      *s_me;
static char        s_narg        = 0;
static int         s_nerror      = 0;
static int         s_nbuild      = 0;
static int         s_neval       = 0;
static int         errornum;
static char        errorstr      [LEN_RECD];


char      nada[5] = "";



/*====================------------------------------------====================*/
/*===----                        error setting                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ERROR___________o () { return; }

tERROR*      /*--> create a single error entry -----------[ leaf   [ ------ ]-*/
ERROR_create       (tCELL *a_owner)
{
   /*---(locals)-----------+-----------+-*/
   tERROR     *x_error     = NULL;
   int         x_tries     = 0;
   tERROR     *x_next      = NULL;
   /*---(defense)------------------------*/
   if (a_owner == NULL)    return NULL;
   /*---(create cell)--------------------*/
   while (x_error == NULL) {
      ++x_tries;
      x_error = (tERROR *) malloc (sizeof (tERROR));
      if (x_tries > 10)   return NULL;
   }
   /*---(into global list)---------------*/
   x_error->gnext    = NULL;
   x_error->gprev    = NULL;
   if (terror == NULL) {
      herror         = x_error;
      terror         = x_error;
   } else {
      x_error->gprev = terror;
      x_error->gnext = NULL;
      terror->gnext  = x_error;
      terror         = x_error;
   }
   ++nerror;
   /*---(assign owner)-------------------*/
   x_error->owner   = a_owner;
   /*---(into owner list)----------------*/
   x_error->next     = NULL;
   x_error->prev     = NULL;
   if (a_owner->nerror == 0) {
      a_owner->errors   = x_error;
   } else {
      x_next = a_owner->errors;
      while (x_next->next != NULL)  x_next = x_next->next;
      x_next->next      = x_error;
      x_error->prev     = x_next;
   }
   ++a_owner->nerror;
   /*---(complete)-----------------------*/
   return x_error;
}

char         /*--> destroy a single error entry ----------[ leaf   [ ------ ]-*/
ERROR_delete       (tERROR *a_error)
{
   /*---(defense: null cell)-------------*/
   if (a_error     == NULL)  return NULL;
   /*---(remove from global list)--------*/
   if (a_error->gnext != NULL) a_error->gnext->gprev   = a_error->gprev;
   else                        terror                  = a_error->gprev;
   if (a_error->gprev != NULL) a_error->gprev->gnext   = a_error->gnext;
   else                        herror                  = a_error->gnext;
   --nerror;
   /*---(if require, take off cell)------*/
   if (a_error->next  != NULL) a_error->next->prev     = a_error->prev;
   if (a_error->prev  != NULL) a_error->prev->next     = a_error->next;
   else                        a_error->owner->errors  = a_error->next;
   --(a_error->owner->nerror);
   /*---(free)---------------------------*/
   free (a_error);
   /*---(complete)-----------------------*/
   return  NULL;
}

char
ERROR_add          (tCELL *a_owner, char a_phase, int a_step, char *a_func, char a_type, char *a_desc)
{
   /*---(locals)-----------+-----------+-*/
   tERROR     *x_error     = NULL;
   /*---(create)-------------------------*/
   x_error = ERROR_create   (a_owner);
   if (x_error == NULL)  return  -1;
   /*---(save values)--------------------*/
   x_error->phase = a_phase;
   x_error->step  = a_step;
   strlcpy (x_error->func , a_func  ,  20);
   x_error->narg  = s_narg;
   x_error->type  = a_type;
   strlcpy (x_error->desc , a_desc  , 100);
   /*---(put in old container)-----------*/
   errornum = -1;
   strncpy (errorstr     , a_desc, LEN_RECD);
   /*---(complete)-----------------------*/
   return;
}

char
ERROR_cleanse      (tCELL *a_owner)
{
   /*---(locals)-----------+-----------+-*/
   tERROR     *x_error     = NULL;
   tERROR     *x_save      = NULL;
   char        rce         =  -10;
   char        rc          = 0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_owner == NULL) {
      return rce;
   }
   /*---(reqs)---------------------------*/
   x_error = a_owner->errors;
   while (x_error != NULL) {
      x_save = x_error->next;
      rc = ERROR_delete (x_error);
      x_error = x_save;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
ERROR_entry        (tCELL *a_cell, char a_seq, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [LEN_RECD];
   static tERROR *x_error  = NULL;
   static int     x_total  = 0;
   static int     x_count  = 0;
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) {
      strlcpy (a_list, g_empty, 80);
      return rce;
   }
   --rce;  if (a_cell  == NULL) {
      strlcpy (a_list, g_empty, 80);
      return rce;
   }
   /*---(check)-----------------------*/
   --rce;  if (a_cell->nerror <= 0) {
      strlcpy (a_list, g_empty, 80);
      return rce;
   }
   /*---(preparation)-----------------*/
   --rce;  if (a_seq == '*') {
      x_error = a_cell->errors;
      x_total = a_cell->nerror;
      x_count = 0;
   } else {
      if (x_error->next == NULL) {
         strlcpy (a_list, g_empty, 80);
         return rce;
      }
      x_error = x_error->next;
      ++x_count;
   }
   /*---(write line)------------------*/
   sprintf (x_line , " %-2d %c %-2d %-15.15s %c %-40.40s ",
         x_count       , x_error->phase, x_error->step ,
         x_error->func , x_error->type , x_error->desc );
   strlcpy (a_list, x_line, 80);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ERROR_list         (void)
{
   /*---(locals)-----------+-----------+-*/
   tERROR     *x_error     = NULL;
   FILE       *f           = NULL;
   int         c           = 0;
   char       *x_title     = "----- ---cell--- phase stp ---function--------- type- cnt ---description--------------------------\n";
   /*---(open)---------------------------*/
   f = fopen ("errors", "w");
   /*---(show errors)--------------------*/
   x_error = herror;
   while (x_error) {
      if (c % 45 == 0)  fprintf (f, x_title);
      if (c %  5 == 0)  fprintf (f, "\n");
      fprintf (f, "%-5d %-10.10s ", c, x_error->owner->label);
      switch (x_error->phase) {
      case PERR_RPN   : fprintf (f, "%-5.5s ", "rpn"  ); break;
      case PERR_BUILD : fprintf (f, "%-5.5s ", "build"); break;
      case PERR_EVAL  : fprintf (f, "%-5.5s ", "eval" ); break;
      case PERR_DISP  : fprintf (f, "%-5.5s ", "disp" ); break;
      }
      fprintf (f, "%-3d %-20.20s ", x_error->step, x_error->func);
      switch (x_error->type ) {
      case TERR_ARGS  : fprintf (f, "%-5.5s ", "args" ); break;
      case TERR_ADDR  : fprintf (f, "%-5.5s ", "addr" ); break;
      case TERR_RANGE : fprintf (f, "%-5.5s ", "range"); break;
      case TERR_FUNC  : fprintf (f, "%-5.5s ", "func" ); break;
      }
      if (x_error->narg == 0)  fprintf (f, "-   " );
      else                     fprintf (f, "%-3d ", x_error->narg);
      fprintf (f, "%-.100s\n", x_error->desc);
      ++c;
      x_error = x_error->gnext;
   }
   /*---(footer)-------------------------*/
   fprintf (f, "\n");
   fprintf (f, x_title);
   /*---(close)--------------------------*/
   fclose (f);
   /*---(complete)-----------------------*/
   return 0;
}



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
CALC_pushstr       (char *a_func, char *a_new)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* very simply adds a stack entry with a string literal as its value       */
   /*---(defense: stack overflow)--------*//*---------------------------------*/
   if (calc__nstack >= MAX_STACK) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_OTHER, "stack full, could not push string");
      return -1;
   }
   /*---(update stack item)--------------*/
   calc__stack[calc__nstack].typ = 's';
   calc__stack[calc__nstack].ref = NULL;
   calc__stack[calc__nstack].num = 0;
   calc__stack[calc__nstack].str = strndup (a_new, LEN_RECD);
   /*---(update stack counter)-----------*/
   ++calc__nstack;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> add a value to the stack --------------[ ------ [ ------ ]-*/
CALC_pushval       (char *a_func, double a_new)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* very simply adds a stack entry with a numeric literal as its value      */
   /*---(defense: stack overflow)--------*//*---------------------------------*/
   if (calc__nstack >= MAX_STACK) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_OTHER, "stack full, could not push value");
      return -1;
   }
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
CALC_pushref       (char *a_func, tCELL *a_new)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* adds a cell reference for later intepretation in the calculation        */
   /*---(defense: stack overflow)--------*//*---------------------------------*/
   if (calc__nstack >= MAX_STACK) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_OTHER, "stack full, could not push reference");
      return -1;
   }
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
CALC__popval          (char *a_func, char a_seq)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /* always returns a value for the stack entry.                             */
   /* -- for a numeric literal, it returns the number field on the stack item */
   /* -- for a string literal, it returns 0.0 (since strings have no value)   */
   /* -- for a reference, it returns the value contained in that cell         */
   /* -- if it can't figure it out, it returns a 0.0                          */
   /*---(prepare)------------------------*/
   if (calc__nstack <= 0) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "stack empty, could not get value");
      return 0.0;
   }
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'v' :
      return  calc__stack[calc__nstack].num;
      break;
   case 's' :
      return  0.0;
      break;
   case 'r' :
      return  calc__stack[calc__nstack].ref->v_num;
      break;
   }
   /*---(complete)-----------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "wrong argument type on stack");
   return 0.0;
}

char*        /*--> get an string off the stack -----------[ ------ [ ------ ]-*/
CALC__popstr       (char *a_func, char a_seq)
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
   if (calc__nstack <= 0) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "stack empty, could not get string");
      return strndup (nada, LEN_RECD);
   }
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'v' :  return  strndup (nada, LEN_RECD);                 break;
   case 's' :  return  calc__stack[calc__nstack].str;           break;
   case 'r' :  if        (calc__stack[calc__nstack].ref->t == '-') {
                  return  strndup (nada, LEN_RECD);
               } else if (calc__stack[calc__nstack].ref->v_str != NULL) {
                  return  strndup (calc__stack[calc__nstack].ref->v_str, LEN_RECD);
               } else if (calc__stack[calc__nstack].ref->s != NULL) {
                  return  strndup (calc__stack[calc__nstack].ref->s, LEN_RECD);
               } else {
                  return strndup (nada, LEN_RECD);
               }
   }
   /*---(complete)-----------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "wrong argument type on stack");
   return strndup (nada, LEN_RECD);
}

tCELL*       /*--> get a reference off the stack ---------[ ------ [ ------ ]-*/
CALC__popref          (char *a_func, char a_seq)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /*---(prepare)------------------------*/
   if (calc__nstack <= 0) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "stack empty, could not get reference");
      return NULL;
   }
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'r' :
      return  calc__stack[calc__nstack].ref;
      break;
   }
   /*---(complete)-----------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "wrong argument type on stack");
   return NULL;
}

tCELL*       /*--> get a reference off the stack ---------[ ------ [ ------ ]-*/
CALC__popprint        (char *a_func, char a_seq)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /*---(prepare)------------------------*/
   if (calc__nstack <= 0) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "stack empty, could not get printable");
      return NULL;
   }
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'r' :
      return  strndup (calc__stack[calc__nstack].ref->p, LEN_RECD);
      break;
   }
   /*---(complete)-----------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "wrong argument type on stack");
   return NULL;
}

tCELL*       /*--> get a reference off the stack ---------[ ------ [ ------ ]-*/
CALC__popsource       (char *a_func, char a_seq)
{  /*---(design notes)-------------------*//*---------------------------------*/
   /*---(prepare)------------------------*/
   if (calc__nstack <= 0) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "stack empty, could not get printable");
      return NULL;
   }
   --calc__nstack;
   /*---(handle stack types)-------------*/
   switch (calc__stack[calc__nstack].typ) {
   case 'r' :
      return  strndup (calc__stack[calc__nstack].ref->s, LEN_RECD);
      break;
   }
   /*---(complete)-----------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ARGS , "wrong argument type on stack");
   return NULL;
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b + a);
   return;
}

PRIV void
CALC__subtract     (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b - a);
   return;
}

PRIV void
CALC__multiply     (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b * a);
   return;
}

PRIV void
CALC__divide       (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   if (a != 0)  CALC_pushval (__FUNCTION__, b / a);
   else         CALC_pushval (__FUNCTION__, 0);
   return;
}

PRIV void
CALC__modulus      (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, ((int) b) % ((int) a));
   return;
}

PRIV void
CALC__increment    (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a + 1);
   return;
}

PRIV void
CALC__decrement    (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a - 1);
   return;
}

PRIV void
CALC__unaryminus   (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, 0 - a);
   return;
}



/*====================------------------------------------====================*/
/*===----                     relational operators                     ----===*/
/*====================------------------------------------====================*/
PRIV void  o___RELATIONAL______o () { return; }

PRIV void
CALC__equal        (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b == a);
   return;
}

PRIV void
CALC__notequal      (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b != a);
   return;
}

PRIV void
CALC__greater      (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b > a);
   return;
}

PRIV void
CALC__lesser       (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b < a);
   return;
}

PRIV void
CALC__gequal       (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b >= a);
   return;
}

PRIV void
CALC__lequal       (void)
{
   a = CALC__popval( __FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b <= a);
   return;
}

PRIV void
CALC__sequal       (void)
{
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   s = CALC__popstr (__FUNCTION__, ++s_narg);
   if (strcmp (s, r) == 0) CALC_pushval (__FUNCTION__, TRUE );
                           CALC_pushval (__FUNCTION__, FALSE);
   return;
}

PRIV void
CALC__snotequal    (void)
{
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   s = CALC__popstr (__FUNCTION__, ++s_narg);
   if (strcmp (s, r) != 0) CALC_pushval (__FUNCTION__, TRUE );
                           CALC_pushval (__FUNCTION__, FALSE);
   return;
}

PRIV void
CALC__slesser      (void)
{
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   s = CALC__popstr (__FUNCTION__, ++s_narg);
   if (strcmp (s, r) <  0) CALC_pushval (__FUNCTION__, TRUE );
                           CALC_pushval (__FUNCTION__, FALSE);
   return;
}

PRIV void
CALC__sgreater     (void)
{
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   s = CALC__popstr (__FUNCTION__, ++s_narg);
   if (strcmp (s, r) >  0) CALC_pushval (__FUNCTION__, TRUE );
                           CALC_pushval (__FUNCTION__, FALSE);
   return;
}



/*====================------------------------------------====================*/
/*===----                       logical operators                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LOGICAL_________o () { return; }

PRIV void
CALC__not          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, !a);
   return;
}

PRIV void
CALC__and          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b && a);
   return;
}

PRIV void
CALC__or           (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, b || a);
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
   r = CALC__popstr(__FUNCTION__, ++s_narg);
   s = CALC__popstr(__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (s == NULL)  s = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strncpy (t, s,   LEN_RECD);
   strncat (t, r,   LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
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
   r = CALC__popstr(__FUNCTION__, ++s_narg);
   s = CALC__popstr(__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (s == NULL)  s = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strncpy (t, s,   LEN_RECD);
   strncat (t, " ", LEN_RECD);
   strncat (t, r,   LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
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
   r = CALC__popstr(__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strncpy (t, r, LEN_RECD);
   len = strlen(t);
   int i;
   for (i = 0; i < len; ++i) t[i] = tolower(t[i]);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__upper        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr(__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   len = strlen(r);
   strncpy (t, r, LEN_RECD);
   int i;
   for (i = 0; i < len; ++i) t[i] = toupper(t[i]);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__char         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (n     <  32 )  n = '#';
   if (n     >= 127)  n = '#';
   /*---(process)------------------------*/
   sprintf (t, "%c", n);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__code         (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushval (__FUNCTION__, r[0]);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__len          (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   len = strlen (r);
   /*---(return result)------------------*/
   CALC_pushval (__FUNCTION__, (double) len);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__left         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   len = strlen (r);
   strncpy (t, r, LEN_RECD);
   if (n >= len)  n = len;
   t[n] = '\0';
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__right        (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   len = strlen (r);
   m = len - n;
   if (m >= len)  m = len;
   int i;
   for (i = m; i < len; ++i) t[i - m] = r[i];
   t [m] = '\0';
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__mid          (void)
{
   /*---(get arguments)------------------*/
   m = CALC__popval (__FUNCTION__, ++s_narg);
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (m     <  0  )  m = 0;
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   len = strlen (r);
   if (n     >= len)  n = len;
   if (n + m >= len)  m = len - n;
   int i;
   for (i = n; i < n + m; ++i) t[i - n] = r[i];
   t [m] = '\0';
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, t);
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
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__ltrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_HEAD, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__rtrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_TAIL, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__strim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_SINGLE, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__etrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_EVERY, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__mtrim        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_MAX, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__print        (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popprint (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, LEN_RECD);
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__formula      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popsource (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, LEN_RECD);
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__lpad         (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (__FUNCTION__, r);
   } else {
      strncpy (t, g_empty, n - m);
      t [m] = '\0';
      strcat  (t, r);
      CALC_pushstr (__FUNCTION__, t);
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
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (__FUNCTION__, r);
   } else {
      strcpy  (t, r);
      strncat (t, g_empty, n - m);
      t [n] = '\0';
      CALC_pushstr (__FUNCTION__, t);
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
   n = CALC__popval   (__FUNCTION__, ++s_narg);
   r = CALC__popprint (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, LEN_RECD);
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (__FUNCTION__, r);
   } else {
      strncpy (t, g_empty, n - m);
      t [n-m-1] = '\0';
      strcat  (t, r);
      CALC_pushstr (__FUNCTION__, t);
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
   n = CALC__popval   (__FUNCTION__, ++s_narg);
   r = CALC__popprint (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   strltrim (r, ySTR_BOTH, LEN_RECD);
   m = strlen (r);
   if (m >= n) {
      CALC_pushstr (__FUNCTION__, r);
   } else {
      strcpy  (t, r);
      strncat (t, g_empty, n - m);
      t [n] = '\0';
      CALC_pushstr (__FUNCTION__, t);
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
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   a = atof (r);
   /*---(return result)------------------*/
   CALC_pushval (__FUNCTION__, a);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salpha       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALPHA, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salphac      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALPHA, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salnum       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALNUM, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__salnumc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_ALNUM, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sbasic       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_BASIC, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sbasicc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_BASIC, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__swrite       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_WRITE, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__swritec      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_WRITE, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sexten       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_EXTEN, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sextenc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_EXTEN, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sprint       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_PRINT, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sprintc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_PRINT, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__sseven       (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_SEVEN, '-', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__ssevenc      (void)
{
   /*---(get arguments)------------------*/
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(process)------------------------*/
   strlclean (r, ySTR_SEVEN, 'y', LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
   /*---(clean up)-----------------------*/
   free (r);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__replace      (void)
{
   /*---(get arguments)------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   s = CALC__popstr (__FUNCTION__, ++s_narg);
   q = CALC__popstr (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   /*---(defense)------------------------*/
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   if (q == NULL)  q = strndup (nada, LEN_RECD);
   if (s == NULL)  s = strndup (nada, LEN_RECD);
   if (n     <  0  )  n = 0;
   /*---(process)------------------------*/
   strlrepl (r, q, s, n, LEN_RECD);
   /*---(return result)------------------*/
   CALC_pushstr (__FUNCTION__, r);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   c = b;
   for (i = 1; i <  a; ++i)  c *= b;
   CALC_pushval (__FUNCTION__, c);
   return;
}

PRIV void
CALC__abs           (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, fabs(a));
   return;
}

PRIV void
CALC__rtrunc        (void)
{
   int i = 0;
   n = CALC__popval (__FUNCTION__, ++s_narg);
   a = CALC__popval (__FUNCTION__, ++s_narg);
   for (i = 0; i < n; ++i)  a *= 10;
   a = trunc (a);
   for (i = 0; i < n; ++i)  a /= 10;
   CALC_pushval (__FUNCTION__, a);
   return;
}

PRIV void
CALC__trunc         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, trunc(a));
   return;
}

PRIV void
CALC__rround        (void)
{
   int i = 0;
   n = CALC__popval (__FUNCTION__, ++s_narg);
   a = CALC__popval (__FUNCTION__, ++s_narg);
   for (i = 0; i < n; ++i)  a *= 10;
   a = round (a);
   for (i = 0; i < n; ++i)  a /= 10;
   CALC_pushval (__FUNCTION__, a);
   return;
}

PRIV void
CALC__round         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, round(a));
   return;
}

PRIV void
CALC__ceiling       (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, ceil(a));
   return;
}

PRIV void
CALC__floor         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, floor(a));
   return;
}

PRIV void       /* PURPOSE : rand between n-m, not just 0-1 ---------------*/
CALC__rand          (void)
{
   CALC_pushval (__FUNCTION__, (double) rand());
   return;
}

PRIV void       /* PURPOSE : rand between n-m, not just 0-1 ---------------*/
CALC__randr         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, ((double) rand() / (double) RAND_MAX) * (b - a) + a);
   return;
}

PRIV void
CALC__sqrt          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, sqrt(a));
   return;
}

PRIV void
CALC__cbrt          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, cbrt(a));
   return;
}

PRIV void
CALC__sqr           (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a * a);
   return;
}

PRIV void
CALC__cube          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a * a * a);
   return;
}



/*====================------------------------------------====================*/
/*===----                         trig functions                       ----===*/
/*====================------------------------------------====================*/

PRIV void  o___TRIG____________o () { return; }

PRIV void
CALC__degrees       (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a * RAD2DEG);
   return;
}

PRIV void
CALC__radians       (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a * DEG2RAD);
   return;
}

PRIV void
CALC__pi            (void)
{
   CALC_pushval (__FUNCTION__, 3.1415927);
   return;
}

PRIV void
CALC__hypot         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, sqrt(a * a + b * b));
   return;
}

PRIV void
CALC__side          (void)
{
   b = CALC__popval (__FUNCTION__, ++s_narg);
   c = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, sqrt(c * c - b * b));
   return;
}

PRIV void
CALC__sin           (void)
{
   int i;
   a = CALC__popval (__FUNCTION__, ++s_narg);
   i = round (a * 10.0);
   i = i % 3600;
   if (i < 0)  i = 3600 + i;
   CALC_pushval (__FUNCTION__, trig[i].si);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, 1);
   CALC_pushval (__FUNCTION__, a);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   i = round (a * 10.0);
   i = i % 3600;
   if (i < 0)  i = 3600 + i;
   CALC_pushval (__FUNCTION__, trig[i].co);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, 1);
   CALC_pushval (__FUNCTION__, a);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   i = round (a * 10.0);
   i = i % 3600;
   if (i < 0)  i = 3600 + i;
   CALC_pushval (__FUNCTION__, trig[i].ta);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, 1);
   CALC_pushval (__FUNCTION__, a);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a / 2.0);
   CALC__sin     ();
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, a * 2.0);
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, asin(a) * RAD2DEG);
   return;
}

PRIV void
CALC__asinr         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, asin(a));
   return;
}

PRIV void
CALC__acos          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, acos(a) * RAD2DEG);
   return;
}

PRIV void
CALC__acosr         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, acos(a));
   return;
}

PRIV void
CALC__atan          (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, atan(a) * RAD2DEG);
   return;
}

PRIV void
CALC__atanr         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, atan(a));
   return;
}

PRIV void
CALC__atan2         (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, atan2(b,a) * RAD2DEG);
   return;
}

PRIV void
CALC__atanr2        (void)
{
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushval (__FUNCTION__, atan2(b,a));
   return;
}



/*====================------------------------------------====================*/
/*===----                    cell address functions                    ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ADDRESS_________o () { return; }

PRIV void
CALC__offset        (char *a_func, int a_tab, int a_col, int a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rc;
   int         x_tab;
   int         x_col;
   int         x_row;
   int         x_abs;
   tCELL      *x_base;
   tCELL      *x_new;
   /*---(get the base reference)---------*/
   x_base = CALC__popref (a_func      , ++s_narg);
   if (x_base == NULL) return;
   /*---(parse base reference)-----------*/
   rc = LOC_coordinates (x_base, &x_tab, &x_col, &x_row);
   if (rc    <  0   )   {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ADDR , "base reference could not be parsed");
      return;
   }
   /*---(calc offset reference)----------*/
   x_tab += a_tab;
   x_col += a_col;
   x_row += a_row;
   rc     = LOC_legal  (x_tab, x_col, x_row, CELL_FIXED);
   if (rc    <  0   )   {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ADDR , "new offset reference is not valid");
      return;
   }
   /*---(identify new cell)--------------*/
   x_new  = LOC_cell   (x_tab, x_col, x_row);
   if      (x_new == NULL)                      CALC_pushval (__FUNCTION__, 0);
   else if (x_new->s == NULL)                   CALC_pushval (__FUNCTION__, 0);
   else if (x_new->t == 'n' || x_new->t == 'f') CALC_pushval (__FUNCTION__, x_new->v_num);
   else                                         CALC_pushstr (__FUNCTION__, x_new->s);
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__offs          (void)
{
   n = CALC__popval (__FUNCTION__, ++s_narg);
   m = CALC__popval (__FUNCTION__, ++s_narg);
   o = CALC__popval (__FUNCTION__, ++s_narg);
   CALC__offset  (__FUNCTION__,     o,    m,    n);
   return;
}

PRIV void
CALC__offt          (void)
{
   n = CALC__popval (__FUNCTION__, ++s_narg);
   CALC__offset  ("offt",     n,    0,    0);
   return;
}

PRIV void
CALC__offc          (void)
{
   n = CALC__popval (__FUNCTION__, ++s_narg);
   CALC__offset  ("offc",     0,    n,    0);
   return;
}

PRIV void
CALC__offr          (void)
{
   n = CALC__popval (__FUNCTION__, ++s_narg);
   CALC__offset  ("offr",     0,    0,    n);
   return;
}

PRIV void
CALC__loc           (void)
{
   char   rc;
   tCELL *x_new;
   n = CALC__popval (__FUNCTION__, ++s_narg);
   m = CALC__popval (__FUNCTION__, ++s_narg);
   o = CALC__popval (__FUNCTION__, ++s_narg);
   rc = LOC_legal  (    o,    m,    n, CELL_FIXED);
   if (rc    <  0   )   {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "address created is not legal");
      return;
   }
   x_new  = LOC_cell   (o, m, n);
   /*> if (s_me->u != x_new->u) {                                                     <*/
   DEP_delcalcref (s_me);
   DEP_create     (G_DEP_CALCREF, s_me, x_new);
   /*> }                                                                              <*/
   CALC_pushref (__FUNCTION__, x_new);
   /*> printf ("CALC_loc %s\n", s_me->label);                                         <*/
   return;
}

PRIV void
SHARED__address    (char *a_func, char a_type)
{
   char   rc;
   int    x_tab;
   int    x_col;
   int    x_row;
   tCELL *x_base;
   /*---(get the base reference)---------*/
   x_base = CALC__popref (a_func      , ++s_narg);
   if (x_base == NULL)   return;
   /*---(parse base reference)-----------*/
   rc = LOC_coordinates (x_base, &x_tab, &x_col, &x_row);
   if (rc    <  0   )   {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ADDR , "base reference could not be parsed");
      return;
   }
   /*---(push the info back on stack)----*/
   switch (a_type) {
   case 't' :
      CALC_pushval (__FUNCTION__, x_tab);
      break;
   case 'c' :
      CALC_pushval (__FUNCTION__, x_col);
      break;
   default  :
      CALC_pushval (__FUNCTION__, x_row);
      break;
   }
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__addr          (void)
{
   tCELL *x_base;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   if (x_base == NULL)   return;
   CALC_pushstr (__FUNCTION__, x_base->label);
   return;
}

PRIV void
CALC__cell          (char a_type)
{
   tCELL *x_base;
   char   x_type  = a_type;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   if (x_base == NULL) {
      CALC_pushval (__FUNCTION__, FALSE);
      return;
   }
   if (x_base->t == a_type)  CALC_pushval (__FUNCTION__, TRUE);
   else                      CALC_pushval (__FUNCTION__, FALSE);
   return;
}

PRIV void
CALC__isnum         (void)
{
   CALC__cell ('n');
   return;
}

PRIV void
CALC__isfor         (void)
{
   tCELL *x_base;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   /*---(formula)--------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell ('f');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(like)-----------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('l');
   /*---(complete)-------*/
   return;
}

PRIV void
CALC__isval         (void)
{
   tCELL *x_base;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   /*---(number)---------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('n');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(formula)--------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('f');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(like)-----------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('l');
   /*---(complete)-------*/
   return;
}

PRIV void
CALC__iscalc        (void)
{
   tCELL *x_base;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   /*---(formula)--------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('f');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(formula)--------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('m');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(like)-----------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('l');
   /*---(complete)-------*/
   return;
}

PRIV void
CALC__isstr         (void)
{
   CALC__cell ('s');
   return;
}

PRIV void
CALC__ismod         (void)
{
   CALC__cell ('m');
   return;
}

PRIV void
CALC__istext        (void)
{
   tCELL *x_base;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   /*---(formula)--------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('s');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(like)-----------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('m');
   /*---(complete)-------*/
   return;
}

PRIV void
CALC__iserror       (void)
{
   CALC__cell ('E');
   return;
}

PRIV void
CALC__islike        (void)
{
   CALC__cell ('l');
   return;
}

PRIV void
CALC__isblank       (void)
{
   CALC__cell ('-');
   return;
}

PRIV void
CALC__ispoint       (void)
{
   tCELL *x_base;
   x_base = CALC__popref (__FUNCTION__, ++s_narg);
   /*---(range)----------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('p');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   if (n == TRUE) {
      CALC_pushval (__FUNCTION__, n);
      return;
   }
   /*---(address---------*/
   CALC_pushref (__FUNCTION__, x_base);
   CALC__cell   ('a');
   /*---(complete)-------*/
   return;
}

PRIV void
CALC__me            (void)
{
   CALC_pushstr (__FUNCTION__, s_me->label);
   return;
}

PRIV void
CALC__filename      (void)
{
   CALC_pushstr (__FUNCTION__, my.f_title);
   return;
}

PRIV void
CALC__filebase      (void)
{
   CALC_pushstr (__FUNCTION__, my.f_name);
   return;
}

PRIV void
CALC__tabname       (void)
{
   SHARED__address (__FUNCTION__, 't');
   n = CALC__popval (__FUNCTION__, ++s_narg);
   CALC_pushstr (__FUNCTION__, tabs[n].name);
   return;
}

PRIV void
CALC__tab           (void)
{  SHARED__address (__FUNCTION__, 't');
   return;
}

PRIV void
CALC__col           (void)
{  SHARED__address (__FUNCTION__, 'c');
   return;
}

PRIV void
CALC__row           (void)
{  SHARED__address (__FUNCTION__, 'r');
   return;
}

static short   s_btab     = 0;
static short   s_bcol     = 0;
static short   s_brow     = 0;
static short   s_etab     = 0;
static short   s_ecol     = 0;
static short   s_erow     = 0;

static char
SHARED__rangeparse (char *a_func)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_beg       = NULL;
   tCELL      *x_end       = NULL;
   char        rce         = -10;
   char        rc          = 0;
   /*---(get range addresses)------------*/
   x_end = CALC__popref (a_func      , ++s_narg);
   DEBUG_CALC   yLOG_point   ("x_end"     , x_end);
   --rce;  if (x_end == NULL)   return rce;
   x_beg = CALC__popref (a_func      , ++s_narg);
   DEBUG_CALC   yLOG_point   ("x_beg"     , x_beg);
   --rce;  if (x_beg == NULL)   return rce;
   /*---(parse beginning)----------------*/
   rc = LOC_coordinates (x_beg, &s_btab, &s_bcol, &s_brow);
   DEBUG_CALC   yLOG_value   ("s_btab"    , s_btab);
   DEBUG_CALC   yLOG_value   ("s_bcol"    , s_bcol);
   DEBUG_CALC   yLOG_value   ("s_brow"    , s_brow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   --rce;  if (rc <  0 ) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ADDR , "beg reference could not be parsed");
      return rce;
   }
   /*---(parse ending)-------------------*/
   rc = LOC_coordinates (x_end, &s_etab, &s_ecol, &s_erow);
   DEBUG_CALC   yLOG_value   ("s_etab"    , s_etab);
   DEBUG_CALC   yLOG_value   ("s_ecol"    , s_ecol);
   DEBUG_CALC   yLOG_value   ("s_erow"    , s_erow);
   DEBUG_CALC   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      ERROR_add (s_me, PERR_EVAL, s_neval, a_func, TERR_ADDR , "end reference could not be parsed");
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

PRIV void        /* PURPOSE : total the numeric cells in a range ----------*/
SHARED__rangestat  (char *a_func, char a_type)
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
   rc = SHARED__rangeparse (a_func);
   if (rc < 0)  return;
   /*---(return)-------------------------*/
   switch (a_type) {
   case 'd' :
      CALC_pushval (a_func, sqrt (pow (s_ecol - s_bcol + 1, 2) + pow (s_erow - s_brow + 1, 2)));
      break;
   case 't' :
      CALC_pushval (a_func, s_etab - s_btab + 1);
      break;
   case 'c' :
      CALC_pushval (a_func, s_ecol - s_bcol + 1);
      break;
   case 'r' :
      CALC_pushval (a_func, s_ecol - s_bcol + 1);
      break;
   default  : 
      CALC_pushval (a_func, s_erow - s_brow + 1);
      break;
   }
   /*---(complete)-----------------------*/
   return;
}

PRIV void
CALC__dist          (void)
{  SHARED__rangestat (__FUNCTION__, 'd');
   return;
}

PRIV void
CALC__tabs          (void)
{  SHARED__rangestat (__FUNCTION__, 't');
   return;
}

PRIV void
CALC__cols          (void)
{  SHARED__rangestat (__FUNCTION__, 'c');
   return;
}

PRIV void
CALC__rows          (void)
{  SHARED__rangestat (__FUNCTION__, 'r');
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
   a = CALC__popval (__FUNCTION__, ++s_narg);
   b = CALC__popval (__FUNCTION__, ++s_narg);
   c = CALC__popval (__FUNCTION__, ++s_narg);
   if (c) CALC_pushval (__FUNCTION__, b);
   else   CALC_pushval (__FUNCTION__, a);
   return;
}

PRIV void
CALC__ifs           (void)
{
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   s = CALC__popstr (__FUNCTION__, ++s_narg);
   c = CALC__popval (__FUNCTION__, ++s_narg);
   if (c) CALC_pushstr (__FUNCTION__, s);
   else   CALC_pushstr (__FUNCTION__, r);
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
   CALC_pushval (__FUNCTION__, time(NULL));
   return;
}

PRIV void
CALC__year          (void)
{
   char temp[100];
   a = CALC__popval (__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%Y", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__month         (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%m", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__day           (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%d", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__hour          (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%H", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__minute        (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%M", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__second        (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%S", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__weekday       (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%w", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
   return;
}

PRIV void
CALC__weeknum       (void)
{
   char temp[100];
   a = CALC__popval(__FUNCTION__, ++s_narg);
   time_t  xtime = (time_t) a;
   strftime(temp, 100, "%W", localtime(&xtime));
   CALC_pushval (__FUNCTION__, atoi(temp));
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
   r   = CALC__popstr(__FUNCTION__, ++s_narg);
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
                  ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_OTHER, "time value could not be parsed");
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
   else if (yr <  1900) {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_OTHER, "year can not be less than 1900");
      return;
   }
   else if (yr >= 1900) temp->tm_year = yr - 1900;
   time_t  xtime = mktime(temp);
   /*---(complete)-----------------------*/
   CALC_pushval (__FUNCTION__, (double) xtime);
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
SHARED__gather     (char *a_func)
{
   /*---(locals)-----------+-----------+-*/
   int         x_row       = 0;
   int         x_col       = 0;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   /*---(get values)---------------------*/
   rc = SHARED__rangeparse (a_func);
   if (rc < 0)  return;
   /*---(process)------------------------*/
   cnt = cnta = cnts = cntb = cntr = 0;
   tot = 0;
   min =   MAX;
   max = -(MAX);
   for (x_col = s_bcol; x_col <= s_ecol; ++x_col) {
      for (x_row = s_brow; x_row <= s_erow; ++x_row) {
         DEBUG_CALC   yLOG_value   ("x_col"    , x_col);
         DEBUG_CALC   yLOG_value   ("x_row"     , x_row);
         x_curr = tabs[s_btab].sheet[x_col][x_row];
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
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, tot);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count all numeric cells in a range ----[ ------ [ ------ ]-*/
CALC__count        (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, cnt);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count all filled cells in a range -----[ ------ [ ------ ]-*/
CALC__counts       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, cnts);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count all filled cells in a range -----[ ------ [ ------ ]-*/
CALC__counta       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, cnta);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count non-filled cells in a range -----[ ------ [ ------ ]-*/
CALC__countb       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, cntb);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> count non-filled cells in a range -----[ ------ [ ------ ]-*/
CALC__countr       (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, cntr);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> average all numeric cells in a range --[ ------ [ ------ ]-*/
CALC__average      (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0) {
      if (cnt > 0)  CALC_pushval   (__FUNCTION__, tot / cnt);
      else          CALC_pushval   (__FUNCTION__, cnt);
   }
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find min of all numeric cells in range [ ------ [ ------ ]-*/
CALC__minimum      (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, min);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find max of all numeric cells in range [ ------ [ ------ ]-*/
CALC__maximum      (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, max);
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return;
}

PRIV void    /*--> find max of all numeric cells in range [ ------ [ ------ ]-*/
CALC__range        (void)
{
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   SHARED__gather (__FUNCTION__);
   if (errornum == 0)  CALC_pushval   (__FUNCTION__, max - min);
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
   SHARED__gather (__FUNCTION__);
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
   if      (cnt == 0)  {
      DEBUG_CALC   yLOG_value   ("pushing"   , 0);
      CALC_pushval (__FUNCTION__,  0);
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
   CALC_pushval (__FUNCTION__,  q1 );
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
   CALC_pushval (__FUNCTION__,  q2 );
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
   CALC_pushval (__FUNCTION__,  q3 );
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
   CALC_pushval (__FUNCTION__,  qr );
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
   SHARED__gather (__FUNCTION__);
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
   CALC__rangesort ();
   if      (cnt == 0)  {
      DEBUG_CALC   yLOG_value   ("pushing"   , 0);
      CALC_pushval (__FUNCTION__,  0);
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
   CALC_pushval (__FUNCTION__, (double) x_mode);
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
   SHARED__gather (__FUNCTION__);
   /*---(defense: bad range)-------------*/
   if (errornum <  0) {
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return;
   }
   DEBUG_CALC   yLOG_value   ("cnt"       , cnt);
   /*---(defense: too few values)--------*/
   if      (cnt <= 1)  {
      DEBUG_CALC   yLOG_value   ("pushing"   , 0);
      CALC_pushval (__FUNCTION__,  0);
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
   CALC_pushval (__FUNCTION__, x_stddev);
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
CALC__vlookup      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         x_row       = 0;
   tCELL      *x_curr      = NULL;
   /*---(get values)---------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(get values)---------------------*/
   rc = SHARED__rangeparse (__FUNCTION__);
   if (rc < 0)  return;
   if (s_btab != s_etab)  {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "beg and end must be on the same tab");
      return;
   }
   if (s_bcol + n >  s_ecol)              {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "requested offset is out of range");
      return;
   }
   /*---(process)------------------------*/
   for (x_row = s_brow; x_row <= s_erow; ++x_row) {
      DEBUG_CALC   yLOG_value   ("x_row"     , x_row);
      x_curr = tabs[s_btab].sheet[s_bcol][x_row];
      DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr == NULL)                                       continue;
      DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
      if (strchr ("s", x_curr->t) == 0   )                      continue;
      if (x_curr->s == NULL)                                    continue;
      if (x_curr->s [0] != r [0])                               continue;
      if (strcmp (x_curr->s, r) != 0)                           continue;
      /*> CALC_pushref (__FUNCTION__, LOC_cell (s_btab, s_bcol, x_row));                           <*/
      x_curr = tabs[s_btab].sheet[s_bcol + n][x_row];
      if      (x_curr == NULL)          CALC_pushval (__FUNCTION__, 0);
      else if (x_curr->s == NULL)       CALC_pushval (__FUNCTION__, 0);
      else if (x_curr->t == 'n')        CALC_pushval (__FUNCTION__, x_curr->v_num);
      else if (x_curr->t == 'f')        CALC_pushval (__FUNCTION__, x_curr->v_num);
      else                              CALC_pushstr (__FUNCTION__, x_curr->s);
      return;
   }
   /*---(nothing found)------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "match not found");
   CALC_pushval (__FUNCTION__, 0);
   /*---(complete)-----------------------*/
   return;
}

PRIV void        /* PURPOSE : search left column in range for string ------*/
CALC__hlookup      (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_col       = 0;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   /*---(get values)---------------------*/
   n = CALC__popval (__FUNCTION__, ++s_narg);
   r = CALC__popstr (__FUNCTION__, ++s_narg);
   if (r == NULL)  r = strndup (nada, LEN_RECD);
   /*---(get values)---------------------*/
   rc = SHARED__rangeparse (__FUNCTION__);
   if (rc < 0)  return;
   if (s_btab != s_etab)  {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "beg and end must be on the same tab");
      return;
   }
   if (s_brow + n >  s_erow)              {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "requested offset is out of range");
      return;
   }
   /*---(process)------------------------*/
   for (x_col = s_bcol; x_col <= s_ecol; ++x_col) {
      DEBUG_CALC   yLOG_value   ("x_col"    , x_col);
      x_curr = tabs[s_btab].sheet[x_col][s_brow];
      DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr == NULL)                                       continue;
      DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
      if (strchr ("s", x_curr->t) == 0   )                      continue;
      if (x_curr->s == NULL)                                    continue;
      if (x_curr->s [0] != r [0])                               continue;
      if (strcmp (x_curr->s, r) != 0)                           continue;
      /*> CALC_pushref (__FUNCTION__, LOC_cell (s_btab, s_bcol, x_crow));                           <*/
      x_curr = tabs[s_btab].sheet[x_col][s_brow + n];
      if (x_curr == NULL)                    { CALC_pushval (__FUNCTION__, 0); return; }
      if (x_curr->s == NULL)                 { CALC_pushval (__FUNCTION__, 0); return; }
      if (strchr ("nfl", x_curr->t) != 0) CALC_pushval (__FUNCTION__, x_curr->v_num);
      else                                CALC_pushstr (__FUNCTION__, x_curr->s);
      return;
   }
   /*---(nothing found)------------------*/
   ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "match not found");
   CALC_pushval (__FUNCTION__, 0);
   /*---(complete)-----------------------*/
   return;
}

PRIV void        /* PURPOSE : search left column in range for string ------*/
CALC__entry        (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_row       = 0;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   /*---(get values)---------------------*/
   rc = SHARED__rangeparse (__FUNCTION__);
   if (rc < 0)  return;
   if (s_btab != s_etab)  {
      ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_ADDR , "beg and end must be on the same tab");
      return;
   }
   /*---(process)------------------------*/
   for (x_row = s_me->row; x_row >= s_brow; --x_row) {
      DEBUG_CALC   yLOG_value   ("x_row"    , x_row);
      x_curr = tabs[s_btab].sheet[s_bcol][x_row];
      DEBUG_CALC   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr    == NULL)                                    continue;
      DEBUG_CALC   yLOG_char    ("x_curr->t" , x_curr->t);
      if (strchr ("sm"  , x_curr->t) == 0)                      continue;
      DEBUG_CALC   yLOG_note    ("found it");
      if (x_curr->t == 's')  CALC_pushstr (__FUNCTION__, x_curr->s);
      else                   CALC_pushstr (__FUNCTION__, x_curr->v_str);
      return;
   }
   /*---(nothing found)------------------*/
   CALC_pushstr   (__FUNCTION__, "");
   /*---(complete)-----------------------*/
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
   char        type;                   /* type (func, op, or const)           */
   char        terms       [10];       /* number of terms                     */
   char        cat         [10];       /* category                            */
   char        desc        [60];       /* descriptive label                   */
} funcs [MAX_FUNCS] = {
   /*---(mathmatical operators)-----------*/
   { "+"          ,  0, CALC__add               , 'o', "v:vv"   , "math"     , "v = m + n;"                                         },
   { "-"          ,  0, CALC__subtract          , 'o', "v:vv"   , "math"     , "v = m - n;"                                         },
   { "*"          ,  0, CALC__multiply          , 'o', "v:vv"   , "math"     , "v = m * n;"                                         },
   { "/"          ,  0, CALC__divide            , 'o', "v:vv"   , "math"     , "v = m / n;"                                         },
   { "%"          ,  0, CALC__modulus           , 'o', "v:vv"   , "math"     , "v = (int) ((m / n - trunc (m / n)) * n);"           },
   { "++"         ,  0, CALC__increment         , 'o', "v:v"    , "math"     , "v = m + 1;"                                         },
   { "--"         ,  0, CALC__decrement         , 'o', "v:v"    , "math"     , "v = m - 1;"                                         },
   { "-:"         ,  0, CALC__unaryminus        , 'o', "v:v"    , "math"     , "v = -m;"                                            },
   /*---(mathmatical functions)-----------*/
   { "exp"        ,  0, CALC__power             , 'f', "v:vv"   , "math"     , ""                                                   },
   { "abs"        ,  0, CALC__abs               , 'f', "v:v"    , "math"     , ""                                                    },
   { "trunc"      ,  0, CALC__trunc             , 'f', "v:v"    , "math"     , ""                                                   },
   { "rtrunc"     ,  0, CALC__rtrunc            , 'f', "v:vv"   , "math"     , ""                                                   },
   { "round"      ,  0, CALC__round             , 'f', "v:v"    , "math"     , ""                                                   },
   { "rround"     ,  0, CALC__rround            , 'f', "v:vv"   , "math"     , ""                                                   },
   { "ceil"       ,  0, CALC__ceiling           , 'f', "v:v"    , "math"     , ""                                                   },
   { "floor"      ,  0, CALC__floor             , 'f', "v:v"    , "math"     , ""                                                   },
   { "sqrt"       ,  0, CALC__sqrt              , 'f', "v:v"    , "math"     , ""                                                   },
   { "cbrt"       ,  0, CALC__cbrt              , 'f', "v:v"    , "math"     , ""                                                   },
   { "sqr"        ,  0, CALC__sqr               , 'f', "v:v"    , "math"     , ""                                                   },
   { "cube"       ,  0, CALC__cube              , 'f', "v:v"    , "math"     , ""                                                   },
   { "rand"       ,  0, CALC__rand              , 'f', "v:"     , "math"     , ""                                                   },
   { "randr"      ,  0, CALC__randr             , 'f', "v:vv"   , "math"     , ""                                                   },
   /*---(logical operators)---------------*/
   { "=="         ,  0, CALC__equal             , 'o', "t:vv"   , "logic"    , ""                                                   },
   { "!="         ,  0, CALC__notequal          , 'o', "t:vv"   , "logic"    , ""                                                   },
   { ">"          ,  0, CALC__greater           , 'o', "t:vv"   , "logic"    , ""                                                   },
   { "<"          ,  0, CALC__lesser            , 'o', "t:vv"   , "logic"    , ""                                                   },
   { ">="         ,  0, CALC__gequal            , 'o', "t:vv"   , "logic"    , ""                                                   },
   { "<="         ,  0, CALC__lequal            , 'o', "t:vv"   , "logic"    , ""                                                   },
   { "!"          ,  0, CALC__not               , 'o', "t:v"    , "logic"    , "if true, returns false, else true"                  },
   { "&&"         ,  0, CALC__and               , 'o', "t:vv"   , "logic"    , "if both arg are true, returns true, else false"     },
   { "||"         ,  0, CALC__or                , 'o', "t:vv"   , "logic"    , "if either arg is true, returns true, else false"    },
   /*---(logical functions)---------------*/
   { "if"         ,  0, CALC__if                , 'f', "v:vvv"  , "logic"    , "if t is true, returns val1, else val2"              },
   /*---(string logic operators)----------*/
   { "#="         ,  0, CALC__sequal            , 'o', "t:ss"   , "slogic"   , ""                                                   },
   { "#!"         ,  0, CALC__snotequal         , 'o', "t:ss"   , "slogic"   , ""                                                   },
   { "#<"         ,  0, CALC__slesser           , 'o', "t:ss"   , "slogic"   , ""                                                   },
   { "#>"         ,  0, CALC__sgreater          , 'o', "t:ss"   , "slogic"   , ""                                                   },
   /*---(string logic functions)----------*/
   { "ifs"        ,  0, CALC__ifs               , 'f', "s:vss"  , "slogic"   , "if t is true, returns str1, else str2"              },
   /*---(string operators)----------------*/
   { "#"          ,  0, CALC__concat            , 'o', "s:ss"   , "string"   , ""                                                   },
   { "##"         ,  0, CALC__concatplus        , 'o', "s:ss"   , "string"   , ""                                                   },
   /*---(string functions)----------------*/
   { "len"        ,  0, CALC__len               , 'f', "v:s"    , "string"   , ""                                                   },
   { "left"       ,  0, CALC__left              , 'f', "s:sv"   , "string"   , ""                                                   },
   { "right"      ,  0, CALC__right             , 'f', "s:sv"   , "string"   , ""                                                   },
   { "mid"        ,  0, CALC__mid               , 'f', "s:svv"  , "string"   , ""                                                   },
   { "trim"       ,  0, CALC__trim              , 'f', "s:s"    , "string"   , ""                                                   },
   { "ltrim"      ,  0, CALC__ltrim             , 'f', "s:s"    , "string"   , ""                                                   },
   { "rtrim"      ,  0, CALC__rtrim             , 'f', "s:s"    , "string"   , ""                                                   },
   { "strim"      ,  0, CALC__strim             , 'f', "s:s"    , "string"   , ""                                                   },
   { "etrim"      ,  0, CALC__etrim             , 'f', "s:s"    , "string"   , ""                                                   },
   { "mtrim"      ,  0, CALC__mtrim             , 'f', "s:s"    , "string"   , ""                                                   },
   { "print"      ,  0, CALC__print             , 'f', "s:a"    , "string"   , ""                                                   },
   { "p"          ,  0, CALC__print             , 'f', "s:a"    , "string"   , ""                                                   },
   { "formula"    ,  0, CALC__formula           , 'f', "s:a"    , "string"   , ""                                                   },
   { "f"          ,  0, CALC__formula           , 'f', "s:a"    , "string"   , ""                                                   },
   { "lpad"       ,  0, CALC__lpad              , 'f', "s:sv"   , "string"   , ""                                                   },
   { "rpad"       ,  0, CALC__rpad              , 'f', "s:sv"   , "string"   , ""                                                   },
   { "lppad"      ,  0, CALC__lppad             , 'f', "s:av"   , "string"   , ""                                                   },
   { "rppad"      ,  0, CALC__rppad             , 'f', "s:av"   , "string"   , ""                                                   },
   /*---(conversion functions)------------*/
   { "lower"      ,  0, CALC__lower             , 'f', "s:s"    , "conv"     , ""                                                   },
   { "upper"      ,  0, CALC__upper             , 'f', "s:s"    , "conv"     , ""                                                   },
   { "char"       ,  0, CALC__char              , 'f', "s:v"    , "conv"     , ""                                                   },
   { "code"       ,  0, CALC__code              , 'f', "s:s"    , "conv"     , ""                                                   },
   { "value"      ,  0, CALC__value             , 'f', "s:s"    , "conv"     , ""                                                   },
   { "salpha"     ,  0, CALC__salpha            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "salphac"    ,  0, CALC__salphac           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "salnum"     ,  0, CALC__salnum            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "salnumc"    ,  0, CALC__salnumc           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sbasic"     ,  0, CALC__sbasic            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sbasicc"    ,  0, CALC__sbasicc           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "swrite"     ,  0, CALC__swrite            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "swritec"    ,  0, CALC__swritec           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sexten"     ,  0, CALC__sexten            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sextenc"    ,  0, CALC__sextenc           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sprint"     ,  0, CALC__sprint            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sprintc"    ,  0, CALC__sprintc           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "sseven"     ,  0, CALC__sseven            , 'f', "s:s"    , "conv"     , ""                                                   },
   { "ssevenc"    ,  0, CALC__ssevenc           , 'f', "s:s"    , "conv"     , ""                                                   },
   { "replace"    ,  0, CALC__replace           , 'f', "s:sssv" , "conv"     , ""                                                   },
   /*---(trig functions)------------------*/
   { "rad"        ,  0, CALC__radians           , 'f', "v:v"    , "trig"     , ""                                                   },
   { "deg"        ,  0, CALC__degrees           , 'f', "v:v"    , "trig"     , ""                                                   },
   { "pi"         ,  0, CALC__pi                , 'f', "v:"     , "trig"     , ""                                                   },
   { "hypot"      ,  0, CALC__hypot             , 'f', "v:vv"   , "trig"     , ""                                                   },
   { "side"       ,  0, CALC__side              , 'f', "v:vv"   , "trig"     , ""                                                   },
   { "sin"        ,  0, CALC__sin               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "sinr"       ,  0, CALC__sinr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "csc"        ,  0, CALC__csc               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "cscr"       ,  0, CALC__cscr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "cos"        ,  0, CALC__cos               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "cosr"       ,  0, CALC__cosr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "sec"        ,  0, CALC__sec               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "secr"       ,  0, CALC__secr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "tan"        ,  0, CALC__tan               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "tanr"       ,  0, CALC__tanr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "cot"        ,  0, CALC__cot               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "cotr"       ,  0, CALC__cotr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "crd"        ,  0, CALC__crd               , 'f', "v:v"    , "trig"     , ""                                                   },
   { "crdr"       ,  0, CALC__crdr              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "asin"       ,  0, CALC__asin              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "asinr"      ,  0, CALC__asinr             , 'f', "v:v"    , "trig"     , ""                                                   },
   { "acos"       ,  0, CALC__acos              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "acosr"      ,  0, CALC__acosr             , 'f', "v:v"    , "trig"     , ""                                                   },
   { "atan"       ,  0, CALC__atan              , 'f', "v:v"    , "trig"     , ""                                                   },
   { "atanr"      ,  0, CALC__atanr             , 'f', "v:v"    , "trig"     , ""                                                   },
   { "atan2"      ,  0, CALC__atan2             , 'f', "v:vv"   , "trig"     , ""                                                   },
   { "atanr2"     ,  0, CALC__atanr2            , 'f', "v:vv"   , "trig"     , ""                                                   },
   /*---(address functions)---------------*/
   { "offs"       ,  0, CALC__offs              , 'f', "?:v"    , "addr"     , ""                                                   },
   { "offt"       ,  0, CALC__offt              , 'f', "?:v"    , "addr"     , ""                                                   },
   { "offc"       ,  0, CALC__offc              , 'f', "?:v"    , "addr"     , ""                                                   },
   { "offr"       ,  0, CALC__offr              , 'f', "?:v"    , "addr"     , ""                                                   },
   { "loc"        ,  0, CALC__loc               , 'f', "r:vvv"  , "addr"     , ""                                                   },
   /*---(cell info functions)-------------*/
   { "isnum"      ,  0, CALC__isnum             , 'f', "t:a"    , "info"     , ""                                                   },
   { "isfor"      ,  0, CALC__isfor             , 'f', "t:a"    , "info"     , ""                                                   },
   { "isvalue"    ,  0, CALC__isval             , 'f', "t:a"    , "info"     , ""                                                   },
   { "isstr"      ,  0, CALC__isstr             , 'f', "t:a"    , "info"     , ""                                                   },
   { "ismod"      ,  0, CALC__ismod             , 'f', "t:a"    , "info"     , ""                                                   },
   { "istext"     ,  0, CALC__istext            , 'f', "t:a"    , "info"     , ""                                                   },
   { "isblank"    ,  0, CALC__isblank           , 'f', "t:a"    , "info"     , ""                                                   },
   { "iscalc"     ,  0, CALC__iscalc            , 'f', "t:a"    , "info"     , ""                                                   },
   { "ispoint"    ,  0, CALC__ispoint           , 'f', "t:a"    , "info"     , ""                                                   },
   { "iserror"    ,  0, CALC__iserror           , 'f', "t:a"    , "info"     , ""                                                   },
   { "me"         ,  0, CALC__me                , 'f', "s:"     , "info"     , ""                                                   },
   { "addr"       ,  0, CALC__addr              , 'f', "s:"     , "info"     , ""                                                   },
   { "filename"   ,  0, CALC__filename          , 'f', "s:"     , "info"     , ""                                                   },
   { "filebase"   ,  0, CALC__filebase          , 'f', "s:"     , "info"     , ""                                                   },
   { "tabname"    ,  0, CALC__tabname           , 'f', "s:a"    , "info"     , ""                                                   },
   { "tab"        ,  0, CALC__tab               , 'f', "v:a"    , "info"     , ""                                                   },
   { "col"        ,  0, CALC__col               , 'f', "v:a"    , "info"     , ""                                                   },
   { "row"        ,  0, CALC__row               , 'f', "v:a"    , "info"     , ""                                                   },
   /*---(range info functions)------------*/
   { "dist"       ,  0, CALC__dist              , 'f', "v:r"    , "range"    , ""                                                   },
   { "tabs"       ,  0, CALC__tabs              , 'f', "v:r"    , "range"    , ""                                                   },
   { "cols"       ,  0, CALC__cols              , 'f', "v:r"    , "range"    , ""                                                   },
   { "rows"       ,  0, CALC__rows              , 'f', "v:r"    , "range"    , ""                                                   },
   /*---(date functions)------------------*/
   { "today"      ,  0, CALC__now               , 'f', "v:"     , "date"     , ""                                                   },
   { "now"        ,  0, CALC__now               , 'f', "v:"     , "date"     , ""                                                   },
   { "year"       ,  0, CALC__year              , 'f', "v:s"    , "date"     , ""                                                   },
   { "month"      ,  0, CALC__month             , 'f', "v:s"    , "date"     , ""                                                   },
   { "day"        ,  0, CALC__day               , 'f', "v:s"    , "date"     , ""                                                   },
   { "hour"       ,  0, CALC__hour              , 'f', "v:s"    , "date"     , ""                                                   },
   { "minute"     ,  0, CALC__minute            , 'f', "v:s"    , "date"     , ""                                                   },
   { "second"     ,  0, CALC__second            , 'f', "v:s"    , "date"     , ""                                                   },
   { "weekday"    ,  0, CALC__weekday           , 'f', "v:s"    , "date"     , ""                                                   },
   { "weeknum"    ,  0, CALC__weeknum           , 'f', "v:s"    , "date"     , ""                                                   },
   { "datevalue"  ,  0, CALC__timevalue         , 'f', "v:s"    , "date"     , ""                                                   },
   { "tv"         ,  0, CALC__timevalue         , 'f', "v:s"    , "date"     , ""                                                   },
   /*---(statistical functions)-----------*/
   { "sum"        ,  0, CALC__sum               , 'f', "v:r"    , "stat"     , ""                                                   },
   { "s"          ,  0, CALC__sum               , 'f', "v:r"    , "stat"     , ""                                                   },
   { "count"      ,  0, CALC__count             , 'f', "v:r"    , "stat"     , ""                                                   },
   { "c"          ,  0, CALC__count             , 'f', "v:r"    , "stat"     , ""                                                   },
   { "countn"     ,  0, CALC__count             , 'f', "v:r"    , "stat"     , ""                                                   },
   { "counts"     ,  0, CALC__counts            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "counta"     ,  0, CALC__counta            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "countb"     ,  0, CALC__countb            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "countr"     ,  0, CALC__countr            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "reqs"       ,  0, CALC__countr            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "avg"        ,  0, CALC__average           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "mean"       ,  0, CALC__average           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "q0"         ,  0, CALC__minimum           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "q1"         ,  0, CALC__quarter1          , 'f', "v:r"    , "stat"     , ""                                                   },
   { "q2"         ,  0, CALC__average           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "q3"         ,  0, CALC__quarter3          , 'f', "v:r"    , "stat"     , ""                                                   },
   { "q4"         ,  0, CALC__maximum           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "min"        ,  0, CALC__minimum           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "max"        ,  0, CALC__maximum           , 'f', "v:r"    , "stat"     , ""                                                   },
   { "range"      ,  0, CALC__range             , 'f', "v:r"    , "stat"     , ""                                                   },
   { "rangeq"     ,  0, CALC__rangeq            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "median"     ,  0, CALC__median            , 'f', "v:r"    , "stat"     , ""                                                   },
   { "mode"       ,  0, CALC__mode              , 'f', "v:r"    , "stat"     , ""                                                   },
   { "stddev"     ,  0, CALC__stddev            , 'f', "v:r"    , "stat"     , ""                                                   },
   /*---(lookup functions)----------------*/
   { "vlookup"    ,  0, CALC__vlookup           , 'f', "?:rsv"  , "look"     , ""                                                   },
   { "v"          ,  0, CALC__vlookup           , 'f', "?:rsv"  , "look"     , ""                                                   },
   { "hlookup"    ,  0, CALC__hlookup           , 'f', "?:rsv"  , "look"     , ""                                                   },
   { "h"          ,  0, CALC__hlookup           , 'f', "?:rsv"  , "look"     , ""                                                   },
   { "entry"      ,  0, CALC__entry             , 'f', "?:r"    , "look"     , ""                                                   },
   /*---(end-of-funcs)--------------------*/
   { "END"        ,  0, NULL                    , '-', ""       , ""         , ""                                                   },
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
   herror     = NULL;
   terror     = NULL;
   nerror     = 0;
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
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   double      result      = 0;
   char       *resstr      = NULL;
   tCALC      *curr        = NULL;
   char        rce         = -10;
   /*---(defense)------------------------*/
   DEBUG_CALC   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr       == NULL)  {
      DEBUG_CALC   yLOG_note    ("can not calculate a null cell");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_CALC   yLOG_info    ("label"     , a_curr->label);
   DEBUG_CALC   yLOG_char    ("type"      , a_curr->t);
   --rce;  if (strchr (G_CELL_CALC, a_curr->t) == 0) {
      DEBUG_CALC   yLOG_note    ("not a calculated type");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_CALC   yLOG_point   ("calc"      , a_curr->calc);
   --rce;  if (a_curr->calc == NULL) {
      DEBUG_CALC   yLOG_note    ("calculation is null, never built");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_CALC   yLOG_info    ("rpn"       , a_curr->rpn);
   /*---(prep)---------------------------*/
   s_me     = a_curr;
   s_neval  = 0;
   errornum = 0;
   strcpy (errorstr, "");
   /*---(main loop)----------------------*/
   curr        = a_curr->calc;
   while (curr != NULL) {
      ++s_neval;
      DEBUG_CALC   yLOG_complex ("element"   , "typ=%c, val=%F, str=%-9p, ref=%-9p, fnc=%-9p", curr->t, curr->v, curr->s, curr->r, curr->f);
      s_narg = 0;
      switch (curr->t) {
      case 'v' :
         CALC_pushval (__FUNCTION__, curr->v);
         break;
      case 's' :
         CALC_pushstr (__FUNCTION__, curr->s);
         break;
      case 'r' :
         CALC_pushref (__FUNCTION__, curr->r);
         break;
      case 'f' :
         curr->f();
         break;
      case 'x' :
         break;
      default  :
         ERROR_add (s_me, PERR_EVAL, s_neval, __FUNCTION__, TERR_OTHER, "bad calculation type requested");
      }
      if (errornum != 0) break;
      curr = curr->next;
   }
   /*---(check errors)-------------------*/
   if        (errornum <  0) {
      a_curr->t = 'E';
      a_curr->v_str = strndup (errorstr, LEN_RECD);
      DEBUG_CALC   yLOG_value   ("errornum"  , errornum);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   } else if (errornum >  0) {
      a_curr->t = 'E';
      a_curr->v_str = strndup (errorstr, LEN_RECD);
      DEBUG_CALC   yLOG_value   ("errornum"  , errornum);
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(results)------------------------*/
   if (a_curr->rpn[0] == '#') {
      resstr        = CALC__popstr(__FUNCTION__, ++s_narg);
      a_curr->v_str = strndup (resstr, LEN_RECD);
      DEBUG_CALC   yLOG_info    ("v_str"     , a_curr->v_str);
   } else {
      result       = CALC__popval(__FUNCTION__, ++s_narg);
      a_curr->v_num = result;
      DEBUG_CALC   yLOG_value   ("v_num"     , a_curr->v_num);
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
      x_len   = strllen (a_str, LEN_RECD);
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
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        work        [LEN_RECD];       /* working copy of source string  */
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
   DEBUG_CALC   yLOG_point   ("a_cell"    , a_cell);
   --rce;  if (a_cell       == NULL)  {
      DEBUG_CALC   yLOG_note    ("can not calculate a null cell");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_CALC   yLOG_info    ("label"     , a_cell->label);
   DEBUG_CALC   yLOG_char    ("type"      , a_cell->t);
   --rce;  if (strchr (G_CELL_CALC, a_cell->t) == 0) {
      DEBUG_CALC   yLOG_note    ("not a calculated type");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_CALC   yLOG_info    ("rpn"       , a_cell->rpn);
   --rce;  if (a_cell->rpn  == NULL) {
      DEBUG_CALC   yLOG_note    ("rpn is null, so can not build");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(clear calc and deps)------------*/
   CALC_cleanse (a_cell);
   /*---(initialize)-------------------------*/
   strncpy (work, a_cell->rpn + 2, LEN_RECD);
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
         x_calc->s = strndup(p + 1, LEN_RECD);
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
            a_cell->v_str = strndup ("#.badrng", LEN_RECD);
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
            a_cell->v_str = strndup ("#.badptr", LEN_RECD);
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
         /*> if (label [0] == '\0')  strncpy (label, funcs[i].h, 19);                 <*/
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
               a_cell->v_str = strndup ("#.badmal", LEN_RECD);
               DEBUG_CALC   yLOG_exit    (__FUNCTION__);
               return rce - 1;
            }
            rc = DEP_create (G_DEP_REQUIRE, a_cell, dest);
            DEBUG_CALC   yLOG_value   ("rc"        , rc);
            if (rc < 0) {
               DEBUG_CALC   yLOG_info    ("error"     , "dependency can not be created");
               a_cell->t = 'E';
               a_cell->v_str = strndup ("#.baddep", LEN_RECD);
               DEBUG_CALC   yLOG_exit    (__FUNCTION__);
               return rce - 2;
            }
            x_calc->t = 'r';
            x_calc->r = dest;
         } else {
            a_cell->t = 'E';
            a_cell->v_str = strndup ("#.badref", LEN_RECD);
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
         a_cell->v_str = strndup ("#.bigref", LEN_RECD);
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
      a_cell->v_str = strndup ("#.unknown", LEN_RECD);
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

char
CALC_func_func       (char *a_func, char *a_terms, char *a_show)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_func  == NULL)   return rce;
   --rce;  if (a_terms == NULL)   return rce;
   --rce;  if (a_show  == NULL)   return rce;
   /*---(prepare)--------------------------*/
   sprintf (a_show, "?func?");
   x_len = strlen (a_terms);
   /*---(void)-----------------------------*/
   if (x_len == 2) {
      if (strcmp ("v:"    , a_terms) == 0)  sprintf (a_show, "val   %s ()"        , a_func);
      if (strcmp ("s:"    , a_terms) == 0)  sprintf (a_show, "str   %s ()"        , a_func);
      if (strcmp ("a:"    , a_terms) == 0)  sprintf (a_show, "adr   %s ()"        , a_func);
      return 0;
   }
   /*---(unary)----------------------------*/
   if (x_len == 3) {
      if (strcmp ("v:v"   , a_terms) == 0)  sprintf (a_show, "val   %s (x)"        , a_func);
      if (strcmp ("v:s"   , a_terms) == 0)  sprintf (a_show, "val   %s (n)"        , a_func);
      if (strcmp ("v:a"   , a_terms) == 0)  sprintf (a_show, "val   %s (a)"        , a_func);
      if (strcmp ("v:r"   , a_terms) == 0)  sprintf (a_show, "val   %s (a..o)"     , a_func);
      if (strcmp ("s:v"   , a_terms) == 0)  sprintf (a_show, "str   %s (x)"        , a_func);
      if (strcmp ("s:s"   , a_terms) == 0)  sprintf (a_show, "str   %s (n)"        , a_func);
      if (strcmp ("s:a"   , a_terms) == 0)  sprintf (a_show, "str   %s (a)"        , a_func);
      if (strcmp ("t:v"   , a_terms) == 0)  sprintf (a_show, "T/F   %s (x)"        , a_func);
      if (strcmp ("t:a"   , a_terms) == 0)  sprintf (a_show, "T/F   %s (a)"        , a_func);
      if (strcmp ("?:v"   , a_terms) == 0)  sprintf (a_show, "???   %s (x)"        , a_func);
      if (strcmp ("?:r"   , a_terms) == 0)  sprintf (a_show, "???   %s (a..o)"     , a_func);
      return 0;
   }
   /*---(binary)---------------------------*/
   if (x_len == 4) {
      if (strcmp ("v:vv"  , a_terms) == 0)  sprintf (a_show, "val   %s (x, y)"      , a_func);
      if (strcmp ("s:ss"  , a_terms) == 0)  sprintf (a_show, "str   %s (n, m)"      , a_func);
      if (strcmp ("s:sv"  , a_terms) == 0)  sprintf (a_show, "str   %s (n, x)"      , a_func);
      if (strcmp ("s:av"  , a_terms) == 0)  sprintf (a_show, "str   %s (a, x)"      , a_func);
   }
   /*---(terciary)-------------------------*/
   if (x_len == 5) {
      if (strcmp ("v:vvv" , a_terms) == 0)  sprintf (a_show, "val   %s (x, y, z)"    , a_func);
      if (strcmp ("s:vss" , a_terms) == 0)  sprintf (a_show, "str   %s (x, n, m)"    , a_func);
      if (strcmp ("s:svv" , a_terms) == 0)  sprintf (a_show, "str   %s (n, x, y)"    , a_func);
      if (strcmp ("r:vvv" , a_terms) == 0)  sprintf (a_show, "adr   %s (x, y, z)"    , a_func);
      if (strcmp ("?:rsv" , a_terms) == 0)  sprintf (a_show, "???   %s (a..o, n, x)" , a_func);
   }
   /*---(quadinary)------------------------*/
   if (x_len == 6) {
      if (strcmp ("s:sssv", a_terms) == 0)  sprintf (a_show, "str   %s (n, m, o, x)"  , a_func);
   }
   /*---(complete)-------------------------*/
   return 0;
}

char
CALC_func_op         (char *a_op, char *a_terms, char *a_show)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_op    == NULL)   return rce;
   --rce;  if (a_terms == NULL)   return rce;
   --rce;  if (a_show  == NULL)   return rce;
   /*---(prepare)--------------------------*/
   sprintf (a_show, "?op?");
   x_len = strlen (a_terms);
   /*---(exceptions)-----------------------*/
   if (x_len == 3 && strcmp (a_op, "-:") == 0) {
      strcpy (a_show, "val   -x");
      return 0;
   }
   /*---(unary)----------------------------*/
   if (x_len == 3) {
      if (a_terms [0] == 'v')  sprintf (a_show, "val   %sx", a_op);
      if (a_terms [0] == 't')  sprintf (a_show, "T/F   %sx", a_op);
      return 0;
   }
   /*---(binary values)--------------------*/
   if (x_len == 4 && a_terms [0] == 'v') {
      sprintf (a_show, "val   x %s y", a_op);
      return 0;
   }
   /*---(binary logic)---------------------*/
   if (x_len == 4 && a_terms [0] == 't') {
      if (a_terms [2] == 'v')  sprintf (a_show, "T/F   x %s y", a_op);
      if (a_terms [2] == 's')  sprintf (a_show, "T/F   n %s m", a_op);
      return 0;
   }
   /*---(binary string)--------------------*/
   if (x_len == 4 && a_terms [0] == 's') {
      sprintf (a_show, "str   n %s m", a_op);
      return 0;
   }
   /*---(complete)-------------------------*/
   return 0;
}

char
CALC_func_list       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_save      [10] = "";
   int         x_len       = 0;
   char        x_terms     [30] = "";
   char       *x_title     = "   ---name---   ret   ---example-----------  ---description------------------------------------";
   /*---(header)-------------------------*/                       /*  (a1..a2,s,v)  */
   printf ("gyges_hekatonkheires - cell formula operators and functions\n");
   printf ("\n%s\n", x_title);
   for (i = 0; i < MAX_FUNCS; ++i) {
      if (funcs [i].n [0] == 'E')   break;
      if (strcmp (funcs [i].cat, x_save) != 0)   printf ("\n   %s\n", funcs [i].cat);
      x_len = strlen (funcs [i].terms);
      if (funcs [i].type == 'o') CALC_func_op   (funcs [i].n, funcs [i].terms, x_terms);
      else                       CALC_func_func (funcs [i].n, funcs [i].terms, x_terms);
      printf ("   %-10.10s   %-25.25s  %-40.40s\n",
            funcs [i].n    , x_terms   , funcs [i].desc );
      strcpy (x_save, funcs [i].cat);
   }
   exit (1);
}


/*============================----end-of-source---============================*/
