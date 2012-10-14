<?php

/**
 * @file
 * Contains Game.
 */
class GameList extends Game {

  private $sdid;
  public $list;

  function __construct($sdid = 0) {
    $this->sdid = 0;
    $this->list = array();
    $this->get_games($sdid);
    return $this;
  }

  private function get_games($sdid = 0) {

    $query = db_select('microgame_games', 'm');
    $query->leftJoin('microgame_subdomain_games', 'msg', 'm.gid = msg.gid');
    $query->join('users', 'u', 'm.created_by_uid = u.uid');
    $query->join('users', 'v', 'm.updated_by_uid = v.uid');
    $query->join('users', 'v', 'm.updated_by_uid = v.uid');
    $query->fields('m');
    $query->fields('msg', array('enabled', 'weight', 'closebtn'));
    $query->addField('u', 'name', 'created_by_name');
    $query->addField('v', 'name', 'updated_by_name');
    if ($sdid > 0) {
      $query->condition('msg.sdid', $sdid, '=');
    }
    $query->groupBy('m.gid');
    $query->orderBy('weight');
    //$string = $query->__toString();
    $gamelist = $query->execute()->fetchAllAssoc('gid');
    if ($gamelist) {
      foreach ($gamelist as $gid => $details) {
        $game = new Game();
        $game->load_these_values($details);
        $this->list[$details->gid] = $game;
      }
    }
  }
  
  function games () {
    return $this->list;
  }
  
}