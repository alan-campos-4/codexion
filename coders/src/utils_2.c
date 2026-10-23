/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:00:27 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 16:32:00 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Returns a value depending on the sceduler:
 * - for EDF, the last_compile_time + burnout_time.
 * - for FIFO, the current time.
 * This value is used by the coders when trying to obtain dongles,
 * the one with the smaller value will be the one who gets the dongle.
 */
int	get_request_value(t_coder *coder)
{
	if (coder->hub->scheduler == EDF)
		return (coder->last_compile_time
			+ ((int)coder->hub->burnout_time * MS_TO_US));
	else
		return (get_time(0));
}

/**
 * Swaps two requests.
 * Used by the heap functions.
 */
void	swap(t_request *x, t_request *y)
{
	t_request	tmp;

	tmp = *y;
	*y = *x;
	*x = tmp;
}