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
 * Programa principal.
 * Versao:3.0
*/

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "aflConst.h" 
#include "aflAplicativo.h"

int main(int argc, char *argv[])
{
	std::string fileName;
		
	std::vector<std::string> stringErro;
	stringErro.push_back("#0 OK #");
	stringErro.push_back("#1 Elemento nao valido #");
	stringErro.push_back("#2 Parametro invalido #");
	stringErro.push_back("#3 Circuito com muitos nos. Maximo = 100 nos #");
	stringErro.push_back("#4 Arquivo invalido. Nao foi possivel abri-lo #");
	stringErro.push_back("#5 O sistema nao convergiu. #");
	stringErro.push_back("#6 Erro ao escrever o arquivo de saida #");
	stringErro.push_back("#7 Pilha vazia #");
	stringErro.push_back("#8 Overflow #");
	stringErro.push_back("#9 Erro ao escrever o arquivo de saida #");
	stringErro.push_back("#10 Circuito sem no de referencia #");

	if (argc==2)
	{
		fileName=argv[1];
	}
	else
	{
		std::cout << "Digite o nome do arquivo de Netlist:"<<std::endl;
		std::cin >> fileName;
	}

	Aplicativo aplicativo(fileName);
	if(aplicativo.erro != OK)
	{
		std::cout<<"Erro "<< stringErro[aplicativo.erro]<<" ao executar o programa."<<std::endl;
		exit (aplicativo.erro);
	}

	std::cout<<"OK"<<std::endl;
	return (0);
}
