#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include <iostream>
#include <string>
#include <fstream>
#include <ios>
#include <vector>

// Guess who learned what RAII is
class FileWrapper {
public:
	FileWrapper(std::string filename, std::ios::openmode openmode = std::ios::in) : file(std::move(filename), openmode) {} // Read by default

	~FileWrapper() {
		file.close(); // Works because RAII
	}

	explicit operator bool() const {
		return static_cast<bool>(file);
	}
	
	std::fstream &get() {
		return file;
	}

	void reset() {
		file.clear();
		file.seekg(0, std::ios::beg);
		file.seekp(0, std::ios::beg);
	}

	bool skip_line() {
		std::string temp;
		return static_cast<bool>(std::getline(file, temp));
	}

	// Reads the next line of the file into &container, returns false on EOF
	bool read_line(std::string &container, char delimiter = '\n') {
		return static_cast<bool>(std::getline(file, container, delimiter));
	}

	template<typename T>
	void reserve_vector(std::vector<T>& v) {
		size_t num_lines = 0;
		std::string line;

		while (read_line(line)) num_lines++;
		v.reserve(num_lines);
		reset();
	}

	template<typename T>
	void copy_to_vector(std::vector<T>& v) {
		v.clear();
		std::string line;

		while (read_line(line)) v.push_back(line);
		reset();
	}

	void debug_print() {
		std::string line;
		while (read_line(line)) {
			std::cout << line << '\n';
		}
		reset();
	}

	bool write_line(const std::string &line) {
		return static_cast<bool>(file << line << '\n');
	}
private:
	std::fstream file;
};

#endif // !FILE_WRAPPER_H
