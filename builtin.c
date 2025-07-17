/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:49:54 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 16:57:55 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (strcmp(cmd, "cd") == 0)
		return (1);
	if (strcmp(cmd, "echo") == 0)
		return (1);
	if (strcmp(cmd, "exit") == 0)
		return (1);
	if (strcmp(cmd, "env") == 0)
		return (1);
	if (strcmp(cmd, "export") == 0)
		return (1);
	if (strcmp(cmd, "unset") == 0)
		return (1);
	if (strcmp(cmd, "pwd") == 0)
		return (1);
	return (0);
}

int	dispatch_builtin(char **cmd, t_env **envlist)
{
	if (strcmp(cmd[0], "echo") == 0)
		return (ft_echo(cmd));
	if (strcmp(cmd[0], "cd") == 0)
		return (ft_cd(cmd, *envlist));
	if (strcmp(cmd[0], "pwd") == 0)
		return (ft_pwd());
	if (strcmp(cmd[0], "export") == 0)
		return (ft_export(cmd, envlist));
	if (strcmp(cmd[0], "unset") == 0)
		return (ft_unset(cmd, envlist));
	if (strcmp(cmd[0], "env") == 0)
		return (ft_env(envlist));
	if (strcmp(cmd[0], "exit") == 0)
		return (ft_exit(cmd, *envlist));
	return (0);
}

int	execute_builtin(t_token *node, t_env **envlist)
{
	int	stdout_backup;
	int	stdin_backup;
	int	result;
	int	redir_result;

	stdout_backup = dup(STDOUT_FILENO);
	stdin_backup = dup(STDIN_FILENO);
	if (node && node->redir)
	{
		redir_result = handle_redirection(node);
		if (redir_result != 0)
		{
			dup2(stdout_backup, STDOUT_FILENO);
			dup2(stdin_backup, STDIN_FILENO);
			close(stdout_backup);
			close(stdin_backup);
			return (redir_result);
		}
	}
	result = dispatch_builtin(node->cmds, envlist);
	if (node && node->redir)
	{
		dup2(stdout_backup, STDOUT_FILENO);
		dup2(stdin_backup, STDIN_FILENO);
		close(stdout_backup);
		close(stdin_backup);
	}
	return (result);
}
