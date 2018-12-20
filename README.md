# Simulador de Partículas

Um simulador de partículas 2D. Move círculos numa janela com física básica. Feito para explorar C++ e SFML, não para ciência de ponta.

![Demonstração do Simulador de Partículas](gravacao.gif)

## Funcionalidades Principais

*   **Partículas (`Particle`):**
    Círculos com `massa`, `raio`, `posição`, `velocidade` e `cor`. Massa e raio afetam colisões e gravidade.

*   **Sistema de Partículas (`ParticleSystem`):**
    Gerencia as partículas: cria, remove, aplica forças (gravidade), trata colisões e atualiza os estados (`deltaTime`).

*   **Interações Físicas:**
    *   **Gravidade:** Aceleração vertical constante (ajustável, liga/desliga).
    *   **Colisões:** Detecção por sobreposição de círculos; resolução elástica básica com correção de posição.
    *   **Repulsão (Experimental):** Força entre pares, baseada no inverso do quadrado da distância (ajustável, liga/desliga).

## Interação

*   **Mouse:**
    *   `Clique Esquerdo`: Partícula padrão.
    *   `Clique Direito`: Partícula maior/pesada.
*   **Teclado:**
    *   `G`: Liga/desliga gravidade.
    *   `R`: Liga/desliga repulsão.
    *   `C`: Limpa partículas.
    *   `Setas Cima/Baixo`: Ajusta gravidade.
    *   `Setas Esquerda/Direita`: Ajusta repulsão.
    *   `ESC`: Sai.

## Compilação

*   **Requisitos:** C++ (11+), SFML (>=2.5).
*   **Build:** Testado com g++/MinGW-w64 (Windows). Use `Makefile` ou `compile_run.bat`. SFML deve estar acessível.

## Considerações

Performance pode variar com muitas partículas/interações - Use por sua conta e risco.

---
Augusto César Perin
