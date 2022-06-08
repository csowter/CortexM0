#ifndef nvic_c
#define nvic_c

#include <cstdint>

namespace CortexM0
{
    class NVIC_c
    {
    public:
        enum class InterruptPriority
        {
            Low,
            LowMedium,
            HighMedium,
            High
        };

        NVIC_c() = delete; 
        static void EnableInterrupt(uint8_t irqn);
        static void DisableInterrupt(uint8_t irqn);
        static void SetInterruptPending(uint8_t irqn);
        static void ClearInterruptPending(uint8_t irqn);
        static void SetInterruptPriority(uint8_t irqn, InterruptPriority priority);
        
        static InterruptPriority GetInterruptPriority(uint8_t irqn);
        static bool IsInterruptPending(uint8_t irqn);
    };
}

#endif
