#pragma once

namespace KidTech::Utilities {

// Base64 encoding utility for file transfer. This is a simple implementation

inline constexpr char kB64Chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * Encodes data in Base64 format.
 * @param data Pointer to the input data.
 * @param len Length of the input data.
 * @return The Base64-encoded string.
 * Note: This function does not add newlines and does not use padding characters
 * for simplicity.
 */
inline std::string base64Encode(const uint8_t* data, std::size_t len) {
  std::string out;
  out.reserve(((len + 2) / 3) * 4);

  for (std::size_t i = 0; i < len; i += 3) {
    uint32_t b = static_cast<uint32_t>(data[i]) << 16;
    if (i + 1 < len) b |= static_cast<uint32_t>(data[i + 1]) << 8;
    if (i + 2 < len) b |= static_cast<uint32_t>(data[i + 2]);

    out.push_back(kB64Chars[(b >> 18) & 0x3F]);
    out.push_back(kB64Chars[(b >> 12) & 0x3F]);
    out.push_back(i + 1 < len ? kB64Chars[(b >> 6) & 0x3F] : '=');
    out.push_back(i + 2 < len ? kB64Chars[(b >> 0) & 0x3F] : '=');
  }
  return out;
}
}  // namespace KidTech::Utilities