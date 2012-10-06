/*
 * UFRJ - Universidade Federal do Rio de Janeiro.
 * Escola Politecnica.
 * DEL - Departamento de Engenharia Eletronica e Computacao.
 * Circuitos Eletricos II - 2007/1.
 * Professor: Antonio Carlos Moreirao de Queiroz.
 * Creation Date: 04/07/2004.
 * Autores:	Ana Carolina Schmidt Borges
 * 		Flavia Correia Tovo
 * 		Leonardo Cidade Ribeiro
 * Descricao: Programa para analise de circuitos usando metodo "backward" de Euler ou o metodo de
			  segunda ordem de Gear, opcao do usuario, acoplado ao metodo de Newton-Raphson, usando
			  analise nodal modificada.
 * Arquivo de constantes.
 * Versao:5.0
*/


#ifndef _AFL_CONST_H_
#define _AFL_CONST_H_ "frgConst.h"

#include <map>
#include <string>

#define PI 3.1415926536
#define DOIS_PI 6.2831853072
#define IS 3.775134544e-014 /* Valor de Is Calculado para i=1mA com v=0.6V */
#define VT 25e-3
#define ALFA 0.99
#define ALFAR 0.5
#define LIMIAR_V_VT 36.0
#define VALOR_MAX 10000000000.0

#define MAX_NOS 100

#define MAX_INTERACAO 100 /*Numero escolhido para limitar o tempo de execucao*/
#define MAX_NUM_CHUTES 50 /*Numero escolhido para limitar o tempo de execucao*/

#define OK 0
#define NOT_VALID_ELEMENT 1
#define INVALID_PARAMETER 2
#define MUITOS_NOS 3
#define INVALID_FILE 4
#define NAO_CONVERGIU 5
#define OUTPUT_ERROR 6
#define EMPTY_PILE 7
#define ERRO_OVERFLOW 8
#define ERRO_WRITE_FILE 9
#define SEM_NO_DE_REFERENCIA 10

//#define EXIT_FAILURE 99

#endif
