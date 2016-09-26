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
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(dependencies)-------------------*/
   DEP_purge ();
   CELL_dtree ("free");
   dhead  = NULL;
   dtail  = NULL;
   ndep   = 0;
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
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

char         /*--> find the index of a dep match type-----[ leaf   [ ------ ]-*/
DEP_abbrev         (char a_index)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(complete)-----------------------*/
   if (a_index >= 0 && a_index < MAX_DEPTYPE) {
      return s_dep_info [a_index].type;
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
   DEBUG_DEPS   yLOG_senter  (__FUNCTION__);
   /*---(defense: null cell)-------------*/
   DEBUG_DEPS   yLOG_spoint  (a_dep);
   if (a_dep       == NULL) {
      DEBUG_DEPS   yLOG_sexit   (__FUNCTION__);
      return NULL;
   }
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
   DEBUG_DEPS   yLOG_sexit   (__FUNCTION__);
   return  NULL;
}



/*====================------------------------------------====================*/
/*===----                      two-way dependency                      ----===*/
/*====================------------------------------------====================*/
PRIV void  o___TWO_WAY_________o () { return; }

tDEP*        /*--> create a requires dependency ----------[ ------ [ ------ ]-*/
DEP__makereq       (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* type of dependency (source, format, or calc) */
      char        a_index,    /* dependency type entry in table               */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *x_next      = NULL;          /* iterator -- dependency lists   */
   tDEP       *x_req       = NULL;          /* new requires entry             */
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(check for existing requires)----*/
   DEBUG_DEPS   yLOG_info    ("src label" , a_source->label);
   DEBUG_DEPS   yLOG_value   ("src nreqs" , a_source->nrequire);
   x_next = a_source->requires;
   while (x_next != NULL) {
      DEBUG_DEPS   yLOG_info    ("trg label" , x_next->target->label);
      if (x_next->target == a_target) {
         DEBUG_DEPS   yLOG_note    ("found existing, done");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return x_next;
      }
      x_next = x_next->next;
   }
   /*---(create a new requires)-------*/
   DEBUG_DEPS   yLOG_note    ("existing not found, create new requires");
   x_req           = DEP__new ();
   if (x_req == NULL)   {
      DEBUG_DEPS yLOG_note      ("could not make a new requires entry");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return NULL; 
   }
   /*---(assign basics)---------------*/
   DEBUG_DEPS   yLOG_note    ("assign basic values");
   x_req->type     = a_type;
   x_req->source   = a_source;
   x_req->target   = a_target;
   /*---(add to dep counters)---------*/
   DEBUG_DEPS   yLOG_note    ("increment dependency table counters");
   ++(s_dep_info [a_index].count);
   ++(s_dep_info [a_index].total);
   /*---(hook it up to cell)----------*/
   DEBUG_DEPS   yLOG_note    ("hook up dependency into cell linked list");
   if (a_source->requires == NULL) {
      a_source->requires = x_req;
   } else {
      x_next = a_source->requires;
      while (x_next->next != NULL)  x_next = x_next->next;
      x_next->next         = x_req;
      x_req->prev          = x_next;
   }
   ++a_source->nrequire;
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return x_req;
}

tDEP*        /*--> create a provides dependency ----------[ ------ [ ------ ]-*/
DEP__makepro       (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* type of dependency (source, format, or calc) */
      char        a_index,    /* dependency type entry in table               */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *x_next      = NULL;          /* iterator -- dependency lists   */
   tDEP       *x_pro       = NULL;          /* new requires entry             */
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(check for existing requires)----*/
   DEBUG_DEPS   yLOG_info    ("src label" , a_source->label);
   DEBUG_DEPS   yLOG_value   ("src npros" , a_source->nprovide);
   x_next = a_source->provides;
   while (x_next != NULL) {
      DEBUG_DEPS   yLOG_info    ("trg label" , x_next->target->label);
      if (x_next->target == a_target) {
         DEBUG_DEPS   yLOG_note    ("found existing, done");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return x_next;
      }
      x_next = x_next->next;
   }
   /*---(create a new provides)-------*/
   DEBUG_DEPS   yLOG_note    ("existing not found, create new requires");
   x_pro           = DEP__new ();
   if (x_pro == NULL)   {
      DEBUG_DEPS yLOG_note      ("could not make a new provides entry");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return NULL; 
   }
   /*---(assign basics)---------------*/
   DEBUG_DEPS   yLOG_note    ("assign basic values");
   x_pro->type     = DEP_abbrev (a_index);
   x_pro->source   = a_source;
   x_pro->target   = a_target;
   /*---(add to dep counters)---------*/
   DEBUG_DEPS   yLOG_note    ("increment dependency table counters");
   ++(s_dep_info [a_index].count);
   ++(s_dep_info [a_index].total);
   /*---(hook it up to cell)----------*/
   DEBUG_DEPS   yLOG_note    ("hook up dependency into cell linked list");
   if (a_source->provides == NULL) {
      a_source->provides = x_pro;
   } else {
      x_next = a_source->provides;
      while (x_next->next != NULL)  x_next = x_next->next;
      x_next->next         = x_pro;
      x_pro->prev          = x_next;
   }
   ++a_source->nprovide;
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return x_pro;
}

char         /*--> root and unroot cells -----------------[ ------ [ ------ ]-*/
DEP__rooting       (tCELL *a_cell, char a_type)
{  /*---(design notes)-------------------*/
   /*
    *   since root can carry a huge number of dependencies, this search checks
    *   upward from the target for speed.
    *      a = audit/check root
    *      u = unroot
    *      r = root
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tDEP       *x_next      = NULL;
   int         x_norms     = 0;
   int         x_roots     = 0;
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   if (a_cell == NULL) {
      DEBUG_DEPS   yLOG_note    ("cell is null, FAIL");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_cell == dtree) {
      DEBUG_DEPS   yLOG_note    ("cell is root, nothing to do");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return 1;
   }
   DEBUG_DEPS   yLOG_value   ("owners"    , a_cell->nprovide);
   /*---(quick path, existing root)------*/
   --rce;
   if (a_cell->nprovide == 1 && a_cell->provides->target == dtree) {
      switch (a_type) {
      case DEP_ROOT      : 
      case DEP_CHECKROOT : 
         DEBUG_DEPS   yLOG_note    ("perfectly rooted");
         break;
      case DEP_UNROOT    : 
         DEBUG_DEPS   yLOG_note    ("perfect root found, unrooting");
         rc = DEP_delete (DEP_REQUIRE, dtree, a_cell);
         if (rc != 0) {
            DEBUG_DEPS   yLOG_note    ("could not delete root dependency");
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_value   ("owners"    , a_cell->nprovide);
         break;
      }
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return a_cell->nprovide;
   }
   /*---(indenpendent cell)--------------*/
   --rce;
   if (a_cell->nprovide == 0) {
      switch (a_type) {
      case DEP_ROOT      : 
         DEBUG_DEPS   yLOG_note    ("independent cell, rooting");
         rc = DEP_create (DEP_REQUIRE, dtree, a_cell);
         if (rc != 0) {
            DEBUG_DEPS   yLOG_note    ("could not create root dependency");
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_value   ("owners"    , a_cell->nprovide);
         break;
      case DEP_CHECKROOT : 
      case DEP_UNROOT    : 
         DEBUG_DEPS   yLOG_note    ("independent cell, nothing to do");
         break;
      }
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return a_cell->nprovide;
   }
   /*---(count link types)---------------*/
   --rce;
   x_next = a_cell->provides;
   while (x_next != NULL) {
      DEBUG_DEPS   yLOG_info    ("owned by"  , x_next->target->label);
      if (x_next->target != dtree)  ++x_norms;
      else                          ++x_roots;
      x_next = x_next->next;
   }
   DEBUG_DEPS   yLOG_value   ("roots"     , x_roots);
   DEBUG_DEPS   yLOG_value   ("norms"     , x_norms);
   /*---(normal dependent cell)----------*/
   if (x_roots == 0) {
      switch (a_type) {
      case DEP_CHECKROOT : 
         DEBUG_DEPS   yLOG_note    ("normal dependent cell, no root");
         break;
      case DEP_ROOT      : 
         DEBUG_DEPS   yLOG_note    ("normal dependent cell, can not root");
         break;
      case DEP_UNROOT    : 
         DEBUG_DEPS   yLOG_note    ("normal dependent cell, no root, can not unroot");
         break;
      }
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return x_roots;
   }
   /*---(deal with duplicate roots)------*/
   --rce;
   x_next  = a_cell->provides;
   if (x_norms == 0)  x_roots = 0;
   else               x_roots = 1;
   while (x_next != NULL) {
      if (x_next->target == dtree && x_roots > 0) {
         DEBUG_DEPS   yLOG_note    ("found reduntant root, cleaning");
         rc = DEP_delete (DEP_REQUIRE, dtree, a_cell);
      }
      ++x_roots;
      x_next = x_next->next;
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_value   ("owners"    , a_cell->nprovide);
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> create a two-way dependency -----------[ ------ [ ------ ]-*/
DEP_create         (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* type of dependency (source, format, or calc) */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{  /*---(design notes)--------------------------------------------------------*/
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;          /* return code for errors         */
   char        rc          =    0;          /* generic return code            */
   char        x_index     =    0;          /* dependency type table index    */
   tDEP       *x_require   = NULL;    /* new requires entry                   */
   tDEP       *x_provide   = NULL;    /* new provides entry                   */
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source     == NULL)   {
      DEBUG_DEPS yLOG_note      ("source is null");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return rce; 
   }
   DEBUG_DEPS   yLOG_info    ("src label" , a_source->label);
   DEBUG_DEPS   yLOG_point   ("a_target"  , a_target);
   --rce;  if (a_target     == NULL)   {
      DEBUG_DEPS yLOG_note      ("target is null");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_info    ("trg label" , a_target->label);
   /*---(defense: linking back to root)--*/
   --rce;  if (a_target     == dtree)  {
      DEBUG_DEPS yLOG_note      ("can not route back to root");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return rce;
   }
   /*---(defense: circular ref)----------*/
   --rce;  if (a_source     == a_target) {
      DEBUG_DEPS   yLOG_note    ("source and target are same");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: type)------------------*/
   DEBUG_DEPS   yLOG_char    ("a_type"    , a_type);
   x_index     = DEP_index (a_type);
   DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
   --rce;  if (x_index      <  0) {
      DEBUG_DEPS   yLOG_note    ("dependency type not found");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check for circlular ref)--------*/
   DEBUG_DEPS   yLOG_note    ("check for potential circular reference");
   rc     = DEP_circle (0, a_target, a_source, rand());
   DEBUG_DEPS   yLOG_value   ("circle rc" , rc);
   --rce;  if (rc  <  0) {
      DEBUG_DEPS   yLOG_note    ("request would cause a circular reference");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check if target needs unroot)---*/
   DEBUG_DEPS   yLOG_note    ("check target for unrooting");
   rc = DEP__rooting (a_target, DEP_UNROOT);
   if (rc <  0) {
      DEBUG_DEPS   yLOG_note    ("target could not be properly unrooted");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(create require)-----------------*/
   x_require   = DEP__makereq (a_type, x_index, a_source, a_target);
   --rce;  if (x_require    == NULL    ) {
      DEBUG_DEPS   yLOG_note    ("requires did not get made");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(create provide)-----------------*/
   x_index     = DEP_match    (a_type);
   x_provide   = DEP__makepro (a_type, x_index, a_target, a_source);
   --rce;  if (x_require    == NULL    ) {
      DEBUG_DEPS   yLOG_note    ("provides did not get made");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(cross link them)----------------*/
   DEBUG_DEPS   yLOG_note    ("link require and provide together");
   x_require->match     = x_provide;
   x_provide->match     = x_require;
   /*---(check if source needs rooting)--*/
   DEBUG_DEPS   yLOG_note    ("check source for rooting");
   --rce;
   rc = DEP__rooting (a_source, DEP_ROOT);
   if (rc <  1) {
      DEBUG_DEPS   yLOG_note    ("source could not be properly rooted");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check for dup linking)----------*/
   /*> rc = DEP_check (0, a_source, 'n', rand());                                     <*/
   /*> rc = DEP_checkall ('n');                                                       <* 
    *> if (rc <  0) {                                                                 <* 
    *>    DEBUG_DEPS   yLOG_note    ("failed dependency check");                      <* 
    *>    DEBUG_DEPS   yLOG_exit    (__FUNCTION__);                                   <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
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

char         /*--> remove a requires dependency ----------[ ------ [ ------ ]-*/
DEP__remvreq       (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* type of dependency (source, format, or calc) */
      char        a_index,    /* dependency type entry in table               */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *x_next      = NULL;          /* iterator -- dependency lists   */
   tDEP       *x_req       = NULL;          /* new requires entry             */
   char        rce         = -10;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(check for existing requires)----*/
   DEBUG_DEPS   yLOG_info    ("src label" , a_source->label);
   DEBUG_DEPS   yLOG_value   ("src nreqs" , a_source->nrequire);
   x_next = a_source->requires;
   --rce; while (x_next != NULL) {
      DEBUG_DEPS   yLOG_info    ("trg label" , x_next->target->label);
      if (x_next->target == a_target) {
         DEBUG_DEPS   yLOG_note    ("found existing");
         rc = DEP__free (x_next);
         if (rc != 0)  {
            DEBUG_DEPS   yLOG_note    ("deletion failed");
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_note    ("update dependency table totals");
         --(s_dep_info [a_index].count);
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      x_next = x_next->next;
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_note    ("did not find dependency");
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   --rce;
   return rce;
}

char         /*--> remove a provides dependency ----------[ ------ [ ------ ]-*/
DEP__remvpro       (
      /*----------+-----------+-----------------------------------------------*/
      char        a_type,     /* type of dependency (source, format, or calc) */
      char        a_index,    /* dependency type entry in table               */
      tCELL      *a_source,   /* cell with the calculation                    */
      tCELL      *a_target)   /* cell with value the source cell needs        */
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *x_next      = NULL;          /* iterator -- dependency lists   */
   tDEP       *x_req       = NULL;          /* new requires entry             */
   char        rce         = -10;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   /*---(check for existing requires)----*/
   DEBUG_DEPS   yLOG_info    ("source"    , a_source->label);
   DEBUG_DEPS   yLOG_value   ("nprovide"  , a_source->nprovide);
   x_next = a_source->provides;
   --rce; while (x_next != NULL) {
      DEBUG_DEPS   yLOG_info    ("target"    , x_next->target->label);
      if (x_next->target == a_target) {
         DEBUG_DEPS   yLOG_note    ("found existing");
         rc = DEP__free (x_next);
         if (rc != 0)  {
            DEBUG_DEPS   yLOG_note    ("deletion failed");
            DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         DEBUG_DEPS   yLOG_note    ("update dependency table totals");
         --(s_dep_info [a_index].count);
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      x_next = x_next->next;
   }
   /*---(complete)-----------------------*/
   DEBUG_DEPS   yLOG_note    ("did not find dependency");
   DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
   --rce;
   return rce;
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
   char        rce         =  -10;          /* return code for errors         */
   char        rc          =    0;          /* generic return code            */
   char        x_index     =    0;          /* dependency type table index    */
   tDEP       *x_require   = NULL;    /* new requires entry                   */
   tDEP       *x_provide   = NULL;    /* new provides entry                   */
   /*---(defense: null pointers)---------*/
   DEBUG_DEPS   yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source     == NULL)   {
      DEBUG_DEPS yLOG_note      ("source is null");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return rce; 
   }
   DEBUG_DEPS   yLOG_info    ("label"     , a_source->label);
   DEBUG_DEPS   yLOG_value   ("nrequire"  , a_source->nrequire);
   DEBUG_DEPS   yLOG_point   ("a_target"  , a_target);
   --rce;  if (a_target     == NULL)   {
      DEBUG_DEPS yLOG_note      ("target is null");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_info    ("label"     , a_target->label);
   DEBUG_DEPS   yLOG_value   ("nprovide"  , a_target->nprovide);
   /*---(defense: linking back to root)--*/
   --rce;  if (a_target     == dtree)  {
      DEBUG_DEPS yLOG_note      ("can not delete a route back to root");
      DEBUG_DEPS yLOG_exit      (__FUNCTION__);
      return rce;
   }
   /*---(defense: circular ref)----------*/
   --rce;  if (a_source     == a_target) {
      DEBUG_DEPS   yLOG_note    ("source and target are same");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defense: type)------------------*/
   DEBUG_DEPS   yLOG_char    ("a_type"    , a_type);
   x_index     = DEP_index (a_type);
   DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
   --rce;  if (x_index      <  0) {
      DEBUG_DEPS   yLOG_note    ("dependency type not found");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(delete require)-----------------*/
   rc   = DEP__remvreq (a_type, x_index, a_source, a_target);
   --rce;  if (rc <  0) {
      DEBUG_DEPS   yLOG_note    ("requires did not get removed");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_value   ("nrequire"  , a_source->nrequire);
   /*---(delete provide)-----------------*/
   x_index     = DEP_match (a_type);
   DEBUG_DEPS   yLOG_value   ("dep index" , x_index);
   rc   = DEP__remvpro (a_type, x_index, a_target, a_source);
   --rce;  if (rc <  0) {
      DEBUG_DEPS   yLOG_note    ("provides did not get removed");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_value   ("nprovide"  , a_target->nprovide);
   /*---(check if source needs unroot)---*/
   --rce;
   if        (a_source           == dtree) {
      DEBUG_DEPS   yLOG_note    ("source is root, so no unrooting, done");
   } else if (a_source->nrequire >  0) {
      DEBUG_DEPS   yLOG_note    ("source has requires, so leave alone");
   } else {
      DEBUG_DEPS   yLOG_note    ("must check/unroot source");
      rc = DEP__rooting (a_source, DEP_UNROOT);
      if (rc != 0) {
         DEBUG_DEPS   yLOG_note    ("source could not be properly unrooted");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(check on target)----------------*/
   if        (a_source           == dtree) {
      DEBUG_DEPS   yLOG_note    ("source was root, so this is unrooting, done");
   } else if (a_target->nprovide >  0) {
      DEBUG_DEPS   yLOG_note    ("target already connected to tree");
   } else if (a_target->nrequire >  0) {
      DEBUG_DEPS   yLOG_note    ("target is still needed, must root");
      rc = DEP__rooting (a_target, DEP_ROOT);
      if (rc <= 0) {
         DEBUG_DEPS   yLOG_note    ("target could not be properly rooted");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
   } else if (a_target->t == '-') {
      DEBUG_DEPS   yLOG_note    ("target is a zombie and not required, delete");
      rc = CELL__delete (CHG_NOHIST, a_target->tab, a_target->col, a_target->row);
      if (rc != 0) {
         DEBUG_DEPS   yLOG_note    ("target could not be properly deleted");
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      a_target == NULL;
   } else {
      DEBUG_DEPS   yLOG_note    ("target is now independent, nothing to do");
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
   if (a_curr->u == a_stamp) {
      DEBUG_CALC   yLOG_note    ("already stamped and visited");
      DEBUG_CALC   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(current)------------------------*/
   DEBUG_CALC   yLOG_complex ("detail"    , "label %s, type %c", a_curr->label, a_curr->t);
   if (a_curr->nrequire == 0) {
      DEBUG_CALC   yLOG_note    ("cell requires no other cells");
   } else {
      DEBUG_CALC   yLOG_value   ("nrequire"  , a_curr->nrequire);
      /*---(recurse)------------------------*/
      n = a_curr->requires;
      while (n != NULL) {
         rc = DEP_tail (a_file, a_type, a_seq, a_level + 1, n->target, a_stamp, a_func);
         n = n->next;
      }
   }
   /*---(perform function)---------------*/
   DEBUG_CALC   yLOG_note    ("clean up cell");
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
   for (i = 0; i < a_level; ++i) printf("   ");
   printf ("%d %s\n", a_level, a_curr->label);
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
DEP_circle         (int a_level, tCELL *a_source, tCELL *a_target, long a_stamp)
{  /*---(design notes)-------------------*/
   /*
    *  burrows down into the a_source "requires" dependencies to see if the
    *  a_target cell is found.  if so, adding a dependency from a_target to
    *  a_source would create a circular reference.
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   tDEP       *x_next      = NULL;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_DEPS   yLOG_enter   (__FUNCTION__);
   DEBUG_DEPS   yLOG_value   ("a_level"   , a_level);
   /*---(defenses)-----------------------*/
   DEBUG_DEPS   yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source   == NULL)  {
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "source pointer is null");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   DEBUG_DEPS   yLOG_info    ("label"     , a_source->label);
   DEBUG_DEPS   yLOG_point   ("a_target"  , a_target);
   --rce;  if (a_target   == NULL)  {
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "target pointer is null");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   --rce;  if (a_source == a_target) {
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "match, circular link");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_DEPS   yLOG_info    ("label"     , a_target->label);
   --rce;  if (a_level  >= MAX_DEP) {
      DEBUG_DEPS   yLOG_info    ("FAILED"    , "past max depth");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  rce;
   }
   /*---(check for already processed)----*/
   if (a_source->u == a_stamp) {
      DEBUG_DEPS   yLOG_info    ("SKIP"      , "already processed");
      DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(stamp at end)-------------------*/
   a_source->u = a_stamp;
   /*---(process children)---------------*/
   DEBUG_DEPS   yLOG_value   ("nrequire"  , a_source->nrequire);
   --rce;
   x_next = a_source->requires;
   while (x_next != NULL) {
      /*---(recurse to child)------------*/
      DEBUG_DEPS   yLOG_info    ("process"   , x_next->target->label);
      rc = DEP_circle (a_level + 1, x_next->target, a_target, a_stamp);
      if (rc < 0) {
         DEBUG_DEPS   yLOG_exit    (__FUNCTION__);
         return rc;
      }
      /*---(prepare for next)------------*/
      x_next = x_next->next;
   }
   /*---(complete)-----------------------*/
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
   /*> if (x_cell->t == '-')  fprintf (a_file, "%d %-6.6s:%c:%c:%3d:\n"      , a_level, x_cell->label, a_dep->type, x_cell->t);                         <* 
    *> else                   fprintf (a_file, "%d %-6.6s:%c:%c:%3d:%3d:%s\n", a_level, x_cell->label, a_dep->type, x_cell->t, x_cell->l, x_cell->s);   <*/
   if (x_cell->t == '-')  fprintf (a_file, "%d %-6.6s:%c:%c:\n"      , a_level, x_cell->label, a_dep->type, x_cell->t);
   else                   fprintf (a_file, "%d %-6.6s:%c:%c:%3d:%s\n", a_level, x_cell->label, a_dep->type, x_cell->t, x_cell->l, x_cell->s);
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
DEP_writescreen    (void)
{
   /*---(locals)-----------+-----------+-*/
   tDEP       *x_next      = NULL;
   x_next = dtree->requires;
   fprintf (stdout, "root-----------------\n");
   while (x_next != NULL) {
      DEP_write (stdout, 1, x_next);
      x_next = x_next->next;
   }
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
