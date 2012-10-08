<?php

/**
 * @file
 * Content for activities page
 */
function portal_games_page() {
  
  $games = subdomain()->get_games();
  $output = array();
  foreach ($games as $gid =>$game) {
      $output["game_{$gid}"] = array(
          '#type' => 'markup',
          '#prefix' => '<div class="game_widget">',
          '#suffix' => '</div>',
          '#markup' => "<p>{$game->name()}</p>",
      );
  }
  
  $a=1;
  return $output;

}

?>
