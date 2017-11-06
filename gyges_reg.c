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
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(registers)----------------------*/
   strlcpy (s_regnames , REG_NAMES, MAX_REG);
   REG_purge    ('y');
   TREG_init    ();
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> clear out all buffers -----------------[ ------ [ ------ ]-*/
REG_purge          (char a_init)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(clear)--------------------------*/
   my.reg_curr  = '"';
   x_len = strlen (s_regnames);
   for (i = 0; i < x_len; ++i) {
      REG_clear (s_regnames[i], a_init);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
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
      my.layout_status = G_STATUS_REGS;
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
REG_mode           (int a_major, int a_minor)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_buf       =   -1;
   char       *p           = NULL;
   /*---(defenses)-----------------------*/
   --rce;  if (yVIKEYS_mode_not (SMOD_REGISTER)) {
      return rce;
   }
   if (a_minor == K_ESCAPE)  {
      yVIKEYS_mode_exit ();
      return  0;
   }
   /*---(check for control keys)---------*/
   --rce;  if (a_major == '"') {
      if (strchr (s_regnames, a_minor) != 0) {
         REG_set    (a_minor);
         return 0;
      } else if (a_minor == '?') {
         my.info_win = G_INFO_REGS;
         REG_set ('"');
         /*> yVIKEYS_mode_exit ();                                                          <*/
         return  0;
      } else if (a_minor == '!') {
         my.layout_status = G_STATUS_REGS;
         REG_set ('"');
         yVIKEYS_mode_exit ();
         return  0;
      }
      return rce;
   }
   --rce;  if (a_major == ' ' && my.reg_curr == '+') {
      p = strchr ("vsctrVSCTR", a_minor);
      if        (p != NULL) {
         REG_inpt_driver (a_minor);
      }
      REG_set ('"');
      yVIKEYS_mode_exit ();
      if (p == NULL)   return rce;
      return 0;
   }
   --rce;  if (a_major == ' ' && my.reg_curr == '-') {
      switch (a_minor) {
      case  'v' : REG_valuesout('v');
                  break;
      case  'V' : REG_valuesout('V');
                  break;
      case  'r' : REG_valuesout('r');
                  break;
      case  'm' : REG_valuesout('m');
                  break;
      case  'M' : REG_valuesout('M');
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
                  REG_inpt_driver ('-');
                  break;
      default   : REG_set ('"');
                  yVIKEYS_mode_exit ();
                  return rce;
                  break;
      }
      REG_set ('"');
      yVIKEYS_mode_exit ();
      return 0;
   }
   --rce;  if (a_major == ' ') {
      switch (a_minor) {
      case  '#' : REG_clear (my.reg_curr, '-');
                  break;
      case  'y' : REG_copy  ();
                  break;
      case  'p' : REG_paste (G_PASTE_NORM);
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
      default   : REG_set ('"');
                  yVIKEYS_mode_exit ();
                  return rce;
                  break;
      }
      REG_set ('"');
      yVIKEYS_mode_exit ();
      return 0;
   }
   /*---(failure)------------------------*/
   --rce;
   REG_set ('"');
   yVIKEYS_mode_exit ();
   return rce;
}


/*====================------------------------------------====================*/
/*===----                             moving                           ----===*/
/*====================------------------------------------====================*/
static void  o___MOVING__________o () { return; }

char         /*--> tail recursion function for copy ------[ ------ [ ------ ]-*/
REG_deps           (tCELL *a_curr, long a_stamp)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   tCELL      *x_copy      = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr    == NULL) {
      DEBUG_REGS   yLOG_note    ("original cell null");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* no cell                       */
   }
   DEBUG_REGS   yLOG_point   ("s"         , a_curr->s);
   --rce;  if (a_curr->s == NULL) {
      DEBUG_REGS   yLOG_note    ("no source");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* nothing to write              */
   }
   DEBUG_REGS   yLOG_char    ("t"         , a_curr->t);
   --rce;  if (a_curr->t == '-')  {
      DEBUG_REGS   yLOG_note    ("could not copy an empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;     /* don't write, recreate on read */
   }
   /*---(check for bounds)---------------*/
   rc = VISU_selected (a_curr->tab, a_curr->col, a_curr->row);
   DEBUG_REGS   yLOG_value   ("visu_rc"   , rc);
   --rce;  if (rc == 0)  {
      DEBUG_REGS   yLOG_note    ("could not get cell");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(make a copy)--------------------*/
   rc = CELL_dup (&x_copy, a_curr);
   DEBUG_REGS   yLOG_value   ("dup_rc"    , rc);
   --rce;  if (rc < 0 ) {
      DEBUG_REGS   yLOG_note    ("could not duplicate");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(move in critical data)----------*/
   strlcpy (x_copy->label, a_curr->label, LEN_LABEL);
   a_curr->u   = a_stamp;
   DEBUG_REGS   yLOG_complex ("STAMPED"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d, with %d", a_curr, a_curr->tab, a_curr->col, a_curr->row, a_curr->t, a_curr->u, a_stamp);
   /*---(place in buffer)----------------*/
   rc = REG__hook   (x_copy, my.reg_curr, 'd');
   DEBUG_REGS   yLOG_value   ("hook_rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_note    ("could not hook to register");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_entry          (char a_reg, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [LEN_RECD];
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
      strlcpy (a_list, g_empty, 80);
      return rce;
   }
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("register is no good");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      strlcpy (a_list, g_empty, 80);
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
      strncat (a_list, curr->label, LEN_RECD);
      strncat (a_list, ",", LEN_RECD);
   }
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> add cells to a register ---------------[ ------ [ ------ ]-*/
REG_save             (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   tCELL      *x_curr      = NULL;
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
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (my.reg_curr);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("bad register requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(clear existing buffer)----------*/
   DEBUG_REGS   yLOG_note    ("clear and initialize register");
   REG_clear (my.reg_curr, '-');
   VISU_range (&s_reg[x_reg].otab,
         &s_reg[x_reg].begc, &s_reg[x_reg].begr,
         &s_reg[x_reg].endc, &s_reg[x_reg].endr);
   /*---(copy dependent cells)-----------*/
   x_stamp = rand ();
   x_seq   = 0;
   DEBUG_REGS   yLOG_note    ("DEPENDENT CELLS");
   DEBUG_REGS   yLOG_llong   ("x_stamp"   , x_stamp);
   DEBUG_REGS   yLOG_value   ("x_seq"     , x_seq);
   rc = SEQ_reg_deps (x_stamp);
   DEBUG_REGS   yLOG_value   ("x_seq"     , x_seq);
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
   /*---(process independent cells)------*/
   DEBUG_REGS   yLOG_note    ("INDEPENDENT CELLS");
   x_curr  = VISU_first (&x_tab, &x_col, &x_row);
   /*> s_reg[x_reg].scol = x_col;                                                       <* 
    *> s_reg[x_reg].srow = x_row;                                                       <*/
   while (x_curr != DONE_DONE) {
      ++x_total;
      DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr == NULL) {
         DEBUG_REGS   yLOG_note    ("skipping, nobody home");
         ++x_skipped;
         x_curr  = VISU_next (&x_tab, &x_col, &x_row);
         continue;
      }
      DEBUG_REGS   yLOG_complex ("current"   , "ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", x_curr, x_tab, x_col, x_row, x_curr->t, x_curr->u);
      if (x_curr->u == x_stamp) {
         DEBUG_REGS   yLOG_note    ("skipping, timestamp matches, already copied");
         ++x_skipped;
         x_curr  = VISU_next (&x_tab, &x_col, &x_row);
         continue;
      }
      ++x_processed;
      x_copy  = NULL;
      rc = CELL_dup (&x_copy, x_curr);
      strlcpy (x_copy->label, x_curr->label, LEN_LABEL);
      x_copy->u   = x_stamp;
      rc = REG__hook   (x_copy, my.reg_curr, '-');
      ++x_seq;
      DEBUG_REGS   yLOG_note    ("copied");
      x_curr  = VISU_next (&x_tab, &x_col, &x_row);
   };
   DEBUG_REGS   yLOG_value   ("x_seq"     , x_seq);
   DEBUG_REGS   yLOG_value   ("x_total"   , x_total);
   DEBUG_REGS   yLOG_value   ("x_skipped" , x_skipped);
   DEBUG_REGS   yLOG_value   ("x_process" , x_processed);
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char           /*-> delete the original cells ----------[ ------ [----------]-*/
REG_delorig          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   int         x_reg       =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   tCELL      *x_curr      = NULL;
   int         x_processed =    0;
   int         x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , my.reg_curr);
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (my.reg_curr);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("bad register requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(erase originals if needed)------*/
   --rce;
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
      x_curr = LOC_cell_at_loc (x_tab, x_col, x_row);
      DEBUG_REGS   yLOG_complex ("erasing"   , "idx=%4d, ptr=%p, tab=%4d, col=%4d, row=%4d, t=%c, u=%d", i, x_curr, x_tab, x_col, x_row, x_curr->t, x_curr->u);
      if (x_count == 0)  rc = CELL_delete (CHG_INPUT   , x_tab, x_col, x_row);
      else               rc = CELL_delete (CHG_INPUTAND, x_tab, x_col, x_row);
      if (rc < 0) {
         DEBUG_REGS   yLOG_value   ("CELL_delet", rc);
         DEBUG_REGS   yLOG_exit    (__FUNCTION__);
         return rce - 1;
      }
      ++x_processed;
      ++x_count;
   }
   DEBUG_REGS   yLOG_value   ("x_process" , x_processed);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char           /*-> charge provider references ---------[ ------ [----------]-*/
REG__paste_pros      (char a_pros)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   int         x_reg       =    0;               /* focus register            */
   int         x_atab      =    0;               /* tab adjustment            */
   int         x_acol      =    0;               /* col adjustment            */
   int         x_arow      =    0;               /* row adjustment            */
   int         i           =    0;               /* iterator -> register pos  */
   char        x_label     [LEN_LABEL] = "";     /* label of register cell    */
   int         x_otab      =    0;               /* pre-reg original tab      */
   int         x_ocol      =    0;               /* pre-reg original col      */
   int         x_orow      =    0;               /* pre-reg original row      */
   char        x_list      [LEN_RECD ] = "";     /* list of all providers     */
   char       *p           = NULL;               /* pointer for providers     */
   char       *q           = ",";                /* delimiter for providers   */
   char       *s           = NULL;               /* context for providers     */
   tCELL      *x_provider  = NULL;               /* provider cell to adjust   */
   char        x_source    [LEN_RECD ] = "";     /* updated provider source   */
   char        x_bformat   [LEN_RECD ] = "";     /* updated provider format   */
   int         x_processed =    0;
   int         x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_buf"     , my.reg_curr);
   /*---(defense)------------------------*/
   if (a_pros  != 'y') {
      DEBUG_REGS   yLOG_note    ("not altering providing refs");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (my.reg_curr);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("bad register requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(figure offsets)-----------------*/
   x_atab = CTAB - s_reg[x_reg].otab;
   x_acol = CCOL - s_reg[x_reg].begc;
   x_arow = CROW - s_reg[x_reg].begr;
   DEBUG_REGS   yLOG_value   ("x_atab"    , x_atab);
   DEBUG_REGS   yLOG_value   ("x_acol"    , x_acol);
   DEBUG_REGS   yLOG_value   ("x_arow"    , x_arow);
   /*---(buffer number)------------------*/
   x_reg  = REG__reg2index  (my.reg_curr);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("bad register requested");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(erase originals if needed)------*/
   --rce;
   x_processed = 0;
   DEBUG_REGS   yLOG_note    ("walk through buffer");
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg[x_reg].nbuf);
   for (i = s_reg[x_reg].nbuf - 1; i >= 0; --i) {
      DEBUG_REGS   yLOG_value   ("i"         , i);
      DEBUG_REGS   yLOG_point   ("label"     , s_reg[x_reg].buf[i]->label);
      if (s_reg[x_reg].buf[i]->label == NULL)  continue;
      strcpy (x_label, s_reg[x_reg].buf[i]->label);
      DEBUG_REGS   yLOG_info    ("label"     , x_label);
      rc = LOC_parse (x_label, &x_otab, &x_ocol, &x_orow, NULL);
      DEBUG_REGS   yLOG_value   ("rc"        , rc);
      DEBUG_REGS   yLOG_value   ("x_otab"    , x_otab);
      DEBUG_REGS   yLOG_value   ("x_ocol"    , x_ocol);
      DEBUG_REGS   yLOG_value   ("x_orow"    , x_orow);
      if (rc < 0) {
         DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEP_disp_pros (LOC_cell_at_loc (x_otab, x_ocol, x_orow), x_list);
      DEBUG_REGS   yLOG_info    ("x_list"    , x_list);
      if (x_list [0] == '-')  continue;
      if (x_list [0] == '.')  continue;
      p  = strtok_r (x_list, q, &s);
      DEBUG_REGS   yLOG_point   ("p"         , p);
      while (p != NULL) {
         DEBUG_REGS   yLOG_info    ("p"         , p);
         x_provider = LOC_cell_labeled (p);
         DEBUG_REGS   yLOG_point   ("x_provider", x_provider);
         if (x_provider == NULL) {
            p  = strtok_r (NULL  , q, &s);
            continue;
         }
         rc = RPN_adjust_ref (x_provider, a_pros, x_atab, x_acol, x_arow, x_source, x_label);
         DEBUG_REGS   yLOG_info    ("x_source"  , x_source);
         sprintf (x_bformat, "%c%c%c", x_provider->f, x_provider->a, x_provider->d);
         DEBUG_REGS   yLOG_info    ("x_bformat" , x_bformat);
         if (x_count == 0)  CELL_overwrite (CHG_OVER   , x_provider->tab, x_provider->col, x_provider->row, x_source, x_bformat);
         else               CELL_overwrite (CHG_OVERAND, x_provider->tab, x_provider->col, x_provider->row, x_source, x_bformat);
         p  = strtok_r (NULL  , q, &s);
      }
      ++x_processed;
      ++x_count;
   }
   DEBUG_REGS   yLOG_value   ("x_process" , x_processed);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*> cut range into the current buffer -------[ ------ [ ------ ]-*/
REG_copy           (void)
{
   char        rc          = 0;
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   rc = REG_save    ();
   rc = VISU_clear ();
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return rc;
}

char         /*> cut range into the current buffer -------[ ------ [ ------ ]-*/
REG_cut            (void)
{
   char        rc          = 0;
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   rc = REG_save    ();
   rc = REG_delorig ();
   rc = VISU_clear ();
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return rc;
}

short       s_index        =    0;
short       s_atab         =    0;
short       s_acol         =    0;
short       s_arow         =    0;

char           /*-> prepare for a paste ----------------[ ------ [----------]-*/
REG__paste_check     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_reg       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   s_index   =   -1;
   s_atab    =    0;
   s_acol    =    0;
   s_arow    =    0;
   /*---(buffer number)------------------*/
   DEBUG_REGS   yLOG_char    ("reg_curr"  , my.reg_curr);
   x_reg  = REG__reg2index  (my.reg_curr);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0) {
      DEBUG_REGS   yLOG_note    ("bad register requested");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg [x_reg].nbuf);
   --rce;  if (s_reg [x_reg].nbuf <= 0) {
      DEBUG_REGS   yLOG_note    ("register is empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set the offsets)----------------*/
   s_index = x_reg;
   s_atab  = CTAB - s_reg [s_index].otab;
   s_acol  = CCOL - s_reg [s_index].begc;
   s_arow  = CROW - s_reg [s_index].begr;
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char           /*-> clear the destination area ---------[ ------ [----------]-*/
REG__paste_clear     (char a_clear)
{
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   if (a_clear != 'y') {
      DEBUG_REGS   yLOG_note    ("not clearing the destination area");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(select)-------------------------*/
   DEBUG_REGS   yLOG_note    ("visually select the desination area");
   VISU_set   (
         s_reg [s_index].otab + s_atab,
         s_reg [s_index].begc + s_acol, s_reg [s_index].begr + s_arow,
         s_reg [s_index].endc + s_acol, s_reg [s_index].endr + s_arow);
   /*---(clear)--------------------------*/
   DEBUG_REGS   yLOG_note    ("erase the selection");
   CELL_erase ();
   DEBUG_REGS   yLOG_note    ("clear the visual selection");
   VISU_clear ();
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char               /* PURPOSE : use save range to move cells -----------------*/
REG__paste_cells     (char a_reqs)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   int         i           = 0;             /* iterator -- sequence           */
   int         x_stab      = 0;             /* source tab                     */
   int         x_scol      = 0;             /* source col                     */
   int         x_srow      = 0;             /* source row                     */
   int         x_dtab      = 0;             /* destination tab                */
   int         x_dcol      = 0;             /* destination col                */
   int         x_drow      = 0;             /* destination row                */
   tCELL      *x_curr      = NULL;
   tCELL      *x_copy      = NULL;
   char        x_source    [LEN_RECD]   = "";
   char        x_bformat   [LEN_RECD]   = "";
   int         x_count     = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(move cells in)------------------*/
   DEBUG_REGS   yLOG_value   ("nbuf"      , s_reg [s_index].nbuf);
   for (i = 0; i < s_reg [s_index].nbuf; ++i) {
      x_curr = s_reg [s_index].buf[i];
      if (x_curr == NULL)  continue;
      DEBUG_REGS   yLOG_complex ("entry"     , "i=%3d, %p, %s", i, x_curr, x_curr->label);
      rc = LOC_parse (x_curr->label, &x_stab, &x_scol, &x_srow, NULL);
      DEBUG_REGS   yLOG_complex ("parsed"    , "tab=%4d, col=%4d, row=%4d", x_stab, x_scol, x_srow);
      x_dtab  = x_stab + s_atab;
      x_dcol  = x_scol + s_acol;
      x_drow  = x_srow + s_arow;
      DEBUG_REGS   yLOG_complex ("going to"  , "tab=%4d, col=%4d, row=%4d", x_dtab, x_dcol, x_drow);
      DEBUG_REGS   yLOG_info    ("source"    , x_curr->s);
      DEBUG_REGS   yLOG_char    ("type"      , x_curr->t);
      strcpy (x_source, "");
      if (strchr (G_CELL_RPN, x_curr->t) != 0 && a_reqs == 'y') {
         DEBUG_REGS   yLOG_note    ("formula, calling yRPN_adjust");
         rc = RPN_adjust (x_curr, G_RPN_NORM, s_atab, s_acol, s_arow, x_source);
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
      if (x_count == 0)  x_copy = CELL_overwrite (CHG_OVER   , x_dtab, x_dcol, x_drow, x_source, x_bformat);
      else               x_copy = CELL_overwrite (CHG_OVERAND, x_dtab, x_dcol, x_drow, x_source, x_bformat);
      if (x_copy == NULL)    continue;
      ++x_count;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char           /*-> merge cells into providers ---------[ ------ [----------]-*/
REG__paste_integ     (char a_integ)
{
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   if (a_integ != 'y') {
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char           /*-> main paste driver ------------------[ ------ [----------]-*/
REG__paste_main      (char a_clear, char a_reqs, char a_pros, char a_integ)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_clear"   , a_clear);
   DEBUG_REGS   yLOG_char    ("a_reqs"    , a_reqs );
   DEBUG_REGS   yLOG_char    ("a_pros"    , a_pros );
   DEBUG_REGS   yLOG_char    ("a_integ"   , a_integ);
   /*---(run)----------------------------*/
   rc = REG__paste_check  ();
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = REG__paste_clear  (a_clear);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = REG__paste_cells  (a_reqs);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = REG__paste_pros   (a_pros);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = REG__paste_integ  (a_integ);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char           /*-> most common paste call -------------[ ------ [----------]-*/
REG_paste            (char a_type)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_type"    , a_type);
   /*---(check type)---------------------*/
   switch (a_type) {
   case G_PASTE_NORM   :
      rc = REG__paste_main ('-', 'y', '-', '-');
      break;
   case G_PASTE_REPL   :
      rc = REG__paste_main ('y', 'y', '-', '-');
      break;
   case G_PASTE_DUPL   :
      rc = REG__paste_main ('-', '-', '-', '-');
      break;
   case G_PASTE_MOVE   :
      rc = REG__paste_main ('-', '-', 'r', '-');
      break;
   case G_PASTE_FORCE  :
      rc = REG__paste_main ('-', '-', 'a', '-');
      break;
   default             :
      rc = -50;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         data import                          ----===*/
/*====================------------------------------------====================*/
static void  o___IMPORT__________o () { return; }

static FILE   *s_clip     = NULL;
static int     s_lines    =    0;
static char    s_q        [5]         = "";

static char    s_recd     [LEN_RECD]  = "";
static int     s_max      =    0;

static char    s_sizer    =  '-';
static char    s_style    =  '-';
static char    s_mapper   =  '-';
static short   s_map      [MAX_COLS];

static short   s_icol     =    0;
static short   s_mcol     =    0;
static short   s_ccol     =    0;
static short   s_crow     =    0;

char         /*--> prepare for data import ---------------[ leaf-- [ ------ ]-*/
REG__inpt_prep       (char a_style)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(save style)---------------------*/
   DEBUG_REGS   yLOG_note    ("save style");
   s_style  = a_style;
   /*---(clear sizer)--------------------*/
   DEBUG_REGS   yLOG_note    ("clear sizer");
   s_sizer  = '-';
   /*---(clear sizer)--------------------*/
   DEBUG_REGS   yLOG_note    ("clear map");
   s_mapper = '-';
   for (i = 0; i < MAX_COLS; ++i)  s_map [i] = 0;
   /*---(prepare)------------------------*/
   DEBUG_REGS   yLOG_note    ("prepare positions");
   s_icol  =    0;
   s_mcol  =    0;
   s_ccol  =    0;
   s_crow  =    0;
   s_lines =    0;
   /*---(delimiter)----------------------*/
   DEBUG_REGS   yLOG_note    ("set delimiter");
   switch (s_style) {
   case 's'  : case 'S'  :
      strlcpy (s_q, " "  , 5);
      break;
   case 'c'  : case 'C'  :
      strlcpy (s_q, ","  , 5);
      break;
   case 't'  : case 'T'  :
      strlcpy (s_q, "\t" , 5);
      break;
   case 'r'  : case 'R'  :
      strlcpy (s_q, "" , 5);
      break;
   case 'v'  : case 'V'  :
      strlcpy (s_q, " "  , 5);
      break;
   default   :
      return rce;
      break;
   }
   /*---(sizer)--------------------------*/
   DEBUG_REGS   yLOG_note    ("set sizer value");
   switch (s_style) {
   case 'T' : case 'C' : case 'R' : case 'V' : case 'S' :
      s_sizer = 'a';
      break;
   case 'v' :
      s_sizer = 'f';
      break;
   default  :
      s_sizer = '-';
      break;
   }
   /*---(open output file)---------------*/
   DEBUG_REGS   yLOG_note    ("open clip file");
   s_clip = fopen("/root/z_gehye/vi_clip.txt", "r");
   DEBUG_REGS   yLOG_point   ("s_clip"    , s_clip);
   --rce;  if (s_clip == NULL) {
      DEBUG_REGS   yLOG_note    ("can not open clip file");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> process a mapping request -------------[ leaf-- [ ------ ]-*/
REG__inpt_map        (void)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   tCELL      *x_curr      = NULL;
   short       x_ccol      =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("s_style"   , s_style);
   if (s_style == 'v') {
      DEBUG_REGS   yLOG_note    ("mapping not allowed in this import style");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(import mapping)-----------------*/
   DEBUG_REGS   yLOG_note    ("start mapping");
   for (i = 0; i < MAX_COLS; ++i) {
      x_ccol = CCOL + i;
      DEBUG_REGS   yLOG_value   ("x_ccol"    , x_ccol);
      x_curr = LOC_cell_at_loc (CTAB, x_ccol, CROW);
      DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
      if (x_curr == NULL)  {
         DEBUG_REGS   yLOG_note    ("end of mapping (null cell)");
         DEBUG_REGS   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      DEBUG_REGS   yLOG_value   ("v_num"     , x_curr->v_num);
      if (x_curr->v_num <= 0) {
         DEBUG_REGS   yLOG_note    ("end of mapping (zero or negative cell)");
         DEBUG_REGS   yLOG_exit    (__FUNCTION__);
         return 0;
      }
      s_mapper = 'y';
      s_crow   = CROW + 1;
      s_map [i] = x_curr->v_num;
      DEBUG_REGS   yLOG_pair    (i          , s_map [i] );
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> read an import record -----------------[ leaf-- [ ------ ]-*/
REG__inpt_read       (void)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(read and clean)-----------------*/
   ++s_lines;
   DEBUG_REGS   yLOG_value   ("line"      , s_lines);
   fgets (s_recd, LEN_RECD, s_clip);
   if (feof (s_clip))  {
      DEBUG_REGS   yLOG_note    ("end of file reached");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (s_recd);
   if (x_len <= 0)  {
      DEBUG_REGS   yLOG_note    ("record is empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   s_recd [--x_len] = '\0';
   DEBUG_REGS   yLOG_value   ("length"    , x_len);
   DEBUG_REGS   yLOG_info    ("fixed"     , s_recd);
   if (s_recd [0] == '\0') {
      DEBUG_REGS   yLOG_note    ("starts with a null");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   if (s_recd [0] == ' ' ) {
      DEBUG_REGS   yLOG_note    ("starts with a space");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   if (s_recd [0] == '#' && strchr ("<>=", s_recd [1]) == NULL) {
      DEBUG_REGS   yLOG_note    ("starts with a comment marker");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   s_max = x_len;
   DEBUG_REGS   yLOG_value   ("s_max"     , s_max);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> adjust a column sizing ----------------[ leaf-- [ ------ ]-*/
REG__inpt_width      (short a_col, short a_wide)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rce         =  -10;
   short       w           =   0;
   /*---(defense)------------------------*/
   --rce;  if (s_sizer != 'a')      return rce;
   /*---(resize)-------------------------*/
   w = LOC_col_width (CTAB, CCOL + a_col);
   --rce;  if (w >= a_wide + 1)     return rce;
   LOC_col_widen (CTAB, CCOL + a_col, a_wide + 1);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> process a column sizing request -------[ leaf-- [ ------ ]-*/
REG__inpt_sizer      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char       *p           = NULL;
   char       *s           = NULL;
   int         x_len       =    0;
   short       x_col       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_value   ("s_sizer"   , s_sizer);
   --rce;  if (s_sizer != 'a') {
      DEBUG_REGS   yLOG_note    ("wrong type, only 'a' allowed");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("s_recd"    , s_recd);
   --rce;  if ((s_style != 'C' && strncmp ("#<34", s_recd, 4) != 0) ||
         (s_style == 'C' && strncmp ("\"#<34", s_recd, 5) != 0)) {
      DEBUG_REGS   yLOG_note    ("not a sizing line");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process)------------------------*/
   p  = strtok_r (s_recd, s_q, &s);
   while (p != NULL) {
      DEBUG_REGS   yLOG_value   ("x_col"     , x_col);
      x_len = strlen (p);
      if (s_style == 'C' && x_len >= 3)  p [0] = p [x_len - 1] = ' ';
      strltrim (p, ySTR_BOTH, LEN_RECD);
      DEBUG_REGS   yLOG_info    ("p (field)" , p);
      x_len = strlen (p);
      DEBUG_REGS   yLOG_value   ("x_len"     , x_len);
      if (x_len == 0)  break;  /* stop at unsized column */
      LOC_col_widen (CTAB, CCOL + x_col, x_len + 1);
      ++x_col;
      p  = strtok_r (NULL, s_q, &s);
   }
   /*---(update)-------------------------*/
   DEBUG_REGS   yLOG_note    ("mark sizer processed");
   s_sizer  = 's';
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> place import data into a cell ---------[ leaf-- [ ------ ]-*/
REG__inpt_place      (short a_col, short a_row, char *a_value)
{
   /*---(locals)-----------+-----+-----+-*/
   /*---(non-mapped)---------------------*/
   if (s_mapper == '-') {
      CELL_change (NULL, CHG_INPUT, CTAB, CCOL + a_col, CROW + a_row, a_value);
   }
   /*---(mapped)-------------------------*/
   else {
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> complete data import ------------------[ leaf-- [ ------ ]-*/
REG__inpt_done       (void)
{
   /*---(close file)---------------------*/
   DEBUG_REGS   yLOG_note    ("closing file");
   fclose  (s_clip);
   /*---(screen)-------------------------*/
   CURS_screen_reset ();
   /*---(calculate)----------------------*/
   DEBUG_INPT  yLOG_note    ("recalc");
   SEQ_calc_full ();
   /*---(complete)-----------------------*/
   return 0;
}

char
REG__inpt_values     (short a_row)
{ 
   /*---(locals)-----+-----+-----+-----+-*/
   short       x_col       =    0;
   int         w           =    0;
   int         cw          =    0;
   char        t           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(process cells)---------------*/
   while (cw < s_max) {
      /*---(read import data)---------*/
      w     = LOC_col_width (CTAB, CCOL + x_col);
      DEBUG_REGS  yLOG_value   ("w"         , w);
      strlcpy (t, s_recd + cw, w);
      DEBUG_REGS  yLOG_info    ("t (orig)"  , t);
      /*---(process)------------------*/
      strltrim (t, ySTR_BOTH, LEN_RECD);
      DEBUG_REGS  yLOG_info    ("t (trim)"  , t);
      strlencode   (t, LEN_RECD);
      DEBUG_REGS  yLOG_info    ("t (new)"   , t);
      DEBUG_REGS  yLOG_value   ("x_col"     , x_col);
      /*---(change)-------------------*/
      REG__inpt_place  (x_col, a_row, t);
      /*---(next)---------------------*/
      ++x_col;
      cw += w;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG__inpt_delims     (short a_row)
{
   /*---(locals)-----+-----+-----+-----+-*/
   short       x_col       =    0;
   char       *p           = NULL;
   char       *s           = NULL;
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(parse first)-----------------*/
   p  = strtok_r (s_recd, s_q, &s);
   /*---(process cells)---------------*/
   while (p != NULL) {
      /*---(read import data)---------*/
      DEBUG_REGS  yLOG_info    ("p (orig)"  , p);
      x_len = strlen (p);
      DEBUG_REGS  yLOG_value   ("x_len"     , x_len);
      /*---(process)------------------*/
      if (s_style == 'c' && x_len >= 3)  p [0] = p [x_len - 1] = ' ';
      strltrim (p, ySTR_BOTH, LEN_RECD);
      DEBUG_REGS  yLOG_info    ("p (trim)"  , p);
      x_len = strlen (p);
      DEBUG_REGS  yLOG_value   ("x_len"     , x_len);
      if (s_sizer == 'a')  REG__inpt_width (x_col, x_len);
      strlencode   (p, LEN_RECD);
      DEBUG_REGS  yLOG_info    ("p (new)"   , p);
      DEBUG_REGS  yLOG_value   ("x_col"     , x_col);
      REG__inpt_place  (x_col, a_row, p);
      /*---(parse next)---------------*/
      p  = strtok_r (NULL, s_q, &s);
      ++x_col;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
REG_inpt_driver      (char a_style)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   short       x_row       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   rc = REG__inpt_prep   (a_style);
   DEBUG_REGS   yLOG_value   ("prep rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = REG__inpt_map ();
   /*---(process lines)------------------*/
   while (1) {
      /*---(read)------------------------*/
      rc = REG__inpt_read ();
      if (rc >  0)  continue;
      if (rc <  0)  break;
      /*---(process cells)---------------*/
      DEBUG_REGS  yLOG_value   ("x_row"     , x_row);
      /*---(check sizer)-----------------*/
      if (strncmp (s_recd, "#<34"  , 4) == 0) {
         REG__inpt_sizer ();
         continue;
      }
      /*---(handle values)---------------*/
      if (strchr ("Vv", s_style) != NULL) {
         REG__inpt_values  (x_row);
      } else {
         REG__inpt_delims  (x_row);
      }
      /*---(prepare for next)------------*/
      ++x_row;
   }
   /*---(wrap up)------------------------*/
   rc = REG__inpt_done ();
   DEBUG_REGS   yLOG_value   ("done rc"   , rc);
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
   char        x_print     [LEN_RECD]  = "";
   char        x_trim      [LEN_RECD]  = "";
   char        x_source    [LEN_RECD]  = "";
   char        x_full      [LEN_RECD]  = "";
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
      if (strchr ("vVmMcCtTrR", a_style) != 0 && x_row != x_rowsave) {
         DEBUG_REGS   yLOG_note    ("line break");
         fprintf (f, "\n");
      }
      /*---(fill in blank cells)---------*/
      if (curr == NULL) {
         DEBUG_REGS   yLOG_note    ("NULL cell");
         w = LOC_col_width (x_tab, x_col);
         switch (a_style) {
         case 'v' : case 'r' : case 'R' : 
            fprintf (f, "%*.*s", w, w, g_empty);
            break;
         case 'c' : case 'C' :
            fprintf (f, "\"\",");
            break;
         case 't' : case 'T' :
            fprintf (f, "\t");
            break;
         }
      }
      /*---(write filled cells)----------*/
      else {
         DEBUG_REGS   yLOG_info    ("cell"      , curr->label);
         /*---(source)-------------------*/
         if (curr->s != NULL) {
            DEBUG_REGS   yLOG_note    ("convert source string");
            strlcpy      (x_source, curr->s, LEN_RECD);
            strldecode   (x_source, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("source is NULL");
         }
         DEBUG_REGS   yLOG_info    ("source"    , x_source);
         /*---(full outcome)-------------*/
         if (curr->v_str != NULL) {
            DEBUG_REGS   yLOG_note    ("convert modified string");
            strlcpy      (x_full  , curr->v_str, LEN_RECD);
            strldecode   (x_full, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("modified is NULL");
         }
         /*---(printable)----------------*/
         if (curr->p != NULL) {
            DEBUG_REGS   yLOG_note    ("convert printable");
            strlcpy  (x_print , curr->p, LEN_RECD);
            strldecode   (x_print, LEN_RECD);
            strlcpy  (x_trim  , x_print, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("printable is NULL");
         }
         /*---(trimmed printable)--------*/
         strltrim (x_trim, ySTR_BOTH, LEN_RECD);
         DEBUG_REGS   yLOG_info    ("printable" , x_trim);
         DEBUG_REGS   yLOG_info    ("modified"  , x_full);
         DEBUG_REGS   yLOG_note    ("handle");
         switch (a_style) {
         case 'v' : fprintf (f, "%s"                  , x_print);
                    break;
         case 'r' : fprintf (f, "%s  "              , x_print);
                    break;
         case 'V' : fprintf (f, "%s "                 , x_trim);
                    break;
         case 'm' : 
         case 'M' : fprintf (f, "%s"                  , x_full);
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
/*===----                        text registers                        ----===*/
/*====================------------------------------------====================*/
static void  o___TEXT_REGS_______o () { return; }

static char  s_treg_curr  = '"';
static char  s_treg_watch = '"';

typedef struct  cTEXTREG  tTEXTREG;
struct cTEXTREG {
   char        label       [10];            /* original cell                  */
   int         bpos;                        /* original starting point        */
   int         epos;                        /* original ending point          */
   int         len;                         /* length of text                 */
   char        data        [LEN_RECD];       /* text                           */
   char        source;                      /* user, file, import             */
};
static      tTEXTREG    s_textreg   [MAX_REG];
#define     TREG_NONE     '-'
#define     TREG_USER     'u'
#define     TREG_FILE     'f'
#define     TREG_IMPORT   'i'

char             /* clear all selections -----------------[ ------ [ ------ ]-*/
TREG_init          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(registers)----------------------*/
   strlcpy (s_regnames , REG_NAMES, MAX_REG);
   s_treg_curr  = '"';
   s_treg_watch = '"';
   /*---(purge)--------------------------*/
   for (i = 0; i < MAX_REG; ++i) {
      strlcpy (s_textreg [i].label, "", 10);
      s_textreg [i].bpos   = -1;
      s_textreg [i].epos   = -1;
      s_textreg [i].len    =  0;
      strlcpy (s_textreg [i].data , "", LEN_RECD);
      s_textreg [i].source = TREG_NONE;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
TREG_entry         (char a_reg, char *a_list)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_reg       = 0;
   char        x_line      [LEN_RECD];
   int         x_len       = 0;
   /*---(beginning)----------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_reg"     , a_reg);
   DEBUG_REGS   yLOG_point   ("a_list"    , a_list);
   /*---(defenses)--------------------*/
   --rce;  if (a_list  == NULL) {
      DEBUG_REGS   yLOG_note    ("list is null, no point");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      strlcpy (a_list, g_empty, 80);
      return rce;
   }
   /*---(buffer number)------------------*/
   if (a_reg == REG_CURR)  a_reg = s_treg_watch;  /* for status line */
   x_reg  = REG__reg2index  (a_reg);
   DEBUG_REGS   yLOG_value   ("x_reg"     , x_reg);
   --rce;  if (x_reg < 0)  {
      DEBUG_REGS   yLOG_note    ("register is no good");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      strlcpy (a_list, g_empty, 80);
      return rce;
   }
   /*---(write empty line)------------*/
   if (s_textreg [x_reg].len == 0) {
      sprintf (x_line, "  %c    -                                                      -   -  -  ", a_reg);
      strlcpy (a_list, x_line, 80);
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write line)------------------*/
   sprintf (x_line , "  %c  %3d [%-40.40s  %-7.7s %3d %3d  %c  ",
         a_reg                   ,
         s_textreg [x_reg].len   ,
         s_textreg [x_reg].data  ,
         s_textreg [x_reg].label ,
         s_textreg [x_reg].bpos  ,
         s_textreg [x_reg].epos  ,
         s_textreg [x_reg].source);
   x_len = s_textreg [x_reg].len;
   if (x_len <= 40) {
      x_line [10 + x_len] = ']';
   } else if (x_len > 40) {
      x_line [10 + 40   ] = '>';
   }
   strlcpy (a_list, x_line, 80);
   /*---(complete)--------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char          /* PURPOSE : process keys for register actions -----------------*/
TREG_clear         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =   0;
   /*---(delete)-------------------------*/
   for (i = SELC_from (); i <= SELC_to (); ++i) {
      g_contents [i] = ' ';
   }
   /*---(complete)-----------------------*/
   return 0;
}

char          /* PURPOSE : process keys for register actions -----------------*/
TREG_delete        (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_len       =   0;
   int         x_diff      =   0;
   int         i           =   0;
   /*---(set size)-----------------------*/
   x_len   = strlen (g_contents);
   x_diff  = SELC_to() - SELC_from() + 1;
   /*---(delete)-------------------------*/
   for (i = SELC_to () + 1; i <= x_len; ++i) {
      g_contents [i - x_diff] = g_contents [i];
   }
   my.cpos = SELC_from ();
   /*---(complete)-----------------------*/
   return 0;
}

char          /* PURPOSE : process keys for register actions -----------------*/
TREG_copy          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   char       *x_start     = NULL;
   int         x_len       =   0;
   char        x_label     [10]        = "";
   char        rce         = -10;
   /*---(identify register)--------------*/
   x_index = REG__reg2index (s_treg_curr);
   /*---(set size)-----------------------*/
   x_start = g_contents + SELC_from();
   x_len   = SELC_to() - SELC_from() + 1;
   /*---(copy)---------------------------*/
   strlcpy (s_textreg [x_index].data, x_start, x_len + 1);
   /*---(fill in details)----------------*/
   s_textreg [x_index].len    = x_len;
   s_textreg [x_index].bpos   = SELC_from ();
   s_textreg [x_index].epos   = SELC_to   ();
   LOC_ref (CTAB, CCOL, CROW, 0, x_label );
   strlcpy (s_textreg [x_index].label, x_label, 10);
   s_textreg [x_index].source = TREG_USER;
   /*---(complete)-----------------------*/
   return 0;
}

char          /* PURPOSE : process keys for register actions -----------------*/
TREG_replace       (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_len       =   0;
   int         x_gap       =   0;
   int         x_start     =   0;
   char        x_null      = '-';
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = REG__reg2index (s_treg_curr);
   /*---(set the start)------------------*/
   x_start = my.cpos;
   /*---(open)---------------------------*/
   x_len   = strlen (g_contents);
   x_gap   = s_textreg [x_index].len;
   if (x_start + x_gap >= x_len)  x_null = 'y';
   /*---(open)---------------------------*/
   x_len   = s_textreg [x_index].len;
   for (i  = 0; i < x_len; ++i) {
      g_contents [i + x_start] = s_textreg [x_index].data [i];
   }
   /*---(final null)---------------------*/
   if (x_null == 'y') {
      g_contents [x_start + x_gap + 1] == '\0';
   }
   /*---(complete)-----------------------*/
   return 0;
}

char          /* PURPOSE : process keys for register actions -----------------*/
TREG_paste         (char a_dir)
{
   /*---(locals)-----------+-----------+-*/
   int         x_index     =   0;
   int         x_len       =   0;
   int         x_gap       =   0;
   int         x_start     =   0;
   int         i           =   0;
   /*---(identify register)--------------*/
   x_index = REG__reg2index (s_treg_curr);
   /*---(set the start)------------------*/
   x_start = my.cpos;
   if (a_dir == '>')   ++x_start;
   /*---(open)---------------------------*/
   x_len   = strlen (g_contents);
   x_gap   = s_textreg [x_index].len;
   for (i = x_len; i >= x_start; --i) {
      g_contents [i + x_gap] = g_contents [i];
      g_contents [i]         = G_CHAR_PLACE;
   }
   /*---(fill)---------------------------*/
   x_len   = s_textreg [x_index].len;
   for (i  = 0; i < x_len; ++i) {
      g_contents [i + x_start] = s_textreg [x_index].data [i];
   }
   /*---(complete)-----------------------*/
   return 0;
}

char          /* PURPOSE : process keys for register actions -----------------*/
TREG_mode          (int a_major, int a_minor)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         x_buf       =  -1;
   int         x_index     =   0;
   int         i           =   0;
   char       *x_start     = NULL;
   int         x_len       =   0;
   char        x_label     [10]        = "";
   int         x_diff      =   0;
   /*---(header)-------------------------*/
   DEBUG_USER   yLOG_enter   (__FUNCTION__);
   DEBUG_USER   yLOG_char    ("a_major"   , a_major);
   DEBUG_USER   yLOG_char    ("a_minor"   , a_minor);
   /*---(defenses)-----------------------*/
   DEBUG_USER   yLOG_char    ("mode"      , yVIKEYS_mode_curr());
   --rce;  if (yVIKEYS_mode_not (SMOD_TEXTREG )) {
      DEBUG_USER   yLOG_note    ("not the correct mode");
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(escape)-------------------------*/
   if (a_minor == K_ESCAPE)  {
      DEBUG_USER   yLOG_note    ("escape and return to previous mode");
      yVIKEYS_mode_exit ();
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return  0;
   }
   /*---(check for control keys)---------*/
   --rce;  if (a_major == '"') {
      if (strchr (s_regnames, a_minor) != 0) {
         DEBUG_USER   yLOG_note    ("select a text register");
         s_treg_curr = a_minor;
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return 0;
      } else if (a_minor == '?') {
         DEBUG_USER   yLOG_note    ("show text register inventory");
         my.info_win  = G_INFO_TREG;
         s_treg_curr = '"';
         DEBUG_USER   yLOG_exit    (__FUNCTION__);
         return  0;
      }
      DEBUG_USER   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (a_major == ' ') {
      switch (a_minor) {
      case  '!' :
         my.layout_status     = G_STATUS_TREG;
         s_treg_watch = s_treg_curr;
         yVIKEYS_mode_exit ();
         break;
      case  '#' :
         DEBUG_USER   yLOG_note    ("wipe text register");
         x_index = REG__reg2index (s_treg_curr);
         strlcpy (s_textreg [x_index].label, "", 10);
         s_textreg [x_index].bpos  = -1;
         s_textreg [x_index].epos  = -1;
         s_textreg [x_index].len   =  0;
         strlcpy (s_textreg [x_index].data , "", LEN_RECD);
         yVIKEYS_mode_exit ();
         break;
      case  'y' : case  'Y' :
         DEBUG_USER   yLOG_note    ("yank selection text");
         TREG_copy   ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'x' : case  'X' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TREG_copy   ();
         TREG_clear  ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'd' : case  'D' :
         DEBUG_USER   yLOG_note    ("delete selection text");
         TREG_copy   ();
         TREG_delete ();
         EDIT_done ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'r' : case  'R' :
         DEBUG_USER   yLOG_note    ("replace selection text");
         TREG_replace();
         EDIT_done ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'p' :
         DEBUG_USER   yLOG_note    ("paste after selection text");
         TREG_paste  ('>');
         EDIT_done ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'P' :
         DEBUG_USER   yLOG_note    ("paste before selection text");
         TREG_paste  ('<');
         EDIT_done ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'g' :
         DEBUG_USER   yLOG_note    ("go to beginning selection position");
         x_index = REG__reg2index (s_treg_curr);
         my.cpos = s_textreg [x_index].bpos;
         EDIT_done ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      case  'G' :
         DEBUG_USER   yLOG_note    ("go to ending selection position");
         x_index = REG__reg2index (s_treg_curr);
         my.cpos = s_textreg [x_index].epos;
         EDIT_done ();
         yVIKEYS_mode_exit ();
         if (yVIKEYS_mode_curr == SMOD_SELECT)  SELC_mode   (' ', K_ESCAPE);
         break;
      }
   }
   DEBUG_USER   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TREG_read          (char a_reg, char *a_label, int a_beg, int a_end, char *a_source)
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
   /*---(data)---------------------------*/
   strlcpy (s_textreg [x_reg].label, a_label , 10);
   s_textreg [x_reg].bpos   = a_beg;
   s_textreg [x_reg].epos   = a_end;
   strlcpy (s_textreg [x_reg].data , a_source, LEN_RECD);
   s_textreg [x_reg].len    = strllen (s_textreg [x_reg].data, LEN_RECD);
   s_textreg [x_reg].source = TREG_FILE;
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
TREG_write         (FILE *a_file, int  *a_seq, char a_buf)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   int         i           = 0;             /* iterator -- buffer entry       */
   int         x_buf       = 0;
   char        x_regid     [20];
   int         x_tab       = 0;
   /*---(buffer number)------------------*/
   x_buf  = REG__reg2index  (a_buf);
   --rce;  if (x_buf < 0 )                         return rce;
   --rce;  if (s_textreg [x_buf].len  <= 0)        return rce;
   /*---(register entry)-----------------*/
   LOC_parse (s_textreg [x_buf].label, &x_tab, NULL, NULL, NULL);
   sprintf (x_regid, "%c", a_buf);
   fprintf (a_file, "#---------  ver  ---lvl/reg--  -tab-  --beg---  --end---  --min---  --max---  t  ---data------------\n");
   fprintf (a_file, "source_reg  -A-  %-12.12s    %1d    %-8.8s  %-8.8s  %-8d  %-8d  %c  %s\n",
         x_regid                , x_tab                  ,
         s_textreg [x_buf].label, "-"                    ,
         s_textreg [x_buf].bpos , s_textreg [x_buf].epos ,
         't'                    , s_textreg [x_buf].data );
   ++(*a_seq);
   /*---(complete)-----------------------*/
   fflush (a_file);
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
   } else if (strcmp (a_request, "s_recd"             ) == 0) {
      strlcpy (s_recd, a_data, LEN_RECD);
      s_max = strlen (s_recd);
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
   char        x_list      [LEN_RECD];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_reg            : question not understood");
   /*---(buffer number)------------------*/
   if (a_reg != 0) {
      x_reg = REG__reg2index  (a_reg);
   }
   /*---(defenses)-----------------------*/
   if      (x_reg < 0) {
      snprintf (unit_answer, LEN_UNIT, "s_reg error      : register reference (%c) is not valid", a_reg);
   }
   /*---(register list)------------------*/
   else if (strcmp (a_question, "reg_names"    )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_reg names      : %-.45s", s_regnames);
   }
   else if (strcmp (a_question, "reg_count"    )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_reg count      : %d", strlen (s_regnames));
   }
   /*---(register list)------------------*/
   else if (strcmp (a_question, "reg_sort"     )  == 0) {
      REG_list  (a_reg, x_list);
      if (strlen (x_list) > 12)  DEP_disp_sort (x_list + 11);
      snprintf (unit_answer, LEN_UNIT, "s_reg sort %-.45s", x_list);
   }
   else if (strcmp (a_question, "reg_list"     )  == 0) {
      REG_list  (a_reg, x_list);
      snprintf (unit_answer, LEN_UNIT, "s_reg list %-.45s", x_list);
   }
   else if (strcmp (a_question, "reg_range"    )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_reg range      : %c, ta=%4d, bc=%4d, br=%4d, ec=%4d, er=%4d", a_reg, s_reg[x_reg].otab, s_reg[x_reg].begc, s_reg[x_reg].begr, s_reg[x_reg].endc, s_reg[x_reg].endr);
   }
   else if (strcmp (a_question, "reg_reach"    )  == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_reg reach      : %c, ta=%4d, nc=%4d, nr=%4d, xc=%4d, xr=%4d", a_reg, s_reg[x_reg].otab, s_reg[x_reg].minc, s_reg[x_reg].minr, s_reg[x_reg].maxc, s_reg[x_reg].maxr);
   }
   else if (strcmp (a_question, "reg_buffer")     == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_reg buffer (%c) : (%02d) r=%3d, n=%3d, h=%10p, t=%10p", a_reg, x_reg, s_reg[x_reg].real, s_reg[x_reg].nbuf, s_reg[x_reg].buf[0], s_reg[x_reg].buf[s_reg[x_reg].nbuf - 1]);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*===============================[[ end code ]]===============================*/
