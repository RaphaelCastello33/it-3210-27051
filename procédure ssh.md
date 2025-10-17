# Ajouter une clé ssh à son compte GitHub (pour pouvoir bosser proprement)


1. créer la clé SSH
```bash 
ssh-keygen -t ed25519 -C "votre_email@enseirb-matmeca.com" # vous pouvez utiliser votre email perso
```
appuyez sur entrée jusqu'a que la clé soit crée

2. Ajouter la clé SSH à l'agent SSH
```bash 
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
```

3. Afficher la clé publique (celle que vous mettrez sur votre github)
```bash
cat ~/.ssh/id_ed25519.pub
```
copiez la ligne que ca print (toute la ligne) <br>
pour info la mienne ressemble à ca : `ssh-ed25519 truc_trop_long raphaelloureiro00@gmail.com`

4. Ajouter la clé sur github
4.1 aller sur votre compte GitHub <br>
4.2 cliquer sur son profil (en haut à droite) et aller dans paramètre (ou Settings si vous êtes bilingue) <br>
4.3 A gauche allez dans l'onglet "SSH and GPG keys" <br>
4.4 cliquez sur "new SSH key" <br>
4.5 collez la clé que vous aviez copié à l'étape 3 dans le champ "Key" <br>
4.6 donner le nom que vous voulez <br>
4.7 cliquez sur "Add SSH key" <br>

5. Vérifier que tout soit bon
sur votre terminal écrivez `ssh -T git@github.com` <br>
ca doit print un truc du genre : `Hi username! You've successfully authenticated, but GitHub does not provide shell access.`<br>
