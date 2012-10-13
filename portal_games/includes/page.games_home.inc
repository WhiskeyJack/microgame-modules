<?php

/**
 * @file
 * Content for games page
 */
function portal_games_page() {

  $games = subdomain()->get_games();
  $gamedivs = array();

  $output = array();
  $count = 0;
  foreach ($games as $gid => $game) {
    $prefix =  $count == 0 ? '<div id="game_box">' : '';
    $suffix =  $count == count($games)-1 ? '</div>' : '';
    $onclick = "location.href='/portal/game/{$game->gid()}'";
    $output["game_box_{$gid}"] = array(
        '#type' => 'container',
        '#prefix' => $prefix,
        '#suffix' => $suffix,
        '#attributes' => array('class' => array('game_widget'), 'id' => "game_box_{$gid}", 'onclick' => $onclick)
    );
    $output["game_box_{$gid}"]["game_title_{$gid}"] = array('#markup' => "<p>{$game->name()}</p>");
    $count++;
  }

  return $output;
}

?>
