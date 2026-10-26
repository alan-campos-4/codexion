/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:34:23 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/26 17:06:51 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Join the given threads.
 */
static void	join_threads(pthread_t *ts, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_join(ts[i], NULL);
		i++;
	}
}

/**
 * Return whether the creation of the monitor thread was successful.
 */
static int	create_monitor(pthread_t *monitor, t_hub *hub)
{
	if (!pthread_create(monitor, NULL, monitor_routine, hub))
		return (0);
	pthread_mutex_lock(&hub->lock);
	hub->stop_flag = 1;
	hub->start_flag = 1;
	pthread_cond_broadcast(&hub->cond);
	pthread_mutex_unlock(&hub->lock);
	return (1);
}

/**
 * ...
 */
int	codexion(t_hub *hub, t_coder *coders, t_dongle *dongles)
{
	pthread_t	*c_threads;
	pthread_t	monitor;
	int			monitor_status;

	c_threads = init_threads(coders);
	if (!c_threads)
	{
		cleanup_all(dongles, coders, hub);
		return (1);
	}
	monitor_status = create_monitor(&monitor, hub);
	if (monitor_status == 1)
		join_threads(c_threads, hub->n_coders);
	else
	{
		wait_for_start(hub);
		join_threads(c_threads, hub->n_coders);
		pthread_join(monitor, NULL);
	}
	free(c_threads);
	cleanup_all(dongles, coders, hub);
	return (monitor_status);
}

int	main(int argc, char **argv)
{
	t_hub		hub;
	t_coder		*coders;
	t_dongle	*dongles;
	int			init_status;

	if (parse_args(argc, argv, &hub))
		return (1);
	dongles = init_dongles(hub.n_coders);
	coders = init_coders(hub.n_coders);
	if (!coders || !dongles)
		return (1);
	init_status = setup_program(&hub, coders, dongles);
	if (init_status == 0)
		return (codexion(&hub, coders, dongles));
	return (init_status);
}
