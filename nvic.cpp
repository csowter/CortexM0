#include "nvic.h"
#include <cstddef>

namespace
{
    constexpr uint32_t NVICBaseAddress = 0xE000E100UL;

    struct NVICRegisters
    {
        volatile uint32_t ISER;
        uint32_t unused[31];
        volatile uint32_t ICER;
        uint32_t unused1[31];
        volatile uint32_t ISPR;
        volatile uint32_t unused2[31];
        volatile uint32_t ICPR;
        volatile uint32_t unused3[95];
        volatile uint32_t IPR[8];
    };

    static_assert(0x00 == offsetof(NVICRegisters, ISER));
    static_assert(0x80 == offsetof(NVICRegisters, ICER));
    static_assert(0x100 == offsetof(NVICRegisters, ISPR));
    static_assert(0x180 == offsetof(NVICRegisters, ICPR));
    static_assert(0x300 == offsetof(NVICRegisters, IPR[0]));
    static_assert(0x31C == offsetof(NVICRegisters, IPR[7]));

    NVICRegisters *NVICInstance = reinterpret_cast<NVICRegisters *>(NVICBaseAddress);

    constexpr uint32_t NumberOfPriorityLevels = 4UL;
    constexpr uint32_t InterruptPriorityValues[] = 
    {
        192UL, //lowest
        128UL, //low medium
        64UL,  //high medium
        0UL    //highest
    };

    constexpr uint32_t PriorityMask = 0xFFUL;
}

namespace CortexM0
{
    void NVIC_c::EnableInterrupt(uint8_t irqn)
    {
        NVICInstance->ISER = (1UL << irqn);
    }

    void NVIC_c::DisableInterrupt(uint8_t irqn)
    {
        NVICInstance->ICER = (1UL << irqn);
    }

    void NVIC_c::SetInterruptPending(uint8_t irqn)
    {
        NVICInstance->ISPR = (1UL << irqn);
    }

    void NVIC_c::ClearInterruptPending(uint8_t irqn)
    {
        NVICInstance->ICPR = (1UL << irqn);
    }

    void NVIC_c::SetInterruptPriority(uint8_t irqn, InterruptPriority priority)
    {
        const uint32_t registerNumber = irqn / 4UL;
        const uint32_t bitPostion = (irqn % 4UL) * 8UL;
        uint32_t ipr = NVICInstance->IPR[registerNumber];
        ipr &= ~(PriorityMask << bitPostion);
        ipr |= (InterruptPriorityValues[static_cast<std::size_t>(priority)] << bitPostion);
        NVICInstance->IPR[registerNumber] = ipr;
    }

    NVIC_c::InterruptPriority NVIC_c::GetInterruptPriority(uint8_t irqn)
    {
        const uint32_t registerNumber = irqn / 4UL;
        const uint32_t bitPostion = (irqn % 4UL) * 8UL;
        uint32_t ipr = NVICInstance->IPR[registerNumber];
        ipr >>= bitPostion;
        ipr &= PriorityMask;
        
        for(uint32_t i = 0; i < NumberOfPriorityLevels; i++)
        {
            if(InterruptPriorityValues[i] == ipr)
            {
                return static_cast<NVIC_c::InterruptPriority>(i);
            }
        }
        
        return InterruptPriority::Low;
    }

    bool NVIC_c::IsInterruptPending(uint8_t irqn)
    {
        return ((1UL << irqn) == (NVICInstance->ISPR & (1UL << irqn)));
    }
}