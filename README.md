# DROID BACKUP

## Description
- Converts `backup.ab` to `backup.ab.gz`, use `./droidbackup backup.ab` which creates `backup.ab.gz`   
You can extract using `tar -xvzf backup.ab.gz`
- List all folders with db (Sqlite) files from the apps folder   
You can use `./droidbackup -l app/`


### TASKS
- [x] Convert Android Backup to GZ format. 
- [x] List Sqlite .db files from the backup folder.
- [ ] Decrypts if encrypted using password.


## Compile and Build using CMAKE
```bash
# On Linux - install C++ compiler (g++) / cmake / gdb
sudo apt update && sudo apt install build-essential gdb  
sudo apt update && sudo apt install cmake

# Clone the project
git clone https://github.com/sukesh-ak/DroidBackup
cd DroidBackup

# Build binary 'droidbackup'
mkdir build && cd build
cmake .. && cmake -build .
```

## Usage
```bash
# Application help output
+-+-+-+-+-+ +-+-+-+-+-+-+
|D|R|O|I|D| |B|A|C|K|U|P|
+-+-+-+-+-+ +-+-+-+-+-+-+
Version: 0.0.1

Convert Android Backup to GZ format
Usage:
  ./droidbackup [OPTION...]

  -f, --filename FILE      Backup Filename (default: backup.ab)
  -p, --password PASSWORD  Encryption Password
  -l, --list FOLDER        List .db files
  -h, --help               Print usage
```

## How to take Android device Backup
Download Android Platform Tools from [here](https://developer.android.com/studio/releases/platform-tools) which has Android Debug Bridge - `adb`
```bash
# Backup everything to file called backup.ab
./adb backup -apk -obb -shared -all -f backup.ab

# Restore can be done like below
./adb restore backup.ab

```