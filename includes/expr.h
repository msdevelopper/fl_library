/** \file expr.h */
#ifndef _EXPR_H_
#define _EXPR_H_
#include "common.h"
#include "fl_token.h"

// functions below defined in expr_methods.c
double sigmoide(double x, double a, double b);
double gaussian(double x, double a, double b);
/*! \addtogroup methods_exposed_for_writing_equations
 *
 * @{
 * \brief thoses functions are exposed and 
 * can be used inside TSK subset
 *
 * can be a work for others developpers 
 * to integrate new functions
 * or other idea is to used lua library
 * to easily script functions
 *
 *
 * looking at math.h
 *
 * <https://www.tutorialspoint.com/c_standard_library/math_h.htm>
 *
 *
 * \attention maximum number of arguments are sets to 5
 *
 */
double d_cos(double l[5]);
double d_acos(double l[5]);
double d_sin(double l[5]);
double d_sigm(double l[5]);
double d_gauss(double l[5]);
double d_lt(double l[5]);
double d_gt(double l[5]);
double d_norm(double l[5]);
double d_normdomain(double l[5]);
/*! @}*/

/*! \typedef t_func
 * \brief this object expose functions available 
 * for users to write and execute
 * 
 */
typedef struct s_func t_func;
	// nb_params
	// peut servir a gerer les erreurs d'ecriture
	// de la part de l'utilisateur
	// voir fichier expr.c
	// dans la fonction expr_iter 
	// qui prepare les arguments en vue d'etre executer
	// en cet instant il n'est pas verifié 
	// si le nombre de parametres correspondant est exact
struct s_func{
	char	*name;				/**< the name to expose */
	int 	nb_params;			/**< not used */ 
	double 	(*meth_double)(double[5]); 	/**< the function to trigger */
};
/*! \brief array of exposed functions through t_func object
 *
 */
static t_func TOKEN_FUNC[8]={
	{"cos", 1, d_cos},
	{"acos", 1, d_acos},
	{"sin", 1, d_sin},
	{"sigm", 3,  d_sigm},
	{"gauss", 3, d_gauss},
	{"lt", 3, d_lt},
	{"gt", 3, d_gt},
	{"norm", 3, d_norm},
	{"normdomain", 5, d_normdomain}
};

static void print_list_expr(t_listhead *h);

/*! \typedef t_expr
 * \brief this object is used to store and build binary tree of datas
 *	limit of 64 bytes/64 characters of text to represent names,values 
 */
typedef struct s_expr t_expr;


struct s_expr{
	t_expr 		*parent; /**< pointer on parent node */
	t_expr 		*subnodes[2]; /**< left and right childs of current node */
	t_listhead 	*subnodes_list; /**< t_listhead object storing pointers of nodes */
	int type; 	/**< the type representation of the node (expr,primitive...) */
	char value[64]; /**< the string representation of the node datas */
	int leaf;	/**< if we are on leaf node = 1 otherwise 0 */
	int priority;	/**< use to define the priority between operator +,- // 0,1,2 */
	int negative;	/**< set to 1 if it represente a negative number otherwise 0 */

	int reduce;	/**< set to 1 if we should reduce the list of token/nodes otherwise 0 */

	char *func_name;/**< string name of a function (should be an exposed function in TOKEN_FUNC array */
	t_func *method; /**< pointer on function inside TOKEN_FUNC array*/ 

	char *var_name; /**< string name of a variable */

	double result; /**< result of operation/execution of the evaluted expression */
};


/*! \typedef t_expr_container
 * \brief a facility object to contain string representation and builded t_expr tree
 */
typedef struct s_expr_container t_expr_container;

struct s_expr_container{
	t_listhead *head;	/**< Pointer on t_listhead containing textual representation of data */
	t_expr *obj;		/**< Pointer on the builded tree */
};


double 			expr_container_eval(t_expr_container *e, t_listhead *varlist);

t_expr_container	*expr_container(char *str, t_listhead *var_list);
void 			release_expr_container(t_expr_container **e);

double			expr_iter(t_expr *e);
t_expr 			*expr_tokenise(char *str, t_listhead *h, t_listhead *var_list);
void 			release_expr(t_expr *e);

/*! \typedef t_order
 * \brief utility to organize & evaluate the order of execution
 */
typedef struct s_order{
	int index; 	/**< place into list */
	char priority; /**< set to 1 if + or - token otherwise 2*/
}t_order;


/*! \enum e_expr_err
 * \brief Enumerate error code when accessing object or memory problems occured and function call failed
 *
 */
enum e_expr_err
{
	EXPR_MEMALLOC_ERR = -1, /**< memory allocation space for t_expr  object */
	EXPR_FREE_ERR = -2,	/**< release space for t_expr object */
	EXPR_CONTAINER_MEMALLOC_ERR = 0 /**< memory allocation space for t_expr_container object */
};



#endif
