/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/11/04 18:36:36 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Checks that the number given is within the range and INT should have.
 */
static int	is_int(long n, int sign, long max_limit, long min_limit)
{
	if (n * sign > max_limit)
		return (1);
	if (n * sign < min_limit)
		return (1);
	return (0);
}

/**
 * Custom 'ascii to number' parser that prevents overflow or underflow.
 * Can work as atoi or atol, depending on the limits given.
 * Returns -1 if at any point the string passed contains invalid characters
 * or exceeds the limits of an int.
 */
int	ft_aton(char *str, long max_limit, long min_limit)
{
	long	result;
	int		sign;
	int		i;

	i = 0;
	sign = 1;
	result = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		result = (str[i] - '0') + (result * 10);
		if (is_int(result, sign, max_limit, min_limit))
			return (-1);
		i++;
	}
	return ((int)result * sign);
}

/**
 * Assign all the values found to the corresponding values in the hub.
 */
static int	fill_hub(t_hub *hub, long *time_values, int *values,
		char *scheduler)
{
	hub->n_coders = values[0];
	hub->n_compiles_required = values[1];
	hub->burnout_time = time_values[0];
	hub->compile_time = time_values[1];
	hub->debug_time = time_values[2];
	hub->refactor_time = time_values[3];
	hub->cooldown = time_values[4];
	if (!strcmp(scheduler, "fifo"))
		hub->scheduler = FIFO;
	else if (!strcmp(scheduler, "edf"))
		hub->scheduler = EDF;
	else
		return (1);
	return (0);
}

/**
 * Iterate through the command line arguments the program receives,
 * and stores them in arrays to then assign them to the hub attributes.
 */
int	parse_args(int argc, char **argv, t_hub *hub)
{
	long	time_values[6];
	int		values[2];
	int		tv_counter;
	int		val_counter;
	int		i;

	if (argc != 9)
		return (1);
	i = 1;
	tv_counter = 0;
	val_counter = 0;
	while (i < argc - 1)
	{
		if (i == 1 || i == 6)
		{
			if (add_int_value(values, &val_counter, argv[i]))
				return (1);
		}
		else
		{
			if (add_time_value(time_values, &tv_counter, argv[i]))
				return (1);
		}
		i++;
	}
	return (fill_hub(hub, time_values, values, argv[8]));
}
