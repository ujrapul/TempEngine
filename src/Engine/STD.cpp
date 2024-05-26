// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "STD.hpp" // IWYU pragma: keep

template class std::allocator<char const*>;

template class std::allocator<char>;
template class std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >;

template class std::basic_string_view<char, std::char_traits<char> >;

template class std::allocator<float>;

template class std::allocator<unsigned int>;

template void std::__detail::__to_chars_10_impl<unsigned long>(char*, unsigned int, unsigned long);
