<?php

function game_new_hash() {
  
  $lasthash = isset($_GET['lasthash']) ? $_GET['lasthash'] : '';
  
  if (strlen($lasthash) != 12) {
    // don't return, just exit
    exit();
  }
  
  $game = new Game();
  $newhash = $game->new_hash($lasthash);
  echo $newhash;
  
}