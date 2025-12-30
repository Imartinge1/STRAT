#!/bin/bash

echo "========================================="
echo "🚀 CONFIGURATION GIT POUR PROJET STRAT"
echo "========================================="
echo ""

# Couleurs
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 1. Vérifier qu'on est dans le bon dossier
echo -e "${YELLOW}📁 Étape 1/7 : Vérification du dossier${NC}"
if [ ! -f "platformio.ini" ]; then
    echo "❌ Erreur : fichier platformio.ini non trouvé"
    echo "Tu dois lancer ce script depuis ton dossier Ecran_Strategie_V4"
    echo ""
    echo "Utilise : cd /chemin/vers/Ecran_Strategie_V4"
    echo "Puis relance ce script"
    exit 1
fi
echo -e "${GREEN}✅ Dossier projet détecté${NC}"
echo ""

# 2. Configuration Git globale
echo -e "${YELLOW}📝 Étape 2/7 : Configuration Git globale${NC}"
echo "Configuration de ton nom et email..."
read -p "Ton nom complet (ex: Yanis Dupont) : " git_name
read -p "Ton email GitHub : " git_email

git config --global user.name "$git_name"
git config --global user.email "$git_email"

# Aliases pratiques
git config --global alias.st "status"
git config --global alias.co "checkout"
git config --global alias.cm "commit"
git config --global alias.lg "log --oneline --graph --all --decorate"
git config --global alias.save "!git add -A && git commit -m 'checkpoint'"

echo -e "${GREEN}✅ Configuration globale terminée${NC}"
echo ""

# 3. Initialisation Git
echo -e "${YELLOW}🔧 Étape 3/7 : Initialisation du repository local${NC}"
if [ -d ".git" ]; then
    echo "⚠️  Un repository Git existe déjà"
    read -p "Veux-tu le réinitialiser ? (y/N) : " reset_git
    if [ "$reset_git" = "y" ] || [ "$reset_git" = "Y" ]; then
        rm -rf .git
        git init
        echo -e "${GREEN}✅ Repository réinitialisé${NC}"
    else
        echo "Repository Git existant conservé"
    fi
else
    git init
    echo -e "${GREEN}✅ Git initialisé${NC}"
fi
echo ""

# 4. Création du .gitignore
echo -e "${YELLOW}📄 Étape 4/7 : Création du .gitignore${NC}"
cat > .gitignore << 'EOF'
# PlatformIO
.pio
.vscode/.browse.c_cpp.db*
.vscode/c_cpp_properties.json
.vscode/launch.json
.vscode/ipch

# Build files
build/
*.o
*.elf
*.bin
*.hex

# OS files
.DS_Store
Thumbs.db

# IDE
*.swp
*.swo
*~

# Logs
*.log
EOF
echo -e "${GREEN}✅ .gitignore créé${NC}"
echo ""

# 5. Premier commit
echo -e "${YELLOW}💾 Étape 5/7 : Premier commit${NC}"
git add .
git commit -m "🎉 Initial commit - projet STRAT clean"
echo -e "${GREEN}✅ Commit initial créé${NC}"
echo ""

# 6. Configuration GitHub
echo -e "${YELLOW}🌐 Étape 6/7 : Configuration GitHub${NC}"
echo ""
echo "Maintenant, va sur GitHub :"
echo "1. Va sur https://github.com/new"
echo "2. Nom du repository : STRAT"
echo "3. Description : Écran stratégie robotique - STM32F469I"
echo "4. Laisse en Public ou Private (ton choix)"
echo "5. NE COCHE PAS 'Add README' ou '.gitignore'"
echo "6. Clique 'Create repository'"
echo ""
read -p "Repository créé sur GitHub ? (appuie sur Entrée quand c'est fait) " 

read -p "Ton nom d'utilisateur GitHub : " github_username

git remote add origin "https://github.com/$github_username/STRAT.git"
git branch -M main
git push -u origin main

echo -e "${GREEN}✅ Code pushé sur GitHub${NC}"
echo ""

# 7. Récapitulatif
echo -e "${YELLOW}📋 Étape 7/7 : Récapitulatif${NC}"
echo ""
echo "========================================="
echo "✨ CONFIGURATION TERMINÉE !"
echo "========================================="
echo ""
echo "🎯 Commandes essentielles :"
echo ""
echo "  git st              → Voir l'état"
echo "  git lg              → Voir l'historique"
echo "  git save            → Commit rapide checkpoint"
echo "  git add .           → Ajouter tous les fichiers"
echo "  git cm 'message'    → Commit avec message"
echo "  git push            → Envoyer sur GitHub"
echo "  git pull            → Récupérer depuis GitHub"
echo ""
echo "🔄 Workflow recommandé :"
echo ""
echo "  # Avant de coder"
echo "  git pull"
echo ""
echo "  # Toutes les 30 min ou après une feature"
echo "  git add ."
echo "  git cm 'feat: description'"
echo "  git push"
echo ""
echo "  # En cas de problème"
echo "  git reset --hard origin/main"
echo ""
echo "========================================="
echo ""
echo "Repository : https://github.com/$github_username/STRAT"
echo ""
