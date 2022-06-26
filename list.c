/** \file list.c */
#include "fl.h"

/*! \brief Create a new t_listhead object
 *
 * @param[in] release Pointer to function used, called to release memory allocated
 * @return A pointer on the new t_listhead object
 * or #LISTHEAD_MEMALLOC_ERR enum if system allocation failed
 */
t_listhead *new_list(void *release)
{
	t_listhead *e;

	e = malloc(sizeof(t_listhead));
	if (!e)
		return LISTHEAD_MEMALLOC_ERR;
	memset(e, 0, sizeof(*e));
	if (release != 0)
		e->release = release;
	return e;
}
/* not used...
 * @param[in] h Pointer on the t_listhead object to release
 *
 * it will loop on the list and free memory allocated objects defined by user
 *
 * @return Nothing
 */
void list_free(t_listhead *h)
{
	t_list *c;
	t_list *n;
	if (!h) return;
	c = h->head;
	while (c)
	{
		n = c->next;
		free(c);
		c = n;
		h->length--;
	}
	free(h);

}
/*! \brief Release a t_listhead object list
 *
 * @param[in] head Address of pointer on the t_listhead object to release
 *
 * it will loop on the list and free memory allocated objects defined by user
 *
 * @return 0 on success or #LISTHEAD_FREE_ERR 
 *
 */
int 	free_list(t_listhead **head)
{
	t_list *c;
	t_list *n;
	t_listhead *h;

	h = *head;
	if (!h) 
		return LISTHEAD_FREE_ERR;
	c = h->head;
	if (!c)
	{
		free(h);
		*head = 0;
		return 0;
	}
	while (c)
	{
		n = c->next;
		if (h->release && c->data)
		{
			h->release(c->data);
			c->data = 0;
		}
		free(c);
		c = n;
		h->length--;
	}
	free(h);
	*head = 0;
	return 0;
}
/*! \brief Push an element to the end of the list
 *	@param[in] h Pointer on t_listhead object
 *	@param[in] d A pointer on data to store into the list
 *
 * @return 0 on success or #LISTHEAD_PUSH_ERR if memory allocation failed
 */
int 	push_list(t_listhead *h, void *d)
{
	t_list *e;
	t_list *c;

	e = malloc(sizeof(t_list));
	if (!e)
		return LISTHEAD_MEMALLOC_ERR;
	memset(e, 0, sizeof(*e));
	e->data = d;
	e->next = 0;
	if (h->length == 0)
	{
		h->head = e;
		h->length++;
		return 0; 
	}
	c = h->head;
	while (c->next)
		c = c->next;
	e->prev = c;
	c->next = e;
	h->length++;
	return 0;
}
/*! \brief Push an element to the beginning of the list
 *	@param[in] h Pointer on t_listhead object
 *	@param[in] d A pointer on data to store into the list
 *
 * @return 0 on success or #LISTHEAD_UNSHIFT_ERR if memory allocation failed
 */
int 	unshift_list(t_listhead *h, void *d)
{
	t_list *e;
	t_list *c;

	e = malloc(sizeof(t_list));
	if (!e)
		return LISTHEAD_MEMALLOC_ERR;
	memset(e, 0, sizeof(*e));
	e->data = d;
	if (h->length == 0)
	{
		h->length++;
		h->head = e;
		return 0;
	}
	c = h->head;
	c->prev = e;
	e->next = c;
	h->head = e;
	h->length++;
	return 0;
}
/*! \brief Remove the first element of the list
 *
 *	@param[in] h Pointer on t_listhead object
 *
 */
void shift_list(t_listhead *h)
{
	t_list *c;
	void *d;

	if (h->length == 0)
		return;

	c = h->head;
	if (c->next)
	{
		c->next->prev =0;
		h->head = c->next;
	}
	else
	{
		h->head = 0;
	}
	h->length--;
	d = c->data;
	if (h->release && d)
	{
		h->release(d);
		c->data = 0;
	}
	free(c);
}
/*! \brief Get the element of the list specified by index
 *
 *	@param[in] h Pointer on t_listhead object
 *	@param[in] i The index position of element to retrieve
 *
 *	@return 0 if no element was found otherwise pointer onto the data 
 *
 */
void *at_list(t_listhead *h, int i)
{
	t_list *c;
	int count;

	if (i < 0) return 0;
	count = 0;
	c = h->head;
	while (count < i && c)
	{
		c = c->next;
		count++;
	}
	return c ? c->data : 0;
}
/*! \brief Insert a new element into the list specified by index
 *
 *	@param[in] h Pointer on t_listhead object
 *	@param[in] idx Specify the position into the list
 *	@param[in] data The data to insert
 *
 *	@return 0 on success or #LISTHEAD_MEMALLOC_ERR if memory allocation fail
 *
 */
int 	insert_at_list(t_listhead *h, int idx, void *data)
{
	t_list *e;
	t_list *c;
	t_list *n;
	t_list *p;

	if (idx >= (int)h->length)
	{
		idx = (int)h->length;
	}
	if (idx < 0) idx = 0;

	e = malloc(sizeof(t_list));
	if (!e)
		return LISTHEAD_MEMALLOC_ERR;
	memset(e,0,sizeof(t_list));
	e->data = data;

	h->length++;
	c = h->head;
	if (idx == 0)
	{
		h->head = e;
		e->next = c;
		c->prev = e;
		return 0;
	}
	while (c && idx != 0)
	{
		p = c;
		c = c->next;
		idx--;
	}
	n = 0;
	if (c)
	{
		n = c->prev;
	}
	e->prev = n;
      	e->next = c;	
	if (n)
		n->next = e;
	if (c)
		c->prev = e;
	if (p && !c)
	{
		p->next = e;
		e->prev = p;
	}
	return 0;
}
void splice_list(t_listhead *h, int start, int howmany)
{
	t_list *c;
	t_list *d;
	int count;

	count =(int)h->length;
	if (start < 0 || start >= count)
		return;
	if (howmany < 1 || howmany > count)
		return;
	c = h->head;
	while (--start > -1)
	{
		c = c->next;
	}
	while (howmany > 0 && c)
	{
		d = c->next;
		if (d)
			d->prev = c->prev;
		if (c->prev)
			c->prev->next = d ? d: 0;
		if (h->release && c->data)
		{
			h->release(c->data);
			c->data = 0;
		}
		if (c == h->head)
			h->head = d ? d : 0;
		free(c);
		c = d;
		h->length--;
		howmany--;
	}
}
t_listhead  *splices_list(t_listhead *h, int start, int howmany, void *add_data, int create_list)
{
	t_list *c;
	t_list *d;
	int count;
	t_listhead *sublist;


	count =(int)h->length;
	if (start < 0 || start >= count)
		return 0;
	if (howmany < 1 || howmany > count)
		return 0;
	c = h->head;
	while (--start > -1)
	{
		c = c->next;
	}
	sublist = 0;
	if (create_list)
		sublist = new_list(h->release);
	while (howmany && c)
	{
		if (howmany == 1 && add_data != 0)
		{
			if (h->release)
			{
				if (create_list)
					push_list(sublist, c->data);
				else
				{
					h->release(c->data);
					c->data = 0;
				}
			}
			c->data = add_data;
			break;
		}
		d = c->next;
		if (d)
			d->prev = c->prev;
		if (c->prev)
			c->prev->next = d;
		if (h->release)
		{
			if (create_list)
				push_list(sublist, c->data);
			else
			{
				h->release(c->data);
				c->data = 0;
			}
		}
		if (c == h->head)
			h->head = d;
		free(c);
		c = d;
		h->length--;
		howmany--;
	}
	return sublist;
}
t_listhead  *splice_get_list(t_listhead *h, int start, int howmany)
{
	t_list *c;
	t_list *d;
	int count;
	t_listhead *sublist;


	count =(int)h->length;
	if (start < 0 || start >= count)
		return 0;
	if (howmany < 1 || howmany > count)
		return 0;
	c = h->head;
	while (--start > -1)
	{
		c = c->next;
	}
	sublist = new_list(h->release);
	while (howmany && c)
	{
		d = c->next;
		if (d)
			d->prev = c->prev;
		if (c->prev)
			c->prev->next = d;
		if (h->release)
		{
			push_list(sublist, c->data);
			//h->release(c->data);
		}
		if (c == h->head)
			h->head = d;
		free(c);
		c = d;
		h->length--;
		howmany--;
	}
	return sublist;
}
void sort_list(t_listhead *h , int (*cmp)(void*,void*))
{
	t_list *c;
	t_list *cc;
	void *tmp;
	int r;
	int swap;
	c = h->head;
	while (c)
	{
		swap = 0;
		cc = c->next;
		while (cc)
		{
			if (c->data != cc->data)
			{
				r = cmp(c->data, cc->data);
				if (r >0)
				{
					tmp = c->data;
					c->data = cc->data;
					cc->data = tmp;
					swap = 1;
					break;
				}
			}
			cc = cc->next;
		}
		if (swap)
			c = h->head;
		else
			c = c->next;
	}
}
