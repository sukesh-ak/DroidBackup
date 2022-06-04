# DROID BACKUP
***Convert Android Backup to GZ format. Also Decrypts if encryption exist.***


### Compile and Build using CMAKE
```
git clone https://github.com/sukesh-ak/DroidBackup
cd DroidBackup

# After git clone update the submodule using following command
git submodule update --init --recursive

# Build bindary 'droidbackup'
sudo apt update && sudo apt install cmake
mkdir build && cd build
cmake .. && cmake -build .
```

### Usage
```
+-+-+-+-+-+ +-+-+-+-+-+-+
|D|R|O|I|D| |B|A|C|K|U|P|
+-+-+-+-+-+ +-+-+-+-+-+-+
Version: 0.0.1

Convert Android Backup to GZ format
Usage:
  ./bin/droidbackup [OPTION...]

  -f, --filename FILE      Backup Filename (default: backup.ab)
  -p, --password PASSWORD  Encryption Password
  -l, --list FOLDER        List .db files
  -h, --help               Print usage
```