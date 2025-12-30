# 📘 AIDE-MÉMOIRE GIT - PROJET STRAT

## 🎯 WORKFLOW QUOTIDIEN

### Début de session
```bash
cd Ecran_Strategie_V4
git pull                    # Récupérer les dernières modifs
git st                      # Vérifier l'état
```

### Pendant le travail
```bash
# Toutes les 30 min ou après chaque feature
git add .
git cm "feat: description de ce que tu as fait"
git push

# OU en une seule commande pour un checkpoint rapide
git save
git push
```

### Fin de session
```bash
git push                    # Sauvegarder sur GitHub
```

---

## 🚨 COMMANDES D'URGENCE

### Tu as cassé quelque chose ?
```bash
# Annuler les modifs non commitées
git restore .               # ⚠️ Perte définitive !

# Revenir au dernier commit
git reset --hard HEAD

# Revenir à la version GitHub
git fetch origin
git reset --hard origin/main
```

### Voir ce qui a changé
```bash
git st                      # État actuel
git diff                    # Différences détaillées
git lg                      # Historique graphique
```

---

## 📦 SAUVEGARDES AVANT CLAUDE CODE

### Méthode 1 : Commit de sécurité
```bash
git add .
git cm "🔒 checkpoint stable avant Claude Code"
git push
```

### Méthode 2 : Branche de backup
```bash
git checkout -b backup-$(date +%Y%m%d-%H%M)
git push origin backup-$(date +%Y%m%d-%H%M)
git checkout main
```

---

## ⏮️ REVENIR EN ARRIÈRE

### Voir l'historique
```bash
git lg                      # Version courte
git log                     # Version détaillée
```

### Revenir temporairement (juste regarder)
```bash
git checkout abc123         # abc123 = hash du commit
git checkout main           # Revenir au présent
```

### Revenir définitivement
```bash
# Méthode douce (garde les modifs)
git reset --soft abc123

# Méthode dure (supprime tout)
git reset --hard abc123     # ⚠️ Perte définitive
```

### Annuler un commit proprement
```bash
git revert abc123           # Crée un nouveau commit d'annulation
```

---

## 🌿 BRANCHES (pour features complexes)

```bash
# Créer et switcher sur nouvelle branche
git checkout -b feature/nom-feature

# Travailler normalement...
git add .
git cm "feat: ma feature"

# Revenir sur main et fusionner
git checkout main
git merge feature/nom-feature

# Pousser sur GitHub
git push

# Supprimer la branche
git branch -d feature/nom-feature
```

---

## 📊 VISUALISATION

```bash
git lg                      # Graphe de l'historique
git show abc123             # Voir un commit spécifique
git diff HEAD~1             # Comparer avec commit précédent
git diff abc123 def456      # Comparer 2 commits
```

---

## 🔧 COMMANDES ALIASES CONFIGURÉS

```bash
git st       →  git status
git co       →  git checkout
git cm       →  git commit -m
git lg       →  git log --oneline --graph --all
git save     →  git add -A && git commit -m 'checkpoint'
```

---

## 💡 ASTUCES

### Messages de commit clairs
```bash
git cm "feat: ajout navigation menu ventouse"
git cm "fix: correction bug CAN bus timeout"
git cm "refactor: nettoyage code LVGL"
git cm "docs: ajout commentaires fonction init"
git cm "test: test unitaire communication STM32"
```

### Ignorer un fichier après coup
```bash
echo "fichier.log" >> .gitignore
git rm --cached fichier.log
git cm "chore: ignore fichier.log"
```

### Voir qui a modifié une ligne
```bash
git blame fichier.c
```

---

## 🆘 EN CAS DE PANIQUE

```bash
# 1. Ne panique pas !
# 2. Regarde ce qui s'est passé
git st
git lg

# 3. Si vraiment tout est cassé
git fetch origin
git reset --hard origin/main

# 4. Tu as perdu au max 1 session de travail
#    (si tu push régulièrement)
```

---

## 📱 LIENS UTILES

- Ton repo : https://github.com/TON-USERNAME/STRAT
- Aide Git : https://git-scm.com/docs
- Visualisateur : https://git-school.github.io/visualizing-git/

---

**Règle d'or** : Commit souvent, push régulièrement !
