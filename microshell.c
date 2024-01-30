
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int g_fd;

static int ft_print(char *str)
{
    int i = 0;

    while (str[i])
        i++;
    write(2, str, i);
    return (1);
}

static int  pipe_exec(char **argv, int i, char **env)
{
    int status;
    int fd[2];
    int pid;
    int next = 0;

    if (argv[i] && strcmp(argv[i], "|") == 0)
        next = 1;
    if (argv[i] == *argv)
        return (0);
    if (pipe(fd) == -1)
        return(ft_print("error: fatal\n"));
    pid = fork();
    if (pid == -1)
        return (ft_print("error: fatal\n"));
    else if (pid == 0)
    {
        close(fd[0]);
        dup2(g_fd, 0);
        argv[i] = 0;
        if (next)
            dup2(fd[1], 1);
        if (g_fd != 0)
            close (g_fd);
        close(fd[1]);
        if (execve(*argv, argv, env) == -1)
        {
            ft_print("error: cannot execute ");
            ft_print(*argv);
            ft_print("\n");
            exit(0);
        }
    }
    else
    {
        close (fd[1]);
        waitpid(pid, &status, 0);
        if (g_fd != 0)
            close(g_fd);
        if (next)
            g_fd = dup(fd[0]);
        close(fd[0]);
    }
    return (0);
}

static int  ft_cd(char **argv)
{
    if (argv[2] && strcmp(argv[2], "|") != 0 && strcmp(argv[2], ";") != 0)
        return (ft_print("error: cd: bad arguments\n"));
    if (chdir(argv[1]) == -1)
        return (ft_print("error: cd: cannot change directory to "), ft_print(argv[1]), ft_print("\n"));
    return (0);
}

int main(int argc, char **argv, char **env)
{
    int i = 1;

    if (argc == 1)
        return (1);
    argv[argc] = 0;
    while (argv[i - 1] && argv[i])
    {
        argv = argv + i;
        i = 0;
        while (argv[i] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
            i++;
        if (!strcmp(*argv, "cd"))
            ft_cd(argv);
        else
            pipe_exec(argv, i, env);
        i++;
    }
}
