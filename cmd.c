/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:50:21 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 13:50:23 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_cmd(char **cmds, t_env *envlist, t_token *node)
{
	pid_t	pid;
	int		status;
	char	*full_path;
	char	**env_array;
	int		redir_result;

	if (!cmds || !cmds[0] || cmds[0][0] == '\0')
		return (0);
	pid = fork();
	if (pid == -1)
	{
		write_error_no_exit(cmds[0], "fork failed");
		return (1);
	}
	if (pid == 0)
	{
		if (node && node->redir)
		{
			redir_result = handle_redirection(node);
			if (redir_result != 0)
				exit(redir_result);
		}	
		full_path = find_cmd_path(cmds[0], &envlist);
		if (!full_path)
		{
			write_error_no_exit(cmds[0], "command not found");
			exit(127);
		}
		if (access(full_path, X_OK) != 0)
		{
			write_error_no_exit(cmds[0], "Permission denied");
			free(full_path);
			exit(126);
		}
		env_array = env_list_to_array(envlist);
		if (!env_array)
		{
			free(full_path);
			write_error_no_exit(cmds[0], "environment conversion failed");
			exit(1);
		}	
		execve(full_path, cmds, env_array);
		free(full_path);
		free_env_array(env_array);
		write_error_no_exit(cmds[0], "command not found");
		exit(127);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}