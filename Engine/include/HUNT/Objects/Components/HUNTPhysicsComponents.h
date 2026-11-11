
#pragma once

#include <functional>
#include "HUNT/Objects/Components/HUNTBaseComponent.h"

namespace HUNT {
    class HUNTObject;

    struct EnterResponseComponent : public HUNTBaseComponent
    {
        explicit EnterResponseComponent(
            const std::function<void(HUNTObject& other)>& callback)
            : m_callback{ callback }
        {
        }

        std::function<void(HUNTObject& other)> m_callback;
    };


    struct OverlapResponseComponent : public HUNTBaseComponent
    {
        explicit OverlapResponseComponent(
            const std::function<void(HUNTObject& other)>& callback)
            : m_callback{ callback }
        {
        }

        std::function<void(HUNTObject& other)> m_callback;
    };

    struct ExitResponseComponent : public HUNTBaseComponent
    {
        explicit ExitResponseComponent(
            const std::function<void(HUNTObject& other)>& callback)
            : m_callback{ callback }
        {
        }

        std::function<void(HUNTObject& other)> m_callback;
    };
}
