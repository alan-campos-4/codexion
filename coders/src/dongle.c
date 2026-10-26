/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:11:03 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/26 13:24:50 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Safely returns the time to wait until the dongle's cooldown has passed.
 */
static int	get_wait_time(t_dongle *dongle, int cooldown, int *wait)
{
	if (!dongle)
		return (1);
	*wait = 0;
	pthread_mutex_lock(&dongle->lock);
	if (dongle->last_use_time)
	{
		*wait = dongle->last_use_time + (cooldown)
			- get_time(0);
	}
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

/**
 * Checks for the wait time of each dongle and makes the coder wait
 * for the biggest amount of time.
 */
static int	apply_cooldown(t_coder *coder, t_dongle *dl, t_dongle *dr)
{
	int	dl_wait_time;
	int	dr_wait_time;
	int	max_wait_time;

	dl_wait_time = 0;
	dr_wait_time = 0;
	if (get_wait_time(dl, coder->hub->cooldown, &dl_wait_time))
		return (1);
	if (get_wait_time(dr, coder->hub->cooldown, &dr_wait_time))
		return (1);
	max_wait_time = dl_wait_time;
	if (dr_wait_time > dl_wait_time)
		max_wait_time = dr_wait_time;
	if (max_wait_time <= 0)
		return (0);
	return (coder_do(coder, max_wait_time));
}

/**
 * Make the dongle that was not available wait until
 * its cond is broadcast by the coder release or other event.
 * Unlock both mutexes after being locked at the start of check_and_take.
 */
static void	wait_for_dongle(t_dongle *to_wait, t_dongle *other)
{
	pthread_mutex_unlock(&other->lock);
	pthread_cond_wait(&to_wait->cond, &to_wait->lock);
	pthread_mutex_unlock(&to_wait->lock);
}

/**
 * Check the availability of a coder's dongles, and take them if possible.
 * - Check if the first request in the heap belongs to the given coder.
 * - Once taken, a dongle is marked as not available and removed from its heap.
 */
static int	check_and_take(t_coder *coder, t_dongle *dl, t_dongle *dr)
{
	int	dl_ready;
	int	dr_ready;

	pthread_mutex_lock(&dl->lock);
	pthread_mutex_lock(&dr->lock);
	dl_ready = (dl->heap.requests[0].id == coder->id) * dl->is_available;
	dr_ready = (dr->heap.requests[0].id == coder->id) * dr->is_available;
	if (dl_ready && dr_ready)
	{
		dl->is_available = 0;
		dr->is_available = 0;
		remove_request_top(&dl->heap);
		remove_request_top(&dr->heap);
		pthread_mutex_unlock(&dr->lock);
		pthread_mutex_unlock(&dl->lock);
		return (0);
	}
	else if (!dl_ready)
		wait_for_dongle(dl, dr);
	else
		wait_for_dongle(dr, dl);
	return (1);
}

/**
 * The coder's action for trying to obtaining its two assigned dongles.
 * - Their requests is added to the corresponding dongles.
 * - They continiously try to take them.
 * - Once taken, they can go on to the next action.
 */
int	try_get_dongles(t_coder *coder)
{
	t_dongle	*dl;
	t_dongle	*dr;

	dl = coder->dongle_l;
	dr = coder->dongle_r;
	add_requests_heap(coder, dl, dr, get_request_value(coder));
	while (1)
	{
		if (stop_program(coder->hub))
			return (1);
		if (!check_and_take(coder, dl, dr))
			break ;
	}
	if (apply_cooldown(coder, dl, dr))
		return (1);
	if (log_status(coder->hub, coder->id, LOG_DONGLE)
		|| log_status(coder->hub, coder->id, LOG_DONGLE))
		return (1);
	return (0);
}
