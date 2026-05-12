#pragma once

#include <array>
#include <ctime>
#include <iostream>
#include <string>

namespace ZanaBlocks::Utilities {
inline std::string timestamp() {
  const auto time = std::time(nullptr);
  auto constexpr BUF_SIZE{20};
  std::array<char, BUF_SIZE> buf{};
  std::strftime(buf.data(), buf.size(), "%Y-%m-%d %H:%M:%S",
                std::localtime(&time));

  return std::string{buf.data()};
}
}  // namespace ZanaBlocks::Utilities