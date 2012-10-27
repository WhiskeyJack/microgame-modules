<?php

/**
 * @file
 * Content for no access page
 */
function game_validate() {
  
  
  $key = "xZh6a60B96wJUxUlxADnorfO4zCxqbhPep1F4UaSDCp=";
  $iv = "XpVkbVGR1eaTiE6bisbJxGFetBMcKWH8PUiLIRpTTK6=";

  // Get encrypted dat from gzipfile
  $encrypted = mg_gzfile_get_contents($_FILES['data']['tmp_name']);
  // convert/clean data in correct encoding
  $encrypted = iconv('UTF-16', 'UTF-8', $encrypted);
  // decrypt the cleaned data
  $decrypted = mg_decrypt($key, $iv, $encrypted);
  // parse the decrypted json data
  $json = json_decode($decrypted);


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


