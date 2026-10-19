/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:33:11 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/19 11:57:54 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
Lock the request_counter to increase its value
When a coder requests a dongle ("priority waiting list")
*/
void	assign_request_order(t_hub *hub, t_coder *coder)
{
	pthread_mutex_lock(&hub->request_lock);
	hub->request_counter++;
	coder->request_order = hub->request_counter;
	pthread_mutex_unlock(&hub->request_lock);
}

/**
 * Print
 */
static int	coder_compile(t_hub *hub, t_coder *coder)
{
	if (!take_both_dongles(hub, coder))
		return (0);
	pthread_mutex_lock(&coder->time_lock);
	coder->last_compile = get_time(0);
	pthread_mutex_unlock(&coder->time_lock);
	log_status(hub, coder, LOG_COMPILE);
	usleep(hub->time_compile * 1000);
	pthread_mutex_lock(&coder->time_lock);
	coder->n_compiles++;
	pthread_mutex_unlock(&coder->time_lock);
	release_both_dongle(coder);
	return (1);
}

/**
 * Coder
 */
void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_hub	*hub;

	coder = (t_coder *)arg;
	hub = coder->hub;
	while (!check_stop_flag(hub))
	{
		if (!coder_compile(hub, coder))
			break ;
		if (check_stop_flag(hub))
			break ;
		//sleep_phase(hub, coder, hub->time_debug, LOG_DEBUG);
		log_status(hub, coder, LOG_DEBUG);
		usleep(hub->time_debug * 1000);
		if (check_stop_flag(hub))
			break ;
		//sleep_phase(hub, coder, hub->time_refactor, LOG_REFACTOR);
		log_status(hub, coder, LOG_REFACTOR);
		usleep(hub->time_refactor * 1000);
	}
	return (NULL);
}
