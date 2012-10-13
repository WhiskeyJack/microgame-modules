<?php

/**
 * @file
 * Content for games page
 */
function portal_games_page() {

  $games = subdomain()->get_games();
  $output = array();
  foreach ($games as $gid => $game) {
    $onclick = "location.href='/portal/game/{$game->gid()}'";
    $divwrapper = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('game_widget'), 'onclick' => $onclick)
    );
    $divwrapper["game_{$gid}"] = array('#markup' => "<p>{$game->name()}</p>");
    $output[] = $divwrapper;
  }

  return $output;
}

?>
