<?php
/**
 * Created by PhpStorm.
 * User: sir
 * Date: 15/12/14
 * Time: 15:12
 */
class prioriteDev
{
    private static $etats = array('Immédiatement', 'Dans la journée', 'Sous 3 jours', 'Dans la semaine', 'Dans les 2 semaines', 'Dans le mois', 'Sur disponibilité');
    private $etat = 0;

    function __construct($etat = 0)
    {
        $this->etat = $etat;
        return $this;
    }

    /**
     * @return int
     */
    public function getEtat()
    {
        return $this->etat;
    }

    /**
     * @param int $etat
     */
    public function setEtat($etat)
    {
        $this->etat = $etat;
    }

    function getEtatLabel()
    {
        return self::$etats[$this->etat];
    }

}