Le but du projet est de récuperer les valeurs de concentration de particules d'une taille de 2.5 um en fonction de cette valeur on connait la qualité de l'air,  tout est présent dans le code arduino.
Ensuite une fois qu'on a les valuers on fait une requête pour récuperer les valeurs et les stockés.
j'ai ensuite fait de page html une qui est un formuolaire qui permet d'ajouter modifier supprimer un device chaque device a un Id une localisation et un nom et l'autre page sert a afficher les valeurs de Pm 2.5 stockes on peut choisir d'afficher les valeurs d'une date et d'une plage hotraire bien précise j'affiche les valeurs dans un graphique à l'aide de charteJS
dans le fichier index.mjs il y'a toute mes routes qui servent à faire les requêtes.

Mise en place du projet 

Tout d'abort mettre en marche le microcontrolleur pour recuperer les valeurs et les envoyées ensuite se positionner dans une ligne de commande dans le dossier siteBDD et taper node index.mjs

ensuite sur une page internet taper : http://localhost:8080/birdie.html pour le formulaire des valeurs pm2.5.
                                      http://localhost:8080/index.html  pour le formulaire des devices.

                                      
