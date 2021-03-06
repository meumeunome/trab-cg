#include <GL/glut.h>
#include <stdio.h>
#include <list>
#include "tinyxml/tinyxml.h"
#include "math.h"
#include "circulo.h"
#include "linha.h"
#include "arena.h"
#include "jogador.h"
#include "projetil.h"
#include "bomba.h"
#include <sstream>
#include <string>

GLfloat vermelho[] = {1.0, 0.0, 0.0};
GLfloat verde[] = {0.0, 0.8, 0.3};
GLfloat azul[] = {0.0, 0.0, 1.0};
GLfloat laranja[] = {1.0, 0.7, 0.0};
GLfloat amarelo[] = {1.0, 1.0, 0.0};
GLfloat preto[] = {0.0, 0.0, 0.0};

// Variáveis globais de configuração
Jogador* jogador;
float larguraDimensao, alturaDimensao;
float fundoR, fundoG, fundoB;
int teclasTeclado[256];
GLfloat tempoAntigo, tempoNovo, tempoAntigoDecolagem, tempoDecolagem;

// Objetos auxiliares
Arena* arena = NULL;

// ---- Métodos ---- //
GLfloat* retornaCor(std::string fill){
    if(fill.compare("red") == 0){
        return vermelho;
    }
    if(fill.compare("green") == 0){
        return verde;
    }
    if(fill.compare("blue") == 0){
        return azul;
    }
    if(fill.compare("orange") == 0){
        return laranja;
    }
    return NULL;
}

void keyPress(unsigned char key, int x, int y){
  teclasTeclado[key] = 1;
}

void keyup(unsigned char key, int x, int y){
  teclasTeclado[key] = 0;
}

void limpaTeclas(){
    for(int i = 0; i < 256; i++){
        teclasTeclado[i] = 0;
    }
}

void mouseAction(int button, int state, int x, int y){
    if(jogador->isVoando() && jogador->isVivo()){
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
            Projetil *p = new Projetil(
                jogador->getX() + ((jogador->getRaio()) * cos(jogador->getAnguloJogador() * M_PI / 180)) + (jogador->getRaio()/2) * cos(jogador->getAnguloJogador() * M_PI / 180 + jogador->getAnguloCanhao() * M_PI / 180),
                jogador->getY() + ((jogador->getRaio()) * sin(jogador->getAnguloJogador() * M_PI / 180)) + (jogador->getRaio()/2) * sin(jogador->getAnguloJogador() * M_PI / 180 + jogador->getAnguloCanhao() * M_PI / 180),
                (GLfloat) (jogador->getRaio()/8), 
                (GLfloat) 0.0, 
                (GLfloat) 0.0, 
                (GLfloat) 0.0, 
                jogador->getVelocidade() * jogador->getVelocidadeMultiplicadora() * jogador->getVelocidadeTiro(), 
                jogador->getAnguloCanhao(),
                jogador->getAnguloJogador(),
                (GLfloat) (jogador->getRaio() / 2),
                (GLfloat) (jogador->getRaio() - 1)
            );
            jogador->addProjetil(p);
        }
        if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
            Bomba *b = new Bomba(
                jogador->getX(), 
                jogador->getY(), 
                (GLfloat) (jogador->getRaio()/3),  
                (GLfloat) 0.0, 
                (GLfloat) 0.0, 
                (GLfloat) 0.0, 
                jogador->getVelocidade() * jogador->getVelocidadeMultiplicadora(),
                jogador->getAnguloJogador()
            );
            jogador->addBomba(b);
        }
    }

}

void mouseMove(int x, int y){
    if(jogador->isVoando() && jogador->isVivo()){
        if(jogador->getMouseX() == 0.0){
            jogador->setMouseX(x);
        }else{
            GLfloat angulo = (jogador->getMouseX() - x) / 2.0;
            if(jogador->getAnguloCanhao() + angulo >= -45 && jogador->getAnguloCanhao() + angulo <= 45){
                jogador->setAnguloCanhao(angulo);
            }
        }
        jogador->setMouseX(x);
    }
}

void display(void){
    // Inicialização das variáveis
    float theta, px, py;
    jogador = arena->getJogador();
    Linha* linha = arena->getLinha();
    list<Circulo*> inimigosAereos = arena->getInimigosAereos();
    list<Circulo*> inimigosTerrestres = arena->getInimigosTerrestres();

    // Limpar todos os pixels
    glClear(GL_COLOR_BUFFER_BIT);

    if(arena != NULL){
        arena->desenhaArena();
    }
    if(linha != NULL){
        linha->desenhaLinha();
    }
    for(list<Circulo*>::iterator c = inimigosTerrestres.begin(); c != inimigosTerrestres.end(); ++c){
        (*c)->desenha();
    }
    if(jogador != NULL){
        jogador->desenhaBombas();
    	// jogador->desenhaProjeteis();
    }
    for(list<Circulo*>::iterator c = inimigosAereos.begin(); c != inimigosAereos.end(); ++c){
        (*c)->desenha();
    }
    if(jogador != NULL){
        jogador->desenhaJogador();
        if(!jogador->isLigado() && !jogador->isVoando()){
            jogador->exibeDecolagem(arena->getX() - arena->getRaio(), arena->getY() - arena->getRaio()); 
        }
        if(!jogador->isVivo()){ 
            jogador->exibeGameOver(arena->getX() - arena->getRaio(), arena->getY() - arena->getRaio()); 
        }
    }
    
    glutSwapBuffers();
}

void idle(void){
    Jogador* jogador = arena->getJogador();
    // Cálculo do tempo de sincronização.
    tempoNovo = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    GLfloat t = tempoNovo - tempoAntigo;
    tempoAntigo = tempoNovo;
    jogador->setTempoAjustador(t);
    if(jogador->isVivo()){
        if((teclasTeclado['u'] || teclasTeclado['U']) && !jogador->isLigado()){
            jogador->setLigado(true);
            tempoAntigoDecolagem = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        }
        if(jogador->isLigado() && !jogador->isVoando()){
            tempoDecolagem = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) - tempoAntigoDecolagem;
            jogador->decola(arena->getLinha(), tempoAntigoDecolagem, tempoDecolagem);
        }
        if(jogador->isLigado() && jogador->isVoando()){
            GLfloat vel = jogador->getVelocidade();
            if(teclasTeclado['a'] || teclasTeclado['A']){
                jogador->moveX(100.0);
            }else if(teclasTeclado['d'] || teclasTeclado['D']){
                jogador->moveX(-100.0);
            }
            if(teclasTeclado['='] || teclasTeclado['+']){
                jogador->incrementaVelocidade(3.0);
            }
            if(teclasTeclado['-']){
                jogador->decrementaVelocidade(3.0);
            }
            jogador->voa(vel);
            jogador->voaProjeteis(t);
            jogador->voaBombas(t);
        }
    }
    if(teclasTeclado['r']){
        jogador->reseta();
        limpaTeclas();
    }
    
    glutPostRedisplay();
}

void init(float fundoR, float fundoG, float fundoB){
    glClearColor(fundoR, fundoG, fundoB, 0.0);

    // Iniciar sistema de viz
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        arena->getX() - arena->getRaio(), 
        arena->getX() + arena->getRaio(), 
        arena->getY() - arena->getRaio(), 
        arena->getY() + arena->getRaio(), 
        -1.0, 1.0);
}

bool lerXML(char* caminhoArquivo){
    TiXmlDocument doc(strcat(caminhoArquivo, "/config.xml"));
    doc.LoadFile();
    TiXmlElement *aplicacao = doc.RootElement();
    if(aplicacao !=  NULL){
        // Declaração de variáveis
        TiXmlElement *arquivoDaArena = NULL;
        TiXmlElement *nome = NULL;
        TiXmlElement *tipo = NULL;
        TiXmlElement *caminho = NULL;
        TiXmlElement *jogadorElemento = NULL;

        // Atribui valores
        arquivoDaArena = aplicacao->FirstChildElement("arquivoDaArena");
        nome = arquivoDaArena->FirstChildElement("nome");
        tipo = arquivoDaArena->FirstChildElement("tipo");
        caminho = arquivoDaArena->FirstChildElement("caminho");
        jogadorElemento = aplicacao->FirstChildElement("jogador");

        // Abre arquivo SVG
        std::string arquivoSVG = (std::string)caminho->GetText() + '/' + (std::string)nome->GetText() + '.' + (std::string)tipo->GetText();
        const char *abrirArquivo = arquivoSVG.c_str();
        TiXmlDocument svgFile(abrirArquivo);
        svgFile.LoadFile();
        
        // Leitura da arena
        TiXmlElement *arenaElement = svgFile.RootElement();
        if(arenaElement != NULL) {
            TiXmlElement *circuloElemento = NULL;
            circuloElemento = arenaElement->FirstChildElement("circle");

            GLint id;
            GLfloat arenaX, arenaY, arenaRaio;
            GLfloat cx, r, cy, *cores;

            // Leitura de círculos
            while(circuloElemento){
                // Leitura da arena
                if(((std::string)circuloElemento->Attribute("fill")).compare("blue") == 0){
                    arenaRaio = atof(circuloElemento->Attribute("r"));
                    larguraDimensao = arenaRaio * 2;
                    alturaDimensao = arenaRaio * 2;
                    id = atoi(circuloElemento->Attribute("id"));
                    cores = retornaCor(circuloElemento->Attribute("fill"));
                    arenaX = arenaY = 0.0;
                    arena = new Arena(id, arenaRaio, arenaX, arenaY, cores[0], cores[1], cores[2]);
                    arenaX = atof(circuloElemento->Attribute("cx"));
                    arenaY = alturaDimensao - atof(circuloElemento->Attribute("cy"));
                    break;
                }
                circuloElemento = circuloElemento->NextSiblingElement("circle");
            }
            circuloElemento = arenaElement->FirstChildElement("circle");
            while(circuloElemento){
                // Leitura dos demais círculos
                id = atoi(circuloElemento->Attribute("id"));
                r = atof(circuloElemento->Attribute("r"));
                cx = atof(circuloElemento->Attribute("cx")) - arenaX;
                cy = alturaDimensao - atof(circuloElemento->Attribute("cy")) - arenaY;
                cores = retornaCor(circuloElemento->Attribute("fill"));

                // Leitura do jogador
                if(((std::string)circuloElemento->Attribute("fill")).compare("green") == 0){
                    Jogador* jogador = new Jogador(id, r, cx, cy, cores[0], cores[1], cores[2], arena);
                    GLfloat vel = atof(jogadorElemento->Attribute("vel"));
                    GLfloat velTiro = atof(jogadorElemento->Attribute("velTiro"));

                    // Velocidade no final da decolagem
                    jogador->setVelocidadeMultiplicadora(vel);
                    jogador->setVelocidadeTiro(velTiro);
                    arena->setJogador(jogador);

                    // Leitura dos inimigos aéreos
                }else if(((std::string)circuloElemento->Attribute("fill")).compare("red") == 0){
                    arena->criaInimigosAereos(id, r, cx, cy, cores[0], cores[1], cores[2]);

                    // Leitura dos inimigos terrestres
                }else if(((std::string)circuloElemento->Attribute("fill")).compare("orange") == 0){
                    arena->criaInimigosTerrestres(id, r, cx, cy, cores[0], cores[1], cores[2]);
                }
                
                // circulo++
                circuloElemento = circuloElemento->NextSiblingElement("circle");
            }

            TiXmlElement *linhaElemento = NULL;
            linhaElemento = arenaElement->FirstChildElement("line");

            // Leitura da pista de lançamento (linha)
            if(linhaElemento != NULL){
                
                // Parse das cores
                std::string estiloLinha = linhaElemento->Attribute("style");
                int inicio, fim;
                inicio = estiloLinha.find("(") + 1;
                fim = estiloLinha.find(")") - inicio;
                std::string coresLinha = estiloLinha.substr(inicio, fim);

                // Atribuindo valores
                GLint id = atoi(linhaElemento->Attribute("id"));
                GLfloat x1 = atof(linhaElemento->Attribute("x1")) - arenaX;
                GLfloat y1 = alturaDimensao - atof(linhaElemento->Attribute("y1")) - arenaY;
                GLfloat x2 = atof(linhaElemento->Attribute("x2")) - arenaX;
                GLfloat y2 = alturaDimensao - atof(linhaElemento->Attribute("y2")) - arenaY;
                std::stringstream coresStream(coresLinha);
                GLfloat cor[3];
                int i = 0;
                while(std::getline(coresStream,coresLinha,',')){
                    cor[i++] = stof(coresLinha);
                }
                Linha* linha = new Linha(id, x1, y1, x2, y2, cor[0], cor[1], cor[2]);
                arena->setLinha(linha);
                arena->getJogador()->calculaPontoCrescimento(linha);
                arena->getJogador()->calculaAngulo(linha);
            }
            return true;
        }else{
            std::cout << "Arquivo arena.svg não encontrado! Certifique-se de colocar o caminho do arquivo arena.svg no config.xml." << std::endl;
            return false;
        }
    }else{
        std::cout << "Arquivo config.xml não encontrado!" << std::endl;
        return false;
    }
}

int main(int argc, char** argv){
    if(argc > 1){
        if(lerXML(argv[1])){
            fundoR = 1.0;
            fundoG = 1.0;
            fundoB = 1.0;
            tempoAntigo = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
            // Inicializa
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
            glutInitWindowSize(larguraDimensao, alturaDimensao);
            glutInitWindowPosition(50, 50);
            glutCreateWindow("Airplane Combat");
            init(fundoR, fundoG, fundoB);
            glutDisplayFunc(display);
            glutKeyboardUpFunc(keyup);
            glutKeyboardFunc(keyPress);
            glutMouseFunc(mouseAction);
            glutPassiveMotionFunc(mouseMove);
            glutIdleFunc(idle);
            glutMainLoop();
        }
    }else{
        std::cout << "Localização do arquivo config.xml não foi informada." << std::endl;
    }

    return 0;
}
