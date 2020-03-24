/*============================----beg-of-source---============================*/

/*===[[ DECISION :: FORMAT ]]=================================================*/

/*   PROBLEM (pr)
 *   gyges is a intended to be a powerful, technical, and stable spreadsheet on
 *   which i will constantly depend.  i simply won't have time and energy to
 *   constantly update the file format or desire to debug something complicated.
 *
 *   OBJECTIVES (o)
 *      -- human readable and editable
 *      -- not require an external library or tool to access, read, or convert
 *      -- stable over very long periods
 *      -- only store what gyges requires and nothing more
 *
 *    ALTERNATIVES (act)
 *      a) excel     proprietary, binary, evolving, and complex
 *      b) oocalc    open, xml, compressed, evolving, and complex
 *      c) gnumeric  open, evolving, and complex
 *      d) sylk      open, xml, and complex
 *      e) custom    requires definition and might not work long term
 *
 *   DECISION (ion)
 *   gyges is an order of magnitude simplier that these spreadsheets.  they
 *   each do so much and have tons of complexity, not the least of which is
 *   all the formatting options that i will never have or allow.  the answer
 *   is obvious -- custom and simple.
 *
 */

/*===[[ DESIGN PRINCIPLES ]]==================================================*/

/*   given that i will be away from the code for extended periods,
 *   maintainability requires simplicity and clarity rather than compactness
 *   and brilliance.
 *
 *   the design principles for the file format are...
 *      -- ascii text only that can be read, altered, and created in an editor
 *      -- 7 bit safe (unsigned char) for transportability (values 0 - 127)
 *      -- columnar for readability, even though it wastes space
 *      -- ascii field separator delimited to make column boundaries clear
 *      -- record type fields to aid parsing and interpretation
 *      -- internal comments, headings, and counts for human readability
 *      -- as close to gyges data representation as possible for clarity
 *      -- some additional contextual data if it helps readability
 *
 */



#include    "gyges.h"
#define     FILE_EXIM   "/root/z_gehye/vi_clip.txt"




/*---(globals)----------+-----------+-*/
char        ver_ctrl    = '-';
char        ver_num     [10]        = "----";
char        ver_txt     [100]       = "-----";

char        s_vers      [LEN_RECD]   = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char        s_recd      [LEN_RECD];
int         s_len       = 0;
char       *s_p         = NULL;               /* strtok return pointer     */
char       *s_q         = "";               /* strtok delimeters         */
char       *s_context   = NULL;               /* strtok context variable   */
char        s_fields    [20][LEN_RECD];
int         s_nfield    =   0;


char        s_cellbad   = 0;


#define     FILE_BLANK  "untitled"
#define     FILE_SUFFIX "gyges"
static char     f_suffix    [LEN_RECD];      /* file suffix for spreadsheet    */
static FILE    *s_file;                      /* file pointer                   */


char
FILE_prepper            (void)
{
   /*---(header)-------------------------*/
   DEBUG_OUTP    yLOG_enter   (__FUNCTION__);
   /*> yCALC_cleanse ();                                                              <*/
   CELL_purge   ();
   LOC_purge    ();
   TAB_new_in_abbr ('0', NULL, NULL);
   TAB_new_in_abbr ('®', NULL, NULL);
   TAB_new_in_abbr ('¯', NULL, NULL);
   MAP_mapper (YVIKEYS_INIT);
   yVIKEYS_jump (0, 0, 0, 0);
   yCALC_set_state ('L');
   DEBUG_OUTP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
FILE_finisher           (void)
{
   DEBUG_OUTP    yLOG_enter   (__FUNCTION__);
   yCALC_set_state ('-');
   DEBUG_OUTP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
FILE_new                (void)
{
   /*> yCALC_cleanse ();                                                              <*/
   CELL_purge   ();
   LOC_purge    ();
   TAB_new_in_abbr ('0', NULL, NULL);
   TAB_new_in_abbr ('®', NULL, NULL);
   TAB_new_in_abbr ('¯', NULL, NULL);
   MAP_mapper (YVIKEYS_INIT);
   yVIKEYS_jump (0, 0, 0, 0);
   /*> yVIKEYS_cmds_direct ("0a1");                                                   <*/
   return 0;
}

char
FILE_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(handlers)-----------------------*/
   rc = yVIKEYS_cmds_add (YVIKEYS_M_FILE  , "new"         , ""    , ""     , FILE_new                     , "clear out all content"                                       );
   /*---(complete)-----------------------*/
   return 0;
}





/*> char         /+-> tbd --------------------------------[ ------ [gc.320.121.32]+/ /+-[00.0000.00#.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> FILE_writeas         (char *a_name)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    char        x_name      [LEN_RECD]  = "";                                                                                   <* 
 *>    strlcpy (x_name, my.f_name, LEN_RECD);                                                                                      <* 
 *>    if (rc >= 0)  rc = FILE_rename (a_name);                                                                                    <* 
 *>    if (rc >= 0)  rc = FILE_write  ();                                                                                          <* 
 *>    if (rc >= 0)  rc = FILE_rename (x_name);                                                                                    <* 
 *>    return rc;                                                                                                                  <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                         import/export                        ----===*/
/*====================------------------------------------====================*/
static void  o___IMPORT__________o () { return; }

static FILE   *s_clip     = NULL;
static int     s_lines    =    0;

static int     s_max      =    0;

static char    s_sizer    =  '-';
static char    s_style    =  '-';
static char    s_hist      = HIST_BEG;
static char    s_mapper   =  '-';
static short   s_map      [MAX_COLS];

static short   s_icol     =    0;
static short   s_mcol     =    0;
static short   s_ccol     =    0;
static short   s_crow     =    0;


static int     s_borig    =    0;
static int     s_xorig    =    0;
static int     s_yorig    =    0;

char
EXIM__open              (char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(open file)----------------------*/
   DEBUG_REGS   yLOG_char    ("a_dir"     , a_dir);
   switch (a_dir) {
   case 'r'  :
      DEBUG_REGS   yLOG_note    ("open for reading");
      s_clip = fopen (FILE_EXIM, "rt");
      break;
   case 'w'  :
      DEBUG_REGS   yLOG_note    ("open for writing");
      s_clip = fopen (FILE_EXIM, "wt");
      break;
   default   :
      DEBUG_REGS   yLOG_note    ("not a legal file mode");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
      break;
   }
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

char
EXIM__close             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(close file)---------------------*/
   rc = fclose  (s_clip);
   DEBUG_REGS   yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_note    ("can not close clip file");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char         /+-> prepare for data import ------------[ leaf   [fe.F83.123.30]+/ /+-[01.0000.014.X]-+/ /+-[--.---.---.--]-+/   <* 
 *> EXIM__import_prep       (char a_style)                                                                                         <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    int         i           =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(save style)---------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("save style");                                                                                   <* 
 *>    s_style  = a_style;                                                                                                         <* 
 *>    /+---(clear sizer)--------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("clear sizer");                                                                                  <* 
 *>    s_sizer  = '-';                                                                                                             <* 
 *>    /+---(clear sizer)--------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("clear map");                                                                                    <* 
 *>    s_mapper = '-';                                                                                                             <* 
 *>    for (i = 0; i < MAX_COLS; ++i)  s_map [i] = 0;                                                                              <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("prepare positions");                                                                            <* 
 *>    s_icol  =    0;                                                                                                             <* 
 *>    s_mcol  =    0;                                                                                                             <* 
 *>    s_ccol  =    0;                                                                                                             <* 
 *>    s_crow  =    0;                                                                                                             <* 
 *>    s_lines =    0;                                                                                                             <* 
 *>    /+---(delimiter)----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("set delimiter");                                                                                <* 
 *>    switch (s_style) {                                                                                                          <* 
 *>    case 's'  : case 'S'  :                                                                                                     <* 
 *>       strlcpy (s_q, " "  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 'c'  : case 'C'  :                                                                                                     <* 
 *>       strlcpy (s_q, ","  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 't'  : case 'T'  :                                                                                                     <* 
 *>       strlcpy (s_q, "\t" , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 'r'  : case 'R'  :                                                                                                     <* 
 *>       strlcpy (s_q, "" , 5);                                                                                                  <* 
 *>       break;                                                                                                                   <* 
 *>    case 'v'  : case 'V'  :                                                                                                     <* 
 *>       strlcpy (s_q, " "  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    default   :                                                                                                                 <* 
 *>       return rce;                                                                                                              <* 
 *>       break;                                                                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    /+---(sizer)--------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("set sizer value");                                                                              <* 
 *>    switch (s_style) {                                                                                                          <* 
 *>    case 'T' : case 'C' : case 'R' : case 'V' : case 'S' :                                                                      <* 
 *>       s_sizer = 'a';                                                                                                           <* 
 *>       break;                                                                                                                   <* 
 *>    case 'v' :                                                                                                                  <* 
 *>       s_sizer = 'f';                                                                                                           <* 
 *>       break;                                                                                                                   <* 
 *>    default  :                                                                                                                  <* 
 *>       s_sizer = '-';                                                                                                           <* 
 *>       break;                                                                                                                   <* 
 *>    }                                                                                                                           <* 
 *>    /+---(open output file)---------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("open clip file");                                                                               <* 
 *>    s_clip = fopen (FILE_EXIM, "rt");                                                                                           <* 
 *>    DEBUG_REGS   yLOG_point   ("s_clip"    , s_clip);                                                                           <* 
 *>    --rce;  if (s_clip == NULL) {                                                                                               <* 
 *>       DEBUG_REGS   yLOG_note    ("can not open clip file");                                                                    <* 
 *>       DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
*>    return 0;                                                                                                                   <* 
*> }                                                                                                                              <*/

/*> char         /+-> process a mapping request ----------[ ------ [fe.943.044.31]+/ /+-[02.0000.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> EXIM_mapping         (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    int         i           =    0;                                                                                             <* 
 *>    tCELL      *x_curr      = NULL;                                                                                             <* 
 *>    short       x_ccol      =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    DEBUG_REGS   yLOG_char    ("s_style"   , s_style);                                                                          <* 
 *>    if (s_style == 'v') {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_note    ("mapping not allowed in this import style");                                                  <* 
 *>       DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(import mapping)-----------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("start mapping");                                                                                <* 
 *>    for (i = 0; i < MAX_COLS; ++i) {                                                                                            <* 
 *>       x_ccol = CCOL + i;                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_value   ("x_ccol"    , x_ccol);                                                                        <* 
 *>       x_curr = LOC_cell_at_loc (CTAB, x_ccol, CROW);                                                                           <* 
 *>       DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);                                                                        <* 
 *>       if (x_curr == NULL)  {                                                                                                   <* 
 *>          DEBUG_REGS   yLOG_note    ("end of mapping (null cell)");                                                             <* 
 *>          DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       DEBUG_REGS   yLOG_value   ("v_num"     , x_curr->v_num);                                                                 <* 
 *>       if (x_curr->v_num <= 0) {                                                                                                <* 
 *>          DEBUG_REGS   yLOG_note    ("end of mapping (zero or negative cell)");                                                 <* 
 *>          DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       s_mapper = 'y';                                                                                                          <* 
 *>       s_crow   = CROW + 1;                                                                                                     <* 
 *>       s_map [i] = x_curr->v_num;                                                                                               <* 
 *>       DEBUG_REGS   yLOG_pair    (i          , s_map [i] );                                                                     <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> adjust a column sizing -------------[ ------ [fe.420.223.22]+/ /+-[00.0000.015.B]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_width      (short a_col, short a_wide)                                                                               <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    short       w           =   0;                                                                                              <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    --rce;  if (s_sizer != 'a')      return rce;                                                                                <* 
 *>    /+---(resize)-------------------------+/                                                                                    <* 
 *>    w = COL_size (CTAB, CCOL + a_col);                                                                                         <* 
 *>    --rce;  if (w >= a_wide + 1)     return rce;                                                                                <* 
 *>    COL_resize (CTAB, CCOL + a_col, a_wide + 1);                                                                                 <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> process a column sizing request ----[ ------ [fe.A53.053.51]+/ /+-[02.0000.014.E]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_sizer      (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    char       *p           = NULL;                                                                                             <* 
 *>    char       *s           = NULL;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    short       x_col       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(defense)------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_value   ("s_sizer"   , s_sizer);                                                                          <* 
 *>    --rce;  if (s_sizer != 'a') {                                                                                               <* 
 *>       DEBUG_REGS   yLOG_note    ("wrong type, only 'a' allowed");                                                              <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_REGS   yLOG_info    ("s_recd"    , s_recd);                                                                           <* 
 *>    --rce;  if ((s_style != 'C' && strncmp ("#<34", s_recd, 4) != 0) ||                                                         <* 
 *>          (s_style == 'C' && strncmp ("\"#<34", s_recd, 5) != 0)) {                                                             <* 
 *>       DEBUG_REGS   yLOG_note    ("not a sizing line");                                                                         <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    /+---(process)------------------------+/                                                                                    <* 
 *>    p  = strtok_r (s_recd, s_q, &s);                                                                                            <* 
 *>    while (p != NULL) {                                                                                                         <* 
 *>       DEBUG_REGS   yLOG_value   ("x_col"     , x_col);                                                                         <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       if (s_style == 'C' && x_len >= 3)  p [0] = p [x_len - 1] = ' ';                                                          <* 
 *>       strltrim (p, ySTR_BOTH, LEN_RECD);                                                                                       <* 
 *>       DEBUG_REGS   yLOG_info    ("p (field)" , p);                                                                             <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       DEBUG_REGS   yLOG_value   ("x_len"     , x_len);                                                                         <* 
 *>       if (x_len == 0)  break;  /+ stop at unsized column +/                                                                    <* 
 *>       COL_resize (CTAB, CCOL + x_col, x_len + 1);                                                                               <* 
 *>       ++x_col;                                                                                                                 <* 
 *>       p  = strtok_r (NULL, s_q, &s);                                                                                           <* 
 *>    }                                                                                                                           <* 
 *>    /+---(update)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("mark sizer processed");                                                                         <* 
 *>    s_sizer  = 's';                                                                                                             <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> place import data into a cell ------[ ------ [fz.310.301.21]*/ /*-[01.0000.025.!]-*/ /*-[--.---.---.--]-*/
EXIM__import_place   (short a_col, short a_row, char *a_value)
{
   CELL_change (NULL, HIST_BEG, CTAB, CCOL + a_col, CROW + a_row, a_value);
   return 0;
}

/*> char         /+-> complete data import ---------------[ ------ [fz.311.001.02]+/ /+-[00.0000.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> EXIM_import__done       (void)                                                                                                 <* 
 *> {                                                                                                                              <* 
 *>    /+---(close file)---------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("closing file");                                                                                 <* 
 *>    fclose  (s_clip);                                                                                                           <* 
 *>    /+---(screen)-------------------------+/                                                                                    <* 
 *>    /+> CURS_screen_reset ();                                                          <+/                                      <* 
 *>    /+---(calculate)----------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_note    ("recalc");                                                                                        <* 
 *>    /+> SEQ_calc_full ();                                                              <+/                                      <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char
EXIM__export_sizer      (void)
{
   /*---(locals)-----+-----+-----+-----+-*/
   int         b, xb, yb, xe, ye;
   int         x_col, x_row;
   int         w, h;
   char        t           [LEN_RECD]  = "";
   char        s           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(size)---------------------------*/
   yVIKEYS_visu_coords (&b, &xb, &xe, &yb, &ye, NULL);
   if (strchr ("SN", s_style) != NULL) {
      fprintf (s_clip, "##@ bounds %5d %5d %5d %5d %5d %5d\n",  b,  b, xb, yb, xe, ye);
      /*> fprintf (s_clip, "##* reach  %5d %5d %5d %5d %5d %5d\n",  0,  0,  0,  0,  0,  0);   <*/
   }
   /*---(x_size)----------------------*/
   sprintf (s, "##x x-size %5d", xe - xb + 1);
   for (x_col = xb; x_col <= xe; ++x_col) {
      w     = COL_size (CTAB, x_col);
      DEBUG_REGS  yLOG_value   ("w"         , w);
      sprintf (t, " %5d", w);
      DEBUG_REGS  yLOG_info    ("t"         , t);
      strcat  (s, t);
      DEBUG_REGS  yLOG_info    ("s"         , s);
   }
   fprintf (s_clip, "%s\n", s);
   /*---(y_size)----------------------*/
   sprintf (s, "##y y-size %5d", ye - yb + 1);
   for (x_row = yb; x_row <= ye; ++x_row) {
      h     = ROW_size (CTAB, x_row);
      DEBUG_REGS  yLOG_value   ("h"         , h);
      sprintf (t, " %5d", h);
      DEBUG_REGS  yLOG_info    ("t"         , t);
      strcat  (s, t);
      DEBUG_REGS  yLOG_info    ("s"         , s);
   }
   fprintf (s_clip, "%s\n", s);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_sizer      (char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_LABEL];
   int         c           =    0;
   int         x_pos       =    0;
   int         x_size      =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_char    ("a_dir"     , a_dir);
   --rce;  if (strchr ("xy", a_dir) == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("s_recd"    , s_recd);
   --rce;  if (a_dir == 'x' && strncmp (s_recd, "##x x-size ", 11) != 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_dir == 'y' && strncmp (s_recd, "##y y-size ", 11) != 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(count)--------------------------*/
   strncpy (t, s_recd + 11, 5);
   t [5] = '\0';
   DEBUG_REGS   yLOG_delim   ("t"         , t);
   c = atoi (t);
   DEBUG_REGS   yLOG_value   ("c"         , c);
   /*---(sizings)------------------------*/
   for (x_pos = 0; x_pos < c; ++x_pos) {
      strncpy (t, s_recd + 17 + (x_pos * 6), 5);
      t [5] = '\0';
      DEBUG_REGS   yLOG_delim   ("t"         , t);
      x_size = atoi (t);
      DEBUG_REGS   yLOG_value   ("x_size"    , x_size);
      if (a_dir == 'x')  COL_resize    (CTAB, CCOL + x_pos, x_size);
      /*> else               ROW_heighten (CTAB, CROW + x_pos, x_size);               <*/
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_bounds     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        t           [LEN_LABEL];
   int         c           =    0;
   int         x_pos       =    0;
   int         x_value     =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_info    ("s_recd"    , s_recd);
   --rce;  if (strncmp (s_recd, "##@ bounds ", 11) != 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(sizings)------------------------*/
   for (x_pos = 0; x_pos < 6; ++x_pos) {
      strncpy (t, s_recd + 11 + (x_pos * 6), 5);
      t [5] = '\0';
      DEBUG_REGS   yLOG_delim   ("t"         , t);
      x_value = atoi (t);
      DEBUG_REGS   yLOG_value   ("x_value"   , x_value);
      switch (x_pos) {
      case  0 :  s_borig = x_value;     break;
      case  2 :  s_xorig = x_value;     break;
      case  3 :  s_yorig = x_value;     break;
      }
   }
   DEBUG_REGS   yLOG_value   ("s_borig"   , s_borig);
   DEBUG_REGS   yLOG_value   ("s_xorig"   , s_xorig);
   DEBUG_REGS   yLOG_value   ("s_yorig"   , s_yorig);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.732.141.12]*/ /*-[01.0000.014.3]-*/ /*-[--.---.---.--]-*/
EXIM__import_values     (int a_row)
{
   /*---(locals)-----+-----+-----+-----+-*/
   int         x_col       =    0;
   int         w           =    0;
   int         cw          =    0;
   char        t           [LEN_RECD]  = "";
   char        x_format    [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(process cells)---------------*/
   while (cw < s_max) {
      /*---(read import data)---------*/
      w     = COL_size (CTAB, CCOL + x_col);
      DEBUG_REGS  yLOG_value   ("w"         , w);
      strlcpy      (t, s_recd + cw, w + 1);
      DEBUG_REGS  yLOG_delim   ("t (orig)"  , t);
      /*---(guess formatting)---------*/
      strcpy (x_format, "??0--");
      if      (t [0] == ' ' && t [w - 2] == ' ')  x_format [0] = '|';
      else if (t [0] != ' ')                      x_format [0] = '<';
      else if (t [w - 2] != ' ')                  x_format [0] = '>';
      /*---(process)------------------*/
      strltrim     (t, ySTR_BOTH, LEN_RECD);
      DEBUG_REGS  yLOG_info    ("t (trim)"  , t);
      strlencode   (t, ySTR_NORM, LEN_RECD);
      DEBUG_REGS  yLOG_info    ("t (new)"   , t);
      DEBUG_REGS  yLOG_value   ("x_col"     , x_col);
      /*---(change)-------------------*/
      CELL_overwrite (s_hist, CTAB, CCOL + x_col, CROW + a_row, t, x_format);
      s_hist = HIST_ADD;
      /*---(next)---------------------*/
      ++x_col;
      cw += w;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

static   int         s_otab, s_ocol, s_orow;
static   int         s_dtab, s_dcol, s_drow;
static   char        s_source  [LEN_RECD];

char
EXIM__import_dest       (char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab , x_col , x_row;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_info    ("label"     , a_label);
   rc = str2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_REGS  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_REGS  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_complex ("src loc"   , "%2dt, %3dc, %4dr", x_tab, x_col, x_row);
   s_otab = CTAB - s_borig;
   s_ocol = CCOL - s_xorig;
   s_orow = CROW - s_yorig;
   DEBUG_REGS   yLOG_complex ("offset"    , "%2dt, %3dc, %4dr", s_otab, s_ocol, s_orow);
   s_dtab = x_tab + s_otab;
   s_dcol = x_col + s_ocol;
   s_drow = x_row + s_orow;
   DEBUG_REGS   yLOG_complex ("dst loc"   , "%2dt, %3dc, %4dr", s_dtab, s_dcol, s_drow);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_destsrc    (int a_col, int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab , x_col , x_row;
   /*---(header)-------------------------*/
   x_tab = s_borig;
   x_col = s_xorig + a_col;
   x_row = s_yorig + a_row;
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_complex ("src loc"   , "%2dt, %3dc, %4dr", x_tab, x_col, x_row);
   s_otab = CTAB - s_borig;
   s_ocol = CCOL - s_xorig;
   s_orow = CROW - s_yorig;
   DEBUG_REGS   yLOG_complex ("offset"    , "%2dt, %3dc, %4dr", s_otab, s_ocol, s_orow);
   s_dtab = x_tab + s_otab;
   s_dcol = x_col + s_ocol;
   s_drow = x_row + s_orow;
   DEBUG_REGS   yLOG_complex ("dst loc"   , "%2dt, %3dc, %4dr", s_dtab, s_dcol, s_drow);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_content    (char *a_content)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   strltrim (a_content, ySTR_BOTH, LEN_RECD);
   strldchg (a_content, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   DEBUG_REGS   yLOG_info    ("a_content" , a_content);
   /*---(check cell type)----------------*/
   if (a_content [0] == '\0') {
      DEBUG_REGS   yLOG_note    ("empty cell");
      strcpy (s_source, "");
   } else if (strchr ("=#~", a_content [0]) != 0) {
      DEBUG_REGS   yLOG_note    ("formula, calling yRPN_adjust");
      rc = yRPN_addr_require (a_content, 'r', s_otab, s_ocol, s_orow, 0, LEN_RECD, s_source);
      DEBUG_REGS   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_REGS   yLOG_note    ("formula could not be parsed");
         strcpy (s_source, "not_translatable");
      }
   } else {
      DEBUG_REGS   yLOG_note    ("just copy straight across");
      strcpy (s_source, a_content);
   }
   DEBUG_REGS   yLOG_info    ("s_source"  , s_source);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.732.141.12]*/ /*-[01.0000.014.3]-*/ /*-[--.---.---.--]-*/
EXIM__import_fields     (int a_row)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rc          =    0;
   char        x_recd      [LEN_RECD]  = "";
   int         x_len       =    0;
   int         x_col       =    0;
   char       *p           = NULL;
   char        x_format    [LEN_LABEL];
   char        q           [LEN_LABEL];
   tCELL      *x_new       = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prep)------------------------*/
   switch (s_style) {
   case 'f' : case 'F' : case 'd' : case 'D' :
      strcpy (q, "");
      break;
   case 't' : case 'T' :
      strcpy (q, "\t");
      break;
   }
   /*---(process cells)---------------*/
   strncpy (x_recd, s_recd, LEN_RECD);
   p = strtok (x_recd, q);
   while (p != NULL) {
      EXIM__import_destsrc (x_col, a_row);
      /*---(read import data)---------*/
      x_len = strlen (p);
      DEBUG_REGS  yLOG_value   ("x_len"     , x_len);
      DEBUG_REGS  yLOG_delim   ("p (orig)"  , p);
      /*---(guess formatting)---------*/
      strcpy (x_format, "??0--");
      if      (p [0] == ' ' && p [x_len - 2] == ' ')  x_format [0] = '|';
      else if (p [0] != ' ')                          x_format [0] = '<';
      else if (p [x_len - 2] != ' ')                  x_format [0] = '>';
      /*---(process)------------------*/
      rc = EXIM__import_content (p);
      DEBUG_REGS   yLOG_value   ("content"   , rc);
      DEBUG_REGS  yLOG_info    ("s_source"  , s_source);
      /*---(update)-------------------------*/
      x_new = CELL_overwrite (s_hist, s_dtab, s_dcol, s_drow, s_source, x_format);
      DEBUG_REGS  yLOG_point   ("x_new"     , x_new);
      s_hist = HIST_ADD;
      /*---(next)---------------------*/
      ++x_col;
      p = strtok (NULL, q);
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.732.141.12]*/ /*-[01.0000.014.3]-*/ /*-[--.---.---.--]-*/
EXIM__import_native     (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_work      [LEN_RECD]  = "";
   char       *p           = NULL;
   char        x_format    [LEN_LABEL];
   tCELL      *x_new       = NULL;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(process cells)---------------*/
   strncpy (x_work, s_recd, LEN_RECD);
   p = strtok (x_work, "");
   DEBUG_REGS   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_REGS   yLOG_info    ("p"         , p);
   --rce;  if (strncmp ("cell ", p, 5) != 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   p = strtok (NULL, "");
   DEBUG_REGS   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strltrim (p, ySTR_BOTH, LEN_LABEL);
   rc = EXIM__import_dest (p);
   DEBUG_REGS   yLOG_value   ("dest"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(format)-------------------------*/
   p = strtok (NULL, "");
   DEBUG_REGS   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strltrim (p, ySTR_BOTH, LEN_LABEL);
   strlcpy (x_format, p, LEN_LABEL);
   DEBUG_REGS   yLOG_info    ("format"    , x_format);
   /*---(source)-------------------------*/
   p = strtok (NULL, "");
   DEBUG_REGS   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = EXIM__import_content (p);
   DEBUG_REGS   yLOG_value   ("content"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update)-------------------------*/
   x_new = CELL_overwrite (s_hist, s_dtab, s_dcol, s_drow, s_source, x_format);
   DEBUG_REGS  yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL)  {
      DEBUG_REGS  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_hist = HIST_ADD;
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char         /+-> tbd --------------------------------[ ------ [fz.842.141.32]+/ /+-[01.0000.014.9]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_delims     (short a_row)                                                                                             <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    short       x_col       =    0;                                                                                             <* 
 *>    char       *p           = NULL;                                                                                             <* 
 *>    char       *s           = NULL;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(parse first)-----------------+/                                                                                       <* 
 *>    p  = strtok_r (s_recd, s_q, &s);                                                                                            <* 
 *>    /+---(process cells)---------------+/                                                                                       <* 
 *>    while (p != NULL) {                                                                                                         <* 
 *>       /+---(read import data)---------+/                                                                                       <* 
 *>       DEBUG_REGS  yLOG_info    ("p (orig)"  , p);                                                                              <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       DEBUG_REGS  yLOG_value   ("x_len"     , x_len);                                                                          <* 
 *>       /+---(process)------------------+/                                                                                       <* 
 *>       if (s_style == 'c' && x_len >= 3)  p [0] = p [x_len - 1] = ' ';                                                          <* 
 *>       strltrim (p, ySTR_BOTH, LEN_RECD);                                                                                       <* 
 *>       DEBUG_REGS  yLOG_info    ("p (trim)"  , p);                                                                              <* 
 *>       x_len = strlen (p);                                                                                                      <* 
 *>       DEBUG_REGS  yLOG_value   ("x_len"     , x_len);                                                                          <* 
 *>       if (s_sizer == 'a')  REG__inpt_width (x_col, x_len);                                                                     <* 
 *>       strlencode   (p, ySTR_NORM, LEN_RECD);                                                                                   <* 
 *>       DEBUG_REGS  yLOG_info    ("p (new)"   , p);                                                                              <* 
 *>       DEBUG_REGS  yLOG_value   ("x_col"     , x_col);                                                                          <* 
 *>       REG__inpt_place  (x_col, a_row, p);                                                                                      <* 
 *>       /+---(parse next)---------------+/                                                                                       <* 
 *>       p  = strtok_r (NULL, s_q, &s);                                                                                           <* 
 *>       ++x_col;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [ge.A42.132.77]+/ /+-[02.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG_inpt_driver      (char a_style)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    short       x_row       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(prepare)------------------------+/                                                                                    <* 
 *>    rc = EXIM__import_prep   (a_style);                                                                                         <* 
 *>    DEBUG_REGS   yLOG_value   ("prep rc"   , rc);                                                                               <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    rc = EXIM_mapping  ();                                                                                                      <* 
 *>    /+---(process lines)------------------+/                                                                                    <* 
 *>    while (1) {                                                                                                                 <* 
 *>       /+---(read)------------------------+/                                                                                    <* 
 *>       rc = EXIM_import__read ();                                                                                               <* 
 *>       if (rc >  0)  continue;                                                                                                  <* 
 *>       if (rc <  0)  break;                                                                                                     <* 
 *>       /+---(process cells)---------------+/                                                                                    <* 
 *>       DEBUG_REGS  yLOG_value   ("x_row"     , x_row);                                                                          <* 
 *>       /+---(check sizer)-----------------+/                                                                                    <* 
 *>       /+> if (strncmp (s_recd, "#<34"  , 4) == 0) {                                   <*                                       <* 
 *>        *>    REG__inpt_sizer ();                                                      <*                                       <* 
 *>        *>    continue;                                                                <*                                       <* 
 *>        *> }                                                                           <+/                                      <* 
 *>       /+---(handle values)---------------+/                                                                                    <* 
 *>       if (strchr ("Vv", s_style) != NULL) {                                                                                    <* 
 *>          EXIM_import__values  (x_row);                                                                                         <* 
 *>       } else {                                                                                                                 <* 
 *>          REG__inpt_delims  (x_row);                                                                                            <* 
 *>       }                                                                                                                        <* 
 *>       /+---(prepare for next)------------+/                                                                                    <* 
 *>       ++x_row;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(wrap up)------------------------+/                                                                                    <* 
 *>    rc = EXIM_import__done ();                                                                                                  <* 
 *>    DEBUG_REGS   yLOG_value   ("done rc"   , rc);                                                                               <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> read an import record --------------[ leaf   [fe.A34.026.50]*/ /*-[01.0001.014.!]-*/ /*-[--.---.---.--]-*/
EXIM__import_read       (void)
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
   --rce;  if (feof (s_clip))  {
      DEBUG_REGS   yLOG_note    ("end of file reached");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (s_recd);
   if (x_len > 0)  s_recd [--x_len] = '\0';
   DEBUG_REGS   yLOG_value   ("length"    , x_len);
   --rce;  if (x_len <= 0)  {
      DEBUG_REGS   yLOG_note    ("record is empty");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   DEBUG_REGS   yLOG_info    ("fixed"     , s_recd);
   --rce;  if (s_recd [0] == '\0') {
      DEBUG_REGS   yLOG_note    ("starts with a null");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   --rce;  if (s_recd [0] == '#' && s_recd [1] == '#') {
      DEBUG_REGS   yLOG_note    ("starts with a non-content marker (##)");
      DEBUG_REGS   yLOG_char    ("s_recd [2]", s_recd [2]);
      switch (s_recd [2]) {
      case '@' :  EXIM__import_bounds ();        break;
      case 'x' :  EXIM__import_sizer ('x');      break;
      case 'y' :  EXIM__import_sizer ('y');      break;
      }
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   switch (s_recd [x_len - 1]) {
   case '' :
      if (strncmp (s_recd, "cell ", 6) == 0) {
         DEBUG_REGS   yLOG_note    ("native format detected");
         s_style = 'n';
      } else {
         DEBUG_REGS   yLOG_note    ("field/column delimited detected");
         s_style = 'f';
         s_recd [x_len - 1] = '\0';
      }
      break;
   case ','  :
      DEBUG_REGS   yLOG_note    ("comma separated values detected");
      s_style = 'c';
      s_recd [x_len - 1] = '\0';
      break;
   case '\t' :
      DEBUG_REGS   yLOG_note    ("tab delimited detected");
      s_style = 't';
      s_recd [x_len - 1] = '\0';
      break;
   default   :
      DEBUG_REGS   yLOG_note    ("values/printables only detected");
      s_style = 'v';
      break;
   }
   DEBUG_REGS   yLOG_char    ("s_style"   , s_style);
   s_max = x_len;
   DEBUG_REGS   yLOG_value   ("s_max"     , s_max);
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM_import             (char a_style)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   int         x_row       =   0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_REGS   yLOG_char    ("a_style"   , a_style);
   rc = EXIM__open ('r');
   DEBUG_REGS   yLOG_value   ("prep"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process lines)------------------*/
   s_hist      = HIST_BEG;
   while (1) {
      /*---(read)------------------------*/
      rc = EXIM__import_read ();
      if (rc >  0)  continue;
      if (rc <  0)  break;
      /*---(handle values)---------------*/
      switch (s_style) {
      case 'v' : case 'V' :   EXIM__import_values  (x_row);      break;
      case 'f' : case 'F' :   EXIM__import_fields  (x_row);      break;
      case 'd' : case 'D' :   EXIM__import_fields  (x_row);      break;
      case 't' : case 'T' :   EXIM__import_fields  (x_row);      break;
      case 'n' : case 'N' :   EXIM__import_native  ();           break;
      }
      /*---(next)------------------------*/
      ++x_row;
   }
   /*---(close file)---------------------*/
   rc = EXIM__close  ();
   DEBUG_REGS   yLOG_value   ("done"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.RE5.1E2.D3]*/ /*-[05.0000.013.!]-*/ /*-[--.---.---.--]-*/
EXIM_export             (char a_style)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   char       *x_valid     = "vdfctr·VDFCTR·SN";
   FILE       *f           = NULL;
   tCELL      *x_curr      = NULL;
   int         x_tab       = 0;
   int         x_col       = 0;
   int         x_row       = 0;
   int         x_rowsave   = 0;
   int         w           = 0;
   char        x_source    [LEN_RECD]  = "";
   char        x_modded    [LEN_RECD]  = "";
   char        x_print     [LEN_RECD]  = "";
   char        x_trim      [LEN_RECD]  = "";
   int         c           = 0;
   int         o           = 0;
   int         t           = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_REGS   yLOG_char    ("a_style"   , a_style);
   DEBUG_REGS   yLOG_info    ("x_valid"   , x_valid);
   --rce;  if (strchr (x_valid, a_style) == NULL) {
      DEBUG_REGS   yLOG_note    ("style not valid");
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(open output file)---------------*/
   rc = EXIM__open ('w');
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   s_style = a_style;
   if (strchr ("VDFCTRSN", s_style) != NULL) {
      fprintf (s_clip, "### generated by gyges-hekatonkheires (hundred-handed) spreadsheet\n");
      /*> fprintf (s_clip, "##? ");                                                   <* 
       *> switch (s_style) {                                                          <* 
       *> case 'V' :  fprintf (s_clip, "V = pure printable values");      break;      <* 
       *> case 'D' :  fprintf (s_clip, "D = column delimited");           break;      <* 
       *> case 'F' :  fprintf (s_clip, "F = field delimited");            break;      <* 
       *> case 'C' :  fprintf (s_clip, "C = comma separated values");     break;      <* 
       *> case 'T' :  fprintf (s_clip, "T = tab delimited");              break;      <* 
       *> case 'N' :  fprintf (s_clip, "N = native format");              break;      <* 
       *> }                                                                           <* 
       *> fprintf (s_clip, "\n");                                                     <*/
      EXIM__export_sizer ();
   }
   /*> if (strchr ("VDFCTN", s_style) != NULL) {                                      <* 
    *> }                                                                              <*/
   /*---(process independent cells)------*/
   rc    = yVIKEYS_first (&x_tab, &x_col, &x_row, NULL);
   x_curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
   x_rowsave = x_row;
   while (rc >= 0) {
      DEBUG_REGS   yLOG_point   ("x_curr"    , x_curr);
      ++c;
      /*---(look for line break)---------*/
      if (strchr ("vdfctr·VDFCTRS", s_style) != NULL && x_row != x_rowsave) {
         DEBUG_REGS   yLOG_note    ("line break");
         fprintf (s_clip, "\n");
         ++t;
      }
      /*---(fill in blank cells)---------*/
      if (x_curr == NULL) {
         DEBUG_REGS   yLOG_note    ("NULL cell");
         w = COL_size (x_tab, x_col);
         switch (s_style) {
         case 'v' : case 'V' :  fprintf (s_clip, "%*.*s", w, w, g_empty);       break;
         case 'd' : case 'D' :  fprintf (s_clip, "%*.*s", w, w, g_empty);     break;
         case 'f' : case 'F' :  fprintf (s_clip, " ");                        break;
         case 'c' : case 'C' :  fprintf (s_clip, "\"\",");                      break;
         case 't' : case 'T' :  fprintf (s_clip, " \t");                        break;
         case 'r' : case 'R' :  fprintf (s_clip, " ");                        break;
         case 'S' :             fprintf (s_clip, " ");                        break;
         }
      }
      /*---(write filled cells)----------*/
      else {
         DEBUG_REGS   yLOG_info    ("cell"      , x_curr->label);
         /*---(source)-------------------*/
         if (x_curr->source != NULL) {
            DEBUG_REGS   yLOG_note    ("convert source string");
            strlcpy      (x_source, x_curr->source, LEN_RECD);
            strldecode   (x_source, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("source is NULL");
         }
         DEBUG_REGS   yLOG_info    ("source"    , x_source);
         /*---(modded string)------------*/
         if (strchr (YCALC_GROUP_NUM, x_curr->type) != NULL) {
            sprintf (x_modded, "%lf", x_curr->v_num);
         } else {
            if (x_curr->v_str != NULL)   strlcpy (x_modded, x_curr->v_str , LEN_RECD);
            else                         strlcpy (x_modded, x_curr->source, LEN_RECD);
         }
         strldecode   (x_modded, LEN_RECD);
         /*---(printable)----------------*/
         if (x_curr->print != NULL) {
            DEBUG_REGS   yLOG_note    ("convert printable");
            strlcpy  (x_print , x_curr->print, LEN_RECD);
            strldecode   (x_print, LEN_RECD);
            strlcpy  (x_trim  , x_print, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("printable is NULL");
         }
         /*---(trimmed printable)--------*/
         strltrim (x_trim, ySTR_BOTH, LEN_RECD);
         DEBUG_REGS   yLOG_info    ("x_print"   , x_print);
         DEBUG_REGS   yLOG_info    ("x_trim"    , x_trim);
         DEBUG_REGS   yLOG_info    ("modified"  , x_modded);
         switch (s_style) {
         case 'v' : case 'V' :
            fprintf (s_clip, "%s"                  , x_print);
            break;
         case 'd' : case 'D' :
            fprintf (s_clip, "%s"                , x_print);
            break;
         case 'f' : case 'F' :
            fprintf (s_clip, "%s"                , x_trim);
            break;
         case 'r' : case 'R' :
            fprintf (s_clip, "%s"                , x_modded);
            break;
         case 'c' : case 'C' :
            fprintf (s_clip, "\"%s\","             , x_trim);
            break;
         case 't' : case 'T' :
            fprintf (s_clip, "%s\t"                , x_trim);
            break;
         case 'S' :
            fprintf (s_clip, "%s"                , x_source);
            break;
         case 'N' :
            fprintf (s_clip, "cell  %-8.8s  "  , x_curr->label);
            fprintf (s_clip, "%c%c%c%c-  "       , x_curr->align, x_curr->format, x_curr->decs, x_curr->unit);
            fprintf (s_clip, "%s \n"             , x_source);
            ++t;
            break;
         }
         ++o;
      }
      x_rowsave = x_row;
      rc      = yVIKEYS_next  (&x_tab, &x_col, &x_row, NULL);
      x_curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
   };
   /*---(close file)---------------------*/
   DEBUG_REGS   yLOG_note    ("closing file");
   if (strchr ("Nn", s_style) == NULL) {
      fprintf (s_clip, "\n");
      ++t;
   }
   if (strchr ("VDFCTRSN", s_style) != NULL) {
      fprintf (s_clip, "### complete, finito, done.  %d cells, %d occupied, %d lines\n", c, o, t);
   }
   rc = EXIM__close ();
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM_init               (void)
{
   yVIKEYS_cmds_add (YVIKEYS_M_FILE, "import"      , ""    , "c"    , EXIM_import  , "data import driver");
   yVIKEYS_menu_add ("µfma", "auto"      , ":import +¦");
   yVIKEYS_menu_add ("µfmv", "values"    , ":import v¦");
   yVIKEYS_menu_add ("µfmV", "values+"   , ":import V¦");
   yVIKEYS_menu_add ("µfmd", "coldel"    , ":import d¦");
   yVIKEYS_menu_add ("µfmD", "coldel+"   , ":import D¦");
   yVIKEYS_menu_add ("µfmf", "field"     , ":import f¦");
   yVIKEYS_menu_add ("µfmF", "field+"    , ":import F¦");
   yVIKEYS_menu_add ("µfmt", "tab"       , ":import t¦");
   yVIKEYS_menu_add ("µfmT", "tab+"      , ":import T¦");
   yVIKEYS_menu_add ("µfmn", "navive"    , ":import N¦");
   yVIKEYS_cmds_add (YVIKEYS_M_FILE, "export"      , ""    , "c"    , EXIM_export  , "data export driver");
   yVIKEYS_menu_add ("µfxa", "values"    , ":export v¦");
   yVIKEYS_menu_add ("µfxV", "values+"   , ":export V¦");
   yVIKEYS_menu_add ("µfed", "coldel"    , ":export d¦");
   yVIKEYS_menu_add ("µfxD", "coldel+"   , ":export D¦");
   yVIKEYS_menu_add ("µfxf", "field"     , ":export f¦");
   yVIKEYS_menu_add ("µfxF", "field+"    , ":export F¦");
   yVIKEYS_menu_add ("µfxc", "csv"       , ":export c¦");
   yVIKEYS_menu_add ("µfxC", "csv+"      , ":export C¦");
   yVIKEYS_menu_add ("µfxt", "tab"       , ":export t¦");
   yVIKEYS_menu_add ("µfxT", "tab+"      , ":export T¦");
   yVIKEYS_menu_add ("µfxr", "results"   , ":export r¦");
   yVIKEYS_menu_add ("µfxR", "results+"  , ":export R¦");
   yVIKEYS_menu_add ("µfxs", "source"    , ":export S¦");
   yVIKEYS_menu_add ("µfxn", "navive"    , ":export N¦");
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void  o___UNIT_TEST_______o () { return; }

char         /*--> read the last heartbeat -----------------------------------*/
FILE__unit_exim_entry   (int a_num, char *a_entry)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   FILE       *f           = NULL;
   int         c           =    0;
   char        x_recd      [LEN_RECD] = "n/a";
   char        x_found     =  '-';
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_REGS   yLOG_point   ("a_entry"   , a_entry);
   --rce;  if (a_entry == NULL) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strncpy (a_entry, "", LEN_RECD);
   DEBUG_REGS   yLOG_value   ("a_num"     , a_num);
   --rce;  if (a_num < 0)  {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(open)---------------------------*/
   DEBUG_REGS   yLOG_info    ("a_file"    , FILE_EXIM);
   f = fopen (FILE_EXIM, "rt");
   DEBUG_REGS   yLOG_point   ("f"         , f);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(read)---------------------------*/
   --rce;  while (1) {
      fgets (x_recd, LEN_RECD, f);
      if (feof (f)) {
         DEBUG_REGS   yLOG_note    ("hit end of file");
         break;
      }
      if (c >= a_num) {
         x_len = strlen (x_recd);
         if (x_len > 0)  x_recd [--x_len] = '\0';
         strncpy (a_entry, x_recd, LEN_RECD);
         DEBUG_REGS   yLOG_info    ("entry"     , a_entry);
         x_found = 'y';
         break;
      }
      ++c;
   }
   /*---(close)--------------------------*/
   rc = fclose (f);
   DEBUG_REGS   yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(not found)----------------------*/
   --rce;  if (x_found != 'y') {
      DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return 0;
}

char*        /*-> unit test accessor -----------------[ ------ [gs.950.221.M1]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
FILE__unit              (char *a_question, int a_ref)
{
   /*---(locals)-----------+-----+-----+-*/
   int         rc          =   -1;
   char        t           [LEN_RECD]  = "";
   char        s           [LEN_RECD]  = "";
   int         i           =    0;
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "FILE             : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "exim"      )    == 0) {
      rc = FILE__unit_exim_entry (a_ref, t);
      if (rc < 0) {
         snprintf (unit_answer, LEN_FULL, "FILE exim   (%2d) : none", a_ref);
      } else {
         strlencode (t, ySTR_MAX, LEN_RECD);
         snprintf (unit_answer, LEN_FULL, "FILE exim   (%2d) : %2d[%-.40s]", a_ref, strlen (t), t);
      }
   }
   if      (strcmp (a_question, "map"       )    == 0) {
      for (i = 0; i < 10; ++i) {
         sprintf (t, " %2d", s_map [i]);
         strcat  (s, t);
      }
      snprintf (unit_answer, LEN_FULL, "FILE map         :%s", s);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
