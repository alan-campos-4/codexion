/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:33:11 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:56:44 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile = get_curr_time();
	pthread_mutex_unlock(&coder->lock);
	if (print_log("is compiling", coder, coder->hub))
		return (FAIL);
	if (better_sleep(coder, (int)coder->hub->compile_time * MS_IN_US))
		return (FAIL);
	pthread_mutex_lock(&coder->lock);
	coder->completed_compiles += 1;
	pthread_mutex_unlock(&coder->lock);
	if (release_dongle(coder->first_dongle))
		return (FAIL);
	if (release_dongle(coder->second_dongle))
		return (FAIL);
	return (SUCCESS);
}

int	debug(t_coder *coder)
{
	if (print_log("is debugging", coder, coder->hub))
		return (FAIL);
	if (better_sleep(coder, (int)coder->hub->debug_time * MS_IN_US))
		return (FAIL);
	return (SUCCESS);
}

int	refactor(t_coder *coder)
{
	if (print_log("is refactoring", coder, coder->hub))
		return (FAIL);
	if (better_sleep(coder, (int)coder->hub->refactor_time * MS_IN_US))
		return (FAIL);
	return (SUCCESS);
}

int	wait_for_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->hub->sim_lock);
	coder->hub->ready_count++;
	while (!coder->hub->start_sim)
		pthread_cond_wait(&coder->hub->cond, &coder->hub->sim_lock);
	pthread_mutex_unlock(&coder->hub->sim_lock);
	return (SUCCESS);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (wait_for_start(coder))
		return (NULL);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile = coder->hub->start_time;
	pthread_mutex_unlock(&coder->lock);
	if (coder->hub->n_coders == 1)
		return (single_coder(coder, coder->first_dongle));
	if (coder->id % 2 == 0 || coder->id == coder->hub->n_coders)
	{
		if (better_sleep(coder,
				((int)coder->hub->compile_time * MS_IN_US) / 2))
			return (SUCCESS);
	}
	while (1)
	{
		if (get_dongles(coder) || compile(coder)
			|| debug(coder) || refactor(coder))
			return (NULL);
	}
	return (NULL);
}
