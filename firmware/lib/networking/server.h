#ifndef SERVER_H
#define SERVER_H

#include <PIRCS.h>
#include <controller.h>

extern "C" {
#include "mongoose.h"
}

namespace Ox_Networking {

constexpr char *s_listen_on = (char *)"http://localhost:8001";
void server_rest_cb(struct mg_connection *c, int ev, void *ev_data,
                    void *fn_data);
void server_udp_cb(struct mg_connection *c, int ev, void *ev_data,
                   void *fn_data);

constexpr char *s_udp = (char *)"udp://localhost:6111";

void server_poll();
int server_send_udp(const void *data, size_t s);
bool server_init(VentController *vc);
bool server_close();

} // namespace Ox_Networking

#endif