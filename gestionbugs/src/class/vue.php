<html>
<head>
    <meta charset="utf-8">
    <title>Gestion Bugs v3</title></head>
<body>
<h1>Gestion Bugs v3</h1>

<?php foreach ($workspaces AS $workspace) : ?>
    <h2><?php echo count($workspace) ?> workspaces</h2>

    <?php foreach ($workspace->getProjets() AS $projet) : ?>
        <h3><?php echo $projet->getNom() ?></h3>

        <?php foreach ($projet->getDeveloppements() AS $developpement) : ?>
            <h4><?php echo $developpement->getDescription() ?></h4>
            <h5><?php echo $developpement->getEtatDev()->getEtatLabel() ?> -
             <?php echo $developpement->getPrioriteDev()->getEtatLabel() ?> -
             <?php echo $developpement->getTypeDev()->getEtatLabel() ?></h5>

            <?php foreach ($developpement->getCommentaires() AS $commentaire) : ?>
                <h5><?php echo count($developpement->getCommentaires()) ?> commentaires</h5>

            <?php endforeach; ?>

            <?php foreach ($developpement->getFichiersModifies() AS $fichierModifie) : ?>
                <h5><?php echo count($developpement->getFichiersModifies()) ?> fichiers modifies</h5>

            <?php endforeach; ?>


        <?php endforeach; ?>

    <?php endforeach; ?>

<?php endforeach; ?>
</body>
</html>