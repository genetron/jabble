<?php
/**
 * Created by PhpStorm.
 * User: sir
 * Date: 15/12/14
 * Time: 15:15
 */
class workspace
{
    private $projets = array();

    function addProjet($projet)
    {
        if(is_null($projet))
        {
            $projet = new projet();
        }
        $this->projets[] = $projet;
        return $projet;
    }

    function getProjets()
    {
        return $this->projets;
    }
}
