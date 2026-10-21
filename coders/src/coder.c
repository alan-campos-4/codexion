/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:33:11 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 17:07:17 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile = get_time(0);
	pthread_mutex_unlock(&coder->lock);
	if (log_status("is compiling", coder, coder->hub))
		return (1);
	if (better_sleep(coder, (int)coder->hub->compile_time * MS_IN_US))
		return (1);
	pthread_mutex_lock(&coder->lock);
	coder->completed_compiles += 1;
	pthread_mutex_unlock(&coder->lock);
	if (release_dongle(coder->first_dongle))
		return (1);
	if (release_dongle(coder->second_dongle))
		return (1);
	return (0);
}

int	debug(t_coder *coder)
{
	if (log_status("is debugging", coder, coder->hub))
		return (1);
	if (better_sleep(coder, (int)coder->hub->debug_time * MS_IN_US))
		return (1);
	return (0);
}

int	refactor(t_coder *coder)
{
	if (log_status("is refactoring", coder, coder->hub))
		return (1);
	if (better_sleep(coder, (int)coder->hub->refactor_time * MS_IN_US))
		return (1);
	return (0);
}

int	wait_for_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->hub->lock);
	coder->hub->ready_count++;
	while (!coder->hub->start_flag)
		pthread_cond_wait(&coder->hub->cond, &coder->hub->lock);
	pthread_mutex_unlock(&coder->hub->lock);
	return (0);
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
			return (0);
	}
	while (1)
	{
		if (get_dongles(coder) || compile(coder)
			|| debug(coder) || refactor(coder))
			return (NULL);
	}
	return (NULL);
}
