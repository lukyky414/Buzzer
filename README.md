# Buzzer

Ces buzzers https://fr.wikipedia.org/wiki/Buzz! sont disponibles pour un jeu PS2.  
Il n'existe pas de driver pour utiliser ces buzzer sur ordinateur, il faut donc programmer tout le protocole pour récupérer les informations de ces manettes. Voici les seules informations disponibles en ligne: https://www.developerfusion.com/article/84338/making-usb-c-friendly/.  
Ces informations ne sont pas exactes, il y a quelques détails manquant. Les fichiers buzzer.cpp et buzzer.hpp peuvent être utilisés séparément du projet afin d'intégrer facilement les buzzers dans une application. Le main se contente de faire clignoter les manettes, et de s'arrêter lorsqu'un buzz est pressé. La touche 'k' du clavier sera automatiquement préssée, afin de faire 'pause' sur youtube. Cela fonctionne comme un simple blind test.
