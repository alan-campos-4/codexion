/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 14:27:56 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 16:14:40 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#define LIMIT 5

typedef struct s_request
{
	int					id;
	int					metric;
}						t_request;

typedef struct s_heap
{
	t_request			requests[LIMIT];
	int					size;
}						t_heap;

void	swap(t_request *x, t_request *y)
{
	t_request	tmp;

	tmp = *y;
	*y = *x;
	*x = tmp;
}

static void	bubble_up(t_request *requests, int i)
{
	int	parent_idx;

    printf("bubble up\n");
	while (i > 0)
	{
		parent_idx = (i - 1);//(i - 1) / 2;
        printf("for i %i, parent_index %i\n", i, parent_idx);
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

	printf("parent %i\n", parent);
	least = parent;
	while (1)
	{
		left_child_idx = (parent * 2) + 1;
		right_child_idx = (parent * 2) + 2;
		printf("l idx %i\n", left_child_idx);
		printf("r idx %i\n", right_child_idx);
		if (left_child_idx < size && requests[left_child_idx].metric
			< requests[parent].metric)
			least = left_child_idx;
		if (right_child_idx < size && requests[right_child_idx].metric
			< requests[least].metric)
			least = right_child_idx;
		printf("least %i\n", least);
		if (least == parent)
			break ;
		swap(&requests[least], &requests[parent]);
		parent = least;
	}
}

void	remove_request_top(t_heap *heap)
{
	if (!heap || !heap->size)
		return ;
	if (&heap->requests[0] != &heap->requests[heap->size - 1])
		swap(&heap->requests[0], &heap->requests[heap->size - 1]);
	heap->size -= 1;
	heapify(heap->requests, 0, heap->size);
}

void	add_request(t_heap *heap, int id, int metric)
{
	if (!heap || (heap->size == LIMIT))
		return ;
	heap->requests[heap->size].id = id;
	heap->requests[heap->size].metric = metric;
	bubble_up(heap->requests, heap->size);
	heap->size += 1;
}


void    print_heap(t_heap *heap)
{
    t_request r;

    if (!heap)
        return ;
    for (int i=0; i < heap->size; i++)
    {
        r = heap->requests[i];
        printf("%i: [ %i, %i ]\n", i, r.id, r.metric);
    }
	printf("\n");
}

int main(void)
{
    t_heap  h;

    //h = malloc(sizeof(t_heap));
    h.size = 0;
    add_request(&h, 1, 20);
    add_request(&h, 2, 10);
    add_request(&h, 3, 15);
    add_request(&h, 4, 30);
    add_request(&h, 5, 16);
	print_heap(&h);
	remove_request_top(&h);
    print_heap(&h);
    //free(h);
}
