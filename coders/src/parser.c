/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/15 16:20:39 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Parse the command line arguments and save them to the hub struct.
 */
static int	fill_args(t_hub *args, char **argv)
{
	args->n_coders = atoi(argv[1]);
	args->time_burnout = atol(argv[2]);
	args->time_compile = atol(argv[3]);
	args->time_debug = atoi(argv[4]);
	args->time_refactor = atoi(argv[5]);
	args->n_compiles_required = atoi(argv[6]);
	args->dongle_cooldown = atoi(argv[7]);
	if (!strcmp(argv[8], "fifo"))
		args->scheduler = FIFO;
	else if (!strcmp(argv[8], "edf"))
		args->scheduler = EDF;
	else
		return (1);
	return (0);
}

/**
 * Iterate through the command line arguments to ensure they are valid,
 * the parse them.
 * Called at the beginning of the main() function.
 */
int	parse_args(t_hub *hub, int argc, char **argv)
{
	int	i;
	int	j;

	i = 1;
	while (i < argc - 1)
	{
		j = 0;
		while (argv[i][j])
		{
			if (argv[i][j] < '0' || argv[i][j] > '9')
				return (1);
			j++;
		}
		i++;
	}
	return (fill_args(hub, argv));
}
