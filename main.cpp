#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "corcalibrada.h"
#include <QApplication>
#include "contornoescolha.h"
#include "math.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fstream> // incluir
using namespace cv;
#define SIZE 8

CorCalibrada coresCalibradas[SIZE];

Mat src_gray, HSV, frame;
int thresh = 100;
RNG rng(12345);
vector<Rect> boundRect;
vector<Rect> insideRect;
vector<CorCalibrada> cores;
int GL[] =  {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,60,90,120,150,180,210,240,270,300,400,500,800,1000};
double PROBABILIDADE[] = {12.7062, 4.3027, 3.1824, 2.7764,
                          2.2281,2.2010,2.1788,2.1604,2.1448,2.1314,
                          2.1199, 2.1098,2.1009,2.0930,2.0860,2.0796,2.0739,
                          2.0687,2.0637,2.0595,2.0555,2.0518,2.0484,2.0452,2.0423,
                          2.0003,1.9867,1.9799,1.9759,1.9732,1.9713,1.9699,
                          1.9688,1.9679,1.9659,1.9647,1.9629,1.9623}   ;
double ObterPorcentagem(int valor, int porcentagem){
    //    return (porcentagem * valor);
    double p = (porcentagem*valor)/100;
    return p;

}
/** @function thresh_callback */
void thresh_callback(int, void *) {

    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;


    Canny(src_gray, canny_output, thresh, thresh * 2, 3);
    findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    //  vector<Moments> mu(contours.size());
    vector<Point2f> center(contours.size());

    vector<float> radius(contours.size());
    vector<vector<Point> > contours_poly(contours.size());
    //for (int i = 0; i < contours.size(); i++) { mu[i] = moments(contours[i], false); }


    boundRect.clear();
    insideRect.clear();
    for (unsigned int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect.push_back(boundingRect(Mat(contours_poly[i])));
        minEnclosingCircle((Mat) contours_poly[i], center[i], radius[i]);

    }

    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);


    //    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    for (unsigned int i = 0; i < contours.size(); i++) {
        //   Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        //               drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
        Rect n = boundRect.at(i);
        Point pTopRight = n.tl();
        Point pBottomLeft = n.br();
        pTopRight.x += 10;
        pTopRight.y += 10;
        pBottomLeft.x -= 10;
        pBottomLeft.y -= 10;
        //         ObterPorcentagem(pTopRight.x, 10);
        //        std::cout <<"Valor x À direita " << pTopRight.x <<", 10% = " << ObterPorcentagem(pTopRight.x, 10)<< std::endl;
        //                pTopRight.x += ObterPorcentagem(pTopRight.x, 10);
        //                pTopRight.y += ObterPorcentagem(pTopRight.y, 10);
        //                pBottomLeft.x -= ObterPorcentagem(pBottomLeft.x, 10);
        //                pBottomLeft.y -= ObterPorcentagem(pBottomLeft.y, 10);
        Rect rRect(pTopRight, pBottomLeft);
        insideRect.push_back(rRect);
        rectangle(drawing, rRect, Scalar(0, 0, 255), 1, 8, 0);

    }

    //   imshow("Contours", drawing);

}
bool CompararRectPorArea(Rect a, Rect b){
    return a.area() < b.area();

}
double Media(){
    double soma=0;
    for(unsigned int i=0;i<insideRect.size(); i++){
        soma+=insideRect.at(i).area();
    }
    soma = soma/insideRect.size();
    return soma;
}

double DesvioPadrao(){
    int media = Media();
    int dif;
    for(unsigned int i=0;i<insideRect.size(); i++){
        dif+=pow((insideRect.at(i).area() - media),2);
    }

    return sqrt(dif/insideRect.size());
}

double EncontrarZ(int gl){
    for(unsigned int i = 0; i < ((sizeof(GL)/sizeof(*GL))); i++){
        if(GL[i]==gl){
            return PROBABILIDADE[i];
            break;
        }
    }
    return 0.0;

}

double* DistribuicaoT(double media, double desvio, int numDados, int gl){
    double Limites[2];

    double z = EncontrarZ(gl);
    if(z==0.0){
        Limites[0]=0;
        Limites[1]=0;
    }
    else {
        Limites[0]=media+(z*(desvio/sqrt(numDados)));
        Limites[1]=media-(z*(desvio/sqrt(numDados)));

    }
    return Limites;
}

/*Levando em Consideracao 95% de i.c e 5%(0,05) de nivel de confiança
 * Tabela de Referencia http://www.epi.uff.br/wp-content/uploads/2015/05/Tabela-T.pdf
*/
double* TdeStudent(){
    double L[2];
    double media = Media();
    double desvioPadrao = DesvioPadrao();
    int NumDados = insideRect.size();
    int GrausDeLiberdade = NumDados-1;
    double* distribuicaoT = DistribuicaoT(media,desvioPadrao,NumDados,GrausDeLiberdade);

    L[0]=distribuicaoT[0];
    L[1]=distribuicaoT[1];
//    std::cout << "Limite Inferior "<< L[1] << "\tLimite Superior "<< L[0] << std::endl;
    return L;
}

vector<Rect> EliminarExcessos(){
    double* Limites =TdeStudent();


    double L1=Limites[1], L2=Limites[0];
    vector<Rect> tamanhoEsperado;
    sort(insideRect.begin(), insideRect.end(),CompararRectPorArea);
    for(unsigned int i=0; i< insideRect.size(); i++){
//        std::cout << insideRect.at(i).area() << std::endl;
        if(insideRect.at(i).area() >  L1 && insideRect.at(i).area() <  L2){
            tamanhoEsperado.push_back(insideRect.at(i));
        }
    }
    return tamanhoEsperado;
}

void MetodoCalcular() {
    cv::cvtColor(frame, HSV, CV_RGB2HSV);
    cv::Vec3b pixel;
    int H[257], S[257], V[257];
    int MIN[3], MAX[0];
    int k;
    for (unsigned int i = 0; i < insideRect.size(); i++) {
        memset(H, 0, sizeof(H));
        memset(S, 0, sizeof(S));
        memset(V, 0, sizeof(V));
        memset(MIN, 0, sizeof(MIN));
        memset(MAX, 0, sizeof(MAX));

        for (int y = insideRect.at(i).tl().y; y < insideRect.at(i).br().y; ++y) {
            for (int x = insideRect.at(i).tl().x; x < insideRect.at(i).br().x; x++) {
                pixel = HSV.at<cv::Vec3b>(y, x); // read current pixel
                H[pixel.val[0]]++;
                S[pixel.val[1]]++;
                V[pixel.val[2]]++;
            }
        }

        for (k = 0; k < 256; k++) {
            if (H[k] != 0) {
                break;
            }
        }
        MIN[0] = k;

        for (k = 255; k > MIN[0]; k--) {
            if (H[k] != 0) {
                break;
            }
        }
        MAX[0] = k;


        //Encontrar min e max S

        for (k = 0; k < 256; k++) {
            if (S[k] != 0) {
                break;
            }
        }
        MIN[1] = k;

        for (k = 255; k > MIN[1]; k--) {
            if (S[k] != 0) {
                break;
            }
        }
        MAX[1] = k;

        //Encontrar min e max V

        for (k = 0; k < 256; k++) {
            if (V[k] != 0) {
                break;
            }
        }
        MIN[2] = k;

        for (k = 255; k >MIN[2]; k--) {
            if (V[k] != 0) {
                break;
            }
        }
        MAX[2] = k;

        //Elimina tons de preto
        if(MIN[2] > 20 ){
            CorCalibrada c;
            c.SetMax(MAX);
            c.SetMin(MIN);

            cores.push_back(c);
        }
    }

}
void SalvarArquivo(){

//std::cout << "Salvando"<<std::endl;
    std::ofstream out;
    out.open("cores.arff");
    for(int i = 0; i < SIZE; i++){
        out << i << " : " << coresCalibradas[i].MIN[0] << "." << coresCalibradas[i].MIN[1]<< "."<< coresCalibradas[i].MIN[2] << std::endl;
        out << i << " : " << coresCalibradas[i].MAX[0] << "." << coresCalibradas[i].MAX[1]<< "."<< coresCalibradas[i].MAX[2] << std::endl;

    }
    out.close();
}

int main(int argc, char **argv) {
    cv::VideoCapture camera(1);
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }
    QApplication a(argc, argv);
    ContornoEscolha c;
    c.show();
    int cont=0;
    c.SetStatus(10, "Detectando Objetos");
    while (!c.INICIAR) {
        camera >> frame;
        imshow("Imagem da Camera", frame);
        if (cv::waitKey(30) >= 0) break;


    }
    cvDestroyAllWindows();

       cvtColor(frame, src_gray, CV_BGR2GRAY);
        blur(src_gray, src_gray, Size(3, 3));
        //createTrackbar(" Canny thresh:", "Janela", &thresh, 255, thresh_callback);
        thresh_callback(0, 0);

    insideRect = EliminarExcessos();
    c.SetStatus(40, "Eliminando Objetos Indesejados");
    sleep(1);
    MetodoCalcular();
    c.SetStatus(80, "Calculando Valores HSV");
    sleep(1);
    c.SetText(cores.size() );
    c.SetStatus(100, "Exibindo Objetos com Threshold");
    sleep(1);
    Mat Threshold;
    Mat res;
    while(1){
        camera >> frame;
        cv::cvtColor(frame,HSV,cv::COLOR_RGB2HSV);
        cv::inRange(HSV,cv::Scalar(cores[c.INDICE_OBJETO].MIN[0],cores[c.INDICE_OBJETO].MIN[1],cores[c.INDICE_OBJETO].MIN[2]),cv::Scalar( cores[c.INDICE_OBJETO].MAX[0] , cores[c.INDICE_OBJETO].MAX[1] , cores[c.INDICE_OBJETO].MAX[2] ),Threshold);
        Mat res;
        if(c.CALIBRADO){
            CorCalibrada cor;
            cor.SetMax(cores[c.INDICE_OBJETO].MAX);
            cor.SetMin(cores[c.INDICE_OBJETO].MIN);
            coresCalibradas[c.INDICE_COR] = cor;
            c.CALIBRADO = false;
        }
        bitwise_and(frame, frame, res, Threshold );
        cv::imshow("Limiar por Objeto", res);
        waitKey(30);
        if(c.FINALIZADO){
            break;
        }
    }
    cvDestroyAllWindows();
    cvDestroyAllWindows();
    SalvarArquivo();
    c.close();
    return a.exec();
}
