
<?php
// Autor: Petr Marek (xmarek69), Daniel Bílý (xbilyd01), Jakub Gajdo¹ík (xgajdo24) 

// Tøída poskytuje metody pro správu èlánkù v redakèním systému
class SpravcePrihlaseni
{

    // Vrátí seznam v¹ech zbo¾í v databázi
    public function zkontrolujUzivatele($username, $pwd)
    {

        $sql = "SELECT * FROM itu_uzivatel WHERE username=? AND user_password=?";

        $row = Db::dotazJeden($sql, array($username, $pwd));
        if($row){
        	return $row['id_uzivatele'];
        }else{
        	return -1;
        }
    }

    public function createUser($login, $heslo){
        $sql = "INSERT INTO itu_uzivatel (username, user_password) VALUES (?, ?)";
        $row = Db::updateOrInsert($sql, array($login, $heslo));
        $sql = "INSERT INTO itu_nastaveni (id_nastaveni, mnozstvi, dodavatel, cena_nakupni, cena_prodejni, mj, id_uzivatele, nazev) VALUES (NULL , '1', '1', '1', '1', '1', ?, '1')";
        $row = Db::updateOrInsert($sql, array($row));

    
    }

} 