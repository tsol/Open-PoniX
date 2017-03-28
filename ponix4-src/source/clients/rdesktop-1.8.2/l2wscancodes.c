#include "X11/XKBlib.h"
#include "X11/keysym.h"

#include "scancodes.h"

void init_l2w_scancode_conv_mx(Display* g_display, int *vks) {
    int i, j;
    KeySym *ks;
    int keysums_per_scancode;
    const int first_scan_code = 8;
    ks = XGetKeyboardMapping(g_display, first_scan_code, 256 - first_scan_code, &keysums_per_scancode);

    
    for (i = 0; i < 255; ++i)
        vks[i] = 0;

    int scancode;
    for (scancode = first_scan_code; scancode < 255; ++scancode) {
        for (j = 0; j < keysums_per_scancode; ++j) {
            if(vks[scancode]) continue;
            switch (ks[(scancode - first_scan_code) * keysums_per_scancode + j]) {
                    //F1 - F12
                case XK_F1: vks[scancode] = SCANCODE_CHAR_F1;
                    break;

                case XK_F2: vks[scancode] = SCANCODE_CHAR_F2;
                    break;

                case XK_F3: vks[scancode] = SCANCODE_CHAR_F3;
                    break;

                case XK_F4: vks[scancode] = SCANCODE_CHAR_F4;
                    break;

                case XK_F5: vks[scancode] = SCANCODE_CHAR_F5;
                    break;

                case XK_F6: vks[scancode] = SCANCODE_CHAR_F6;
                    break;

                case XK_F7: vks[scancode] = SCANCODE_CHAR_F7;
                    break;

                case XK_F8: vks[scancode] = SCANCODE_CHAR_F8;
                    break;

                case XK_F9: vks[scancode] = SCANCODE_CHAR_F9;
                    break;

                case XK_F10: vks[scancode] = SCANCODE_CHAR_F10;
                    break;

                case XK_F11: vks[scancode] = SCANCODE_CHAR_F11;
                    break;

                case XK_F12: vks[scancode] = SCANCODE_CHAR_F12;
                    break;

                    //1, 2, 3 ... 0

                case XK_1: vks[scancode] = SCANCODE_CHAR_1;
                    break;

                case XK_2: vks[scancode] = SCANCODE_CHAR_2;
                    break;

                case XK_3: vks[scancode] = SCANCODE_CHAR_3;
                    break;

                case XK_4: vks[scancode] = SCANCODE_CHAR_4;
                    break;

                case XK_5: vks[scancode] = SCANCODE_CHAR_5;
                    break;

                case XK_6: vks[scancode] = SCANCODE_CHAR_6;
                    break;

                case XK_7: vks[scancode] = SCANCODE_CHAR_7;
                    break;

                case XK_8: vks[scancode] = SCANCODE_CHAR_8;
                    break;

                case XK_9: vks[scancode] = SCANCODE_CHAR_9;
                    break;

                case XK_0: vks[scancode] = SCANCODE_CHAR_0;
                    break;

                    // - =
                case XK_minus: vks[scancode] = SCANCODE_CHAR_MINUS;
                    break;

                case XK_equal: vks[scancode] = SCANCODE_CHAR_EQUAL;
                    break;

                    //A - J (10 keys)
                case XK_A: vks[scancode] = SCANCODE_CHAR_A;
                    break;

                case XK_B: vks[scancode] = SCANCODE_CHAR_B;
                    break;

                case XK_C: vks[scancode] = SCANCODE_CHAR_C;
                    break;

                case XK_D: vks[scancode] = SCANCODE_CHAR_D;
                    break;

                case XK_E: vks[scancode] = SCANCODE_CHAR_E;
                    break;

                case XK_F: vks[scancode] = SCANCODE_CHAR_F;
                    break;

                case XK_G: vks[scancode] = SCANCODE_CHAR_G;
                    break;

                case XK_H: vks[scancode] = SCANCODE_CHAR_H;
                    break;

                case XK_I: vks[scancode] = SCANCODE_CHAR_I;
                    break;

                case XK_J: vks[scancode] = SCANCODE_CHAR_J;
                    break;

                    //K - T(10 keys)
                case XK_K: vks[scancode] = SCANCODE_CHAR_K;
                    break;

                case XK_L: vks[scancode] = SCANCODE_CHAR_L;
                    break;

                case XK_M: vks[scancode] = SCANCODE_CHAR_M;
                    break;

                case XK_N: vks[scancode] = SCANCODE_CHAR_N;
                    break;

                case XK_O: vks[scancode] = SCANCODE_CHAR_O;
                    break;

                case XK_P: vks[scancode] = SCANCODE_CHAR_P;
                    break;

                case XK_Q: vks[scancode] = SCANCODE_CHAR_Q;
                    break;

                case XK_R: vks[scancode] = SCANCODE_CHAR_R;
                    break;

                case XK_S: vks[scancode] = SCANCODE_CHAR_S;
                    break;

                case XK_T: vks[scancode] = SCANCODE_CHAR_T;
                    break;

                    //U - Z (6 keys)
                case XK_U: vks[scancode] = SCANCODE_CHAR_U;
                    break;

                case XK_V: vks[scancode] = SCANCODE_CHAR_V;
                    break;

                case XK_W: vks[scancode] = SCANCODE_CHAR_W;
                    break;

                case XK_X: vks[scancode] = SCANCODE_CHAR_X;
                    break;

                case XK_Y: vks[scancode] = SCANCODE_CHAR_Y;
                    break;

                case XK_Z: vks[scancode] = SCANCODE_CHAR_Z;
                    break;

                    //[ ] ; ' , . /
                case XK_bracketleft: vks[scancode] = SCANCODE_CHAR_BRACKETLEFT;
                    break;

                case XK_bracketright: vks[scancode] = SCANCODE_CHAR_BRACKETRIGHT;
                    break;

                case XK_apostrophe: vks[scancode] = SCANCODE_CHAR_APOSTROPHE;
                    break;

                case XK_semicolon: vks[scancode] = SCANCODE_CHAR_SEMICOLON;
                    break;

                case XK_comma: vks[scancode] = SCANCODE_CHAR_COMMA;
                    break;

                case XK_period: vks[scancode] = SCANCODE_CHAR_DOT;
                    break;

                case XK_slash: vks[scancode] = SCANCODE_CHAR_SLASH;
                    break;

                    //~ backslash
			case XK_grave : vks[scancode] = SCANCODE_CHAR_GRAVE;
                    break;

                case XK_backslash: vks[scancode] = SCANCODE_CHAR_BACKSLASH;
                    break;


                    //Esc, Tab, CapsLock, LShift, LCtrl, LWin(LSuper), LAlt
                case XK_Escape: vks[scancode] = SCANCODE_CHAR_ESC;
                    break;

                case XK_Tab: vks[scancode] = SCANCODE_CHAR_TAB;
                    break;

                case XK_Caps_Lock: vks[scancode] = SCANCODE_CHAR_CAPSLOCK;
                    break;

                case XK_Shift_L: vks[scancode] = SCANCODE_CHAR_LSHIFT;
                    break;

                case XK_Control_L: vks[scancode] = SCANCODE_CHAR_LCTRL;
                    break;

                case XK_Alt_L: vks[scancode] = SCANCODE_CHAR_LALT;
                    break;

                case XK_Super_L: vks[scancode] = SCANCODE_CHAR_LWIN;
                    break;

                    //BSpace, RShift, RCtrl, RAlt, Apps, Ins, Del, RSuper

                case XK_BackSpace: vks[scancode] = SCANCODE_CHAR_BACKSPACE;
                    break;

                case XK_Menu: vks[scancode] = SCANCODE_CHAR_APPLICATION;
                    break;

                case XK_Insert: vks[scancode] = SCANCODE_CHAR_INSERT;
                    break;

                case XK_Shift_R: vks[scancode] = SCANCODE_CHAR_RSHIFT;
                    break;

                case XK_Control_R: vks[scancode] = SCANCODE_CHAR_RCTRL;
                    break;

                case XK_Alt_R: vks[scancode] = SCANCODE_CHAR_RALT;
                    break;

                case XK_Super_R: vks[scancode] = SCANCODE_CHAR_RWIN;
                    break;

                case XK_Delete: vks[scancode] = SCANCODE_CHAR_DELETE;
                    break;

                    //Space, Enter, Up, Down, Right, Left
                case XK_Left: vks[scancode] = SCANCODE_CHAR_LARROW;
                    break;

                case XK_Up: vks[scancode] = SCANCODE_CHAR_UPARROW;
                    break;

                case XK_Right: vks[scancode] = SCANCODE_CHAR_RARROW;
                    break;

                case XK_Down: vks[scancode] = SCANCODE_CHAR_DNARROW;
                    break;

                case XK_Return: vks[scancode] = SCANCODE_CHAR_ENTER;
                    break;

                case XK_space: vks[scancode] = SCANCODE_CHAR_SPACE;
                    break;



                //ScrollLock, PrintScreen, PauseBreak, PgUp, PgDn, Home, End
                case XK_Home: vks[scancode] = SCANCODE_CHAR_HOME;
                    break;

                case XK_End: vks[scancode] = SCANCODE_CHAR_END;
                    break;

                case XK_Page_Up: vks[scancode] = SCANCODE_CHAR_PAGEUP;
                    break;

                case XK_Page_Down: vks[scancode] = SCANCODE_CHAR_PAGEDOWN;
                    break;

                case XK_Print: vks[scancode] = SCANCODE_CHAR_PRINT;
                    break;

                case XK_Pause: vks[scancode] = SCANCODE_CHAR_PAUSE;
                    break;

                case XK_Scroll_Lock: vks[scancode] = SCANCODE_CHAR_SCROLLLOCK;
                    break;

                    ///////
                    //Num block
                    //////


                //0, 1, ... 9
                case XK_KP_1: vks[scancode] = SCANCODE_CHAR_NUMERIC1;
                    break;

                case XK_KP_2: vks[scancode] = SCANCODE_CHAR_NUMERIC2;
                    break;

                case XK_KP_3: vks[scancode] = SCANCODE_CHAR_NUMERIC3;
                    break;

                case XK_KP_4: vks[scancode] = SCANCODE_CHAR_NUMERIC4;
                    break;

                case XK_KP_5: vks[scancode] = SCANCODE_CHAR_NUMERIC5;
                    break;

                case XK_KP_6: vks[scancode] = SCANCODE_CHAR_NUMERIC6;
                    break;

                case XK_KP_7: vks[scancode] = SCANCODE_CHAR_NUMERIC7;
                    break;

                case XK_KP_8: vks[scancode] = SCANCODE_CHAR_NUMERIC8;
                    break;

                case XK_KP_9: vks[scancode] = SCANCODE_CHAR_NUMERIC9;
                    break;

                case XK_KP_0: vks[scancode] = SCANCODE_CHAR_NUMERIC0;
                    break;


                // / * - + .
                case XK_KP_Divide: vks[scancode] = SCANCODE_CHAR_NUMERICSLASH;
                    break;

                case XK_KP_Add: vks[scancode] = SCANCODE_CHAR_NUMERICPLUS;
                    break;

                case XK_KP_Subtract: vks[scancode] = SCANCODE_CHAR_NUMERICMINUS;
                    break;

                case XK_KP_Multiply: vks[scancode] = SCANCODE_CHAR_NUMERICSTAR;
                    break;

                case XK_KP_Delete:
                case XK_KP_Decimal:
                case XK_KP_Separator:  vks[scancode] = SCANCODE_CHAR_NUMERICDOT;
                    break;


                //Enter, Num Lock
                case XK_KP_Enter: vks[scancode] = SCANCODE_CHAR_NUMERICENTER;
                    break;

                case XK_Num_Lock: vks[scancode] = SCANCODE_CHAR_NUMLOCK;
                    break;



            }
        }
    }

}
