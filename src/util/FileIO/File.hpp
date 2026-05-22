// add file abstraction in here for output file and json config file to inherit from
// use std::ofstream (or fstream, color takes ostream so idk) and then have maybe JSON file or output file inherit from it so they share some properties and clean up std abstraction
// you could split tihs into input and output file eventually but rn we just need io

#pragma once

#include <string>

namespace Aero::RT::Util {
	class File {
	public:
		virtual ~File() = default;

		virtual bool good() const = 0;
		virtual bool is_open() const = 0;

		virtual std::string read_all() = 0;
		virtual std::string read_line() = 0;

		virtual void write(const std::string& data) = 0;
		virtual void write_line(const std::string& data) = 0;
		//virtual void write_line(std::string_view data) = 0;

		File(const File&) = delete;
		File& operator=(const File&) = delete;

		File(File&&) noexcept = default;
		File& operator=(File&&) noexcept = default;
	};
}