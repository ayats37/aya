/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:52:17 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 14:27:14 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *get_var(char *value, int i)
{
	int len = 0;
	int start = i;
	while (value[i] && (is_alphanumeric(value[i]) || value[i] == '_'))
	{
		len++;
		i++;
	}
	return (ft_substr(value, start, len));
}

char *get_env_var(t_env *env_list, char *name)
{
    while (env_list)
    {
        if (strcmp(env_list->name, name) == 0)
            return env_list->value;
        env_list = env_list->next;
    }
    return NULL;
}

void replace_var(t_token *tmp, int i, char *env, int len)
{
	char *start = ft_substr(tmp->value, 0, i);
	char *end = ft_strdup(tmp->value + i + 1 + len);
	char *new = ft_strjoin(start, env);
	char *value = ft_strjoin(new, end);
	free(tmp->value);
	tmp->value = value;
	free(start);
	free(end);
	free(new);
}

void to_expand(t_token *tmp, t_env *env_list)
{
	char *var_name = NULL;
	char *env_value = NULL;
	int i = 0;
	while (tmp->value[i])
	{
		if (tmp->value[i] == '$' && (tmp->value[i + 1] == '$' || tmp->value[i + 1] == '\0'))
		{
			i++;
			if (tmp->value[i] == '\0')
				break;
		}
		else if (tmp->value[i] == '$')
		{
			if (tmp->value[i + 1] == '?')
				i++;
			else
			{	
				var_name = get_var(tmp->value, i + 1);
				env_value = get_env_var(env_list, var_name);
				if (env_value)
				{
					replace_var(tmp, i, env_value, ft_strlen(var_name));
					i += ft_strlen(var_name);	
				}
				else
					replace_var(tmp, i, "", ft_strlen(var_name));
			}
			free(var_name);
		}
		i++;
	}
}

void expand_variables(t_token **token_list, t_env *env_list)
{
	if (!token_list)
		return ;
	t_token *tmp = *token_list;
	while (tmp)
	{
		if (tmp->type == 8 && tmp->next)
		{
			tmp = tmp->next;
			if (tmp->next && (tmp->type == 3 || tmp->type == 4 || tmp->next->type == 3 || tmp->next->type == 4))
				tmp->expand_heredoc = 0;
			else
				tmp->expand_heredoc = 1;
		}
		else if (tmp->type == 1 || tmp->type == 4)
			to_expand(tmp, env_list);
		tmp = tmp->next; 		
	}
}
