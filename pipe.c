#include "minishell.h"

int	execute_pipe(t_token *token, t_env **env_list)
{
	int		fd[2];
	pid_t	pid1, pid2;
	int		status;

	if (!token || !token->next)
		return (1);

	if (pipe(fd) == -1)
		return (write_error_no_exit(NULL, "pipe failed"), 1);

	pid1 = fork();
	if (pid1 == -1)
		return (write_error_no_exit(NULL, "fork failed"), 1);
	if (pid1 == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		if (is_builtin(token->cmds[0]))
			exit(execute_builtin(token, env_list));
		else
			exit(execute_cmd(token->cmds, *env_list, token));
	}
	pid2 = fork();
	if (pid2 == -1)
		return (write_error_no_exit(NULL, "fork failed"), 1);
	if (pid2 == 0)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		if (is_builtin(token->next->cmds[0]))
			exit(execute_builtin(token->next, env_list));
		else
			exit(execute_cmd(token->next->cmds, *env_list, token->next));
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
