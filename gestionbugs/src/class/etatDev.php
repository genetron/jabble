<?php
/**
 * Created by PhpStorm.
 * User: sir
 * Date: 15/12/14
 * Time: 15:14
 */
class etatDev
{
    private static $etats = array('A faire', 'Analyse', 'Développement', 'DEV - Terminé / Test', 'A commiter', 'PROD - Test', 'Résolu');
    private $etat = 0;

    function __construct()
    {
        return $this;
    }

    function suivant()
    {
        if(($this->etat+1) <= (count(self::$etats)-1) )
        {
            $this->etat++;
        }
    }

    function precedent()
    {
        if(($this->etat-1) >= 0)
        {
            $this->etat--;
        }
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