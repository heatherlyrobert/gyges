/*============================----beg-of-source---============================*/

/*===[[ MODULE ]]=============================================================*

 *   application   : s
 *   module        : s_dep
 *   purpose       : provide all code for calculating with a dependency graph
 *
 */
/*===[[ EXPLANATION ]]========================================================*/

/*   situation (S)
 *
 *      spreadsheets, as they should be used, are about interactive and
 *      transparent analysis which means lots and lots of interlinked,
 *      changing, and complex calculation.
 *
 *   complication (C)
 *
 *      there are several theories on how spreadsheets can optimize the
 *      trade-off of simplicity versus speed.  small spreadsheets typically
 *      just recalculate everything, larger ones use targeted recalculation.
 *
 *
 *   question (Q)
 *
 *      for gyges, what is the best method?
 *
 *   answer (A)
 *
 *      dependency graph guided calculation to optimize resources
 *
 *   objectives (o)
 *      a) highest learning potential solution
 *      b) ability to leverage methods in other situations
 *      c) reliably correct cell values at all times
 *      d) fast and efficient calculation so i can support massive analysis
 *      e) long-term usage of gyges to support my own personal needs
 *      f) clear data structure allowing reliable unit testing
 *
 *   alternatives (a)
 *
 *      as i understand it, there are three main theories related to
 *      calculation in spreadsheets.  these different models clearly apply
 *      to a wide variety of situations well outside spreadsheets also.
 *
 *      1) the first is wholesale recalculation using some path, such
 *      as, top-left to bottom-right.
 *         ++ this is considered the simpliest and clearest
 *         ++ no new information must be maintained to accomplish it
 *         ++ programming is dead simple
 *         -- it can leave some cells out of sync/wrong values
 *         -- it must be done multiple times to ensure correct values
 *         -- is can be resource expensive on larger spreadsheets
 *         -- it can be much, much slower as it recalcs already correct values
 *
 *      2) the second is connecting cells in a graph structure so when a cell is
 *      changed, the cells requiring recaculation are clear.
 *         -- new data structures are required
 *         -- this is a whole lot harder to program
 *         ++ cell values are always correct and up to date
 *         ++ it is very quick as all recalculation is targeted
 *         ++ it has much lower pressure on the cpu
 *
 *      3) a hybrid approach that can make option two simplier or more
 *      efficient.
 *         -- likely a one off application for spreadsheets
 *         -- more complex programming
 *         ++ perhaps more efficient
 *         ++ perhaps quicker
 *
 *   tradeoffs
 *      ---alternatives-------- -a- -b- -c- -d- -e- -f-
 *      1) wholesale recalc      -   -   -   -   -   X
 *      2) dependency graph      X   X   X   X   X   X
 *      3) optimized hybrid      X   -   X   X   X   -
 *
 *   decision
 *
 *      i feel that a dependency graph solution is reusable in many
 *      environments and that this is the perfect opportunity to learn this
 *      method in a challenging but clear end-use scenario.
*       
*/

/*
 *
 *   on the implementation side, there are many ways to create graph structures
 *   depending on the memory and performance expectations.  in this case, i am
 *   concerned about clarity and performance so i am chosing a method that is
 *   a little more redundant, but solid.
 *
 *   i am going to store two links for every dependency.  a "requires" link
 *   which drives the recalculation and a "provides" link which allows a cell
 *   to know which other cells depend on it.  i could combine them, but that
 *   would cause a more complicated linking environment.  i could also skip the
 *   "provides" links, but i use them for rapid on-screen display of debugging
 *   information and coloration.  bear with me and you can redesign it to your
 *   tastes afterward.
 *
 *   since the data structure will be tied at each cell, the only time having
 *   additional links will be costly is at file loads when many cells are
 *   involved.  in my way of thinking, i'd rather the load be a little slower
 *   if the real-time usage is faster.
 *
 *
 *
 */




/*---[[ global header ]]----------------------------------*/
#include   "gyges.h"

struct cDEP  {
   /*---(#1, dependency)-----------------*/
   char      type;            /* type of connection                           */
   tCELL    *source;          /* pointer to source cell                       */
   tCELL    *target;          /* pointer to target cell                       */
   /*---(#2, cell linked list)-----------*/
   tDEP     *prev;            /* pointer to prev dependency for source cell   */
   tDEP     *next;            /* pointer to next dependency for source cell   */
   tDEP     *match;           /* pointer to matching dependency in ther dir  */
   /*---(#3, deps linked list)-----------*/
   tDEP     *dprev;           /* pointer to prev dependency in full list      */
   tDEP     *dnext;           /* pointer to next dependency in full list      */
   /*---(#4, statistics)-----------------*/
   int       count;           /* number of times used for dep/calc            */
};
tDEP     *dhead;
tDEP     *dtail;
tCELL    *dtree;
int       ndep;



#define   MAX_DEPTYPE     30
struct cDEP_INFO {
   char        type;                   /* connection type                     */
   char        match;                  /* matching connect type               */
   char        dir;                    /* direction (require vs provide)      */
   char        desc        [50];       /* description of dependency type      */
   char        match_index;            /* index of matching type              */
   int         count;                  /* current count of type               */
   int         total;                  /* total of type ever created          */
} s_dep_info [MAX_DEPTYPE] = {
   /*-ty- -ma- -dir ---description---------------------------------------- idx -cnt- -tot- */

   {  'R', 'p', '-', "requires another cell for its value"                , 0 ,    0,    0 },
   {  'p', 'R', '+', "provides its value to another cell"                 , 0 ,    0,    0 },

   {  'P', 'c', '-', "range pointer that provides dependency shortcut"    , 0 ,    0,    0 },
   {  'c', 'P', '+', "individual cell that makes up a range pointer"      , 0 ,    0,    0 },

   {  'F', 'f', '-', "format master cell providing format template"       , 0 ,    0,    0 },
   {  'f', 'F', '+', "individual cell following the a format template"    , 0 ,    0,    0 },

   {  'S', 'l', '-', "source formula master other cell follow"            , 0 ,    0,    0 },
   {  'l', 'S', '+', "follows a source formula with ref adjustments"      , 0 ,    0,    0 },

   {  'M', 'e', '-', "provides contents for set of merged cells"          , 0 ,    0,    0 },
   {  'e', 'M', '+', "provides extra/empty space to display contents"     , 0 ,    0,    0 },

   {  'A', 'a', '-', "contains a calculated/runtime reference function"   , 0 ,    0,    0 },
   {  'a', 'A', '+', "provides its value to a calculated reference"       , 0 ,    0,    0 },

   {  '-', '-', ' ', "newly created dependency, not yet assigned"         , 0 ,    0,    0 },

};
#define     DEP_DIRREQ    '-'
#define     DEP_DIRPRO    '+'

static char S_DEP_REQS [10] = "";
static char S_DEP_PROS [10] = "";



/*====================------------------------------------====================*/
/*===----                   program setup and teardown                 ----===*/
/*====================------------------------------------====================*/
PRIV void  o___PROG____________o () { return; }

char       /*----: prepare dependency capability for use ---------------------*/
DEP_init           (void)
{
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   int         j           = 0;
   int         x_count     = 0;
   int         x_found     = 0;
   int         x_reqs      = 0;
   int         x_pros      = 0;
   int         t           [5];
   /*---(setup)--------------------------*/
   DEP_purge  ();
   CELL_dtree ("new");
   --rce;  if (dtree == NULL)  return rce;
   strcpy (dtree->label, "root");
   /*---(initialize)---------------------*/
   dhead  = NULL;
   dtail  = NULL;
   ndep   = 0;
   strlcpy (S_DEP_REQS, "", 10);
   strlcpy (S_DEP_PROS, "", 10);
   /*---(complete info table)------------*/
   --rce;
   for (i = 0; i < MAX_DEPTYPE; ++i) {
      DEBUG_DEPS   yLOG_char    ("type"      , s_dep_info [i].type);
      /*---(check for end)---------------*/
      if (s_dep_info [i].type == DEP_BLANK)  break;
      /*---(add to lists)----------------*/
      sprintf (t, "%c", s_dep_info [i].type);
      DEBUG_DEPS   yLOG_info    ("str type"  , t);
      DEBUG_DEPS   yLOG_char    ("dir"       , s_dep_info [i].dir);
      if      (s_dep_info [i].dir  == DEP_DIRREQ)  strcat (S_DEP_REQS, t);
      else if (s_dep_info [i].dir  == DEP_DIRPRO)  strcat (S_DEP_PROS, t);
      else {
         DEBUG_DEPS   yLOG_note    ("type direction not + or -");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      /*---(find match)------------------*/
      ++x_count;
      for (j = 0; j < MAX_DEPTYPE; ++j) {
         if (s_dep_info [j].type != s_dep_info [i].match) continue;
         ++x_found;
         s_dep_info [i].match_index = j;
         DEBUG_DEPS   yLOG_char    ("match"     , s_dep_info [j].type);
         DEBUG_DEPS   yLOG_value   ("match_indx", s_dep_info [i].match_index);
      }
   }
   DEBUG_DEPS   yLOG_value   ("x_count"   , x_count);
   DEBUG_DEPS   yLOG_value   ("x_found"   , x_found);
   --rce;  if (x_found != x_count) {
      DEBUG_DEPS   yLOG_note    ("could not match all dep types");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(report out)---------------------*/
   DEBUG_DEPS   yLOG_info    ("S_DEP_REQS", S_DEP_REQS);
   DEBUG_DEPS   yLOG_info    ("S_DEP_PROS", S_DEP_PROS);
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: destroy all dependency objects ----------------------------*/
DEP_purge          (void)
{
   /*---(beginning)----------------------*/
   /*> printf ("DEP_purge\n");                                                        <*/
   /*> DEBUG_CELL  printf("DEP_purge      :: begin\n");                               <*/
   /*---(locals)-------*-----------------*/
   tDEP     *curr      = NULL;
   tDEP     *next      = NULL;
   /*---(walk through list)--------------*/
   /*> printf ("DEP_purge : defenses\n");                                             <*/
   if (dhead == NULL) return -1;
   next = dhead;
   /*> printf ("DEP_purge : entering loop\n");                                        <*/
   while (next != NULL) {
      curr = next;
      /*> printf ("DEP_purge : pass with %9p\n", curr);                               <*/
      next = curr->dnext;
      DEP__free (curr);
   }
   /*---(clean ends)---------------------*/
   dhead = NULL;
   dtail = NULL;
   /*---(ending)-------------------------*/
   /*> DEBUG_CELL  printf("DEP_purge      :: end\n");                                 <*/
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: prepare dependency capability for use ---------------------*/
DEP_wrap           (void)
{
   /*---(dependencies)-------------------*/
   DEP_purge();
   CELL_dtree ("free");
   dhead  = NULL;
   dtail  = NULL;
   ndep   = 0;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        lookup functions                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___LOOKUP__________o () { return; }

char         /*--> find the index of a dep type ----------[ leaf   [ ------ ]-*/
DEP_index          (char a_type)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(search)-------------------------*/
   for (i = 0; i < MAX_DEPTYPE; ++i) {
      if (s_dep_info [i].type != a_type) continue;
      return i;
   }
   /*---(failure)------------------------*/
   return -1;
}

char         /*--> find the index of a dep match type-----[ leaf   [ ------ ]-*/
DEP_match          (char a_type)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(search)-------------------------*/
   for (i = 0; i < MAX_DEPTYPE; ++i) {
      if (s_dep_info [i].type != a_type) continue;
      return s_dep_info [i].match_index;
   }
   /*---(failure)------------------------*/
   return -1;
}



/*====================------------------------------------====================*/
/*===----                      one-way dependency                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___ONE_WAY_________o () { return; }

tDEP*        /*--> create a new, blank dependency --------[ leaf   [ ------ ]-*/
DEP__new           (void)
{  /*---(design notes)--------------------------------------------------------*/
   /* creates a new dependency object which is completely blank except for    */
   /* being linked to the dependency doubly-linked list which allows a        */
   /* single point of control over all dependencies regardless of type or     */
   /* the cell it is associated with.                                         */
   /*---(locals)-----------+-----------+-*/
   tDEP       *curr        = NULL;
   int         tries       = 0;
   /*---(create)-------------------------*/
   while (curr == NULL && tries < 10) {
      curr = (tDEP *) malloc (sizeof (tDEP));
      ++tries;
   }
   if (curr == NULL)    return NULL;
   /*---(dependency fields)--------------*/
   curr->type    = DEP_BLANK;
   curr->source  = NULL;
   curr->target  = NULL;
   /*---(cell fields)--------------------*/
   curr->prev    = NULL;
   curr->next    = NULL;
   curr->match   = NULL;
   /*---(dependency doubly-linked list)--*/
   curr->dprev   = NULL;
   curr->dnext   = NULL;
   if (dtail == NULL) {
      dhead        = curr;
      dtail        = curr;
   } else {
      curr->dprev     = dtail;
      curr->dnext     = NULL;
      dtail->dnext = curr;
      dtail        = curr;
   }
   ++ndep;
   /*---(statistics)---------------------*/
   curr->count   = 0;
   /*---(complete)-----------------------*/
   return curr;
}

tDEP*        /*--> destroy a single dependency -----------[ leaf   [ ------ ]-*/
DEP__free          (
      /*----------+-----------+-----------------------------------------------*/
      tDEP       *a_dep)
{  /*---(design notes)--------------------------------------------------------*/
   /* destroys a single dependency object and removes from the associated     */
   /* cells and also from the paired dependency.                              */
   /*---(defense: null cell)-------------*/
   if (a_dep       == NULL)  return NULL;
   /*---(remove from dependency list)----*/
   if (a_dep->dnext != NULL) a_dep->dnext->dprev         = a_dep->dprev;
   else                      dtail                       = a_dep->dprev;
   if (a_dep->dprev != NULL) a_dep->dprev->dnext         = a_dep->dnext;
   else                      dhead                       = a_dep->dnext;
   --ndep;
   /*---(detatch from paired dep)--------*/
   if (a_dep->match != NULL) {
      a_dep->match->match  = NULL;
      a_dep->match         = NULL;
   }
   /*---(if require, take off cell)------*/
   if      (strchr (S_DEP_REQS, a_dep->type) != NULL) {
      if (a_dep->next  != NULL) a_dep->next->prev        = a_dep->prev;
      if (a_dep->prev  != NULL) a_dep->prev->next        = a_dep->next;
      else                      a_dep->source->requires  = a_dep->next;
      --(a_dep->source->nrequire);
   }
   /*---(if provide, take off cell)------*/
   else if (strchr (S_DEP_PROS, a_dep->type) != NULL) {
      if (a_dep->next  != NULL) a_dep->next->prev        = a_dep->prev;
      if (a_dep->prev  != NULL) a_dep->prev->next        = a_dep->next;
      else                      a_dep->source->provides  = a_dep->next;
      --(a_dep->source->nprovide);
   }
   /*---(free)---------------------------*/
   free (a_dep);
   a_dep = NULL;
   /*---(complete)-----------------------*/
   return  NULL;
}



/*====================------------------------------------====================*/
/*===----                      two-way dependency                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___TWO_WAY_________o () { return; }

char         /*--> create a two-way dependency -----------[ ------ [ ------ ]-*/
DEP_create         (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* type of dependency (source, format, or calc) */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{  /*---(design notes)--------------------------------------------------------*/
   /* creates a two-way dependency so that we can quickly process cell        */
   /* interactions, including, calculations, deletions, etc.  it is critical  */
   /* that these additions maintain a well-formed graph structure and do not  */
   /* introduce loops or cycles.                                              */
   /*---(header)-------------------------*//*---------------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;    /* return code for errors               */
   tDEP       *next        = NULL;    /* iterator -- dependency lists         */
   tDEP       *require     = NULL;    /* new requires entry                   */
   tDEP       *provide     = NULL;    /* new provides entry                   */
   char        sroot       =  'n';    /* flag if source root tie added        */
   char        troot       =  'n';    /* flag if target root tie deleted      */
   char        rc          =    0;
   char        tries       =    0;
   char        x_index     =    0;
   /*---(defense: type)------------------*/
   DEBUG_DEPS   yLOG_char    ("a_type"    , a_type);
   x_index = DEP_index (a_type);
   DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_DEPS   yLOG_note    ("dependency type not found");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source     == NULL)   {
      DEBUG_DEPS yLOG_value ("FAILED", rce);
      DEBUG_DEPS yLOG_exit (__FUNCTION__);
      return rce; 
   }
   DEBUG_DEPS   yLOG_point   ("a_target"  , a_target);
   --rce;  if (a_target     == NULL)   {
      DEBUG_DEPS yLOG_value ("FAILED", rce);
      DEBUG_DEPS yLOG_exit (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_complex ("connect"   , "<%s> and <%s>", a_source->label, a_target->label);
   /*---(defense: circular ref)----------*/
   --rce;  if (a_source == a_target) {
      DEBUG_DEPS   yLOG_note    ("both source and target are equal");
      DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   /*---(check if source needs rooting)--*/
   DEBUG_DEPS   yLOG_info    ("ROOTING"   , "check if source needs to be routed");
   --rce;  if (a_source->provides == NULL && a_source != dtree) {
      DEBUG_DEPS   yLOG_note    ("create a dependence from root to source");
      rc = DEP_create (DEP_REQUIRE, dtree, a_source);
      DEBUG_DEPS   yLOG_value   ("rc"        , rc);
      if (rc != 0) {
         DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      sroot = 'y';  /* flag new tie to dtree/root */
   } else {
      DEBUG_DEPS   yLOG_note    ("looking at root, done");
   }
   /*---(check for existing requires)----*/
   DEBUG_DEPS   yLOG_info    ("REQUIRES"  , "check if requires already exists on source");
   DEBUG_DEPS   yLOG_value   ("nrequire"  , a_source->nrequire);
   next = a_source->requires;
   DEBUG_DEPS   yLOG_complex ("reqs head" , "entry %2d contains pointer %p", tries, next);
   while (next != NULL) {
      DEBUG_DEPS   yLOG_complex ("ties to"   , "%-8.8s at %p", next->target->label, next->target);
      if (next->target == a_target)  break;
      next = next->next;
      ++tries;
      DEBUG_DEPS   yLOG_complex ("reqs next" , "entry %2d contains pointer %p", tries, next);
   }
   /*---(assign requires)----------------*/
   if (next != NULL && next->target == a_target) {
      DEBUG_DEPS   yLOG_note    ("found existing requires");
      require = next;
   } else {
      DEBUG_DEPS   yLOG_note    ("create new requires");
      /*---(first link)------------------*/
      require         = DEP__new ();
      require->type   = a_type;
      require->source = a_source;
      require->target = a_target;
      /*---(add to dep counters)---------*/
      ++(s_dep_info [x_index].count);
      ++(s_dep_info [x_index].total);
      /*---(hook it up to cell)----------*/
      if (a_source->requires == NULL) {
         a_source->requires = require;
      } else {
         next = a_source->requires;
         while (next->next != NULL)  next = next->next;
         next->next         = require;
         require->prev      = next;
      }
      /*---(incement counter)------------*/
      ++a_source->nrequire;
   }
   /*---(check for existing provides)----*/
   DEBUG_DEPS   yLOG_info    ("PROVIDES"  , "check if provides already exists on target");
   DEBUG_DEPS   yLOG_value   ("nprovides" , a_target->nprovide);
   next = a_target->provides;
   tries = 0;
   DEBUG_DEPS   yLOG_complex ("pros head" , "entry %2d contains pointer %p", tries, next);
   while (next != NULL) {
      DEBUG_DEPS   yLOG_point   ("ties to"   , next->target);
      if (next->target == a_source)  break;
      next = next->next;
      ++tries;
      DEBUG_DEPS   yLOG_complex ("pros next" , "entry %2d contains pointer %p", tries, next);
   }
   /*---(assign provides)----------------*/
   if (next != NULL && next->target == a_source) {
      DEBUG_DEPS   yLOG_note    ("found existing provides");
      provide = next;
   } else {
      DEBUG_DEPS   yLOG_note    ("create new provides");
      /*---(second link)-----------------*/
      provide         = DEP__new ();
      x_index =  DEP_match (a_type);
      DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
      --rce;  if (x_index < 0) {
         DEBUG_DEPS   yLOG_note    ("dependency type not found");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      DEBUG_DEPS   yLOG_char    ("match type", s_dep_info [x_index].type);
      provide->type = s_dep_info [x_index].type;
      /*---(add to dep totals)-----------*/
      ++(s_dep_info [x_index].count);
      ++(s_dep_info [x_index].total);
      /*---(assign cells)----------------*/
      provide->source = a_target;
      provide->target = a_source;
      /*---(hook it up to cell)----------*/
      if (a_target->provides == NULL) {
         a_target->provides = provide;
      } else {
         next = a_target->provides;
         while (next->next != NULL) next = next->next;
         next->next         = provide;
         provide->prev      = next;
      }
      /*---(incement counter)------------*/
      ++a_target->nprovide;
   }
   /*---(cross link them)----------------*/
   DEBUG_DEPS   yLOG_info    ("LINKED"    , "link require and provide together");
   require->match  = provide;
   provide->match  = require;
   /*---(check if target needs unroot)---*/
   if (a_source != dtree) {
      DEBUG_DEPS   yLOG_info    ("UNROOTING" , "check if target connected to root");
      DEBUG_DEPS   yLOG_value   ("nprovide"  , a_target->nprovide);
      --rce;
      next = a_target->provides;
      tries = 0;
      DEBUG_DEPS   yLOG_complex ("pros head" , "entry %2d contains pointer %p", tries, next);
      while (next != NULL) {
         DEBUG_DEPS   yLOG_point   ("ties to"   , next->target);
         if (next->target == dtree) {
            DEBUG_DEPS   yLOG_note    ("found, remove a dependence from root to target");
            rc = DEP_delete (DEP_REQUIRE, dtree, a_target);
            DEBUG_DEPS   yLOG_value   ("rc"        , rc);
            if (rc != 0) {
               DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
               DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
               return rce;
            }
            troot = 'y';  /* flag deleted tie to dtree/root */
         }
         next = next->next;
         ++tries;
         DEBUG_DEPS   yLOG_complex ("pros next" , "entry %2d contains pointer %p", tries, next);
      }
   }
   /*---(check for loops)----------------*/
   DEBUG_DEPS   yLOG_info    ("LOOPS"     , "check if dep tree has loops/cycles");
   --rce;
   DEBUG_DEPS   yLOG_note    ("run DEP_check");
   rc = DEP_checkall ('n');
   DEBUG_DEPS   yLOG_value   ("rc"        , rc);
   if (rc < 0) {
      DEBUG_DEPS   yLOG_note    ("run DEP_delete");
      rc = DEP_delete (a_type, a_source, a_target);
      DEBUG_DEPS   yLOG_value   ("rc"        , rc);
      if (rc != 0) {
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      DEBUG_DEPS   yLOG_char    ("troot"     , troot);
      if (troot == 'y') {
         DEBUG_DEPS   yLOG_note    ("run DEP_create");
         rc = DEP_create (DEP_REQUIRE, dtree, a_target);
         DEBUG_DEPS   yLOG_value   ("rc"        , rc);
         if (rc != 0) {
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce - 1;
         }
      }
      DEBUG_DEPS   yLOG_char    ("sroot"     , sroot);
      if (sroot == 'y') {
         DEBUG_DEPS   yLOG_note    ("run DEP_delete");
         rc = DEP_delete (DEP_REQUIRE, dtree, a_source);
         DEBUG_DEPS   yLOG_value   ("rc"        , rc);
         if (rc != 0) {
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce - 2;
         }
      }
      DEBUG_DEPS   yLOG_value   ("returning" , rce - 3);
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce - 3;
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_note    ("all actions complete");
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> remove source of merge ----------------[ ------ [ ------ ]-*/
DEP_delmergeroot   (tCELL *a_target)
{
   /*---(header)-------------------------*//*---------------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_source    = NULL;
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_info    ("DEFENSES"  , "make sure this is processable");
   DEBUG_DEPS   yLOG_point   ("target"    , a_target);
   --rce;  if (a_target     == NULL)   {
      DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_info    ("label"     , a_target->label);
   x_source = LOC_cell (a_target->tab, a_target->col + 1, a_target->row);
   --rce;  if (x_source    == NULL       )  return rce;
   --rce;  if (x_source->t != CTYPE_MERGE)  return rce;
   DEP_delmerge (x_source);
   return 0;
}

tCELL*       /*--> remove a merged cell dependency -------[ ------ [ ------ ]-*/
DEP_delmerge       (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_source)   /* cell with the calculation                    */
{  /*---(design notes)--------------------------------------------------------*/
   /*---(header)-------------------------*//*---------------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   tDEP       *x_next      = NULL;
   char        rc          =    0;
   int         i           = 0;
   tCELL      *x_target    = NULL;
   tCELL      *x_testing   = NULL;
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_info    ("DEFENSES"  , "make sure this is processable");
   DEBUG_DEPS   yLOG_point   ("source"    , a_source);
   --rce;  if (a_source     == NULL)   {
      DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   DEBUG_DEPS   yLOG_info    ("label"     , a_source->label);
   /*---(find merge source)--------------*/
   --rce;
   x_next = a_source->provides;
   DEBUG_DEPS   yLOG_point   ("next"      , x_next);
   while (x_next != NULL) {
      DEBUG_DEPS   yLOG_info    ("target"    , x_next->target->label);
      DEBUG_DEPS   yLOG_char    ("type"      , x_next->type);
      if (x_next->type != DEP_EMPTY) {
         DEBUG_DEPS   yLOG_note    ("wrong type, skipping");
         x_next = x_next->next;
         DEBUG_DEPS   yLOG_point   ("next"      , x_next);
         continue;
      }
      DEBUG_DEPS   yLOG_note    ("FOUND TARGET");
      x_target = x_next->target;
      break;
   }
   if (x_target == NULL)  return NULL;
   /*---(delete ref and to right)--------*/
   for (i = a_source->col; i < NCOL; ++i) {
      DEBUG_DEPS   yLOG_value   ("check col" , i);
      x_testing = LOC_cell (a_source->tab, i, a_source->row);
      DEBUG_DEPS   yLOG_point   ("cell"      , x_testing);
      if (x_testing            == NULL       )  break;
      DEBUG_DEPS   yLOG_info    ("label"     , x_testing->label);
      DEBUG_DEPS   yLOG_char    ("type"      , x_testing->t);
      if (x_testing->t != CTYPE_MERGE)  break;
      DEBUG_DEPS   yLOG_note    ("wack connection");
      DEP_delete (DEP_MERGED, x_target, x_testing);
      x_testing->t = 's';
      CELL_printable (x_testing);
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return x_target;
}

char         /*--> remove a two-way dependency -----------[ ------ [ ------ ]-*/
DEP_delcalcref     (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_source)   /* cell with the calculation                    */
{  /*---(design notes)--------------------------------------------------------*/
   /*---(header)-------------------------*//*---------------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   tDEP       *next        = NULL;
   char        rc          =    0;
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_info    ("DEFENSES"  , "make sure this is processable");
   DEBUG_DEPS   yLOG_point   ("source"    , a_source);
   --rce;  if (a_source     == NULL)   {
      DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_info    ("source"    , a_source->label);
   /*---(review calculated deps)---------*/
   --rce;
   next = a_source->requires;
   while (next != NULL) {
      DEBUG_DEPS   yLOG_info    ("target"    , next->target->label);
      DEBUG_DEPS   yLOG_char    ("type"      , next->type);
      if (next->type != DEP_CALCREF) {
         DEBUG_DEPS   yLOG_note    ("wrong type, skipping");
         next = next->next;
         continue;
      }
      DEP_delete (DEP_CALCREF, a_source, next->target);
      next = next->next;
   }
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> remove a two-way dependency -----------[ ------ [ ------ ]-*/
DEP_delete         (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* dependency type                              */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{  /*---(design notes)--------------------------------------------------------*/
   /* removes a two-way dependency so that the dependency tree remains        */
   /* accurate and unneeded cells are removed.  it is important that these    */
   /* entries remain in a well-formed dependency graph so all necessary cells */
   /* are calculated at the right times.                                      */
   /*---(header)-------------------------*//*---------------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   char        x_debug     =  'y';
   static char x_level     =    0;
   char        x_indent    [100];
   char        i           =    0;
   tDEP       *next        = NULL;
   tDEP       *save        = NULL;
   char        rce         =  -10;
   char        rc          =    0;
   char        tries       =    0;
   char        found       =    0;
   char        x_index     =    0;
   /*---(defense: type)------------------*/
   DEBUG_DEPS   yLOG_char    ("a_type"    , a_type);
   x_index = DEP_index (a_type);
   DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
   --rce;  if (x_index < 0) {
      DEBUG_DEPS   yLOG_note    ("dependency type not found");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_info    ("DEFENSES"  , "make sure this is processable");
   DEBUG_DEPS   yLOG_point   ("source"    , a_source);
   --rce;  if (a_source     == NULL)   { DEBUG_DEPS yLOG_value ("FAILED", rce); DEBUG_DEPS yLOG_exit (__FUNCTION__); return rce; }
   DEBUG_DEPS   yLOG_point   ("target"    , a_target);
   --rce;  if (a_target     == NULL)   { DEBUG_DEPS yLOG_value ("FAILED", rce); DEBUG_DEPS yLOG_exit (__FUNCTION__); return rce; }
   DEBUG_DEPS   yLOG_complex ("focus"     , "review connection between <%s> and <%s>", a_source->label, a_target->label);
   /*---(review requires deps)-----------*/
   DEBUG_DEPS   yLOG_info    ("REQUIRES"  , "review requires type dependencies");
   DEBUG_DEPS   yLOG_value   ("nrequires" , a_source->nrequire);
   --rce;
   next = a_source->requires;
   DEBUG_DEPS   yLOG_complex ("reqs head" , "entry %2d contains pointer %p", tries, next);
   while (next != NULL) {
      save = next->next;
      DEBUG_DEPS   yLOG_complex ("  cell"    , "and that is at %s", next->target->label);
      if (next->target == a_target) {
         DEBUG_DEPS   yLOG_note    ("  HIT, that matches the target");
         if (next->match == NULL)  {
            DEBUG_DEPS   yLOG_note    ("  FAILED, two connections do not match");
            DEBUG_DEPS   yLOG_value   ("  FAILED"  , rce);
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_note    ("  both directions match");
         DEBUG_DEPS   yLOG_complex ("  free A"  , "connection from <%s> to <%s>", a_source->label, a_target->label);
         rc = DEP__free (next->match);
         DEBUG_DEPS   yLOG_value   ("  rc"      , rc);
         if (rc != 0)  {
            DEBUG_DEPS   yLOG_value   ("  FAILED"  , rce);
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce + 1;
         }
         /*---(find type)------------------*/
         x_index =  DEP_match (a_type);
         DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
         --rce;  if (x_index < 0) {
            DEBUG_DEPS   yLOG_note    ("dependency type not found");
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_char    ("match type", s_dep_info [x_index].type);
         /*---(add to dep totals)-----------*/
         --(s_dep_info [x_index].count);
         /*---(handlen original)------------*/
         DEBUG_DEPS   yLOG_complex ("  free B"  , "connection from <%s> to <%s>", next->target->label, a_source->label);
         rc = DEP__free (next);
         DEBUG_DEPS   yLOG_value   ("  rc"      , rc);
         if (rc != 0)  {
            DEBUG_DEPS   yLOG_value   ("  FAILED"  , rce);
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce + 2;
         }
         /*---(find type)------------------*/
         x_index =  DEP_index (a_type);
         DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
         --rce;  if (x_index < 0) {
            DEBUG_DEPS   yLOG_note    ("dependency type not found");
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_char    ("match type", s_dep_info [x_index].type);
         /*---(add to dep counters)---------*/
         --(s_dep_info [x_index].count);
         /*---(add to total)----------------*/
         ++found;
      } else {
         DEBUG_DEPS   yLOG_note    ("  SKIP, does not match the target");
      }
      next = save;
      ++tries;
      DEBUG_DEPS   yLOG_complex ("reqs next" , "entry %2d contains pointer %p", tries, next);
   }
   rce -= 2;
   /*---(if nothing found)---------------*/
   DEBUG_DEPS   yLOG_value   ("total reqs", found);
   --rce;  if (found == 0)  { 
      DEBUG_DEPS   yLOG_note    ("none found, nothing more to do");
      DEBUG_DEPS   yLOG_value   ("EXIT"      , rce);
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check on rooting target)--------*/
   DEBUG_DEPS   yLOG_note    ("ROOTING: check on target needing rooting");
   DEBUG_DEPS   yLOG_value   ("nrequire"  , a_target->nrequire);
   DEBUG_DEPS   yLOG_value   ("nprovide"  , a_target->nprovide);
   --rce;
   if (a_target->nprovide == 0 && a_target->nrequire > 0) {
      DEBUG_DEPS   yLOG_note    ("no one owns, create a connection from root to target");
      rc = DEP_create (DEP_REQUIRE, dtree, a_target);
      DEBUG_DEPS   yLOG_value   ("rc"        , rc);
      if (rc != 0) {
         DEBUG_DEPS   yLOG_value   ("EXIT"      , rce);
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      DEBUG_DEPS   yLOG_note    ("success, target rooted");
   }
   /*---(check on deleting target)-------*/
   DEBUG_DEPS   yLOG_note    ("check on target empty and needing deletion");
   DEBUG_DEPS   yLOG_char    ("targ type" , a_target->t);
   --rce;
   if (a_target->nprovide == 0 && a_target->nrequire == 0 && a_target->t == '-') {
      DEBUG_DEPS   yLOG_note    ("target is null and not required, delete");
      rc = CELL__delete (CHG_NOHIST, a_target->tab, a_target->col, a_target->row);
      DEBUG_DEPS   yLOG_value   ("rc"        , rc);
      if (rc != 0) {
         DEBUG_DEPS   yLOG_value   ("EXIT"      , 0);
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      DEBUG_DEPS   yLOG_note    ("nulling pointer to a_target");
      a_target == NULL;
      DEBUG_DEPS   yLOG_value   ("a_target"  , a_target);
   }
   /*---(check for unrooting source)-----*/
   DEBUG_DEPS   yLOG_note    ("UNROOTING: check on source needing unrooting");
   DEBUG_DEPS   yLOG_value   ("nrequire"  , a_source->nrequire);
   DEBUG_DEPS   yLOG_value   ("nprovide"  , a_source->nprovide);
   --rce;
   if (a_source->nrequire == 0 && a_source->nprovide == 1) {
      DEBUG_DEPS   yLOG_info    ("provide"  , a_source->provides->target->label);
      if (a_source->provides->target == dtree) {
         DEBUG_DEPS   yLOG_note    ("provides only to root, try to delete provides");
         rc = DEP_delete (DEP_REQUIRE, dtree, a_source);   /* DEP_REQUIRE */
         if (rc != 0)   {
            DEBUG_DEPS   yLOG_value   ("FAILED"    , rce);
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_note    ("success, source unrooted");
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_note    ("all actions complete");
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                  range creation/destruction                  ----===*/
/*====================------------------------------------====================*/
PRIV void  o___RANGE___________o () { return; }

char         /*--> establish a range dependency ----------[ ------ [ ------ ]-*/
DEP_range          (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell,     /* cell with range formula                      */
      int         a_btab,     /* upper-left tab of range                      */
      int         a_bcol,     /* upper-left column of range                   */
      int         a_brow,     /* upper-left row of range                      */
      int         a_etab,     /* bottom-right tab of range                    */
      int         a_ecol,     /* bottom-right column of range                 */
      int         a_erow)     /* bottom-right row of range                    */
{  /*---(design notes)--------------------------------------------------------* 
    * creates the full set of dependencies required when a range formula is   * 
    * used in a cell.  given how this spreadsheet works and visual clarity,   *
    * all cells are created in the range if not already existing.  since      *
    * ranges are not uncommon, this is simply the shared logic to get it      *
    * done.  it is critical that callers pay attention to the return code as  *
    * uncaught errors can run amok.                                           */
   /*---(locals)-----------+-----------+-*//*---------------------------------*/
   int         r           = 0;             /* iterator -- rows               */
   int         c           = 0;             /* iterator -- columns            */
   tCELL      *dest        = NULL;          /* cell at current tab, col, row  */
   char        rc          = 0;             /* generic return code character  */
   tCELL      *rcp         = NULL;          /* generic return code pointer    */
   char        rce         = -10;           /* return code for errors         */
   char        rce_save    = 0;             /* saved rce value for backout    */
   /*---(begin)--------------------------*/
   DEBUG_DEPS    yLOG_enter   (__FUNCTION__);
   DEBUG_DEPS    yLOG_point   ("a_cell"    , a_cell);
   DEBUG_DEPS    yLOG_complex ("range"     , "bt=%4d, bc=%4d, br=%4d, et=%4d, ec=%4d, er=%4d", a_btab, a_bcol, a_brow, a_etab, a_ecol, a_erow);
   /*---(defense: multiple tabs)---------*/
   --rce;  if (a_btab != a_etab)  {
      DEBUG_DEPS    yLOG_note    ("aborting, tabs do not match");
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: range top-left)--------*/
   rc  = LOC_legal (a_btab, a_bcol, a_brow, CELL_FIXED);
   --rce;  if (rc  < 0) {
      DEBUG_DEPS    yLOG_note    ("aborting, beg cell coords not legal");
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: range bototm-right)----*/
   rc  = LOC_legal (a_etab, a_ecol, a_erow, CELL_FIXED);
   --rce;  if (rc  < 0)  {
      DEBUG_DEPS    yLOG_note    ("aborting, end cell coords not legal");
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: range orientation)-----*/
   --rce;  if (a_bcol >  a_ecol)  {
      DEBUG_DEPS    yLOG_note    ("aborting, beg col after end col");
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (a_brow >  a_erow) {
      DEBUG_DEPS    yLOG_note    ("aborting, beg row after end row");
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(setup dependencies)-------------*/
   DEBUG_DEPS    yLOG_info    ("source"    , a_cell->label);
   --rce;
   for (r = a_brow; r <= a_erow; ++r) {
      for (c = a_bcol; c <= a_ecol; ++c) {
         /*---(get existing)-------------*/
         dest  = tabs[a_btab].sheet[c][r];
         DEBUG_DEPS    yLOG_complex ("target"    , "col=%4d, row=%4d, ptr=%9p", c, r, dest);
         /*---(create if null)-----------*/
         if (dest    == NULL)          dest = CELL_change (CHG_NOHIST, a_btab, c, r, "");
         if (dest    == NULL)        { rce_save = rce;     break; }
         /*---(add source if blank)------*/
         rcp = dest;   /* must initialize */
         if (dest->s == NULL)          rcp  = CELL_change (CHG_NOHIST, a_btab, c, r, "");
         if (rcp == NULL)            { rce_save = rce - 1; break; }
         if (rcp != dest)            { rce_save = rce - 2; break; }
         /*---(create dependency)--------*/
         DEBUG_DEPS    yLOG_info    ("target"    , dest->label);
         rc  = DEP_create (DEP_RANGE, a_cell, dest);
         if (rc  <  0)               { rce_save = rce - 3; break; }
      }
      if (rce_save < 0) break;
   }
   /*---(backout)------------------------*/
   if (rce_save < 0) {
      DEBUG_DEPS    yLOG_note    ("aborting, error previously encountered");
      DEP_cleanse (a_cell);
      rce = rce_save;
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> remove all requires deps from a cell --[ ------ [ ------ ]-*/
DEP_cleanse        (
      /*----------+-----------+-----------------------------------------------*/
      tCELL      *a_cell)     /* cell to be cleansed                          */
{  /*---(design notes)--------------------------------------------------------*/
   /*---(locals)-----------+-----------+-*/
   tDEP       *next        = NULL;
   tDEP       *save        = NULL;
   char        rce         =  -10;
   char        rc          = 0;
   /*---(begin)--------------------------*/
   DEBUG_DEPS    yLOG_enter   (__FUNCTION__);
   DEBUG_DEPS    yLOG_point   ("a_cell"    , a_cell);
   /*---(defenses)-----------------------*/
   --rce;  if (a_cell == NULL) {
      DEBUG_DEPS    yLOG_note    ("exiting, cell is NULL");
      DEBUG_DEPS    yLOG_enter   (__FUNCTION__);
      return rce;
   }
   /*---(reqs)---------------------------*/
   DEBUG_DEPS    yLOG_info    ("label"     , a_cell->label);
   DEBUG_DEPS    yLOG_value   ("nrequire"  , a_cell->nrequire);
   next = a_cell->requires;
   while (next != NULL) {
      DEBUG_DEPS    yLOG_point   ("next"      , next);
      save = next->next;
      DEBUG_DEPS    yLOG_complex ("target"    , "ptr=%9p, label=%s", next->target, next->target->label);
      rc = DEP_delete (next->type, a_cell, next->target);
      DEBUG_DEPS    yLOG_value   ("nrequire"  , a_cell->nrequire);
      next = save;
   }
   /*---(check if rooted)----------------*/
   DEBUG_DEPS    yLOG_value   ("nprovide"  , a_cell->nprovide);
   DEBUG_DEPS    yLOG_point   ("provides"  , a_cell->provides);
   if (a_cell->nprovide == 1) {
      DEBUG_DEPS    yLOG_point   ("source"    , a_cell->provides->source);
      DEBUG_DEPS    yLOG_info    ("label"     , a_cell->provides->source->label);
      DEBUG_DEPS    yLOG_point   ("target"    , a_cell->provides->target);
      DEBUG_DEPS    yLOG_info    ("label"     , a_cell->provides->target->label);
      DEBUG_DEPS    yLOG_point   ("dtree"     , dtree);
      if (a_cell->provides->target == dtree) {
         DEBUG_DEPS    yLOG_note    ("unrooting");
         rc = DEP_delete (DEP_REQUIRE, dtree, a_cell);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         information                          ----===*/
/*====================------------------------------------====================*/
PRIV void  o___INFO____________o () { return; }

char       /*----: list the requirements -------------------------------------*/
DEP_requires       (tCELL  *a_me, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *n           = NULL;
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", MAX_STR);   /* special for a null list */
   --rce;  if (a_me    == NULL)  return rce;
   /*---(walk the list)------------------*/
   strncpy (a_list, ",", MAX_STR);
   n = a_me->requires;
   while (n != NULL) {
      if (strchr (S_DEP_REQS, n->type) != 0) {
         strncat    (a_list, n->target->label, MAX_STR);
         strncat    (a_list, ","             , MAX_STR);
      }
      n = n->next;
   }
   /*---(catch empty)--------------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: list the provisions ---------------------------------------*/
DEP_provides       (tCELL  *a_me, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *n           = NULL;
   char        rce         = -10;
   /*---(defenses)-----------------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", MAX_STR);   /* special for a null list */
   --rce;  if (a_me    == NULL)  return rce;
   /*---(walk the list)---------------*/
   strncpy (a_list, ",", MAX_STR);
   n = a_me->provides;
   while (n != NULL) {
      if (strchr (S_DEP_PROS, n->type) != 0) {
         if (n->type != DEP_LIKE  ) {
            strncat    (a_list, n->target->label, MAX_STR);
            strncat    (a_list, ","             , MAX_STR);
         }
      }
      n = n->next;
   }
   /*---(catch empty)-----------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)--------------------*/
   return 0;
}

char 
DEP_like           (tCELL  *a_me, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *n           = NULL;
   char        rce         = -10;
   /*---(defenses)-------s---------------*/
   --rce;  if (a_list  == NULL)  return rce;
   strncpy (a_list, "-", MAX_STR);   /* special for a null list */
   --rce;  if (a_me    == NULL)  return rce;
   /*---(walk the list)---------------*/
   strncpy (a_list, ",", MAX_STR);
   n = a_me->provides;
   while (n != NULL) {
      if (n->type == DEP_LIKE  ) {
         strncat    (a_list, n->target->label, MAX_STR);
         strncat    (a_list, ","             , MAX_STR);
      }
      n = n->next;
   }
   /*---(catch empty)-----------------*/
   if (strcmp (a_list, ",") == 0)   strcpy (a_list, ".");
   /*---(complete)--------------------*/
   return 0;
}

char         /*--> sort cell label list ------------------[ ------ [ ------ ]-*/
DEP_gnome          (char *a_list)
{
   /*---(purpose)-------------------------------------------------------------* 
    * for unit testing to be reliable, cosmetic changes must not result in    *
    * failures or the need to update the unit test script.  label lists from  *
    * the dependency linked lists are a prime example.  they are added in the *
    * order they are encountered, but simple reordering of operations can     *
    * make them appear different even if the same cells are listed.  this     *
    * routine simply sorts a list of dependencies so they are more stable.    */
   /*---(design notes)--------------------------------------------------------* 
    * there are many sort algorithms which can handle vast amounts of complex *
    * data, but they are simply overkill and problematic for small, simple    *
    * lists.  the teleporting gnome sort is my personal favorite due to its   *
    * simplicity which in turn leads to maintainability and confidence.       */
   /*---(oddities)------------------------------------------------------------* 
    * one is added to tab, col, and row before storage in the array and       *
    * sorting as the all three are consolidated mathmatically into a single   *
    * fieild and zeros are just a little harder to debug ;)                   */
   /*---(locals)-----------+-----------+-*//*---------------------------------*/
   char        rce         = -10;           /* return code for errors         */
   char        x_list      [1000] = "";     /* copy of the list for parsing   */
   char       *p           = NULL;          /* strtok parsing pointer         */
   char       *q           = ",;";          /* strtok delimeter string        */
   int         x_array     [1000];          /* working list as integers       */
   int         n_array     =  0;            /* working count of entries       */
   int         i           =  0;            /* loop iterator -- entry         */
   int         x_tab       =  0;            /* tab of current entry           */
   int         x_col       =  0;            /* col of current entry           */
   int         x_row       =  0;            /* row of current entry           */
   char        x_abs       =  0;            /* referencing of current entry   */
   char        x_label     [15];            /* label for sorted entry         */
   char        rc          =  0;            /* return code                    */
   int         a           =  0;            /* comparison entry one           */
   int         b           =  0;            /* comparison entry two           */
   int         tele        = -1;            /* teleport point to speed sort   */
   char       *r           = NULL;
   /*---(header)-------------------------*/
   DEBUG_GNOME   yLOG_enter   (__FUNCTION__);
   /*---(defense: null)------------------*/
   --rce;
   if (a_list  == NULL)  {
      DEBUG_GNOME   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: empty)-----------------*/
   --rce;
   if (strlen (a_list)  == 0)  {
      DEBUG_GNOME   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(initialize)---------------------*/
   DEBUG_GNOME   yLOG_note    ("initialize");
   for (i = 0; i < 100; ++i)  x_array[i] = 0;
   n_array = 0;
   /*---(parse/load)---------------------*/
   DEBUG_GNOME   yLOG_note    ("load the array");
   strcpy (x_list, a_list);
   p = strtok_r (x_list, q, &r);
   i = 0;
   --rce;
   while (p != NULL) {
      DEBUG_GNOME   yLOG_info    ("parse"     , p);
      rc = LOC_parse (p, &x_tab, &x_col, &x_row, &x_abs);
      DEBUG_GNOME   yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         DEBUG_GNOME   yLOG_note    ("could not parse, EXITING");
         DEBUG_GNOME   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      DEBUG_GNOME   yLOG_complex ("parts"     , "tab=%04d, col=%04d, row=%04d, abs=%d", x_tab, x_col, x_row, x_abs);
      ++x_tab;
      ++x_col;
      ++x_row;
      DEBUG_GNOME   yLOG_complex ("inserted"  , "tab=%04d, col=%04d, row=%04d, abs=%d", x_tab, x_col, x_row, x_abs);
      x_array[i] = (x_tab * 100000000) + (x_col * 100000) + (x_row * 10) + x_abs;
      DEBUG_GNOME   yLOG_pair    (i         , x_array [i]);
      p = strtok_r (NULL  , q, &r);
      ++i;
   }
   n_array = i;
   /*---(sort)---------------------------*/
   DEBUG_GNOME   yLOG_note    ("run the teleporting gnome");
   i = 1;
   while (i < n_array) {
      /*---(load vars)-------------------*/
      a = x_array [i - 1];
      b = x_array [i];
      DEBUG_GNOME   yLOG_complex ("current"   , "compare i,%3d (t,%3d) =  a,%10d to b,%10d", i, tele, a, b);
      /*---(compare)---------------------*/
      if (i == 0 || a < b) {
         if (tele >= 0) {
            i    = tele;
            tele = -1;
         } else {
            ++i;
         }
         continue;
      }
      /*---(swap)------------------------*/
      x_array [i - 1] = b;
      x_array [i]     = a;
      a = x_array [i - 1];
      b = x_array [i];
      DEBUG_GNOME   yLOG_complex ("swapped"   , "now     i,%3d (t,%3d) =  a,%10d to b,%10d", i, tele, a, b);
      /*---(update)----------------------*/
      if (tele < 0) tele = i;
      if (i > 1) --i;
   }
   /*---(put back in list)---------------*/
   DEBUG_GNOME   yLOG_note    ("replace in the list");
   strcpy (a_list, ",");
   for (i = 0; i < n_array; ++i) {
      DEBUG_GNOME   yLOG_value   ("value"   , x_array[i]);
      x_tab = x_array[i] / 100000000;
      x_col = (x_array[i] - (x_tab * 100000000))         / 100000;
      x_row = (x_array[i] - (x_tab * 100000000) - (x_col * 100000))         / 10;
      x_abs = (x_array[i] - (x_tab * 100000000) - (x_col * 100000) - (x_row * 10));
      DEBUG_GNOME   yLOG_complex ("removed"   , "tab=%04d, col=%04d, row=%04d, abs=%d", x_tab, x_col, x_row, x_abs);
      --x_tab;
      --x_col;
      --x_row;
      DEBUG_GNOME   yLOG_complex ("parts"     , "tab=%04d, col=%04d, row=%04d, abs=%d", x_tab, x_col, x_row, x_abs);
      rc = LOC_ref (x_tab, x_col, x_row, x_abs, x_label);
      DEBUG_GNOME   yLOG_value   ("label"   , x_label);
      DEBUG_GNOME   yLOG_pair    (i         , x_label);
      strcat (a_list, x_label);
      strcat (a_list, ",");
   }
   DEBUG_GNOME   yLOG_info    ("final"     , a_list);
   /*---(complete)-----------------------*/
   DEBUG_GNOME   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           calculation                        ----===*/
/*====================------------------------------------====================*/
PR void  o___CALCULATION_____o () { return; }

char       /*----: list the provisions ---------------------------------------*/
DEP_updatelikes    (tCELL  *a_me)
{
   DEBUG_DEPS    yLOG_enter   (__FUNCTION__);
   /*---(locals)-----------+-----------+-*/
   tDEP       *n           = NULL;
   char        rce         = -10;
   int         x_tab       =   0;
   int         x_col       =   0;
   int         x_row       =   0;
   char       *x_src       = NULL;
   tCELL      *x_likes     [1000];
   int         x_count     =    0;
   int         i           =    0;
   /*---(defense: cell)------------------*/
   DEBUG_DEPS    yLOG_point   ("a_me"      , a_me);
   --rce;  if (a_me    == NULL) {
      DEBUG_DEPS    yLOG_note    ("cell pointer is null");
      DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(build the list)--------------*/
   DEBUG_DEPS    yLOG_point   ("nprovide"  , a_me->nprovide);
   n = a_me->provides;
   while (n != NULL) {
      DEBUG_DEPS    yLOG_point   ("dep cell"  , n->target);
      DEBUG_DEPS    yLOG_info    ("dep targ"  , n->target->label);
      DEBUG_DEPS    yLOG_char    ("dep type"  , n->type);
      if (n->type == DEP_LIKE) {
         DEBUG_DEPS    yLOG_note    ("ADDED");
         x_likes [x_count] = n->target;
         ++x_count;
      }
      n = n->next;
   }
   /*---(walk the list)---------------*/
   for (i = 0; i < x_count; ++i) {
      DEBUG_DEPS    yLOG_value   ("process"   , i);
      DEBUG_DEPS    yLOG_info    ("label"     , x_likes [i]->label);
      x_tab = x_likes [i]->tab;
      x_col = x_likes [i]->col;
      x_row = x_likes [i]->row;
      x_src = strdup (x_likes [i]->s);
      CELL_change (CHG_NOHIST, x_tab, x_col, x_row, x_src);
      free (x_src);
   }
   /*---(complete)--------------------*/
   DEBUG_DEPS    yLOG_exit    (__FUNCTION__);
   return 0;
}

PR char    /*----: dependency-based calculation engine -----------------------*/
DEP__print         (int a_level, tCELL *a_curr)
{
   /*---(defenses)-----------------------*/
   if (a_curr       == NULL) return 0;
   /*---(locals)-------------------------*/
   int       i         = 0;
   /*---(recurse)------------------------*/
   /*> for (i = 0; i < a_level; ++i) printf("   ");                                   <*/
   /*> printf ("%d %-6.6s = %8.2lf  (t=%2d, c=%3d, r=%3d, s=%s)\n", a_level, a_curr->label, a_curr->v_num, a_curr->tab, a_curr->col, a_curr->row, a_curr->s);   <*/
   /*---(complete)-----------------------*/
   return 0;
}

/*> char         /+--> head recursion through deptree --------[ ------ [ ------ ]-+/                                                                                                                         <* 
 *> DEP_head           (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp, void (*a_func) (FILE *r_file, char a_type, int *r_seq, int r_level, tCELL *r_curr, long r_stamp))   <* 
 *> {                                                                                                                                                                                                        <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                                                                                              <* 
 *>    tCELL      *x_list      [1000];          /+ list of dependent cells        +/                                                                                                                         <* 
 *>    int         x_count     = 0;             /+ count of dependent cells       +/                                                                                                                         <* 
 *>    int         i           = 0;             /+ iterator -- cell list          +/                                                                                                                         <* 
 *>    tDEP       *n           = NULL;                                                                                                                                                                       <* 
 *>    /+---(header)-------------------------+/                                                                                                                                                              <* 
 *>    DEBUG_CALC   yLOG_enter   (__FUNCTION__);                                                                                                                                                             <* 
 *>    DEBUG_CALC   yLOG_complex ("focus"     , "level %d, cell %p, stamp %ld", a_level, a_curr, a_stamp);                                                                                                   <* 
 *>    /+---(defense: null cell)-------------+/                                                                                                                                                              <* 
 *>    if (a_curr == NULL) {                                                                                                                                                                                 <* 
 *>       DEBUG_CALC   yLOG_note    ("cell is NULL");                                                                                                                                                        <* 
 *>       DEBUG_CALC   yLOG_exit    (__FUNCTION__);                                                                                                                                                          <* 
 *>       return 0;                                                                                                                                                                                          <* 
 *>    }                                                                                                                                                                                                     <* 
 *>    /+---(current)------------------------+/                                                                                                                                                              <* 
 *>    DEBUG_CALC   yLOG_complex ("detail"    , "label %s, type %c, nrequire %d", a_curr->label, a_curr->t, a_curr->nrequire);                                                                               <* 
 *>    /+---(gather required cells)----------+/                                                                                                                                                              <* 
 *>    n = a_curr->requires;                                                                                                                                                                                 <* 
 *>    while (n != NULL && x_count < 1000) {                                                                                                                                                                 <* 
 *>       if (n->target->nprovides > 1) {                                                                                                                                                                    <* 
 *>          tCELL [x_count] = n->target;                                                                                                                                                                    <* 
 *>          ++x_count;                                                                                                                                                                                      <* 
 *>       }                                                                                                                                                                                                  <* 
 *>       n = n->next;                                                                                                                                                                                       <* 
 *>    }                                                                                                                                                                                                     <* 
 *>    DEBUG_CALC   yLOG_value   ("x_count"   , x_count);                                                                                                                                                    <* 
 *>    /+---(defense: count too large)-------+/                                                                                                                                                              <* 
 *>    if (x_count >= 1000) {                                                                                                                                                                                <* 
 *>       DEBUG_CALC   yLOG_note    ("too many requires to process");                                                                                                                                        <* 
 *>       DEBUG_CALC   yLOG_exit    (__FUNCTION__);                                                                                                                                                          <* 
 *>       return -1;                                                                                                                                                                                         <* 
 *>    }                                                                                                                                                                                                     <* 
 *>    /+---(perform function)---------------+/                                                                                                                                                              <* 
 *>    a_func (a_file, a_type, a_seq, a_level, a_curr, a_stamp);                                                                                                                                             <* 
 *>    /+---(recurse)------------------------+/                                                                                                                                                              <* 
 *>    n = a_curr->requires;                                                                                                                                                                                 <* 
 *>    while (n != NULL) {                                                                                                                                                                                   <* 
 *>       DEP_tail (a_file, a_type, a_seq, a_level + 1, n->target, a_stamp, a_func);                                                                                                                         <* 
 *>       n = n->next;                                                                                                                                                                                       <* 
 *>    }                                                                                                                                                                                                     <* 
 *>    /+---(complete)-----------------------+/                                                                                                                                                              <* 
 *>    DEBUG_CALC   yLOG_exit    (__FUNCTION__);                                                                                                                                                             <* 
 *>    return 0;                                                                                                                                                                                             <* 
 *> }                                                                                                                                                                                                        <*/

char         /*--> tail recursion through deptree --------[ ------ [ ------ ]-*/
DEP_tail           (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp, char (*a_func) (FILE *r_file, char a_type, int *r_seq, int r_level, tCELL *r_curr, long r_stamp))
{
   /*---(locals)-------------------------*/
   tDEP     *n         = NULL;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_CALC   yLOG_complex ("focus"     , "level %d, cell %p, stamp %ld", a_level, a_curr, a_stamp);
   /*---(defenses)-----------------------*/
   if (a_curr == NULL) {
      DEBUG_CALC   yLOG_note    ("cell is NULL");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(current)------------------------*/
   DEBUG_CALC   yLOG_complex ("detail"    , "label %s, type %c, nrequire %d", a_curr->label, a_curr->t, a_curr->nrequire);
   /*---(recurse)------------------------*/
   n = a_curr->requires;
   while (n != NULL) {
      rc = DEP_tail (a_file, a_type, a_seq, a_level + 1, n->target, a_stamp, a_func);
      n = n->next;
   }
   /*---(perform function)---------------*/
   rc = a_func (a_file, a_type, a_seq, a_level, a_curr, a_stamp);
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

PR char    /*----: dependency-based calculation from root (all) --------------*/
DEP__exec          (int a_level, tDEP *a_dep, long a_stamp)
{
   /*---(locals)-------------------------*/
   tCELL      *x_cell      = NULL;
   tDEP       *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   x_cell     = a_dep->target;
   DEBUG_CALC   yLOG_complex ("focus"     , "level %d, cell %p, stamp %ld", a_level, x_cell, a_stamp);
   /*---(defenses)-----------------------*/
   if (x_cell       == NULL) {
      DEBUG_CALC   yLOG_note    ("cell is NULL");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(recurse)------------------------*/
   DEBUG_CALC   yLOG_info    ("label"     , x_cell->label);
   DEBUG_CALC   yLOG_char    ("type"      , x_cell->t);
   DEBUG_CALC   yLOG_value   ("nrequire"  , x_cell->nrequire);
   x_next = x_cell->requires;
   while (x_next != NULL) {
      DEP__exec (a_level + 1, x_next, a_stamp);
      x_next = x_next->next;
   }
   if (strchr (G_CELL_CALC, x_cell->t) != 0 && x_cell->u != a_stamp) {
      CALC_eval (x_cell);
      CELL_printable (x_cell);
      x_cell->u = a_stamp;
      a_dep->count++;
   }
   DEBUG_CALC   DEP__print (a_level, x_cell);
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: recalculate everything ------------------------------------*/
DEP_recalc         (void)
{
   /*---(locals)-------------------------*/
   tDEP       *x_next      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   if (debug.dtree == 'y') endwin();
   /*---(recurse)------------------------*/
   x_next = dtree->requires;
   while (x_next != NULL) {
      DEP__exec (1, x_next, rand());
      x_next = x_next->next;
   }
   if (debug.dtree == 'y') exit (0);
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

PR char    /*----: dependency-based calculation upward from cell -------------*/
DEP__revs          (int a_level, tDEP *a_dep, long a_stamp)
{
   /*---(locals)-------------------------*/
   tDEP       *x_next      = NULL;
   tCELL      *x_cell      = NULL;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   x_cell     = a_dep->target;
   DEBUG_CALC   yLOG_value   ("a_level"   , a_level);
   DEBUG_CALC   yLOG_value   ("a_stamp"   , a_stamp);
   DEBUG_CALC   yLOG_point   ("*x_cell"   , x_cell);
   /*---(defenses)-----------------------*/
   if (x_cell       == NULL) {
      DEBUG_CALC   yLOG_note    ("cell is NULL");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(recurse)------------------------*/
   DEBUG_CALC   yLOG_info    ("label"     , x_cell->label);
   DEBUG_CALC   yLOG_char    ("type"      , x_cell->t);
   if (x_cell->u != a_stamp) {
      CALC_eval (x_cell);
      CELL_printable (x_cell);
      x_cell->u = a_stamp;
      a_dep->count++;
   }
   DEBUG_CALC   yLOG_value   ("nprovide"  , x_cell->nprovide);
   x_next = x_cell->provides;
   while (x_next != NULL) {
      DEP__revs (a_level + 1, x_next->target, a_stamp);
      x_next = x_next->next;
   }
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /*----: recalculate from cell upwards -----------------------------*/
DEP_calc_up        (tCELL *a_cell)
{
   /*---(locals)-------------------------*/
   tDEP       *x_next      = NULL;
   int         c           = 0;
   /*---(header)-------------------------*/
   DEBUG_CALC   yLOG_enter   (__FUNCTION__);
   DEBUG_CALC   yLOG_info    ("base cell" , a_cell->label);
   DEBUG_CALC   yLOG_value   ("nprovide"  , a_cell->nprovide);
   if (debug.dtree == 'y') endwin();
   /*---(recurse)------------------------*/
   x_next = a_cell->provides;
   while (x_next != NULL) {
      ++c;
      DEBUG_CALC   yLOG_value   ("recurse"   , c);
      DEP__revs (1, x_next, rand());
      x_next = x_next->next;
   }
   DEBUG_CALC   yLOG_note    ("done recursing");
   if (debug.dtree == 'y') exit (0);
   /*---(complete)-----------------------*/
   DEBUG_CALC   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                             auditing                         ----===*/
/*====================------------------------------------====================*/
PR void  o___AUDIT___________o () { return; }

char
DEP_trace          (int a_level, tCELL *a_curr)
{
   /*---(defenses)-----------------------*/
   if (a_curr       == NULL) return 0;
   /*---(process)------------------------*/
   tDEP     *n         = NULL;
   int       i;
   /*> for (i = 0; i < a_level; ++i) printf("   ");                                   <*/
   /*> printf ("%d %s\n", a_level, a_curr->label);                                    <*/
   n = a_curr->provides;
   while (n != NULL) {
      DEP_trace (a_level + 1, n->target);
      n = n->next;
   }
   /*---(complete)-----------------------*/
   return 0;
}

#define     MAX_DEP    100
char        dep_loop [MAX_DEP][10];
int         dep_count = 0;

char
DEP_checkall       (char a_print)
{
   char        rcc         = 0;
   rcc = DEP_check (0, dtree, a_print, rand());
   return rcc;
}

char
DEP_check          (int a_level, tCELL *a_curr, char a_print, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *next        = NULL;
   int         i;
   int         j;
   char        looped      =    0;
   char        rcc         =    0;
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   DEBUG_DEPS   yLOG_value   ("a_level"   , a_level);
   DEBUG_DEPS   yLOG_point   ("a_curr"    , a_curr);
   /*---(defenses)-----------------------*/
   --rce;  if (a_curr   == NULL)  {
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "current pointer is null");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   --rce;  if (a_level  >= MAX_DEP) {
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "past max depth");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   /*---(checksums)----------------------*/
   if (a_level == 0) {
      DEBUG_DEPS   yLOG_info    ("ROOT"      , "root, reset counters");
      dep_count  = 0;
   }
   /*> if (a_curr->u != a_stamp) {                                                      <* 
    *>    if (a_level == 0) {                                                           <* 
    *>       DEBUG_DEPS   yLOG_info    ("counting"  , "root, reset counters");          <* 
    *>       dep_count  = 0;                                                            <* 
    *>    } else {                                                                      <* 
    *>       DEBUG_DEPS   yLOG_info    ("counting"  , "new cell, increase counters");   <* 
    *>       dep_count += 2;                                                            <* 
    *>    }                                                                             <* 
    *> } else {                                                                         <* 
    *>    DEBUG_DEPS   yLOG_info    ("counting"  , "already processed, don't add");     <* 
    *> }                                                                                <*/
   DEBUG_DEPS   yLOG_value   ("dep_count" , dep_count);
   /*---(get and save label)-------------*/
   DEBUG_DEPS   yLOG_info    ("label"     , a_curr->label);
   strncpy (dep_loop[a_level], a_curr->label, 9);
   /*---(check for already processed)----*/
   /*> if (a_curr->u == a_stamp) {                                                    <* 
    *>    DEBUG_DEPS   yLOG_info    ("SKIP"      , "already processed");              <* 
    *>    DEBUG_DEPS   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return  0;                                                                  <* 
    *> }                                                                              <*/
   /*---(print current line)-------------*/
   if (a_print == 'y') {
      for (i = 0; i <= a_level; ++i) {
         /*> printf ("%s ", dep_loop[i]);                                             <*/
      }
   }
   /*---(check for duplicate)------------*/
   for (i = 0; i <= a_level; ++i) {
      for (j = i + 1; j <= a_level; ++j) {
         DEBUG_DEPS   yLOG_complex ("check"     , "%02d, %-10.10s VS %02d, %-10.10s", i, dep_loop[i], j, dep_loop[j]);
         if (strcmp (dep_loop[i], dep_loop[j]) == 0) {
            DEBUG_DEPS   yLOG_value   ("MATCH#"    , looped);
            ++looped;
         }
      }
   }
   /*> if (a_print == 'y')  printf ("(%2d)", looped);                                 <*/
   --rce;
   if (looped > 0) {
      /*> if (a_print == 'y')  printf ("   AHHHHHHHHHHHHHHHHHHHHH\n");                <*/
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "loop/cycle found");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   /*> if (a_print == 'y')  printf ("\n");                                            <*/
   /*---(update)-------------------------*/
   /*> a_curr->u = a_stamp;                                                           <*/
   /*---(process children)---------------*/
   DEBUG_DEPS   yLOG_value   ("chidren"   , a_curr->nrequire);
   --rce;
   next = a_curr->requires;
   while (next != NULL) {
      DEBUG_DEPS   yLOG_info    ("process"   , next->target->label);
      /*---(checksums)-------------------*/
      if (a_curr->u != a_stamp) {
         DEBUG_DEPS   yLOG_info    ("counting"  , "new cell, increase counters");
         dep_count += 2;
      } else {
         DEBUG_DEPS   yLOG_info    ("counting"  , "already processed, don't add");
      }
      DEBUG_DEPS   yLOG_value   ("dep_count" , dep_count);
      /*---(recurse to child)------------*/
      rcc = DEP_check (a_level + 1, next->target, a_print, a_stamp);
      DEBUG_DEPS   yLOG_value   ("rcc"       , rcc);
      if (rcc < 0) {
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rcc;
      }
      /*---(prepare for next)------------*/
      next = next->next;
   }
   /*> if (a_print == 'y' && a_level == 0)  printf ("total = %d vs %d\n", dep_count, ndep);   <*/
   /*---(stamp at end)-------------------*/
   a_curr->u = a_stamp;
   /*---(complete)-----------------------*/
   --rce;
   if (a_level == 0) {
      DEBUG_DEPS   yLOG_note    ("checking root exit");
      DEBUG_DEPS   yLOG_value   ("dep_count" , dep_count);
      DEBUG_DEPS   yLOG_value   ("ndep"      , ndep);
      if (dep_count != ndep) {
         DEBUG_DEPS   yLOG_info    ("FAILED"    , "dep_count and ndep do not match");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
DEP_write          (FILE *a_file, int a_level, tDEP *a_dep)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_cell      = NULL;
   tDEP       *x_next      = NULL;
   int         i;
   /*---(prepare)------------------------*/
   x_cell  = a_dep->target;
   /*---(print)--------------------------*/
   for (i = 0; i < a_level; ++i)  fprintf(a_file, "   ");
   if (x_cell->t == '-')  fprintf (a_file, "%d %-6.6s:%c:%c:%3d:\n"      , a_level, x_cell->label, a_dep->type, x_cell->t);
   else                   fprintf (a_file, "%d %-6.6s:%c:%c:%3d:%3d:%s\n", a_level, x_cell->label, a_dep->type, x_cell->t, x_cell->l, x_cell->s);
   /*---(process children)---------------*/
   x_next = x_cell->requires;
   while (x_next != NULL) {
      DEP_write (a_file, a_level + 1, x_next);
      x_next = x_next->next;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
DEP_writeall       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   int         i           = 0;
   int         x_count     = 0;
   int         x_total     = 0;
   tDEP       *x_next      = NULL;
   /*---(open)---------------------------*/
   snprintf (x_name, 95, "%s.deps", my.f_title);
   x_file = fopen(x_name, "w");
   --rce;  if (x_file == NULL)      return rce;
   /*---(totals)-------------------------*/
   fprintf (x_file, "#!/usr/local/bin/gyges\n");
   fprintf (x_file, "#  file     : %s\n", my.f_title);
   fprintf (x_file, "#  report   : dependency audit file\n");
   fprintf (x_file, "\nsummary statistics\n\n");
   fprintf (x_file, "idx  ty  ma  idx  desc----------------------------------------------  count  total\n");
   for (i = 0; i < MAX_DEPTYPE; ++i) {
      if (s_dep_info [i].type == '-')  break;
      if ((i % 2) == 0)  fprintf (x_file, "\n");
      fprintf  (x_file, "%-2d   %c    %c   %2d  %-50.50s  %5d  %5d\n", i,
            s_dep_info [i].type       , s_dep_info [i].match      ,
            s_dep_info [i].match_index, s_dep_info [i].desc       ,
            s_dep_info [i].count      , s_dep_info [i].total      );
      x_count += s_dep_info [i].count;
      x_total += s_dep_info [i].total;
   }
   fprintf (x_file, "\n");
   fprintf (x_file, "---  --  --  ---  -----------------------------------grand-totals---  %5d  %5d\n", x_count, x_total);
   fprintf (x_file, "                                                     (check) ndep     %5d\n", ndep);
   /*---(recurse)------------------------*/
   fprintf (x_file, "\ndetails arranged by first requirement and indented\n\n");
   x_next = dtree->requires;
   while (x_next != NULL) {
      DEP_write (x_file, 1, x_next);
      x_next = x_next->next;
   }
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}

char
DEP_show           (int a_level, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *next        = NULL;
   int       i;
   /*---(defenses)-----------------------*/
   if (a_curr       == NULL) return 0;
   /*---(print)--------------------------*/
   /*> for (i = 0; i < a_level; ++i) printf("   ");                                   <*/
   /*> printf ("%d %s\n", a_level, a_curr->label);                                    <*/
   /*---(process children)---------------*/
   next = a_curr->requires;
   while (next != NULL) {
      DEP_show (a_level + 1, next->target);
      next = next->next;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
DEP_full           (void)
{
   int i = 0;
   tDEP  *curr;
   curr = dhead;
   while (curr != NULL) {
      ++i;
      curr = curr->dnext;
   }
   /*> printf ("forward   = %d\n", i);                                                <*/
   curr = dtail;
   i = 0;
   while (curr != NULL) {
      ++i;
      curr = curr->dprev;
   }
   /*> printf ("backward  = %d\n", i);                                                <*/
   /*> printf ("ndep      = %d\n", ndep);                                             <*/
   curr = dhead;
   i = 0;
   int match     = 0;
   int no_parent = 0;
   int no_other  = 0;
   int no_match  = 0;
   int no_sym1   = 0;
   int no_sym2   = 0;
   while (curr != NULL) {
      ++i;
      if (curr->source == NULL) ++no_parent;
      if (curr->target == NULL) ++no_other;
      if (curr->match  == NULL) ++no_match;
      if (curr->source != curr->match->target) ++no_sym1;
      if (curr->target != curr->match->source) ++no_sym2;
      curr = curr->dprev;
   }
   /*> printf ("no source = %d\n", no_parent);                                        <*/
   /*> printf ("no target = %d\n", no_other);                                         <*/
   /*> printf ("no match  = %d\n", no_match);                                         <*/
   /*> printf ("no sym1   = %d\n", no_sym1);                                          <*/
   /*> printf ("no sym2   = %d\n", no_sym2);                                          <*/
   /*---(complete)-----------------------*/
   return 0;
}

char
DEP_dump           (void)
{
   char xlabel[MAX_STR];
   endwin();
   DEP_show  (0, dtree);
   DEP_trace (0, tabs[0].sheet[2][11]);
   DEP_full  ();
   /*> printf ("0c12  = %9p\n", tabs[0].sheet[2][11]);                                <*/
   DEP_provides  (tabs[0].sheet[2][11], xlabel);
   /*> printf ("deps  = %s\n",  xlabel);                                              <*/
   DEP_requires  (tabs[0].sheet[2][11], xlabel);
   /*> printf ("reqs  = %s\n",  xlabel);                                              <*/
   /*> printf ("dtree = %9p\n", dtree);                                               <*/
   DEP_provides  (dtree, xlabel);
   /*> printf ("deps  = %s\n", xlabel);                                               <*/
   DEP_requires  (dtree, xlabel);
   /*> printf ("reqs  = %s\n", xlabel);                                               <*/
   DEP_checkall ('y');
   exit (0);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*--> unit testing accessor -----------------[--------[--------]-*/
DEP_unit           (
      /*----------+--------------+--------------------------------------------*/
      char       *a_question,    /* text string identifying the question      */
      char       *a_label)       /* label of cell that has the answer         */
{  /*---(design notes)--------------------------------------------------------*/
   /*
    *  i formerly used cell pointers as the second argument, but it was hugely
    *  problematic when testing cells that are created and destroyed regularly.
    *  switching to labels adds an enormous safety factor and takes complexity
    *  out of the unit testing script.  more safety, more resilience, all good.
    */
   /*---(locals)-----------+-----------+-*/
   int         len         = 0;
   char        temp        [MAX_STR];
   int         x_fore      = 0;
   int         x_back      = 0;
   tDEP       *x_dep       = NULL;
   tCELL      *x_cell      = NULL;
   char        rc          = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_dep            : question not understood");
   /*---(identify the cell pointer)------*/
   if (a_label == NULL || strcmp ("root", a_label) == 0) {
      x_cell = dtree;
   } else {
      rc     = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);
      if (rc < 0) {
         sprintf (unit_answer, "s_dep            : label <%s> not legal, rc = %d", a_label, rc);
         return unit_answer;
      }
      x_cell = LOC_cell  (x_tab, x_col, x_row);
      /*> if (x_cell == NULL) {                                                         <* 
       *>    sprintf (unit_answer, "s_dep            : label <%s> is NULL", a_label);   <* 
       *>    return unit_answer;                                                        <* 
       *> }                                                                             <*/
   }
   /*---(dependency list)----------------*/
   if      (strcmp (a_question, "dep_pointers")   == 0) {
      snprintf(unit_answer, LEN_TEXT, "s_dep pointers   : num=%4d, head=%9p, tail=%9p", ndep, dhead, dtail);
   }
   else if (strcmp (a_question, "dep_count")      == 0) {
      x_dep  = dhead; while (x_dep  != NULL) { ++x_fore; x_dep  = x_dep ->dnext; }
      x_dep  = dtail; while (x_dep  != NULL) { ++x_back; x_dep  = x_dep ->dprev; }
      snprintf(unit_answer, LEN_TEXT, "s_dep count      : num=%4d, fore=%4d, back=%4d", ndep, x_fore, x_back);
   }
   /*---(cell reqs/deps)-----------------*/
   else if (strcmp (a_question, "cell_requires")  == 0) {
      DEP_requires (x_cell, temp);
      DEP_gnome    (temp);
      if (x_cell != NULL)  snprintf(unit_answer, LEN_TEXT, "s_dep requires   : %-5.5s (%2d) %-.35s", x_cell->label, x_cell->nrequire, temp);
      else                 snprintf(unit_answer, LEN_TEXT, "s_dep requires   : %-5.5s (%2d) %-.35s", "-----"      , 0               , temp);
   }
   else if (strcmp (a_question, "cell_provides")  == 0) {
      DEP_provides (x_cell, temp);
      DEP_gnome    (temp);
      if (x_cell != NULL)  snprintf(unit_answer, LEN_TEXT, "s_dep provides   : %-5.5s (%2d) %-.35s", x_cell->label, x_cell->nprovide, temp);
      else                 snprintf(unit_answer, LEN_TEXT, "s_dep provides   : %-5.5s (%2d) %-.35s", "-----"      , 0               , temp);
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, LEN_TEXT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}




/*============================----end-of-source---============================*/
