#include "thash.h"
#include <string.h>
#include "readst3.h"

#include "logit.h"

// is char a valid char for param name
char th_is_param_char(char c)
{
        if(
                (c>='a' && c<='z') ||
                (c>='A' && c<='Z') ||
                (c>='0' && c<='9') ||
                (c=='_')
        )
                { return 1; }
        return 0;
}

// starts from '=' char and reads back param name
// dst should have at least TH_MAXPLEN bytes of memory allocated
char th_readback_param_name(char *s0, char *s, char *dst)
{
        char buf[TH_MAXPLEN+1];
        buf[TH_MAXPLEN] = '\0';
        char pbuf = TH_MAXPLEN;

        *dst = '\0';
        char *p = s;

        while (! th_is_param_char(*p) ) // seek off equal sign
        {
                if (--p == s0)
                        { return 0; }
        }

        while (p>=s0)
        {
                if (th_is_param_char(*p))
                {
                        if (pbuf==0)
                                { return 0; }
                        buf[--pbuf] = *p;
                }
                else
                {
                        break;
                }

                p--;
        }

        strcpy(dst, (char *)&buf[pbuf]);

        return 1;
}

// dst should have at least TH_MAXVLEN bytes of memory allocated
char th_read_param_value(char *s, char *dst)
{
        char *p = s;
        *dst = '\0';

        char buf[TH_MAXVLEN+1];
        memset(&buf,0,sizeof(buf));
        char pbuf = 0;

        while ( (*p=='=')||(*p=='\'')||(*p==' ') )
        {
  		if (*p=='\'')
                	{ p++; break; }

                if (*p=='\0')
                        { return 0; }
                p++;
        }

        while ( *p != '\'' )
        {
                if ((pbuf == TH_MAXVLEN)||(*p=='\0'))
                        { return 0; }
                buf[pbuf++] = *p;
                p++;
        }

        strcpy(dst,(char *)&buf);
        return 1;
}

char Thash :: parseString(char *st)
{
        char *p = st;
        char buf1[TH_MAXPLEN];
        char buf2[TH_MAXVLEN];

        while (*p != '\0')
        {
                if (*p == '=')
                {
                        th_readback_param_name(st, p, (char*)&buf1);

                        th_read_param_value(p, (char*)&buf2);

//                        INFO(St("param='")+St((char*)&buf1)+St("'"));
//                        INFO(St("value='")+St((char*)&buf2)+St("'"));

                        this->set(buf1,buf2);

                }
                p++;
        }


        return 1;
}

St Thash :: toSt()
{
	St r;
	char first=1;

        for (unsigned char i=0; i < TH_MAXPARAMS; i++)
        {
		if (params[i][0] != '\0')
                {
			if (!first)
                        {
				r = r + St(", ");
                        }

                    	first = 0;

			r = r + St(params[i]) + St("='") +
                        	St(values[i]) + St("'");
                }
        }

	return r;
}

char Thash :: loadFile(char *filename)
{
        char *file = readtextfile(filename);

        if (file == NULL)
                { return 0; }

        char *br_nl = "\n\r";
        setBreakList(br_nl);

        char token[STRSIZE];
        memset(&token,0,STRSIZE);
        char *z = file;

        while (*readst(&z,(char*)&token) != '\0')
        {
                if (token[0] == ';')
                        { continue; }
                char *eq_pos = strstr((char*)&token,"=");
                if (eq_pos != NULL)
                {
                        char *p_end = eq_pos;

                        // cut off equal sign and spaces to get pure param
                        while (((*p_end == '=')||(*p_end == ' ')
                                || (*p_end == '\t')) && (p_end>=(char*)&token) )
                        {
                                *p_end = '\0';
                                p_end--;
                        }

                        if (token[0] == '\0')       // null param name
                                { continue; }

                        eq_pos++;
                        while (
                                ((*eq_pos==' ')||(*eq_pos=='\t'))
                                && (*eq_pos != '\0')
                                )
                        {
                                eq_pos++;
                        }

                        if (*eq_pos == '\0')  // null value
                                { continue; }

                        set((char*)&token,eq_pos);

//                        INFO(St((char*)&token) + St("=") + St(eq_pos));
                }
        }

        delete file;
        return 1;
};

char Thash :: del(char *param)
{
        for (unsigned char i=0; i < TH_MAXPARAMS; i++)
        {
                if (strcmp(param, params[i])==0)
                {
                        memset(&params[i],0,TH_MAXPLEN);
                        memset(&values[i],0,TH_MAXVLEN);
                        return 1;
                }
        }

        return 0;
};



char Thash :: set(char *param, char *value)
{
        if (value == NULL)
                { return 0; }

//	INFO(St("set ") + St(param) + St("='") +St(value)+St("'"));

        unsigned char first_free = TH_MAXPARAMS;
        unsigned char found = TH_MAXPARAMS;

        for (unsigned char i=0; i < TH_MAXPARAMS; i++)
        {
                if (strcmp(param, params[i])==0)
                {
                        found = i;
                        break;
                }

                if (first_free == TH_MAXPARAMS)
                {
                        if (params[i][0] == '\0')
                                { first_free = i; }
                }
        }

        if (found != TH_MAXPARAMS)
        {
                strcpy((char*)&values[found], value);
                return 1;
        }

        if (first_free != TH_MAXPARAMS)
        {
                strcpy((char*)&params[first_free], param);
                strcpy((char*)&values[first_free], value);
                return 2;
        }

        return 0;
};

char* Thash :: get(char *param)
{
//	INFO(St("get_p='")+St(param)+St("'"));

        for (unsigned char i=0; i < TH_MAXPARAMS; i++)
        {
//		INFO(St("get_c='")+St(params[i])+St("'"));
                if (strcmp(param, params[i])==0)
                {
//			INFO(St("get_f='")+St(values[i])+St("'"));
                        return (char*)values[i];
                }
        }

//	INFO(St("get_f='empty'"));
        return (char*)&empty;
};



Thash :: Thash()
{
	empty[0] = '\0';
	clear();
};

Thash :: ~Thash()
{
};

char Thash :: cmp(char *param, char *value)
{
        if (!strcmp(get(param),value))
        { return 1; }
        return 0;
} // compares hash value with given value

void Thash :: clear()
{
        for (unsigned char i=0; i<TH_MAXPARAMS; i++)
        {
                memset(&params[i],0,TH_MAXPLEN);
                memset(&values[i],0,TH_MAXVLEN);
        }
};

