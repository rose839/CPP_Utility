/**
 * @file  istream_line_reader.h
 *
 * Header file for istream_line_reader, an easy-to-use line-based
 * istream reader.
 *
 * This class allows using istreams in a Pythonic way (if your compiler
 * supports C++11 or later), e.g.:
 *
 * @code
 * for (auto& line : istream_line_reader(std::cin)) {
 *     // Process line
 * }
 
 * @endcode
 *
 * This code can be used without C++11, but using it would be less
 * convenient then.
 *
 *
 * @date  2020-07-02
 */

#ifndef ISTREAM_LINE_READER_H
#define ISTREAM_LINE_READER_H

#include <assert.h>  // assert
#include <stddef.h>  // ptrdiff_t
#include <istream>   // std::isream
#include <iterator>  // std::input_iterator_tag
#include <stdexcept> // std::runtime_error
#include <string>    // std::string

/* Class to allow iteration over all lines from input stream. */
class IstreamLineReader {
public:
	
	/* Iterator that contains the line content. */
	class iterator {
	public:
		/* those five types below is necessary for defining an iterator */
		using difference_type   = ptrdiff_t;               // distance type between two iterator
		using value_type        = std::string;             // value type of obj pointed by iterator
		using pointer           = const value_type*;       // pointer type of obj pointed by iterator
		using reference         = const value_type&;       // reference type of obj pointed by iterator
		using iterator_category = std::input_iterator_tag; // means this iterator is an input iterator

		iterator() noexcept : m_stream(nullptr) {}

		explicit iterator(std::istream& stream) : m_stream(&stream) {
			++*this;
		}

		/* return line reference */
		reference operator*() const noexcept {
			assert(m_stream != nullptr);
			return m_line;
		}

		/* return line pointer */
		pointer operator->() const noexcept {
			assert(m_stream != nullptr);
			return &m_line;
		}

		/* pre ++ */
		iterator& operator++() {
			assert(m_stream != nullptr);
			getline(*m_stream, m_line);

			if (!*m_stream) {
				m_stream = nullptr;
			}
			
			return *this;
		}

		/* post ++ */
		iterator operator++(int) {
			iterator tmp(*this);
			++*this;
			return tmp;
		}

		bool operator==(const iterator& rhs) const noexcept {
			return m_stream == rhs.m_stream;
		}

		 bool operator!=(const iterator& rhs) const noexcept {
		 	/* method reuse */
            return !operator==(rhs);
        }
	private:
		std::istream *m_stream; // point to the original istream
		std::string m_line;     // used to contain one line content from istream
	};

	IstreamLineReader() noexcept : m_stream(nullptr) {}

	explicit IstreamLineReader(std::istream& is) noexcept : m_stream(&is) {}

	/* used to get beginning position iterator */
	iterator begin() {
		if (!m_stream) {
			throw std::logic_error("input stream is null");
		}

		if (m_stream->fail()) {
			throw std::runtime_error("input stream is null");
		}

		return iterator(*m_stream);
	}

	/* uesed to get ending position iterator */
	iterator end() const noexcept {
		return iterator();
	}
	
private:
	std::istream *m_stream;
};

#endif