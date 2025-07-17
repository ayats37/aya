/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:51:12 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 13:51:13 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int	g_heredoc_interrupted = 0;

void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_heredoc_interrupted = 1;
	write(1, "\n", 1);
	exit(130);
}

void	handle_heredoc_input(char *delimiter, int write_fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			write(1, " ", 1);
			break ;
		}
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
}

void close_heredoc_fds(t_token *token)
{
	t_token *tmp = token;
	t_token *redir;
	
	while (tmp)
	{
		if (tmp->redir)
		{
			redir = tmp->redir;
			while (redir)
			{
				if (redir->type == HEREDOC && redir->fd != -1)
				{
					close(redir->fd);
					redir->fd = -1;
				}
				redir = redir->next;
			}
		}
		tmp = tmp->next;
	}
}

void process_heredoc(t_token *token)
{
	int		status;
	pid_t pid;
	int pipe_fd[2];
	t_token *redir;
	
	if (!token)
		return ;
	g_heredoc_interrupted = 0;
	t_token *tmp = token;
	while (tmp)
	{
		if (tmp->redir)
		{
			redir = tmp->redir;
			while (redir)
			{
				if (redir->type == HEREDOC)
				{
					if (redir->fd != -1)
					{
						close(redir->fd);
						redir->fd = -1;
					}			
					if (pipe(pipe_fd) == -1)
					{
						perror("pipe failed");
						redir = redir->next;
						continue ;
					}
					pid = fork();
					if (pid == -1)
					{
						perror("fork failed");
						close(pipe_fd[0]);
						close(pipe_fd[1]);
						redir = redir->next;
						continue ;
					}
					if (pid == 0)
					{
						close(pipe_fd[0]);
						signal(SIGINT, heredoc_sigint_handler);
						signal(SIGQUIT, SIG_IGN);
						handle_heredoc_input(redir->value, pipe_fd[1]);
						close(pipe_fd[1]);
						exit(0);
					}
					close(pipe_fd[1]);
					signal(SIGINT, SIG_IGN);
					waitpid(pid, &status, 0);
					if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
					{
						close(pipe_fd[0]);
						redir->fd = -1;
						g_heredoc_interrupted = 1;
						signal(SIGINT, handler);
						reset_terminal_mode();
						close_heredoc_fds(token);
						return ;
					}
					else if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
					{
						close(pipe_fd[0]);
						redir->fd = -1;
						g_heredoc_interrupted = 1;
						signal(SIGINT, handler);
						reset_terminal_mode();
						close_heredoc_fds(token);
						return ;
					}
					else
						redir->fd = pipe_fd[0];
					signal(SIGINT, handler);
					reset_terminal_mode();
				}
				redir = redir->next;
			}
		}
		tmp = tmp->next;	
	}
}
