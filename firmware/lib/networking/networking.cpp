#include <debug.h>
#include <networking.h>
namespace VentOS {

bool NetworkingController::setup(VentController *vc) {
  DebugLnCC("Networking setup\n");
  VentOS_Networking::server_init(vc);
  return true;
}

bool NetworkingController::run(uint32_t msNow) {
  VentOS_Networking::server_poll();
  return true;
}

bool NetworkingController::connect() { return false; }

bool NetworkingController::send_udp(const void *data, size_t s) {
  VentOS_Networking::server_send_udp(data, s);
  return true;
}

} // namespace VentOS