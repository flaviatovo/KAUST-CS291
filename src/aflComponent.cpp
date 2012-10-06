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
 * Arquivo de Componente.
 * Versao:5.0 Funcionando 100%
 */


#include <stdlib.h>

#include "aflComponent.h"

/*class Componente //R, C, L, D, E, F, G, H, V, I, Q
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
	Componente(std::string linha);

	protected:
};
*/

unsigned long Componente::avaliarCondicaoInicial()
{
	if ((tipo==C)||(tipo==L))
	{
		std::string conInicial = condicaoInicial.substr();
		//transforma condicaoInicial em condicao
		std::string temp = strtok((char *) conInicial.c_str(),"=");
		//strtok pode ser feito com um uso combinado de uma variavel i, que guarda a posicao do
		//caracter, a funcao string::find e posteriormente string::copy ou a string::substr.
		//Caso nao seja possivel arrumar essa funcao, a alternativa eh uma boa opcao.
        temp = strtok(NULL,"=");
        condicao = strtod(temp.c_str(), NULL);
	}
	return (OK);
}

unsigned Componente::avaliarParametro()
{
	if ((parametro.compare("NPN")==0)||(parametro.compare("npn")==0))
	{
		componenteEspecial = NPN;
		return(OK);
	}
	componenteEspecial = PNP;
	return(OK);
}

unsigned Componente::avaliarTipoComponente()
{
	switch(nome[0])
		{
			case 'r':
			case 'R':
				tipo = R;
				break;
			case 'c':
			case 'C':
				tipo = C;
				break;
			case 'l':
			case 'L':
				tipo = L;
				break;
			case 'd':
			case 'D':
				tipo = D;
				break;
			case 'e':
			case 'E':
				tipo = E;
				break;
			case 'f':
			case 'F':
				tipo = F;
				break;
			case 'g':
			case 'G':
				tipo = G;
				break;
			case 'h':
			case 'H':
				tipo = H;
				break;
			case 'v':
			case 'V':
				tipo = V;
				break;
			case 'i':
			case 'I':
				tipo = I;
				break;
			case 'q':
			case 'Q':
				tipo = Q;
				break;
			default:
				return (NOT_VALID_ELEMENT);
			break;
	   } 
	   return (OK);
}

Componente::Componente()
{
	nome.erase(); linha.erase();
	condicaoInicial.erase();
	parametro.erase();
	noControladoMais = 0; noControladoMenos = 0;
	noControladorMais = 0; noControladorMenos = 0;
	emissor = 0; base = 0; coletor = 0;
	ciclos = 0; periodo = 0; frequencia = 0;
	valor = 0; valor2 = 0; condicao = 0;
	Is = 0; Vt = 0; Isbc = 0; Vtbc = 0; Isbe = 0; Vtbe = 0;
	atraso = 0; tempo_subida = 0; tempo_descida = 0;
	tempo_ligada = 0; angulo = 0; fator_atenuacao = 0;
	alfa = 0; alfar = 0;
}

Componente::~Componente()
{
}

Componente::Componente(std::string linha_Do_Arquivo)
{
	//Monta o componente a partir da linha do arquivo.
	std::string temporario;

	linha=linha_Do_Arquivo.substr();
	std::istringstream linhaDoArquivo(linha_Do_Arquivo);
	linhaDoArquivo >> nome;

	//inicializando as variaveis
	condicaoInicial.erase();
	parametro.erase();
	noControladoMais = 0; noControladoMenos = 0;
	noControladorMais = 0; noControladorMenos = 0;
	emissor = 0; base = 0; coletor = 0;
	ciclos = 0; periodo = 0; frequencia = 0;
	valor = 0; valor2 = 0; condicao = 0;
	Is = 0; Vt = 0; Isbc = 0; Vtbc = 0; Isbe = 0; Vtbe = 0;
	atraso = 0; tempo_subida = 0; tempo_descida = 0;
	tempo_ligada = 0; angulo = 0; fator_atenuacao = 0;
	alfa = 0; alfar = 0;

	Componente::avaliarTipoComponente();
	if (tipo == R)
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);
	}
	else if (tipo == C)
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);

		temporario = ("");
		linhaDoArquivo >> temporario;
		if (temporario.length()>0)
		{
			condicaoInicial=temporario.substr();
			Componente::avaliarCondicaoInicial();
		}
	}
	else if (tipo == L)
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);

		temporario = ("");
		linhaDoArquivo >> temporario;
		if (temporario.length()>0)
		{
			condicaoInicial=temporario.substr();
			Componente::avaliarCondicaoInicial();
		}
	}
	else if (tipo == D)
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		temporario = ("");
		linhaDoArquivo >> temporario;
		if (temporario.length()>0)
		{
			Is = strtod(temporario.c_str(), NULL);

			linhaDoArquivo >> temporario;
			if (temporario.length()>0)
				Vt = strtod(temporario.c_str(), NULL);
			else
				Vt = VT;
		}
		else
		{
			Is = IS;
			Vt = VT;
		}
	}
	else if (tipo == E) /*Usa a variavel valor para guardar o Av*/
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);
	}
	else if (tipo == F) /*Usa a variavel valor para guardar o Ai*/
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);
	}
	else if (tipo == G) /*Usa a variavel valor para guardar o Gm*/
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);
	}
	else if (tipo == H) /*Usa a variavel valor para guardar o Rm*/
	{
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladorMenos = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		valor = strtod(temporario.c_str(), NULL);
	}
	else if (tipo == V)
	{
		/*no + = noControladoMais
			no - = noControladoMenos
			DC
				valor = valor
			PULSE
				amplitude 1 = valor
				amplitude 2 = auxiliar_2
				atraso = auxiliar_1
				periodo = auxiliar_3
				tempo subida = auxiliar_4
				tempo descida = auxiliar_5
				tempo ligada = auxiliar_6
				numero de ciclos = noControladorMais
			SIN
				amplitude = valor
				atraso = auxiliar_1
				nivel continuo = condicao
				fator de atenuacao = auxiliar_2
				angulo = auxiliar_3
				frequencia = auxiliar_4
				numero de cilocs = noControladorMais */
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		temporario = ("");
		linhaDoArquivo >> parametro;
		if ((parametro.compare("PULSE(")==0)||(parametro.compare("pulse(")==0)||(parametro.compare("PULSE")==0)||(parametro.compare("pulse")==0))
		{
			componenteEspecial = PULSE;
			if ((parametro.length()==5))
			{
				linhaDoArquivo >> temporario;
			}
			//amplitude 1
			linhaDoArquivo >> temporario;
			valor = strtod(temporario.c_str(), NULL);
			//amplitude 2
			linhaDoArquivo >> temporario;
			valor2 = strtod(temporario.c_str(), NULL);
			//atraso
			linhaDoArquivo >> temporario;
			atraso = strtod(temporario.c_str(), NULL);
			//tempo de subida
			linhaDoArquivo >> temporario;
			tempo_subida = strtod(temporario.c_str(), NULL);
			//tempo de descida
			linhaDoArquivo >> temporario;
			tempo_descida = strtod(temporario.c_str(), NULL);
			//tempo ligada
			linhaDoArquivo >> temporario;
			tempo_ligada = strtod(temporario.c_str(), NULL);
			//periodo
			linhaDoArquivo >> temporario;
			periodo = strtod(temporario.c_str(), NULL);
			//numero de ciclos
			linhaDoArquivo >> temporario;
			ciclos = strtoul(temporario.c_str(), NULL, 10);
		}
		else if ((parametro.compare("SIN(")==0)||(parametro.compare("sin(")==0)||(parametro.compare("SIN")==0)||(parametro.compare("sin")==0))
		{
			componenteEspecial = SIN;
			if ((parametro.length()==3))
			{
				linhaDoArquivo >> temporario;
			}
			//nivel continuo
			linhaDoArquivo >> temporario;
			valor2 = strtod(temporario.c_str(), NULL);
			//amplitude
			linhaDoArquivo >> temporario;
			valor = strtod(temporario.c_str(), NULL);
			//frequencia
			linhaDoArquivo >> temporario;
			frequencia = strtod(temporario.c_str(), NULL);
			//atraso
			linhaDoArquivo >> temporario;
			atraso = strtod(temporario.c_str(), NULL);
			//fator de atenuacao
			linhaDoArquivo >> temporario;
			fator_atenuacao = strtod(temporario.c_str(), NULL);
			//angulo
			linhaDoArquivo >> temporario;
			angulo = strtod(temporario.c_str(), NULL);
			//numero de ciclos
			linhaDoArquivo >> temporario;
			ciclos = strtoul(temporario.c_str(), NULL, 10);
			//periodo
			periodo = 1/frequencia;
		}
		else if ((parametro.compare("DC")==0)||(parametro.compare("dc")==0))
		{
			componenteEspecial = DC;

			linhaDoArquivo >> temporario;
			valor = strtod(temporario.c_str(), NULL);
		}
	}
	else if (tipo == I)
	{
		/*no + = noControladoMais
			no - = noControladoMenos
			DC
				valor = valor
			PULSE
				amplitude 1 = valor
				amplitude 2 = auxiliar_2
				atraso = auxiliar_1
				periodo = auxiliar_3
				tempo subida = auxiliar_4
				tempo descida = auxiliar_5
				tempo ligada = auxiliar_6
				numero de ciclos = noControladorMais
			SIN
				amplitude = valor
				atraso = auxiliar_1
				nivel continuo = condicao
				fator de atenuacao = auxiliar_2
				angulo = auxiliar_3
				frequencia = auxiliar_4
				numero de cilocs = noControladorMais */
		linhaDoArquivo >> temporario;
		noControladoMais = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		noControladoMenos = strtoul(temporario.c_str(), NULL, 10);

		temporario = ("");
		linhaDoArquivo >> parametro;
		if ((parametro.compare("PULSE(")==0)||(parametro.compare("pulse(")==0)||(parametro.compare("PULSE")==0)||(parametro.compare("pulse")==0))
		{
			componenteEspecial = PULSE;
			if ((parametro.length()==5))
			{
				linhaDoArquivo >> temporario;
			}
			//amplitude 1
			linhaDoArquivo >> temporario;
			valor = strtod(temporario.c_str(), NULL);
			//amplitude 2
			linhaDoArquivo >> temporario;
			valor2 = strtod(temporario.c_str(), NULL);
			//atraso
			linhaDoArquivo >> temporario;
			atraso = strtod(temporario.c_str(), NULL);
			//tempo de subida
			linhaDoArquivo >> temporario;
			tempo_subida = strtod(temporario.c_str(), NULL);
			//tempo de descida
			linhaDoArquivo >> temporario;
			tempo_descida = strtod(temporario.c_str(), NULL);
			//tempo ligada
			linhaDoArquivo >> temporario;
			tempo_ligada = strtod(temporario.c_str(), NULL);
			//periodo
			linhaDoArquivo >> temporario;
			periodo = strtod(temporario.c_str(), NULL);
			//numero de ciclos
			linhaDoArquivo >> temporario;
			ciclos = strtoul(temporario.c_str(), NULL, 10);
		}
		if ((parametro.compare("SIN(")==0)||(parametro.compare("sin(")==0)||(parametro.compare("SIN")==0)||(parametro.compare("sin")==0))
		{
			componenteEspecial = SIN;
			if ((parametro.length()==3))
			{
				linhaDoArquivo >> temporario;
			}
			//nivel continuo
			linhaDoArquivo >> temporario;
			valor2 = strtod(temporario.c_str(), NULL);
			//amplitude
			linhaDoArquivo >> temporario;
			valor = strtod(temporario.c_str(), NULL);
			//frequencia
			linhaDoArquivo >> temporario;
			frequencia = strtod(temporario.c_str(), NULL);
			//atraso
			linhaDoArquivo >> temporario;
			atraso = strtod(temporario.c_str(), NULL);
			//fator de atenuacao
			linhaDoArquivo >> temporario;
			fator_atenuacao = strtod(temporario.c_str(), NULL);
			//angulo
			linhaDoArquivo >> temporario;
			angulo = strtod(temporario.c_str(), NULL);
			//numero de ciclos
			linhaDoArquivo >> temporario;
			ciclos = strtoul(temporario.c_str(), NULL, 10);
			//periodo
			periodo = 1/frequencia;
		}
		else if ((parametro.compare("DC")==0)||(parametro.compare("dc")==0))
		{
			componenteEspecial = DC;

			linhaDoArquivo >> temporario;
			valor = strtod(temporario.c_str(), NULL);
		}
	}
	else if (tipo == Q)
	{
		/*no c = coletor
			no b = base
			no e = emissor
			alfa = alfa
			alfar = alfar
			Isbe = Isbe
			Vtbe = Vtbe
			Isbc = Isbc
			Vtbc = Vtbc */
		linhaDoArquivo >> temporario;
		coletor = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		base = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> temporario;
		emissor = strtoul(temporario.c_str(), NULL, 10);

		linhaDoArquivo >> parametro;
		Componente::avaliarParametro();

		temporario = ("");
		linhaDoArquivo >> temporario;
		if (temporario.length()>0)
		{
			alfa = strtod(temporario.c_str(), NULL);

			temporario = ("");
			linhaDoArquivo >> temporario;
			if (temporario.length()>0)
			{
				alfar = strtod(temporario.c_str(), NULL);

				temporario = ("");
				linhaDoArquivo >> temporario;
				if (temporario.length()>0)
				{
					Isbe = strtod(temporario.c_str(), NULL);

					temporario = ("");
					linhaDoArquivo >> temporario;
					if (temporario.length()>0)
					{
						Vtbe = strtod(temporario.c_str(), NULL);

						temporario = ("");
						linhaDoArquivo >> temporario;
						if (temporario.length()>0)
						{
							Isbc = strtod(temporario.c_str(), NULL);

							temporario = ("");
							linhaDoArquivo >> temporario;
							if (temporario.length()>0)
								Vtbc = strtod(temporario.c_str(), NULL);
							else
								Vtbc = VT;
						}
						else
						{
							Isbc = IS;
							Vtbc = VT;
						}
					}
					else
					{
						Vtbe = VT;
						Isbc = IS;
						Vtbc = VT;
					}
				}
				else
				{
					Isbe = IS;
					Vtbe = VT;
					Isbc = IS;
					Vtbc = VT;
				}
			}
			else
			{
				alfar = ALFAR;
				Isbe = IS;
				Vtbe = VT;
				Isbc = IS;
				Vtbc = VT;
			}
		}
		else
		{
			alfa = ALFA;
			alfar = ALFAR;
			Isbe = IS;
			Vtbe = VT;
			Isbc = IS;
			Vtbc = VT;
		}
	}
}
