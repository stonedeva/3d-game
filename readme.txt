The Maze
===========

Das Spiel beginnt in einem Laybrinth. Das Ziel ist es dieses zuverlassen bevor ein Timer
von 15min abläuft.
Es gibt verschiedene Welten mit unterschiedlichen Stillen und Blöcken/Items.


1. Struktur
===========

src/ - Quellcode Datein
include/ - Definitionen für Quellcode
res/ - Resourcen (Texturen, Maps, ...)
utils/ - Tools für die Entwicklung


Das ausführbare Spiel game.exe MUSS im gleichen Ordner wie res/ liegen,
damit die Resourcen vom Spiel gefunden werden könnnen.


2. Technik
==========

Die Technik des Spieles basiert auf der Raycasting-Render Technik des Spiel Wolfenstein 3D.
Ein Integer-Array "int pixels[WIDTH * HEIGHT];" wird verwendet um die Pixel des Fensters
einzeln anzusprechen und ihre Farbwerte zuändern.

Die Map ist ein N x N großes Kastenfeld mit den Werten 0 (Leer) und 1 (für eine Wand).
Der Spieler hat eine Position (x,y) in diesem Feld und eine Blickrichtung.
Der Spieler schießt nun ein Gerade in Blickrichtung, bis die Gerade auf eine Wand trifft.
Das Ziel des Programs ist es nun die Länge der Gerade (den Abstand zwischen dem Spieler und
einer Wand zufinden) um die getroffene Wand größer oder kleiner erscheinen zulassen.


3. Code
=======

Der Code des Spieles startet in src/game.c in der Funktion main().
Hierauf wird das Spiel durch init() initaliziert und eine unendliche Schleife
gestartet, welche jedes Frame das Spiel rendert und updatet.

- https://lodev.org/cgtutor/raycasting.html
