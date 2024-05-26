// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Math_fwd.hpp" // IWYU pragma: keep
#include "MemoryManager.hpp" // IWYU pragma: keep
#include "MemoryUtils.hpp" // IWYU pragma: keep
#include "Array_fwd.hpp" // IWYU pragma: keep
#include "String.hpp" // IWYU pragma: keep
#include "HashMap.hpp" // IWYU pragma: keep
#include "Queue.hpp" // IWYU pragma: keep
#include "FileWriter.hpp" // IWYU pragma: keep
#include "FileSystem.hpp" // IWYU pragma: keep
#include "TGA.hpp" // IWYU pragma: keep
#include "Logger.hpp" // IWYU pragma: keep
#include "EngineUtils.hpp" // IWYU pragma: keep
#include "OpenGLWrapper.hpp" // IWYU pragma: keep

extern template class std::allocator<char const*>;

extern template class std::allocator<char>;
extern template class std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >;

extern template struct std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Alloc_hider;

extern template class std::basic_string_view<char, std::char_traits<char> >;

extern template class std::allocator<float>;

extern template class std::allocator<unsigned int>;

extern template void std::__detail::__to_chars_10_impl<unsigned long>(char*, unsigned int, unsigned long);
