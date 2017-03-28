/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         ematch_parse
#define yylex           ematch_lex
#define yyerror         ematch_error
#define yylval          ematch_lval
#define yychar          ematch_char
#define yydebug         ematch_debug
#define yynerrs         ematch_nerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 12 "route/cls/ematch_syntax.y"

#include <netlink-local.h>
#include <netlink-tc.h>
#include <netlink/netlink.h>
#include <netlink/utils.h>
#include <netlink/route/pktloc.h>
#include <netlink/route/cls/ematch.h>
#include <netlink/route/cls/ematch/cmp.h>
#include <netlink/route/cls/ematch/nbyte.h>
#include <netlink/route/cls/ematch/text.h>
#include <netlink/route/cls/ematch/meta.h>

#define META_ALLOC rtnl_meta_value_alloc_id
#define META_ID(name) TCF_META_ID_##name
#define META_INT TCF_META_TYPE_INT
#define META_VAR TCF_META_TYPE_VAR


/* Line 189 of yacc.c  */
#line 99 "route/cls/ematch_syntax.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ERROR = 258,
     LOGIC = 259,
     NOT = 260,
     OPERAND = 261,
     NUMBER = 262,
     ALIGN = 263,
     LAYER = 264,
     KW_OPEN = 265,
     KW_CLOSE = 266,
     KW_PLUS = 267,
     KW_MASK = 268,
     KW_SHIFT = 269,
     KW_AT = 270,
     EMATCH_CMP = 271,
     EMATCH_NBYTE = 272,
     EMATCH_TEXT = 273,
     EMATCH_META = 274,
     KW_EQ = 275,
     KW_GT = 276,
     KW_LT = 277,
     KW_FROM = 278,
     KW_TO = 279,
     META_RANDOM = 280,
     META_LOADAVG_0 = 281,
     META_LOADAVG_1 = 282,
     META_LOADAVG_2 = 283,
     META_DEV = 284,
     META_PRIO = 285,
     META_PROTO = 286,
     META_PKTTYPE = 287,
     META_PKTLEN = 288,
     META_DATALEN = 289,
     META_MACLEN = 290,
     META_MARK = 291,
     META_TCINDEX = 292,
     META_RTCLASSID = 293,
     META_RTIIF = 294,
     META_SK_FAMILY = 295,
     META_SK_STATE = 296,
     META_SK_REUSE = 297,
     META_SK_REFCNT = 298,
     META_SK_RCVBUF = 299,
     META_SK_SNDBUF = 300,
     META_SK_SHUTDOWN = 301,
     META_SK_PROTO = 302,
     META_SK_TYPE = 303,
     META_SK_RMEM_ALLOC = 304,
     META_SK_WMEM_ALLOC = 305,
     META_SK_WMEM_QUEUED = 306,
     META_SK_RCV_QLEN = 307,
     META_SK_SND_QLEN = 308,
     META_SK_ERR_QLEN = 309,
     META_SK_FORWARD_ALLOCS = 310,
     META_SK_ALLOCS = 311,
     META_SK_ROUTE_CAPS = 312,
     META_SK_HASH = 313,
     META_SK_LINGERTIME = 314,
     META_SK_ACK_BACKLOG = 315,
     META_SK_MAX_ACK_BACKLOG = 316,
     META_SK_PRIO = 317,
     META_SK_RCVLOWAT = 318,
     META_SK_RCVTIMEO = 319,
     META_SK_SNDTIMEO = 320,
     META_SK_SENDMSG_OFF = 321,
     META_SK_WRITE_PENDING = 322,
     META_VLAN = 323,
     META_RXHASH = 324,
     META_DEVNAME = 325,
     META_SK_BOUND_IF = 326,
     STR = 327,
     QUOTED = 328
   };
#endif
/* Tokens.  */
#define ERROR 258
#define LOGIC 259
#define NOT 260
#define OPERAND 261
#define NUMBER 262
#define ALIGN 263
#define LAYER 264
#define KW_OPEN 265
#define KW_CLOSE 266
#define KW_PLUS 267
#define KW_MASK 268
#define KW_SHIFT 269
#define KW_AT 270
#define EMATCH_CMP 271
#define EMATCH_NBYTE 272
#define EMATCH_TEXT 273
#define EMATCH_META 274
#define KW_EQ 275
#define KW_GT 276
#define KW_LT 277
#define KW_FROM 278
#define KW_TO 279
#define META_RANDOM 280
#define META_LOADAVG_0 281
#define META_LOADAVG_1 282
#define META_LOADAVG_2 283
#define META_DEV 284
#define META_PRIO 285
#define META_PROTO 286
#define META_PKTTYPE 287
#define META_PKTLEN 288
#define META_DATALEN 289
#define META_MACLEN 290
#define META_MARK 291
#define META_TCINDEX 292
#define META_RTCLASSID 293
#define META_RTIIF 294
#define META_SK_FAMILY 295
#define META_SK_STATE 296
#define META_SK_REUSE 297
#define META_SK_REFCNT 298
#define META_SK_RCVBUF 299
#define META_SK_SNDBUF 300
#define META_SK_SHUTDOWN 301
#define META_SK_PROTO 302
#define META_SK_TYPE 303
#define META_SK_RMEM_ALLOC 304
#define META_SK_WMEM_ALLOC 305
#define META_SK_WMEM_QUEUED 306
#define META_SK_RCV_QLEN 307
#define META_SK_SND_QLEN 308
#define META_SK_ERR_QLEN 309
#define META_SK_FORWARD_ALLOCS 310
#define META_SK_ALLOCS 311
#define META_SK_ROUTE_CAPS 312
#define META_SK_HASH 313
#define META_SK_LINGERTIME 314
#define META_SK_ACK_BACKLOG 315
#define META_SK_MAX_ACK_BACKLOG 316
#define META_SK_PRIO 317
#define META_SK_RCVLOWAT 318
#define META_SK_RCVTIMEO 319
#define META_SK_SNDTIMEO 320
#define META_SK_SENDMSG_OFF 321
#define META_SK_WRITE_PENDING 322
#define META_VLAN 323
#define META_RXHASH 324
#define META_DEVNAME 325
#define META_SK_BOUND_IF 326
#define STR 327
#define QUOTED 328




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 39 "route/cls/ematch_syntax.y"

	struct tcf_em_cmp	cmp;
	struct ematch_quoted	q;
	struct rtnl_ematch *	e;
	struct rtnl_pktloc *	loc;
	struct rtnl_meta_value *mv;
	uint32_t		i;
	uint64_t		i64;
	char *			s;



/* Line 214 of yacc.c  */
#line 294 "route/cls/ematch_syntax.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 50 "route/cls/ematch_syntax.y"

extern int ematch_lex(YYSTYPE *, void *);

static void yyerror(void *scanner, char **errp, struct nl_list_head *root, const char *msg)
{
	if (msg)
            *errp = strdup(msg);
        else
	    *errp = NULL;
}


/* Line 264 of yacc.c  */
#line 319 "route/cls/ematch_syntax.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  26
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   138

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  84
/* YYNRULES -- Number of states.  */
#define YYNSTATES  118

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    12,    15,    17,    19,
      26,    34,    41,    45,    50,    52,    56,    57,    60,    61,
      64,    66,    68,    72,    75,    77,    79,    81,    83,    85,
      87,    89,    91,    93,    95,    97,    99,   101,   103,   105,
     107,   109,   111,   113,   115,   117,   119,   121,   123,   125,
     127,   129,   131,   133,   135,   137,   139,   141,   143,   145,
     147,   149,   151,   153,   155,   157,   159,   161,   163,   165,
     167,   169,   171,   173,   175,   181,   182,   185,   188,   189,
     192,   193,   196,   198,   200
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      75,     0,    -1,    -1,    76,    -1,    77,    -1,    77,     4,
      76,    -1,     5,    78,    -1,    78,    -1,    79,    -1,    17,
      10,    87,    20,    86,    11,    -1,    18,    10,    72,    73,
      81,    82,    11,    -1,    19,    10,    83,    91,    83,    11,
      -1,    10,    76,    11,    -1,    16,    10,    80,    11,    -1,
      80,    -1,    87,    91,     7,    -1,    -1,    23,    87,    -1,
      -1,    24,    87,    -1,    73,    -1,     7,    -1,    84,    90,
      89,    -1,    85,    90,    -1,    25,    -1,    26,    -1,    27,
      -1,    28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,
      -1,    33,    -1,    34,    -1,    35,    -1,    36,    -1,    37,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,
      -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,
      -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,    52,
      -1,    53,    -1,    54,    -1,    55,    -1,    56,    -1,    57,
      -1,    58,    -1,    59,    -1,    60,    -1,    61,    -1,    62,
      -1,    63,    -1,    64,    -1,    65,    -1,    66,    -1,    67,
      -1,    68,    -1,    69,    -1,    70,    -1,    71,    -1,    73,
      -1,    72,    -1,    72,    -1,    88,     9,    12,     7,    89,
      -1,    -1,     8,    15,    -1,     7,    15,    -1,    -1,    13,
       7,    -1,    -1,    14,     7,    -1,    20,    -1,    21,    -1,
      22,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   148,   148,   150,   157,   161,   173,   178,   186,   201,
     219,   246,   265,   293,   295,   300,   321,   322,   328,   329,
     334,   336,   338,   340,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
     361,   362,   363,   364,   365,   366,   367,   368,   369,   370,
     371,   372,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   393,
     394,   401,   405,   434,   447,   473,   474,   476,   482,   483,
     489,   490,   495,   497,   499
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERROR", "LOGIC", "NOT", "OPERAND",
  "NUMBER", "ALIGN", "LAYER", "\"(\"", "\")\"", "\"+\"", "\"mask\"",
  "\">>\"", "\"at\"", "\"cmp\"", "\"pattern\"", "\"text\"", "\"meta\"",
  "\"=\"", "\">\"", "\"<\"", "\"from\"", "\"to\"", "\"random\"",
  "\"loadavg_0\"", "\"loadavg_1\"", "\"loadavg_2\"", "\"dev\"", "\"prio\"",
  "\"proto\"", "\"pkttype\"", "\"pktlen\"", "\"datalen\"", "\"maclen\"",
  "\"mark\"", "\"tcindex\"", "\"rtclassid\"", "\"rtiif\"", "\"sk_family\"",
  "\"sk_state\"", "\"sk_reuse\"", "\"sk_refcnt\"", "\"sk_rcvbuf\"",
  "\"sk_sndbuf\"", "\"sk_shutdown\"", "\"sk_proto\"", "\"sk_type\"",
  "\"sk_rmem_alloc\"", "\"sk_wmem_alloc\"", "\"sk_wmem_queued\"",
  "\"sk_rcv_qlen\"", "\"sk_snd_qlen\"", "\"sk_err_qlen\"",
  "\"sk_forward_allocs\"", "\"sk_allocs\"", "\"sk_route_caps\"",
  "\"sk_hash\"", "\"sk_lingertime\"", "\"sk_ack_backlog\"",
  "\"sk_max_ack_backlog\"", "\"sk_prio\"", "\"sk_rcvlowat\"",
  "\"sk_rcvtimeo\"", "\"sk_sndtimeo\"", "\"sk_sendmsg_off\"",
  "\"sk_write_pending\"", "\"vlan\"", "\"rxhash\"", "\"devname\"",
  "\"sk_bound_if\"", "STR", "QUOTED", "$accept", "input", "expr", "match",
  "ematch", "cmp_match", "cmp_expr", "text_from", "text_to", "meta_value",
  "meta_int_id", "meta_var_id", "pattern", "pktloc", "align", "mask",
  "shift", "operand", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    75,    75,    76,    76,    77,    77,    78,    78,
      78,    78,    78,    79,    79,    80,    81,    81,    82,    82,
      83,    83,    83,    83,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    88,    89,    89,
      90,    90,    91,    91,    91
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     3,     2,     1,     1,     6,
       7,     6,     3,     4,     1,     3,     0,     2,     0,     2,
       1,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     0,     2,     2,     0,     2,
       0,     2,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,    75,     0,     0,    75,     0,     0,     0,     0,    73,
       0,     3,     4,     7,     8,    14,     0,     0,     6,    77,
      76,     0,    75,    75,     0,     0,     1,    75,    82,    83,
      84,     0,     0,    12,     0,     0,     0,    21,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    20,     0,    80,    80,     5,
      15,     0,    13,     0,    16,     0,     0,    78,    23,    78,
      72,    71,     0,    75,    18,     0,    81,     0,    22,    74,
       9,    17,    75,     0,    11,    79,    19,    10
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    10,    11,    12,    13,    14,    15,   104,   113,    86,
      87,    88,   102,    16,    17,   108,    97,    31
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -63
static const yytype_int8 yypact[] =
{
      -4,    15,   -13,    -8,    11,    10,    14,    25,    29,   -63,
      26,   -63,    37,   -63,   -63,   -63,    16,    33,   -63,   -63,
     -63,    32,     1,     1,   -28,    65,   -63,    11,   -63,   -63,
     -63,    38,    34,   -63,    36,    28,   -24,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,    16,    39,    39,   -63,
     -63,    43,   -63,   -62,    31,    65,    44,    42,   -63,    42,
     -63,   -63,    41,     1,    35,    45,   -63,    50,   -63,   -63,
     -63,   -63,     1,    47,   -63,   -63,   -63,   -63
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -63,   -63,    13,   -63,    59,   -63,    40,   -63,   -63,   -34,
     -63,   -63,   -63,   -23,   -63,   -36,   -22,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -76
static const yytype_int8 yytable[] =
{
      35,     1,    19,     2,     3,   -75,     4,    20,     2,     3,
     100,   101,     5,     6,     7,     8,     1,    21,     2,     3,
      22,     4,     2,     3,    23,     4,    26,     5,     6,     7,
       8,     5,     6,     7,     8,    24,    28,    29,    30,    25,
      89,    27,    32,    33,    36,    90,    91,    92,    93,    94,
      99,   106,   110,    96,   103,   107,   114,   115,   117,   112,
      18,   105,    34,   109,     0,    95,    98,     0,     9,     0,
       0,     0,    37,     9,     0,     0,     0,     0,     0,     0,
     111,     0,     0,     9,     0,     0,     0,     9,     0,   116,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,     0,    85
};

static const yytype_int8 yycheck[] =
{
      23,     5,    15,     7,     8,     9,    10,    15,     7,     8,
      72,    73,    16,    17,    18,    19,     5,     4,     7,     8,
      10,    10,     7,     8,    10,    10,     0,    16,    17,    18,
      19,    16,    17,    18,    19,    10,    20,    21,    22,    10,
      27,     4,     9,    11,    72,     7,    12,    11,    20,    73,
       7,     7,    11,    14,    23,    13,    11,     7,    11,    24,
       1,    95,    22,    99,    -1,    86,    88,    -1,    72,    -1,
      -1,    -1,     7,    72,    -1,    -1,    -1,    -1,    -1,    -1,
     103,    -1,    -1,    72,    -1,    -1,    -1,    72,    -1,   112,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    -1,    73
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     7,     8,    10,    16,    17,    18,    19,    72,
      75,    76,    77,    78,    79,    80,    87,    88,    78,    15,
      15,    76,    10,    10,    10,    10,     0,     4,    20,    21,
      22,    91,     9,    11,    80,    87,    72,     7,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    73,    83,    84,    85,    76,
       7,    12,    11,    20,    73,    91,    14,    90,    90,     7,
      72,    73,    86,    23,    81,    83,     7,    13,    89,    89,
      11,    87,    24,    82,    11,     7,    87,    11
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (scanner, errp, root, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, scanner, errp, root); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *scanner, char **errp, struct nl_list_head *root)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner, errp, root)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void *scanner;
    char **errp;
    struct nl_list_head *root;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (scanner);
  YYUSE (errp);
  YYUSE (root);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *scanner, char **errp, struct nl_list_head *root)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, scanner, errp, root)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void *scanner;
    char **errp;
    struct nl_list_head *root;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner, errp, root);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, void *scanner, char **errp, struct nl_list_head *root)
#else
static void
yy_reduce_print (yyvsp, yyrule, scanner, errp, root)
    YYSTYPE *yyvsp;
    int yyrule;
    void *scanner;
    char **errp;
    struct nl_list_head *root;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , scanner, errp, root);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, scanner, errp, root); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner, char **errp, struct nl_list_head *root)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, scanner, errp, root)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    void *scanner;
    char **errp;
    struct nl_list_head *root;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  YYUSE (errp);
  YYUSE (root);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 72: /* "STR" */

/* Line 1009 of yacc.c  */
#line 139 "route/cls/ematch_syntax.y"
	{ free((yyvaluep->s)); NL_DBG(2, "string destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1372 "route/cls/ematch_syntax.c"
	break;
      case 73: /* "QUOTED" */

/* Line 1009 of yacc.c  */
#line 141 "route/cls/ematch_syntax.y"
	{ free((yyvaluep->q).data); NL_DBG(2, "quoted destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1381 "route/cls/ematch_syntax.c"
	break;
      case 81: /* "text_from" */

/* Line 1009 of yacc.c  */
#line 140 "route/cls/ematch_syntax.y"
	{ rtnl_pktloc_put((yyvaluep->loc)); NL_DBG(2, "pktloc destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1390 "route/cls/ematch_syntax.c"
	break;
      case 82: /* "text_to" */

/* Line 1009 of yacc.c  */
#line 140 "route/cls/ematch_syntax.y"
	{ rtnl_pktloc_put((yyvaluep->loc)); NL_DBG(2, "pktloc destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1399 "route/cls/ematch_syntax.c"
	break;
      case 83: /* "meta_value" */

/* Line 1009 of yacc.c  */
#line 142 "route/cls/ematch_syntax.y"
	{ rtnl_meta_value_put((yyvaluep->mv)); NL_DBG(2, "meta value destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1408 "route/cls/ematch_syntax.c"
	break;
      case 86: /* "pattern" */

/* Line 1009 of yacc.c  */
#line 141 "route/cls/ematch_syntax.y"
	{ free((yyvaluep->q).data); NL_DBG(2, "quoted destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1417 "route/cls/ematch_syntax.c"
	break;
      case 87: /* "pktloc" */

/* Line 1009 of yacc.c  */
#line 140 "route/cls/ematch_syntax.y"
	{ rtnl_pktloc_put((yyvaluep->loc)); NL_DBG(2, "pktloc destructor\n"); };

/* Line 1009 of yacc.c  */
#line 1426 "route/cls/ematch_syntax.c"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void *scanner, char **errp, struct nl_list_head *root);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *scanner, char **errp, struct nl_list_head *root)
#else
int
yyparse (scanner, errp, root)
    void *scanner;
    char **errp;
    struct nl_list_head *root;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1464 of yacc.c  */
#line 151 "route/cls/ematch_syntax.y"
    {
			nl_list_add_tail(root, &(yyvsp[(1) - (1)].e)->e_list);
		}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 158 "route/cls/ematch_syntax.y"
    {
			(yyval.e) = (yyvsp[(1) - (1)].e);
		}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 162 "route/cls/ematch_syntax.y"
    {
			rtnl_ematch_set_flags((yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].i));

			/* make ematch new head */
			nl_list_add_tail(&(yyvsp[(1) - (3)].e)->e_list, &(yyvsp[(3) - (3)].e)->e_list);

			(yyval.e) = (yyvsp[(1) - (3)].e);
		}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 174 "route/cls/ematch_syntax.y"
    {
			rtnl_ematch_set_flags((yyvsp[(2) - (2)].e), TCF_EM_INVERT);
			(yyval.e) = (yyvsp[(2) - (2)].e);
		}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 179 "route/cls/ematch_syntax.y"
    {
			(yyval.e) = (yyvsp[(1) - (1)].e);
		}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 187 "route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_CMP) < 0)
				BUG();

			rtnl_ematch_cmp_set(e, &(yyvsp[(1) - (1)].cmp));
			(yyval.e) = e;
		}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 202 "route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_NBYTE) < 0)
				BUG();

			rtnl_ematch_nbyte_set_offset(e, (yyvsp[(3) - (6)].loc)->layer, (yyvsp[(3) - (6)].loc)->offset);
			rtnl_pktloc_put((yyvsp[(3) - (6)].loc));
			rtnl_ematch_nbyte_set_pattern(e, (uint8_t *) (yyvsp[(5) - (6)].q).data, (yyvsp[(5) - (6)].q).index);

			(yyval.e) = e;
		}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 220 "route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_TEXT) < 0)
				BUG();

			rtnl_ematch_text_set_algo(e, (yyvsp[(3) - (7)].s));
			rtnl_ematch_text_set_pattern(e, (yyvsp[(4) - (7)].q).data, (yyvsp[(4) - (7)].q).index);

			if ((yyvsp[(5) - (7)].loc)) {
				rtnl_ematch_text_set_from(e, (yyvsp[(5) - (7)].loc)->layer, (yyvsp[(5) - (7)].loc)->offset);
				rtnl_pktloc_put((yyvsp[(5) - (7)].loc));
			}

			if ((yyvsp[(6) - (7)].loc)) {
				rtnl_ematch_text_set_to(e, (yyvsp[(6) - (7)].loc)->layer, (yyvsp[(6) - (7)].loc)->offset);
				rtnl_pktloc_put((yyvsp[(6) - (7)].loc));
			}

			(yyval.e) = e;
		}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 247 "route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_META) < 0)
				BUG();

			rtnl_ematch_meta_set_lvalue(e, (yyvsp[(3) - (6)].mv));
			rtnl_ematch_meta_set_rvalue(e, (yyvsp[(5) - (6)].mv));
			rtnl_ematch_meta_set_operand(e, (yyvsp[(4) - (6)].i));

			(yyval.e) = e;
		}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 266 "route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_CONTAINER) < 0)
				BUG();

			/* Make e->childs the list head of a the ematch sequence */
			nl_list_add_tail(&e->e_childs, &(yyvsp[(2) - (3)].e)->e_list);

			(yyval.e) = e;
		}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 294 "route/cls/ematch_syntax.y"
    { (yyval.cmp) = (yyvsp[(3) - (4)].cmp); }
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 296 "route/cls/ematch_syntax.y"
    { (yyval.cmp) = (yyvsp[(1) - (1)].cmp); }
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 301 "route/cls/ematch_syntax.y"
    {
			if ((yyvsp[(1) - (3)].loc)->align == TCF_EM_ALIGN_U16 ||
			    (yyvsp[(1) - (3)].loc)->align == TCF_EM_ALIGN_U32)
				(yyval.cmp).flags = TCF_EM_CMP_TRANS;

			memset(&(yyval.cmp), 0, sizeof((yyval.cmp)));

			(yyval.cmp).mask = (yyvsp[(1) - (3)].loc)->mask;
			(yyval.cmp).off = (yyvsp[(1) - (3)].loc)->offset;
			(yyval.cmp).align = (yyvsp[(1) - (3)].loc)->align;
			(yyval.cmp).layer = (yyvsp[(1) - (3)].loc)->layer;
			(yyval.cmp).opnd = (yyvsp[(2) - (3)].i);
			(yyval.cmp).val = (yyvsp[(3) - (3)].i);

			rtnl_pktloc_put((yyvsp[(1) - (3)].loc));
		}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 321 "route/cls/ematch_syntax.y"
    { (yyval.loc) = NULL; }
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 323 "route/cls/ematch_syntax.y"
    { (yyval.loc) = (yyvsp[(2) - (2)].loc); }
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 328 "route/cls/ematch_syntax.y"
    { (yyval.loc) = NULL; }
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 330 "route/cls/ematch_syntax.y"
    { (yyval.loc) = (yyvsp[(2) - (2)].loc); }
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 335 "route/cls/ematch_syntax.y"
    { (yyval.mv) = rtnl_meta_value_alloc_var((yyvsp[(1) - (1)].q).data, (yyvsp[(1) - (1)].q).len); }
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 337 "route/cls/ematch_syntax.y"
    { (yyval.mv) = rtnl_meta_value_alloc_int((yyvsp[(1) - (1)].i)); }
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 339 "route/cls/ematch_syntax.y"
    { (yyval.mv) = META_ALLOC(META_INT, (yyvsp[(1) - (3)].i), (yyvsp[(2) - (3)].i), (yyvsp[(3) - (3)].i64)); }
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 341 "route/cls/ematch_syntax.y"
    { (yyval.mv) = META_ALLOC(META_VAR, (yyvsp[(1) - (2)].i), (yyvsp[(2) - (2)].i), 0); }
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 345 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RANDOM); }
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 346 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(LOADAVG_0); }
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 347 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(LOADAVG_1); }
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 348 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(LOADAVG_2); }
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 349 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(DEV); }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 350 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PRIORITY); }
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 351 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PROTOCOL); }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 352 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PKTTYPE); }
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 353 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PKTLEN); }
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 354 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(DATALEN); }
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 355 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(MACLEN); }
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 356 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(NFMARK); }
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 357 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(TCINDEX); }
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 358 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RTCLASSID); }
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 359 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RTIIF); }
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 360 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_FAMILY); }
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 361 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_STATE); }
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 362 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_REUSE); }
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 363 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_REFCNT); }
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 364 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCVBUF); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 365 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SNDBUF); }
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 366 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SHUTDOWN); }
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 367 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_PROTO); }
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 368 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_TYPE); }
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 369 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RMEM_ALLOC); }
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 370 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_WMEM_ALLOC); }
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 371 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_WMEM_QUEUED); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 372 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCV_QLEN); }
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 373 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SND_QLEN); }
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 374 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ERR_QLEN); }
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 375 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_FORWARD_ALLOCS); }
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 376 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ALLOCS); }
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 377 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ROUTE_CAPS); }
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 378 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_HASH); }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 379 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_LINGERTIME); }
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 380 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ACK_BACKLOG); }
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 381 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_MAX_ACK_BACKLOG); }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 382 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_PRIO); }
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 383 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCVLOWAT); }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 384 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCVTIMEO); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 385 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SNDTIMEO); }
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 386 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SENDMSG_OFF); }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 387 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_WRITE_PENDING); }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 388 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(VLAN_TAG); }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 389 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RXHASH); }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 393 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(DEV); }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 394 "route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_BOUND_IF); }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 402 "route/cls/ematch_syntax.y"
    {
			(yyval.q) = (yyvsp[(1) - (1)].q);
		}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 406 "route/cls/ematch_syntax.y"
    {
			struct nl_addr *addr;

			if (nl_addr_parse((yyvsp[(1) - (1)].s), AF_UNSPEC, &addr) == 0) {
				(yyval.q).len = nl_addr_get_len(addr);

				(yyval.q).index = min_t(int, (yyval.q).len, nl_addr_get_prefixlen(addr)/8);

				if (!((yyval.q).data = calloc(1, (yyval.q).len))) {
					nl_addr_put(addr);
					YYABORT;
				}

				memcpy((yyval.q).data, nl_addr_get_binary_addr(addr), (yyval.q).len);
				nl_addr_put(addr);
			} else {
				if (asprintf(errp, "invalid pattern \"%s\"", (yyvsp[(1) - (1)].s)) == -1)
					*errp = NULL;
				YYABORT;
			}
		}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 435 "route/cls/ematch_syntax.y"
    {
			struct rtnl_pktloc *loc;

			if (rtnl_pktloc_lookup((yyvsp[(1) - (1)].s), &loc) < 0) {
				if (asprintf(errp, "Packet location \"%s\" not found", (yyvsp[(1) - (1)].s)) == -1)
					*errp = NULL;
				YYABORT;
			}

			(yyval.loc) = loc;
		}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 448 "route/cls/ematch_syntax.y"
    {
			struct rtnl_pktloc *loc;

			if ((yyvsp[(5) - (5)].i64) && (!(yyvsp[(1) - (5)].i) || (yyvsp[(1) - (5)].i) > TCF_EM_ALIGN_U32)) {
				*errp = strdup("mask only allowed for alignments u8|u16|u32");
				YYABORT;
			}

			if (!(loc = rtnl_pktloc_alloc())) {
				*errp = strdup("Unable to allocate packet location object");
				YYABORT;
			}

			loc->name = strdup("<USER-DEFINED>");
			loc->align = (yyvsp[(1) - (5)].i);
			loc->layer = (yyvsp[(2) - (5)].i);
			loc->offset = (yyvsp[(4) - (5)].i);
			loc->mask = (yyvsp[(5) - (5)].i64);

			(yyval.loc) = loc;
		}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 473 "route/cls/ematch_syntax.y"
    { (yyval.i) = 0; }
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 475 "route/cls/ematch_syntax.y"
    { (yyval.i) = (yyvsp[(1) - (2)].i); }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 477 "route/cls/ematch_syntax.y"
    { (yyval.i) = (yyvsp[(1) - (2)].i); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 482 "route/cls/ematch_syntax.y"
    { (yyval.i64) = 0; }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 484 "route/cls/ematch_syntax.y"
    { (yyval.i64) = (yyvsp[(2) - (2)].i); }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 489 "route/cls/ematch_syntax.y"
    { (yyval.i) = 0; }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 491 "route/cls/ematch_syntax.y"
    { (yyval.i) = (yyvsp[(2) - (2)].i); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 496 "route/cls/ematch_syntax.y"
    { (yyval.i) = TCF_EM_OPND_EQ; }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 498 "route/cls/ematch_syntax.y"
    { (yyval.i) = TCF_EM_OPND_GT; }
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 500 "route/cls/ematch_syntax.y"
    { (yyval.i) = TCF_EM_OPND_LT; }
    break;



/* Line 1464 of yacc.c  */
#line 2474 "route/cls/ematch_syntax.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, errp, root, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (scanner, errp, root, yymsg);
	  }
	else
	  {
	    yyerror (scanner, errp, root, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, scanner, errp, root);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, scanner, errp, root);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, errp, root, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, scanner, errp, root);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, scanner, errp, root);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



