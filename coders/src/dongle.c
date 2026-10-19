/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:11:03 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/19 18:02:00 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	get_cooldown_deadline(t_dongle *dongle, int cooldown,
	struct timespec *ts)
{
	long	deadline_ms;

	deadline_ms = dongle->last_use + cooldown;
	ts->tv_sec = deadline_ms / 1000;
	ts->tv_nsec = deadline_ms % 1000 * 1000000;
}

/*
Try to acquire a single dongle: waits (via cond_wait) until the
dongle is available, its cooldown has passed, AND this coder is
next in line (top of the waiting heap). Marks it as taken before
returning.
*/
static void	try_take_dongle(t_hub *hub, t_coder *coder, t_dongle *dongle,
	char *msg)
{
	long			remaining_cooldown_t;
	struct timespec	ts;

	pthread_mutex_lock(&dongle->lock);
	coder->priority_ms = calculate_priority(hub, coder);
	assign_request_order(hub, coder);
	remaining_cooldown_t = get_time(dongle->last_use);
	min_heap_push(hub, &dongle->heap, coder);
	while ((!dongle->is_available
			|| remaining_cooldown_t < hub->dongle_cooldown
			|| dongle->heap.data[0] != coder) && !check_stop_flag(hub))
	{
		get_cooldown_deadline(dongle, hub->dongle_cooldown, &ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		remaining_cooldown_t = get_time(dongle->last_use);
	}
	if (check_stop_flag(hub))
	{
		pthread_mutex_unlock(&dongle->lock);
		return ;
	}
	dongle->is_available = 0;
	log_status(hub, coder, msg);
	min_heap_pop(&dongle->heap, hub);
	pthread_mutex_unlock(&dongle->lock);
}

/*
Attempt to use both dongles from a coder:
Left and right side. But first, choose the one to take
first based on their dongle_id.
*/
int	take_both_dongles(t_hub *hub, t_coder *coder)
{
	t_dongle	*first_place;
	t_dongle	*second_place;

	if (coder->dongle_l == coder->dongle_r)
	{
		try_take_dongle(hub, coder, coder->dongle_l, LOG_DONGLE_L);
		while (!check_stop_flag(hub))
			usleep(1000);
		return (0);
	}
	if (coder->dongle_l->id < coder->dongle_r->id)
	{
		first_place = coder->dongle_l;
		second_place = coder->dongle_r;
	}
	else
	{
		first_place = coder->dongle_r;
		second_place = coder->dongle_l;
	}
	try_take_dongle(hub, coder, first_place, LOG_DONGLE_L);
	if (check_stop_flag(hub))
		return (0);
	try_take_dongle(hub, coder, second_place, LOG_DONGLE_R);
	return (!check_stop_flag(hub));
}

/*
Release a currently occupied dongle.
If the heap.size > 0 it means that there are still
coders waiting to use it - So the condition variable
needs to be sinalized to let it know its available.
*/
static void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	dongle->last_use = get_time(0);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

// Release both dongles from one coder
void	release_both_dongle(t_coder *coder)
{
	release_dongle(coder->dongle_l);
	if (coder->dongle_l != coder->dongle_r)
		release_dongle(coder->dongle_r);
}
