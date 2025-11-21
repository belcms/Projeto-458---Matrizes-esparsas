# Implementação e Análise de Estruturas para Matrizes Esparsas

Este repositório contém a implementação de duas estruturas de dados distintas projetadas para o armazenamento e manipulação eficiente de **Matrizes Esparsas** em C++. O projeto foca na otimização de memória e tempo de processamento, comparando uma abordagem baseada em **Tabelas Hash** com uma baseada em **Árvores Balanceadas**.

## 📖 Introdução

Uma matriz esparsa é, por definição, uma matriz em que a maioria dos elementos é igual a zero.

Mais formalmente, uma matriz $A \in \mathbb{R}^{n \times m}$ é considerada esparsa se o número de elementos não nulos $k$ satisfaz a seguinte condição:

$$k \ll n \times m$$

Isto é, o número de elementos não nulos ($k$) é muito menor que o número total de posições possíveis ($n \times m$).

Matrizes esparsas aparecem frequentemente nos mais diversos campos da computação e matemática (como processamento de imagens, aprendizado de máquina e simulações físicas), por vezes representando processos reais que dependem de eficiência e otimização.

Este projeto visa, por meio da ótica experimental e teórica, implementar e comparar o uso de duas estruturas distintas que promovem o uso mais eficiente de memória O(k) e poder computacional em comparação à representação densa tradicional.

---

## 🏗️ Estruturas Implementadas

### Estrutura 1: Hash de Hashes (Hash Table)
Focada no acesso rápido e inserção em tempo constante esperado.

* **Arquitetura:** Um **Hash Externo** mapeia os índices das linhas ($i$) para buckets. Cada nó de linha contém um **Hash Interno** que mapeia os índices das colunas ($j$) para os valores.

### Estrutura 2: Árvore Rubro-Negra (Balanced Tree)
Focada em operações ordenadas e robustez no pior caso.

* **Arquitetura:** Utiliza árvores binárias de busca balanceadas (Rubro-Negras) para armazenar os elementos não nulos.

---

## Autores
* **Isabel Cristina Marras Salles - RA 188790**
* **Victor Luigi Roquetto - RA 250955**
* **Rafael Feltrin Lamen Rocha - RA 276246**
