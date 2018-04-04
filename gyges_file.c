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

char
TABS_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -11;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   int         x_size      =    0;
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
   /*---(resize)-------------------------*/
   x_col   = atoi (d);
   if (x_col == 0)  x_col = LOC_col_max (x_tab);
   DEBUG_INPT   yLOG_value   ("x_col"     , x_col);
   x_row   = atoi (f);
   if (x_row == 0)  x_row = LOC_row_max (x_tab);
   DEBUG_INPT   yLOG_value   ("x_row"     , x_row);
   rc = LOC_legal  (x_tab, x_col - 1, x_row - 1, CELL_EXACT);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_INPT  yLOG_value   ("col_max"   , LOC_col_max (x_tab));
   DEBUG_INPT  yLOG_value   ("row_max"   , LOC_row_max (x_tab));
   /*---(check name)---------------------*/
   DEBUG_INPT  yLOG_point   ("name"      , b);
   if (b != NULL && strllen (b, LEN_LABEL) > 0) {
      rc = LOC_tab_rename     (x_tab, b);
      DEBUG_INPT  yLOG_value   ("rename"    , rc);
      --rce;  if (rc < 0)  {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      DEBUG_INPT  yLOG_info    ("a_name"    , b);
   }
   /*---(default sizes)------------------*/
   x_size  = atoi (g);
   if (x_size > 0)  LOC_col_defwidth  (x_tab, x_size);
   x_size  = atoi (h);
   if (x_size > 0)  LOC_row_defheight (x_tab, x_size);
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
TABS_writer           (char  a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   char        c           =    0;
   int         x_btab      =  '-';
   int         x_etab      =  '-';
   char        x_type      =  '-';
   int         x_cols      =    0;
   int         x_rows      =    0;
   char        x_name      [LEN_LABEL];
   int         x_zero      =    0;
   int         x_wide      =    0;
   int         x_tall      =    0;
   /*---(prepare tab)--------------------*/
   yVIKEYS_unit_reset ();
   if (a_tab == -1) {
      x_btab = 0;
      x_etab = MAX_TABS - 1;
   } else {
      rc = LOC_tab_valid (a_tab);
      if (rc < 0)  return rce;
      x_btab = x_etab = a_tab;
   }
   /*---(find marked entries)------------*/
   for (i = x_btab; i <= x_etab; ++i) {
      if (LOC_tab_used (i) <= 0)  continue;
      x_type = LOC_tab_type (i);
      x_cols = LOC_col_max  (i);
      x_rows = LOC_row_max  (i);
      x_wide = LOC_tab_colwide (i);
      x_tall = LOC_tab_rowtall (i);
      LOC_tab_name    (i, x_name);
      yVIKEYS_file_write (FILE_TABS, &i, x_name, &x_zero, &x_cols, &x_zero, &x_rows, &x_wide, &x_tall, &x_type);
      ++c;
   }
   /*---(complete)-----------------------*/
   return c;
}

char
COLS_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_size      =    0;
   int         x_count     =    0;
   int         x_cols      =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'D') {
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
   rc = LOC_legal  (x_tab, x_col, 0, CELL_GROW);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update size)--------------*/
   x_size   = atoi (c);
   x_count  = atoi (d);
   if (x_count == 0)  x_count = 1;
   for (x_cols = 0; x_cols < x_count; ++x_cols) {
      rc = LOC_col_widen (x_tab, x_col + x_cols, x_size);
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

char         /*-> tbd --------------------------------[ ------ [ge.732.124.21]*/ /*-[02.0000.01#.#]-*/ /*-[--.---.---.--]-*/
DETAIL_writer           (char a_type, char a_tab, short a_cur)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        c           =    0;
   int         n           =    0;
   int         i           =    0;
   int         x_btab      =  '-';
   int         x_etab      =  '-';
   int         j           =    0;
   int         x_beg       =  '-';
   int         x_end       =  '-';
   int         x_max       =  '-';
   int         k           =    0;
   int         x_def       =    0;
   int         x_size      =    0;
   int         x_prev      =    0;
   /*---(header)-------------------------*/
   DEBUG_OUTP   yLOG_enter   (__FUNCTION__);
   /*---(clear output)-------------------*/
   yVIKEYS_unit_reset ();
   /*---(prepare tab)--------------------*/
   DEBUG_OUTP   yLOG_value   ("a_tab"     , a_tab);
   --rce;  if (a_tab == -1) {
      x_btab = 0;
      x_etab = MAX_TABS - 1;
   } else {
      rc = LOC_tab_valid (a_tab);
      if (rc < 0) { 
         DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_btab = x_etab = a_tab;
   }
   DEBUG_OUTP   yLOG_value   ("x_btab"    , x_btab);
   DEBUG_OUTP   yLOG_value   ("x_etab"    , x_etab);
   /*---(prepare col)--------------------*/
   DEBUG_OUTP   yLOG_value   ("a_cur"     , a_cur);
   --rce;  if (a_cur == -1) {
      x_beg = 0;
      if (a_type == 'c')  x_end = MAX_COLS - 1;
      else                x_end = MAX_ROWS - 1;
   } else {
      if (a_type == 'c')  rc = LOC_col_valid (a_tab, a_cur);
      else                rc = LOC_row_valid (a_tab, a_cur);
      if (rc < 0) {
         DEBUG_OUTP   yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      x_beg = x_end = a_cur;
   }
   DEBUG_OUTP   yLOG_value   ("x_beg"     , x_beg);
   DEBUG_OUTP   yLOG_value   ("x_end"     , x_end);
   /*---(find marked entries)------------*/
   for (i = x_btab; i <= x_etab; ++i) {
      DEBUG_OUTP   yLOG_value   ("tab (i)"   , i);
      if (LOC_tab_used (i) <= 0)  continue;
      if (a_type == 'c')  x_def  = LOC_tab_colwide (i);
      else                x_def  = LOC_tab_rowtall (i);
      DEBUG_OUTP   yLOG_value   ("x_def"     , x_def);
      for (j = x_beg; j <= x_end; ++j) {
         DEBUG_OUTP   yLOG_value   ("col (j)"   , j);
         if (LOC_col_valid (i, j) < 0)  continue;
         if      (a_type == 'c' && j > 0)  x_prev = LOC_col_width  (i, j - 1);
         else if (a_type == 'r' && j > 0)  x_prev = LOC_row_height (i, j - 1);
         else                              x_prev = -1;
         DEBUG_OUTP   yLOG_value   ("x_prev"    , x_prev);
         if (a_type == 'c')  x_size = LOC_col_width  (i, j);
         else                x_size = LOC_row_height (i, j);
         DEBUG_OUTP   yLOG_value   ("x_size"    , x_size);
         if (x_size == x_prev)  continue;
         if (x_size == x_def )  continue;
         n = 1;
         if (a_type == 'c')  x_max = LOC_col_max (i) - 1;
         else                x_max = LOC_row_max (i) - 1;
         for (k = j + 1; k <= x_max; ++k) {
            if (a_type == 'c' && x_size != LOC_col_width  (i, k))  break;
            if (a_type == 'r' && x_size != LOC_row_height (i, k))  break;
            ++n;
         }
         DEBUG_OUTP   yLOG_value   ("n"         , n);
         if (a_type == 'c')  yVIKEYS_file_write (FILE_COLS, &i, &j, &x_size, &n, NULL, NULL, NULL, NULL, NULL);
         else                yVIKEYS_file_write (FILE_ROWS, &i, &j, &x_size, &n, NULL, NULL, NULL, NULL, NULL);
         c += n;
         j += n - 1;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_OUTP  yLOG_exit    (__FUNCTION__);
   return c;
}

char
ROWS_reader          (char n, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_tab       =    0;
   int         x_row       =    0;
   int         x_size      =    0;
   int         x_count     =    0;
   int         x_rows      =    0;
   /*---(header)-------------------------*/
   DEBUG_INPT   yLOG_enter   (__FUNCTION__);
   /*---(check version)------------------*/
   DEBUG_INPT   yLOG_char    ("version"   , n);
   --rce;  if (n != 'D') {
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
   /*---(check row)----------------------*/
   x_row   = atoi (b);
   DEBUG_INPT   yLOG_value   ("x_row"     , x_row);
   rc = LOC_legal  (x_tab, 0, x_row, CELL_GROW);
   DEBUG_INPT   yLOG_value   ("rc"        , rc);
   --rce;  if (rc < 0) {
      DEBUG_INPT   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(update size)--------------*/
   x_size   = atoi (c);
   x_count  = atoi (d);
   if (x_count == 0)  x_count = 1;
   for (x_rows = 0; x_rows < x_count; ++x_rows) {
      rc = LOC_row_heighten (x_tab, x_row + x_rows, x_size);
      DEBUG_INPT  yLOG_value   ("heigten"   , rc);
      --rce;  if (rc < 0) {
         DEBUG_INPT  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_INPT  yLOG_exit    (__FUNCTION__);
   return 0;
}

char TABS_writer_all (void) { return TABS_writer    (-1);          }
char COLS_writer_all (void) { return DETAIL_writer  ('c', -1, -1); }
char ROWS_writer_all (void) { return DETAIL_writer  ('r', -1, -1); }




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
