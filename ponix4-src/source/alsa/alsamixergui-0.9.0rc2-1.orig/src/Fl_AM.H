#ifndef __FL_AM__
#define __FL_AM__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Box.H>

#include <stdio.h>

class Fl_AMGroup:public Fl_Group
{
public:
	Fl_Slider *volumeL,*volumeR;
	Fl_Button *lock,*muteL,*muteR,*captureL,*captureR;
	Fl_Button    *name;
	Fl_Box	*captureLbox,*captureRbox;
	int elem_id;
	
	Fl_AMGroup(int X,int Y,int W,int H,char* L,int id);
	
	static void volumeL_cb(Fl_Slider* s);
	static void volumeR_cb(Fl_Slider* s);

	static void muteL_cb(Fl_Button* s);
	static void muteR_cb(Fl_Button* s);

	static void captureL_cb(Fl_Button* s);
	static void captureR_cb(Fl_Button* s);

	static void lock_cb(Fl_Button* s);
	static void name_cb(Fl_Button* s);

	int handle(int e)
	{
		return Fl_Group::handle(e);
	}

};

class Fl_AMWindow:public Fl_Window
{
public:
	Fl_AMGroup* group[256];
	Fl_Scroll *scroll;
	Fl_Box* card_name;
	Fl_Box* device_name;

	Fl_AMWindow();
	int handle(int e);
	void add_group(char* name);
};

class Fl_AMProcWindow:public Fl_Window
{
public:
	Fl_AMProcWindow();
	static void cb(Fl_Window* ptr);
	~Fl_AMProcWindow();
	int handle(int e);
};

class Fl_AMHelpWindow:public Fl_Window
{
public:
	Fl_AMHelpWindow();
	static void cb(Fl_Window* ptr);
	~Fl_AMHelpWindow();
	int handle(int e);
};

#endif
