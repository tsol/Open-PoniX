#ifndef __Pixmap_Button__
#define __Pixmap_Button__

#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/fl_draw.H>

class Fl_Pixmap_Button:public Fl_Button
{
public:
	Fl_Pixmap_Button(int X,int Y,int W,int H,char* L=""):
	Fl_Button(X,Y,W,H,L)
	{
		box(FL_FLAT_BOX);
		down_box(FL_FLAT_BOX);
	}
	void pixmaps(Fl_Pixmap* p0,Fl_Pixmap* p1)
	{
		pm[0]=p0;
		pm[1]=p1;
	}
	int handle(int e)
	{
		if (e==FL_PUSH) {
			do_callback();
			return 1;
		}
		return 0;
	}

	Fl_Pixmap* pm[2];

	void draw(void);
};

#include <FL/Enumerations.H>

inline void Fl_Pixmap_Button::draw(void)
{
	draw_box();
	Fl_Pixmap* p = pm[value()];
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
	int pw = p->w;
	int ph = p->h;
#else
	int pw = p->w();
	int ph = p->h();
#endif
	
	if (pw<0) {
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
		fl_measure_pixmap(p->data,pw,ph);
#else
		fl_measure_pixmap(p->data(),pw,ph);
#endif
	}
	int px = x()+(w()-pw)/2; 
	int py = y()+(h()-ph)/2;
	
	fl_push_clip(px,py,pw,ph);
	Fl_Button::draw();
	fl_pop_clip();
	p->draw(px,py);
}

#endif
