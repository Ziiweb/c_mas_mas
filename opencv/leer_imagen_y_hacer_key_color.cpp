#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Cargar la imagen
    cv::Mat imagen = cv::imread("foto.jpg");
    if (imagen.empty()) {
        std::cerr << "No se pudo abrir la imagen 'foto.jpg'" << std::endl;
        return -1;
    }

    // Convertir a espacio de color HSV
    cv::Mat imagen_hsv;
    cv::cvtColor(imagen, imagen_hsv, cv::COLOR_BGR2HSV);

    // Crear la máscara para tonos rojizos
    cv::Mat mascara1, mascara2, mascara_total;
    
    // Rango 1: rojo cercano a 0°
    cv::inRange(imagen_hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mascara1);
    
    // Rango 2: rojo cercano a 180°
    cv::inRange(imagen_hsv, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), mascara2);
    
    // Combinar las dos máscaras
    cv::bitwise_or(mascara1, mascara2, mascara_total);

    // Crear la imagen de salida
    cv::Mat resultado = imagen.clone();
    resultado.setTo(cv::Scalar(0, 0, 0), mascara_total); // reemplazar rojo por negro

    // Mostrar imágenes
    cv::imshow("Original", imagen);
    cv::imshow("Rojo Eliminado", resultado);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
