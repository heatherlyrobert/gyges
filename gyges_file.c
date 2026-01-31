/*============================----beg-of-source---============================*/



/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/
/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */

#define  P_COPYRIGHT   \
   "copyright (c) 2010 robert.s.heatherly at balsashrike at gmail dot com"

#define  P_LICENSE     \
   "the only place you could have gotten this code is my github, my website,¦"   \
   "or illegal sharing. given that, you should be aware that this is GPL licensed."

#define  P_COPYLEFT    \
   "the GPL COPYLEFT REQUIREMENT means any modifications or derivative works¦"   \
   "must be released under the same GPL license, i.e, must be free and open."

#define  P_INCLUDE     \
   "the GPL DOCUMENTATION REQUIREMENT means that you must include the original¦" \
   "copyright notice and the full licence text with any resulting anything."

#define  P_AS_IS       \
   "the GPL NO WARRANTY CLAUSE means the software is provided without any¦"      \
   "warranty and the author cannot be held liable for damages."

#define  P_THEFT    \
   "if you knowingly violate the spirit of these ideas, i suspect you might "    \
   "find any number of freedom-minded hackers may take it quite personally ;)"

/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */
/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/

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


/*
 *  export/import sizer
 *  º  beginning of input
 *  Ï  beginning of field, interpret intelligently
 *     beginning of field, force as string by appending '·'
 *  #  beginning of field, force as number (especially leading zeros)
 *  -  character included in field
 *  ·  space to be ignored
 *  »  end of sizer
 *
 *  sizer's length is important as content input is ignored if it is beyond
 *  the end of the sizer.
 *
 *
 *               127-bit mode                    256-bit mode
 *  outside      [   ]                           å···æ
 *  junk space                                   ··
 *  intellegent    (-------                      ··Ï-------··
 *  string         #-------                      ··#-------··
 *  number         9-------                      ··9-------··
 *  field          (-------f                     ··Ï-------f··
 *  comma          (-------c                     ··Ï-------c··
 *  tab            (-------t                     ··Ï-------t··
 *
 *  sizer indicates final column width in host application, e.g., gyges
 *  f c t fields means the data continues until, but field is as sized (not auto)
 *
 *
 *  ·· -------··   force as string (make a number into a string with trailing space)
 *  ··=-------··   force as number (take out lead zeros, commas, dollar signs, etc)
 *
 *  f  §  
 *  c  ,  ±
 *  t  Ú
 *
 */


#define  EXIM_VALID    "vdfctrsnVDFCTRSN"
#define  EXIM_UPPER    "VDFCTRSN"


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
char        s_xsizer    [LEN_RECD];
int         s_nsizer    =   0;

#define     MAX_POS     20
uchar       s_beg       [MAX_POS];          /* start pos of each column       */
uchar       s_wide      [MAX_POS];          /* width of each column           */
uchar       s_type      [MAX_POS];          /* type of each column            */
uchar       s_xsep      [MAX_POS];
int         s_npos      =   0;
int         s_mundos    =   0;


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
   /*> TAB_new_in_abbr ('0', NULL, NULL);                                             <*/
   TAB_new_in_abbr ('®', NULL, NULL);
   TAB_new_in_abbr ('¯', NULL, NULL);
   /*> MAP_mapper (YMAP_INIT);                                                        <*/
   /*> yMAP_jump (0, 0, 0, 0);                                                        <*/
   yCALC_set_state ('L');
   my.start = 0;
   DEBUG_OUTP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
FILE_finisher           (void)
{
   DEBUG_OUTP    yLOG_enter   (__FUNCTION__);
   yCALC_set_state ('-');
   TAB_switch (my.start);
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
   /*> MAP_mapper (YMAP_INIT);                                                        <*/
   yMAP_jump (0, 0, 0, 0);
   /*> yCMD_direct ("0a1");                                                   <*/
   return 0;
}

char
FILE_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(handlers)-----------------------*/
   /*> rc = yCMD_add (YVIKEYS_M_FILE  , "new"         , ""    , ""     , FILE_new                     , "clear out all content"                                       );   <*/
   /*---(complete)-----------------------*/
   return 0;
}





/*> char         /+-> tbd --------------------------------[ ------ [gc.320.121.32]+/ /+-[00.0000.00#.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> FILE_writeas         (char *a_name)                                                                                            <* 
 *> {                                                                                                                              <* 
 *>    char        rc          =    0;                                                                                             <* 
 *>    char        x_name      [LEN_RECD]  = "";                                                                                   <* 
 *>    ystrlcpy (x_name, my.f_name, LEN_RECD);                                                                                      <* 
 *>    if (rc >= 0)  rc = FILE_rename (a_name);                                                                                    <* 
 *>    if (rc >= 0)  rc = FILE_write  ();                                                                                          <* 
 *>    if (rc >= 0)  rc = FILE_rename (x_name);                                                                                    <* 
 *>    return rc;                                                                                                                  <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                         import/export                        ----===*/
/*====================------------------------------------====================*/
static void  o___IMPORT__________o () { return; }

FILE   *g_clip     = NULL;
static int     s_lines    =    0;

static int     s_max      =    0;

static char    s_sizer    =  '-';
static char    s_style    =  '-';
static char    s_hist      = YMAP_BEG;
static char    s_mapper   =  '-';
static short   s_map      [MAX_COLS];

static short   s_icol     =    0;
static short   s_mcol     =    0;
static short   s_ccol     =    0;
static short   s_crow     =    0;


static int     s_uorig    =    0;
static int     s_xorig    =    0;
static int     s_yorig    =    0;

char
EXIM__open              (char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_FILE   yLOG_point   ("g_clip"    , g_clip);
   --rce;  if (g_clip != NULL) {
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(open file)----------------------*/
   DEBUG_FILE   yLOG_info    ("FILE_EXIM" , FILE_EXIM);
   DEBUG_FILE   yLOG_char    ("a_dir"     , a_dir);
   switch (a_dir) {
   case 'r'  :
      DEBUG_FILE   yLOG_note    ("open for reading");
      g_clip = fopen (FILE_EXIM, "rt");
      break;
   case 'w'  :
      DEBUG_FILE   yLOG_note    ("open for writing");
      g_clip = fopen (FILE_EXIM, "wt");
      break;
   default   :
      DEBUG_FILE   yLOG_note    ("not a legal file mode");
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return rce;
      break;
   }
   DEBUG_FILE   yLOG_point   ("g_clip"    , g_clip);
   --rce;  if (g_clip == NULL) {
      DEBUG_FILE   yLOG_note    ("can not open clip file");
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__close             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_FILE   yLOG_point   ("g_clip"    , g_clip);
   --rce;  if (g_clip == NULL) {
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(close file)---------------------*/
   rc = fclose  (g_clip);
   DEBUG_FILE   yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_FILE   yLOG_note    ("can not close clip file");
      DEBUG_FILE   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(ground)-------------------------*/
   g_clip = NULL;
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
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
 *>       ystrlcpy (s_q, " "  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 'c'  : case 'C'  :                                                                                                     <* 
 *>       ystrlcpy (s_q, ","  , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 't'  : case 'T'  :                                                                                                     <* 
 *>       ystrlcpy (s_q, "\t" , 5);                                                                                                 <* 
 *>       break;                                                                                                                   <* 
 *>    case 'r'  : case 'R'  :                                                                                                     <* 
 *>       ystrlcpy (s_q, "" , 5);                                                                                                  <* 
 *>       break;                                                                                                                   <* 
 *>    case 'v'  : case 'V'  :                                                                                                     <* 
 *>       ystrlcpy (s_q, " "  , 5);                                                                                                 <* 
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
 *>    g_clip = fopen (FILE_EXIM, "rt");                                                                                           <* 
 *>    DEBUG_REGS   yLOG_point   ("g_clip"    , g_clip);                                                                           <* 
 *>    --rce;  if (g_clip == NULL) {                                                                                               <* 
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
 *>       DEBUG_REGS   yLOG_value   ("d_num"     , x_curr->d_num);                                                                 <* 
 *>       if (x_curr->d_num <= 0) {                                                                                                <* 
 *>          DEBUG_REGS   yLOG_note    ("end of mapping (zero or negative cell)");                                                 <* 
 *>          DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                             <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       s_mapper = 'y';                                                                                                          <* 
 *>       s_crow   = CROW + 1;                                                                                                     <* 
 *>       s_map [i] = x_curr->d_num;                                                                                               <* 
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
 *>       ystrltrim (p, ySTR_BOTH, LEN_RECD);                                                                                       <* 
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

/*> char         /+-> complete data import ---------------[ ------ [fz.311.001.02]+/ /+-[00.0000.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> EXIM_import__done       (void)                                                                                                 <* 
 *> {                                                                                                                              <* 
 *>    /+---(close file)---------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("closing file");                                                                                 <* 
 *>    fclose  (g_clip);                                                                                                           <* 
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
   ushort      u, xb, xe, yb, ye, zb, ze;
   int         x_col, x_row, i;
   int         w, h;
   char        t           [LEN_RECD]  = "";
   char        s           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(size)---------------------------*/
   yMAP_visu_range (&u, &xb, &xe, &yb, &ye, &zb, &ze, NULL);
   fprintf (g_clip, "#@ bounds  %2d %4d %4d %4d %4d %4d %4d\n",  u, xb, xe, yb, ye, zb, ze);
   /*---(x_size)----------------------*/
   sprintf (s, "#@ x-parse %3då", xe - xb + 1);
   for (x_col = xb; x_col <= xe; ++x_col) {
      w     = COL_size (CTAB, x_col);
      DEBUG_OUTP  yLOG_value   ("w"         , w);
      strcat  (s, "Ï");
      for (i = 1; i < (w - 1); ++i)   strcat  (s, "-");
      switch (s_style) {
      case 'V' : case 'N' :
         strcat  (s, "·");
         break;
      case 'D' : case 'F' :  case 'R' :  case 'S' :
         strcat  (s, "§");
         break;
      case 'T' :
         strcat  (s, "Ú");
         break;
      case 'C' :
         strcat  (s, ",");
         break;
      }
      DEBUG_OUTP  yLOG_info    ("s"         , s);
   }
   fprintf (g_clip, "%sæ\n", s);
   /*---(y_size)----------------------*/
   /*> sprintf (s, "#@ y-parse %3då", ye - yb + 1);                                   <* 
    *> for (x_row = yb; x_row <= ye; ++x_row) {                                       <* 
    *>    h     = ROW_size (CTAB, x_row);                                             <* 
    *>    DEBUG_OUTP  yLOG_value   ("h"         , h);                                 <* 
    *>    sprintf (t, " %5d", h);                                                     <* 
    *>    strcat  (s, "Ï");                                                           <* 
    *>    for (i = 1; i < h; ++i)   strcat  (s, "-");                                 <* 
    *>    DEBUG_OUTP  yLOG_info    ("s"         , s);                                 <* 
    *> }                                                                              <* 
    *> fprintf (g_clip, "%sæ\n", s);                                                  <*/
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_fail       (void)
{
   int         i           =    0;
   if (s_npos >= 0)  yMAP_mundo_rollback ();
   for (i = 0; i < MAX_POS; ++i) {
      s_wide  [i] = 0;
      s_xsep  [i] = '·';
      s_beg   [i] = 0;
      s_type  [i] = '·';
   }
   s_npos = 0;
   return 0;
}

char
EXEC__import_space      (cchar c, char *r_space)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_spoint  (r_space);
   --rce;  if (r_space == NULL) {
      DEBUG_INPT   yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_schar   (*r_space);
   /*---(start)--------------------------*/
   if (strchr ("· " , c) != NULL) {
      DEBUG_INPT   yLOG_snote   ("normal spacer");
      *r_space = 'y';
      rc = 1;
   }
   /*---(end)----------------------------*/
   else if (strchr ("]æ", c) != NULL) {
      DEBUG_INPT   yLOG_snote   ("end-of-definition");
      *r_space = '-';
   }
   else if (strchr ("(Ï#9", c) != NULL) {
      DEBUG_INPT   yLOG_snote   ("beg-of-field");
      *r_space = '-';
   }
   /*---(trouble)------------------------*/
   if (*r_space == 'y' && strchr ("· ", c) == NULL) {
      DEBUG_INPT   yLOG_snote   ("space broken by unexpected character");
      EXIM__import_fail ();
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_sexit   (__FUNCTION__);
   return rc;
}

char
EXIM__import_sizer      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_dir       =  '-';
   char        x_label     [LEN_LABEL];
   int         l           =    0;
   int         i           =    0;
   char        c           =    0;
   int         x_size      =    0;
   char        x_space     =  '-';
   int         x_mundo     =    0;
   int         x_count     =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   for (i = 0; i < MAX_POS; ++i) { s_wide  [i] = 0; s_xsep [i] = '·'; s_beg [i] = 0; s_type [i] = '·'; }
   s_npos = 0;
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_info    ("s_recd"    , s_recd);
   --rce;  if (strncmp (s_recd, "#@ ",  3) != 0) {
      DEBUG_INPT   yLOG_note    ("does not start with #@ ");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_dir = s_recd [3];
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_char    ("x_dir"     , x_dir);
   --rce;  if (strchr ("xy", x_dir) == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("s_recd"    , s_recd);
   --rce;  if (x_dir == 'x' && strncmp (s_recd, "#@ x-parse ", 11) != 0) {
      DEBUG_INPT   yLOG_note    ("prefix is not #@ x-parse ");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (x_dir == 'y' && strncmp (s_recd, "#@ y-parse ", 11) != 0) {
      DEBUG_INPT   yLOG_note    ("prefix is not #@ y-parse ");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   l = strlen (s_recd);
   /*> --rce;  if (s_recd [l - 1] != 'æ') {                                           <* 
    *>    DEBUG_INPT   yLOG_note    ("does not end with æ");                          <* 
    *>    DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);                              <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
   /*---(open sizer)---------------------*/
   i = 14;
   c = s_recd [i];
   DEBUG_INPT   yLOG_complex ("curr"      , "%2d#, %c, %c, %2dn, %2ds", i, c, x_space, s_npos, x_size);
   --rce;  if (strchr ("å[", c) == NULL) {
      DEBUG_INPT   yLOG_note    ("does not beg with å");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_npos = -1;
   /*---(backup sizer)-------------------*/
   if (x_dir == 'x') {
      ystrlcpy (s_xsizer, s_recd + i + 1, LEN_RECD);
      s_nsizer = strlen (s_xsizer) - 1;
   }
   /*---(open sizer)---------------------*/
   c = s_recd [++i];
   DEBUG_INPT   yLOG_complex ("curr"      , "%2d#, %c, %c, %2dn, %2ds", i, c, x_space, s_npos, x_size);
   --rce;  while (c != '\0') {
      /*---(check for spacers)-----------*/
      if (strchr ("· " , c) != NULL)   x_space = 'y';
      if (strchr ("Ïæ] x", c) != NULL)   x_space = '-';
      if (x_space == 'y') {
         if (strchr ("· ", c) == NULL) {
            DEBUG_INPT   yLOG_note    ("space broken by unexpected character");
            EXIM__import_fail ();
            DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
            return rce;
         }
      }
      /*---(real things)-----------------*/
      if (x_space == '-') {
         /*---(new field)-------------------*/
         if (strchr ("Ïæ] x", c) != NULL) {
            DEBUG_INPT   yLOG_note    ("Ï x start next field or æ] ending");
            if (strchr ("Ï x", c) != NULL) {
               s_beg  [s_npos + 1] = i;
               s_type [s_npos + 1] = c;
            }
            if (s_npos >= 0) {
               DEBUG_INPT   yLOG_note    ("check for resize");
               ++x_size;
               if (s_recd [s_beg [s_npos]] == ' ') ++x_size;
               DEBUG_INPT   yLOG_value   ("x_size"    , x_size);
               s_wide   [s_npos] = x_size;
               DEBUG_INPT  yLOG_complex ("mundos"    , "%3dc, %3di, %3ds", yMAP_mundo_count (), yMAP_mundo_current (), s_mundos);
               x_mundo = yMAP_mundo_count ();
               DEBUG_INPT   yLOG_value   ("x_mundo"   , x_mundo);
               if (x_dir == 'x') {
                  DEBUG_INPT   yLOG_note    ("update width");
                  ystr4gyges (CTAB, CCOL + s_npos, 0, 0, 0, x_label, YSTR_ADAPT);
                  yMAP_multi_wide (x_label, x_size, 0);
                  DEBUG_INPT  yLOG_complex ("mundos+"   , "%3dc, %3di, %3ds", yMAP_mundo_count (), yMAP_mundo_current (), s_mundos);
                  DEBUG_INPT   yLOG_value   ("x_count"   , x_count);
                  if (yMAP_mundo_count () != x_mundo) {
                     DEBUG_INPT   yLOG_note    ("new one added");
                     DEBUG_INPT   yLOG_info    ("detail"    , yMAP_mundo_detail (x_count));
                     if (yMAP_mundo_count () > s_mundos && x_count > 0) {
                        DEBUG_INPT   yLOG_note    ("change to add");
                        yMAP_mundo_make_add ();
                     }
                     ++x_count;
                  }
               } else if (x_dir == 'y') {
                  DEBUG_INPT   yLOG_note    ("update height");
                  /*> ystr4gyges (CTAB, 0, CROW + s_npos, 0, 0, x_label, YSTR_ADAPT);   <* 
                   *> yMAP_multi_tall (x_label, x_size, 0);                            <* 
                   *> if (yMAP_mundo_count () > s_mundos)  yMAP_mundo_make_add ();     <*/
               }
            }
            ++s_npos;
            x_size = 1;
            if (strchr ("æ]", c) != NULL) {
               DEBUG_INPT   yLOG_note    ("æ] ending so break");
               break;
            }
         }
         /*---(continuation)----------------*/
         else if (c == '-') {
            DEBUG_INPT   yLOG_note    ("- continuation character");
            if (s_npos < 0) {
               DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
            }
            ++x_size;
         }
         /*---(other formatting)------------*/
         else {
            switch (c) {
            case 'f' : case '§' : case '' :
               DEBUG_INPT   yLOG_note    ("field delimiter");
               s_xsep [s_npos] = 'f';
               break;
            case ',' : case 'c' : case '±' :
               DEBUG_INPT   yLOG_note    ("comma delimiter");
               s_xsep [s_npos] = 'c';
               break;
            case 't' : case 'Ú' :
               DEBUG_INPT   yLOG_note    ("tab delimiter");
               s_xsep [s_npos] = 't';
               break;
            default  :
               DEBUG_INPT   yLOG_note    ("unknown character");
               EXIM__import_fail ();
               DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
               break;
            }
         }
      }
      /*---(next)------------------------*/
      c = s_recd [++i];
      DEBUG_INPT   yLOG_complex ("curr"      , "%2d#, %c, %c, %2dn, %2ds", i, c, x_space, s_npos, x_size);
      /*---(done)------------------------*/
   }
   /*---(finished right)-----------------*/
   --rce;  if (c == 0 || strchr ("æ]", c) == NULL) {
      DEBUG_INPT   yLOG_note    ("did not complete with æ]");
      EXIM__import_fail ();
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
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
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_INPT   yLOG_info    ("s_recd"    , s_recd);
   --rce;  if (strncmp (s_recd, "#@ bounds ", 10) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(sizings)------------------------*/
   for (x_pos = 0; x_pos < 7; ++x_pos) {
      ystrlcpy (t, s_recd + 9 + (x_pos * 5), 5);
      t [4] = '\0';
      DEBUG_INPT   yLOG_delim   ("t"         , t);
      x_value = atoi (t);
      DEBUG_INPT   yLOG_value   ("x_value"   , x_value);
      switch (x_pos) {
      case  0 :  s_uorig = x_value;     break;
      case  1 :  s_xorig = x_value;     break;
      case  3 :  s_yorig = x_value;     break;
      }
   }
   DEBUG_INPT   yLOG_value   ("s_uorig"   , s_uorig);
   DEBUG_INPT   yLOG_value   ("s_xorig"   , s_xorig);
   DEBUG_INPT   yLOG_value   ("s_yorig"   , s_yorig);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_one        (int a_tab, int a_col, int a_row, char *a_format, char *a_source)
{
   char        rce         =  -10;
   tCELL      *x_new       = NULL;
   char        t           [LEN_RECD]  = "";
   int         x_len       =    0;
   DEBUG_INPT  yLOG_note    ("processing and handling mundo");
   if (yMAP_mundo_count () > s_mundos) {
      DEBUG_INPT  yLOG_note    ("switch to an add");
      s_hist = YMAP_ADD;
   }
   ystrlcpy    (t, a_source , LEN_RECD);
   ystrlencode (t, ySTR_NORM, LEN_RECD);
   ystrltrim   (t, ySTR_BOTH, LEN_RECD);
   x_len = strlen (t);
   if (x_len <= 0) {
      DEBUG_INPT  yLOG_note    ("empty source, skipping");
      return 0;
   }
   x_new = CELL_overwrite (s_hist, a_tab, a_col, a_row, a_source, a_format);
   DEBUG_INPT  yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL)   return rce;
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.732.141.12]*/ /*-[01.0000.014.3]-*/ /*-[--.---.---.--]-*/
EXIM__import_values     (int a_row)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rc          =    0;
   int         x_col       =    0;
   int         w           =    0;
   int         cw          =    0;
   char        t           [LEN_RECD]  = "";
   char        x_format    [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(process cells)---------------*/
   while (cw < s_max) {
      /*---(check for masking)--------*/
      DEBUG_INPT  yLOG_complex ("s_nsizer"  , "%3d, %3d, %s", cw, s_nsizer, s_xsizer);
      if (s_nsizer > 0 && cw >= s_nsizer) {
         DEBUG_INPT  yLOG_note    ("past end of sizer, break");
         break;
      }
      DEBUG_INPT  yLOG_char    ("s_xsizer"  , s_xsizer [cw]);
      if (s_nsizer > 0 && strchr ("· ", s_xsizer [cw]) != NULL) {
         DEBUG_INPT  yLOG_note    ("found a space, skipping character");
         ++cw;
         continue;
      }
      /*---(read import data)---------*/
      w     = COL_size (CTAB, CCOL + x_col);
      if (s_xsizer [cw] == ' ') --w;
      DEBUG_INPT  yLOG_value   ("w"         , w);
      if (s_nsizer > 0 && cw + w - 1 > s_nsizer) {
         DEBUG_INPT  yLOG_note    ("truncating width to match nzizer");
         w = s_nsizer - cw;
         DEBUG_INPT  yLOG_value   ("w"         , w);
      }
      /*---(filter to0 short)---------*/
      if (w < 2) {
         ++x_col;
         cw += w;
         continue;
      }
      /*---(take string section)------*/
      DEBUG_INPT  yLOG_delim   ("t (orig)"  , t);
      ystrlcpy      (t, s_recd + cw, w);
      if (s_nsizer > 0) {
         if (s_xsizer [cw] == ' ') ystrlcat (t, "·", LEN_RECD);
      }
      DEBUG_INPT  yLOG_delim   ("t (orig)"  , t);
      /*---(guess formatting)---------*/
      strcpy (x_format, DEF_FORMAT);
      if      (t [0] == ' ' && t [w - 2] == ' ')  x_format [0] = '|';
      else if (t [0] != ' ')                      x_format [0] = '<';
      else if (t [w - 2] != ' ')                  x_format [0] = '>';
      /*---(process)------------------*/
      ystrlencode   (t, ySTR_NORM, LEN_RECD);
      DEBUG_INPT  yLOG_info    ("t (new)"   , t);
      ystrltrim     (t, ySTR_BOTH, LEN_RECD);
      DEBUG_INPT  yLOG_info    ("t (trim)"  , t);
      DEBUG_INPT  yLOG_value   ("x_col"     , x_col);
      /*---(change)-------------------*/
      DEBUG_INPT  yLOG_complex ("mundos"    , "%3dc, %3di, %3ds", yMAP_mundo_count (), yMAP_mundo_current (), s_mundos);
      if (strlen (t) > 0) {
         rc = EXIM__import_one (CTAB, CCOL + x_col, CROW + a_row, x_format, t);
         if (rc < 0) {
            EXIM__import_fail ();
            DEBUG_INPT   yLOG_exit    (__FUNCTION__);
            return rc;
         }
         /*> DEBUG_INPT  yLOG_note    ("processing and handling mundo");              <*/
         /*> if (yMAP_mundo_count () > s_mundos) {                                     <* 
          *>    DEBUG_INPT  yLOG_note    ("switch to an add");                         <* 
          *>    s_hist = YMAP_ADD;                                                     <* 
          *> }                                                                         <* 
          *> CELL_overwrite (s_hist, CTAB, CCOL + x_col, CROW + a_row, t, x_format);   <*/
         /*> DEBUG_INPT  yLOG_complex ("after over", "%3dc, %3di, %3ds", yMAP_mundo_count (), yMAP_mundo_current (), s_mundos);   <*/
         /*> if (yMAP_mundo_count () > s_mundos) {                                    <* 
          *>    DEBUG_INPT  yLOG_note    ("switch to an add");                        <* 
          *>    yMAP_mundo_make_add ();                                               <* 
          *> }                                                                        <*/
      } else {
         DEBUG_INPT  yLOG_note    ("empty, skipping");
      }
      /*---(next)---------------------*/
      ++x_col;
      --w;
      cw += w;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
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
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   DEBUG_INPT   yLOG_info    ("label"     , a_label);
   rc = ystr2gyges (a_label, &x_tab, &x_col, &x_row, NULL, NULL, 0, YSTR_ADAPT);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_complex ("src loc"   , "%2dt, %3dc, %4dr", x_tab, x_col, x_row);
   s_otab = CTAB - s_uorig;
   s_ocol = CCOL - s_xorig;
   s_orow = CROW - s_yorig;
   DEBUG_INPT   yLOG_complex ("offset"    , "%2dt, %3dc, %4dr", s_otab, s_ocol, s_orow);
   s_dtab = x_tab + s_otab;
   s_dcol = x_col + s_ocol;
   s_drow = x_row + s_orow;
   DEBUG_INPT   yLOG_complex ("dst loc"   , "%2dt, %3dc, %4dr", s_dtab, s_dcol, s_drow);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
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
   x_tab = s_uorig;
   x_col = s_xorig + a_col;
   x_row = s_yorig + a_row;
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   DEBUG_INPT   yLOG_complex ("src loc"   , "%2dt, %3dc, %4dr", x_tab, x_col, x_row);
   s_otab = CTAB - s_uorig;
   s_ocol = CCOL - s_xorig;
   s_orow = CROW - s_yorig;
   DEBUG_INPT   yLOG_complex ("offset"    , "%2dt, %3dc, %4dr", s_otab, s_ocol, s_orow);
   s_dtab = x_tab + s_otab;
   s_dcol = x_col + s_ocol;
   s_drow = x_row + s_orow;
   DEBUG_INPT   yLOG_complex ("dst loc"   , "%2dt, %3dc, %4dr", s_dtab, s_dcol, s_drow);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__import_content    (char *a_content)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   if (strchr ("cC", s_style) == NULL)  ystrltrim (a_content, ySTR_BOTH, LEN_RECD);
   ystrldchg (a_content, G_CHAR_STORAGE, G_KEY_SPACE, LEN_RECD);
   DEBUG_INPT   yLOG_info    ("a_content" , a_content);
   /*---(check cell type)----------------*/
   if (a_content [0] == '\0') {
      DEBUG_INPT   yLOG_note    ("empty cell");
      strcpy (s_source, "");
   } else if (strchr ("=#~", a_content [0]) != 0) {
      DEBUG_INPT   yLOG_note    ("formula, calling yRPN_adjust");
      rc = yRPN_addr_require (a_content, 'r', s_otab, s_ocol, s_orow, 0, LEN_RECD, s_source);
      DEBUG_INPT   yLOG_value   ("rc"        , rc);
      if (rc < 0) {
         DEBUG_INPT   yLOG_note    ("formula could not be parsed");
         strcpy (s_source, "not_translatable");
      }
   } else {
      DEBUG_INPT   yLOG_note    ("just copy straight across");
      strcpy (s_source, a_content);
   }
   DEBUG_INPT   yLOG_info    ("s_source"  , s_source);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.732.141.12]*/ /*-[01.0000.014.3]-*/ /*-[--.---.---.--]-*/
EXIM__import_fields     (int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_recd      [LEN_RECD]  = "";
   char        x_field     [LEN_RECD]  = "";
   int         x_len       =    0;
   int         x_col       =    0;
   char       *p           = NULL;
   char        x_format    [LEN_LABEL];
   char        q           [LEN_LABEL];
   tCELL      *x_new       = NULL;
   int         l           =    0;
   int         n           =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(prep)------------------------*/
   -rce;  switch (s_style) {
   case 'f' : case 'F' : case 'd' : case 'D' :
      strcpy (q, "");
      break;
   case 't' : case 'T' :
      strcpy (q, "\t");
      break;
   default  :
      return rce;
      break;
   }
   /*---(process cells)---------------*/
   ystrlcpy (x_recd, s_recd, LEN_RECD);
   p = strtok (x_recd, q);
   while (p != NULL) {
      /*---(prepare)------------------*/
      EXIM__import_destsrc (x_col, a_row);
      /*---(clean up)-----------------*/
      if (p [0] == ' ')   ystrlcpy  (x_field, p + 1, LEN_RECD);
      else                ystrlcpy  (x_field, p    , LEN_RECD);
      l = strlen (x_field);
      DEBUG_INPT  yLOG_value   ("l"         , l);
      if (n > 0)  x_field [l--] = '\0';  /*  final spacer  */
      if (strcmp (x_field, "²") == 0)  x_field [0] = '\0';
      DEBUG_INPT  yLOG_delim   ("x_field"   , x_field);
      /*---(guess formatting)---------*/
      strcpy (x_format, DEF_FORMAT);
      if      (x_field [0] == ' ' && x_field [l - 2] == ' ')  x_format [0] = '|';
      else if (x_field [0] != ' ' && x_field [l - 2] != ' ')  ;
      else if (x_field [0] != ' ')                            x_format [0] = '<';
      else if (x_field [l - 1] != ' ')                        x_format [0] = '>';
      /*---(process)------------------*/
      rc = EXIM__import_content (x_field);
      DEBUG_INPT   yLOG_value   ("content"   , rc);
      DEBUG_INPT  yLOG_info    ("s_source"  , s_source);
      /*---(update)-------------------------*/
      rc = EXIM__import_one (s_dtab, s_dcol, s_drow, x_format, s_source);
      DEBUG_INPT  yLOG_value   ("import_one", rc);
      if (rc < 0) {
         DEBUG_INPT  yLOG_note    ("FAILED IMPORT");
         EXIM__import_fail ();
         DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(next)---------------------*/
      ++x_col;
      ++n;
      p = strtok (NULL, q);
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [fz.732.141.12]*/ /*-[01.0000.014.3]-*/ /*-[--.---.---.--]-*/
EXIM__import_csv        (int a_row)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_recd      [LEN_RECD]  = "";
   char        x_field     [LEN_RECD]  = "";
   int         x_col       =    0;
   char       *p           = NULL;
   char       *b           = NULL;
   char        x_format    [LEN_LABEL] = DEF_FORMAT;
   char        q           [LEN_LABEL] = "\", ";
   int         l           =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(process cells)---------------*/
   ystrlcpy (x_recd, s_recd, LEN_RECD);
   ystrlcat (x_recd, ", ", LEN_RECD);
   b = x_recd;
   DEBUG_INPT  yLOG_info    ("b"         , b);
   p = strstr (b, q);
   DEBUG_INPT  yLOG_point   ("p"         , p);
   --rce;  while (p != NULL) {
      /*---(prepare)------------------*/
      EXIM__import_destsrc (x_col, a_row);
      DEBUG_INPT  yLOG_complex ("pos"       , "%3dc, %4dr", x_col, a_row);
      /*---(clean up)-----------------*/
      DEBUG_INPT  yLOG_info    ("b"         , b);
      p [0] = p [1] = p [2] = '\0';
      DEBUG_INPT  yLOG_info    ("b"         , b);
      b [0] = '\0';
      ++b;
      DEBUG_INPT  yLOG_info    ("b"         , b);
      if (b [0] != '\0') {
         ystrlcpy  (x_field, b, LEN_RECD);
         l = strlen (x_field);
         DEBUG_INPT  yLOG_value   ("l"         , l);
         DEBUG_INPT  yLOG_delim   ("x_field"   , x_field);
         /*---(process)------------------*/
         rc = EXIM__import_content (b);
         DEBUG_INPT  yLOG_value   ("content"   , rc);
         DEBUG_INPT  yLOG_info    ("s_source"  , s_source);
         /*---(update)-------------------------*/
         rc = EXIM__import_one (s_dtab, s_dcol, s_drow, x_format, s_source);
         if (rc < 0) {
            EXIM__import_fail ();
            DEBUG_INPT   yLOG_exit    (__FUNCTION__);
            return rc;
         }
      }
      /*---(next)---------------------*/
      ++x_col;
      b = p + 3;
      DEBUG_INPT  yLOG_info    ("b"         , b);
      p = strstr (b, q);
      DEBUG_INPT  yLOG_point   ("p"         , p);
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
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
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(process cells)---------------*/
   ystrlcpy (x_work, s_recd, LEN_RECD);
   p = strtok (x_work, "");
   DEBUG_INPT   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT   yLOG_info    ("p"         , p);
   --rce;  if (strncmp ("cell ", p, 5) != 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(label)--------------------------*/
   p = strtok (NULL, "");
   DEBUG_INPT   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ystrltrim (p, ySTR_BOTH, LEN_LABEL);
   rc = EXIM__import_dest (p);
   DEBUG_INPT   yLOG_value   ("dest"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(format)-------------------------*/
   p = strtok (NULL, "");
   DEBUG_INPT   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ystrltrim (p, ySTR_BOTH, LEN_LABEL);
   ystrlcpy (x_format, p, LEN_LABEL);
   DEBUG_INPT   yLOG_info    ("format"    , x_format);
   /*---(source)-------------------------*/
   p = strtok (NULL, "");
   DEBUG_INPT   yLOG_point   ("p"         , p);
   --rce;  if (p == NULL) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = EXIM__import_content (p);
   DEBUG_INPT   yLOG_value   ("content"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update)-------------------------*/
   rc = EXIM__import_one (s_dtab, s_dcol, s_drow, x_format, s_source);
   if (rc < 0) {
      EXIM__import_fail ();
      DEBUG_INPT   yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*> if (yMAP_mundo_current () >= s_mundos)  s_hist = YMAP_ADD;                     <* 
    *> x_new = CELL_overwrite (s_hist, s_dtab, s_dcol, s_drow, s_source, x_format);   <* 
    *> DEBUG_INPT  yLOG_point   ("x_new"     , x_new);                                <* 
    *> --rce;  if (x_new == NULL)  {                                                  <* 
    *>    DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                               <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <* 
    *> s_hist = YMAP_ADD;                                                             <*/
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> read an import record --------------[ leaf   [fe.A34.026.50]*/ /*-[01.0001.014.!]-*/ /*-[--.---.---.--]-*/
EXIM__import_read       (void)
{
   /*---(locals)-----+-----+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   char        x_del       =  '·';
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(read and clean)-----------------*/
   ++s_lines;
   DEBUG_INPT   yLOG_value   ("line"      , s_lines);
   fgets (s_recd, LEN_RECD, g_clip);
   --rce;  if (feof (g_clip))  {
      DEBUG_INPT   yLOG_note    ("end of file reached");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (s_recd);
   if (x_len > 0)  s_recd [--x_len] = '\0';
   DEBUG_INPT   yLOG_value   ("length"    , x_len);
   --rce;  if (x_len <= 0)  {
      DEBUG_INPT   yLOG_note    ("record is empty");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   DEBUG_INPT   yLOG_info    ("fixed"     , s_recd);
   --rce;  if (s_recd [0] == '\0') {
      DEBUG_INPT   yLOG_note    ("starts with a null");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   --rce;  if (s_recd [0] == '#') {
      if (strncmp (s_recd, "#@ ", 3) == 0) {
         DEBUG_INPT   yLOG_note    ("starts with a hint marker (#@)");
         DEBUG_INPT   yLOG_char    ("s_recd [3]", s_recd [3]);
         switch (s_recd [3]) {
         case 's' : /* EXIM__import_bounds (); */   break;
         case 'b' :   EXIM__import_bounds ();       break;
         case 'x' :   EXIM__import_sizer  ();       break;
         case 't' : /* EXIM__import_titles (); */   break;
         }
      }
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, -rce);
      return -rce;
   }
   DEBUG_INPT   yLOG_complex ("ready"     , "å%sæ", s_recd);
   ystrldchg (s_recd, '·', ' ',  LEN_RECD);
   DEBUG_INPT   yLOG_complex ("strldchg"  , "å%sæ", s_recd);
   x_len = strlen (s_recd);
   DEBUG_INPT   yLOG_value   ("x_len"     , x_len);
   if (s_recd [x_len - 1] == ' ')  s_recd [x_len--] = '\0';
   x_del = s_recd [x_len - 1];
   DEBUG_INPT   yLOG_char    ("x_del"     , x_del);
   switch (x_del) {
   case '' : case '§' :
      if (strncmp (s_recd, "cell          ", 15) == 0) {
         DEBUG_INPT   yLOG_note    ("native format detected");
         s_style = 'n';
      } else {
         DEBUG_INPT   yLOG_note    ("field/column delimited detected");
         s_style = 'f';
         s_recd [x_len - 1] = '\0';
      }
      break;
   case ','  :
      DEBUG_INPT   yLOG_note    ("comma separated values detected");
      s_style = 'c';
      s_recd [x_len - 1] = '\0';
      break;
   case '\t': case 'Ú' :
      DEBUG_INPT   yLOG_note    ("tab delimited detected");
      s_style = 't';
      s_recd [x_len - 1] = '\0';
      break;
   default   :
      DEBUG_INPT   yLOG_note    ("values/printables only detected");
      s_style = 'v';
      break;
   }
   DEBUG_INPT   yLOG_char    ("s_style"   , s_style);
   s_max = x_len;
   DEBUG_INPT   yLOG_value   ("s_max"     , s_max);
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__update_mundos     (void)
{
   s_mundos    = yMAP_mundo_current ();
   if (s_mundos < 0)  s_mundos = 0;
   if (s_mundos > 0)  ++s_mundos;
   DEBUG_INPT   yLOG_value   ("s_mundos"  , s_mundos);
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
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   DEBUG_INPT   yLOG_char    ("a_style"   , a_style);
   rc = EXIM__open ('r');
   DEBUG_INPT   yLOG_value   ("prep"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_hist      = YMAP_BEG;
   ystrlcpy (s_xsizer, "", LEN_RECD);
   s_nsizer = 0;
   EXIM__update_mundos ();
   /*---(process lines)------------------*/
   while (1) {
      /*---(read)------------------------*/
      rc = EXIM__import_read ();
      if (rc >  0)  continue;
      if (rc <  0)  break;
      /*---(handle values)---------------*/
      if (a_style != '+')  s_style = a_style;
      switch (s_style) {
      case 'v' : case 'V' :   rc = EXIM__import_values  (x_row);      break;
      case 'f' : case 'F' :   rc = EXIM__import_fields  (x_row);      break;
      case 'd' : case 'D' :   rc = EXIM__import_fields  (x_row);      break;
      case 't' : case 'T' :   rc = EXIM__import_fields  (x_row);      break;
      case 'c' : case 'C' :   rc = EXIM__import_csv     (x_row);      break;
      case 'n' : case 'N' :   rc = EXIM__import_native  ();           break;
      }
      --rce;  if (rc < 0) {
         DEBUG_INPT   yLOG_note    ("IMPORT FAILED");
         rc = EXIM__close  ();
         DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      /*---(next)------------------------*/
      ++x_row;
   }
   /*---(close file)---------------------*/
   rc = EXIM__close  ();
   DEBUG_INPT   yLOG_value   ("done"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM__header            (FILE *f, char a_style)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_style     =  '·';
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   s_style = '·';
   x_style = a_style;
   if (a_style == 0 || strchr ("-·", a_style) != NULL)  x_style = 'V';
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_char    ("x_style"   , x_style);
   --rce;  if (strchr (EXIM_VALID, x_style) == NULL) {
      DEBUG_OUTP   yLOG_note    ("style not valid");
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_OUTP   yLOG_point   ("f"         , f);
   --rce;  if (f == NULL) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save back)----------------------*/
   s_style = x_style;
   /*---(silent styles)------------------*/
   if (x_style == 0 || strchr (EXIM_UPPER, x_style) == NULL) {
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(handle noisy styles)------------*/
   fprintf (f, "#!%s\n", P_FULLPATH);
   fprintf (f, "##   generated by %s\n", P_ONELINE);
   fprintf (f, "#@ style     ");
   --rce;  switch (x_style) {
   case 'V' :  fprintf (f, "V = printable columnar values");  break;
   case 'D' :  fprintf (f, "D = column delimited");           break;
   case 'F' :  fprintf (f, "F = field delimited");            break;
   case 'T' :  fprintf (f, "T = tab delimited");              break;
   case 'C' :  fprintf (f, "C = comma separated values");     break;
   case 'R' :  fprintf (f, "R = results/raw values");         break;
   case 'S' :  fprintf (f, "S = source/contents");            break;
   case 'N' :  fprintf (f, "N = native format");              break;
   default  :
               DEBUG_OUTP   yLOG_note    ("no header found for style");
               s_style = '·';
               DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
               return rce;
   }
   fprintf (f, "\n");
   /*---(sizer)--------------------------*/
   rc = EXIM__export_sizer ();
   DEBUG_OUTP   yLOG_value   ("sizer"     , rc);
   --rce;  if (rc < 0) {
      s_style = '·';
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
EXIM__write_one         (FILE *f, char a_style, char a_tab, short a_col, short a_row)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   int         w           =    0;
   char        x_source    [LEN_RECD]  = "";
   char        x_modded    [LEN_RECD]  = "";
   char        x_print     [LEN_RECD]  = "";
   char        x_trim      [LEN_RECD]  = "";
   char        x_label     [LEN_LABEL] = "";
   char        t           [LEN_LABEL] = "";
   int         l           =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_OUTP   yLOG_point   ("f"         , f);
   --rce;  if (f == NULL) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get cell)-----------------------*/
   x_curr  = LOC_cell_at_loc (a_tab, a_col, a_row);
   DEBUG_OUTP   yLOG_complex ("x_curr"    , "%2dt, %3dc, %4dr, %p", a_tab, a_col, a_row, x_curr);
   /*---(fill in blank cells)------------*/
   if (x_curr == NULL) {
      DEBUG_OUTP   yLOG_note    ("NULL cell");
      w = COL_size (a_tab, a_col);
      switch (a_style) {
      case 'V' :             fprintf (f, "%*.*s", w, w, YSTR_EMPTY);       break;
      case 'v' :             fprintf (f, "%*.*s", w - 1, w - 1, YSTR_EMPTY);  break;
      case 'd' : case 'D' :  fprintf (f, "%*.*s··", w - 1, w - 1, YSTR_EMPTY);   break;
      case 'f' : case 'F' :  fprintf (f, "²··");                         break;
      case 'c' : case 'C' :  fprintf (f, "\"\",·");                        break;
      case 't' : case 'T' :  fprintf (f, "²·\t·");                         break;
      case 'r' : case 'R' :  fprintf (f, "²··");                         break;
      case 's' : case 'S' :  fprintf (f, "²··");                         break;
      case 'n' : case 'N' :  ystr4gyges (a_tab, a_col, a_row, 0, 0, t, YSTR_LEGAL);
                             ystrlpad (t, x_label, '.', '<', 11);
                             fprintf (f, "cell··········%-10.10s··??0--··\n"  , x_label);
                             break;
      }
      DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(write filled cells)-------------*/
   DEBUG_OUTP   yLOG_info    ("cell"      , x_curr->d_label);
   /*---(get source)---------------------*/
   if (x_curr->d_source != NULL) {
      DEBUG_OUTP   yLOG_note    ("convert source string");
      ystrlcpy    (x_source, x_curr->d_source, LEN_RECD);
      ystrldecode (x_source, LEN_RECD);
   } else {
      DEBUG_OUTP   yLOG_note    ("source is NULL");
   }
   DEBUG_OUTP   yLOG_info    ("source"    , x_source);
   /*---(get modded)---------------------*/
   if (strchr (YCALC_GROUP_NUM, x_curr->d_type) != NULL) {
      sprintf (x_modded, "%lf", x_curr->d_num);
   } else {
      if (x_curr->d_str != NULL)   ystrlcpy (x_modded, x_curr->d_str , LEN_RECD);
      else                         ystrlcpy (x_modded, x_curr->d_source, LEN_RECD);
   }
   ystrldecode    (x_modded, LEN_RECD);
   /*---(get printable)------------------*/
   if (x_curr->d_print != NULL) {
      DEBUG_OUTP   yLOG_note    ("convert printable");
      ystrlcpy    (x_print, x_curr->d_print, LEN_RECD);
      ystrldecode (x_print, LEN_RECD);
      ystrlcpy    (x_trim , x_print  , LEN_RECD);
      ystrltrim   (x_trim , ySTR_BOTH, LEN_RECD);
   } else {
      DEBUG_OUTP   yLOG_note    ("printable is NULL");
   }
   /*---(output)-------------------------*/
   DEBUG_OUTP   yLOG_info    ("x_print"   , x_print);
   DEBUG_OUTP   yLOG_info    ("x_trim"    , x_trim);
   DEBUG_OUTP   yLOG_info    ("modified"  , x_modded);
   /*---(handle)-------------------------*/
   switch (a_style) {
   case 'v' :
      l = strlen (x_print);
      x_print [l - 1] = '\0';
      fprintf (f, "%s"        , x_print);
      break;
   case 'V' :
      fprintf (f, "%s"        , x_print);
      break;
   case 'd' : case 'D' :
      l = strlen (x_print);
      x_print [l - 1] = '\0';
      fprintf (f, "%s··"    , x_print);
      break;
   case 'f' : case 'F' :
      fprintf (f, "%s··"    , x_trim);
      break;
   case 'c' : case 'C' :
      fprintf (f, "\"%s\",·"  , x_trim);
      break;
   case 't' : case 'T' :
      fprintf (f, "%s·\t·"    , x_trim);
      break;
   case 'r' : case 'R' :
      fprintf (f, "%s··"    , x_modded);
      break;
   case 's' : case 'S' :
      fprintf (f, "%s··"    , x_source);
      break;
   case 'n' : case 'N' :
      ystrlpad (x_curr->d_label, x_label, '.', '<', 11);
      fprintf (f, "cell··········%-10.10s··"  , x_label);
      fprintf (f, "%c%c%c%c%c%c%c··"      , x_curr->d_align, x_curr->d_format, x_curr->d_decs, x_curr->d_unit, x_curr->d_fill, x_curr->d_zero);
      fprintf (f, "%s·\n"   , x_source);
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 1;
}

char         /*-> tbd --------------------------------[ ------ [ge.RE5.1E2.D3]*/ /*-[05.0000.013.!]-*/ /*-[--.---.---.--]-*/
EXIM_export             (char a_style)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          = 0;
   FILE       *f           = NULL;
   tCELL      *x_curr      = NULL;
   ushort      x_tab       = 0;
   ushort      x_col       = 0;
   ushort      x_row       = 0;
   ushort      x_rowsave   = 0;
   int         w           = 0;
   char        x_source    [LEN_RECD]  = "";
   char        x_modded    [LEN_RECD]  = "";
   char        x_print     [LEN_RECD]  = "";
   char        x_trim      [LEN_RECD]  = "";
   int         c           = 0;
   int         o           = 0;
   int         t           = 0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(open output file)---------------*/
   rc = EXIM__open ('w');
   DEBUG_OUTP   yLOG_value   ("open"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(header)-------------------------*/
   rc = EXIM__header (g_clip, a_style);
   DEBUG_OUTP   yLOG_value   ("header"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   rc      = yMAP_visu_first (&x_tab, &x_col, &x_row, NULL);
   x_rowsave = x_row;
   /*---(walk cells)---------------------*/
   while (rc >= 0) {
      ++c;
      /*---(look for line break)---------*/
      if (strchr ("Nn", s_style) == NULL && x_row != x_rowsave) {
         DEBUG_OUTP   yLOG_note    ("line break");
         fprintf (g_clip, "\n");
         ++t;
      } else if (strchr ("Nn", s_style) != NULL)  ++t;
      rc = EXIM__write_one  (g_clip, s_style, x_tab, x_col, x_row);
      if (rc == 1)  ++o;
      x_rowsave = x_row;
      rc      = yMAP_visu_next  (&x_tab, &x_col, &x_row, NULL);
   };
   /*---(close file)---------------------*/
   DEBUG_OUTP   yLOG_note    ("closing file");
   if (strchr ("Nn", s_style) == NULL) {
      fprintf (g_clip, "\n");
      ++t;
   }
   if (strchr (EXIM_UPPER, s_style) != NULL) {
      fprintf (g_clip, "## complete, finito, done.  %d cells, %d occupied, %d lines\n", c, o, t);
   }
   rc = EXIM__close ();
   DEBUG_OUTP   yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
EXIM_init               (void)
{
   /*> yCMD_add (YVIKEYS_M_FILE, "import"      , ""    , "c"    , EXIM_import  , "data import driver");   <* 
    *> yVIKEYS_menu_add ("µfma", "auto"      , ":import +¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmv", "values"    , ":import v¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmV", "values+"   , ":import V¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmd", "coldel"    , ":import d¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmD", "coldel+"   , ":import D¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmf", "field"     , ":import f¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmF", "field+"    , ":import F¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmt", "tab"       , ":import t¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmT", "tab+"      , ":import T¦");                                             <* 
    *> yVIKEYS_menu_add ("µfmn", "navive"    , ":import N¦");                                             <*/
   /*> yCMD_add (YVIKEYS_M_FILE, "export"      , ""    , "c"    , EXIM_export  , "data export driver");   <* 
    *> yVIKEYS_menu_add ("µfxa", "values"    , ":export v¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxV", "values+"   , ":export V¦");                                             <* 
    *> yVIKEYS_menu_add ("µfed", "coldel"    , ":export d¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxD", "coldel+"   , ":export D¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxf", "field"     , ":export f¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxF", "field+"    , ":export F¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxc", "csv"       , ":export c¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxC", "csv+"      , ":export C¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxt", "tab"       , ":export t¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxT", "tab+"      , ":export T¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxr", "results"   , ":export r¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxR", "results+"  , ":export R¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxs", "source"    , ":export S¦");                                             <* 
    *> yVIKEYS_menu_add ("µfxn", "navive"    , ":export N¦");                                             <*/
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
   ystrlcpy (a_entry, "", LEN_RECD);
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
         ystrlcpy (a_entry, x_recd, LEN_RECD);
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

char
FILE_recd_shove         (char *a_recd)
{
   ystrlcpy (s_recd, a_recd, LEN_RECD);
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
   if      (strcmp (a_question, "bounds"    )    == 0) {
      snprintf (unit_answer, LEN_FULL, "FILE bounds      : %4du  %4dx  %4dy", s_uorig, s_xorig, s_yorig);
   }
   else if (strcmp (a_question, "sizer"     )    == 0) {
      for (i = 0; i < s_npos; ++i) {
         sprintf (t, "%5d%c"   , s_wide [i], s_xsep [i]);
         ystrlcat (s, t, LEN_RECD);
      }
      snprintf (unit_answer, LEN_FULL, "FILE sizer  (%2d) :%s", s_npos, s);
   }
   else if (strcmp (a_question, "exact"     )    == 0) {
      for (i = 0; i < s_npos; ++i) {
         sprintf (t, "%5d·"   , s_beg [i]);
         ystrlcat (s, t, LEN_RECD);
      }
      snprintf (unit_answer, LEN_FULL, "FILE exact  (%2d) :%s", s_npos, s);
   }
   else if (strcmp (a_question, "exim"      )    == 0) {
      rc = FILE__unit_exim_entry (a_ref, t);
      if (rc < 0) {
         snprintf (unit_answer, LEN_FULL, "FILE exim   (%2d) : none", a_ref);
      } else {
         ystrlencode (t, ySTR_MAX, LEN_RECD);
         snprintf (unit_answer, LEN_FULL, "FILE exim   (%2d) : %2d[%-.40s]", a_ref, strlen (t), t);
      }
   }
   else if (strcmp (a_question, "map"       )    == 0) {
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
