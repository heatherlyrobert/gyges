/*============================----beg-of-source---============================*/


#include   "gyges.h"




int
main (int argc, char *argv[])
{
   /*---(locals)-----------+-----------+-*/
   /*---(formats)---------*/
   char        msg_normal   [500] = "(N) normal   : horz(a)=0HhlL$  horz(g/z)=sh,le  vert(a)=_KkjJG  vert(g/z)=tk.jb  modes=vIFV:{ret}";
   /*> char        msg_format   [500] = "(O) format   : ali=<|>[^]  fmt=ifpeE,acs#%%xXbBoOtdT  fil= _-=.+  w=mhHlLnNwW h=jk  dec=0123456789  mrg=MU  modes={esc}{ret}";   <*/
   char        msg_format   [500] = "(f) format   : ali=<|>[^] num=ir,as$%%p tec=#eExXbBoO tim=tdT dec=0-9 str= _-=.+";
   char        msg_visual   [500] = "(V) visual   : dxy  !: ~uU /nN oO sS";
   char        msg_source   [500] = "(S) source   : horz(a)=0HhlL$  words=wbe  find=f;  undo=u  modes=IiaA{esc}{ret}";
   char        msg_buffer   [500] = "(B) buffer   : select=0...9  modes={ret}(esc}";
   char        msg_input    [500] = "(I) input    : mod={esc}";
   char        msg_wander   [500] = "(W) wander   : modes={ret}{esc}";
   /*---(working)---------*/
   int         updates     = 0;
   int         cch         = 0;        /* current keystroke                   */
   int         sch         = 0;        /* saved keystroke                     */
   char        rc          = 0;
   /*---(initialize)---------------------*/
   if (rc == 0)  rc = PROG_init    ();
   if (rc == 0)  rc = PROG_urgs    (argc, argv);
   if (rc == 0)  rc = PROG_args    (argc, argv);
   if (rc == 0)  rc = PROG_begin   ();
   if (rc != 0)  {
      PROG_end     ();
      exit (-1);
   }
   /*---(post-initialization)------------*/
   strncpy (message, msg_normal, MAX_STR);
   if (strcmp (f_title, FILE_BLANK) != 0)  FILE_read (f_title);
   if (rc == 0)  rc = CURS_begin ();
   if (rc != 0)  {
      PROG_end     ();
      exit (-1);
   }
   hist_active = 'y';
   /*---(process)------------------------*/
   KEYS_col (" r");
   KEYS_row (" r");
   sch = ' ';
   /*---(main-loop)----------------------*/
   DEBUG_TOPS   yLOG_note    ("entering main processing loop");
   DEBUG_TOPS   yLOG_break   ();
   while (done) {
      /*---(show screen)-----------------*/
      cch = CURS_main ();
      KEYS_record (cch);
      /*---(log)-------------------------*/
      ++updates;
      DEBUG_LOOP  yLOG_complex ("update"    , "loop = %6d, val = %4d, cch = %c", updates, cch, (cch > 32) ? cch : '-');
      /*---(create command display)------*/
      snprintf (cmd,   9, "%02x", cch);
      /*> if      (cch == '%') snprintf (cmd,   9, "  %% ");                          <* 
       *> else if (sch == '%') snprintf (cmd,   9, "%%%c ", cch);                     <* 
       *> else                 snprintf (cmd,   9, "%c%c ", sch, cch);                <*/
      /*---(handle keystroke)------------*/
      switch (mode) {
      case MODE_NORMAL   : rc = KEYS_normal   (sch, cch); break;
      case MODE_FORMAT   : rc = KEYS_format   (' ', cch); break;
      case MODE_SOURCE   : rc = KEYS_source   (sch, cch); break;
      case MODE_BUFFER   : rc = KEYS_buffer   (' ', cch); break;
      case MODE_INPUT    : rc = KEYS_input    (sch, cch); break;
      case MODE_WANDER   : rc = KEYS_wander   (' ', cch); break;
      case MODE_COMMAND  : rc = KEYS_command  (' ', cch); break;
      default            : rc = KEYS_normal   (sch, cch); break;
      }
      /*---(setup for next keystroke)----*/
      if      (rc == 0)    sch = ' ';
      else if (rc >  0)    sch = cch;
      else               { sch = ' ';  sta_error = 'y'; }
      /*---(setup status line)-----------*/
      switch (mode) {
      case MODE_NORMAL   : strncpy (message, msg_normal  , MAX_STR); break;
      case MODE_FORMAT   : strncpy (message, msg_format  , MAX_STR); break;
      case MODE_VISUAL   : strncpy (message, msg_visual  , MAX_STR); break;
      case MODE_SOURCE   : strncpy (message, msg_source  , MAX_STR); break;
      case MODE_BUFFER   : strncpy (message, msg_buffer  , MAX_STR); break;
      case MODE_INPUT    : strncpy (message, msg_input   , MAX_STR); break;
      case MODE_WANDER   : strncpy (message, msg_wander  , MAX_STR); break;
      case MODE_COMMAND  : strncpy (message, command     , MAX_STR); break;
      default            : strncpy (message, msg_normal  , MAX_STR); break;
      }
      /*---(translate unprintable)-------*/
      if      (cch ==  0) strcpy  (cmd,   "n/a");
      else if (cch == 10) strcpy  (cmd,   "ret");
      else if (cch == 27) strcpy  (cmd,   "esc");
      /*---(done)------------------------*/
   }
   DEBUG_TOPS  yLOG_break   ();
   DEBUG_TOPS  yLOG_note    ("exiting main processing loop");
   /*---(wrap-up)------------------------*/
   /*> FILE_write   (f_title);                                                        <*/
   CURS_end     ();
   PROG_end     ();
   /*---(complete)-----------------------*/
   return 0;
}



/*============================----end-of-source---============================*/
