typedef void* WINDOW;

#define TRUE 1
#define FALSE 1

#define mixer_init_dc(a,b,c,d,e) 
#define mixer_dc(a) 
#define mvaddch(a,b,c) 
#define mvaddstr(a,b,c) 
#define addstr(a) 
#define mixer_cbar_get_pos(a,b,c) 1
#define initscr() 0
#define keypad(a,b) 
#define refresh() 
#define leaveok(a,b) 
#define endwin() 
#define noecho() 
#define timeout(a) 
#define getmaxyx(a,b,c) 

extern Fl_AMWindow* mainWindow;
extern Fl_AMProcWindow* procWindow;
extern Fl_AMHelpWindow* helpWindow;
extern int gui_do_update_all;
extern int gui_do_select;
extern int gui_prev_focus_elem;

void gui_select(void);
void gui_idle_cb(void*);
void gui_start(void);
void gui_init(void);
void gui_abort(const char* errorstr);
void gui_update_all(void);
void gui_add_groups(void);
void gui_show_text(char* text);
void gui_mixer_change_view(void);
