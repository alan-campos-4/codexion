/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:00:27 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 14:00:36 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	pick_metric(t_coder *coder)
{
	if (coder->hub->scheduler == EDF)
		return (coder->last_compile
			+ ((int)coder->hub->burnout_time * MS_IN_US));
	else
		return (get_time(0));
}

void	swap(t_request *x, t_request *y)
{
	t_request	tmp;

	tmp = *y;
	*y = *x;
	*x = tmp;
}