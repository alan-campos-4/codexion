/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 17:40:20 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/20 17:46:18 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongles.h"

static void	swap(t_request *x, t_request *y)
{
	t_request	tmp;

	tmp = *y;
	*y = *x;
	*x = tmp;
}

int	pick_metric(t_coder *coder)
{
	if (coder->hub->scheduler == EDF)
		return (coder->last_compile + (coder->hub->time_burnout * 1000));
	else
		return (get_time(0));
}

static void	bubble_up(t_request *requests, int i)
{
	int	parent_idx;

	while (i > 0)
	{
		parent_idx = (i - 1) / 2;
		if (requests[i].metric < requests[parent_idx].metric)
			swap(&requests[parent_idx], &requests[i]);
		else
			break ;
		i = parent_idx;
	}
}

static void	heapify(t_request *requests, int parent, int size)
{
	int	left_child_idx;
	int	right_child_idx;
	int	least;

	least = parent;
	while (1)
	{
		left_child_idx = (parent * 2) + 1;
		right_child_idx = (parent * 2) + 2;
		if (left_child_idx < size
			&& requests[left_child_idx].metric < requests[parent].metric)
			least = left_child_idx;
		if (right_child_idx < size
			&& requests[right_child_idx].metric < requests[least].metric)
			least = right_child_idx;
		if (least == parent)
			break ;
		swap(&requests[least], &requests[parent]);
		parent = least;
	}
}

void	add_request(t_heap *heap, int id, int metric)
{
	if (!heap || (heap->size == 2))
		return ;
	heap->requests[heap->size].id = id;
	heap->requests[heap->size].metric = metric;
	bubble_up(heap->requests, heap->size);
	heap->size += 1;
}

void	remove_request_top(t_heap *heap)
{
	if (!heap || !heap->size)
		return ;
	if (&heap->requests[0] != &heap->requests[heap->size - 1])
		swap(&heap->requests[0], &heap->requests[heap->size - 1]);
	heap->size -= 1;
	heapify(heap->requests, 0, heap->size);
	return ;
}

void	pre_register_heaps(t_dongle *first, t_dongle *second, t_coder *coder,
		int metric)
{
	pthread_mutex_lock(&first->lock);
	pthread_mutex_lock(&second->lock);
	add_request(&first->heap, coder->id, metric);
	add_request(&second->heap, coder->id, metric);
	pthread_mutex_unlock(&second->lock);
	pthread_mutex_unlock(&first->lock);
}
