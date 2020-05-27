#include <libgen.h>
#include <zconf.h>
#include <stdlib.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

long getTime()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_usec % 1000;
}

const char* AppName = NULL;

int getPid(const char* appName, const char* fileName)
{
    struct stat fileInfo;
    int returnCode = lstat(fileName, &fileInfo);

    if (returnCode == EOF)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), AppName, strerror(errno));

        return -1;
    }

    int fileSize = (int)fileInfo.st_size + 1;

    FILE* file = fopen(fileName, "r");

    if (file == NULL)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), AppName, strerror(errno));

        return -1;
    }

    char* pointer;
    char pidString[fileSize];

    fgets(pidString, fileSize, file);
    long pid = strtol(pidString, &pointer, 10);

    if (pid == 0 || pid == LONG_MIN || pid == LONG_MAX)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), AppName, strerror(errno));

        return -1;
    }

    fclose(file);

    return (int)pid;
}

int ProcNum = -1;
int NumReceivedSignals = 0;
int NumSendSignals = 0;

void sendSIGUSR(int processNumber, int groupId)
{
    fprintf(stdout, "%d %d %d послал SIGUSR1 %ld\n", ProcNum, getpid(), getppid(), getTime());
    fflush(stdout);
    kill(-groupId, SIGUSR1);
    NumSendSignals++;
}

void receiveSIGUSR()
{
    fprintf(stdout, "%d %d %d получил SIGUSR1 %ld\n", ProcNum, getpid(), getppid(), getTime());
    fflush(stdout);

    NumReceivedSignals++;
}

void receiveSIGTERM()
{
    fprintf(stdout, "%d %d завершил работу после %d-го сигнала SIGUSR1 и 0-го сигнала SIGUSR2\n", getpid(), getppid(), NumSendSignals);
    fflush(stdout);
}

void firstHandlerSIGUSR()
{
    receiveSIGUSR();

    int secondProcPid = getPid(AppName, "SecondProcPid.txt");
    int thirdProcPid = getPid(AppName, "ThirdProcPid.txt");

    if (NumReceivedSignals == 101)
    {
        kill(secondProcPid, SIGTERM);
        kill(thirdProcPid, SIGTERM);
        waitpid(secondProcPid, 0, 0);
        waitpid(thirdProcPid, 0, 0);

        exit(0);
    }

    sendSIGUSR(ProcNum, secondProcPid);
}

void secondHandlerSIGUSR()
{
    receiveSIGUSR();
    int fourthProcPid = getPid(AppName, "FourthProcPid.txt");

    sendSIGUSR(ProcNum, fourthProcPid);
}

void thirdHandlerSIGUSR()
{
    receiveSIGUSR();
}

void fourthHandlerSIGUSR()
{
    receiveSIGUSR();
}

void fifthHandlerSIGUSR()
{
    receiveSIGUSR();
    int sixthProcPid = getPid(AppName, "SixthProcPid.txt");

    sendSIGUSR(ProcNum, sixthProcPid);
}

void sixthHandlerSIGUSR()
{
    receiveSIGUSR();
    int seventhProcPid = getPid(AppName, "SeventhProcPid.txt");

    sendSIGUSR(ProcNum, seventhProcPid);
}

void seventhHandlerSIGUSR()
{
    receiveSIGUSR();
}

void eighthHandlerSIGUSR()
{
    receiveSIGUSR();
    int firstProcPid = getPid(AppName, "FirstProcPid.txt");
    sendSIGUSR(ProcNum, firstProcPid);
}

void savePid(const char* appName, const char* fileName, int pid)
{
    FILE* file = fopen(fileName, "w");

    if (file == NULL)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));

        return;
    }

    fprintf(file, "%d", pid);
    fclose(file);
}

void firstHandlerSIGTERM()
{
    receiveSIGTERM();

    exit(0);
}

void secondHandlerSIGTERM()
{
    receiveSIGTERM();

    exit(0);
}

void thirdHandlerSIGTERM()
{
    receiveSIGTERM();
    int fourthProcPid = getPid(AppName, "FourthProcPid.txt");
    kill(fourthProcPid, SIGTERM);
    waitpid(fourthProcPid, 0, 0);

    exit(0);
}

void fourthHandlerSIGTERM()
{
    receiveSIGTERM();
    int fifthProcPid = getPid(AppName, "FifthProcPid.txt");
    int sixthProcPid = getPid(AppName, "SixthProcPid.txt");
    int seventhProcPid = getPid(AppName, "SeventhProcPid.txt");
    kill(fifthProcPid, SIGTERM);
    kill(sixthProcPid, SIGTERM);
    kill(seventhProcPid, SIGTERM);
    waitpid(fifthProcPid, 0, 0);
    waitpid(sixthProcPid, 0, 0);
    waitpid(seventhProcPid, 0, 0);

    exit(0);
}

void fifthHandlerSIGTERM()
{
    receiveSIGTERM();

    exit(0);
}

void sixthHandlerSIGTERM()
{
    receiveSIGTERM();

    exit(0);
}

void seventhHandlerSIGTERM()
{
    receiveSIGTERM();
    int eighthProcPid = getPid(AppName, "EighthProcPid.txt");
    kill(eighthProcPid, SIGTERM);
    waitpid(eighthProcPid, 0, 0);

    exit(0);
}

void eighthHandlerSIGTERM()
{
    receiveSIGTERM();

    exit(0);
}

void initFirstHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = firstHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initSecondHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = secondHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initThirdHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = thirdHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initFourthHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = fourthHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initFifthHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = fifthHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initSixthHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = sixthHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initSeventhHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = seventhHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initEighthHandlerOfSIGTERM()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = eighthHandlerSIGTERM;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    action.sa_mask = set;
    sigaction(SIGTERM, &action, 0);
}

void initFirstHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = firstHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initSecondHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = secondHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initThirdHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = thirdHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initFourthHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = fourthHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initFifthHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = fifthHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initSixthHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = sixthHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initSeventhHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = seventhHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void initEighthHandlerOfSIGUSR(int procNum)
{
    ProcNum = procNum;
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = eighthHandlerSIGUSR;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    action.sa_mask = set;
    sigaction(SIGUSR1, &action, 0);
}

void createFifthProc(const char* appName)
{
    AppName = appName;
    savePid(AppName, "FifthProcPid.txt", getpid());
    initFifthHandlerOfSIGUSR(5);
    initFifthHandlerOfSIGTERM();

    struct stat fileInfo;
    while (lstat("FourthProcPid.txt", &fileInfo) == EOF) {}
    int fourthProcessPid = getPid(AppName, "FourthProcPid.txt");

    int returnCode = setpgid(getpid(), fourthProcessPid);
    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), AppName, strerror(errno));
    }

    while (1)
    {
        pause();
    }
}

void createSixthProc(const char* appName)
{
    AppName = appName;
    savePid(appName, "SixthProcPid.txt", getpid());
    initSixthHandlerOfSIGUSR(6);
    initSixthHandlerOfSIGTERM();

    int returnCode = setpgid(getpid(), getpid());

    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    while (1)
    {
        pause();
    }
}

void createEighthProc(const char* appName)
{
    struct stat fileInfo;
    while (lstat("SeventhProcPid.txt", &fileInfo) == EOF) {}
    int seventhProcessPid = getPid(appName, "SeventhProcPid.txt");

    int returnCode = setpgid(getpid(), seventhProcessPid);
    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    int mainProcessPid = getPid(appName, "ZeroProcPid.txt");

    char psTreeCommand[50] = { 0 };
    sprintf(psTreeCommand, "pstree -p %d", mainProcessPid);
    system(psTreeCommand);

    AppName = appName;
    initEighthHandlerOfSIGUSR(8);
    initEighthHandlerOfSIGTERM();
    savePid(appName, "EighthProcPid.txt", getpid());

    while (1)
    {
        pause();
    }
}

void createSeventhProc(const char* appName)
{
    AppName = appName;
    savePid(appName, "SeventhProcPid.txt", getpid());
    initSeventhHandlerOfSIGUSR(7);
    initSeventhHandlerOfSIGTERM();

    int returnCode = setpgid(getpid(), getpid());

    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    int eighthProcessPid = fork();

    if (eighthProcessPid == 0)
    {
        createEighthProc(appName);
    }
    else
    {
        while (1)
        {
            pause();
        }
    }
}

void createFourthProc(const char* appName)
{
    AppName = appName;
    savePid(appName, "FourthProcPid.txt", getpid());
    initFourthHandlerOfSIGUSR(4);
    initFourthHandlerOfSIGTERM();

    int returnCode = setpgid(getpid(), getpid());

    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    int fifthProcessPid = fork();

    if (fifthProcessPid == 0)
    {
        createFifthProc(appName);
    }
    else
    {
        int sixthProcessPid = fork();

        if (sixthProcessPid == 0)
        {
            createSixthProc(appName);
        }
        else
        {
            int sevenProcessPid = fork();

            if (sevenProcessPid == 0)
            {
                createSeventhProc(appName);
            }
            else
            {
                while (1)
                {
                    pause();
                }
            }
        }
    }
}

void createThirdProc(const char* appName)
{
    AppName = appName;
    savePid(appName, "ThirdProcPid.txt", getpid());
    initThirdHandlerOfSIGUSR(3);
    initThirdHandlerOfSIGTERM();

    struct stat fileInfo;
    while (lstat("SecondProcPid.txt", &fileInfo) == EOF) {}
    int secondProcessPid = getPid(appName, "SecondProcPid.txt");
    int returnCode = setpgid(getpid(), secondProcessPid);

    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    int fourthProcessPid = fork();

    if (fourthProcessPid == 0)
    {
        createFourthProc(appName);
    }
    else
    {
        while (1)
        {
            pause();
        }
    }
}

void createSecondProc(const char* appName)
{
    AppName = appName;
    savePid(appName, "SecondProcPid.txt", getpid());
    initSecondHandlerOfSIGUSR(2);
    initSecondHandlerOfSIGTERM();

    int returnCode = setpgid(getpid(), getpid());

    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    while (1)
    {
        pause();
    }
}

void createFirstProc(const char* appName)
{
    AppName = appName;
    savePid(appName, "FirstProcPid.txt", getpid());
    initFirstHandlerOfSIGUSR(1);

    int returnCode = setpgid(getpid(), getpid());

    if (returnCode == -1)
    {
        fprintf(stderr, "%d: %s: %s\n", getpid(), appName, strerror(errno));
    }

    int secondProcessPid = fork();

    if (secondProcessPid == 0)
    {
        createSecondProc(appName);
    }
    else
    {
        int thirdProcessPid = fork();

        if (thirdProcessPid == 0)
        {
            createThirdProc(appName);
        }
        else
        {
            struct stat fileInfo;
            while (lstat("EighthProcPid.txt", &fileInfo) == EOF) {}

            sendSIGUSR(1, secondProcessPid);
            NumSendSignals++;

            while (1)
            {
                pause();
            }
        }
    }
}

int main(int argc, char* argv[])
{
    char* appName = basename(argv[0]);
    if (appName == NULL)
    {
        fprintf(stderr, "%d: Error with executable file \"%s\"", getpid(), argv[0]);
        return 1;
    }

    savePid(appName, "ZeroProcPid.txt", getpid());

    int firstProcessPid = fork();

    if (firstProcessPid == 0)
    {
        createFirstProc(appName);
    }
    else
    {
        waitpid(firstProcessPid, 0, 0);
    }

    return 0;
}
