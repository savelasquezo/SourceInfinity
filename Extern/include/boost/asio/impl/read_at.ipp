//
// read_at.ipp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_READ_AT_IPP
#define BOOST_ASIO_READ_AT_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/detail/push_options.hpp>
#include <algorithm>
#include <boost/asio/detail/pop_options.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/detail/bind_handler.hpp>
#include <boost/asio/detail/consuming_buffers.hpp>
#include <boost/asio/detail/handler_alloc_helpers.hpp>
#include <boost/asio/detail/handler_invoke_helpers.hpp>
#include <boost/asio/detail/throw_error.hpp>

namespace boost {
namespace asio {

template <typename SyncRandomAccessReadDevice, typename MutableBufferSequence,
    typename CompletionCondition>
std::size_t read_at(SyncRandomAccessReadDevice& d,
    boost::uint64_t offset, const MutableBufferSequence& buffers,
    CompletionCondition completion_condition, boost::system::error_code& ec)
{
  ec = boost::system::error_code();
  boost::asio::detail::consuming_buffers<
    mutable_buffer, MutableBufferSequence> tmp(buffers);
  std::size_t total_transferred = 0;
  tmp.prepare(detail::adapt_completion_condition_result(
        completion_condition(ec, total_transferred)));
  while (tmp.begin() != tmp.end())
  {
    std::size_t bytes_transferred = d.read_some_at(
        offset + total_transferred, tmp, ec);
    tmp.consume(bytes_transferred);
    total_transferred += bytes_transferred;
    tmp.prepare(detail::adapt_completion_condition_result(
          completion_condition(ec, total_transferred)));
  }
  return total_transferred;
}

template <typename SyncRandomAccessReadDevice, typename MutableBufferSequence>
inline std::size_t read_at(SyncRandomAccessReadDevice& d,
    boost::uint64_t offset, const MutableBufferSequence& buffers)
{
  boost::system::error_code ec;
  std::size_t bytes_transferred = read_at(
      d, offset, buffers, transfer_all(), ec);
  boost::asio::detail::throw_error(ec);
  return bytes_transferred;
}

template <typename SyncRandomAccessReadDevice, typename MutableBufferSequence,
    typename CompletionCondition>
inline std::size_t read_at(SyncRandomAccessReadDevice& d,
    boost::uint64_t offset, const MutableBufferSequence& buffers,
    CompletionCondition completion_condition)
{
  boost::system::error_code ec;
  std::size_t bytes_transferred = read_at(
      d, offset, buffers, completion_condition, ec);
  boost::asio::detail::throw_error(ec);
  return bytes_transferred;
}

#if !defined(BOOST_NO_IOSTREAM)

template <typename SyncRandomAccessReadDevice, typename Allocator,
    typename CompletionCondition>
std::size_t read_at(SyncRandomAccessReadDevice& d,
    boost::uint64_t offset, boost::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition, boost::system::error_code& ec)
{
  std::size_t total_transferred = 0;
  for (;;)
  {
    std::size_t bytes_available =
      std::min<std::size_t>(512, b.max_size() - b.size());
    std::size_t bytes_transferred = d.read_some_at(
        offset + total_transferred, b.prepare(bytes_available), ec);
    b.commit(bytes_transferred);
    total_transferred += bytes_transferred;
    if (b.size() == b.max_size()
        || completion_condition(ec, total_transferred))
      return total_transferred;
  }
}

template <typename SyncRandomAccessReadDevice, typename Allocator>
inline std::size_t read_at(SyncRandomAccessReadDevice& d,
    boost::uint64_t offset, boost::asio::basic_streambuf<Allocator>& b)
{
  boost::system::error_code ec;
  std::size_t bytes_transferred = read_at(
      d, offset, b, transfer_all(), ec);
  boost::asio::detail::throw_error(ec);
  return bytes_transferred;
}

template <typename SyncRandomAccessReadDevice, typename Allocator,
    typename CompletionCondition>
inline std::size_t read_at(SyncRandomAccessReadDevice& d,
    boost::uint64_t offset, boost::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition)
{
  boost::system::error_code ec;
  std::size_t bytes_transferred = read_at(
      d, offset, b, completion_condition, ec);
  boost::asio::detail::throw_error(ec);
  return bytes_transferred;
}

#endif // !defined(BOOST_NO_IOSTREAM)

namespace detail
{
  template <typename AsyncRandomAccessReadDevice,
      typename MutableBufferSequence, typename CompletionCondition,
      typename ReadHandler>
  class read_at_handler
  {
  public:
    typedef boost::asio::detail::consuming_buffers<
      mutable_buffer, MutableBufferSequence> buffers_type;

    read_at_handler(AsyncRandomAccessReadDevice& stream,
        boost::uint64_t offset, const buffers_type& buffers,
        CompletionCondition completion_condition, ReadHandler handler)
      : stream_(stream),
        offset_(offset),
        buffers_(buffers),
        total_transferred_(0),
        completion_condition_(completion_condition),
        handler_(handler)
    {
    }

    void operator()(const boost::system::error_code& ec,
        std::size_t bytes_transferred)
    {
      total_transferred_ += bytes_transferred;
      buffers_.consume(bytes_transferred);
      buffers_.prepare(detail::adapt_completion_condition_result(
            completion_condition_(ec, total_transferred_)));
      if (buffers_.begin() == buffers_.end())
      {
        handler_(ec, total_transferred_);
      }
      else
      {
        stream_.async_read_some_at(
            offset_ + total_transferred_, buffers_, *this);
      }
    }

  //private:
    AsyncRandomAccessReadDevice& stream_;
    boost::uint64_t offset_;
    buffers_type buffers_;
    std::size_t total_transferred_;
    CompletionCondition completion_condition_;
    ReadHandler handler_;
  };

  template <typename AsyncRandomAccessReadDevice,
      typename MutableBufferSequence, typename CompletionCondition,
      typename ReadHandler>
  inline void* asio_handler_allocate(std::size_t size,
      read_at_handler<AsyncRandomAccessReadDevice, MutableBufferSequence,
        CompletionCondition, ReadHandler>* this_handler)
  {
    return boost_asio_handler_alloc_helpers::allocate(
        size, this_handler->handler_);
  }

  template <typename AsyncRandomAccessReadDevice,
      typename MutableBufferSequence, typename CompletionCondition,
      typename ReadHandler>
  inline void asio_handler_deallocate(void* pointer, std::size_t size,
      read_at_handler<AsyncRandomAccessReadDevice, MutableBufferSequence,
        CompletionCondition, ReadHandler>* this_handler)
  {
    boost_asio_handler_alloc_helpers::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename Function, typename AsyncRandomAccessReadDevice,
      typename MutableBufferSequence, typename CompletionCondition,
      typename ReadHandler>
  inline void asio_handler_invoke(const Function& function,
      read_at_handler<AsyncRandomAccessReadDevice, MutableBufferSequence,
        CompletionCondition, ReadHandler>* this_handler)
  {
    boost_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }
} // namespace detail

template <typename AsyncRandomAccessReadDevice, typename MutableBufferSequence,
    typename CompletionCondition, typename ReadHandler>
inline void async_read_at(AsyncRandomAccessReadDevice& d,
    boost::uint64_t offset, const MutableBufferSequence& buffers,
    CompletionCondition completion_condition, ReadHandler handler)
{
  boost::asio::detail::consuming_buffers<
    mutable_buffer, MutableBufferSequence> tmp(buffers);

  boost::system::error_code ec;
  std::size_t total_transferred = 0;
  tmp.prepare(detail::adapt_completion_condition_result(
        completion_condition(ec, total_transferred)));
  if (tmp.begin() == tmp.end())
  {
    d.get_io_service().post(detail::bind_handler(
          handler, ec, total_transferred));
    return;
  }

  d.async_read_some_at(offset, tmp,
      detail::read_at_handler<AsyncRandomAccessReadDevice,
        MutableBufferSequence, CompletionCondition, ReadHandler>(
          d, offset, tmp, completion_condition, handler));
}

template <typename AsyncRandomAccessReadDevice, typename MutableBufferSequence,
    typename ReadHandler>
inline void async_read_at(AsyncRandomAccessReadDevice& d,
    boost::uint64_t offset, const MutableBufferSequence& buffers,
    ReadHandler handler)
{
  async_read_at(d, offset, buffers, transfer_all(), handler);
}

#if !defined(BOOST_NO_IOSTREAM)

namespace detail
{
  template <typename AsyncRandomAccessReadDevice, typename Allocator,
      typename CompletionCondition, typename ReadHandler>
  class read_at_streambuf_handler
  {
  public:
    read_at_streambuf_handler(AsyncRandomAccessReadDevice& stream,
        boost::uint64_t offset, basic_streambuf<Allocator>& streambuf,
        CompletionCondition completion_condition, ReadHandler handler)
      : stream_(stream),
        offset_(offset),
        streambuf_(streambuf),
        total_transferred_(0),
        completion_condition_(completion_condition),
        handler_(handler)
    {
    }

    void operator()(const boost::system::error_code& ec,
        std::size_t bytes_transferred)
    {
      total_transferred_ += bytes_transferred;
      streambuf_.commit(bytes_transferred);
      std::size_t max_size = detail::adapt_completion_condition_result(
            completion_condition_(ec, total_transferred_));
      std::size_t bytes_available = std::min<std::size_t>(512,
          std::min<std::size_t>(max_size,
            streambuf_.max_size() - streambuf_.size()));
      if (bytes_available == 0)
      {
        handler_(ec, total_transferred_);
      }
      else
      {
        stream_.async_read_some_at(offset_ + total_transferred_,
            streambuf_.prepare(bytes_available), *this);
      }
    }

  //private:
    AsyncRandomAccessReadDevice& stream_;
    boost::uint64_t offset_;
    boost::asio::basic_streambuf<Allocator>& streambuf_;
    std::size_t total_transferred_;
    CompletionCondition completion_condition_;
    ReadHandler handler_;
  };

  template <typename AsyncRandomAccessReadDevice, typename Allocator,
      typename CompletionCondition, typename ReadHandler>
  inline void* asio_handler_allocate(std::size_t size,
      read_at_streambuf_handler<AsyncRandomAccessReadDevice, Allocator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    return boost_asio_handler_alloc_helpers::allocate(
        size, this_handler->handler_);
  }

  template <typename AsyncRandomAccessReadDevice, typename Allocator,
      typename CompletionCondition, typename ReadHandler>
  inline void asio_handler_deallocate(void* pointer, std::size_t size,
      read_at_streambuf_handler<AsyncRandomAccessReadDevice, Allocator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    boost_asio_handler_alloc_helpers::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename Function, typename AsyncRandomAccessReadDevice,
      typename Allocator, typename CompletionCondition, typename ReadHandler>
  inline void asio_handler_invoke(const Function& function,
      read_at_streambuf_handler<AsyncRandomAccessReadDevice, Allocator,
        CompletionCondition, ReadHandler>* this_handler)
  {
    boost_asio_handler_invoke_helpers::invoke(
        function, this_handler->handler_);
  }
} // namespace detail

template <typename AsyncRandomAccessReadDevice, typename Allocator,
    typename CompletionCondition, typename ReadHandler>
inline void async_read_at(AsyncRandomAccessReadDevice& d,
    boost::uint64_t offset, boost::asio::basic_streambuf<Allocator>& b,
    CompletionCondition completion_condition, ReadHandler handler)
{
  boost::system::error_code ec;
  std::size_t total_transferred = 0;
  std::size_t max_size = detail::adapt_completion_condition_result(
        completion_condition(ec, total_transferred));
  std::size_t bytes_available = std::min<std::size_t>(512,
      std::min<std::size_t>(max_size, b.max_size() - b.size()));
  if (bytes_available == 0)
  {
    d.get_io_service().post(detail::bind_handler(
          handler, ec, total_transferred));
    return;
  }

  d.async_read_some_at(offset, b.prepare(bytes_available),
      detail::read_at_streambuf_handler<AsyncRandomAccessReadDevice, Allocator,
        CompletionCondition, ReadHandler>(
          d, offset, b, completion_condition, handler));
}

template <typename AsyncRandomAccessReadDevice, typename Allocator,
    typename ReadHandler>
inline void async_read_at(AsyncRandomAccessReadDevice& d,
    boost::uint64_t offset, boost::asio::basic_streambuf<Allocator>& b,
    ReadHandler handler)
{
  async_read_at(d, offset, b, transfer_all(), handler);
}

#endif // !defined(BOOST_NO_IOSTREAM)

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_READ_AT_IPP
