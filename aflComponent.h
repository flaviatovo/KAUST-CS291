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
 * Arquivo de componentes.
 * Versao:4.0 Funcionando 100%
*/


#ifndef _AFL_COMPONENT_H_
#define _AFL_COMPONENT_H_  "aflComponent.h"

#include <string>
#include <sstream>

#include "aflConst.h"

enum netComponent {R, C, L, D, E, F, G, H, V, I, Q}; 
enum tipoEspecial {NPN, PNP, PULSE, SIN, DC};

class Componente /*R, C, L, D, E, F, G, H, V, I, Q*/ 
{
	private:

	public:

	std::string nome;
	std::string condicaoInicial;
	std::string parametro;
	unsigned long noControladoMais;
	unsigned long noControladoMenos;
	unsigned long noControladorMais;
	unsigned long noControladorMenos;
	unsigned long emissor, base, coletor;
	unsigned long ciclos;
	double valor;
	double valor2;
	double condicao;
	double Is, Vt;
	double Isbc, Vtbc;
	double Isbe, Vtbe;
	double atraso, tempo_subida, tempo_descida, tempo_ligada;
	double periodo, angulo, fator_atenuacao, frequencia;
	double alfa, alfar;
	netComponent tipo;
	tipoEspecial componenteEspecial;
	std::string linha;

	//Este metodo transforma condicaoInicial em condicao para indutores e capacitores
	unsigned long avaliarCondicaoInicial();
	//Este metodo transforma a primeira letra em nome em tipo
	unsigned avaliarTipoComponente();
	//Este metodo transforma os parametros das fontes nas variaveis
	unsigned avaliarParametro();

	Componente();
	~Componente();
	Componente(std::string linha_Do_Arquivo);

	protected:
};

#endif
