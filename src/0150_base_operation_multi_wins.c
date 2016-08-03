#include "e_test_runner.h"

struct _E_TC_Data
{
   E_TC_Win *tw_red;
   E_TC_Win *tw_green;
   E_TC_Win *tw_blue;
   E_TC_Win *tw_register;
};

static Eina_Bool _tc_win_register(E_TC *tc, E_TC_Win *win);
static void      _tc_win_unregister(E_TC *tc);
static Eina_Bool _tc_multi_wins_pre_run(E_TC *tc, Eina_Bool alpha);
static void      _tc_multi_wins_post_run(E_TC *tc);
static void      _tc_multi_wins_shutdown(E_TC *tc);

static Eina_Bool
_tc_win_register(E_TC *tc, E_TC_Win *win)
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
_tc_win_unregister(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc);
   EINA_SAFETY_ON_NULL_RETURN(tc->data);
   EINA_SAFETY_ON_NULL_RETURN(tc->data->tw_register);

   e_test_runner_req_win_deregister(tc->runner, tc->data->tw_register);
   tc->data->tw_register = NULL;
}


static Eina_Bool
_tc_multi_wins_pre_run(E_TC *tc, Eina_Bool show_win)
{
   tc->data = E_NEW(E_TC_Data, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc->data, EINA_FALSE);

   tc->data->tw_red = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                   EINA_FALSE, "red",
                                   0, 0, 400, 400, EINA_TRUE,
                                   200, E_TC_WIN_COLOR_RED);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_red, cleanup);

   if (show_win)
     {
        e_tc_win_geom_update(tc->data->tw_red);
        e_tc_win_show(tc->data->tw_red);
     }

   tc->data->tw_green = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                     EINA_FALSE, "green",
                                     150, 150, 400, 400, EINA_TRUE,
                                     200, E_TC_WIN_COLOR_GREEN);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_green, cleanup);

   if (show_win)
     {
        e_tc_win_geom_update(tc->data->tw_green);
        e_tc_win_show(tc->data->tw_green);
     }

   tc->data->tw_blue = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                    EINA_FALSE, "blue",
                                    20, 350, 400, 400, EINA_TRUE,
                                    200, E_TC_WIN_COLOR_BLUE);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_blue, cleanup);

   if (show_win)
     {
        _tc_win_register(tc, tc->data->tw_blue);

        e_tc_win_geom_update(tc->data->tw_blue);
        e_tc_win_show(tc->data->tw_blue);
        e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);
     }

   e_test_runner_work();

   return EINA_TRUE;

cleanup:
   _tc_multi_wins_shutdown(tc);
   return EINA_FALSE;
}

static void
_tc_multi_wins_post_run(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   e_tc_win_hide(tc->data->tw_red);
   e_tc_win_hide(tc->data->tw_green);
   e_tc_win_hide(tc->data->tw_blue);
}

static void
_tc_multi_wins_shutdown(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   _tc_win_unregister(tc);
   e_tc_win_del(tc->data->tw_red);
   e_tc_win_del(tc->data->tw_green);
   e_tc_win_del(tc->data->tw_blue);

   E_FREE(tc->data);
}

static Eina_Bool
_tc_win_show(E_TC *tc, E_TC_Win *tw)
{
   Eina_Bool res = EINA_FALSE;

   res = _tc_win_register(tc, tw);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(res, EINA_FALSE);

   e_tc_win_geom_update(tw);
   e_tc_win_show(tw);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);

   return EINA_TRUE;
}

Eina_Bool
tc_0150_multi_all_wins_basic(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0151_multi_all_wins_show1(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   res = _tc_multi_wins_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // show red
   res = _tc_win_show(tc, tc->data->tw_red);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show green
   res = _tc_win_show(tc, tc->data->tw_green);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show blue
   res = _tc_win_show(tc, tc->data->tw_blue);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0152_multi_all_wins_show2(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // show blue
   res = _tc_win_show(tc, tc->data->tw_blue);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show red
   res = _tc_win_show(tc, tc->data->tw_red);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show green
   res = _tc_win_show(tc, tc->data->tw_green);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // Expected stack result:
   // [Top] Green -> Red -> Blue [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}


Eina_Bool
tc_0153_multi_all_wins_show3(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // show green
   res = _tc_win_show(tc, tc->data->tw_green);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show blue
   res = _tc_win_show(tc, tc->data->tw_blue);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show red
   res = _tc_win_show(tc, tc->data->tw_red);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // Expected stack result:
   // [Top] Red -> Blue -> Green [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0154_multi_all_wins_raise1(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Raise Red
   e_test_runner_req_win_stack_set(tc->runner, tw_red, NULL, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] Red -> Blue -> Green [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0155_multi_all_wins_raise2(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Raise Green
   e_test_runner_req_win_stack_set(tc->runner, tw_green, NULL, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] Green -> Blue -> Red [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0156_multi_all_wins_stack_above1(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Raise Red on the Green
   e_test_runner_req_win_stack_set(tc->runner, tw_red, tw_green, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] Blue -> Red -> Green [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}


Eina_Bool
tc_0157_multi_all_wins_stack_above2(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Raise Blue on the Red
   e_test_runner_req_win_stack_set(tc->runner, tw_blue, tw_red, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] Green -> Blue -> Red [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0158_multi_all_wins_stack_above3(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Raise Red on the Blue
   e_test_runner_req_win_stack_set(tc->runner, tw_red, tw_blue, EINA_TRUE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] Red -> Blue -> Green [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0159_multi_all_wins_lower1(E_TC *tc)
{
   E_TC_Win *tw, *tw_above = NULL;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Lower Blue
   e_test_runner_req_win_stack_set(tc->runner, tw_blue, NULL, EINA_FALSE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);

   // Expected stack result:
   // [Top] Green -> Red -> ... -> Blue  [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             tw_above = tw;
             continue;

          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             tw_above = tw;
             continue;
          }
        else
          {
             if (tw->layer < 200)
               {
                  EINA_SAFETY_ON_FALSE_GOTO((tw_above->native_win == tw_blue->native_win), cleanup);
                  pass_count++;
               }
             else
               {
                  tw_above = tw;
                  continue;
               }
          }

        break;
     }
   // check the tw_above when there is no window under 200 layer.
   if (tw == NULL)
     {
        EINA_SAFETY_ON_NULL_GOTO(tw_above, cleanup);
        EINA_SAFETY_ON_FALSE_GOTO((tw_above->native_win == tw_blue->native_win), cleanup);
        pass_count++;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}


Eina_Bool
tc_0160_multi_all_wins_lower2(E_TC *tc)
{
   E_TC_Win *tw, *tw_above = NULL;
   E_TC_Win *tw_red, *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // Expected stack result:
   // [Top] Blue -> Green -> Red [Bottom]

   // Lower Green
   e_test_runner_req_win_stack_set(tc->runner, tw_green, NULL, EINA_FALSE);
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_LOWER);

   // Expected stack result:
   // [Top] Blue -> Red -> ... -> Green [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             tw_above = tw;
             continue;
          }
        else if (pass_count == 1)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             tw_above = tw;
             continue;
          }
        else
          {
             if (tw->layer < 200)
               {
                  EINA_SAFETY_ON_FALSE_GOTO((tw_above->native_win == tw_green->native_win), cleanup);
                  pass_count++;
               }
             else
               {
                  tw_above = tw;
                  continue;
               }
          }

        break;
     }
   // check the tw_above when there is no window under 200 layer.
   if (tw == NULL)
     {
        EINA_SAFETY_ON_NULL_GOTO(tw_above, cleanup);
        EINA_SAFETY_ON_FALSE_GOTO((tw_above->native_win == tw_green->native_win), cleanup);
        pass_count++;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 3), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}


Eina_Bool
tc_0180_multi_2wins_show1(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_green;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_green = tc->data->tw_green;

   // show green
   e_tc_win_geom_update(tc->data->tw_green);
   e_tc_win_show(tc->data->tw_green);

   // register red
   res = _tc_win_register(tc, tc->data->tw_red);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show red
   e_tc_win_geom_update(tc->data->tw_red);
   e_tc_win_show(tc->data->tw_red);

   // wait for showing register_win
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);

   // Expected stack result:
   // [Top] Red -> Green [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 2), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}


Eina_Bool
tc_0181_multi_2wins_show2(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_green, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_green = tc->data->tw_green;
   tw_blue = tc->data->tw_blue;

   // show blue
   e_tc_win_geom_update(tc->data->tw_blue);
   e_tc_win_show(tc->data->tw_blue);

   // register green
   res = _tc_win_register(tc, tc->data->tw_green);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show green
   e_tc_win_geom_update(tc->data->tw_green);
   e_tc_win_show(tc->data->tw_green);

   // wait for showing register_win
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);

   // Expected stack result:
   // [Top] Green -> Blue [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_green->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 2), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}


Eina_Bool
tc_0182_multi_2wins_show3(E_TC *tc)
{
   E_TC_Win *tw;
   E_TC_Win *tw_red, *tw_blue;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;
   int pass_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_multi_wins_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw_red = tc->data->tw_red;
   tw_blue = tc->data->tw_blue;

   // show red
   e_tc_win_geom_update(tc->data->tw_red);
   e_tc_win_show(tc->data->tw_red);

   // register blue
   res = _tc_win_register(tc, tc->data->tw_blue);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   // show blue
   e_tc_win_geom_update(tc->data->tw_blue);
   e_tc_win_show(tc->data->tw_blue);

   // wait for showing register_win
   e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);

   // Expected stack result:
   // [Top] Blue -> Red [Bottom]

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   pass_count = 0;
   EINA_LIST_FOREACH(list, l, tw)
     {
        if (tw->layer > 200) continue;
        if (pass_count == 0)
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_blue->native_win), cleanup);
             pass_count++;
             continue;
          }
        else
          {
             EINA_SAFETY_ON_FALSE_GOTO((tw->native_win == tw_red->native_win), cleanup);
             pass_count++;
          }

        break;
     }

   EINA_SAFETY_ON_FALSE_GOTO((pass_count == 2), cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_multi_wins_post_run(tc);
   _tc_multi_wins_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

