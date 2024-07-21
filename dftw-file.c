
// Defines a feature test macro to expose certain functions in the headers. 
// This macro ensures the inclusion of functions and features that comply with the X/Open standard.
#include<stdio.h> //Includes the standard input/output library functions.
#include<ftw.h> // Includes the File Tree Walk (FTW) library, which provides functions to traverse directories.
#include<stdlib.h> //Includes standard library functions such as memory allocation and system commands.
#include<string.h> //Includes string handling functions.
#include<stdint.h> // Includes fixed-width integer types.
#include<unistd.h> //Includes POSIX operating system API.
#include<limits.h>
// -------- Link to Video ----------- //
// https://drive.google.com/file/d/12_KqjkowIZm5PZVv0rbmq2vZWbAtg7kX/view?usp=sharing // 

#define _XOPEN_SOURCE 500


// Defining Global Variables..
int filecnt = 0;
int foldercnt = 0;
char *global_fpath_source;
char *global_fpath_dest;
const char *global_fpath_type;

// Function to display info about each file and directory
static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    printf("Printing info: ");
    printf("%-3s %2d ", 
           (tflag == FTW_D) ? "d" : (tflag == FTW_DNR) ? "dnr" :
           (tflag == FTW_DP) ? "dp" : (tflag == FTW_F) ? "f" :
           (tflag == FTW_NS) ? "ns" : (tflag == FTW_SL) ? "sl" :
           (tflag == FTW_SLN) ? "sln" : "???",
           ftwbuf->level);

    if (tflag == FTW_NS)
        printf("-------");
    else
        printf("%7jd", (intmax_t) sb->st_size);

    printf("   %-40s %d %s\n", fpath, ftwbuf->base, fpath + ftwbuf->base);
    return 0;
}

// Function to check if a file has a specific extension
int has_extension(const char *filename, const char *ext) {
    const char *dot = strrchr(filename, '.'); // Locate last occurrence of character in string
    if (!dot || dot == filename) return 0; // 2 Case: the filename does not have an extension : hidden file 
    return strcmp(dot, ext) == 0;
}

// Function to construct a new path for the destination
void construct_new_path(const char *src_path, const char *src_base, const char *dst_base, char *new_dst_path) {
    // Calculating the relative path of the source file by removing the base source path from the full source path.
    const char *relative_path = src_path + strlen(src_base); 

    // Concatenating this relative path with the base destination path to form the full destination path.
    snprintf(new_dst_path, 4096, "%s%s", dst_base, relative_path);
}

// Function to handle file copying with nftw
static int cpx_internal(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    char new_dst_path[4096];
    construct_new_path(fpath, global_fpath_source, global_fpath_dest, new_dst_path);
   // printf("New destination path for '%s' is '%s'\n", fpath, new_dst_path);

    if (tflag == FTW_F) {
        if (global_fpath_type && has_extension(fpath, global_fpath_type)) {
            // Skip files with the specified extension
            return 0;
        }
        char cmdLine2[4096];
        snprintf(cmdLine2, sizeof(cmdLine2), "cp %s %s", fpath, new_dst_path);
        //printf("Copying file: %s -> %s\n", fpath, new_dst_path);
        system(cmdLine2); // running the copy command 
    } else if (tflag == FTW_D) {
        char cmdLine3[4096];
        snprintf(cmdLine3, sizeof(cmdLine3), "mkdir -p %s", new_dst_path);
        //printf("Creating directory: %s\n", new_dst_path);
        system(cmdLine3); // running the mkdir command
    } else {
        printf("Skipping: %s\n", fpath);
    }
    return 0;
}

// Function to initiate the copying process
void cpx_function(const char *fpath_source, const char *fpath_dest, const char *fpath_type) {
    if (strcmp(fpath_type, "any") == 0) {
        char cmdLine[4096];
        snprintf(cmdLine, sizeof(cmdLine), "cp -r %s %s", fpath_source, fpath_dest);
        system(cmdLine);
    } else {
        global_fpath_type = fpath_type;
        //printf("Starting specific copy operations...\n");
        if (nftw(fpath_source, cpx_internal, 20, FTW_PHYS) == -1) {
            perror("nftw");
            printf("Error occurred while traversing the file tree.\n");
            exit(EXIT_FAILURE);
        }
   }
}


// Function to Move one file to another
static int mv_internal(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
	char cmdLine2[4096];
	if(tflag == FTW_D | tflag == FTW_DP){
		snprintf(cmdLine2,sizeof(cmdLine2), "rmdir %s",fpath);
		system(cmdLine2);
	}else{
		snprintf(cmdLine2, sizeof(cmdLine2), "rm %s",fpath);
		system(cmdLine2);
	}
	return 0;
}
void mv_function(const char *fpath_source,const char *fpath_dest){
	cpx_function(fpath_source,fpath_dest,"any");	
	if (nftw(fpath_source, mv_internal, 20,FTW_DEPTH) == -1) {
     		perror("nftw");
              	printf("Error occurred while traversing the file tree.\n");
              	exit(EXIT_FAILURE);
        }
}

// Function to count the total number of files
static int display_count(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_F) {
        filecnt++;
    }
    return 0;
}

void count_total_number_of_file(const char *fpath) {
    if (nftw(fpath, display_count, 20, 0) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    printf("Total number of files: %d\n", filecnt);
}

// Function to count the total number of directories
static int display_folder_count(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_D) {
        foldercnt++;
    }
    return 0;
}

void count_total_number_of_folder(const char *fpath) {
    if (nftw(fpath, display_folder_count, 20, 0) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    printf("Total number of directories: %d\n", --foldercnt);
}

// Function to display the size of each file
static int display_file_size(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_F) {
        printf("%-40s - %7jd bytes\n", fpath, (intmax_t) sb->st_size);
    }
    return 0;
}

void list_size_of_file_in_bytes(const char *fpath) {
    if (nftw(fpath, display_file_size, 20, 0) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
}

// Function to copy the whole directory, with optional file type exclusion
void copy_whole(const char *fpath_source, const char *fpath_dest, const char *fpath_type) {
    if (strcmp(fpath_type, "any") == 0) {
        char cmdLine[4096];
        snprintf(cmdLine, sizeof(cmdLine), "cp -r %s %s", fpath_source, fpath_dest);
        system(cmdLine);
    } else {
        char cmdLine2[4096];
        snprintf(cmdLine2, sizeof(cmdLine2), "rsync -a --prune-empty-dirs --include '*/' --exclude '*.%s' --include '*' %s %s", fpath_type, fpath_source, fpath_dest);
        system(cmdLine2);
    }
}

// Function to move the whole directory (copy and delete source)
void move_whole(const char *fpath_source, const char *fpath_dest) {
    copy_whole(fpath_source, fpath_dest, "any");
    char cmdLine3[4096];
    snprintf(cmdLine3, sizeof(cmdLine3), "rm -r %s", fpath_source);
    system(cmdLine3);
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "-nf") == 0) {
        count_total_number_of_file((argv[2] == NULL) ? "/home/patel4p9/" : argv[2]);
    } else if (argc >= 2 && strcmp(argv[1], "-nd") == 0) {
        count_total_number_of_folder((argv[2] == NULL) ? "/home/patel4p9/" : argv[2]);
    } else if (argc >= 2 && strcmp(argv[1], "-sf") == 0) {
        list_size_of_file_in_bytes((argv[2] == NULL) ? "/home/patel4p9/" : argv[2]);
    } else if (argc >= 4 && strcmp(argv[1], "-cpx") == 0) {
        if (argv[2] != NULL && argv[3] != NULL) {
            global_fpath_source = argv[2];
            global_fpath_dest = argv[3];
            //printf("Global Source: %s Dest: %s set to\n", global_fpath_source, global_fpath_dest);
            cpx_function(argv[2], argv[3], (argv[4] == NULL ? "any" : argv[4]));
        } else {
            printf("Invalid arguments for -cpx. Source and destination paths are required.\n");
            exit(EXIT_FAILURE);
        }
    } else if (argc >= 3 && strcmp(argv[1], "-mv") == 0) {
	if (argv[2] != NULL && argv[3] != NULL) {
        	//move_whole( argv[2], argv[3]);
		mv_function(argv[2],argv[3]);
        } else {
            printf("Invalid arguments for -mv. Source and destination paths are required.\n");
            exit(EXIT_FAILURE);
	}
    } else if (argc >= 3 && strcmp(argv[1], "-cp") == 0) {
        copy_whole((argc < 2) ? "." : argv[2], (argc < 3) ? "/mnt/d" : argv[3], (argv[4] == NULL) ? "any" : argv[4]);
    } else {
        if (nftw((argc < 2) ? "." : argv[2], display_info, 20, 0) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}


