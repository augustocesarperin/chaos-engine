# Simulador de Partículas

Um simulador com algumas migalhas de física. Feito para brincar com C++ & SFML, e, quem sabe, aproveitar em projetos futuros que provavelmente não acontecerão.

![Demonstração do Simulador de Partículas](gifrec.gif)

## Funcionalidades
* **Física** - Integração Verlet para simulação de movimento e colisões
* **Interações** - Gravitação, repulsão e campos de força configuráveis em tempo real

## Controles

**Mouse:**
- Botão esquerdo: cria partícula normal
- Botão direito: cria partícula pesada

**Teclado:**
- `G`: liga/desliga gravidade
- `R`: liga/desliga repulsão
- `M`: liga/desliga força do mouse
- `N`: alterna entre atrair e repelir
- `+/-`: ajusta intensidade da força
- `C`: limpa todas as partículas
- `Espaço`: cria partículas aleatórias
- `ESC`: sai

## Como Usar

**Executar direto:**
```
ParticleSimulator.exe
```

**Compilar e executar:**
```
compile.bat
```

**Requisitos:** C++ 11+ e SFML 2.5+

Performance pode variar com muitas partículas/interações - Use por sua conta e risco.

~Na minha máquina funciona~

