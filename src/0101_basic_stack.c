#include "e.h"
#include "e_test_case_main.h"
#include "e_test_case_util.h"
#include "0100_basic.h"

Eina_Bool
test_case_0101_basic_stack(E_Test_Case *tc EINA_UNUSED)
{
   E_TC_Client *client;
   Eina_Bool passed = EINA_FALSE;
   Eina_List *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data->client, EINA_FALSE);

   e_test_case_util_get_clients(_tc_data);

   EINA_LIST_FOREACH(_tc_data->clients, l, client)
     {
        if (client->layer > _tc_data->client->layer)
          continue;
        if (client->layer < _tc_data->client->layer)
          break;

        if (!strncmp(client->name, _tc_data->client->name, strlen(client->name)))
          passed = EINA_TRUE;

        break;
     }

   return passed;
}
