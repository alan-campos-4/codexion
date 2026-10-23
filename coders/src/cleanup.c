/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:06:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 19:04:54 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	dongle_cleanup(t_dongle *dongle, int n, int mutex_c, int cond_c)
{
	int	i;

	if (!dongle)
		return ;
	i = 0;
	while (i < n)
	{
		if (mutex_c-- > 0)
			pthread_mutex_destroy(&dongle[i].lock);
		if (cond_c-- > 0)
			pthread_cond_destroy(&dongle[i].cond);
		i++;
	}
	free(dongle);
}

/**
 * Cleans
 */
void	coder_cleanup(t_coder *coders, int n, int mutex_c)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < n)
	{
		if (mutex_c-- > 0)
			pthread_mutex_destroy(&coders[i].lock);
		i++;
	}
	free(coders);
}

/**
 * Returns whether the number is divisible by the given dividend.
 */
static int check_flag(int *num, int div)
{
	if (*num == 0)
        return (0);
    if (*num >= div)
    {
        *num = *num - div;
        return (1);
    }
    return (0);
}

/**
 * Cleans the hub attributes depending on the flag.
 * This entire 'check_flag' system in necessary because I cannot
 * pass more than four arguments in a function :)))))
 */
int	hub_cleanup(t_hub *hub, int flag)
{
	int	cond;
	int	lock;
	int	log_lock;
	int	stop_lock;

	cond = check_flag(&flag, 8);
    lock = check_flag(&flag, 4);
    log_lock = check_flag(&flag, 2);
    stop_lock = check_flag(&flag, 1);
	if (!hub)
		return (1);
	if (stop_lock)
		pthread_mutex_destroy(&hub->stop_lock);
	if (log_lock)
		pthread_mutex_destroy(&hub->log_lock);
	if (lock)
		pthread_mutex_destroy(&hub->lock);
	if (cond)
		pthread_cond_destroy(&hub->cond);
	return (1);
}

/**
 * Cleans and frees all the variables created for the program.
 */
int	cleanup_all(t_dongle *dongles, t_coder *coders, t_hub *hub)
{
	int n;

	n = 0;
	if (hub)
		n = hub->n_coders;
	dongle_cleanup(dongles, n, n, n);
	coder_cleanup(coders, n, n);
	hub_cleanup(hub, 16);
	return (0);
}