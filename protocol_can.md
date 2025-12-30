# Protocole CAN - Ecran_Strategie_V4

Documentation complète du protocole de communication CAN pour le système de contrôle robotique C.R.A.C 2025.

**Version**: v0.1
**Bus CAN**: CAN2 à 1 Mbps (RX=PB_5, TX=PB_13)
**Endianness**: Little Endian (LSB first)
**IDs définis**: 254 constantes

---

## Table des matières

1. [Résumé exécutif](#résumé-exécutif)
2. [Commandes de mouvement](#commandes-de-mouvement)
3. [Contrôle servomoteurs Herkulex](#contrôle-servomoteurs-herkulex)
4. [Contrôle actionneurs](#contrôle-actionneurs)
5. [Odométrie et feedback](#odométrie-et-feedback)
6. [Messages système](#messages-système)
7. [Définitions complètes des IDs](#définitions-complètes-des-ids)
8. [Encodage des données](#encodage-des-données)

---

## Résumé exécutif

### Caractéristiques du protocole

- **Bus physique**: CAN2 uniquement (CAN1 en conflit avec écran tactile)
- **Débit**: 1 Mbps
- **Format**: CAN Standard (11-bit ID)
- **Ordre des octets**: Little Endian (LSB en premier)
- **Buffers**: 100 messages TX + 100 messages RX
- **Timeout ACK**: 1000 ms
- **Timeout completion**: 20 000 ms

### Statistiques

| Catégorie | Messages TX | Messages RX | Total |
|-----------|-------------|-------------|-------|
| Mouvement | 9 | 4 | 13 |
| Herkulex | 5 | 1 | 6 |
| Actionneurs | 14 | 3 | 17 |
| Odométrie | 2 | 2 | 4 |
| ACKs | 0 | 8 | 8 |
| Completions | 0 | 6 | 6 |
| Système | 3 | 3 | 6 |
| Debug | 20 | 30 | 50 |
| **TOTAL** | **37** | **40** | **77** |

---

## Commandes de mouvement

### 1. Position XYT (0x020)

Déplace le robot à une position absolue (X, Y) avec orientation Theta.

**Fonction**: `deplacement.positionXYTheta(x, y, theta, sens)`

**Format**:
```
ID:   0x020 (ASSERVISSEMENT_XYT)
Len:  7 octets
Data: [x_LSB] [x_MSB] [y_LSB] [y_MSB] [θ_LSB] [θ_MSB] [sens]
```

**Détail des octets**:

| Octet | Type | Description | Unité | Plage |
|-------|------|-------------|-------|-------|
| 0-1 | uint16_t | Position X | mm | 0-3000 |
| 2-3 | uint16_t | Position Y | mm | 0-2000 |
| 4-5 | int16_t | Angle Theta | dixièmes de degré | -1800 à 1800 |
| 6 | int8_t | Sens de déplacement | - | -1=arrière, 0=auto, 1=avant |

**Exemple concret**: Aller à (1000mm, 500mm, 90.0°) en marche avant

```
ID: 0x020
Data: [E8 03 F4 01 84 03 01]

Décodage:
  E8 03 → 0x03E8 = 1000 (X en mm)
  F4 01 → 0x01F4 = 500  (Y en mm)
  84 03 → 0x0384 = 900  (90.0° * 10)
  01    → Sens avant
```

**Flux de communication**:
```
TX → 0x020 [données XYT]
RX ← 0x101 [20 00] (ACK moteur)
... (mouvement en cours)
RX ← 0x111 [20 00] (instruction terminée)
```

---

### 2. Rotation (0x023)

Rotation sur place du robot.

**Fonction**: `deplacement.rotation(angle)`

**Format**:
```
ID:   0x023 (ASSERVISSEMENT_ROTATION)
Len:  2 octets
Data: [angle_LSB] [angle_MSB]
```

**Détail des octets**:

| Octet | Type | Description | Unité | Plage |
|-------|------|-------------|-------|-------|
| 0-1 | int16_t | Angle de rotation | **degrés** (pas dixièmes!) | -180 à 180 |

⚠️ **ATTENTION**: Contrairement aux autres commandes, l'angle est en **degrés entiers**, pas en dixièmes!

**Exemple concret**: Rotation de 90° sens horaire

```
ID: 0x023
Data: [A6 FF]

Décodage:
  A6 FF → 0xFFA6 = -90 (degrés)
```

---

### 3. Ligne droite / Recalage (0x024)

Déplacement en ligne droite ou recalage contre bordure.

**Fonction**: `deplacement.toutDroit(distance)` ou `deplacement.recalage(distance, type, newValue)`

**Format**:
```
ID:   0x024 (ASSERVISSEMENT_RECALAGE)
Len:  6 octets
Data: [dist_LSB] [dist_MSB] [recal_type] [new_val_LSB] [new_val_MSB] [chain]
```

**Détail des octets**:

| Octet | Type | Description | Valeurs |
|-------|------|-------------|---------|
| 0-1 | int16_t | Distance | mm (négatif=arrière) |
| 2 | uint8_t | Type recalage | 0=aucun, 1=recal X, 2=recal Y |
| 3-4 | uint16_t | Nouvelle valeur après recalage | mm |
| 5 | uint8_t | Chaînage | 0=standalone, 1=chaîné, 2=dernier |

**Exemple 1**: Ligne droite 500mm avant

```
ID: 0x024
Data: [F4 01 00 00 00 00]

Décodage:
  F4 01 → 500mm avant
  00    → Pas de recalage
  00 00 → Pas de nouvelle valeur
  00    → Mouvement standalone
```

**Exemple 2**: Recalage arrière sur axe X à 150mm

```
ID: 0x024
Data: [E8 FC 01 96 00 00]

Décodage:
  E8 FC → 0xFCE8 = -1000mm (recule jusqu'à contact)
  01    → Recalage sur X
  96 00 → 0x0096 = 150mm (nouvelle position X)
  00    → Standalone
```

---

### 4. Courbe/Arc (0x021)

Trajectoire courbe avec rayon et angle constants.

**Fonction**: `deplacement.courbure(rayon, angle, sens)`

**Format**:
```
ID:   0x021 (ASSERVISSEMENT_COURBURE)
Len:  6 octets
Data: [rayon_LSB] [rayon_MSB] [angle_LSB] [angle_MSB] [sens] [chain]
```

**Détail des octets**:

| Octet | Type | Description | Unité |
|-------|------|-------------|-------|
| 0-1 | int16_t | Rayon de courbure | mm |
| 2-3 | int16_t | Angle de rotation | dixièmes de degré |
| 4 | int8_t | Sens | 1=gauche/CCW, -1=droite/CW |
| 5 | uint8_t | Chaînage | 0/1/2 |

**Exemple concret**: Arc rayon 300mm, angle 45°, virage à gauche

```
ID: 0x021
Data: [2C 01 C2 01 01 00]

Décodage:
  2C 01 → 300mm (rayon)
  C2 01 → 450 (45.0° * 10)
  01    → Gauche
  00    → Standalone
```

---

### 5. Courbe de Bézier (0x02A)

Trajectoire complexe par courbes de Bézier cubiques.

**Fonction**: `deplacement.courbeBezier(nbCourbes, P1[][], C1[][], C2[][], sens)`

⚠️ **Protocole multi-trames avec délais obligatoires**

**Trame 1 - Initialisation**:
```
ID:   0x02A
Len:  2 octets
Data: [nbCourbes] [sens]
```

**Trame 2 - Points de contrôle (partie 1)**:
```
ID:   0x02A
Len:  7 octets
Data: [P1x_LSB] [P1x_MSB] [P1y_LSB] [P1y_MSB] [C1x_LSB] [C1x_MSB] [index]
```

**Trame 3 - Points de contrôle (partie 2)**:
```
ID:   0x02A
Len:  7 octets
Data: [C1y_LSB] [C1y_MSB] [C2x_LSB] [C2x_MSB] [C2y_LSB] [C2y_MSB] [index+100]
```

**Notes critiques**:
- Délai **150ms obligatoire** entre chaque trame
- Index+100 dans trame 3 pour différencier continuation
- P1 = point d'arrivée, C1/C2 = points de contrôle

**Exemple**: 2 courbes Bézier
```
Trame 1: [02 01]                      // 2 courbes, sens avant
Wait 150ms
Trame 2: [E8 03 F4 01 DC 05 00]      // P1=(1000,500), C1x=1500, index=0
Wait 150ms
Trame 3: [F4 01 DC 05 F4 01 64]      // C1y=500, C2=(1500,500), index=100
Wait 150ms
Trame 4: [D0 07 88 13 B8 0B 01]      // Courbe 2...
...
```

---

### 6. Configuration vitesse/accélération

#### Vitesse seule (0x011)

```
ID:   0x011 (ASSERVISSEMENT_CONFIG_VIT)
Len:  2 octets
Data: [speed_LSB] [speed_MSB]

Unité: mm/s
Exemple: [E8 03] → 1000 mm/s
```

#### Accélération/décélération (0x012)

```
ID:   0x012 (ASSERVISSEMENT_CONFIG_ACCEL)
Len:  4 octets
Data: [accel_LSB] [accel_MSB] [decel_LSB] [decel_MSB]

Unité: mm/s²
Exemple: [F4 01 F4 01] → accel=500, decel=500
```

#### Vitesse + accélération combinée (0x022)

```
ID:   0x022 (ASSERVISSEMENT_CONFIG)
Len:  4 octets
Data: [speed_LSB] [speed_MSB] [accel_LSB] [accel_MSB]

Note: Décélération calculée automatiquement (75% de l'accélération)
```

---

### 7. Arrêt d'urgence (0x001)

```
ID:   0x001 (ASSERVISSEMENT_STOP)
Len:  0 octets
```

Arrête immédiatement tous les mouvements.

---

### 8. Activation asservissement (0x1F7)

```
ID:   0x1F7 (ASSERVISSEMENT_ENABLE)
Len:  1 octet
Data: [enable]

enable: 0=désactiver, 1=activer
```

---

## Contrôle servomoteurs Herkulex

### 1. Position unique (0x050)

Commande de position pour un servo Herkulex.

**Fonction**: `herkulex.controleHerkulexPosition(IDHerkulex, position, setLed)`

**Format**:
```
ID:   0x050 (IDCAN_HERKULEX)
Len:  6 octets
Data: [servo_ID] [commande] [pos_LSB] [pos_MSB] [playtime] [couleur]
```

**Détail des octets**:

| Octet | Type | Description | Valeurs |
|-------|------|-------------|---------|
| 0 | uint8_t | ID du servo | 1-104 |
| 1 | uint8_t | Type de commande | 0=position simple |
| 2-3 | int16_t | Position cible | Unités servo (0-1023) |
| 4 | uint8_t | Playtime | 0x3C=60 → 600ms |
| 5 | uint8_t | Couleur LED | 0x04=vert, 0x08=bleu, 0x10=rouge |

**Exemple concret**: Servo #7 → position 512, LED verte, 600ms

```
ID: 0x050
Data: [07 00 00 02 3C 04]

Décodage:
  07    → Servo ID 7
  00    → Mode position
  00 02 → 512
  3C    → 60 → 600ms
  04    → Vert
```

---

### 2. Position double simultanée (0x050)

Contrôle synchronisé de 2 servos.

**Fonction**: `herkulex.controleHerkulexPositionMulEnsemble(ID1, pos1, ID2, pos2)`

**Format**:
```
ID:   0x050
Len:  8 octets
Data: [ID1] [cmd=1] [pos1_LSB] [pos1_MSB] [ID2] [pos2_LSB] [pos2_MSB] [playtime]
```

**Exemple**: Servos #3 et #5 → positions 200 et 800

```
ID: 0x050
Data: [03 01 C8 00 05 20 03 3C]

Décodage:
  03    → Servo 1 = ID 3
  01    → Mode dual position
  C8 00 → 200 (position servo 1)
  05    → Servo 2 = ID 5
  20 03 → 800 (position servo 2)
  3C    → Playtime 600ms
```

---

### 3. Contrôle couple (torque) (0x052)

Active ou désactive le couple du servo.

**Format**:
```
ID:   0x052 (IDCAN_HERKULEX_Torque)
Len:  2 octets
Data: [servo_ID] [torque_state]

torque_state: 0x60=ON, 0x00=OFF
```

**Exemple**: Désactiver couple servo #10

```
ID: 0x052
Data: [0A 00]
```

---

### 4. Mode vitesse continue (0x063)

Configure le servo en mode rotation continue.

**Format**:
```
ID:   0x063 (ID_HERKULEX_VITESSE)
Len:  4 octets
Data: [servo_ID] [speed_LSB] [speed_MSB] [LED=0x04]
```

**Exemple**: Servo #8 vitesse 300

```
ID: 0x063
Data: [08 2C 01 04]

Décodage:
  08    → Servo 8
  2C 01 → 300 (vitesse)
  04    → LED verte
```

---

### 5. Effacer erreurs (0x055)

Réinitialise les erreurs d'un servo.

**Format**:
```
ID:   0x055 (IDCAN_HERKULEX_Clear)
Len:  1 octet
Data: [servo_ID]
```

---

## Contrôle actionneurs

### 1. Pince avant (0x070)

Contrôle de la pince/préhenseur avant.

**Fonction**: `herkulex.controlePince(Etage, etatHerkulex, sens)`

**Format**:
```
ID:   0x070 (IDCAN_PINCE)
Len:  3 octets
Data: [Etage] [etat] [sens]
```

**Détail**:

| Octet | Description | Valeurs |
|-------|-------------|---------|
| 0 | Étage/niveau | 0-4 |
| 1 | État | 0=fermé, 1=ouvert |
| 2 | Côté/sens | Dépend du contexte |

---

### 2. Pince arrière (0x075)

**Format**:
```
ID:   0x075 (IDCAN_PINCE_ARRIERE)
Len:  3 octets
Data: [etat] [poseCerise] [presenceGato]
```

**Détail**:

| Octet | Description | Valeurs |
|-------|-------------|---------|
| 0 | État pince | 0=fermé, 1=position gâteau, 2=ouvert |
| 1 | Poser cerise | bool (0/1) |
| 2 | Présence gâteau | bool (toujours 1) |

---

### 3. Moteur pas-à-pas position (0x060)

Contrôle de hauteur par moteur stepper.

**Fonction**: `herkulex.stepMotorHauteur(mm)`

**Format**:
```
ID:   0x060 (IDCAN_STEP_MOT_POS)
Len:  4 octets
Data: [pos_LSB] [pos_B1] [pos_B2] [pos_MSB]
```

**Calcul**: `position = mm × 45`

**Exemple**: Monter à 50mm

```
ID: 0x060
Data: [D2 08 00 00]

Décodage:
  D2 08 00 00 → 0x000008D2 = 2250 = 50mm × 45
```

---

### 4. Moteur pas-à-pas mode (0x061)

Configure le mode du stepper (pas complet, demi-pas, etc.).

**Format**:
```
ID:   0x061 (IDCAN_STEP_MOT_MODE)
Len:  3 octets
Data: [m0] [m1] [m2]

m0 = mode & 0x01
m1 = (mode >> 1) & 0x01
m2 = (mode >> 2) & 0x01
```

---

### 5. Actions de jeu spécifiques

Tous les IDs suivants sont des commandes **sans données** (len=0):

| ID | Hex | Nom | Description |
|----|-----|-----|-------------|
| 0x090 | 144 | IDCAN_Construire_avant | Construire avant |
| 0x091 | 145 | IDCAN_Construire_arriere | Construire arrière |
| 0x171 | 369 | saisir_avant | Saisir avant |
| 0x174 | 372 | saisir_arriere | Saisir arrière |
| 0x255 | 597 | Pompe_av | Pompe avant |
| 0x256 | 598 | Pompe_ar | Pompe arrière |
| 0x257 | 599 | DEP_banderole | Déployer banderole |
| 0x258 | 600 | POS_avant | Position init avant |
| 0x259 | 601 | POS_arriere | Position init arrière |
| 0x266 | 614 | VENT_AV | Ventouse avant |
| 0x267 | 615 | VENT_AR | Ventouse arrière |

**Actions avec paramètres** (via `sendAck`):

```
ID:   0x092 (IDCAN_NIV_BASE_AV) ou 0x093 (IDCAN_NIV_BASE_ARR)
Len:  2 octets
Data: [niveau_LSB] [niveau_MSB]

Exemple: Niveau 3 avant → [03 00]
```

```
ID:   0x172 (IDCAN_LACHE_AV) ou 0x173 (IDCAN_LACHE_AR)
Len:  2 octets
Data: [niveau_LSB] [niveau_MSB]

Exemple: Lâcher niveau 5 arrière → [05 00]
```

---

## Odométrie et feedback

### 1. Définir position odométrie (0x026, 0x028)

**Fonction**: `deplacement.setOdoPetit(x, y, theta)` ou `deplacement.setOdoGrand(x, y, theta)`

**Format**:
```
ID:   0x026 (petit robot) ou 0x028 (grand robot)
Len:  6 octets
Data: [x_LSB] [x_MSB] [y_LSB] [y_MSB] [θ_LSB] [θ_MSB]
```

**Détail**:

| Octet | Type | Description | Unité |
|-------|------|-------------|-------|
| 0-1 | uint16_t | Position X | mm |
| 2-3 | uint16_t | Position Y | mm |
| 4-5 | int16_t | Angle Theta | dixièmes de degré |

**Exemple**: Initialiser position (1500, 200, 0.0°)

```
ID: 0x028
Data: [DC 05 C8 00 00 00]

Décodage:
  DC 05 → 1500mm (X)
  C8 00 → 200mm (Y)
  00 00 → 0° (Theta)
```

---

### 2. Réception position odométrie (RX 0x026, 0x028)

**Émetteur**: Carte moteur

**Format**: Identique ci-dessus

**Traitement** (strategie.cpp:341-352):
```cpp
x_robot = rxMsg->data[0] | (rxMsg->data[1] << 8);
y_robot = rxMsg->data[2] | (rxMsg->data[3] << 8);
theta_robot = rxMsg->data[4] | (rxMsg->data[5] << 8);
```

**Fréquence**: Environ 10-50 Hz (dépend implémentation carte moteur)

---

### 3. Détection objet LIDAR (RX 0x082)

**ID**: `IDCAN_POS_XY_OBJET`

**Format**:
```
ID:   0x082
Len:  7 octets
Data: [obj_id] [x_LSB] [x_MSB] [y_LSB] [y_MSB] [θ_LSB] [θ_MSB]
```

**Détail**:

| Octet | Type | Description |
|-------|------|-------------|
| 0 | uint8_t | ID objet (0xFF=aucun objet) |
| 1-2 | int16_t | Position X objet (mm) |
| 3-4 | int16_t | Position Y objet (mm) |
| 5-6 | int16_t | Orientation objet (dixièmes de degré) |

**Traitement**: Envoyé au module `evitement` pour logique d'évitement

---

## Messages système

### 1. Signal jack (TX/RX 0x008)

```
ID:   0x008 (GLOBAL_JACK)
Len:  0 octets
```

Peut être envoyé par IHM (jack simulé) ou reçu d'un capteur externe.

---

### 2. Mise à jour score (RX 0x097)

```
ID:   0x097 (IDCAN_SET_SCORE)
Len:  2 octets
Data: [score_LSB] [score_MSB]

Exemple: Score 150 → [96 00]

Traitement:
SCORE_GLOBAL = data[0] | (data[1] << 8);
```

---

### 3. Initialisation position terminée (TX 0x005)

```
ID:   0x005 (Pos_Init)
Len:  4 octets
Data: [x_LSB] [x_MSB] [y_LSB] [y_MSB]

Exemple: Position (1500, 200) → [DC 05 C8 00]
```

Envoyé après séquence d'initialisation des actionneurs.

---

### 4. Mode simulateur (TX 0x765)

```
ID:   0x765 (simulateur)
Len:  0 octets
```

Envoyé au début du match pour indiquer mode simulation.

---

### 5. Début de match (TX 0x002)

```
ID:   0x002 (GLOBAL_START)
Len:  0 octets
```

---

### 6. Fin de match (TX 0x004)

```
ID:   0x004 (GLOBAL_GAME_END)
Len:  0 octets
```

---

## Flux de communication typiques

### Flux mouvement

```
1. TX → 0x020 [E8 03 F4 01 84 03 01]     // Commande XYT
2. Set waitingAckID=0x020, waitingAckFrom=0x101
3. RX ← 0x101 [20 00]                     // ACK moteur (commande reçue)
4. Clear waitingAckID, set flag AckFrom_FLAG
5. Set waitingAckID_FIN=0x020, waitingAckFrom_FIN=0x111
6. ... (mouvement en cours, odométrie reçue en continu)
7. RX ← 0x111 [20 00]                     // INSTRUCTION_END_MOTEUR
8. Clear waitingAckID_FIN, set flag AckFrom_FIN_FLAG
9. Mouvement terminé
```

### Flux actionneur

```
1. TX → 0x092 [03 00]                     // Niveaux base avant = 3
2. Set waitingAckID=0x092, waitingAckFrom=0x103
3. RX ← 0x103 [92 00]                     // ACK actionneurs
4. Clear waitingAckID, set flag
5. Set waitingAckID_FIN=0x092, waitingAckFrom_FIN=0x074
6. ... (action en cours)
7. RX ← 0x074 [92 00]                     // INSTRUCTION_END_Actionneur
8. Clear waitingAckID_FIN, set flag
9. Action terminée
```

---

## Définitions complètes des IDs

### Contrôle global (0x001-0x010)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 1 | 0x001 | ASSERVISSEMENT_STOP | TX | Arrêt urgence |
| 2 | 0x002 | GLOBAL_START | TX | Début match |
| 3 | 0x003 | BALISE_STOP | TX | Arrêt balise |
| 4 | 0x004 | GLOBAL_GAME_END | TX | Fin match |
| 5 | 0x005 | Pos_Init | TX | Init position terminée |
| 6 | 0x006 | ASSERVISSEMENT_SPEED_DANGER | TX | Limite vitesse danger |
| 8 | 0x008 | GLOBAL_JACK | TX/RX | Signal jack |
| 9 | 0x009 | ACKNOWLEDGE_JACK | RX | ACK jack |
| 10 | 0x00A | BALISE_DANGER | RX | Alerte danger balise |
| 11 | 0x00B | BALISE_END_DANGER | RX | Fin danger balise |
| 16 | 0x010 | RECALAGE_START | TX | Début recalage |

### Commandes mouvement (0x011-0x02A, 0x037-0x038)

| ID | Hex | Nom | Dir | Len | Description |
|----|-----|-----|-----|-----|-------------|
| 17 | 0x011 | ASSERVISSEMENT_CONFIG_VIT | TX | 2 | Config vitesse |
| 18 | 0x012 | ASSERVISSEMENT_CONFIG_ACCEL | TX | 4 | Config accel/decel |
| 21 | 0x015 | ASSERVISSEMENT_ERROR_MOTEUR | RX | - | Erreur collision moteur |
| 32 | 0x020 | ASSERVISSEMENT_XYT | TX | 7 | Position XYT |
| 33 | 0x021 | ASSERVISSEMENT_COURBURE | TX | 6 | Courbe/arc |
| 34 | 0x022 | ASSERVISSEMENT_CONFIG | TX | 4 | Config vitesse+accel |
| 35 | 0x023 | ASSERVISSEMENT_ROTATION | TX | 2 | Rotation |
| 36 | 0x024 | ASSERVISSEMENT_RECALAGE | TX | 6 | Ligne/recalage |
| 37 | 0x025 | ASSERVISSEMENT_ERREUR | RX | - | Erreur asserv |
| 42 | 0x02A | ASSERVISSEMENT_BEZIER | TX | 2/7 | Courbe Bézier |
| 55 | 0x037 | ASSERVISSEMENT_XYT_ROTATE | RX | - | Phase rotation XYT |
| 56 | 0x038 | ASSERVISSEMENT_XYT_LINE | RX | - | Phase ligne XYT |

### Reset (0x030-0x03A)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 48 | 0x030 | RESET_BALISE | TX | Reset balise |
| 49 | 0x031 | RESET_MOTEUR | TX | Reset moteur |
| 50 | 0x032 | RESET_IHM | TX | Reset IHM |
| 51 | 0x033 | RESET_ACTIONNEURS | TX | Reset actionneurs |
| 52 | 0x034 | RESET_POMPES | TX | Reset pompes |
| 53 | 0x035 | RESET_AX12 | TX | Reset servos |
| 54 | 0x036 | RESET_TELEMETRE | TX | Reset télémètre |
| 58 | 0x03A | RESET_STRAT | TX | Reset stratégie |

### Herkulex (0x050-0x065)

| ID | Hex | Nom | Dir | Len | Description |
|----|-----|-----|-----|-----|-------------|
| 64 | 0x040 | IDCAN_ACK | RX | 2 | ACK générique |
| 80 | 0x050 | IDCAN_HERKULEX | TX | 6/8 | Position servo |
| 81 | 0x051 | IDCAN_HERKULEX_ANSWER | RX | - | Réponse servo |
| 82 | 0x052 | IDCAN_HERKULEX_Torque | TX | 2 | Contrôle couple |
| 83 | 0x053 | IDCAN_HERKULEX_GET_ID | TX | 1 | Lire ID servo |
| 84 | 0x054 | IDCAN_HERKULEX_CHANGE_ID | TX | 2 | Changer ID servo |
| 85 | 0x055 | IDCAN_HERKULEX_Clear | TX | 1 | Effacer erreurs |
| 86 | 0x056 | IDCAN_HERKULEX_GET_POS | TX | 1 | Lire position |
| 87 | 0x057 | IDCAN_HERKULEX_STAT | TX | 1 | Lire statut |
| 88 | 0x058 | IDCAN_HERKULEX_SERIAL_BAUDRATE | TX | - | Config baud série |
| 89 | 0x059 | IDCAN_HERKULEX_BAUDRATE_OF_HERKULEX | TX | - | Config baud Herkulex |
| 99 | 0x063 | ID_HERKULEX_VITESSE | TX | 4 | Mode vitesse |
| 100 | 0x064 | ID_HERKULEX_TOURNER_NB_TOUR | TX | - | Mode rotation |

### Steppers (0x060-0x061)

| ID | Hex | Nom | Dir | Len | Description |
|----|-----|-----|-----|-----|-------------|
| 96 | 0x060 | IDCAN_STEP_MOT_POS | TX | 4 | Position stepper |
| 97 | 0x061 | IDCAN_STEP_MOT_MODE | TX | 3 | Mode stepper |

### Check/Alive (0x060-0x076)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 96 | 0x060 | CHECK_BALISE | TX | Check balise |
| 97 | 0x061 | CHECK_MOTEUR | TX | Check moteur |
| 98 | 0x062 | CHECK_IHM | TX | Check IHM |
| 99 | 0x063 | CHECK_ACTIONNEURS_AVANT | TX | Check act. avant |
| 100 | 0x064 | CHECK_ACTIONNEURS_ARRIERE | TX | Check act. arrière |
| 101 | 0x065 | CHECK_AX12 | TX | Check servos |
| 102 | 0x066 | CHECK_OK_TELEMETRE | TX | Check télémètre |
| 112 | 0x070 | ALIVE_BALISE | RX | Heartbeat balise |
| 113 | 0x071 | ALIVE_MOTEUR | RX | Heartbeat moteur |
| 114 | 0x072 | ALIVE_IHM | RX | Heartbeat IHM |
| 115 | 0x073 | ALIVE_ACTIONNEURS_AVANT | RX | Heartbeat act. avant |
| 116 | 0x074 | ALIVE_ACTIONNEURS_ARRIERE | RX | Heartbeat act. arrière |
| 117 | 0x075 | ALIVE_HERKULEX | RX | Heartbeat servos |
| 118 | 0x076 | ALIVE_TELEMETRE | RX | Heartbeat télémètre |

### Actionneurs (0x070-0x099)

| ID | Hex | Nom | Dir | Len | Description |
|----|-----|-----|-----|-----|-------------|
| 112 | 0x070 | IDCAN_PINCE | TX | 3 | Pince avant |
| 113 | 0x071 | IDCAN_PINCE_CHANGE_ID_HERKULEX | TX | - | Changer ID servo pince |
| 114 | 0x072 | INSTRUCTION_END_STEP_MOTEUR | RX | 2 | Stepper terminé |
| 115 | 0x073 | INSTRUCTION_END_HERKULEX | RX | 2 | Herkulex terminé |
| 116 | 0x074 | INSTRUCTION_END_Actionneur | RX | 2 | Actionneur terminé |
| 117 | 0x075 | IDCAN_PINCE_ARRIERE | TX | 3 | Pince arrière |
| 118 | 0x076 | IDCAN_ASPIRATEUR | TX | 0 | Aspirateur |
| 130 | 0x082 | IDCAN_POS_XY_OBJET | RX | 7 | Position objet LIDAR |
| 144 | 0x090 | MONTER_IMMEUBLE_DOUBLE / IDCAN_Construire_avant | TX | 0 | Construire avant |
| 145 | 0x091 | MONTER_IMMEUBLE / IDCAN_Construire_arriere | TX | 0 | Construire arrière |
| 146 | 0x092 | IDCAN_NIV_BASE_AV | TX | 2 | Niveaux base avant |
| 147 | 0x093 | IDCAN_NIV_BASE_ARR | TX | 2 | Niveaux base arrière |
| 151 | 0x097 | IDCAN_SET_SCORE | RX | 2 | Mise à jour score |
| 153 | 0x099 | ACK_ACTION | RX | 2 | ACK action complexe |

### Acknowledges (0x100-0x116)

| ID | Hex | Nom | Dir | Len | Description |
|----|-----|-----|-----|-----|-------------|
| 256 | 0x100 | ACKNOWLEDGE_BALISE | RX | 2 | ACK balise |
| 257 | 0x101 | ACKNOWLEDGE_MOTEUR | RX | 2 | ACK moteur |
| 258 | 0x102 | ACKNOWLEDGE_IHM | RX | 2 | ACK IHM |
| 259 | 0x103 | ACKNOWLEDGE_ACTIONNEURS | RX | 2 | ACK actionneurs |
| 261 | 0x105 | ACKNOWLEDGE_TELEMETRE | RX | 2 | ACK télémètre |
| 262 | 0x106 | ACKNOWLEDGE_HERKULEX | RX | 2 | ACK servos |
| 264 | 0x108 | ACKNOWLEDGE_CAMERA | RX | 2 | ACK caméra |
| 266 | 0x10A | ACKNOWLEDGE_STRAT | RX | 2 | ACK stratégie |
| 272 | 0x110 | INSTRUCTION_END_BALISE | RX | 2 | Fin instruction balise |
| 273 | 0x111 | INSTRUCTION_END_MOTEUR | RX | 2 | Fin instruction moteur |
| 274 | 0x112 | INSTRUCTION_END_IHM | RX | 2 | Fin instruction IHM |
| 275 | 0x113 | INSTRUCTION_END_ACTIONNEURS | RX | 2 | Fin instruction act. |
| 278 | 0x116 | ACK_FIN_ACTION | RX | 2 | Fin action |

### Actions spécifiques jeu (0x170-0x179, 0x255-0x27A)

| ID | Hex | Nom | Dir | Len | Description |
|----|-----|-----|-----|-----|-------------|
| 368 | 0x170 | CHASSE_NEIGE | TX | 0 | Chasse-neige |
| 369 | 0x171 | saisir_avant | TX | 0 | Saisir avant |
| 370 | 0x172 | IDCAN_LACHE_AV | TX | 2 | Lâcher avant |
| 371 | 0x173 | IDCAN_LACHE_AR | TX | 2 | Lâcher arrière |
| 372 | 0x174 | saisir_arriere | TX | 0 | Saisir arrière |
| 373 | 0x175 | PRISE_BORDURE | TX | 0 | Prise bordure |
| 374 | 0x176 | POSE_BORDURE | TX | 0 | Pose bordure |
| 375 | 0x177 | PRISE_DISTRIBUTEUR | TX | 0 | Prise distributeur |
| 376 | 0x178 | RANGEMENT | TX | 0 | Position rangement |
| 377 | 0x179 | GESTION_TORQUE | TX | 0 | Gestion couple |
| 503 | 0x1F7 | ASSERVISSEMENT_ENABLE | TX | 1 | Activer/désactiver asserv |
| 592 | 0x250 | capteur_avant | TX | 0 | Capteur avant |
| 593 | 0x251 | capteur_arriere | TX | 0 | Capteur arrière |
| 594 | 0x252 | TEST_BRAS_C | TX | 0 | Test bras C |
| 595 | 0x253 | TEST_BRAS_D | TX | 0 | Test bras D |
| 596 | 0x254 | TEST_BRAS_1 | TX | 0 | Test bras 1 |
| 597 | 0x255 | Pompe_av | TX | 0 | Pompe avant |
| 598 | 0x256 | Pompe_ar | TX | 0 | Pompe arrière |
| 599 | 0x257 | DEP_banderole | TX | 0 | Déployer banderole |
| 600 | 0x258 | POS_avant | TX | 0 | Position init avant |
| 601 | 0x259 | POS_arriere | TX | 0 | Position init arrière |
| 608 | 0x260 | BRAS_AT | TX | 0 | Bras attaque |
| 609 | 0x261 | BRAS_RE | TX | 0 | Bras retour |
| 610 | 0x262 | AUTOMATE_MANCHE_HAUT | TX | 0 | Manche à air haut |
| 611 | 0x263 | AUTOMATE_MANCHE_MOY | TX | 0 | Manche à air moyen |
| 612 | 0x264 | AUTOMATE_MANCHE_BAS | TX | 0 | Manche à air bas |
| 613 | 0x265 | PAVILLON_DEPLOYE | TX | 0 | Déployer pavillon |
| 614 | 0x266 | VENT_AV | TX | 0 | Ventouse avant |
| 615 | 0x267 | VENT_AR | TX | 0 | Ventouse arrière |
| 616 | 0x268 | TEST_VENT_1_ON | TX | 0 | Test ventouse 1 ON |
| 617 | 0x269 | TEST_VENT_1_OFF | TX | 0 | Test ventouse 1 OFF |
| 618 | 0x26A | GABARIT_ROBOT | TX | 0 | Gabarit robot |
| 624 | 0x270 | BRAS_PREPA | TX | 0 | Bras préparation |
| 625 | 0x271 | BRAS_POSE | TX | 0 | Bras pose |
| 634 | 0x27A | VENT_ETAT | RX | - | État ventouse |
| 645 | 0x285 | ATTENDRE | TX | 0 | Attendre |

### Capteurs/Télémétrie (0x300, 0x3B0-0x3B9, 0x420-0x422)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 768 | 0x300 | ARRIVER_BON_PORT | RX | Arrivé au port |
| 944 | 0x3B0 | DATA_TELEMETRE | TX | Requête télémètre |
| 945 | 0x3B1 | RECEPTION_DATA | RX | Réponse télémètre |
| 946 | 0x3B2 | TELEMETRE_OBJET | RX | Détection objet |
| 947 | 0x3B3 | OBJET_SUR_TABLE | RX | Objet sur table |
| 949 | 0x3B5 | RECEPTION_RECALAGE | RX | Données recalage |
| 950 | 0x3B6 | DATA_RECALAGE | TX | Requête données recalage |
| 951 | 0x3B7 | LIRE_PANNEAU | TX | Lire panneau |
| 952 | 0x3B8 | VIBRO | RX | Capteur vibration |
| 953 | 0x3B9 | DATA_TELEMETRE_LOGIQUE | TX | Télémètre logique |
| 1056 | 0x420 | LECTURE_GIROUETTE | TX | Lire girouette |
| 1057 | 0x421 | VALEUR_GIROUETTE | RX | Valeur girouette |
| 1058 | 0x422 | TEST_LECTURE_GIROUETTE | TX | Test girouette |

### Audio (0x4A0-0x4AC)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 1184 | 0x4A0 | SOMO_answers | RX | Réponse module SOMO |
| 1185 | 0x4A1 | SOMO_playtrack | TX | Jouer piste |
| 1186 | 0x4A2 | SOMO_playalltracks | TX | Jouer toutes pistes |
| 1187 | 0x4A3 | SOMO_stop | TX | Arrêter lecture |
| 1188 | 0x4A4 | SOMO_volume | TX | Volume |
| 1189 | 0x4A5 | SOMO_pause | TX | Pause |
| 1190 | 0x4A6 | SOMO_resume | TX | Reprendre |
| 1191 | 0x4A7 | SOMO_reset | TX | Reset |
| 1192 | 0x4A8 | SOMO_set_EQ | TX | Égaliseur |
| 1193 | 0x4A9 | SOMO_random_track | TX | Piste aléatoire |
| 1194 | 0x4AA | SOMO_next | TX | Piste suivante |
| 1195 | 0x4AB | SOMO_previous | TX | Piste précédente |
| 1196 | 0x4AC | SOMO_repeat | TX | Mode répétition |

### Debug (0x501-0x522, 0x5A0-0x5C6, 0x602-0x620, 0x710-0x716, 0x760-0x765)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 1281 | 0x501 | ID_FIN_CLOTHO | RX | Fin calcul clothoïde |
| 1296-1314 | 0x510-0x522 | Paramètres clothoïde | RX | Données trajectoire |
| 1440 | 0x5A0 | ERREUR_TEMP_CALCUL | RX | Erreur temps calcul |
| 1441-1449 | 0x5A1-0x5A9 | Debug trajectoire | RX | Debug ligne/courbe |
| 1472-1478 | 0x5C0-0x5C6 | Handling trajectoire | RX | Debug gestion trajectoire |
| 1538 | 0x602 | CHOICE_COLOR | TX | Choix couleur |
| 1539 | 0x603 | RECEPTION_COULEUR | RX | Couleur reçue |
| 1568 | 0x620 | ECRAN_ALL_CHECK | TX/RX | Toutes cartes OK |
| 1795-1797 | 0x703-0x705 | Mécanismes spéciaux | TX | Configs spéciales |
| 1808-1814 | 0x710-0x716 | Config PID | TX | Tuning PID moteur |
| 1888 | 0x760 | DEBUG_STRATEGIE_AUTOMATE | TX | Debug état stratégie |
| 1889 | 0x761 | DEBUG_FAKE_JAKE | TX/RX | Fake jack debug |
| 1890 | 0x762 | DEBUG_ASSERV | TX | Debug asserv |
| 1893 | 0x765 | simulateur | TX | Mode simulateur |

### Erreurs (0x040-0x043, 0x785-0x789)

| ID | Hex | Nom | Dir | Description |
|----|-----|-----|-----|-------------|
| 64 | 0x040 | ERROR_OVERFLOW_BALISE | RX | Overflow balise |
| 65 | 0x041 | ERROR_OVERFLOW_MOTEUR | RX | Overflow moteur |
| 66 | 0x042 | ERROR_OVERFLOW_IHM | RX | Overflow IHM |
| 67 | 0x043 | ERROR_OVERFLOW_STRAT | RX | Overflow stratégie |
| 1925 | 0x785 | ERROR_BALISE | RX | Erreur balise |
| 1926 | 0x786 | ERROR_RTC | RX | Erreur RTC |
| 1927 | 0x787 | ERROR_MOTEUR | RX | Erreur moteur |
| 1928 | 0x788 | ERROR_TELEMETRIE | RX | Erreur télémétrie |
| 1929 | 0x789 | ERROR_STRATEGIE | RX | Erreur stratégie |

---

## Encodage des données

### Convention Little Endian

Tous les entiers multi-octets utilisent l'ordre **Little Endian** (LSB d'abord).

**Exemples**:

```cpp
// uint16_t → octets
uint16_t value = 1000;  // 0x03E8
data[0] = 0xE8;  // LSB
data[1] = 0x03;  // MSB

// Octets → uint16_t
uint16_t value = data[0] | (data[1] << 8);

// int16_t → octets (identique)
int16_t value = -90;  // 0xFFA6
data[0] = 0xA6;
data[1] = 0xFF;

// uint32_t → octets
uint32_t value = 2250;  // 0x000008D2
data[0] = 0xD2;  // LSB
data[1] = 0x08;
data[2] = 0x00;
data[3] = 0x00;  // MSB
```

### Unités et conversions

| Donnée | Unité CAN | Unité réelle | Conversion |
|--------|-----------|--------------|------------|
| Position X/Y | uint16_t | mm | Directe |
| Distance | int16_t | mm | Directe (négatif=arrière) |
| Angle Theta XYT | int16_t | dixièmes de degré | angle_can = angle_deg × 10 |
| Angle Rotation | int16_t | **degrés** | Directe (exception!) |
| Vitesse | uint16_t | mm/s | Directe |
| Accélération | uint16_t | mm/s² | Directe |
| Position servo | int16_t | Unités servo | Dépend du servo (0-1023 Herkulex) |
| Position stepper | uint32_t | Steps | steps = mm × 45 |
| Playtime | uint8_t | Dizaines de ms | 0x3C = 60 → 600ms |

### Inversion couleur

Lorsque `color == Jaune`, les coordonnées sont symétriques:

```cpp
x = 2000 - x;
theta = 1800 + theta;
if (theta > 1800) theta -= 3600;
else if (theta < -1800) theta += 3600;
```

Cette transformation est appliquée automatiquement dans `strategie.cpp` avant envoi CAN.

---

## Notes importantes

### Timing critique

- **Délai Bézier**: 150ms obligatoire entre trames
- **Timeout ACK**: 1000ms (1s)
- **Timeout completion**: 20000ms (20s) ou infini selon commande
- **Fréquence odométrie**: ~10-50 Hz

### Buffers et overflow

- **Buffer TX/RX**: 100 messages chacun
- **Gestion overflow**: Message le plus ancien écrasé, flag `CAN_MSG_RD_LOST` ou `CAN_MSG_WR_LOST` activé
- **Pas de retry automatique**: Application doit gérer les pertes

### Mode émulation série

Le système peut fonctionner via **USB série à 921600 bauds** au lieu de CAN hardware:

- Framing custom: `0x55` (standard data), `0x5A` (standard remote), `0xA5` (extended data), `0xAA` (extended remote)
- Checksum inclus
- Permet tests sans matériel CAN

### Priorités threads

- **CAN Read/Dispatch**: `osPriorityHigh` (temps réel)
- **CAN Write**: `osPriorityAboveNormal`
- **Main/Strategy**: `osPriorityNormal`

---

## Exemples de séquences complètes

### Séquence match typique

```
1. TX → 0x765 []                         // Mode simulateur
2. TX → 0x002 []                         // Début match
3. TX → 0x008 []                         // Jack retiré
4. TX → 0x022 [E8 03 F4 01]             // Config vitesse 1000 mm/s, accel 500
5. TX → 0x020 [E8 03 F4 01 00 00 01]    // Aller (1000, 500, 0°) avant
6. RX ← 0x101 [20 00]                    // ACK moteur
7. RX ← 0x026 [64 00 32 00 00 00]       // Odométrie (100, 50, 0°)
   ... (odométrie continue)
8. RX ← 0x111 [20 00]                    // Mouvement terminé
9. TX → 0x023 [5A 00]                    // Rotation 90°
10. RX ← 0x101 [23 00]                   // ACK
11. RX ← 0x111 [23 00]                   // Rotation terminée
12. TX → 0x255 []                        // Pompe avant ON
13. RX ← 0x103 [55 02]                   // ACK actionneurs
14. ThisThread::sleep_for(1000ms)        // Attente vacuum
15. TX → 0x092 [03 00]                   // Niveaux base avant = 3
16. RX ← 0x103 [92 00]                   // ACK
17. RX ← 0x074 [92 00]                   // Action terminée
18. TX → 0x004 []                        // Fin match
```

### Test actionneur

```
1. TX → 0x258 []                         // Position init avant
2. RX ← 0x103 [58 02]                    // ACK
3. RX ← 0x074 [58 02]                    // Terminé
4. TX → 0x050 [07 00 00 02 3C 04]       // Servo 7 → pos 512, LED verte
5. RX ← 0x106 [50 00]                    // ACK Herkulex
6. RX ← 0x073 [50 00]                    // Herkulex terminé
```

---

## Fichiers source

**Définitions ID**: `lib/identCrac/identCrac.h`
**Envoi CAN**: `lib/threadCAN/threadCAN.cpp`
**Mouvement**: `lib/deplacement/deplacement.cpp`
**Herkulex**: `lib/herkulex/herkulex.cpp`
**Stratégie**: `lib/strategie/strategie.cpp`
**Instructions**: `lib/instruction/instruction.cpp`

---

**Document généré**: 2025-12-29
**Version protocole**: v0.1
**254 IDs CAN catalogués**
