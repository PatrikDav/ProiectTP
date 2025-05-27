# Jocul 2048 în C

Acest proiect este o implementare a popularului joc 2048 în limbajul de programare C. Jocul implică combinarea plăcilor cu același număr pentru a crea o placă cu suma celor două. Obiectivul este să ajungi la placa 2048.

## Caracteristici
- Gameplay clasic 2048
- Controale simple și intuitive
- Actualizări în timp real în terminal
- Posibilitatea de a seta dimensiunea boardului (4x4, 5x5, 6x6, etc.)

## Cerințe
- Compilator C (de exemplu, GCC)
- Biblioteca SDL2 pentru grafică
- Biblioteca SDL2_ttf pentru fonturi
- Biblioteca matematică standard (libm)
- Sistem de operare compatibil (Linux, macOS, Windows)

## Instalare Biblioteci
Pentru a instala bibliotecile necesare, poți folosi managerul de pachete specific sistemului tău de operare.

### Linux (Debian/Ubuntu)
```sh
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```
### macOS
```sh
brew install sdl2 sdl2_ttf
```
### Windows
Pentru Windows, poți descărca bibliotecile SDL2 și SDL2_ttf de pe site-ul oficial SDL și le poți include în proiectul tău. Asigură-te că adaugi căile către biblioteci în setările de compilare.
### Windows (MinGW)
```sh
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```
### Windows (vcpkg)
```sh
vcpkg install sdl2 sdl2-ttf
```

## Instalare
1. Clonează depozitul:
    ```sh
    git clone https://github.com/PatrikDav/ProiectTP.git
    ```

2. Compilează proiectul:
    ```sh
    gcc -Wall -o 2048 2048.c -lSDL2 -lSDL2_ttf -lm
    ```

## Utilizare
Rulează jocul compilat:
```sh
./2048
```

## Controale
- Folosește tastele SAGETI ( ^, <, >, ˅) pentru a muta plăcile.
- Combină plăcile cu același număr pentru a crea o nouă placă cu suma lor.
- Încearcă să ajungi la placa 2048 pentru a câștiga jocul.
- Setează dimensiunea boardului la începutul jocului.