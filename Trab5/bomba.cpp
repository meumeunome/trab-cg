#include "bomba.h"

Bomba::Bomba(GLfloat x, GLfloat y, GLfloat z, GLfloat raio, GLfloat corR, GLfloat corG, GLfloat corB, GLfloat velocidade, GLfloat anguloJogadorBase, Arena *arena){
    this->x = x;
    this->y = y;
    this->z = z;
    this->raio = raio;
	this->raioInicial = raio;
    this->corR = corR;
    this->corG = corG;
    this->corB = corB;
	this->velocidade = velocidade;
  this->velocidadeVertical = 10;
    this->anguloJogadorBase = anguloJogadorBase;
	this->tempoInicial = (glutGet(GLUT_ELAPSED_TIME) / 1000.0);
	this->explode = false;
  this->camera = false;
  this->arena = arena;
}

GLfloat Bomba::getX(){
	return this->x;
}

void Bomba::setX(GLfloat x){
	this->x = x;
}

GLfloat Bomba::getY(){
	return this->y;
}

void Bomba::setY(GLfloat y){
	this->y = y;
}

GLfloat Bomba::getZ(){
	return this->z;
}

void Bomba::setZ(GLfloat z){
	this->z = z;
}

GLfloat Bomba::getRaio(){
	return this->raio;
}

void Bomba::setRaio(GLfloat raio){
	this->raio = raio;
}

GLfloat Bomba::getCorR(){
	return this->corR;
}

void Bomba::setCorR(GLfloat corR){
	this->corR = corR;
}

GLfloat Bomba::getCorG(){
	return this->corG;
}

void Bomba::setCorG(GLfloat corG){
	this->corG = corG;
}

GLfloat Bomba::getCorB(){
	return this->corB;
}

void Bomba::setCorB(GLfloat corB){
	this->corB = corB;
}

GLfloat Bomba::getVelocidade(){
	return this->velocidade;
}

void Bomba::setVelocidade(GLfloat velocidade){
	this->velocidade = velocidade;
}

GLfloat Bomba::getAnguloJogadorBase(){
    return this->anguloJogadorBase - 90;
}

void Bomba::desenhaCirculo(GLfloat raio, GLfloat corR, GLfloat corG, GLfloat corB){
    float theta, px, py;
    defineCor(corR, corG, corB);
	glutSolidSphere(raio, 30, 30);
}

void Bomba::desenhaBomba(){
	glPushMatrix();
		glTranslatef(this->x, this->y, this->z);
        glRotatef(this->getAnguloJogadorBase(), 0.0, 0.0, 1.0);
		desenhaCirculo(this->raio, this->corR, this->corG, this->corB);
	glPopMatrix();
}

void Bomba::voa(GLfloat tempoAjustador){
	GLfloat cx = this->getX() + (cos(((this->getAnguloJogadorBase() + 90) * (M_PI / 180))) * this->velocidade * tempoAjustador);
  GLfloat cy = this->getY() + (sin(((this->getAnguloJogadorBase() + 90) * (M_PI / 180))) * this->velocidade * tempoAjustador);
	this->setX(cx);
	this->setY(cy);
	this->cai(tempoAjustador);
}

void Bomba::cai(GLfloat tempoAjustador){
	// GLfloat tempoFinal = 2.0;
	// GLfloat tempoAtual = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	// GLfloat tempoQueda = tempoAtual - this->tempoInicial;
	// if(this->getZ() > 0){
    GLfloat cz = this->getZ() - this->velocidadeVertical*tempoAjustador;
    this->setZ(cz);

    this->velocidadeVertical += 2;
		// // V = del(S) / del(t)
		// GLfloat	velocidadeRaio = ((this->raioInicial / 2) - this->raioInicial) / tempoFinal;
		// // S = So + Vt
		// GLfloat passoRaio = velocidadeRaio * tempoQueda;
		// GLfloat raioDecrescente = this->raioInicial + passoRaio;
		// this->setRaio(raioDecrescente);
	// }else{
		// this->setRaio(this->raioInicial / 2);
	// 	this->explodeBomba();
	// }
}

void Bomba::explodeBomba(){
	this->explode = true;
  if(this->camera)
    this->arena->setCamera(false);
}

bool Bomba::explodiu(){
	return this->explode;
}

void Bomba::setCamera(bool status){
	this->camera = status;
}
bool Bomba::hasCamera(){
	return this->camera;
}
