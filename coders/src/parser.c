/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 12:08:25 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_overflow(long int n, int sign)
{
	if (n * sign > INT_MAX)
		return (1);
	if (n * sign < INT_MIN)
		return (1);
	return (0);
}

static int	ft_atoi(char *str)
{
	long int	result;
	int			sign;
	int			i;

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
		if (check_overflow(result, sign))
			return (-1);
		i++;
	}
	return (int)(result * sign);
}

static int	fill_hub(t_hub *hub, char **argv)
{
	hub->n_coders = atoi(argv[1]);
	hub->burnout_time = atoi(argv[2]);
	hub->compile_time = atoi(argv[3]);
	hub->debug_time = atoi(argv[4]);
	hub->refactor_time = atoi(argv[5]);
	hub->n_compiles_required = atoi(argv[6]);
	hub->cooldown = atoi(argv[7]);
	if (!strcmp(argv[8], "fifo"))
		hub->scheduler = FIFO;
	else if (!strcmp(argv[8], "edf"))
		hub->scheduler = EDF;
	else
		return (1);
	return (0);
}

int	parse_args(int argc, char **argv, t_hub *hub)
{
	int	i;
	int	n;

	if (argc != 9)
		return (1);
	i = 1;
	while (i < argc - 1)
	{
		n = ft_atoi(argv[i]);
		if (n <= 0)
			return (-1);
		i++;
	}
	return (fill_hub(hub, argv));
}
