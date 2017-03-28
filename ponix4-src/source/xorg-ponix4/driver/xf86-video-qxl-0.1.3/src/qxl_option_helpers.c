#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <xf86.h>
#include <xf86Opt.h>

#include "qxl_option_helpers.h"

int get_int_option(OptionInfoPtr options, int option_index,
                   const char *env_name)
{
    if (env_name && getenv(env_name)) {
        return atoi(getenv(env_name));
    }
    return options[option_index].value.num;
}

const char *get_str_option(OptionInfoPtr options, int option_index,
                           const char *env_name)
{
    if (getenv(env_name)) {
        return getenv(env_name);
    }
    return options[option_index].value.str;
}

int get_bool_option(OptionInfoPtr options, int option_index,
                     const char *env_name)
{
    if (getenv(env_name)) {
        /* we don't support the whole range of boolean true and
         * false values documented in man xorg.conf, just the c
         * convention - 0 is false, anything else is true, so
         * just like a number. */
        return !!atoi(getenv(env_name));
    }
    return options[option_index].value.bool;
}
