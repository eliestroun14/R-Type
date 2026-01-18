#pragma once

namespace spdlog {

template <typename... Args>
inline void debug(const char*, Args&&...) {}

template <typename... Args>
inline void error(const char*, Args&&...) {}

} // namespace spdlog
