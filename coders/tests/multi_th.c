/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multi_th.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:04:34 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/20 15:04:35 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#define TIMES_TO_COUNT 5
#define N_CODERS 5
#define TIME_BURNOUT 800
#define TIME_COMPILE 200
#define TIME_DEBUG 200
#define TIME_REFACTOR 200
#define N_COMPILES 5
#define D_COOLDOWN 5
#define SCHEDULER 1

typedef struct s_coder		t_coder;
typedef struct s_program	t_program;

typedef struct s_coder
{
	unsigned int			id;
	pthread_t				thread;
	pthread_mutex_t			lock;
	pthread_mutex_t			coder_mutex;
	t_program				*program;
	unsigned int			completed_compiles;
	unsigned long			last_compile_time;
}							t_coder;

typedef struct s_program
{
	int						start_flag;
	int						stop_flag;
	unsigned int			count;
	unsigned long			start_time;
	unsigned long			curr_time;
	t_coder					*coders;
	pthread_mutex_t			lock;
	pthread_mutex_t			print_lock;
	pthread_cond_t			cond;
}							t_program;

void	log_status(t_coder *coder, char *msg)
{
	pthread_mutex_lock(&coder->program->print_lock);
	printf("Thread %u: %s - %u. Time %lu.\n", coder->id, msg,
		coder->completed_compiles, coder->program->curr_time);
	pthread_mutex_unlock(&coder->program->print_lock);
}

void	*thread_routine(void *data)
{
	unsigned int	i;
	t_coder			*coder;

	coder = (t_coder *)data;
	log_status(coder, "start");
	i = 0;
	while (coder->program->stop_flag != 1)
	{
		//if (coder->program->stop_flag == 1)
			//break ;
		pthread_mutex_lock(&coder->coder_mutex);
		usleep(TIME_BURNOUT * 100);
		coder->completed_compiles++;
		coder->program->curr_time = get_time(coder->program->start_time);
		coder->last_compile_time = coder->program->curr_time;
		log_status(coder, "count");
		// if (program->count % 10 == 0)
		pthread_mutex_unlock(&coder->coder_mutex);
		i++;
	}
	log_status(coder, "_end_");
	return (NULL); // Thread termine ici.
}

int	burnout_handle(t_coder *coders, int i, int last_compile_time)
{
	if (last_compile_time)
	{
		if (get_time(last_compile_time) < TIME_BURNOUT)
			return (0);
	}
	else
	{
		if (get_time(coders->program->start_time) < TIME_BURNOUT)
			return (0);
	}
	pthread_mutex_lock(&coders->program->lock);
	coders->program->stop_flag = 1;
	pthread_mutex_unlock(&coders->program->lock);
	log_status(&coders[i], "has burned out");
	// wakeup_coders(coders);
	return (1);
}

int	reset_monitor(int *i, int *all_done)
{
	if (*all_done)
	{
		return (1);
	}
	*i = 0;
	*all_done = 1;
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	int		all_done;
	int		i;
	int		c;

	coders = (t_coder *)arg;
	i = 0;
	c = 0;
	all_done = 1;
	while (1)
	{
		// pthread_mutex_lock(&coders[i].lock);
		// completed_compiles = coders[i].completed_compiles;
		//last_compile_time = coders[i].last_compile_time;
		//if (burnout_handle(coders, i++, last_compile_time))
			//break ;
		for (int i=0; i < N_CODERS; i++)
		{
			if (coders[i].completed_compiles == N_COMPILES)
				c++;
		}
		printf("%i threads completed\n", c);
		if (c == N_CODERS)
			break ;
	}
	return (NULL);
}

int	create_monitor(pthread_t *monitor, t_coder *coders)
{
	if (!pthread_create(monitor, NULL, monitor_routine, coders))
		return (0);
	/*pthread_mutex_lock(&coders->program->lock);
	coders->program->stop_flag = 1;
	coders->program->start_flag = 1;
	pthread_cond_broadcast(&coders->program->cond);
	pthread_mutex_unlock(&coders->program->lock);*/
	return (1);
}

int	main1(void)
{
	t_coder *coders;
	t_program program;
	pthread_t monitor;
	int monitor_status;

	// program.count = 0;
	// program.start_time = get_time(0);
	// program.curr_time = get_time(0);
	coders = malloc(sizeof(t_coder) * N_CODERS);
	if (!coders)
		return (1);
	for (int i = 0; i < N_CODERS; i++)
	{
		coders[i].id = i + 1;
		coders[i].completed_compiles = 0;
		coders[i].program = &program;
		pthread_mutex_init(&coders[i].coder_mutex, NULL);
		pthread_mutex_init(&coders[i].lock, NULL);
	}
	monitor_status = create_monitor(&monitor, coders);
	if (monitor_status == 1)
		return (1);
	program.count = 0;
	program.start_flag = 1;
	program.stop_flag = 0;
	program.start_time = get_time(0);
	program.curr_time = get_time(program.start_time);
	program.coders = coders;
	pthread_mutex_init(&program.print_lock, NULL);
	printf("Main: Expected count is %u\n", N_CODERS * TIMES_TO_COUNT);
	// Thread creation:
	for (int i = 0; i < N_CODERS; i++)
	{
		pthread_create(&coders[i].thread, NULL, thread_routine, &coders[i]);
	}
	// Thread joining:
	for (int i = 0; i < N_CODERS; i++)
	{
		pthread_join(coders[i].thread, NULL);
		printf("Main: Joined thread [%i]\n", coders[i].id);
	}
	pthread_join(monitor, NULL);
	printf("Main: Joined monitor\n");

	if (program.count != (unsigned)(N_CODERS * TIMES_TO_COUNT))
		printf("Main: ERROR ! Total count is %u\n", program.count);
	else
		printf("Main: OK. Total count is %u\n", program.count);
	// Destroy the mutex at the end of the program:
	for (int i = 0; i < N_CODERS; i++)
	{
		pthread_mutex_destroy(&coders[i].coder_mutex);
		pthread_mutex_destroy(&coders[i].lock);
	}
	pthread_mutex_destroy(&program.print_lock);
	free(coders);
	// free(program);
	return (0);
}