<!-- Autor: Petr Marek (xmarek69), Daniel Bílý (xbilyd01), Jakub Gajdo±ík (xgajdo24) -->
<?php

class ChybaKontroler extends Kontroler
{
    public function zpracuj($parametry)
    {
		// HlaviÄka poÅ¾adavku
		header("HTTP/1.0 404 Not Found");
		// NastavenÃ­ Å¡ablony
		$this->pohled[0] = 'chyba';
    }
}
?>