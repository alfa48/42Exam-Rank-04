#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of file\n");
}

int accept(char **s, char c)
{
    if (**s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}


//

node    *parse_expre(char **s);

node *parse_fact(char **s){	
	if (isdigit(**s)){
		node n = {VAL, **s - '0', NULL, NULL};
		(*s)++;
		return (new_node(n));
	}else if (accept(s, '(')){
		node *n = parse_expre(s);
		if (!n){return (NULL);};
		if (!expect(s, ')')){destroy_tree(n);return (NULL);}
		return (n);
		
	}else{
		unexpected(**s);
		return NULL;
	}
}

node    *parse_term(char **s)
{
    node *left = parse_fact(s);
    if (!left){return (NULL);};
    while(accept(s, '*')){
    	node *right = parse_fact(s);
    	if (!right){destroy_tree(left);return (NULL);}
    	node *tmp = new_node((node){MULTI, 0, left, right});
    	if (!tmp){destroy_tree(left);return (NULL);}
    	left =  tmp;
    }
    
    return (left);
}

node    *parse_expre(char **s)
{
    node *left = parse_term(s);
    if (!left){return (NULL);};
    while(accept(s, '+')){
    	node *right = parse_term(s);
    	if (!right){destroy_tree(left);return (NULL);}
    	node *tmp = new_node((node){ADD, 0, left, right});
    	if (!tmp){destroy_tree(left);return (NULL);}
    	left =  tmp;
    }
    
    return (left);
}

node    *parse_expr(char *s)
{
    char *ptr = s;
    node *ret = parse_expre(&ptr);
    if (!ret){return (NULL);};
    if (*ptr) 
    {
    	unexpected(*ptr);
    	destroy_tree(ret);
    	return (NULL);
    }
    return (ret);
}


int eval_tree(node *tree)
{
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    node *tree = parse_expr(argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return (0);
}




