#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h> // entrada usuario e manipulação de janelas (obs: AQUI SO FOI USADO GLUT)
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h" // Biblioteca para descompactar a foto em pixels brutos (PNG ou JPEG)

#define MAX_VERTICES 2000
#define MAX_FACES 2000

// gcc textura.c -lfreeglut -lopengl32 -lglu32 -lm -o textura
// ou
// gcc textura.c -lglut -lGL -lGLU -lm -o textura

static GLfloat yRot = -90.0f;
static GLfloat zPos = -10.0f; // Afastado para visualizar o castelo
static GLfloat xRot = 0.0f;

// Esquema LV (lista de vértices), uma estrutura de dados para vértices, e faces referenciando os vértices:
// Nesse tipo de dado, eu armazeno cada vértice existente em um vetor (3D) suas posições no espaço R3
// Para textura eu armazeno o mapa nas dimensões (s,t)
// Como a malha do meu objeto foi modelada por triangulos, as faces terão 3 vértices.
typedef struct { float x, y, z; } Vertice;
typedef struct { float s, t; } Textura;
typedef struct { int v[3], t[3], n[3]; } FaceTriangular; // obj renderizado como triangulated mesh, logo 3 vértices

// listas que serão utilizadas
Vertice listaVertices[MAX_VERTICES];
Textura listaTexturas[MAX_VERTICES];
FaceTriangular listaFaces[MAX_FACES];

int contV = 0, contT = 0, contF = 0; // quantidade vértices, vértices de textura e faces
GLuint texturaID; 

// parser simples para obj, vetices normais são lidos mas não serão usados para gerar os poligonos
void carregarOBJ(const char* caminho) {
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo OBJ: %s!\n", caminho);
        exit(1);
    }

    char tipo[128]; // buffer
    
    while ( fscanf(arquivo, "%s", tipo) != -1) { 
        if (strcmp(tipo, "v") == 0) { // vertice
            fscanf(arquivo, "%f %f %f", &listaVertices[contV].x, &listaVertices[contV].y, &listaVertices[contV].z);
            contV++;
        }
        else if (strcmp(tipo, "vt") == 0) { // vertice textura
            fscanf(arquivo, "%f %f", &listaTexturas[contT].s, &listaTexturas[contT].t);
            contT++;
        }
        else if (strcmp(tipo, "f") == 0) { // face
            int v1, t1, n1, v2, t2, n2, v3, t3, n3; // considerando triangulo
            
            // o Blender usou Triangulated Mesh e armazenou os dados no .obj, do modo: (armazena o indice dos vertices)
            // f vértice1/textura1/normal1 vértice2/textura2/normal2 vértice3/textura3/normal3
            fscanf(arquivo, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);

            listaFaces[contF].v[0] = v1 - 1; // menos um, indexação começa em 0
            listaFaces[contF].t[0] = t1 - 1;

            listaFaces[contF].v[1] = v2 - 1;
            listaFaces[contF].t[1] = t2 - 1;

            listaFaces[contF].v[2] = v3 - 1;
            listaFaces[contF].t[2] = t3 - 1;

            contF++;
        }
    }
    fclose(arquivo);
    printf("OBJ carregado com: %d Vertices, %d Faces.\n", contV, contF);
}


void SpecialKeys(int key, int x, int y) {  // Teclas usadas
    if(key == GLUT_KEY_LEFT) yRot -= 5.0f;  
    if(key == GLUT_KEY_RIGHT) yRot += 5.0f;  
    if(key == GLUT_KEY_UP) zPos += 1.0f;
    if(key == GLUT_KEY_DOWN) zPos -= 1.0f;
    if(key == GLUT_KEY_PAGE_UP) xRot += 5.0f;
    if(key == GLUT_KEY_PAGE_DOWN) xRot -= 5.0f;

    yRot = (GLfloat)((const int)yRot % 360);  
    xRot = (GLfloat)((const int)xRot % 360); 
    glutPostRedisplay();  
}

void init() { 
    glClearColor(0.0, 0.0, 0.0, 1.0); // cor de fundo 

    glEnable(GL_DEPTH_TEST);   // ativa algoritmo de Remoção de Faces ocultas por meio do z-buffer       
    glEnable(GL_TEXTURE_2D);   // ligar mapeamento de texturas, é uma imagem BIDIMENSIONAL (2D)   
    
    // A textura (2D) é praticamente uma função T(s,t) que armazena uma matriz de texels (texture elements),
    // que é praticamente uma matriz bruta de cores. Como uma imagem JPG é comprimida, usamos stbi para descomprimir e armazenar os dados da imagem;
    int width, height, channels; // widht = s, height = t
    stbi_set_flip_vertically_on_load(true); // 
    unsigned char* img = stbi_load("textura2.png", &width, &height, &channels, 3);
    if (!img) {
        printf("Erro ao carregar a imagem! Verifique se ela esta na mesma pasta.\n");
        exit(1);
    }

    glGenTextures(1, &texturaID); // Identificador para a foto da caixa de leite
    glBindTexture(GL_TEXTURE_2D, texturaID); // Marca que todas as configurações seguintes serão para a textura identificada por "texturaID"
    // filtragem para evitar aliasing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // funcao principal: glTexImage2D( target, nivel_detalhe, components, w, h, border, format, type, *texels );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img); 
}

void display() { // gera o objeto com as texturas
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glLoadIdentity();
    glPushMatrix();

    glTranslatef(0.0f, -1.0f, zPos); 
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);

   
    glBindTexture(GL_TEXTURE_2D, texturaID); // Marca que todas as configurações seguintes serão para a textura identificada por "texturaID"

    // chão (sem textura)
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex3f(-30.0f, -2.5f,  30.0f); 
        glVertex3f( 30.0f, -2.5f,  30.0f); 
        glVertex3f( 30.0f, -2.5f, -30.0f); 
        glVertex3f(-30.0f, -2.5f, -30.0f); 
    glEnd();

    // religar mapeamento
    glEnable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f); // pinta (antes de tudo) o solido de branco. Cor de fundo embaixo da imagem de textura (para caso de erros)
    glBegin(GL_TRIANGLES); 
    for (int i = 0; i < contF; i++) {
        for (int j = 0; j < 3; j++) { 
            int idT = listaFaces[i].t[j];
            int idV = listaFaces[i].v[j];

            glTexCoord2f(listaTexturas[idT].s, listaTexturas[idT].t);
            glVertex3f(listaVertices[idV].x, listaVertices[idV].y, listaVertices[idV].z); 
        }
    }
    glEnd();


    glPopMatrix();
    glutSwapBuffers();
    glutPostRedisplay(); 
}

void reshape(int w, int h) { // Pipeline de Visualização 3D
    glViewport(0, 0, w, h); // viewport de acordo com resolução do monitor (nesse caso 800x600)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0); // cria o volume de visualização
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    carregarOBJ("cxleiteTriangulateMesh.obj");

    glutInit(&argc, argv); // incializa biblioteca GLUT e suas configurações em seguida
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("trabalho CGR - TC5");

    init(); 

    glutSpecialFunc(SpecialKeys);  // SpecialKeys chamada para "teclas especiais"
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop(); // loop infinito
    return 0;
}