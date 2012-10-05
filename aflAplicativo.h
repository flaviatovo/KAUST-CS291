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
 * Arquivo de Aplicativo.
 * Versao:3.0
*/

#ifndef _AFL_APLICATIVO_H_
#define _AFL_APLICATIVO_H_  "aflAplicativo.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>
#include <sstream>

#include "Matrix.hpp"
#include "aflConst.h"
#include "aflComponent.h"

class Matrix;
class Componente;

enum TipoSimulacao {BE, GEAR};

class Aplicativo
{
	private:

	public:

	std::vector<Componente> netlistVirtual;
	
	std::string nomeDoArquivo;
	std::string nomeDasVariaveis;
	
	std::ifstream file;
	std::ofstream fileOut;

	unsigned numeroDeNos;
	unsigned erro;
	unsigned interacao;
	unsigned interacoesNR;
	unsigned valor_muito_grande;
	unsigned convergiu;
	unsigned chutes;

	double passo;//tamanho do passo
	unsigned passoInterno;//numero de passos internos
	double tempoFinal;//tempo final
	double tempoAtual;//variavel que guarda o tempo atual

	TipoSimulacao tipoDeSimulacao;

	PACC::Matrix matrizMNA;
	PACC::Matrix matrizNoTempoAtual;//to
	PACC::Matrix matrizNoTempoAnterior;//to - delta t
	PACC::Matrix matrizNoTempoFuturoInteracaoN;//to + delta t
	PACC::Matrix matrizFontesIndependentes;
	PACC::Matrix saidaDoPrograma;

	//Este metodo monta o netlist virtual
	unsigned montaNetlistVirtual();
	//Este metodo monta as matrizes
	unsigned montaMatrizes();
	//Este metodo calcula "tudo", para todos os tempos as tensoes nos nos
	//e as correntes no elementos especiais
	unsigned cad();
	//Este metodo escreve a saida no arquivo
	unsigned escreverNoArquivo();
	//Este metodo retira o no zero do circuito
	unsigned ajustaMatriz();

	Aplicativo();
	~Aplicativo();
	Aplicativo(std::string nomeArquivo);

	protected:
};

#endif

