/*
	restinio
*/

/*!
	Sendfile routine.

	@since v.0.4.3
*/

#pragma once

#include <string>
#include <chrono>
#include <array>

#include <fmt/format.h>

#include <restinio/asio_include.hpp>
#include <restinio/string_view.hpp>
#include <restinio/exception.hpp>

/*
	Defenitions for:
		file_descriptor_t
		file_offset_t
		file_size_t
*/

#if defined( _MSC_VER ) || defined(__MINGW32__)
	#include "sendfile_defs_win.hpp"
#elif (defined( __clang__ ) || defined( __GNUC__ )) && !defined(__WIN32__)
	#include "sendfile_defs_posix.hpp"
#else
	#if defined (RESTINIO_ENABLE_SENDFILE_DEFAULT_IMPL)
		#include "sendfile_defs_default.hpp"
	#else
		#error "Sendfile not supported, to enable default implementation define RESTINIO_ENABLE_SENDFILE_DEFAULT_IMPL macro"
	#endif
#endif

namespace restinio
{

//! Default chunk size for sendfile operation.
//! @since v.0.4.3
constexpr file_size_t sendfile_default_chunk_size = 1024 * 1024;

//! Maximum size of a chunk
//! @since v.0.4.3
constexpr file_size_t sendfile_max_chunk_size = 1024 * 1024 * 1024;

//
// sendfile_chunk_size_guarded_value_t
//

//! A guard class for setting chunk size.
/*!
	If chunk_size_value does not fit in [1, sendfile_max_chunk_size].
	interval then it is shrinked to fit in the interval.

	@since v.0.4.3
*/
class sendfile_chunk_size_guarded_value_t
{
		//! Checks chunk_size_value and returns a value in [1, sendfile_max_chunk_size].
		/*
			- If chunk_size_value is zero returns 1.
			- If chunk_size_value is greater than sendfile_max_chunk_size returns sendfile_max_chunk_size.
			- Otherwise returns chunk_size_value itself.
		*/
		static file_size_t
		clarify_chunk_size( file_size_t chunk_size_value )
		{
			if( 0 == chunk_size_value )
				return sendfile_default_chunk_size;

			if( sendfile_max_chunk_size < chunk_size_value )
				return sendfile_max_chunk_size;

			return chunk_size_value;
		}

	public:

		sendfile_chunk_size_guarded_value_t( file_size_t chunk_size_value )
			:	m_chunk_size{ clarify_chunk_size( chunk_size_value ) }
		{}

		//! Get the valid value of a chunk size.
		file_size_t
		value( ) const
		{
			return m_chunk_size;
		}

	private:
		//! Valid value of the chunk size.
		const file_size_t m_chunk_size;
};

//
// file_descriptor_holder_t
//

//! Wrapper class for working with native file handler.
/*
	Class is responsible for managing file descriptor as resource.

	@since v.0.4.3
*/
class file_descriptor_holder_t
{
	public:
		//! Swap two descriptors
		friend void
		swap( file_descriptor_holder_t & left, file_descriptor_holder_t & right ) noexcept
		{
			using std::swap;
			swap( left.m_file_descriptor, right.m_file_descriptor );
		}

		//! Init constructor.
		file_descriptor_holder_t( file_descriptor_t fd )
			:	m_file_descriptor{ fd }
		{}

		/** @name Copy semantics.
		 * @brief Not allowed.
		*/
		///@{
		file_descriptor_holder_t( const file_descriptor_holder_t & ) = delete;
		const file_descriptor_holder_t & operator = ( const file_descriptor_holder_t & ) = delete;
		///@}

		file_descriptor_holder_t( file_descriptor_holder_t && fdh ) noexcept
			:	m_file_descriptor{ fdh.m_file_descriptor }
		{
			fdh.release();
		}

		file_descriptor_holder_t & operator = ( file_descriptor_holder_t && fdh ) noexcept
		{
			if( this != &fdh )
			{
				file_descriptor_holder_t tmp{ std::move( fdh ) };
				swap( *this, tmp );
			}
			return *this;
		}

		~file_descriptor_holder_t()
		{
			if( is_valid() )
				close_file( m_file_descriptor );
		}

		//! Check if file descriptor is valid.
		bool is_valid() const noexcept
		{
			return null_file_descriptor() != m_file_descriptor;
		}

		//Get file descriptor.
		file_descriptor_t fd() const noexcept
		{
			return m_file_descriptor;
		}

		// Release stored descriptor.
		void release() noexcept
		{
			m_file_descriptor = null_file_descriptor();
		}

	private:
		//! Target file descriptor.
		file_descriptor_t m_file_descriptor;
};

//
// sendfile_t
//

//! Send file write operation description.
/*!
	Class gives a fluen-interface for setting various parameters
	for performing send file operation.

	@since v.0.4.3
*/
class sendfile_t
{
		friend sendfile_t sendfile( file_descriptor_holder_t , file_size_t , file_size_t );

		sendfile_t(
			file_descriptor_holder_t fdh,
			file_size_t file_total_size,
			sendfile_chunk_size_guarded_value_t chunk )
			:	m_file_descriptor{ std::move( fdh ) }
			,	m_file_total_size{ file_total_size }
			,	m_offset{ 0 }
			,	m_size{ file_total_size }
			,	m_chunk_size{ chunk.value() }
			,	m_timelimit{ std::chrono::steady_clock::duration::zero() }
		{}

	public:
		friend void swap( sendfile_t & left, sendfile_t & right ) noexcept
		{
			using std::swap;
			std::swap( left.m_file_descriptor, right.m_file_descriptor );
			std::swap( left.m_file_total_size, right.m_file_total_size );
			std::swap( left.m_offset, right.m_offset );
			std::swap( left.m_size, right.m_size );
			std::swap( left.m_chunk_size, right.m_chunk_size );
			std::swap( left.m_timelimit, right.m_timelimit );
		}

		/** @name Copy semantics.
		 * @brief Not allowed.
		*/
		///@{
		sendfile_t( const sendfile_t & ) = delete;
		const sendfile_t & operator = ( const sendfile_t & ) = delete;
		///@}

		/** @name Move semantics.
		 * @brief After move sf prameter becomes invalid.
		*/
		///@{
		sendfile_t( sendfile_t && sf ) noexcept
			:	m_file_descriptor{ std::move( sf.m_file_descriptor ) }
			,	m_file_total_size{ sf.m_file_total_size }
			,	m_offset{ sf.m_offset }
			,	m_size{ sf.m_size }
			,	m_chunk_size{ sf.m_chunk_size }
			,	m_timelimit{ sf.m_timelimit }
		{}

		sendfile_t & operator = ( sendfile_t && sf ) noexcept
		{
			if( this != &sf )
			{
				sendfile_t tmp{ std::move( sf ) };
				swap( *this, tmp );
			}

			return *this;
		}
		///@}

		//! Check if file is valid.
		bool is_valid() const noexcept { return m_file_descriptor.is_valid(); }

		//! Get total file size.
		auto file_total_size() const noexcept { return m_file_total_size; }

		//! Get offset of data to write.
		auto offset() const noexcept { return m_offset; }

		//! Get size of data to write.
		auto size() const noexcept { return m_size; }

		/** @name Set file offset and size.
		 * @brief Tries to set offset parameter to offset_value and size to size value.
		 *
		 * If sendfile_t object is invalid then exception is thrown.
		 *
		 * If offset_value is a valid offset within current file then ofsett is
		 * set to new value. The size might be shrinked so to represent at most
		 * the length of file from a given offset.
		*/
		///@{
		sendfile_t &
		offset_and_size(
			file_offset_t offset_value,
			file_size_t size_value = std::numeric_limits< file_size_t >::max() ) &
		{
			check_file_is_valid();

			if( static_cast< file_size_t >( offset_value ) > m_file_total_size )
			{
				throw exception_t{
					fmt::format(
						"invalid file offset: {}, while file size is {}",
						offset_value,
						m_file_total_size ) };
			}

			m_offset = offset_value;
			m_size =
				std::min< file_size_t >(
					m_file_total_size - static_cast< file_size_t >( offset_value ),
					size_value );

			return *this;
		}

		sendfile_t &&
		offset_and_size(
			file_offset_t offset_value,
			file_size_t size_value = std::numeric_limits< file_size_t >::max() ) &&
		{
			return std::move( this->offset_and_size( offset_value, size_value ) );
		}
		///@}



		auto chunk_size() const { return m_chunk_size; }

		/** @name Set prefered chunk size to use in  write operation.
		 * @brief Set the maximum possible size of the portion of data
		 * to be send at a single write file operation (from file to socket).
		*/
		///@{
		sendfile_t &
		chunk_size( sendfile_chunk_size_guarded_value_t chunk ) &
		{
			check_file_is_valid();

			m_chunk_size = chunk.value();
			return *this;
		}

		//! Set prefered chunk size to use in  write operation.
		sendfile_t &&
		chunk_size( sendfile_chunk_size_guarded_value_t chunk ) &&
		{
			return std::move( this->chunk_size( chunk ) );
		}
		///@}

		auto timelimit() const noexcept { return m_timelimit; }

		/** @name Set timelimit on  write operation..
		 * @brief Set the maximum dureation of this sendfile operation
		 * (the whole thing, not just a single iteration).
		*/
		///@{
		sendfile_t &
		timelimit( std::chrono::steady_clock::duration timelimit_value ) &
		{
			check_file_is_valid();

			m_timelimit = std::max( timelimit_value, std::chrono::steady_clock::duration::zero() );
			return *this;
		}

		sendfile_t &&
		timelimit( std::chrono::steady_clock::duration timelimit_value ) &&
		{
			return std::move( this->timelimit( timelimit_value ) );
		}
		///@}

		//! Get the file descriptor of a given sendfile operation.
		file_descriptor_t
		file_descriptor() const noexcept
		{
			return m_file_descriptor.fd();
		}

	private:
		//! Check if stored file descriptor is valid, and throws if it is not.
		void
		check_file_is_valid() const
		{
			if( !is_valid() )
			{
				throw exception_t{ "invalid file descriptor" };
			}
		}

		//! Native file descriptor.
		file_descriptor_holder_t m_file_descriptor;
		//! The size of the file.
		file_size_t m_file_total_size;

		//! Data offset within the file.
		file_offset_t m_offset;
		//! The size of data portion in file.
		file_size_t m_size;

		//! A prefered chunk size for a single write call.
		file_size_t m_chunk_size;

		//! Timelimit for writing all the given data.
		/*!
			Zero value stands for default write operation timeout.
		*/
		std::chrono::steady_clock::duration m_timelimit{ std::chrono::steady_clock::duration::zero() };
};

//
// sendfile()
//

/** @name Functions for creating sendfile_t objects.
 * @brief A group of function to create sendfile_t, that is convertad to writable items
 * used as a part of response.
 * @since v.0.4.3
*/
///@{
inline sendfile_t
sendfile(
	//! Native file descriptor.
	file_descriptor_holder_t fd,
	//! Total file size.
	file_size_t total_file_size,
	//! The max size of a data to be send on a single iteration.
	file_size_t chunk_size = sendfile_default_chunk_size )
{
	return sendfile_t{ std::move( fd ), total_file_size, chunk_size };
}

inline sendfile_t
sendfile(
	//! Path to file.
	const char * file_path,
	//! The max size of a data to be send on a single iteration.
	file_size_t chunk_size = sendfile_default_chunk_size )
{
	file_descriptor_holder_t fd{ open_file( file_path ) };

	auto total_file_size = size_of_file( fd.fd() );

	return sendfile( std::move( fd ), total_file_size , chunk_size );
}

inline sendfile_t
sendfile(
	//! Path to file.
	const std::string & file_path,
	//! The max size of a data to be send on a single iteration.
	file_size_t chunk_size = sendfile_default_chunk_size )
{
	return sendfile( file_path.c_str(), chunk_size );
}

inline sendfile_t
sendfile(
	//! Path to file.
	string_view_t file_path,
	//! The max size of a data to be send on a single iteration.
	file_size_t chunk_size = sendfile_default_chunk_size )
{
	return
		sendfile(
			std::string{ file_path.data(), file_path.size() },
			chunk_size );
}
///@}

} /* namespace restinio */
