void add_focus             (Window new, struct Focus_list *focus);
void remove_focus          (Window old, struct Focus_list *focus);
void check_and_set_new_frame_focus (Display *display, struct Frame *frame, struct Workspace *frames);
void unfocus_frames        (Display *display, struct Workspace *frames);
void recover_focus         (Display *display, struct Workspace *frames, struct Themes *themes, struct Atoms *atoms);
