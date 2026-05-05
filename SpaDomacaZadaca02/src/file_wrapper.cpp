#include "file_wrapper.hpp"

using namespace util; // For convenience, really

FileWrapper::FileWrapper(std::string filename, std::ios::openmode openmode) : file(std::move(filename), openmode) {} // Read by default

FileWrapper::~FileWrapper() {
	file.close(); // Works because RAII
}

FileWrapper::operator bool() const {
	return static_cast<bool>(file);
}

std::fstream& FileWrapper::get() {
	return file;
}

void FileWrapper::reset() {
	file.clear();
	file.seekg(0, std::ios::beg);
	file.seekp(0, std::ios::beg);
}

bool FileWrapper::skip_line() {
	std::string temp;
	return static_cast<bool>(std::getline(file, temp));
}

// Reads the next line of the file into &container, returns false on EOF
bool FileWrapper::read_line(std::string &container, char delimiter) {
	return static_cast<bool>(std::getline(file, container, delimiter));
}

template<typename T>
void FileWrapper::reserve_vector(std::vector<T>& v) {
	size_t num_lines = 0;
	std::string line;

	while (read_line(line)) num_lines++;
	v.reserve(num_lines);
	reset();
}

template<typename T>
void FileWrapper::copy_to_vector(std::vector<T>& v) {
	v.clear();
	std::string line;

	while (read_line(line)) v.push_back(line);
	reset();
}

void FileWrapper::debug_print() {
	std::string line;
	while (read_line(line)) {
		std::cout << line << '\n';
	}
	reset();
}

bool FileWrapper::write_line(const std::string &line) {
	return static_cast<bool>(file << line << '\n');
}
