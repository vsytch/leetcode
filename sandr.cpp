#include <fmt/core.h>

#include <unifex/delay.hpp>
#include <unifex/for_each.hpp>
#include <unifex/range_stream.hpp>
#include <unifex/single.hpp>
#include <unifex/stop_immediately.hpp>
#include <unifex/take_until.hpp>
#include <unifex/thread_unsafe_event_loop.hpp>
#include <unifex/typed_via_stream.hpp>

#include <chrono>

auto main() -> int {
  using namespace std::chrono_literals;

  auto &&eventLoop = unifex::thread_unsafe_event_loop{};

  fmt::print("starting\n");
  auto &&start = std::chrono::steady_clock::now();

  [[maybe_unused]] std::optional<unifex::unit> result = eventLoop.sync_wait(
      unifex::range_stream{0, 100} |
      unifex::delay(eventLoop.get_scheduler(), 50ms) |
      unifex::stop_immediately<int>() |
      unifex::take_until(
          unifex::schedule_after(eventLoop.get_scheduler(), 500ms) |
          unifex::single()) |
      unifex::for_each([start](int value) {
        auto ms = duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        fmt::print("[{} ms] {}\n", static_cast<int>(ms.count()), value);
      }));

  return 0;
}
