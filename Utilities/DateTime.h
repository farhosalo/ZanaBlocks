#pragma once

#include <array>
#include <ctime>
#include <iostream>
#include <string>

namespace ZanaBlocks::Utilities {
inline std::string timestamp() noexcept {
  const auto time = std::time(nullptr);
  constexpr auto BUF_SIZE{20};
  std::array<char, BUF_SIZE> buf{};
  const auto* tm = std::localtime(&time);
  if (tm == nullptr) {
    return {};
  }

  if (std::strftime(buf.data(), buf.size(), "%Y-%m-%d %H:%M:%S", tm) == 0) {
    return {};
  }

  return std::string{buf.data()};  // noexcept via SSO — fits in 19 chars
}
}  // namespace ZanaBlocks::Utilities