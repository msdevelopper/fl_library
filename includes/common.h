/** \file common.h */
#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define zero(v) (memset(v,0,sizeof(v)))
#define zerostr(v) (memset(v,0,strlen(v)))
#define min(a,b)(a <= b ? a : b)
#define max(a,b)(a >= b ? a : b)



/////////////////////////////////////////
//utils.c
//ces fonctions sont utilisées dans le parseur
int 	indexof(char *_arr[], size_t _sz, char *_s);
int 	char_indexof(char _arr[], size_t _sz, char _s);


/////////////////////////////////////////
// list.c

/*! \enum e_listhead_err
 * \brief Enumerate error code when accessing object or memory problems occured and function call failed
 *
 */
enum e_listhead_err
{
	LISTHEAD_MEMALLOC_ERR = -1,	/**< memory allocation space for t_listhead  or t_list object */
	LISTHEAD_FREE_ERR = -2,		/**< release space for t_listhead object */
};

/*! \typedef t_listhead
 * \brief Define the base container of list object within all datas are stored
 */
typedef struct s_listhead t_listhead;
/*! \typedef t_list
 * \brief Element of a t_listhead object
 */
typedef struct s_list t_list;
/*! \typedef t_release
 * \brief Define a function pointer used to release memory of a t_listhead object
 */
typedef void (*t_release)(void*);

struct s_listhead{
	size_t length; /**< length of the list */
	t_list *head; /**< first element of the list */
	t_release release; /**< pointer on function used to released memory */
};
struct s_list{
	t_list *prev; /**< pointer on previous t_list object */
	t_list *next; /**< pointer on next t_list object */
	void	*data; /**< pointer on data stored */
};


t_listhead	*new_list(void*free_func);
void		list_free(t_listhead *h);
int 		free_list(t_listhead **head);
int		push_list(t_listhead *h, void *d);
int		unshift_list(t_listhead *h, void *d);
void		shift_list(t_listhead *h);
void		*at_list(t_listhead *h, int i);
int	 	insert_at_list(t_listhead *h, int idx, void *data);
void 		splice_list(t_listhead *h, int start, int howmany);
t_listhead 	*splices_list(t_listhead *h, int start, int howmany, void *add_data, int create_list);
t_listhead 	*splice_get_list(t_listhead *h, int start, int howmany);

void		sort_list(t_listhead *h , int (*cmp)(void*,void*));

#endif
