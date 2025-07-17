/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:50:47 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 16:31:47 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int execute_cmds(t_token *token, t_env **env_list, int *last_exit_status)
{
    int status = 0;
    t_token *tmp;
    int has_pipe;

    if (!token)
        return (1);
    tmp = token;
    has_pipe = 0;
    while (tmp)
    {
        if (tmp->type == 2)
        {
            has_pipe = 1;
            break;
        }
        tmp = tmp->next;
    }
    if (has_pipe)
        return (execute_pipeline(token, env_list, last_exit_status));

    tmp = token;
    while (tmp)
    {
        if (tmp->type == 1 || tmp->type == 3 || tmp->type == 4)
        {
            if (is_builtin(tmp->cmds[0]))
                status = execute_builtin(tmp, env_list);
            else
                status = execute_cmd(tmp->cmds, *env_list, tmp, last_exit_status);
            *last_exit_status = status;
        }
        tmp = tmp->next;
    }
    return (status);
}
