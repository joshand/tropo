<?php
include 'common.inc';
include 'config.inc';

$pkid = checkGetAndPost("id");
$ret = ShowForm($dbtype,$dbserver,$dbname,$dbun,$dbpw,$pkid);

$act = checkGetAndPost("action");
if($act=="save") {
	$ret = DoRegister($dbtype,$dbserver,$dbname,$dbun,$dbpw,$pkid);
}

function ShowForm($dbt,$dbs,$dbn,$dbu,$dbp,$pkid) {
	echo "<form method='post' action='doregister.php?action=save'>";
	echo "<input type='hidden' name='id' value='$pkid'>";
	echo "Enter Spark API Key:<br>";
	echo "<input type='text' name='sparkkey'>&nbsp;";
	echo "<input type='submit' value='Save'>";
}

function DoRegister($dbt,$dbs,$dbn,$dbu,$dbp,$pkid) {
	$apikey = checkGetAndPost("sparkkey");

	//echo "--$pkid--<br>--$apikey--";

	if(1==1) {
		$conn = do_db_connect($dbt,$dbs,$dbn,$dbu,$dbp);
		$strSQL = "UPDATE users SET sparktoken = ENCODE('$apikey', '$dbu-$dbp') WHERE pkid = '$pkid'";
                $result = do_db_query($conn,$strSQL,0,$dbt,$dbs,$dbn,$dbu,$dbp);
		if($result) {
			echo "<br>Registration Successful";
		} else {
			echo "<br>Registration Failed";
		}
		
		do_db_close($dbt,$conn);
	}
}
?>

