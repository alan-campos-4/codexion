/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:33:11 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:09:23 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	wait_for_others(t_hub *hub)
{
	pthread_mutex_lock(&hub->lock);
	hub->ready_count++;
	//log_status(coder->hub, coder, "is ready");
	while (hub->start_flag != 1)
		pthread_cond_wait(&hub->cond, &hub->lock);
	pthread_mutex_unlock(&hub->lock);
	return (0);
}

int	release_dongle(t_dongle	*dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	dongle->last_use = get_time(0);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);;
	return (0);
}

int	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile = get_time(0);
	pthread_mutex_unlock(&coder->lock);
	log_status(coder->hub, coder, "is compiling");
	usleep(coder->hub->time_compile * 1000);
	pthread_mutex_lock(&coder->lock);
	coder->n_compiles += 1;
	pthread_mutex_unlock(&coder->lock);
	if (release_dongle(coder->dongle_l))
		return (1);
	if (release_dongle(coder->dongle_r))
		return (1);
	return (0);
}

void	*single_coder(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->lock);
	log_status(coder->hub, coder, "has taken dongle");
	usleep(coder->hub->time_burnout * 1000);
	return (NULL);
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
	if (wait_for_others(coder->hub))
		return (NULL);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile = coder->hub->start_time;
	pthread_mutex_unlock(&coder->lock);
	if (coder->hub->n_coders == 1)
		return (single_coder(coder, coder->dongle_l));
	if (coder->id % 2 == 0 || coder->id == coder->hub->n_coders)
	{
		if (usleep((coder->hub->time_compile * 1000) / 2))
			return (0);
	}
	while (1)
	{
		if (get_dongles(coder) || compile(coder)
			|| debug(coder) || refactor(coder))
			return (NULL);
	}
	return (NULL);
}
