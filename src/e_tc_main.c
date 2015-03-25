#include "e_tc_main.h"

int _log_dom = -1;

static void
_cb_method_win_info_list_get(void *data,
                             const Eldbus_Message *msg,
                             Eldbus_Pending *p EINA_UNUSED)
{
   const char *name = NULL, *text = NULL;
   Eldbus_Message_Iter *array, *ec;
   Ecore_Window target_win = 0;
   Eina_List *list = data;
   Eina_Bool res;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   res = eldbus_message_arguments_get(msg, "ua(usiiiiibb)", &target_win, &array);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   list = eina_list_free(list);

   while (eldbus_message_iter_get_and_next(array, 'r', &ec))
     {
        const char *win_name;
        int x, y, w, h, layer;
        Eina_Bool visible, alpha;
        Ecore_Window native_win;
        E_TC_Win *tw = NULL;

        res = eldbus_message_iter_arguments_get(ec,
                                                "usiiiiibb",
                                                &native_win,
                                                &win_name,
                                                &x,
                                                &y,
                                                &w,
                                                &h,
                                                &layer,
                                                &visible,
                                                &alpha);
        if (!res)
          {
             WRN("Failed to get win info\n");
             continue;
          }

        tw = e_tc_win_info_add(native_win, alpha, win_name, x, y, w, h, layer);
        list = eina_list_append(list, tw);
     }

finish:
   if ((name) || (text))
     {
        ERR("errname:%s errmsg:%s\n", name, text);
     }

   elm_exit();
}

static void
_cb_method_window_register(void *data,
                           const Eldbus_Message *msg,
                           Eldbus_Pending *p EINA_UNUSED)
{
   E_TC *tc = (E_TC *)data;
   const char *name = NULL, *text = NULL;
   Eina_Bool res;
   Eina_Bool *accepted = data;

   EINA_SAFETY_ON_NULL_GOTO(tc, finish);

   *accepted = EINA_FALSE;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   res = eldbus_message_arguments_get(msg, "b", accepted);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

finish:
   if ((name) || (text))
     {
        ERR("errname:%s errmsg:%s\n", name, text);
     }

   elm_exit();
}

static void
_cb_method_window_deregister(void *data,
                             const Eldbus_Message *msg,
                             Eldbus_Pending *p EINA_UNUSED)
{
   const char *name = NULL, *text = NULL;
   Eina_Bool res;
   Eina_Bool *allowed = data;

   *allowed = EINA_FALSE;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   res = eldbus_message_arguments_get(msg, "b", &allowed);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

finish:
   if ((name) || (text))
     {
        ERR("errname:%s errmsg:%s\n", name, text);
     }

   elm_exit();
}

static void
_cb_signal_vis_changed(void *data,
                       const Eldbus_Message *msg)
{
   E_TC_Runner *runner = data;
   const char *name = NULL, *text = NULL;
   Eina_Bool res;
   int vis = 0;
   Ecore_Window id;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   res = eldbus_message_arguments_get(msg, "ub", &id, &vis);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   /* TODO */
   if (((E_TC_EVENT_TYPE_VIS_ON  == runner->ev.expect) && ( vis)) ||
       ((E_TC_EVENT_TYPE_VIS_OFF == runner->ev.expect) && (!vis)))
     {
        runner->ev.response = runner->ev.expect;
        elm_exit();
     }

finish:
   if ((name) || (text))
     {
        ERR("errname:%s errmsg:%s\n", name, text);
     }
}

static void
_cb_signal_stack_changed(void *data,
                         const Eldbus_Message *msg)
{
   E_TC_Runner *runner = data;
   const char *name = NULL, *text = NULL;
   Eina_Bool res;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   /* TODO */
   if ((E_TC_EVENT_TYPE_STACK_RAISE <= runner->ev.expect) &&
       (E_TC_EVENT_TYPE_STACK_BELOW >= runner->ev.expect))
     {
        runner->ev.response = runner->ev.expect;
        elm_exit();
     }

finish:
   if ((name) || (text))
     {
        ERR("errname:%s errmsg:%s\n", name, text);
     }
}

static Eina_Bool
_ev_wait_timeout(void *data)
{
   E_TC_Runner *runner = data;

   runner->ev.expire_timer = NULL;
   runner->ev.response = E_TC_EVENT_TYPE_TIMEOUT;

   elm_exit();

   return ECORE_CALLBACK_DONE;
}

Eina_Bool
e_tc_runner_req_win_register(E_TC_Runner *runner,
                             E_TC_Win *tw)
{
   Eldbus_Pending *p;
   Eina_Bool accepted = EINA_FALSE;

   p = eldbus_proxy_call(runner->dbus.proxy,
                         "RegisterWindow",
                         _cb_method_window_register,
                         &accepted,
                         -1,
                         "u",
                         tw->native_win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p, EINA_FALSE);

   elm_run();

   return accepted;
}

Eina_Bool
e_tc_runner_req_win_deregister(E_TC_Runner *runner,
                               E_TC_Win *tw)
{
   Eldbus_Pending *p;
   Eina_Bool allowed = EINA_FALSE;

   p = eldbus_proxy_call(runner->dbus.proxy,
                         "DeregisterWindow",
                         _cb_method_window_deregister,
                         &allowed,
                         -1,
                         "u",
                         tw->native_win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p, EINA_FALSE);

   elm_run();

   return allowed;
}

Eina_List *
e_tc_runner_req_win_info_list_get(E_TC_Runner *runner)
{
   Eldbus_Pending *p;
   Eina_List *list = NULL;

   p = eldbus_proxy_call(runner->dbus.proxy,
                         "GetClients",
                         _cb_method_win_info_list_get,
                         list,
                         -1,
                         "");
   EINA_SAFETY_ON_NULL_RETURN_VAL(p, NULL);

   elm_run();

   return list;
}

Eina_Bool
e_tc_runner_ev_wait(E_TC_Runner *runner,
                    E_TC_Event_Type ev)
{
   Eldbus_Signal_Handler *sh;
   Eina_Bool res = EINA_FALSE;

   runner->ev.expect = ev;
   runner->ev.response = E_TC_EVENT_TYPE_NONE;

   switch (ev)
     {
      case E_TC_EVENT_TYPE_VIS_ON:
      case E_TC_EVENT_TYPE_VIS_OFF:
         sh = eldbus_proxy_signal_handler_add(runner->dbus.proxy,
                                              "ChangeVisibility",
                                              _cb_signal_vis_changed,
                                              runner);
         EINA_SAFETY_ON_NULL_GOTO(sh, finish);
         break;

      case E_TC_EVENT_TYPE_STACK_RAISE:
      case E_TC_EVENT_TYPE_STACK_LOWER:
      case E_TC_EVENT_TYPE_STACK_ABOVE:
      case E_TC_EVENT_TYPE_STACK_BELOW:
         sh = eldbus_proxy_signal_handler_add(runner->dbus.proxy,
                                              "Restack",
                                              _cb_signal_stack_changed,
                                              runner);
         EINA_SAFETY_ON_NULL_GOTO(sh, finish);
         break;

      default:
         goto finish;
         break;
     }

   runner->ev.expire_timer = ecore_timer_add(2.0, _ev_wait_timeout, runner);

   elm_run();

   if (runner->ev.expire_timer)
     {
        ecore_timer_del(runner->ev.expire_timer);
        runner->ev.expire_timer = NULL;
     }

   eldbus_signal_handler_del(sh);

   res = (runner->ev.response == runner->ev.expect);

finish:
   runner->ev.expect = E_TC_EVENT_TYPE_NONE;
   runner->ev.response = E_TC_EVENT_TYPE_NONE;

   return res;
}

E_TC_Win *
e_tc_win_add(E_TC_Win *parent,
             Elm_Win_Type type,
             Eina_Bool alpha,
             const char *name,
             int x, int y,
             int w, int h,
             int layer)
{
   E_TC_Win *tw = NULL;
   Evas_Object *elm_win = NULL, *bg = NULL;
   Evas_Object *p_elm_win = NULL;

   if (parent) p_elm_win = tw->elm_win;

   elm_win = elm_win_add(p_elm_win, name, ELM_WIN_BASIC);
   EINA_SAFETY_ON_NULL_GOTO(elm_win, err);

   if (alpha) elm_win_alpha_set(elm_win, EINA_TRUE);

   elm_win_title_set(elm_win, name);
   elm_win_autodel_set(elm_win, EINA_FALSE);

   bg = elm_bg_add(elm_win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(elm_win, bg);
   elm_bg_color_set(bg, 0, 120, 100);
   evas_object_show(bg);

   tw = E_NEW(E_TC_Win, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tw, NULL);

   tw->elm_win = elm_win;
   tw->native_win = elm_win_xwindow_get(elm_win); // TODO: wayland
   tw->name = eina_stringshare_add(name);
   tw->x = x;
   tw->y = y;
   tw->w = w;
   tw->h = h;
   tw->layer = layer;
   tw->alpha = alpha;

   return tw;

err:
   evas_object_del(elm_win);
   E_FREE(tw);

   return NULL;
}

E_TC_Win *
e_tc_win_info_add(Ecore_Window native_win,
                  Eina_Bool alpha,
                  const char *name,
                  int x, int y,
                  int w, int h,
                  int layer)
{
   E_TC_Win *tw = NULL;

   tw = E_NEW(E_TC_Win, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tw, NULL);

   tw->elm_win = NULL;
   tw->native_win = native_win;
   tw->name = eina_stringshare_add(name);
   tw->x = x;
   tw->y = y;
   tw->w = w;
   tw->h = h;
   tw->layer = layer;
   tw->alpha = alpha;

   return tw;
}

void
e_tc_win_del(E_TC_Win *tw)
{
   EINA_SAFETY_ON_NULL_RETURN(tw);

   if (tw->elm_win)
     evas_object_del(tw->elm_win);

   if (tw->name)
     eina_stringshare_del(tw->name);

   E_FREE(tw);
}

void
e_tc_win_geom_update(E_TC_Win *tw)
{
   evas_object_move(tw->elm_win, tw->x, tw->y);
   evas_object_resize(tw->elm_win, tw->w, tw->h);
}

void
e_tc_win_show(E_TC_Win *tw)
{
   evas_object_show(tw->elm_win);
}

void
e_tc_win_hide(E_TC_Win *tw)
{
   evas_object_hide(tw->elm_win);
}

void
e_tc_win_stack_change(E_TC_Win *tw, E_TC_Win *sibling, Eina_Bool above)
{
   /* TODO */
   if (sibling)
     {
        if (above)
          {
             ; // stack above: window is placed above sibling window.
          }
        else
          {
             ; // stack below: window is placed below sibling window.
          }
     }
   else
     {
        if (above)
          {
             ; // raise: window is placed at the top of stack.
          }
        else
          {
             ; // lower: window is placed at the bottom of stack.
          }
     }
}

static E_TC *
_e_tc_add(unsigned int num,
          const char *name,
          Eina_Bool (*func)(E_TC*),
          Eina_Bool expect,
          E_TC_Runner *runner)
{
   E_TC *tc;

   tc = E_NEW(E_TC, 1);

   tc->num = num;
   tc->name = name;
   tc->func = func;
   tc->expect = expect;
   tc->runner = runner;

   runner->tc_list = eina_list_append(runner->tc_list, tc);

   return tc;
}

#undef TC_ADD
#define TC_ADD(num, name, func, expect) _e_tc_add(num, name, func, expect, runner)

static void
_e_tc_runner_init(E_TC_Runner *runner)
{
#undef T_FUNC
#define T_FUNC(num_, test_) tc_000##num_##_##test_
   /* [0000 - 0099] test runner verification */
   TC_ADD(   0, "Base: Pass",                 T_FUNC(   0, base_pass                ), 1);
   TC_ADD(   1, "Base: Fail",                 T_FUNC(   1, base_fail                ), 0);
#undef T_FUNC
#define T_FUNC(num_, test_) tc_00##num_##_##test_
   TC_ADD(  10, "DBus: Introspect",           T_FUNC(  10, introspect               ), 1);
   TC_ADD(  11, "DBus: Window register",      T_FUNC(  11, win_register             ), 1);
#undef T_FUNC
#define T_FUNC(num_, test_) tc_0##num_##_##test_
   /* [0100 - 0199] window base operation */
   TC_ADD( 100, "Basic window: Show",         T_FUNC( 100, win_show                 ), 1);
   TC_ADD( 101, "Basic window: Stack",        T_FUNC( 101, win_stack                ), 1);
   TC_ADD( 110, "Alpha window: Show",         T_FUNC( 110, alpha_win_show           ), 1);
   TC_ADD( 111, "Alpha window: Stack",        T_FUNC( 111, alpha_win_stack          ), 1);
   /* [0200 - 0299] transient for */
   TC_ADD( 200, "Transient for: Basic",       T_FUNC( 200, transient_for_basic      ), 1);
   TC_ADD( 201, "Transient for: Raise",       T_FUNC( 201, transient_for_raise      ), 1);
   TC_ADD( 202, "Transient for: Lower",       T_FUNC( 202, transient_for_lower      ), 1);
   TC_ADD( 203, "Transient for: Stack above", T_FUNC( 203, transient_for_stack_above), 1);
   TC_ADD( 204, "Transient for: Stack below", T_FUNC( 204, transient_for_stack_below), 1);
   /* [0300 - 0399] notification */
   /* TODO */
#undef T_FUNC
}

static void
_e_tc_runner_shutdown(E_TC_Runner *runner)
{
   E_TC *tc;

   EINA_LIST_FREE(runner->tc_list, tc)
     {
        E_FREE(tc);
     }
}

static void
_e_tc_runner_run(E_TC_Runner *runner)
{
   Eina_List *l;
   E_TC *tc;
   Eina_Bool pass;

   EINA_LIST_FOREACH(runner->tc_list, l, tc)
     {
        pass = tc->func(tc);
        tc->passed = (pass == tc->expect);

        printf("TEST \"%s\" : %s\n",
               tc->name,
               tc->passed ? "PASS" : "FAIL");
     }
}

static void
_e_tc_runner_result(E_TC_Runner *runner)
{
   Eina_Strbuf *buf;
   Eina_List *l;
   E_TC *tc;
   int pass_case = 0;
   int fail_case = 0;
   int total = 0;

   if (!(buf = eina_strbuf_new())) return;

   eina_strbuf_append(buf, "\n\n");
   eina_strbuf_append(buf, "==============================================\n");
   eina_strbuf_append(buf, "TEST CASE RESULT\n");
   eina_strbuf_append(buf, "==============================================\n");

   EINA_LIST_FOREACH(runner->tc_list, l, tc)
     {
        eina_strbuf_append_printf(buf, "[%04d] TEST \"%-25.25s\" : %s\n",
                                  tc->num, tc->name, tc->passed ? "PASS" : "FAIL");

        total++;
        tc->passed? pass_case++ : fail_case++;

        if (!tc->passed)
          {
             eina_strbuf_append_printf(buf, "\n!!Test Case failed at \"%s\"\n", tc->name);
             break;
          }
     }

   eina_strbuf_append(buf, "==============================================\n");
   eina_strbuf_append_printf(buf, "TOTAL: %4d Case\n", total);
   eina_strbuf_append_printf(buf, "PASS : %4d Case\n", pass_case);
   eina_strbuf_append_printf(buf, "FAIL : %4d Case\n", fail_case);
   eina_strbuf_append(buf, "==============================================\n");

   printf("%s", eina_strbuf_string_get(buf));
}

EAPI_MAIN int
elm_main(int argc EINA_UNUSED,
         char **argv EINA_UNUSED)
{
   E_TC_Runner *runner = NULL;
   int res;

   runner = E_NEW(E_TC_Runner, 1);
   EINA_SAFETY_ON_NULL_GOTO(runner, err);

   _log_dom = eina_log_domain_register("e-tc", EINA_COLOR_BLUE);
   if (_log_dom < 0)
     {
        ERR("Can't register e-tc log domain.");
        goto err;
     }

   res = eldbus_init();
   EINA_SAFETY_ON_FALSE_GOTO((res > 0), err);

   runner->dbus.conn = eldbus_connection_get(ELDBUS_CONNECTION_TYPE_SYSTEM);
   if (!runner->dbus.conn)
     {
        ERR("Can't get dbus connection.");
        goto err;
     }

   runner->dbus.obj = eldbus_object_get(runner->dbus.conn,
                                        "org.enlightenment.wm",
                                        "/org/enlightenment/wm");
   if (!runner->dbus.obj)
     {
        ERR("Can't get dbus object.");
        goto err;
     }

   runner->dbus.proxy = eldbus_proxy_get(runner->dbus.obj,
                                         "org.enlightenment.wm.Test");
   if (!runner->dbus.proxy)
     {
        ERR("Can't get dbus proxy.");
        goto err;
     }

   _e_tc_runner_init(runner);
   _e_tc_runner_run(runner);
   _e_tc_runner_result(runner);
   _e_tc_runner_shutdown(runner);

   eldbus_proxy_unref(runner->dbus.proxy);
   eldbus_object_unref(runner->dbus.obj);
   eldbus_connection_unref(runner->dbus.conn);
   eldbus_shutdown();
   eina_log_domain_unregister(_log_dom);

   E_FREE(runner);
   return 0;

err:
   E_FREE(runner);
   return -1;
}
ELM_MAIN()
