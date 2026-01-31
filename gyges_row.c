/*============================----beg-of-source---============================*/
#include   "gyges.h"



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

short ROW_max_set    (char a_index, short a_max) { return NODE_max_set         (a_index, IS_ROW, a_max); }

short ROW_max_adjust (char a_index) { return NODE_max_adjust      (a_index, IS_ROW); }



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

uchar ROW_size        (char a_index, short a_ref) { return NODE_size (a_index, IS_ROW, a_ref); }

char  ROW_resize      (char a_index, short a_ref, uchar a_size) { return NODE_resize (a_index, IS_ROW, a_ref, a_size); }



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

char ROW_reader      (int c, uchar *a_verb) { return NODE_reader (c, a_verb); }

char ROW_writer      (char a_index, short a_ref) { return NODE_writer_one (a_index, IS_ROW, a_ref); }

char ROW_writer_all  (void) { return NODE_writer_all (IS_ROW); }



/*====================------------------------------------====================*/
/*===----                    yvikeys screen mapping                    ----===*/
/*====================------------------------------------====================*/
static void  o___YVIKEYS_________o () { return; }

char ROW_mapper      (char a_level) { return NODE_map_update (IS_ROW, a_level); }

char ROW_entry       (ushort a_pos, short *r_ref, uchar *r_wide, uchar *r_used) { return NODE_entry (IS_ROW, a_pos, r_ref, r_wide, r_used); }



/*====================------------------------------------====================*/
/*===----                      unit testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___UNITTEST________o () { return; }

char* ROW__unit      (char *a_question, uchar a_tab, ushort a_row) { NODE__unit (a_question, a_tab, IS_ROW, a_row); }

