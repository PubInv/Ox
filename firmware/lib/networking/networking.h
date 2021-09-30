#ifndef NETWORKING_H
#define NETWORKING_H

#include <PIRCS.h>
#include <PIRDS.h>
#include <controller.h>
#include <server.h>
#include <task.h>

// using Ox_Networking;

namespace Ox {

class NetworkingController : public Task {
public:
  bool setup(VentController *vc);
  bool run(uint32_t msNow);
  bool connect();
  bool send_udp(const void *data, size_t s);
};

} // namespace Ox

#endif