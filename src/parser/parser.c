/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser/parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_walker.h"
#include "../core/scheduler.h"
#include "../core/signal.h"
#include "../core/utils.h"
#include "../output/vcd.h"
#include "../analysis/dependency.h"
#include "../output/trace.h"
#include "../output/vcd.h"
#include "../sim/parallel.h"
#include "../core/event.h"
#include "../sim/sequential.h"
#include "../core/delta.h"
#include "../core/event_queue.h"
#include <omp.h>

void yyerror(const char* s);
int yylex();
extern char* yytext;
struct ASTNode;
extern struct ASTNode* ast_root;
extern struct ASTNode* ast_entity;

// CHIRAG 19-03-26 :: temporary storage during parsing
// ports and processes are collected here as they are parsed
// then copied into the entity/arch node when that rule completes
ASTNode* temp_ports[32];
ASTNode* temp_stmts[32];
int temp_stmt_count = 0;
int temp_port_count = 0;
ASTNode* temp_processes[32];
int temp_process_count = 0;
char* temp_sens[32];
int temp_sens_count = 0;
// CHIRAG 18-04-26 :: temp storage for internal signal declarations
// signal CARRY : bit; between IS and BEGIN goes here
// separate from temp_ports ... ports belong to entity ... these belong to arch
ASTNode* temp_arch_signals[32];
int temp_arch_signal_count = 0;

#line 115 "src/parser/parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_ZERO = 4,                       /* ZERO  */
  YYSYMBOL_ONE = 5,                        /* ONE  */
  YYSYMBOL_ENTITY = 6,                     /* ENTITY  */
  YYSYMBOL_ARCHITECTURE = 7,               /* ARCHITECTURE  */
  YYSYMBOL_PORT = 8,                       /* PORT  */
  YYSYMBOL_PROCESS = 9,                    /* PROCESS  */
  YYSYMBOL_BEGIN_TOK = 10,                 /* BEGIN_TOK  */
  YYSYMBOL_END_TOK = 11,                   /* END_TOK  */
  YYSYMBOL_IS = 12,                        /* IS  */
  YYSYMBOL_OF = 13,                        /* OF  */
  YYSYMBOL_IN_TOK = 14,                    /* IN_TOK  */
  YYSYMBOL_OUT_TOK = 15,                   /* OUT_TOK  */
  YYSYMBOL_BIT = 16,                       /* BIT  */
  YYSYMBOL_AND_TOK = 17,                   /* AND_TOK  */
  YYSYMBOL_OR_TOK = 18,                    /* OR_TOK  */
  YYSYMBOL_NOT_TOK = 19,                   /* NOT_TOK  */
  YYSYMBOL_XOR_TOK = 20,                   /* XOR_TOK  */
  YYSYMBOL_IF = 21,                        /* IF  */
  YYSYMBOL_THEN = 22,                      /* THEN  */
  YYSYMBOL_SIGNAL = 23,                    /* SIGNAL  */
  YYSYMBOL_ASSIGN = 24,                    /* ASSIGN  */
  YYSYMBOL_25_ = 25,                       /* '('  */
  YYSYMBOL_26_ = 26,                       /* ')'  */
  YYSYMBOL_27_ = 27,                       /* ';'  */
  YYSYMBOL_28_ = 28,                       /* ':'  */
  YYSYMBOL_29_ = 29,                       /* ','  */
  YYSYMBOL_30_ = 30,                       /* '='  */
  YYSYMBOL_YYACCEPT = 31,                  /* $accept  */
  YYSYMBOL_program = 32,                   /* program  */
  YYSYMBOL_entity_decl = 33,               /* entity_decl  */
  YYSYMBOL_port_list = 34,                 /* port_list  */
  YYSYMBOL_port_item = 35,                 /* port_item  */
  YYSYMBOL_signal_decl_list = 36,          /* signal_decl_list  */
  YYSYMBOL_signal_decl = 37,               /* signal_decl  */
  YYSYMBOL_identifier_list = 38,           /* identifier_list  */
  YYSYMBOL_process_list = 39,              /* process_list  */
  YYSYMBOL_architecture_decl = 40,         /* architecture_decl  */
  YYSYMBOL_process_decl = 41,              /* process_decl  */
  YYSYMBOL_statement_list = 42,            /* statement_list  */
  YYSYMBOL_statement = 43,                 /* statement  */
  YYSYMBOL_signal_assignment = 44,         /* signal_assignment  */
  YYSYMBOL_if_statement = 45,              /* if_statement  */
  YYSYMBOL_bit_literal = 46,               /* bit_literal  */
  YYSYMBOL_expression = 47                 /* expression  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   99

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  17
/* YYNRULES -- Number of rules.  */
#define YYNRULES  31
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  93

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   279


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      25,    26,     2,     2,    29,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    28,    27,
       2,    30,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    78,    78,    88,   102,   104,   109,   118,   130,   132,
     137,   151,   153,   159,   161,   166,   183,   201,   220,   222,
     227,   229,   234,   244,   266,   267,   271,   277,   284,   291,
     298,   304
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER", "ZERO",
  "ONE", "ENTITY", "ARCHITECTURE", "PORT", "PROCESS", "BEGIN_TOK",
  "END_TOK", "IS", "OF", "IN_TOK", "OUT_TOK", "BIT", "AND_TOK", "OR_TOK",
  "NOT_TOK", "XOR_TOK", "IF", "THEN", "SIGNAL", "ASSIGN", "'('", "')'",
  "';'", "':'", "','", "'='", "$accept", "program", "entity_decl",
  "port_list", "port_item", "signal_decl_list", "signal_decl",
  "identifier_list", "process_list", "architecture_decl", "process_decl",
  "statement_list", "statement", "signal_assignment", "if_statement",
  "bit_literal", "expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-63)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       3,    10,    27,    29,    31,   -63,    44,   -63,    46,    42,
      32,    53,    55,    47,   -63,    19,   -63,    20,    -5,    34,
      55,    36,    57,    54,    59,    -4,   -63,    56,   -63,    49,
      50,   -63,    39,    23,   -63,    40,    54,   -63,    66,   -63,
     -63,    55,    67,   -63,    58,    33,    45,    11,    51,    52,
      68,   -63,    63,   -63,   -63,    60,     1,   -63,    61,    72,
      -1,   -63,   -63,   -63,    -2,    62,    71,   -63,   -63,    -2,
      -2,     8,    48,    64,   -63,    13,    -2,    -2,    -2,   -63,
     -63,   -63,    73,   -63,   -63,   -63,    21,   -63,     1,     0,
      65,    69,   -63
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     1,     0,     2,     0,     0,
       0,     0,     0,     0,    11,     0,     4,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     8,     0,     5,     0,
       0,    12,     0,     0,    13,     0,     0,     9,     0,     6,
       7,     0,     0,    14,     0,     0,     0,     0,     0,     0,
       0,     3,     0,    16,    10,     0,     0,    15,     0,     0,
       0,    18,    20,    21,     0,     0,     0,    19,    26,     0,
       0,     0,     0,     0,    30,     0,     0,     0,     0,    22,
      24,    25,     0,    17,    31,    27,    29,    28,     0,     0,
       0,     0,    23
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -63,   -63,   -63,   -63,    70,   -63,    74,    35,    41,   -63,
     -21,    -7,   -60,   -63,   -63,   -63,   -62
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,    15,    16,    25,    26,    17,    33,     7,
      34,    60,    61,    62,    63,    82,    71
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      67,    68,    58,    58,    58,    23,    36,    74,    75,     1,
      66,    90,    43,     4,    85,    86,    87,    69,    24,    24,
      59,    59,    59,    70,    43,    76,    77,     5,    78,    67,
      76,    77,    32,    78,    42,    79,     6,    52,    76,    84,
      22,    78,    32,     8,    50,    19,    20,     9,    21,    22,
      29,    30,    80,    81,    10,    11,    13,    12,    14,    18,
      31,    27,    35,    32,    41,    39,    40,    38,    44,    46,
      48,    55,    51,    56,    49,    65,    47,    45,    53,    54,
      73,    89,     0,     0,     0,    64,    91,    57,     0,     0,
      28,    83,    72,     0,     0,    88,    92,     0,     0,    37
};

static const yytype_int8 yycheck[] =
{
      60,     3,     3,     3,     3,    10,    10,    69,    70,     6,
      11,    11,    33,     3,    76,    77,    78,    19,    23,    23,
      21,    21,    21,    25,    45,    17,    18,     0,    20,    89,
      17,    18,     9,    20,    11,    27,     7,    26,    17,    26,
      29,    20,     9,    12,    11,    26,    27,     3,    28,    29,
      14,    15,     4,     5,     8,    13,     3,    25,     3,    12,
       3,    27,     3,     9,    25,    16,    16,    11,    28,     3,
       3,     3,    27,    10,    16,     3,    41,    36,    27,    27,
       9,    88,    -1,    -1,    -1,    24,    21,    27,    -1,    -1,
      20,    27,    30,    -1,    -1,    22,    27,    -1,    -1,    25
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,    32,    33,     3,     0,     7,    40,    12,     3,
       8,    13,    25,     3,     3,    34,    35,    38,    12,    26,
      27,    28,    29,    10,    23,    36,    37,    27,    35,    14,
      15,     3,     9,    39,    41,     3,    10,    37,    11,    16,
      16,    25,    11,    41,    28,    39,     3,    38,     3,    16,
      11,    27,    26,    27,    27,     3,    10,    27,     3,    21,
      42,    43,    44,    45,    24,     3,    11,    43,     3,    19,
      25,    47,    30,     9,    47,    47,    17,    18,    20,    27,
       4,     5,    46,    27,    26,    47,    47,    47,    22,    42,
      11,    21,    27
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    31,    32,    33,    34,    34,    35,    35,    36,    36,
      37,    38,    38,    39,    39,    40,    40,    41,    42,    42,
      43,    43,    44,    45,    46,    46,    47,    47,    47,    47,
      47,    47
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,    11,     1,     3,     4,     4,     1,     2,
       5,     1,     3,     1,     2,    11,    10,     9,     1,     2,
       1,     1,     4,     9,     1,     1,     1,     3,     3,     3,
       2,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: entity_decl architecture_decl  */
#line 79 "src/parser/parser.y"
         {
            ast_entity = (yyvsp[-1].node);  
            (yyval.node) = (yyvsp[0].node);
            ast_root = (yyval.node);
            ast_print(ast_root, 0);
        }
#line 1205 "src/parser/parser.c"
    break;

  case 3: /* entity_decl: ENTITY IDENTIFIER IS PORT '(' port_list ')' ';' END_TOK IDENTIFIER ';'  */
#line 89 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_ENTITY);
            (yyval.node)->data.entity.name = strdup((yyvsp[-9].str));
            // CHIRAG : copy collected ports into entity node
            for(int i = 0; i < temp_port_count; i++)
                (yyval.node)->data.entity.ports[i] = temp_ports[i];
            (yyval.node)->data.entity.port_count = temp_port_count;
            temp_port_count = 0;
            printf("parsed entity: %s with %d ports\n", (yyvsp[-9].str), (yyval.node)->data.entity.port_count);
        }
#line 1220 "src/parser/parser.c"
    break;

  case 4: /* port_list: port_item  */
#line 103 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1226 "src/parser/parser.c"
    break;

  case 5: /* port_list: port_list ';' port_item  */
#line 105 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1232 "src/parser/parser.c"
    break;

  case 6: /* port_item: identifier_list ':' IN_TOK BIT  */
#line 110 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_PORT);
            (yyval.node)->data.port.name = strdup((yyvsp[-3].str));
            (yyval.node)->data.port.direction = DIR_IN;
            temp_ports[temp_port_count++] = (yyval.node);
            temp_sens_count = 0;  // ← add this
            printf("parsed input port: %s\n", (yyvsp[-3].str));
        }
#line 1245 "src/parser/parser.c"
    break;

  case 7: /* port_item: identifier_list ':' OUT_TOK BIT  */
#line 119 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_PORT);
            (yyval.node)->data.port.name = strdup((yyvsp[-3].str));
            (yyval.node)->data.port.direction = DIR_OUT;
            temp_ports[temp_port_count++] = (yyval.node);
            temp_sens_count = 0;  // ← add this
            printf("parsed output port: %s\n", (yyvsp[-3].str));
        }
#line 1258 "src/parser/parser.c"
    break;

  case 8: /* signal_decl_list: signal_decl  */
#line 131 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1264 "src/parser/parser.c"
    break;

  case 9: /* signal_decl_list: signal_decl_list signal_decl  */
#line 133 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1270 "src/parser/parser.c"
    break;

  case 10: /* signal_decl: SIGNAL IDENTIFIER ':' BIT ';'  */
#line 138 "src/parser/parser.y"
        {
            // CHIRAG 18-04-26 :: internal signal ... lives in arch not entity
            // reusing NODE_PORT with DIR_INTERNAL so walker can create Signal struct
            // same logic as port ... just different direction tag
            (yyval.node) = ast_new_node(NODE_PORT);
            (yyval.node)->data.port.name = strdup((yyvsp[-3].str));
            (yyval.node)->data.port.direction = DIR_INTERNAL;
            temp_arch_signals[temp_arch_signal_count++] = (yyval.node);
            printf("parsed internal signal: %s\n", (yyvsp[-3].str));
        }
#line 1285 "src/parser/parser.c"
    break;

  case 11: /* identifier_list: IDENTIFIER  */
#line 152 "src/parser/parser.y"
        { (yyval.str) = (yyvsp[0].str); temp_sens[temp_sens_count++] = strdup((yyvsp[0].str)); }
#line 1291 "src/parser/parser.c"
    break;

  case 12: /* identifier_list: identifier_list ',' IDENTIFIER  */
#line 154 "src/parser/parser.y"
        { (yyval.str) = (yyvsp[-2].str); temp_sens[temp_sens_count++] = strdup((yyvsp[0].str)); }
#line 1297 "src/parser/parser.c"
    break;

  case 13: /* process_list: process_decl  */
#line 160 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1303 "src/parser/parser.c"
    break;

  case 14: /* process_list: process_list process_decl  */
#line 162 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1309 "src/parser/parser.c"
    break;

  case 15: /* architecture_decl: ARCHITECTURE IDENTIFIER OF IDENTIFIER IS signal_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'  */
#line 167 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_ARCH);
            (yyval.node)->data.arch.name = strdup((yyvsp[-9].str));
            (yyval.node)->data.arch.entity_name = strdup((yyvsp[-7].str));
            for(int i = 0; i < temp_process_count; i++)
                (yyval.node)->data.arch.processes[i] = temp_processes[i];
            (yyval.node)->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            // CHIRAG 18-04-26 :: copy internal signals into arch node
            for(int i = 0; i < temp_arch_signal_count; i++)
                (yyval.node)->data.arch.signals[i] = temp_arch_signals[i];
            (yyval.node)->data.arch.signal_count = temp_arch_signal_count;
            temp_arch_signal_count = 0;
            printf("parsed architecture: %s of %s with %d processes, %d internal signals\n",
                (yyvsp[-9].str), (yyvsp[-7].str), (yyval.node)->data.arch.process_count, (yyval.node)->data.arch.signal_count);
        }
#line 1330 "src/parser/parser.c"
    break;

  case 16: /* architecture_decl: ARCHITECTURE IDENTIFIER OF IDENTIFIER IS BEGIN_TOK process_list END_TOK IDENTIFIER ';'  */
#line 184 "src/parser/parser.y"
        {
            // CHIRAG 18-04-26 :: no internal signals ... original rule still works
            // circuits like and_gate have no signal declarations ... this keeps them working
            (yyval.node) = ast_new_node(NODE_ARCH);
            (yyval.node)->data.arch.name = strdup((yyvsp[-8].str));
            (yyval.node)->data.arch.entity_name = strdup((yyvsp[-6].str));
            for(int i = 0; i < temp_process_count; i++)
                (yyval.node)->data.arch.processes[i] = temp_processes[i];
            (yyval.node)->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            (yyval.node)->data.arch.signal_count = 0;
            printf("parsed architecture: %s of %s with %d processes\n",
                (yyvsp[-8].str), (yyvsp[-6].str), (yyval.node)->data.arch.process_count);
        }
#line 1349 "src/parser/parser.c"
    break;

  case 17: /* process_decl: PROCESS '(' identifier_list ')' BEGIN_TOK statement_list END_TOK PROCESS ';'  */
#line 202 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_PROCESS);
            // CHIRAG : copy all sensitivity signals, not just first one
            for(int i = 0; i < temp_sens_count; i++)
                (yyval.node)->data.process.sensitivity[i] = temp_sens[i];
            (yyval.node)->data.process.sensitivity_count = temp_sens_count;
            temp_sens_count = 0;
            for(int i = 0; i < temp_stmt_count; i++)
                (yyval.node)->data.process.statements[i] = temp_stmts[i];
            (yyval.node)->data.process.statement_count = temp_stmt_count;
            temp_stmt_count = 0;
            temp_processes[temp_process_count++] = (yyval.node);
            printf("parsed process with sensitivity: %d signals, statements: %d\n",
                (yyval.node)->data.process.sensitivity_count, (yyval.node)->data.process.statement_count);
        }
#line 1369 "src/parser/parser.c"
    break;

  case 18: /* statement_list: statement  */
#line 221 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1375 "src/parser/parser.c"
    break;

  case 19: /* statement_list: statement_list statement  */
#line 223 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1381 "src/parser/parser.c"
    break;

  case 20: /* statement: signal_assignment  */
#line 228 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); temp_stmts[temp_stmt_count++] = (yyvsp[0].node); }
#line 1387 "src/parser/parser.c"
    break;

  case 21: /* statement: if_statement  */
#line 230 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); temp_stmts[temp_stmt_count++] = (yyvsp[0].node); }
#line 1393 "src/parser/parser.c"
    break;

  case 22: /* signal_assignment: IDENTIFIER ASSIGN expression ';'  */
#line 235 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_ASSIGN);
            (yyval.node)->data.assign.target = strdup((yyvsp[-3].str));
            (yyval.node)->data.assign.expr = (yyvsp[-1].node);
            printf("parsed assignment: %s <=\n", (yyvsp[-3].str));
        }
#line 1404 "src/parser/parser.c"
    break;

  case 23: /* if_statement: IF IDENTIFIER '=' bit_literal THEN statement_list END_TOK IF ';'  */
#line 245 "src/parser/parser.y"
        {
            // CHIRAG 18-04-26 :: fixed if statement parsing
            // old code never stored inner statements into NODE_IF
            // they leaked into outer process statement list instead
            // fix ... save temp_stmt_count before parsing inner statements
            // then copy them into if node and restore count
            (yyval.node) = ast_new_node(NODE_IF);
            (yyval.node)->data.if_stmt.signal_name = strdup((yyvsp[-7].str));
            (yyval.node)->data.if_stmt.bit_value = (yyvsp[-5].num);
            // copy inner statements that were collected into temp_stmts
            // these belong to the if block not the outer process
            (yyval.node)->data.if_stmt.statement_count = temp_stmt_count;
            for(int i = 0; i < temp_stmt_count; i++)
                (yyval.node)->data.if_stmt.statements[i] = temp_stmts[i];
            // reset count so outer process doesnt see these
            temp_stmt_count = 0;
            printf("parsed if: %s = '%d' with %d statements\n", (yyvsp[-7].str), (yyvsp[-5].num), (yyval.node)->data.if_stmt.statement_count);
        }
#line 1427 "src/parser/parser.c"
    break;

  case 24: /* bit_literal: ZERO  */
#line 266 "src/parser/parser.y"
            { (yyval.num) = 0; }
#line 1433 "src/parser/parser.c"
    break;

  case 25: /* bit_literal: ONE  */
#line 267 "src/parser/parser.y"
            { (yyval.num) = 1; }
#line 1439 "src/parser/parser.c"
    break;

  case 26: /* expression: IDENTIFIER  */
#line 272 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_IDENTIFIER;
            (yyval.node)->data.expr.identifier = strdup((yyvsp[0].str));
        }
#line 1449 "src/parser/parser.c"
    break;

  case 27: /* expression: expression AND_TOK expression  */
#line 278 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_AND;
            (yyval.node)->data.expr.left = (yyvsp[-2].node);
            (yyval.node)->data.expr.right = (yyvsp[0].node);
        }
#line 1460 "src/parser/parser.c"
    break;

  case 28: /* expression: expression XOR_TOK expression  */
#line 285 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_XOR;
            (yyval.node)->data.expr.left = (yyvsp[-2].node);
            (yyval.node)->data.expr.right = (yyvsp[0].node);
        }
#line 1471 "src/parser/parser.c"
    break;

  case 29: /* expression: expression OR_TOK expression  */
#line 292 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_OR;
            (yyval.node)->data.expr.left = (yyvsp[-2].node);
            (yyval.node)->data.expr.right = (yyvsp[0].node);
        }
#line 1482 "src/parser/parser.c"
    break;

  case 30: /* expression: NOT_TOK expression  */
#line 299 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_NOT;
            (yyval.node)->data.expr.left = (yyvsp[0].node);
        }
#line 1492 "src/parser/parser.c"
    break;

  case 31: /* expression: '(' expression ')'  */
#line 305 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[-1].node); }
#line 1498 "src/parser/parser.c"
    break;


#line 1502 "src/parser/parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
                      yytoken, &yylval);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 308 "src/parser/parser.y"


void yyerror(const char* s)
{
    fprintf(stderr, "parse error: %s\n", s);
}

int main(int argc, char* argv[])
{
    int result = yyparse();
    if(result != 0) 
    { 
        printf("parsing failed\n"); 
        return 1; 
    }
    
    printf("parsing done! walking AST now...\n");

    DynArray_Signal signals;
    DYNARRAY_INIT(signals)
    Scheduler sch = scheduler_init();

    // walk entity first ... creates all signals from port declarations
    // walk arch second ... creates all processes and adds them to scheduler
    ast_walk(ast_entity, &signals, &sch);
    ast_walk(ast_root, &signals, &sch);
    printf("AST walk done! %d signals created\n", signals.size);

    // CHIRAG 04-04-26 :: build dependency graph after ast_walk
    // process count comes from scheduler ... ast_walk populates it
    // dependency_extract walks AST finds read/write sets per process
    // builds edges where processes share signals ... then colors graph
    // same color = no conflicts = can run in parallel
    DepGraph* g = graph_build(sch.process_ARRAY.size);
    dependency_extract(ast_root, g);
    // graph_free(g);

    EventQueue eq = init_queue();

    // walker_queue = &eq;
    // CHIRAG 15-04-26 :: thread 0 gets the main queue ... sequential mode uses thread 0
    // omp_get_thread_num() returns 0 in sequential ... so run_proc_generic picks up correct queue
    walker_queues[0] = &eq;
    // CHIRAG 13-04-26 :: collect all input signals for testbench generation
    // direction 0 = input ... set in ast_walker when walking NODE_PORT
    int input_signals[64];
    int input_count = 0;
    for(int i = 0; i < signals.size; i++)
        if(signals.data[i].direction == 0)
            input_signals[input_count++] = i;

    Event e;
    e.type = 0; e.delta = 0;

    // CHIRAG 13-04-26 :: two modes for seeding events
    // mode 1 ... manual testbench file passed as argv[2]
    // format is simple ... SIGNAL_NAME VALUE TIME ... one per line ... # for comments
    // mode 2 ... auto generate input combinations ... capped at 16 so output stays readable
    if(argc > 2)
    {
        // manual testbench mode ... user controls exactly what gets tested
        // good for sequential circuits ... clocks ... specific scenarios
        printf("testbench mode: reading from %s\n", argv[2]);
        FILE* tb = fopen(argv[2], "r");
        if(tb == NULL)
        {
            printf("error: cant open testbench %s\n", argv[2]);
            return 1;
        }

        char sig[64]; int val; double time;
        char line[128];
        while(fgets(line, 128, tb))
        {
            // skip comments and empty lines
            if(line[0] == '#' || line[0] == '\n') continue;
            if(sscanf(line, "%s %d %lf", sig, &val, &time) == 3)
            {
                e.signal_name = strdup(sig);
                e.new_value = val;
                e.time = time;
                e.delta = 0;
                e.type = 0;
                insert_ele(&eq, e);
                printf("tb: %s = %d at t=%.0f\n", sig, val, time);
            }
        }
        fclose(tb);
    }
    else
    {
        // auto testbench mode ... generates all 2^N input combinations
        // capped at 16 combinations so output stays readable for large circuits
        // good for combinational circuits ... exhaustive for small input counts
        int limit = (1 << input_count);
        if(limit > 16) limit = 16;
        printf("auto testbench: found %d input signals ... generating %d combinations\n",
            input_count, limit);

        // combination c ... bit j of c = value of input signal j
        // so c=0 means all inputs 0 ... c=1 means first input 1 rest 0 ... etc
        for(int c = 0; c < limit; c++)
            for(int j = 0; j < input_count; j++)
            {
                e.signal_name = signals.data[input_signals[j]].name;
                e.new_value = (c >> j) & 1;
                e.time = c + 1;
                insert_ele(&eq, e);
            }
    }

    // CHIRAG 13-04-26 :: vcd filename from argv[1] ... default to output-parser.vcd
    char vcd_name[64];
    if(argc > 1)
        snprintf(vcd_name, 64, "output-%s.vcd", argv[1]);
    else
        snprintf(vcd_name, 64, "output-parser.vcd");

    vcd_init(vcd_name);
    vcd_write_header(&signals);
    init_run();
    trace_init();
    
    // CHIRAG 17-04-26 :: mode flag ... argv[3] is --seq or --par
    
    // old method was that i had both runs and was commenting a functtion call then running other
    // well that has been annowing so will be adding a mode variable to ask for the mode and based on that we run mode
    
    // run_simulation(&eq, &sch, &signals);
    // run_parallel_simulation(&eq, &sch, &signals, g);

    
    // default is parallel if not specified
    // just now it occured to me... like why not clock?
    // ddoesnt it ddo the same thing? isnt it same as Openmp?
    // timing uses omp_get_wtime() not clock()
    // clock() measures total CPU time across all threads ... if 4 threads run 1s each clock() = 4s
    // omp_get_wtime() measures wall clock time ... actual real world time user waited
    // speedup = seq_wall_time / par_wall_time ... this is why we never use clock to test speedup
    int use_parallel = 1;
    if(argc > 3 && strcmp(argv[3], "--seq") == 0)
        use_parallel = 0;

    double t_start = omp_get_wtime();
    if(use_parallel)
        run_parallel_simulation(&eq, &sch, &signals, g);
    else
        run_simulation(&eq, &sch, &signals);
    double t_end = omp_get_wtime();
    printf("mode: %s\n", use_parallel ? "parallel" : "sequential");
    printf("threads: %d\n", omp_get_max_threads());
    printf("simulation time: %.9f seconds\n", t_end - t_start);
    printf("\nfinal signal values:\n");
    for(int i = 0; i < signals.size; i++)
        printf("  %s = %d\n", signals.data[i].name, signals.data[i].value);

    // CHIRAG 13-04-26 :: print hash at end ... this is the correctness contract
    // sequential hash must equal parallel hash later ... if they differ there is a bug
    printf("trace hash: %u\n", trace_hash());
    graph_free(g); 
    vcd_close();
    return 0;
}
