# Process this file with autoconf to produce a configure script.
dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([e_test_runner], [0.1.0], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])

AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([1.11])

GETTEXT_PACKAGE="e_test_runner"
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], "$GETTEXT_PACKAGE", [Gettext package])

AC_USE_SYSTEM_EXTENSIONS
AM_PROG_CC_STDC
AM_PROG_CC_C_O

## X11
AC_ARG_WITH([x11],
            AC_HELP_STRING([--with-x11], [use x11]),
            [WITH_X="yes"
            AC_SUBST([WITH_X])], [WITH_X="no"])
if test "x${WITH_X}" = x"yes"; then
  AC_DEFINE_UNQUOTED([HAVE_X],[1],[define HAVE_X])
fi
AM_CONDITIONAL(HAVE_X, [test "${WITH_X}" = x"yes"])

## Wayland
AC_ARG_WITH([wayland],
            AC_HELP_STRING([--with-wayland], [use wayland]),
            [WITH_WAYLAND="yes"
             AC_SUBST([WIHT_WAYLAND])], [WITH_WAYLAND="no"])
if test "x${WITH_WAYLAND}" = x"yes"; then
  AC_DEFINE_UNQUOTED([HAVE_WAYLAND],[1],[define HAVE_WAYLAND])
fi
AM_CONDITIONAL(HAVE_WAYLAND, [test "${WITH_WAYLAND}" = x"yes"])

requirements="\
   elementary \
   eldbus \
   enlightenment \
   capi-ui-efl-util"

if test "x$WITH_WAYLAND" = x"yes"; then
requirements="$requirements \
              wayland-client \
              ecore-wayland"
fi

PKG_CHECK_MODULES(E_TEST_RUNNER, [${requirements}])

AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
