# Sonomètre
Ce projet est un sonomètre connecté utilisé dans la salle des machine d'Eirlab à l'ENSEIRB-Matmeca (33). Il permet d'avertir les utilisateurs en cas d'intensité sonore trop importante, via une LED RGB et un afficheur LCD.

# Matériel

- Un ESP32 ([ref](https://www.gotronic.fr/art-module-nodemcu-esp32-28407.htm))
- Un afficheur LCD 1602
- Un microphone amplifieur MAX4466 de la marque Adafruit ([ref](https://www.adafruit.com/product/1063))

# Interface

L'interface utilisateur affiche la valeur en décibels actuelle, ainsi qu'un historique des valeurs moyennes sur 5 secondes sur la minute passée.

# Installation

Le fichier à envoyer à l'ESP 32 est situé dans le dossier code. Remplissez le ssid et le mot de passe de votre réseau et envoyez le via l'IDE Arduino de votre choix. À l'initialisation, l'ESP 32 envoie sur le port Serial son adresse IP locale. Celle-ci est à reporter dans la variable `ESP32url` du fichier [`app.py`](server/app.py).
L'installation du serveur se fait via uWSGI (plus d'infos [ici](https://flask.palletsprojects.com/en/2.2.x/deploying/uwsgi/)).

# Calibrage du micro

Cette fonction permet de faire le lien entre l'amplitude mesurée (ampl). Pour plus de précisions, modifiez-la en testant avec vôtre micro. Ici j'ai opté pour une approche linéaire par morceaux, mais la vraie manière de calculer le volume en décibels est d'utiliser sa [définition logarithmique](https://fr.wikipedia.org/wiki/Niveau_(audio)), si vôtre micro est assez précis.
```C
int matchDB(double vol)
{
	if (ampl < 150)
	{
		return 38;
	}
	if (ampl < 500)
	{
		return 0.062 * ampl + 39;
	}
	double temp = 0.02 * ampl + 60;
	if(temp > 100) temp = 100;
	return temp;
}
```
# Remerciements

- L'interface web utilise le thème bootstrap [SB Admin 2](https://startbootstrap.com/theme/sb-admin-2). Merci à [Start Bootstrap](https://startbootstrap.com) pour leur travail.
- Merci à [Eirlab](https://www.eirlab.net) pour l'accueil et la mise à disposition du matériel.
