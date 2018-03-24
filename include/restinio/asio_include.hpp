/*
	restinio
*/

/*!
	Selective include of asio/boost::asio.
*/

#pragma once

#if !defined(RESTINIO_USE_BOOST_ASIO)

// RESTinio uses stand-alone version of asio.
#include <asio.hpp>

namespace restinio
{
	namespace asio_ns = ::asio;

	//! Adoptation functions to cover differences between snad-alone and beast asio.
	//! \{
	inline bool error_is_operation_aborted( const asio_ns::error_code & ec )
	{
		return ec == asio_ns::error::operation_aborted;
	}

	inline bool error_is_eof( const asio_ns::error_code & ec )
	{
		return ec == asio_ns::error::eof;
	}

	namespace asio_ec
	{
		constexpr auto  eof = asio_ns::error::eof;
	} /* namespace err */

	//! \}
} /* namespace restinio */

	#if defined(ASIO_HAS_WINDOWS_OVERLAPPED_PTR)
		// Define feature macro with the same name for stand-alone and boost asio.
		#define RESTINIO_ASIO_HAS_WINDOWS_OVERLAPPED_PTR
	#endif

#else

// RESTinio uses boost::asio.
#include <boost/asio.hpp>

namespace restinio
{

	namespace asio_ns
	{
		using namespace ::boost::asio;
		using error_code = ::boost::system::error_code;
	} /* namespace asio_ns */

	inline bool error_is_operation_aborted( const asio_ns::error_code & ec )
	{
		return ec == asio_ns::error::basic_errors::operation_aborted;
	}

	inline bool error_is_eof( const asio_ns::error_code & ec )
	{
		return ec == asio_ns::error::misc_errors::eof;
	}


	namespace asio_ec
	{
		constexpr auto eof = asio_ns::error::misc_errors::eof;
	} /* namespace err */

} /* namespace restinio */

	#if defined(BOOST_ASIO_HAS_WINDOWS_OVERLAPPED_PTR)
		// Define feature macro with the same name for stand-alone and boost asio.
		#define RESTINIO_ASIO_HAS_WINDOWS_OVERLAPPED_PTR
	#endif

#endif
