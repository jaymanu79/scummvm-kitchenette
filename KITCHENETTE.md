# ScummVM — fork Kitchenette

Fork perso de ScummVM pour faire tourner **Kitchenette** (quiz télé, Macromedia Director 6.0, PC Windows).
Branche de travail : `kitchenette`. Ces changements ne sont pas destinés à être proposés tels quels au
projet officiel (voir `AI-GUIDELINES.md`).

## Ce que la branche ajoute

| Fichier | Quoi |
|---|---|
| `engines/director/lingo/xtras/m/miles.*` | Xtra **Miles Sound System** (`mssx32.x32`) : ouverture de WAV sur disque ou de sons de cast, lecture via le mixeur ScummVM, volume, boucles, pause. `MilesGetStatus` renvoie 4 pendant la lecture, 2 sinon. |
| `engines/director/lingo/xtras/c/cpuxtra.*` | Xtra **Cpuxtra** (Le Lab) : `CpuId` renvoie `[#CPU:5, #FPU:1, #SPEED:233, #MMX:1]`. |
| `graphics/macgui/macfontmanager.cpp` | Table Mac Roman → Unicode pour les polices TrueType de remplacement (accents). |

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
./scummvm director-win-fallback
```

Il faut les fichiers de données de ScummVM (`classicmacfonts.dat`, `fonts.dat`…) : ceux d'une ScummVM
installée suffisent, sinon `--extrapath=/usr/share/scummvm`.

## Commandes

- **1 / 2 / 3** : nombre de joueurs, puis saisie des noms au clavier + Entrée.
- **A / N / P** : buzzers des joueurs 1, 2, 3 (arrêter la mosaïque, répondre).
- **Échap** : pause.

## Ce qui marche

Intro, choix des joueurs, mosaïque des thèmes, animations de thème, « QUESTION n », questions avec accents,
score, enchères. Jamais joué une partie complète.

## Reste à faire

1. **Pavés sur les lettres des réponses** : sprites qui gardent l'encre « Copy » au lieu de
   « Background Transparent ». Piste : le marqueur `_autoPuppet` des sprites (Director 6) n'est jamais remis à
   zéro à la fin de la plage du sprite dans le score (`engines/director/channel.cpp`, `Channel::setClean`).
   Deux essais naïfs (remise à zéro au changement de plage / quand la piste devient vide) cassent d'autres écrans.
2. **Palette du lave-linge** (écran du nombre de joueurs) : couleurs fausses ; l'image d'origine utilise
   la palette `ACCES00000.PICT Palette` (cast `log`).
3. **Détection** : ajouter l'empreinte de `Kitchenette.exe` à `engines/director/detection_tables.h` pour que le
   jeu soit reconnu sous son nom.
4. Vérifier à l'oreille le son Miles (testé seulement par le timing des animations).
