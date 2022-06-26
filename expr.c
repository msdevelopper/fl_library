/** \file expr.c */
#include "fl.h"
#include "fl_token.h"
#include "expr.h"

/*! \brief create a new t_expr object
 * @param[in] type the type of expression it represent
 * @param[in] value character representing the value
 *
 * @return pointer on the new t_expr object or #EXPR_MEMALLOC_ERR 
 */
static t_expr *create_expr(int type, char value)
{
	t_expr *obj;

	obj = malloc(sizeof(t_expr));
	if (!obj)
		return 0;
	memset(obj, 0, sizeof(*obj));
	obj->type = type;
	*(obj->value) = value;

	obj->subnodes[0] = 0;
	obj->subnodes[1] = 0;
	obj->subnodes_list = 0;
	obj->priority = 0;

	if (type == TYPE_PRIMITIVE)
		obj->leaf = 1;
	if (type == TYPE_EXP)
		obj->priority = (value == TOKEN_MINUS || value == TOKEN_PLUS) ? 1 : 2;
	return obj;
}
/*!
 * \brief print the content of a t_expr object
 * @param[in] e the object to print
 */
static void print_expr(t_expr *e)
{
	if (!e) return;
	printf("type:(%d)\tvalue:[%s]\tleaf:(%d)\tprior:(%d)\tres:(%f)\tneg:(%d)\n",e->type,e->value, e->leaf,e->priority, e->result , e->negative);
	if (e->subnodes_list != 0)
	{
		printf("subnodes->list:\t");
		print_list_expr(e->subnodes_list);
	}
	if (e->subnodes[0])
	{
		printf("subnodes[0]\t");
		print_expr(e->subnodes[0]);
	}
	if (e->subnodes[1])
	{
		printf("subnodes[1]\t");
		print_expr(e->subnodes[1]);
	}
}
/*!
 * \brief print the content of a list of t_expr objects
 * @param[in] h list of objects to print
 */
static void print_list_expr(t_listhead *h)
{
	t_list *c;
	int count = 0;
	t_expr *e;

	c = h->head;
	printf("length: %zu\n",h->length);
	while (c)
	{
		e = (t_expr*)c->data;
		//printf("[%d] type:(%d)\tvalue:[%s]\n",count , e->type,e->value);
		print_expr(e);
		count++;
		c = c->next;
	}
}
/*!
 * \brief release all the memory allocated inside a t_expr object
 * @param[in] r Pointer on address of t_expr objects
 */
static void release_expr_depth(t_expr **r)
{
	t_list *c;
	t_list *cc;
	t_expr *e;
	t_expr *n;

	e = *r;
	if (!e) 
		return;

	if (e->subnodes_list)
	{
		c = e->subnodes_list->head;
		while (c)
		{
			cc = c->next;
			n = (t_expr*)c->data;
			release_expr_depth(&n);
			c->data = 0;
			free(c);
			c = cc;
		}
		free(e->subnodes_list);
	}
	if (e->subnodes[1])
		release_expr_depth(&e->subnodes[1]);
	if (e->subnodes[0])
		release_expr_depth(&e->subnodes[0]);
	free(e);
	*r = 0;
}
void release_expr(t_expr *p)
{
	t_expr *e;

	e = p;
	release_expr_depth(&e);
}
/*! \brief reduce the list of tokens to extract floating value
 *
 */
static void reduce_float_token(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	t_expr *obj3;
	int i = 0;

	while ( i < (int) h->length)
	{
		if (i + 2 >= (int)h->length) break;
		obj = at_list(h, i);
		obj2 = at_list(h, i+1);
		obj3 = at_list(h, i+2);
		if (obj && obj2 && obj3)
		{
			if (obj->type == TYPE_PRIMITIVE 
					&& *(obj2->value) == TOKEN_DOT
					&& obj3->type == TYPE_PRIMITIVE)
			{
				memcpy(obj->value + strlen(obj->value), ".", 1);
				memcpy(obj->value + strlen(obj->value), obj3->value, strlen(obj3->value));

				splice_list(h, i+1, 2);
			}
		}
		i++;
	}

}
/*! \brief reduce the list of tokens to extract number value
 *
 */
static void reduce_number_token(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	int i = 0;
	int j;
	char buff[64];
	char *ptr;

	zero(buff);
	ptr = buff;
	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && obj->type == TYPE_PRIMITIVE)
		{
			j = i+1;
			if (j >= (int) h->length)
				break;
			*ptr = *(obj->value);
			ptr++;
			obj2 = at_list(h, j); 
			while (j < (int)h->length && obj2->type == TYPE_PRIMITIVE)
			{
				*ptr = *(obj2->value);
				ptr++;
				j++;
				obj2 = at_list(h, j); 
			}	
			j--;
			memcpy(obj->value, buff, strlen(buff));
			zero(buff);
			ptr = buff;
			if (i != j)
			{
				splice_list(h, i+1, (j-i));
			}
		}
		i++;
	}

}
/*! \brief reduce the list of tokens to extract characters used to represent name 
 *
 */
static void reduce_char_token(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	int i = 0;
	int j;
	char buff[64];
	char *ptr;

	zero(buff);
	ptr = buff;
	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && obj->type == TYPE_CHAR)
		{
			j = i+1;
			if (j >= (int) h->length)
				break;
			*ptr = *(obj->value);
			ptr++;
			obj2 = at_list(h, j); 
			while (j < (int)h->length && obj2->type == TYPE_CHAR)
			{
				*ptr = *(obj2->value);
				ptr++;
				j++;
				obj2 = at_list(h, j); 
			}	
			j--;
			memcpy(obj->value, buff, strlen(buff));
			zero(buff);
			ptr = buff;
			if (i != j)
			{
				splice_list(h, i+1, (j-i));
			}
		}
		i++;
	}

}
/*! \brief set token to match character that correspond to a function exposed 
 * and set #TYPE_FN for a function or #TYPE_PRIMITIVE for a leaf node containing
 * the variable name  
 *
 */
static void rely_on_func_or_var(t_listhead *h, t_listhead *list_varnames)
{
	t_expr *obj;
	t_list *c;
	int i = 0;
	int j;
	int len;
	char *str;


	len = (int)sizeof(TOKEN_FUNC)/sizeof(TOKEN_FUNC[0]);
	c = h->head;
	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && obj->type == TYPE_CHAR)
		{
			j = -1;
			while (++j < len)
			{
				if (!strcmp(obj->value, TOKEN_FUNC[j].name))
				{
					obj->method = &TOKEN_FUNC[j];
					obj->type = TYPE_FN;
					break;
				}
			}
			if (j == len && list_varnames != 0)
			{
				c = list_varnames->head;
				while (c)
				{
					str = (char*)c->data;
					if (!strcmp(str, obj->value))
					{
						obj->type = TYPE_PRIMITIVE;
						obj->leaf = 1;
						obj->var_name = str;
						obj->result = 0;
					}
					c = c->next;
				}
			}
		}
		i++;
	}
}
/*! \brief check if a string / variable name cannot rely on list of vars
 * @param[in] h t_listhead object pointer which contain list of tokens
 * @return 0 on success otherwise -1
 */
static int check_leaving_char(t_listhead *h)
{
	t_expr *obj;
	int i = 0;

	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && (obj->type) == TYPE_CHAR)
		{
			return -1;
		}
		i++;
	}
	return 0;
}
/*! \brief remove parenthesis in the list of token
 * @param[in] h t_listhead object pointer which contain list of tokens
 */
static void reduce_parenthesis_token(t_listhead *h)
{
	t_expr *obj;
	int i = 0;

	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && *(obj->value) == TOKEN_OPEN_P)
		{
			obj = at_list(h, i+2);
			if (obj && *(obj->value) == TOKEN_CLOSE_P)
			{
				splice_list(h, i +2, 1);
				splice_list(h, i, 1);
			}
		}
		i++;
	}
}
/*! \brief check if there is a token which split function parameters
 * @param[in] h t_listhead object pointer which contain list of tokens
 * @return 1 if separator exist otherwise zero
 */
static int exist_fn_separator(t_listhead *h)
{
	t_expr *obj;
	int i = 0;

	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && obj->type == TYPE_FN_PARAM_SEPARATOR)
		{
			return 1;
		}
		i++;
	}
	return 0;
}
/*! \brief check trailing token which split function parameters
 * @param[in] h t_listhead object pointer which contain list of tokens
 * @return -1 if no separator exist otherwise zero
 */
static int check_if_fn_separator_without_func(t_listhead *h)
{
	int exist;
	t_expr *obj;
	int i = 0;
	int ct;

	exist = exist_fn_separator(h);
	if (!exist) return 0;

	ct = 0;
	while ( i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj && obj->type == TYPE_FN)
		{
			ct++;
		}
		i++;
	}
	return ct != 0 ? 0 : -1;
}
/*! \brief compare priority to sort value 
 * @param[in] _a first element
 * @param[in] _b second element
 * @return difference between values
 */
static int sort_order(void *_a, void *_b)
{
	t_order *a; t_order *b;

	a = (t_order*)_a;
	b = (t_order*)_b;
	return b->priority - a->priority;
}
/*! \brief release memory of a t_order object
 */
void release_order(t_order *s)
{
	free(s);
}
/*! \brief set current object into the list to a function type
 * @param[in] h t_listhead object pointer which contain list of tokens
 */
static void reduce_fn(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	int i = -1;

	while (++i < (int) h->length)
	{
		obj = at_list(h, i);
		obj2 = at_list(h, i +1);
		if (obj && obj2)
		{
			if (obj->type == TYPE_FN)
			{
				if (obj2->type == TYPE_EXP
						&& obj2->subnodes[0] == 0
						&& obj2->subnodes[1] == 0
						&& obj2->subnodes_list == 0)
				{
					continue;
				}
				obj->func_name = obj->value;
				obj->subnodes[0] = obj2;
				h->release = 0;
				splice_list(h, i+1, 1);
				h->release = (void(*)(void*)) release_expr;
			}
		}
	}
}
/*! \brief set object into the list to a negative number
 * @param[in] h t_listhead object pointer which contain list of tokens
 */
static void reduce_negative(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	t_expr *obj3;
	int i;
	int indexof;

	h->release = (void(*)(void*)) release_expr;
	i = (int)h->length;
//	printf("\t==== enter reduce_negative\n");
//	print_list_expr(h);
	if (i == 2)
	{
		obj = at_list(h, 0);
		obj2 = at_list(h, 1);
		if (*(obj->value) == TOKEN_MINUS)
		{
			splice_list(h,0,1);
			obj = at_list(h,0);
			if (obj->negative == 1)
				obj->negative = 0;
			else
				obj->negative = 1;
			return;
		}
	}
	while (--i > -1)
	{
		obj = at_list(h, i);
		obj2 = at_list(h, i-1);
		obj3 = at_list(h, i-2);
		if (obj && obj2 && obj3 && *(obj2->value) == TOKEN_MINUS)
		{
			indexof = char_indexof(TOKEN_EXP, sizeof(TOKEN_EXP), *(obj3->value));
			if (indexof != -1)
			{
				if (obj->negative == 1)
					obj->negative = 0;
				else
					obj->negative = 1;
				splice_list(h, i-1, 1);
			}
		}
	}
}
/*! \brief reduce object into the list to a #TYPE_EXP token
 * @param[in] h t_listhead object pointer which contain list of tokens
 */
static void reduce_exp(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	t_expr *obj3;
	int i ;
	int idxof;
	int g;
	t_listhead *tmp;
	t_order *order;
	int flag;

	g = 0;
	while (1)
	{
		g++; if (g > 100) {printf("guard\n");break;}

		flag = 0;
		i = -1;
		tmp = new_list(release_order);
		tmp->release = (t_release) release_order;
		while (++i < (int) h->length)
		{
			obj = at_list(h, i);
			if (obj != 0)
			{
				if (obj->type == TYPE_EXP && !obj->reduce)
				{
					idxof = char_indexof(TOKEN_EXP, sizeof(TOKEN_EXP), *(obj->value));
					if (idxof != -1)
					{
						order = malloc(sizeof(t_order));
						memset(order,0,sizeof(t_order));
						order->index = i;
						order->priority = obj->priority;
						push_list(tmp, order);		
						flag = 1;
					}
				}
			}
		}
		if (flag == 0)
		{
			free(tmp);
			return;
		}
		sort_list(tmp, sort_order);
		order = at_list(tmp, 0);
		obj = at_list(h, order->index);
		obj2 = at_list(h, order->index + 1);
		obj3 = at_list(h, order->index -1);

		if (obj && obj2 && obj3)
		{
			obj->subnodes[0] = obj3;
			obj->subnodes[1] = obj2;
			obj->type = TYPE_EXP;
			obj->reduce = 1;
			obj->priority = (*(obj->value) == TOKEN_PLUS || *(obj->value) == TOKEN_MINUS) ? 1 : 2;
			h->release = 0;
			splices_list(h, order->index - 1, 3, obj, 0); 
			h->release = (t_release) release_expr;

		}
		free_list(&tmp);
	}
}
/*! \brief build the tree  
 * @param[in] h t_listhead object pointer which contain list of tokens
 * @return pointer on the root node of the tree
 */
static t_expr *build(t_listhead *h)
{
	t_expr *obj;
	t_expr *obj2;
	int i;
	int j;
	int openp;
	int gate;
	int ct;
	int lct;
	t_listhead *sublist;
	t_listhead *subnodes;

	t_listhead *rec;
	t_listhead *_t;
	t_listhead *list;

	i = -1;
	while (++i < (int) h->length)
	{
		obj = at_list(h, i);
		if (obj)
		{ 
			if (*(obj->value) == TOKEN_OPEN_P)
			{
				openp = i;
			}
			else if (*(obj->value) == TOKEN_CLOSE_P)
			{
				j = i;
				while (j > openp + 1 && j > -1) j--;	

				sublist = splices_list(h, j, i - j, 0, 1);
				sublist->release = (t_release)h->release;
				//sublist = splice_get_list(h, j, i - j);
				if (!sublist)
				{
					printf("maybe error\n");
					break;
				}
				if ((int)sublist->length == 0)
				{
					free_list(&sublist);
					break;
				}

				gate = exist_fn_separator(sublist);
				if (!gate)
				{
					/*
					printf("\tbuild before reduce negative\n");
					printf("H list\n");
					print_list_expr(h);
					printf("sublist list\n");
					print_list_expr(sublist);
					*/
					reduce_negative(sublist);
					/*
					printf("\tbuild before reduce fn\n");
					printf("sublist list\n");
					print_list_expr(sublist);
					*/
					reduce_fn(sublist);
					/*
					printf("\tbuild before reduce exp\n");
					printf("sublist list\n");
					*/
					reduce_exp(sublist);
					/*
					printf("\tbuild after reduce expr\n");
					printf("H list\n");
					print_list_expr(h);
					printf("sublist list\n");
					print_list_expr(sublist);
					*/
				}
				else
				{
					// need to build seperator of func
					i = 0;
					ct = 0;
					lct = 0;
					rec = new_list(0);
					rec->release = 0;
					subnodes = new_list(0);
					subnodes->release = (void(*)(void*)) list_free;
					_t = new_list(0);
					push_list(subnodes, _t);
					while (i < (int)sublist->length)
					{
						obj = at_list(sublist, i);
						if (obj->type == TYPE_FN_PARAM_SEPARATOR)
						{

							splice_list(sublist, i, 1);
							i--;
							ct++;
						}
						else
						{
							if (ct != lct)
							{
								lct = ct;
								_t = new_list(0);
								push_list(subnodes, _t);
							}
							else
							{
								push_list(at_list(subnodes, ct), obj);
							}
							i++;
						}
					}
					i = 0;
					while (i < (int)subnodes->length)
					{
						list = at_list(subnodes, i);
						reduce_negative(list);
						reduce_fn(list);
						reduce_exp(list);
						//printf("\t\tPRINT LIST:\n");
						//print_list_expr(list);
						obj = at_list(list, 0);
						push_list(rec, obj);
						i++;
					}
					free_list(&subnodes);
				}
				// remove parenthesis
				splice_list(h, j, 1);
				splice_list(h, j-1, 1);
				/*
				printf("\tbuild after remove parenthesis\n");
				printf("H list\n");
				print_list_expr(h);
				printf("sublist list\n");
				print_list_expr(sublist);
				*/
				if (!gate)
				{
					obj = at_list(sublist, 0);
					insert_at_list(h, j-1, obj);
				}
				else
				{
					obj = at_list(h, j-2);
					obj->subnodes_list = rec; 
				}
				sublist->release = 0;
				free_list(&sublist);
				i = -1;
			}
		}
	}
	obj = at_list(h, 0);
	if ((int)h->length == 2)
	{
		obj2 = at_list(h, 1);
		obj->subnodes[0] = obj2;
		h->release = 0;
		splice_list(h, 1, 1);
		h->release = (t_release) release_expr;
	}
	return obj;
}
//////////////////////////////////////

// function was used when testing
/*
double getfunc(t_expr *e, double v)
{
	t_listhead *h;

	h = new_list(0);
	h->release = 0;
	push_list(h, &v);
	v = e->method->meth(h);
	free_list(&h);
	return v;
}
*/

/*! \brief evaluate the tree
 * @param[in] e t_expr object pointer referencing the root node
 * @return the value evaluated by the tree 
 */
double expr_iter(t_expr *e)
{
	char op;
	char *endptr = 0;
	double v;
	double vl;
	double vr;
	double argsv[5];
	int i;
	t_expr *el;
	t_expr *er;
	t_expr *n;
	t_list *h;

	if (!e)
	{
		return 0;
	}
	if (e->leaf)
	{
		if (e->var_name != 0)
			v = e->result;
		else
			v = strtod(e->value, &endptr);

		//e->result = v > 0 && e->negative ? -v : v;
		e->result = e->negative ? -v : v;
		return e->result;
	}
	if (e->subnodes_list)
	{
		h = e->subnodes_list->head;
		/*
		   args = new_list(0);
		   while (h)
		   {
		   n =(t_expr*) h->data;
		   expr_iter(n);
		   push_list(args, &n->result);
		   h = h->next;
		   }
		   e->result = e->method->meth(args);
		   free_list(&args);
		   */
		i = 0;
		while (h)
		{
			n =(t_expr*) h->data;
			expr_iter(n);
			argsv[i] = n->result;
			i++;
			h = h->next;
		}
		v = e->method->meth_double(argsv);
		e->result = e->negative ? -v : v;
		return e->result;
	}
	el = (e->subnodes[0]);
	er = (e->subnodes[1]);
	if (e->type == TYPE_FN)
	{
		if (!el)
			return 0;
		expr_iter(el);
		v = el->result;
		if (*(e->value) != TOKEN_ROOT)//rootnode
		{
			//	v = getfunc(e, v);
			argsv[0] = v;
			v = e->method->meth_double(argsv);
		}
		e->result = e->negative ? -v : v;
		return e->result;
	}
	expr_iter(e->subnodes[0]);
	expr_iter(e->subnodes[1]);


	vl = el ? el->result : 0;
	vr = er ? er->result : 0;

	op = *(e->value);
	if (op == TOKEN_PLUS)
		v = vl + vr;
	else if (op == TOKEN_MINUS)
		v = vl - vr;
	else if (op == TOKEN_MULTIPLICATOR)
		v = vl * vr;
	else if (op == TOKEN_DIVISOR)
		v = (vr != 0) ? (vl / vr) : 0;
	e->result = e->negative ? -v : v;
	return e->result;
}
/*! \brief tokenize input string to create tree
 * @param[in] str the input string representing an expression
 * @param[in] h pointer on a t_listhead object where token will be stored
 * @param[in] var_list pointer on a t_listhead object to search if the name of variable does exist
 * @return 0 if an error occurs or a #t_expr pointer root node
 */

t_expr *expr_tokenise(char *str, t_listhead *h, t_listhead *var_list)
{
	int isexp;
	int isparenthesis;
	int err;
	t_expr *obj;

	while (str && *str)
	{
		isexp = char_indexof(TOKEN_EXP, sizeof(TOKEN_EXP), *str);
		isparenthesis = (*str == TOKEN_OPEN_P) || (*str == TOKEN_CLOSE_P);
		if (*str == TOKEN_SPACE)
		{
			str++; continue;
		}
		else if (*str == TOKEN_COMMA)
		{
			obj = create_expr(TYPE_FN_PARAM_SEPARATOR, *str);
		}
		else if (isexp != -1)
			obj = create_expr(TYPE_EXP, *str);
		else if (isparenthesis == 1)
			obj = create_expr(TYPE_PARENTHESIS, *str);
		else if (*str >= '0' && *str <= '9')
			obj = create_expr(TYPE_PRIMITIVE, *str);
		else if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z'))
			obj = create_expr(TYPE_CHAR, *str);
		else 
			obj = create_expr(TYPE_NOT_COVERED, *str);

		if (obj != 0)
		{
			push_list(h, obj); 
		}
		str++;
	}
	reduce_number_token(h);
	reduce_float_token(h);
	reduce_char_token(h);
	rely_on_func_or_var(h , var_list);
	//	print_list_expr(h);
	err = check_leaving_char(h);
	if (err != 0) return 0;
	reduce_parenthesis_token(h);
	err = check_if_fn_separator_without_func(h);
	if (err != 0) return 0;

	push_list(h, create_expr(TYPE_PARENTHESIS, TOKEN_CLOSE_P ));
	unshift_list(h, create_expr(TYPE_PARENTHESIS, TOKEN_OPEN_P ));
	unshift_list(h, create_expr(TYPE_FN, TOKEN_ROOT));

	//	print_list_expr(h);
	obj = build(h);
	obj->leaf = 0;
	return obj;
}
/*! \brief apply variables value onto tree of t_expr object/tree
 * @param[in] e pointer on t_expr object where we will apply variables values
 * @param[in] varlist pointer on t_listhead object where variables are defined
 */
void apply_value_onto_var(t_expr *e, t_listhead *varlist)
{
	t_list *c;
	t_fuzzvariable_obj *o;

	if (!e)
		return;
	if (e->type == TYPE_PRIMITIVE)
	{
		e->result = 0;
		c = varlist->head;
		while (c)
		{
			o = (t_fuzzvariable_obj*)c->data;
			if (e->var_name && !strcmp(e->var_name, o->name))	
			{
				e->result = o->ref_value;
			}
			c = c->next;
		}
	}
	apply_value_onto_var(e->subnodes[0],varlist);
	apply_value_onto_var(e->subnodes[1],varlist);
	if (!e->subnodes_list)
		return;
	c = e->subnodes_list->head;
	while (c)
	{
		apply_value_onto_var(c->data, varlist);
		c = c->next;
	}
}

/*! \addtogroup expr_container
 * \brief functions represent the interface to use for developpement
 * @{
 */

/*! \brief evaluate the t_expr object/tree
 * @param[in] e pointer on t_expr_container object which will be evaluated
 * @param[in] varlist pointer on t_listhead object where variables are defined
 * @return the value evaluated by the tree
 */
double expr_container_eval(t_expr_container *e, t_listhead *varlist) 
{
	double r;
	apply_value_onto_var(e->obj, varlist);
	r = expr_iter(e->obj);
	return r;
}
/*! \brief create the #t_expr_container object
 * @param[in] str the input string to parse,tokenize and build
 * @param[in] varlist pointer on t_listhead object where variables are defined
 * @return pointer on the new t_expr_container object if success otherwise #EXPR_CONTAINER_MEMALLOC_ERR
 */

t_expr_container *expr_container(char*str, t_listhead *var_list)
{
	t_expr_container *e;
	t_listhead *h;
	t_expr *obj;

	e = malloc(sizeof(t_expr_container));
	if (!e)
		return EXPR_CONTAINER_MEMALLOC_ERR;
	memset(e,0,sizeof(*e));
	h = new_list(0);
	h->release = (t_release)(release_expr);
	obj = expr_tokenise(str, h, var_list);
	if (!obj)
	{
		free_list(&h);
		free(e);
		return EXPR_CONTAINER_MEMALLOC_ERR;
	}
	e->head = h;
	e->obj = obj;
	return e;
}
/*! \brief release memory of the #t_expr_container object
 * @param[in] e address of the pointer to release
 */

void release_expr_container(t_expr_container **e)
{
	t_listhead *h;
	t_list *c;
	t_list *cc;

	h = (*e)->head;
	c = h->head;
	while (c)
	{
		cc = c->next;
		release_expr(c->data);
		free(c);
		c = cc;
	}
	free(h);
	free(*e);
	*e = 0;
}
/*! @} */
