/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#include <PIRCS.h>
#include <core/task.h>
#include <server.h>

extern "C" {
#include "mongoose.h"
}

namespace Ox_Networking {

bool lock = false;
struct mg_mgr mgr;
mg_connection *c_rest;
mg_connection *c_udp;
Task *callbackTask;

bool server_init(Task *task) {
  lock = true;
  callbackTask = callback;
  mg_mgr_init(&mgr);
  c_rest = mg_http_listen(&mgr, s_listen_on, server_rest_cb, NULL);
  c_udp = mg_connect(&mgr, s_udp, server_udp_cb, NULL);
  return true;
}

void server_poll() {
  // printf("polling network\n");
  mg_mgr_poll(&mgr, 5); // second parameter is the wait time
}

int server_send_udp(const void *data, size_t s) {
  return mg_send(c_udp, data, s);
}

void server_rest_cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    if (mg_http_match_uri(hm, "/api/rest")) {
      // Serve REST response
      mg_http_reply(c, 200, "", "{\"result\": %d}\n", 123);
    } else if (mg_http_match_uri(hm, "/api/pircs")) {
      // curl -d '{"com":"C","par":"P","int":"T","mod":"0","val":400}' -H
      // "Content-Type: application/json" -X POST
      // http://localhost:8001/api/pircs
      // printf("%s\n", hm->body.ptr);
      char *buff = strdup(hm->body.ptr);
      printf("API CALLED");
      printf("%s\n", buff);
      //SetCommand sc = get_set_command_from_JSON(buff, (uint16_t)256); // PIRCS
      //printf("%c %c %c %c %i\n", sc.command, sc.parameter, sc.interpretation, sc.modifier, sc.val);
      //ventController->setCommand(sc);
      callbackTask->callback(buff);
      free(buff);
      mg_http_reply(c, 200, "", "%s\n", hm->body);
    }
  }
  (void)fn_data;
}

void server_udp_cb(struct mg_connection *c, int ev, void *ev_data,
                   void *fn_data) {
  // printf("%i\n", ev); // 1 =  MG_EV_POLL
  // printf("%u\n", ev_data); // mg_mgr_poll iteration, unsigned long *millis
  (void)fn_data;
}

bool server_close() {
  mg_mgr_free(&mgr);
  lock = false;
  return true;
}

} // namespace Ox_Networking
