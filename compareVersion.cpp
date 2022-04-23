#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <compare>
#include <cstdint>
#include <string_view>

const auto compareVersion = [](const std::string_view sv1,
                               const std::string_view sv2) -> std::int32_t {
  using namespace std::literals;
  using namespace ranges;

  auto splitAndStrip = [](auto &&sv) {
    return sv | views::split('.') | views::transform([](auto &&r) {
             return r | views::drop_while([](auto &&c) { return c == '0'; });
           });
  };

  auto r = views::zip(splitAndStrip(sv1), splitAndStrip(sv2)) |
           views::drop_while([](auto &&x) {
             auto &&[lhs, rhs] = x;
             return equal(lhs, rhs);
           });

  auto &&[lhs, rhs] = *begin(r);
  auto cmp = (lhs | to<std::string>) <=> (rhs | to<std::string>);
  return (cmp > 0) ? 1 : (cmp == 0) ? 0 : (cmp < 0) ? -1 : -1;
};

auto main() -> int {
  using namespace std::literals;
  using namespace ranges;

  constexpr auto sv1 = "0123.00456.000789"sv;
  constexpr auto sv2 = "0111.00222.000333"sv;

  fmt::print("compareVersion(sv1, sv1): {}\n", compareVersion(sv1, sv1));
  fmt::print("compareVersion(sv1, sv2): {}\n", compareVersion(sv1, sv2));
  fmt::print("compareVersion(sv2, sv1): {}\n", compareVersion(sv2, sv1));
  fmt::print("compareVersion(sv2, sv2): {}\n", compareVersion(sv2, sv2));

  return 0;
}