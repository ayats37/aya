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



int g_heredoc_interrupted = 0;

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

// int	is_builtin(char *cmd);
// int	dispatch_builtin(char **cmd, t_env **envlist);
// int	execute_builtin(t_token *node, t_env **envlist);
// int	execute_cmd(char **cmds, t_env *envlist, t_token *node);
// int execute_cmds(t_token *token, t_env **env_list);
// char	*get_path(t_env *envlist);
// char	**get_paths(t_env **envlist);
// char	*build_path(char *path, char *cmd);
// char	*check_paths(char **paths, char *cmd);
// char	*find_cmd_path(char *cmd, t_env **envlist);
// int	execute_pipe(t_token *token, t_env **env_list);
// int	handle_output_redir(t_token *redir);
// void	handle_heredoc_redir(t_token *redir);
// int	handle_input_redir(t_token *redir);
// int	handle_redirection(t_token *node);

#endif