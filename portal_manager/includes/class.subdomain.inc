<?php
/**
 * @file
 * Contains Subdomain.
 */

class Subdomain {

  private $id;
  private $name;
  private $group_name;
  private $theme_name;
  

  function __construct() {
    $servername = explode('.', $_SERVER["SERVER_NAME"]);
    $firstdomain = $servername[0] == 'www' ? $servername[1] : $servername[0];
    $subdomain = $firstdomain == $servername[count($servername)-2] ? '' : $firstdomain;
    $details = $this->fetch_by_name($subdomain);
    $this->name = $details['name'];
    $this->group_name = $details['group_name'];
    $this->theme_name = $details['theme_name'];
    $this->id = $details['sdid'];
  }

  function name() {
    return $this->name;
  }

  function group() {
    return $this->group_name;
  }

  function theme() {
    return $this->theme_name;
  }
  
  function id() {
    return $this->id;
  }

  function get_array($id = NULL) {
    if (is_null($id)){
      return array('sdid' => $this->id, 'name' => $this->name, 'group_name' => $this->group_name, 'theme_name' => $this->theme_name);
    }
    $array =  is_numeric($id) ? $this->fetch_by_id($id) : $this->fetch_by_name($id);
    return $array;
  }
  
  /***
   * Returns the sdid if the subdomain exists, false otherwise
   */
  function name_exists($name) {
    $details = $this->fetch_by_name($name);
    if ($details) {
      return $details['sdid'];
    }
    else {
      return FALSE;
    }
      
  }

  function get_games($subdomain = ''){
    global $user;
    //$games  = array();
    if (empty($subdomain) || $subdomain == $this->name){
      $subdomain = $this->id;
    }
    if (!is_numeric($subdomain)){
      $subdomain = $this->name_exists($subdomain);
    }
    if (empty($subdomain) && ($user->uid ==1 || in_array('portal manager', $user->roles))) {
      // main site and user is admin or portal manager => show all games
     $gamelist = new GameList();
     $games = $gamelist->games();
    } elseif (!empty($subdomain)) {
      $gamelist = new GameList($subdomain);
      $games = $gamelist->games();
    } 
    return $games;
  }
  
  private function fetch_by_id($id) {
     return db_select('microgame_subdomains', 'm')->fields('m')->condition('sdid', $id, '=')->execute()->fetchAssoc();
  }

  private function fetch_by_name($name) {
     return db_select('microgame_subdomains', 'm')->fields('m')->condition('name', $name, '=')->execute()->fetchAssoc();
  }
}