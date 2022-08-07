#include <machine.h>
// there is not yet anything for this to do
#include <core.h>

void outputReport(MachineStatusReport msr) {
        OxCore::Debug<const char *>("Post Heater C: ");
        OxCore::DebugLn<float>(msr.post_heater_C);
        OxCore::Debug<const char *>("Post Stack  C: ");
        OxCore::DebugLn<float>(msr.post_stack_C);
        OxCore::Debug<const char *>("Heater      V: ");
        OxCore::DebugLn<float>(msr.heater_voltage);
        OxCore::Debug<const char *>("Stack       V: ");
        OxCore::DebugLn<float>(msr.stack_voltage);
        OxCore::Debug<const char *>("Fan Speed    : ");
        OxCore::DebugLn<float>(msr.fan_speed);
}
