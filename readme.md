# dftw-file-tree-utility

## Video to Explaination and demo
https://drive.google.com/file/d/12_KqjkowIZm5PZVv0rbmq2vZWbAtg7kX/view?usp=sharing  


## Overview
This repository contains a C program named `dftw` which uses the `nftw()` system call to perform various file tree traversal operations. This program is developed as part of Assignment 01 for the COMP-8567 course in Summer 2024.

## Features
- **Count Files:** List the count of all files in the subtree rooted at a specified directory.
- **Count Directories:** List the count of all directories in the subtree rooted at a specified directory.
- **Size of Files:** List the total size of all files in bytes in the subtree rooted at a specified directory.
- **Copy Subdirectory:** Copy a subdirectory to another location, excluding files of a specified extension.
- **Move Subdirectory:** Move a subdirectory to another location, deleting the original.

## Command Line Arguments
- `dftw -nf [root_dir]`: Lists the count of all files present in the entire subtree rooted at `root_dir`.
- `dftw -nd [root_dir]`: Lists the count of all directories present in the entire subtree rooted at `root_dir`.
- `dftw -sf [root_dir]`: Lists the size of all files (in bytes) present in the entire subtree rooted at `root_dir`.
- `dftw -cpx [source_dir] [destination_dir] [file extension]`: Copies the entire subdirectory rooted at `source_dir` to `destination_dir`, excluding files of the specified extension.
- `dftw -mv [source_dir] [destination_dir]`: Moves the entire subdirectory rooted at `source_dir` to `destination_dir`, deleting the original.

## Usage Examples
- Count files: `$ dftw -nf /home/user/documents`
- Count directories: `$ dftw -nd /home/user/documents`
- List file sizes: `$ dftw -sf /home/user/documents`
- Copy subdirectory excluding `.txt` files: `$ dftw -cpx /home/user/source /home/user/destination .txt`
- Move subdirectory: `$ dftw -mv /home/user/source /home/user/destination`

## Mandatory Requirements
The program makes use of the `nftw()` system call to traverse the file tree. The `nftw()` function recursively visits all files/directories and calls a user-defined function.

## Author
Meet Patel
