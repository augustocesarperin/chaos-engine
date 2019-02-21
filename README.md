# Simulador de Partículas

Uma engine com algumas migalhas de física e muitas otimizações. Feito para fins didáticos e procrastinatórios em C/C++ & SFML.


<br>

<img src="gifs/v09.gif" alt="Demonstração da Versão Final" width="650"/>

<br>


## Como funciona a física? (Integração Verlet)

O programa usa integração de Verlet para os cálculos de movimento. Diferente do método Euler (usado nas versões iniciais), o método Verlet evita que as partículas atravessem umas às outras quando estão em alta velocidade, ou quando o FPS cai muito. O sistema também simula perda de energia nas colisões, e evita que elas fiquem ricocheteando ad infinitum.


## Controles

**Mouse:**
- Botão esquerdo: cria partícula normal
- Botão direito: cria partícula grande

**Teclado:**
- `G`: liga/desliga gravidade
- `R`: liga/desliga repulsão
- `M`: liga/desliga força do mouse
- `N`: alterna entre atrair e repelir
- `F`: Troca o estilo de força do mouse
- `+/-`: ajusta intensidade da força
- `C`: limpa todas as partículas
- `Espaço`: cria partículas aleatórias
- `ESC`: bye


### Windows
```
compile.bat
```
> **Nota:** É necessário ter o [MSYS2](https://www.msys2.org/) com as ferramentas MinGW (g++, make, cmake) e SFML instalados e no PATH do sistema.

### Linux
```sh
sudo apt update && sudo apt install build-essential git cmake libsfml-dev

git clone https://github.com/augustocesarperin/chaos-engine.git
cd chaos-engine
mkdir build && cd build
cmake ..
make
./Chaos
```

Apesasar de otimizado, o desempenho pode variar com muitas partículas/interações. Use por sua conta e risco.

(~Na minha máquina funciona~)

<details>
<summary>Versões Anteriores</summary>

#### v0.5
<img src="gifs/gifrec.gif" alt="Demonstração da Versão 0.5" width="650"/>

#### v0.1
<img src="gifs/v01.gif" alt="Demonstração da Versão 0.1" width="650"/>

</details>

<div align="right">
  <sub><a href="https://github.com/augustoperin">Augusto Cesar Perin</a> | 2018-2019</sub>
</div>

