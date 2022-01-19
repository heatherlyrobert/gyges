/*============================----beg-of-source---============================*/
#include   "gyges.h"



/*====================------------------------------------====================*/
/*===----                       memory allocation                      ----===*/
/*====================------------------------------------====================*/
static void  o___MEMORY__________o () { return; }

char COL__new        (tNODE **a_new, tTAB *a_tab, ushort a_col) { return NODE_new (a_new, a_tab, IS_COL, a_col); }

char COL__free       (tNODE **a_old) { return NODE_free (a_old); }



/*====================------------------------------------====================*/
/*===----                       search and finding                     ----===*/
/*====================------------------------------------====================*/
static void  o___SEARCH__________o () { return; }

char COL__by_cursor  (tNODE **a_found, tTAB *a_tab, char a_move) { return NODE_by_cursor (a_found, a_tab, IS_COL, a_move); }

char COL__by_index   (tNODE **a_found, tTAB *a_tab, ushort a_col) { return NODE_by_index (a_found, a_tab, IS_COL, a_col); }

char COL__ensure     (tNODE **a_found, tTAB *a_tab, ushort a_col) { return NODE_ensure (a_found, a_tab, IS_COL, a_col); }



/*====================------------------------------------====================*/
/*===----                  hooking and unhooking cells                 ----===*/
/*====================------------------------------------====================*/
static void  o___HOOKING_________o () { return; }

char COL_hook_cell   (tTAB *a_tab, ushort a_col, ushort a_row, tCELL *a_cell) { NODE_hook_cell (a_tab, IS_COL, a_col, a_row, a_cell); }

char COL_unhook_cell (tCELL *a_cell) { NODE_unhook_cell (IS_COL, a_cell); }



/*====================------------------------------------====================*/
/*===----                     usage statistics                         ----===*/
/*====================------------------------------------====================*/
static void  o___USAGE___________o () { return; }

short COL_used       (char a_index, short a_ref) { return NODE_used       (a_index, IS_COL, a_ref); }

short COL_min_used   (char a_index)              { return NODE_min_used   (a_index, IS_COL); }

short COL_max_used   (char a_index)              { return NODE_max_used   (a_index, IS_COL); }

short COL_max        (char a_index)              { return NODE_max        (a_index, IS_COL); }

short COL_max_set    (char a_index, short a_max) { return NODE_max_set    (a_index, IS_COL, a_max); }

short COL_max_adjust (char a_index)              { return NODE_max_adjust (a_index, IS_COL); }



/*====================------------------------------------====================*/
/*===----                         program level                        ----===*/
/*====================------------------------------------====================*/
static void  o___PROGRAM_________o () { return; }

char COL_cleanse     (tTAB *a_tab) { return NODE_cleanse (a_tab, IS_COL); }

char COL_cleanse_curr(void)        { return NODE_cleanse (PTAB, IS_COL); }



/*====================------------------------------------====================*/
/*===----                   work with sizes                            ----===*/
/*====================------------------------------------====================*/
static void  o___SIZING__________o () { return; }

char COL_size        (char a_index, short a_ref) { return NODE_size (a_index, IS_COL, a_ref); }

char COL_resize      (char a_index, short a_ref, char a_size) { return NODE_resize (a_index, IS_COL, a_ref, a_size); }

char COL_reset       (char a_index, short a_ref) { return NODE_reset (a_index, IS_COL, a_ref); }

char COL_multisize   (char *a_label, char a_size, char a_count) { return NODE_multisize   (a_label, IS_COL, a_size, a_count); }

char COL_multikey    (char a_key) { return NODE_multikey (IS_COL, a_key); }



/*====================------------------------------------====================*/
/*===----                     freezing col/row                         ----===*/
/*====================------------------------------------====================*/
static void  o___FREEZING________o () { return; }

char COL_freeze      (char a_index, short a_bcol, short a_ecol) { return NODE_freeze (a_index, IS_COL, a_bcol, a_ecol); }

char COL_unfreeze    (char a_index) { return NODE_unfreeze (a_index, IS_COL); }



/*====================------------------------------------====================*/
/*===----                    yparse record handling                    ----===*/
/*====================------------------------------------====================*/
static void  o___YPARSE__________o () { return; }

char COL_reader      (int c, uchar *a_verb) { return NODE_reader (c, a_verb); }

char COL_writer      (char a_index, short a_ref) { return NODE_writer_one (a_index, IS_COL, a_ref); }

char COL_writer_all  (void) { return NODE_writer_all (IS_COL); }



/*====================------------------------------------====================*/
/*===----                    yvikeys screen mapping                    ----===*/
/*====================------------------------------------====================*/
static void  o___YVIKEYS_________o () { return; }

char COL_mapper      (char a_level) { return NODE_map_update (IS_COL, a_level); }

char COL_entry       (ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used) { return NODE_entry (IS_COL, a_pos, r_ref, r_wide, r_used); }



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char* COL__unit      (char *a_question, uchar a_tab, ushort a_col) { NODE__unit (a_question, a_tab, IS_COL, a_col); }



