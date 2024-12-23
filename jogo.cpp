#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

const vector<string> OBJ_CORRETOS = {"Estojo", "Notebook", "Mochila", "Garrafa", "Lapis", "Cafe"};
const vector<string> OBJ_ERRADOS = {"Televisao", "Travesseiro", "Chuteira", "Casca de Banana", "Baralho", "Bola de Futebol"};

// Implementação da Fila
template<typename T>
class Fila {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node* frontNode;
    Node* backNode;
    size_t count;

public:
    Fila() : frontNode(nullptr), backNode(nullptr), count(0) {}

    ~Fila() {
        while (!empty()) {
            pop();
        }
    }

    void push(const T& elemento) {
        Node* newNode = new Node(elemento);
        if (backNode) {
            backNode->next = newNode;
        } else {
            frontNode = newNode;
        }
        backNode = newNode;
        count++;
    }

    void pop() {
        if (!empty()) {
            Node* temp = frontNode;
            frontNode = frontNode->next;
            if (!frontNode) {
                backNode = nullptr;
            }
            delete temp;
            count--;
        }
    }

    T& front() {
        return frontNode->data;
    }

    bool empty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }

    vector<T> getElementos() const {
        vector<T> elementos;
        Node* current = frontNode;
        while (current) {
            elementos.push_back(current->data);
            current = current->next;
        }
        return elementos;
    }
};

struct Objeto {
    string nome;
    bool correto;
    sf::Text texto;
    sf::Vector2f posicao;
};

Objeto gerarObjeto(sf::Font &fonte, float larguraJanela) {
    Objeto obj;
    if (rand() % 2 == 0) {
        obj.nome = OBJ_CORRETOS[rand() % OBJ_CORRETOS.size()];
        obj.correto = true;
    } else {
        obj.nome = OBJ_ERRADOS[rand() % OBJ_ERRADOS.size()];
        obj.correto = false;
    }

    obj.texto.setFont(fonte);
    obj.texto.setString(obj.nome);
    obj.texto.setCharacterSize(24);
    obj.texto.setFillColor(sf::Color::White);
    obj.posicao = sf::Vector2f(rand() % static_cast<int>(larguraJanela - 100), 0);
    obj.texto.setPosition(obj.posicao);

    return obj;
}

bool mostrarTelaInicial(sf::RenderWindow &janela, sf::Font &fonte) {
    sf::Text textoTitulo;
    textoTitulo.setFont(fonte);
    textoTitulo.setString("Mariazinha acordou atrasada para a aula,\najude-a pegando os objetos corretos!");
    textoTitulo.setCharacterSize(24);
    textoTitulo.setFillColor(sf::Color::White);
    textoTitulo.setPosition(100, 150);

    sf::RectangleShape botaoComecar(sf::Vector2f(200, 50));
    botaoComecar.setFillColor(sf::Color::Green);
    botaoComecar.setPosition(300, 300);

    sf::Text textoComecar;
    textoComecar.setFont(fonte);
    textoComecar.setString("Jogar");
    textoComecar.setCharacterSize(20);
    textoComecar.setFillColor(sf::Color::White);
    textoComecar.setPosition(365, 310);

    sf::RectangleShape botaoSair(sf::Vector2f(200, 50));
    botaoSair.setFillColor(sf::Color::Red);
    botaoSair.setPosition(300, 400);

    sf::Text textoSair;
    textoSair.setFont(fonte);
    textoSair.setString("Sair");
    textoSair.setCharacterSize(20);
    textoSair.setFillColor(sf::Color::White);
    textoSair.setPosition(370, 410);

    while (janela.isOpen()) {
        sf::Event evento;
        while (janela.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                janela.close();
                return false;
            }

            if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(janela);

                if (botaoComecar.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return true;
                }

                if (botaoSair.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    janela.close();
                    return false;
                }
            }
        }

        janela.clear();
        janela.draw(textoTitulo);
        janela.draw(botaoComecar);
        janela.draw(textoComecar);
        janela.draw(botaoSair);
        janela.draw(textoSair);
        janela.display();
    }
    return false;
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    sf::RenderWindow janela(sf::VideoMode(800, 600), "Ajude Mariazinha a ir a escola!", sf::Style::Close);
    janela.setFramerateLimit(60);

    sf::Font fonte;
    if (!fonte.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        cerr << "Erro ao carregar fonte!\n";
        return -1;
    }

    if (!mostrarTelaInicial(janela, fonte)) {
        return 0;
    }

    Fila<Objeto> filaObjetos;
    const vector<string> sequenciaCorreta = {"Notebook", "Cafe", "Mochila", "Lapis"};
    vector<string> objetosPegos;

    size_t sequenciaIndex = 0;
    int pontos = 0;
    bool jogando = true;

    sf::Clock relogio;
    const int duracao = 60;

    sf::Text textoTempo;
    textoTempo.setFont(fonte);
    textoTempo.setCharacterSize(24);
    textoTempo.setFillColor(sf::Color::Red);
    textoTempo.setPosition(650, 50);

    sf::Text textoSequencia;
    textoSequencia.setFont(fonte);
    textoSequencia.setCharacterSize(18);
    textoSequencia.setFillColor(sf::Color::Yellow);
    textoSequencia.setPosition(10, 50);

    sf::Text textoObjetosPegos;
    textoObjetosPegos.setFont(fonte);
    textoObjetosPegos.setCharacterSize(18);
    textoObjetosPegos.setFillColor(sf::Color::Cyan);
    textoObjetosPegos.setPosition(10, 300);

    sf::RectangleShape player(sf::Vector2f(50, 50));
    player.setFillColor(sf::Color::Green);
    player.setPosition(375, 550);

    float velocidadePlayer = 5.0f;

    while (janela.isOpen()) {
        sf::Event evento;
        while (janela.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                janela.close();
            }
        }

        if (!jogando) {
            continue;
        }

        float tempoDecorrido = relogio.getElapsedTime().asSeconds();
        if (tempoDecorrido >= duracao) {
            jogando = false;
            break;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.move(-velocidadePlayer, 0);
            if (player.getPosition().x < 0) player.setPosition(0, player.getPosition().y);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.move(velocidadePlayer, 0);
            if (player.getPosition().x > 750) player.setPosition(750, player.getPosition().y);
        }

        if (filaObjetos.size() < 5 && rand() % 100 < 2) {
            filaObjetos.push(gerarObjeto(fonte, janela.getSize().x));
        }

        if (!filaObjetos.empty()) {
            Objeto &obj = filaObjetos.front();
            obj.posicao.y += 3;
            obj.texto.setPosition(obj.posicao);

            if (player.getGlobalBounds().intersects(obj.texto.getGlobalBounds())) {
                objetosPegos.push_back(obj.nome);
                if (obj.correto) {
                    pontos += 5;
                } else {
                    pontos -= 2;
                }
                filaObjetos.pop();
            } else if (obj.posicao.y > janela.getSize().y) {
                filaObjetos.pop();
            }
        }

        string textoSeq = "Pegue:\n";
        for (const auto &obj : sequenciaCorreta) {
            textoSeq += "- " + obj + "\n";
        }
        textoSequencia.setString(textoSeq);

        string textoPegos = "Pegos:\n";
        for (const auto &obj : objetosPegos) {
            textoPegos += "- " + obj + "\n";
        }
        textoObjetosPegos.setString(textoPegos);

        textoTempo.setString(to_string(duracao - static_cast<int>(tempoDecorrido)));

        janela.clear();
        janela.draw(player);
        janela.draw(textoTempo);
        janela.draw(textoSequencia);
        janela.draw(textoObjetosPegos);

        for (auto &obj : filaObjetos.getElementos()) {
            janela.draw(obj.texto);
        }

        janela.display();
    }
// Calcular porcentagem de acertos
    int acertos = 0;
    for (size_t i = 0; i < min(sequenciaCorreta.size(), objetosPegos.size()); ++i) {
        if (sequenciaCorreta[i] == objetosPegos[i]) {
            acertos++;
        }
    }
    float porcentagemAcerto = (static_cast<float>(acertos) / sequenciaCorreta.size()) * 100;

    sf::Text textoFim;
    textoFim.setFont(fonte);
    textoFim.setCharacterSize(24);
    textoFim.setFillColor(sf::Color::White);
    textoFim.setString("Fim de jogo!\nPontuacao final: " + to_string(pontos) +
                       "\nPorcentagem de acerto: " + to_string(static_cast<int>(porcentagemAcerto)) + "%");
    textoFim.setPosition(200, 300);

    janela.clear();
    janela.draw(textoFim);
    janela.display();

    sf::sleep(sf::seconds(5));
    return 0;
}
