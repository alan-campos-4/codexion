/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:04:31 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 12:11:23 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongles.h"



void	sim_start(t_hub *hub)
{
	while (1)
	{
		pthread_mutex_lock(&hub->lock);
		if (hub->ready_count == hub->n_coders)
		{
			//log_status(hub, hub->coders, "__ all ready");
			break ;
		}
		log_status(hub, hub->coders, "__ not yet");
		pthread_mutex_unlock(&hub->lock);
		usleep(1000);
	}
	hub->start_time = get_time(0);
	hub->start_flag = 1;
	pthread_cond_broadcast(&hub->cond);
	pthread_mutex_unlock(&hub->lock);
}

int	wait_for_others(t_coder *coder)
{
	pthread_mutex_lock(&coder->hub->lock);
	coder->hub->ready_count++;
	log_status(coder->hub, coder, "is ready");
	while (coder->hub->start_flag != 1)
		pthread_cond_wait(&coder->hub->cond, &coder->hub->lock);
	pthread_mutex_unlock(&coder->hub->lock);
	return (0);
}


/* ****************************** */
/* ****************************** */

static int	safe_get_remaining(t_dongle *dongle, int cooldown, int *remaining)
{
	if (!dongle)
		return (1);
	*remaining = 0;
	pthread_mutex_lock(&dongle->lock);
	if (dongle->last_use)
	{
		*remaining = dongle->last_use + (cooldown * 10000) - get_time(0);
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
	if (safe_get_remaining(first, coder->hub->dongle_cooldown, &r_first))
		return (1);
	if (safe_get_remaining(second, coder->hub->dongle_cooldown, &r_second))
		return (1);
	max_wait = r_first;
	if (r_second > r_first)
		max_wait = r_second;
	if (max_wait <= 0)
		return (0);
	usleep(max_wait * 1000);
	return (1);
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

int	try_get_dongles(t_coder *coder)
{
	t_dongle	*f;
	t_dongle	*s;

	f = coder->dongle_l;
	s = coder->dongle_r;
	add_requests_heap(f, s, coder, get_request_value(coder));
	while (1)
	{
		if (stop_program(coder->hub))
			return (1);
		if (!check_and_take(f, s, coder))
			break ;
	}
	if (apply_cooldown(f, s, coder))
		return (1);
	log_status(coder->hub, coder, "has taken dongle");
	log_status(coder->hub, coder, "has taken dongle");
	return (0);
}
/* ****************************** */
/* ****************************** */





/*static void	display_dongles(t_hub *hub)
{
	pthread_mutex_lock(&hub->log_lock);
	for (int i = 0; i < hub->n_coders; i++)
		printf("D%i\t", hub->dongles[i].id);
	printf("\n");
	for (int i = 0; i < hub->n_coders; i++)
	{
		if (hub->dongles[i].is_available)
			printf("[ ]\t");
		else
			printf("[X]\t");
	}
	printf("\n");
	pthread_mutex_unlock(&hub->log_lock);
}*/
/*static int	take_dongle(t_coder *coder, int side)
{
	t_dongle	*dongle;

	if (stop_program(coder->hub))
		return (0);
	if (side)
		dongle = coder->dongle_l;
	else
		dongle = coder->dongle_r;
	pthread_mutex_lock(&dongle->lock);
	if (!dongle->is_available)
	{
		pthread_mutex_unlock(&dongle->lock);
		usleep(1000);
		return (1);
	}
	else
	{
		dongle->is_available = 0;
		pthread_mutex_unlock(&dongle->lock);
		return (0);
	}
}*/

static int	release_dongle(t_hub *hub, t_dongle	*dongle)
{
	if (stop_program(hub))
		return (1);
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

int	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = get_time(0);
	pthread_mutex_unlock(&coder->lock);
	log_status(coder->hub, coder, "is compiling");
	usleep(coder->hub->time_compile * 1000);
	pthread_mutex_lock(&coder->lock);
	coder->n_compiles += 1;
	pthread_mutex_unlock(&coder->lock);
	if (release_dongle(coder->hub, coder->dongle_l))
		return (1);
	if (release_dongle(coder->hub, coder->dongle_r))
		return (1);
	return (0);
}

int	debug(t_coder *coder)
{
	log_status(coder->hub, coder, "is debugging");
	usleep(coder->hub->time_debug * 1000);
	return (0);
}

int	refactor(t_coder *coder)
{
	log_status(coder->hub, coder, "is refactoring");
	usleep(coder->hub->time_refactor * 1000);
	return (0);
}


void    *coder_routine(void *arg)
{
    t_coder	*coder;

	coder = (t_coder *)arg;
	if (wait_for_others(coder))
		return (NULL);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = coder->hub->start_time;
	pthread_mutex_unlock(&coder->lock);
	//if (coder->hub->n_coders == 1)
	//	return (single_coder(coder, coder->dongle_l));
	if (coder->id % 2 == 0 || coder->id == coder->hub->n_coders)
	{
		if (usleep((coder->hub->time_compile * 1000) / 2))
			return (0);
	}
	while (1)
	{
		if (try_get_dongles(coder) || compile(coder)
			|| debug(coder) || refactor(coder))
			return (NULL);
	}
	return (NULL);
}



















long	get_time(long start_time)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time);
}

void	log_status(t_hub *hub, t_coder *coder, char *msg)
{
	long	time;

	time = get_time(0) - hub->start_time;
	if (time == -1)
		return ;
	pthread_mutex_lock(&hub->log_lock);
	if (stop_program(hub) || strcmp(msg, "has burned out") == 0)
	{
		pthread_mutex_unlock(&hub->log_lock);
		return ;
	}
	printf("%ld %d [D%i, D%i] %s\n", time, coder->dongle_l->id,
        coder->dongle_r->id, coder->id, msg);
	pthread_mutex_unlock(&hub->log_lock);
}

int	stop_program(t_hub *hub)
{
	int	result;

	pthread_mutex_lock(&hub->stop_lock);
	result = hub->stop_flag;
	pthread_mutex_unlock(&hub->stop_lock);
	return (result);
}
