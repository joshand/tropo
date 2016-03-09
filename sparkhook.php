<?php
require('tropo-webapi-php-master/tropo.class.php');
include 'common.inc';
include 'config.inc';

$dbt = $dbtype;
$dbs = $dbserver;
$dbn = $dbname;
$dbu = $dbun;
$dbp = $dbpw;

$entityBody = file_get_contents('php://input');
$arr_body = json_decode($entityBody, true);
$hookid = $arr_body["id"];
$msgid = $arr_body["data"]["id"];

$conn = do_db_connect($dbt,$dbs,$dbn,$dbu,$dbp);
$strSQL = "SELECT subscriptions.*,users.usernum,DECODE(users.sparktoken, '$dbu-$dbp') AS sparktoken FROM subscriptions INNER JOIN users ON (subscriptions.fkuser = users.pkid) WHERE (subscriptions.hookid = '$hookid')";
$arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
$num = $arr_result[0];
$result = $arr_result[1];
if($num>0) {
	$sparkaccesstoken = do_db_result($dbt,$result,0,"sparktoken");
	$roomid = do_db_result($dbt,$result,0,"shortid");
	$roomid = "@" . dechex($roomid);
	$roomname = do_db_result($dbt,$result,0,"roomname");
	$maxlen = 16;
	if(strlen($roomname) > $maxlen) {
		$roomname = substr($roomname, 0, $maxlen - 3);
		$roomname .= "...";
	}
	$outnum = do_db_result($dbt,$result,0,"usernum");
	if(strlen($outnum)==10) {
		$outnum = "+1" . $outnum;
	} else if(strlen($outnum)==11) {
		$outnum = "+" . $outnum;
	}
}

if($msgid!="") {
	$arr_ret = DoGet("https://api.ciscospark.com/v1/messages/$msgid","","","Authorization: Bearer $sparkaccesstoken"); 
	$body = $arr_ret[0];
	$arr_body = json_decode($body, true);
	$bodytext = $arr_body["text"];
	$bodytext = str_replace("\"","'",$bodytext);
	$bodyfrom = $arr_body["personEmail"];

	$thenum = urlencode($outnum);
	$themsg = urlencode("-=[$roomname ($roomid)]=-\n($bodyfrom): $bodytext");
	$url = "https://api.tropo.com/1.0/sessions?action=create&token=$tropoaccesstoken&num=$thenum&msg=$themsg&act=sendmsg";
	$url = $url;
	$arr_ret = DoGet($url,"","","");
} else {
	//do nothing
}

?>
