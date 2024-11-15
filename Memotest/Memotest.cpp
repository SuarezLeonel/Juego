#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

const int TAMANIO_CELDA = 100;
const int ESPACIO_ENTRE_CELDAS = 10;
const int FILAS = 3;
const int COLUMNAS = 4;
const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA = 600;

struct Jugador {
    string nombre;
    int pares = 0;
};

vector<vector<sf::Color>> generarTablero(int filas, int columnas) {
    vector<sf::Color> colores = { sf::Color::Blue, sf::Color::Blue, sf::Color::Yellow, sf::Color::Yellow,
                                  sf::Color::Green, sf::Color::Green, sf::Color::Red, sf::Color::Red,
                                  sf::Color::Cyan, sf::Color::Cyan, sf::Color::Magenta, sf::Color::Magenta };
    random_shuffle(colores.begin(), colores.end());

    vector<vector<sf::Color>> tablero(filas, vector<sf::Color>(columnas));
    int indice = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            tablero[i][j] = colores[indice++];
        }
    }
    return tablero;
}

bool coordenadasValidas(int x, int y, int filas, int columnas) {
    return x >= 0 && x < filas && y >= 0 && y < columnas;
}

pair<int, int> obtenerCoordenadasDeClick(int mouseX, int mouseY, int offsetX, int offsetY) {
    int fila = (mouseY - offsetY) / (TAMANIO_CELDA + ESPACIO_ENTRE_CELDAS);
    int columna = (mouseX - offsetX) / (TAMANIO_CELDA + ESPACIO_ENTRE_CELDAS);
    return make_pair(fila, columna);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "Juego de Memoria");
    window.setFramerateLimit(60);

    // Variables de juego
    vector<vector<sf::Color>> tablero = generarTablero(FILAS, COLUMNAS);
    vector<vector<bool>> descubiertas(FILAS, vector<bool>(COLUMNAS, false));
    int turnoActual = 0;
    bool esperandoPareja = false;
    bool mostrandoError = false;
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    sf::Clock relojError;

    // Posiciones para el tablero
    int anchoTablero = (COLUMNAS * TAMANIO_CELDA) + (COLUMNAS - 1) * ESPACIO_ENTRE_CELDAS;
    int altoTablero = (FILAS * TAMANIO_CELDA) + (FILAS - 1) * ESPACIO_ENTRE_CELDAS;
    int offsetX = (ANCHO_VENTANA - anchoTablero) / 2;
    int offsetY = (ALTO_VENTANA - altoTablero) / 2;

    // Ciclo principal del juego
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (!mostrandoError && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                pair<int, int> coord = obtenerCoordenadasDeClick(mouseX, mouseY, offsetX, offsetY);
                int fila = coord.first;
                int columna = coord.second;

                if (!coordenadasValidas(fila, columna, FILAS, COLUMNAS) || descubiertas[fila][columna]) {
                    continue;
                }

                if (!esperandoPareja) {
                    x1 = fila;
                    y1 = columna;
                    esperandoPareja = true;

                    // Dibujar la primera carta al instante
                    window.clear(sf::Color::White);
                    descubiertas[x1][y1] = true;
                    for (int i = 0; i < FILAS; i++) {
                        for (int j = 0; j < COLUMNAS; j++) {
                            sf::RectangleShape celda(sf::Vector2f(TAMANIO_CELDA, TAMANIO_CELDA));
                            celda.setPosition(j * (TAMANIO_CELDA + ESPACIO_ENTRE_CELDAS) + offsetX, i * (TAMANIO_CELDA + ESPACIO_ENTRE_CELDAS) + offsetY);

                            if (descubiertas[i][j]) {
                                celda.setFillColor(tablero[i][j]);
                            }
                            else {
                                celda.setFillColor(sf::Color::White);
                            }
                            celda.setOutlineColor(sf::Color::Black);
                            celda.setOutlineThickness(2);
                            window.draw(celda);
                        }
                    }
                    window.display();
                }
                else {
                    x2 = fila;
                    y2 = columna;
                    esperandoPareja = false;

                    if (coordenadasValidas(x1, y1, FILAS, COLUMNAS) && tablero[x1][y1] == tablero[x2][y2]) {
                        descubiertas[x1][y1] = true;
                        descubiertas[x2][y2] = true;
                    }
                    else {
                        mostrandoError = true;
                        relojError.restart();
                    }
                    turnoActual = 1 - turnoActual;
                }
            }
        }

        if (mostrandoError && relojError.getElapsedTime().asSeconds() > 1) {
            mostrandoError = false;
            descubiertas[x1][y1] = descubiertas[x2][y2] = false;
            x1 = y1 = x2 = y2 = -1;
        }

        window.clear(sf::Color::White);

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                sf::RectangleShape celda(sf::Vector2f(TAMANIO_CELDA, TAMANIO_CELDA));
                celda.setPosition(j * (TAMANIO_CELDA + ESPACIO_ENTRE_CELDAS) + offsetX, i * (TAMANIO_CELDA + ESPACIO_ENTRE_CELDAS) + offsetY);

                if (descubiertas[i][j] || (mostrandoError && ((i == x1 && j == y1) || (i == x2 && j == y2)))) {
                    celda.setFillColor(tablero[i][j]);
                }
                else {
                    celda.setFillColor(sf::Color::White);
                }
                celda.setOutlineColor(sf::Color::Black);
                celda.setOutlineThickness(2);
                window.draw(celda);
            }
        }

        window.display();
    }
    return 0;
}
