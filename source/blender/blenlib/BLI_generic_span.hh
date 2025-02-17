/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/** \file
 * \ingroup bli
 */

#include "BLI_cpp_type.hh"
#include "BLI_span.hh"

namespace blender {

/**
 * A generic span. It behaves just like a blender::Span<T>, but the type is only known at run-time.
 */
class GSpan {
 protected:
  const CPPType *type_;
  const void *data_;
  int64_t size_;

 public:
  GSpan(const CPPType &type, const void *buffer, int64_t size)
      : type_(&type), data_(buffer), size_(size)
  {
    BLI_assert(size >= 0);
    BLI_assert(buffer != nullptr || size == 0);
    BLI_assert(type.pointer_has_valid_alignment(buffer));
  }

  GSpan(const CPPType &type) : GSpan(type, nullptr, 0)
  {
  }

  template<typename T>
  GSpan(Span<T> array)
      : GSpan(CPPType::get<T>(), static_cast<const void *>(array.data()), array.size())
  {
  }

  const CPPType &type() const
  {
    return *type_;
  }

  bool is_empty() const
  {
    return size_ == 0;
  }

  int64_t size() const
  {
    return size_;
  }

  const void *data() const
  {
    return data_;
  }

  const void *operator[](int64_t index) const
  {
    BLI_assert(index < size_);
    return POINTER_OFFSET(data_, type_->size() * index);
  }

  template<typename T> Span<T> typed() const
  {
    BLI_assert(type_->is<T>());
    return Span<T>(static_cast<const T *>(data_), size_);
  }

  GSpan slice(const int64_t start, int64_t size) const
  {
    BLI_assert(start >= 0);
    BLI_assert(size >= 0);
    const int64_t new_size = std::max<int64_t>(0, std::min(size, size_ - start));
    return GSpan(*type_, POINTER_OFFSET(data_, type_->size() * start), new_size);
  }

  GSpan slice(const IndexRange range) const
  {
    return this->slice(range.start(), range.size());
  }
};

/**
 * A generic mutable span. It behaves just like a blender::MutableSpan<T>, but the type is only
 * known at run-time.
 */
class GMutableSpan {
 protected:
  const CPPType *type_;
  void *data_;
  int64_t size_;

 public:
  GMutableSpan(const CPPType &type, void *buffer, int64_t size)
      : type_(&type), data_(buffer), size_(size)
  {
    BLI_assert(size >= 0);
    BLI_assert(buffer != nullptr || size == 0);
    BLI_assert(type.pointer_has_valid_alignment(buffer));
  }

  GMutableSpan(const CPPType &type) : GMutableSpan(type, nullptr, 0)
  {
  }

  template<typename T>
  GMutableSpan(MutableSpan<T> array)
      : GMutableSpan(CPPType::get<T>(), static_cast<void *>(array.begin()), array.size())
  {
  }

  operator GSpan() const
  {
    return GSpan(*type_, data_, size_);
  }

  const CPPType &type() const
  {
    return *type_;
  }

  bool is_empty() const
  {
    return size_ == 0;
  }

  int64_t size() const
  {
    return size_;
  }

  void *data() const
  {
    return data_;
  }

  void *operator[](int64_t index) const
  {
    BLI_assert(index >= 0);
    BLI_assert(index < size_);
    return POINTER_OFFSET(data_, type_->size() * index);
  }

  template<typename T> MutableSpan<T> typed() const
  {
    BLI_assert(type_->is<T>());
    return MutableSpan<T>(static_cast<T *>(data_), size_);
  }

  GMutableSpan slice(const int64_t start, int64_t size) const
  {
    BLI_assert(start >= 0);
    BLI_assert(size >= 0);
    const int64_t new_size = std::max<int64_t>(0, std::min(size, size_ - start));
    return GMutableSpan(*type_, POINTER_OFFSET(data_, type_->size() * start), new_size);
  }

  GMutableSpan slice(IndexRange range) const
  {
    return this->slice(range.start(), range.size());
  }
};

}  // namespace blender
