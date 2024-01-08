# Projet Ecosystème - WANG Jinhua
## Description
Ce projet est un projet de simulation d'un écosystème. Il est réalisé en C++ avec la librairie OpenGL dans Qt.
## Installation
1. Si vous avez qtcreator avec qt6, vous pouvez ouvrir le fichier ecosystem.pro et lancer le projet.
2. Sinon vous pouvez ouvrir le fichier build -> release -> ecosystem.exe, c'est l'exécutable du projet compilé par windeployqt.
## Fonctionnalités
1. Bouton space pour lancer la simulation, chaque fois que vous appuyez sur space, la simulation avance d'un pas.
2. Bouton gauche de la souris pour la rotation de la caméra, bouton droit pour la translation de la caméra.
3. J'ai utilisé des graphiques simples pour représenter les différentes classes :
    - Le programme s'initialise avec 10 plantes et 4 animaux(une carnivore et une herbivore pour chaque sexe).
    - Les triangles représentent les plantes.
    - Les cercles représentent les animaux.
      - Les couleurs rouge et jaune représentent respectivement les hommes et les femmes.
      - Les cercles relativement grands représentent les carnivores, les petits cercles les herbivores.
## Conception
1. Les classes Animal et Plant stockent leurs propriétés.
2. La classe Grid gère chaque grille, dans laquelle sont stockées des informations sur les animaux et les plantes qui s'y trouvent.
3. La classe Ground est responsable de la gestion de toutes les grilles, y compris la gestion de tous les comportements dynamiques des plantes et des animaux.
## Conclusion
Globalement, il remplit la majeure partie des fonctions que vous avez demandées dans votre document, ainsi que les deux premiers de bonus.
   "# Ecosystem-Simulation" 
