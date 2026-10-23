/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:01:58 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 19:05:35 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Initialize the dongle array.
 * Delete what has been assigned if anything goes wrong.
 */
t_dongle	*init_dongles(int n)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle) * n);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < n)
	{
		dongles[i].heap.size = 0;
		dongles[i].is_available = 1;
		dongles[i].last_use_time = 0;
		if (pthread_mutex_init(&dongles[i].lock, NULL))
			return (dongle_cleanup(dongles, n, i, i), NULL);
		if (pthread_cond_init(&dongles[i].cond, NULL))
			return (dongle_cleanup(dongles, n, i + 1, i), NULL);
		i++;
	}
	return (dongles);
}

/**
 * Initialize the coder array.
 * Delete what has been assigned if anything goes wrong.
 */
t_coder	*init_coders(int n)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * n);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < n)
	{
		coders[i].id = i + 1;
		coders[i].n_compiles = 0;
		coders[i].last_compile_time = 0;
		if (pthread_mutex_init(&coders[i].lock, NULL))
			return (coder_cleanup(coders, n, i), NULL);
		i++;
	}
	return (coders);
}

/*
if	(pthread_mutex_init(&coders[i].lock, NULL))
		{
			while (i-- > 0)
				pthread_mutex_destroy(&coders[i].lock);
			free(coders);
			return (NULL);
		}
*/

int	init_hub_2(t_hub *hub)
{
	int	stop_lock_flag;
	int	log_lock_flag;
	int	lock_flag;

	stop_lock_flag = 0;
	log_lock_flag = 0;
	lock_flag = 0;
	if (pthread_mutex_init(&hub->stop_lock, NULL))
		return (1);
	if (pthread_mutex_init(&hub->log_lock, NULL))
		stop_lock_flag = 1;
	if (pthread_mutex_init(&hub->lock, NULL))
		log_lock_flag = 1;
	if (pthread_cond_init(&hub->cond, NULL))
		log_lock_flag = 1;
	if (stop_lock_flag || log_lock_flag || lock_flag)
		return (hub_cleanup(hub, (lock_flag * 4 + log_lock_flag * 2
				+ stop_lock_flag * 1)));
	return (0);
}

/**
 * Initialize the coder array.
 * Delete what has been assigned if anything goes wrong.
 * Please the hub_cleanup() docstring for more info.
 */
int	init_hub(t_hub *hub)
{
	int	stop_lock_flag;
	int	log_lock_flag;
	int	lock_flag;

	if (!hub)
		return (1);
	hub->stop_flag = 0;
	hub->start_flag = 0;
	hub->ready_count = 0;
	stop_lock_flag = 0;
	log_lock_flag = 0;
	lock_flag = 0;
	if (pthread_mutex_init(&hub->stop_lock, NULL))
		return (1);
	if (pthread_mutex_init(&hub->log_lock, NULL))
		stop_lock_flag = 1;
	if (pthread_mutex_init(&hub->lock, NULL))
		log_lock_flag = 1;
	if (pthread_cond_init(&hub->cond, NULL))
		log_lock_flag = 1;
	if (stop_lock_flag || log_lock_flag || lock_flag)
		return (hub_cleanup(hub, (lock_flag * 4 + log_lock_flag * 2
				+ stop_lock_flag * 1)));
	return (0);
}
