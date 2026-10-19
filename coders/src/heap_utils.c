/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:28:48 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/15 18:36:23 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
Compute the priority key based on the active scheduler (fifo or edf)
- fifo: the current time. Whoever calls this first gets the smallest
  value, so first-come is served first.
- edf: the coder's burnout deadline. Whoever is closest to burning out
  gets the smallest value, so the most urgent coder is served first.
The smaller value always wins priority in both modes.
*/
long	calculate_priority(t_hub *hub, t_coder *coder)
{
	long	priority_ms;
	long	last_compile;

	if (hub->scheduler == EDF)
	{
		pthread_mutex_lock(&coder->time_lock);
		last_compile = coder->last_compile;
		pthread_mutex_unlock(&coder->time_lock);
		priority_ms = last_compile + hub->time_burnout;
	}
	else
		priority_ms = get_time(0);
	return (priority_ms);
}

/*
Resolve equal priorities between two coders.
EDF: the coder with the lowest coder_id wins.
FIFO: the coder with the lowest request_order wins.
*/
long	priority_tie_breaker(t_hub *hub, t_coder *coder)
{
	if (hub->scheduler == EDF)
		return (coder->id);
	return (coder->request_order);
}

/*
To use on the heap file
Compares two coders and decides which one has priority.
If priority_ms ties, falls back to priority_tie_breaker.
Returns true if "a" has priority over "b"
*/
int	is_higher_priority(t_hub *hub, t_coder *a, t_coder *b)
{
	if (a->priority_ms < b->priority_ms)
		return (1);
	if (a->priority_ms == b->priority_ms)
		return (priority_tie_breaker(hub, a)
			< priority_tie_breaker(hub, b));
	return (0);
}

// To swap the data nodes in the heap struct
void	ft_swap(t_coder **a, t_coder **b)
{
	t_coder	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}
