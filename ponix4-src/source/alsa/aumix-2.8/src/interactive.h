#ifndef INTERACTIVE_H
#define INTERACTIVE_H
#define MUTE_NO_DEVICE -1
#define MUTE_OFF 0
#define MUTE_GLOBAL 1
#define MUTE_ONLY 2
#define        REFRESH_PERIOD 1	/* number of seconds between updates */

char           *chark, *charl, *charm, *charo, *charq, *chars, *charu;
void            AumixSignalHandler(int signal_number);
void            InitScreen(void);
void            KeysBox(void);
void            RefreshAllSettings(void);
void            RefreshNewSettings(void);
void            Muting(int device, int newstate);
void            ToggleMuting(void);
void            MuteAll(void);
void            UnmuteAll(void);
void            StoreAll(void);
void            StoreOne(int device);
void            MuteAllButOne(int device);
void            MuteOne(int device);
void            UnmuteAllButOne(int device);
void            UnmuteOne(int dev);
void            DrawLevelBalMode(int dev, int mode);
void            AdjustLevel(int dev, int incr, int setlevel);
void            DrawLevel(int dev);
void            EraseLevel(int dev);
void            AdjustBalance(int dev, int incr, int setabs);
void            RedrawBalance(int dev);
void            SwitchRecordPlay(int dev);
void            DrawRecordPlay(int dev);
void            CloseScreen(void);
void            ReadInteractiveKeys(void);
#endif				/* INTERACTIVE_H */
