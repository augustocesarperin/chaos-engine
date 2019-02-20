# Simulador de Partículas

Um simulador com algumas migalhas de física. Feito para fins didáticos e procrastinatórios em C/C++ & SFML.


<br>

<img src="gifs/v09.gif" alt="Demonstração da Versão Final" width="650"/>

<br>

## Verlet

O simulador está utilizando integração de Verlet para os cálculos de movimento. Diferente do método Euler (usado nas versões iniciais), o método Verlet evita que as partículas atravessem umas às outras quando estão em alta velocidade, ou quando o FPS cai muito. O sistema também simula perda de energia nas colisões, impedindo que as partículas fiquem quicando como se não houvesse amanhã.
Em resumo: O método Verlet não se preocupa só com a posição atual e velocidade, mas também lembra da posição anterior e aceleração pra prever onde a partícula deveria estar, como se cada ppartícula tivesse uma "memória". Isso torna os movimentos mais suaves/realistas.

## Controles

**Mouse:**
- Botão esquerdo: cria partícula normal
- Botão direito: cria partícula maior

**Teclado:**
- `G`: liga/desliga gravidade
- `R`: liga/desliga repulsão
- `M`: liga/desliga força do mouse
- `N`: alterna entre atrair e repelir
- `F`: Troca o estilo de força do mouse
- `+/-`: ajusta intensidade da força
- `C`: limpa todas as partículas
- `Espaço`: cria partículas aleatórias
- `ESC`: sai


### Compilar:
```
compile.bat
```

### Executar:
```
Chaos.exe
```


**Requisitos:** C++ 11+ e SFML 2.5+

O desempenho pode variar com muitas partículas/interações - Use por sua conta e risco.

~Na minha máquina funciona~

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

