/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 15:48:01 by taya              #+#    #+#             */
/*   Updated: 2025/07/20 15:48:02 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void replace_var_heredoc(char *line, int i, char *env, int len)
{
	char *start = ft_substr(line, 0, i);
	char *end = ft_strdup(line + i + 1 + len);
	char *new = ft_strjoin(start, env);
	char *value = ft_strjoin(new, end);
	free(line);
	line = value;
	free(start);
	free(end);
	free(new);
}

void to_expand_heredoc(char *line, t_env *env_list)
{
	char *var_name = NULL;
	char *env_value = NULL;
	int i = 0;
	while (line[i])
	{
		if (line[i] == '$' && (line[i + 1] == '$' || line[i + 1] == '\0'))
		{
			i++;
			if (line[i] == '\0')
				break;
		}
		else if (line[i] == '$')
		{
			if (line[i + 1] == '?')
			{
				// get_exit_status();
				i++;
			}
			else
			{	
				var_name = get_var(line, i + 1);
				env_value = get_env_var(env_list, var_name);
				if (env_value)
				{
					replace_var_heredoc(line, i, env_value, ft_strlen(var_name));
					i += ft_strlen(var_name);	
				}
				else
					replace_var_heredoc(line, i, "", ft_strlen(var_name));
			}
			free(var_name);
		}
		i++;
	}
}

void expand_heredoc(char **line, t_env *env_list) 
{
	if (!line || !line[0])
		return ;
	to_expand_heredoc(line[0], env_list);
}