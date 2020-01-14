/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char ROW__new        (tNODE **a_new, tTAB *a_tab, ushort a_row) { return NODE_new (a_new, a_tab, IS_ROW, a_row); }

char ROW__free       (tNODE **a_old) { return NODE_free (a_old); }



/*====================------------------------------------====================*/
/*===----                       search and finding                     ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char ROW__by_cursor  (tNODE **a_found, tTAB *a_tab, char a_move) { return NODE_by_cursor (a_found, a_tab, IS_ROW, a_move); }

char ROW__by_index   (tNODE **a_found, tTAB *a_tab, ushort a_row) { return NODE_by_index (a_found, a_tab, IS_ROW, a_row); }

char ROW__ensure     (tNODE **a_found, tTAB *a_tab, ushort a_row) { return NODE_ensure (a_found, a_tab, IS_ROW, a_row); }



/*====================------------------------------------====================*/
/*===----                  hooking and unhooking cells                 ----===*/
/*====================------------------------------------====================*/
static void  o___HOOKING_________o () { return; }

char ROW_hook_cell   (tTAB *a_tab, ushort a_col, ushort a_row, tCELL *a_cell) { return NODE_hook_cell (a_tab, IS_ROW, a_row, a_col, a_cell); }

char ROW_unhook_cell (tCELL *a_cell) { return NODE_unhook_cell (IS_ROW, a_cell); }



/*====================------------------------------------====================*/
/*===----                     usage statistics                         ----===*/
/*====================------------------------------------====================*/
static void  o___USAGE___________o () { return; }

short ROW_used       (char a_index, short a_ref) { return NODE_used (a_index, IS_ROW, a_ref); }

short ROW_min_used   (char a_index) { return NODE_min_used        (a_index, IS_ROW); }

short ROW_max_used   (char a_index) { return NODE_max_used        (a_index, IS_ROW); }

short ROW_max        (char a_index) { return NODE_max             (a_index, IS_ROW); }

char  ROW_max_set    (char a_index, short a_max) { return NODE_max_set         (a_index, IS_ROW, a_max); }

char  ROW_max_adjust (char a_index) { return NODE_max_adjust      (a_index, IS_ROW); }



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char ROW_cleanse     (tTAB *a_tab) { return NODE_cleanse (a_tab, IS_ROW); }

char ROW_cleanse_curr(void)        { return NODE_cleanse (PTAB, IS_ROW); }



/*====================------------------------------------====================*/
/*===----                   work with sizes                            ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char ROW_size        (char a_index, short a_ref) { return NODE_size (a_index, IS_ROW, a_ref); }

char ROW_resize      (char a_index, short a_ref, char a_size) { return NODE_resize (a_index, IS_ROW, a_ref, a_size); }

char ROW_reset       (char a_index, short a_ref) { return NODE_reset (a_index, IS_ROW, a_ref); }

char ROW_multisize   (char *a_label, char a_size, char a_count) { return NODE_multisize   (a_label, IS_ROW, a_size, a_count); }

char ROW_multikey    (char a_key) { return NODE_multikey (IS_ROW, a_key); }



/*====================------------------------------------====================*/
/*===----                     freezing col/row                         ----===*/
/*====================------------------------------------====================*/
static void  o___FREEZING________o () { return; }

char ROW_freeze      (char a_index, short a_brow, short a_erow) { return NODE_freeze (a_index, IS_ROW, a_brow, a_erow); }

char ROW_unfreeze    (char a_index) { return NODE_unfreeze (a_index, IS_ROW); }



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char ROW_reader      (void) { return NODE_reader (); }

char ROW_writer      (char a_index, short a_ref) { return NODE_writer_one (a_index, IS_ROW, a_ref); }

char ROW_writer_all  (void) { return NODE_writer_all (IS_ROW); }



/*====================------------------------------------====================*/
/*===----                    yvikeys screen mapping                    ----===*/
/*====================------------------------------------====================*/
static void  o___YVIKEYS_________o () { return; }

char ROW_map_clear   (void) { return NODE_map_clear (IS_ROW); }

char ROW_map_mapper  (void) { return NODE_map_mapper (IS_ROW); }

char ROW_map_absolute(void) { return NODE_map_absolute (IS_ROW); }

char ROW_map_local   (void) { return NODE_map_local (IS_ROW); }

char ROW_map_update  (void) { return NODE_map_update (IS_ROW); }



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char* ROW__unit      (char *a_question, uchar a_tab, ushort a_row) { NODE__unit (a_question, a_tab, IS_ROW, a_row); }

