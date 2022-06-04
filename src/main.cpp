/*
      SIMPLE CLI TEMPLATE
      By Sukesh Ashok Kumar

      using https://github.com/jarro2783/cxxopts/ for command line parsing.
*/
#include <iostream>
#include <fstream>
#include <cxxopts.hpp>

#include "version.hpp"

using namespace std;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */

// gz header 31 139 8 0 0 0 0 0
#define GZ_HEADER "\x1f\x8b\x08\x00\x00\x00\x00\x00"
#define GZ_HEADER_SIZE 8

std::string bfilename;

std::vector<std::string> split(const std::string& s, char seperator)
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

void AppInfo()
{
      std::cerr << GREEN;
      // cout << "╔╦╗╦═╗╔═╗╦╔╦╗  ╔╗ ╔═╗╔═╗╦╔═╦ ╦╔═╗" << endl;
      // cout << " ║║╠╦╝║ ║║ ║║  ╠╩╗╠═╣║  ╠╩╗║ ║╠═╝" << endl;
      // cout << "═╩╝╩╚═╚═╝╩ ╩╝  ╚═╝╩ ╩╚═╝╩ ╩╚═╝╩  " << endl;

      std::cerr << "+-+-+-+-+-+ +-+-+-+-+-+-+" << endl;
      std::cerr << "|D|R|O|I|D| |B|A|C|K|U|P|" << endl;
      std::cerr << "+-+-+-+-+-+ +-+-+-+-+-+-+" << endl;
      std::cerr << RESET;

      //cout << GREEN << "DROID BACKUP" << RESET << endl;
      //cout << "Convert Android Backup to GZ format" << endl;
      std::cerr << "Version: " << DroidBackupVersion << endl << endl;
}

/*
* Usage : ./droidbackup -f <backup.ab>
* Usage : ./droidbackup -f <backup.ab> -p password
      * ANDROID BACKUP
      * 5=version
      * 1=compression
      * AES-256=encryption
      * User SALT
      * Checksum SALT
      * PBKDF2 rounds (1000?)
      * User IV
      * Master Key Blob
      * <encrypted content>
*/
void parse(int argc, const char* argv[])
{

   try
   {
     cxxopts::Options options(argv[0], "Convert Android Backup to GZ format");

      options.add_options()
            ("f,filename", "Backup Filename", cxxopts::value<std::string>()->default_value("backup.ab"),"FILE")
            ("p,password", "Encryption Password", cxxopts::value<string>(),"PASSWORD")
            ("h,help", "Print usage");

      auto result = options.parse(argc, argv);

      if (result.count("h"))
      {
            std::cerr << options.help() << endl;
            exit(0);
      }

      string password;
      if (result.count("p"))
      {
            password = result["p"].as<string>();
            std::cerr << "Password: " << password << endl;
      }

      // If filename missing default is assumed
      bfilename= result["f"].as<std::string>();
      std::cerr << "Backup file : " << bfilename << endl;

   }
   //cxxopts::OptionSpecException, cxxopts::OptionParseException
   catch (const cxxopts::OptionException& e)
   {
      std::cerr << RED << "error parsing options: " << e.what() << RESET << endl;
      exit(1);
   }
}

int main(int argc, const char* argv[])
{
      AppInfo();
      parse(argc, argv);


      // Time to open the file
      ifstream backup_file(bfilename,ios::in);

      if (!backup_file.is_open())
      {
            std::cerr << "Could not open backup file: " << bfilename << endl;
            return EXIT_FAILURE;
      }

      // Read and process each line in the file
      std::string line;
      int linecnt = 1;
      int hlength = 0;

      bool IsBackup = false;
      int version = 0;
      bool IsCompressed = 0;
      bool IsEncrypted = false;
      string encryption = "none";

      // Backup header check
      while(std::getline(backup_file,line))
      {
            if (linecnt>4) break;   // interested only in first 4 lines
            // * ANDROID BACKUP
            // * 5=version
            // * 1=compression
            // * none/AES-256=encryption
            // * <content>
            hlength += line.length()+1;   // 1 for \n
            line = split(line,'\n')[0];

            //std::cerr << linecnt << ". Header Length : " << line.length() << endl;

            if ((linecnt == 1) && line.compare("ANDROID BACKUP")==0) IsBackup = true;
            if (linecnt == 2) version = stoi(line);
            if (linecnt == 3) IsCompressed = (stoi(line)==1)?true:false;
            if (linecnt == 4) IsEncrypted = line.compare("none")?false:true;
            if ((linecnt == 4) && IsEncrypted) encryption = line;
            
            linecnt++;
      }
      backup_file.close();
      
      std::cerr << "Header Length : " << hlength << endl;

      std::cerr << "Valid Backup : " << (IsBackup?"Yes":"No") << endl;
      std::cerr << "Version : " << version << endl;
      std::cerr << "Compressed : " << (IsCompressed?"Yes":"No") << endl;
      std::cerr << "Encryption : " << encryption << endl <<endl;
      // *******************************************************************

      if (!IsCompressed)
      {
            std::cerr << "Backup file '" << bfilename << "' is not compressed" << endl;
            return EXIT_FAILURE;
      }

      // Conversion routine
      backup_file.open(bfilename,ios::binary);
      
      // get pointer to associated buffer object
      std::filebuf* pbuf = backup_file.rdbuf();

      // get file size using buffer's members
      std::size_t size = pbuf->pubseekoff (0,backup_file.end);
      size = size - hlength;

      //std::cerr << "Buffer Size : " << size << endl;

      // allocate memory to contain file data
      char* buffer=new char[size];

      backup_file.seekg(hlength, ios::beg);     // set stream cursor to skip headers      
      backup_file.read(buffer,size);

      backup_file.close();

      ofstream gzfile("s.gz");

      // Write GZ header
      //std::cerr << "GZ Header Size : " << GZ_HEADER_SIZE << endl;
      gzfile << std::hex << GZ_HEADER;;

      // Write file content after moving the write pointer ahead
      gzfile.seekp(GZ_HEADER_SIZE, ios::beg);
      gzfile.write (buffer,size);

      //gzfile.flush();
      gzfile.close();

      delete[] buffer;

      std::cerr << endl;

  return 0;
}