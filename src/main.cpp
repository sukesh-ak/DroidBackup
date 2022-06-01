/*
      SIMPLE CLI TEMPLATE
      By Sukesh Ashok Kumar

      using https://github.com/jarro2783/cxxopts/ for command line parsing.
*/
#include <iostream>
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
      cxxopts::Options options("droidbackup", "Convert Android Backup to GZ format");

      options.add_options()
            ("p,password", "Encryption Password", cxxopts::value<string>())
            ("f,filename", "Backup Filename", cxxopts::value<std::string>(), "FILE")
            ("h,help", "Print usage");

      auto result = options.parse(argc, argv);

      if (result.count("help"))
      {
            cout << options.help() << endl;
            exit(0);
      }

      string password;
      if (result.count("p"))
      {
            password = result["password"].as<string>();
            cout << "Password: " << password << endl;
      }

      string filename;
      if (result.count("f"))
      {
            filename= result["filename"].as<std::string>();
            cout << "Filename: " << filename << endl;
      }

   }
   catch (const cxxopts::OptionException& e)
   {
      std::cout << "error parsing options: " << e.what() << endl;
      exit(1);
   }
}

int main(int argc, const char* argv[])
{
      

      cout << "Android Backup" << endl;
      cout << "Version: " << DroidBackupVersion << endl << endl;

      
      parse(argc, argv);

  return 0;
}