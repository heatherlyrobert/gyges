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



#include   "gyges.h"



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


/*====================------------------------------------====================*/
/*===----                          versioning                          ----===*/
/*====================------------------------------------====================*/
PRIV void  o___VERSIONING______o () { return; }

/*> char FILE_bump_major    (void)  { return FILE_bump ("M"); }                                                                    <* 
 *> char FILE_bump_minor    (void)  { return FILE_bump ("m"); }                                                                    <* 
 *> char FILE_bump_inc      (void)  { return FILE_bump ("i"); }                                                                    <* 
 *>                                                                                                                                <* 
 *> char         /+-> tbd --------------------------------[ ------ [ge.C70.13#.E1]+/ /+-[02.0000.03#.T]-+/ /+-[--.---.---.--]-+/   <* 
 *> FILE_bump          (char *a_type)                                                                                              <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rc          = 0;                                                                                                <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    char        x_type      = ' ';                                                                                              <* 
 *>    /+---(defense : not controlled)-------+/                                                                                    <* 
 *>    --rce;  if (ver_ctrl != 'y')  return rce;                                                                                   <* 
 *>    /+---(defense: a_type)----------------+/                                                                                    <* 
 *>    --rce;  if (a_type == NULL)                   return rce;                                                                   <* 
 *>    --rce;  if (a_type [0] == '\0')               return rce;                                                                   <* 
 *>    x_type = a_type [0];                                                                                                        <* 
 *>    --rce;  if (strchr ("Mmi", x_type) == NULL)   return rce;                                                                   <* 
 *>    FILE_vertxt (NULL);                                                                                                         <* 
 *>    /+---(tiny)---------------------------+/                                                                                    <* 
 *>    if (strchr ("i", x_type) != NULL) {                                                                                         <* 
 *>       if (ver_num [3] <  'z') {                                                                                                <* 
 *>          ++ver_num[3];                                                                                                         <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    ver_num [3] = 'a';                                                                                                          <* 
 *>    /+---(minor)--------------------------+/                                                                                    <* 
 *>    if (strchr ("mi", x_type) != NULL) {                                                                                        <* 
 *>       if (ver_num [2] <  '9') {                                                                                                <* 
 *>          ++ver_num[2];                                                                                                         <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       if (ver_num [2] == '9') {                                                                                                <* 
 *>          ver_num  [2] =  'A';                                                                                                  <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       if (ver_num [2] <  'Z') {                                                                                                <* 
 *>          ++ver_num[2];                                                                                                         <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    ver_num [2] = '0';                                                                                                          <* 
 *>    /+---(major)--------------------------+/                                                                                    <* 
 *>    if (strchr ("Mmi", x_type) != NULL) {                                                                                       <* 
 *>       if (ver_num [0] <  '9') {                                                                                                <* 
 *>          ++ver_num[0];                                                                                                         <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       if (ver_num [0] == '9') {                                                                                                <* 
 *>          ver_num  [0] =  'A';                                                                                                  <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>       if (ver_num [0] <  'Z') {                                                                                                <* 
 *>          ++ver_num[0];                                                                                                         <* 
 *>          return 0;                                                                                                             <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    strcpy (ver_num, "Z.Zz");                                                                                                   <* 
 *>    --rce;  return  rce;                                                                                                        <* 
 *> }                                                                                                                              <* 
 *>                                                                                                                                <* 
 *> char FILE_control       (void)  { return FILE_controlled ("y"); }                                                              <* 
 *> char FILE_nocontrol     (void)  { return FILE_controlled ("n"); }                                                              <* 
 *>                                                                                                                                <* 
 *> char         /+-> tbd --------------------------------[ ------ [gc.520.103.41]+/ /+-[02.0000.02#.G]-+/ /+-[--.---.---.--]-+/   <* 
 *> FILE_controlled    (char *a_yes)                                                                                               <* 
 *> {                                                                                                                              <* 
 *>    FILE_vertxt (NULL);                                                                                                         <* 
 *>    if (a_yes [0] == 'n') {                                                                                                     <* 
*>       if (ver_ctrl == 'y') {                                                                                                   <* 
   *>          ver_ctrl = '-';                                                                                                       <* 
      *>          strcpy (ver_num, "----");                                                                                             <* 
      *>       }                                                                                                                        <* 
      *>       return 0;                                                                                                                <* 
      *>    }                                                                                                                           <* 
      *>    if (a_yes [0] == 'y') {                                                                                                     <* 
         *>       if (ver_ctrl == '-') {                                                                                                   <* 
            *>          ver_ctrl = 'y';                                                                                                       <* 
               *>          strcpy (ver_num, "0.0a");                                                                                             <* 
               *>       }                                                                                                                        <* 
               *>       return 0;                                                                                                                <* 
               *>    }                                                                                                                           <* 
               *>    return -1;                                                                                                                  <* 
               *> }                                                                                                                              <* 
               *>                                                                                                                                <* 
               *> char         /+-> tbd --------------------------------[ ------ [ge.880.13#.G1]+/ /+-[02.0000.00#.#]-+/ /+-[--.---.---.--]-+/   <* 
               *> FILE_version       (char *a_ver)                                                                                               <* 
               *> {                                                                                                                              <* 
                  *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
                     *>    int         x_len       = 0;                                                                                                <* 
                     *>    char        rce         = -10;                                                                                              <* 
                     *>    char        x_work      [10];                                                                                               <* 
                     *>    /+---(defense : not controlled)-------+/                                                                                    <* 
                     *>    --rce;  if (ver_ctrl != 'y')  return rce;                                                                                   <* 
                     *>    /+---(defense : empty)----------------+/                                                                                    <* 
                     *>    --rce;  if (a_ver == NULL)               return rce;                                                                        <* 
                     *>    x_len = strlen (a_ver);                                                                                                     <* 
                     *>    --rce;  if (strcmp (a_ver, "") == 0)     return rce;                                                                        <* 
                     *>    --rce;  if (x_len <= 0)                  return rce;                                                                        <* 
                     *>    /+---(defense: bad length)------------+/                                                                                    <* 
                     *>    --rce;  if (x_len < 4)                   return rce;                                                                        <* 
                     *>    --rce;  if (x_len > 4)                   return rce;                                                                        <* 
                     *>    /+---(prepare)------------------------+/                                                                                    <* 
                     *>    strcpy  (x_work, a_ver);                                                                                                    <* 
                     *>    /+---(test chars)---------------------+/                                                                                    <* 
                     *>    --rce;  if (strchr ("abcdefghijklmnopqrstuvwxyz",           x_work [3]) == 0)  return rce;                                  <* 
                     *>    --rce;  if (strchr ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", x_work [2]) == 0)  return rce;                                  <* 
                     *>    --rce;  if (x_work [1] != '.')                       return rce;                                                            <* 
                     *>    --rce;  if (strchr ("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ", x_work [0]) == 0)  return rce;                                  <* 
                     *>    /+---(check increase only)------------+/                                                                                    <* 
                     *>    --rce;  if (x_work [0] <  ver_num [0])    return rce;                                                                       <* 
                     *>    if (x_work [0] == ver_num [0]) {                                                                                            <* 
                        *>       --rce;  if (x_work [2] <  ver_num [2])    return rce;                                                                    <* 
                           *>       if (x_work [2] == ver_num [2]) {                                                                                         <* 
                              *>          --rce;  if (x_work [3] <  ver_num [3])    return rce;                                                                 <* 
                                 *>          --rce;  if (x_work [3] == ver_num [3])    return rce;                                                                 <* 
                                 *>       }                                                                                                                        <* 
                                 *>    }                                                                                                                           <* 
                                 *>    /+---(finalize)-----------------------+/                                                                                    <* 
                                 *>    strcpy (ver_num, x_work);                                                                                                   <* 
                                 *>    FILE_vertxt (NULL);                                                                                                         <* 
                                 *>    /+---(complete)-----------------------+/                                                                                    <* 
                                 *>    return 0;                                                                                                                   <* 
                                 *> }                                                                                                                              <* 
                                 *>                                                                                                                                <* 
                                 *> char         /+-> tbd --------------------------------[ leaf   [ge.330.114.30]+/ /+-[00.0000.03#.7]-+/ /+-[--.---.---.--]-+/   <* 
                                 *> FILE_vertxt        (char *a_txt)                                                                                               <* 
                                 *> {                                                                                                                              <* 
                                    *>    char        rce         =  -10;                                                                                             <* 
                                       *>    strlcpy (ver_txt, "-----", LEN_DESC);                                                                                       <* 
                                       *>    --rce;  if (ver_ctrl   != 'y' )  return rce;                                                                                <* 
                                       *>    --rce;  if (a_txt      == NULL)  return rce;                                                                                <* 
                                       *>    --rce;  if (a_txt [0]  == '\0')  return rce;                                                                                <* 
                                       *>    strlcpy (ver_txt, a_txt, LEN_DESC);                                                                                         <* 
                                       *>    return 0;                                                                                                                   <* 
                                       *> }                                                                                                                              <* 
                                       *>                                                                                                                                <* 
                                       *> char         /+-> tbd --------------------------------[ leaf   [gc.C55.124.30]+/ /+-[01.0000.112.!]-+/ /+-[--.---.---.--]-+/   <* 
                                       *> FILE_rename          (char *a_name)                                                                                            <* 
                                       *> {                                                                                                                              <* 
                                          *>    /+---(locals)-----------+-----+-----+-+/                                                                                    <* 
                                             *>    char        t           [LEN_STR]   = "";                                                                                   <* 
                                             *>    char       *p           = NULL;                                                                                             <* 
                                             *>    /+---(header)-------------------------+/                                                                                    <* 
                                             *>    DEBUG_INPT   yLOG_enter   (__FUNCTION__);                                                                                   <* 
                                             *>    /+---(defense)------------------------+/                                                                                    <* 
                                             *>    DEBUG_INPT   yLOG_point   ("a_name"    , a_name);                                                                           <* 
                                             *>    if (a_name == NULL) {                                                                                                       <* 
                                                *>       DEBUG_INPT   yLOG_note    ("a_name was null, using defaults");                                                           <* 
                                                   *>       strlcpy (my.f_loc  , ""        , LEN_RECD);                                                                              <* 
                                                   *>       strlcpy (my.f_name , FILE_BLANK, LEN_RECD);                                                                              <* 
                                                   *>       sprintf (my.f_title, "%s.%s"   , my.f_name, FILE_SUFFIX);                                                                <* 
                                                   *>       DEBUG_INPT   yLOG_info    ("my.f_name" , my.f_name);                                                                     <* 
                                                   *>       DEBUG_INPT   yLOG_info    ("my.f_title", my.f_title);                                                                    <* 
                                                   *>       DEBUG_INPT   yLOG_exit    (__FUNCTION__);                                                                                <* 
                                                   *>       return 0;                                                                                                                <* 
                                                   *>    }                                                                                                                           <* 
                                                   *>    DEBUG_INPT   yLOG_info    ("a_name"    , a_name);                                                                           <* 
                                                   *>    if (a_name [0] == '\0') {                                                                                                   <* 
                                                      *>       DEBUG_INPT   yLOG_note    ("a_name was blank, using defaults");                                                          <* 
                                                         *>       strlcpy (my.f_loc  , ""        , LEN_RECD);                                                                              <* 
                                                         *>       strlcpy (my.f_name , FILE_BLANK, LEN_RECD);                                                                              <* 
                                                         *>       sprintf (my.f_title, "%s.%s"   , my.f_name, FILE_SUFFIX);                                                                <* 
                                                         *>       DEBUG_INPT   yLOG_info    ("my.f_name" , my.f_name);                                                                     <* 
                                                         *>       DEBUG_INPT   yLOG_info    ("my.f_title", my.f_title);                                                                    <* 
                                                         *>       DEBUG_INPT   yLOG_exit    (__FUNCTION__);                                                                                <* 
                                                         *>       return 0;                                                                                                                <* 
                                                         *>    }                                                                                                                           <* 
                                                         *>    /+---(parse base name)----------------+/                                                                                    <* 
                                                         *>    strlcpy (t, a_name, LEN_STR);                                                                                               <* 
                                                         *>    p = strrchr (t, "/");                                                                                                       <* 
                                                         *>    DEBUG_INPT   yLOG_point   ("p"         , p);                                                                                <* 
                                                         *>    if (p == NULL) {                                                                                                            <* 
                                                            *>       DEBUG_INPT   yLOG_note    ("name only, no directory");                                                                   <* 
                                                               *>       strlcpy (my.f_loc  , ""        , LEN_RECD);                                                                              <* 
                                                               *>       strlcpy (my.f_name , a_name, LEN_RECD);                                                                                  <* 
                                                               *>       sprintf (my.f_title, "%s.%s", my.f_name, FILE_SUFFIX);                                                                   <* 
                                                               *>       DEBUG_INPT   yLOG_info    ("my.f_name" , my.f_name);                                                                     <* 
                                                               *>       DEBUG_INPT   yLOG_info    ("my.f_title", my.f_title);                                                                    <* 
                                                               *>       DEBUG_INPT   yLOG_exit    (__FUNCTION__);                                                                                <* 
                                                               *>       return 0;                                                                                                                <* 
                                                               *>    }                                                                                                                           <* 
                                                               *>    /+---(parse qualified name)-----------+/                                                                                    <* 
                                                               *>    DEBUG_INPT   yLOG_note    ("fully qualified name, with directory");                                                         <* 
                                                               *>    p = '\0';                                                                                                                   <* 
                                                               *>    strlcpy (my.f_loc  , t     , LEN_RECD);                                                                                     <* 
                                                               *>    strlcpy (my.f_name , p + 1 , LEN_RECD);                                                                                     <* 
                                                               *>    sprintf (my.f_title, "%s/%s.%s", my.f_loc, my.f_name, FILE_SUFFIX);                                                         <* 
                                                               *>    DEBUG_INPT   yLOG_info    ("my.f_loc"  , my.f_loc);                                                                         <* 
                                                               *>    DEBUG_INPT   yLOG_info    ("my.f_name" , my.f_name);                                                                        <* 
                                                               *>    DEBUG_INPT   yLOG_info    ("my.f_title", my.f_title);                                                                       <* 
                                                               *>    /+---(complete)-----------------------+/                                                                                    <* 
                                                               *>    DEBUG_INPT   yLOG_exit    (__FUNCTION__);                                                                                   <* 
                                                               *>    return 0;                                                                                                                   <* 
                                                               *> }                                                                                                                              <*/



                                                               /*====================------------------------------------====================*/
                                                               /*===----                       history recording                      ----===*/
                                                               /*====================------------------------------------====================*/
                                                               PRIV void  o___HISTORY_________o () { return; }

char         /*-> clear out the history --------------[ leaf   [gz.530.011.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_clear         (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(clear list)------------------*/
   for (i = 0; i < MAX_HIST; ++i) {
      strcpy (hist [i].act   , "");
      hist [i].btab  = -1;
      hist [i].bcol  = -1;
      hist [i].brow  = -1;
      strcpy (hist [i].before, "");
      strcpy (hist [i].after , "");
   }
   /*---(clear vars)------------------*/
   nhist =  0;
   chist = -1;
   /*---(complete)--------------------*/
   return 0;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.204.!]-*/ /*-[--.---.---.--]-*/
HIST_change        (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of formatting                           */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      char       *a_before,   /* contents before change (for undo)            */
      char       *a_after)    /* contents after change  (for redo)            */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   strcpy (hist [chist].before, a_before);
   strcpy (hist [chist].after , a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*-> record a cell change ---------------[ leaf   [gz.620.101.00]*/ /*-[01.0000.105.!]-*/ /*-[--.---.---.--]-*/
HIST_overwrite     (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of formatting                           */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      char       *a_before,   /* contents before change (for undo)            */
      char       *a_beforeF,  /* contents before change (for undo)            */
      char       *a_after,    /* contents after change  (for redo)            */
      char       *a_afterF)   /* contents after change  (for redo)            */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   sprintf (hist [chist].before, "%s::%s", a_beforeF, a_before);
   sprintf (hist [chist].after , "%s::%s", a_afterF , a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.304.!]-*/ /*-[--.---.---.--]-*/
HIST_format        (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of formatting                           */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      char        a_before,   /* format before                                */
      char        a_after)    /* format after                                 */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   sprintf (hist [chist].before, "%c", a_before);
   sprintf (hist [chist].after , "%c", a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*-> record a cell change ---------------[ leaf   [gz.520.101.00]*/ /*-[01.0000.00#.!]-*/ /*-[--.---.---.--]-*/
HIST_size          (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_type,     /* type of sizing                               */
      int         a_tab,      /* tab of change                                */
      int         a_col,      /* col of change                                */
      int         a_row,      /* row of change                                */
      int         a_before,   /* size before                                  */
      int         a_after)    /* size after                                   */
{
   /*---(update pointers)-------------*/
   ++chist;
   nhist = chist + 1;
   /*---(add record)------------------*/
   strncpy (hist [chist].act   , a_type, 14);
   hist [chist].btab  = a_tab;
   hist [chist].bcol  = a_col;
   hist [chist].brow  = a_row;
   sprintf (hist [chist].before, "%d", a_before);
   sprintf (hist [chist].after , "%d", a_after );
   /*---(complete)--------------------*/
   return 0;
}

char         /*-> take a change away -----------------[ leaf   [gc.J76.043.IA]*/ /*-[03.0000.113.!]-*/ /*-[--.---.---.--]-*/
HIST_undo          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_lower     [15];                 /* action in lower case      */
   char        x_upper     [15];                 /* action in upper case      */
   int         i           = 0;                  /* generic iterator          */
   char        x_temp      [15];
   /*---(header)-------------------------*/
   DEBUG_HIST  yLOG_enter   (__FUNCTION__);
   DEBUG_HIST  yLOG_value   ("nhist"     , nhist);
   DEBUG_HIST  yLOG_value   ("chist"     , chist);
   /*---(defense)------------------------*/
   if (chist <  0    ) {
      DEBUG_HIST  yLOG_note    ("chist less than zero");
      DEBUG_HIST  yLOG_exit    (__FUNCTION__);
      return -1;
   }
   if (chist >= nhist) {
      DEBUG_HIST  yLOG_note    ("chist greater than or equal to nhist");
      DEBUG_HIST  yLOG_exit    (__FUNCTION__);
      return -2;
   }
   /*---(prepare)------------------------*/
   DEBUG_HIST  yLOG_info    ("action"    , hist[chist].act);
   strcpy (x_lower, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_lower[i] = tolower (x_lower[i]);
   DEBUG_HIST  yLOG_info    ("lower"     , x_lower);
   strcpy (x_upper, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_upper[i] = toupper (x_upper[i]);
   DEBUG_HIST  yLOG_info    ("upper"     , x_upper);
   /*---(get to right location)----------*/
   DEBUG_HIST  yLOG_note    ("clear any existing selection");
   /*> VISU_clear ();                                                                 <*/
   DEBUG_HIST  yLOG_complex ("jump to"   , "t=%4d, c=%4d, r=%4d", hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   LOC_jump  (hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   /*---(handle request)-----------------*/
   DEBUG_HIST  yLOG_info    ("before"    , hist[chist].before);
   if        (strcmp ("change", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_change");
      if (strcmp (hist[chist].before, "[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         CELL_change  (NULL, CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before);
      }
   } else if (strcmp ("overwrite", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_overwrite");
      if (strcmp (hist[chist].before, "???::[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         sprintf (x_temp, "%c%c%c", hist[chist].before[0], hist[chist].before[1], hist[chist].before[2]);
         CELL_overwrite (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before + 5, x_temp);
      }
   } else if (strcmp ("delete", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("delete/erase");
      CELL_change   (NULL, CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].before);
   } else if (strcmp ("decimals", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("decimals");
      CELL_visual   (CHANGE_DECIMAL, CHG_NOHIST, hist[chist].before[0]);
   } else if (strcmp ("align"   , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("align");
      CELL_visual   (CHANGE_ALIGN  , CHG_NOHIST, hist[chist].before[0]);
   } else if (strcmp ("format"  , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("format");
      CELL_visual   (CHANGE_FORMAT , CHG_NOHIST, hist[chist].before[0]);
   } else if (strcmp ("width"   , x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("width");
      CELL_visual   (CHANGE_WIDTH  , CHG_NOHIST, -(atoi (hist[chist].before)));
   }
   /*---(uptate)-------------------------*/
   --chist;
   DEBUG_HIST  yLOG_value   ("chist"     , chist);
   /*---(tail recursion)-----------------*/
   if (chist >= 0) {
      DEBUG_HIST  yLOG_info    ("action"    , hist[chist + 1].act);
      if (strcmp (hist[chist + 1].act, x_upper) == 0) {
         DEBUG_HIST  yLOG_note    ("check previous action");
         if      (strcmp (hist[chist].act, x_lower) == 0) {
            DEBUG_HIST  yLOG_note    ("final recursive call");
            HIST_undo ();
         }
         else if (strcmp (hist[chist].act, x_upper) == 0) {
            DEBUG_HIST  yLOG_note    ("another in series");
            HIST_undo ();
         }
         else {
            DEBUG_HIST  yLOG_note    ("does not match, don't recurse");
         }
      }
   }
   /*---(complet)------------------------*/
   DEBUG_HIST  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> put a change back ------------------[ leaf   [gc.B61.042.EA]*/ /*-[02.0000.113.!]-*/ /*-[--.---.---.--]-*/
HIST_redo          (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_lower     [15];                 /* action in lower case      */
   char        x_upper     [15];                 /* action in upper case      */
   int         i           = 0;                  /* generic iterator          */
   char        x_temp      [15];
   /*---(defense)------------------------*/
   if (chist >= nhist - 1) return -1;
   /*---(uptate)-------------------------*/
   ++chist;
   /*---(prepare)------------------------*/
   strcpy (x_lower, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_lower[i] = tolower (x_lower[i]);
   strcpy (x_upper, hist[chist].act);
   for (i = 0; i < 15; ++i)   x_upper[i] = toupper (x_upper[i]);
   /*---(get to right location)----------*/
   LOC_jump  (hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   /*> VISU_clear ();                                                                 <*/
   /*---(handle request)-----------------*/
   if        (strcmp ("change"  , x_lower) == 0) {
      if (strcmp (hist[chist].after , "[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         CELL_change (NULL, CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].after);
      }
   } else if (strcmp ("overwrite", x_lower) == 0) {
      DEBUG_HIST  yLOG_note    ("call CELL_overwrite");
      if (strcmp (hist[chist].after , "???::[<{(null)}>]") == 0) {
         CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
      } else {
         sprintf (x_temp, "%c%c%c", hist[chist].after [0], hist[chist].after [1], hist[chist].after [2]);
         CELL_overwrite (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow, hist[chist].after  + 5, x_temp);
      }
   } else if (strcmp ("delete", x_lower) == 0) {
      CELL_delete (CHG_NOHIST, hist[chist].btab, hist[chist].bcol, hist[chist].brow);
   } else if (strcmp ("decimals", x_lower) == 0) {
      CELL_visual   (CHANGE_DECIMAL, CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("align"   , x_lower) == 0) {
      CELL_visual   (CHANGE_ALIGN  , CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("format"  , x_lower) == 0) {
      CELL_visual   (CHANGE_FORMAT , CHG_NOHIST, hist[chist].after[0]);
   } else if (strcmp ("width"   , x_lower) == 0) {
      CELL_visual   (CHANGE_WIDTH  , CHG_NOHIST, -(atoi (hist[chist].after)));
   }
   /*---(tail recursion)-----------------*/
   if (chist <  nhist - 1) {
      if (strcmp (hist[chist + 1].act, x_upper) == 0)  HIST_redo ();
   }
   /*---(complet)------------------------*/
   return 0;
}

char         /*-> list history -----------------------[ leaf   [ge.740.042.20]*/ /*-[03.0000.103.!]-*/ /*-[--.---.---.--]-*/
HIST_list          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_name      [100]       = "";
   char        rce         = -10;
   FILE       *x_file      = NULL;
   /*---(open)---------------------------*/
   snprintf (x_name, 95, "%s.hist", my.f_title);
   x_file = fopen(x_name, "w");
   --rce;
   if (x_file == NULL)      return rce;
   /*---(print)--------------------------*/
   fprintf (x_file, "details of history\n\n");
   fprintf (x_file, "nhist = %d\n" , nhist);
   fprintf (x_file, "chist = %d\n" , chist);
   for (i = 0; i < nhist; ++i) {
      if (i % 5 == 0)  fprintf (x_file, "\n-ref- %c action--------- %c btab %c bcol %c brow %c before/after ----------------------------------------\n", 31, 31, 31, 31, 31);
      fprintf (x_file, "%-5d %c %-15.15s %c %4d %c %4d %c %4d %c %s %c %s %c\n", i,
            31, hist [i].act   , 31, hist [i].btab  ,
            31, hist [i].bcol  , 31, hist [i].brow  ,
            31, hist [i].before, 31, hist [i].after , 31);
   }
   fprintf (x_file, "\ndone\n");
   /*---(close)--------------------------*/
   fclose (x_file);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    reading and writing tabs                  ----===*/
/*====================------------------------------------====================*/
static void   o___TABS____________o (void) { return; }

/* tab records are allowed to evolve separately from other input types as the
 * need arises.  as such, they have their own versioning and format.
 *
 * C) tab         tab#  ncol  nrow  ccol  crow  name-------------------
 *    tab            0    26   100     2     6  tab00
 *
 * D) tab         tab#  ncol  nrow  bcol  brow  ccol  crow  name-------------------
 *    tab            0    26   100     2     3     5    13  tab00
 *
 * E) tab         tab#  ncol  nrow  bcol  brow  ccol  crow  t -lbeg- -lend-  name-------------------
 *    tab            0    26   100     0     1     5    23  - ------ ------  tab00
 *
 * F) tab         ver  tab#  --max--  --beg--  --cur--  t  lockbeg  lockend  name-------------------
 *    tab         -F-     0  0az999   0a1      0g5      b  0a5      0b5      one
 *
 * G) tab         ver  --max--  name-------------------
 *    tab         -G-  0az999   one
 *
 *
 */

/*> char         /+-> process a tab size record ----------[ ------ [ge.943.354.35]+/ /+-[01.0000.013.E]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_tab           (char *a_label, char *a_name, char a_type)                                                                  <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    int         rc          = 0;                                                                                                <* 
 *>    int         x_tab       = 0;                                                                                                <* 
 *>    int         x_col       = 0;                                                                                                <* 
 *>    int         x_row       = 0;                                                                                                <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    DEBUG_INPT  yLOG_point   ("a_label"   , a_label);                                                                           <* 
 *>    DEBUG_INPT  yLOG_point   ("a_name"    , a_name);                                                                            <* 
 *>    /+---(parse address)------------+/                                                                                          <* 
 *>    rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);                                                                     <* 
 *>    DEBUG_INPT  yLOG_value   ("parse"     , rc);                                                                                <* 
 *>    --rce;  if (rc < 0)  {                                                                                                      <* 
 *>       DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                                                                            <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_INPT  yLOG_info    ("a_label"   , a_label);                                                                           <* 
 *>    /+---(update size)--------------+/                                                                                          <* 
 *>    rc = LOC_tab_resize     (a_label);                                                                                          <* 
 *>    DEBUG_INPT  yLOG_value   ("resize"    , rc);                                                                                <* 
 *>    --rce;  if (rc < 0)  {                                                                                                      <* 
 *>       DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                                                                            <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_INPT  yLOG_value   ("col_max"   , LOC_col_max (x_tab));                                                               <* 
 *>    DEBUG_INPT  yLOG_value   ("row_max"   , LOC_row_max (x_tab));                                                               <* 
 *>    /+---(change name)--------------+/                                                                                          <* 
 *>    rc = LOC_tab_rename     (x_tab, a_name);                                                                                    <* 
 *>    DEBUG_INPT  yLOG_value   ("rename"    , rc);                                                                                <* 
 *>    --rce;  if (rc < 0)  {                                                                                                      <* 
 *>       DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                                                                            <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_INPT  yLOG_info    ("a_name"    , a_name);                                                                            <* 
 *>    /+---(complete)-----------------+/                                                                                          <* 
 *>    DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/






char         /*-> write file tab information ---------[ ------ [ge.960.195.59]*/ /*-[01.0000.01#.#]-*/ /*-[--.---.---.--]-*/
OUTP_tab             (short a_tab)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;          /* generic return code            */
   char        rce         =  -10;          /* return code for errors         */
   char        x_write     =  '-';
   short       x_cols      =    0;
   short       x_rows      =    0;
   char        x_addr      [25]        = "";
   char        x_name      [25]        = "";
   char        x_default   [25]        = "";
   char        x_type      =  '-';
   /*---(prepare)------------------------*/
   sprintf (my.f_recd, "");
   /*---(defense)------------------------*/
   rc = LOC_tab_valid (a_tab);
   --rce;  if (rc      <  0)                     return rce;
   x_type = LOC_tab_type (a_tab);
   /*---(gather size)--------------------*/
   x_cols = LOC_col_max (a_tab);
   /*> printf ("x_cols = %4d, default = %4d\n", x_cols, LOC_col_defmax());            <*/
   if (x_cols != LOC_col_defmax())       x_write = 'y';
   x_rows = LOC_row_max (a_tab);
   /*> printf ("x_rows = %4d, default = %4d\n", x_rows, LOC_row_defmax());            <*/
   if (x_rows != LOC_row_defmax())       x_write = 'y';
   LOC_ref (a_tab, x_cols - 1, x_rows - 1, 0, x_addr);
   /*---(gather name)--------------------*/
   LOC_tab_name    (a_tab, x_name);
   LOC_tab_defname (a_tab, x_default);
   /*> printf ("name   = %-10.10s, default = %-10.10s\n", x_name, x_default);         <*/
   if (strcmp (x_default, x_name) != 0)  x_write = 'y';
   /*---(build record)-------------------*/
   if (x_write == 'y') {
      sprintf (my.f_recd, "tab         -H-  %-8s  %-12.12s  %c ", x_addr, x_name, x_type);
      return 1;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> write file tab information ---------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_tab_head        (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   fprintf (a_file, "#===[[ TAB LAYOUT ]]=================================================================================================#\n");
   fprintf (a_file, "#---------  ver  --max---  ---name----- \n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> write file tab information ---------[ leaf   [ge.420.213.30]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_tab_foot        (FILE *a_file, int a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   if (a_count == 0)  fprintf (a_file, "# no special or unique tab information\n");
   else               fprintf (a_file, "#---------  ver  --max---  ---name----- \n");
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

int          /*-> write file tab information ---------[ ------ [gn.430.131.23]*/ /*-[01.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_tabs            (FILE *a_file)
{
   int i = 0;
   int c = 0;
   char rc = 0;
   rc = OUTP_tab_head  (a_file);
   for (i = 0; i <  MAX_TABS; ++i) {
      rc = OUTP_tab    (i);
      if (rc <= 0)   continue;
      if (a_file != NULL)  fprintf (a_file, "%s\n", my.f_recd);
      ++c;
   }
   rc = OUTP_tab_foot  (a_file, c);
   return c;
}



/*====================------------------------------------====================*/
/*===----                       column width                           ----===*/
/*====================------------------------------------====================*/
static void   o___COLUMNS_________o (void) { return; }

/*> char         /+-> process a column width record ------[ ------ [ge.733.253.22]+/ /+-[01.0000.013.8]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_col           (char *a_label, int a_size, int a_count)                                                                    <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    int         rc          = 0;                                                                                                <* 
 *>    int         x_tab       = 0;                                                                                                <* 
 *>    int         x_col       = 0;                                                                                                <* 
 *>    int         x_row       = 0;                                                                                                <* 
 *>    int         i           = 0;                                                                                                <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    DEBUG_INPT  yLOG_point   ("a_label"   , a_label);                                                                           <* 
 *>    DEBUG_INPT  yLOG_value   ("a_size"    , a_size);                                                                            <* 
 *>    DEBUG_INPT  yLOG_value   ("a_count"   , a_count);                                                                           <* 
 *>    /+---(parse address)------------+/                                                                                          <* 
 *>    rc = LOC_parse (a_label, &x_tab, &x_col, &x_row, NULL);                                                                     <* 
 *>    DEBUG_INPT  yLOG_value   ("parse"     , rc);                                                                                <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                                                                            <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_INPT  yLOG_info    ("a_label"   , a_label);                                                                           <* 
 *>    DEBUG_INPT  yLOG_value   ("tab"       , x_tab);                                                                             <* 
 *>    DEBUG_INPT  yLOG_value   ("col"       , x_col);                                                                             <* 
 *>    /+---(update size)--------------+/                                                                                          <* 
 *>    if (a_count == 0)  a_count = 1;                                                                                             <* 
 *>    for (i = 0; i < a_count; ++i) {                                                                                             <* 
 *>       rc = LOC_col_widen      (x_tab, x_col + i, a_size);                                                                      <* 
 *>       DEBUG_INPT  yLOG_value   ("widen"     , rc);                                                                             <* 
 *>       --rce;  if (rc < 0) {                                                                                                    <* 
 *>          DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);                                                                         <* 
 *>          return rce;                                                                                                           <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------+/                                                                                          <* 
 *>    DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

char         /*-> write file col information ---------[ ------ [ge.640.266.34]*/ /*-[00.0000.01#.#]-*/ /*-[--.---.---.--]-*/
OUTP_col             (short a_tab, short a_col)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;          /* generic return code            */
   char        rce         =  -10;          /* return code for errors         */
   char        x_write     =  '-';
   short       x_cols      =    0;
   short       x_size      =    0;
   char        x_addr      [25]        = "";
   /*---(prepare)------------------------*/
   sprintf (my.f_recd, "");
   /*---(defense)------------------------*/
   rc = LOC_col_valid (a_tab, a_col);
   --rce;  if (rc      <  0)                     return rce;
   /*---(gather size)--------------------*/
   x_cols = LOC_col_max   (a_tab);
   --rce;  if (a_col > x_cols)                   return 0;
   x_size = LOC_col_width (a_tab, a_col);
   --rce;  if (x_size == DEF_WIDTH)              return 0;
   /*---(create a label)-----------------*/
   LOC_ref (a_tab, a_col, 0, 0, x_addr);
   /*---(build record)-------------------*/
   sprintf (my.f_recd, "width       -A-  %-8s  %4d ", x_addr, x_size);
   /*---(complete)-----------------------*/
   return 1;
}

char         /*-> write file col header --------------[ leaf   [ge.320.113.10]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_col_head        (FILE *a_file)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   fprintf (a_file, "#===[[ COLUMN WIDTHS ]]==============================================================================================#\n");
   fprintf (a_file, "#---------  ver  ---loc--  size \n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*-> write file col footer --------------[ leaf   [ge.420.213.20]*/ /*-[00.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_col_foot        (FILE *a_file, int a_count)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;           /* return code for errors         */
   /*---(defenses)-----------------------*/
   --rce;  if (a_file == NULL)                   return rce;
   /*---(header)-------------------------*/
   if (a_count == 0)  fprintf (a_file, "# no special or unique col information\n");
   fprintf (a_file, "#---------  ver  ---loc--  size \n");
   fprintf (a_file, "\n\n\n");
   fflush  (a_file);
   /*---(complete)-----------------------*/
   return 0;
}

int          /*-> write file col section -------------[ ------ [gn.440.141.23]*/ /*-[02.0000.01#.4]-*/ /*-[--.---.---.--]-*/
OUTP_cols            (FILE *a_file)
{
   int i = 0;
   int j = 0;
   int c = 0;
   char rc = 0;
   rc = OUTP_col_head  (a_file);
   for (i = 0; i < MAX_TABS; ++i) {
      for (j = 0; j <= MAX_COLS; ++j) {
         rc = OUTP_col    (i, j);
         if (rc <= 0)   continue;
         if (a_file != NULL)  fprintf (a_file, "%s\n", my.f_recd);
         ++c;
      }
   }
   rc = OUTP_col_foot  (a_file, c);
   return c;
}


char
INPT_tab             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_label     [LEN_LABEL];
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'I') {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check tab)----------------------*/
   x_tab   = atoi (a);
   DEBUG_INPT   yLOG_value   ("x_tab"     , x_tab);
   rc = LOC_tab_valid (x_tab);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check name)---------------------*/
   DEBUG_INPT  yLOG_point   ("name"      , b);
   if (b != NULL) {
      rc = LOC_tab_rename     (x_tab, b);
      DEBUG_INPT  yLOG_value   ("rename"    , rc);
      --rce;  if (rc < 0)  {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_INPT  yLOG_info    ("a_name"    , b);
   }
   /*---(resize)-------------------------*/
   x_col   = atoi (d);
   DEBUG_INPT   yLOG_value   ("x_col"     , x_col);
   x_row   = atoi (f);
   DEBUG_INPT   yLOG_value   ("x_row"     , x_row);
   rc = LOC_ref (x_tab, x_col - 1, x_row - 1, 0, x_label);
   DEBUG_INPT  yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("x_label"   , x_label);
   rc = LOC_tab_resize     (x_label);
   DEBUG_INPT  yLOG_value   ("resize"    , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_value   ("col_max"   , LOC_col_max (x_tab));
   DEBUG_INPT  yLOG_value   ("row_max"   , LOC_row_max (x_tab));
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
INPT_col             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_size      =    0;
   int         x_count     =    0;
   int         z           =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'B') {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check tab)----------------------*/
   x_tab   = atoi (a);
   DEBUG_INPT   yLOG_value   ("x_tab"     , x_tab);
   rc = LOC_tab_valid (x_tab);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check col)----------------------*/
   x_col   = atoi (b);
   DEBUG_INPT   yLOG_value   ("x_col"     , x_col);
   rc = LOC_tab_valid (x_col);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update size)--------------*/
   x_size  = atoi (c);
   x_count = atoi (d);
   if (x_count == 0)  x_count = 1;
   for (z = 0; z < x_count; ++z) {
      rc = LOC_col_widen      (x_tab, x_col + z, x_size);
      DEBUG_INPT  yLOG_value   ("widen"     , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
INPT_row             (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_row       =    0;
   int         x_size      =    0;
   int         x_count     =    0;
   int         z           =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'B') {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check tab)----------------------*/
   x_tab   = atoi (a);
   DEBUG_INPT   yLOG_value   ("x_tab"     , x_tab);
   rc = LOC_tab_valid (x_tab);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check col)----------------------*/
   x_row   = atoi (b);
   DEBUG_INPT   yLOG_value   ("x_row"     , x_row);
   rc = LOC_tab_valid (x_row);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update size)--------------*/
   x_size  = atoi (c);
   x_count = atoi (d);
   if (x_count == 0)  x_count = 1;
   for (z = 0; z < x_count; ++z) {
      rc = LOC_row_heighten   (x_tab, x_row + z, x_size);
      DEBUG_INPT  yLOG_value   ("heighten"  , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
OUTP_row              (void)
{
   /*> /+---(locals)-----------+-----------+-+/                                                                                                                       <* 
    *> char        i           =    0;                                                                                                                                <* 
    *> char        c           =    0;                                                                                                                                <* 
    *> /+---(find marked entries)------------+/                                                                                                                       <* 
    *> for (i = 1; i < s_nmark; ++i) {                                                                                                                                <* 
    *>    strlcpy (myVIKEYS.f_recd, "", LEN_RECD);                                                                                                                    <* 
    *>    if (s_mark_info [i].source == MARK_NONE)  continue;                                                                                                         <* 
    *>    yVIKEYS_file_write (UMOD_MARK, &(S_MARK_LIST [i]), &s_mark_info [i].x_pos, &s_mark_info [i].y_pos, &s_mark_info [i].z_pos, NULL, NULL, NULL, NULL, NULL);   <* 
    *>    ++c;                                                                                                                                                        <* 
    *> }                                                                                                                                                              <* 
    *> /+---(complete)-----------------------+/                                                                                                                       <* 
    *> return c;                                                                                                                                                      <*/
}



/*====================------------------------------------====================*/
/*===----                    reading and writing cells                 ----===*/
/*====================------------------------------------====================*/
PRIV void  o___CELLS___________o () { return; }

static int s_stamp     = 0;

char         /*-> write a single cell to a file ------[ leaf   [ge.630.514.50]*/ /*-[02.0000.124.!]-*/ /*-[--.---.---.--]-*/
OUTP_cell          (char a_type, int a_seq, int a_level, tCELL *a_curr)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        x_format    [LEN_LABEL] = "";
   char        x_level     [LEN_LABEL] = "";
   char        t           [LEN_RECD]  = "";
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_OUTP   yLOG_point   ("a_curr"    , a_curr);
   --rce;  if (a_curr  == NULL) {
      DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(level)--------------------------*/
   if      (a_level <   0)   sprintf (x_level, "             ");
   else if (a_level <  10)   sprintf (x_level, "%-*.*s%d%-15.15s", a_level, a_level, "------------", a_level, " ");
   else                      sprintf (x_level, "          +++");
   /*---(format)-------------------------*/
   sprintf (x_format, "%c %c %c %c %c", a_curr->t, a_curr->f, a_curr->d, a_curr->a, a_curr->n);
   /*---(call writer)--------------------*/
   strlcpy  (t, a_curr->s, LEN_RECD);
   strldchg (t, G_KEY_SPACE, G_CHAR_SPACE, LEN_RECD);
   yVIKEYS_file_write (a_type, x_level, &a_seq, a_curr->label, x_format, t, NULL, NULL, NULL, NULL);
   /*---(complete)-----------------------*/
   DEBUG_OUTP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> write file independent cells -------[ ------ [ge.9A0.86#.I4]*/ /*-[02.0000.01#.!]-*/ /*-[--.---.---.--]-*/
OUTP_cell_free          (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x           =    0;             /* iterator -- columns            */
   int         x_end       =    0;
   int         y           =    0;             /* iterator -- row                */
   int         y_end       =    0;
   int         z           =    0;             /* iterator -- tabs               */
   tCELL      *x_curr      = NULL;
   int         x_seq       =    0;
   /*---(cells)--------------------------*/
   for (z = 0; z < NTAB; ++z) {
      x_end = LOC_col_max (z) - 1;
      y_end = LOC_row_max (z) - 1;
      for (x = 0; x <= x_end; ++x) {
         for (y = 0; y <= y_end; ++y) {
            x_curr = LOC_cell_at_loc (z, x, y);
            if (x_curr    == NULL)                       continue;
            if (x_curr->s == NULL)                       continue;
            if (x_curr->t == CTYPE_BLANK)                continue;
            if (x_curr->u == s_stamp)                    continue;
            OUTP_cell (FILE_FREECEL, x_seq++, -1, x_curr);
         }
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
OUTP_cell_dep           (void)
{
   char        rc          =    0;
   s_stamp   = rand ();
   rc = SEQ_file_deps (s_stamp);
   return 0;
}

char
INPT_cell            (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   char        x_string    [LEN_LABEL] = "?0?";
   tCELL      *x_new       = NULL;
   char        x_format    =  '?';
   char        x_decs      =  '0';
   char        x_align     =  '?';
   /*> int         x_index     =    0;                                                <*/
   /*> char        x_label     [LEN_LABEL] = "";                                      <*/
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (strchr ("D", n) == NULL) {
      DEBUG_INPT   yLOG_note    ("illegal version");
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse address)------------*/
   DEBUG_INPT  yLOG_point   ("label"     , c);
   --rce;  if (*c  == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = LOC_parse (c, &x_tab, &x_col, &x_row, NULL);
   DEBUG_INPT  yLOG_value   ("parse"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_info    ("label"     , c);
   /*---(get position)-------------------*/
   DEBUG_INPT  yLOG_value   ("x_tab"     , x_tab);
   DEBUG_INPT  yLOG_value   ("x_col"     , x_col);
   DEBUG_INPT  yLOG_value   ("x_row"     , x_row);
   /*---(expand everything as needed)----*/
   rc = LOC_legal (x_tab, x_col, x_row, CELL_GROW);
   DEBUG_INPT  yLOG_value   ("legal"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(fix format)---------------------*/
   if (d == NULL) {
      strcpy  (x_string, "??0");
   } else if (strlen (d) == 9) {
      x_format  = CELL_format_valid   (d [2]);
      DEBUG_INPT  yLOG_char    ("x_format"  , x_format);
      x_decs    = CELL_decimals_valid (d [4]);
      DEBUG_INPT  yLOG_char    ("x_decs"    , x_decs);
      x_align   = CELL_align_valid    (d [6]);
      DEBUG_INPT  yLOG_char    ("x_align"   , x_align);
      sprintf (x_string, "%c%c%c", x_format, x_align, x_decs);
   } else {
      strcpy  (x_string, "??0");
   }
   DEBUG_INPT  yLOG_info    ("x_string"  , x_string);
   /*---(update)-------------------------*/
   DEBUG_INPT  yLOG_info    ("source"    , e);
   strldchg (e, G_CHAR_SPACE, G_KEY_SPACE, LEN_RECD);
   x_new = CELL_overwrite (CHG_NOHIST, x_tab, x_col, x_row, e, x_string);
   DEBUG_INPT  yLOG_point   ("x_new"     , x_new);
   --rce;  if (x_new == NULL)  {
      DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          reading files                       ----===*/
/*====================------------------------------------====================*/
static void   o___READ____________o (void) { return; }

/*> char         /+-> open file for reading and prep -----[ leaf   [ge.723.023.20]+/ /+-[01.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_open          ()                                                                                                          <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         =  -10;                                                                                             <* 
 *>    char       *p           = NULL;                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    DEBUG_INPT  yLOG_info    ("f_loc"     , my.f_loc);                                                                          <* 
 *>    DEBUG_INPT  yLOG_info    ("f_name"    , my.f_name);                                                                         <* 
 *>    DEBUG_INPT  yLOG_info    ("f_title"   , my.f_title);                                                                        <* 
 *>    /+---(parse name)---------------------+/                                                                                    <* 
 *>    s_file = NULL;                                                                                                              <* 
 *>    --rce;  if (strcmp (my.f_name, FILE_BLANK) == 0) {                                                                          <* 
 *>       DEBUG_INPT  yLOG_note    ("file name is default");                                                                       <* 
 *>       DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                 <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    /+---(open file)----------------------+/                                                                                    <* 
 *>    s_file = fopen (my.f_title, "r");                                                                                           <* 
 *>    DEBUG_INPT  yLOG_point   ("s_file"    , s_file);                                                                            <* 
 *>    --rce;  if (s_file == NULL) {                                                                                               <* 
 *>       DEBUG_INPT  yLOG_note    ("file could not be openned");                                                                  <* 
 *>       DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                 <* 
 *>       return 0;                                                                                                                <* 
 *>    }                                                                                                                           <* 
 *>    DEBUG_INPT  yLOG_note    ("file successfully opened");                                                                      <* 
 *>    /+---(complete)-----------------+/                                                                                          <* 
 *>    DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> open file for reading and prep -----[ leaf   [gz.421.001.00]+/ /+-[00.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_prep          (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    /+---(initialize)---------------------+/                                                                                    <* 
 *>    strncpy (f_maker, "unknown", LEN_RECD);                                                                                     <* 
 *>    my.f_lines = 0;                                                                                                             <* 
 *>    DEBUG_INPT  yLOG_note    ("setting default size and locations");                                                            <* 
 *>    NCOL = DEF_COLS;                                                                                                            <* 
 *>    BCOL = ECOL = 0;                                                                                                            <* 
 *>    NROW = DEF_ROWS;                                                                                                            <* 
 *>    BROW = EROW = 0;                                                                                                            <* 
 *>    /+---(complete)-----------------+/                                                                                          <* 
 *>    DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> open file for reading and prep -----[ ------ [gz.421.001.06]+/ /+-[00.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_done          (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    /+---(screen)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT   yLOG_note    ("set screen positions correctly");                                                               <* 
 *>    /+> KEYS_basics (' ', 'r');                                                        <+/                                      <* 
 *>    /+> KEYS_bcol (BCOL);                                                              <+/                                      <* 
 *>    /+> CURS_col_head();                                                               <+/                                      <* 
 *>    /+> KEYS_brow (BROW);                                                              <+/                                      <* 
 *>    /+> CURS_row_head();                                                               <+/                                      <* 
 *>    /+---(calculate)----------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_note    ("recalc");                                                                                        <* 
 *>    SEQ_calc_full ();                                                                                                           <* 
 *>    /+---(complete)-----------------+/                                                                                          <* 
 *>    DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> close file for reading and wrap ----[ leaf   [ge.411.011.20]+/ /+-[01.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_close         (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    /+---(close file)---------------------+/                                                                                    <* 
 *>    if (s_file == NULL) {                                                                                                       <* 
 *>       DEBUG_INPT  yLOG_note    ("no file to close");                                                                           <* 
 *>    } else {                                                                                                                    <* 
 *>       DEBUG_INPT  yLOG_note    ("close file");                                                                                 <* 
 *>       fclose  (s_file);                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------+/                                                                                          <* 
 *>    DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> file reading driver ----------------[ leaf   [ge.632.025.30]+/ /+-[01.0001.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_read          (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         =  -10;               /+ return code for errors    +/                                               <* 
 *>    int         x_len       =    0;               /+ string length             +/                                               <* 
 *>    /+---(read and clean)--------------+/                                                                                       <* 
 *>    ++my.f_lines;                                                                                                               <* 
 *>    DEBUG_INPT  yLOG_value   ("line"      , my.f_lines);                                                                        <* 
 *>    fgets (my.f_recd, LEN_RECD, s_file);                                                                                        <* 
 *>    --rce;  if (feof (s_file))  {                                                                                               <* 
 *>       DEBUG_INPT  yLOG_note    ("end of file reached");                                                                        <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    x_len = strlen (my.f_recd);                                                                                                 <* 
 *>    --rce;  if (x_len <= 0)  {                                                                                                  <* 
 *>       DEBUG_INPT  yLOG_note    ("record empty");                                                                               <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    my.f_recd [--x_len] = '\0';                                                                                                 <* 
 *>    DEBUG_INPT  yLOG_value   ("length"    , x_len);                                                                             <* 
 *>    DEBUG_INPT  yLOG_info    ("fixed"     , my.f_recd);                                                                         <* 
 *>    --rce;  if (my.f_recd [0] == '#') {                                                                                         <* 
 *>       DEBUG_INPT  yLOG_note    ("comment line, skipping");                                                                     <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> file reading driver ----------------[ leaf   [ge.851.163.30]+/ /+-[01.0000.013.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_parse         (cchar *a_recd)                                                                                             <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         =  -10;               /+ return code for errors    +/                                               <* 
 *>    char        x_recd      [LEN_RECD];                                                                                         <* 
 *>    int         x_len       =    0;                                                                                             <* 
 *>    char       *p           = NULL;               /+ strtok pointer            +/                                               <* 
 *>    char       *q           = "";               /+ strtok delimeters         +/                                                <* 
 *>    char       *r           = NULL;               /+ strtok context variable   +/                                               <* 
 *>    /+---(cleanse)---------------------+/                                                                                       <* 
 *>    for (s_nfield = 0; s_nfield < 20; ++s_nfield) {                                                                             <* 
 *>       strlcpy (s_fields [s_nfield], "", LEN_RECD);                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    s_nfield = 0;                                                                                                               <* 
 *>    strlcpy (x_recd, a_recd, LEN_RECD);                                                                                         <* 
 *>    /+---(parse fields)----------------+/                                                                                       <* 
 *>    p = strtok_r (x_recd, q, &r);                                                                                               <* 
 *>    while (p != NULL) {                                                                                                         <* 
 *>       strltrim  (p, ySTR_BOTH, LEN_RECD);                                                                                      <* 
 *>       DEBUG_INPT  yLOG_bullet  (s_nfield    , p);                                                                              <* 
 *>       strncpy   (s_fields [s_nfield++], p, LEN_RECD);                                                                          <* 
 *>       p = strtok_r (NULL     , q, &r);                                                                                         <* 
 *>    }                                                                                                                           <* 
 *>    --rce;  if (s_nfield < 1) {                                                                                                 <* 
 *>       DEBUG_INPT  yLOG_note    ("no fields found");                                                                            <* 
 *>       return -rce;                                                                                                             <* 
 *>    }                                                                                                                           <* 
 *>    strncpy   (my.f_type, s_fields [0], LEN_RECD);                                                                              <* 
 *>    DEBUG_INPT  yLOG_info    ("type"      , my.f_type);                                                                         <* 
 *>    if (s_fields [1][0] == '-' && s_fields [1][2] == '-')                                                                       <* 
 *>       my.f_vers  = s_fields [1][1];                                                                                            <* 
 *>    else                                                                                                                        <* 
 *>       my.f_vers  = '?';                                                                                                        <* 
 *>    DEBUG_INPT  yLOG_char    ("vers"      , my.f_vers);                                                                         <* 
 *>    /+---(complete)-----------------------+/                                                                                    <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/

/*> char         /+-> file reading driver ----------------[ leaf   [ge.C71.072.GA]+/ /+-[02.0000.102.!]-+/ /+-[--.---.---.--]-+/   <* 
 *> INPT_main          (void)                                                                                                      <* 
 *> {                                                                                                                              <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                    <* 
 *>    char        rce         = -10;                                                                                              <* 
 *>    int         rc          = 0;                                                                                                <* 
 *>    char        x_temp      [LEN_RECD];            /+ strtok version of input   +/                                              <* 
 *>    int         x_len       = 0;                  /+ string length             +/                                               <* 
 *>    char       *p;                                                                                                              <* 
 *>    int         x_celltry   = 0;                                                                                                <* 
 *>    int         x_cellbad   = 0;                                                                                                <* 
 *>    /+---(header)-------------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_enter   (__FUNCTION__);                                                                                    <* 
 *>    /+---(open file)----------------------+/                                                                                    <* 
 *>    rc = INPT_open   ();                                                                                                        <* 
 *>    --rce;  if (rc < 0) {                                                                                                       <* 
 *>       DEBUG_INPT  yLOG_exit    (__FUNCTION__);                                                                                 <* 
 *>       return rce;                                                                                                              <* 
 *>    }                                                                                                                           <* 
 *>    rc = INPT_prep   ();                                                                                                        <* 
 *>    /+---(read lines)---------------------+/                                                                                    <* 
 *>    DEBUG_INPT  yLOG_note    ("read lines");                                                                                    <* 
 *>    while (s_file != NULL) {                                                                                                    <* 
 *>       /+---(read and clean)--------------+/                                                                                    <* 
 *>       rc = INPT_read ();                                                                                                       <* 
 *>       if (rc < 0)  break;                                                                                                      <* 
 *>       if (rc > 0)  continue;                                                                                                   <* 
 *>       rc = INPT_parse (my.f_recd);                                                                                             <* 
 *>       if (rc < 0)  break;                                                                                                      <* 
 *>       if (rc > 0)  continue;                                                                                                   <* 
 *>       /+---(handle types)----------------+/                                                                                    <* 
 *>       switch (my.f_type [0]) {                                                                                                 <* 
 *>       case 'w' :                                                                                                               <* 
 *>          if  (my.f_vers == 'B')  rc = INPT_col   (s_fields [2], atoi (s_fields [3]), atoi (s_fields [4]));                     <* 
 *>          if  (my.f_vers == 'A')  rc = INPT_col   (s_fields [2], atoi (s_fields [3]), 1);                                       <* 
 *>          if  (my.f_vers == '?')  rc = INPT_col   (s_fields [1], atoi (s_fields [2]), 1);                                       <* 
 *>          break;                                                                                                                <* 
 *>       case 't' :                                                                                                               <* 
 *>          if  (my.f_vers == 'F')  rc = INPT_tab   (s_fields [3], s_fields [9], '-');                                            <* 
 *>          if  (my.f_vers == 'G')  rc = INPT_tab   (s_fields [2], s_fields [3], '-');                                            <* 
 *>          if  (my.f_vers == 'H')  rc = INPT_tab   (s_fields [2], s_fields [3], s_fields [4][0]);                                <* 
 *>          break;                                                                                                                <* 
 *>       case 'c' :                                                                                                               <* 
 *>          if  (my.f_vers == 'D')  rc = INPT_cell_OLD  (s_fields [4], s_fields [5], s_fields [6]);                               <* 
 *>          if (rc < 0)  ++x_cellbad;                                                                                             <* 
 *>          break;                                                                                                                <* 
 *>       case 'l' :                                                                                                               <* 
 *>          if  (my.f_vers == 'A')  rc = MARK_read  (s_fields [2][0], s_fields [3]);                                              <* 
 *>          if (rc < 0)  ++x_cellbad;                                                                                             <* 
 *>          break;                                                                                                                <* 
 *>       }                                                                                                                        <* 
 *>    }                                                                                                                           <* 
 *>    /+---(close file)---------------------+/                                                                                    <* 
 *>    INPT_close ();                                                                                                              <* 
 *>    INPT_done  ();                                                                                                              <* 
 *>    /+---(complete)-------------------------+/                                                                                  <* 
 *>    DEBUG_INPT yLOG_exit    (__FUNCTION__);                                                                                     <* 
 *>    return 0;                                                                                                                   <* 
 *> }                                                                                                                              <*/



/*====================------------------------------------====================*/
/*===----                          writing files                       ----===*/
/*====================------------------------------------====================*/
PRIV void  o___WRITE___________o () { return; }


/*> char         /+-> tbd --------------------------------[ ------ [ge.C71.093.38]+/ /+-[01.0000.11#.!]-+/ /+-[--.---.---.--]-+/                  <* 
 *> FILE_write         (void)                                                                                                                     <* 
 *> {                                                                                                                                             <* 
 *>    /+---(locals)-----------+-----------+-+/                                                                                                   <* 
 *>    char        rce         =  -10;                                                                                                            <* 
 *>    char        rc          =    0;                                                                                                            <* 
 *>    FILE       *f           = NULL;                                                                                                            <* 
 *>    int         x_seq;                                                                                                                         <* 
 *>    int         i           = 0;       /+ iterator -- columns                            +/                                                    <* 
 *>    long        x_stamp     = 0;                                                                                                               <* 
 *>    char        x_temp      [100];                                                                                                             <* 
 *>    char       *x_bufs      = "abcdefghijklmnopqrstuvwxyz";                                                                                    <* 
 *>    int         x_len       = 0;                                                                                                               <* 
 *>    /+---(header)-------------------------+/                                                                                                   <* 
 *>    DEBUG_OUTP yLOG_enter   (__FUNCTION__);                                                                                                    <* 
 *>    /+---(open file)----------------------+/                                                                                                   <* 
 *>    --rce;  if (strcmp (my.f_name, FILE_BLANK) == 0) {                                                                                         <* 
 *>       DEBUG_OUTP  yLOG_note    ("file name is default");                                                                                      <* 
 *>       DEBUG_OUTP  yLOG_exit    (__FUNCTION__);                                                                                                <* 
 *>       return rce;                                                                                                                             <* 
 *>    }                                                                                                                                          <* 
 *>    f = fopen (my.f_title, "w");                                                                                                               <* 
 *>    if (f == NULL)      return -2;                                                                                                             <* 
 *>    /+---(header)-------------------------+/                                                                                                   <* 
 *>    OUTP_header (f);                                                                                                                           <* 
 *>    /+---(tab data)-----------------------+/                                                                                                   <* 
 *>    OUTP_tabs   (f);                                                                                                                           <* 
 *>    /+---(column data)--------------------+/                                                                                                   <* 
 *>    OUTP_cols   (f);                                                                                                                           <* 
 *>    /+---(dependent cells)------------------+/                                                                                                 <* 
 *>    fprintf (f, "#===[[ DEPENDENCY TREE CELLS, in reverse order ]]====================================================================#\n");   <* 
 *>    x_stamp   = rand ();                                                                                                                       <* 
 *>    /+> rc = SEQ_file_deps (x_s, f);                                               <+/                                                         <* 
 *>    fprintf (f, "# dependent cells complete\n\n\n\n");                                                                                         <* 
 *>    /+---(non-dependency cells)-------------+/                                                                                                 <* 
 *>    fprintf (f, "#===[[ INDENPENDENT CELLS, tab then col then row order]]=============================================================#\n");   <* 
 *>    x_seq     = 0;                                                                                                                             <* 
 *>    for (i = 0; i < MAX_TABS; ++i) {                                                                                                           <* 
 *>       /+> rc = OUTP_cell_free (f, &x_seq, x_stamp, i, 0, LOC_col_max (i) - 1, 0, LOC_row_max (i) - 1);   <+/                                  <* 
 *>    }                                                                                                                                          <* 
 *>    fprintf (f, "# independent cells complete\n\n\n\n");                                                                                       <* 
 *>    /+---(mark data)----------------------+/                                                                                                   <* 
 *>    /+> MARK_writeall (f);                                                             <+/                                                     <* 
 *>    /+---(footer data)----------------------+/                                                                                                 <* 
 *>    fprintf (f, "# done, finito, complete\n");                                                                                                 <* 
 *>    /+---(close file)-----------------------+/                                                                                                 <* 
 *>    fclose  (f);                                                                                                                               <* 
 *>    /+---(make version)---------------------+/                                                                                                 <* 
 *>    if (ver_ctrl == 'y') {                                                                                                                     <* 
 *>       sprintf (x_temp, "cp -f %s %s.v%c%c%s.gyges", my.f_title, my.f_name, ver_num[0], ver_num[1], ver_num + 3);                              <* 
 *>       system (x_temp);                                                                                                                        <* 
 *>    }                                                                                                                                          <* 
 *>    /+---(complete)-------------------------+/                                                                                                 <* 
 *>    DEBUG_INPT yLOG_exit    (__FUNCTION__);                                                                                                    <* 
 *>    return 0;                                                                                                                                  <* 
 *> }                                                                                                                                             <*/

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
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
PRIV void  o___UNIT_TEST_______o () { return; }

char*        /*-> unit test accessor -----------------[ ------ [gs.950.221.M1]*/ /*-[03.0000.00#.#]-*/ /*-[--.---.---.--]-*/
FILE_unit          (char *a_question, int a_ref)
{
   /*---(locals)-----------+-----------+-*/
   int         rc          = -1;
   char        x_name      [LEN_LABEL];
   /*---(preprare)-----------------------*/
   strcpy  (unit_answer, "s_file           : question not understood");
   /*---(selection)----------------------*/
   if      (strcmp (a_question, "ver_num"   )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file ver_num   : %s", ver_num);
   } else if (strcmp (a_question, "version"   )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file version   : %c %-4.4s %s", ver_ctrl, ver_num, ver_txt);
   } else if (strcmp (a_question, "recd"      )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file recd      : %s", my.f_recd);
   } else if (strcmp (a_question, "freeze"    )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file freeze    : col=%c (%4d to %4d)   row=%c (%4d to %4d)",
            FR_COL, FR_BCOL, FR_ECOL, FR_ROW, FR_BROW, FR_EROW);
   } else if (strcmp (a_question, "tab_name"  )    == 0) {
      LOC_tab_name (a_ref, x_name);
      snprintf (unit_answer, LEN_UNIT, "s_file tab name  : tab=%4d, act=%c, :%s:", a_ref, 'y', x_name);
   } else if (strcmp (a_question, "tab_count" )    == 0) {
      snprintf (unit_answer, LEN_UNIT, "s_file tab count : ntab=%4d", MAX_TABS);
   } else if (strcmp (a_question, "history"   )    == 0) {
      if      (nhist == 0    )  snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, n/a", nhist, chist);
      if      (chist <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, n/a", nhist, chist);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file history   : n=%4d, c=%4d, %s" , nhist, chist, hist[chist].act);
   } else if (strcmp (a_question, "entry"     )    == 0) {
      if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d too small", a_ref);
      else if (a_ref >= nhist)  snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d too large", a_ref);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file entry     : %4d, t=%4d, c=%4d, r=%4d, %s", a_ref, hist[a_ref].btab, hist[a_ref].bcol, hist[a_ref].brow, hist[a_ref].act);
   } else if (strcmp (a_question, "before"    )    == 0) {
      if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d too small", a_ref);
      else if (a_ref >= nhist)  snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d too large", a_ref);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file before    : %4d :%s:", a_ref, hist[a_ref].before);
   } else if (strcmp (a_question, "after"     )    == 0) {
      if      (a_ref <  0    )  snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d too small", a_ref);
      else if (a_ref >= nhist)  snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d too large", a_ref);
      else                      snprintf (unit_answer, LEN_UNIT, "s_file after     : %4d :%s:", a_ref, hist[a_ref].after);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
