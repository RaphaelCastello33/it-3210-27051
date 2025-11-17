# it-3210-27051

# Hygiène de vie GitHub 

## Habitudes à prendre 
> ne **JAMAIS** travailler sur les memes fichiers que votre duo <br>
> **TOUJOURS** `git status` et `git pull` à chaque début de scéance<br>
> **COMMUNIQUEZ** entre vous, vous n'êtes que 2 par groupes<br>
> faites des messages de commit qui **EXPLIQUE** votre commit <br>

## Arborressence du git
> il y a une branche principale. **Personne ne push dedans** <br>
> chaque groupe aura 2 branches (vous pourrez en créer d'autres si besoin) <br>
> vous pushez des modifs minime (nouvelle fonction codée par exemple) sur la branche "devel"<br>
> vous pushez les grosses modifs (validation d'une fonctionnalité) sur la branche main_[votre groupe]<br>


## Commandes pour push une modif

```bash
git branch                                  # Vérifie sur quelle branche tu es
git checkout nom_de_ta_branche              # (Optionnel) Si tu n’es pas sur la bonne branche
git fetch                                   # Reload le git
git status                                  # Vérifie que t'as pas des commit de retard (git pull avant sinon)
git add .                                   # Ajoute les fichiers modifiés
git add chemin/vers/fichier                 # (ou pour ajouter un seul fichier)
git commit -m "Ton message de commit ici"   # Fais ton commit avec un message
git push origin nom_de_ta_branche           # Pousse les changements sur la bonne branche distante
```

## Commandes pour pull des modifs de master depuis vos branches
```bash
git branch                          # Vérifie sur quelle branche tu es
git checkout nom_de_ta_branche      # (Optionnel) Change si besoin
git fetch origin                    # Récupère les dernières modifs du dépôt distant
git merge origin/master             # Intègre les modifs de master dans ta branche
git push origin nom_de_ta_branche   # Pousse les changements vers ta branche distante
```
# Commandes ROS

## Se connecter au robot

## Setup le robot apres connexion

```bash
# Activer les servomoteurs
rosservice call /xarm/motion_ctrl 8 1

# Mettre le robot dans un mode prêt à l'emploi
rosservice call /xarm/set_mode 0
rosservice call /xarm/set_state 0

# déplacer le robot en position initiale
rosservice call /xarm/go_home [] 0.35 7 0 0

# déplacer le robot
rosservice call /xarm/move_line_tool [50,100,100,0,0,0] 200 2000 0 0 # delta_x = 50; delta_y = 100; delta_z = 100; vitesse = 200; accélération = 2000
```