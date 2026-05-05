#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include <iostream>
#include <string>
#include <fstream>
#include <ios>
#include <vector>

namespace util {

class FileWrapper {
public:
	FileWrapper(std::string filename, std::ios::openmode openmode = std::ios::in); // Read by default

	~FileWrapper();

	explicit operator bool() const;
	
	std::fstream& get();

	void reset();

	bool skip_line();

	// Reads the next line of the file into &container, returns false on EOF
	bool read_line(std::string &container, char delimiter = '\n');

	template<typename T>
	void reserve_vector(std::vector<T>& v);

	template<typename T>
	void copy_to_vector(std::vector<T>& v);

	void debug_print();

	bool write_line(const std::string &line);
private:
	std::fstream file;
};

};

#endif // !FILE_WRAPPER_H
