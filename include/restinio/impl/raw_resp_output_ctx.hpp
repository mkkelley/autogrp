/*
	restinio
*/

/*!
	Helper output context for writing buffers to output stream (socket).
*/

#pragma once

#include <vector>

#include <restinio/asio_include.hpp>

#include <restinio/buffers.hpp>
#include <restinio/impl/sendfile_operation.hpp>

namespace restinio
{

namespace impl
{

//
// raw_resp_output_ctx_t
//

//! Helper class for writting response data.
struct raw_resp_output_ctx_t
{
	static constexpr auto
	max_iov_len()
	{
		using len_t = decltype( asio_ns::detail::max_iov_len );
		return std::min< len_t >( asio_ns::detail::max_iov_len, 64 );
	}

	raw_resp_output_ctx_t()
	{
		m_asio_bufs.reserve( max_iov_len() );
		m_bufs.reserve( max_iov_len() );
	}

	const std::vector< asio_ns::const_buffer > &
	create_bufs()
	{
		for( const auto & buf : m_bufs )
		{
			m_asio_bufs.emplace_back( buf.buf() );
		}

		m_transmitting = true;
		return m_asio_bufs;
	}

	void
	done()
	{
		m_asio_bufs.resize( 0 );
		m_bufs.resize( 0 );
		m_transmitting = false;
	}

	template< typename Socket, typename After_Write_CB >
	void
	start_sendfile_operation(
		asio_ns::executor executor,
		Socket & socket,
		After_Write_CB && after_sendfile_cb )
	{
		assert( !m_sendfile_operation );
		assert( 1 == m_bufs.size() );

		const auto & sf_op = m_bufs.front().sendfile_operation();

		assert( sf_op.is_valid() );

		if( !sf_op.is_valid() )
		{
			// This must never happen.
			throw exception_t{ "invalid file descriptor in sendfile operation." };
		}

		auto sendfile_operation =
			std::make_shared< sendfile_operation_runner_t< Socket > >(
				sf_op,
				std::move( executor ),
				socket,
				std::move( after_sendfile_cb ) );

		m_sendfile_operation = std::move( sendfile_operation );
		m_transmitting = true;
		m_sendfile_operation->start();
	}

	void
	finish_sendfile_operation()
	{
		assert( m_sendfile_operation );
		m_sendfile_operation.reset();
		m_bufs.resize( 0 );
		m_transmitting = false;
	}

	auto
	sendfile_timelimit()
	{
		assert( 1 == m_bufs.size() );
		return m_bufs.front().sendfile_operation().timelimit();
	}

	bool
	transmitting() const
	{
		return m_transmitting;
	}

	//! Obtains ready buffers if any.
	/*!
		\note Ready_Buffers_Source must have
		pop_ready_buffers() member function.
	*/
	template < class Ready_Buffers_Source >
	writable_item_type_t
	obtain_bufs(
		Ready_Buffers_Source & ready_buffers_source )
	{
		return ready_buffers_source.pop_ready_buffers( max_iov_len(), m_bufs );
	}

	private:
		//! Is transmition running?
		bool m_transmitting{ false };

		//! Asio buffers.
		std::vector< asio_ns::const_buffer > m_asio_bufs;

		//! Real buffers with data.
		writable_items_container_t m_bufs;

		sendfile_operation_shared_ptr_t m_sendfile_operation;
};

} /* namespace impl */

} /* namespace restinio */
