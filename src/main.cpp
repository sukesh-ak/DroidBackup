/*
      SIMPLE CLI TEMPLATE
      By Sukesh Ashok Kumar

      using https://github.com/jarro2783/cxxopts/ for command line parsing.
*/
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cxxopts.hpp>

#include "abackup.hpp"
#include "version.hpp"

using namespace std;

namespace fs = std::filesystem;

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

abackup *ab;

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
            ("l,list","List .db files",cxxopts::value<std::string>(),"FOLDER")
            ("h,help", "Print usage");

      auto result = options.parse(argc, argv);

      if (result.count("h"))
      {
            std::cerr << options.help() << endl;
            exit(0);
      }

      if (result.count("p"))
      {
            ab->GzPassword = result["p"].as<string>();
            std::cerr << "Password: " << ab->GzPassword << endl;
      }

      if (result.count("l"))
      {
            ab->dbFolder = result["l"].as<string>();
            std::cerr << "List Folder: " << ab->dbFolder << endl;
      }

      if (result.count("p"))
      {
            ab->GzPassword = result["p"].as<string>();
            std::cerr << "Password: " << ab->GzPassword << endl;
      }

      // If filename missing default is assumed
      ab->BackupFilename = result["f"].as<std::string>();
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
      ab = new abackup();

      parse(argc, argv);

      if (!ab->dbFolder.empty())
      {
            ab->ListOfDB();
            return EXIT_SUCCESS;
      }

      std::cerr << "Backup file : " << ab->BackupFilename << endl;
      if (!fs::exists(ab->BackupFilename))
      {
            std::cerr << "Could not find backup file: " << ab->BackupFilename << endl;
            return EXIT_FAILURE;
      }

      if (!ab->GetMetadata()) // error and unable to read file...
      {
            return EXIT_FAILURE;
      }

      std::cerr << "Valid Backup : " << (ab->IsBackup?"Yes":"No") << endl;
      std::cerr << "Version : " << ab->Version << endl;
      std::cerr << "Compressed : " << (ab->IsCompressed?"Yes":"No") << endl;
      std::cerr << "Encryption : " << ab->encryption << endl <<endl;
      // *******************************************************************

      if (!ab->IsCompressed)
      {
            std::cerr << "Backup file '" << ab->BackupFilename << "' is not compressed" << endl;
            return EXIT_FAILURE;
      }

      std::cerr << "Converting backup file...";
      std::cerr.flush();
      if (ab->ConvertToGz())
            std::cerr << "\rConversion Completed!     " <<endl;
      else
            std::cerr << "\rConversion Failed!        " <<endl;

  return 0;
}