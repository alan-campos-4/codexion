/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:35:37 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:08:48 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wakeup_coders(t_coder *coders)
{
	int	i;

	pthread_mutex_lock(&coders->hub->lock);
	pthread_cond_broadcast(&coders->hub->cond);
	pthread_mutex_unlock(&coders->hub->lock);
	i = 0;
	while (i < coders->hub->n_coders)
	{
		pthread_mutex_lock(&coders[i].dongle_l->lock);
		pthread_cond_broadcast(&coders[i].dongle_l->cond);
		pthread_mutex_unlock(&coders[i].dongle_l->lock);
		pthread_mutex_lock(&coders[i].dongle_r->lock);
		pthread_cond_broadcast(&coders[i].dongle_r->cond);
		pthread_mutex_unlock(&coders[i].dongle_r->lock);
		i++;
	}
}

int	burnout_handle(t_coder *coders, int i, int last_compile)
{
	if (last_compile)
	{
		if (get_time(0) - last_compile
			< (int)coders->hub->time_burnout * 1000)
			return (0);
	}
	else
	{
		if (get_time(0) - coders->hub->start_time
			< (int)coders->hub->time_burnout * 1000)
			return (0);
	}
	pthread_mutex_lock(&coders->hub->lock);
	coders->hub->stop_flag = 1;
	pthread_mutex_unlock(&coders->hub->lock);
	log_status(coders->hub, &coders[i], "burned out");
	wakeup_coders(coders);
	return (1);
}

int	routine_primer(t_coder *coders, int *i, int *all_done)
{
	if (*all_done)
	{
		pthread_mutex_lock(&coders->hub->lock);
		coders->hub->stop_flag = 1;
		pthread_mutex_unlock(&coders->hub->lock);
		wakeup_coders(coders);
		return (1);
	}
	*i = 0;
	*all_done = 1;
	usleep(100);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder		*coders;
	int		all_done;
	int		i;
	int		last_compile;
	int		completed_compiles;

	coders = (t_coder *)arg;
	if (wait_for_others(coders->hub))
		return (NULL);
	i = 0;
	all_done = 1;
	while (1)
	{
		pthread_mutex_lock(&coders[i].lock);
		completed_compiles = coders[i].n_compiles;
		last_compile = coders[i].last_compile;
		pthread_mutex_unlock(&coders[i].lock);
		if (burnout_handle(coders, i++, last_compile))
			return (NULL);
		if (completed_compiles < coders->hub->n_compiles_required)
			all_done = 0;
		if (i == coders->hub->n_coders
			&& routine_primer(coders, &i, &all_done))
			return (NULL);
	}
}



/*
// Check if the coder has reached burnout
static int	coder_is_in_burnout(t_hub *hub, t_coder *coder)
{
	//long	time_since_last_compile;
	long	last_compile;

	pthread_mutex_lock(&coder->time_lock);
	last_compile = coder->last_compile;
	pthread_mutex_unlock(&coder->time_lock);
	//time_since_last_compile = get_time(0) - last_compile;
	return (get_time(0) - last_compile >= (hub->time_burnout * 1000));
}

// Check if the coder has reached the number of compiles required
static int	coder_has_finished(t_hub *hub, t_coder *coder)
{
	int	compiled;

	pthread_mutex_lock(&coder->time_lock);
	compiled = coder->n_compiles;
	pthread_mutex_unlock(&coder->time_lock);
	return (compiled >= hub->n_compiles_required);
}


Loop through every coder once: log and stop early if anyone is
in burnout, otherwise track whether all of them have reached the
required number of compiles.

static int	check_all_coders(t_hub *hub)
{
	t_coder	*coder;
	int		i;
	int		all_finished;

	i = 0;
	all_finished = 1;
	while (i < hub->n_coders)
	{
		coder = &hub->coders[i];
		if (coder_is_in_burnout(hub, coder))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->stop_flag = 1;
			pthread_mutex_unlock(&hub->stop_lock);
			log_status(hub, coder, LOG_BURNOUT);
			return (1);
		}
		if (!coder_has_finished(hub, coder))
			all_finished = 0;
		i++;
	}
	return (all_finished);
}


Monitor thread: repeatedly checks all coders until either a
burnout is detected or everyone has finished compiling — then
sets should_stop and exits.

void	*monitor_routine(void *arg)
{
	t_hub	*hub;

	hub = (t_hub *)arg;
	while (1)
	{
		if (check_all_coders(hub))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->stop_flag = 1;
			pthread_mutex_unlock(&hub->stop_lock);
			return (NULL);
		}
		usleep(2000);
	}
}*/
