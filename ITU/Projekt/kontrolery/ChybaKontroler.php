<!-- Autor: Petr Marek (xmarek69), Daniel B�l� (xbilyd01), Jakub Gajdo��k (xgajdo24) -->
<?php

class ChybaKontroler extends Kontroler
{
    public function zpracuj($parametry)
    {
		// Hlavička požadavku
		header("HTTP/1.0 404 Not Found");
		// Nastavení šablony
		$this->pohled[0] = 'chyba';
    }
}
?>