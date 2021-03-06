<?php

/**
 * @file
 * Content for no access page
 */
function game_validate() {
  $allow_test = TRUE;
  // watchdog('portal_manager', 'Validation Started.', array(), WATCHDOG_DEBUG);
  $sdid = subdomain()->id();

  // see if we're testing or not
  if ($allow_test && isset($_GET['t']) && $_GET['t'] == 1) {
    $json = create_test_game_session($sdid);
  } else {
    $key = "xZh6a60B96wJUxUlxADnorfO4zCxqbhPep1F4UaSDCp=";
    $iv = "XpVkbVGR1eaTiE6bisbJxGFetBMcKWH8PUiLIRpTTK6=";

    if (!isset($_FILES['data']['tmp_name'])) {
      $error_txt = 'Validation Failed. No file was found with stats form submit';
      if (!isset($_POST['game'])) {
        $error_txt .= ' and no game was specified in the form.';
        $g = array();
      } else {
        $error_txt .= ' for game "%game"';
        $g = array('%game' => $_POST['game']);
      }
      watchdog('portal_manager', $error_txt, $g, WATCHDOG_ERROR);
      exit();
    }
    // Get encrypted dat from gzipfile
    $encrypted = mg_gzfile_get_contents($_FILES['data']['tmp_name']);
    if (!$encrypted) {
      // don't return, just exit
      watchdog('portal_manager', 'Validation failed, could not decrypt stats data.', array(), WATCHDOG_ERROR, $link = NULL);
      exit();
    }

    // convert/clean data in correct encoding
    $encrypted = iconv('UTF-16', 'UTF-8', $encrypted);
    // decrypt the cleaned data
    $decrypted = mg_decrypt($key, $iv, $encrypted);
    // mg_debug($decrypted);
    // parse the decrypted json data
    $json = json_decode($decrypted);
  }

  // if testing we can create a game session before submitting
  if ($allow_test && isset($_POST['sdid']) && isset($_POST['gameid'])) {
    watchdog('portal_manager', 'Creating test game session for subdomain %subdomain and game %game.', array('%subdomain' => $_POST['sdid'], '%game' => $_POST['gameid']), WATCHDOG_DEBUG);
    $game = new Game($_POST['gameid'], $_POST['sdid']);
    $game->start_game($_POST['sdid']);
    mg_debug("Hash is " . $game->hash());
    $json->generics->hash =  $game->hash();
    mg_debug(print_r($json,1));
  }

  $game = new Game();
  $game->save_stats($json);


// don't return anything
}

function mg_gzfile_get_contents($filename, $use_include_path = 0) {
  $file = @gzopen($filename, 'rb', $use_include_path);
  if ($file) {
    $data = '';
    while (!gzeof($file)) {
      $data .= gzread($file, 1024);
    }
    gzclose($file);
  }
  return $data;
}

function mg_addpadding($string, $blocksize = 32) {
  $len = strlen($string);
  $pad = $blocksize - ($len % $blocksize);
  $string .= str_repeat(chr($pad), $pad);
  return $string;
}

function mg_strippadding($string) {
  $slast = ord(substr($string, -1));
  $slastc = chr($slast);
  $pcheck = substr($string, -$slast);
  if (preg_match("/$slastc{" . $slast . "}/", $string)) {
    $string = substr($string, 0, strlen($string) - $slast);
    return $string;
  } else {
    return false;
  }
}

function mg_encrypt($key, $iv, $string = "") {
  $key = base64_decode($key);
  $iv = base64_decode($iv);

  return base64_encode(mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, mg_addpadding($string), MCRYPT_MODE_CBC, $iv));
}

function mg_decrypt($key, $iv, $string = "") {
  $key = base64_decode($key);
  $iv = base64_decode($iv);

  $string = base64_decode($string);
  return mg_strippadding(mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $key, $string, MCRYPT_MODE_CBC, $iv));
}

function create_test_game_session($sdid, $gameid = 0) {
  if ($gameid == 0) {
    $currentgames = db_select('microgame_subdomain_games', 'm')
            ->fields('m')
            ->condition('sdid', $sdid)
            ->condition('enabled', 1)
            ->orderBy('weight', 'ASC')
            ->execute()
            ->fetchallAssoc('gid');

    $pick = array_shift($currentgames);
    $gameid = $pick->gid;
  }
  $game = new Game($gameid, $sdid);
  $game->start_game($sdid);

  // Doesn't work, probably too slow
  //$hash = $_COOKIE['gr_session'];
  //$version = $_COOKIE['gr_version'];

  $hash = $game->hash();


  $generics = new stdClass();
  $generics->hash = $hash;
  $generics->difficulty = 1;
  $generics->score = 123;
  $generics->gametime = 23;
  $generics->timesplayed = 0;

  $customdata = new stdClass();
  $customdata->requestedCards = "13";
  $customdata->timeTraditional = "46.90109";
  $customdata->timeDomus = "67.59272";
  $customdata->cardsViewed = "112";
  $customdata->cardsFound = "13";
  $customdata->testString = "test";

  $customtypes = new stdClass();
  $customtypes->requestedCards = "int";
  $customtypes->timeTraditional = "float";
  $customtypes->timeDomus = "float";
  $customtypes->cardsViewed = "int";
  $customtypes->cardsFound = "int";
  $customtypes->testString = "string";

  $customdescr = new stdClass();
  $customdescr->requestedCards = "Total requested cards";
  $customdescr->timeTraditional = "Total search time on the traditional table";
  $customdescr->timeDomus = "Total search time on the Domus table";
  $customdescr->cardsViewed = "The total number of cards viewed while searching";
  $customdescr->cardsFound = "The total number of cards requested by the cowboy";
  $customdescr->testString = "";

  $json = new stdClass();
  $json->generics = $generics;
  $json->customdata = $customdata;
  $json->customtypes = $customtypes;
  $json->customdescr = $customdescr;

//  $msg = '{"generics":{"hash":"%hash%","difficulty":1,"score":0,"gametime":2,"timesplayed":1},"customdata":{"TotalClickCount":"0","MissedCickCount":"0","LikesGame":"True","teststring1":"bladiebla1","teststring2":"bladiebla2","teststring3":"bladiebla3","testboolean1":"Fals","testint1":"123","testint2":"456","testint3":"789","testfloat1":"1.23","testfloat2":"4.56","testfloat3":"7"},"customtypes":{"TotalClckCount":"int","MissedClickCount":"int","LikesGame":"bool","teststring1":"string","teststring2":"string","teststring3":"string","testbolean1":"bool","testint1":"int","testint2":"int","testint3":"int","testfloat1":"float","testfloat2":"float","testfloat3":"float"},"cusomdescr":{"TotalClickCount":"The total click count","MissedClickCount":"The missed click count","LikesGame":"","teststring1":"","testsring2":"","teststring3":"","testboolean1":"","testint1":"","testint2":"","testint3":"","testfloat1":"","testfloat2":"","testfloat3":""}';
  //$msg = str_replace("%hash%", $hash, $msg);
  // ($msg);
//  $json = json_decode($msg);


  return $json;
}

/*
  global $str;
  $str = "";

  function do_dump(&$var, $var_name = NULL, $indent = NULL, $reference = NULL) {
    global $str;
    $do_dump_indent = "<span style='color:#666666;'>|</span> &nbsp;&nbsp; ";
    $reference = $reference . $var_name;
    $keyvar = 'the_do_dump_recursion_protection_scheme';
    $keyname = 'referenced_object_name';

    // So this is always visible and always left justified and readable
    $str .= "<div style='text-align:left; background-color:white; font: 100% monospace; color:black;'>\n";

    if (is_array($var) && isset($var[$keyvar])) {
      $real_var = &$var[$keyvar];
      $real_name = &$var[$keyname];
      $type = ucfirst(gettype($real_var));
      $str .= "$indent$var_name <span style='color:#666666'>$type</span> = <span style='color:#e87800;'>&amp;$real_name</span><br>";
    } else {
      $var = array($keyvar => $var, $keyname => $reference);
      $avar = &$var[$keyvar];

      $type = ucfirst(gettype($avar));
      if ($type == "String")
        $type_color = "<span style='color:green'>";
      elseif ($type == "Integer")
        $type_color = "<span style='color:red'>";
      elseif ($type == "Double") {
        $type_color = "<span style='color:#0099c5'>";
        $type = "Float";
      } elseif ($type == "Boolean")
        $type_color = "<span style='color:#92008d'>";
      elseif ($type == "NULL")
        $type_color = "<span style='color:black'>";

      if (is_array($avar)) {
        $count = count($avar);
        $str .= "$indent" . ($var_name ? "$var_name => " : "") . "<span style='color:#666666'>$type ($count)</span><br>$indent(<br>";
        $keys = array_keys($avar);
        foreach ($keys as $name) {
          $value = &$avar[$name];
          do_dump($value, "['$name']", $indent . $do_dump_indent, $reference);
        }
        $str .= "$indent)<br>";
      } elseif (is_object($avar)) {
        $str .= "$indent$var_name <span style='color:#666666'>$type</span><br>$indent(<br>";
        foreach ($avar as $name => $value)
          do_dump($value, "$name", $indent . $do_dump_indent, $reference);
        $str .= "$indent)<br>";
      } elseif (is_int($avar))
        $str .= "$indent$var_name = <span style='color:#666666'>$type(" . strlen($avar) . ")</span> $type_color" . htmlentities($avar) . "</span><br>";
      elseif (is_string($avar))
        $str .= "$indent$var_name = <span style='color:#666666'>$type(" . strlen($avar) . ")</span> $type_color\"" . htmlentities($avar) . "\"</span><br>";
      elseif (is_float($avar))
        $str .= "$indent$var_name = <span style='color:#666666'>$type(" . strlen($avar) . ")</span> $type_color" . htmlentities($avar) . "</span><br>";
      elseif (is_bool($avar))
        $str .= "$indent$var_name = <span style='color:#666666'>$type(" . strlen($avar) . ")</span> $type_color" . ($avar == 1 ? "TRUE" : "FALSE") . "</span><br>";
      elseif (is_null($avar))
        $str .= "$indent$var_name = <span style='color:#666666'>$type(" . strlen($avar) . ")</span> {$type_color}NULL</span><br>";
      else
        $str .= "$indent$var_name = <span style='color:#666666'>$type(" . strlen($avar) . ")</span> " . htmlentities($avar) . "<br>";

      $var = $var[$keyvar];
    }

    $str .= "\n</div>\n\n";
    return $str;
  }
*/


