#ifndef systick_h
#define systick_h

#include <cstdint>
#include <functional>

namespace CortexM0
{
class SysTick_c
{
public:
    enum class ClockSource
    {
        ReferenceClock,
        ProcessorClock
    };

    SysTick_c(ClockSource clockSource, uint32_t reloadValue);
    void UpdateReloadValue(uint32_t reloadValue);
    void Enable();
    void Disable();
    bool HasElapsed() const;
    void EnableIRQ(std::function<void()> callback);
    void DisableIRQ();
};
}
#endif
