int supress_xerror          (Display *display, XErrorEvent *event);
int create_frame            (Display *display, struct Frame* result, Window framed_window, struct Popup_menu *window_menu, struct Separators *seps, struct Themes *themes, struct Cursors *cursors, struct Atoms *atoms);
void get_frame_hints        (Display* display, struct Frame* frame);
void get_frame_strut_hints_as_normal_hints(Display *display, struct Frame *frame, struct Atoms *atoms);
void remove_frame           (Display* display, struct Workspace_list *workspaces, int index, int current_workspace, struct Atoms *atoms, struct Themes *themes);
void recover_frame          (Display *display, struct Workspace *frames, int i /*index*/, struct Separators *seps, struct Themes *themes);
void close_window           (Display *display, Window framed_window);
void free_frame_name        (struct Frame *frame);
void create_frame_name      (Display *display, struct Popup_menu *window_menu, struct Frame* frame, struct Themes *themes, struct Atoms *atoms);
void centre_frame           (const int container_width, const int container_height, const int w, const int h, int *x, int *y);
Bool suitable_for_foreign_workspaces(struct Frame *frame);
void make_frame_coordinates_minmax(Display *display, struct Frame *frame);

//void create_frame_icon_size (Display *display, struct Frame *frame, int new_size);
//void free_frame_icon_size   (Display *display, struct Frame *frame);

