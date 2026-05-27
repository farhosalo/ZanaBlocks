#pragma once

#include "DateTime.h"

auto constexpr RESET{"\033[0m"};
auto constexpr WHITE{"\033[37m"};
auto constexpr CYAN{"\033[36m"};
auto constexpr YELLOW{"\033[33m"};
auto constexpr RED{"\033[31m"};
auto constexpr LIGHT_RED{"\033[91m"};
auto constexpr PURPLE{"\033[35m"};

// NOLINTBEGIN (cppcoreguidelines-macro-usage)
#define LOG(color, level, msg)                                            \
  std::cout << color << "[" << ZanaBlocks::Utilities::timestamp() << "] " \
            << "[" << level << "] "                                       \
            << "[" << __FILE__ << ":" << __LINE__ << "] " << msg << RESET \
            << "\n"

#define INFO(msg) LOG(WHITE, "INFO ", msg)
#define DEBUG(msg) LOG(CYAN, "DEBUG", msg)
#define WARN(msg) LOG(YELLOW, "WARN ", msg)
#define ERROR(msg) LOG(PURPLE, "ERROR", msg)
#define FATAL(msg)           \
  do {                       \
    LOG(RED, "FATAL", msg);  \
    std::exit(EXIT_FAILURE); \
  } while (0)
// NOLINTEND