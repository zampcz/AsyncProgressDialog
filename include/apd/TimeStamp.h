#pragma once

#include <chrono>

#include <QMetaType>

namespace APD
{
    using TimeStamp = std::chrono::time_point<std::chrono::steady_clock>;
}
