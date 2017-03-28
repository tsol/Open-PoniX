/*** workspaces.c ***/
int 
find_frame_with_framed_window(Window window, struct Workspace_list* workspaces);

int
find_frame_with_framed_window_in_workspace(Window window, struct Workspace_list* workspaces, int k);

int
find_frame_with_menu_item_in_workspace(Window window, struct Workspace_list* workspaces, int current_workspace);

int
find_workspace_with_menu_item(Window window, struct Workspace_list* workspaces);

enum Frame_widget
find_frame_with_widget_in_workspace(Window key_window, struct Workspace_list* workspaces, int k, int *i);

int 
create_workspace
(Display *display, struct Workspace_list* workspaces, char *workspace_name, struct Themes *themes);

void 
remove_workspace(Display *display, struct Workspace_list* workspaces, int index);

Bool
create_startup_workspaces(Display *display, struct Workspace_list *workspaces
, int *current_workspace, struct Workspace **frames
, struct Separators *seps
, struct Popup_menu *window_menu, struct Themes *themes, struct Cursors *cursors, struct Atoms *atoms);

int 
add_frame_to_workspace(Display *display, struct Workspace_list *workspaces, Window window, int *current_workspace, struct Workspace **frames
, struct Popup_menu *window_menu
, struct Separators *seps
, struct Themes *themes, struct Cursors *cursors, struct Atoms *atoms);

void 
change_to_workspace(Display *display, struct Workspace_list *workspaces, int *current_workspace, struct Workspace **frames, int index, struct Separators *seps, struct Themes *themes, struct Atoms *atoms);

void 
update_client_list(Display *display, struct Workspace *frames, struct Atoms *atoms);
