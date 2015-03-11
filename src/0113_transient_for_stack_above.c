#include "e_test_case_main.h"
#include "e_test_case_util.h"
#include "0110_transient_for.h"

static Eldbus_Signal_Handler *sh;

static void
_signal_cb_restack(void *data EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *errname, *errmsg;

   eldbus_signal_handler_del(sh);
   sh = NULL;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
        return;
     }

   elm_exit();
}

Eina_Bool
test_case_0113_transient_for_stack_above(E_Test_Case *tc)
{
   E_TC_Client *client = NULL, *above = NULL;
   Eina_List *l;
   Eina_Bool passed = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);

   if (eldbus_proxy_call(dbus_proxy, "ChangeStack", NULL,
                          NULL, -1, "uiu", _parent->win, 2, _tc_data->client->win))
     return EINA_FALSE;

   sh = eldbus_proxy_signal_handler_add(dbus_proxy,
                                        "Restack",
                                        _signal_cb_restack,
                                        NULL);

   elm_run();

   if (sh) eldbus_signal_handler_del(sh);

   e_test_case_util_get_clients(_tc_data);
   EINA_LIST_FOREACH(_tc_data->clients, l, client)
     {
        if (client->layer > _tc_data->client->layer)
          continue;
        if (client->layer < _tc_data->client->layer)
          break;

        if (client->win == _parent->win)
          {
             if ((above) && (above->win == _child->win))
               passed = EINA_TRUE;

             break;
          }
        else if (client->win != _child->win)
          break;

        above = client;
     }

   return passed;

}

