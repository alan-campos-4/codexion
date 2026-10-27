/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:44:02 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 14:09:15 by alvespin         ###   ########.fr       */
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
			return (cleanup_dongles(dongles, n, i, i), NULL);
		if (pthread_cond_init(&dongles[i].cond, NULL))
			return (cleanup_dongles(dongles, n, i + 1, i), NULL);
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
			return (cleanup_coders(coders, n, i), NULL);
		i++;
	}
	return (coders);
}

/**
 * Initialize the hub attributes that are not obtained from the
 * arguments, deleting what has been assigned if anything goes wrong.
 * Please the cleanup_hub() docstring for more info.
 */
static int	init_hub(t_hub *hub)
{
	int	stop_lock_flag;
	int	log_lock_flag;
	int	lock_flag;

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
		lock_flag = 1;
	if (lock_flag || log_lock_flag || stop_lock_flag)
		return (cleanup_hub(hub, (lock_flag * 4 + log_lock_flag * 2
					+ stop_lock_flag * 1)));
	return (0);
}

/**
 * Execute after parsing the program arguments.
 * Set up all the variables and attributes that are not obtained
 * fromt the command line.
 */
int	setup_program(t_hub *hub, t_coder *coders, t_dongle *dongles)
{
	int	i;

	if (!hub || !coders || !dongles)
		return (1);
	if (init_hub(hub))
		return (1);
	i = 0;
	hub->coders = coders;
	while (i < hub->n_coders)
	{
		coders[i].hub = hub;
		coders[i].dongle_l = &dongles[i];
		coders[i].dongle_r = (&dongles[(i + 1) % hub->n_coders]);
		i++;
	}
	return (0);
}
