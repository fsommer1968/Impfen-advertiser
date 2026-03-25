#!/bin/bash

# Impfen-Advertiser - Git Setup Script
# Dieses Script hilft beim initialen Setup des Git Repositories

echo "=========================================="
echo "Impfen-Advertiser Git Setup"
echo "=========================================="
echo ""

# Prüfen ob Git installiert ist
if ! command -v git &> /dev/null; then
    echo "❌ Git ist nicht installiert!"
    echo "Bitte installieren Sie Git: https://git-scm.com/downloads"
    exit 1
fi

echo "✅ Git ist installiert"
echo ""

# Git Repository initialisieren
echo "📦 Initialisiere Git Repository..."
git init
echo ""

# Git Konfiguration prüfen
echo "🔧 Prüfe Git Konfiguration..."
if [ -z "$(git config user.name)" ]; then
    echo "⚠️  Git user.name ist nicht gesetzt"
    read -p "Ihr Name: " username
    git config user.name "$username"
fi

if [ -z "$(git config user.email)" ]; then
    echo "⚠️  Git user.email ist nicht gesetzt"
    read -p "Ihre E-Mail: " useremail
    git config user.email "$useremail"
fi

echo "✅ Git Benutzer: $(git config user.name) <$(git config user.email)>"
echo ""

# Dateien hinzufügen
echo "📝 Füge Dateien hinzu..."
git add .
echo ""

# Status anzeigen
echo "📊 Git Status:"
git status --short
echo ""

# Ersten Commit erstellen
echo "💾 Erstelle ersten Commit..."
git commit -m "Initial commit: Impfen-Advertiser v2.0.0

- Vollständig dokumentierter Code
- Separate Konfigurationsdatei (config.h.example)
- Umfassende README (README_V2.md)
- Migrations-Guide (MIGRATION_V1_TO_V2.md)
- Contributing Guidelines (CONTRIBUTING.md)
- GitHub Setup Anleitung (GITHUB_SETUP.md)
- MIT Lizenz"
echo ""

# Branch umbenennen
echo "🌿 Benenne Branch zu 'main' um..."
git branch -M main
echo ""

# Remote Repository hinzufügen
echo "🔗 Remote Repository hinzufügen..."
echo "Bitte geben Sie die URL Ihres GitHub Repositories ein:"
echo "Format: https://github.com/IhrUsername/impfen-advertiser.git"
read -p "Repository URL: " repo_url

if [ -n "$repo_url" ]; then
    git remote add origin "$repo_url"
    echo "✅ Remote 'origin' hinzugefügt: $repo_url"
    echo ""
    
    # Push anbieten
    read -p "Möchten Sie jetzt zu GitHub pushen? (j/n): " push_now
    if [ "$push_now" = "j" ] || [ "$push_now" = "J" ]; then
        echo "🚀 Pushe zu GitHub..."
        git push -u origin main
        echo ""
        echo "✅ Code erfolgreich zu GitHub hochgeladen!"
    else
        echo "ℹ️  Sie können später mit 'git push -u origin main' pushen"
    fi
else
    echo "⚠️  Keine URL eingegeben. Remote wurde nicht hinzugefügt."
    echo "Sie können später mit folgendem Befehl hinzufügen:"
    echo "git remote add origin https://github.com/IhrUsername/impfen-advertiser.git"
fi

echo ""
echo "=========================================="
echo "✅ Git Setup abgeschlossen!"
echo "=========================================="
echo ""
echo "Nächste Schritte:"
echo "1. Erstellen Sie ein Repository auf GitHub.com"
echo "2. Falls noch nicht geschehen: git remote add origin <URL>"
echo "3. git push -u origin main"
echo ""
echo "Siehe GITHUB_SETUP.md für detaillierte Anleitung"
