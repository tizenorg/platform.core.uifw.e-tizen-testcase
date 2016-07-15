#include "e_test_runner.h"

struct _E_TC_Data
{
   E_TC_Win *tw;
   E_TC_Win *tw_parent;
   E_TC_Win *tw_child;
   E_TC_Win *tw_register;
};

static Eina_Bool _tc_transient_for_win_register(E_TC *tc, E_TC_Win *win);
static void      _tc_transient_for_win_unregister(E_TC *tc);
static Eina_Bool _tc_transient_for_pre_run(E_TC *tc);
static void      _tc_transient_for_post_run(E_TC *tc);
static void      _tc_transient_for_shutdown(E_TC *tc);

static Eina_Bool
_tc_transient_for_win_register(E_TC *tc, E_TC_Win *win)
{
   Eina_Bool res;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc->data, EINA_FALSE);

   EINA_SAFETY_ON_NULL_RETURN_VAL(win, EINA_FALSE);

   if (tc->data->tw_register)
     {
        e_test_runner_req_win_deregister(tc->runner, tc->data->tw_register);
        tc->data->tw_register = NULL;
     }

   tc->data->tw_register = win;

   res = e_test_runner_req_win_register(tc->runner, tc->data->tw_register);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(res, EINA_FALSE);

   return EINA_TRUE;
}

static void
_tc_transient_for_win_unregister(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc);
   EINA_SAFETY_ON_NULL_RETURN(tc->data);
   EINA_SAFETY_ON_NULL_RETURN(tc->data->tw_register);

   e_test_runner_req_win_deregister(tc->runner, tc->data->tw_register);
   tc->data->tw_register = NULL;
}

static Eina_Bool
_tc_win_show(E_TC *tc, E_TC_Win *tw)
{
   Eina_Bool res = EINA_FALSE;

   res = _tc_transient_for_win_register(tc, tw);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(res, EINA_FALSE);

   e_tc_win_geom_update(tw);
   e_tc_win_show(tw);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);

   return EINA_TRUE;
}

static Eina_Bool
_tc_transient_for_pre_run(E_TC *tc)
{
   Eina_Bool res;

   tc->data = E_NEW(E_TC_Data, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc->data, EINA_FALSE);

   tc->data->tw_parent = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                      EINA_FALSE, "parent",
                                      0, 0, 400, 400, EINA_FALSE,
                                      200, E_TC_WIN_COLOR_BLUE);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_parent, cleanup);

   tc->data->tw_child = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                     EINA_FALSE, "child",
                                     0, 0, 320, 320, EINA_FALSE,
                                     200, E_TC_WIN_COLOR_RED);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_child, cleanup);

   tc->data->tw = e_tc_win_add(NULL, ELM_WIN_BASIC,
                               EINA_FALSE, "tc",
                               200, 200, 500, 500, EINA_FALSE,
                               200, E_TC_WIN_COLOR_GREEN);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw, cleanup);

   // show tw_parent
   res = _tc_win_show(tc, tc->data->tw_parent);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show tw
   res = _tc_win_show(tc, tc->data->tw);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);


   // show tw_child
   res = _tc_win_show(tc, tc->data->tw_child);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);


   // set transient_for
   res = e_tc_win_transient_for_set(tc->data->tw_child,
                                    tc->data->tw_parent,
                                    EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   e_test_runner_work();

   return EINA_TRUE;

cleanup:
   _tc_transient_for_shutdown(tc);
   return EINA_FALSE;
}

static void
_tc_transient_for_post_run(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   e_tc_win_hide(tc->data->tw_child);
   e_tc_win_hide(tc->data->tw);
   e_tc_win_hide(tc->data->tw_parent);
}

static void
_tc_transient_for_shutdown(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   _tc_transient_for_win_unregister(tc);
   e_tc_win_del(tc->data->tw_child);
   e_tc_win_del(tc->data->tw_parent);
   e_tc_win_del(tc->data->tw);

   E_FREE(tc->data);
}

Eina_Bool
tc_0200_transient_for_basic(E_TC *tc)
{
   E_TC_Win *tw, *tw_main, *tw_parent, *tw_child;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_transient_for_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_main = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   // Expected stack result:
   // [Top] tw_main -> tw_child -> tw_parent [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_main->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_child->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_parent->native_win), cleanup);
             pass_count++;
          }
        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_transient_for_post_run(tc);
   _tc_transient_for_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0201_transient_for_raise(E_TC *tc)
{
   E_TC_Win *tw, *tw_main, *tw_parent, *tw_child;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_transient_for_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_main = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   // Expected stack result:
   // [Top] tw_main -> tw_child -> tw_parent [Bottom]

   res = _tc_transient_for_win_register(tc, tw_parent);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_test_runner_req_win_stack_set(tc->runner, tw_parent, NULL, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] tw_child -> tw_parent -> tw_main [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_child->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_parent->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_main->native_win), cleanup);
             pass_count++;
          }
        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_transient_for_post_run(tc);
   _tc_transient_for_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0202_transient_for_lower(E_TC *tc)
{
   E_TC_Win *tw, *tw_main, *tw_parent, *tw_child;
   E_TC_Win *tw_below = NULL;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_transient_for_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_main = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   // Expected stack result:
   // [Top] tw_main -> tw_child -> tw_parent [Bottom]

   res = _tc_transient_for_win_register(tc, tw_parent);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // Raise Transient_for Parent window
   e_test_runner_req_win_stack_set(tc->runner, tw_parent, NULL, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] tw_child -> tw_parent -> tw_main [Bottom]


   // lower tw_parent
   e_test_runner_req_win_stack_set(tc->runner, tw_parent, NULL, EINA_FALSE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_LOWER);

   // Expected stack result:
   // [Top] tw_main -> ... -> tw_child -> tw_parent [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   // bottom to top search
   EINA_LIST_REVERSE_FOREACH(list, l, tw)
     {
        if (tw->layer < 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_parent->native_win), cleanup);
             tw_below = tw;
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_child->native_win), cleanup);
             tw_below = tw;
             pass_count++;
             continue;
          }
        else
          {
             if (tw->layer > 200)
               {
                  EINA_SAFETY_ON_FALSE_GOTO((tw_below->native_win == tw_main->native_win), cleanup);
                  pass_count++;
               }
             else
               {
                  tw_below = tw;
                  continue;
               }
          }
        break;
     }
   // check the tw_below if there is no window over 200 layer.
   if (tw == NULL)
     {
        EINA_SAFETY_ON_FALSE_GOTO((pass_count == 2), cleanup);
        EINA_SAFETY_ON_NULL_GOTO(tw_below, cleanup);
        EINA_SAFETY_ON_FALSE_GOTO((tw_below->native_win == tw_main->native_win), cleanup);
        pass_count++;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_transient_for_post_run(tc);
   _tc_transient_for_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0203_transient_for_stack_above(E_TC *tc)
{
   E_TC_Win *tw, *tw_main, *tw_parent, *tw_child;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_transient_for_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_main = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   // Expected stack result:
   // [Top] tw_main -> tw_child -> tw_parent [Bottom]

   res = _tc_transient_for_win_register(tc, tw_parent);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_test_runner_req_win_stack_set(tc->runner, tw_parent, tw_main, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_ABOVE);

   // Expected stack result:
   // [Top] tw_child -> tw_parent -> tw_main [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_child->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_parent->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_main->native_win), cleanup);
             pass_count++;
          }
        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_transient_for_post_run(tc);
   _tc_transient_for_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0204_transient_for_stack_below(E_TC *tc)
{
   E_TC_Win *tw, *tw_main, *tw_parent, *tw_child;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_transient_for_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_main = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   // Expected stack result:
   // [Top] tw_main -> tw_child -> tw_parent [Bottom]

   res = _tc_transient_for_win_register(tc, tw_parent);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // raise tw_parent
   e_test_runner_req_win_stack_set(tc->runner, tw_parent, NULL, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] tw_child -> tw_parent -> tw_main [Bottom]

   e_test_runner_req_win_stack_set(tc->runner, tw_parent, tw_main, EINA_FALSE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_BELOW);

   // Expected stack result:
   // [Top] tw_main -> tw_child -> tw_parent  [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   // bottom to top search
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_main->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_child->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_parent->native_win), cleanup);
             pass_count++;
          }
        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_transient_for_post_run(tc);
   _tc_transient_for_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}
