<!-- Autor: Petr Marek (xmarek69), Daniel B�l� (xbilyd01), Jakub Gajdo��k (xgajdo24) -->
<?php

class SkladKontroler extends Kontroler
{
	// Instance controlleru
	protected $kontroler;
	
    public $parametry = null;

    // Naparsov�n� URL adresy a vytvo�en� p��slu�n�ho controlleru
    public function zpracuj($parametry)
    {
        $spravceZbozi = new SpravceZbozi();

        
        $this->data['zmeny_ulozeny'] = 0;
        if(isset($_POST) && (isset($_POST['save']) || isset($_POST['save_x']))){//Ulozit 
            $edit = $spravceZbozi->editujZbozi($_SESSION['id'], $_POST);
	        $_SESSION['zmeny_ulozeny'] = 1;
	    	$this->data['zmeny_ulozeny'] = 1;
			//$this->presmeruj('?akce=sklad');
        }

	    if(isset($_SESSION['zmeny_ulozeny']) && ($_SESSION['zmeny_ulozeny'] == 1)){
	    	$this->data['zmeny_ulozeny'] = 1;
	    	unset($_SESSION['zmeny_ulozeny']);
	    }
        
        $zbozi = $spravceZbozi->vratZbozi($parametry['id']);
        $this->data['zbozi'] = $zbozi;

        $spravceNastaveni = new SpravceNastaveni();
        $nastaveni = $spravceNastaveni->vratNastaveni($parametry['id']);
        $this->data['nastaveni'] = $nastaveni;

        $slova = $spravceZbozi->vratVsechnaSlova($parametry['id']);
        $this->data['slova'] = $slova;

        $nazvy = $spravceZbozi->vratVsechnyNazvy($parametry['id']);
        $this->data['nazvy'] = $nazvy;

        $dodavatele = $spravceZbozi->vratVsechnyDodavatele($parametry['id'], "dodavatel");
        $this->data['dodavatele'] = $dodavatele;

        $mj = $spravceZbozi->vratVsechnyMJ($parametry['id'], "mj");
        $this->data['mj'] = $mj;

        // Nastaven� �ablony
		$this->pohled[0] = 'sklad';
		
    }

   

}
?>