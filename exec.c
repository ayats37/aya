#include "minishell.h"

int execute_cmds(t_token *token, t_env **env_list)
{
	int status = 0;
	if (!token)
		return 1;
	t_token *tmp = token;
	while (tmp)
	{
		if (tmp->type == 1 || tmp->type == 3 || tmp->type == 4)
		{
			if (is_builtin(tmp->cmds[0]))
				status = execute_builtin(token, env_list);
			else
				status = execute_cmd(token->cmds, *env_list, token);
		}
		else if (tmp->type == 2)
			status = execute_pipe(token, env_list);
		tmp = tmp->next;
	}
	return (status);
}