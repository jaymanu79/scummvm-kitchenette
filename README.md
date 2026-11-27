# ScummVM — fork Kitchenette

Fork perso de ScummVM pour faire tourner **Kitchenette** (quiz télé, Macromedia Director 6.0, PC Windows).
Branche principale : `master`. Ces changements ne sont
pas destinés à être proposés tels quels au projet officiel (voir `AI-GUIDELINES.md`).

Pour la documentation générale de ScummVM, voir le [README officiel](https://github.com/scummvm/scummvm/blob/master/README.md).

## Ce que le fork ajoute

| Fichier | Quoi |
|---|---|
| `engines/director/lingo/xtras/m/miles.*` | Xtra **Miles Sound System** (`mssx32.x32`) : ouverture de WAV sur disque ou de sons de cast, lecture via le mixeur ScummVM, volume, boucles, pause. `MilesGetStatus` renvoie 4 pendant la lecture, 2 sinon. |
| `engines/director/lingo/xtras/c/cpuxtra.*` | Xtra **Cpuxtra** (Le Lab) : `CpuId` renvoie `[#CPU:5, #FPU:1, #SPEED:233, #MMX:1]`. |
| `engines/director/detection_tables.h` | Kitchenette reconnu sous son nom (Windows, français), cible `kitchenette-win-fr`. |
| `engines/director/metaengine.cpp` | Commandes « Buzzer joueur 1/2/3 » réaffectables (touche, souris, manette). |
| `engines/director/frame.cpp` | Score D6 : une piste palette vide (« membre -1 du castLib 0 ») n'est plus prise pour la palette système Mac. Corrige les couleurs (écran 1/2/3, fondu noir de 8 s avant le logo). |
| `engines/director/score.cpp` (tempo) | En D6, `puppetTempo(n)` est une cadence en images/s (le score code les cadences en 246 + valeur) : avant, ScummVM l'interprétait comme « attendre la vidéo du canal n » et les animations des thèmes tournaient sans limite. |
| `engines/director/frame.cpp` (palette, suite) | Le marqueur « canal palette vide » (castLib 0) est aussi vérifié quand castLib et member arrivent dans des deltas séparés : sinon un fondu de palette fantôme de 8 s bloquait le jeu avant chaque première question. |
| `engines/director/castmember/digitalvideo.cpp` | Vidéos synchronisées : saute les images en retard au lieu de les jouer en accéléré (logo Le Lab). En 256 couleurs, les images sont recalées sur la palette de la scène quand elle change au démarrage de la vidéo (logo Kitchenette inversé sinon). |
| `engines/director/lingo/lingo-builtins.cpp`, `director.h` | `updateStage` n'affiche l'écran qu'une fois par rafraîchissement (60 Hz) : le jeu l'appelle pour chaque lettre des titres « QUESTION N », ce qui attendait la synchro écran à chaque fois et rendait l'animation saccadée. |
| `engines/director/score.cpp` | Director 6 : un sprite modifié par le Lingo sans `puppetSprite` (« auto-puppet ») rend la main au score à la fin de sa plage. Corrige les lettres en pavés. |

## Paquets tout faits

GitHub Actions (`.github/workflows/kitchenette-builds.yml`) compile à chaque push sur `master` :

- **Ubuntu** : `scummvm-kitchenette_*.deb`, installé dans `/opt/scummvm-kitchenette`, lancé par `scummvm-kitchenette`.
  Ne remplace pas la ScummVM officielle.
- **Android** : `ScummVM-debug.apk` (arm64), application `org.scummvm.scummvm.debug`, à côté de l'officielle.

Les fichiers sont dans l'onglet Actions → l'exécution → Artifacts.

## Compiler (Ubuntu)

```sh
sudo apt install build-essential libsdl2-dev zlib1g-dev libpng-dev libfreetype-dev
./configure --disable-all-engines --enable-engine=director
make -j$(nproc)
```

## Préparer le jeu

Le lanceur (`Kitchenette.exe`), les Xtras, `Lingo.ini` et trois casts ne sont pas en clair sur le CD : ils sont
compressés dans l'installeur Wise `Installation.EXE`. Pas besoin de Windows pour les sortir :

```sh
7z x Kitchenette.iso -o/tmp/kitchenette-cd        # ou monter l'ISO
python3 dists/kitchenette/kitchenette-install.py /tmp/kitchenette-cd ~/Jeux/Kitchenette
```

Le script copie le CD dans le dossier de destination et y ajoute les fichiers extraits de l'installeur
(reconnus par leur MD5 : il ne marche qu'avec cette version du CD).

Puis :

```sh
./scummvm --add -p /chemin/vers/kitchenette
./scummvm kitchenette-win-fr
```

Au lancement, ScummVM prévient que le jeu n'est pas entièrement supporté : choisir « Démarrer quand même ».

Il faut les fichiers de données de ScummVM (`classicmacfonts.dat`, `fonts.dat`…) : ceux d'une ScummVM
installée suffisent, sinon `--extrapath=/usr/share/scummvm`.

## Commandes

- **1 / 2 / 3** : nombre de joueurs, puis saisie des noms au clavier + Entrée.
- **A / N / P** : buzzers des joueurs 1, 2, 3 (arrêter la mosaïque, répondre).
  Réaffectables dans Options du jeu → Raccourcis → « Kitchenette buzzers » : n'importe quelle touche, bouton de
  souris ou bouton de manette. ScummVM ne gère qu'une manette à la fois.
  Attention : la touche choisie tape la lettre du buzzer (a, n ou p) aussi pendant la saisie des noms.
- **Échap** : pause.

## Ce qui marche

Intro, choix des joueurs, mosaïque des thèmes, animations de thème, « QUESTION n », questions avec accents,
mini-jeux (grille-pain, lave-linge, tourne-disque) avec leurs lettres, score, enchères. Jamais joué une partie complète.

## Reste à faire

1. **Erreur Lingo dans le jeu du tourne-disque** : `getAt` sur `gMilesReps` qui vaut 0
   (`PlayRepSound` ← `ChangeDisqueRep` ← `DisqueAnim`). Les sons des réponses sont déchargés par
   `IncrSequence` alors que l'animation du disque tourne encore. Vu une fois, après des appuis au hasard.
2. Vérifier à l'oreille le son Miles (testé seulement par le timing des animations).
