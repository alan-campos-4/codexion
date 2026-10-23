/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:11:03 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/22 15:45:59 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * ...
 */
static int	safe_get_remaining(t_dongle *dongle, int cooldown,
				int *remaining)
{
	if (!dongle)
		return (1);
	*remaining = 0;
	pthread_mutex_lock(&dongle->lock);
	if (dongle->last_use_time)
	{
		*remaining = dongle->last_use_time + (cooldown)
			- get_time(0);
	}
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

/**
 * ...
 */
static int	apply_cooldown(t_dongle *first, t_dongle *second,
	t_coder *coder)
{
	int	remaining_l;
	int	remaining_r;
	int	max_wait;

	remaining_l = 0;
	remaining_r = 0;
	if (safe_get_remaining(first, coder->hub->cooldown, &remaining_l))
		return (1);
	if (safe_get_remaining(second, coder->hub->cooldown, &remaining_r))
		return (1);
	max_wait = remaining_l;
	if (remaining_r > remaining_l)
		max_wait = remaining_r;
	if (max_wait <= 0)
		return (0);
	return (coder_sleep(coder, max_wait));
}

/**
 * ...
 */
static void	wait_for_dongle(t_dongle *to_wait, t_dongle *other)
{
	pthread_mutex_unlock(&other->lock);
	pthread_cond_wait(&to_wait->cond, &to_wait->lock);
	pthread_mutex_unlock(&to_wait->lock);
}

/**
 * ...
 */
static int	check_and_take(t_dongle *first, t_dongle *second,
	t_coder *coder)
{
	int	f_ready;
	int	s_ready;

	pthread_mutex_lock(&first->lock);
	pthread_mutex_lock(&second->lock);
	f_ready = (first->heap.requests[0].id == coder->id) * first->is_available;
	s_ready = (second->heap.requests[0].id == coder->id) * second->is_available;
	if (f_ready && s_ready)
	{
		first->is_available = 0;
		second->is_available = 0;
		remove_request_top(&first->heap);
		remove_request_top(&second->heap);
		pthread_mutex_unlock(&second->lock);
		pthread_mutex_unlock(&first->lock);
		return (0);
	}
	else if (!f_ready)
		wait_for_dongle(first, second);
	else
		wait_for_dongle(second, first);
	return (1);
}

/**
 * The coder's action for obtaining its two assigned dongles.
 */
int	get_dongles(t_coder *coder)
{
	t_dongle	*f;
	t_dongle	*s;

	f = coder->dongle_l;
	s = coder->dongle_r;
	pre_register_heaps(f, s, coder, pick_metric(coder));
	while (1)
	{
		if (stop_program(coder->hub))
			return (1);
		if (!check_and_take(f, s, coder))
			break ;
	}
	if (apply_cooldown(f, s, coder))
		return (1);
	if (log_status(coder->hub, coder->id, LOG_DONGLE)
		|| log_status(coder->hub, coder->id, LOG_DONGLE))
		return (1);
	return (0);
}
