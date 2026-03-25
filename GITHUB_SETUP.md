# GitHub Repository Setup - Schritt-für-Schritt Anleitung

Diese Anleitung hilft Ihnen, das Impfen-Advertiser Projekt auf GitHub zu veröffentlichen.

## 📋 Voraussetzungen

- GitHub Account
- Git installiert auf Ihrem Computer
- Terminal/Kommandozeile Zugang

## 🚀 Schnellstart

### Option 1: Neues Repository erstellen

```bash
# 1. In das Projektverzeichnis wechseln
cd /Users/franksommer/Desktop/impfen/Impfen-advertiser

# 2. Git Repository initialisieren
git init

# 3. Alle Dateien hinzufügen
git add .

# 4. Ersten Commit erstellen
git commit -m "Initial commit: Impfen-Advertiser v2.0.0"

# 5. Hauptbranch umbenennen (optional, aber empfohlen)
git branch -M main

# 6. Remote Repository hinzufügen (URL durch Ihre ersetzen)
git remote add origin https://github.com/IhrUsername/impfen-advertiser.git

# 7. Code hochladen
git push -u origin main
```

### Option 2: GitHub Desktop verwenden

1. GitHub Desktop öffnen
2. **File** → **Add Local Repository**
3. Projektordner auswählen
4. **Publish repository** klicken
5. Repository-Name und Beschreibung eingeben
6. **Publish Repository** bestätigen

## 📝 Detaillierte Anleitung

### Schritt 1: GitHub Repository erstellen

1. Auf [GitHub.com](https://github.com) einloggen
2. Klick auf **+** (oben rechts) → **New repository**
3. Repository-Details eingeben:
   - **Repository name**: `impfen-advertiser`
   - **Description**: `ESP8266-basierter WLAN-Advertiser mit SSID-Rotation und Captive Portal`
   - **Visibility**: Public oder Private
   - **NICHT** "Initialize with README" ankreuzen (haben wir schon)
   - **NICHT** .gitignore hinzufügen (haben wir schon)
   - **License**: MIT (oder leer lassen, haben wir schon)
4. **Create repository** klicken

### Schritt 2: Lokales Repository vorbereiten

```bash
# In Projektverzeichnis wechseln
cd /Users/franksommer/Desktop/impfen/Impfen-advertiser

# Git initialisieren
git init

# Prüfen welche Dateien hinzugefügt werden
git status
```

**Erwartete Ausgabe:**
```
Untracked files:
  .gitignore
  CONTRIBUTING.md
  DOKUMENTATION_V2_VORSCHLAEGE.md
  LICENSE
  MIGRATION_V1_TO_V2.md
  README.md
  README_V2.md
  config.h.example
  impfen-Advertiser.ino
  impfen-advertiser-v2.ino
  ...
```

### Schritt 3: Dateien committen

```bash
# Alle Dateien zum Staging hinzufügen
git add .

# Ersten Commit erstellen
git commit -m "Initial commit: Impfen-Advertiser v2.0.0

- Vollständig dokumentierter Code
- Separate Konfigurationsdatei
- Umfassende README
- Migrations-Guide von v1 zu v2
- MIT Lizenz"

# Branch umbenennen
git branch -M main
```

### Schritt 4: Mit GitHub verbinden

```bash
# Remote Repository hinzufügen (URL von GitHub kopieren)
git remote add origin https://github.com/IhrUsername/impfen-advertiser.git

# Verbindung prüfen
git remote -v
```

**Erwartete Ausgabe:**
```
origin  https://github.com/IhrUsername/impfen-advertiser.git (fetch)
origin  https://github.com/IhrUsername/impfen-advertiser.git (push)
```

### Schritt 5: Code hochladen

```bash
# Code zu GitHub pushen
git push -u origin main
```

Bei Authentifizierungsproblemen:
```bash
# Personal Access Token verwenden (empfohlen)
# Erstellen unter: GitHub → Settings → Developer settings → Personal access tokens
```

## 🏷️ Tags und Releases

### Version 2.0.0 taggen

```bash
# Tag erstellen
git tag -a v2.0.0 -m "Version 2.0.0 - Vollständige Überarbeitung

Hauptfeatures:
- Vollständige Code-Dokumentation
- Modularisierte Struktur
- Separate Konfigurationsdatei
- Status-Endpunkt
- Verbesserte Fehlerbehandlung
- Umfassende Dokumentation"

# Tag zu GitHub pushen
git push origin v2.0.0
```

### Release auf GitHub erstellen

1. Auf GitHub zum Repository gehen
2. **Releases** → **Create a new release**
3. Tag auswählen: `v2.0.0`
4. Release-Titel: `Version 2.0.0 - Major Update`
5. Beschreibung hinzufügen (siehe unten)
6. **Publish release** klicken

**Release-Beschreibung Template:**
```markdown
## 🎉 Version 2.0.0 - Major Update

Vollständige Überarbeitung des Impfen-Advertiser Projekts mit verbesserter Code-Qualität, Dokumentation und Features.

### ✨ Neue Features
- 📊 Status-Endpunkt für System-Monitoring
- 🐛 Debug- und Verbose-Modi
- ⚙️ Separate Konfigurationsdatei
- 📝 Vollständige Code-Dokumentation

### 🔧 Verbesserungen
- Modularisierte Code-Struktur
- Einheitliche Namenskonventionen
- Bessere Fehlerbehandlung
- Optimierte Speichernutzung

### 📚 Dokumentation
- Umfassende README mit Installationsanleitung
- Migrations-Guide von v1 zu v2
- Contributing Guidelines
- Troubleshooting-Sektion

### 📥 Installation

1. Repository clonen oder ZIP herunterladen
2. `config.h.example` nach `config.h` kopieren
3. `config.h` mit eigenen Daten anpassen
4. Code auf ESP8266 hochladen

Siehe [README_V2.md](README_V2.md) für detaillierte Anleitung.

### 🔄 Migration von v1

Siehe [MIGRATION_V1_TO_V2.md](MIGRATION_V1_TO_V2.md) für Upgrade-Anleitung.

### 📄 Changelog

Vollständiges Changelog siehe [README_V2.md](README_V2.md#-changelog)
```

## 📁 Repository-Struktur

Nach dem Upload sollte Ihr Repository so aussehen:

```
impfen-advertiser/
├── .gitignore                        # Git-Ignore-Regeln
├── CONTRIBUTING.md                   # Beitrags-Richtlinien
├── DOKUMENTATION_V2_VORSCHLAEGE.md   # Analyse & Vorschläge
├── LICENSE                           # MIT Lizenz
├── MIGRATION_V1_TO_V2.md             # Migrations-Guide
├── README.md                         # Original README (v1)
├── README_V2.md                      # Neue README (v2)
├── config.h.example                  # Konfigurationsvorlage
├── impfen-Advertiser.ino             # Original Code (v1)
├── impfen-advertiser-v2.ino          # Neuer Code (v2)
└── IMG_*.JPG                         # Bilder (falls vorhanden)
```

## 🎨 Repository verschönern

### About-Sektion konfigurieren

1. Auf GitHub zum Repository
2. Zahnrad-Symbol bei "About" klicken
3. Eingeben:
   - **Description**: `ESP8266 WLAN-Advertiser mit SSID-Rotation und Captive Portal`
   - **Website**: (falls vorhanden)
   - **Topics**: `esp8266`, `arduino`, `captive-portal`, `wifi`, `iot`, `advertising`
4. **Save changes**

### README als Hauptdatei festlegen

GitHub zeigt automatisch `README.md` an. Für v2:

**Option 1: README.md ersetzen**
```bash
# Alte README sichern
mv README.md README_V1.md

# Neue README als Haupt-README
cp README_V2.md README.md

# Committen
git add .
git commit -m "docs: README_V2 als Haupt-README festgelegt"
git push
```

**Option 2: Beide behalten**
- `README.md` zeigt auf beide Versionen
- Benutzer können wählen

### Badges hinzufügen

In `README.md` am Anfang:

```markdown
[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/IhrUsername/impfen-advertiser/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-ESP8266-orange.svg)](https://www.espressif.com/)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-00979D.svg)](https://www.arduino.cc/)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)
```

## 🔒 Sicherheit

### Wichtig: config.h nicht committen!

Die `.gitignore` verhindert das automatisch, aber prüfen Sie:

```bash
# Prüfen ob config.h ignoriert wird
git status

# config.h sollte NICHT in der Liste sein
```

Falls doch:
```bash
# Aus Git entfernen (aber lokal behalten)
git rm --cached config.h
git commit -m "security: config.h aus Repository entfernt"
git push
```

### Security Policy hinzufügen

Erstellen Sie `SECURITY.md`:

```markdown
# Security Policy

## Unterstützte Versionen

| Version | Unterstützt |
| ------- | ----------- |
| 2.0.x   | ✅          |
| 1.0.x   | ❌          |

## Sicherheitslücke melden

Bitte melden Sie Sicherheitslücken NICHT über öffentliche Issues.

Stattdessen:
- E-Mail an: security@example.com
- Oder: Private Security Advisory auf GitHub

Wir werden uns innerhalb von 48 Stunden melden.
```

## 📊 GitHub Features aktivieren

### Issues aktivieren

1. **Settings** → **Features**
2. **Issues** aktivieren
3. Issue-Templates erstellen (optional)

### Discussions aktivieren

1. **Settings** → **Features**
2. **Discussions** aktivieren
3. Kategorien erstellen:
   - 💡 Ideas
   - 🙏 Q&A
   - 📣 Announcements
   - 🐛 Bug Reports

### Wiki aktivieren (optional)

1. **Settings** → **Features**
2. **Wiki** aktivieren
3. Dokumentation hinzufügen

### Projects (optional)

Für Roadmap und Task-Management:
1. **Projects** → **New project**
2. Template wählen (z.B. "Board")
3. Tasks hinzufügen

## 🔄 Workflow nach Setup

### Änderungen pushen

```bash
# Status prüfen
git status

# Dateien hinzufügen
git add .

# Committen
git commit -m "feat: neue Funktion hinzugefügt"

# Pushen
git push
```

### Branch für Features erstellen

```bash
# Neuen Branch erstellen
git checkout -b feature/neue-funktion

# Änderungen machen und committen
git add .
git commit -m "feat: neue Funktion implementiert"

# Branch pushen
git push -u origin feature/neue-funktion

# Auf GitHub: Pull Request erstellen
```

## 📱 GitHub Mobile App

Für unterwegs:
- **iOS**: [App Store](https://apps.apple.com/app/github/id1477376905)
- **Android**: [Google Play](https://play.google.com/store/apps/details?id=com.github.android)

## 🎓 Weitere Ressourcen

- [GitHub Docs](https://docs.github.com)
- [Git Cheat Sheet](https://education.github.com/git-cheat-sheet-education.pdf)
- [GitHub Skills](https://skills.github.com/)

## ✅ Checkliste

Nach dem Setup:

- [ ] Repository auf GitHub erstellt
- [ ] Code hochgeladen
- [ ] README.md wird korrekt angezeigt
- [ ] config.h ist in .gitignore
- [ ] LICENSE vorhanden
- [ ] Release v2.0.0 erstellt
- [ ] About-Sektion ausgefüllt
- [ ] Topics hinzugefügt
- [ ] Issues aktiviert
- [ ] CONTRIBUTING.md vorhanden

## 🆘 Probleme?

### "Permission denied"
```bash
# SSH-Key einrichten oder Personal Access Token verwenden
# Siehe: https://docs.github.com/en/authentication
```

### "Repository not found"
```bash
# Remote URL prüfen
git remote -v

# Korrigieren falls nötig
git remote set-url origin https://github.com/IhrUsername/impfen-advertiser.git
```

### "Merge conflict"
```bash
# Lokale Änderungen stashen
git stash

# Pullen
git pull

# Stash anwenden
git stash pop
```

## 🎉 Fertig!

Ihr Repository ist jetzt auf GitHub verfügbar! 🚀

**Nächste Schritte:**
1. Repository-Link teilen
2. Community einladen
3. Issues und PRs bearbeiten
4. Weiterentwickeln

**Repository-URL:**
```
https://github.com/IhrUsername/impfen-advertiser