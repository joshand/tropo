<?php
require('tropo-webapi-php-master/tropo.class.php');
include 'common.inc';
include 'config.inc';

$dbt = $dbtype;
$dbs = $dbserver;
$dbn = $dbname;
$dbu = $dbun;
$dbp = $dbpw;

$action = checkGetAndPost("act");
if($action=="") {
	$entityBody = file_get_contents('php://input');
	if((isJson($entityBody)) && (strlen($entityBody) > 0)) {
		$session = new Session();
		$action = $session->getParameters("act");
	}

	if($action=="") { $action = "initial"; }
}

if($action=="sendmsg") {
	//$session = new Session();
	$msg = $session->getParameters("msg");
	$num = $session->getParameters("num");
	if((isset($num)) && ($msg!="")) {
		$tropo = new Tropo(); 
		$tropo->call($num, array('network'=>'SMS')); 
		$tropo->say($msg); 
		return $tropo->RenderJson();
	}
} else if($action=="initial") {
	$msg = checkGetAndPost("msg");
	if($msg=="") { $tropomode = 1; } else { $tropomode = 0; }

	if($tropomode==1) {
		$session = new Session();
		$initialText = $session->getInitialText();
		$tropo = new Tropo();
		$from = $session->getFrom();
		$callerid = $from["id"];
	} else {
		$initialText = $msg;
		$callerid = checkGetAndPost("cid");
	}

	$arr_text = explode(" ",$initialText);
	$conn = do_db_connect($dbt,$dbs,$dbn,$dbu,$dbp);

	if(strtolower($arr_text[0])=="list") {
                $strSQL = "SELECT users.pkid, DECODE(sparktoken, '$dbu-$dbp') AS sparktoken FROM users WHERE (users.usernum = '$callerid')";
                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
                $num = $arr_result[0];
                $result = $arr_result[1];
                if($num>0) {
                        $sparkaccesstoken = do_db_result($dbt,$result,0,"sparktoken");
	        	//Use Spark API to get List of Rooms
	        	$arr_ret = DoGet("https://api.ciscospark.com/v1/rooms","","","Authorization: Bearer $sparkaccesstoken");
	        	$retbody = $arr_ret[0];
	        	$arr_rooms = json_decode($retbody, true);
	        	$roomid = "";
			$ret = "";
			$shortret = "";
	        	for($x=0;$x<count($arr_rooms["items"]);$x++) {
	        	        $roomid = $arr_rooms["items"][$x]["id"];
	        	        $roomtitle = $arr_rooms["items"][$x]["title"];
				//$shortret .= "[$roomtitle]\n";
				if(1==1) {
					$txtlimit = 140;
					$newline = "[$roomtitle] \n";
					if(strlen($shortret) + strlen($newline) > $txtlimit) {
						if($tropomode==1) {
							$tropo->say($shortret . "(" . strlen($shortret) . ")");
							sleep(3);
						}
						$ret .= $shortret;
						$shortret = $newline;
					} else {
	                        	        $shortret .= $newline;
					}
				} else {
	        			$ret .= $newline;
				}
			}
		} else {
			$ret = "Unable to find user";
		}

                if($tropomode==1) {
                        //$tropo->say($ret);
                } else {
                        echo str_replace("\n","<br>",$ret);
                }
	} else if(strtolower($arr_text[0])=="subscribe") {
		$roomloc = strpos($initialText," ");
		$roomname = substr($initialText,$roomloc+1);
		$ret = "Subscribe request for ($roomname)";

                $strSQL = "SELECT users.pkid, DECODE(sparktoken, '$dbu-$dbp') AS sparktoken FROM users WHERE (users.usernum = '$callerid')";
                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
                $num = $arr_result[0];
                $result = $arr_result[1];
                if($num>0) {
                        $sparkaccesstoken = do_db_result($dbt,$result,0,"sparktoken");

			//Use Spark API to get List of Rooms, search room names to find matching room, and get room ID
			$arr_ret = DoGet("https://api.ciscospark.com/v1/rooms","","","Authorization: Bearer $sparkaccesstoken");
			$retbody = $arr_ret[0];
			$arr_rooms = json_decode($retbody, true);
			$roomid = "";
			for($x=0;$x<count($arr_rooms["items"]);$x++) {
				$roomid = $arr_rooms["items"][$x]["id"];
				$roomtitle = $arr_rooms["items"][$x]["title"];
				if($roomtitle==$roomname) {
					break;
				}
			}

			if($roomid=="") {
				$ret = "Unable to find ($roomname)";
			} else {
				$strSQL = "SELECT subscriptions.* FROM subscriptions INNER JOIN users ON (subscriptions.fkuser = users.pkid) WHERE (users.usernum = '$callerid') AND (subscriptions.roomid = '$roomid')";
		                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
		  	        $num = $arr_result[0];
				if($num>0) {
					$ret = "Already Subscribed to ($roomname)";
				} else {
					$userpkid = do_db_result($dbt,$result,0,"pkid");
					$sparkaccesstoken = do_db_result($dbt,$result,0,"sparktoken");
					$strSQL = "SELECT MAX(shortid) AS lastid FROM subscriptions WHERE fkuser = '$userpkid'";
	                                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
        	                        $num = $arr_result[0];
                	                $result = $arr_result[1];
                        	        if($num>0) {
						$newshortid = do_db_result($dbt,$result,0,"lastid");
						$newshortid = $newshortid + 1;
					} else {
						$newshortid = 1;
					}
					//Use Spark API to create Webhook
					$url = "$baseurl/sparkhook.php?act=newmsg";
					$hookdata = array("name" => "Tropo Webhook for $callerid", "targetUrl" => $url,"resource" => "messages","event" => "created","filter" => "roomId=$roomid");
					$hookdata = json_encode($hookdata);
					$arr_ret = DoPost("https://api.ciscospark.com/v1/webhooks",$hookdata,"","application/json","","","Authorization: Bearer $sparkaccesstoken");
					$retbody = $arr_ret[0];
					$arr_hook = json_decode($retbody, true);
					$hookid = $arr_hook["id"];
					$hookstat = $arr_hook["event"];
					if($hookstat=="created") {
						$ret = "Subscribed to ($roomname)";
						$strSQL = "INSERT INTO subscriptions (roomname,roomid,hookid,fkuser,shortid) VALUES ('$roomtitle','$roomid','$hookid','$userpkid',$newshortid)";
	                        		$result = do_db_query($conn,$strSQL,0,$dbt,$dbs,$dbn,$dbu,$dbp);
					} else {
						$ret = "Unable to subscribe to ($roomname)";
					}
				}
			}
		}
	
		if($tropomode==1) {
			$tropo->say($ret);
		} else {
			echo $ret;
		}
	} else if(strtolower($arr_text[0])=="unsubscribe") {
	        $roomloc = strpos($initialText," ");
	        $roomname = substr($initialText,$roomloc+1);
	        $ret = "Unsubscribe request for ($roomname)";

                $strSQL = "SELECT users.pkid, DECODE(sparktoken, '$dbu-$dbp') AS sparktoken FROM users WHERE (users.usernum = '$callerid')";
                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
                $num = $arr_result[0];
                $result = $arr_result[1];
                if($num>0) {
                        $sparkaccesstoken = do_db_result($dbt,$result,0,"sparktoken");

			$strSQL = "SELECT subscriptions.* FROM subscriptions INNER JOIN users ON (subscriptions.fkuser = users.pkid) WHERE (users.usernum = '$callerid') AND (subscriptions.roomname = '$roomname')";
			$arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
			$num = $arr_result[0];
			$result = $arr_result[1];
			if($num>0) {
				$pkid = do_db_result($dbt,$result,0,"pkid");
				$hookid = do_db_result($dbt,$result,0,"hookid");
				$url = "https://api.ciscospark.com/v1/webhooks/$hookid";
				$ret = DoDelete($url,"Authorization: Bearer $sparkaccesstoken");
				if($ret=="1") {
					$ret = "Unsubscribed from ($roomname)";
					$strSQL = "DELETE FROM subscriptions WHERE pkid = '$pkid'";
					$result = do_db_query($conn,$strSQL,0,$dbt,$dbs,$dbn,$dbu,$dbp);
				} else {
					$ret = "Unable to unsubscribe from ($roomname)";
				}
			}
		}

        	if($tropomode==1) {
        	        $tropo->say($ret);
        	} else {
        	        echo $ret;
        	}
	} else if(substr($initialText,0,1)=="@") {
		$numend = strpos($initialText, " ");
		$roomid = substr($initialText, 1, $numend);
		$roomid = hexdec($roomid);
		$textrest = substr($initialText, $numend + 1);

                $strSQL = "SELECT users.pkid, DECODE(sparktoken, '$dbu-$dbp') AS sparktoken FROM users WHERE (users.usernum = '$callerid')";
                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
                $num = $arr_result[0];
                $result = $arr_result[1];
                if($num>0) {
                        $sparkaccesstoken = do_db_result($dbt,$result,0,"sparktoken");

	                $strSQL = "SELECT subscriptions.* FROM subscriptions INNER JOIN users ON (subscriptions.fkuser = users.pkid) WHERE (subscriptions.shortid = '$roomid') AND (users.usernum = '$callerid')";
	                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
	                $num = $arr_result[0];
	                $result = $arr_result[1];
	                if($num>0) {
	                        $roomid = do_db_result($dbt,$result,0,"roomid");

	                	$url = "https://api.ciscospark.com/v1/messages";
	                	$hookdata = array("roomId" => $roomid,"text" => $textrest);
	                	$hookdata = json_encode($hookdata);
	                	$arr_ret = DoPost($url,$hookdata,"","application/json","","","Authorization: Bearer $sparkaccesstoken");
	                	$retbody = $arr_ret[0];
	                	$arr_hook = json_decode($retbody, true);
			}
		}
	} else if(strtolower($arr_text[0])=="register") {
		$headers = "MIME-Version: 1.0" . "\r\n";
		$headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";
		$msghead = "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><title>SMS Gateway Registration</title></head>";
		$emailaddy = $arr_text[1];
                $strSQL = "SELECT * FROM users WHERE email = '$emailaddy'";
                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
                $num = $arr_result[0];
                $result = $arr_result[1];
                if($num<=0) {
			$strSQL = "INSERT INTO users (usernum,email) VALUES ('$callerid','$emailaddy')";
			//$reterr = $strSQL;
			$result = do_db_query($conn,$strSQL,0,$dbt,$dbs,$dbn,$dbu,$dbp);
	                $strSQL = "SELECT * FROM users WHERE email = '$emailaddy'";
	                $arr_result = do_db_query($conn,$strSQL,1,$dbt,$dbs,$dbn,$dbu,$dbp);
	                $num = $arr_result[0];
	                $result = $arr_result[1];
	                if($num>0) {
				$pkid = do_db_result($dbt,$result,0,"pkid");
				$url = "$baseurl/doregister.php?id=$pkid";
				mail($emailaddy,"Spark SMS Gateway Registration","<html>$msghead<body>Click <a href='$url'>here</a> to Register</body></html>",$headers);
				$ret = "Registration email sent";
			} else {
				$ret = "Problem with registration";
			}
		} else {
                        $pkid = do_db_result($dbt,$result,0,"pkid");
                        $url = "$baseurl/doregister.php?id=$pkid";
                        mail($emailaddy,"Spark SMS Gateway Registration","<html>$msghead<body>Click <a href='$url'>here</a> to Register</body></html>",$headers);
			$ret = "Email already registered";
		}
                if($tropomode==1) {
                        $tropo->say($ret);
                } else {
                        echo $ret;
                }
	} else if(strtolower($arr_text[0])=="help") {
		$ret = "1) Text 'register email@domain.com (NOT cisco.com)\n";
		$ret .= "2) Click signup link in email\n";
		$ret .= "3) Enter Spark Access Token and Click Save\n";
		$ret .= "4) Text 'list' to get a list of rooms (format is messy!)\n";
		$ret .= "5) Text 'subscribe roomname' to subscribe to a room\n";
		$ret .= "6) When you receive an update from a room, there is a short code (@x). To respond to a room, text '@x message'\n";
		$ret .= "7) Text 'unsubscribe roomname' to unsubscribe from a room\n";
		if($tropomode==1) {
			$tropo->say($ret);
		} else {
			echo $ret;
		}
	}
	do_db_close($dbt,$conn);

	if($tropomode==1) {
		return $tropo->RenderJson();
	}
}

function isJson($string) {
 json_decode($string);
 return (json_last_error() == JSON_ERROR_NONE);
}
?>
