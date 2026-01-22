# 🗑️ mini-rm

A minimal implementation of the Unix `rm` command written in C for Linux.  
This project recreates core functionality of `rm`, including recursive deletion, empty-directory removal, and verbose output—without relying on the actual GNU `rm`.

## ✨ Features
- Remove regular files
- Remove empty directories (`-d`)
- Recursively delete directories and their contents (`-r`)
- Verbose output (`-v`)
- Command-line flag parsing
- Manual stack-based argument handling (no dynamic allocation)

## 🧠 How It Works
- Command-line arguments are parsed and stored using a custom stack
- Flags are stored using bitmasks for efficient checks
- File types are identified using `stat`
- Recursive deletion walks directories using `opendir`, `readdir`, and `rmdir`
- Uses Linux syscalls like `unlinkat` for file removal

## 🧾 Usage
```bash
./rm [OPTION]... [FILE]...
```

## Supported Options
```bash
--help  Display help information  
-r      Remove directories and their contents recursively  
-d      Remove empty directories  
-v      Print what is being removed  
```
## 📌 Examples

Remove a file:
```bash
./rm file.txt
```
Remove an empty directory:
```bash
./rm -d empty_dir
```

Recursively delete a directory:
```bash
./rm -r my_folder
```

Verbose recursive deletion:
```bash
./rm -r -v project_dir
```

## ⚠️ Notes
- Directories cannot be removed without -r or -d  
- Symbolic links and special files are not fully handled  
- No interactive prompt (-i) is implemented  
- Stack size is limited to 100 paths  
 
- ## 🛠️ Build
```bash
    gcc -Wall -Wextra -o rm rm.c
```

## 📚 Learning Goals
- This project was built to practice:
- Low-level Linux filesystem APIs  
- Recursive directory traversal  
- Bitwise flag management  
- Stack-based argument handling  
- Writing safe and readable C code  

## 📄 License
```
Educational / personal use only.  
Not intended as a replacement for GNU rm.
```
