/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:11:03 by alvespin          #+#    #+#             */
/*   Updated: 2026/11/10 13:51:40 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Safely return a dongle's wait time, i.e. the difference between
 * the current time and the time the dongle should be available.
 * - If the wait time is positive (last_use + cooldown > get_time),
 * it means the dongle was recently used, and the coder must wait.
 * - If the wait time is negative, (get_time > last_use + cooldown),
 * it means the cooldown has passed and the coder doesn't need to wait.
 */
static int	get_wait_time(t_dongle *dongle, long cooldown)
{
	long	wait;

	if (dongle->last_use_time)
		wait = dongle->last_use_time + cooldown - get_time(0);
	else
		wait = 0;
	return (wait);
}

/*
Cooldown not passed

cooldown	   10
curr_time 1000020
last_used 1000015
last_used + cooldown - curr_time = 5
curr_time - (lastused + cooldown) = -5

Cooldown passed

cooldown	   10
curr_time 1000050
last_used 1000025
last_used + cooldown - curr_time = -15
curr_time - (lastused + cooldown) = 15

*/

/**
 * Calculate the wait time of each dongle and make the coder wait if necessary.
 */
static int	apply_cooldown(t_dongle *dl, t_dongle *dr, long cooldown)
{
	long	dl_wait_time;
	long	dr_wait_time;
	long	max_wait_time;

	dl_wait_time = get_wait_time(dl, cooldown);
	dr_wait_time = get_wait_time(dr, cooldown);
	//printf("waits: l %i r %i\n", dl_wait_time, dr_wait_time);
	max_wait_time = dl_wait_time;
	if (dr_wait_time > dl_wait_time)
		max_wait_time = dr_wait_time;
	if (max_wait_time <= 0)
		return (0);
	return (coder_do(max_wait_time));
}

/**
 * Check the availability of a coder's dongles, and take them if possible.
 * - Check if the first request in the heap belongs to the given coder.
 * - Once taken, a dongle is marked as not available and removed from its heap.
 * - If a dongle is not ready, it is made to wait.
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
		pthread_cond_wait(&dl->cond, &dl->lock);
	else
		pthread_cond_wait(&dr->cond, &dr->lock);
	pthread_mutex_unlock(&dl->lock);
	pthread_mutex_unlock(&dr->lock);
	return (1);
}

/**
 * The coder's action for trying to obtaining its two assigned dongles.
 * - Their requests is added to the corresponding dongles.
 * - They continiously try to take them.
 * - Once taken, they can go on to the next action.
 */
int	try_get_dongles(t_coder *coder, long cooldown)
{
	t_dongle	*dl;
	t_dongle	*dr;

	dl = coder->dongle_l;
	dr = coder->dongle_r;
	if (!dr || !dl)
		return (1);
	add_requests_heap(coder, dl, dr, get_request_value(coder));
	while (1)
	{
		if (stop_program(coder->hub))
			return (1);
		if (!check_and_take(coder, dl, dr))
			break ;
	}
	if (apply_cooldown(dl, dr, cooldown))
		return (1);
	if (log_status(coder->hub, coder->id, LOG_DONGLE)
		|| log_status(coder->hub, coder->id, LOG_DONGLE))
		return (1);
	return (0);
}
