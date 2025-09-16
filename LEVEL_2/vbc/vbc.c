// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   vbc.c                                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: manandre                                   +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2025/09/16                                #+#    #+#             //
//                                                                            //
// ************************************************************************** //

// Assignment: vbc
// Objetivo: Interpretar expressões matemáticas simples contendo
// números [0–9], +, * e parênteses ( ).
// Funções permitidas: malloc, calloc, realloc, free, printf, isdigit, write
// Em caso de erro, imprimir mensagens conforme enunciado.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// ------------------------------------------------------------------------- //
// Variáveis globais para percorrer a string
// ------------------------------------------------------------------------- //
static const char *g_input; // expressão recebida
static int g_pos;           // posição atual na string

// ------------------------------------------------------------------------- //
// Funções auxiliares para leitura de caracteres
// ------------------------------------------------------------------------- //
static char peek(void)       // olha o caractere atual (sem avançar)
{
    return g_input[g_pos];
}

static char next(void)       // devolve o caractere atual e avança
{
    return g_input[g_pos++];
}

// ------------------------------------------------------------------------- //
// Declarações das funções do parser
// ------------------------------------------------------------------------- //
long parse_expr(int *ok);
long parse_term(int *ok);
long parse_factor(int *ok);

// ------------------------------------------------------------------------- //
// Gramática utilizada:
// expr   := term ( '+' term )*
// term   := factor ( '*' factor )*
// factor := DIGIT | '(' expr ')'
// ------------------------------------------------------------------------- //

// expr: trata soma
long parse_expr(int *ok)
{
    long val = parse_term(ok);
    while (*ok && peek() == '+')
    {
        next(); // consome '+'
        if (peek() == '\0')
        {
            printf("Unexpected end of input\n");
            *ok = 0;
            return 0;
        }
        val += parse_term(ok);
    }
    return val;
}

// term: trata multiplicação
long parse_term(int *ok)
{
    long val = parse_factor(ok);
    while (*ok && peek() == '*')
    {
        next(); // consome '*'
        if (peek() == '\0')
        {
            printf("Unexpected end of input\n");
            *ok = 0;
            return 0;
        }
        val *= parse_factor(ok);
    }
    return val;
}

// factor: número ou expressão entre parênteses
long parse_factor(int *ok)
{
    char c = peek();

    if (c == '\0')
    {
        printf("Unexpected end of input\n");
        *ok = 0;
        return 0;
    }
    if (isdigit(c))
    {
        next(); // consome o dígito
        return c - '0';
    }
    else if (c == '(')
    {
        next(); // consome '('
        long val = parse_expr(ok);
        if (!*ok) return 0;

        if (peek() != ')')
        {
            if (peek() == '\0')
                printf("Unexpected end of input\n");
            else
                printf("Unexpected token '%c'\n", peek());
            *ok = 0;
            return 0;
        }
        next(); // consome ')'
        return val;
    }
    else if (c == ')')
    {
        printf("Unexpected token ')'\n");
        *ok = 0;
        return 0;
    }
    else
    {
        printf("Unexpected token '%c'\n", c);
        *ok = 0;
        return 0;
    }
}

// ------------------------------------------------------------------------- //
// Função principal
// ------------------------------------------------------------------------- //
int main(int ac, char **av)
{
    if (ac != 2)
        return 0; // nada a fazer se não houver argumento

    g_input = av[1];
    g_pos = 0;

    int ok = 1;
    long result = parse_expr(&ok);

    if (!ok) // já imprimiu o erro no parser
        return 0;

    if (peek() != '\0') // sobrou caractere inesperado
    {
        printf("Unexpected token '%c'\n", peek());
        return 0;
    }

    printf("%ld\n", result);
    return 0;
}

