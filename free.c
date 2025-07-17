/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:54:20 by taya              #+#    #+#             */
/*   Updated: 2025/07/17 17:04:23 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void free_lexer(t_lexer *lexer)
{
    if (!lexer)
        return;
    free(lexer);
}

void free_token(t_token *token)
{
    if (!token)
        return;
    if (token->value)
        free(token->value);
    
    free(token);
}


void free_token_list(t_token *token_list)
{
    t_token *current;
    t_token *next;
    
    current = token_list;
    while (current)
    {
        next = current->next;
        free_token(current);
        current = next;
    }
}

