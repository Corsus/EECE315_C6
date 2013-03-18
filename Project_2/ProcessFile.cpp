#include <fstream>
#include <string>

#include "Constants.hpp"

using namespace std;

class ProcessFile {
public:
	ProcessFile() {
		path = DEFAULT_FILE_PATH;
		open();
	}

	~ProcessFile() {
		close();
	}

	string getLine() {
		if (myfile.is_open()) {
			getline(myfile,line);
		}
		return line;
	}

	void setPath(string path) {
		close();
		this->path = path;
		open();
	}

	bool endOfFile() {
		return myfile.eof();
	}

private:
	ifstream myfile;
	string line;
	string path;

	void open() {
		myfile.open(path.c_str());
	}

	void close() {
		if(myfile.is_open())
			myfile.close();
	}
};