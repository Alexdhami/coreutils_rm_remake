#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_STACK_SIZE 100

#define FLAG_HELP      (uint8_t)(1u << 0)
#define FLAG_VERBOSE   (uint8_t)(1u << 1)
#define FLAG_EMPTY_DIR (uint8_t)(1u << 2)
#define FLAG_RECURSIVE (uint8_t)(1u << 3)

typedef struct {
    int arr[MAX_STACK_SIZE]; // stores our indices of argv[filepaths]
    int top;  // to access the top element from the stack
}Stack;


void initilize_stack(Stack *stack){
    // initilize the stack top index with -1
    stack->top = -1;
}

void push(Stack *stack, int index){
    if (stack->top >= MAX_STACK_SIZE){
        return;
    }
    // pre increment the top of the stack
    stack->arr[++stack->top] = index;
}

int pop(Stack *stack){
    if (stack->top <= -1){
        return -1;
    }

    // post decrement the top of the scack
    int value = stack->arr[stack->top--];
    return value;
}

void set_stack_indices_and_flags(Stack *stack, int argc, char** argv, uint8_t* flags){
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--help") == 0){
            *flags |= FLAG_HELP;
            }
        else if(strcmp(argv[i], "-d") == 0){
            *flags |= FLAG_EMPTY_DIR;
            }
        else if(strcmp(argv[i], "-v") == 0){
            *flags |= FLAG_VERBOSE;
            }
        else if(strcmp(argv[i], "-r") == 0){
            *flags |= FLAG_RECURSIVE;
            }
        else{
            push(stack,  i);
            }
        }
}
void show_usage(){
    printf("usage: rm [-FLAGS] [filename]\n");
}

void show_help(){
    printf("Usage: rm [OPTION]... [FILE]...\nRemove (unlink) the FILE(s).\n\n -i                    prompt before every removal\n -r, -R, --recursive   remove directories and their contents recursively\n -d, --dir             remove empty directories\n -v, --verbose         explain what is being done\n");
}


void empty_dir_del(const char* path, const uint8_t* flags){
   if (rmdir(path) == 0){
        if (*flags & FLAG_VERBOSE){
            printf("removed directory '%s'\n",path);
        }
   }
}

void delete_file (const char* path, const uint8_t* flags){
    struct stat buf_stat;

    if (stat(path,&buf_stat) != 0){
        perror(path);
        return;
    }

    if (S_ISREG(buf_stat.st_mode)){
        int returned_val = unlinkat(-100,path,0); 

        if((returned_val == 0) && (*flags & FLAG_VERBOSE)){
            printf("removed '%s'\n", path);
            return;
        }

        else if (returned_val != 0){
            perror(path);
            return;
        }
    }

    else if(S_ISDIR(buf_stat.st_mode)){
        printf("rm: cannot remove '%s': Is a directory\n", path);
        return;
    }
    else{
        printf("%s is something else maybe ln\n",path);
        return;
    }
}
        
int recursive_del(const char* path, const uint8_t* flags){
    struct stat stat_buf;

    if (stat(path, &stat_buf) == -1){
        perror(path);
        return -1;
    }
    
    // case 1: reg file
    if (!S_ISDIR(stat_buf.st_mode)){
        if(unlinkat(-100, path, 0) != 0){
            printf("here\n");
            perror(path);
            return -1;
        }
        else{
            if (*flags & FLAG_VERBOSE){
                printf("removing '%s'\n",path);
            }
        return 0;
        }
    }

    DIR *dir = opendir(path);
    if (!dir){
        perror(path);
        return -1;
    }

    struct dirent *entry;

    while((entry = readdir(dir)) != NULL){
        // skip "." and ".."
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){
            continue;
        }


        char fullpath[PATH_MAX];

        // concatinates the paths 
        snprintf(fullpath, sizeof(fullpath),"%s/%s",path, entry->d_name);

        // recursive call
        if (recursive_del(fullpath, flags) == -1){
            // close the dir for every failure in recursive call
            closedir(dir);
            return -1;
        }
    }

    // closedir anyway
    closedir(dir);

    // case 2: empty dir
    if (rmdir(path) == -1 ){
        perror(path);
        return -1;
    }
    else{
        printf("removed directory '%s'\n",path);
    }
    return 0;
}

void cmd_rm(Stack *stack, char** argv, const uint8_t* flags){
    if(*flags & FLAG_HELP){
        show_help();
        return;
    }

    else if (*flags & FLAG_RECURSIVE){
        while(stack->top >= 0){
            char* path = argv[stack->arr[stack->top--]];
            recursive_del(path,flags);
        }
    }

    else if (*flags & FLAG_EMPTY_DIR){
        while(stack->top >= 0){
            char* path = argv[stack->arr[stack->top--]];
            empty_dir_del(path,flags);
        }
    }

    else if (!(*flags & FLAG_EMPTY_DIR) && !(*flags & FLAG_RECURSIVE)){
        while(stack->top >= 0){
            char* path = argv[stack->arr[stack->top--]];
            delete_file(path, flags);
        }
    }
}

int main(int argc, char** argv){
    if (argc < 2){
        show_usage();
        return 1;
    }

    uint8_t flags = 0;
    Stack stack;

    initilize_stack(&stack);

    set_stack_indices_and_flags(&stack, argc, argv, &flags);

    cmd_rm(&stack, argv, &flags);

    return 0;
}

