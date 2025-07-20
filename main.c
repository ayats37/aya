/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:45:13 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/07/20 15:44:59 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*create_token(char *value, char quote, int has_space)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	else
	{
		token->value = ft_strdup(value);
		token->next = NULL;
	}
	// if (!token->value){
	// 	free(token);
	// 	return NULL;
	// }
	if (quote == '\'')
		token->type = 3;
	else if (quote == '"')
		token->type = 4;
	else
		token->type = 0;
	token->has_space = has_space;
	token->expand_heredoc = 0;
	token->cmds = NULL;
	token->redir = NULL;
	token->prev = NULL;
	token->fd = -1;
	token->expand = -1;
	return (token);
}

int	is_special_char(char c)
{
	return (ft_strchr("|<>()&", c) != NULL);
}

int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

void	handle_word_quote(char **result, t_lexer *lexer, int *in_quotes)
{
	char	quote;
	char	*temp;
	// char	*new_result;
	size_t	start;

	printf("pos1 : %d\n", lexer->position);
	quote = lexer->input[lexer->position++];
	start = lexer->position;
	*in_quotes = 1;
	while (lexer->position < lexer->length
		&& lexer->input[lexer->position] != quote)
		lexer->position++;
	printf("pos2 : %d\n", lexer->position);
	if (lexer->position >= lexer->length)
	{
		ft_putstr_fd("bash : syntax error unclosed quotes\n", 2);
		return ;
	}
	temp = ft_substr(lexer->input, start, lexer->position - start);
	// new_result = ft_strjoin(*result, temp);
	// free(*result);
	*result = temp;
	lexer->position++;
	*in_quotes = 0;
	free(temp);
}

int	is_space(t_lexer *lexer)
{
	if ((lexer->input[lexer->position] >= 9
			&& lexer->input[lexer->position] <= 13)
		|| lexer->input[lexer->position] == 32)
		return (1);
	return (0);
}

void	handle_plain_word(char **result, t_lexer *lexer)
{
	size_t	start;
	char	*temp;
	char	*new_result;

	start = lexer->position;
	while (lexer->position < lexer->length && !ft_strchr("\"'|<>()& ",
			lexer->input[lexer->position]))
		lexer->position++;
	temp = ft_substr(lexer->input, start, lexer->position - start);
	new_result = ft_strjoin(*result, temp);
	free(temp);
	free(*result);
	*result = new_result;
}

t_token	*handle_word(t_lexer *lexer)
{
	char	*result;
	int		has_space;
	int		in_quotes;
	t_token	*token;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	in_quotes = 0;
	while (lexer->position < lexer->length && !is_space(lexer))
	{
		if (is_quote(lexer->input[lexer->position]))
		{
			break;
		}
		else if (!in_quotes && is_special_char(lexer->input[lexer->position]))
			break ;
		else
			handle_plain_word(&result, lexer);
	}
	if (lexer->position < lexer->length && lexer->input[lexer->position] == ' ')
		has_space = 1;
	else
		has_space = 0;
	token = create_token(result, 0, has_space);
	free(result);
	return (token);
}

t_type	token_type(t_token *token)
{
	if (token->type == 3)
		return (SINGLE_QUOTE);
	else if (token->type == 4)
		return (DOUBLE_QUOTE);
	else if (strcmp(token->value, "|") == 0)
		return (PIPE);
	else if (strcmp(token->value, ">>") == 0)
		return (APPEND);
	else if (strcmp(token->value, "<<") == 0)
		return (HEREDOC);
	else if (strcmp(token->value, "<") == 0)
		return (REDIR_IN);
	else if (strcmp(token->value, ">") == 0)
		return (REDIR_OUT);
	else
		return (CMD);
}

void	append_token(t_token **head, t_token *token)
{
	t_token	*tmp;

	tmp = NULL;
	if (!token)
		return ;
	if (!*head)
	{
		*head = token;
		return ;
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = token;
}

t_token	*handle_quote(t_lexer *lexer, char quote)
{
	t_token	*token;
	size_t	length;
	size_t	start;
	char	*value;

	token = NULL;
	lexer->position += 1;
	start = lexer->position;
	while (lexer->position < lexer->length
		&& lexer->input[lexer->position] != quote)
		lexer->position++;
	if (lexer->position >= lexer->length)
	{
		ft_putstr_fd("bash : syntax error unclosed quote\n", 2);
		return (0);
	}
	length = lexer->position - start;
	value = ft_substr(lexer->input, start, length);
	lexer->position += 1;
	if (lexer->input[lexer->position] == 32)
		token = create_token(value, quote, 1);
	else
		token = create_token(value, quote, 0);
	free(value);
	return (token);
}

t_token	*handle_operations(t_lexer *lexer, char *oper, int i)
{
	char	*str;
	t_token	*token;

	str = ft_substr(oper, 0, i);
	if (!str)
		return (NULL);
	lexer->position += i;
	if (lexer->input[lexer->position] == 32)
		token = create_token(str, str[0], 1);
	else
		token = create_token(str, str[0], 0);
	free(str);
	return (token);
}

t_lexer	*initialize_lexer(char *input)
{
	t_lexer	*lexer;

	lexer = malloc((sizeof(t_lexer)));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->length = ft_strlen(input);
	lexer->position = 0;
	return (lexer);
}

void	skip_whitespace(t_lexer *lexer)
{
	while (lexer->position < lexer->length && is_space(lexer))
		lexer->position++;
}

t_token	*get_next_token(t_lexer *lexer)
{
	char	*current;

	skip_whitespace(lexer);
	if (lexer->position >= lexer->length)
		return (NULL);
	current = lexer->input + lexer->position;
	if (current[0] == '\'' || current[0] == '"')
		return (handle_quote(lexer, *current));
	if ((lexer->input[lexer->position] == '|' && lexer->input[lexer->position
			+ 1] == '|') || (lexer->input[lexer->position] == '&'
			&& lexer->input[lexer->position + 1] == '&'))
		return (handle_operations(lexer, current, 2));
	if ((lexer->input[lexer->position] == '>' && lexer->input[lexer->position
			+ 1] == '>') || (lexer->input[lexer->position] == '<'
			&& lexer->input[lexer->position + 1] == '<'))
		return (handle_operations(lexer, current, 2));
	if (current[0] == '|' || current[0] == '<' || current[0] == '>'
		|| current[0] == '(' || current[0] == ')' || current[0] == '&')
		return (handle_operations(lexer, current, 1));
	return (handle_word(lexer));
}

int	check_errors(t_token *token)
{
	t_token	*tmp;

	if (!token)
		return (1);
	if (token->type == 2)
	{
		printf("bash: syntax error near unexpected token `|'\n");
		return (1);
	}
	tmp = token;
	while (tmp)
	{
		if ((tmp->type == 2 || tmp->type == 5 || tmp->type == 6
				|| tmp->type == 7 || tmp->type == 8) && !tmp->next)
		{
			printf("bash: syntax error near unexpected token `newline'\n");
			return (1);
		}
		if ((tmp->type == 5 || tmp->type == 6 || tmp->type == 7
				|| tmp->type == 8) && tmp->next && (tmp->next->type == 2
				|| tmp->next->type == 5 || tmp->next->type == 6
				|| tmp->next->type == 7 || tmp->next->type == 8))
		{
			printf("bash: syntax error near unexpected token `%s'\n",
					tmp->next->value);
			return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}

void	print_linked_list(t_token *token_list)
{
	t_token	*current;
	t_token	*redir_tmp;
	int		i;

	current = token_list;
	while (current)
	{
		printf("token->value =%s	token->type =%d			token->has_space = %d		token->expand_heredoc = %d\n",
				current->value,
				current->type,
				current->has_space,
				current->expand_heredoc);
		if (current->cmds)
		{
			printf("  cmds: ");
			i = 0;
			while (current->cmds[i])
			{
				printf("[%s] ", current->cmds[i]);
				i++;
			}
			printf("\n");
		}
		if (current->redir)
		{
			printf("  redir: ");
			redir_tmp = current->redir;
			while (redir_tmp)
			{
				printf("[type:%d value:%s] ", redir_tmp->type,
						redir_tmp->value);
				redir_tmp = redir_tmp->next;
			}
			printf("\n");
		}
		current = current->next;
	}
}

t_token	*get_cmd_and_redir(t_token *token_list)
{
	t_token	*final_token;
	t_token	*tmp;
	t_token	*pipe;
	t_token	*cmd_token;
	char	**cmds;
	t_token	*redir_head;
	t_token	*redir_tail;
	int		cmd_count;
	int		cmd_capacity;
	char	**new_cmds;
	t_token	*redir_op;
	t_token	*redir_target;
	t_token	*redir_token;

	final_token = NULL;
	tmp = token_list;
	while (tmp)
	{
		if (tmp->type != PIPE)
		{
			cmd_token = NULL;
			cmds = NULL;
			redir_head = NULL;
			redir_tail = NULL;
			cmd_count = 0;
			cmd_capacity = 8;
			cmds = malloc(sizeof(char *) * cmd_capacity);
			if (!cmds)
				return (NULL);
			while (tmp && tmp->type != PIPE)
			{
				if (tmp->type == CMD || tmp->type == SINGLE_QUOTE
					|| tmp->type == DOUBLE_QUOTE)
				{
					if (cmd_count >= cmd_capacity)
					{
						cmd_capacity *= 2;
						new_cmds = realloc(cmds, sizeof(char *) * cmd_capacity);
						if (!new_cmds)
						{
							free(cmds);
							return (NULL);
						}
						cmds = new_cmds;
					}
					cmds[cmd_count++] = strdup(tmp->value);
					tmp = tmp->next;
				}
				else if (tmp->type == REDIR_IN || tmp->type == REDIR_OUT
						|| tmp->type == APPEND || tmp->type == HEREDOC)
				{
					redir_op = tmp;
					redir_target = tmp->next;
					if (!redir_target)
						break ;
					redir_token = create_token(redir_target->value, 0,
							redir_target->has_space);
					redir_token->type = redir_op->type;
					if (!redir_head)
						redir_head = redir_token;
					else
						redir_tail->next = redir_token;
					redir_tail = redir_token;
					tmp = redir_target->next;
				}
				else
					tmp = tmp->next;
			}
			cmds[cmd_count] = NULL;
			if (cmds && cmds[0])
			{
				cmd_token = create_token(cmds[0], 0, 0);
				cmd_token->type = CMD;
			}
			else
			{
				cmd_token = create_token("", 0, 0);
				cmd_token->type = CMD;
			}
			cmd_token->cmds = cmds;
			cmd_token->redir = redir_head;
			append_token(&final_token, cmd_token);
		}
		else if (tmp && tmp->type == PIPE)
		{
			pipe = create_token(tmp->value, 0, tmp->has_space);
			pipe->type = PIPE;
			append_token(&final_token, pipe);
			tmp = tmp->next;
		}
	}
	return (final_token);
}

void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	join_tokens(t_token **tokens)
{
	t_token	*tmp;
	t_token	*del;
	int		is_expand;
	char	*new_value;

	tmp = *tokens;
	while (tmp)
	{
		if (tmp->has_space == 0 && tmp->next && ((tmp->type == 3
					|| tmp->type == 4 || tmp->type == 1)
				&& (tmp->next->type == 3 || tmp->next->type == 4
					|| tmp->next->type == 1)))
		{
			is_expand = tmp->expand_heredoc;
			new_value = ft_strjoin(tmp->value, tmp->next->value);
			free(tmp->value);
			tmp->value = new_value;
			tmp->type = 1;
			tmp->expand_heredoc = is_expand;
			tmp->has_space = tmp->next->has_space;
			del = tmp->next;
			tmp->next = del->next;
		}
		else
			tmp = tmp->next;
	}
}

void	reset_terminal_mode(void)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	write(STDERR_FILENO, "\r\033[K", 4);
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ECHOCTL);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int	main(int ac, char **av, char **env)
{
	// int last_exit_status = 0;
	t_lexer *lexer = NULL;
	t_token *token = NULL;
	t_token *token_list = NULL;
	t_token *final_token = NULL;
	char *input = NULL;
	(void)ac;
	(void)av;
	int last_exit_status = 0;
	t_env *env_list = init_env(env);
	while (1)
	{
		input = readline("minishell> ");
		if (!input)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (input[0] == '\0')
		{
			free(input);
			continue ;
		}
		add_history(input);
		token_list = NULL;
		lexer = initialize_lexer(input);
		while (lexer->position < lexer->length)
		{
			token = get_next_token(lexer);
			if (!token)
				continue ;
			token->type = token_type(token);
			append_token(&token_list, token);
		}
		free_lexer(lexer);
		lexer = NULL;
		if (!token_list)
		{
			free(input);
			continue ;
		}
		if (check_errors(token_list) == 1)
		{
			last_exit_status = 258;
			free(input);
			free_token_list(token_list);
			continue ;
		}
		expand_variables(&token_list, env_list);
		join_tokens(&token_list);
		split_expanded_tokens(&token_list);
		final_token = get_cmd_and_redir(token_list);
		process_heredoc(final_token, env_list);
		close_heredoc_fds(final_token);
		execute_cmds(final_token, &env_list, &last_exit_status);
		// print_linked_list(final_token);
	}
}