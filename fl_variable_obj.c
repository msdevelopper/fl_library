/** \file fl_variable_obj.c */
#include "fl.h"
#include <stdio.h>
#include <string.h>
/*! \brief release memory allocated for a #t_fuzzvariable_set 
 * @param[in] s pointer on the t_fuzzvariable_set to release
 */
void release_subset(t_fuzzvariable_set *s)
{
	if (s->tsk)
		release_expr_container(&s->tsk);
	if (s->tsk_string)
		free(s->tsk_string);
	free(s->name);
	free(s->wave);
	free(s);
}
/*! \brief create a #t_fuzzvariable_obj pointer
 *
 * @return pointer on the new allocated t_fuzzvariable_obj object
 */
static t_fuzzvariable_obj *create_var(void)
{
	t_fuzzvariable_obj *obj;
	
	obj = malloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	obj->subsets = new_list(release_subset);
	obj->subsets->release = release_subset;
	return obj;
}
/*! \brief release memory allocated for a #t_fuzzvariable_obj 
 * @param[in] v pointer on the t_fuzzvariable_obj to release
 */
void release_vars(t_fuzzvariable_obj *v)
{
	t_listhead *s;

	s = v->subsets;
	if (s)
		free_list(&s);
	if (v->name)
		free(v->name);
	if (v->type)
		free(v->type);
	free(v);
}
/*! \brief print the structure and datas of a #t_fuzzvariable_obj 
 * stored in the list
 *
 * @param[in] h pointer on the head of #t_listhead to look at objects
 */
void print_list_fuzzvariable_obj(t_listhead *h)
{
	printf("length: %zu\n",h->length);
	t_list *c;
	t_list *d;
	t_fuzzvariable_obj *o;
	int count = 0;
	int count1 = 0;
	t_fuzzvariable_set *s;

	c = h->head;
	while (c)
	{
		o = (t_fuzzvariable_obj*)c->data;
		printf("[%d] = [type:'%s' , name:'%s']\n",count, o->type, (o->name));


		count1 = 0;
		d = o->subsets->head;
		s = 0;
		while (d)
		{
			s = (t_fuzzvariable_set*)d->data;
			printf("\t[%d] = [name:'%s' , wave:'%s']\n",count1, s->name, (s->wave));

			int r = -1;
			while ( ++r < 4)
				printf("\t\trange[%d]=%f\n",r,s->range[r]);
			count1++;
			d = d->next;
		}

		count++;
		c = c->next;
	}
}
/*! \brief create the subsets defined for a variable #t_fuzzvariable_obj
 *
 * @param[in] token pointer on the list of input token to parse
 * @param[in] var pointer on the variable object for which we create subsets
 * @param[in] start a flag use to define the first recursion , set as 1 on the first call and then at 0
 * @return 0 if all the list of token has been readed or error code when happens
 */
static int create_sets(t_listhead *token, t_fuzzvariable_obj *var, int start)
{
	t_fuzzvariable_set 	*set;
	t_fuzzvariable_set 	*lastsubset;
	t_fuzzvariable_token 	*t;
	t_listhead 		*sublist;
	int test;
	char *endptr;
	int ct;
	char buf[128];

	if ((int)token->length == 0)
		return 0;

	t = at_list(token, 0);
	test = indexof(TOKEN_WAVES, sizeof(TOKEN_WAVES), t->value);

	if (start == 1 &&  -1 == test) return -1;
	
	lastsubset = at_list(var->subsets, var->subsets->length -1);

	set = malloc(sizeof(*set));
	memset(set, 0, sizeof(*set));

	if (!lastsubset)
		set->wave = strdup(t->value);
	else
	{
		if (test != -1)
		{
			set->wave = strdup(TOKEN_WAVES[test]);
		}
		else
		{
			set->wave = strdup(lastsubset->wave);
		}
	}
	if (test != -1)
	{
		shift_list(token);
		t = at_list(token, 0);
		if (t->type != TYPE_SPACE) return -11;
		shift_list(token);
	}
	t = at_list(token, 0);
	set->name = strdup(t->value);
	shift_list(token);
	
	t = at_list(token, 0);
	
	if ((int)token->length == 0) return -12;
	if (t->type != TYPE_SPACE) return -21;
	shift_list(token);

	
	test = 0;
	while (test < (int)token->length)
	{
		t = at_list(token, test);
		if (t->type == TYPE_LOGICAL) break;
		test++;
	
	}
	sublist = splice_get_list(token, 0, test); 
	if (test == (int)token->length) //end
	{}
	else
	{
		shift_list(token);
	}


	// if wave == TSK
	//
	if (!strcmp(set->wave, TOKEN_W_TSK))
	{
		test = -1;
		zero(buf);
		while (++test < (int)sublist->length)
		{
			t = at_list(sublist, test);
			memcpy(buf + strlen(buf), t->value, strlen(t->value));
		}
		set->tsk_string = strdup(buf);
	}
	else
	{
		test = -1;
		ct = 0;
		while (++test < (int)sublist->length)
		{
			t = at_list(sublist, test);
			if (t->type == TYPE_COMMA) continue;
			// get the value and push
			set->range[ct] = strtod( t->value, &endptr);	
			ct++;

		}
	}
	test = indexof(TOKEN_WAVES, sizeof(TOKEN_WAVES), set->wave);
	set->fuzzyfy = FUZZ_METHODS[test];
	set->value = 0;
	set->min = 0;
	set->max= 0;
	set->parent = var;

	get_set_domain(set);
	push_list(var->subsets, set);
	free_list(&sublist);
	return create_sets(token, var, 0);
}
/*! \brief create a new #t_fuzzvariable_obj object
 * @param[in] list Pointer on the list where new object are stored
 * @param[in] token Pointer on the list of token for which we create new variables & subsets
 * @return new t_fuzzvariable_obj on success otherwise 0 
 *
 * note : define error code to keep track of bad format 
 */
t_fuzzvariable_obj *flvariable_create(t_listhead *list, t_listhead *token)
{
	t_fuzzvariable_obj *var;
	t_fuzzvariable_token *t;
	int err;

	var = create_var();
	if (!var)
		return 0;
	t = at_list(token, 0);
	if (strcmp(t->value, TOKEN_INPUT_VAR) && strcmp(t->value, TOKEN_OUTPUT_VAR))
	{
		release_vars(var);
		return 0;
	}
	var->type = strdup(t->value);
	shift_list(token);
	t = at_list(token, 0);
	if (t->type != TYPE_SPACE)
	{
		release_vars(var);
		return 0;
	}
	shift_list(token);
	t = at_list(token, 0);
	if (t->type != TYPE_CHAR)
	{
		release_vars(var);
		return 0;
	}
	var->name = strdup(t->value);
	shift_list(token);
	t = at_list(token, 0);
	if (t->type != TYPE_SPACE)
	{
		release_vars(var);
		return 0;
	}
	shift_list(token);

	err = create_sets(token, var, 1);
	if (err)
	{
		release_vars(var);
		return 0;
	}
	push_list(list,var);
	return var;
}
