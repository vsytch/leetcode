#include <fmt/core.h>

#include <chrono>
#include <coroutine>
#include <exception>
#include <future>
#include <thread>
#include <type_traits>
#include <vector>

// Enable the use of std::future<T> as a coroutine type
// by using a std::promise<T> as the promise type.
template <typename T, typename... Args>
requires(!std::is_void_v<T> &&
         !std::is_reference_v<T>) struct std::coroutine_traits<std::future<T>,
                                                               Args...> {
  struct promise_type : std::promise<T> {
    auto get_return_object() noexcept -> std::future<T> {
      fmt::print("get_return_object()\n");
      return this->get_future();
    }

    [[nodiscard]] auto initial_suspend() const noexcept -> std::suspend_never {
      fmt::print("initial_suspend()\n");
      return {};
    }

    [[nodiscard]] auto final_suspend() const noexcept -> std::suspend_never {
      fmt::print("final_suspend()\n");
      return {};
    }

    auto return_value(const T &value) noexcept(
        std::is_nothrow_copy_constructible_v<T>) -> void {
      fmt::print("return_value()\n");
      this->set_value(value);
    }

    auto
    return_value(T &&value) noexcept(std::is_nothrow_move_constructible_v<T>)
        -> void {
      fmt::print("return_value()\n");
      this->set_value(std::move(value));
    }

    auto unhandled_exception() noexcept -> void {
      fmt::print("unhandled_exception()\n");
      this->set_exception(std::current_exception());
    }
  };
};

// Allow co_await'ing std::future<T> and std::future<void>
// by naively spawning a new thread for each co_await.
template <typename T>
requires(!std::is_reference_v<T>) [[nodiscard]] auto
operator co_await(std::future<T> future) noexcept {
  struct awaiter : std::future<T> {
    [[nodiscard]] auto await_ready() const noexcept -> bool {
      fmt::print("await_ready()\n");
      using namespace std::chrono_literals;
      return this->wait_for(0s) != std::future_status::timeout;
    }

    auto await_suspend(std::coroutine_handle<> cont) const -> void {
      fmt::print("await_suspend()\n");
      std::thread([this, cont] {
        this->wait();
        cont();
      }).detach();
    }

    [[nodiscard]] auto await_resume() -> T {
      fmt::print("await_resume()\n");
      return this->get();
    }
  };

  fmt::print("operator co_await()\n");
  return awaiter{std::move(future)};
}

auto compute(int id) -> std::future<int> {
  fmt::print("compute({})\n", id);
  int a = co_await std::async([] {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    return 6;
  });
  int b = co_await std::async([] {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    return 7;
  });
  co_return a *b;
}

auto main() -> int {
  auto v = std::vector<std::future<int>>{};
  for (int i = 0; i < 10; i++) {
    v.push_back(compute(i));
  }

  for (auto &&fut : v) {
    fmt::print("{}\n", fut.get());
  }

  return 0;
}
