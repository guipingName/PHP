dnl $Id$
dnl config.m4 for extension wechat

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(wechat, for wechat support,
Make sure that the comment is aligned:
[  --with-wechat             Include wechat support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(wechat, whether to enable wechat support,
dnl Make sure that the comment is aligned:
dnl [  --enable-wechat           Enable wechat support])

if test "$PHP_WECHAT" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-wechat -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/wechat.h"  # you most likely want to change this
  dnl if test -r $PHP_WECHAT/$SEARCH_FOR; then # path given as parameter
  dnl   WECHAT_DIR=$PHP_WECHAT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for wechat files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       WECHAT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$WECHAT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the wechat distribution])
  dnl fi

  dnl # --with-wechat -> add include path
  dnl PHP_ADD_INCLUDE($WECHAT_DIR/include)

  dnl # --with-wechat -> check for lib and symbol presence
  dnl LIBNAME=wechat # you may want to change this
  dnl LIBSYMBOL=wechat # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WECHAT_DIR/lib, WECHAT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_WECHATLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong wechat lib version or lib not found])
  dnl ],[
  dnl   -L$WECHAT_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(WECHAT_SHARED_LIBADD)

  PHP_NEW_EXTENSION(wechat, auth_service.c wechat.c udp_bind.c udp_control.c udp_control_unpack.c udp_control_pack.c tea.c, $ext_shared)
fi
