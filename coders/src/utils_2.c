/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:00:27 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/26 17:27:37 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * To perform after creating the coder and monitor threads.
 * Start the program after all the actors have indicated they are ready.
 * Set the start_flag, the start_time, and wake up the waiting threads.
 */
void	wait_for_start(t_hub *hub)
{
	while (1)
	{
		pthread_mutex_lock(&hub->lock);
		if (hub->ready_count == hub->n_coders + 1)
			break ;
		pthread_mutex_unlock(&hub->lock);
		usleep(MS_TO_US);
	}
	hub->start_time = get_time(0);
	hub->start_flag = 1;
	pthread_cond_broadcast(&hub->cond);
	pthread_mutex_unlock(&hub->lock);
}

/**
 * To perform by the coder and monitor threads when they start.
 * Indicate the actor that calls it is ready and wait
 * until all other actors in the program are ready.
 */
int	wait_for_others(t_hub *hub)
{
	pthread_mutex_lock(&hub->lock);
	hub->ready_count++;
	while (!hub->start_flag)
		pthread_cond_wait(&hub->cond, &hub->lock);
	pthread_mutex_unlock(&hub->lock);
	return (0);
}

/**
 * Safely broadcast to the condition variables in the hub and 
 * in each dongle of each coder.
 */
void	wakeup_coders(t_coder *coders)
{
	t_hub		*hub;
	t_dongle	*dl;
	t_dongle	*dr;
	int			i;

	hub = coders->hub;
	pthread_mutex_lock(&hub->lock);
	pthread_cond_broadcast(&hub->cond);
	pthread_mutex_unlock(&hub->lock);
	i = 0;
	while (i < hub->n_coders)
	{
		dl = coders[i].dongle_l;
		dr = coders[i].dongle_r;
		pthread_mutex_lock(&dl->lock);
		pthread_cond_broadcast(&dl->cond);
		pthread_mutex_unlock(&dl->lock);
		pthread_mutex_lock(&dr->lock);
		pthread_cond_broadcast(&dr->cond);
		pthread_mutex_unlock(&dr->lock);
		i++;
	}
}
