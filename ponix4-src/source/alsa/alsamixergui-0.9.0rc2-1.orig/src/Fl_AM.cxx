
#include <FL/Fl_Menu_Button.H>

#include "Fl_AM.H"
#include "Fl_Pixmap_Button.H"
#include <FL/Fl_Browser.H>

#include <string.h>
#include <alsa/asoundlib.h>

#include "images/rec0.xpm"
#include "images/rec1.xpm"
#include "images/speakerleft0.xpm"
#include "images/speakerleft1.xpm"
#include "images/speakerright0.xpm"
#include "images/speakerright1.xpm"
#include "images/lock0.xpm"
#include "images/lock1.xpm"
#include "images/alsalogo.xpm"

#define BGCOLOR FL_GRAY
#define NAMECOLOR1 FL_LIGHT1
#define NAMECOLOR2 FL_DARK1
#define SLIDERCOLOR FL_GRAY
#define TEXTCOLOR FL_BLACK

#define GROUPW 40

/**********************************/

Fl_Pixmap pixmap_speakerleft0(speakerleft0_xpm);
Fl_Pixmap pixmap_speakerleft1(speakerleft1_xpm);
Fl_Pixmap pixmap_speakerright0(speakerright0_xpm);
Fl_Pixmap pixmap_speakerright1(speakerright1_xpm);
Fl_Pixmap pixmap_rec0(rec0_xpm);
Fl_Pixmap pixmap_rec1(rec1_xpm);
Fl_Pixmap pixmap_lock0(lock0_xpm);
Fl_Pixmap pixmap_lock1(lock1_xpm);
Fl_Pixmap pixmap_alsalogo(alsalogo_xpm);

/**********************************/

enum {
  VIEW_CHANNELS,
  VIEW_HELP,
  VIEW_PROCINFO
};

/**********************************/

/* mixer bar channel : left or right */
#define MIXER_CHN_LEFT		0
#define MIXER_CHN_RIGHT		1
/* mask for toggle mute and capture */
#define MIXER_MASK_LEFT		(1 << 0)
#define MIXER_MASK_RIGHT	(1 << 1)
#define MIXER_MASK_STEREO	(MIXER_MASK_LEFT|MIXER_MASK_RIGHT)

int mixer_iteration (void);
void mixer_write_cbar (int elem_index);
void mixer_update_cbar (int);
void mixer_update_cbars (void);
void mixer_show_procinfo (void);
void
mixer_show_text (char *title,
		 char *text,
		 int  *xoffs,
		 int  *yoffs);

extern void 	*mixer_sid;
extern int mixer_n_selems;

extern int mixer_view;
extern int mixer_n_vis_elems;
extern int mixer_n_elems;
extern int mixer_focus_elem;
extern int mixer_toggle_mute;
extern int mixer_toggle_capture;
extern int mixer_volume_hard[];
extern char mixer_card_name[128];
extern char mixer_device_name[128];
extern snd_mixer_t *mixer_handle;
extern char*	 mixer_help_text;
extern int* mixer_grpidx;
extern int *mixer_type;
/**********************************/

Fl_AMWindow* mainWindow = 0;
Fl_AMProcWindow* procWindow = 0;
Fl_AMHelpWindow* helpWindow = 0;

Fl_Browser* textOut = 0;

int gui_do_update_all = 0;
int gui_do_select = 0;
int gui_prev_focus_elem = -1;

/**********************************/

void gui_select(void)
{
	// check for external mixer change
	mixer_n_vis_elems = mixer_n_elems;
	gui_do_select=1;
	mixer_iteration();
	gui_do_select=0;
}

void gui_update(void)
{
	// called after gui/key event 
	mixer_n_vis_elems = mixer_n_elems;
	mixer_write_cbar(mixer_focus_elem);
	mixer_update_cbar(mixer_focus_elem);
	gui_select();
}

void gui_update_all(void)
{
	mixer_n_vis_elems = mixer_n_elems;
	gui_do_update_all = 1;
	mixer_update_cbars();
	gui_do_update_all = 0;
}

void gui_init(void)
{
	mainWindow = new Fl_AMWindow;
}

void gui_show_text(char* text)
{
	int i=0;
	int j=0;
	char tmp[256];

	while (text[i]) {
		if (text[i]=='\n') {
			tmp[j]='\0';
			textOut->add(strdup(tmp));
			j=0;
		}else{
			tmp[j++]=text[i];
		}
		i++;
	}	
}

#include <FL/fl_ask.H>

void gui_abort(const char* errorstr)
{
	if (strcmp(errorstr,""))
		fl_alert(errorstr);
}

void gui_mixer_change_view(void)
{
	if (mixer_view == VIEW_CHANNELS) 
	{
		mainWindow->show();
		
	}else if (mixer_view == VIEW_PROCINFO) 
	{
		if (procWindow == 0) {
			procWindow = new Fl_AMProcWindow;
		}
		procWindow->show();
	}else if (mixer_view == VIEW_HELP) 
	{
		if (helpWindow == 0) {
			helpWindow = new Fl_AMHelpWindow;
		}
		helpWindow->show();
	}
}

void gui_open_help(void)
{
	mixer_view = VIEW_HELP;
	gui_mixer_change_view();
}

void gui_open_proc(void)
{
	mixer_view = VIEW_PROCINFO;
	gui_mixer_change_view();
}

void gui_idle_cb(void*)
{
	gui_select();
	Fl::add_timeout(0.1,gui_idle_cb,0);
}

#include <FL/Enumerations.H>

void gui_start(void)
{
	char tmp[256];

	strcpy(tmp,"Card: ");
	strcat(tmp,mixer_card_name);
	
	mainWindow->card_name->label(strdup(tmp));

	strcpy(tmp,"Chip: ");
	strcat(tmp,mixer_device_name);

	mainWindow->device_name->label(strdup(tmp));

	gui_update_all();
	mainWindow->show();

	Fl::add_timeout(0.05,gui_idle_cb,0);
	
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION >= 1
	Fl::visible_focus(0);
#endif

	Fl::run();
}

static int cx;

void get_name(int elem_index,char* string1)
{
  int dc;
  snd_mixer_elem_t *elem;
  int type;
  snd_mixer_selem_id_t *sid;

  /* set new scontrol indices and read info
   */
  if (mixer_sid == NULL)
    return;

  sid = (snd_mixer_selem_id_t *)(((char *)mixer_sid) + snd_mixer_selem_id_sizeof() * mixer_grpidx[elem_index]);
  elem = snd_mixer_find_selem(mixer_handle, sid);
  if (elem == NULL)
		return;
  type = mixer_type[elem_index];

  if (snd_mixer_selem_id_get_index(sid) > 0)
    sprintf(string1, "%s %d", snd_mixer_selem_id_get_name(sid), snd_mixer_selem_id_get_index(sid));
  else
    strcpy(string1, snd_mixer_selem_id_get_name(sid));
}

void gui_add_groups(void)
{
	cx = mainWindow->scroll->x();
	int cy = mainWindow->scroll->y();
	mainWindow->scroll->add(new Fl_Box(cx,cy,3,0));
	int i;
	for (i= 0; i < mixer_n_elems; i++) {
  		mixer_volume_hard[i] = -1;
	}
	for (i = 0; i < mixer_n_elems; i++)
	{
		char string[256];
		get_name(i,string);
		if (
			strncmp(string,"PCM Chorus ",
				strlen("PCM Chorus ")) &&
			strncmp(string,"PCM Front ",
				strlen("PCM Front ")) &&
			strncmp(string,"PCM Pan Playback Control ",
				strlen("PCM Pan Playback Control ")) &&
			strncmp(string,"PCM Reverb ",strlen("PCM Reverb "))
		)
			mainWindow->add_group(strdup(string));
		else 
			mainWindow->add_group(0);
	}
	cx-=3;
	mainWindow->scroll->add(new Fl_Box(cx,cy,3,0));
	cx += 3;
}

/***************************************/

Fl_Menu_Item alsa_menu[] = 
{
 {"Help", 0,  (Fl_Callback*) gui_open_help, 0, 0, 0, 0, 12, 0},
 {"Proc Info", 0, (Fl_Callback*) gui_open_proc,  0, 0, 0, 0, 12, 0},
 {0}
};

/***************************************/
Fl_AMProcWindow::Fl_AMProcWindow():Fl_Window(500,240,"/proc/asound")
{
	callback((Fl_Callback*) cb);
	Fl_Browser* o = new Fl_Browser(3,3,w()-6,h()-6);
	o->box(FL_FLAT_BOX);
	o->color(color());
	o->textsize(12);
	o->textfont(FL_COURIER);

	textOut = o;
	mixer_show_procinfo ();
	
	resizable(o);
	end();
}

void Fl_AMProcWindow::cb(Fl_Window* ptr)
{
	delete ptr;
}

Fl_AMProcWindow::~Fl_AMProcWindow() {
	procWindow = 0;
	mixer_view = VIEW_CHANNELS;
	gui_mixer_change_view();
}

int Fl_AMProcWindow::handle(int e) {
	if (e==FL_KEYBOARD) {
		mixer_iteration(); // mixer_iteration handles key event
		if (mainWindow) {
			mixer_write_cbar( mixer_focus_elem );
			mixer_update_cbar( mixer_focus_elem );
		}
		gui_select(); // check for external changes

		return 1;
	}
	return Fl_Window::handle(e);	
}

Fl_AMHelpWindow::Fl_AMHelpWindow():Fl_Window(500,240,"ALSA Mixer Help")
{
	callback((Fl_Callback*) cb);
	Fl_Browser* o = new Fl_Browser(3,3,w()-6,h()-6);
	o->box(FL_FLAT_BOX);
	o->color(color());
	o->textfont(FL_COURIER);
	o->textsize(12);

	textOut = o;
	mixer_show_text ("Help", mixer_help_text,0,0);
	
	resizable(o);
	end();
}

void Fl_AMHelpWindow::cb(Fl_Window* ptr)
{
	delete ptr;
}

Fl_AMHelpWindow::~Fl_AMHelpWindow() {
	helpWindow = 0;
	mixer_view = VIEW_CHANNELS;
	gui_mixer_change_view();
}

int Fl_AMHelpWindow::handle(int e) {
	if (e==FL_KEYBOARD) {
		mixer_iteration(); // mixer_iteration handles key event
		if (mainWindow) {
			mixer_write_cbar( mixer_focus_elem );
			mixer_update_cbar( mixer_focus_elem );
		}
		gui_select(); // check for external changes

		return 1;
	}
	return Fl_Window::handle(e);	
}
	
#define NAMEBOXH 60
	
class Fl_AMSlider:public Fl_Slider
{
public:
	Fl_AMSlider(int X,int Y,int W,int H,char* L ="")
	:Fl_Slider(X,Y,W,H,L)
	{
	}
	void draw(void)
	{
		Fl_Slider::draw();
	}
	int handle(int e)
	{
		return Fl_Slider::handle(e);
	}
};
	
Fl_AMGroup::Fl_AMGroup(int X,int Y,int W,int H,char* L,int id)
:Fl_Group(X,Y,W,H)
{
	muteL = new Fl_Pixmap_Button(X,Y,W/2,16);
	muteR = new Fl_Pixmap_Button(X+W/2,Y,W/2,16);
	muteL->callback((Fl_Callback*)muteL_cb);
	muteR->callback((Fl_Callback*)muteR_cb);
	muteL->when(FL_WHEN_CHANGED);
	muteR->when(FL_WHEN_CHANGED);

	muteL->color(BGCOLOR,BGCOLOR);
	muteR->color(BGCOLOR,BGCOLOR);

	((Fl_Pixmap_Button*)muteL)->pixmaps(&pixmap_speakerleft1,&pixmap_speakerleft0);
	((Fl_Pixmap_Button*)muteR)->pixmaps(&pixmap_speakerright1,&pixmap_speakerright0);

	captureL = new Fl_Pixmap_Button(X,Y+16,W/2,16);
	captureR = new Fl_Pixmap_Button(X+W/2,Y+16,W/2,16);
	captureL->callback((Fl_Callback*)captureL_cb);
	captureR->callback((Fl_Callback*)captureR_cb);
	captureL->when(FL_WHEN_CHANGED);
	captureR->when(FL_WHEN_CHANGED);

	captureL->color(BGCOLOR,BGCOLOR);
	captureR->color(BGCOLOR,BGCOLOR);

	captureLbox = new Fl_Box(X,Y+16,W/2,16);
	captureRbox = new Fl_Box(X+W/2,Y+16,W/2,16);

	captureLbox->color(BGCOLOR,BGCOLOR);
	captureRbox->color(BGCOLOR,BGCOLOR);

	captureLbox->box(FL_FLAT_BOX);
	captureRbox->box(FL_FLAT_BOX);
	
	captureLbox->hide();
	captureRbox->hide();	
	((Fl_Pixmap_Button*)captureL)->pixmaps(&pixmap_rec0,&pixmap_rec1);
	((Fl_Pixmap_Button*)captureR)->pixmaps(&pixmap_rec0,&pixmap_rec1);

	volumeL = new Fl_AMSlider(X,Y+32,W/2,H-NAMEBOXH-32-20);
	volumeR = new Fl_AMSlider(X+W/2,Y+32,W/2,H-NAMEBOXH-32-20);
	volumeL->callback((Fl_Callback*)volumeL_cb);
	volumeR->callback((Fl_Callback*)volumeR_cb);

	volumeL->color(BGCOLOR,SLIDERCOLOR);
	volumeR->color(BGCOLOR,SLIDERCOLOR);

	volumeL->type(FL_VERT_FILL_SLIDER);
	volumeR->type(FL_VERT_FILL_SLIDER);

	volumeL->box(FL_THIN_DOWN_BOX);
	volumeR->box(FL_THIN_DOWN_BOX);

	volumeL->range(100,0);
	volumeR->range(100,0);

	char* tmp=strdup(L);
	int j=0;
	char* substr[16];
	int substri=0;
	char* prevptr = tmp;
	char* ptr=tmp;
	while (*ptr) {
		if (*ptr==' ') {
			*ptr=0;
			if (strcmp(prevptr,"-"))
				substr[substri++]=prevptr;
			prevptr=ptr+1;
		}
		ptr++;
	}
	substr[substri++]=prevptr;
	fl_font(FL_HELVETICA,10);
	ptr=tmp;
	for (int i=0;i<substri;i++)
	{
		int n=strlen(substr[i]);
		int flag=0;
		while (fl_width(substr[i],n)>40) 
{
			n--;
			flag=1;
		}
		if (flag==1)
		{
			substr[i][n-1]='.';
			substr[i][n-2]='.';
		}
		strncpy(ptr,substr[i],n);
		ptr+=n;
		if (i!=substri-1) *ptr++='\n';
	}
	*ptr=0;
	
	lock = new Fl_Pixmap_Button(X,Y+H-NAMEBOXH-20,W,20);
	lock->color(BGCOLOR,BGCOLOR);
	lock->value(0);
	((Fl_Pixmap_Button*)lock)->pixmaps(&pixmap_lock0,&pixmap_lock1);
	lock->callback((Fl_Callback*)lock_cb);

	name = new Fl_Button(X,Y+H-NAMEBOXH,W,NAMEBOXH,tmp);
	name->labelsize(10);
	name->box(FL_FLAT_BOX);
	name->color(NAMECOLOR1,NAMECOLOR2);
	name->labelcolor(TEXTCOLOR);
	name->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP|FL_ALIGN_CLIP);
	name->callback((Fl_Callback*)name_cb);
	
	elem_id = id;
	end();
}

void Fl_AMGroup::lock_cb(Fl_Button* b)
{
	Fl_AMGroup* g = (Fl_AMGroup*) b->parent();
	mixer_focus_elem = g->elem_id;
	b->value(!b->value());
	gui_update();
}

void Fl_AMGroup::name_cb(Fl_Button* b)
{
	Fl_AMGroup* g = (Fl_AMGroup*) b->parent();
	mixer_focus_elem = g->elem_id;
	gui_update();
}

void Fl_AMGroup::volumeL_cb(Fl_Slider* s)
{
	Fl_AMGroup* g = (Fl_AMGroup*) s->parent();
	mixer_focus_elem = g->elem_id;
	if (g->lock->value()) {
		mixer_volume_hard[MIXER_CHN_RIGHT] = int(s->value());
	}
	mixer_volume_hard[MIXER_CHN_LEFT] = int(s->value());
	gui_update();
}

void Fl_AMGroup::volumeR_cb(Fl_Slider* s)
{
	Fl_AMGroup* g = (Fl_AMGroup*) s->parent();
	mixer_focus_elem = g->elem_id;
	if (g->lock->value()) {
		mixer_volume_hard[MIXER_CHN_LEFT] = int(s->value());
	}
	mixer_volume_hard[MIXER_CHN_RIGHT] = int(s->value());
	gui_update();
}

void Fl_AMGroup::muteL_cb(Fl_Button* s)
{
	Fl_AMGroup* g = (Fl_AMGroup*) s->parent();
	mixer_focus_elem = g->elem_id;
	mixer_toggle_mute |= MIXER_MASK_LEFT;
	if (g->lock->value()) {
		mixer_toggle_mute |= MIXER_MASK_RIGHT;
	}
	int prevvalue = s->value();
	gui_update();
	if (s->value()==prevvalue) {
		mixer_toggle_mute = MIXER_MASK_RIGHT | MIXER_MASK_LEFT;	
		gui_update();
	}
}

void Fl_AMGroup::muteR_cb(Fl_Button* s)
{
	Fl_AMGroup* g = (Fl_AMGroup*) s->parent();
	mixer_focus_elem = g->elem_id;
	mixer_toggle_mute |= MIXER_MASK_RIGHT;
	if (g->lock->value()) {
		mixer_toggle_mute |= MIXER_MASK_LEFT;
	}
	int prevvalue = s->value();
	gui_update();
	if (s->value()==prevvalue) {
		mixer_toggle_mute = MIXER_MASK_RIGHT | MIXER_MASK_LEFT;	
		gui_update();
	}
}

void Fl_AMGroup::captureL_cb(Fl_Button* s)
{
	Fl_AMGroup* g = (Fl_AMGroup*) s->parent();
	mixer_focus_elem = g->elem_id;
	mixer_toggle_capture |= MIXER_MASK_LEFT;
	if (g->lock->value()) {
		mixer_toggle_capture |= MIXER_MASK_RIGHT;
	}
	int prevvalue = s->value();
	gui_update();
	if (s->value()==prevvalue) {
		mixer_toggle_capture = MIXER_MASK_RIGHT | MIXER_MASK_LEFT;	
		gui_update();
	}
}

void Fl_AMGroup::captureR_cb(Fl_Button* s)
{
	Fl_AMGroup* g = (Fl_AMGroup*) s->parent();
	mixer_focus_elem = g->elem_id;
	mixer_toggle_capture |= MIXER_MASK_RIGHT;
	if (g->lock->value()) {
		mixer_toggle_capture |= MIXER_MASK_LEFT;
	}
	int prevvalue = s->value();
	gui_update();
	if (s->value()==prevvalue) {
		mixer_toggle_capture = MIXER_MASK_RIGHT | MIXER_MASK_LEFT;	
		gui_update();
	}
}

Fl_AMWindow::Fl_AMWindow():Fl_Window(512,256,"ALSA Mixer")
{
	color(BGCOLOR);
	box(FL_NO_BOX);
	{
		Fl_Box* b = new Fl_Box(0,0,200,24);
		b->labelsize(10);
		b->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
		b->box(FL_THIN_UP_BOX);
		b->color(BGCOLOR);
		b->labelcolor(TEXTCOLOR);
		card_name = b;
	}
	{
		Fl_Box* b = new Fl_Box(200,0,200,24);
		b->labelsize(10);
		b->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
		b->box(FL_THIN_UP_BOX);
		b->color(BGCOLOR);
		b->labelcolor(TEXTCOLOR);
		device_name = b;
	}
	{
		Fl_Menu_Button* b = new Fl_Menu_Button(400,0,112,24);
		b->labelsize(10);
		b->box(FL_THIN_UP_BOX);
		b->align(FL_ALIGN_INSIDE|FL_ALIGN_RIGHT);
		pixmap_alsalogo.label(b);
		b->color(BGCOLOR);
		b->labelcolor(TEXTCOLOR);
		b->menu(alsa_menu);
	}

	{
		Fl_Box* b = new Fl_Box(0,24,w(),3);
		b->box(FL_FLAT_BOX);
	}

	scroll = new Fl_Scroll(0,27,w(),h()-27);
	scroll->box(FL_NO_BOX);
	scroll->color(BGCOLOR);
	resizable(scroll);
}

void Fl_AMWindow::add_group(char* name)
{
	static int i=0;
	static int first = 1;
	if (name) {
		Fl_Box* b;

		if (first)
		{
			scroll->add(b = new Fl_Box(cx,scroll->y(),3,scroll->h()-20));
			b->box(FL_FLAT_BOX);
			cx+=3;
			first = 0;
		}

		group[i] = new Fl_AMGroup(cx,scroll->y(),GROUPW,scroll->h()-20,name,i);
		scroll->add(group[i]);
		cx+=GROUPW;

		scroll->add(b = new Fl_Box(cx,scroll->y(),3,scroll->h()-20));
		b->box(FL_FLAT_BOX);
		cx+=3;
	} else {
		group[i] = 0;
	}
	i++;
}

int Fl_AMWindow::handle(int e)
{
	if (e==FL_KEYBOARD) {
		mixer_iteration(); // mixer_iteration handles key event
		if (mainWindow) {
			mixer_write_cbar( mixer_focus_elem );
			mixer_update_cbar( mixer_focus_elem );
		}
		gui_select(); // check for external changes
		
		return 1;
	}
	return Fl_Window::handle(e);
}

