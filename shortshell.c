#include "microshell.h"

int print(char *str)
{
    while (*str)
        write(2, str++, 1);
    return (1);
}

int cd(char **argv, int i)
{
    if (i != 2)
        return (print("error: cd: bad arguments\n"));
    else if (chdir(argv[1]) == -1)
        return (print("error: cd: cannot change directory to "), print(argv[1]), print("\n"));
    return (0);
}

int exec(char **argv, char **envp, int i)
{
    int fd[2];
    int status;
    int next;
    int pid;

    next = argv[i] && !strcmp(argv[i], "|");
    if (next && pipe(fd) == -1)
        return (print("error: fatal\n"));
    pid = fork();
    if (!pid)
    {
        argv[i] = 0;
        if (next && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
            return (print("error: fatal\n"));
        execve(*argv, argv, envp);
        return (print("error: cannot execute "), print(*argv), print("\n"));
    }
    waitpid(pid, &status, 0);
    if (next && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
        return (print("error: fatal\n"));
    return (WIFEXITED(status) && WEXITSTATUS(status));
}

int main(int argc, char **argv, char **envp)
{
    int i = 0;
    int status = 0;

    if (argc > 1)
    {
        while (argv[i] && argv[++i])
        {
            argv += i;
            i = 0;
            while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
                i++;
            if (!strcmp(*argv, "cd"))
                status = cd(argv, i);
            else if (i)
                status = exec(argv, envp, i);
        }
    }
    return status;
}