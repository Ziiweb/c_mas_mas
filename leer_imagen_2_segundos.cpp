#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

cv::Mat crearMascara(cv::Mat &imagen_hsv, cv::Scalar lower1, cv::Scalar upper1, 
                     cv::Scalar lower2 = cv::Scalar(0,0,0), cv::Scalar upper2 = cv::Scalar(0,0,0),
                     bool usarSegundoRango = false) {
    cv::Mat mascara1, mascara2, mascara_total;
    cv::inRange(imagen_hsv, lower1, upper1, mascara1);
    if (usarSegundoRango) {
        cv::inRange(imagen_hsv, lower2, upper2, mascara2);
        cv::bitwise_or(mascara1, mascara2, mascara_total);
    } else {
        mascara_total = mascara1;
    }
    return mascara_total;
}

int main() {
    cv::Mat imagen = cv::imread("foto.jpg");
    if (imagen.empty()) {
        std::cerr << "No se pudo abrir 'foto.jpg'" << std::endl;
        return -1;
    }

    cv::Mat imagen_hsv;
    cv::cvtColor(imagen, imagen_hsv, cv::COLOR_BGR2HSV);

    // Máscaras
    cv::Mat mascara_rojo = crearMascara(imagen_hsv,
                                        cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255),
                                        cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255),
                                        true);
    cv::Mat mascara_amarillo = crearMascara(imagen_hsv,
                                            cv::Scalar(20, 100, 100), cv::Scalar(30, 255, 255));

    cv::Mat mascara_amarillo = crearMascara(imagen_hsv,
                                            cv::Scalar(20, 100, 100), cv::Scalar(30, 255, 255));



                                            
    // Resultados
    cv::Mat resultado_rojo = imagen.clone();
    resultado_rojo.setTo(cv::Scalar(0,0,0), mascara_rojo);

    cv::Mat resultado_amarillo = imagen.clone();
    resultado_amarillo.setTo(cv::Scalar(0,0,0), mascara_amarillo);

    // Crear ventana única
    cv::namedWindow("Croma", cv::WINDOW_AUTOSIZE);

    // Bucle infinito alternando rojo y amarillo
    while (true) {
        cv::imshow("Croma", resultado_rojo);
        if (cv::waitKey(2000) == 27) break; // Espera 2 segundos, ESC para salir

        cv::imshow("Croma", resultado_amarillo);
        if (cv::waitKey(2000) == 27) break; // Espera 2 segundos, ESC para salir
    }

    cv::destroyAllWindows();
    return 0;
}
