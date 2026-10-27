/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:06:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 11:01:11 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Frees the dongle array, destroys its mutexes and its conds.
 * @param mutex_c The amount of coders whose pthread_mutex has been initialized.
 * @param cond_c The amount of coders whose pthread_cond has been initialized.
 */
void	cleanup_dongles(t_dongle *dongle, int n, int mutex_c, int cond_c)
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
 * Frees the coder array and destroys its mutexes.
 * @param mutex_c The amount of coders whose pthread_mutex has been initialized.
 */
void	cleanup_coders(t_coder *coders, int n, int mutex_c)
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
static int	check_flag(int *num, int div)
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
 * @param flag An integer (0-16) that, when interpreted as four bits,
 * 		determines which hub attibutes should be destroyed.
 * This entire 'decimal to binary' system has been implemented because
 * of the four-arguments-per-function restriction.
 */
int	cleanup_hub(t_hub *hub, int flag)
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
int	cleanup_all(t_hub *hub, t_coder *coders, t_dongle *dongles)
{
	int	n;

	n = 0;
	if (hub)
		n = hub->n_coders;
	cleanup_dongles(dongles, n, n, n);
	cleanup_coders(coders, n, n);
	cleanup_hub(hub, 16);
	return (0);
}
