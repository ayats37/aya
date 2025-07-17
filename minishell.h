#ifndef MINISHELL_H
#define MINISHELL_H


#include "./libft/libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <errno.h>
#include <termios.h>




extern int g_heredoc_interrupted;

typedef struct s_lexer {
	int position;
	int length;
	char *input;
}	t_lexer;

typedef enum s_type {
    CMD = 1,
	PIPE,	
	SINGLE_QUOTE,
	DOUBLE_QUOTE,
    REDIR_IN,
    REDIR_OUT,
    APPEND,
    HEREDOC
} t_type;

typedef struct s_token {
	char	*value;
	t_type		type;
	int			has_space;
	int expand_heredoc;
	char **cmds;
	struct s_token *redir;
	int fd;
	struct s_token *next;
}	t_token;

typedef struct s_env
{
	char *name;
	char *value;
	char **env;
	struct s_env *next;
}	t_env;

int	is_builtin(char *cmd);
int	dispatch_builtin(char **cmd, t_env **envlist);
int	execute_builtin(t_token *node, t_env **envlist);
int	execute_cmd(char **cmds, t_env *envlist, t_token *node);
int execute_cmds(t_token *token, t_env **env_list);
char	*get_path(t_env *envlist);
char	**get_paths(t_env **envlist);
char	*build_path(char *path, char *cmd);
char	*check_paths(char **paths, char *cmd);
char	*find_cmd_path(char *cmd, t_env **envlist);
int	handle_output_redir(t_token *redir);
void	handle_heredoc_redir(t_token *redir);
int	handle_input_redir(t_token *redir);
int	handle_redirection(t_token *node);
int execute_pipeline(t_token *token, t_env **env_list);
void	write_error_no_exit(char *command, char *message);

void	write_error(char *command, char *message);
char	*str_join_free(char *s1, const char *s2);
int	is_alphanumeric(int c);
int	is_alpha(int c);
int	is_digit(int c);
int	is_num(const char *str);
char	*char_to_str(char c);

void	free_env_array(char **env_array);
void	free_env_array_partial(char **env_array, int i);
void	ft_free_arr(char **arr);
void free_lexer(t_lexer *lexer);
void free_token(t_token *token);
void free_token_list(t_token *token_list);
void	free_env_list(t_env *env);


void	heredoc_sigint_handler(int sig);
void	handle_heredoc_input(char *delimiter, int write_fd);
void close_heredoc_fds(t_token *token);
void process_heredoc(t_token *token);


void	update_env(char *name, char *value, t_env **env_list);
char	*get_env_value(char *name, t_env *env_list);
t_env	*find_env_var(char *name, t_env *env_list);
void	env_append(char *name, char *value, t_env **env_list);
int	count_env_nodes(t_env *env_list);
t_env	*create_env_node(char *env_var);
void	add_to_env_list(t_env **head, t_env *new_node);
t_env	*init_env(char **envp);
char	**env_list_to_array(t_env *env_list);
char	*build_env_string(char *name, char *value);


void	unset_var(t_env **env_list, char *name);
int	ft_unset(char **cmd, t_env **env_list);
void	update_pwd_vars(char *oldpwd, t_env *envlist);
int	ft_pwd(void);
int	ft_export(char **cmd, t_env **env_list);
int	process_export(char *arg, t_env **env_list);
int	parse_export_arg(char *arg, char **name, char **value, int *append);
int	valid_identifier(char *str);
int	check_exit_args(char **cmd);
int	ft_exit(char **cmd, t_env *env_list);
int	ft_env(t_env **env_list);
int	is_valid_n_flag(char *arg);
int	ft_echo(char **cmd);
char	*get_home_path(char **cmd, char *oldpwd);
char	*get_oldpwd_path(char **cmd, char *oldpwd);
char	*get_envvar_path(char **cmd, char *oldpwd);
char	*get_cd_path(char **cmd, char *oldpwd);
int	ft_cd(char **cmd, t_env *envlist);

void	handler(int sig);
void    reset_terminal_mode(void);

void expand_variables(t_token *tokens, t_env *env_list, int last_exit_status);
char *expand_token(char *str, t_env *env_list, int last_exit_status, int type);
char *get_var_name(char *str, int *i);
char *get_env(t_env *env_list, const char *name);
void	free_token_array(char **tokens);
char	**split_whitespace(char *str);
void insert_tokens_after(t_token *token, char **words, int type);


#endif