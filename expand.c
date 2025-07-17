/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 13:52:17 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 17:41:20 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_env(t_env *env_list, const char *name)
{
	while (env_list)
	{
		if (strcmp(env_list->name, name) == 0)
			return (env_list->value);
		env_list = env_list->next;
	}
	return (NULL);
}

char	*get_var_name(char *str, int *i)
{
	int	start;

	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	return (ft_substr(str, start, *i - start));
}

void	free_token_array(char **tokens)
{
	int	i;

	i = 0;
	while (tokens && tokens[i])
		free(tokens[i++]);
	free(tokens);
}

char	**split_whitespace(char *str)
{
	char	**words;
	int		count;
	int		i;
	int		j;
	int		k;

	count = 0;
	i = 0;
	if (!str || !*str)
		return (NULL);
	while (str[i])
	{
		while (str[i] && (str[i] == ' ' || str[i] == '\t'))
			i++;
		if (str[i])
		{
			count++;
			while (str[i] && str[i] != ' ' && str[i] != '\t')
				i++;
		}
	}
	if (count == 0)
		return (NULL);
	words = malloc(sizeof(char *) * (count + 1));
	if (!words)
		return (NULL);
	i = 0;
	k = 0;
	while (str[i] && k < count)
	{
		while (str[i] && (str[i] == ' ' || str[i] == '\t'))
			i++;
		if (str[i])
		{
			j = i;
			while (str[j] && str[j] != ' ' && str[j] != '\t')
				j++;
			words[k] = strndup(&str[i], j - i);
			if (!words[k])
			{
				while (k > 0)
					free(words[--k]);
				free(words);
				return (NULL);
			}
			k++;
			i = j;
		}
	}
	words[k] = NULL;
	return (words);
}

void	insert_tokens_after(t_token *token, char **words, int type)
{
	t_token	*curr;
	t_token	*new;

	curr = token;
	for (int i = 1; words[i]; i++)
	{
		new = malloc(sizeof(t_token));
		if (!new)
			return ;
		new->value = ft_strdup(words[i]);
		new->type = type;
		new->next = curr->next;
		curr->next = new;
		curr = new;
	}
}

char	*expand_token(char *str, t_env *env_list, int last_exit_status,
		int type)
{
	int		i;
	char	*result;
	char	*tmp;
	char	*var_name;
	char	*val;
	char	*new_result;

	i = 0;
	result = ft_strdup("");
	tmp = NULL;
	if (!result)
		return (NULL);
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
				var_name = get_var_name(str, &i);
				val = get_env(env_list, var_name);
				if (val != NULL)
					tmp = ft_strdup(val);
				else
					tmp = ft_strdup("");
				free(var_name);
			}
			else
				tmp = ft_strdup("$");
		}
		else
		{
			tmp = ft_substr(str, i, 1);
			i++;
		}
		if (!tmp)
		{
			free(result);
			return (NULL);
		}
		new_result = ft_strjoin(result, tmp);
		free(tmp);
		free(result);
		result = new_result;
		if (!result)
			return (NULL);
	}
	return (result);
}

void	expand_variables(t_token *tokens, t_env *env_list, int last_exit_status)
{
	char	*expanded;
	char	**words;

	while (tokens)
	{
		if (tokens->type == 1 || tokens->type == 4)
		{
			expanded = expand_token(tokens->value, env_list, last_exit_status,
					tokens->type);
			if (expanded)
			{
				words = split_whitespace(expanded);
				free(tokens->value);
				if (words && words[0])
				{
					tokens->value = ft_strdup(words[0]);
					insert_tokens_after(tokens, words, tokens->type);
				}
				else
				{
					tokens->value = ft_strdup("");
				}
				free_token_array(words);
				free(expanded);
			}
		}
		tokens = tokens->next;
	}
}
