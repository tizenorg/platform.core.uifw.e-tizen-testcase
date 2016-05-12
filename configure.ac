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

requirements="\
   elementary \
   eldbus \
   enlightenment \
   capi-ui-efl-util \
   wayland-client \
   ecore-wayland \
   tizen-extension-client"

PKG_CHECK_MODULES(E_TEST_RUNNER, [${requirements}])

AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
