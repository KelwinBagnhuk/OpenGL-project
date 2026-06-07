# Trabalhos de Computação Gráfica: Texturização e Iluminação Global

Este repositório contém a implementação dos Trabalhos Complementares da disciplina de Computação Gráfica, focados em **Mapeamento de Textura** e de **Modelos de Iluminação**.

O repositório é dividido em duas partes, partindo de um modelo de uma "Caixa de Leite" modelada e texturizada por meio de fotos reais.

### Parte 1: Parser simples para `.obj` e Texturização em OpenGL (C)
Foi desenvolvido um programa em linguagem C puro utilizando OpenGL. 
(como o objeto sera simples e definido, verificações de memoria, como tamanhos dos vetores, não foram feitas)

### Parte 2: Comparação Analítica (Iluminação Local vs. Iluminação Global)
Imagem comparativa (`comparacao.png`) mostrando o objeto renderizado no programa em C (OpenGL) lado a lado com uma renderização do mesmo modelo no Blender (utilizando o motor Cycles). 

---

## Como rodar:

**Pré-requisitos:**
* Compilador C (GCC)
* Bibliotecas OpenGL, GLU e FreeGLUT instaladas.
* O arquivo header `stb_image.h` na mesma pasta do código.

```bash
gcc main.c -lglut -lGL -lGLU -lm -o textura
```
Ou -lfreeglut
