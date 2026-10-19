/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:34:23 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/19 17:38:28 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->n_coders)
	{
		free(hub->dongles[i].heap.data);
		pthread_mutex_destroy(&hub->dongles[i].lock);
		pthread_cond_destroy(&hub->dongles[i].cond);
		pthread_mutex_destroy(&hub->coders[i].time_lock);
		i++;
	}
	pthread_mutex_destroy(&hub->log_lock);
	pthread_mutex_destroy(&hub->stop_lock);
	pthread_mutex_destroy(&hub->request_lock);
	free(hub->dongles);
	free(hub->coders);
}

static void	create_all_threads(t_hub *hub, pthread_t *monitor)
{
	int	i;

	i = 0;
	while (i < hub->n_coders)
	{
		pthread_create(&hub->coders[i].thread, NULL, coder_routine,
			&hub->coders[i]);
		i++;
	}
	pthread_create(monitor, NULL, monitor_routine, hub);
}

static void	join_all_threads(t_hub *hub, pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < hub->n_coders)
	{
		pthread_join(hub->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_hub		hub;
	pthread_t	monitor_thread;

	if (parse_args(&hub, argc, argv) == 1)
		return (1);
	if (setup_program(&hub) == 1)
		return (1);
	create_all_threads(&hub, &monitor_thread);
	join_all_threads(&hub, monitor_thread);
	cleanup(&hub);
	return (0);
}
