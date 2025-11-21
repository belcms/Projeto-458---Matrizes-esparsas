#ifndef MATRIZ_NORMAL_HPP
#define MATRIZ_NORMAL_HPP

#include <vector>
#include <iostream>
#include <stdexcept>

using namespace std;

class MatrizNormal {
private:
	int linhas;
	int colunas;
	vector<double> dados;

	inline int indice(int i, int j) const {
		return i * colunas + j;
	}


public:
	MatrizNormal(int linhas = 0, int colunas = 0, double valor_inicial = 0.0)
		: linhas(linhas), colunas(colunas), dados(linhas * colunas, valor_inicial) {}


	void limpar() {
		linhas = 0;
		colunas = 0;
		dados.clear();
	}

	void inserir(int i, int j, double valor) {
		dados[indice(i, j)] = valor;
	}

	double acessar(int i, int j) const {
		return dados[indice(i, j)];
	}

	void multiplicar_por_escalar(double escalar) {
		for (double &valor : dados) {
			valor *= escalar;
		}
	}

	MatrizNormal somar(const MatrizNormal &outra) const {
		MatrizNormal resultado(linhas, colunas, 0.0);

		for (size_t idx = 0; idx < dados.size(); ++idx) {
			resultado.dados[idx] = dados[idx] + outra.dados[idx];
		}
		return resultado;
	}
	MatrizNormal multiplicar(const MatrizNormal &outra) const {
        MatrizNormal resultado(linhas, outra.colunas, 0.0);
    
        for (int i = 0; i < linhas; ++i) {        
            for (int j = 0; j < outra.colunas; ++j) { 
                
                double soma = 0.0;
                
                for (int k = 0; k < colunas; ++k) { 
                    double valor_a = dados[i * colunas + k];
                    double valor_b = outra.dados[k * outra.colunas + j];
                    
                    soma += valor_a * valor_b;
                }
                
                resultado.dados[i * outra.colunas + j] = soma;
            }
        }
        return resultado;
    }

	MatrizNormal transposta() const {
		MatrizNormal resultado(colunas, linhas, 0.0);
		for (int i = 0; i < linhas; ++i) {
			for (int j = 0; j < colunas; ++j) {
				resultado.dados[j * linhas + i] = dados[indice(i, j)];
			}
		}
		return resultado;
	}
};

#endif