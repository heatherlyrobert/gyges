/*============================----beg-of-source---============================*/
#include   "gyges.h"

/*
 * metis � wn2#� � col width changes not updating effected merged cell printables         � M2O126 �  1 �
 * metis � dn4#� � cut and paste is disabled                                              � M2O1DD �  2 �
 * metis � yv+�� � be able to delete cell and remove from all formulas that require it    � M2PMDa �  � �
 * metis � yv+�� � add move option that copies, pastes, then clears original              � M2PNVU �  � �
 *
 * metis � dn2#� � interactive cut and paste does not update formulas (#REF)              � M2R63C �  1 �
 * metis � wv4#� � remove flickering when screen is redrawn                               � M2S52Y �  1 �
 *
 * metis � dn8#� � delete-right (dl) does not work when last row/col has contents         � M2T3Ht �  1 �
 * metis � mc4�� � pastes/deletes/appends should check available space before allow       � M3047q �  � �
 *
 * metis � ����� � undo/redo config setting that does not reposition afterward            � M3A4FJ �  � �
 *
 * metis � wg4�� � repeat allowed on x� and d� if no range highlighted                    � M3L0X6 �  � �
 * metis � wg4�� � repeat on x� and d� with range causes keylogging warning               � M3L0Y0 �  � �
 *
 */

/*
 *  complex copy/paste scenario...
 *  -- INNER has all four absolute/relative, provides/requires combos
 *  -- OUTER has all four absolute/relative, provides/requires combos
 *  -- first, INNER is copied
 *  -- then, pasted on top of SQUASH area
 *  -- INNER connections must apporpriately stay intack
 *  -- SQUASH areas not overwritten must still work (0g8 � 0d7 � 0a6)
 *  -- all OUTER connections must be appropriately updated
 *
 *
 *   legend   � source    � destination
 *
 *  ORIGINAL ====================================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                  abs req ����                 OUTER
 *                  ����������������������INNER��
 *  2               �  rel proπ� ����rel req   �
 *               ��������������ϛ ��            �
 *  3  rel req ���  �          crucial ��������������� rel pro
 *                 ��������������  � ��������������
 *  4              ��  abs proπ�  ���abs req   � �
 *                 ������������������������������ � 
 *  5              ������������������������������ ���� abs pro
 *                 �������������������SQUASH�����
 *  6  rel req ��� ������������������������������ ���� future
 *               � ������������������������������
 *  7            � �������������safe�������������
 *               � ��������������ϲ��������������
 *  8            � ��������������� �              ���� rel pro
 *               ����������������� ����������������
 *  9                             �
 *                                �
 * 10                          future
 *
 *
 *
 *  COPY/NORMAL (confirmed) =====================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                  abs req ����                 OUTER
 *                  ����������������������INNER��
 *  2               �  rel proπ� ����rel req   �
 *               ��������������ϛ ��            �
 *  3  rel req ���  �          crucial ��������������� rel pro
 *                 ��������������  � ��������������
 *  4              ��  abs proπ�� ���abs req   � �
 *                 ������������������������������ �
 *  5              ����rel proπ������rel req���� ���� abs pro
 *                 ����������������Ϸ��������������
 *  6  rel req ��� ������������CRUCIAL ��������������� rel pro
 *               � ������������������������������
 *  7            � ����abs pro��safe �abs req����
 *               � ��������������ϲ��������������
 *  8            � ��������������� �              ���� rel pro
 *               ����������������� ����������������
 *  9                �      
 *                   �      
 * 10                ���������� SAFE
 *
 *
 *
 *  COPY/REPLACE ================================================
 *
 *     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
 *
 *  1                  abs req ����                 OUTER
 *                  ����������������������INNER��
*  2               �  rel proπ� ����rel req   �
*               ��������������ϛ ��            �
*  3  rel req ���  �          crucial ��������������� rel pro
*                 ��������������  � ��������������
*  4              ��  abs proπ�� ���abs req   � �
*                 ������������������������������ �
*  5              ����rel proπ������rel req���� ���� abs pro
*                 ����������������Ϸ��������������
*  6    ERROR ��� ������������CRUCIAL ��������������� rel pro
*               � ������������������������������
*  7            � ����abs pro��XXXX �abs req����
*               � ��������������ϲ��������������
*  8            � ���������������                   � rel pro
*               �����������������                 
*  9                �      
*                   �      
* 10                ���������� SAFE
*
*
*
*  CUT/NORMAL (confirmed) ======================================
*
*     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
*
*  1                    ERROR ����                 OUTER
*                  ����������������������INNER��
*  2               �    BLANK�   �  �BLANK     �
*               ���������������  �             �
*  3    ERROR ���  �          XXXXXXX          �    � rel pro
*                  �              �            �  
*  4               �    BLANKπ�� ���BLANK     �  
*                  �����������������������������  
*  5               ���rel proπ������rel req���� ���� abs pro
*                  ���������������Ϸ��������������
*  6  rel req ���  ����������� ERROR  ��������������� rel pro
*               �  �����������������������������
*  7            �  ���abs pro��safe �  ERROR����
*               �  �������������ϲ��������������
*  8            �   �           � �              ���� rel pro
*               ����������������� ����������������
*  9                �      
*                   �      
* 10                ���������� SAFE
*
*
*
*  CUT/MOVE ====================================================
*     (ybR-) clear, adjust all copied formulas, adjust relative in providers
*
*     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
*
*  1                    ERROR ����                 OUTER
*                  ����������������������INNER��
*  2               �             �             �
*                  �             �             �
*  3  rel req ���� �          XXXXXXX          �    � rel pro
*                � �                           �  
*  4             � �                           �  
*                � �����������������������������  
*  5             � ���rel proπ������rel req����
*                ������������Ϸ���Ϸ������������  
*  6    ERROR ���  �����������CRUCIAL ��������������� rel pro
*               �  ���������������π��������������
*  7            �  ���abs proϷ�XXX��abs req���� ���� abs pro
*               �  �������������ϲ��������������
*  8            �   �           �                   � rel pro
*               �����������������                 
*  9                �      
*                   �      
* 10                ���������� SAFE
*
*
*
*
*  CUT/FORCE ===================================================
*     (ybA-) clear, adjust all copied formulas, adjust all providers
*
*     ---a--- ---b--- ---c--- ---d--- ---e--- ---f--- ---g---
*
*  1                  abs pro ����                 OUTER
*                  ����������������������INNER��
*  2               �             �             �
*                  �             �             �
*  3  rel req ���� �             �             �    � rel pro
*                � �             �             �  
*  4             � �             �             �  
*                � �����������������������������  
*  5             � ���rel proπ������rel req����
*                ������������Ϸ���Ϸ������������  
*  6    ERROR ���  �����������CRUCIAL ��������������� rel pro
*               �  ���������������π��������������
*  7            �  ���abs proϷ�XXX��abs req���� ���� abs pro
*               �  �������������ϲ��������������
*  8            �   �           �                   � rel pro
*               �����������������                 
*  9                �      
*                   �      
* 10                ���������� SAFE
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*/

struct cACCESSOR my;
char    g_contents      [LEN_RECD] = "";
char    message [LEN_RECD] = "";
char    special = 0;




char
api_vikeys_init         (void)
{
   /*> yVIKEYS_menu_add ("�ePn", "normal"    , "Pn");                                 <* 
    *> yVIKEYS_menu_add ("�eP#", "clear"     , "P#");                                 <* 
    *> yVIKEYS_menu_add ("�ePr", "replace"   , "Pr");                                 <* 
    *> yVIKEYS_menu_add ("�ePd", "dup"       , "Pd");                                 <* 
    *> yVIKEYS_menu_add ("�ePc", "combo"     , "Pc");                                 <* 
    *> yVIKEYS_menu_add ("�ePm", "move"      , "Pm");                                 <* 
    *> yVIKEYS_menu_add ("�ePf", "force"     , "Pf");                                 <* 
    *> yVIKEYS_menu_add ("�ePv", "values"    , "Pv");                                 <*/
   return 0;
}

char
api_yvikeys_handlers    (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_FILE   yLOG_enter   (__FUNCTION__);
   /*---(handlers)-----------------------*/
   rc = yPARSE_handler_max (FILE_TABS    , "tab"       , 4.1, "LTcTTc------", -1, TAB_reader      , TAB_writer_all  , "------------" , "name,max,t,beg,cur,c"      , "gyges tabs (universe)"    );
   DEBUG_FILE   yLOG_value   ("tab"       , rc);
   rc = yPARSE_handler_max (FILE_COLS    , "width"     , 4.2, "Tss---------", -1, COL_reader      , COL_writer_all  , "------------" , "label,wid,cnt"             , "gyges cols (x-axis)"      );
   DEBUG_FILE   yLOG_value   ("width"     , rc);
   rc = yPARSE_handler_max (FILE_ROWS    , "height"    , 4.3, "Tss---------", -1, ROW_reader      , ROW_writer_all  , "------------" , "label,tal,cnt"             , "gyges rows (y-axis)"      );
   DEBUG_FILE   yLOG_value   ("height"    , rc);
   rc = yPARSE_handler_max (FILE_DEPCEL  , "cell_dep"  , 5.1, "TTO---------", -1, CELL_reader     , CELL_writer_all , "------------" , "label,afdu?,contents"      , "gyges dependent cells"    );
   DEBUG_FILE   yLOG_value   ("cell_dep"  , rc);
   rc = yPARSE_handler_max (FILE_FREECEL , "cell_free" , 5.2, "TTO---------", -1, CELL_reader     , NULL            , "------------" , "label,afdu?,contents"      , "gyges free cells"         );
   DEBUG_FILE   yLOG_value   ("cell_free" , rc);
   /*---(complete)-----------------------*/
   DEBUG_FILE   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                   support for map mode                       ----===*/
/*====================------------------------------------====================*/
static void   o___MAP_____________o (void) { return; }



/*====================------------------------------------====================*/
/*===----                    source mode                               ----===*/
/*====================------------------------------------====================*/
static void   o___SOURCE__________o (void) { return; }

char
api_yvikeys_saver       (char *a_contents)
{
   CELL_change  (NULL, YMAP_BEG, CTAB, CCOL, CROW, a_contents);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     data export/import                       ----===*/
/*====================------------------------------------====================*/
static void   o___EXIM____________o (void) { return; }

char         /*-> keys for formatting sub-mode -------[ ------ [gc.MT0.202.C7]*/ /*-[01.0000.112.!]-*/ /*-[--.---.---.--]-*/
api_yvikeys_exim        (char a_dir, char a_style)
{
   char        rc          =    0;
   /*---(header)--------------------s----*/
   DEBUG_REGS   yLOG_enter   (__FUNCTION__);
   DEBUG_REGS   yLOG_char    ("a_dir"     , a_dir);
   DEBUG_REGS   yLOG_char    ("a_style"   , a_style);
   switch (a_dir) {
   case '+' :
      DEBUG_REGS   yLOG_note    ("call importer (+)");
      rc = EXIM_import (a_style);
      break;
   case '-' :
      DEBUG_REGS   yLOG_note    ("call exporter (-)");
      rc = EXIM_export (a_style);
      break;
   }
   DEBUG_REGS   yLOG_value   ("rc"        , rc);
   DEBUG_REGS   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        searching                             ----===*/
/*====================------------------------------------====================*/
PRIV void  o___SEARCH__________o () { return; }

char         /*-> tbd --------------------------------[ ------ [ge.#M5.1C#.#7]*/ /*-[03.0000.013.L]-*/ /*-[--.---.---.--]-*/
api_yvikeys_searcher      (uchar a_not, uchar *a_search)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   tCELL      *x_next      = NULL;
   int         x_tab       =    0;
   int         x_col       =    0;
   int         x_row       =    0;
   /*---(header)--------------------s----*/
   DEBUG_YMARK   yLOG_enter   (__FUNCTION__);
   DEBUG_YMARK   yLOG_point   ("a_search"  , a_search);
   /*---(defenses)---------------------------*/
   --rce;  if (a_search == NULL) {
      DEBUG_YMARK   yLOG_note    ("can not use null search");
      DEBUG_YMARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YMARK   yLOG_info    ("a_search"  , a_search);
   x_len = strlen (a_search);
   DEBUG_YMARK   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len <= 0) {
      DEBUG_YMARK   yLOG_note    ("can be an empty search");
      DEBUG_YMARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (a_search [0] != '/') {
      DEBUG_YMARK   yLOG_note    ("must start with a forward slash");
      DEBUG_YMARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   if (x_len == 1) {
      DEBUG_YMARK   yLOG_note    ("nothing to do, redraw only");
      DEBUG_YMARK   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   rc = yREGEX_comp (a_search + 1);
   DEBUG_YMARK   yLOG_value   ("comp rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YMARK   yLOG_note    ("could not compile search");
      DEBUG_YMARK   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(process range)----------------------*/
   DEBUG_CELL   yLOG_point   ("hcell"     , hcell);
   x_next = hcell;
   do {
      DEBUG_YMARK   yLOG_complex ("x_next"    , "ptr %p, tab %2d, col %3d, row %4d", x_next, x_tab, x_col, x_row);
      if (x_next != NULL && x_next->source != NULL) {
         DEBUG_YMARK   yLOG_char    ("->type"    , x_next->type);
         switch (x_next->type) {
         case YCALC_DATA_STR   :
            DEBUG_YMARK   yLOG_info    ("->source"       , x_next->source);
            rc = yREGEX_filter (x_next->source);
            break;
         case YCALC_DATA_SFORM :
         case YCALC_DATA_SLIKE :
            DEBUG_YMARK   yLOG_info    ("->v_str"   , x_next->v_str);
            rc = yREGEX_filter (x_next->v_str);
            break;
         default          :
            DEBUG_YMARK   yLOG_note    ("can not process cell type");
            rc = -1;
            break;
         }
         DEBUG_YMARK   yLOG_value   ("exec rc"   , rc);
         if (a_not != 'y' && rc > 0) {
            yMARK_found (x_next->label, x_next->tab, x_next->col, x_next->row, 0);
            x_next->note = 's';
         } else if (a_not == 'y' && rc <= 0) {
            yMARK_found (x_next->label, x_next->tab, x_next->col, x_next->row, 0);
            x_next->note = 's';
         }
      }
      x_next = x_next->m_next;
   } while (x_next != NULL && x_next != DONE_DONE);
   /*---(complete)---------------------------*/
   DEBUG_YMARK   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
api_yvikeys_unsearcher   (uchar *a_label, ushort u, ushort x, ushort y, ushort z)
{
   /*---(locals)-----------+------+----+-*/
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YMARK   yLOG_enter   (__FUNCTION__);
   DEBUG_YMARK   yLOG_value   ("u"         , u);
   DEBUG_YMARK   yLOG_value   ("x"         , x);
   DEBUG_YMARK   yLOG_value   ("y"         , y);
   DEBUG_YMARK   yLOG_value   ("z"         , z);
   x_curr = LOC_cell_at_loc (u, x, y);
   DEBUG_YMARK   yLOG_point   ("x_curr"    , x_curr);
   if (x_curr != NULL) x_curr->note = '-';
   DEBUG_YMARK   yLOG_char    ("x_curr->n" , x_curr->note);
   /*---(complete)---------------------------*/
   DEBUG_YMARK   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                     mapping for map mode                     ----===*/
/*====================------------------------------------====================*/
static void  o___MAPPING_________o () { return; }

/*> char                                                                               <* 
 *> MAP__clear            (tMAPPED *a_map)                                             <* 
 *> {                                                                                  <* 
 *>    /+---(locals)-----------+-----------+-+/                                        <* 
 *>    int         i           =    0;                                                 <* 
 *>    /+---(lefts)--------------------------+/                                        <* 
 *>    a_map->umin = a_map->gmin = a_map->gamin = a_map->glmin = a_map->gprev = -1;    <* 
 *>    /+---(map)----------------------------+/                                        <* 
 *>    for (i= 0; i < LEN_HUGE; ++i)  a_map->map [i] =  YVIKEYS_EMPTY;                 <* 
 *>    /+---(rights)-------------------------+/                                        <* 
 *>    a_map->umax = a_map->gmax = a_map->gamax = a_map->glmax = a_map->gnext = -1;    <* 
 *>    /+---(screen)-------------------------+/                                        <* 
 *>    a_map->ubeg  = a_map->ucur  = a_map->uend  = a_map->ulen  = a_map->utend = 0;   <* 
 *>    /+---(grids)--------------------------+/                                        <* 
 *>    a_map->gbeg = a_map->gcur = a_map->gend = 0;                                    <* 
 *>    /+---(complete)-----------------------+/                                        <* 
 *>    return  0;                                                                      <* 
 *> }                                                                                  <*/

char
MAP_rows             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_enter   (__FUNCTION__);
   /*---(rows)---------------------------*/
   rc = yMAP_axis_grid (YMAP_YAXIS, &b, &c, &e, NULL);
   DEBUG_YMAP    yLOG_complex ("ROWS"      , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
   if (CROW == c) {
      DEBUG_YMAP    yLOG_note    ("row did not change, do not remap");
   } else {
      DEBUG_YMAP    yLOG_note    ("row change, remap");
      COL_mapper (0);
   }
   BROW   = b;
   CROW   = c;
   EROW   = e;
   DEBUG_YMAP    yLOG_complex ("ROWS+"     , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
   /*---(ocmplete)-----------------------*/
   DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
MAP_columns          (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_enter   (__FUNCTION__);
   /*---(columns)------------------------*/
   rc = yMAP_axis_grid (YMAP_XAXIS, &b, &c, &e, NULL);
   DEBUG_YMAP    yLOG_complex ("COLS"      , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
   if (CCOL == c) {
      DEBUG_YMAP    yLOG_note    ("column did not change, do not remap");
   } else {
      DEBUG_YMAP    yLOG_note    ("column change, remap");
      ROW_mapper (0);
   }
   BCOL   = b;
   CCOL   = c;
   ECOL   = e;
   DEBUG_YMAP    yLOG_complex ("COLS+"     , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
   /*---(ocmplete)-----------------------*/
   DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
MAP_universe         (char a_req)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_enter   (__FUNCTION__);
   /*---(tabs)---------------------------*/
   rc = yMAP_axis_grid (YMAP_UNIV , &b, &c, &e, NULL);
   DEBUG_YMAP    yLOG_complex ("TABS"      , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", NTAB, 0   , CTAB, 0   , b, c, e);
   if (CTAB != c || a_req == YMAP_UNIVERSE) {
      DEBUG_YMAP    yLOG_note    ("tab change, force remap");
      TAB_mapper ();
   } else if (a_req == YMAP_UNIVDISP) {
      DEBUG_YMAP    yLOG_note    ("UNIVDISP, tab did not change, but forcing remap");
      TAB_mapper ();
   } else {
      DEBUG_YMAP    yLOG_note    ("tab did not change, do not remap");
   }
   DEBUG_YMAP    yLOG_complex ("TABS+"     , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", NTAB, 0   , CTAB, 0   , b, c, e);
   /*---(ocmplete)-----------------------*/
   DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char                                                                                                                         <* 
 *> MAP_display_only     (void)                                                                                                  <* 
 *> {                                                                                                                            <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                                                                  <* 
 *>    char        rc          =    0;                                                                                           <* 
 *>    ushort      b, c, e;                                                                                                      <* 
 *>    /+---(header)-------------------------+/                                                                                  <* 
 *>    DEBUG_YMAP    yLOG_enter   (__FUNCTION__);                                                                                 <* 
 *>    /+---(columns)------------------------+/                                                                                  <* 
 *>    yMAP_axis_grid (YMAP_XAXIS, &b, &c, &e, NULL);                                                                            <* 
 *>    BCOL   = b;                                                                                                               <* 
 *>    CCOL   = c;                                                                                                               <* 
 *>    ECOL   = e;                                                                                                               <* 
 *>    DEBUG_YMAP    yLOG_complex ("columns"   , "%4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", NCOL, BCOL, CCOL, ECOL, b, c, e);   <* 
 *>    /+---(rows)---------------------------+/                                                                                  <* 
 *>    yMAP_axis_grid (YMAP_YAXIS, &b, &c, &e, NULL);                                                                            <* 
 *>    BROW   = b;                                                                                                               <* 
 *>    CROW   = c;                                                                                                               <* 
 *>    EROW   = e;                                                                                                               <* 
 *>    DEBUG_YMAP    yLOG_complex ("rows"      , "%4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", NROW, BROW, CROW, EROW, b, c, e);   <* 
 *>    /+---(ocmplete)-----------------------+/                                                                                  <* 
 *>    DEBUG_YMAP    yLOG_exit    (__FUNCTION__);                                                                                 <* 
 *>    return 0;                                                                                                                 <* 
 *> }                                                                                                                            <*/


char
api_yvikeys_axis_get    (char a_axis, ushort *n, ushort *b, ushort *c, ushort *e)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_senter  (__FUNCTION__);
   DEBUG_YMAP    yLOG_schar   (a_axis);
   /*---(defaults)-----------------------*/
   DEBUG_YMAP    yLOG_snote   ("default");
   if (n != NULL)  *n = 0;
   if (b != NULL)  *b = 0;
   if (c != NULL)  *c = 0;
   if (e != NULL)  *e = 0;
   /*---(axis)---------------------------*/
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_YMAP    yLOG_snote   ("u-axis is tabs");
      if (n != NULL)  *n = NTAB;
      if (b != NULL)  *b = 0;
      if (c != NULL)  *c = CTAB;
      if (e != NULL)  *e = NTAB - 1;
      break;
   case YMAP_XAXIS :
      DEBUG_YMAP    yLOG_snote   ("x-axis is cols");
      if (n != NULL)  *n = NCOL;
      if (b != NULL)  *b = BCOL;
      if (c != NULL)  *c = CTAB;
      if (e != NULL)  *e = ECOL;
      break;
   case YMAP_YAXIS :
      DEBUG_YMAP    yLOG_snote   ("y-axis is rows");
      if (n != NULL)  *n = NROW;
      if (b != NULL)  *b = BROW;
      if (c != NULL)  *c = CROW;
      if (e != NULL)  *e = EROW;
      break;
   case YMAP_ZAXIS :
      DEBUG_YMAP    yLOG_snote   ("z-axis, un-used in gyges");
      break;
   default  :
      DEBUG_YMAP    yLOG_snote   ("axis not understood");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_yvikeys_axis_set    (char a_axis, ushort b, ushort c, ushort e)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_senter  (__FUNCTION__);
   DEBUG_YMAP    yLOG_schar   (a_axis);
   /*---(axis)---------------------------*/
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_YMAP    yLOG_snote   ("u-axis is tabs");
      CTAB = c;
      break;
   case YMAP_XAXIS :
      DEBUG_YMAP    yLOG_snote   ("x-axis is cols");
      BCOL = b;
      CCOL = c;
      ECOL = e;
      break;
   case YMAP_YAXIS :
      DEBUG_YMAP    yLOG_snote   ("y-axis is rows");
      BROW = b;
      CROW = c;
      EROW = e;
      break;
   case YMAP_ZAXIS :
      DEBUG_YMAP    yLOG_snote   ("z-axis, un-used in gyges");
      break;
   default  :
      DEBUG_YMAP    yLOG_snote   ("axis not understood");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
api_yvikeys_axis_entry  (char a_axis, ushort a_pos, short *a_ref, uchar *a_size, uchar *a_used)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        c           =    0;
   tCELL      *x_curr      = NULL;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_senter  (__FUNCTION__);
   DEBUG_YMAP    yLOG_schar   (a_axis);
   /*---(defaults)-----------------------*/
   DEBUG_YMAP    yLOG_snote   ("default");
   if (a_ref  != NULL)  *a_ref  = 0;
   if (a_size != NULL)  *a_size = 0;
   if (a_used != NULL)  *a_used = 0;
   /*---(axis)---------------------------*/
   --rce;  switch (a_axis) {
   case YMAP_UNIV  :
      DEBUG_YMAP    yLOG_snote   ("u-axis is tabs");
      /*---(illegal)---------------------*/
      if (VALID_tab (a_pos) != 1) {
         DEBUG_YMAP    yLOG_snote   ("not legal");
         DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(basics)----------------------*/
      if (a_ref  != NULL)  *a_ref  = YSTR_UNIV [a_pos];
      if (a_size != NULL)  *a_size = 1;
      /*---(usage)-----------------------*/
      if (a_used == NULL)  break;
      c = TAB_used (a_pos);
      if      (c <  0)   *a_used = YMAP_NADA;
      else if (c == 0)   *a_used = YMAP_PLACE;
      else               *a_used = YMAP_USED;
      /*---(done)------------------------*/
      break;
   case YMAP_XAXIS :
      DEBUG_YMAP    yLOG_snote   ("x-axis is cols");
      /*---(illegal)---------------------*/
      if (VALID_col (a_pos) != 1) {
         DEBUG_YMAP    yLOG_snote   ("not legal");
         DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(basics)----------------------*/
      if (a_ref  != NULL)  *a_ref  = a_pos;
      if (a_size != NULL)  *a_size = COL_size (CTAB, a_pos);
      if (a_used == NULL)  break;
      /*---(usage)-----------------------*/
      x_curr = LOC_cell_at_loc (CTAB, a_pos, CROW);
      if (x_curr == NULL)  *a_used = YMAP_NADA;
      else                 *a_used = YMAP_USED;
      /*---(done)------------------------*/
      break;
   case YMAP_YAXIS :
      DEBUG_YMAP    yLOG_snote   ("y-axis is rows");
      /*---(illegal)---------------------*/
      if (VALID_row (a_pos) != 1) {
         DEBUG_YMAP    yLOG_snote   ("not legal");
         DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
         return rce;
      }
      /*---(basics)----------------------*/
      if (a_ref  != NULL)  *a_ref  = a_pos;
      if (a_size != NULL)  *a_size = ROW_size (CTAB, a_pos);
      if (a_used == NULL)  break;
      /*---(usage)-----------------------*/
      x_curr = LOC_cell_at_loc (CTAB, CCOL, a_pos);
      if (x_curr == NULL)  *a_used = YMAP_NADA;
      else                 *a_used = YMAP_USED;
      /*---(done)------------------------*/
      break;
   case YMAP_ZAXIS :
      DEBUG_YMAP    yLOG_snote   ("z-axis, un-used in gyges");
      break;
   default  :
      DEBUG_YMAP    yLOG_snote   ("axis not understood");
      DEBUG_YMAP    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP    yLOG_sexit   (__FUNCTION__);
   return 0;
}

/*> char                                                                                 <* 
 *> NODE_map_update            (char a_type, char a_level)                               <* 
 *> {                                                                                    <* 
 *>    /+---(locals)-----------+-----+-----+-+/                                          <* 
 *>    char        rce         =  -10;                                                   <* 
 *>    char        rc          =    0;                                                   <* 
 *>    uchar       x_axis      =  '-';                                                   <* 
 *>    ushort      x_size      =    0;                                                   <* 
 *>    short       x_avail     =    0;                                                   <* 
 *>    int         i           =    0;                                                   <* 
 *>    tCELL      *x_curr      = NULL;                                                   <* 
 *>    tNODE      *x_node      = NULL;                                                   <* 
 *>    ushort      x_wide      =    0;                                                   <* 
 *>    /+---(header)-------------------------+/                                          <* 
 *>    DEBUG_YMAP    yLOG_enter   (__FUNCTION__);                                         <* 
 *>    DEBUG_YMAP    yLOG_char    ("a_type"    , a_type);                                 <* 
 *>    DEBUG_YMAP    yLOG_complex ("sizes"     , "%4dt, %4dc, %4dr", CTAB, NCOL, NROW);   <* 
 *>    /+---(defense)------------------------+/                                          <* 
 *>    --rce;  switch (a_type) {                                                         <* 
 *>    case 'c' :                                                                        <* 
 *>       x_axis = 'x';                                                                  <* 
 *>       x_size = NCOL;                                                                 <* 
 *>       break;                                                                         <* 
 *>    case 'r' :                                                                        <* 
 *>       x_axis = 'y';                                                                  <* 
 *>       x_size = NROW;                                                                 <* 
 *>       break;                                                                         <* 
 *>    default  :                                                                        <* 
 *>       DEBUG_YMAP    yLOG_exitr   (__FUNCTION__, rce);                                 <* 
 *>       return rce;                                                                    <* 
 *>       break;                                                                         <* 
 *>    }                                                                                 <* 
 *>    DEBUG_YMAP    yLOG_char    ("x_axis"    , x_axis);                                 <* 
 *>    DEBUG_YMAP    yLOG_value   ("x_size"    , x_size);                                 <* 
 *>    /+---(preparation)--------------------+/                                          <* 
 *>    rc = yMAP_clear  (x_axis);                                                        <* 
 *>    rc = yMAP_start  (x_axis, x_size);                                                <* 
 *>    IF_COL    yVIEW_size (YVIEW_MAIN, NULL, NULL, &x_avail, NULL, NULL);              <* 
 *>    ELSE_ROW  yVIEW_size (YVIEW_MAIN, NULL, NULL, NULL, NULL, &x_avail);              <* 
 *>    yMAP_axis_avail (x_axis, x_avail);                                                <* 
 *>    /+---(update grid)--------------------+/                                          <* 
 *>    for (i = 0; i < x_size; ++i) {                                                    <* 
 *>       /+---(defaults)--------------------+/                                          <* 
 *>       IF_COL    x_wide = DEF_WIDTH;                                                  <* 
 *>       ELSE_ROW  x_wide = DEF_HEIGHT;                                                 <* 
 *>       /+---(get cell)--------------------+/                                          <* 
 *>       IF_COL    x_curr = LOC_cell_at_loc (CTAB, i, CROW);                            <* 
 *>       ELSE_ROW  x_curr = LOC_cell_at_loc (CTAB, CCOL, i);                            <* 
 *>       /+---(handle used)-----------------+/                                          <* 
 *>       if (x_curr != NULL) {                                                          <* 
 *>          rc = NODE_by_index (&x_node, CTAB, a_type, i);                              <* 
 *>          if (x_node != NULL)   x_wide = x_node->size;                                <* 
 *>          rc = yMAP_append (i, x_wide, '�');                                          <* 
 *>       }                                                                              <* 
 *>       /+---(handle empty)----------------+/                                          <* 
 *>       else {                                                                         <* 
 *>          rc = yMAP_append (i, x_wide, '�');                                          <* 
 *>       }                                                                              <* 
 *>       /+---(done)------------------------+/                                          <* 
 *>    }                                                                                 <* 
 *>    /+---(update)-------------------------+/                                          <* 
 *>    rc = yMAP_finish ();                                                              <* 
 *>    /+---(complete)-----------------------+/                                          <* 
 *>    DEBUG_YMAP    yLOG_exit    (__FUNCTION__);                                         <* 
 *>    return rc;                                                                        <* 
 *> }                                                                                    <*/


char
MAP_mapper           (char a_req)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   tCELL      *x_curr      = NULL;
   char        t           [LEN_RECD];
   ushort      b, c, e;
   /*---(header)-------------------------*/
   DEBUG_YMAP    yLOG_enter   (__FUNCTION__);
   DEBUG_YMAP    yLOG_char    ("a_req"     , a_req);
   /*---(just display)---------*/
   if (a_req == YMAP_DISPONLY) {
      MAP_universe (a_req);
      MAP_columns ();
      MAP_rows    ();
      DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(universe change)------*/
   if (a_req == YMAP_UNIVERSE) {
      MAP_universe (a_req);
      ++CCOL;
      MAP_columns  ();
      ++CROW;
      MAP_rows     ();
      DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (a_req == YMAP_UNIVDISP) {
      MAP_universe (a_req);
   }
   /*---(initialization)-------*/
   /*> if (a_req == YMAP_INIT) {                                                      <* 
    *>    TAB_mapper       (a_req);                                                   <* 
    *>    COL_mapper       (a_req);                                                   <* 
    *>    ROW_mapper       (a_req);                                                   <* 
    *>    return 0;                                                                   <* 
    *> }                                                                              <*/
   /*---(everything)-----------*/
   if (a_req == YMAP_INIT) {
      DEBUG_YMAP    yLOG_note    ("full initialization");
      TAB_mapper       ();
      COL_mapper       (0);
      ROW_mapper       (0);
   }
   /*---(tab)------------------*/
   if (a_req == YMAP_INIT) {
      DEBUG_YMAP    yLOG_note    ("full initialization, map tab");
      TAB_mapper       ();
   } else {
      rc = yMAP_axis_grid (YMAP_UNIV , &b, &c, &e, NULL);
      DEBUG_YMAP    yLOG_complex ("TABS"      , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NTAB, 0, CTAB, 0, b, c, e);
      if (CTAB == c)  {
         DEBUG_YMAP    yLOG_note    ("tab did not change, do not remap");
      } else {
         DEBUG_YMAP    yLOG_note    ("tab change, remap");
         a_req = YMAP_INIT;
         TAB_mapper       ();
      }
   }
   /*---(col)------------------*/
   if (a_req == YMAP_INIT) {
      DEBUG_YMAP    yLOG_note    ("full initialization, map col");
      ROW_mapper       (0);
   } else {
      rc = yMAP_axis_grid (YMAP_XAXIS, &b, &c, &e, NULL);
      DEBUG_YMAP    yLOG_complex ("COLS"      , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
      if (CCOL == c) {
         DEBUG_YMAP    yLOG_note    ("col did not change, do not remap");
      } else {
         DEBUG_YMAP    yLOG_note    ("col change, remap");
         ROW_mapper (0);
      }
      BCOL   = b;
      CCOL   = c;
      ECOL   = e;
      DEBUG_YMAP    yLOG_complex ("COLS+"     , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NCOL, BCOL, CCOL, ECOL, b, c, e);
   }
   /*---(row)------------------*/
   if (a_req == YMAP_INIT) {
      DEBUG_YMAP    yLOG_note    ("full initialization, map row");
      COL_mapper       (0);
   } else {
      rc = yMAP_axis_grid (YMAP_YAXIS, &b, &c, &e, NULL);
      DEBUG_YMAP    yLOG_complex ("ROWS"      , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
      if (CROW == c) {
         DEBUG_YMAP    yLOG_note    ("row did not change, do not remap");
      } else {
         DEBUG_YMAP    yLOG_note    ("row change, remap");
         COL_mapper (0);
      }
      BROW   = b;
      CROW   = c;
      EROW   = e;
      DEBUG_YMAP    yLOG_complex ("ROWS+"     , "%4d � %4dn, %4db, %4dc, %4de � %4db, %4dc, %4de", rc, NROW, BROW, CROW, EROW, b, c, e);
   }
   /*---(get current)----------*/
   x_curr = LOC_cell_at_curr ();
   if  (yMODE_curr () == UMOD_WANDER) {
      ;;
   } else if (x_curr == NULL || x_curr->source == NULL) {
      DEBUG_YMAP    yLOG_complex ("CURRENT"   , "%4dt, %4dc, %4dr", CTAB, CCOL, CROW);
      ySRC_update (t, "�����", "");
      strlcpy (my.reqs_list, "n/a", LEN_RECD);
      strlcpy (my.deps_list, "n/a", LEN_RECD);
      strlcpy (my.like_list, "n/a", LEN_RECD);
      strlcpy (my.copy_list, "n/a", LEN_RECD);
      strlcpy (my.rpn_list , "n/a", LEN_RECD);
      strlcpy (g_curr.label, "-"          , LEN_LABEL);
      g_curr.len    = 0;
      g_curr.type   = YCALC_DATA_BLANK;
      g_curr.tab    = -1;
      g_curr.col    = -1;
      g_curr.row    = -1;
      g_curr.align  = '-';
      g_curr.format = '-';
      g_curr.decs   = '-';
      g_curr.unit   = '-';
   } else {
      sprintf (t, "%c%c%c%c%c", x_curr->type, x_curr->align, x_curr->format, x_curr->decs, x_curr->unit);
      ySRC_update (x_curr->label, t, x_curr->source);
      yCALC_disp_reqs (x_curr->ycalc, my.reqs_list);
      yCALC_disp_pros (x_curr->ycalc, my.deps_list);
      yCALC_disp_like (x_curr->ycalc, my.like_list);
      yCALC_disp_copy (x_curr->ycalc, my.copy_list);
      strlcpy (g_curr.label, x_curr->label, LEN_LABEL);
      g_curr.len    = x_curr->len;
      g_curr.type   = x_curr->type;
      g_curr.tab    = x_curr->tab;
      g_curr.col    = x_curr->col;
      g_curr.row    = x_curr->row;
      g_curr.align  = x_curr->align;
      g_curr.format = x_curr->format;
      g_curr.decs   = x_curr->decs;
      g_curr.unit   = x_curr->unit;
   }
   /*---(complete)-----------------------*/
   DEBUG_YMAP    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                    unit testing accessor                     ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;}

char*        /*-> unit test accessor -----------------[ leaf   [gs.930.201.A0]*/ /*-[01.0000.00#.#]-*/ /*-[--.---.---.--]-*/
api_yvikeys__unit      (char *a_question, int a_num)
{
   /*---(prepare)------------------------*/
   strcpy  (unit_answer, "s_move unit      : question not understood");
   /*---(defenses)-----------------------*/
   if      (a_num < 0) {
      snprintf (unit_answer, LEN_FULL, "s_move error     : tab ref (%d) is negative", a_num);
   }
   else if (a_num >= MAX_TABS) {
      snprintf (unit_answer, LEN_FULL, "s_move error     : tab ref (%d vs %d) is too high", a_num, MAX_TABS);
   }
   /*---(screen location)----------------*/
   else if (strcmp(a_question, "row_stats"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move row stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NROW, my.y_avail, BROW, CROW, EROW);
   }
   else if (strcmp(a_question, "col_stats"     ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move col stat  : num=%4d, scr=%4d, beg=%4d, cur=%4d, end=%4d", NCOL, my.x_avail, BCOL, CCOL, ECOL);
   }
   /*---(current position)---------------*/
   else if (strcmp(a_question, "tab_cur"       ) == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move tab cur   : tab=%4d, col=%4d, row=%4d", CTAB, CCOL, CROW);
   }
   /*> else if (strcmp(a_question, "tab_beg"       ) == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab beg   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].bcol, s_tabs [a_num].brow);   <* 
    *> }                                                                                                                                      <*/
   /*> else if (strcmp(a_question, "tab_pos"       ) == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab pos   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ccol, s_tabs [a_num].crow);   <* 
    *> }                                                                                                                                      <*/
   /*> else if (strcmp(a_question, "tab_max" )       == 0) {                                                                                  <* 
    *>    snprintf(unit_answer, LEN_FULL, "s_move tab max   : tab=%4d, col=%4d, row=%4d", a_num, s_tabs [a_num].ncol, s_tabs [a_num].nrow);   <* 
    *> }                                                                                                                                      <*/
   else if (strcmp(a_question, "edit_con")       == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move edit con  : :%-*.*s:", my.apos, my.apos, g_contents + my.bpos);
   }
   else if (strcmp(a_question, "edit_pos")       == 0) {
      snprintf(unit_answer, LEN_FULL, "s_move edit pos  : n=%3d, a=%3d, b=%3d, c=%3d, e=%3d", my.npos, my.apos, my.bpos, my.cpos, my.epos);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



