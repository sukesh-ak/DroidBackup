#ifndef DROIDBACKUP_A_H_
#define DROIDBACKUP_A_H_

#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

// gz header 31 139 8 0 0 0 0 0
#define GZ_HEADER "\x1f\x8b\x08\x00\x00\x00\x00\x00"
#define GZ_HEADER_SIZE 8

class abackup
{
    public:
        bool IsBackup = false;
        int Version;
        bool IsCompressed = false;
        bool IsEncrypted = false;
        std::string dbFolder ;
        std::string encryption = "none";


        std::string BackupFilename = "backup.ab";
        std::string GzFilename;
        std::string GzPassword;
        /*
        * User SALT
        * Checksum SALT
        * PBKDF2 rounds (1000?)
        * User IV
        * Master Key Blob        
        */

        /***
         * Get backup file metadata
        */
        bool GetMetadata(); 
        /***
         * Convert the .ab file to .gz format
        */
        bool ConvertToGz();

        void ListOfDB();

        /***
         * Split String with a specified delimiter
        */
        std::vector<std::string> Split(const std::string& s, char seperator);

    private:
        int header_length = 0;

};

std::vector<std::string> abackup::Split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

bool abackup::GetMetadata()
{
    // Time to open the file
    ifstream backup_file(BackupFilename,ios::in);

    if (!backup_file.is_open())
    {
        std::cerr << "Could not open backup file: " << BackupFilename << endl;
        return EXIT_FAILURE;
    }

    // Read and process each line in the file
    std::string line;
    int linecnt = 1;

    // Backup header check
    while(std::getline(backup_file,line))
    {
        if (linecnt>4) break;   // interested only in first 4 lines
        // * ANDROID BACKUP
        // * 5=version
        // * 1=compression
        // * none/AES-256=encryption
        // * <content>
        header_length += line.length()+1;   // 1 for \n
        line = Split(line,'\n')[0];

        if ((linecnt == 1) && line.compare("ANDROID BACKUP")==0) IsBackup = true;
        if (linecnt == 2) Version = stoi(line);
        if (linecnt == 3) IsCompressed = (stoi(line)==1)?true:false;
        if (linecnt == 4) IsEncrypted = line.compare("none")?false:true;
        if ((linecnt == 4) && IsEncrypted) encryption = line;
        
        linecnt++;
    }
    backup_file.close();

    return true;
}

bool abackup::ConvertToGz()
{
    // Conversion routine
    ifstream backup_file(BackupFilename,ios::binary);
    
    // get pointer to associated buffer object
    std::filebuf* pbuf = backup_file.rdbuf();

    // get file size using buffer's members
    std::size_t size = pbuf->pubseekoff (0,backup_file.end);
    size = size - header_length;

    //std::cerr << "Buffer Size : " << size << endl;

    // allocate memory to contain file data
    char* buffer=new char[size];

    backup_file.seekg(header_length, ios::beg);     // set stream cursor to skip headers      
    backup_file.read(buffer,size);

    backup_file.close();

    ofstream gzfile(BackupFilename.append(".gz"));

    // Write GZ header
    //std::cerr << "GZ Header Size : " << GZ_HEADER_SIZE << endl;
    gzfile << std::hex << GZ_HEADER;;

    // Write file content after moving the write pointer ahead
    gzfile.seekp(GZ_HEADER_SIZE, ios::beg);
    gzfile.write (buffer,size);

    //gzfile.flush();
    gzfile.close();

    delete[] buffer;

    return true;
}

void abackup::ListOfDB()
{
    // dbFolder
    std::string path;

    // Iterate over the `std::filesystem::directory_entry` 
    for (const fs::directory_entry& dir_entry : 
        fs::recursive_directory_iterator(dbFolder))
    {
        path = dir_entry.path();
        if (path.ends_with(".db"))
            std::cout << path << '\n';
    }

}


#endif