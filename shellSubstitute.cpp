#include <fmt/core.h>
#include <range/v3/all.hpp>

#include <string_view>
#include <unordered_map>

const auto shellSubstitute =
    [](std::string_view input,
       const std::unordered_map<std::string, std::string> variables)
    -> std::string {
  using namespace std::literals;
  using namespace ranges;

  auto &&output =
      input | views::split('$') |
      views::transform([&variables](range auto &&rng) {
        auto &&splitRng = rng | views::split('}');

        auto &&word = *begin(splitRng);
        auto &&wordSv = std::string_view{
            &*word.begin(), static_cast<std::size_t>(distance(word))};
        auto &&trimmedWord = (wordSv[0] == '{') ? wordSv.substr(1) : wordSv;

        auto &&rest = splitRng | views::tail | views::join | views::common;

        auto substitute =
            [&variables](std::string_view sv) -> std::string_view {
          if (auto it = variables.find(std::string{sv});
              it != variables.cend()) {
            return it->second;
          } else {
            return ""sv;
          }
        };
        return views::concat(substitute(trimmedWord), rest);
      }) |
      views::join;

  return output | to<std::string>;
};

auto main() -> int {
  using namespace std::literals;

  auto &&input = "${ABC}/hello/${HOME}/dir/${USER}/file"sv;
  auto &&variables = std::unordered_map<std::string, std::string>{
      {"HOME", "root"}, {"USER", "vlad"}};
  auto &&output = shellSubstitute(input, variables);

  fmt::print("input: {}\n", input);
  fmt::print("output: {}\n", output);

  return 0;
}