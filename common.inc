<?php
//2015-10-14 : joshand	: Added StripHTML Function (strip_tags wrapper)
//			: Added CleanText Function (removes quotes)
//2016-01-?? : joshand	: Added get_ip_address, DoPost (was sendRequest)
//2016-01-26 : joshand	: Modified DoGet to allow credentials to be optional
//2016-01-27 : joshand	: Added do_db_rowaffect (mysql); Add Header in DoPost.
//			: Added ExtractCookie function; changes to DoGet/DoPost
//2016-01-28 : joshand	: Updated ExtractCookie to fix multi-cookie/; issues
//2016-02-26 : joshand  : Update DoGet/DoPost to allow raw Authorization, added DoDelete

/**
  * Retrieves the best guess of the client's actual IP address.
  * Takes into account numerous HTTP proxy headers due to variations
  * in how different ISPs handle IP addresses in headers between hops.
  */
 function get_ip_address() {
  // Check for shared internet/ISP IP
  if (!empty($_SERVER['HTTP_CLIENT_IP']) && $this->validate_ip($_SERVER['HTTP_CLIENT_IP'])) {
   return $_SERVER['HTTP_CLIENT_IP'];

  // Check for IPs passing through proxies
  if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
   // Check if multiple IP addresses exist in var
    $iplist = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR']);
    foreach ($iplist as $ip) {
     if ($this->validate_ip($ip))
      return $ip;
    }
   }
  }
  if (!empty($_SERVER['HTTP_X_FORWARDED']) && $this->validate_ip($_SERVER['HTTP_X_FORWARDED']))
   return $_SERVER['HTTP_X_FORWARDED'];
  if (!empty($_SERVER['HTTP_X_CLUSTER_CLIENT_IP']) && $this->validate_ip($_SERVER['HTTP_X_CLUSTER_CLIENT_IP']))
   return $_SERVER['HTTP_X_CLUSTER_CLIENT_IP'];
  if (!empty($_SERVER['HTTP_FORWARDED_FOR']) && $this->validate_ip($_SERVER['HTTP_FORWARDED_FOR']))
   return $_SERVER['HTTP_FORWARDED_FOR'];
  if (!empty($_SERVER['HTTP_FORWARDED']) && $this->validate_ip($_SERVER['HTTP_FORWARDED']))
   return $_SERVER['HTTP_FORWARDED'];

  // Return unreliable IP address since all else failed
  return $_SERVER['REMOTE_ADDR'];
 }

 /**
  * Ensures an IP address is both a valid IP address and does not fall within
  * a private network range.
  */

function checkGetAndPost($formval) {
	if (isset($_GET[$formval])) {
		$chkval=$_GET[$formval];
	} elseif (isset($_POST[$formval])) {
		$chkval=$_POST[$formval];
	} else {
		$chkval = "";
	}
	return $chkval;
}

function CheckAccess($dbs,$dbu,$dbp,$dbd,$ul) {
	if ($ul=="0") {
		return 99;
	} else {
		$stracc = CheckLogin(3,$dbs,$dbu,$dbp,$dbd);
		return $stracc;
	}
}

function CheckLogin($strmode,$dbs,$dbu,$dbp,$dbd) {
	if ($uselogins=="0") {
		return -1;
	} else {
		$doredir = 1;
		if (isset($_COOKIE["uid"])) {
			$uguid = $_COOKIE["uid"];

			$myconn=mysql_connect($dbs,$dbu,$dbp);
			@mysql_select_db($dbd) or die("Unable to select database");
			$result=mysql_query("SELECT * FROM Users WHERE uguid = '$uguid'");
			$num=mysql_numrows($result);

			if($num==1) {
				$doredir = 0;
				$suid=mysql_result($result,0,"ID");
				$sfn=mysql_result($result,0,"FName");
				$sln=mysql_result($result,0,"LName");
				$sacc=mysql_result($result,0,"acclvl");
			} else {
				$doredir = 1;
			}
			//mysql_close($myconn);
		} else {
			$doredir = 1;
		}

		if($doredir==1) {
			$uguid = "";
			if(curPageName()!="login.php") {
				header("Location: login.php");
				exit();
			}
		} else {
			switch ($strmode) {
				case 0:
					return $suid;
					break;
				case 1:
					return $sfn;
					break;
				case 2:
					return $sfn . " " . $sln;
					break;
				case 3:
					return $sacc;
					break;
			}
		}
	}
}

function curPageName() {
	return substr($_SERVER["SCRIPT_NAME"],strrpos($_SERVER["SCRIPT_NAME"],"/")+1);
}

function WriteDebug($str_level,$str_text) {
	$debugvar = "debug";

	if ((isset($_GET[$debugvar])) || (isset($_POST[$debugvar]))) {
		setcookie("scheddebug",checkGetAndPost($debugvar));
	}

//echo "WriteDebug1::" . checkGetAndPost($debugvar) . "::" . $str_level . "::";

	if (isset($_COOKIE["scheddebug"])) {
		$debugval = $_COOKIE["scheddebug"];
//echo "WriteDebug2::" . checkGetAndPost($debugvar) . "::" . $debugval . "::" . $str_level . "::";
		if($debugval>=$str_level) {
			echo $str_text;
		}
	} else {
		//nothing
	}
}

function GenerateTableHeader($str_url,$showadd,$strwidth,$headstring,$headnames,$strdesc,$str_extopt) {
	if($strwidth=="") {
		$newwidth="80%";
	} else {
		$newwidth=$strwidth;
	}
	if($showadd == 1) {
		if(strpos($str_url,"?") === false) {
			$urladdon = "?edit=1";
		} else {
			$urladdon = "&edit=1";
		}
		$stradd = "<a target='_self' href='$str_url$urladdon'><font size='-2' color='white'>Add New</font></a>";
	} else {
		$stradd = "";
	}
	//old filter asp code... $strfilter = "Filter: <input name='filter' onkeyup=" & chr(34) & "filter2(this, 'sf', 1)" & chr(34) & " type='text' size='18'>";
	if($str_url == "") {
		$retval = "";
	} else {
		$retval = "<form name='form' method='post' target='_self' action='$str_url' ID='Form1'>\n";
	}
	$retval .= "	<table border=0 cellpadding=0 cellspacing=0 width='$newwidth' align=center>\n";
	$retval .= "		<tr>\n";
	$retval .= "			<td>\n";
	$retval .= "				<table border=1 bordercolor='black' cellpadding=0 cellspacing=0 width='100%' align=center>\n";
	$retval .= "					<tr>\n";
	$retval .= "						<td>\n";
	$retval .= "							<table border=0 cellpadding=5 cellspacing=0 width='100%'>\n";
	$retval .= "								<tr class='tableHeader'>\n";
	if(($stradd=="") && ($str_extopt=="")) {
		$retval .= "							<td width='1%' align='left'></td>\n";
		$retval .= "							<td width='98%' colspan=5 width='100%'>$strdesc</td>\n";
		$retval .= "							<td width='1%' align='left'></td>\n";
	} else {
		$retval .= "							<td width='30%' align='left'>$stradd</td>\n";
		$retval .= "							<td width='30%' colspan=5 width='100%'>$strdesc</td>\n";
		if($str_extopt=="") {
			$retval .= "						<td width='30%' align='right'>&nbsp;</td>\n";
		} else {
			$retval .= "						<td width='30%' align='right'><font size='-2'>$str_extopt</font></td>\n";
		}
	}
	$retval .= "								</tr>\n";
	$retval .= "							</table>\n";
	$retval .= "						</td>\n";
	$retval .= "					</tr>\n";
	$retval .= "				</table>\n";
	$retval .= "				<table border=1 bordercolor='black' bordercolor='black' cellpadding=0 cellspacing=0 width='100%' align=center>\n";
	$retval .= "					<tr>\n";
	$retval .= "						<td>\n";
	$retval .= "							<table id='sf' border=0 cellpadding=5 cellspacing=0 width='100%' align=center>\n";
	$retval .= $headstring;

	if($headnames=="") {
		//skip
	} else {
		$retval .= "							<tr class='tableHeader'>\n";
		$arrhead = explode(",",$headnames);
		foreach ($arrhead as $strhead) {
			$retval .= "						<td align='left'>$strhead</td>\n";
		}
		$retval .= "							</tr>\n";
	}
	return $retval;
}

function GenerateTableFooter() {
	$retval = "";
	$retval .= "							</table>\n";
	$retval .= "						</td>\n";
	$retval .= "					</tr>\n";
	$retval .= "				</table>\n";
	$retval .= "			</td>\n";
	$retval .= "		</tr>\n";
	$retval .= "	</table>\n";
	$retval .= "</form>\n";
	return $retval;
}

function GenerateAddUpdateForm($str_width,$str_url,$str_mode,$str_desc,$dbt,$dbs,$dbu,$dbp,$dbd,$str_table,$str_data) {
	$str_id = checkGetAndPost("aeid");
	if($str_id=="") { $str_id = "-1"; }		// Added for MSSQL
	//echo "--$str_id--<br>";
	$showform = 0;
	$strerr = "";

	if($str_mode==4 || $str_mode==3) {
		$arr_data = explode(";",$str_data);
		$xcount = 0;
		$str_sql_fld = "";
		$str_sql_dta = "";
		$str_sql_udt = "";
		$str_password = "";
		foreach ($arr_data as $str_field) {
			$xcount += 1;
			$arr_field = explode(",",$str_field);
			$str_field_desc = $arr_field[0];
			$str_field_type = $arr_field[1];
			$str_field_dbfn = $arr_field[2];
	
			$str_field_name = "addeditval$xcount";
			//echo "--$str_field_desc--$str_field_type--$str_field_dbfn--$str_field_name--<br>";
			$str_cfpwc = CheckForPWChange($str_id,$str_table,$str_field_dbfn,$str_field_type,$str_field_name,$dbs,$dbu,$dbp,$dbd);
			$str_cfuun = CheckForUniqueUN($str_id,$str_table,$str_field_dbfn,$str_field_type,$str_field_name,$dbs,$dbu,$dbp,$dbd);
			//echo "--$str_field_dbfn--$str_cfpwc--$str_cfuun--<br>";
			if(($str_cfpwc==0) && (($str_cfuun==0) || ($str_cfuun==1))) {							// Not a password field
				$str_field_data = checkGetAndPost($str_field_name);
				$str_sql_fld .= $str_field_dbfn . ",";
				$str_field_data = CheckDataCleanup($str_field_type,$str_field_data);
				$str_sql_dta .= "'" . $str_field_data . "',";
				$str_sql_udt .= "$str_field_dbfn = '$str_field_data',";
			} elseif($str_cfpwc==-1) {				// Password field, but passwords don't match
				$strerr = "<font color='red'>Error: Passwords do not match.</font>";
				$showform = 1;
			} elseif($str_cfpwc==-2) {				// Password field, passwords match, but haven't changed
				//Do Nothing - Password is unchanged
			} elseif($str_cfuun==-1) {				// Password field, but passwords don't match
				$strerr = "<font color='red'>Error: EMail already assigned.</font>";
				$showform = 1;
			} elseif($str_cfpwc==1) {				// Password field, need to hash
				$str_password = checkGetAndPost($str_field_name);
			}
		}
		$str_sql_fld = substr($str_sql_fld, 0, -1);
		$str_sql_dta = substr($str_sql_dta, 0, -1);
		$str_sql_udt = substr($str_sql_udt, 0, -1);

		$str_sql_add = "INSERT INTO $str_table ($str_sql_fld) VALUES ($str_sql_dta)";
		$str_sql_upd = "UPDATE $str_table SET $str_sql_udt WHERE ID = $str_id";
		if($str_mode==4) {
			$str_sql = $str_sql_add;
		} else {
			$str_sql = $str_sql_upd;
		}
		//echo "--$str_mode--$str_sql--<br>\n";

		$conn = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
		$result = do_db_query($conn,$str_sql,0,$dbt,$dbs,$dbd,$dbu,$dbp);

		if($str_mode==4) {
			$str_id = do_db_get_insert_id($dbt,$conn);
		}
		do_db_close($dbt,$conn);

		if($str_password!="") {
			$str_sql = "SELECT * FROM Users WHERE ID = $str_id";
			$conn = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
			//echo "--$str_sql--";
			$arr_result = do_db_query($conn,$str_sql,1,$dbt,$dbs,$dbd,$dbu,$dbp);
			$result = $arr_result[1];
			$num = $arr_result[0];

			//echo "--$num--";
			$hashun = do_db_result($dbt,$result,0,"Email");

			do_db_close($dbt,$conn);

			$newhash = PWHash($hashun,$str_password);

			$str_sql = "UPDATE Users SET PW = '$newhash' WHERE ID = $str_id";

			$conn = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
			$result = do_db_query($conn,$str_sql,0,$dbt,$dbs,$dbd,$dbu,$dbp);
			do_db_close($dbt,$conn);
		}

		if($strerr!="") {
			if($str_mode==3) { $str_mode=2; } else { $str_mode=1; }
		}
	}


	if($str_mode==2) {
		$str_descadd = "Edit";
		$str_btnadd = "Save";
		$str_edittype = "3";
		$str_addeditid = "0";
		$showform = 1;
	} elseif($str_mode==1) {
		$str_descadd = "Create New";
		$str_btnadd = "Add";
		$str_edittype = "4";
		$str_addeditid = "-1";
		$showform = 1;
	} elseif($str_mode==-1) {
		$str_descadd = "";
		$str_btnadd = $str_table;
		$str_edittype = "-2";
		$str_addeditid = "-1";
		$showform = 1;
	} elseif($str_mode==-2) {
		$str_sql = "DELETE FROM $str_table WHERE ID = $str_id";
		$conn = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
		$result = do_db_query($conn,$str_sql,0,$dbt,$dbs,$dbd,$dbu,$dbp);
		do_db_close($dbt,$conn);
		$showform = 0;
	} elseif($str_mode==-3) {
		$str_descadd = "";
		$str_btnadd = $str_table;
		$str_edittype = "-4";
		$str_addeditid = "-1";
		$showform = 1;
	} elseif($str_mode==-4) {
		//Reserved for external use
	}

	if($showform==1) {
		$retval = "";
		if(strpos($str_url,"?") === false) {
			$urladdon = "?edit=$str_edittype";
		} else {
			$urladdon = "&edit=$str_edittype";
		}
		$retval .= "<form name='form' method='post' target='_self' action='$str_url$urladdon' ID='Form1'>\n";
		$retval .= "	<table border=1 bordercolor='black' cellpadding=0 cellspacing=0 align=center width='$str_width'>\n";
		$retval .= "		<tr class='tableHeader'>\n";
		$retval .= "			<td>$str_descadd $str_desc</td>\n";
		$retval .= "		</tr>\n";
		$retval .= "		<tr>\n";
		$retval .= "			<td>\n";
		$retval .= "				<table width='100%'>\n";
		if($strerr!="") {
			$retval .= "					<tr class='tableRow'>\n";
			$retval .= "						<td align='left' colspan='2'>$strerr</td>\n";
			$retval .= "					</tr>\n";
		}

		$arr_data = explode(";",$str_data);
		$xcount = 0;
		$str_sql = "SELECT * FROM $str_table WHERE ID = $str_id";
		$conn = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
		//echo $str_sql;
		$result = do_db_query($conn,$str_sql,0,$dbt,$dbs,$dbd,$dbu,$dbp);

		foreach ($arr_data as $str_field) {
			$xcount += 1;
			$arr_field = explode(",",$str_field);
			$str_field_desc = $arr_field[0];
			$str_field_type = $arr_field[1];
			if(count($arr_field)>2) { $str_field_dbfn = $arr_field[2]; } else { $str_field_dbfn = ""; }

			$str_field_name = "addeditval$xcount";
			if($str_mode==1) {
				$str_field_data = checkGetAndPost($str_field_name);
			} elseif($str_mode==2) {
				//$str_field_data = mysql_result($result,0,$str_field_dbfn);
				//echo "--$xcount--$str_field_dbfn--<br>";
				if($str_field_dbfn!="-1") {
					$str_field_data = do_db_result($dbt,$result,0,$str_field_dbfn);
				} else {
					$str_field_data = "";
				}
				//echo "--$str_field_dbfn--<br>";
			} else {
				$str_field_data = "";
			}

			$retval .= "					<tr class='tableRow'>\n";
			$str_field_control = GenerateField($str_field_name,$str_field_data,$str_field_type,$dbs,$dbu,$dbp,$dbd,$dbt);
			if($str_field_control=="") {
				$retval .= "					<td align='left' colspan='2'>$str_field_desc</td>\n";
				$xcount = $xcount - 1;
			} else {
				if($str_field_desc=="") {
					$retval .= "				<td align='left'></td>\n";
				} else {
					$retval .= "				<td align='left'>$str_field_desc:</td>\n";
				}
				$retval .= "					<td align='left'>$str_field_control</td>\n";
			}
			$retval .= "					</tr>\n";
			//$retval .= "					<tr class='tableRow'>\n";
			//$retval .= "						<td align='left'>Device Type:</td>\n";
			//$retval .= "						<td align='left'>" & CreateList("typedevice", "name", "dtype", str_dtype, 1) & "</td>\n";
			//$retval .= "					</tr>\n";
		}
		do_db_close($dbt,$conn);
		$retval .= "					<tr class='tableRow'>\n";
		$retval .= "						<td align='center' colspan=2>\n";
		$retval .= "							<input type='hidden' name='aeid' value='$str_id'>\n";
		$retval .= "							<input type='submit' name='Submit' value='$str_btnadd' ID='Submit'>\n";
		$retval .= "						</td>\n";
		$retval .= "					</tr>\n";
		$retval .= "				</table>\n";
		$retval .= "			</td>\n";
		$retval .= "		</tr>\n";
		$retval .= "	</table>\n";
		$retval .= "</form>\n";
	} else {
		$retval = "";
	}

	return "<!--$str_id-->$retval";
}

function GenerateField($sfn,$sfd,$sft,$dbs,$dbu,$dbp,$dbd,$dbt) {
	$retval = "";
	$arr_sft = explode(":",$sft);
	//echo "--" . $arr_sft[0] . "--<br>";
	switch ($arr_sft[0]) {
		case 0:					// Text only (don't show passed value)
			$retval = "";
			break;
		case 1:					// Basic Text Box
			if(count($arr_sft)>1) { $str_width = $arr_sft[1]; } else { $str_width=8; }
			$retval = "<INPUT type='text' maxLength='1024' size='$str_width' name='$sfn' value='$sfd'>";
			break;
		case 2:					// Dropdown List (with unbacked data)
			$str_opts = $arr_sft[1];
			$arr_opts = explode(".",$str_opts);

			$retval = "\n<select width='125' style='width: 125px' name='$sfn'>\n";
		        foreach($arr_opts as $oval) {
				if($sfd==$oval) {
					$issel = " SELECTED";
				} else {
					$issel = "";
				}
                		$retval .= "<option value='$oval'$issel>$oval</option>\n";
			}
			$retval .= "</select>\n";
			break;
		case 3:					// Text Area
			$str_opts = $arr_sft[1];
			$arr_opts = explode(".",$str_opts);

			$retval = "\n<textarea wrap='off' cols='$arr_opts[0]' rows='$arr_opts[1]' name='$sfn'>$sfd</textarea>\n";
			break;
		case 4:					// Radio Buttons
			$str_opts = $arr_sft[1];
			$arr_opts = explode(".",$str_opts);

		        foreach($arr_opts as $oval) {
				$arr_opt_sep = explode("~",$oval);

				if($sfd==$arr_opt_sep[1]) {
					$issel = " CHECKED";
				} else {
					$issel = "";
				}
                		$retval .= "<input type='radio' name='$sfn' value='$arr_opt_sep[1]'$issel>$arr_opt_sep[0]\n";
			}
			break;
		case 5:					// Dropdown List (From Database) 5:SELECT * FROM Carriers~ID.Name
			$str_opts = $arr_sft[1];
			$arr_opts = explode("~",$str_opts);

			$retval = "\n<select width='125' style='width: 125px' name='$sfn'>\n";
			$str_sql = $arr_opts[0];
			$myconn2 = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
			$arr_result = do_db_query($myconn2,$str_sql,1,$dbt,$dbs,$dbd,$dbu,$dbp);
			$result = $arr_result[1];
			$num = $arr_result[0];
			//echo "--$num--<br>";

			if($sfd==0) {
				$issel = " SELECTED";
			} else {
				$issel = "";
			}
                	$retval .= "<option value='0'$issel>Make Selection</option>\n";

			$i = 0;
			for($i=0;$i<$num;$i++) {
				$j = 0;
				$arr_opt_sep = explode(".",$arr_opts[1]);
				//$optdesc = "";
				foreach($arr_opt_sep as $oval) {
					//echo "--$i--$oval--<br>";
					if(strpos($oval,"'") === false) {
						$arr_opt_val[$j] = do_db_result($dbt,$result,$i,$oval);
						//$arr_opt_val[$j] = mysql_result($result,$i,$oval);
						//$optdesc .= mysql_result($result,$i,$oval);
					} else {
						$arr_opt_val[$j] = str_replace("'","",$oval);
						//$optdesc .= str_replace("'","",$oval);
					}
					//echo "--$i--$j--$oval--" . $arr_opt_val[$j] . "--<br>";
					$j += 1;
				}

				if($sfd==$arr_opt_val[0]) {
					$issel = " SELECTED";
				} else {
					$issel = "";
				}
				$optdesc = substr_replace(implode("",$arr_opt_val),"",0,strlen($arr_opt_val[0]));
                		$retval .= "<option value='" . $arr_opt_val[0] . "'$issel>" . $optdesc . "</option>\n";			//$arr_opt_val[1]
			}
			$retval .= "</select>\n";
			//mysql_close($myconn2);
			break;
		case 6:					// Basic Text Box for passwords
			if(count($arr_sft)>1) { $str_width = $arr_sft[1]; } else { $str_width=8; }
			$retval = "<INPUT type='password' maxLength='254' size='$str_width' name='$sfn' value='$sfd'>";
			break;
		case 7:					// Password Entry Text Box
			if(count($arr_sft)>1) { $str_width = $arr_sft[1]; } else { $str_width=8; }
			$retval = "<INPUT type='password' maxLength='254' size='$str_width' name='$sfn' value='$sfd'> Verify: <INPUT type='password' maxLength='254' size='$str_width' name='" . $sfn . "verify' value='$sfd'><INPUT type='hidden' maxLength='254' name='" . $sfn . "orig' value='$sfd'>";
			break;
		case 8:					// Basic Text Box - Used when needing to validate unique entry against database
			if(count($arr_sft)>1) { $str_width = $arr_sft[1]; } else { $str_width=8; }
			$retval = "<INPUT type='text' maxLength='254' size='$str_width' name='$sfn' value='$sfd'>";
			break;
		case 9:					// Hidden Text Box
			if(count($arr_sft)>1) { $str_val = $arr_sft[1]; } else { $str_val = ""; }
			if($sfd=="") { $sfd = $str_val; }
			$retval = "<INPUT type='hidden' maxLength='254' name='$sfn' value='$sfd'>";
			break;
		case 10:					// Text only (show passed value)
			$retval = $sfd . "<INPUT type='hidden' maxLength='254' name='$sfn' value='$sfd'>";
			break;
		case 11:					// Text Area for HTML code. Make changes to raw text to support this.
			$str_opts = $arr_sft[1];
			$arr_opts = explode(".",$str_opts);

			//$newsfd = $sfd;
			//$newsfd = str_replace("<","&lt;",$newsfd);
			//$newsfd = str_replace(">","&gt;",$newsfd);
			$retval = "\n<textarea wrap='off' cols='$arr_opts[0]' rows='$arr_opts[1]' name='$sfn'>$sfd</textarea>\n";
			break;
		case 12:
			$str_opts = $arr_sft[1];
			$arr_opts = explode(".",$str_opts);
			$str_sql = $arr_opts[0];
			$myconn2 = do_db_connect($dbt,$dbs,$dbd,$dbu,$dbp);
			$arr_result = do_db_query($myconn2,$str_sql,1,$dbt,$dbs,$dbd,$dbu,$dbp);
			$result = $arr_result[1];
			$num = $arr_result[0];
			//echo "==" . $arr_opts[1] . "==<br>";

			$retval = "<ul id=\"sortable1\" class=\"connectedSortable\">";
			//  <li class="ui-state-default">Item 1</li>
			if($arr_opts[1]!="") {
				$arr_vals = explode("~",$arr_opts[1]);
				for($x=0;$x<count($arr_vals);$x++) {
					$y=$arr_vals[$x]-1;
					$dbval1 = do_db_result($dbt,$result,$y,"Description");
					$dbval2 = do_db_result($dbt,$result,$y,"code");
					$dbval3 = do_db_result($dbt,$result,$y,"ID");
					$dbval2 = CheckDataCleanupRev("11",$dbval2);
					$retval .= "  <li class=\"ui-state-highlight\" id=\"$dbval3\">$dbval2</li>";
				}
				$newsrchstr = "~" . $arr_opts[1] . "~";
			} else {
				$newsrchstr = "";
			}
			
			$retval .= "</ul>";
			$retval .= "<ul id=\"sortable2\" class=\"connectedSortable\">";
 
 			$i = 0;
			for($i=0;$i<$num;$i++) {
				$dbval1 = do_db_result($dbt,$result,$i,"Description");
				$dbval2 = do_db_result($dbt,$result,$i,"code");
				$dbval3 = do_db_result($dbt,$result,$i,"ID");
				if($newsrchstr!="") {
					if(strpos($newsrchstr,"~$dbval3~")===false) {
						$doshow = 1;
					} else {
						$doshow = 0;
					}
				} else {
					$doshow = 1;
				}

				if($doshow==1) {
					$dbval2 = CheckDataCleanupRev("11",$dbval2);
					$retval .= "  <li class=\"ui-state-highlight\" id=\"$dbval3\">$dbval2</li>";
				}
			}

			$retval .= "</ul>";
			$retval .= "<INPUT type='hidden' maxLength='1024' name='$sfn' id='hiddensortfield' value='" . $arr_opts[1] . "'>";
			//mysql_close($myconn2);
			
			//$retval .= "\n<script>";
			//$retval .= "\n$('#sortable2').load(function()";
			//$retval .= "\n{";
			//$retval .= "\n    alert('hi');";
			//$retval .= "\n    $('#sortable2 li').each(function()";
			//$retval .= "\n    {";
			//$retval .= "\n        var \$this = $(this);";
			//$retval .= "\n        \$this.appendTo('#sortable1');";
			//$retval .= "\n        $('#sortable1').sortable('option', 'receive')(null, { item: \$this });";
			//$retval .= "\n    });";
			//$retval .= "\n});";
			//$retval .= "\n</script>";

			break;
	}
	return $retval;
}

function CheckDataCleanup($fld_type, $fld_data) {
	$out_data = $fld_data;
	
	$out_data = str_replace("'","&#39;",$out_data);		// Remove ' from input
	$arr_sft = explode(":",$fld_type);
	if($arr_sft[0]=="11") {
		//echo "==$fld_type==$out_data==<br>";
		$out_data = str_replace("<","&lt;",$out_data);
		$out_data = str_replace(">","&gt;",$out_data);
	}
	return $out_data;
}

function CheckDataCleanupRev($fld_type, $fld_data) {
	$out_data = $fld_data;
	
	$arr_sft = explode(":",$fld_type);
	if($arr_sft[0]=="11") {
		//echo "==$fld_type==$out_data==<br>";
		$out_data = str_replace("&lt;","<",$out_data);
		$out_data = str_replace("&gt;",">",$out_data);
	}
	return $out_data;
}

function CheckForPWChange($sid,$sfd,$sff,$sft,$sfn,$dbs,$dbu,$dbp,$dbd) {
	$arr_sft = explode(":",$sft);
	if($arr_sft[0]==7) {
		if(checkGetAndPost($sfn)==checkGetAndPost($sfn . "verify")) {
			if(checkGetAndPost($sfn)==checkGetAndPost($sfn . "orig")) {
				return -2;
			} else {
				return 1;
			}
		} else {
			return -1;
		}
	} else {
		return 0;
	}
}

function CheckForUniqueUN($sid,$sfd,$sff,$sft,$sfn,$dbs,$dbu,$dbp,$dbd) {
	$arr_sft = explode(":",$sft);
	if(($arr_sft[0]==8) && ($sid!="")) {
		$str_sql = "SELECT * FROM $sfd WHERE ID = $sid";
		//echo "++$str_sql++<br>";
		$myconn2=mysql_connect($dbs,$dbu,$dbp);
		@mysql_select_db($dbd) or die("Unable to select database");
		$result=mysql_query($str_sql);
		$num=mysql_numrows($result);

		if($num==1) {
			$dbcurun = mysql_result($result,0,"EMail");
			//echo "==$dbcurun==<br>";
		}

		$str_sql = "SELECT * FROM $sfd WHERE $sff = '". checkGetAndPost($sfn) . "'";
		//echo "++$str_sql++<br>";
		$result=mysql_query($str_sql);
		$num=mysql_numrows($result);

		if($num==1) {
			$dbnewun = mysql_result($result,0,"EMail");
			//echo "==$dbnewun==<br>";
			if($dbnewun==$dbcurun) {
				return 1;
			} else {
				return -1;
			}
		} elseif($num<1) {
			return 1;
		} else {
			return -1;
		}
	} else {
		return 0;
	}
	mysql_close($myconn2);
}

/**
* PBKDF2 key derivation function as defined by RSA's PKCS #5: https://www.ietf.org/rfc/rfc2898.txt
* $algorithm - The hash algorithm to use. Recommended: SHA256
* $password - The password.
* $salt - A salt that is unique to the password.
* $count - Iteration count. Higher is better, but slower. Recommended: At least 1000.
* $key_length - The length of the derived key in bytes.
* $raw_output - If true, the key is returned in raw binary format. Hex encoded otherwise.
* Returns: A $key_length-byte key derived from the password and salt.
*/
if (!function_exists("hash_pbkdf2")) {
    function hash_pbkdf2($algorithm, $password, $salt, $count, $key_length, $raw_output = false) {

        class pbkdf2 {
            public $algorithm;
            public $password;
            public $salt;
            public $count;
            public $key_length;
            public $raw_output;

            private $hash_length;
            private $output         = "";

            public function __construct($data = null)
            {
                if ($data != null) {
                    $this->init($data);
                }
            }

            public function init($data)
            {
                $this->algorithm  = $data["algorithm"];
                $this->password   = $data["password"];
                $this->salt       = $data["salt"];
                $this->count      = $data["count"];
                $this->key_length = $data["key_length"];
                $this->raw_output = $data["raw_output"];
            }

            public function hash()
            {
                $this->algorithm = strtolower($this->algorithm);
                if(!in_array($this->algorithm, hash_algos(), true))
                    throw new Exception('PBKDF2 ERROR: Invalid hash algorithm.');

                if($this->count <= 0 || $this->key_length <= 0)
                    throw new Exception('PBKDF2 ERROR: Invalid parameters.');

                $this->hash_length = strlen(hash($this->algorithm, "", true));
                $block_count = ceil($this->key_length / $this->hash_length);
                for ($i = 1; $i <= $block_count; $i++) {
                    // $i encoded as 4 bytes, big endian.
                    $last = $this->salt . pack("N", $i);
                    // first iteration
                    $last = $xorsum = hash_hmac($this->algorithm, $last, $this->password, true);
                    // perform the other $this->count - 1 iterations
                    for ($j = 1; $j < $this->count; $j++) {
                        $xorsum ^= ($last = hash_hmac($this->algorithm, $last, $this->password, true));
                    }
                    $this->output .= $xorsum;
                    if($this->raw_output)
                        return substr($this->output, 0, $this->key_length);
                    else
                        return bin2hex(substr($this->output, 0, $this->key_length));
                }
            }
        }

        $data = array('algorithm' => $algorithm, 'password' => $password, 'salt' => $salt, 'count' => $count, 'key_length' => $key_length, 'raw_output' => $raw_output);
        try {
            $pbkdf2 = new pbkdf2($data);
            return $pbkdf2->hash();
        } catch (Exception $e) {
            throw $e;
        }
    }
}

function PWHash($strun,$strpw) {
	$strsalt = hash('sha256', uniqid(mt_rand(), true) . $strun);
	return $strsalt . hash_pbkdf2("sha256",$strpw,$strsalt,10000,128);
}

function PWCheck($strpw,$strhash,$strsalt) {
	$testhash = $strsalt . hash_pbkdf2("sha256",$strpw,$strsalt,10000,128);
	if($testhash==$strhash) {
		return 1;
	} else {
		return 0;
	}
}

function GetSystemParameter($parmname,$parmdefault,$dbs,$dbu,$dbp,$dbd) {
	$myconn2=mysql_connect($dbs,$dbu,$dbp);
	@mysql_select_db($dbd) or die("Unable to select database");

	$query = "SELECT * FROM SystemParameters WHERE Name = '$parmname'";
	$result=mysql_query($query);
	$num=mysql_numrows($result);
	if($num>0) { $retval = mysql_result($result,0,"Value"); } else { $retval = "$parmdefault"; }
	return $retval;
}

function GetMSSQLCO($dbn,$dbu,$dbp) {
	if ($dbu=="") {
		$connopt = array('Database'=>$dbn);
	} else {
		$connopt = array('Database'=>$dbn, 'UID'=>$dbu, 'PWD'=>$dbp);
	}

	return $connopt;
}

function mssql_insert_id($conn) {
    $id = 0;
    $res = sqlsrv_query($conn,"SELECT @@identity AS id");
    if ($row = sqlsrv_fetch_array($res, SQLSRV_FETCH_ASSOC)) {
        $id = $row["id"];
    }
    return $id;
}

function do_db_connect($dbt,$dbs,$dbn,$dbu,$dbp) {
	if($dbt=="mssql") {
		$conn = sqlsrv_connect($dbs, GetMSSQLCO($dbn,$dbu,$dbp));
		if(!$conn) { die(print_r(sqlsrv_errors(), true)); }
	} else {
		//echo "--$dbn--";
		$conn=mysql_connect($dbs,$dbu,$dbp);
		@mysql_select_db($dbn) or die("Unable to select database");
	}

	return $conn;
}

function do_db_query($conn,$query,$retnum,$dbt,$dbs,$dbn,$dbu,$dbp) {
	//echo "==$query==<br>";
	if($dbt=="mssql") {
		$result = sqlsrv_query($conn,$query,array(),array( "Scrollable" => 'keyset' ));
		if($result === false) { die(print_r(sqlsrv_errors(), true)); }
		$num = sqlsrv_num_rows($result);
	} else {
		$result=mysql_query($query);
		if(strpos(strtoupper($query),"SELECT") === False) {
			$num=0;
		} else {
			if(strpos(strtoupper($query),"SELECT") > 0) {
				$num=0;
			} else {
				if($result) {
					$num=mysql_numrows($result);
				} else {
					$num=-1;
				}
			}
		}
	}

	if($retnum==1) {
		return array($num,$result);
	} else {
		return $result;
	}
}

function do_db_get_insert_id($dbt,$conn) {
	if($dbt=="mssql") {
		$str_id = mssql_insert_id($conn);
	} else {
		$str_id = mysql_insert_id($conn);
	}

	return $str_id;
}

function do_db_rowaffect($dbt,$conn) {
        if($dbt=="mssql") {
                $rowsaff = "-1";
        } else {
                $rowsaff = mysql_affected_rows();
        }

        return $rowsaff;
}

function do_db_close($dbt,$conn) {
	if($dbt=="mssql") {
		sqlsrv_close($conn);
	} else {
		mysql_close($conn);
	}
}

function do_db_result($dbt,$result,$rid,$fid) {
	if($dbt=="mssql") {
		$row=sqlsrv_fetch_array($result,SQLSRV_FETCH_BOTH,SQLSRV_SCROLL_ABSOLUTE,$rid);
		$retval=$row[$fid];
	} else {
		if($result) {
			$retval = mysql_result($result,$rid,$fid);
		} else {
			$retval = "";
		}
	}

	return $retval;
}

function CleanSQL($insql) {
	$newsql = str_replace("'", "\'", $insql);
	return $newsql;
}

function DoGet($strURL, $credentials, $cookie, $rawauth) {
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $strURL);
	curl_setopt($ch, CURLOPT_TIMEOUT, 10);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
	curl_setopt($ch, CURLOPT_USERPWD, base64_decode($credentials));

	$headers = array($rawauth
	);

	if($cookie!="") { array_push($headers,"Cookie: ".$cookie); }

	curl_setopt($ch, CURLOPT_HEADER, 1);
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
	curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
	curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

	$data = curl_exec($ch);
	$header_size=curl_getinfo($ch,CURLINFO_HEADER_SIZE);
	$header=trim(substr($data,0,$header_size)); //Get the header and trim it to remove \r\n
	$body=substr($data,$header_size); //Get the body
	$ret = array($body,$header);

	curl_close($ch);

	return $ret;
}

Function DoGetOld($strURL, $credentials, $cookie) {
	// Create a stream
	$headers = array(
	  'http'=>array(
	    'method'=>"GET",
            'header'=>""
	  )
	);
        if($credentials!="") { $headers["http"]["header"] = "Authorization: Basic " . $credentials; }
        if($cookie!="") { $headers["http"]["header"] = "Cookie: ".$cookie; }

	$context = stream_context_create($headers);

	$file = file_get_contents($strURL, false, $context);

	return $file;
}

function DoPost($url,$msg,$cookie,$contenttype,$soapaction,$credentials,$rawauth) {
	//$url = "https://" . $host . "/sdk";
	$page = $url;
 
    $headers = array(
    	"POST ".$page." HTTP/1.0",
    	"Content-type: $contenttype;charset=\"utf-8\"",
    	"Accept: $contenttype",
    	"Cache-Control: no-cache",
    	"Pragma: no-cache",
    	"Content-length: ".strlen($msg)
    );

	if($cookie!="") { array_push($headers,"Cookie: ".$cookie); }
	if($credentials!="") { array_push($headers,"Authorization: Basic " . $credentials); }
	if($soapaction!="") { array_push($headers,"SOAPAction: \"$soapaction\""); }
	if($rawauth!="") { array_push($headers,$rawauth); }
      
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL,$url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    //if($inclhead=="1") { curl_setopt($ch, CURLOPT_HEADER, 1); }
    curl_setopt($ch, CURLOPT_HEADER, 1);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    //curl_setopt($ch, CURLOPT_USERAGENT, "VMware VI Client/4.1.0");
    curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0) Gecko/20100101 Firefox/40.1");
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);

    // Apply the XML to our curl call
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $msg);

    $data = curl_exec($ch);

	if(1==1) {
	        if (curl_errno($ch)) {
			$ret = array("Error: " . curl_error($ch),$header);
	        } else {
			$header_size=curl_getinfo($ch,CURLINFO_HEADER_SIZE);
			$header=trim(substr($data,0,$header_size)); //Get the header and trim it to remove \r\n
			$body=substr($data,$header_size); //Get the body
			//$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
			$ret = array($body,$header);
			curl_close($ch);
		}
	} else {
		$ret = array($data,$header);
	}

	return $ret;
}

function DoDelete($url,$rawauth) {
        $headers = array($rawauth
        );

	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL,$url);
	curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
	$result = curl_exec($ch);
	$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
	curl_close($ch);
	return $result;
}

function ExtractCookie($resp,$cookiematch) {
	// get cookie
	// multi-cookie variant contributed by @Combuster in comments
	//preg_match_all('/^Set-Cookie:\s*([^;]*)/mi', $resp, $matches);
	preg_match_all('/Set-Cookie: (.*)\b/mi', $resp, $matches);
	$cookies = array();
	$x=0;

	foreach($matches[1] as $item) {
	    //echo "--$item--\n";
	    parse_str($item, $cookie);
	    //$cookies = array_merge($cookies, $cookie);
	    if(isset($cookie[$cookiematch])) {
		//echo "==$x==$cookiematch==\n";
		$arr_cookie = explode(";", $cookie[$cookiematch]);
		$str_cookie = str_replace(" ", "+", $arr_cookie[0]);
	    	$cookies[$x] = $cookiematch . "=" . $str_cookie;
		$x++;
	    }
	}
	return $cookies;

//	if(isset($cookies[$cookiematch])) {
//		return "$cookiematch=".str_replace(" ","+",$cookies[$cookiematch]);
//	} else {
//		return "";
//	}
}

function getXmlValueByTag($inXmlset,$needle,$retmode,$recret) {
	$intdebug = 0;
	$recpos = 1;
	//$retmode - blank (default), return value. Otherwise use something like "attributes,outCookie"
	$tagValue = "";
	$resource = xml_parser_create(); //Create an XML parser
	xml_parse_into_struct($resource, $inXmlset, $outArray); // Parse XML data into an array structure
	xml_parser_free($resource); //Free an XML parser
	//echo print_r($outArray);
	//echo "~~$intdebug~~" . count($outArray) . "<br>";
       
	for($i=0;$i<count($outArray);$i++){
		if($intdebug==1) { echo "!!" . $outArray[$i]['tag'] . "!!" . strtoupper($needle) . "<br>\n"; }
		if($outArray[$i]['tag']==strtoupper($needle)){
			if($intdebug==1) { echo "found--$retmode<br>"; }
			if($retmode=="") {
				$tagValue = $outArray[$i]['value'];
			} else {
				$arr_ret = explode(",",$retmode);
				try {
					$arrSub = $outArray[$i][$arr_ret[0]];
					if($intdebug==1) { print_r($arrSub); }
					$tagValue = $arrSub[strtoupper($arr_ret[1])];
					if($intdebug==1) { echo "~~$tagValue~~"; }
				} catch (Exception $e) {
					$tagValue = "";
				}
			}
			if($recpos==$recret) { break; } else { $tagValue = ""; }
			$recpos += 1;
		}
	}
	return $tagValue;
} 

Function StripHTML($inhtml) {
	return strip_tags($inhtml);
}

Function CleanText($intext) {
	$new_text = $intext;
	$new_text = str_replace("'", "`", $new_text);
	$new_text = str_replace(chr(34), "``", $new_text);
	return $new_text;
}
?>
