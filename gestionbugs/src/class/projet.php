<?php
/**
 * Created by PhpStorm.
 * User: sir
 * Date: 15/12/14
 * Time: 15:14
 */
class projet
{
    private $nom = 'nouveau projet';
    private $developpements = array();

    /**
     * @return string
     */
    public function getNom()
    {
        return $this->nom;
    }

    /**
     * @param string $nom
     */
    public function setNom($nom)
    {
        $this->nom = $nom;
    }

    function addDeveloppement($developpement)
    {
        if (is_null($developpement)) {
            $developpement = new developpement();
        }
        $this->developpements[] = $developpement;
        return $developpement;
    }

    function getDeveloppements()
    {
        return $this->developpements;
    }
}