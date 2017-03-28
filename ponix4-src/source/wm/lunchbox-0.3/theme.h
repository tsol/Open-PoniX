struct Themes *
create_themes(Display *display, char *theme_name);

void 
remove_themes(Display *display, struct Themes *themes);

void 
create_text_background(Display *display, Window window, const char *restrict text
, const struct Font_theme *restrict font_theme, Pixmap background_p, int b_w, int b_h);

unsigned int 
get_text_width(Display * display, const char *title, struct Font_theme *font_theme);

void 
create_icon_background(Display *display, Window window
, Pixmap icon_p, Pixmap icon_mask_p, int b_w, int b_h);
