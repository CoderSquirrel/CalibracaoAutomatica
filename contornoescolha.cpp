#include "contornoescolha.h"
#include "ui_contornoescolha.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

ContornoEscolha::ContornoEscolha(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ContornoEscolha)
{
    INDICE_COR = INDICE_OBJETO = 0;
    CALIBRADO = false;
    FINALIZADO = false;
    for(int i =0;i< 7; i++)
        CALIBRADOS[i] = false;

    ui->setupUi(this);
    connect(ui->COMBO, SIGNAL(currentIndexChanged(int)), SLOT(ComboChanged(int)));
    connect(ui->BOTAO, SIGNAL(clicked(bool)), SLOT(BotaoIniciar()));
    connect(ui->SALVAR, SIGNAL(clicked(bool)), SLOT(BotaoSalvar()));
    connect(ui->BOTAO_2, SIGNAL(clicked(bool)), SLOT(BotaoFinalizar()));
    connect(ui->COMBO_COR, SIGNAL(currentIndexChanged(int)), SLOT(ComboCorChanged(int)));
    INICIAR = false;
}
void ContornoEscolha::SetText(int qtd){

    for(int i=0;i<qtd;i++){
        ui->COMBO->addItem(QString::number(i));

    }

}
void ContornoEscolha::BotaoSalvar(){
    CALIBRADO=true;
    INDICE_COR = ui->COMBO_COR->currentIndex();
    INDICE_OBJETO = ui->COMBO->currentIndex();
    CALIBRADOS[INDICE_COR] = true;
    ui->checkBox->setChecked( CALIBRADOS[INDICE_COR]);
}
void ContornoEscolha::BotaoIniciar(){
    INICIAR = true;
    ui->BOTAO->setEnabled(false);
}

void ContornoEscolha::SetStatus(int porcento, std::string mensagem){

    ui->LABEL->setText(QString::fromStdString(mensagem));
    ui->LABEL->repaint();
    ui->PROGRESS->setValue(porcento);
    ui->PROGRESS->repaint();
    if(porcento>90){
        ui->COMBO->setEnabled(true);
    }

}
void ContornoEscolha::BotaoFinalizar(){
    FINALIZADO = true;
}
void ContornoEscolha::ComboChanged(int index){
    INDICE_OBJETO = index;
    ui->checkBox->setChecked(CALIBRADOS[INDICE_COR]);
}
void::ContornoEscolha::ComboCorChanged(int index){
    INDICE_COR = index;
    ui->checkBox->setChecked(CALIBRADOS[INDICE_COR]);
}
ContornoEscolha::~ContornoEscolha()
{
    delete ui;
}
