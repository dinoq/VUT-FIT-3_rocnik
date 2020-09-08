<!-- Autor: Petr Marek (xmarek69), Daniel B�l� (xbilyd01), Jakub Gajdo��k (xgajdo24) -->
<?php

class LoginKontroler extends Kontroler
{
	// Instance controlleru
	protected $kontroler;
	


    // Naparsování URL adresy a vytvoření příslušného controlleru
    public function zpracuj($parametry)
    {

    	$spravcePrihlaseni = new SpravcePrihlaseni();

		$this->data['spatne_heslo'] = 0;
	    if($_POST){
	        if(isset($_POST["username"]) && isset($_POST["password"])){
	        	$id = $spravcePrihlaseni->zkontrolujUzivatele($_POST["username"], $_POST["password"]);
	            if($id != -1){
	                $_SESSION['logged_in'] = true;
	                $_SESSION['id'] = $id;
	                $this->presmeruj('index.php');
	            }else{
	        		$this->data['spatne_heslo'] = 1;
	            }

	        }
	    }  
	    

		$this->pohled[0] = 'login';
		
    }

   

}
?>