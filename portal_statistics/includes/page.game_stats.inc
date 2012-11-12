<?php
/**
 * @file
 * Page that will show stats of a game
 */
function portal_game_stats($game) {
  
  //$game = new Game();
  
  $game_name = $game->name();
  
    $output = array(
      'first_para' => array(
          '#type' => 'markup',
          '#markup' => "<p>Portal Statistics Page for game {$game_name}</p>",
      ),
  );
  return $output;

}