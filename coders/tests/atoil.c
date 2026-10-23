#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int	check_overflow(long n, int sign)
{
	if (n * sign > INT_MAX)
	{
		printf("exceeds limit");
		return (1);
	}
	if (n * sign < INT_MIN)
	{
		printf("exceeds limit");
		return (1);
	}
	return (0);
}

static int	ft_atoi(char *str)
{
	long int	result;
	int			sign;
	int			i;

	i = 0;
	sign = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	result = 0;
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

static int	check_arg(char *str)
{
	int	i;
	int	start;
	int limit;

	i = 0;
	limit = 10;
	start = 0;
	while (str[i])
		i++;
	if (str[0] == '-')
	{
		start = 1;
		limit = 12;
	}
	if (i > limit || str[start] > '2')
	{
		printf("exceeds character length");
		return (-1);
	}
	return (ft_atoi(str));
}

int	parse_args(int argc, char **argv)
{
	int	i;
	int	n;

	i = 1;
	while (i < argc)
	{
		n = check_arg(argv[i]);
		printf("%i ", n);
		if (n < 0)
			printf("neg");
		i++;
		printf("\n");
	}
	return (0);
}

int	main(int argc, char **argv)
{
	printf("() %i \n", INT_MAX);
	printf("() %i \n", INT_MIN);
	parse_args(argc, argv);
	return (0);
}