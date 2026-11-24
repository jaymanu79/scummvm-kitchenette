# ScummVM — fork Kitchenette

Fork perso de ScummVM pour faire tourner **Kitchenette** (quiz télé, Macromedia Director 6.0, PC Windows).
Branche de travail : `kitchenette`. Ces changements ne sont pas destinés à être proposés tels quels au
projet officiel (voir `AI-GUIDELINES.md`).

## Ce que la branche ajoute

| Fichier | Quoi |
|---|---|
| `engines/director/lingo/xtras/m/miles.*` | Xtra **Miles Sound System** (`mssx32.x32`) : ouverture de WAV sur disque ou de sons de cast, lecture via le mixeur ScummVM, volume, boucles, pause. `MilesGetStatus` renvoie 4 pendant la lecture, 2 sinon. |
| `engines/director/lingo/xtras/c/cpuxtra.*` | Xtra **Cpuxtra** (Le Lab) : `CpuId` renvoie `[#CPU:5, #FPU:1, #SPEED:233, #MMX:1]`. |
| `engines/director/detection_tables.h` | Kitchenette reconnu sous son nom (Windows, français), cible `kitchenette-win-fr`. |
| `engines/director/metaengine.cpp` | Commandes « Buzzer joueur 1/2/3 » réaffectables (touche, souris, manette). |
| `engines/director/score.cpp` | Director 6 : un sprite modifié par le Lingo sans `puppetSprite` (« auto-puppet ») rend la main au score à la fin de sa plage. Corrige les lettres en pavés. |

## Compiler (Ubuntu)

```sh
sudo apt install build-essential libsdl2-dev zlib1g-dev libpng-dev libfreetype-dev
./configure --disable-all-engines --enable-engine=director
make -j$(nproc)
```

## Préparer le jeu

Le lanceur et les Xtras sont dans l'installeur Wise du CD. Il faut un dossier qui contient à la fois :

- le contenu du CD (`DATA/`, `Installation.EXE`…) ;
- les fichiers installés : `Kitchenette.exe`, `Lingo.ini`, `jojo.cct`, `Themes/` (Intit.cst, listQ.cst, themes.cst),
  `Jojo/ph.cst`, `Xtras/` (Cpuxtra, Fileio, Mix32, mssx32, Sound Import Export, Swadcmpr).

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
2. **Palette du lave-linge** (écran du nombre de joueurs) : couleurs fausses ; l'image d'origine utilise
   la palette `ACCES00000.PICT Palette` (cast `log`).
3. Vérifier à l'oreille le son Miles (testé seulement par le timing des animations).
