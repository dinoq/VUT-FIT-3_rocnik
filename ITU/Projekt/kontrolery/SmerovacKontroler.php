<!-- Autor: Petr Marek (xmarek69), Daniel B�l� (xbilyd01), Jakub Gajdo��k (xgajdo24) -->
<?php

class SmerovacKontroler extends Kontroler
{
	// Instance controlleru
	protected $kontroler;


    // Naparsov�n� URL adresy a vytvo�en� p��slu�n�ho controlleru
    public function zpracuj($parametry)
    {

    	$this->show_success_login();
		
		$tridaKontroleru = $this->vrat_nazev_kontroleru($parametry);
		
    	if(isset($_SESSION) && isset($_SESSION['logged_in']) && $_SESSION['logged_in'] == true){ // Je li uzivatel prihlasen
			$this->pohled[$this->pohledy_index++] = 'header'; //Zobrazime hlavicku

			if (file_exists('kontrolery/' . $tridaKontroleru . '.php')){// A zavolame prislusny kontroler
				$this->kontroler = new $tridaKontroleru();
        		$this->kontroler->data['id'] = $parametry['id'];
			}else{
				$this->presmeruj('?akce=chyba');
			}
		}else{ // Jinak zavolame kontroler pro prihlaseni
			if(isset($parametry['akce']) && $parametry['akce'] == "registrace"){
				$this->kontroler = new RegistraceKontroler;
			}else{
				$this->kontroler = new LoginKontroler;
			}
        }



		// Vol�n� controlleru
        $this->kontroler->zpracuj($parametry);
		
		// Nastaven� prom�nn�ch pro �ablonu
		/*$this->data['titulek'] = $this->kontroler->hlavicka['titulek'];
		$this->data['popis'] = $this->kontroler->hlavicka['popis'];
		$this->data['klicova_slova'] = $this->kontroler->hlavicka['klicova_slova'];*/
		
		// Nastaven� hlavn� �ablony
		$this->pohled[$this->pohledy_index++] = 'rozlozeni';
		
    }

    private function show_success_login(){    	
        //Pokud se uzivatel spravne vyplnil registracni formular, zobrazime hlasku o uspechu            
        if(isset($_SESSION) && isset($_SESSION['reg']) && $_SESSION['reg'] == true){
            unset($_SESSION['reg']);
            echo "<h2 class='message_for_user'>�sp�n� jste se zaregistroval, nyn� se pros�m p�ihla�te:</h2>";
        }
	}

	private function vrat_nazev_kontroleru($parametry){
		if(isset($parametry['akce'])){
			$pole = explode("_", $parametry['akce']);
			$kontroler = "";
			for($i = 0; $i < count($pole); $i++){
				$kontroler .= ucfirst($pole[$i]);
			}
			$kontroler .= 'Kontroler';
		}else{
			$kontroler = "SkladKontroler";
		}
		return $kontroler;
	}

}
?>