/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:34:23 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 14:13:19 by alvespin         ###   ########.fr       */
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
 * Initialize the coder's threads.
 * Delete what has been assigned if anything goes wrong.
 */
pthread_t	*create_coder_threads(t_coder *coders)
{
	pthread_t	*t;
	int			i;

	t = malloc(sizeof(pthread_t) * coders->hub->n_coders);
	if (!t)
		return (NULL);
	i = 0;
	while (i < coders->hub->n_coders)
	{
		if (pthread_create(t + i, NULL, &coder_routine, coders + i))
		{
			pthread_mutex_lock(&coders->hub->lock);
			coders->hub->start_flag = 1;
			coders->hub->stop_flag = 1;
			pthread_mutex_unlock(&coders->hub->lock);
			wakeup_coders(coders);
			while (i-- > 0)
				pthread_join(t[i], NULL);
			return (free(t), NULL);
		}
		i++;
	}
	return (t);
}

/**
 * Return whether the creation of the monitor_thread thread was successful.
 */
static int	create_monitor_thread(pthread_t *monitor_thread, t_hub *hub)
{
	if (!pthread_create(monitor_thread, NULL, monitor_routine, hub))
		return (0);
	pthread_mutex_lock(&hub->lock);
	hub->stop_flag = 1;
	hub->start_flag = 1;
	pthread_cond_broadcast(&hub->cond);
	pthread_mutex_unlock(&hub->lock);
	return (1);
}

/**
 * Create the coder and monitor_thread threads and start the program.
 */
static int	codexion(t_hub *hub, t_coder *coders)
{
	pthread_t	*coder_threads;
	pthread_t	monitor_thread;
	int			monitor_status;

	coder_threads = create_coder_threads(coders);
	if (!coder_threads)
		return (1);
	monitor_status = create_monitor_thread(&monitor_thread, hub);
	if (monitor_status == 0)
	{
		wait_for_start(hub);
		pthread_join(monitor_thread, NULL);
	}
	join_threads(coder_threads, hub->n_coders);
	free(coder_threads);
	return (monitor_status);
}

int	main(int argc, char **argv)
{
	t_hub		hub;
	t_coder		*coders;
	t_dongle	*dongles;
	int			start;

	if (parse_args(argc, argv, &hub))
		return (1);
	dongles = init_dongles(hub.n_coders);
	coders = init_coders(hub.n_coders);
	if (!coders || !dongles)
		return (1);
	if (setup_program(&hub, coders, dongles))
		return (1);
	start = codexion(&hub, coders);
	cleanup_all(&hub, coders, dongles);
	return (start);
}
