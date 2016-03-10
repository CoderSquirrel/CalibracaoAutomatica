#ifndef CONTORNOESCOLHA_H
#define CONTORNOESCOLHA_H

#include <QMainWindow>
#include "string.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
namespace Ui {
class ContornoEscolha;
}

class ContornoEscolha : public QMainWindow
{
    Q_OBJECT

public:
    explicit ContornoEscolha(QWidget *parent = 0);
    ~ContornoEscolha();
    void SetText(int qtd);
    void SetStatus(int porcento, std::string mensagem);
    int INDICE_COR, INDICE_OBJETO;
    bool CALIBRADOS[7], CALIBRADO, INICIAR, FINALIZADO;


public slots:
    void ComboChanged(int index);
    void BotaoIniciar();
    void BotaoSalvar();
    void BotaoFinalizar();
    void ComboCorChanged(int index);
private:
    Ui::ContornoEscolha *ui;
};

#endif // CONTORNOESCOLHA_H
