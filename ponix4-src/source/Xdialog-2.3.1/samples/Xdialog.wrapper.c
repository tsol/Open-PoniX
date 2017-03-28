#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char **argv)
{
	char wrapped_cmd[1024];

	if (getenv("DISPLAY")) {
		setenv("XDIALOG_HIGH_DIALOG_COMPAT", "true", 0);
		setenv("XDIALOG_FORCE_AUTOSIZE", "true", 0);
		execvp("Xdialog", argv);
	} else {
		strcpy(wrapped_cmd, argv[0]);
		strcat(wrapped_cmd, ".wrapped");
		execvp(wrapped_cmd, argv);
	}
	return 0;
}
