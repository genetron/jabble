<?php
require_once 'developpement.php';
require_once 'etatDev.php';
require_once 'fichierModifie.php';
require_once 'workspace.php';
require_once 'projet.php';
require_once 'typeDev.php';
require_once 'prioriteDev.php';

$wp = new workspace();
$workspaces[] = $wp;

$newProjet = $wp->addProjet(null);
$newProjet->setNom('sellplace');
$newProjet2 = $wp->addProjet(null);
$newProjet2->setNom('cdiscount');

$newDeveloppement = $newProjet->addDeveloppement(null);
$newDeveloppement2 = $newProjet->addDeveloppement(null);

$newDeveloppement->setDescription('gros debugages');

$newDeveloppement->addFichierModifie(null);

$newDeveloppement->getEtatDev()->suivant();
$newDeveloppement->getEtatDev()->suivant();

require_once 'vue.php';