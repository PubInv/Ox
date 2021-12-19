#ifndef NETWORKING_H
#define NETWORKING_H

#include <PIRCS.h>
#include <PIRDS.h>
#include <controller.h>
#include <server.h>
#include <task.h>

namespace Ox_Networking {

class NetworkingController : public Task {
public:
  bool setup(Task *callback);
  bool run(uint32_t msNow);
  bool connect();
  bool send_udp(const void *data, size_t s);
};

}

#endif