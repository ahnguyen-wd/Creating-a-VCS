/*
Author: Andrew Nguyen - Team CPL
Contact Info: ahnguyen714@csu.fullerton.edu

Source code for project 2 (CPSC 362 CSUF) - Creating a repository.
*/
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <filesystem>
#include <regex>
#include <fstream>
#include <ctime>

using namespace std;
namespace fs = std::experimental::filesystem;

//Function to get current time and date
string datetime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
	return std::string(buffer);
}

//Function to check if file is empty
//Argument: ifstream file
bool is_empty(ifstream& pFile)
{
    return pFile.peek() == ifstream::traits_type::eof();
}

//Regular expression to check if file is in artID and manifestfile format
//This is used to check folders
regex artifactID("[0-9]+-L[0-9]+.[a-zA-Z]*");
regex manifestFile("manifest[0-9]+.txt");

//Funtion to copy the necessary directory and files from source to target
//Paramters 
//source: Path to source folder. 
//target: Path to target folder
void vcs(string, ofstream&);
void vcs_out(string, ofstream&);

//Function to check if the target folder has already been built, if so it will skip the copying
//Generate the artifact ID
//To get the ASCII byte, cast the char into int: int(<char>)
string artIdGenerator(string, string, int);

//Function declartions
void create_repo(string, string, string);
void check_in(string, string, string);
void check_out(string, string, string);
void labeling(string, string, string);
ofstream& manfile(string,string,string);

int main(int argc, char *argv[]) {
	string argvCmd = argv[1];

	check_in(argv[1], argv[2], argv[3]);

	//if (argvCmd == "create-repo") {
	//	create_repo(argv[1], argv[2], argv[3]);
	//}
	//else if (argvCmd == "check-in") {
	//	source = argv[2];
	//	target = argv[3];
	//} 
	//else if (argvCmd == "check-out") {
	//	source = argv[2];
	//	target = argv[3];
	//}
	//else if (argvCmd == "label") {

	//}


	std::system("PAUSE");
	return 0;
}

//In order to properly create the folders and files, we will first go through each directory.
//Check if file is either is_directory(p) or is_regular(p)
//If it is a directory, create a directory at the target location
//If it is a regular file, create a folder with its name, generate the artID, copy the file with the artID as file name
//Check if file with artID is present, if so, skip
void vcs(string target, ofstream& manfile) {
	string temp = "";
	int fileSize = 0;
	int count = 1;

	//Copy all the files recursively to target location
	//After moving all the directory and files, this
	//function will convert the files to folder,
	//convert the name to artifact ID and move them into
	//the folder
	for (auto & p : fs::directory_iterator(target)) {
		temp = p.path().string();
		manfile << "Path: " << temp << endl;

		if (fs::is_directory(p)) {
			vcs(temp, manfile);
		}
		else if (fs::is_regular_file(p)) {
			if (!regex_match(p.path().filename().string(), manifestFile)) {
				if (!regex_match(p.path().filename().string(), artifactID)) {
					fs::current_path(target);
					string fileName = p.path().filename().string() + "-";

					//Creating the artifact ID to rename the file
					int fileSize = fs::file_size(p);
					string artId = artIdGenerator(temp, p.path().filename().string(), fileSize);
					manfile << "Filepath: " << p.path().string() << endl << "ArtID: " << artId << endl;

					//Creating the directory and moving the file within
					fs::create_directory(fileName);
					fs::rename(p.path().string(), p.path().parent_path().string() + "\\" + fileName + "\\" + artId);
				}
			}
		}
	}
}

//Function used to create repo or check in
void vcs_out(string target, ofstream& manfile) {
	string temp = "";
	int fileSize = 0;
	int count = 1;

	//Copy all the files recursively to target location
	//After moving all the directory and files, this
	//function will convert the files to folder,
	//convert the name to artifact ID and move them into
	//the folder
	for (auto & p : fs::directory_iterator(target)) {
		temp = p.path().string();
		manfile << "Path: " << temp << endl;

		if (fs::is_directory(p)) {
			vcs(temp, manfile);
		}
		else if (fs::is_regular_file(p)) {
			if (!regex_match(p.path().filename().string(), manifestFile)) {
				if (!regex_match(p.path().filename().string(), artifactID)) {
					fs::current_path(target);
					string fileName = p.path().filename().string() + "-";

					//Creating the artifact ID to rename the file
					int fileSize = fs::file_size(p);
					string artId = artIdGenerator(temp, p.path().filename().string(), fileSize);
					manfile << "Filepath: " << endl << p.path().string() << endl << "ArtID: " << endl << artId << endl;

					//Creating the directory and moving the file within
					fs::create_directory(fileName);
					fs::rename(p.path().string(), p.path().parent_path().string() + "\\" + fileName + "\\" + artId);
				}
			}
		}
	}
}

//Generate artifical ID
string artIdGenerator(string sourceFilePath, string fileName, int fileSize) {
	//Initializing variables
	char c;
	string artId, extension = "";
	int chksum = 0;
	int chksumCount = 0;

	//Opening file to read through
	ifstream myFile;
	myFile.open(sourceFilePath);

	//Getting extension of the current source file
	extension = fileName.substr(fileName.find('.'), fileName.length());

	//This loop will go through each character in the file and 
	//properly calculate the checksum. If the chksum exceeds 2,147,483,647
	//it will wrap around.
	while (myFile.get(c)) {
		//This first if statement will check if the file goes over 2,147,483,647
		//After testing, it is realized that the integer becomes negative once it
		//exceeds the maximum size. To combat this, we will add the highest possible
		//ASCII byte checksum size (126 for char '~' * 17)
		if (chksum + 2142 < -50000) {
			chksum = (chksum - 2147483647);
		}
		if (chksumCount == 0) {
			chksum += int(c) * 1;
			chksumCount++;
		}
		else if (chksumCount == 1) {
			chksum += int(c) * 3;
			chksumCount++;
		}
		else if (chksumCount == 2) {
			chksum += int(c) * 7;
			chksumCount++;
		}
		else if (chksumCount == 3) {
			chksum += int(c) * 11;
			chksumCount++;
		}
		else if (chksumCount == 4) {
			chksum += int(c) * 17;
			chksumCount = 0;
		}
	}

	//Display the chksum calculated above to the following format
	//<calculated chksum>-L<file size in KB>.<extension type>
	artId = to_string(chksum) + "-L" + to_string(fileSize) + extension;

	//Close the opened file
	myFile.close();

	//Return artifcat ID
	return artId;
}

//Function to create repo
void create_repo(string cmd, string source, string target) {
	int manVersion = 0;

	fs::copy(source, target, fs::copy_options::recursive | fs::copy_options::skip_existing);

	//Creating manifest file in manData folder
	if (!fs::is_directory(target + "\\manData") || !fs::exists(target + "\\manData")) { // Check if src folder exists
		fs::create_directory(target + "\\manData"); // create src folder
	}

	string path = target + "\\manData\\manifest0.txt";
	ofstream manfile(path, std::ofstream::out);

	//Writing to manifest file
	if (manfile.is_open()) {
		manfile << "**************************************************" << endl;
		manfile << "Command: " << cmd << " " << source << " " << target << endl;
		manfile << "**************************************************" << endl << endl;
		manfile << "Actions:" << endl;
	}

	vcs(target, manfile);

	//Labeling manifest file with command used
	std::cout << "***********************************\n\n";
	std::cout << "      R E P O   C R E A T E D      \n\n";
	std::cout << "      Source: " << source << endl;
	std::cout << "      Target: " << target << endl << endl;
	std::cout << "***********************************\n";

	//Time stamping manifest file
	if(manfile.is_open()) {
		manfile << endl << "**************************************************" << endl;
		manfile << "Date and time is: " << datetime() << endl << endl;
		manfile << "**************************************************" << endl;
	}

	//Closing manfile
	manfile.close();
}

//Function to check in repo (update)
void check_in(string cmd, string source, string target) {
	int manVersion = 0;

	fs::copy(source, target, fs::copy_options::recursive | fs::copy_options::skip_existing);

	for (auto & p : fs::directory_iterator(target + "\\manData"))
		manVersion += 1;

	string path = target + "\\manData\\manifest" + to_string(manVersion) + ".txt";
	ofstream manfile(path, std::ofstream::out);

	if (manfile.is_open()) {
		manfile << "**************************************************" << endl;
		manfile << "Command: " << cmd << " " << source << " " << target << endl;
		manfile << "**************************************************" << endl << endl;
		manfile << "Actions:" << endl;
	}

	vcs(target, manfile);

	std::cout << "***********************************\n\n";
	std::cout << "      R E P O C  C H E C K _ I N     \n\n";
	std::cout << "      Source: " << source << endl;
	std::cout << "      Target: " << target << endl << endl;
	std::cout << "***********************************\n";

	if (manfile.is_open()) {
		manfile << endl << "**************************************************" << endl;
		manfile << "Date and time is: " << datetime() << endl << endl;
		manfile << "**************************************************" << endl;
	}

	manfile.close();
}

void check_out(string source, string target, int maniVersion) {
	fs::copy(source, target, fs::copy_options::recursive | fs::copy_options::skip_existing);

	string manPath = source + "\\manData\\manifest" + maniVersion + ".txt";
	ifstream manFile(manPath);

	vector<string> artName;
	string line = "";
	string artID = "";
	string pathCheck = "";
	string originalName = "";
	string found;
	string str;

	//Adding all artID to vector.
	if(!is_empty(manFile)) {
		while(getline(manFile, line)) {
			if(line == "ArtID: ") {
				getline(manfile, line);
				artName.push_back(line);
			}
		}
	}

	for (auto & p : fs::directory_iterator(target)) {
		temp = p.path().string();

		if (fs::is_directory(p)) {
			pathCheck = temp;
			str = pathCheck.find_last_of("/\\");
			originalName = pathCheck.substr(str+1); 
			originalName.pop_back();

			if(pathCheck.back() == '`') {
				for (auto & p : fs::directory_iterator(pathCheck) {
					for(int i = 0; i < artName.size(); i++) {
						if(p.path().filename().string() == artName[i]) {
							filePath = p.path().parent_path() + "\\" + originalName;
							fs::copy(p.path(), filePath);
						}
					}
				}
			}
		}
		else if (fs::is_regular_file(p)) {
			if (!regex_match(p.path().filename().string(), manifestFile)) {
				if (!regex_match(p.path().filename().string(), artifactID)) {
					fs::current_path(target);
					string fileName = p.path().filename().string() + "-";

					//Creating the artifact ID to rename the file
					int fileSize = fs::file_size(p);
					string artId = artIdGenerator(temp, p.path().filename().string(), fileSize);
					manfile << "Filepath: " << endl << p.path().string() << endl << "ArtID: " << endl << artId << endl;

					//Creating the directory and moving the file within
					fs::create_directory(fileName);
					fs::rename(p.path().string(), p.path().parent_path().string() + "\\" + fileName + "\\" + artId);
				}
			}
		}
	}

	manFile.close();
}

//Function to add label at the end of file.
void labeling(string cmd, string labelName, string maniFile) {
	if (fs::exists(maniFile)) {
		ofstream manfile(maniFile, std::ofstream::app | std::ofstream::out);

		if (manfile.is_open()) {
			manfile << endl << "**************************************************" << endl << "Label added for reference: " << endl << labelName << endl << "**************************************************" << endl;
			manfile.close();
		}
		cout << "Label \"" << labelName << "\" added to " << maniFile << "." << endl;
	}
	else {
		cerr << "File does not exist to add label" << endl;
	}
	
}
