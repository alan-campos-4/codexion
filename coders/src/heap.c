/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:47:02 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/15 18:47:04 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
- sift_up: moves a too-small node up, swapping with its parent,
until the min-heap property holds again.
When it's used: right after a new element is added at the very
bottom (end) of the array — it needs to travel up to its correct spot.
*/
static void	sift_up(t_heap *heap, int i, t_hub *hub)
{
	int	parent_i;

	while (i > 0)
	{
		parent_i = (i - 1) / 2;
		if (!is_higher_priority(hub, heap->data[i], heap->data[parent_i]))
			return ;
		ft_swap(&heap->data[i], &heap->data[parent_i]);
		i = parent_i;
	}
}

/*
- sift_down: moves a too-large node down, swapping with its
SMALLER child, until the min-heap property holds again.
When it's used: right after removing the root (the min) and
moving the last leaf into its place — that displaced element
needs to sink down to where it actually belongs.
- get_smaller_child: assistant to the sift_down function
*/
static int	get_smaller_child(t_heap *heap, int i, t_hub *hub)
{
	int		smaller_i;
	int		left_i;
	int		right_i;

	left_i = 2 * i + 1;
	right_i = 2 * i + 2;
	smaller_i = i;
	if (left_i < heap->size
		&& is_higher_priority(hub, heap->data[left_i],
			heap->data[smaller_i]))
		smaller_i = left_i;
	if (right_i < heap->size
		&& is_higher_priority(hub, heap->data[right_i],
			heap->data[smaller_i]))
		smaller_i = right_i;
	return (smaller_i);
}

static void	sift_down(t_heap *heap, int i, t_hub *hub)
{
	int				smaller_i;

	while (1)
	{
		smaller_i = get_smaller_child(heap, i, hub);
		if (smaller_i == i)
			return ;
		ft_swap(&heap->data[i], &heap->data[smaller_i]);
		i = smaller_i;
	}
}

/*
Add an elements from the heap.
push calls sift_up (new node climbs to its correct spot).
*/
int	min_heap_push(t_hub *hub, t_heap *heap, t_coder *new_coder)
{
	if (heap->size >= hub->n_coders)
		return (1);
	heap->data[heap->size] = new_coder;
	heap->size++;
	sift_up(heap, heap->size - 1, hub);
	return (0);
}

/*
Remove an elements from the heap and return it.
The one to return is always the first one in the array since
its already sorted.
pop calls sift_down (root replacement sinks to its correct spot).
*/
t_coder	*min_heap_pop(t_heap *heap, t_hub *hub)
{
	t_coder	*last_element;
	t_coder	*first_element;

	if (heap->size <= 0)
		return (NULL);
	last_element = heap->data[heap->size - 1];
	first_element = heap->data[0];
	heap->data[0] = last_element;
	heap->size--;
	sift_down(heap, 0, hub);
	return (first_element);
}
