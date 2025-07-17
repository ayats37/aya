/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:50:21 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 18:30:55 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_validated_path(char *cmd, t_env **envlist)
{
	char	*full_path;

	full_path = find_cmd_path(cmd, envlist);
	if (!full_path)
	{
		write_error_no_exit(cmd, "command not found");
		exit(127);
	}
	if (access(full_path, X_OK) != 0)
	{
		write_error_no_exit(cmd, "Permission denied");
		free(full_path);
		exit(126);
	}
	return (full_path);
}

static void	execute_child_process(char **cmds, t_env *envlist, t_token *node)
{
	char	*full_path;
	char	**env_array;
	int		redir_result;

	if (node && node->redir)
	{
		redir_result = handle_redirection(node);
		if (redir_result != 0)
			exit(redir_result);
	}
	full_path = get_validated_path(cmds[0], &envlist);
	env_array = env_list_to_array(envlist);
	if (!env_array)
	{
		free(full_path);
		write_error_no_exit(cmds[0], "environment conversion failed");
		exit(1);
	}
	execve(full_path, cmds, env_array);
	write_error_no_exit(cmds[0], "command not found");
	free(full_path);
	free_env_array(env_array);
	exit(127);
}

static void	handle_parent_wait(pid_t pid, int *last_exit_status)
{
	int	status;

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		*last_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		*last_exit_status = 128 + WTERMSIG(status);
	else
		*last_exit_status = 1;
}

int	execute_cmd(char **cmds, t_env *envlist, t_token *node,
		int *last_exit_status)
{
	pid_t	pid;

	if (!cmds || !cmds[0] || cmds[0][0] == '\0')
	{
		*last_exit_status = 0;
		return (0);
	}
	pid = fork();
	if (pid == -1)
	{
		write_error_no_exit(cmds[0], "fork failed");
		*last_exit_status = 1;
		return (1);
	}
	if (pid == 0)
	{
		execute_child_process(cmds, envlist, node);
	}
	handle_parent_wait(pid, last_exit_status);
	return (*last_exit_status);
}
