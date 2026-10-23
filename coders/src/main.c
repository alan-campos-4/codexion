/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:34:23 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/22 17:27:43 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

static int	create_monitor(pthread_t *monitor, t_coder *coders)
{
	if (!pthread_create(monitor, NULL, monitor_routine, coders))
		return (0);
	pthread_mutex_lock(&coders->hub->lock);
	coders->hub->stop_flag = 0;
	coders->hub->start_flag = 0;
	pthread_cond_broadcast(&coders->hub->cond);
	pthread_mutex_unlock(&coders->hub->lock);
	return (1);
}

int	codexion(t_coder *coders, t_dongle *dongles, t_hub *hub)
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
	monitor_status = create_monitor(&monitor, coders);
	if (monitor_status == 1)
		join_threads(c_threads, hub->n_coders);
	else
	{
		wait_for_start(coders);
		join_threads(c_threads, hub->n_coders);
		pthread_join(monitor, NULL);
	}
	free(c_threads);
	cleanup_all(dongles, coders, hub);
	return (monitor_status);
}

int	main(int ac, char **av)
{
	t_hub		hub;
	t_coder		*coders;
	t_dongle	*dongles;
	int			init_status;

	if (parse_args(ac, av, &hub))
	{
		return (1);
	}
	init_status = init_sim(&hub, &coders, &dongles);
	//if (init_status == 0)
	//	return (codexion(coders, dongles, &hub));
	return (init_status);
}
