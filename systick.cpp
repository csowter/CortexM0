#include "systick.h"

namespace
{
    namespace Registers
    {
        namespace CSR
        {
            namespace Position
            {
                constexpr uint32_t COUNTFLAG = 16UL;
                constexpr uint32_t CLKSOURCE = 2UL;
                constexpr uint32_t TICKINT   = 1UL;
                constexpr uint32_t ENABLE    = 0UL;
            }

            namespace Mask
            {
                [[maybe_unused]] constexpr uint32_t COUNTFLAG = (1UL << Position::COUNTFLAG);
                [[maybe_unused]] constexpr uint32_t CLKSOURCE = (1UL << Position::CLKSOURCE);
                [[maybe_unused]] constexpr uint32_t TICKINT   = (1UL << Position::TICKINT);
                [[maybe_unused]] constexpr uint32_t ENABLE    = (1UL << Position::ENABLE);
            }
        }

        namespace RELOAD
        {
            namespace Position
            {
                constexpr uint32_t RELOAD = 0UL;
            }

            namespace Mask
            {
                [[maybe_unused]] constexpr uint32_t RELOAD = (0x00FFFFFFUL << Position::RELOAD);
            }
        }

        namespace CVR
        {
            namespace Position
            {
                constexpr uint32_t CURRENT = 0UL;
            }

            namespace Mask
            {
                [[maybe_unused]] constexpr uint32_t CURRENT = (0x00FFFFFFUL << Position::CURRENT);
            }
        }

        namespace CALIB
        {
            namespace Position
            {
                constexpr uint32_t NOREF = 31UL;
                constexpr uint32_t SKEW  = 30UL;
                constexpr uint32_t TENMS = 0UL;
            }

            namespace Mask
            {
                [[maybe_unused]] constexpr uint32_t NOREF = (1UL << Position::NOREF);
                [[maybe_unused]] constexpr uint32_t SKEW  = (1UL << Position::SKEW);
                [[maybe_unused]] constexpr uint32_t TENMS = (0x00FFFFFFUL << Position::TENMS);
            }
        }
    }

    struct __attribute__((packed, aligned(1))) SysTickRegisters
    {
        volatile uint32_t CSR;
        volatile uint32_t RVR;
        volatile uint32_t CVR;
        volatile uint32_t CALIB;
    };

    static_assert(0x00 == offsetof(SysTickRegisters, CSR));
    static_assert(0x04 == offsetof(SysTickRegisters, RVR));
    static_assert(0x08 == offsetof(SysTickRegisters, CVR));
    static_assert(0x0C == offsetof(SysTickRegisters, CALIB));

    SysTickRegisters *SysTickInstance{reinterpret_cast<SysTickRegisters *>(0xE000E010UL)};

    std::function<void()> SysTickCallback{[](){}};
}

namespace CortexM0
{
SysTick_c::SysTick_c(ClockSource clockSource, uint32_t reloadValue)
{
    uint32_t csr = SysTickInstance->CSR;
    if(ClockSource::ReferenceClock == clockSource)
    {
        csr &= ~Registers::CSR::Mask::CLKSOURCE;
    }
    else
    {
        csr |= Registers::CSR::Mask::CLKSOURCE;
    }
    SysTickInstance->CSR = csr;

    SysTickInstance->RVR = (reloadValue & Registers::RELOAD::Mask::RELOAD);
}

void SysTick_c::UpdateReloadValue(uint32_t reloadValue)
{
    SysTickInstance->RVR = (reloadValue & Registers::RELOAD::Mask::RELOAD);
}

void SysTick_c::Enable()
{
    uint32_t csr = SysTickInstance->CSR;
    csr |= Registers::CSR::Mask::ENABLE;
    SysTickInstance->CSR = csr;
}

void SysTick_c::Disable()
{
    uint32_t csr = SysTickInstance->CSR;
    csr &= ~Registers::CSR::Mask::ENABLE;
    SysTickInstance->CSR = csr;
}

bool SysTick_c::HasElapsed() const
{
    const uint32_t csr = SysTickInstance->CSR;
    return (Registers::CSR::Mask::COUNTFLAG == (csr & Registers::CSR::Mask::COUNTFLAG)); 
}

void SysTick_c::EnableIRQ(std::function<void()> callback)
{
    SysTickCallback = callback;
    uint32_t csr = SysTickInstance->CSR;
    csr |= Registers::CSR::Mask::TICKINT;
    SysTickInstance->CSR = csr;
}

void SysTick_c::DisableIRQ()
{
    uint32_t csr = SysTickInstance->CSR;
    csr &= ~Registers::CSR::Mask::TICKINT;
    SysTickInstance->CSR = csr;    
    SysTickCallback = [](){};
}
}

extern "C" void SysTick_Handler(void)
{
    SysTickCallback();
}
