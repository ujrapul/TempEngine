// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array.hpp"
#include "String.hpp"
#include "MemoryManager.hpp"
#include <string>
#include <memory>
#include <filesystem>
#include <functional>
#include <deque>
#include <bits/stl_deque.h>

extern template class std::allocator<char const*>;

extern template class std::allocator<char>;
extern template class std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >;

extern template struct std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Alloc_hider;

extern template class std::basic_string_view<char, std::char_traits<char> >;

extern template class std::allocator<float>;

extern template class std::unique_ptr<std::filesystem::__cxx11::path::_List::_Impl, std::filesystem::__cxx11::path::_List::_Impl_deleter>;

extern template class std::allocator<unsigned int>;

extern template class std::function<void ()>;

extern template class std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >;

extern template void std::__detail::__to_chars_10_impl<unsigned long>(char*, unsigned int, unsigned long);
extern template std::_Hashtable<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, int>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, int> >, std::__detail::_Select1st, std::equal_to<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::hash<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<true, false, true> >::_Hashtable::~_Hashtable();
extern template std::_Hashtable<unsigned short, std::pair<unsigned short const, int>, std::allocator<std::pair<unsigned short const, int> >, std::__detail::_Select1st, std::equal_to<unsigned short>, std::hash<unsigned short>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_Hashtable::~_Hashtable();
extern template void std::_Hashtable<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, int>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, int> >, std::__detail::_Select1st, std::equal_to<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::hash<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<true, false, true> >::clear();

extern template std::_Deque_base<std::function<void ()>, std::allocator<std::function<void ()> > >::_Deque_base::~_Deque_base();
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_range_initialize<std::function<void ()> const*>(std::function<void ()> const*, std::function<void ()> const*, std::forward_iterator_tag);
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_range_insert_aux<std::_Deque_iterator<std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*> >(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*>, std::forward_iterator_tag);
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_push_front_aux<std::function<void ()> const&>(std::function<void ()> const&);
extern template class std::function<void ()>& std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::emplace_front<std::function<void ()> >(std::function<void ()>&&);
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_push_back_aux<std::function<void ()> const&>(std::function<void ()> const&);
extern template class std::function<void ()>& std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::emplace_back<std::function<void ()> >(std::function<void ()>&&);
extern template class std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_insert_aux<std::function<void ()> const&>(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::function<void ()> const&);
extern template class std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::emplace<std::function<void ()> >(std::_Deque_iterator<std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*>, std::function<void ()>&&);
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_range_insert_aux<std::function<void ()> const*>(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::function<void ()> const*, std::function<void ()> const*, std::forward_iterator_tag);
extern template void std::_Hashtable<unsigned short, std::pair<unsigned short const, int>, std::allocator<std::pair<unsigned short const, int> >, std::__detail::_Select1st, std::equal_to<unsigned short>, std::hash<unsigned short>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::clear();
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_range_insert_aux<std::move_iterator<std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> > >(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::move_iterator<std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> >, std::move_iterator<std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> >, std::forward_iterator_tag);
extern template class std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> std::__copy_move_a1<false, std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*, std::function<void ()> >(std::_Deque_iterator<std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()> const&, std::function<void ()> const*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>);
extern template void std::__fill_a1<std::function<void ()>, std::function<void ()> >(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> const&, std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> const&, std::function<void ()> const&);
extern template class std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> std::__copy_move_a1<true, std::function<void ()>, std::function<void ()>&, std::function<void ()>*, std::function<void ()> >(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>);
extern template class std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> std::__copy_move_backward_a1<true, std::function<void ()>, std::function<void ()>&, std::function<void ()>*, std::function<void ()> >(std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>, std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*>);
extern template void std::deque<std::function<void ()>, std::allocator<std::function<void ()> > >::_M_range_initialize<std::move_iterator<std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> > >(std::move_iterator<std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> >, std::move_iterator<std::_Deque_iterator<std::function<void ()>, std::function<void ()>&, std::function<void ()>*> >, std::forward_iterator_tag);
extern template std::_Hashtable<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, void*>, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, void*> >, std::__detail::_Select1st, std::equal_to<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::hash<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<true, false, true> >::_Hashtable::~_Hashtable();
