<?php

/**
 * @file
 * Contains Game.
 */
class Game {

  private $gid;
  private $name;
  private $description;
  private $active_version;
  private $active_fid;
  private $new_version;
  private $new_fid;
  private $created_on;
  private $created_by_uid;
  private $created_by_name;
  private $updated_on;
  private $updated_by_uid;
  private $updated_by_name;
  private $enabled;
  private $weight;
  private $versions;
  private $last_version_id;
  private $new;
  private $saved;
  private $hash;
  public $config;
  
  

  function __construct($gid = 0, $sdid = 0) {
    $this->hash = '';
    if ($gid == 0) {
      $this->gid = 0;
      $this->new_fid = 0;
      $this->config = array();
      $this->new = TRUE;
    } else {
      $this->fetch_by_id($gid);
      if ($this->gid > 0) {
        $this->fetch_configuration($sdid);
      }
    }
    $this->saved = TRUE;
    return $this;
  }
  
  function name() {
    return $this->name;
  }

  function description() {
    return $this->description;
  }

  function version() {
    return $this->active_version;
  }
  
  function version_list() {
    return $this->versions;
  }
  
  function version_select_list() {
    $select = array();
    foreach ($this->versions as $id => $val) {
      $time = format_date($val->uploaded_on, 'short');
      $select[$id] = "{$id} - {$val->created_by_name} - $time" ; 
    }
    return $select;
  }

  function id() {
    return $this->gid;
  }

  function gid() {
    return $this->gid;
  }

  function fid() {
    return $this->active_fid;
  }

  function updated_on() {
    return $this->updated_on;
  }

  function updated_by_uid() {
    return $this->updated_by_uid;
  }

  function updated_by_name() {
    return $this->updated_by_name;
  }
  
  function created_on() {
    return $this->created_on;
  }

  function created_by_uid() {
    return $this->created_by_uid;
  }
  
  function created_by_name() {
    return $this->created_by_name;
  }
  
  function enabled() {
    return $this->enabled;
  }

  private function fetch_by_id($id) {
    $query = db_select('microgame_games', 'm');
    $query->join('users', 'u', 'm.created_by_uid = u.uid');
    $query->join('users', 'v', 'm.updated_by_uid = v.uid');
    $query->fields('m');
    $query->addField('u', 'name', 'created_by_name');
    $query->addField('v', 'name', 'updated_by_name');
    $query->condition('m.gid', $id, '=');
    $details = $query->execute()->fetchAssoc();
    if ($details) {
      foreach ($details as $key => $val) {
        $this->$key = $val;
      }
      $this->fetch_versions();
    } else {
      $this->gid = 0;
    }
  }

  private function fetch_by_name($name) {
    $query = db_select('microgame_games', 'm');
    $query->join('users', 'u', 'm.created_by_uid = u.uid');
    $query->join('users', 'v', 'm.updated_by_uid = v.uid');
    $query->fields('m');
    $query->addField('u', 'name', 'created_by_name');
    $query->addField('v', 'name', 'updated_by_name');
    $query->condition('m.name', $name, '=');
    $details = $query->execute()->fetchAssoc();
    if ($details) {
      foreach ($details as $key => $val) {
        $this->$key = $val;
      }
      $this->fetch_versions();
    } else {
      $this->gid = 0;
    }
  }
  
  protected function load_these_values($details, $fetchversions = FALSE) {
     if ($details) {
       $config = new stdClass();
      foreach ($details as $key => $val) {
        if ($key == 'weight' || $key == 'closebtn'){
          $config->gid = $details->gid;
          $config->$key = $val;
        } elseif ($key == 'enabled' ) {
           $config->$key = (int)$val;
           $this->$key = (int)$val;  
        } else {
          $this->$key = $val;
        }
      }
      if (isset($config->gid)) {
        $this->config = array(0=> $config);
      }
      if (!isset($this->enabled) || $this->enabled === null) {
        $this->enabled = 1;
      }
      if ($fetchversions) {
        $this->fetch_versions();
      }
    } else {
      $this->gid = 0;
    }
  }

  private function fetch_versions() {
    $query = db_select('microgame_game_versions', 'm');
    $query->leftJoin('users', 'u', 'm.uploaded_by_uid = u.uid');
    $query->fields('m');
    $query->addField('u', 'name', 'created_by_name');
    $this->versions = $query->condition('gid', $this->gid, '=')->orderBy('version_id', 'DESC')->execute()->fetchAllAssoc('version_id');
    if (!empty($this->versions)) {
      $this->last_version_id = array_shift(array_values($this->versions))->version_id;
    }
  }

  // creates a new game object
  function new_game($name, $description, $fid) {
    $this->name = $name;
    $this->description = $description;
    $this->active_version = 1;
    $this->active_fid = $fid;
    $this->new_version = 1;
    $this->new_fid = $fid;
    $this->saved = FALSE;
    $this->new = TRUE;
    return $this;
  }

  function update_game($name, $description, $fid = 0, $makeactive = FALSE) {
    $this->name = $name;
    $this->description = $description;
    $this->new_version = 1;
    if ($fid != 0) {
      $this->new_fid = $fid;
      $this->new_version = $this->last_version_id + 1;
      if ($makeactive) {
        $this->active_version = $this->new_version;
        $this->active_fid = $this->new_fid;
      }
    }
    $this->new = FALSE;
    $this->saved = FALSE;
    return $this;
  }
  
  function set_active($versionid) {
    if (array_key_exists($versionid, $this->versions)) {
      $this->active_version = $versionid;
    }
    return $this;
  }

  // saves the game object
  function save() {
    global $user;
    if ($this->new) {
      // create new game in db
      $this->gid = db_insert('microgame_games')
              ->fields(array(
                  'name' => $this->name,
                  'description' => $this->description,
                  'active_version' => $this->active_version,
                  'active_fid' => $this->active_fid,
                  'created_by_uid' => $user->uid,
                  'updated_by_uid' => $user->uid,
                  'created_on' => REQUEST_TIME,
                  'updated_on' => REQUEST_TIME,
              ))
              ->execute();
      // insert new version
      db_insert('microgame_game_versions')
              ->fields(array(
                  'gid' => $this->gid,
                  'version_id' => $this->new_version,
                  'fid' => $this->new_fid,
                  'uploaded_on' => REQUEST_TIME,
                  'uploaded_by_uid' => $user->uid,
              ))
              ->execute();
    } else {
      // update existing game in db
      db_update('microgame_games')
              ->fields(array(
                  'name' => $this->name,
                  'description' => $this->description,
                  'active_version' => $this->active_version,
                  'active_fid' => $this->active_fid,
                  'updated_by_uid' => $user->uid,
                  'updated_on' => REQUEST_TIME,
              ))
              ->condition('gid', $this->gid, '=')
              ->execute();
      if ($this->new_fid != 0) {
        // insert new version
        db_insert('microgame_game_versions')
                ->fields(array(
                    'gid' => $this->gid,
                    'version_id' => $this->new_version,
                    'fid' => $this->new_fid,
                    'uploaded_on' => REQUEST_TIME,
                    'uploaded_by_uid' => $user->uid,
                ))
                ->execute();
        $this->new_fid = 0;
        $this->new_version = 0;
        $this->fetch_versions();
      }
    }

    $this->saved = true;
    return $this;
  }

  function version_count() {
    return count($this->versions);
  }
  
  protected function fetch_configuration($sdid) {
    if ($sdid == 0) {
      $config = new stdClass();
      $config->sdid = 0;
      $config->gid = $this->gid;
      $config->enabled = 1;
      $config->closebtn = 1;
      $config->weight = 0;
      $this->config = array(0 => $config);
    } else {
      $config = db_select('microgame_subdomain_games', 'm')
          ->fields('m')
          ->condition('sdid', $sdid, '=')
          ->condition('gid', $this->gid, '=')    
          ->execute()
          ->fetchObject();
      if ($config) {
        $this->config = array($sdid => $config);
      } else {
        // no config found for given subdomain
        $this->config = array(0 => $config);
      }
    }
  }
  
  function config($sdid) {
    if (!array_key_exists($sdid, $this->config)) {
      $this->fetch_configuration($sdid);
    } else {
      return $this->config[$sdid];
    }
    if (array_key_exists($sdid, $this->config)) {
      return $this->config[$sdid];
    }
    return FALSE;
  }
  
  // generates a random string of 12 characters
  private function random_hash() {
   $this->hash = base_convert(mt_rand(0x1D39D3E06400000, 0x41C21CB8E0FFFFFF), 10, 36); 
   return $this->hash;
  }
  
  function hash() {
      return $this->hash;
  }
  
  function start_game($sdid){
    if (!array_key_exists($sdid, $this->config)) {
      $this->fetch_configuration($sdid);
      if (!array_key_exists($sdid, $this->config)) {
        return FALSE;
      }
    }
    global $user;
    $this->random_hash();
    $this->gid = db_insert('microgame_game_active_sessions')
              ->fields(array(
                  'hash' => $this->hash,
                  'gid' => $this->gid,
                  'version_id' => $this->config[$sdid]->version_id,
                  'sdid' =>$this->sdid,
                  'created_by_uid' => $user->uid,
                  'created_on' => REQUEST_TIME,
              ))
              ->execute();
    // set cookie
    setcookie('gr_session', $this->hash, time()+3600,'/');
    setcookie('gr_version', 2, time()+3600,'/');
  }
}