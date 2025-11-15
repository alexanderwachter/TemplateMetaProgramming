/*
 * Copyright (c) 2025 Alexander Wachter
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "Typelist.hpp"
#include "Registers.hpp"

#include <type_traits>

namespace tmp
{
struct readonly {};
struct writeonly {};

template<>
struct is_attribute<readonly> : std::true_type {};

template<>
struct is_attribute<writeonly> : std::true_type {};

template<concepts::attribute... ATTRIBUTEs>
using attributes = typelist<ATTRIBUTEs...>;

template<typename T>
using is_readonly = has_attribute<readonly, T>;

template<typename T>
inline constexpr bool is_readonly_v = is_readonly<T>::value;

template<typename T>
using is_writeonly = has_attribute<writeonly, T>;

template<typename T>
inline constexpr bool is_writeonly_v = is_writeonly<T>::value;

} // namespace tmp
