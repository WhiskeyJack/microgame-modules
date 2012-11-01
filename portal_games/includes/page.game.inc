<?php

/**
 * @file
 * Content for a game page
 */
function portal_game_page($game) {

  menu_tree_set_path('microgame_portal', 'portal/games');

  $game_id = $game->gid();
  $mpath = drupal_get_path('module', 'portal_games');
  drupal_add_js($mpath . '/includes/fancybox/jquery.fancybox.pack.js', array('weight' => 1));
  drupal_add_js($mpath . '/includes/js/portal_games.js', array('weight' => 1));
  drupal_add_css($mpath . '/includes/fancybox/jquery.fancybox.css', array('weight' => 1));

  $divwrapper = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('game_play_btn')),
      '#prefix' => '<div id="gameinfo_box">',
      '#suffix' => '</div>'
  );
  $divwrapper["game_{$game_id}"] = array('#markup' => '<p>' . t('Play') . '</p>');
  $output[] = $divwrapper;

  $videourl = '';
  if ($game->youtube_id() != '') {
    $videourl = 'http://www.youtube.com/v/' . $game->youtube_id() . '?autoplay=1';
    $divwrapper = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('game_video_btn')),
        '#prefix' => '<div id="gameinfo_box">',
        '#suffix' => '</div>'
    );
    $divwrapper["video_{$game_id}"] = array('#markup' => '<p>' . t('Video') . '</p>');
    $output[] = $divwrapper;
  }

  $sdid = subdomain()->id();
  $gameurl = url("portal/game/{$game_id}/play", array('absolute' => TRUE));
  if (empty($sdid) || !$game->config($sdid)) {
    $param = array('gameurl' => $gameurl, 'videourl' => $videourl, 'closebtn' => $game->config(0)->closebtn);
  } else {
    $param = array('gameurl' => $gameurl, 'videourl' => $videourl, 'closebtn' => $game->config($sdid)->closebtn);
  }
  drupal_add_js(array('param' => $param), array('type' => 'setting'));

  return $output;
}

