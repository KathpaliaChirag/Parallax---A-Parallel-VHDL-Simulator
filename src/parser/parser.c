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
ASTNode* temp_ports[256];
ASTNode* temp_stmts[256];
int temp_stmt_count = 0;
int temp_port_count = 0;
ASTNode* temp_processes[256];
int temp_process_count = 0;
char* temp_sens[256];
int temp_sens_count = 0;
// CHIRAG 18-04-26 :: temp storage for internal signal declarations
// signal CARRY : bit; between IS and BEGIN goes here
// separate from temp_ports ... ports belong to entity ... these belong to arch
ASTNode* temp_arch_signals[256];
int temp_arch_signal_count = 0;
// CHIRAG 21-04-26 :: temp storage for function declarations
// functions live between IS and BEGIN in architecture ... like signal decls
// stored here during parsing ... copied into arch node when rule completes
ASTNode* temp_funcs[128];
int temp_func_count = 0;

// temp storage for function parameters during parsing
char* temp_params[128];
int temp_param_count = 0;

// temp storage for function call arguments during parsing
ASTNode* temp_args[128];
int temp_arg_count = 0;

#line 128 "src/parser/parser.c"

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
  YYSYMBOL_ELSE_TOK = 24,                  /* ELSE_TOK  */
  YYSYMBOL_ASSIGN = 25,                    /* ASSIGN  */
  YYSYMBOL_AFTER_TOK = 26,                 /* AFTER_TOK  */
  YYSYMBOL_NS_TOK = 27,                    /* NS_TOK  */
  YYSYMBOL_FUNCTION_TOK = 28,              /* FUNCTION_TOK  */
  YYSYMBOL_RETURN_TOK = 29,                /* RETURN_TOK  */
  YYSYMBOL_INTEGER = 30,                   /* INTEGER  */
  YYSYMBOL_31_ = 31,                       /* '('  */
  YYSYMBOL_32_ = 32,                       /* ')'  */
  YYSYMBOL_33_ = 33,                       /* ';'  */
  YYSYMBOL_34_ = 34,                       /* ':'  */
  YYSYMBOL_35_ = 35,                       /* ','  */
  YYSYMBOL_36_ = 36,                       /* '='  */
  YYSYMBOL_YYACCEPT = 37,                  /* $accept  */
  YYSYMBOL_program = 38,                   /* program  */
  YYSYMBOL_entity_decl = 39,               /* entity_decl  */
  YYSYMBOL_port_list = 40,                 /* port_list  */
  YYSYMBOL_port_item = 41,                 /* port_item  */
  YYSYMBOL_func_decl_list = 42,            /* func_decl_list  */
  YYSYMBOL_func_decl = 43,                 /* func_decl  */
  YYSYMBOL_func_param_list = 44,           /* func_param_list  */
  YYSYMBOL_func_param = 45,                /* func_param  */
  YYSYMBOL_arg_list = 46,                  /* arg_list  */
  YYSYMBOL_signal_decl_list = 47,          /* signal_decl_list  */
  YYSYMBOL_signal_decl = 48,               /* signal_decl  */
  YYSYMBOL_identifier_list = 49,           /* identifier_list  */
  YYSYMBOL_process_list = 50,              /* process_list  */
  YYSYMBOL_architecture_decl = 51,         /* architecture_decl  */
  YYSYMBOL_process_decl = 52,              /* process_decl  */
  YYSYMBOL_statement_list = 53,            /* statement_list  */
  YYSYMBOL_statement = 54,                 /* statement  */
  YYSYMBOL_signal_assignment = 55,         /* signal_assignment  */
  YYSYMBOL_if_statement = 56,              /* if_statement  */
  YYSYMBOL_57_1 = 57,                      /* @1  */
  YYSYMBOL_bit_literal = 58,               /* bit_literal  */
  YYSYMBOL_expression = 59                 /* expression  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYLAST   181

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  37
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  45
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  144

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   285


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
      31,    32,     2,     2,    35,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    34,    33,
       2,    36,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    95,    95,   105,   119,   121,   126,   135,   146,   148,
     153,   179,   181,   186,   198,   203,   210,   212,   217,   231,
     233,   239,   241,   246,   276,   295,   316,   336,   355,   357,
     362,   364,   370,   380,   394,   413,   413,   448,   449,   453,
     459,   466,   473,   480,   486,   499
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
  "NOT_TOK", "XOR_TOK", "IF", "THEN", "SIGNAL", "ELSE_TOK", "ASSIGN",
  "AFTER_TOK", "NS_TOK", "FUNCTION_TOK", "RETURN_TOK", "INTEGER", "'('",
  "')'", "';'", "':'", "','", "'='", "$accept", "program", "entity_decl",
  "port_list", "port_item", "func_decl_list", "func_decl",
  "func_param_list", "func_param", "arg_list", "signal_decl_list",
  "signal_decl", "identifier_list", "process_list", "architecture_decl",
  "process_decl", "statement_list", "statement", "signal_assignment",
  "if_statement", "@1", "bit_literal", "expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-97)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      19,     1,    36,    33,    34,   -97,    67,   -97,    66,    70,
      48,    84,    93,    85,   -97,    53,   -97,    54,    -5,    65,
      93,    76,    96,    91,    98,    99,    -2,   -97,     5,   -97,
      92,   -97,    88,    89,   -97,    75,    41,   -97,    74,    78,
      91,   -97,    91,    14,   -97,   104,   -97,   -97,    93,   107,
     -97,    95,   109,    64,    69,    91,    80,     9,    81,    82,
      83,    60,   -97,   113,   115,    73,   -97,   110,   -97,   -97,
     103,    94,   109,    97,   100,   118,    32,   -97,   106,   -97,
     -97,   -97,   101,   102,   121,     6,   -97,   -97,   -97,   114,
     -97,     3,   105,   116,   -97,   119,   108,     3,     3,    39,
      90,   111,   117,     3,   -97,    49,     3,     3,     3,   112,
     -97,   -97,   -97,   123,   -97,     3,    16,    -4,   -97,   -97,
      51,   -97,   120,    32,    43,   -97,     3,   122,    26,   124,
      -4,   -97,   127,   125,   126,   128,    32,   129,   -97,    28,
     -97,   130,   131,   -97
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     1,     0,     2,     0,     0,
       0,     0,     0,     0,    19,     0,     4,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     8,     0,    16,
       0,     5,     0,     0,    20,     0,     0,    21,     0,     0,
       0,     9,     0,     0,    17,     0,     6,     7,     0,     0,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,     0,     0,     0,     3,     0,    26,    18,
       0,     0,     0,     0,     0,     0,     0,    13,     0,    12,
      24,    25,     0,     0,     0,     0,    28,    30,    31,     0,
      23,     0,     0,     0,    29,     0,    39,     0,     0,     0,
       0,     0,     0,     0,    43,     0,     0,     0,     0,     0,
      32,    37,    38,     0,    27,     0,     0,    14,    45,    40,
      42,    41,     0,     0,     0,    44,     0,     0,    35,     0,
      15,    33,     0,     0,     0,     0,     0,     0,    34,     0,
      10,     0,     0,    36
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -97,   -97,   -97,   -97,   132,   135,    12,   -97,    56,   -97,
     -97,   137,   133,    22,   -97,   -33,   -78,   -85,   -97,   -97,
     -97,   -97,   -96
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,    15,    16,    26,    27,    61,    62,   116,
      28,    29,    17,    36,     7,    37,    85,    86,    87,    88,
     133,   113,    99
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      94,   104,   105,    50,     4,    23,    96,   117,    40,    83,
     119,   120,   121,   106,   107,    42,   108,    93,    24,   124,
      50,    50,    97,    25,    55,     1,    25,    84,    24,    83,
     130,    83,    50,    25,    98,    83,     5,   132,    41,   141,
       6,    67,    25,    94,    22,   128,     8,    84,   125,    84,
      35,   126,    49,    84,    94,    41,   106,   107,   139,   108,
     106,   107,    53,   108,    54,   109,   106,   107,   106,   108,
       9,   108,   110,    35,    10,    63,   129,    65,    35,    12,
      64,   118,    35,    11,    75,    19,    20,    13,    21,    22,
      32,    33,    71,    72,   111,   112,    14,    18,    30,    34,
      35,    38,    39,    45,    46,    47,    48,    56,    51,    52,
      58,    59,    60,    66,    68,    69,    73,    70,    74,    77,
      76,    82,    89,    78,    92,   101,    95,    91,    79,   102,
      80,     0,     0,    81,    90,   134,     0,     0,     0,   103,
       0,   100,   122,     0,   114,   123,   115,   127,   135,   136,
       0,   142,    31,     0,   137,   131,     0,     0,     0,     0,
       0,   138,   140,    43,   143,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    57
};

static const yytype_int16 yycheck[] =
{
      85,    97,    98,    36,     3,    10,     3,   103,    10,     3,
     106,   107,   108,    17,    18,    10,    20,    11,    23,   115,
      53,    54,    19,    28,    10,     6,    28,    21,    23,     3,
     126,     3,    65,    28,    31,     3,     0,    11,    26,    11,
       7,    32,    28,   128,    35,   123,    12,    21,    32,    21,
       9,    35,    11,    21,   139,    43,    17,    18,   136,    20,
      17,    18,    40,    20,    42,    26,    17,    18,    17,    20,
       3,    20,    33,     9,     8,    11,    33,    55,     9,    31,
      11,    32,     9,    13,    11,    32,    33,     3,    34,    35,
      14,    15,    32,    33,     4,     5,     3,    12,    33,     3,
       9,     3,     3,    11,    16,    16,    31,     3,    34,    31,
       3,    16,     3,    33,    33,    33,     3,    34,     3,    16,
      10,     3,    16,    29,     3,     9,    12,    25,    72,    10,
      33,    -1,    -1,    33,    33,    11,    -1,    -1,    -1,    31,
      -1,    36,    30,    -1,    33,    22,    29,    27,    21,    24,
      -1,    21,    20,    -1,    28,    33,    -1,    -1,    -1,    -1,
      -1,    33,    33,    28,    33,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,    38,    39,     3,     0,     7,    51,    12,     3,
       8,    13,    31,     3,     3,    40,    41,    49,    12,    32,
      33,    34,    35,    10,    23,    28,    42,    43,    47,    48,
      33,    41,    14,    15,     3,     9,    50,    52,     3,     3,
      10,    43,    10,    42,    48,    11,    16,    16,    31,    11,
      52,    34,    31,    50,    50,    10,     3,    49,     3,    16,
       3,    44,    45,    11,    11,    50,    33,    32,    33,    33,
      34,    32,    33,     3,     3,    11,    10,    16,    29,    45,
      33,    33,     3,     3,    21,    53,    54,    55,    56,    16,
      33,    25,     3,    11,    54,    12,     3,    19,    31,    59,
      36,     9,    10,    31,    59,    59,    17,    18,    20,    26,
      33,     4,     5,    58,    33,    29,    46,    59,    32,    59,
      59,    59,    30,    22,    59,    32,    35,    27,    53,    33,
      59,    33,    11,    57,    11,    21,    24,    28,    33,    53,
      33,    11,    21,    33
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    37,    38,    39,    40,    40,    41,    41,    42,    42,
      43,    44,    44,    45,    46,    46,    47,    47,    48,    49,
      49,    50,    50,    51,    51,    51,    51,    52,    53,    53,
      54,    54,    55,    55,    56,    57,    56,    58,    58,    59,
      59,    59,    59,    59,    59,    59
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,    11,     1,     3,     4,     4,     1,     2,
      15,     1,     3,     3,     1,     3,     1,     2,     5,     1,
       3,     1,     2,    12,    11,    11,    10,     9,     1,     2,
       1,     1,     4,     7,     9,     0,    12,     1,     1,     1,
       3,     3,     3,     2,     4,     3
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
#line 96 "src/parser/parser.y"
         {
            ast_entity = (yyvsp[-1].node);  
            (yyval.node) = (yyvsp[0].node);
            ast_root = (yyval.node);
            ast_print(ast_root, 0);
        }
#line 1271 "src/parser/parser.c"
    break;

  case 3: /* entity_decl: ENTITY IDENTIFIER IS PORT '(' port_list ')' ';' END_TOK IDENTIFIER ';'  */
#line 106 "src/parser/parser.y"
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
#line 1286 "src/parser/parser.c"
    break;

  case 4: /* port_list: port_item  */
#line 120 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1292 "src/parser/parser.c"
    break;

  case 5: /* port_list: port_list ';' port_item  */
#line 122 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1298 "src/parser/parser.c"
    break;

  case 6: /* port_item: identifier_list ':' IN_TOK BIT  */
#line 127 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_PORT);
            (yyval.node)->data.port.name = strdup((yyvsp[-3].str));
            (yyval.node)->data.port.direction = DIR_IN;
            temp_ports[temp_port_count++] = (yyval.node);
            temp_sens_count = 0;  // ← add this
            printf("parsed input port: %s\n", (yyvsp[-3].str));
        }
#line 1311 "src/parser/parser.c"
    break;

  case 7: /* port_item: identifier_list ':' OUT_TOK BIT  */
#line 136 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_PORT);
            (yyval.node)->data.port.name = strdup((yyvsp[-3].str));
            (yyval.node)->data.port.direction = DIR_OUT;
            temp_ports[temp_port_count++] = (yyval.node);
            temp_sens_count = 0;  // ← add this
            printf("parsed output port: %s\n", (yyvsp[-3].str));
        }
#line 1324 "src/parser/parser.c"
    break;

  case 8: /* func_decl_list: func_decl  */
#line 147 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1330 "src/parser/parser.c"
    break;

  case 9: /* func_decl_list: func_decl_list func_decl  */
#line 149 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1336 "src/parser/parser.c"
    break;

  case 10: /* func_decl: FUNCTION_TOK IDENTIFIER '(' func_param_list ')' RETURN_TOK BIT IS BEGIN_TOK RETURN_TOK expression ';' END_TOK FUNCTION_TOK ';'  */
#line 154 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: function declaration
            // idea ... support function majority(A:bit; B:bit) return bit is begin return expr; end function;
            // problem ... functions need their own AST node to store params and body
            // solution ... NODE_FUNC_DECL stores name, param names, and body expression
            // only single-return functions supported ... no local variables ... no loops
            // this covers 90% of useful utility functions in RTL design
            (yyval.node) = ast_new_node(NODE_FUNC_DECL);
            (yyval.node)->data.func_decl.name = strdup((yyvsp[-13].str));
            (yyval.node)->data.func_decl.param_count = temp_param_count;
            for(int i = 0; i < temp_param_count; i++)
                (yyval.node)->data.func_decl.params[i] = strdup(temp_params[i]);
            // CHIRAG 21-04-26 :: was $12 before ... counted tokens wrong
            // FUNCTION_TOK($1) IDENTIFIER($2) '('($3) func_param_list($4) ')'($5) 
            // RETURN_TOK($6) BIT($7) IS($8) BEGIN_TOK($9) RETURN_TOK($10) expression($11)
            // so expression is $11 not $12 ... $12 is the semicolon ... no type on semicolons
            (yyval.node)->data.func_decl.body = (yyvsp[-4].node);
            // $$->data.func_decl.body = $12;
            temp_param_count = 0;
            temp_funcs[temp_func_count++] = (yyval.node);
            printf("parsed function: %s with %d params\n", (yyvsp[-13].str), (yyval.node)->data.func_decl.param_count);
        }
#line 1363 "src/parser/parser.c"
    break;

  case 11: /* func_param_list: func_param  */
#line 180 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1369 "src/parser/parser.c"
    break;

  case 12: /* func_param_list: func_param_list ';' func_param  */
#line 182 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1375 "src/parser/parser.c"
    break;

  case 13: /* func_param: IDENTIFIER ':' BIT  */
#line 187 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: one function parameter ... A : bit
            // store name in temp_params ... func_decl rule copies them
            (yyval.node) = ast_new_node(NODE_PORT);
            (yyval.node)->data.port.name = strdup((yyvsp[-2].str));
            temp_params[temp_param_count++] = strdup((yyvsp[-2].str));
            printf("parsed func param: %s\n", (yyvsp[-2].str));
        }
#line 1388 "src/parser/parser.c"
    break;

  case 14: /* arg_list: expression  */
#line 199 "src/parser/parser.y"
        {
            (yyval.node) = (yyvsp[0].node);
            temp_args[temp_arg_count++] = (yyvsp[0].node);
        }
#line 1397 "src/parser/parser.c"
    break;

  case 15: /* arg_list: arg_list ',' expression  */
#line 204 "src/parser/parser.y"
        {
            (yyval.node) = (yyvsp[0].node);
            temp_args[temp_arg_count++] = (yyvsp[0].node);
        }
#line 1406 "src/parser/parser.c"
    break;

  case 16: /* signal_decl_list: signal_decl  */
#line 211 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1412 "src/parser/parser.c"
    break;

  case 17: /* signal_decl_list: signal_decl_list signal_decl  */
#line 213 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1418 "src/parser/parser.c"
    break;

  case 18: /* signal_decl: SIGNAL IDENTIFIER ':' BIT ';'  */
#line 218 "src/parser/parser.y"
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
#line 1433 "src/parser/parser.c"
    break;

  case 19: /* identifier_list: IDENTIFIER  */
#line 232 "src/parser/parser.y"
        { (yyval.str) = (yyvsp[0].str); temp_sens[temp_sens_count++] = strdup((yyvsp[0].str)); }
#line 1439 "src/parser/parser.c"
    break;

  case 20: /* identifier_list: identifier_list ',' IDENTIFIER  */
#line 234 "src/parser/parser.y"
        { (yyval.str) = (yyvsp[-2].str); temp_sens[temp_sens_count++] = strdup((yyvsp[0].str)); }
#line 1445 "src/parser/parser.c"
    break;

  case 21: /* process_list: process_decl  */
#line 240 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1451 "src/parser/parser.c"
    break;

  case 22: /* process_list: process_list process_decl  */
#line 242 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1457 "src/parser/parser.c"
    break;

  case 23: /* architecture_decl: ARCHITECTURE IDENTIFIER OF IDENTIFIER IS signal_decl_list func_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'  */
#line 247 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: signals + functions + processes ... most complete variant
            // problem ... old architecture_decl only had 2 variants ... with signals and without
            // it never referenced func_decl_list so bison marked those rules as useless
            // fix ... added 4 variants to cover all combinations
            // variant 1 ... signals + functions + processes ... for circuits like majority_voter
            // variant 2 ... functions only ... no signal declarations
            // variant 3 ... signals only ... no functions ... ripple carry adder etc
            // variant 4 ... neither ... simple circuits like and_gate
            (yyval.node) = ast_new_node(NODE_ARCH);
            (yyval.node)->data.arch.name = strdup((yyvsp[-10].str));
            (yyval.node)->data.arch.entity_name = strdup((yyvsp[-8].str));
            for(int i = 0; i < temp_process_count; i++)
                (yyval.node)->data.arch.processes[i] = temp_processes[i];
            (yyval.node)->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            // CHIRAG 18-04-26 :: copy internal signals into arch node
            for(int i = 0; i < temp_arch_signal_count; i++)
                (yyval.node)->data.arch.signals[i] = temp_arch_signals[i];
            (yyval.node)->data.arch.signal_count = temp_arch_signal_count;
            temp_arch_signal_count = 0;
            // CHIRAG 21-04-26 :: copy function declarations into arch node
            for(int i = 0; i < temp_func_count; i++)
                (yyval.node)->data.arch.funcs[i] = temp_funcs[i];
            (yyval.node)->data.arch.func_count = temp_func_count;
            temp_func_count = 0;
            printf("parsed architecture: %s with %d processes, %d signals, %d functions\n",
                (yyvsp[-10].str), (yyval.node)->data.arch.process_count, (yyval.node)->data.arch.signal_count, (yyval.node)->data.arch.func_count);
        }
#line 1491 "src/parser/parser.c"
    break;

  case 24: /* architecture_decl: ARCHITECTURE IDENTIFIER OF IDENTIFIER IS func_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'  */
#line 277 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: functions only ... no signal declarations
            // for circuits that use helper functions but no internal wires
            (yyval.node) = ast_new_node(NODE_ARCH);
            (yyval.node)->data.arch.name = strdup((yyvsp[-9].str));
            (yyval.node)->data.arch.entity_name = strdup((yyvsp[-7].str));
            for(int i = 0; i < temp_process_count; i++)
                (yyval.node)->data.arch.processes[i] = temp_processes[i];
            (yyval.node)->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            (yyval.node)->data.arch.signal_count = 0;
            for(int i = 0; i < temp_func_count; i++)
                (yyval.node)->data.arch.funcs[i] = temp_funcs[i];
            (yyval.node)->data.arch.func_count = temp_func_count;
            temp_func_count = 0;
            printf("parsed architecture: %s with %d processes, %d functions\n",
                (yyvsp[-9].str), (yyval.node)->data.arch.process_count, (yyval.node)->data.arch.func_count);
        }
#line 1514 "src/parser/parser.c"
    break;

  case 25: /* architecture_decl: ARCHITECTURE IDENTIFIER OF IDENTIFIER IS signal_decl_list BEGIN_TOK process_list END_TOK IDENTIFIER ';'  */
#line 296 "src/parser/parser.y"
        {
            // CHIRAG 18-04-26 :: signals only ... no functions
            // circuits like and_gate have no signal declarations ... this keeps them working
            // CHIRAG 21-04-26 :: kept this variant ... func_count set to 0 explicitly
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
            (yyval.node)->data.arch.func_count = 0;
            printf("parsed architecture: %s of %s with %d processes, %d internal signals\n",
                (yyvsp[-9].str), (yyvsp[-7].str), (yyval.node)->data.arch.process_count, (yyval.node)->data.arch.signal_count);
        }
#line 1539 "src/parser/parser.c"
    break;

  case 26: /* architecture_decl: ARCHITECTURE IDENTIFIER OF IDENTIFIER IS BEGIN_TOK process_list END_TOK IDENTIFIER ';'  */
#line 317 "src/parser/parser.y"
        {
            // CHIRAG 18-04-26 :: no internal signals ... original rule still works
            // circuits like and_gate have no signal declarations ... this keeps them working
            // CHIRAG 21-04-26 :: added func_count = 0 explicitly ... walker needs this
            (yyval.node) = ast_new_node(NODE_ARCH);
            (yyval.node)->data.arch.name = strdup((yyvsp[-8].str));
            (yyval.node)->data.arch.entity_name = strdup((yyvsp[-6].str));
            for(int i = 0; i < temp_process_count; i++)
                (yyval.node)->data.arch.processes[i] = temp_processes[i];
            (yyval.node)->data.arch.process_count = temp_process_count;
            temp_process_count = 0;
            (yyval.node)->data.arch.signal_count = 0;
            (yyval.node)->data.arch.func_count = 0;
            printf("parsed architecture: %s of %s with %d processes\n",
                (yyvsp[-8].str), (yyvsp[-6].str), (yyval.node)->data.arch.process_count);
        }
#line 1560 "src/parser/parser.c"
    break;

  case 27: /* process_decl: PROCESS '(' identifier_list ')' BEGIN_TOK statement_list END_TOK PROCESS ';'  */
#line 337 "src/parser/parser.y"
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
#line 1580 "src/parser/parser.c"
    break;

  case 28: /* statement_list: statement  */
#line 356 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1586 "src/parser/parser.c"
    break;

  case 29: /* statement_list: statement_list statement  */
#line 358 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); }
#line 1592 "src/parser/parser.c"
    break;

  case 30: /* statement: signal_assignment  */
#line 363 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); temp_stmts[temp_stmt_count++] = (yyvsp[0].node); }
#line 1598 "src/parser/parser.c"
    break;

  case 31: /* statement: if_statement  */
#line 365 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[0].node); temp_stmts[temp_stmt_count++] = (yyvsp[0].node); }
#line 1604 "src/parser/parser.c"
    break;

  case 32: /* signal_assignment: IDENTIFIER ASSIGN expression ';'  */
#line 371 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: this part is for when no after delay ... same as before
            // delay_ns = 0 means schedule at delta+1 of current time
            (yyval.node) = ast_new_node(NODE_ASSIGN);
            (yyval.node)->data.assign.target = strdup((yyvsp[-3].str));
            (yyval.node)->data.assign.expr = (yyvsp[-1].node);
            (yyval.node)->data.assign.delay_ns = 0;
            printf("parsed assignment: %s <=\n", (yyvsp[-3].str));
        }
#line 1618 "src/parser/parser.c"
    break;

  case 33: /* signal_assignment: IDENTIFIER ASSIGN expression AFTER_TOK INTEGER NS_TOK ';'  */
#line 381 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: this part is for when after delay is used ... Y <= A and B after 10 ns;
            // delay_ns stores the delay value ... ast_walker uses it to schedule
            // at current_time + delay_ns instead of current_time delta+1
            (yyval.node) = ast_new_node(NODE_ASSIGN);
            (yyval.node)->data.assign.target = strdup((yyvsp[-6].str));
            (yyval.node)->data.assign.expr = (yyvsp[-4].node);
            (yyval.node)->data.assign.delay_ns = (yyvsp[-2].num);
            printf("parsed assignment with delay: %s <= after %d ns\n", (yyvsp[-6].str), (yyvsp[-2].num));
        }
#line 1633 "src/parser/parser.c"
    break;

  case 34: /* if_statement: IF IDENTIFIER '=' bit_literal THEN statement_list END_TOK IF ';'  */
#line 395 "src/parser/parser.y"
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
#line 1656 "src/parser/parser.c"
    break;

  case 35: /* @1: %empty  */
#line 413 "src/parser/parser.y"
                                                        { (yyval.num) = temp_stmt_count; }
#line 1662 "src/parser/parser.c"
    break;

  case 36: /* if_statement: IF IDENTIFIER '=' bit_literal THEN statement_list @1 ELSE_TOK statement_list END_TOK IF ';'  */
#line 414 "src/parser/parser.y"
        {
            // CHIRAG 18-04-26 :: if with else branch
            // problem here ... both then-block and else-block use temp_stmts
            // by the time we reach this action ... temp_stmts has ONLY the else statements
            // why? ... then-block statements were already consumed by first statement_list
            // and temp_stmt_count was reset by ... wait no it wasnt reset between the two lists
            // so temp_stmts has ALL statements ... then + else concatenated
            // we cant split them here cleanly without knowing where then ends
            //
            // fix ... use a separator ... save then-count before parsing else
            // but bison actions run AFTER the whole rule matches ... too late
            //
            // real fix ... use a mid-rule action to snapshot temp_stmt_count
            // after first statement_list but before ELSE_TOK
            // $<num>5 trick ... store count in a typed mid-rule slot
            (yyval.node) = ast_new_node(NODE_IF);
            (yyval.node)->data.if_stmt.signal_name = strdup((yyvsp[-10].str));
            (yyval.node)->data.if_stmt.bit_value = (yyvsp[-8].num);
            // then-block count was saved by mid-rule action into $6
            // else-block is everything after that
            int then_count = (yyvsp[-5].num);
            int else_count = temp_stmt_count - then_count;
            (yyval.node)->data.if_stmt.statement_count = then_count;
            for(int i = 0; i < then_count; i++)
                (yyval.node)->data.if_stmt.statements[i] = temp_stmts[i];
            (yyval.node)->data.if_stmt.else_statement_count = else_count;
            for(int i = 0; i < else_count; i++)
                (yyval.node)->data.if_stmt.else_statements[i] = temp_stmts[then_count + i];
            temp_stmt_count = 0;
            printf("parsed if-else: %s = '%d' then=%d else=%d\n", (yyvsp[-10].str), (yyvsp[-8].num), then_count, else_count);
        }
#line 1698 "src/parser/parser.c"
    break;

  case 37: /* bit_literal: ZERO  */
#line 448 "src/parser/parser.y"
            { (yyval.num) = 0; }
#line 1704 "src/parser/parser.c"
    break;

  case 38: /* bit_literal: ONE  */
#line 449 "src/parser/parser.y"
            { (yyval.num) = 1; }
#line 1710 "src/parser/parser.c"
    break;

  case 39: /* expression: IDENTIFIER  */
#line 454 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_IDENTIFIER;
            (yyval.node)->data.expr.identifier = strdup((yyvsp[0].str));
        }
#line 1720 "src/parser/parser.c"
    break;

  case 40: /* expression: expression AND_TOK expression  */
#line 460 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_AND;
            (yyval.node)->data.expr.left = (yyvsp[-2].node);
            (yyval.node)->data.expr.right = (yyvsp[0].node);
        }
#line 1731 "src/parser/parser.c"
    break;

  case 41: /* expression: expression XOR_TOK expression  */
#line 467 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_XOR;
            (yyval.node)->data.expr.left = (yyvsp[-2].node);
            (yyval.node)->data.expr.right = (yyvsp[0].node);
        }
#line 1742 "src/parser/parser.c"
    break;

  case 42: /* expression: expression OR_TOK expression  */
#line 474 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_OR;
            (yyval.node)->data.expr.left = (yyvsp[-2].node);
            (yyval.node)->data.expr.right = (yyvsp[0].node);
        }
#line 1753 "src/parser/parser.c"
    break;

  case 43: /* expression: NOT_TOK expression  */
#line 481 "src/parser/parser.y"
        {
            (yyval.node) = ast_new_node(NODE_EXPR);
            (yyval.node)->data.expr.expr_type = EXPR_NOT;
            (yyval.node)->data.expr.left = (yyvsp[0].node);
        }
#line 1763 "src/parser/parser.c"
    break;

  case 44: /* expression: IDENTIFIER '(' arg_list ')'  */
#line 487 "src/parser/parser.y"
        {
            // CHIRAG 21-04-26 :: function call ... majority(A, B, C)
            // IDENTIFIER followed by ( means its a function call not a signal read
            // bison lookahead handles the ambiguity ... sees ( after IDENTIFIER = call
            (yyval.node) = ast_new_node(NODE_FUNC_CALL);
            (yyval.node)->data.func_call.name = strdup((yyvsp[-3].str));
            (yyval.node)->data.func_call.arg_count = temp_arg_count;
            for(int i = 0; i < temp_arg_count; i++)
                (yyval.node)->data.func_call.args[i] = temp_args[i];
            temp_arg_count = 0;
            printf("parsed func call: %s with %d args\n", (yyvsp[-3].str), (yyval.node)->data.func_call.arg_count);
        }
#line 1780 "src/parser/parser.c"
    break;

  case 45: /* expression: '(' expression ')'  */
#line 500 "src/parser/parser.y"
        { (yyval.node) = (yyvsp[-1].node); }
#line 1786 "src/parser/parser.c"
    break;


#line 1790 "src/parser/parser.c"

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

#line 503 "src/parser/parser.y"


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
    // CHIRAG 20-04-26 :: write DOT file right after graph is built and colored
    graph_write_dot(g, "dependency.dot");
    // graph_free(g);

    EventQueue eq = init_queue();

    // walker_queue = &eq;
    // CHIRAG 15-04-26 :: thread 0 gets the main queue ... sequential mode uses thread 0
    // omp_get_thread_num() returns 0 in sequential ... so run_proc_generic picks up correct queue
    walker_queues[0] = &eq;
    // CHIRAG 13-04-26 :: collect all input signals for testbench generation
    // direction 0 = input ... set in ast_walker when walking NODE_PORT
    // int input_signals[64];
    int input_signals[256];
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
   // if(argc > 2)
   // CHIRAG 20-04-26 :: check argv[2] is actually a testbench not a mode flag
    // when no TB given ... makefile passes --seq as argv[2] ... old check opened it as file
    // fix ... if argv[2] starts with "--" its a mode flag not a testbench ... skip to auto mode
    if(argc > 2 && argv[2][0] != '-')
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
    // if(argc > 3 && strcmp(argv[3], "--seq") == 0)
    //     use_parallel = 0;
    // NEW -- CHIRAG 21-04-26 :: fix mode detection when no TB file given
    // old code assumed argv[3] always has mode flag ... only true when TB is present
    // when no TB ... argv[2] is the mode flag ... argv[3] doesnt exist
    // fix ... check both argv[2] and argv[3] for --seq flag
    if((argc > 2 && strcmp(argv[2], "--seq") == 0) ||
       (argc > 3 && strcmp(argv[3], "--seq") == 0))
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
    // CHIRAG 20-04-26 :: print simulation stats
    // idea ... give user insight into what the sim actually did internally
    // problem ... before this ... black box ... hash and values but no internal metrics
    // solution ... four counters updated throughout sim ... printed here at end
    // delta_count tells how many delta cycles ran ... feedback chain depth story
    // event_count tells how many external stimulus events were processed
    // process_firings tells how many times processes actually executed
    // max_delta_depth is the most interesting ... deepest feedback chain seen
    printf("\n----> simulation stats <----\n");
    printf("total delta cycles    : %d\n", stat_delta_count);
    printf("total events processed: %d\n", stat_event_count);
    printf("total process firings : %d\n", stat_process_firings);
    printf("max delta depth       : %d\n", stat_max_delta_depth);
    printf("--------------------------\n");
     // CHIRAG 21-04-26 :: Amdahl's law theoretical speedup
    // idea ... show what speedup is theoretically possible given our parallel fraction
    // problem ... measured times on small circuits are noisy ... timer resolution too coarse
    // solution ... compute theoretical Amdahl numbers from graph structure
    // serial fraction S = 1 / num_colors ... one color batch must run serially before next
    // if all processes same color ... S is just overhead fraction ... estimate 0.3
    // Amdahl ... Speedup(N) = 1 / (S + (1-S)/N)
    // this gives upper bound on speedup ... real speedup will be lower due to overhead
    float S = (g->num_colors > 1) ? (1.0f / g->num_colors) : 0.3f;
    printf("\n--- amdahl's law (theoretical) ---\n");
    printf("serial fraction S     : %.2f\n", S);
    printf("parallel fraction     : %.2f\n", 1.0f - S);
    for(int n = 1; n <= 16; n *= 2)
    {
        float speedup = 1.0f / (S + (1.0f - S) / n);
        printf("threads=%2d  speedup   : %.2fx\n", n, speedup);
    }
    printf("----------------------------------\n");
    graph_free(g); 
    vcd_close();
    return 0;
}
