/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:52:17 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 15:48:45 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *get_env(t_env *env_list, const char *name)
{
    while (env_list)
    {
        if (strcmp(env_list->name, name) == 0)
            return env_list->value;
        env_list = env_list->next;
    }
    return NULL;
}


char *get_var_name(char *str, int *i)
{
    int start = *i;
    while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
        (*i)++;
    return ft_substr(str, start, *i - start);
}

char *expand_token(char *str, t_env *env_list, int last_exit_status, int type)
{
    int i = 0;
    char *result = ft_strdup("");
    char *tmp = NULL;

    if (!result)
        return NULL;

    while (str[i])
    {
        if (str[i] == '$' && type != 3)
        {
            i++;
            if (str[i] == '?')
            {
                tmp = ft_itoa(last_exit_status);
                i++;
            }
            else if (str[i] && (ft_isalpha(str[i]) || str[i] == '_'))
            {
                char *var_name = get_var_name(str, &i);
                char *val = get_env(env_list, var_name);
                if (val != NULL)
                    tmp = ft_strdup(val);
                else
                    tmp = ft_strdup("");
                free(var_name);
            }
            else
            {
                tmp = ft_strdup("$");
            }
        }
        else
        {
            tmp = ft_substr(str, i, 1);
            i++;
        }

        if (!tmp)
        {
            free(result);
            return NULL;
        }

        char *new_result = ft_strjoin(result, tmp);
        free(tmp);
        free(result);
        result = new_result;

        if (!result)
            return NULL;
    }

    return result;
}

void expand_variables(t_token *tokens, t_env *env_list, int last_exit_status)
{
    while (tokens)
    {
        if (tokens->type == 1 || tokens->type == 4)
        {
            char *expanded = expand_token(tokens->value, env_list, last_exit_status, tokens->type);
            if (expanded)
            {
                free(tokens->value);
                tokens->value = expanded;
            }
        }
        tokens = tokens->next;
    }
}
