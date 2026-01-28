#include <ihm.h>
#include <mbed.h>
#include <threadCAN.h>
#include <instruction.h>

int choix_niveau = 0;
couleurDepart hyou;

static lv_obj_t *msgBoxSelection = nullptr;
static lv_obj_t *msgBoxSelectionascenceur = nullptr;

int selected_level = 0;
int selected_ascenceur = 0;

Ihm::Ihm(ThreadLvgl *t)
{
    m_threadLvgl = t;
    msgBoxmatchmessage = nullptr;
    canMessagesTextArea = nullptr;

    t->lock();

    lv_style_init(&styleTitre);
    lv_style_set_text_font(&styleTitre, FONT_LARGE);
    tabView = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 70);
    lv_obj_set_style_text_font(lv_scr_act(), FONT_NORMAL, 0);
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabView);
    lv_obj_t *label = lv_label_create(tab_btns);
    lv_label_set_text(label, "CRAC2026");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 337, 0);

    // Ajout de l'onglet CarteSD
    tabCarteSD = lv_tabview_add_tab(tabView, "CarteSD");
    carteSDInit(tabCarteSD);

    t->unlock();
}

void Ihm::carteSDInit(lv_obj_t *parent)
{
    // Titre
    lv_obj_t *titre = lv_label_create(parent);
    lv_label_set_text(titre, LV_SYMBOL_SD_CARD " Détection Carte SD");
    lv_obj_set_style_text_font(titre, FONT_LARGE, 0);
    lv_obj_align(titre, LV_ALIGN_TOP_MID, 0, 20);

    // Spinner
    spinnerCarteSD = lv_spinner_create(parent, 1000, 60);
    lv_obj_set_size(spinnerCarteSD, 120, 120);
    lv_obj_align(spinnerCarteSD, LV_ALIGN_CENTER, 0, -20);

    // Label de statut principal
    labelCarteSDStatus = lv_label_create(parent);
    lv_label_set_text(labelCarteSDStatus, LV_SYMBOL_REFRESH " En attente de la carte SD...");
    lv_obj_set_style_text_font(labelCarteSDStatus, FONT_NORMAL, 0);
    lv_obj_align(labelCarteSDStatus, LV_ALIGN_CENTER, 0, 80);

    // Label pour le nombre de fichiers
    labelCarteSDFileCount = lv_label_create(parent);
    lv_label_set_text(labelCarteSDFileCount, "");
    lv_obj_set_style_text_font(labelCarteSDFileCount, FONT_NORMAL, 0);
    lv_obj_align(labelCarteSDFileCount, LV_ALIGN_CENTER, 0, 130);
}

void Ihm::updateCarteSDStatus(bool detected, int fileCount)
{
    m_threadLvgl->lock();

    if (detected)
    {
        // Masquer le spinner quand la carte SD est détectée
        lv_obj_add_flag(spinnerCarteSD, LV_OBJ_FLAG_HIDDEN);

        // Mettre à jour le message de statut
        lv_label_set_text(labelCarteSDStatus, LV_SYMBOL_OK " Carte SD détectée");
        lv_obj_set_style_text_color(labelCarteSDStatus, lv_palette_main(LV_PALETTE_GREEN), 0);

        // Afficher le nombre de fichiers
        char buf[100];
        sprintf(buf, LV_SYMBOL_FILE " %d fichier%s trouvé%s",
                fileCount,
                (fileCount > 1) ? "s" : "",
                (fileCount > 1) ? "s" : "");
        lv_label_set_text(labelCarteSDFileCount, buf);
        lv_obj_set_style_text_color(labelCarteSDFileCount, lv_palette_main(LV_PALETTE_BLUE), 0);
    }
    else
    {
        // Afficher le spinner quand la carte SD n'est pas détectée
        lv_obj_clear_flag(spinnerCarteSD, LV_OBJ_FLAG_HIDDEN);

        // Mettre à jour le message de statut
        lv_label_set_text(labelCarteSDStatus, LV_SYMBOL_WARNING " Carte SD non détectée - En attente...");
        lv_obj_set_style_text_color(labelCarteSDStatus, lv_palette_main(LV_PALETTE_ORANGE), 0);

        // Effacer le compteur de fichiers
        lv_label_set_text(labelCarteSDFileCount, "");
    }

    m_threadLvgl->unlock();
}

void Ihm::show(const vector<string> fichiers)
{
    m_threadLvgl->lock();

    // Ajoute un onglet nommé "Show" à l'objet tabView
    tabMatch = lv_tabview_add_tab(tabView, "Show");

    // Déclaration de la variable label utilisée pour les boutons
    lv_obj_t *label;

    // Configuration de la grille
    static lv_coord_t col_dsc[] = {LV_GRID_FR(5), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    static lv_coord_t row_dsc[] = {30, LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    // Création d'un conteneur pour la grille dans l'onglet tabMatch
    lv_obj_t *cont = lv_obj_create(tabMatch);
    lv_obj_center(cont);            // Centre le conteneur dans l'onglet
    lv_obj_update_layout(tabMatch); // Met à jour la disposition pour s'assurer que les dimensions sont correctes

    // Définit la taille du conteneur pour occuper toute la largeur et hauteur disponibles
    lv_obj_set_size(cont, lv_obj_get_content_width(tabMatch), lv_obj_get_content_height(tabMatch));

    // Applique la description de la grille au conteneur (colonnes et lignes définies précédemment)
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    // Création du titre de l'interface
    lv_obj_t *titre = lv_label_create(cont);                          // Crée un label dans le conteneur
    lv_obj_set_style_text_font(titre, &liberation_24, 0);             // Définit la police du texte
    lv_label_set_text(titre, LV_SYMBOL_SD_CARD "Choix Stratégique "); // Définit le texte avec une icône SD card

    // Place le titre dans la grille
    lv_obj_set_grid_cell(titre, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    // Création d'un roller (sélecteur déroulant)
    roller = lv_roller_create(cont);

    // Configure les options du roller avec la liste de fichiers
    matchRollerSetOptions(fichiers, false);

    // Applique la police de texte au roller
    lv_obj_set_style_text_font(roller, &liberation_24, 0);
    lv_roller_set_selected(roller, 1, LV_ANIM_OFF);
    lv_obj_set_grid_cell(roller, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 2);

    couleur = lv_btn_create(cont); // Crée un bouton dans le conteneur
    label = lv_label_create(couleur);
    lv_label_set_text(label, "Couleur");
    lv_obj_add_flag(couleur, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_set_style_bg_color(couleur, lv_color_make(0, 92, 230), LV_STATE_CHECKED);  // Bleu
    lv_obj_set_style_bg_color(couleur, lv_color_make(255, 255, 0), LV_STATE_DEFAULT); // Jaune

    lv_obj_center(label);

    lv_obj_set_grid_cell(couleur, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 2);

    depart = lv_btn_create(cont); // Crée un bouton dans le conteneur

    label = lv_label_create(depart);
    lv_label_set_text(label, "GO");

    lv_obj_set_style_bg_color(depart, lv_palette_main(LV_PALETTE_RED), LV_STATE_DEFAULT);

    lv_obj_center(label);

    lv_obj_set_grid_cell(depart, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 2, 1);

    // Ajoute un gestionnaire d'événements pour le bouton "GO" lors d'un clic
    lv_obj_add_event_cb(depart, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    m_threadLvgl->unlock();
}

void Ihm::matchRollerSetOptions(const vector<string> fichiers, bool lock)
{
    string choix;
    for (auto f : fichiers)
        choix += "\n" + f;
    if (lock)
        m_threadLvgl->lock();
    lv_roller_set_options(roller, choix.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_tabview_set_act(tabView, 1, LV_ANIM_ON);
    if (lock)
        m_threadLvgl->unlock();
}
void Ihm::ActionneurInit()
{
    m_threadLvgl->lock();

    static lv_coord_t col[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    static lv_coord_t row[] = {30, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    tabActionneur = lv_tabview_add_tab(tabView, "Test Actionneur");
    /*Create a container with grid*/
    lv_obj_t *container = lv_obj_create(tabActionneur);
    lv_obj_center(container);
    lv_obj_update_layout(tabActionneur);
    lv_obj_set_size(container, lv_obj_get_content_width(tabActionneur), lv_obj_get_content_height(tabActionneur));
    lv_obj_set_grid_dsc_array(container, col, row);

    lv_obj_t *titreActionneur = lv_label_create(container);
    lv_obj_set_style_text_font(titreActionneur, &liberation_24, 0);
    lv_label_set_text(titreActionneur, LV_SYMBOL_EDIT " Test/preglage"); // LV_SYMBOL_ENVELOPE LV_SYMBOL_DRIVE LV_SYMBOL_EDIT
    lv_obj_set_grid_cell(titreActionneur, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);

    // Bouton "Pos Init"
    Pos_init = lv_btn_create(container);
    lv_obj_t *label = lv_label_create(Pos_init);
    lv_label_set_text(label, "Pos Init ");
    lv_obj_add_flag(Pos_init, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(Pos_init, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(Pos_init, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_STATE_CHECKED);
    lv_obj_center(label);
    lv_obj_set_grid_cell(Pos_init, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_add_event_cb(Pos_init, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    // Bouton "Test Ventouse"
    testventouse = lv_btn_create(container);
    label = lv_label_create(testventouse);
    lv_label_set_text(label, "Test Ventouse  ");
    lv_obj_add_flag(testventouse, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(testventouse, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(testventouse, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_STATE_CHECKED);
    lv_obj_center(label);
    lv_obj_set_grid_cell(testventouse, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_add_event_cb(testventouse, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    // Zone de texte pour afficher les messages CAN
    canMessagesTextArea = lv_textarea_create(container);
    lv_textarea_set_text(canMessagesTextArea, "Messages CAN:\n");
    lv_textarea_set_cursor_click_pos(canMessagesTextArea, false);
    lv_obj_set_style_text_font(canMessagesTextArea, &liberation_24, 0);
    lv_obj_set_grid_cell(canMessagesTextArea, LV_GRID_ALIGN_STRETCH, 0, 3,
                         LV_GRID_ALIGN_STRETCH, 2, 1);

    // // Bouton "Test Construction"
    // Gradinniveaux2 = lv_btn_create(container);
    // label = lv_label_create(Gradinniveaux2);
    // lv_label_set_text(label, "Test Construction");
    // lv_obj_add_flag(Gradinniveaux2, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_set_style_bg_color(Gradinniveaux2, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(Gradinniveaux2, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_STATE_CHECKED);
    // lv_obj_center(label);
    // lv_obj_set_grid_cell(Gradinniveaux2, LV_GRID_ALIGN_STRETCH, 1, 1,
    //                      LV_GRID_ALIGN_STRETCH, 1, 1);
    // lv_obj_add_event_cb(Gradinniveaux2, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    // // Bouton "Test Niveaux B"
    // niveaux_base = lv_btn_create(container);
    // label = lv_label_create(niveaux_base);
    // lv_label_set_text(label, "Test Niveaux B");
    // lv_obj_add_flag(niveaux_base, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_set_style_bg_color(niveaux_base, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(niveaux_base, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_STATE_CHECKED);
    // lv_obj_center(label);
    // lv_obj_set_grid_cell(niveaux_base, LV_GRID_ALIGN_STRETCH, 1, 1,
    //                      LV_GRID_ALIGN_STRETCH, 2, 1);
    // lv_obj_add_event_cb(niveaux_base, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    // // Bouton "Lâcher"
    // lacher = lv_btn_create(container);
    // label = lv_label_create(lacher);
    // lv_label_set_text(label, " Test Lâcher");
    // lv_obj_add_flag(lacher, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_set_style_bg_color(lacher, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(lacher, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_STATE_CHECKED);
    // lv_obj_center(label);
    // lv_obj_set_grid_cell(lacher, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // lv_obj_add_event_cb(lacher, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    // // Bouton "Autre"
    // autre = lv_btn_create(container);
    // label = lv_label_create(autre);
    // lv_label_set_text(label, "Test jpo");
    // lv_obj_add_flag(autre, LV_OBJ_FLAG_CHECKABLE);
    // lv_obj_set_style_bg_color(autre, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(autre, lv_palette_main(LV_PALETTE_DEEP_PURPLE), LV_STATE_CHECKED);
    // lv_obj_center(label);
    // lv_obj_set_grid_cell(autre, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    // lv_obj_add_event_cb(autre, Ihm::eventHandler, LV_EVENT_CLICKED, this);

    m_threadLvgl->unlock();
}
void Ihm::configRollerSetOptions(const vector<string> fichiers, bool lock)
{
    string choix = "";
    for (auto f : fichiers)
        choix += f + "\n";
    if (choix != "")
        choix.pop_back();
    if (lock)
        m_threadLvgl->lock();
    lv_roller_set_options(configRoller, choix.c_str(), LV_ROLLER_MODE_NORMAL);
    if (lock)
        m_threadLvgl->unlock();
}
void Ihm::configStopPlaying()
{
    lv_state_t etat = lv_obj_get_state(configPlay);
    if (etat & LV_STATE_CHECKED)
    {
        lv_obj_clear_state(configPlay, LV_STATE_CHECKED);
        lv_label_set_text(labelPlay, LV_SYMBOL_PLAY);
    }
}
void Ihm::eventHandler(lv_event_t *e)
{
    Ihm *ihm = static_cast<Ihm *>(lv_event_get_user_data(e));
    lv_obj_t *emetteur = lv_event_get_current_target(e);

    if (emetteur == ihm->testventouse && lv_obj_has_state(emetteur, LV_STATE_CHECKED))
    {
        ihm->flags.set(IHM_FLAG_Testventouse);
    }

    else if (emetteur == ihm->Gradinniveaux2 && lv_obj_has_state(emetteur, LV_STATE_CHECKED))
    {
        ihm->flags.set(IHM_FLAG_Gradin_niveaux_2);
    }
    else if (emetteur == ihm->niveaux_base && lv_obj_has_state(emetteur, LV_STATE_CHECKED))
    {
        ihm->flags.set(IHM_FLAG_Niveaux_base);
    }
    else if (emetteur == ihm->Pos_init && lv_obj_has_state(emetteur, LV_STATE_CHECKED))
    {
        ihm->flags.set(IHM_FLAG__Position_init);
    }
    else if (emetteur == ihm->autre && lv_obj_has_state(emetteur, LV_STATE_CHECKED))
    {
        ihm->flags.set(IHM_FLAG__autre);
    }
    else if (emetteur == ihm->depart)
    {
        ihm->departCouleur = lv_obj_get_state(ihm->couleur);
        ihm->departStrategie = int(lv_roller_get_selected(ihm->roller)) - 1;
        ihm->flags.set(IHM_FLAG_DEPART);
    }

    else if (emetteur == ihm->lacher)
    {
        ihm->flags.set(IHM_FLAG_Lacher);
    }

    else if (emetteur == ihm->msgBoxRecalage && lv_msgbox_get_active_btn_text(emetteur) != nullptr && strcmp(lv_msgbox_get_active_btn_text(emetteur), "Ok") == 0)
    {

        ihm->flags.set(IHM_FLAG__autre);
        lv_msgbox_close(emetteur);
    }
}
bool Ihm::getFlag(IhmFlag f, bool clearIfSet)
{
    if (flags.get() & f)
    {
        if (clearIfSet)
            flags.clear(f);
        return true;
    }
    return false;
}

void Ihm::msgBoxRecalageInit(const string &strategie)
{
    m_threadLvgl->lock();
    static const char *btns[] = {"Ok", ""};
    string str = "Stratégie : " + strategie;
    str += string("\nCouleur : ") + ((departCouleur == 0) ? string("Jaune\n") : string("BLEU\n"));
    msgBoxRecalage = lv_msgbox_create(NULL, "Run\n", str.c_str(), btns, true);
    lv_obj_set_size(msgBoxRecalage, 740, 420);
    lv_obj_center(msgBoxRecalage);
    lv_obj_set_style_bg_color(msgBoxRecalage, (departCouleur == 0) ? lv_color_make(255, 255, 0) : lv_color_make(0, 92, 230), 0);
    lv_obj_set_style_text_color(msgBoxRecalage, lv_color_white(), 0);
    lv_obj_t *boxbtns = lv_msgbox_get_btns(msgBoxRecalage);
    lv_obj_update_layout(msgBoxRecalage);
    lv_obj_set_width(boxbtns, lv_obj_get_content_width(msgBoxRecalage));
    lv_obj_set_height(boxbtns, lv_obj_get_content_height(msgBoxRecalage) - lv_obj_get_y(boxbtns));
    lv_obj_align(boxbtns, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(msgBoxRecalage, Ihm::eventHandler, LV_EVENT_VALUE_CHANGED, this);
    m_threadLvgl->unlock();
}

void Ihm::msgBoxRecalageClose()
{
    m_threadLvgl->lock();
    lv_msgbox_close(msgBoxRecalage);
    msgBoxRecalage = nullptr;
    m_threadLvgl->unlock();
}

void Ihm::msgBoxJackInit()
{
    m_threadLvgl->lock();
    static const char *btns[] = {"Jack simulé", ""};
    msgBoxJack = lv_msgbox_create(NULL, "\n" LV_SYMBOL_WARNING " Attention au départ " LV_SYMBOL_WARNING "\n", "Tirer le jack pour démarrer !\n", btns, true);
    lv_obj_set_size(msgBoxJack, 740, 420);
    lv_obj_center(msgBoxJack);
    lv_obj_t *titre = lv_msgbox_get_title(msgBoxJack);
    lv_obj_set_style_text_color(titre, lv_color_make(255, 0, 0), 0);
    lv_obj_t *boxbtns = lv_msgbox_get_btns(msgBoxJack);
    lv_obj_update_layout(msgBoxJack);
    lv_obj_set_width(boxbtns, lv_obj_get_content_width(msgBoxJack));
    lv_obj_set_height(boxbtns, lv_obj_get_content_height(msgBoxJack) - lv_obj_get_y(boxbtns));
    lv_obj_align(boxbtns, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(msgBoxJack, eventHandler, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(msgBoxJack, eventHandler, LV_EVENT_DELETE, this);
    m_threadLvgl->unlock();
}

void Ihm::msgBoxJackClose()
{
    m_threadLvgl->lock();
    lv_msgbox_close(msgBoxJack);
    jackAnnuleClicked();
    msgBoxJack = nullptr;
    m_threadLvgl->unlock();
}

void Ihm::msgBoxInit(const char *titre, const char *msg, bool boutonAnnuler)
{
    m_threadLvgl->lock();
    static const char *btns[] = {""};
    msgBox = lv_msgbox_create(NULL, titre, msg, btns, boutonAnnuler);
    lv_obj_set_size(msgBox, 740, 420);
    lv_obj_center(msgBox);
    lv_obj_t *titreObj = lv_msgbox_get_title(msgBox);
    lv_obj_set_style_text_color(titreObj, lv_color_make(255, 0, 0), 0);
    lv_obj_add_event_cb(msgBox, eventHandler, LV_EVENT_DELETE, this);
    m_threadLvgl->unlock();
}

void Ihm::msgBoxClose()
{
    m_threadLvgl->lock();
    lv_msgbox_close(msgBox);
    msgBoxCancelClicked();
    msgBox = nullptr;
    m_threadLvgl->unlock();
}

void Ihm::msgBoxMessage(const char *msg)
{
    if (msgBox)
    {
        m_threadLvgl->lock();
        lv_obj_t *txt = lv_msgbox_get_text(msgBox);
        lv_label_set_text(txt, msg);
        m_threadLvgl->unlock();
    }
}

void Ihm::msgBoxmatchshow(const string &strategie)
{
    m_threadLvgl->lock();
    static const char *btns[] = {""};
    string str = "Stratégie : " + strategie;
    str += string("\nCouleur : ") + ((departCouleur == 0) ? string("Jaune\n") : string("BLEU\n"));
    if (departCouleur == 1)
    {
        hyou = BLEU;
    }
    else
    {
        hyou = Jaune;
    }
    msgBoxmatchmessage = lv_msgbox_create(NULL, "Run\n", str.c_str(), btns, true);
    lv_obj_set_size(msgBoxmatchmessage, 740, 420);
    lv_obj_center(msgBoxmatchmessage);
    lv_obj_set_style_bg_color(msgBoxmatchmessage, (departCouleur == 0) ? lv_color_make(255, 255, 0) : lv_color_make(0, 92, 230), 0);
    lv_obj_set_style_text_color(msgBoxmatchmessage, lv_color_white(), 0);
    lv_obj_t *boxbtns = lv_msgbox_get_btns(msgBoxmatchmessage);
    lv_obj_update_layout(msgBoxmatchmessage);
    lv_obj_add_event_cb(msgBoxmatchmessage, Ihm::eventHandler, LV_EVENT_VALUE_CHANGED, this);
    m_threadLvgl->unlock();
}

void Ihm::msgBoxmatchshowclose()
{
    m_threadLvgl->lock();
    if (msgBoxmatchmessage != NULL)
    {
        lv_obj_del(msgBoxmatchmessage);
        msgBoxmatchmessage = NULL;
    }
    m_threadLvgl->unlock();
}

void Ihm::showButtonSelectionBoxClose()
{
    if (msgBoxSelection != NULL)
    {
        lv_obj_del(msgBoxSelection);
        msgBoxSelection = NULL;
    }
}

// Fonction appelée quand un bouton est pressé
static void selection_event_handler(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    const char *btn_txt = lv_msgbox_get_active_btn_text(msgBoxSelection);

    if (strcmp(btn_txt, "1") == 0)
    {
        selected_level = 1;
    }
    else if (strcmp(btn_txt, "2") == 0)
    {
        selected_level = 2;
    }
    else if (strcmp(btn_txt, "3") == 0)
    {
        selected_level = 3;
    }
    else if (strcmp(btn_txt, "4") == 0)
    {
        selected_level = 4;
    }
    else if (strcmp(btn_txt, "Annuler") == 0)
    {
        selected_level = 0;
    }
//     else if (strcmp(btn_txt, "Ventouse bas") == 0)
//     {
//         selected_level = 1;
//     }
//     else if (strcmp(btn_txt, " Ventouse haut") == 0)
//     {
//         selected_level = 2;
//     }
    else if (strcmp(btn_txt, "Toutes") == 0)
    {
        selected_level = 3;
    }
}

// Fonction pour afficher la boîte de sélection
void Ihm::showButtonSelectionBox()
{
    static const char *btns[] = {
        "1", "2", "3", "4", "\n",            // Première ligne
        "Toutes", "Annuler", "" // Deuxième ligne avec dernier élément vide
    };

    msgBoxSelection = lv_msgbox_create(lv_scr_act(), "Sélectionner une  ventouse a tester", "Choisissez un paramètre et attendez un délai de 5s :", btns, true);
    lv_obj_set_size(msgBoxSelection, 740, 420);
    lv_obj_add_event_cb(msgBoxSelection, selection_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    // Centrage
    lv_obj_center(msgBoxSelection);

    // Récupération du conteneur des boutons
    lv_obj_t *btnmatrix = lv_msgbox_get_btns(msgBoxSelection);
    if (btnmatrix)
    {
        lv_obj_set_width(btnmatrix, lv_pct(90));                // Ajuste la largeur à 90% de la msgbox
        lv_obj_set_flex_flow(btnmatrix, LV_FLEX_FLOW_ROW_WRAP); // Organisation en ligne avec retour automatique
        lv_obj_set_style_pad_row(btnmatrix, 20, 0);             // Augmente l'espacement vertical entre les boutons
        lv_obj_set_style_pad_column(btnmatrix, 15, 0);          // Augmente l'espacement horizontal
        lv_obj_set_style_min_width(btnmatrix, 160, 0);          // Augmente la largeur minimale des boutons
        lv_obj_set_style_min_height(btnmatrix, 80, 0);          // Augmente la hauteur minimale des boutons
    }
}

static void selection_ascenseur_event_handler(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    const char *btn_txt = lv_msgbox_get_active_btn_text(msgBoxSelectionascenceur);

    if (strcmp(btn_txt, "Gauche") == 0)
    {
        selected_ascenceur = 1;
    }
    else if (strcmp(btn_txt, "Droite") == 0)
    {
        selected_ascenceur = 2;
    }
    else if (strcmp(btn_txt, "Les Deux") == 0)
    {
        selected_ascenceur = 3;
    }
    else if (strcmp(btn_txt, "Annuler") == 0)
    {
        selected_ascenceur = 4;
    }
}

void Ihm::showButtonascenceurBoxClose()
{
    if (msgBoxSelectionascenceur != NULL)
    {
        lv_obj_del(msgBoxSelectionascenceur);
        msgBoxSelectionascenceur = NULL;
    }
}

void Ihm::showButtonascenceurBox()
{
    static const char *btns[] = {"Gauche", "Droite", "Les Deux", "Annuler", ""}; // Dernier élément vide

    msgBoxSelectionascenceur = lv_msgbox_create(lv_scr_act(), "Sélectionner une partie du robot", "Une fois selectionnée attendre un delai de 5s :", btns, true);
    lv_obj_set_size(msgBoxSelectionascenceur, 740, 420);
    lv_obj_add_event_cb(msgBoxSelectionascenceur, selection_ascenseur_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_center(msgBoxSelectionascenceur);
    // Récupération du conteneur des boutons
    lv_obj_t *btnmatrix = lv_msgbox_get_btns(msgBoxSelectionascenceur);
    if (btnmatrix)
    {
        lv_obj_set_width(btnmatrix, lv_pct(90));                // Ajuste la largeur à 90% de la msgbox
        lv_obj_set_flex_flow(btnmatrix, LV_FLEX_FLOW_ROW_WRAP); // Organisation en ligne avec retour automatique
        lv_obj_set_style_pad_row(btnmatrix, 10, 0);             // Espacement vertical entre les boutons
        lv_obj_set_style_pad_column(btnmatrix, 10, 0);          // Espacement horizontal
    }
}

void Ihm::updateCANMessages(uint32_t id, const uint8_t *data, uint8_t len)
{
    printf("updateCANMessages called: ID=0x%03X\n", (unsigned int)id);

    m_threadLvgl->lock();

    if (canMessagesTextArea != nullptr)
    {
        printf("TextArea is valid, updating...\n");
        // Récupérer le texte actuel
        const char* currentText = lv_textarea_get_text(canMessagesTextArea);

        // Créer le nouveau message formaté
        char newMsg[100];
        int pos = sprintf(newMsg, "ID:0x%03X Len:%d Data:", (unsigned int)id, len);

        for (int i = 0; i < len && i < 8; i++)
        {
            pos += sprintf(newMsg + pos, " %02X", data[i]);
        }
        sprintf(newMsg + pos, "\n");

        // Compter le nombre de lignes
        int lineCount = 0;
        for (const char* p = currentText; *p; p++)
        {
            if (*p == '\n') lineCount++;
        }

        // Si plus de 10 lignes, supprimer la plus ancienne
        if (lineCount > 10)
        {
            const char* secondLine = strchr(currentText, '\n');
            if (secondLine != nullptr)
            {
                secondLine++; // Passer le \n
                lv_textarea_set_text(canMessagesTextArea, secondLine);
            }
        }

        // Ajouter le nouveau message
        lv_textarea_add_text(canMessagesTextArea, newMsg);
        printf("Message added to textarea\n");
    }
    else
    {
        printf("ERROR: canMessagesTextArea is nullptr!\n");
    }

    m_threadLvgl->unlock();
}

