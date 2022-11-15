/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atrilles <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/20 16:06:50 by atrilles          #+#    #+#             */
/*   Updated: 2022/07/20 16:07:00 by atrilles         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int next_line(char ** argv, int i)
{
	while (argv[i] && strcmp(argv[i], ";"))
		i++;
	return i;
}

int next_pipe(char ** argv, int i, int end)
{
	while (argv[i] && i < end && strcmp(argv[i], "|"))
		i++;
	return i;
}

char **arg(char **argv, int start, int end)
{
	int nb = end - start;
	if (!nb)
		return 0;
	char **res = malloc(sizeof(char *) * (nb + 1));
	if (!res)
		return 0;
	int i = 0;
	while (i < nb)
	{
		res[i] = argv[i + start];
		i++;
	}
	res[i] = 0;
	return res;
}

int	main(int argc, char ** argv, char ** envp)
{
	int i = 1;
	int pos_end_line;
	int pos_start_pipe;
	int pos_end_pipe;

	char **cmd;

	int fd_in;
	int fd[2];
	pid_t pid;

	while (i < argc)
	{
		fd_in = 0;
		pos_end_line = i;
		pos_start_pipe = i;
		pos_end_pipe = i;

		while(pos_end_line < argc && strcmp(argv[pos_end_line], ";"))
			pos_end_line++;

		while(pos_start_pipe < pos_end_line)
		{
			pos_end_pipe = pos_start_pipe;
			while (pos_end_pipe < pos_end_line && strcmp(argv[pos_end_pipe], "|"))
				pos_end_pipe++;

			cmd = arg(argv, pos_start_pipe, pos_end_pipe); 
			printf("cmd = %s\n", cmd[0]);
			if (!cmd)
			{
				printf("error cmd\n");
				return (1);   // error
			}
			if (pipe(fd) == -1)
			{
				printf("error pipe\n");
				return (1); // error pipe
			}
			pid = fork();
			if (pid < 0)
			{
				printf("error fork\n");
				return (1); // fork error
			}			
			if (pid == 0)
			{
				if (dup2(fd_in, 0) == -1)
				{
					printf("error dup2\n");
					return (1); // error dup2
				}
				if (pos_end_pipe < pos_end_line && dup2(fd[1], 1) == -1)
				{
					printf("error dup2\n");
					return (1); // error dup2
				}
				close (fd_in);
				close (fd[0]);
				close (fd[1]);
				if (!strcmp(cmd[0], "cd"))
				{
					//case cd
				}
				else
				{
					if (execve(cmd[0], cmd, envp) == -1)
					{
						printf("error execve\n");
						return (1); // error execve
					}
				}
				free(cmd);
				exit(0);
			}
			else 
			{
				waitpid(pid, 0, 0);
				close(fd[1]);
				if (fd_in)
					close(fd_in);
				fd_in = fd[0];
				free(cmd);
			}
			pos_start_pipe = pos_end_pipe + 1;
		}
		close(fd_in);
		i = pos_end_line + 1;
	}

	return (0);
}
