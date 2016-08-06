/*===============================[[ beg code ]]===============================*/

/*====================-----------------+------------------====================*/
/*===----                           OVERVIEW                           ----===*/
/*====================-----------------+------------------====================*/
/*
 *   s_reg manages a series of temporary registers which are used to facilitate
 *   the movement of cells from one spreadsheet location to another using the
 *   yank, delete, and put commands.  in order to support scripting, batch
 *   operations, and power/technical users; a large number of registers will
 *   be created to complicated operations and scripts can be facilitated.
 *
 *   since registers are vital to all applications that allow any form of
 *   editing or modification, a consistent interface will allow users to
 *   quickly become efficient and effective in each tool.  also, consistency
 *   will allow deeper tool usage as training will easily translate across
 *   applications.
 *
 *   gyges registers will be based on the usage and capabilities of vi/vim, but
 *   as mentioned above, generalized to be useful in a broad range of
 *   applications.  the resulting consistent command set is called the vi-keys
 *   mini-language.  because of this desired consistency, though, i will have to
 *   manage a series of broad applicability versus power/depth trade-offs which
 *   will result in better transportability, but perhaps a slightly sub-optimal
 *   feature set for a spreadsheet.
 *
 *   vi/vim has a huge number of registers, many of which are special purpose.
 *   we will focus on delivering the following fifty-five registers to be used
 *   for (y)ank, (d)elete, and (p)ut.
 *      -      default, unnamed register
 *      a-z    named registers, accessable only within this spreadsheet
 *      A-Z    named registers, accessable to all spreadsheet instances
 *      +      window manager clipboard as delimited, 7bit-safe, ascii text
 *      0      working area                  
 *
 *   gyges registers are meant to support power users, scripting, and robust
 *   batch usage.  for everyday they are way overbuilt ;)
 *
 *
 */



#include   "gyges.h"



/*====================-----------------+------------------====================*/
/*===----                    REGISTER DATA STRUCTURE                   ----===*/
/*====================-----------------+------------------====================*/
/*
 *   the register data structure is about saving cells that have been copied
 *   or deleted from the spreadsheet.  it is critical that all the register
 *   maintain all context information and ordering necessary to properly
 *   reintegrate the cells into a new location.
 *
 */
#define     MAX_REG     100
#define     MAX_BUF    1000
struct cREG {
   /*---(#1, ORIGIN TAB)-----------------*/
   /*   stores the tab number of the original selection so that cell          */
   /*   references can be properly adjusted when placed back into a tab.      */
   int         otab;
   /*---(#2, MINIMUM REACH)--------------*/
   /*   stores the column and row of the furthest upper-left cell reference   */
   /*   within a formula of a register cell (for error-checking).             */
   int         minc;
   int         minr;
   /*---(#3, MINIMUM BOUND)--------------*/
   /*   stores the column and row of the upper-left corner of the selection   */
   /*   used to source the cells for the register.                            */
   int         begc;
   int         begr;
   /*---(#4, MAXIMUM BOUND)--------------*/
   /*   stores the column and row of the lower-right corner of the selection  */
   /*   used to source the cells for the register.                            */
   int         endc;
   int         endr;
   /*---(#5, MAXIMUM REACH)--------------*/
   /*   stores the column and row of the furthest lower-right cell reference  */
   /*   within a formula of a register cell (for error-checking).             */
   int         maxc;
   int         maxr;
   /*---(#6, CELLS)----------------------*/
   /*   stores the an array of cells within the register in the order that    */
   /*   they need to be placed back into a tab to correctly calculate.        */
   int         nbuf;                             /* total spots               */
   tCELL      *buf         [MAX_BUF];            /* cell pointers             */
   char        notes       [MAX_BUF];            /* source coding             */
   int         real;                             /* filled spots              */
   /*---(#7, SPECIAL)--------------------*/
   /*   the type flag indicates whether non-selection formula cells with      */
   /*   references into the selection should be adjusted when pasted.         */
   char        type;
   /*---(end)----------------------------*/
};
static      tREG        s_reg       [MAX_REG];





#define     REG_NAMES      "\"abcdefghijklmnopqrstuvwxyz-+"

static      char        s_regnames     [MAX_REG] = REG_NAMES;



/*====================------------------------------------====================*/
/*===----                   initialization and wrapup                  ----===*/
/*====================------------------------------------====================*/
static void  o___INTIIALIZE______o () { return; }

char             /* clear all selections -----------------[ ------ [ ------ ]-*/
REG_init           (void)
{
   /*---(registers)----------------------*/
   strlcpy (s_regnames , REG_NAMES, MAX_REG);
   my.reg_curr = '"';
   REG_purge   ('y');
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> clear out all buffers -----------------[ ------ [ ------ ]-*/
REG_purge          (char a_init)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_len       = 0;
   /*---(clear)--------------------------*/
   x_len = strlen (s_regnames);
   for (i = 0; i < x_len; ++i) {
      REG_clear (s_regnames[i], a_init);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> clear out a register ------------------[ ------ [ ------ ]-*/
REG_clear          (char a_reg, char a_init)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   int         i           = 0;
   tCELL      *x_curr      = NULL;
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   DEBUG_REGS   yLOG_char    ("a_init"    , a_init);
   /*---(get register number)------------*/
   x_reg  = REG__reg2index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(boundaries)---------------------*/
   DEBUG_REGS   yLOG_note    ("reset all values to zero");
   s_reg [x_reg].otab  = 0;
   s_reg [x_reg].minc  = s_reg [x_reg].minr  = 0;
   s_reg [x_reg].begc  = s_reg [x_reg].begr  = 0;
   s_reg [x_reg].endc  = s_reg [x_reg].endr  = 0;
   s_reg [x_reg].maxc  = s_reg [x_reg].maxr  = 0;
   s_reg [x_reg].type  = '-';
   s_reg [x_reg].nbuf  = 0;
   s_reg [x_reg].real  = 0;
   /*---(cells)--------------------------*/
   DEBUG_REGS   yLOG_note    ("clear all register positions");
   --rce;
   for (i = 0; i < MAX_BUF; ++i) {
      x_curr = s_reg [x_reg].buf [i];
      if (a_init != 'y' && x_curr != NULL) {
         rc = CELL_regdel (x_curr);
         if (rc < 0) {
            DEBUG_REGS   yLOG_note    ("found a bad register position");
            DEBUG_REGS   yLOG_value   ("posid"     , i);
            DEBUG_REGS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
      }
      s_reg[x_reg].buf  [i] = NULL;
      s_reg[x_reg].notes[i] = '-';
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          conversions                         ----===*/
/*====================------------------------------------====================*/
static void  o___CONVERSION______o () { return; }

int          /*--> convert bufname to bufnum -------------[ leaf   [--------]-*/
REG__reg2index     (char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   char       *x_loc       = NULL;
   int         x_index     =  -1;
   char        rce         = -10;
   /*---(defense)------------------------*/
   x_loc     = strchr (s_regnames, a_reg);
   --rce;  if (x_loc == 0)                            return rce;
   /*---(buffer number)------------------*/
   x_index   = (int) (x_loc - s_regnames);
   /*> if      (a_reg == '0')                   x_index = 0;                          <* 
    *> else if (a_reg == '-')                   x_index = 1;                          <* 
    *> else if (a_reg >= 'a' && a_reg <= 'z')   x_index = a_reg - 'a' +  2;           <* 
    *> else if (a_reg >= 'A' && a_reg <= 'Z')   x_index = a_reg - 'A' + 28;           <* 
    *> else if (a_reg == '+')                   x_index = 54;                         <*/
   /*---(defense on range)---------------*/
   --rce; if (x_index <  0      )                     return rce;
   --rce; if (x_index >= MAX_REG)                     return rce;
   /*---(complete)-----------------------*/
   return x_index;
}

int          /*--> convert a register to a tabnum --------[ ------ [ ------ ]-*/
REG__reg2tab       (char a_buf)
{
   /*---(locals)-----------+-----------+-*/
   int         x_buf       =  -1;
   char        x_tab       =  -1;
   char        rce         = -10;
   /*---(get buffer number)--------------*/
   x_buf = REG__reg2index  (a_buf);
   if (x_buf < 0)  return rce;
   /*---(convert to tab number)----------*/
   x_tab = x_buf + MAX_TABS;
   /*---(complete)-----------------------*/
   return x_tab;
}

int          /*--> convert a tab number to a register ----[ ------ [ ------ ]-*/
REG__tab2index     (int a_tab)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     = 0;
   char        rce         = -10;
   /*---(defense)------------------------*/
   --rce;  if (a_tab <  MAX_TABS)                        return rce;
   --rce;  if (a_tab >= MAX_TABS + strlen (s_regnames))  return rce;
   /*---(convert)------------------------*/
   x_index = a_tab - MAX_TABS;
   /*---(complete)-----------------------*/
   return x_index;
}



/*====================------------------------------------====================*/
/*===----                           attaching                          ----===*/
/*====================------------------------------------====================*/
static void  o___ATTACHING_______o () { return; }

char         /*> attach a cell to a buffer ---------------[ -----  [ ------ ]-*/
REG__hook          (tCELL *a_curr, char a_reg, char a_note)
{
   /*---(locals)-----------+-----------+-*/
   int         x_reg       =   0;
   int         x_nbuf      =   0;
   char        rce         = -10;
   char        rc          =   0;
   int         x_tab       =   0;
   int         x_col       =   0;
   int         x_row       =   0;
   /*---(defense: non existant)----------*/
   --rce;  if (a_curr      == NULL)              return  rce;
   /*---(defense: still hooked)----------*/
   --rce;  if (a_curr->tab != UNHOOKED)          return  rce;
   --rce;  if (a_curr->col != UNHOOKED)          return  rce;
   --rce;  if (a_curr->row != UNHOOKED)          return  rce;
   /*---(verify the tab)-----------------*/
   x_reg  = REG__reg2index  (a_reg);
   /*---(defense: out of bounds)---------*/
   --rce;  if (x_reg < 0)                        return  rce;
   /*---(defense: buffer full)-----------*/
   --rce;  if (s_reg[x_reg].nbuf >= MAX_BUF)       return  rce;
   /*---(defense: place taken)-----------*/
   x_nbuf = s_reg[x_reg].nbuf;
   --rce;  if (s_reg[x_reg].buf[x_nbuf] != NULL)   return  rce;
   /*---(attach cell to buffer)----------*/
   s_reg[x_reg].buf  [x_nbuf] = a_curr;
   s_reg[x_reg].notes[x_nbuf] = a_note;
   ++s_reg[x_reg].nbuf;
   ++s_reg[x_reg].real;
   /*---(update cell)--------------------*/
   a_curr->tab = REG__reg2tab (a_reg);
   a_curr->col = x_nbuf;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*>-detach-a-cell-from-a-buffer--------------[--------[--------]-*/
REG__unhook        (tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   tCELL      *x_curr      = NULL;
   int         x_reg       = -1;
   char        rce         = -10;                /* return code for errors    */
   /*---(defenses)-----------------------*/
   --rce;  if (a_curr == NULL)                        return rce;
   --rce;  if (a_curr->tab <  MAX_TABS)               return rce;
   --rce;  if (a_curr->tab == UNHOOKED)               return rce;
   --rce;  if (a_curr->col == UNHOOKED)               return rce;
   /*---(get register)-------------------*/
   x_reg = REG__tab2index (a_curr->tab);
   --rce;  if (x_reg < 0)                             return rce;
   /*---(check register location)--------*/
   --rce;  if (a_curr->col >= s_reg [x_reg].nbuf)     return rce;
   x_curr = s_reg [x_reg].buf[a_curr->col];
   --rce;  if (x_curr == NULL)                        return rce;
   --rce;  if (x_curr != a_curr)                      return rce;
   /*---(take out of register)-----------*/
   s_reg [x_reg].buf  [a_curr->col]  = NULL;
   s_reg [x_reg].notes[a_curr->col]  = 'x';
   --s_reg [x_reg].real;
   /*---(mark as unhooked)---------------*/
   a_curr->tab     = UNHOOKED;
   a_curr->col     = UNHOOKED;
   a_curr->row     = UNHOOKED;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> changes the default register ----------[ leaf   [ ------ ]-*/
REG_set            (char a_reg)
{
   if (strchr (s_regnames, a_reg) != 0) {
      my.reg_curr      = a_reg;
      return  0;
   } else if (a_reg == '"') {
      my.reg_curr      = '"';
      return  0;
   } else if (a_reg == ',') {
      my.reg_curr      = '+';
      return  0;
   } else if (a_reg == '#') {
      REG_clear (my.reg_curr, '-');
      return  0;
   } else if (a_reg == '!') {
      sta_type = '"';
      return  0;
   }
   /*---(complete)-----------------------*/
   return -1;
}



/*====================------------------------------------====================*/
/*===----                       keyboard input                         ----===*/
/*====================------------------------------------====================*/
static void  o___KEYS____________o () { return; }

char          /* PURPOSE : process keys for register actions -----------------*/
REG_mode           (int a_prev, int a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_buf       =  -1;
   /*---(defenses)-----------------------*/
   if (my.mode != SMOD_REGISTER)             return -1;   /* wrong mode                    */
   if (a_curr == K_ESCAPE)  {
      my.mode = MODE_MAP;
      return  0;
   }
   /*---(check for control keys)---------*/
   --rce;  if (a_prev == '"') {
      if (strchr (s_regnames, a_curr) != 0) {
         REG_set    (a_curr);
         return 0;
      } else if (a_curr == '?') {
         my.info_win = '"';
         my.mode = MODE_MAP;
         REG_set ('"');
         return  0;
      } else if (a_curr == '!') {
         sta_type = '"';
         my.mode = MODE_MAP;
         REG_set ('"');
         return  0;
      }
      return rce;
   }
   --rce;  if (a_prev == ' ' && strchr ("+-", my.reg_curr) != 0) {
      switch (a_curr) {
      case  'v' : REG_valuesout('v');
                  break;
      case  'V' : REG_valuesout('V');
                  break;
      case  'c' : REG_valuesout('c');
                  break;
      case  'C' : REG_valuesout('C');
                  break;
      case  't' : REG_valuesout('t');
                  break;
      case  'T' : REG_valuesout('T');
                  break;
      case  's' : REG_valuesout('s');
                  break;
      case  'S' : REG_valuesout('S');
                  break;
      case  'f' : REG_valuesout('f');
                  break;
      case  'F' : REG_valuesout('F');
                  break;
      case  'p' : case  'P' :
                  REG_valuesin ('-');
                  break;
      default   : my.mode = MODE_MAP;
                  REG_set ('"');
                  return rce;
                  break;
      }
      my.mode = MODE_MAP;
      REG_set ('"');
      return 0;
   }
   --rce;  if (a_prev == ' ') {
      switch (a_curr) {
      case  '#' : REG_clear (my.reg_curr, '-');
                  break;
      case  'y' : REG_copy  ();
                  break;
      case  'p' : REG_paste ('y');
                  break;
      case  'd' :
      case  'D' :
      case  'x' :
      case  'X' : REG_cut   ();
                  break;
      case  'W' : REG_bufwrite (my.reg_curr);
                  break;
      case  'g' : x_buf  = REG__reg2index  (my.reg_curr);
                  if (x_buf < 0) {
                     REG_set ('"');
                     return rce;
                     break;
                  }
                  if (s_reg[x_buf].nbuf <= 0) {
                     REG_set ('"');
                     return rce;
                     break;
                  }
                  VISU_set   (s_reg[x_buf].otab,
                        s_reg[x_buf].begc, s_reg[x_buf].begr,
                        s_reg[x_buf].endc, s_reg[x_buf].endr);
                  CTAB = s_reg[x_buf].otab;
                  CCOL = s_reg[x_buf].begc;
                  CROW = s_reg[x_buf].begr;
                  break;
      default   : my.mode = MODE_MAP;
                  REG_set ('"');
                  return rce;
                  break;
      }
      my.mode = MODE_MAP;
      REG_set ('"');
      return 0;
   }
   /*---(failure)------------------------*/
   --rce;
   my.mode = MODE_MAP;
   REG_set ('"');
   return rce;
}



/*====================------------------------------------====================*/
/*===----                             moving                           ----===*/
/*====================------------------------------------====================*/
static void  o___MOVING__________o () { return; }

char         /*--> tail recursion function for copy ------[ ------ [ ------ ]-*/
REG_tail           (FILE *a_file, char a_type, int *a_seq, int a_level, tCELL *a_curr, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *x_copy      = NULL;
   /*---(defense)------------------------*/
   --rce;  if (a_curr    == NULL)        return rce;     /* no cell                       */
   --rce;  if (a_curr->s == NULL)        return rce;     /* nothing to write              */
   --rce;  if (a_curr->u == a_stamp)     return rce;     /* already written               */
   --rce;  if (a_curr->t == '-')         return rce;     /* don't write, recreate on read */
   /*---(check for bounds)---------------*/
   rc = VISU_selected (a_curr->tab, a_curr->col, a_curr->row);
   --rce;  if (rc == 0)                  return rce;
   /*---(make a copy)--------------------*/
   x_copy = CELL_dup (a_curr);
   --rce;  if (x_copy == NULL)           return rce;
   /*---(move in critical data)----------*/
   strcpy (x_copy->label, a_curr->label);
   x_copy->t   = a_curr->t;
   x_copy->f   = a_curr->f;
   x_copy->d   = a_curr->d;
   x_copy->a   = a_curr->a;
   x_copy->u   = a_stamp;
   a_curr->u   = a_stamp;
   DEBUG_REGS   yLOG_complex ("STAMPED"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d, with %d", a_curr, a_curr->tab, a_curr->col, a_curr->row, a_curr->t, a_curr->u, a_stamp);
   /*---(place in buffer)----------------*/
   rc = REG__hook   (x_copy, my.reg_curr, 'd');
   --rce;  if (rc < 0)                   return rce;
   ++(*a_seq);
   /*---(complete)-----------------------*/
   return 0;
}

char
REG_entry          (char a_reg, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [MAX_STR];
   char        x_toplef    [10];
   char        x_botrig    [10];
   char        x_min       [10];
   char        x_max       [10];
   char        x_size      [10];
   char        x_reach     [10];
   int         x_cells     = 0;
   /*---(beginning)----------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   DEBUG_REGS   yLOG_point   ("a_list"    , a_list);
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) {
      DEBUG_REGS   yLOG_note    ("list is null, no point");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      strlcpy (a_list, empty, 80);
      return rce;
   }
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("register is no good");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      strlcpy (a_list, empty, 80);
      return rce;
   }
   /*---(write line)------------------*/
   if (s_reg [x_reg].nbuf == 0) {
      sprintf (x_line, "  %c   -                                                      -  ", a_reg);
      strlcpy (a_list, x_line, 80);
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write line)------------------*/
   DEBUG_INPT  yLOG_complex ("address"   , "r=%4d, t=%4d, c=%4d, r=%4d", x_reg, s_reg [x_reg].otab, s_reg [x_reg].begc, s_reg [x_reg].begr);
   LOC_ref (s_reg [x_reg].otab, s_reg [x_reg].minc, s_reg [x_reg].minr, 0, x_min   );
   LOC_ref (s_reg [x_reg].otab, s_reg [x_reg].begc, s_reg [x_reg].begr, 0, x_toplef);
   LOC_ref (s_reg [x_reg].otab, s_reg [x_reg].endc, s_reg [x_reg].endr, 0, x_botrig);
   LOC_ref (s_reg [x_reg].otab, s_reg [x_reg].maxc, s_reg [x_reg].maxr, 0, x_max   );
   x_cells = (s_reg [x_reg].endc - s_reg [x_reg].begc + 1) *
      (s_reg [x_reg].endr - s_reg [x_reg].begr + 1);
   sprintf (x_size , "%dx%d",
         s_reg [x_reg].endc - s_reg [x_reg].begc + 1,
         s_reg [x_reg].endr - s_reg [x_reg].begr + 1);
   sprintf (x_reach, "%dx%d",
         s_reg [x_reg].maxc - s_reg [x_reg].minc + 1,
         s_reg [x_reg].maxr - s_reg [x_reg].minr + 1);
   sprintf (x_line , "  %c   %1d  %-5s  %3d  %3d   %-5.5s  %-5.5s %-5s  %-5.5s  %-5.5s  %c  ",
         a_reg, s_reg[x_reg].otab,
         x_size, x_cells, s_reg[x_reg].nbuf, x_toplef + 1, x_botrig + 1,
         x_reach, x_min + 1, x_max + 1,
         s_reg[x_reg].type);
   strlcpy (a_list, x_line, 80);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_list           (char a_buf, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   int         x_buf       = 0;
   int         i           = 0;
   char        rce         = -10;
   tCELL      *curr        = NULL;
   /*---(beginning)----------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_buf"     , a_buf);
   /*---(buffer number)------------------*/
   x_buf  = REG__reg2index  (a_buf);
   DEBUG_REGS   yLOG_value   ("x_buf"     , x_buf);
   --rce;  if (x_buf < 0)  {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(defenses)--------------------*/
   if (a_list  == NULL) return -1;     /* then no point                       */
   /*---(walk the list)---------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_buf].nbuf);
   sprintf (a_list, "%c (%02d) %03d ,", a_buf, x_buf, s_reg[x_buf].nbuf);
   for (i = 0; i < s_reg [x_buf].nbuf; ++i) {
      curr = s_reg[x_buf].buf[i];
      DEBUG_REGS   yLOG_complex ("curr"      , "%2d, %p, %s", i, curr, curr->label);
      if (curr == NULL)  continue;
      strncat (a_list, curr->label, MAX_STR);
      strncat (a_list, ",", MAX_STR);
   }
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> add cells to a register ---------------[ ------ [ ------ ]-*/
REG_save           (char a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   tCELL      *curr        = NULL;
   tCELL      *x_copy      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   long        x_stamp     = 0;
   int         x_seq       = 0;
   char        x_label     [10]        = "";
   int         i           = 0;
   int         x_reg       = 0;
   char        rce         = -10;
   int         x_total     = 0;
   int         x_skipped   = 0;
   int         x_processed = 0;
   int         x_count     = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , my.reg_curr);
   DEBUG_REGS   yLOG_char    ("a_type"    , a_type);
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (my.reg_curr);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("bad register requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(clear existing buffer)----------*/
   if (a_type != 'a') {
      DEBUG_REGS   yLOG_note    ("clear and initialize register");
      REG_clear (my.reg_curr, '-');
      VISU_range (&s_reg[x_reg].otab,
            &s_reg[x_reg].begc, &s_reg[x_reg].begr,
            &s_reg[x_reg].endc, &s_reg[x_reg].endr);
   }
   /*---(copy dependent cells)-----------*/
   x_stamp = rand ();
   x_seq   = 0;
   DEBUG_REGS   yLOG_note    ("DEPENDENT CELLS");
   DEBUG_REGS   yLOG_llong   ("x_stamp"   , x_stamp);
   DEBUG_REGS   yLOG_value   ("x_seq"     , x_seq);
   rc = DEP_tail (NULL, a_type, &x_seq, 0, dtree, x_stamp, REG_tail);
   DEBUG_REGS   yLOG_value   ("x_seq"     , x_seq);
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
   /*---(process independent cells)------*/
   DEBUG_REGS   yLOG_note    ("INDEPENDENT CELLS");
   curr  = VISU_first (&x_tab, &x_col, &x_row);
   /*> s_reg[x_reg].scol = x_col;                                                       <* 
    *> s_reg[x_reg].srow = x_row;                                                       <*/
   while (curr != DONE_DONE) {
      ++x_total;
      DEBUG_REGS   yLOG_point   ("curr"      , curr);
      if (curr == NULL) {
         DEBUG_REGS   yLOG_note    ("skipping, nobody home");
         ++x_skipped;
         curr  = VISU_next (&x_tab, &x_col, &x_row);
         continue;
      }
      DEBUG_REGS   yLOG_complex ("current"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", curr, x_tab, x_col, x_row, curr->t, curr->u);
      if (curr->u == x_stamp) {
         DEBUG_REGS   yLOG_note    ("skipping, timestamp matches, already copied");
         ++x_skipped;
         curr  = VISU_next (&x_tab, &x_col, &x_row);
         continue;
      }
      ++x_processed;
      x_copy = CELL_dup (curr);
      strcpy (x_copy->label, curr->label);
      x_copy->t   = curr->t;
      x_copy->f   = curr->f;
      x_copy->d   = curr->d;
      x_copy->a   = curr->a;
      x_copy->u = x_stamp;
      REG__hook   (x_copy, my.reg_curr, '-');
      ++x_seq;
      DEBUG_REGS   yLOG_note    ("copied");
      curr  = VISU_next (&x_tab, &x_col, &x_row);
   };
   DEBUG_REGS   yLOG_value   ("x_seq"     , x_seq);
   DEBUG_REGS   yLOG_value   ("x_total"   , x_total);
   DEBUG_REGS   yLOG_value   ("x_skipped" , x_skipped);
   DEBUG_REGS   yLOG_value   ("x_process" , x_processed);
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
   /*---(erase originals if needed)------*/
   --rce;
   if (a_type == 'x') {
      x_processed = 0;
      DEBUG_REGS   yLOG_note    ("ERASE CELLS");
      DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
      for (i = s_reg[x_reg].nbuf - 1; i >= 0; --i) {
         rc = LOC_parse (s_reg[x_reg].buf[i]->label, &x_tab, &x_col, &x_row, NULL);
         if (rc < 0) {
            DEBUG_REGS   yLOG_value   ("LOC_parse" , rc);
            DEBUG_REGS   yLOG_exit    (__FUNCTION__);
            return rce;
         }
         curr = tabs[x_tab].sheet[x_col][x_row];
         DEBUG_REGS   yLOG_complex ("erasing"   , "idx=%4d, ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", i, curr, x_tab, x_col, x_row, curr->t, curr->u);
         if (x_count == 0)  rc = CELL__delete (CHG_INPUT   , x_tab, x_col, x_row);
         else               rc = CELL__delete (CHG_INPUTAND, x_tab, x_col, x_row);
         if (rc < 0) {
            DEBUG_REGS   yLOG_value   ("CELL_delet", rc);
            DEBUG_REGS   yLOG_exit    (__FUNCTION__);
            return rce - 1;
         }
         ++x_processed;
         ++x_count;
      }
      DEBUG_REGS   yLOG_value   ("x_process" , x_processed);
   }
   rce -= 1; /* extra code inside loop */
   /*---(clear selection)----------------*/
   VISU_clear ();
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*> cut range into the current buffer -------[ ------ [ ------ ]-*/
REG_copy           (void)
{
   char        rc          = 0;
   rc = REG_save ('-');
   return rc;
}

char         /*> cut range into the current buffer -------[ ------ [ ------ ]-*/
REG_cut            (void)
{
   char        rc          = 0;
   rc = REG_save ('x');
   return rc;
}

char         /*> add range into the current buffer -------[ ------ [ ------ ]-*/
REG_append         (void)
{
   char        rc          = 0;
   rc = REG_save ('a');
   return rc;
}

char               /* PURPOSE : use save range to move cells -----------------*/
REG_paste          (char a_adapt)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;             /* iterator -- sequence           */
   int         x_toff      = 0;
   int         x_xoff      = 0;
   int         x_yoff      = 0;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
   tCELL      *x_copy      = NULL;
   char       *x_tokens    = NULL;
   char       *p           = NULL;
   int         x_ptab      = 0;
   int         x_pcol      = 0;
   int         x_prow      = 0;
   char        x_abs       = 0;
   char        x_addr      [20]        = "";
   char        x_source    [MAX_STR]   = "";
   char        x_bformat   [MAX_STR]   = "";
   int         x_reg       = 0;
   char        rce         = -10;
   int         x_count     = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_buf"     , my.reg_curr);
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (my.reg_curr);
   --rce;  if (x_reg < 0)      return rce;
   /*---(figure offsets)-----------------*/
   x_toff = CTAB - s_reg[x_reg].otab;
   x_xoff = CCOL - s_reg[x_reg].begc;
   x_yoff = CROW - s_reg[x_reg].begr;
   DEBUG_REGS   yLOG_value   ("x_xoff"    , x_xoff);
   DEBUG_REGS   yLOG_value   ("x_yoff"    , x_yoff);
   /*---(move cells in)------------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
   for (i = 0; i < s_reg[x_reg].nbuf; ++i) {
      x_curr = s_reg[x_reg].buf[i];
      if (x_curr == NULL)  continue;
      DEBUG_REGS   yLOG_complex ("entry"     , "i=%3d, %p, %s", i, x_curr, x_curr->label);
      rc = LOC_parse (x_curr->label, &x_tab, &x_col, &x_row, NULL);
      DEBUG_REGS   yLOG_complex ("parsed"    , "tab=%4d, col=%4d, row=%4d", x_tab, x_col, x_row);
      DEBUG_REGS   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", CTAB, x_col + x_xoff, x_row + x_yoff);
      DEBUG_REGS   yLOG_info    ("source"    , x_curr->s);
      DEBUG_REGS   yLOG_char    ("type"      , x_curr->t);
      strcpy (x_source, "");
      if (strchr (G_CELL_RPN, x_curr->t) != 0) {
         DEBUG_REGS   yLOG_note    ("formula, calling yRPN_adjust");
         rc = RPN_adjust (x_curr, x_toff, x_xoff, x_yoff, x_source);
         DEBUG_REGS   yLOG_value   ("rc"        , rc);
         if (rc < 0) {
            DEBUG_REGS   yLOG_note    ("formual could not be parsed");
            strcpy (x_source, "not_translatable");
         }
      } else {
         DEBUG_REGS   yLOG_note    ("just copy straight across");
         strcpy (x_source, x_curr->s);
      }
      DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
      sprintf (x_bformat, "%c%c%c", x_curr->f, x_curr->a, x_curr->d);
      DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
      DEBUG_REGS   yLOG_value   ("len"       , strlen (x_bformat));
      DEBUG_REGS   yLOG_value   ("x_count"   , x_count);
      if (x_count == 0)  x_copy = CELL_overwrite (CHG_OVER   , CTAB, x_col + x_xoff, x_row + x_yoff, x_source, x_bformat);
      else               x_copy = CELL_overwrite (CHG_OVERAND, CTAB, x_col + x_xoff, x_row + x_yoff, x_source, x_bformat);
      if (x_copy == NULL)    continue;
      ++x_count;
   }
   /*---(highlight new range)------------*/
   /*> BCOL  = my.col1_save + col_off;                                                <* 
    *> ECOL  = my.ecol_save + col_off;                                                <* 
    *> BROW  = my.brow_save + row_off;                                                <* 
    *> EROW  = my.erow_save + row_off;                                                <* 
    *> s_reg[x_buf].live = 1;                                                                  <*/
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_valuesin      (char a_style)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   FILE       *f           = NULL;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_lines     = 0;
   char        x_recd      [MAX_STR];
   int         x_len       = 0;
   char       *p           = NULL;
   char       *q           = "\t";
   char       *s           = NULL;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(open output file)---------------*/
   f = fopen("/root/z_gehye/vi_clip.txt", "r");
   DEBUG_REGS   yLOG_point   ("f (file)"  , f);
   --rce;  if (f == NULL) {
      DEBUG_REGS   yLOG_note    ("can not open clip file");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(prepare)------------------------*/
   x_col = CCOL;
   x_row = CROW;
   /*---(process lines)------------------*/
   while (1) {
      /*---(read and clean)--------------*/
      ++x_lines;
      DEBUG_INPT  yLOG_value   ("line"      , x_lines);
      fgets (x_recd, MAX_STR, f);
      if (feof (f))  {
         DEBUG_INPT  yLOG_note    ("end of file reached");
         break;
      }
      x_len = strlen (x_recd);
      if (x_len <= 0)  {
         DEBUG_INPT  yLOG_note    ("record empty");
         continue;
      }
      x_recd [--x_len] = '\0';
      DEBUG_INPT  yLOG_value   ("length"    , x_len);
      DEBUG_INPT  yLOG_info    ("fixed"     , x_recd);
      /*---(process cells)------------------*/
      p = strtok_r (x_recd, q, &s);
      DEBUG_INPT  yLOG_value   ("x_row"     , x_row);
      while (p != NULL) {
         DEBUG_INPT  yLOG_point   ("p"         , p);
         strldchg (p, 29, 166, MAX_STR);   /* group     */
         strldchg (p, 31, 167, MAX_STR);   /* field     */
         strldchg (p,  9, 187, MAX_STR);   /* tab       */
         strldchg (p, 27, 234, MAX_STR);   /* escape    */
         DEBUG_INPT  yLOG_info    ("value"     , p);
         DEBUG_INPT  yLOG_value   ("x_col"     , x_col);
         x_curr = CELL_change (CHG_INPUT, CTAB, x_col, x_row, p);
         DEBUG_INPT  yLOG_point   ("x_curr"    , x_curr);
         ++x_col;
         p = strtok_r (NULL, q, &s);
      }
      x_col = CCOL;
      ++x_row;
   }
   /*---(close file)---------------------*/
   DEBUG_REGS   yLOG_note    ("closing file");
   fclose  (f);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_valuesout     (char a_style)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   FILE       *f           = NULL;
   tCELL      *curr        = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_rowsave   = 0;
   int         w           = 0;
   int         x_print     [MAX_STR];
   int         x_trim      [MAX_STR];
   int         x_source    [MAX_STR];
   int         c           = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(open output file)---------------*/
   f = fopen("/root/z_gehye/vi_clip.txt", "w");
   DEBUG_REGS   yLOG_point   ("f (file)"  , f);
   --rce;  if (f == NULL) {
      DEBUG_REGS   yLOG_note    ("can not open clip file");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process independent cells)------*/
   curr  = VISU_first (&x_tab, &x_col, &x_row);
   x_rowsave = x_row;
   while (curr != DONE_DONE) {
      DEBUG_REGS   yLOG_point   ("curr"      , curr);
      ++c;
      /*---(look for line break)---------*/
      if (strchr ("vVcCtT", a_style) != 0 && x_row != x_rowsave) {
         fprintf (f, "\n");
      }
      /*---(fill in blank cells)---------*/
      if (curr == NULL) {
         w = tabs [x_tab].cols [x_col].w;
         switch (a_style) {
         case 'v' : fprintf (f, "%*.*s", w, w, empty);
                    break;
         case 'V' : break;
         case 'c' : case 'C' :
                    fprintf (f, "\"\",");
                    break;
         case 't' : case 'T' :
                    fprintf (f, "\t");
                    break;
         case 's' : case 'S' :
                    break;
         case 'f' : case 'F' :
                    break;
         }
      }
      /*---(write filled cells)----------*/
      else {
         /*---(printable)----------------*/
         strlcpy  (x_print , curr->p, MAX_STR);
         strldchg (x_print , 166, 29, MAX_STR);   /* group     */
         strldchg (x_print , 167, 31, MAX_STR);   /* field     */
         strldchg (x_print , 187,  9, MAX_STR);   /* tab       */
         strldchg (x_print , 234, 27, MAX_STR);   /* escape    */
         strlcpy  (x_trim  , x_print, MAX_STR);
         /*---(trimmed printable)--------*/
         strltrim (x_trim, ySTR_BOTH, MAX_STR);
         /*---(source)-------------------*/
         strlcpy  (x_source, curr->s, MAX_STR);
         strldchg (x_source, 166, 29, MAX_STR);   /* group     */
         strldchg (x_source, 167, 31, MAX_STR);   /* field     */
         strldchg (x_source, 187,  9, MAX_STR);   /* tab       */
         strldchg (x_source, 234, 27, MAX_STR);   /* escape    */
         switch (a_style) {
         case 'v' : fprintf (f, "%s"                  , x_print);
                    break;
         case 'V' : fprintf (f, "%s "                 , x_trim);
                    break;
         case 'c' : fprintf (f, "\"%s\","             , x_trim);
                    break;
         case 'C' : fprintf (f, "\"%s\","             , x_source);
                    break;
         case 't' : fprintf (f, "%s\t"                , x_trim);
                    break;
         case 'T' : fprintf (f, "%s\t"                , x_source);
                    break;
         case 's' : fprintf (f, "%s\n"                , x_source);
                    break;
         case 'S' : fprintf (f, "(%s) %s\n"           , curr->label, x_source);
                    break;
         case 'f' : fprintf (f, "%-10.10s  %s\n"    , curr->label, x_source);
                    break;
         case 'F' : fprintf (f, "cell_reg    -D-                ");
                    fprintf (f, "%5d  %-8.8s  "   , c, curr->label);
                    fprintf (f, "%c %c %c %c %c  "  , curr->t, curr->f, curr->d, curr->a, curr->n);
                    fprintf (f, "%s\n"                , x_source);
                    break;
         }
      }
      x_rowsave = x_row;
      curr  = VISU_next (&x_tab, &x_col, &x_row);
   };
   /*---(close file)---------------------*/
   DEBUG_REGS   yLOG_note    ("closing file");
   fprintf (f, "\n");
   fclose  (f);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                           files                              ----===*/
/*====================------------------------------------====================*/
static void  o___FILES___________o () { return; }

char
REG_read           (char a_reg, int a_tab, char *a_beg, char *a_end, char *a_min, char *a_max, char a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   char        rc          =   0;           /* generic return code            */
   int         x_reg       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   DEBUG_INPT   yLOG_char    ("a_reg"     , a_reg);
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (a_reg);
   DEBUG_INPT   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(beg)----------------------------*/
   rc = LOC_parse (a_beg, NULL, &x_col, &x_row, NULL);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   s_reg [x_reg].begc = x_col;
   s_reg [x_reg].begr = x_row;
   DEBUG_INPT  yLOG_complex ("address"   , "r=%4d, t=%4d, c=%4d, r=%4d", x_reg, s_reg [x_reg].otab, s_reg [x_reg].begc, s_reg [x_reg].begr);
   /*---(end)----------------------------*/
   rc = LOC_parse (a_end, NULL, &x_col, &x_row, NULL);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   s_reg [x_reg].endc = x_col;
   s_reg [x_reg].endr = x_row;
   DEBUG_INPT  yLOG_complex ("address"   , "t=%4d, c=%4d, r=%4d", a_tab, s_reg [x_reg].endc, s_reg [x_reg].endr);
   /*---(min)----------------------------*/
   rc = LOC_parse (a_min, NULL, &x_col, &x_row, NULL);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   s_reg [x_reg].minc = x_col;
   s_reg [x_reg].minr = x_row;
   DEBUG_INPT  yLOG_complex ("address"   , "t=%4d, c=%4d, r=%4d", a_tab, s_reg [x_reg].minc, s_reg [x_reg].minr);
   /*---(max)----------------------------*/
   rc = LOC_parse (a_max, NULL, &x_col, &x_row, NULL);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   s_reg [x_reg].maxc = x_col;
   s_reg [x_reg].maxr = x_row;
   DEBUG_INPT  yLOG_complex ("address"   , "t=%4d, c=%4d, r=%4d", a_tab, s_reg [x_reg].maxc, s_reg [x_reg].maxr);
   /*---(type)---------------------------*/
   s_reg [x_reg].type = a_type;
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_write          (FILE *a_file, int  *a_seq, char a_buf)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         i           = 0;             /* iterator -- buffer entry       */
   int         x_buf       = 0;
   tCELL      *x_curr      = NULL;
   char        x_regid     [20];
   char        x_toplef    [10];
   char        x_botrig    [10];
   char        x_min       [10];
   char        x_max       [10];
   /*---(buffer number)------------------*/
   x_buf  = REG__reg2index  (a_buf);
   --rce;  if (x_buf < 0)                   return rce;
   --rce;  if (s_reg[x_buf].nbuf <= 0)        return rce;
   /*---(register entry)-----------------*/
   LOC_ref (s_reg [x_buf].otab, s_reg [x_buf].minc, s_reg [x_buf].minr, 0, x_min   );
   LOC_ref (s_reg [x_buf].otab, s_reg [x_buf].begc, s_reg [x_buf].begr, 0, x_toplef);
   LOC_ref (s_reg [x_buf].otab, s_reg [x_buf].endc, s_reg [x_buf].endr, 0, x_botrig);
   LOC_ref (s_reg [x_buf].otab, s_reg [x_buf].maxc, s_reg [x_buf].maxr, 0, x_max   );
   sprintf (x_regid, "%c", a_buf);
   fprintf (a_file, "#---------  ver  ---lvl/reg--  -tab-  --beg---  --end---  --min---  --max---  t \n");
   fprintf (a_file, "register    -A-  %-12.12s    %1d    %-8.8s  %-8.8s  %-8.8s  %-8.8s  %c \n",
         x_regid, s_reg [x_buf].otab,
         x_toplef, x_botrig, x_min, x_max, s_reg [x_buf].type);
   /*---(cell entries)-------------------*/
   for (i = 0; i < s_reg[x_buf].nbuf; ++i) {
      x_curr = s_reg[x_buf].buf[i];
      OUTP_cell (a_file, "cell_reg", *a_seq, x_regid, x_curr);
      ++(*a_seq);
   }
   /*---(complete)-----------------------*/
   fflush (a_file);
   return 0;
}

char
REG_bufwrite       (char a_buf)
{
   /*---(locals)-------------------------*/
   FILE       *f           = NULL;          /* file for writing               */
   char        x_temp      [100];
   char        rce         = -10;           /* return code for errors         */
   int         x_seq       = 0;             /* sequence number                */
   int         i           = 0;             /* iterator -- buffer entry       */
   tCELL      *x_curr      = NULL;
   long        x_stamp     = 0;
   time_t      x_time;
   int         x_buf       = 0;
   /*---(buffer number)------------------*/
   x_buf  = REG__reg2index  (a_buf);
   --rce;  if (x_buf < 0)      return rce;
   /*---(open file)----------------------*/
   f = fopen(FILE_BUF, "w");
   --rce;  if (f == NULL)      return rce;
   fprintf (f, "#!/usr/local/bin/gyges\n");
   fprintf (f, "#   generated by the heatherly spreadsheet tool gyges-hekatonkheires (hundred-handed)\n");
   fprintf (f, "#\n");
   fprintf (f, "#   this is a buffer transfer extract\n");
   fprintf (f, "#\n");
   /*---(header data)--------------------*/
   x_time = time(NULL);
   strftime (x_temp, 100, "%y.%m.%d.%H.%M.%S", localtime(&x_time));
   fprintf (f, "\n\n\n");
   fprintf (f, "#===[[ GENERAL INFO AND SETTINGS ]]===========================================-=======\n");
   fprintf (f, "#--------- %c -ver- %c ---description ------------------------------------------------\n", 31, 31);
   fprintf (f, "gyges      %c %5s %c %-60.60s %c\n", 31, VER_NUM, 31, VER_TXT                      , 31);
   fprintf (f, "format     %c %5d %c %-60.60s %c\n", 31, 6      , 31, "add bcol and brow for tabs" , 31);
   fprintf (f, "timestamp  %c %5s %c %-60.60s %c\n", 31, "-----", 31, x_temp                       , 31);
   /*---(cell header)--------------------*/
   fprintf (f, "\n\n\n");
   fprintf (f, "#===[[ DEPENDENCY TREE, in reverse order ]]===============================================\n");
   /*---(write cells)--------------------*/
   x_stamp = rand ();
   /*> for (i = s_reg[x_buf].nbuf - 1; i >= 0; --i) {                                   <*/
   for (i = 0; i < s_reg[x_buf].nbuf; ++i) {
      /*---(prepare)----------------------*/
      x_curr = s_reg[x_buf].buf[i];
      /*---(print)------------------------*/
      if (x_seq % 5 == 0) {
         fprintf (f, "#--------- %c ---level---- %c -seq- %c ---loc--", 31, 31, 31);
         fprintf (f, " %c t-f-d-a %c ---source-------------------\n", 31, 31);
      }
      fprintf (f, "cell_dep   %c %-12.12s %c %5d %c %-8.8s %c %c %c %c %c %c %s\n",
            31, "------------", 31, x_seq, 31, x_curr->label,
            31, x_curr->t, x_curr->f, x_curr->d, x_curr->a,
            31, x_curr->s);
      ++x_seq;
      x_curr->u = x_stamp;
      fflush  (f);
   }
   /*---(footer data)----------------------*/
   fprintf (f, "\n\n\n");
   fprintf (f, "done\n");
   /*---(close file)-----------------------*/
   fclose  (f);
   /*---(complete)-------------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char         /*--> unit test setter ----------------------[ leaf   [ ------ ]-*/
REG__setter        (char *a_request, char *a_data)
{
   if        (strcmp (a_request, "reg_names"         ) == 0) {
      strlcpy (s_regnames, a_data, MAX_REG);
   } else if (strcmp (a_request, "reg_names_def"      ) == 0) {
      strlcpy (s_regnames, REG_NAMES, MAX_REG);
   } else if (strcmp (a_request, "reg_curr"           ) == 0) {
      my.reg_curr = a_data [0];
   } else {
      return -1;
   }
   return 0;
}

char*        /*--> unit test accessor --------------------[ leaf   [ ------ ]-*/
REG__getter        (char *a_question, char a_reg)
{
   /*---(locals)-----------+-----------+-*/
   int         x_reg       = 0;
   char        x_list      [MAX_STR];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_reg            : question not understood");
   /*---(buffer number)------------------*/
   if (a_reg != 0) {
      x_reg = REG__reg2index  (a_reg);
   }
   /*---(defenses)-----------------------*/
   if      (x_reg < 0) {
      snprintf (unit_answer, LEN_TEXT, "s_reg error      : register reference (%c) is not valid", a_reg);
   }
   /*---(register list)------------------*/
   else if (strcmp (a_question, "reg_names"    )  == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_reg names      : %-.45s", s_regnames);
   }
   else if (strcmp (a_question, "reg_count"    )  == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_reg count      : %d", strlen (s_regnames));
   }
   /*---(register list)------------------*/
   else if (strcmp (a_question, "reg_sort"     )  == 0) {
      REG_list  (a_reg, x_list);
      if (strlen (x_list) > 12)  DEP_gnome (x_list + 11);
      snprintf (unit_answer, LEN_TEXT, "s_reg sort %-.45s", x_list);
   }
   else if (strcmp (a_question, "reg_list"     )  == 0) {
      REG_list  (a_reg, x_list);
      snprintf (unit_answer, LEN_TEXT, "s_reg list %-.45s", x_list);
   }
   else if (strcmp (a_question, "reg_range"    )  == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_reg range      : %c, ta=%4d, bc=%4d, br=%4d, ec=%4d, er=%4d", a_reg, s_reg[x_reg].otab, s_reg[x_reg].begc, s_reg[x_reg].begr, s_reg[x_reg].endc, s_reg[x_reg].endr);
   }
   else if (strcmp (a_question, "reg_reach"    )  == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_reg reach      : %c, ta=%4d, nc=%4d, nr=%4d, xc=%4d, xr=%4d", a_reg, s_reg[x_reg].otab, s_reg[x_reg].minc, s_reg[x_reg].minr, s_reg[x_reg].maxc, s_reg[x_reg].maxr);
   }
   else if (strcmp (a_question, "reg_buffer")     == 0) {
      snprintf (unit_answer, LEN_TEXT, "s_reg buffer (%c) : (%02d) r=%3d, n=%3d, h=%10p, t=%10p", a_reg, x_reg, s_reg[x_reg].real, s_reg[x_reg].nbuf, s_reg[x_reg].buf[0], s_reg[x_reg].buf[s_reg[x_reg].nbuf - 1]);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*===============================[[ end code ]]===============================*/
