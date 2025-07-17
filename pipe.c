/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:51:18 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 13:51:19 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int execute_pipeline(t_token *token, t_env **env_list)
{
    int cmd_count = 0;
    t_token *tmp = token;
    while (tmp)
    {
        if (tmp->type == 1 || tmp->type == 3 || tmp->type == 4)
            cmd_count++;
        tmp = tmp->next;
    }
    
    if (cmd_count <= 1)
    {
        if (is_builtin(token->cmds[0]))
            return execute_builtin(token, env_list);
        else
            return execute_cmd(token->cmds, *env_list, token);
    }
    int pipes[cmd_count - 1][2];
    pid_t pids[cmd_count];
    for (int i = 0; i < cmd_count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            write_error_no_exit(NULL, "pipe failed");
            return 1;
        }
    }
    tmp = token;
    int cmd_index = 0;
    while (tmp && cmd_index < cmd_count)
    {
        if (tmp->type == 1 || tmp->type == 3 || tmp->type == 4)
        {
            pids[cmd_index] = fork();
            if (pids[cmd_index] == -1)
            {
                write_error_no_exit(NULL, "fork failed");
                return 1;
            }
            
            if (pids[cmd_index] == 0)
            {
                if (cmd_index > 0)
                    dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
                if (cmd_index < cmd_count - 1)
                    dup2(pipes[cmd_index][1], STDOUT_FILENO);
                for (int i = 0; i < cmd_count - 1; i++)
                {
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }
                if (is_builtin(tmp->cmds[0]))
                    exit(execute_builtin(tmp, env_list));
                else
                    exit(execute_cmd(tmp->cmds, *env_list, tmp));
            }
            cmd_index++;
        }
        tmp = tmp->next;
    }
    for (int i = 0; i < cmd_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    int status = 0;
    for (int i = 0; i < cmd_count; i++)
    {
        int temp_status;
        waitpid(pids[i], &temp_status, 0);
        if (i == cmd_count - 1)
            status = temp_status;
    }
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        return (128 + WTERMSIG(status));
    return 1;
}
