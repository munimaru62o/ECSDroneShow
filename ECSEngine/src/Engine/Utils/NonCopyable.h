// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

/**
 * @class NonCopyable
 * @brief A utility base class to strictly disable copy and move semantics.
 *
 * Inherit from this class using the 'private' access modifier to easily apply
 * non-copyable restrictions to managers or core systems without writing
 * boilerplate '= delete' code in every class.
 *
 * Example:
 * class SystemManager final : private NonCopyable { ... };
 */
class NonCopyable
{
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};
