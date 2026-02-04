#ifndef __IDENT_CRAC_H
#define __IDENT_CRAC_H

// ID CRCAC 2026

// Géneral
#define GLOBAL_START 0x002                // Start
#define GLOBAL_GAME_END 0x004             // Stop fin du match
#define RECALAGE_START 0x010              // on a commencé le recalage de début
#define Pos_Init 0x005                    // Fin positionnement robot avant depart
#define GLOBAL_JACK 0x008                 // Jack inséré
#define BALISE_STOP 0x003                 // stop
#define BALISE_DANGER 0xA                 // danger
#define BALISE_END_DANGER 0xB             // fin de danger stop-urgence
#define ATTENDRE 0x285                    // temps en ms à attendre
// ENVOI DE PARAMETRES
#define CHOICE_COLOR 0x602      // Couleur  (0->jaune;1->bleu)
#define RECEPTION_COULEUR 0x603 // Code Couleur
#define ECRAN_ALL_CHECK 0x620   // Carte all check  (Si provient de carte strat => toutes les cartes sont en ligne, Si provient IHM => forcer le lancement)
// // Constantes de temps
#define PLAYTIME 0x321 // temps de jeu en secondes
#define TEMPS_VENTOUSE 0x320 // temps de pompe en ms
// Simulation
#define simulateur 0x765
//  ACTION 2026
#define noisetteB 0x022
#define noisetteJ 0x023
#define prendreG 0x171
#define prendreD 0x172
#define poserG 0x026
#define poserD 0x027
#define trier 0x028
#define vent_on 0x268
#define vent_off 0x269
#define vent_etat 0x27A
#define VENT_G 0x266
#define VENT_D 0x267
// Asservissement
#define ASSERVISSEMENT_ENABLE 0x1F7     // Activation asservissement  (0 : désactivation, 1 : activation)
#define ASSERVISSEMENT_XYT_ROTATE 0x037 // Asservissement position XYT avec rotation initiale
#define ASSERVISSEMENT_LINE 0x038       // Asservissement position XYT ligne droite
#define ASSERVISSEMENT_GO_TO_XYT 0x039 // Asservissement position XYZ (x en mm, y en mm,theta en degré)
#define ASSERVISSEMENT_COURBURE 0x021   // Asservissement rayon de courbure  (+ gauche, - droite , sens : 1avt , -1arr; enchainement => 1 oui, 0 => non, 2=>derniére instruction de l'enchainement)
#define ASSERVISSEMENT_RECALAGE 0x024   // Moteur tout droit  (recalage : 0 mouvement seul, 1 x, 2y valeur : coordonnée à laquelle est recalé x/y; enchainement => 1 oui, 0 => non)
#define ASSERVISSEMENT_STOP 0x001       // Stop moteur
#define ASSERVISSEMENT_BEZIER 0x02A     // Asservissement courbe de bezier
#define ASSERVISSEMENT_CONFIG 0x022     // Asservissement paramètre
#define ASSERVISSEMENT_CONFIG_VIT 0x011   // Vitesse max
#define ASSERVISSEMENT_CONFIG_ACCEL 0x012   // Accélération max
#define ASSERVISSEMENT_SPEED_DANGER 0x006      // Vitesse en danger
#define ASSERVISSEMENT_INFO_CONSIGNE 0x1F0     // Info Consigne et Commande moteur
#define ASSERVISSEMENT_CONFIG_KPP_DROITE 0x1F1 // Config coef KPP_Droit
#define ASSERVISSEMENT_CONFIG_KPI_DROITE 0x1F2 // Config coef KPI_Droit
#define ASSERVISSEMENT_CONFIG_KPD_DROITE 0x1F3 // Config coef KPD_Droit
#define ASSERVISSEMENT_CONFIG_KPP_GAUCHE 0x1F4 // Config coef KPP_Gauche
#define ASSERVISSEMENT_CONFIG_KPI_GAUCHE 0x1F5 // Config coef KPI_Gauche
#define ASSERVISSEMENT_CONFIG_KPD_GAUCHE 0x1F6 // Config coef KPD_Gauche
#define ASSERVISSEMENT_CONFIG_KPP 0x710        // Config coef KPP
#define ASSERVISSEMENT_CONFIG_KPI 0x711        // Config coef KPI
#define ASSERVISSEMENT_CONFIG_KPD 0x712        // Config coef KPD
#define ASSERVISSEMENT_CONFIG_LARG 0x713     // Largeur entre les roues
#define ASSERVISSEMENT_CONFIG_PERIM 0x714   // Périmètre des roues
#define ASSERVISSEMENT_CONFIG_COEF_G 0x715  // Config coef de conversion des tics en mm pour la roue gauche
#define ASSERVISSEMENT_CONFIG_COEF_D 0x716          // Config coef de conversion des tics en mm pour la roue droite
#define ASSERVISSEMENT_ERREUR 0x025     // Erreur asservissement  (Indique la raison de l'arrêt de l'asservissement)po
#define ASSERVISSEMENT_ERROR_MOTEUR 0x015 // robot s'arrete car percuté quelque chose
#define ASSERVISSEMENT_XYT 0x020  // Asservissement (x,y,theta)  (0 : au choix 1 : avant -1 : arrière)
// ACK
#define ACKNOWLEDGE_BALISE 0x100      // Acknowledge balise
#define ACKNOWLEDGE_MOTEUR 0x101      // Acknowledge moteur
#define ACKNOWLEDGE_IHM 0x102         // Acknowledge ecran tactile
#define ACKNOWLEDGE_ACTIONNEURS 0x103 // Acknowledge actionneurs
#define ACKNOWLEDGE_TELEMETRE 0x105   // Acknowledge telemetre
#define ACKNOWLEDGE_HERKULEX 0x106    // Ack servo
#define ACKNOWLEDGE_VENTOUSES 0x10A       // Acknowledge pompes
#define ACKNOWLEDGE_CAMERA 0x108      // Acknowledge couleur caméra
#define ACK_ACTION 0x99               // autre action possible via les herkulex, ne peut pas passer en sendrawid
#define ACKNOWLEDGE_JACK 0X009        // Ack Jack inséré
#define ACK_FIN_ACTION 0x116              // Fin instruction actionneurs  (Indique que l'instruction est terminée)
// FIN INSTRUCTION
#define INSTRUCTION_END_BALISE 0x110      // Fin instruction balise  (Indique que l'instruction est terminée)
#define INSTRUCTION_END_MOTEUR 0x111      // Fin instruction moteur  (Indique que l'instruction est terminée)
#define INSTRUCTION_END_IHM 0x112         // Fin instruction ecran tactile  (Indique que l'instruction est terminée)
#define INSTRUCTION_END_ACTIONNEURS 0x113 // Fin instruction actionneurs  (Indique que l'instruction est terminée)
// Odométrie
#define OdoG 0x026   // Odométrie position robot gauche
#define OdoD 0x027 // Odométrie position robot droite
#define Odo_request 0x028 // Demande odométrie  (Demande la position actuel du robot)
// #define ODOMETRIE_BIG_VITESSE 0x029  // Odométrie vitesse  (Indication sur l'état actuel)
// #define ODOMETRIE_SMALL_VITESSE 0027  // Odométrie vitesse  (Indication sur l'état actuel)
// #define ODOMETRIE_BIG_POSITION 0x028   // Odométrie position robot  (Position actuel du robot)
// #define ODOMETRIE_SMALL_POSITION 0x026 // Odométrie position robot  (Position actuel du robot)
//Débug asserv
#define ID_FIN_CLOTHO 0x501
#define ID_ENTRAXE 0x510
#define ID_RAYON 0x511
#define ID_ALPHA 0x512
#define ID_VITESSE 0x513
#define ID_ACCELERATION 0x514
#define ID_TCLOTHO 0x515
#define ID_TARC 0x516
#define ID_TEMPS 0x517
#define ID_VIT 0x518
#define ID_VIT1 0x519
#define ID_POS 0x520
#define ID_POS1 0x521
#define ID_T_CALCUL 0x522
#define ERREUR_TEMP_CALCUL 0x5A0
#define ID_DBUG_ETAT 0x5A1
#define ID_DBUG_ETAT_DPL 0x5A2
#define ID_DBUG_LIGNE_TPS 0x5A3
#define ID_DBUG_LIGNE_PCONS 0x5A4
#define ID_DBUG_LIGNE_VIT 0x5A5
#define ID_DIST_TIC_GENE 0x5A6
#define ID_TEMPS_CALCUL_CLOTHO 0x5A7
#define ID_DBUG_LIGNE_GENE_VIT 0x5A8
#define ID_CLOTHO_IMPOSSIBLE 0x5A9
#define ID_TRAIT_LIGNE_GENE 0x5C0
#define ID_TRAIT_CLOTHO 0x5C1
#define ID_TRAIT 0x5C2
#define ID_TEMPS_LONG_1 0x5C3
#define ID_TEMPS_LONG_2 0x5C4
#define TEST_BRAS_1 0x254
#define ID_TEST_VITESSE 0x5C5
#define ID_REACLLAGE_AVANT 0x5C6
// Reset cartes
#define RESET_BALISE 0x030      // Reset balise
#define RESET_MOTEUR 0x031      // Reset moteur
#define RESET_IHM 0x032         // Reset écran tactile
#define RESET_ACTIONNEURS 0x033 // Reset actionneurs
#define RESET_VENTOUSES 0x034      // Reset pompes
#define RESET_AX12 0x035        // Reset AX12
#define RESET_TELEMETRE 0x036   // Reset telemetre
#define RESET_STRAT 0x3A        // Reset stratégie
// Erreurs
#define ERROR_OVERFLOW_BALISE 0x040 // Overflow odométrie
#define ERROR_OVERFLOW_MOTEUR 0x041 // Overflow asservissement
#define ERROR_OVERFLOW_IHM 0x042    // Overflow balise
#define ERROR_OVERFLOW_STRAT 0x043  // Overflow stratégie
#define ERROR_BALISE 0x785          // Bug balise
#define ERROR_RTC 0x786             // Bug RTC
#define ERROR_MOTEUR 0x787          // Bug moteur
#define ERROR_TELEMETRIE 0x788      // Bug télémètre
#define ERROR_STRATEGIE 0x789       // Bug stratégie
// Lidar
#define IDCAN_POS_XY_OBJET 0x82      // Position XY d'un objet détecté par le LIDAR

// A changer ?

//Capteurs
//Debug
//Son




//-----------------------------------define bras/ventouse 2022 (valeur)--------------------------//

// Serial carte interface
#define SERIAL_GAUCHE 2  // bras 0 et 5
#define SERIAL_MILIEU 3  // bras 1 et 4
#define SERIAL_DROITE 4  // bras 2 et 3
#define SERIAL_SPECIAL 5 // bras mesure et chasse neige
// 1 serial est cassé aléatoirement sur chaque carte
/* Correspondance serial --> RX-TX sur la carte
serial1 : RX1-TX1
serial2 : RX5-TX5
serial3 : RX3-TX3
serial4 : RX6-TX6
serial5 : RX4-TX4*/
// ID servo bras Bas Avant (BAV)
#define BAV_BASE 1
#define BAV_MILIEU 2
#define BAV_HAUT 3
// ID servo bras Bas Arriere (BAR)
#define BAR_BASE 4
#define BAR_MILIEU 5
#define BAR_HAUT 6
// ID servo bras Haut Avant (HAV)
#define HAV_BASE 7
#define HAV_MILIEU 8
#define HAV_HAUT 9
// ID servo bras Haut Arriere (HAR)
#define HAR_BASE 10
#define HAR_MILIEU 11
#define HAR_HAUT 12
// ID servo bras spéciaux
#define SPE_GAUCHE_BASE 101
#define SPE_GAUCHE_HAUT 102
#define SPE_DROITE_BASE 103
#define SPE_DROITE_HAUT 104
#define CN_GAUCHE_AV 5
#define CN_DROITE_AV 6
#define CN_GAUCHE_AR 7
#define CN_DROITE_AR 8
#define BF_RANGER 0x156
#define BF_PRETEST 0x154
#define BF_MESURE 0x155
#define BF_COLOR 0x703
#define BF_POS 0x704
#define BF_TEST 0x705
#define ID_HAUT 104
#define ID_MILLIEU 103
#define CHASSE_NEIGE 0x170
#define PRISE_BORDURE 0x175
#define POSE_BORDURE 0x176
#define PRISE_DISTRIBUTEUR 0x177
#define RANGEMENT 0x178
#define GESTION_TORQUE 0x179
#define saisir_avant 0x171
#define saisir_arriere 0x174
//-----------------------------------define actions manche à air et pavillon---------------------------//
#define AUTOMATE_MANCHE_HAUT 0x262
#define AUTOMATE_MANCHE_MOY 0x263
#define AUTOMATE_MANCHE_BAS 0x264
#define PAVILLON_DEPLOYE 0x265

//-----------------------------------define lecture girouette et bon port--------------------------//
#define ARRIVER_BON_PORT 0x300
#define LECTURE_GIROUETTE 0x420
#define VALEUR_GIROUETTE 0x421
#define TEST_LECTURE_GIROUETTE 0x422

// Check cartes
#define CHECK_BALISE 0x060              // Check balise
#define CHECK_MOTEUR 0x061              // Check moteur
#define CHECK_IHM 0x062                 // Check écran tactile
#define CHECK_ACTIONNEURS 0x063   // Check actionneurs
#define CHECK_STRAT 0x064               // Check stratégie
#define CHECK_AX12 0x065                // Check AX12
#define CHECK_OK_TELEMETRE 0x066        // Check telemetre
// Alive cartes
#define ALIVE_BALISE 0x070              // Alive balise
#define ALIVE_MOTEUR 0x071              // Alive moteur
#define ALIVE_IHM 0x072                 // Alive écran tactile
#define ALIVE_ACTIONNEURS 0x073   // Alive actionneurs
#define ALIVE_ACTIONNEURS_STRAT 0x074 // Alive stratégie
#define ALIVE_HERKULEX 0x075            // Alive AX12
#define ALIVE_TELEMETRE 0x076           // Alive telemetre

// Carte herkulex
#define IDCAN_ACK 0x40
#define IDCAN_HERKULEX 0x50
#define ID_HERKULEX_VITESSE 0x63
#define ID_HERKULEX_TOURNER_NB_TOUR 0x64
#define IDCAN_HERKULEX_ANSWER 0x51
#define IDCAN_HERKULEX_Torque 0x52
#define IDCAN_HERKULEX_GET_ID 0x53
#define IDCAN_HERKULEX_CHANGE_ID 0x54
#define IDCAN_HERKULEX_Clear 0x55
#define IDCAN_HERKULEX_GET_POS 0x56
#define IDCAN_HERKULEX_STAT 0x57
#define IDCAN_HERKULEX_SERIAL_BAUDRATE 0x58
#define IDCAN_HERKULEX_BAUDRATE_OF_HERKULEX 0x59
#define IDCAN_STEP_MOT_POS 0x60
#define IDCAN_STEP_MOT_MODE 0x61
#define IDCAN_PINCE_CHANGE_ID_HERKULEX 0x71
#define IDCAN_PINCE 0x70
#define INSTRUCTION_END_STEP_MOTEUR 0x72
#define INSTRUCTION_END_HERKULEX 0x73
#define INSTRUCTION_END_Actionneur 0x74
#define IDCAN_PINCE_ARRIERE 0x75
#define IDCAN_Construire_avant 0x90
#define IDCAN_Construire_arriere 0x91
#define IDCAN_NIV_BASE_AV 0x92
#define IDCAN_NIV_BASE_ARR 0x93
#define IDCAN_ASPIRATEUR 0x76
// /CAPTEURS
#define DATA_TELEMETRE 0x3B0 // Demande de la valeur d'un des télémètres
#define RECEPTION_DATA 0x3B1 // envoi de la valeur d'un des télémètres
#define TELEMETRE_OBJET 0x3B2
#define OBJET_SUR_TABLE 0x3B3
#define RECEPTION_RECALAGE 0x3B5 // Valeur des télémètres
#define DATA_RECALAGE 0x3B6      // Demande de la valeur de tous les télémètres afin de procèder au récalage
#define LIRE_PANNEAU 0x3B7
#define VIBRO 0x3B8
#define DATA_TELEMETRE_LOGIQUE 0x3B9
#define RECEPTION_TELEMETRE_LOGIQUE 0x3B0
#define capteur_avant 0x250
#define capteur_arriere 0x251
// DEBUGS
#define DEBUG_STRATEGIE_AUTOMATE 0x760 // Etat automate stratégie  (Permet de savoir l'etat de l'automate)
#define DEBUG_FAKE_JAKE 0x761          // Fake jack  (Permet d'outre passerr le JACk du robot)
#define DEBUG_ASSERV 0x762             // Info debug carte moteur
// SOMO
#define SOMO_answers 0x4A0
#define SOMO_playtrack 0x4A1
#define SOMO_playalltracks 0x4A2
#define SOMO_stop 0x4A3
#define SOMO_volume 0x4A4
#define SOMO_pause 0x4A5
#define SOMO_resume 0x4A6
#define SOMO_reset 0x4A7
#define SOMO_set_EQ 0x4A8
#define SOMO_random_track 0x4A9
#define SOMO_next 0x4AA
#define SOMO_previous 0x4AB
#define SOMO_repeat 0x4AC
// jsp
#define IDCAN_LACHE_AV 0x172 // PASSE
#define IDCAN_LACHE_AR 0x173 //  RELACHE_BAS
#define MONTER_IMMEUBLE_DOUBLE 0x090 // Monte deux immeubles selon un code couleur
#define MONTER_IMMEUBLE 0x091        // Monte un immeuble selon un code couleur
#define ACCELERATEUR_INSERTION_AVANT_GAUCHE 0X218
#define ACCELERATEUR_INSERTION_ARRIERE_GAUCHE 0X219

// 2025
#define DEP_banderole 0x257
#define POS_avant 0x258
#define POS_arriere 0x259
#define GABARIT_ROBOT 0x26A
#define BRAS_AT 0x260
#define BRAS_RE 0x261
#define BRAS_POSE 0x271
#define Pompe_av 0x255
#define Pompe_ar 0x256
#define VENT_AV 0x266
#define VENT_AR 0x267


// IDENT NECESSAIRE ANCIENS CODE

#define ALIVE_ACTIONNEURS_AVANT 0x073  // Alive actionneurs
#define ALIVE_ACTIONNEURS_ARRIERE 0x074  // Alive pompes
#define ODOMETRIE_BIG_VITESSE 0x029  // Odométrie vitesse  (Indication sur l'état actuel)
#define ODOMETRIE_SMALL_VITESSE 0x027  // Odométrie vitesse  (Indication sur l'état actuel)
#define ODOMETRIE_BIG_POSITION 0x028  // Odométrie position robot  (Position actuel du robot)
#define ODOMETRIE_SMALL_POSITION 0x026  // Odométrie position robot  (Position actuel du robot)
#define IDCAN_SET_SCORE 0x97
#define ASSERVISSEMENT_ROTATION 0x023  // Asservissement rotation

#endif
