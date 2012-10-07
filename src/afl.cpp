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
	stringErro.push_back("#1 Invalid element #");
	stringErro.push_back("#2 Invalid parameter #");
	stringErro.push_back("#3 Too many nodes on circuit. Max = 100 nodes #");
	stringErro.push_back("#4 Invalid file. It was not possible to open it #");
	stringErro.push_back("#5 The system didn't converge. #");
	stringErro.push_back("#6 Error writing output file #");
	stringErro.push_back("#7 Empty stack #");
	stringErro.push_back("#8 Overflow #");
	stringErro.push_back("#9 Error writing output file #");
	stringErro.push_back("#10 Circuit without reference node #");

	if (argc==2)
	{
		fileName=argv[1];
	}
	else
	{
		std::cout << "Enter the netlist filename:"<<std::endl;
		std::cin >> fileName;
	}

	Aplicativo aplicativo(fileName);
	if(aplicativo.erro != OK)
	{
		std::cout<<"Error "<< stringErro[aplicativo.erro]<<" while running the program."<<std::endl;
		exit (aplicativo.erro);
	}

	std::cout<<"OK"<<std::endl;
	return (0);
}
