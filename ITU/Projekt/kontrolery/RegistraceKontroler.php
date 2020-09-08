<!-- Autor: Petr Marek (xmarek69), Daniel B�l� (xbilyd01), Jakub Gajdo��k (xgajdo24) -->
<?php

class RegistraceKontroler extends Kontroler
{
	// Instance controlleru
	protected $kontroler;
	


    // Naparsování URL adresy a vytvoření příslušného controlleru
    public function zpracuj($parametry)
    {

    	$spravcePrihlaseni = new SpravcePrihlaseni();

		$this->data['reg'] = 0;
		if(isset($_POST) && isset($_POST['username'])){//Uzivatel odeslal registracni formular ke zpracovani
			//create_new_user_in_table_itu_users($_POST['username'], $_POST['password']);
			$spravcePrihlaseni->createUser($_POST['username'], $_POST['password']);
			session_start();
			$this->data['reg'] = 1;
			$_SESSION['reg'] = 1;
			header("Location: index.php");
		}
		/*
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
	    }  */
	    

		$this->pohled[0] = 'registrace';
		
    }

   

}
?>