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
FILE_init               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(handlers)-----------------------*/
   /*> rc = yPARSE_handler (FILE_TABS    , "tab"       , 4.1, "NLLsssc-----", TABS_writer_all , TABS_reader     , "------------" , "name,min,max,x_def,y_def,z_def,type"  , "gyges tabs (v-axis)"      );   <*/


   /*> { 'e', FILE_DEPCEL , "dependent cells"    , "cell_dep"  , 'D', "TiaTO-----", "lvl/reg"   , "seq"       , "label"     , "t-f-d-a-m" , "contents"  , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_FREECEL, "independent cells"  , "cell_free" , 'D', "TiaTO-----", "lvl/reg"   , "seq"       , "label"     , "t-f-d-a-m" , "contents"  , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_TABS   , "tab (v-axis)"       , "tab"       , 'I', "Naaiiic---", "name"      , "min"       , "max"       , "x_size"    , "y_size"    , "z_size"    , "type"      , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_COLS   , "columns (x-axis)"   , "width"     , 'D', "aii-------", "label"     , "size"      , "count"     , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <* 
    *> { 'e', FILE_ROWS   , "rows (y-axis)"      , "height"    , 'D', "aii-------", "label"     , "size"      , "count"     , ""          , ""          , ""          , ""          , ""          , ""          , NULL  , NULL  ,   0,   0 },   <*/
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
static char    s_mapper   =  '-';
static short   s_map      [MAX_COLS];

static short   s_icol     =    0;
static short   s_mcol     =    0;
static short   s_ccol     =    0;
static short   s_crow     =    0;

/*> char         /+-> prepare for data import ------------[ leaf   [fe.F83.123.30]+/ /+-[01.0000.014.X]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_prep       (char a_style)                                                                                            <* 
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
 *>    s_clip = fopen (FILE_EXIM, "r");                                                                           <* 
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
 *> REG__inpt_map        (void)                                                                                                    <* 
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

/*> char         /+-> read an import record --------------[ leaf   [fe.A34.026.50]+/ /+-[01.0001.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_read       (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(read and clean)-----------------+/                                                                                    <* 
 *>    ++s_lines;                                                                                                                  <* 
 *>    DEBUG_REGS   yLOG_value   ("line"      , s_lines);                                                                          <* 
 *>    fgets (s_recd, LEN_RECD, s_clip);                                                                                           <* 
 *>    if (feof (s_clip))  {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_note    ("end of file reached");                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    x_len = strlen (s_recd);                                                                                                    <* 
 *>    if (x_len <= 0)  {                                                                                                          <* 
 *>       DEBUG_REGS   yLOG_note    ("record is empty");                                                                           <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    s_recd [--x_len] = '\0';                                                                                                    <* 
 *>    DEBUG_REGS   yLOG_value   ("length"    , x_len);                                                                            <* 
 *>    DEBUG_REGS   yLOG_info    ("fixed"     , s_recd);                                                                           <* 
 *>    if (s_recd [0] == '\0') {                                                                                                   <* 
 *>       DEBUG_REGS   yLOG_note    ("starts with a null");                                                                        <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    if (s_recd [0] == ' ' ) {                                                                                                   <* 
 *>       DEBUG_REGS   yLOG_note    ("starts with a space");                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    if (s_recd [0] == '#' && strchr ("<>=", s_recd [1]) == NULL) {                                                              <* 
 *>       DEBUG_REGS   yLOG_note    ("starts with a comment marker");                                                              <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, -rce);                                                                          <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    s_max = x_len;                                                                                                              <* 
 *>    DEBUG_REGS   yLOG_value   ("s_max"     , s_max);                                                                            <* 
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
 *>    w = COL_width (CTAB, CCOL + a_col);                                                                                     <* 
 *>    --rce;  if (w >= a_wide + 1)     return rce;                                                                                <* 
 *>    LOC_col_widen (CTAB, CCOL + a_col, a_wide + 1);                                                                             <* 
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
 *>       LOC_col_widen (CTAB, CCOL + x_col, x_len + 1);                                                                           <* 
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

/*> char         /+-> place import data into a cell ------[ ------ [fz.310.301.21]+/ /+-[01.0000.025.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_place      (short a_col, short a_row, char *a_value)                                                                 <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
 *>    /+---(non-mapped)---------------------+/                                                                                    <* 
 *>    if (s_mapper == '-') {                                                                                                      <* 
 *>       CELL_change (NULL, CHG_INPUT, CTAB, CCOL + a_col, CROW + a_row, a_value);                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(mapped)-------------------------+/                                                                                    <* 
 *>    else {                                                                                                                      <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> complete data import ---------------[ ------ [fz.311.001.02]+/ /+-[00.0000.014.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_done       (void)                                                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(close file)---------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_note    ("closing file");                                                                                 <* 
 *>    fclose  (s_clip);                                                                                                           <* 
 *>    /+---(screen)-------------------------+/                                                                                    <* 
 *>    /+> CURS_screen_reset ();                                                          <+/                                      <* 
 *>    /+---(calculate)----------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_note    ("recalc");                                                                                        <* 
 *>    SEQ_calc_full ();                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> tbd --------------------------------[ ------ [fz.732.141.12]+/ /+-[01.0000.014.3]-+/ /+-[--.---.---.--]-+/   <* 
 *> REG__inpt_values     (short a_row)                                                                                             <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----+-----+-----+-----+-+/                                                                                    <* 
 *>    short       x_col       =    0;                                                                                             <* 
 *>    int         w           =    0;                                                                                             <* 
 *>    int         cw          =    0;                                                                                             <* 
 *>    char        t           [LEN_RECD]  = "";                                                                                   <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_enter   (__FUNCTION__);                                                                                   <* 
 *>    /+---(process cells)---------------+/                                                                                       <* 
 *>    while (cw < s_max) {                                                                                                        <* 
 *>       /+---(read import data)---------+/                                                                                       <* 
 *>       w     = COL_width (CTAB, CCOL + x_col);                                                                              <* 
 *>       DEBUG_REGS  yLOG_value   ("w"         , w);                                                                              <* 
 *>       strlcpy (t, s_recd + cw, w);                                                                                             <* 
 *>       DEBUG_REGS  yLOG_info    ("t (orig)"  , t);                                                                              <* 
 *>       /+---(process)------------------+/                                                                                       <* 
 *>       strltrim (t, ySTR_BOTH, LEN_RECD);                                                                                       <* 
 *>       DEBUG_REGS  yLOG_info    ("t (trim)"  , t);                                                                              <* 
 *>       strlencode   (t, ySTR_NORM, LEN_RECD);                                                                                   <* 
 *>       DEBUG_REGS  yLOG_info    ("t (new)"   , t);                                                                              <* 
 *>       DEBUG_REGS  yLOG_value   ("x_col"     , x_col);                                                                          <* 
 *>       /+---(change)-------------------+/                                                                                       <* 
 *>       REG__inpt_place  (x_col, a_row, t);                                                                                      <* 
 *>       /+---(next)---------------------+/                                                                                       <* 
 *>       ++x_col;                                                                                                                 <* 
 *>       cw += w;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

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
 *>    rc = REG__inpt_prep   (a_style);                                                                                            <* 
 *>    DEBUG_REGS   yLOG_value   ("prep rc"   , rc);                                                                               <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_REGS   yLOG_exitr   (__FUNCTION__, rce);                                                                           <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    rc = REG__inpt_map ();                                                                                                      <* 
 *>    /+---(process lines)------------------+/                                                                                    <* 
 *>    while (1) {                                                                                                                 <* 
 *>       /+---(read)------------------------+/                                                                                    <* 
 *>       rc = REG__inpt_read ();                                                                                                  <* 
 *>       if (rc >  0)  continue;                                                                                                  <* 
 *>       if (rc <  0)  break;                                                                                                     <* 
 *>       /+---(process cells)---------------+/                                                                                    <* 
 *>       DEBUG_REGS  yLOG_value   ("x_row"     , x_row);                                                                          <* 
 *>       /+---(check sizer)-----------------+/                                                                                    <* 
 *>       if (strncmp (s_recd, "#<34"  , 4) == 0) {                                                                                <* 
 *>          REG__inpt_sizer ();                                                                                                   <* 
 *>          continue;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       /+---(handle values)---------------+/                                                                                    <* 
 *>       if (strchr ("Vv", s_style) != NULL) {                                                                                    <* 
 *>          REG__inpt_values  (x_row);                                                                                            <* 
 *>       } else {                                                                                                                 <* 
 *>          REG__inpt_delims  (x_row);                                                                                            <* 
 *>       }                                                                                                                        <* 
 *>       /+---(prepare for next)------------+/                                                                                    <* 
 *>       ++x_row;                                                                                                                 <* 
 *>    }                                                                                                                           <* 
 *>    /+---(wrap up)------------------------+/                                                                                    <* 
 *>    rc = REG__inpt_done ();                                                                                                     <* 
 *>    DEBUG_REGS   yLOG_value   ("done rc"   , rc);                                                                               <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    DEBUG_REGS   yLOG_exit    (__FUNCTION__);                                                                                   <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> tbd --------------------------------[ ------ [ge.RE5.1E2.D3]*/ /*-[05.0000.013.!]-*/ /*-[--.---.---.--]-*/
EXIM_export             (char a_style)
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
   char        x_source    [LEN_RECD]  = "";
   char        x_modded    [LEN_RECD]  = "";
   char        x_print     [LEN_RECD]  = "";
   char        x_trim      [LEN_RECD]  = "";
   int         c           = 0;
   /*---(header)-------------------------*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   /*---(open output file)---------------*/
   f = fopen(FILE_EXIM, "w");
   DEBUG_REGS   yLOG_point   ("f (file)"  , f);
   --rce;  if (f == NULL) {
      DEBUG_REGS   yLOG_note    ("can not open clip file");
      DEBUG_REGS   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*> fprintf (f, "#!/usr/local/bin/gyges\n");                                                <* 
    *> fprintf (f, "##   generated by gyges-hekatonkheires (hundred-handed) spreadsheet\n");   <* 
    *> fprintf (f, "#[-----][-----------][------]\n");                                         <*/
   /*---(process independent cells)------*/
   rc    = yVIKEYS_first (&x_tab, &x_col, &x_row, NULL);
   curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
   x_rowsave = x_row;
   while (rc >= 0) {
      DEBUG_REGS   yLOG_point   ("curr"      , curr);
      ++c;
      /*---(look for line break)---------*/
      if (strchr ("vdfct", a_style) != 0 && x_row != x_rowsave) {
         DEBUG_REGS   yLOG_note    ("line break");
         fprintf (f, "\n");
      }
      /*---(fill in blank cells)---------*/
      if (curr == NULL) {
         DEBUG_REGS   yLOG_note    ("NULL cell");
         w = COL_width (x_tab, x_col);
         switch (a_style) {
         case 'v' :
            fprintf (f, "%*.*s", w, w, g_empty);
            break;
         case 'd' :
            fprintf (f, "%*.*s", w, w, g_empty);
            break;
         case 'f' :
            fprintf (f, "");
            break;
         case 'c' :
            fprintf (f, "\"\",");
            break;
         case 't' :
            fprintf (f, "\t");
            break;
         }
      }
      /*---(write filled cells)----------*/
      else {
         DEBUG_REGS   yLOG_info    ("cell"      , curr->label);
         /*---(source)-------------------*/
         if (curr->source != NULL) {
            DEBUG_REGS   yLOG_note    ("convert source string");
            strlcpy      (x_source, curr->source, LEN_RECD);
            strldecode   (x_source, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("source is NULL");
         }
         DEBUG_REGS   yLOG_info    ("source"    , x_source);
         /*---(modded string)------------*/
         if (curr->v_str != NULL) {
            DEBUG_REGS   yLOG_note    ("convert modified string");
            strlcpy      (x_modded, curr->v_str, LEN_RECD);
            strldecode   (x_modded, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("modified is NULL");
         }
         /*---(printable)----------------*/
         if (curr->print != NULL) {
            DEBUG_REGS   yLOG_note    ("convert printable");
            strlcpy  (x_print , curr->print, LEN_RECD);
            strldecode   (x_print, LEN_RECD);
            strlcpy  (x_trim  , x_print, LEN_RECD);
         } else {
            DEBUG_REGS   yLOG_note    ("printable is NULL");
         }
         /*---(trimmed printable)--------*/
         strltrim (x_trim, ySTR_BOTH, LEN_RECD);
         DEBUG_REGS   yLOG_info    ("printable" , x_trim);
         DEBUG_REGS   yLOG_info    ("modified"  , x_modded);
         DEBUG_REGS   yLOG_note    ("handle");
         switch (a_style) {
         case 'v' : fprintf (f, "%s"                  , x_print);
                    break;
         case 'd' : fprintf (f, "%s"                , x_print);
                    break;
         case 'f' : fprintf (f, "%s"                , x_trim);
                    break;
         case 'c' : fprintf (f, "\"%s\","             , x_trim);
                    break;
         case 't' : fprintf (f, "%s\t"                , x_trim);
                    break;
         case 'n' : fprintf (f, "cell  %-8.8s  "  , curr->label);
                    fprintf (f, "%c%c%c%c-  "       , curr->align, curr->format, curr->decs, curr->unit);
                    fprintf (f, "%s \n"             , x_source);
                    break;
         }
      }
      x_rowsave = x_row;
      rc      = yVIKEYS_next  (&x_tab, &x_col, &x_row, NULL);
      curr  = LOC_cell_at_loc (x_tab, x_col, x_row);
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
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

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
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   char        x_entry     [LEN_RECD];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "FILE             : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "exim"      )    == 0) {
      rc = FILE__unit_exim_entry (a_ref, x_entry);
      if (rc < 0) {
         snprintf (unit_answer, LEN_FULL, "FILE exim   (%2d) : none", a_ref);
      } else {
         strlencode (x_entry, ySTR_MAX, LEN_RECD);
         snprintf (unit_answer, LEN_FULL, "FILE exim   (%2d) : %2d[%-.40s]", a_ref, strlen (x_entry), x_entry);
      }
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
