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
            ("f,filename", "Backup Filename", cxxopts::value<std::string>(),"FILE")
            ("p,password", "Encryption Password", cxxopts::value<string>(),"PASSWORD")
            ("h,help", "Print usage");

      auto result = options.parse(argc, argv);
      if (result.count("help"))
      {
            cout << options.help() << endl;
            exit(0);
      }

      string password;
      if (result.count("password"))
      {
            password = result["password"].as<string>();
            cout << "Password: " << password << endl;
      }

      if (result.count("filename"))
      {
            bfilename= result["filename"].as<std::string>();
            cout << "Filename: " << bfilename << endl;
      }
   }
   //cxxopts::OptionSpecException, cxxopts::OptionParseException
   catch (const cxxopts::OptionException& e)
   {
      std::cout << RED << "error parsing options: " << e.what() << RESET << endl;
      exit(1);
   }
}

int main(int argc, const char* argv[])
{
      cout << "DROID BACKUP" << endl;
      cout << "Convert Android Backup to GZ format" << endl;
      cout << "Version: " << DroidBackupVersion << endl << endl;

      parse(argc, argv);

      // Time to open the file
      ifstream config_file(bfilename,ios::in);

      if (!config_file.is_open())
      {
            std:cerr << "Could not open backup file: " << bfilename << endl;
            return EXIT_FAILURE;
      }

      // Read and process each line in the file
      std::string line;
      int linecnt = 1;

      bool IsBackup = false;
      int version = 0;
      bool IsCompressed = 0;
      bool IsEncrypted = false;
      string encryption = "none";

      // Backup Headers
      while(std::getline(config_file,line))
      {
            // * ANDROID BACKUP
            // * 5=version
            // * 1=compression
            // * AES-256=encryption
            line = split(line,'\n')[0];

            if ((linecnt == 1) && line.compare("ANDROID BACKUP")==0) IsBackup = true;
            if (linecnt == 2) version = stoi(line);
            if (linecnt == 3) IsCompressed = (stoi(line)==1)?true:false;
            if (linecnt == 4) IsEncrypted = line.compare("none")?false:true;
            if ((linecnt == 4) && IsEncrypted) encryption = line;

            if (linecnt>4) break;
            linecnt++;
      }
      config_file.close();

      cout << "Valid Backup : " << (IsBackup?"Yes":"No") << endl;
      cout << "Version : " << version << endl;
      cout << "Compressed : " << (IsCompressed?"Yes":"No") << endl;
      cout << "Encryption : " << encryption << endl;

  return 0;
}