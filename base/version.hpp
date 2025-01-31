#pragma once

#include "base/macros.hpp"  // 🧙 For OS_WIN.

namespace principia {
namespace base {
namespace _version {
namespace internal {

extern char const BuildDate[];
extern char const Version[];

#if OS_WIN
#pragma message("Compiler version: " STRINGIFY_EXPANSION(_MSC_FULL_VER))
#endif

}  // namespace internal

using internal::BuildDate;
using internal::Version;

}  // namespace _version
}  // namespace base
}  // namespace principia
