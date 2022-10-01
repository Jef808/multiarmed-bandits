#ifndef HELPERS_H_
#define HELPERS_H_

#include <chrono>
#include <thread>

enum class timescale { s, ms };

template <timescale ts = timescale::ms> static void sleep_sync(unsigned int t) {
  const auto duration = [&t]() {
    if constexpr (ts == timescale::s)
      return std::chrono::seconds(t);
    else if constexpr (ts == timescale::ms)
      return std::chrono::milliseconds(t);
  }();
  std::this_thread::sleep_for(duration);
}

#endif // HELPERS_H_
