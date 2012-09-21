<?php
/**
 * @file
 * Contains Subdomain.
 */

class Subdomain {

  private $name;
  private $group;
  private $id;

  function __construct() {
    $servername = explode('.', $_SERVER["SERVER_NAME"]);
    $firstdomain = $servername[0] == 'www' ? $servername[1] : $servername[0];
    $subdomain = $firstdomain == $servername[count($servername)-2] ? '' : $firstdomain;
    $details = $this->fetch_by_name($subdomain);
    $this->name = $details['name'];
    $this->group = $details['group'];
    $this->id = $details['sdid'];
  }

  function name() {
    return $this->name;
  }

  function group() {
    return $this->group;
  }

  function id() {
    return $this->id;
  }

  function get_array($id = NULL) {
    if (is_null($id)){
      return array('sdid' => $this->id, 'name' => $this->name, 'group' => $this->group);
    }
    $array =  is_numeric($id) ? $this->fetch_by_id($id) : $this->fetch_by_name($id);
    return $array;
  }

  private function fetch_by_id($id) {
     return db_select('microgame_subdomains', 'm')->fields('m')->condition('sdid', $id, '=')->execute()->fetchAssoc();
  }

  private function fetch_by_name($name) {
     return db_select('microgame_subdomains', 'm')->fields('m')->condition('name', $name, '=')->execute()->fetchAssoc();
  }
}