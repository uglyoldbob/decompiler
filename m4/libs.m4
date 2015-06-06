# AC_CHECK_FRAMEWORK(FRAMEWORK, FUNCTION,
#              [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#              [OTHER-FRAMEWORKS])
# ------------------------------------------------------
#
# Use a cache variable name containing both the library and function name,
# because the test really is for library $1 defining function $2, not
# just for library $1.  Separate tests with the same $1 and different $2s
# may have different results.
#
# Note that using directly AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2])
# is asking for troubles, since AC_CHECK_LIB($lib, fun) would give
# ac_cv_lib_$lib_fun, which is definitely not what was meant.  Hence
# the AS_LITERAL_IF indirection.
#
# FIXME: This macro is extremely suspicious.  It DEFINEs unconditionally,
# whatever the FUNCTION, in addition to not being a *S macro.  Note
# that the cache does depend upon the function we are looking for.
#
# It is on purpose we used `ac_check_lib_save_LIBS' and not just
# `ac_save_LIBS': there are many macros which don't want to see `LIBS'
# changed but still want to use AC_CHECK_LIB, so they save `LIBS'.
# And ``ac_save_LIBS' is too tempting a name, so let's leave them some
# freedom.
AC_DEFUN([AC_CHECK_FRAMEWORK],
[m4_ifval([$3], , [AH_CHECK_FRAMEWORK([$1])])dnl
AS_LITERAL_IF([$1],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2])],
	      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1''_$2])])dnl
AC_CACHE_CHECK([for $2 in -framework $1], [ac_Lib],
[ac_check_lib_save_LIBS=$LIBS
LIBS="-framework $1 $5 $LIBS"
AC_LINK_IFELSE([AC_LANG_CALL([], [$2])],
	       [AS_VAR_SET([ac_Lib], [yes])],
	       [AS_VAR_SET([ac_Lib], [no])])
LIBS=$ac_check_lib_save_LIBS])
AS_IF([test AS_VAR_GET([ac_Lib]) = yes],
      [m4_default([$3], [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB$1))
  LIBS="-framework $1 $LIBS"
])],
      [$4])dnl
AS_VAR_POPDEF([ac_Lib])dnl
])# AC_CHECK_FRAMEWORK


# AH_CHECK_FRAMEWORK(LIBNAME)
# ---------------------
m4_define([AH_CHECK_FRAMEWORK],
[AH_TEMPLATE(AS_TR_CPP([HAVE_LIB$1]),
	     [Define to 1 if you have the `$1' framework (-l$1).])])
