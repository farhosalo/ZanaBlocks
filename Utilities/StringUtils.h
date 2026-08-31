#pragma once

#include <string>

namespace ZanaBlocks::Utilities {

/**
 * @brief Escapes special characters in a string for safe use within double
 * quotes.
 *
 * This function processes the input string and escapes any occurrence of double
 * quotes (") or backslashes (\) by prefixing them with a backslash. This is
 * commonly used when embedding strings within C-style string literals or JSON
 * data structures.
 *
 * @param str The input string to escape. Can be empty or contain any
 * characters.
 * @return A new string with special characters properly escaped. Empty if the
 * input was empty, otherwise contains all original characters plus escape
 * sequences.
 *
 * @note This function is noexcept - it will never throw exceptions.
 * @note Thread-safe: no shared mutable state, safe to call from multiple
 * threads.
 */
inline std::string escapeString(const std::string& str) noexcept {
  std::string escaped;
  escaped.reserve(str.size() * 2);

  for (const char character : str) {
    if (character == '"' || character == '\\') {
      escaped += '\\';
    }
    escaped += character;
  }

  return escaped;
}

/**
 * @brief Safely converts an integer to a string representation.
 *
 * This function converts any signed integer value to its string form using
 * std::to_string, with exception handling for edge cases where conversion
 * might fail (out_of_range exceptions).
 *
 * @param num The integer value to convert to a string.
 * @return A string representation of the number if successful, empty string if
 * conversion would fail due to out-of-range values.
 *
 * @note Returns an empty string when std::to_string throws std::out_of_range.
 * @note Thread-safe: no shared mutable state, safe to call from multiple
 * threads.
 */
inline std::string safeToString(const int num) noexcept {
  try {
    return std::to_string(num);
  } catch (const std::out_of_range&) {
    // Handle out of range error by returning an empty string as a safe fallback
    return "";
  }
}

}  // namespace ZanaBlocks::Utilities
