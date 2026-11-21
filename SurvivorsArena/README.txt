1. DESCRIZIONE DEL PROGETTO
---------------------------
Il giocatore controlla un personaggio al centro di un'arena armato di balestra.
Nell'arena compaiono due tipi di nemici modellati con curve cubiche di Hermite:
  - Pipistrelli che inseguono il player per colpirlo a contatto.
  - Fantasmi che mantengono la distanza e sparano pallini bianchi.

Ogni nemico ha 2 punti vita (al primo colpo rimpicciolisce leggermente, al secondo
muore) e dopo 3 secondi dalla morte rinasce in un punto casuale lontano dal player.
Alla morte i nemici rilasciano a terra una gemma a forma di rombo che pulsa nel tempo:
il giocatore deve raccoglierla prima che sparisca per aumentare lo SCORE.
Il giocatore dispone di 5 cuori di vita mostrati in alto a sinistra
Ogni contatto con un pipistrello o con un pallino bianco dei fantasmi toglie 1 cuore.

2. ISTRUZIONI PER COMPILAZIONE E AVVIO
--------------------------------------
1. Aprire il file di soluzione "SurvivorsArena.sln" con Visual Studio 2022.
2. Se la cartella "packages" non e' presente, ripristinare il pacchetto NuGet di GLM
3. Impostare in alto la configurazione su "Debug" (o "Release") e la piattaforma su "x64".
4. Compilare la soluzione e avviare il programma con F5


3. CONTROLLI E INTERAZIONE
--------------------------
- Schermata Iniziale:
    Click su "Inizia gioco" per avviare la partita, oppure "Esci dal gioco" per chiudere.

- Durante la partita:
    W, A, S, D  (o Frecce direzionali) : Movimento del personaggio nell'arena
    Mouse                              : Mira e rotazione a 360 gradi della balestra
    Click sinistro  (o Barra Spazio)   : Sparo dei dardi della balestra
    F1                                 : Apre / chiude il menu "Debug e Statistiche"
    ESC                                : Chiude l'applicazione

- Menu Debug e Statistiche (tasto F1):
    Permette di modificare in tempo reale tramite slider:
      * Velocita' del player
      * Velocita' dei proiettili, tempo di ricarica e numero massimo di dardi
      * Numero e velocita' dei pipistrelli
      * Numero e velocita' dei fantasmi
    Consente inoltre di attivare/disattivare la modalita' Wireframe e di resettare la scena.

- Schermata di Game Over:
    Mostra il punteggio raggiunto e permette di scegliere tra "Ricomincia" ed "Esci dal gioco".


4. STRUTTURA DEI FILE PRINCIPALI
--------------------------------
- main.cpp               : Game loop, gestione tempo (delta time), logica nemici, sparo, gemme e collisioni.
- Player.h / Player.cpp  : Costruzione del player tramite assemblaggio di primitive, animazione camminata e balestra.
- ShapeGenerator.h/.cpp  : Generazione procedurale delle primitive 2D (triangoli, rettangoli, cerchi, cuori, gemme).
- PlayerShapeLoader.h    : Loader dei modelli OBJ/MTL esportati dal modellatore di curve di Hermite.
- Scene.h / Scene.cpp    : Gestione separata delle geometrie GPU condivise (Geometry) e delle istanze (GeometryInstance).
- Renderer.h/.cpp        : Rendering delle istanze 2D della scena con matrice di proiezione ortografica e model matrix.
- BackgroundRenderer.*   : Rendering del quad fullscreen per lo sfondo con shader program dedicato.
- Gui.h / Gui.cpp        : Schermate di Start / Game Over, HUD dello Score e menu di controllo F1 in Dear ImGui.
- Input.h / Input.cpp    : Callback GLFW per tastiera, posizione del mouse e ridimensionamento finestra.
- shader/background.*    : Vertex e Fragment Shader dedicati allo sfondo procedurale animato a spirale (uTime).
- shader/vertexshaderC.glsl, fragmentshaderC.glsl : Shader degli oggetti 2D della scena.
- assets/pipistrello.obj/.mtl, fantasma.obj/.mtl  : Modelli 2D realizzati con curve cubiche di Hermite.
