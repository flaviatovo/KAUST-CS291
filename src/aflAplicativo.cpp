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
 * Versao:4.0
*/


#include <cstdio>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sstream>

#include "aflAplicativo.h"
#include "Matrix.hpp"
#include "aflConst.h"
#include "aflComponent.h"

class Matrix;
class Componente;

/*class Aplicativo
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
	unsigned convergiu;
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
};*/

unsigned Aplicativo::montaNetlistVirtual()
{
	std::string linhaDoArquivo;
	std::string tipo;
	Componente elemento;

	//Pegando o numero de nos da primeira linha do arquivo
	std::getline(file, linhaDoArquivo);
	
	std::string nos = strtok((char *) linhaDoArquivo.c_str()," ");
	numeroDeNos = strtoul(nos.c_str(),NULL,10);
	if (numeroDeNos > MAX_NOS)
		return (MUITOS_NOS);
	std::getline(file, linhaDoArquivo);

	do
	{
		std::istringstream linha (linhaDoArquivo);
		//Se for um compenente
		if ((linhaDoArquivo[0]=='R')||(linhaDoArquivo[0]=='C')||(linhaDoArquivo[0]=='L')||(linhaDoArquivo[0]=='D')||
			(linhaDoArquivo[0]=='E')||(linhaDoArquivo[0]=='F')||(linhaDoArquivo[0]=='G')||(linhaDoArquivo[0]=='H')||
			(linhaDoArquivo[0]=='I')||(linhaDoArquivo[0]=='V')||(linhaDoArquivo[0]=='Q')||
			(linhaDoArquivo[0]=='r')||(linhaDoArquivo[0]=='c')||(linhaDoArquivo[0]=='l')||(linhaDoArquivo[0]=='d')||
			(linhaDoArquivo[0]=='e')||(linhaDoArquivo[0]=='f')||(linhaDoArquivo[0]=='g')||(linhaDoArquivo[0]=='h')||
			(linhaDoArquivo[0]=='i')||(linhaDoArquivo[0]=='v')||(linhaDoArquivo[0]=='q'))
		{
			elemento = Componente(linhaDoArquivo);
			netlistVirtual.push_back(elemento);
		}
		else //linha vazia ou linha de instrucao
		{
			if (linhaDoArquivo[0]=='.')
			{
				linha >> tipo;

				if ((tipo.compare(".tran") == 0)||(tipo.compare(".TRAN") == 0))
				{
					linha >> tipo;
					tempoFinal = strtod(tipo.c_str(), NULL);
					
					linha >> tipo;
					passo = strtod(tipo.c_str(), NULL);
					
					linha >> tipo;
					if (tipo.length() == 2)
						tipoDeSimulacao = BE;
					else if (tipo.length() == 4)
						tipoDeSimulacao = GEAR;
					else
						return (INVALID_PARAMETER);
					
					linha >> tipo;
					passoInterno = strtoul(tipo.c_str(), NULL, 10);
				}
				else
				{
					return (NOT_VALID_ELEMENT);
				}
			}
			else
			{
				//Nao faz nada. Significa que a linha eh comentario ou esta vazia.
			}
		}
	}while (std::getline(file, linhaDoArquivo));
	return OK;
}

//Este metodo retira o no zero do circuito
unsigned Aplicativo::ajustaMatriz()
{
	unsigned linha, coluna;
	unsigned tamanho = matrizMNA.cols();
	//Retirando as linha e coluna zero da matriz matrizMNA
	{
	for (linha = 1; linha < tamanho; linha ++)
		for (coluna = 1; coluna<tamanho; coluna ++)
			matrizMNA(linha - 1, coluna - 1) = matrizMNA(linha, coluna);
	}
	//Retirando a linha zero da matriz matrizFontesIndependentes
	{
	for (linha = 1; linha<tamanho; linha ++)
		matrizFontesIndependentes(linha - 1, 0) = matrizFontesIndependentes(linha, 0);
	}

	//Ajustando o tamanho das matrizes
	tamanho -= 1;
	matrizMNA.resize(tamanho,tamanho);
	matrizFontesIndependentes.resize(matrizMNA.cols(), 1);

	return(OK);
}

unsigned Aplicativo::montaMatrizes()
{
	int indice, qtd_cmp;
	double valor, v, I, G;
	double vbc, vbe, Gbc, Ibc, Gbe, Ibe;
	double tempoNormalizado;
	double tempoPassoInterno = passo/passoInterno;

	//para saber quantas linhas não-especiais irão haver assim as especiais poderão ser inseridas
	qtd_cmp=numeroDeNos+1;
	
	for (indice = (netlistVirtual.size() - 1); indice>=0; indice--)
	{
		if(netlistVirtual[indice].tipo == R)
		{			
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMais)+=(1/netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMenos)+=(-1/netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMais)+=(-1/netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMenos)+=(1/netlistVirtual[indice].valor);
		}
/*		else if(netlistVirtual[indice].tipo == G)//Transcondutor
		{
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladorMais)+=(netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladorMenos)+=(-netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladorMais)+=(-netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladorMenos)+=(netlistVirtual[indice].valor);
			std::cout<<netlistVirtual[indice].nome<<" "<<netlistVirtual[indice].tipo<<std::endl;
		}*/
		else if(netlistVirtual[indice].tipo == I)
		{
			if(netlistVirtual[indice].componenteEspecial == DC)
			{
				valor = netlistVirtual[indice].valor;
			}
			else if (netlistVirtual[indice].componenteEspecial == PULSE)
			{
				if ((tempoAtual < netlistVirtual[indice].atraso)||(tempoAtual>(netlistVirtual[indice].atraso + netlistVirtual[indice].periodo * netlistVirtual[indice].ciclos)))
					valor = netlistVirtual[indice].valor;
				else
				{
					tempoNormalizado = tempoAtual - netlistVirtual[indice].atraso - netlistVirtual[indice].periodo * floor((tempoAtual - netlistVirtual[indice].atraso)/netlistVirtual[indice].periodo);
					if(tempoNormalizado < netlistVirtual[indice].tempo_subida)
						valor = netlistVirtual[indice].valor + (tempoNormalizado * (netlistVirtual[indice].valor2-netlistVirtual[indice].valor))/netlistVirtual[indice].tempo_subida;
					else if (tempoNormalizado < (netlistVirtual[indice].tempo_subida + netlistVirtual[indice].tempo_ligada))
						valor = netlistVirtual[indice].valor2;
					else if (tempoNormalizado < (netlistVirtual[indice].tempo_subida + netlistVirtual[indice].tempo_ligada + netlistVirtual[indice].tempo_descida))
						valor = netlistVirtual[indice].valor2 + ((netlistVirtual[indice].valor - netlistVirtual[indice].valor2) * (tempoNormalizado - netlistVirtual[indice].tempo_subida - netlistVirtual[indice].tempo_ligada) / netlistVirtual[indice].tempo_descida);
					else
						valor = netlistVirtual[indice].valor;
				}
			}
			else if (netlistVirtual[indice].componenteEspecial == SIN)
			{
				if ((tempoAtual < netlistVirtual[indice].atraso)||(tempoAtual>(netlistVirtual[indice].atraso + netlistVirtual[indice].periodo * netlistVirtual[indice].ciclos)))
					valor = netlistVirtual[indice].valor2 + netlistVirtual[indice].valor 
						* sin(netlistVirtual[indice].angulo * PI / 180);
				else
				{
					tempoNormalizado = tempoAtual - netlistVirtual[indice].atraso;
					valor = netlistVirtual[indice].valor2 + netlistVirtual[indice].valor
						* exp(- netlistVirtual[indice].fator_atenuacao *tempoAtual) 
						* sin((DOIS_PI * netlistVirtual[indice].frequencia * tempoNormalizado)
						+ (netlistVirtual[indice].angulo * PI / 180));
				}
			}

			matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0)+=(-valor);
			matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=(valor);
		}
		else if (netlistVirtual[indice].tipo == V)
		{
			matrizMNA.resize(qtd_cmp+1,qtd_cmp+1);

			matrizMNA(netlistVirtual[indice].noControladoMais,qtd_cmp)+=(1);
			matrizMNA(netlistVirtual[indice].noControladoMenos,qtd_cmp)+=(-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladoMais)+=(-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladoMenos)+=(1);

			if(netlistVirtual[indice].componenteEspecial == DC)
			{
				valor = netlistVirtual[indice].valor;
			}
			else if (netlistVirtual[indice].componenteEspecial == PULSE)
			{
				if ((tempoAtual < netlistVirtual[indice].atraso)||(tempoAtual>(netlistVirtual[indice].atraso + netlistVirtual[indice].periodo * netlistVirtual[indice].ciclos)))
					valor = netlistVirtual[indice].valor;
				else
				{
					tempoNormalizado = tempoAtual - netlistVirtual[indice].atraso - netlistVirtual[indice].periodo * floor((tempoAtual - netlistVirtual[indice].atraso)/netlistVirtual[indice].periodo);
					if(tempoNormalizado < netlistVirtual[indice].tempo_subida)
						valor = netlistVirtual[indice].valor + (tempoNormalizado * (netlistVirtual[indice].valor2-netlistVirtual[indice].valor))/netlistVirtual[indice].tempo_subida;
					else if (tempoNormalizado < (netlistVirtual[indice].tempo_subida + netlistVirtual[indice].tempo_ligada))
						valor = netlistVirtual[indice].valor2;
					else if (tempoNormalizado < (netlistVirtual[indice].tempo_subida + netlistVirtual[indice].tempo_ligada + netlistVirtual[indice].tempo_descida))
						valor = netlistVirtual[indice].valor2 + ((netlistVirtual[indice].valor - netlistVirtual[indice].valor2) * (tempoNormalizado - netlistVirtual[indice].tempo_subida - netlistVirtual[indice].tempo_ligada) / netlistVirtual[indice].tempo_descida);
					else
						valor = netlistVirtual[indice].valor;
				}
			}
			else if (netlistVirtual[indice].componenteEspecial == SIN)
			{
				if ((tempoAtual < netlistVirtual[indice].atraso)||(tempoAtual>(netlistVirtual[indice].atraso + netlistVirtual[indice].periodo * netlistVirtual[indice].ciclos)))
					valor = netlistVirtual[indice].valor2 + netlistVirtual[indice].valor 
						* sin(netlistVirtual[indice].angulo * PI / 180);
				else
				{
					tempoNormalizado = tempoAtual - netlistVirtual[indice].atraso;
					valor = netlistVirtual[indice].valor2 + netlistVirtual[indice].valor
						* exp(- netlistVirtual[indice].fator_atenuacao *tempoAtual) 
						* sin((DOIS_PI * netlistVirtual[indice].frequencia * tempoNormalizado)
						+ (netlistVirtual[indice].angulo * PI / 180));
				}
			}

			//inclui a variavel corrente na fonte
			if ((interacao == 0 )&&(interacoesNR == 0)&&(convergiu == 0)&&(chutes == 0))
			{
				nomeDasVariaveis += std::string("   Icomp");
				nomeDasVariaveis += netlistVirtual[indice].nome;
				nomeDasVariaveis += std::string("(A)   ");
			}

			matrizFontesIndependentes.resize(qtd_cmp+1,1);
			matrizFontesIndependentes(qtd_cmp,0) = 0;
			matrizFontesIndependentes(qtd_cmp,0) -= valor;
			
			qtd_cmp ++;
		}
		else if (netlistVirtual[indice].tipo == E)//Amplificador de tensao
		{
			//coloca mais uma linha e mais uma coluna em cada matrz
			matrizMNA.resize(qtd_cmp+1,qtd_cmp+1);
			matrizFontesIndependentes.resize(qtd_cmp+1,1);
			matrizFontesIndependentes(qtd_cmp,0) = 0;

			matrizMNA(netlistVirtual[indice].noControladoMais,qtd_cmp)   +=( 1);
			matrizMNA(netlistVirtual[indice].noControladoMenos,qtd_cmp)  +=(-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladoMais)   +=(-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladoMenos)  +=( 1);

			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladorMais)  +=( netlistVirtual[indice].valor);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladorMenos) +=(-netlistVirtual[indice].valor);
			
			//inclui a variavel corrente na fonte
			if ((interacao == 0 )&&(interacoesNR == 0)&&(convergiu == 0)&&(chutes == 0))
			{
				nomeDasVariaveis += std::string("   Icomp");
				nomeDasVariaveis += netlistVirtual[indice].nome;
				nomeDasVariaveis += std::string("(A)   ");
			}

			qtd_cmp ++;
		}
		else if (netlistVirtual[indice].tipo == F)//Transresistor
		{
			//coloca mais uma linha e mais uma coluna em cada matrz
			matrizMNA.resize(qtd_cmp+1,qtd_cmp+1);
			matrizFontesIndependentes.resize(qtd_cmp+1,1);
			matrizFontesIndependentes(qtd_cmp,0) = 0;

			matrizMNA(netlistVirtual[indice].noControladoMais,qtd_cmp)   += ( netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,qtd_cmp)  += (-netlistVirtual[indice].valor);

			matrizMNA(netlistVirtual[indice].noControladorMais,qtd_cmp)  += ( 1);
			matrizMNA(netlistVirtual[indice].noControladorMenos,qtd_cmp) += (-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladorMais)  += (-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladorMenos) += ( 1);

			//inclui a variavel corrente na fonte
			if ((interacao == 0 )&&(interacoesNR == 0)&&(convergiu == 0)&&(chutes == 0))
			{
				nomeDasVariaveis += std::string("   Icomp");
				nomeDasVariaveis += netlistVirtual[indice].nome;
				nomeDasVariaveis += std::string("(A)   ");
			}
			
			qtd_cmp ++;
		}
		else if (netlistVirtual[indice].tipo == H)//Amplificador de corrente
		{
			//coloca mais uma linha e mais uma coluna em cada matrz
			matrizMNA.resize(qtd_cmp+2,qtd_cmp+2);
			matrizFontesIndependentes.resize(qtd_cmp+2,1);
			matrizFontesIndependentes(qtd_cmp,0) = 0;
			matrizFontesIndependentes(qtd_cmp + 1,0) = 0;

			//colunas especiais
			matrizMNA(netlistVirtual[indice].noControladoMais,qtd_cmp+1)+=(1);
			matrizMNA(netlistVirtual[indice].noControladoMenos,qtd_cmp+1)+=(-1);

			matrizMNA(netlistVirtual[indice].noControladorMais,qtd_cmp)+=(1);
			matrizMNA(netlistVirtual[indice].noControladorMenos,qtd_cmp)+=(-1);
			//linhas especiais
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladorMais)+=(-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladorMenos)+=(1);

			matrizMNA(qtd_cmp+1,netlistVirtual[indice].noControladoMais)+=(-1);
			matrizMNA(qtd_cmp+1,netlistVirtual[indice].noControladoMenos)+=(1);

			matrizMNA(qtd_cmp+1,qtd_cmp)+=(netlistVirtual[indice].valor);

			//inclui a variavel corrente no curto
			if ((interacao == 0 )&&(interacoesNR == 0)&&(convergiu == 0)&&(chutes == 0))
			{
				nomeDasVariaveis += std::string("   Icurto");
				nomeDasVariaveis += netlistVirtual[indice].nome;
				nomeDasVariaveis += std::string("(A)   ");
			}

			//inclui a variavel corrente na fonte
			if ((interacao == 0 )&&(interacoesNR == 0)&&(convergiu == 0)&&(chutes == 0))
			{
				nomeDasVariaveis += std::string("   Icomp");
				nomeDasVariaveis += netlistVirtual[indice].nome;
				nomeDasVariaveis += std::string("(A)   ");
			}
						
			qtd_cmp += 2;
		}

		else if(netlistVirtual[indice].tipo ==C)
		{
			if ((tipoDeSimulacao == BE)||(interacao ==0)) //Monta matrizes usando as estampas para o modo backward
			{
				if (interacao==0)
				{
					matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMais)   += (netlistVirtual[indice].valor /tempoAtual);
					matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMenos)  += (-netlistVirtual[indice].valor /tempoAtual);
					matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMais)  += (-netlistVirtual[indice].valor /tempoAtual);
					matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMenos) += ( netlistVirtual[indice].valor /tempoAtual);

				}
				else
				{
					matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMais)   += (netlistVirtual[indice].valor /tempoPassoInterno);
					matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMenos)  += (-netlistVirtual[indice].valor /tempoPassoInterno);
					matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMais)  += (-netlistVirtual[indice].valor /tempoPassoInterno);
					matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMenos) += ( netlistVirtual[indice].valor /tempoPassoInterno);
				}

				if (interacao ==0)
				{
					matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0)  += ( netlistVirtual[indice].condicao*netlistVirtual[indice].valor /tempoAtual);
					matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0) += (-netlistVirtual[indice].condicao*netlistVirtual[indice].valor /tempoAtual);
				}
				else
				{
					if (netlistVirtual[indice].noControladoMais !=0) // no + != 0
					{
						if (netlistVirtual[indice].noControladoMenos !=0) // no + != 0 && no - != 0
						{
							matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) += 
								( ( matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0) - 
								 matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0))
								 * (netlistVirtual[indice].valor /tempoPassoInterno));

							matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0) +=
								( -( matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0) - 
								 matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0)) 
								 * (netlistVirtual[indice].valor /tempoPassoInterno));
						}
						else // no + != 0 && no - == 0
						{
							matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) += 
								( ( matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0))
								 * (netlistVirtual[indice].valor /tempoPassoInterno));

							matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0) +=
								( -( matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0)) 
								 * (netlistVirtual[indice].valor /tempoPassoInterno));
						}
					}
					else // no + == 0 && no - != 0
					{
						matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) += 
							(( 0 - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0))
							 * (netlistVirtual[indice].valor/tempoPassoInterno));

						matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0) +=
							(( 0 - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0)) 
							 * (netlistVirtual[indice].valor/tempoPassoInterno));
					}
				}
			}//Fim do BE
			else //Monta matrizes usando as estampas para o modo gear de segunda ordem
			{
				matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMais)   +=( (3*netlistVirtual[indice].valor)/(2*tempoPassoInterno));
				matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMenos)  +=(-(3*netlistVirtual[indice].valor)/(2*tempoPassoInterno));
				matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMais)  +=(-(3*netlistVirtual[indice].valor)/(2*tempoPassoInterno));
				matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMenos) +=( (3*netlistVirtual[indice].valor)/(2*tempoPassoInterno));

				if (interacao ==1)
				{
					if (netlistVirtual[indice].noControladoMais !=0) // no + != 0
					{
						if (netlistVirtual[indice].noControladoMenos !=0) // no + != 0 && no - != 0
						{
							matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) +=
								((( 2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0) - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0))
									* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor 
									* (netlistVirtual[indice].condicao)) / (2*tempoPassoInterno)));

							matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=
								(-((( 2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0) - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0)) 
									* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor
									*(netlistVirtual[indice].condicao)) / (2*tempoPassoInterno))));
						}
						else // no + != 0 && no - == 0
						{
							matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) +=
								((( 2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0))
									* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor 
									* (netlistVirtual[indice].condicao)) / (2*tempoPassoInterno)));

							matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=
								(-((( 2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0)) 
									* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor
									*(netlistVirtual[indice].condicao)) / (2*tempoPassoInterno))));
						}
					}
					else // no + == 0 && no - != 0
					{
						matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) +=
							((( 2 * ( - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0))
								* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor 
								* (netlistVirtual[indice].condicao)) / (2*tempoPassoInterno)));

						matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=
							(-((( 2 * ( - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0)) 
								* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor
								*(netlistVirtual[indice].condicao)) / (2*tempoPassoInterno))));
					}
				}
				
				if (netlistVirtual[indice].noControladoMais !=0) // no + != 0
				{
					if (netlistVirtual[indice].noControladoMenos !=0) // no + != 0 && no - != 0
					{
						matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) +=
							(((2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0) - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0))
								* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor 
								* (matrizNoTempoAnterior(netlistVirtual[indice].noControladoMais -1,0) - matrizNoTempoAnterior(netlistVirtual[indice].noControladoMenos -1,0))
								) / (2*tempoPassoInterno)));

						matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=
							(-(((2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0) - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0)) 
								* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor
								*(matrizNoTempoAnterior(netlistVirtual[indice].noControladoMais -1,0) - matrizNoTempoAnterior(netlistVirtual[indice].noControladoMenos -1,0))
								) / (2*tempoPassoInterno))));
					}
					else // no + != 0 && no - == 0
					{
						matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) +=
							(((2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0))
								* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor 
								* (matrizNoTempoAnterior(netlistVirtual[indice].noControladoMais -1,0))
								) / (2*tempoPassoInterno)));

						matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=
							(-(((2 * (matrizNoTempoAtual(netlistVirtual[indice].noControladoMais -1,0)) 
								* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor
								*(matrizNoTempoAnterior(netlistVirtual[indice].noControladoMais -1,0))
								) / (2*tempoPassoInterno))));
					}
				}
				else // no + == 0 && no - != 0
				{
					matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0) +=
						(((2 * (- matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0))
							* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor 
							* ( - matrizNoTempoAnterior(netlistVirtual[indice].noControladoMenos -1,0))
							) / (2*tempoPassoInterno)));

					matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0)+=
						(-(((2 * ( - matrizNoTempoAtual(netlistVirtual[indice].noControladoMenos -1,0)) 
							* netlistVirtual[indice].valor) / tempoPassoInterno) - ((netlistVirtual[indice].valor
							*( - matrizNoTempoAnterior(netlistVirtual[indice].noControladoMenos -1,0))
							) / (2*tempoPassoInterno))));
				}
			}//Fim do GEAR
		}//Fim do C
		else if(netlistVirtual[indice].tipo ==L)
		{
			matrizMNA.resize(qtd_cmp+1,qtd_cmp+1);
			matrizFontesIndependentes.resize(qtd_cmp+1,1);
			matrizFontesIndependentes(qtd_cmp,0) = 0;

			matrizMNA(netlistVirtual[indice].noControladoMais,qtd_cmp)  += (1);
			matrizMNA(netlistVirtual[indice].noControladoMenos,qtd_cmp) += (-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladoMais)  += (-1);
			matrizMNA(qtd_cmp,netlistVirtual[indice].noControladoMenos) += (1);

			if ((tipoDeSimulacao == BE)||(interacao == 0)) //Monta matrizes usando as estampas para o modo backward
			{
				if (interacao==0)
				{
					matrizMNA(qtd_cmp,qtd_cmp)   += (netlistVirtual[indice].valor / tempoAtual);
					
					matrizFontesIndependentes(qtd_cmp,0)  += ((netlistVirtual[indice].valor / tempoAtual)* netlistVirtual[indice].condicao);
				}
				else
				{
					matrizMNA(qtd_cmp,qtd_cmp) += (netlistVirtual[indice].valor / tempoPassoInterno);

					matrizFontesIndependentes(qtd_cmp,0)  += ((netlistVirtual[indice].valor / tempoPassoInterno)* matrizNoTempoAtual(qtd_cmp-1,0));
				}
			}//Fim do BE
			else //Monta matrizes usando as estampas para o modo gear de segunda ordem
			{
				matrizFontesIndependentes(qtd_cmp,0) += (netlistVirtual[indice].valor / tempoPassoInterno)*
									((2* matrizNoTempoAtual(qtd_cmp-1,0))
									- (matrizNoTempoAnterior(qtd_cmp-1,0))/2);
			}//Fim do GEAR

			//inclui a variavel corrente na fonte
			if ((interacao == 0 )&&(interacoesNR == 0)&&(convergiu == 0)&&(chutes == 0))
			{
				nomeDasVariaveis += std::string("   Icomp");
				nomeDasVariaveis += netlistVirtual[indice].nome;
				nomeDasVariaveis += std::string("(A)   ");
			}
			qtd_cmp++;
		}//Fim do L
		else if(netlistVirtual[indice].tipo == D)
		{
			if (netlistVirtual[indice].noControladoMais !=0) // no + != 0
			{
				if (netlistVirtual[indice].noControladoMenos !=0) // no + != 0 && no - != 0
				{
					v= matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].noControladoMais-1,0)
						- matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].noControladoMenos-1,0);
				}
				else // no + != 0 && no - == 0
				{
					v= matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].noControladoMais-1,0);
				}
			}
			else // no + == 0 && no - != 0
			{
				v= - matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].noControladoMenos-1,0);
			}
			
			if(v<(LIMIAR_V_VT * netlistVirtual[indice].Vt))
			{
				G= (netlistVirtual[indice].Is / netlistVirtual[indice].Vt) * exp(v/netlistVirtual[indice].Vt);

				I= (netlistVirtual[indice].Is * (exp(v/netlistVirtual[indice].Vt)-1)) - G * v;
			}
			else //modelado por uma reta
			{
				G= (netlistVirtual[indice].Is / netlistVirtual[indice].Vt) * (exp(LIMIAR_V_VT)-1);
				I= netlistVirtual[indice].Is *(exp(LIMIAR_V_VT)-1) + G * LIMIAR_V_VT;
			}
						 
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMais)   += ( G);
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladoMenos)  += (-G);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMais)  += (-G);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladoMenos) += ( G);

			matrizFontesIndependentes(netlistVirtual[indice].noControladoMais,0)  += (-I);
			matrizFontesIndependentes(netlistVirtual[indice].noControladoMenos,0) += ( I);

		}
		else if(netlistVirtual[indice].tipo == Q)
		{
			double Isbc, Vtbc, Isbe, Vtbe;
			if (netlistVirtual[indice].componenteEspecial ==PNP)
			{
				Isbc = -netlistVirtual[indice].Isbc;
				Vtbc = -netlistVirtual[indice].Vtbc;
				Isbe = -netlistVirtual[indice].Isbe;
				Vtbe = -netlistVirtual[indice].Vtbe;
			}
			else
			{
				Isbc = netlistVirtual[indice].Isbc;
				Vtbc = netlistVirtual[indice].Vtbc;
				Isbe = netlistVirtual[indice].Isbe;
				Vtbe = netlistVirtual[indice].Vtbe;
			}
			if (netlistVirtual[indice].base !=0) // base != 0
			{
				if (netlistVirtual[indice].coletor !=0) // base != 0 && coletor != 0
				{
					vbc= matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].base-1,0)
						- matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].coletor-1,0);
				}
				else // base != 0 && coletor == 0
				{
					vbc= matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].base-1,0);
				}

				if (netlistVirtual[indice].emissor !=0) // base != 0 && emissor != 0
				{
					vbe= matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].base-1,0) 
						- matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].emissor-1,0);
				}
				else // base != 0 && emissor == 0
				{
					vbe= matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].base-1,0);
				}
			}
			else // base == 0
			{
				if (netlistVirtual[indice].coletor !=0) // base == 0 && coletor != 0
				{
					vbc= - matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].coletor-1,0);
				}
				else // base == 0 && coletor == 0
				{
					vbc= 0;
				}

				if (netlistVirtual[indice].emissor !=0) // base == 0 && emissor != 0
				{
					vbe= - matrizNoTempoFuturoInteracaoN(netlistVirtual[indice].emissor-1,0);
				}
				else // base == 0 && emissor == 0
				{
					vbe= 0;
				}
			}

			if(vbc<(LIMIAR_V_VT * netlistVirtual[indice].Vtbc))
			{
				Gbc = (Isbc / Vtbc) * exp(vbc/Vtbc);

				Ibc= (Isbc * (exp(vbc/Vtbc) -1)) - Gbc * vbc;
			}
			else //modelado por reta
			{
				Gbc= (Isbc / Vtbc) * (exp(LIMIAR_V_VT) -1);

				Ibc= Isbc *(exp(LIMIAR_V_VT)-1) + G * LIMIAR_V_VT;
			}

			if(vbe<(LIMIAR_V_VT * netlistVirtual[indice].Vtbe))
			{
				Gbe= (Isbe / Vtbe) * exp(vbe/Vtbe);

				Ibe= (Isbe * (exp(vbe/Vtbe) -1)) - Gbe * vbe;
			}
			else //modelado por reta
			{
				Gbe= (Isbe / Vtbe) * (exp(LIMIAR_V_VT) -1);

				Ibe= Isbe *(exp(LIMIAR_V_VT)-1) + G * LIMIAR_V_VT;
			}

			//elementos Base-Emissor
			matrizMNA(netlistVirtual[indice].emissor, netlistVirtual[indice].emissor) += (Gbe*(1-netlistVirtual[indice].alfa));
			matrizMNA(netlistVirtual[indice].emissor, netlistVirtual[indice].base)    += (Gbe*(netlistVirtual[indice].alfa-1));
			matrizMNA(netlistVirtual[indice].base, netlistVirtual[indice].emissor)    += (Gbe*(netlistVirtual[indice].alfa-1));
			matrizMNA(netlistVirtual[indice].base, netlistVirtual[indice].base)       += (Gbe*(1-netlistVirtual[indice].alfa));

			//elementos Base-Coletor
			matrizMNA(netlistVirtual[indice].base, netlistVirtual[indice].base)       += (Gbc*(1-netlistVirtual[indice].alfar));
			matrizMNA(netlistVirtual[indice].base, netlistVirtual[indice].coletor)    += (Gbc*(netlistVirtual[indice].alfar-1));
			matrizMNA(netlistVirtual[indice].coletor, netlistVirtual[indice].base)    += (Gbc*(netlistVirtual[indice].alfar-1));
			matrizMNA(netlistVirtual[indice].coletor, netlistVirtual[indice].coletor) += (Gbc*(1-netlistVirtual[indice].alfar));

			matrizFontesIndependentes(netlistVirtual[indice].emissor,0)  += (Ibe - (netlistVirtual[indice].alfar * Ibc));
			matrizFontesIndependentes(netlistVirtual[indice].base,0)     += (( (netlistVirtual[indice].alfa-1) * Ibe) + ( (netlistVirtual[indice].alfar-1) * Ibc));
			matrizFontesIndependentes(netlistVirtual[indice].coletor,0)  += (Ibc - (netlistVirtual[indice].alfa * Ibe));
		}
		else
		{
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladorMais)+=(netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMais,netlistVirtual[indice].noControladorMenos)+=(-netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladorMais)+=(-netlistVirtual[indice].valor);
			matrizMNA(netlistVirtual[indice].noControladoMenos,netlistVirtual[indice].noControladorMenos)+=(netlistVirtual[indice].valor);
		}
	}
	// verificar se há nó de referência

	unsigned diferente_zero = 0;
	{
	for (unsigned i = 0; i< matrizMNA.cols() ; i++)
	{
		if (matrizMNA(0,i)!=0)
			diferente_zero = 1;
		if (matrizMNA(i,0)!=0)
			diferente_zero = 1;
	}
	}
	if (diferente_zero ==0)
		return (SEM_NO_DE_REFERENCIA);

	ajustaMatriz();
	return(OK);
}

unsigned Aplicativo::cad()
{
	double erroAceitavel = 1e-7;
	PACC::Matrix matrizNoTempoFuturoInteracaoNmaisUm;
	unsigned i;
	unsigned escrever = 0;
	double tempoPassoInterno = (passo/passoInterno);
	unsigned numeroPassos = (unsigned)floor(tempoFinal/passo) +1;
	
	interacoesNR = 0;
	convergiu = 0;
	valor_muito_grande = 0;
	
	if (chutes == MAX_NUM_CHUTES)
	{
		valor_muito_grande = 0;
		return (NAO_CONVERGIU);
	}

	//Escrevendo o nome das variaveis na string que sera usada como cabecario do resultado
	nomeDasVariaveis = std::string("#     Time(s)   ");
	{
	for (i = 1; i<=numeroDeNos; i++)
	{
		std::ostringstream temp("");
		nomeDasVariaveis += std::string("   Vno'");
		if (i<10)
			temp <<"00" << i;
		else if (i<100)
			temp <<"0" << i;
		else
			temp << i;
		nomeDasVariaveis += temp.str();
		nomeDasVariaveis += std::string("(V)   ");
	}
	}
	
	//Escrevendo os tempos na variavel de saída
	saidaDoPrograma.resize((numeroPassos + 1),1);
	saidaDoPrograma(0,0) = tempoPassoInterno/1000;
	tempoAtual = saidaDoPrograma(0,0);
	saidaDoPrograma(1,0) = passo;

	{
	for (i = 2; (saidaDoPrograma(i-1,0) + passo) <=tempoFinal; i++)
	{
		saidaDoPrograma(i,0)= saidaDoPrograma(i-1,0) + passo;
	}
	}

	//Inicializando as matrizes, todos os valores sao zero
	matrizMNA.resize(numeroDeNos +1, numeroDeNos +1);
	matrizFontesIndependentes.resize(numeroDeNos +1, 1);
	matrizMNA = matrizMNA - matrizMNA;
	matrizFontesIndependentes = matrizFontesIndependentes - matrizFontesIndependentes;

	interacao = 0;

	matrizNoTempoFuturoInteracaoN.resize(numeroDeNos +1, 1);//to + delta t

	// Criando a semente para os numeros aleatorios	
	srand( time(NULL) );
	
	//Criando a matriz aleatoria para comecar o metodo de convergencia Newton-Raphson
	{
	for (i = 0;i <= numeroDeNos; i++)
		matrizNoTempoFuturoInteracaoN(i,0)=((rand()%200)-100)/10;
	}

	//Monta as matrizes, se der erro sai do programa
	if((erro = montaMatrizes ()) != OK)
	{
		valor_muito_grande = 0;
		return (erro);
	}

	//Terminando de inicializar as matrizes, todos os valores sao zero, dependia da quantidade de termos especiais
	//Termos Especiais: elementos que tem as correntes representadas na MNA
	matrizNoTempoAtual.resize(matrizMNA.cols(), 1);//to
	matrizNoTempoAnterior.resize(matrizMNA.cols(), 1);//to - delta t
	matrizNoTempoFuturoInteracaoN.resize(matrizMNA.cols(), 1);//to + delta t
	matrizNoTempoFuturoInteracaoNmaisUm.resize(matrizMNA.cols(), 1);//to + delta t
	
	//Completando a matriz aleatoria para comecar o metodo de convergencia Newton-Raphson
	{
	for (;i<matrizMNA.cols(); i++)
		matrizNoTempoFuturoInteracaoN(i,0)=((rand()%200)-100)/10;
	}
	
	//Ajustando o tamanho
	saidaDoPrograma.resize((numeroPassos + 1),(matrizMNA.cols()+1));
	
	//Loop que calcula as interacoes para todos os tempos
	do
	{
		//Ate que haja convergencia, da um novo chute.
		do
		{
			//Para um determinado tempo, calcula a convergencia por NR
			do
			{
				convergiu = 0;

				//Resolvendo o sistema
				matrizNoTempoFuturoInteracaoNmaisUm = matrizMNA.invert() * matrizFontesIndependentes; 

				//Testando a convergencia termo a termo
				{
				for (i = 0;i<matrizMNA.cols(); i++)
					//Se cada elemento da matriz tiver convergido, a matriz convergiu.
					if(fabs(matrizNoTempoFuturoInteracaoNmaisUm(i,0) - matrizNoTempoFuturoInteracaoN(i,0))>erroAceitavel)
						convergiu += 1;
				}

				//atualizando o nomero da interacao
				interacoesNR ++;

				if (convergiu!=0)
				{
					//Calculando as matrizes para a proxima interacao de NR
					//Se esta for necessaria
					matrizMNA = matrizMNA - matrizMNA;
					matrizFontesIndependentes = matrizFontesIndependentes - matrizFontesIndependentes;
					if((erro = montaMatrizes ()) != OK)
					{
						valor_muito_grande = 0;
						return (erro);
					}
				}
				//fazendo a matriz n(to + delta t) = n+1(to + delta t)
				matrizNoTempoFuturoInteracaoN = matrizNoTempoFuturoInteracaoNmaisUm;
				
			} while ((convergiu != 0) && (interacoesNR < MAX_INTERACAO));

			//Zerando a variavel interacoesNR para comecar a contar de novo
			interacoesNR = 0;

			if ((convergiu != 0))
			//Criando a matriz aleatória para comecar o metodo de convergencia Newton-Raphson
			{
				for (i = 0;i<matrizMNA.cols(); i++)
					matrizNoTempoFuturoInteracaoN(i,0)=((rand()%200)-100)/10;
			}
			
			//atualizando o numero do chute
			chutes ++;

			//montando a nova matriz para a nova interacao
			matrizMNA = matrizMNA - matrizMNA;
			matrizFontesIndependentes = matrizFontesIndependentes - matrizFontesIndependentes;
			if((erro = montaMatrizes ()) != OK)
			{
				valor_muito_grande = 0;
				return (erro);
			}

		} while (( convergiu!=0) && (chutes < MAX_NUM_CHUTES));

		//Se nao convergiu, saia do programa
		if (convergiu!=0)
		{
			valor_muito_grande = 0;
			return (NAO_CONVERGIU);
		}

		//escrevendo na matriz resultado o valor convergido
		if (interacao == passoInterno * escrever)
		{
			for (i=0; i<matrizNoTempoFuturoInteracaoN.rows(); i++)
			{
				saidaDoPrograma(escrever,i+1) = matrizNoTempoFuturoInteracaoN(i,0);
				if ((matrizNoTempoFuturoInteracaoN(i,0)>VALOR_MAX)||(matrizNoTempoFuturoInteracaoN(i,0)<-VALOR_MAX))
				{
					valor_muito_grande = 1;
					return (ERRO_OVERFLOW);
				}
			}
			escrever ++;
		}
		//atualizando o tempo
		if (interacao == 0)
			tempoAtual = tempoPassoInterno;
		else
			tempoAtual += tempoPassoInterno;

		interacao++;//aumentando o numero da interacao

		//Se preparando para o calculo para o proximo tempo.
		matrizNoTempoAnterior = matrizNoTempoAtual; //atualizando a matriz to - delta t
		matrizNoTempoAtual = matrizNoTempoFuturoInteracaoN;// atualizando a matriz to

		//Zerando a variavel chutes para comecar a contar de novo
		chutes = 0;

	} while(tempoAtual <= tempoFinal);

	//Ajustando o tamanho
	saidaDoPrograma.resize((escrever + 1),(matrizMNA.cols()+1));

	//escrevendo na matriz resultado o valor convergido
	if (saidaDoPrograma(escrever,0)!=0)
		for (i=0; i<matrizNoTempoFuturoInteracaoN.rows(); i++)
			saidaDoPrograma(escrever,i+1) = matrizNoTempoFuturoInteracaoN(i,0);
	else
		saidaDoPrograma.resize((escrever),(matrizMNA.cols()+1));

	valor_muito_grande = 0;
	return OK;
}

unsigned Aplicativo::escreverNoArquivo()
{
	std::string arquivo = strtok((char *) nomeDoArquivo.c_str(),".");
	arquivo += ".dat";

	fileOut.open(arquivo.c_str());
	if (!fileOut.is_open())
		return(ERRO_WRITE_FILE);

	fileOut << nomeDasVariaveis <<std::endl;
	fileOut << saidaDoPrograma;

	return (OK);
}

Aplicativo::Aplicativo()
{
}

Aplicativo::~Aplicativo()
{
}

Aplicativo::Aplicativo(std::string nomeArquivo)
{
	nomeDoArquivo = nomeArquivo.substr();
	chutes = 0;
	
	file.open(nomeDoArquivo.c_str());

	if (!file.is_open())
		erro = INVALID_FILE;
	else if(montaNetlistVirtual()==OK)
		erro = cad();

	if (erro == OK)
		erro = escreverNoArquivo();
}
