<?php

/**
 * @file
 * Content for a game page
 */
function portal_game_page() {

  menu_tree_set_path('microgame_portal', 'portal/games');

  $arg = arg();
  $game_id = $arg[2];
  $mpath = drupal_get_path('module', 'portal_games');
  drupal_add_js($mpath . '/includes/fancybox/jquery.fancybox.pack.js', array('weight' => 1));
  drupal_add_js($mpath . '/includes/js/portal_games.js', array('weight' => 1));
  drupal_add_css($mpath . '/includes/fancybox/jquery.fancybox.css', array('weight' => 1));

  $gameurl = url("portal/game/{$game_id}/play", array('absolute' => TRUE));
  $param = array('gameurl' => $gameurl);
  drupal_add_js(array('param' => $param), array('type' => 'setting'));

  $divwrapper = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('game_play_btn'),)
  );
  $divwrapper["game_{$game_id}"] = array('#markup' => '<p>' . t('Play') . '</p>');
  $output[] = $divwrapper;


  return $output;
}

