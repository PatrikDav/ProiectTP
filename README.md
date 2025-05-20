# Jocul 2048 în C

Acest proiect este o implementare a popularului joc 2048 în limbajul de programare C folosind o bibliotecă de terminal. Jocul implică combinarea plăcilor cu același număr pentru a crea o placă cu suma celor două. Obiectivul este să ajungi la placa 2048.

## Caracteristici
- Gameplay clasic 2048
- Controale simple și intuitive
- Actualizări în timp real în terminal
- Listă cu ultimele 10 scoruri maxime (timpul în care ai ajuns la 2048)
- Posibilitatea de a seta dimensiunea boardului (4x4, 5x5, 6x6, etc.)

## Cerințe
- Compilator C (de exemplu, GCC)
- Bibliotecă de terminal (de exemplu, ncurses)

## Instalare
1. Clonează depozitul:
    ```sh
    git clone <repository_url>
    ```

2. Compilează proiectul:
    ```sh
    gcc -Wall -o 2048 2048.c -lSDL2 -lSDL2_ttf -lm
    ```

## Utilizare
Rulează jocul compilat:
```sh
./2048_game
```

## Controale
- Folosește tastele WASD pentru a muta plăcile.
- Combină plăcile cu același număr pentru a crea o nouă placă cu suma lor.
- Încearcă să ajungi la placa 2048 pentru a câștiga jocul.
- Setează dimensiunea boardului la începutul jocului.