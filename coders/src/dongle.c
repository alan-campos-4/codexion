/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:11:03 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:57:42 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	safe_get_remaining(t_dongle *dongle, int cooldown,
				int *remaining)
{
	if (!dongle)
		return (1);
	*remaining = 0;
	pthread_mutex_lock(&dongle->lock);
	if (dongle->last_use_time)
	{
		*remaining = dongle->last_use_time + (cooldown * MS_IN_US)
			- get_time(0);
	}
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

static int	apply_cooldown(t_dongle *first, t_dongle *second,
	t_coder *coder)
{
	int	r_first;
	int	r_second;
	int	max_wait;

	r_first = 0;
	r_second = 0;
	if (safe_get_remaining(first, coder->hub->cooldown, &r_first))
		return (1);
	if (safe_get_remaining(second, coder->hub->cooldown, &r_second))
		return (1);
	max_wait = r_first;
	if (r_second > r_first)
		max_wait = r_second;
	if (max_wait <= 0)
		return (0);
	return (better_sleep(coder, max_wait));
}

static void	wait_for_dongle(t_dongle *to_wait, t_dongle *other)
{
	pthread_mutex_unlock(&other->lock);
	pthread_cond_wait(&to_wait->cond, &to_wait->lock);
	pthread_mutex_unlock(&to_wait->lock);
}

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

int	get_dongles(t_coder *coder)
{
	t_dongle	*f;
	t_dongle	*s;

	f = coder->first_dongle;
	s = coder->second_dongle;
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
	if (log_status("has taken dongle", coder, coder->hub)
		|| log_status("has taken dongle", coder, coder->hub))
		return (1);
	return (0);
}












/*static void	get_cooldown_deadline(t_dongle *dongle, int cooldown,
		struct timespec *ts)
{
	long	deadline_ms;

	deadline_ms = dongle->last_use + cooldown;
	ts->tv_sec = deadline_ms / 1000;
	ts->tv_nsec = deadline_ms % 1000 * 1000000;
}

////
Try to acquire a single dongle: waits (via cond_wait) until the
dongle is available, its cooldown has passed, AND this coder is
next in line (top of the waiting heap). Marks it as taken before
returning.

static void	try_take_dongle(t_hub *hub, t_coder *coder, t_dongle *dongle,
		char *msg)
{
	long			remaining_cooldown_t;
	struct timespec	ts;

	pthread_mutex_lock(&dongle->lock);
	coder->priority_ms = calculate_priority(hub, coder);
	assign_request_order(hub, coder);
	remaining_cooldown_t = get_time(0) - dongle->last_use;
	min_heap_push(hub, &dongle->heap, coder);
	while ((!dongle->is_available || remaining_cooldown_t < hub->dongle_cooldown
			|| dongle->heap.data[0] != coder) && !stop_program(hub))
	{
		get_cooldown_deadline(dongle, hub->dongle_cooldown, &ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		remaining_cooldown_t = get_time(0) - dongle->last_use;
	}
	if (stop_program(hub))
	{
		pthread_mutex_unlock(&dongle->lock);
		return ;
	}
	dongle->is_available = 0;
	log_status(hub, coder, msg);
	min_heap_pop(&dongle->heap, hub);
	pthread_mutex_unlock(&dongle->lock);
}


Attempt to use both dongles from a coder:
Left and right side. But first, choose the one to take
first based on their dongle_id.

static int	try_take_dongle(t_hub *hub, t_coder *coder, t_dongle *dongle, char *msg)
{
	long			remaining_cooldown_t;
	struct timespec	ts;

	while ((!dongle->is_available || remaining_cooldown_t < hub->dongle_cooldown
			|| dongle->heap.data[0] != coder) && !stop_program(hub))
	{
		get_cooldown_deadline(dongle, hub->dongle_cooldown, &ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
		remaining_cooldown_t = get_time(0) - dongle->last_use;
	}
	pthread_mutex_lock(&dongle->lock);
	if (!dongle->is_available)
	{
		pthread_mutex_unlock(&dongle->lock);
		usleep(1000);
		return (1);
	}
	else
	{
		dongle->is_available = 1;
		pthread_mutex_unlock(&dongle->lock);
		log_status(coder->hub, coder, msg);
		return (0);
	}
}

int	take_both_dongles(t_hub *hub, t_coder *coder)
{
	t_dongle	*first_place;
	t_dongle	*second_place;

	if (coder->dongle_l == coder->dongle_r)
	{
		try_take_dongle(hub, coder, coder->dongle_l, LOG_DONGLE_L);
		while (!stop_program(hub))
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
	if (stop_program(hub))
		return (0);
	try_take_dongle(hub, coder, second_place, LOG_DONGLE_R);
	return (!stop_program(hub));
}


Release a currently occupied dongle.
If the heap.size > 0 it means that there are still
coders waiting to use it - So the condition variable
needs to be sinalized to let it know its available.

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
}*/
