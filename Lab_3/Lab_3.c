#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<alloca.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<libgen.h>
#include<limits.h>
#include<errno.h>
#include<unistd.h>
#include<dirent.h>

#define BUF_SIZE 1024*1024

char *AppName;
sig_atomic_t chldcnt = 0;
sig_atomic_t mxchldcnt = 0;

int ProcFile (char *path)
{
    int file = open(path, O_RDONLY);

    if (file == -1)
    {
        fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), path);

        exit(-1);
    }

    char flag = -1;
    unsigned w = 0, b = 0;
    char buf[BUF_SIZE];
    int cnt = 0;

    while ((cnt = read(file, &buf, sizeof(BUF_SIZE))) != 0)
    {
        if (cnt == -1)
        {
            fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), path);

            exit(-1);
        }

        for (int i = 0; i < cnt; i++)
        {
            b++;

            switch (buf[i])
            {
                case ' ':
                case '\n':
                case '\t':
                    flag = -1;
                    break;
                default:
                    if(flag==-1){ flag = 1; w++; }
                    break;
            }
        }
    }

    printf("%d %s %u %u\n", (int)getpid(), path, b, w);

    if (close(file) == -1)
    {
        fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), path);

        exit(-1);
    }

    exit(0);
}

void Task (char *curPath)
{
    DIR *curDir;

    if ((curDir = opendir(curPath)) == NULL) 
    {
        fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), curPath);
        errno = 0;

        return;
    }

    struct dirent *dent;
    char *file = alloca(strlen(curPath) + NAME_MAX + 2);

    if (file == NULL)
    {
        fprintf(stderr, "%s: %s.", AppName, strerror(errno));

        return;
    }

    errno = 0;

    while (dent = readdir(curDir))
    {
        if (strcmp(".", dent->d_name) && strcmp("..", dent->d_name))
        {
            strcpy(file, curPath);

            if (strcmp(file,"/")!=0)
            {
                strcat(file, "/");
            }

            strcat(file, dent->d_name);
            struct stat buf;

            if (lstat(file,&buf) == -1) 
            {
                fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), curPath);

                return;
            }

            if (S_ISDIR(buf.st_mode))
            {
                Task(file);
            }

            int c;

            if (S_ISREG(buf.st_mode))
            {
                if (chldcnt==mxchldcnt) 
                {
                    wait(&c);
                    chldcnt--;
                }

                switch (fork())
                {
                    case (pid_t)-1:
                        fprintf(stderr, "%s: %s.\n", AppName, strerror(errno));
                        errno = 0;
                        break;
                    case (pid_t)0:
                        ProcFile(file);
                        break;
                    default:
                        chldcnt++;
                        break;
                }
            }
        }
    }

    if (errno != 0)
    {
        fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), curPath);
        errno = 0;

        return;
    }

    if (closedir(curDir) == -1)
    {
        fprintf(stderr, "%s: %s. File: %s\n", AppName, strerror(errno), curPath);
        errno = 0;

        return;
    }

    return;
}

int main(int argc, char**argv)
{
    AppName = alloca(strlen(basename(argv[0])));
    strcpy(AppName, basename(argv[0]));

    if (argc < 2)
    {
        fprintf(stderr, "%s: Too few arguments\n", AppName);

        return -1;
    }

    mxchldcnt = atoi(argv[2]);

    if (mxchldcnt <= 0)
    {
        fprintf(stderr, "%s: Second argument is wrong\n", AppName);

        return -1;
    }

    char *res;

    if ((res = realpath(argv[1], NULL)) == NULL) 
    {
        fprintf(stderr, "%s: %s. File: %s.\n", AppName, strerror(errno), argv[1]);
        
        return -1;
    }

    Task(res);

    return 0;
}
